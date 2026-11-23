/*****************************************************************************
 * vlz_lite.c: VLZ Lite mode (Settings › Performance › Lite mode)
 *****************************************************************************
 * Copyright (C) 2026 VLZ contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*
 * Lite mode is one switch ("qt-vlz-lite") for a lighter set of VLC options. It is applied once, at start-up,
 * as variables on the libvlc object: every module reads its options with var_Inherit*(), which finds these
 * before the saved config. Nothing is written to the config, so turning Lite mode off brings back exactly
 * what was set before. An option given on the command line wins over Lite mode.
 *
 * Each entry was kept because it measurably saves memory or start-up time; see extras/vlz/benchmarks.md.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_variables.h>

#include "config/configuration.h"
#include "vlz_lite.h"

struct vlz_lite_option
{
    const char *name;
    int type;          /* VLC_VAR_BOOL or VLC_VAR_INTEGER */
    int64_t value;
};

static const struct vlz_lite_option lite_options[] =
{
    /* Memory saver: 2 decoder threads and 0.3 s read-ahead instead of automatic threads and 1 s.
     * 1080p H.264 playing: 321 -> 271 MB with the rest of Lite. */
    { "avcodec-threads",          VLC_VAR_INTEGER, 2 },
    { "file-caching",             VLC_VAR_INTEGER, 300 },
    /* No background parsing of queued items, and no cover art or metadata from the internet:
     * no vlc-preparser helper process (3.5 MB) */
    { "auto-preparse",            VLC_VAR_BOOL,    false },
    { "metadata-network-access",  VLC_VAR_BOOL,    false },
    /* The media library is skipped in libvlc.c (vlz_LiteEnabled), and the Qt interface draws itself with
     * Qt Quick's software renderer (qt.cpp): idle 144 -> 80 MB, about 170 ms sooner to a ready window.
     * Measured and left out because they saved nothing: no backdrop blur, no tray icon, no Lua extensions,
     * the basic render loop, the QML interpreter. */
};

bool vlz_LiteEnabled(vlc_object_t *obj)
{
    /* The option belongs to the Qt module; without it (another interface, a stripped build) Lite is off */
    return config_FindConfig("qt-vlz-lite") != NULL && var_InheritBool(obj, "qt-vlz-lite");
}

void vlz_LiteApply(vlc_object_t *libvlc)
{
    if (!vlz_LiteEnabled(libvlc))
        return;

    for (size_t i = 0; i < ARRAY_SIZE(lite_options); i++)
    {
        const struct vlz_lite_option *o = &lite_options[i];

        if (config_FindConfig(o->name) == NULL)
            continue; /* its module isn't in this build */
        if (var_Type(libvlc, o->name) != 0)
            continue; /* set on the command line: that wins */

        if (var_Create(libvlc, o->name, o->type) != VLC_SUCCESS)
            continue;
        if (o->type == VLC_VAR_BOOL)
            var_SetBool(libvlc, o->name, o->value != 0);
        else
            var_SetInteger(libvlc, o->name, o->value);
    }
    msg_Dbg(libvlc, "VLZ Lite mode is on");
}
