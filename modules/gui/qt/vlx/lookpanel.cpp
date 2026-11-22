/*****************************************************************************
 * lookpanel.cpp: VLX look settings inside the classic preferences dialog
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

#include "vlx/lookpanel.hpp"
#include "vlx/vlxsettings.hpp"
#include "maininterface/mainctx.hpp"
#include "dialogs/dialogs_provider.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QVBoxLayout>

#include <cmath>
#include <functional>

namespace {

using Options = QList<QPair<QString, QString>>; // value, label

/* A small VLX window: sidebar, a playing video, and the player bar in the chosen style and position. */
class LookPreview : public QWidget
{
public:
    explicit LookPreview(QWidget *parent) : QWidget(parent)
    {
        setFixedSize(320, 200);
        setAccessibleName(qtr("Preview"));
    }

    QString theme, accent, barStyle, barPosition, barAlign, barSize;
    bool pinBar = false, pinTop = false;

protected:
    void paintEvent(QPaintEvent *) override
    {
        const vlx::ThemeColors c = vlx::themeColors(theme, accent);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const QRectF win = QRectF(rect()).adjusted(1, 1, -1, -1);
        QPainterPath frame;
        frame.addRoundedRect(win, 10, 10);
        p.fillPath(frame, c.bg);
        p.setClipPath(frame);

        // title bar and sidebar
        const QRectF title(win.left(), win.top(), win.width(), 22);
        p.fillRect(title, c.panel);
        p.setBrush(QColor("#ff7a1a"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRectF(title.left() + 8, title.top() + 5, 12, 12), 3, 3);
        const QRectF side(win.left(), title.bottom(), 70, win.height() - title.height());
        p.fillRect(side, c.panel);
        for (int i = 0; i < 4; ++i)
        {
            const QRectF item(side.left() + 10, side.top() + 12 + i * 18, i == 0 ? 44 : 36, 6);
            p.setBrush(i == 0 ? c.text : c.muted);
            p.drawRoundedRect(item, 3, 3);
            if (i == 0)
            {
                p.setBrush(c.accent);
                p.drawRoundedRect(QRectF(side.left() + 3, item.top() - 2, 3, 10), 1.5, 1.5);
            }
        }

        // the "video", between whichever bars are always shown
        const bool top = barPosition == QLatin1String("top");
        const qreal k = barSize == QLatin1String("compact") ? 0.65 : barSize == QLatin1String("large") ? 1.25 : 1.0;
        const bool docked = pinBar || barStyle == QLatin1String("docked");
        const QString style = pinBar ? QStringLiteral("docked") : barStyle; // a pinned bar is docked
        const qreal dockH = 26 * k;
        const qreal topH = 14;
        QRectF area(side.right() + 8, title.bottom() + 8, win.right() - side.right() - 16, 0);
        area.setBottom(win.bottom() - 8);
        QRectF video = area;
        if (pinTop)
        {
            p.setBrush(c.raised);
            p.drawRoundedRect(QRectF(area.left(), area.top(), area.width(), topH), 3, 3);
            p.setBrush(c.muted);
            p.drawRoundedRect(QRectF(area.left() + 6, area.top() + topH / 2 - 2, area.width() * 0.35, 4), 2, 2);
            video.setTop(area.top() + topH + 3);
        }
        if (docked)
        {
            if (top) video.setTop(video.top() + dockH);
            else video.setBottom(video.bottom() - dockH);
        }
        QLinearGradient sky(video.topLeft(), video.bottomRight());
        sky.setColorAt(0, QColor("#2d5b7a"));
        sky.setColorAt(0.6, QColor("#3f7f4f"));
        sky.setColorAt(1, QColor("#7a9a3a"));
        QPainterPath videoPath;
        videoPath.addRoundedRect(video, docked ? 4 : 6, docked ? 4 : 6);
        p.fillPath(videoPath, sky);
        p.setBrush(QColor(255, 236, 170, 200));
        p.drawEllipse(QPointF(video.right() - 30, video.top() + 24), 9, 9); // a sun, so it reads as a picture

        // the bar
        QRectF bar;
        qreal radius = 0;
        QColor surface = c.raised;
        if (style == QLatin1String("island"))
        {
            bar = QRectF(video.left() + video.width() * 0.08, 0, video.width() * 0.84, 30 * k);
            radius = 9;
        }
        else if (style == QLatin1String("pill"))
        {
            bar = QRectF(video.center().x() - video.width() * 0.24, 0, video.width() * 0.48, 24 * k);
            radius = 12 * k;
        }
        else if (docked)
        {
            bar = QRectF(video.left(), 0, video.width(), dockH - 4);
            radius = 4;
        }
        else if (style == QLatin1String("minimal"))
        {
            bar = QRectF(video.left() + 10, 0, video.width() - 20, 18 * k);
            surface = Qt::transparent;
        }
        else // classic: across the bottom of the video, over a shade
        {
            bar = QRectF(video.left(), 0, video.width(), 32 * k);
            surface.setAlphaF(0.88);
        }
        if (docked)
            bar.moveTop(top ? video.top() - dockH : video.bottom() + 4);
        else if (style == QLatin1String("classic"))
            bar.moveTop(top ? video.top() : video.bottom() - bar.height());
        else
            bar.moveTop(top ? video.top() + 8 : video.bottom() - bar.height() - 8);

        if (surface.alpha() > 0)
        {
            QPainterPath barPath;
            if (style == QLatin1String("classic"))
                barPath.addRect(bar);
            else
                barPath.addRoundedRect(bar, radius, radius);
            p.save();
            if (!docked)
                p.setClipPath(videoPath.united(barPath));
            p.fillPath(barPath, surface);
            p.restore();
        }

        // seek line
        const bool pill = style == QLatin1String("pill");
        const qreal seekY = pill ? bar.center().y() : bar.top() + bar.height() * 0.24;
        const qreal seekL = bar.left() + (pill ? 26 : 10), seekR = bar.right() - 10;
        p.setBrush(QColor(c.text.red(), c.text.green(), c.text.blue(), 70));
        p.drawRoundedRect(QRectF(seekL, seekY - 1.5, seekR - seekL, 3), 1.5, 1.5);
        p.setBrush(c.accent);
        p.drawRoundedRect(QRectF(seekL, seekY - 1.5, (seekR - seekL) * 0.38, 3), 1.5, 1.5);
        p.drawEllipse(QPointF(seekL + (seekR - seekL) * 0.38, seekY), 3.5, 3.5);

        // play button with previous and next, placed by the alignment; the other buttons opposite
        const qreal r = qBound(4.5, 8 * k, 10.0);
        const qreal rowY = pill ? bar.center().y() : bar.top() + bar.height() * 0.66;
        const qreal groupW = 2 * r + 2 * 14;
        qreal playX;
        if (pill)
            playX = bar.left() + 13;
        else if (barAlign == QLatin1String("left"))
            playX = bar.left() + 10 + groupW / 2;
        else if (barAlign == QLatin1String("right"))
            playX = bar.right() - 10 - groupW / 2;
        else
            playX = bar.center().x();
        const QPointF play(playX, rowY);
        p.setBrush(c.accent);
        p.drawEllipse(play, r, r);
        p.setBrush(c.dark || surface.alpha() == 0 ? QColor(Qt::white) : c.bg);
        p.drawEllipse(play, r - 1.6, r - 1.6);
        QPolygonF tri;
        tri << QPointF(play.x() - r * 0.28, play.y() - r * 0.42) << QPointF(play.x() + r * 0.45, play.y())
            << QPointF(play.x() - r * 0.28, play.y() + r * 0.42);
        p.setBrush(QColor(Qt::black));
        p.drawPolygon(tri);
        if (!pill)
        {
            p.setBrush(c.text);
            p.drawRoundedRect(QRectF(play.x() - r - 11, play.y() - 2.5, 6, 5), 1.5, 1.5); // previous
            p.drawRoundedRect(QRectF(play.x() + r + 5, play.y() - 2.5, 6, 5), 1.5, 1.5);  // next
            const bool othersLeft = barAlign == QLatin1String("right");
            for (int i = 0; i < 3; ++i) // subtitles, volume, fullscreen
            {
                const qreal x = othersLeft ? bar.left() + 10 + i * 11 : bar.right() - 16 - i * 11;
                p.drawRoundedRect(QRectF(x, play.y() - 2.5, 6, 5), 1.5, 1.5);
            }
        }
    }
};

/* The audio player: the chosen skin in its colours (the app theme or the skin's own) and background. */
class AudioPreview : public QWidget
{
public:
    explicit AudioPreview(QWidget *parent) : QWidget(parent)
    {
        setFixedSize(320, 136);
        setAccessibleName(qtr("Audio player preview"));
    }

    QString skin, background, audioTheme, appTheme, accent;

protected:
    void paintEvent(QPaintEvent *) override
    {
        const vlx::ThemeColors c = audioTheme == QLatin1String("app")
                ? vlx::themeColors(appTheme, accent) : vlx::themeColors(audioTheme, QStringLiteral("default"));
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);

        const QRectF r = QRectF(rect()).adjusted(1, 1, -1, -1);
        QPainterPath frame;
        frame.addRoundedRect(r, 10, 10);
        p.fillPath(frame, c.bg);
        p.setClipPath(frame);
        if (background == QLatin1String("tinted"))
        {
            QLinearGradient tint(r.topLeft(), r.bottomRight());
            tint.setColorAt(0, QColor::fromHslF(0.06, 0.6, 0.5, 0.45));
            QColor fade = c.bg;
            fade.setAlphaF(0.6);
            tint.setColorAt(1, fade);
            p.fillPath(frame, tint);
        }

        const qreal u = r.width() / 42;
        const bool disc = skin == QLatin1String("turntable") || skin == QLatin1String("turntablecompact");
        const bool compact = skin == QLatin1String("albumcompact") || skin == QLatin1String("turntablecompact");
        const bool mini = skin == QLatin1String("mini");

        if (skin == QLatin1String("classic"))
        {
            // VLC's plain music view: a note in the middle
            p.setBrush(c.muted);
            const QPointF m = r.center();
            p.drawEllipse(QPointF(m.x() - 9, m.y() + 10), 7, 5.5);
            p.drawEllipse(QPointF(m.x() + 11, m.y() + 6), 7, 5.5);
            p.drawRect(QRectF(m.x() - 3.5, m.y() - 18, 2.5, 28));
            p.drawRect(QRectF(m.x() + 16.5, m.y() - 22, 2.5, 28));
            QPolygonF beam;
            beam << QPointF(m.x() - 3.5, m.y() - 18) << QPointF(m.x() + 19, m.y() - 24)
                 << QPointF(m.x() + 19, m.y() - 18) << QPointF(m.x() - 3.5, m.y() - 12);
            p.drawPolygon(beam);
        }
        else
        {
            if (compact || mini)
            {
                const QRectF card = mini ? QRectF(r.center().x() - r.width() * 0.38, r.center().y() - r.height() * 0.15,
                                                  r.width() * 0.76, r.height() * 0.3)
                                         : QRectF(r.center().x() - r.width() * 0.35, r.center().y() - r.height() * 0.26,
                                                  r.width() * 0.7, r.height() * 0.52);
                p.setBrush(c.panel);
                p.setPen(QPen(c.line, 1));
                p.drawRoundedRect(card, mini ? card.height() / 2 : u, mini ? card.height() / 2 : u);
                p.setPen(Qt::NoPen);
            }
            const qreal side = mini ? r.height() * 0.18 : compact ? r.height() * 0.38 : r.height() * 0.66;
            const qreal ax = mini ? r.left() + r.width() * 0.12 + u * 0.8
                       : compact ? r.left() + r.width() * 0.15 + u * 1.2 : r.left() + u * 3;
            const qreal ay = r.center().y() - side / 2 - ((compact || mini) ? 0 : u);
            const QRectF art(ax, ay, side, side);
            if (disc)
            {
                p.setBrush(QColor(18, 18, 20));
                p.setPen(QPen(QColor(255, 255, 255, 60), 1)); // a rim, so the record reads on dark cards too
                p.drawEllipse(art);
                p.setBrush(Qt::NoBrush);
                p.setPen(QPen(QColor(255, 255, 255, 22), 1));
                for (qreal g = 0.82; g > 0.45; g -= 0.12) // grooves
                    p.drawEllipse(art.center(), side * g / 2, side * g / 2);
                p.setPen(Qt::NoPen);
                p.setBrush(c.accent);
                p.drawEllipse(art.center(), side * 0.2, side * 0.2);
                p.setBrush(c.bg);
                p.drawEllipse(art.center(), 1.6, 1.6);
                p.setPen(QPen(QColor(170, 170, 175), 2, Qt::SolidLine, Qt::RoundCap)); // tone arm
                p.drawLine(QPointF(art.right() + u * 0.4, art.top()), QPointF(art.right() - side * 0.18, art.center().y() + side * 0.12));
                p.setPen(Qt::NoPen);
            }
            else
            {
                QLinearGradient cover(art.topLeft(), art.bottomLeft());
                cover.setColorAt(0, QColor::fromHslF(0.06, 0.62, 0.52));
                cover.setColorAt(1, QColor::fromHslF(0.1, 0.5, 0.25));
                p.setBrush(cover);
                p.drawRoundedRect(art, u * 0.6, u * 0.6);
            }

            const qreal tx = art.right() + u * (mini ? 0.8 : 1.5);
            qreal ty = art.center().y() - (mini ? u * 0.45 : u * 1.2);
            p.setBrush(c.text);
            p.drawRoundedRect(QRectF(tx, ty, u * (mini ? 7 : 10), u * 0.9), u * 0.45, u * 0.45);
            if (!mini)
            {
                ty += u * 1.4;
                p.setBrush(c.muted);
                p.drawRoundedRect(QRectF(tx, ty, u * 6, u * 0.7), u * 0.35, u * 0.35);
            }
            if (skin == QLatin1String("album") || skin == QLatin1String("turntable"))
            {
                QColor list = c.muted;
                list.setAlphaF(0.5);
                p.setBrush(list);
                for (int i = 0; i < 3; ++i)
                    p.drawRoundedRect(QRectF(tx, ty + u * (1.6 + i * 1.2), u * 12, u * 0.6), u * 0.3, u * 0.3);
            }
        }

        if (!mini) // seek line
        {
            const QRectF seek(r.left() + u * 2, r.bottom() - u * 2 - 2, r.width() - u * 4, 3);
            QColor track = c.text;
            track.setAlphaF(0.2);
            p.setBrush(track);
            p.drawRoundedRect(seek, 1.5, 1.5);
            p.setBrush(c.accent);
            p.drawRoundedRect(QRectF(seek.left(), seek.top(), seek.width() * 0.38, 3), 1.5, 1.5);
        }
    }
};

QComboBox *addChoice(QFormLayout *form, const QString &label, const Options &options, const QString &current,
                     const std::function<void(const QString &)> &apply)
{
    auto *box = new QComboBox;
    for (const auto &o : options)
        box->addItem(o.second, o.first);
    const int i = box->findData(current);
    box->setCurrentIndex(i >= 0 ? i : 0);
    QObject::connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), box,
                     [box, apply](int) { apply(box->currentData().toString()); });
    form->addRow(label, box);
    return box;
}

} // namespace

QWidget *vlx::createLookPanel(qt_intf_t *intf, QWidget *parent)
{
    VLXSettings *s = VLXSettings::instance();
    auto *group = new QGroupBox(qtr("Look (VLX)"), parent);
    if (!s)
    {
        group->hide();
        return group;
    }

    auto *row = new QHBoxLayout(group);
    auto *form = new QFormLayout;
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    auto *preview = new LookPreview(group);
    preview->theme = s->theme();
    preview->accent = s->accent();
    preview->barStyle = s->barStyle();
    preview->barPosition = s->barPosition();
    preview->barAlign = s->barAlign();
    preview->barSize = s->barSize();
    preview->pinBar = s->pinControls();
    preview->pinTop = s->pinTopBar();
    auto *audio = new AudioPreview(group);
    audio->skin = s->audioSkin();
    audio->background = s->audioBackground();
    audio->audioTheme = s->audioTheme();
    audio->appTheme = s->theme();
    audio->accent = s->accent();

    addChoice(form, qtr("Theme"),
              { { "default", qtr("VLC default") }, { "dark", qtr("Dark") }, { "light", qtr("Light") },
                { "paper", qtr("Paper") }, { "ink", qtr("Ink") }, { "nord", qtr("Nord") },
                { "mocha", qtr("Mocha") }, { "tokyo", qtr("Tokyo") } },
              s->theme(), [s, preview, audio](const QString &v) { s->setTheme(v); preview->theme = v; preview->update(); audio->appTheme = v; audio->update(); });
    QComboBox *accent = addChoice(form, qtr("Accent colour"),
              { { "default", qtr("Theme's own") }, { "#FF8800", qtr("VLC orange") }, { "#E6B450", qtr("Amber") },
                { "#E0466A", qtr("Rose") }, { "#5B8CFF", qtr("Blue") }, { "#10B981", qtr("Emerald") },
                { "#8B5CF6", qtr("Grape") }, { "#9F234A", qtr("Wine") }, { "system", qtr("Windows accent") } },
              s->accent(), [s, preview, audio](const QString &v) { s->setAccent(v); preview->accent = v; preview->update(); audio->accent = v; audio->update(); });
    for (int i = 0; i < accent->count(); ++i) // a swatch next to each colour
    {
        const QString v = accent->itemData(i).toString();
        if (!QColor::isValidColorName(v))
            continue;
        QPixmap swatch(14, 14);
        swatch.fill(Qt::transparent);
        QPainter sp(&swatch);
        sp.setRenderHint(QPainter::Antialiasing);
        sp.setBrush(QColor(v));
        sp.setPen(Qt::NoPen);
        sp.drawEllipse(1, 1, 12, 12);
        accent->setItemIcon(i, QIcon(swatch));
    }

    if (MainCtx *mi = intf->p_mi)
    {
        const double scale = mi->getIntfUserScaleFactor();
        QString current = QString::number(std::round(scale * 100) / 100);
        addChoice(form, qtr("Interface size"),
                  { { "0.8", "80 %" }, { "0.9", "90 %" }, { "1", "100 %" }, { "1.1", "110 %" },
                    { "1.25", "125 %" }, { "1.5", "150 %" } },
                  current, [mi](const QString &v) { mi->setIntfUserScaleFactor(v.toDouble()); });
    }

    addChoice(form, qtr("Player bar"),
              { { "classic", qtr("Classic") }, { "island", qtr("Island") }, { "docked", qtr("Docked bar") },
                { "minimal", qtr("Minimal") }, { "pill", qtr("Compact pill") } },
              s->barStyle(), [s, preview](const QString &v) { s->setBarStyle(v); preview->barStyle = v; preview->update(); });
    addChoice(form, qtr("Controls position"), { { "bottom", qtr("Bottom") }, { "top", qtr("Top") } },
              s->barPosition(), [s, preview](const QString &v) { s->setBarPosition(v); preview->barPosition = v; preview->update(); });
    addChoice(form, qtr("Controls alignment"), { { "center", qtr("Centered") }, { "left", qtr("Left") }, { "right", qtr("Right") } },
              s->barAlign(), [s, preview](const QString &v) { s->setBarAlign(v); preview->barAlign = v; preview->update(); });
    addChoice(form, qtr("Bar size"), { { "compact", qtr("Compact") }, { "normal", qtr("Normal") }, { "large", qtr("Large") } },
              s->barSize(), [s, preview](const QString &v) { s->setBarSize(v); preview->barSize = v; preview->update(); });
    addChoice(form, qtr("Show the controls"),
              { { "false", qtr("Over the video, auto-hide") }, { "true", qtr("Always, under the video") } },
              s->pinControls() ? QStringLiteral("true") : QStringLiteral("false"),
              [s, preview](const QString &v) { s->setPinControls(v == "true"); preview->pinBar = v == "true"; preview->update(); });
    addChoice(form, qtr("Show the top bar"),
              { { "false", qtr("Over the video, auto-hide") }, { "true", qtr("Always, above the video") } },
              s->pinTopBar() ? QStringLiteral("true") : QStringLiteral("false"),
              [s, preview](const QString &v) { s->setPinTopBar(v == "true"); preview->pinTop = v == "true"; preview->update(); });
    addChoice(form, qtr("When playback ends"),
              { { "player", qtr("Stay on the player") }, { "home", qtr("Go back to Home") } },
              s->endView(), [s](const QString &v) { s->setEndView(v); });
    addChoice(form, qtr("Hide controls after"),
              { { "1000", "1 s" }, { "2000", "2 s" }, { "3000", "3 s" }, { "5000", "5 s" }, { "86400000", qtr("Never") } },
              QString::number(s->intOption(QStringLiteral("mouse-hide-timeout"))),
              [s](const QString &v) { s->setIntOption(QStringLiteral("mouse-hide-timeout"), v.toInt()); });
    addChoice(form, qtr("Audio player skin"),
              { { "albumcompact", qtr("Album compact") }, { "album", qtr("Album") }, { "turntable", qtr("Turntable") },
                { "turntablecompact", qtr("Turntable compact") }, { "mini", qtr("Mini player") },
                { "classic", qtr("VLC classic") } },
              s->audioSkin(), [s, audio](const QString &v) { s->setAudioSkin(v); audio->skin = v; audio->update(); });
    addChoice(form, qtr("Audio background"),
              { { "plain", qtr("Plain theme") }, { "tinted", qtr("Tinted from the cover art") } },
              s->audioBackground(), [s, audio](const QString &v) { s->setAudioBackground(v); audio->background = v; audio->update(); });
    addChoice(form, qtr("Audio colours"),
              { { "app", qtr("Follow the app theme") }, { "dark", qtr("Dark") }, { "light", qtr("Light") },
                { "paper", qtr("Paper") }, { "ink", qtr("Ink") }, { "nord", qtr("Nord") },
                { "mocha", qtr("Mocha") }, { "tokyo", qtr("Tokyo") } },
              s->audioTheme(), [s, audio](const QString &v) { s->setAudioTheme(v); audio->audioTheme = v; audio->update(); });
#ifdef _WIN32
    addChoice(form, qtr("File icon style"), { { "band", qtr("Page and band") }, { "echo", qtr("Echo") } },
              s->fileIcons(), [s](const QString &v) { s->setFileIcons(v); });
    addChoice(form, qtr("File icon look"),
              { { "auto", qtr("Follow Windows") }, { "light", qtr("Light") }, { "dark", qtr("Dark") } },
              s->fileIconLook(), [s](const QString &v) { s->setFileIconLook(v); });
#endif

    auto *note = new QLabel(qtr("Changes apply at once. The same settings, with search, are on the Settings page."));
    note->setWordWrap(true);
    note->setEnabled(false);
    // VLC 4's toolbar editor: the buttons on the video, audio and mini player bars, with profiles
    auto *buttons = new QPushButton(qtr("Customise player buttons..."));
    QObject::connect(buttons, &QPushButton::clicked, DialogsProvider::getInstance(), &DialogsProvider::showToolbarEditorDialog);
    form->addRow(qtr("Player buttons"), buttons);

    auto *left = new QVBoxLayout;
    left->addLayout(form);
    left->addWidget(note);
    left->addStretch(1);

    auto *previews = new QVBoxLayout;
    previews->addWidget(preview);
    auto *audioLabel = new QLabel(qtr("Audio player"));
    audioLabel->setEnabled(false);
    previews->addSpacing(6);
    previews->addWidget(audioLabel);
    previews->addWidget(audio);
    previews->addStretch(1);

    row->addLayout(left, 1);
    row->addSpacing(12);
    row->addLayout(previews);
    return group;
}
