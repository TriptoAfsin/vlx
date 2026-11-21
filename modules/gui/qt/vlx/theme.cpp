/*****************************************************************************
 * theme.cpp: VLX theme presets and accent (DESIGN.md › Colour)
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

#include "style/systempalette.hpp"
#include "maininterface/mainctx.hpp"

#include <QApplication>
#include <QPalette>

#include <cmath>
#include <cstring>

namespace {

struct VlxTheme
{
    const char *id;
    bool dark;
    const char *bg, *panel, *raised, *line, *text, *muted, *accent;
};

/* Same tables as DESIGN.md › Colour. */
const VlxTheme kThemes[] = {
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

/* VLX: called from updatePalette(). Returns false to keep VLC's own palette. */
bool SystemPalette::applyVlxTheme()
{
    if (!m_ctx)
        return false;

    char *themeId = var_InheritString(m_ctx->getIntf(), "qt-vlx-theme");
    const VlxTheme *theme = nullptr;
    if (themeId)
    {
        for (const VlxTheme &t : kThemes)
            if (strcmp(t.id, themeId) == 0)
                theme = &t;
        free(themeId);
    }
    if (!theme)
        return false; /* "default": VLC's own palette and colour scheme */

    const QColor bg(theme->bg), panel(theme->panel), raised(theme->raised), line(theme->line);
    const QColor text(theme->text), muted(theme->muted);

    /* Accent: the theme's own for "default", the system one, or any colour. */
    QColor accent(theme->accent);
    if (char *opt = var_InheritString(m_ctx->getIntf(), "qt-accent-color"))
    {
        if (!strcmp(opt, "system") && qApp)
            accent = qApp->palette().color(QPalette::Normal, QPalette::Accent);
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
