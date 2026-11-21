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

#include "qt.hpp"

#include <QStringList>

#include <vlc_configuration.h>

namespace {
const QStringList kBarStyles { "island", "docked", "minimal", "pill", "classic" };
const QStringList kBarPositions { "bottom", "top" };
const QStringList kAudioSkins { "classic", "albumcompact", "album", "turntable", "turntablecompact", "mini" };
}

VLXSettings::VLXSettings(qt_intf_t *intf, QObject *parent)
    : QObject(parent)
    , m_intf(intf)
{
    m_barStyle = readString("qt-vlx-bar-style", "classic");
    if (!kBarStyles.contains(m_barStyle))
        m_barStyle = "classic";
    m_barPosition = readString("qt-vlx-bar-position", "bottom");
    if (!kBarPositions.contains(m_barPosition))
        m_barPosition = "bottom";
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
