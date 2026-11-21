/*****************************************************************************
 * vlxsettings.hpp: VLX interface settings exposed to QML (DESIGN.md › Player)
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
#ifndef VLX_VLXSETTINGS_HPP
#define VLX_VLXSETTINGS_HPP

#include <QObject>
#include <QString>

struct qt_intf_t;

/*
 * One place for VLX's look-and-layout options, read from and saved to VLC's config
 * ("qt-vlx-*" options declared in qt.cpp). Setters save at once and notify, so a
 * settings page can preview a change live.
 */
class VLXSettings : public QObject
{
    Q_OBJECT

    // island, docked, minimal, pill, classic
    Q_PROPERTY(QString barStyle READ barStyle WRITE setBarStyle NOTIFY barStyleChanged FINAL)
    // bottom, top
    Q_PROPERTY(QString barPosition READ barPosition WRITE setBarPosition NOTIFY barPositionChanged FINAL)
    // classic (VLC), albumcompact, album, turntable, turntablecompact, mini
    Q_PROPERTY(QString audioSkin READ audioSkin WRITE setAudioSkin NOTIFY audioSkinChanged FINAL)

public:
    explicit VLXSettings(qt_intf_t *intf, QObject *parent = nullptr);

    QString barStyle() const { return m_barStyle; }
    void setBarStyle(const QString &style);

    QString barPosition() const { return m_barPosition; }
    void setBarPosition(const QString &position);

    QString audioSkin() const { return m_audioSkin; }
    void setAudioSkin(const QString &skin);

signals:
    void barStyleChanged();
    void barPositionChanged();
    void audioSkinChanged();

private:
    QString readString(const char *name, const QString &fallback) const;
    void writeString(const char *name, const QString &value);

    qt_intf_t *m_intf;
    QString m_barStyle;
    QString m_barPosition;
    QString m_audioSkin;
};

#endif
