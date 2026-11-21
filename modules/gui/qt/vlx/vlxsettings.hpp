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
#include <QVariantList>

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
    // audio view background ("plain", "tinted") and colours ("app" or a theme id)
    Q_PROPERTY(QString audioBackground READ audioBackground WRITE setAudioBackground NOTIFY audioLookChanged FINAL)
    Q_PROPERTY(QString audioTheme READ audioTheme WRITE setAudioTheme NOTIFY audioLookChanged FINAL)
    // default (VLC), dark, light, paper, ink, nord, mocha, tokyo
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged FINAL)
    // "default" (the theme's own), "system", or a colour such as "#10B981"
    Q_PROPERTY(QString accent READ accent WRITE setAccent NOTIFY themeChanged FINAL)
    // band, echo / auto, light, dark
    Q_PROPERTY(QString fileIcons READ fileIcons WRITE setFileIcons NOTIFY fileIconsChanged FINAL)
    Q_PROPERTY(QString fileIconLook READ fileIconLook WRITE setFileIconLook NOTIFY fileIconsChanged FINAL)
    // page to open at start (--qt-vlx-start-view), for screenshots and tests; empty for the usual home
    Q_PROPERTY(QString startView READ startView CONSTANT FINAL)

public:
    explicit VLXSettings(qt_intf_t *intf, QObject *parent = nullptr);

    QString barStyle() const { return m_barStyle; }
    void setBarStyle(const QString &style);

    QString barPosition() const { return m_barPosition; }
    void setBarPosition(const QString &position);

    QString audioSkin() const { return m_audioSkin; }
    void setAudioSkin(const QString &skin);
    QString audioBackground() const { return m_audioBackground; }
    void setAudioBackground(const QString &background);
    QString audioTheme() const { return m_audioTheme; }
    void setAudioTheme(const QString &theme);
    // a theme's colours (bg, panel, raised, line, text, muted, accent, dark), for views with their own theme
    Q_INVOKABLE QVariantMap themePalette(const QString &theme) const;

    QString theme() const { return m_theme; }
    void setTheme(const QString &theme);
    QString accent() const { return m_accent; }
    void setAccent(const QString &accent);

    QString fileIcons() const { return m_fileIcons; }
    void setFileIcons(const QString &style);
    QString fileIconLook() const { return m_fileIconLook; }
    QString startView() const { return m_startView; }
    void setFileIconLook(const QString &look);

    /* VLC's own integer options (e.g. "mouse-hide-timeout"), for the settings page. */
    Q_INVOKABLE int intOption(const QString &name) const;
    Q_INVOKABLE void setIntOption(const QString &name, int value);
    // Opening a file from Explorer while VLX runs: "new" (a new window each time, several videos at once),
    // "running" (play it in the open window, VLC's default) or "queue" (add it to the open window's queue).
    // Sets VLC's one-instance, one-instance-when-started-from-file and playlist-enqueue; takes effect for the
    // next file opened.
    Q_INVOKABLE QString openMode() const;
    Q_INVOKABLE void setOpenMode(const QString &mode);

    /* Command palette (Ctrl+K): every VLC hotkey action with its label and current keys,
     * as [{ id: "key-play-pause", label, keys: ["Space"] }], and running one by id
     * exactly as if its keys were pressed. */
    Q_INVOKABLE QVariantList commands() const;
    Q_INVOKABLE bool runCommand(const QString &id);

    /* The interface's instance, for C++ code that reacts to changes (the palette). */
    static VLXSettings *instance() { return s_instance; }

signals:
    void barStyleChanged();
    void barPositionChanged();
    void audioSkinChanged();
    void audioLookChanged();
    void themeChanged();
    void fileIconsChanged();

private:
    QString readString(const char *name, const QString &fallback) const;
    void writeString(const char *name, const QString &value);

    qt_intf_t *m_intf;
    QString m_barStyle;
    QString m_barPosition;
    QString m_audioSkin;
    QString m_audioBackground;
    QString m_audioTheme;
    QString m_theme;
    QString m_accent;
    QString m_fileIcons;
    QString m_fileIconLook;
    QString m_startView;

    static VLXSettings *s_instance;
};

#endif
