/*****************************************************************************
 * quitfade.cpp: quit without a click or a stale tail of audio
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

#include "quitfade.hpp"

#include "qt.hpp"
#include "maininterface/mainctx.hpp"
#include "player/player_controller.hpp"
#include "playlist/playlist_controller.hpp"

#include <QCoreApplication>
#include <QTimer>
#include <QWindow>

void vlz::quitAfterFade(qt_intf_t *p_intf, std::function<void()> quit)
{
    PlayerController *player = p_intf ? THEMIM : nullptr;
    if (!player || player->getPlayingState() != PlayerController::PLAYING_STATE_PLAYING
        || player->isMuted() || player->getVolume() <= 0.f)
    {
        quit();
        return;
    }

    if (p_intf->p_mi && p_intf->p_mi->intfMainWindow())
        p_intf->p_mi->intfMainWindow()->hide(); /* the close feels instant; the fade is only heard */

    const float volume = player->getVolume();
    constexpr int fadeSteps = 6;      /* 6 x 15 ms: 90 ms, quick but not a step */
    constexpr int silenceSteps = 8;   /* 120 ms of silence for the device buffer */
    constexpr int maxStopSteps = 20;  /* wait up to 300 ms for playback to stop */
    auto *timer = new QTimer(qApp);
    timer->setInterval(15);
    QObject::connect(timer, &QTimer::timeout, timer, [p_intf, player, volume, quit, timer, step = 0]() mutable {
        ++step;
        if (step <= fadeSteps)
        {
            const float left = 1.f - float(step) / fadeSteps;
            player->setVolume(volume * left * left); /* eased: loudness falls evenly */
            return;
        }
        if (step <= fadeSteps + silenceSteps)
        {
            if (step == fadeSteps + silenceSteps)
                THEMPL->stop();
            return;
        }
        const bool stopped = player->getPlayingState() == PlayerController::PLAYING_STATE_STOPPED;
        if (!stopped && step < fadeSteps + silenceSteps + maxStopSteps)
            return;
        timer->stop();
        timer->deleteLater();
        player->setVolume(volume); /* nothing plays any more, so this is silent */
        quit();
    });
    timer->start();
}
