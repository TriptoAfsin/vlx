/*****************************************************************************
 * vlzsettings.cpp: VLZ interface settings exposed to QML
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

#include "vlzsettings.hpp"
#include "lookpanel.hpp" // VLZ: themeColors for themePalette()

#include "qt.hpp"

#include <QStringList>

#include <vlc_configuration.h>
#include <vlc_modules.h>
#include <vlc_plugin.h>
#include <vlc_actions.h>

#include <QVariantMap>

namespace {
const QStringList kBarStyles { "island", "docked", "minimal", "pill", "classic" };
const QStringList kBarPositions { "bottom", "top" };
const QStringList kBarAligns { "center", "left", "right" };
const QStringList kBarSizes { "compact", "normal", "large" };
const QStringList kAudioSkins { "classic", "albumcompact", "album", "turntable", "turntablecompact", "mini" };
const QStringList kThemes { "default", "dark", "light", "paper", "ink", "nord", "mocha", "tokyo" };
const QStringList kFileIcons { "band", "echo" };
const QStringList kFileIconLooks { "auto", "light", "dark" };
}

VLZSettings *VLZSettings::s_instance = nullptr;

VLZSettings::VLZSettings(qt_intf_t *intf, QObject *parent)
    : QObject(parent)
    , m_intf(intf)
{
    s_instance = this;
    m_theme = readString("qt-vlz-theme", "ink");
    m_accent = readString("qt-accent-color", "default");
    m_fileIcons = readString("qt-vlz-file-icons", "band");
    m_fileIconLook = readString("qt-vlz-file-icon-look", "auto");
    m_startView = readString("qt-vlz-start-view", "");
    m_barStyle = readString("qt-vlz-bar-style", "classic");
    if (!kBarStyles.contains(m_barStyle))
        m_barStyle = "classic";
    m_barPosition = readString("qt-vlz-bar-position", "bottom");
    if (!kBarPositions.contains(m_barPosition))
        m_barPosition = "bottom";
    m_barAlign = readString("qt-vlz-bar-align", "center");
    if (!kBarAligns.contains(m_barAlign))
        m_barAlign = "center";
    m_barSize = readString("qt-vlz-bar-size", "normal");
    if (!kBarSizes.contains(m_barSize))
        m_barSize = "normal";
    m_pinControls = var_InheritBool(m_intf, "qt-vlz-pin-controls");
    m_pinTopBar = var_InheritBool(m_intf, "qt-vlz-pin-topbar");
    m_showFileName = var_InheritBool(m_intf, "qt-vlz-topbar-filename");
    m_endView = readString("qt-vlz-end-view", "player") == "home" ? "home" : "player";
    m_audioBackground = readString("qt-vlz-audio-background", "plain") == "tinted" ? "tinted" : "plain";
    m_audioTheme = readString("qt-vlz-audio-theme", "app");
    if (m_audioTheme != "app" && !kThemes.contains(m_audioTheme))
        m_audioTheme = "app";
    m_audioSkin = readString("qt-vlz-audio-skin", "albumcompact");
    if (!kAudioSkins.contains(m_audioSkin))
        m_audioSkin = "albumcompact";
}

QString VLZSettings::readString(const char *name, const QString &fallback) const
{
    char *value = var_InheritString(m_intf, name);
    const QString out = value ? QString::fromUtf8(value) : fallback;
    free(value);
    return out;
}

void VLZSettings::writeString(const char *name, const QString &value)
{
    config_PutPsz(name, qtu(value));
}

void VLZSettings::setBarStyle(const QString &style)
{
    if (!kBarStyles.contains(style) || style == m_barStyle)
        return;
    m_barStyle = style;
    writeString("qt-vlz-bar-style", style);
    emit barStyleChanged();
}

void VLZSettings::setBarPosition(const QString &position)
{
    if (!kBarPositions.contains(position) || position == m_barPosition)
        return;
    m_barPosition = position;
    writeString("qt-vlz-bar-position", position);
    emit barPositionChanged();
}

void VLZSettings::setAudioSkin(const QString &skin)
{
    if (!kAudioSkins.contains(skin) || skin == m_audioSkin)
        return;
    m_audioSkin = skin;
    writeString("qt-vlz-audio-skin", skin);
    emit audioSkinChanged();
}

void VLZSettings::setBarAlign(const QString &align)
{
    if (!kBarAligns.contains(align) || align == m_barAlign)
        return;
    m_barAlign = align;
    writeString("qt-vlz-bar-align", align);
    emit barLayoutChanged();
}

void VLZSettings::setBarSize(const QString &size)
{
    if (!kBarSizes.contains(size) || size == m_barSize)
        return;
    m_barSize = size;
    writeString("qt-vlz-bar-size", size);
    emit barLayoutChanged();
}

void VLZSettings::setPinControls(bool pin)
{
    if (pin == m_pinControls)
        return;
    m_pinControls = pin;
    config_PutInt("qt-vlz-pin-controls", pin);
    emit barLayoutChanged();
}

void VLZSettings::setPinTopBar(bool pin)
{
    if (pin == m_pinTopBar)
        return;
    m_pinTopBar = pin;
    config_PutInt("qt-vlz-pin-topbar", pin);
    emit barLayoutChanged();
}

void VLZSettings::setShowFileName(bool show)
{
    if (show == m_showFileName)
        return;
    m_showFileName = show;
    config_PutInt("qt-vlz-topbar-filename", show);
    emit barLayoutChanged();
}

void VLZSettings::setEndView(const QString &view)
{
    if ((view != "player" && view != "home") || view == m_endView)
        return;
    m_endView = view;
    writeString("qt-vlz-end-view", view);
    emit barLayoutChanged();
}

void VLZSettings::setAudioBackground(const QString &background)
{
    if ((background != "plain" && background != "tinted") || background == m_audioBackground)
        return;
    m_audioBackground = background;
    writeString("qt-vlz-audio-background", background);
    emit audioLookChanged();
}

void VLZSettings::setAudioTheme(const QString &theme)
{
    if ((theme != "app" && (!kThemes.contains(theme) || theme == "default")) || theme == m_audioTheme)
        return;
    m_audioTheme = theme;
    writeString("qt-vlz-audio-theme", theme);
    emit audioLookChanged();
}

QVariantMap VLZSettings::themePalette(const QString &theme) const
{
    const vlz::ThemeColors c = vlz::themeColors(theme, QStringLiteral("default"));
    return { { "bg", c.bg }, { "panel", c.panel }, { "raised", c.raised }, { "line", c.line },
             { "text", c.text }, { "muted", c.muted }, { "accent", c.accent }, { "dark", c.dark } };
}

void VLZSettings::setTheme(const QString &theme)
{
    if (!kThemes.contains(theme) || theme == m_theme)
        return;
    m_theme = theme;
    writeString("qt-vlz-theme", theme);
    emit themeChanged();
}

void VLZSettings::setAccent(const QString &accent)
{
    if (accent == m_accent)
        return;
    m_accent = accent;
    writeString("qt-accent-color", accent);
    emit themeChanged();
}

void VLZSettings::setFileIcons(const QString &style)
{
    if (!kFileIcons.contains(style) || style == m_fileIcons)
        return;
    m_fileIcons = style;
    writeString("qt-vlz-file-icons", style);
    emit fileIconsChanged();
}

void VLZSettings::setFileIconLook(const QString &look)
{
    if (!kFileIconLooks.contains(look) || look == m_fileIconLook)
        return;
    m_fileIconLook = look;
    writeString("qt-vlz-file-icon-look", look);
    emit fileIconsChanged();
}

int VLZSettings::intOption(const QString &name) const
{
    return static_cast<int>(var_InheritInteger(m_intf, qtu(name)));
}

void VLZSettings::setIntOption(const QString &name, int value)
{
    config_PutInt(qtu(name), value);
    /* the video output reads it per window; update the live variable too */
    var_SetInteger(vlc_object_instance(m_intf), qtu(name), value);
}

QString VLZSettings::openMode() const
{
    if (!config_GetInt("one-instance") && !config_GetInt("one-instance-when-started-from-file"))
        return QStringLiteral("new");
    return config_GetInt("playlist-enqueue") ? QStringLiteral("queue") : QStringLiteral("running");
}

void VLZSettings::setOpenMode(const QString &mode)
{
    if (mode != "new" && mode != "running" && mode != "queue")
        return;
    /* read by each new VLZ process at start-up (src/win32/specific.c), so the config is enough */
    config_PutInt("one-instance", 0);
    config_PutInt("one-instance-when-started-from-file", mode == "new" ? 0 : 1);
    config_PutInt("playlist-enqueue", mode == "queue" ? 1 : 0);
    config_SaveConfigFile(m_intf);
}

bool VLZSettings::memorySaver() const
{
    return config_GetInt("qt-vlz-memory-saver") != 0;
}

void VLZSettings::setMemorySaver(bool on)
{
    config_PutInt("qt-vlz-memory-saver", on);
    /* VLC's own options, so the full preferences show the real values; off restores VLC's defaults */
    config_PutInt("avcodec-threads", on ? 2 : 0);
    config_PutInt("file-caching", on ? 300 : 1000);
    config_SaveConfigFile(m_intf);
}

bool VLZSettings::mediaLibrary() const
{
    return config_GetInt("qt-vlz-media-library") != 0;
}

void VLZSettings::setMediaLibrary(bool on)
{
    config_PutInt("qt-vlz-media-library", on);
    config_SaveConfigFile(m_intf);
}
bool VLZSettings::liteMode() const
{
    return config_GetInt("qt-vlz-lite") != 0;
}
void VLZSettings::setLiteMode(bool on)
{
    config_PutInt("qt-vlz-lite", on);
    config_SaveConfigFile(m_intf);
}

QVariantList VLZSettings::commands() const
{
    QVariantList out;
    unsigned count = 0;
    module_config_t *config = module_config_get(module_get_main(), &count);
    for (unsigned i = 0; i < count; ++i)
    {
        const module_config_t &item = config[i];
        if (item.i_type != CONFIG_ITEM_KEY || !item.psz_name || strncmp(item.psz_name, "key-", 4) != 0)
            continue;

        QStringList keys;
        if (char *value = config_GetPsz(item.psz_name))
        {
            keys = QString::fromUtf8(value).split(QLatin1Char('\t'), Qt::SkipEmptyParts);
            free(value);
        }
        QVariantMap entry;
        entry.insert(QStringLiteral("id"), QString::fromUtf8(item.psz_name));
        entry.insert(QStringLiteral("label"), item.psz_text ? QString::fromUtf8(vlc_gettext(item.psz_text))
                                                            : QString::fromUtf8(item.psz_name));
        entry.insert(QStringLiteral("keys"), keys);
        out.append(entry);
    }
    module_config_free(config);
    return out;
}

bool VLZSettings::runCommand(const QString &id)
{
    const vlc_action_id_t action = vlc_actions_get_id(qtu(id));
    if (action == ACTIONID_NONE)
        return false;
    var_SetInteger(vlc_object_instance(m_intf), "key-action", action);
    return true;
}
