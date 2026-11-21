/*****************************************************************************
 * vlxsettings.cpp: VLX interface settings exposed to QML
 *****************************************************************************
 * Copyright (C) 2026 VLX contributors
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

#include "vlxsettings.hpp"
#include "lookpanel.hpp" // VLX: themeColors for themePalette()

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
const QStringList kAudioSkins { "classic", "albumcompact", "album", "turntable", "turntablecompact", "mini" };
const QStringList kThemes { "default", "dark", "light", "paper", "ink", "nord", "mocha", "tokyo" };
const QStringList kFileIcons { "band", "echo" };
const QStringList kFileIconLooks { "auto", "light", "dark" };
}

VLXSettings *VLXSettings::s_instance = nullptr;

VLXSettings::VLXSettings(qt_intf_t *intf, QObject *parent)
    : QObject(parent)
    , m_intf(intf)
{
    s_instance = this;
    m_theme = readString("qt-vlx-theme", "ink");
    m_accent = readString("qt-accent-color", "default");
    m_fileIcons = readString("qt-vlx-file-icons", "band");
    m_fileIconLook = readString("qt-vlx-file-icon-look", "auto");
    m_startView = readString("qt-vlx-start-view", "");
    m_barStyle = readString("qt-vlx-bar-style", "classic");
    if (!kBarStyles.contains(m_barStyle))
        m_barStyle = "classic";
    m_barPosition = readString("qt-vlx-bar-position", "bottom");
    if (!kBarPositions.contains(m_barPosition))
        m_barPosition = "bottom";
    m_audioBackground = readString("qt-vlx-audio-background", "plain") == "tinted" ? "tinted" : "plain";
    m_audioTheme = readString("qt-vlx-audio-theme", "app");
    if (m_audioTheme != "app" && !kThemes.contains(m_audioTheme))
        m_audioTheme = "app";
    m_audioSkin = readString("qt-vlx-audio-skin", "albumcompact");
    if (!kAudioSkins.contains(m_audioSkin))
        m_audioSkin = "albumcompact";
}

QString VLXSettings::readString(const char *name, const QString &fallback) const
{
    char *value = var_InheritString(m_intf, name);
    const QString out = value ? QString::fromUtf8(value) : fallback;
    free(value);
    return out;
}

void VLXSettings::writeString(const char *name, const QString &value)
{
    config_PutPsz(name, qtu(value));
}

void VLXSettings::setBarStyle(const QString &style)
{
    if (!kBarStyles.contains(style) || style == m_barStyle)
        return;
    m_barStyle = style;
    writeString("qt-vlx-bar-style", style);
    emit barStyleChanged();
}

void VLXSettings::setBarPosition(const QString &position)
{
    if (!kBarPositions.contains(position) || position == m_barPosition)
        return;
    m_barPosition = position;
    writeString("qt-vlx-bar-position", position);
    emit barPositionChanged();
}

void VLXSettings::setAudioSkin(const QString &skin)
{
    if (!kAudioSkins.contains(skin) || skin == m_audioSkin)
        return;
    m_audioSkin = skin;
    writeString("qt-vlx-audio-skin", skin);
    emit audioSkinChanged();
}

void VLXSettings::setAudioBackground(const QString &background)
{
    if ((background != "plain" && background != "tinted") || background == m_audioBackground)
        return;
    m_audioBackground = background;
    writeString("qt-vlx-audio-background", background);
    emit audioLookChanged();
}

void VLXSettings::setAudioTheme(const QString &theme)
{
    if ((theme != "app" && (!kThemes.contains(theme) || theme == "default")) || theme == m_audioTheme)
        return;
    m_audioTheme = theme;
    writeString("qt-vlx-audio-theme", theme);
    emit audioLookChanged();
}

QVariantMap VLXSettings::themePalette(const QString &theme) const
{
    const vlx::ThemeColors c = vlx::themeColors(theme, QStringLiteral("default"));
    return { { "bg", c.bg }, { "panel", c.panel }, { "raised", c.raised }, { "line", c.line },
             { "text", c.text }, { "muted", c.muted }, { "accent", c.accent }, { "dark", c.dark } };
}

void VLXSettings::setTheme(const QString &theme)
{
    if (!kThemes.contains(theme) || theme == m_theme)
        return;
    m_theme = theme;
    writeString("qt-vlx-theme", theme);
    emit themeChanged();
}

void VLXSettings::setAccent(const QString &accent)
{
    if (accent == m_accent)
        return;
    m_accent = accent;
    writeString("qt-accent-color", accent);
    emit themeChanged();
}

void VLXSettings::setFileIcons(const QString &style)
{
    if (!kFileIcons.contains(style) || style == m_fileIcons)
        return;
    m_fileIcons = style;
    writeString("qt-vlx-file-icons", style);
    emit fileIconsChanged();
}

void VLXSettings::setFileIconLook(const QString &look)
{
    if (!kFileIconLooks.contains(look) || look == m_fileIconLook)
        return;
    m_fileIconLook = look;
    writeString("qt-vlx-file-icon-look", look);
    emit fileIconsChanged();
}

int VLXSettings::intOption(const QString &name) const
{
    return static_cast<int>(var_InheritInteger(m_intf, qtu(name)));
}

void VLXSettings::setIntOption(const QString &name, int value)
{
    config_PutInt(qtu(name), value);
    /* the video output reads it per window; update the live variable too */
    var_SetInteger(vlc_object_instance(m_intf), qtu(name), value);
}

QString VLXSettings::openMode() const
{
    if (!config_GetInt("one-instance") && !config_GetInt("one-instance-when-started-from-file"))
        return QStringLiteral("new");
    return config_GetInt("playlist-enqueue") ? QStringLiteral("queue") : QStringLiteral("running");
}

void VLXSettings::setOpenMode(const QString &mode)
{
    if (mode != "new" && mode != "running" && mode != "queue")
        return;
    /* read by each new VLX process at start-up (src/win32/specific.c), so the config is enough */
    config_PutInt("one-instance", 0);
    config_PutInt("one-instance-when-started-from-file", mode == "new" ? 0 : 1);
    config_PutInt("playlist-enqueue", mode == "queue" ? 1 : 0);
    config_SaveConfigFile(m_intf);
}

QVariantList VLXSettings::commands() const
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

bool VLXSettings::runCommand(const QString &id)
{
    const vlc_action_id_t action = vlc_actions_get_id(qtu(id));
    if (action == ACTIONID_NONE)
        return false;
    var_SetInteger(vlc_object_instance(m_intf), "key-action", action);
    return true;
}
