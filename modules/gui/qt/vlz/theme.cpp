/*****************************************************************************
 * theme.cpp: VLZ theme presets and accent (DESIGN.md › Colour)
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

#include "style/systempalette.hpp"
#include "maininterface/mainctx.hpp"
#include "vlz/vlzsettings.hpp"
#include "vlz/lookpanel.hpp"

#include <QApplication>
#include <QPalette>
#include <QStyleHints>
#include <QHash>

#include <cmath>
#include <cstring>

namespace {

struct VlzTheme
{
    const char *id;
    bool dark;
    const char *bg, *panel, *raised, *line, *text, *muted, *accent;
};

/* Same tables as DESIGN.md › Colour. */
const VlzTheme kThemes[] = {
    { "dark",  true,  "#141415", "#111112", "#1E1E21", "#27272B", "#ECECEE", "#A1A1A7", "#FF8800" },
    { "light", false, "#FFFFFF", "#FAFAFB", "#F0F0F3", "#E8E8EC", "#18181B", "#55555D", "#D96A00" },
    { "paper", false, "#F4EEE3", "#EFE8DB", "#E8DFCF", "#DCD1BF", "#2B241D", "#5F5346", "#A65424" },
    { "ink",   true,  "#000000", "#050506", "#131316", "#1E1E23", "#F4F4F6", "#A7A7B0", "#8EA8FF" },
    { "nord",  true,  "#2E3440", "#2A303B", "#3B4252", "#434C5E", "#ECEFF4", "#C3CAD6", "#88C0D0" },
    { "mocha", true,  "#1E1E2E", "#181825", "#313244", "#313244", "#CDD6F4", "#BAC2DE", "#CBA6F7" },
    { "tokyo", true,  "#1A1B26", "#16161E", "#24283B", "#292E42", "#C0CAF5", "#A9B1D6", "#7AA2F7" },
};

QColor mix(const QColor &a, const QColor &b, double t)
{
    return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                            a.greenF() + (b.greenF() - a.greenF()) * t,
                            a.blueF() + (b.blueF() - a.blueF()) * t);
}

double luminance(const QColor &c)
{
    auto ch = [](double v) { return v <= 0.03928 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); };
    return 0.2126 * ch(c.redF()) + 0.7152 * ch(c.greenF()) + 0.0722 * ch(c.blueF());
}

double contrast(const QColor &a, const QColor &b)
{
    const double la = luminance(a), lb = luminance(b);
    return (std::max(la, lb) + 0.05) / (std::min(la, lb) + 0.05);
}

/* Mix the accent toward white (dark themes) or black (light themes) until it has 3:1 on bg. */
QColor readableAccent(const QColor &accent, const QColor &bg, bool dark)
{
    const QColor target = dark ? QColor(Qt::white) : QColor(Qt::black);
    QColor out = accent;
    for (double t = 0.04; contrast(out, bg) < 3.0 && t <= 1.0; t += 0.04)
        out = mix(accent, target, t);
    return out;
}

} // namespace

namespace {
/* The application palette as Qt first set it, before VLZ themed the widgets: Windows' own colours and accent. */
const QPalette &originalPalette()
{
    static const QPalette p = qApp->palette();
    return p;
}

/* Qt also keeps palettes per widget class, from Windows' theme, on top of the application palette (with Windows'
 * own text-on-accent colour, for example). VLZ sets those too, and keeps Qt's to restore them. */
const char *const kWidgetClasses[] = {
    "QAbstractButton", "QCheckBox", "QRadioButton", "QToolButton", "QPushButton", "QComboBox", "QGroupBox",
    "QLabel", "QHeaderView", "QAbstractItemView", "QTabBar", "QMenu", "QMenuBar", "QTextEdit", "QTextControl",
    "QLineEdit", "QMessageBoxLabel",
};

const QHash<QByteArray, QPalette> &originalClassPalettes()
{
    static const QHash<QByteArray, QPalette> palettes = [] {
        QHash<QByteArray, QPalette> h;
        for (const char *cls : kWidgetClasses)
            h.insert(cls, QApplication::palette(cls));
        return h;
    }();
    return palettes;
}
}

QColor vlz::systemAccent()
{
    return qApp ? originalPalette().color(QPalette::Normal, QPalette::Accent) : QColor("#FF8800");
}

/* Widget dialogs (preferences, About, media information...) take the VLZ theme instead of Windows' light or dark
 * setting: the colour scheme (which also picks their title bar) and a palette from the theme's table, with text and
 * marks on the accent and links made readable on the surface behind them. "default" leaves them to VLC and Windows. */
void vlz::applyWidgetTheme(qt_intf_t *intf)
{
    if (!qApp || !intf)
        return;
    (void)originalPalette(); /* capture before the first change */
    (void)originalClassPalettes();
    static bool applied = false;

    char *themeId = var_InheritString(intf, "qt-vlz-theme");
    char *accentOpt = var_InheritString(intf, "qt-accent-color");
    const QString id = themeId ? QString::fromUtf8(themeId) : QString();
    const QString accentOption = accentOpt ? QString::fromUtf8(accentOpt) : QStringLiteral("default");
    free(themeId);
    free(accentOpt);

    bool known = false;
    for (const VlzTheme &t : kThemes)
        known = known || id == QLatin1String(t.id);
    if (!known)
    {
        if (applied)
        {
            qApp->styleHints()->unsetColorScheme();
            qApp->setPalette(originalPalette());
            for (auto it = originalClassPalettes().cbegin(); it != originalClassPalettes().cend(); ++it)
                QApplication::setPalette(it.value(), it.key().constData());
            applied = false;
        }
        return;
    }

    const ThemeColors c = themeColors(id, accentOption);
    qApp->styleHints()->setColorScheme(c.dark ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light);

    const QColor nearBlack("#0B0B0C");
    const QColor onAccent = contrast(Qt::white, c.accent) >= contrast(nearBlack, c.accent) ? QColor(Qt::white) : nearBlack;
    QColor link = c.accent;
    for (double t = 0.05; contrast(link, c.panel) < 4.5 && t <= 1.0; t += 0.05)
        link = mix(c.accent, c.text, t);
    const QColor disabled = mix(c.muted, c.panel, 0.35);

    QPalette p(originalPalette());
    auto set = [&p](QPalette::ColorRole role, const QColor &normal, const QColor &off) {
        p.setColor(QPalette::Active, role, normal);
        p.setColor(QPalette::Inactive, role, normal);
        p.setColor(QPalette::Disabled, role, off);
    };
    set(QPalette::Window, c.panel, c.panel);
    set(QPalette::WindowText, c.text, disabled);
    set(QPalette::Base, c.bg, c.bg);
    set(QPalette::AlternateBase, c.raised, c.raised);
    set(QPalette::Text, c.text, disabled);
    set(QPalette::PlaceholderText, c.muted, disabled);
    set(QPalette::Button, c.raised, c.raised);
    set(QPalette::ButtonText, c.text, disabled);
    set(QPalette::BrightText, c.dark ? QColor(Qt::white) : QColor(Qt::black), disabled);
    set(QPalette::ToolTipBase, c.raised, c.raised);
    set(QPalette::ToolTipText, c.text, c.text);
    set(QPalette::Light, mix(c.raised, c.text, 0.12), mix(c.raised, c.text, 0.12));
    set(QPalette::Midlight, mix(c.raised, c.line, 0.5), mix(c.raised, c.line, 0.5));
    set(QPalette::Mid, c.line, c.line);
    set(QPalette::Dark, mix(c.line, c.bg, 0.5), mix(c.line, c.bg, 0.5));
    set(QPalette::Shadow, QColor(0, 0, 0), QColor(0, 0, 0));
    set(QPalette::Highlight, c.accent, c.line);
    set(QPalette::HighlightedText, onAccent, disabled);
    set(QPalette::Accent, c.accent, c.line);
    set(QPalette::Link, link, link);
    set(QPalette::LinkVisited, link, link);
    qApp->setPalette(p);
    for (const char *cls : kWidgetClasses)
        QApplication::setPalette(p, cls);
    applied = true;
}

/* For previews (the preferences dialog): a theme's colours as the app would apply them. "default" is VLC's own
 * dark palette with its orange. */
vlz::ThemeColors vlz::themeColors(const QString &themeId, const QString &accentOption)
{
    static const VlzTheme vlcDefault = { "default", true, "#1E1E1E", "#212121", "#272727", "#303030",
                                         "#FFFFFF", "#AAAAAA", "#FF8800" };
    const VlzTheme *theme = &vlcDefault;
    for (const VlzTheme &t : kThemes)
        if (themeId == QLatin1String(t.id))
            theme = &t;

    ThemeColors c;
    c.dark = theme->dark;
    c.bg = QColor(theme->bg); c.panel = QColor(theme->panel); c.raised = QColor(theme->raised);
    c.line = QColor(theme->line); c.text = QColor(theme->text); c.muted = QColor(theme->muted);
    QColor accent(theme->accent);
    if (accentOption == QLatin1String("system") && qApp)
        accent = vlz::systemAccent();
    else if (accentOption != QLatin1String("default") && QColor::isValidColorName(accentOption))
        accent = QColor(accentOption);
    c.accent = theme == &vlcDefault ? accent : readableAccent(accent, c.bg, c.dark);
    return c;
}

/* VLZ: called from updatePalette(). Returns false to keep VLC's own palette. */
bool SystemPalette::applyVlzTheme()
{
    if (!m_ctx)
        return false;

    char *themeId = var_InheritString(m_ctx->getIntf(), "qt-vlz-theme");
    const VlzTheme *theme = nullptr;
    if (themeId)
    {
        for (const VlzTheme &t : kThemes)
            if (strcmp(t.id, themeId) == 0)
                theme = &t;
        free(themeId);
    }
    if (!theme)
    {
        /* "default": VLC's own palette and colour scheme. Coming back from a VLZ theme,
         * restore the colours that VLZ overwrote (values from systempalette.hpp). */
        if (m_vlzThemed)
        {
            darkGrey200 = QColor("#1E1E1E"); darkGrey300 = QColor("#212121"); darkGrey400 = QColor("#242424");
            darkGrey500 = QColor("#272727"); darkGrey600 = QColor("#2A2A2A"); darkGrey700 = QColor("#2D2D2D");
            darkGrey800 = QColor("#303030");
            lightGrey100 = QColor("#FAFAFA"); lightGrey200 = QColor("#F6F6F6"); lightGrey300 = QColor("#F2F2F2");
            lightGrey400 = QColor("#EDEDED"); lightGrey500 = QColor("#E9E9E9"); lightGrey600 = QColor("#E5E5E5");
            lightGrey700 = QColor("#E0E0E0");
            orange400 = QColor("#f89a06"); orange200 = QColor("#e29a06");
            orange500 = QColor("#FF8800"); orange800 = QColor("#FF610A");
            if (char *opt = var_InheritString(m_ctx->getIntf(), "qt-accent-color"))
            {
                if (!strcmp(opt, "system") && qApp)
                    orange500 = vlz::systemAccent();
                else if (strcmp(opt, "default") != 0 && QColor::isValidColorName(QString::fromUtf8(opt)))
                    orange500 = QColor(QString::fromUtf8(opt));
                orange800 = orange500.lighter(150);
                free(opt);
            }
            m_vlzThemed = false;
        }
        return false;
    }
    m_vlzThemed = true;

    const QColor bg(theme->bg), panel(theme->panel), raised(theme->raised), line(theme->line);
    const QColor text(theme->text), muted(theme->muted);

    /* Accent: the theme's own for "default", the system one, or any colour. */
    QColor accent(theme->accent);
    if (char *opt = var_InheritString(m_ctx->getIntf(), "qt-accent-color"))
    {
        if (!strcmp(opt, "system") && qApp)
            accent = vlz::systemAccent();
        else if (strcmp(opt, "default") != 0 && QColor::isValidColorName(QString::fromUtf8(opt)))
            accent = QColor(QString::fromUtf8(opt));
        free(opt);
    }
    orange500 = readableAccent(accent, bg, theme->dark);
    orange800 = orange500;
    orange400 = orange500;
    orange200 = orange500;

    if (theme->dark)
    {
        /* Background ramp used by makeDarkPalette(), darkest to lightest. */
        darkGrey200 = panel;
        darkGrey300 = bg;
        darkGrey400 = mix(bg, raised, 0.33);
        darkGrey500 = mix(bg, raised, 0.66);
        darkGrey600 = raised;
        darkGrey700 = mix(raised, line, 0.5);
        darkGrey800 = line;
        makeDarkPalette();
    }
    else
    {
        /* Light palette: lightGrey is the background ramp, darkGrey the text ramp. */
        lightGrey100 = bg;
        lightGrey200 = mix(bg, raised, 0.33);
        lightGrey300 = mix(bg, raised, 0.66);
        lightGrey400 = raised;
        lightGrey500 = mix(raised, line, 0.5);
        lightGrey600 = line;
        lightGrey700 = mix(line, muted, 0.15);
        darkGrey200 = text;
        darkGrey300 = text;
        darkGrey400 = mix(text, muted, 0.2);
        darkGrey500 = mix(text, muted, 0.4);
        darkGrey600 = mix(text, muted, 0.6);
        darkGrey700 = mix(text, muted, 0.8);
        darkGrey800 = muted;
        makeLightPalette();
    }
    return true;
}

/* VLZ: follow live theme and accent changes from the settings page. */
void SystemPalette::vlzConnectTheme()
{
    if (VLZSettings *settings = VLZSettings::instance())
        connect(settings, &VLZSettings::themeChanged, this, &SystemPalette::updatePalette, Qt::UniqueConnection);
}
