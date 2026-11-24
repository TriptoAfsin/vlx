/*****************************************************************************
 * menuicons.cpp: one monochrome icon for every menu entry (DESIGN.md › Menus)
 *****************************************************************************
 * Copyright (C) 2026 VLZ contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "menuicons.hpp"

#include "qt.hpp"
#include "util/colorizedsvgicon.hpp"

#include <vlc_intf_strings.h>

#include <QAction>
#include <QHash>
#include <QMenu>

namespace {

/* Menu text (as VLC's menu code writes it, translated) -> icon. */
const QHash<QString, QString> &iconTable()
{
    static const QHash<QString, QString> table = [] {
        QHash<QString, QString> t;
        auto add = [&t](const QString &text, const char *icon) {
            t.insert(text, QString::fromLatin1(icon).startsWith(':') ? QString::fromLatin1(icon)
                                                                       : QStringLiteral(":/menu/vlz-%1.svg").arg(QLatin1String(icon)));
        };
        /* menu bar and ⋮ menu */
        for (const QString &s : { qtr("&Media") }) add(s, "menu-media");
        for (const QString &s : { qtr("P&layback"), qtr("&Playback") }) add(s, "menu-playback");
        add(qtr("&Audio"), "menu-audio");
        add(qtr("&Video"), "menu-video");
        for (const QString &s : { qtr("Subti&tle"), qtr("&Subtitle") }) add(s, "menu-subtitle");
        for (const QString &s : { qtr("Tool&s"), qtr("&Tools") }) add(s, "menu-tools");
        add(qtr("V&iew"), "menu-view");
        for (const QString &s : { qtr("&Help"), qtr("Help") }) add(s, "help");
        for (const QString &s : { qtr("Interface"), qtr("Interfaces") }) add(s, "window");
        add(qtr("Open Media"), "open-media");
        /* Media */
        add(qtr("Open &File..."), "file");
        add(qfut(I_OP_OPF), "file");
        add(qtr("&Open Multiple Files..."), "file-multiple");
        add(qfut(I_OP_OPDIR), "folder");
        for (const QString &s : { qtr("Open &Disc..."), qtr("Open &Network..."), qtr("Open &Network Stream..."),
                                  qtr("Open &Capture Device...") })
            add(s, s == qtr("Open &Disc...") ? "disc" : s == qtr("Open &Capture Device...") ? "capture-card" : "network");
        add(qtr("Open &Location from clipboard"), "clipboard");
        add(qtr("Open &Recent Media"), "recent");
        add(qfut(I_PL_SAVE), "save");
        add(qtr("Conve&rt / Save..."), "convert");
        add(qtr("&Stream..."), "stream");
        for (const QString &s : { qtr("&Quit"), qtr("Quit") }) add(s, "exit");
        /* Tools */
        add(qtr("&Effects and Filters"), "ic_fluent_options");
        add(qtr("&Track Synchronization"), "sync");
        add(qfut(I_MENU_INFO), "info");
        add(qfut(I_MENU_CODECINFO), "codec");
        add(qfut(I_MENU_VLM), "vlm");
        add(qtr("Program Guide"), "epg");
        add(qfut(I_MENU_MSG), "messages");
        add(qtr("Plu&gins and extensions"), "plugins");
        add(qtr("&Settings"), "settings");
        add(qtr("Customi&ze Interface..."), "customize");
        add(qtr("&Preferences"), "preferences");
        /* View */
        add(qtr("Play &Queue"), "ic_playlist");
        add(qtr("&Color Scheme"), "color-scheme");
        /* Audio */
        add(qtr("Audio &Track"), "audio-track");
        add(qtr("&Audio Device"), "audio-device");
        add(qtr("&Stereo Mode"), "stereo");
        add(qtr("&Mix Mode"), "mix");
        add(qtr("&Visualizations"), "visualization");
        add(qtr("&Increase Volume"), "volume-high");
        add(qtr("&Decrease Volume"), "volume-low");
        add(qtr("&Mute"), "volume-muted");
        /* Video */
        add(qtr("Video &Track"), "video-track");
        add(qtr("&3D Output"), "3d");
        add(qtr("&Zoom"), "zoom");
        add(qtr("&Aspect Ratio"), "aspect");
        add(qtr("&Crop"), "crop");
        add(qtr("&Fit"), "fit");
        add(qtr("&Deinterlace"), "deinterlace");
        add(qtr("&Deinterlace mode"), "deinterlace");
        add(qtr("Take &Snapshot"), "snapshot");
        /* Subtitle */
        add(qtr("Add &Subtitle File..."), "subtitle-add");
        add(qtr("Sub &Track"), "subtitle-track");
        /* Playback */
        add(qtr("T&itle"), "title");
        add(qtr("&Chapter"), "chapter");
        add(qtr("&Program"), "program");
        add(qfut(I_MENU_BOOKMARK), "bookmark");
        add(qtr("&Renderer"), "renderer");
        for (const QString &s : { qtr("Previous Title"), qtr("Previous Chapter") }) add(s, ":/menu/ic_fluent_previous.svg");
        for (const QString &s : { qtr("Next Title"), qtr("Next Chapter") }) add(s, ":/menu/ic_fluent_next.svg");
        add(qtr("Sp&eed"), "speed");
        add(qtr("N&ormal Speed"), "speed-normal");
        add(qfut(I_MENU_GOTOTIME), "goto-time");
        /* Help */
        add(qtr("Check for &Updates..."), "update");
        add(qfut(I_MENU_ABOUT), "info");
        return t;
    }();
    return table;
}

} // namespace

void vlz::decorateMenu(QMenu *menu)
{
    if (!menu)
        return;
    const QHash<QString, QString> &table = iconTable();
    for (QAction *action : menu->actions())
    {
        if (action->isSeparator())
            continue;
        if (!action->isCheckable())
        {
            const auto it = table.constFind(action->text());
            if (it != table.constEnd())
                action->setIcon(ColorizedSvgIcon::colorizedIconForWidget(it.value(), menu));
        }
        QMenu *sub = action->menu();
        if (sub && !sub->property("vlzDecorated").toBool())
        {
            sub->setProperty("vlzDecorated", true);
            /* After VLC's own aboutToShow handlers, which rebuild some submenus */
            QObject::connect(sub, &QMenu::aboutToShow, sub, [sub]() { vlz::decorateMenu(sub); });
            vlz::decorateMenu(sub);
        }
    }
}
