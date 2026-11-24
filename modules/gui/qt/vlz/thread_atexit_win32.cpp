/*****************************************************************************
 * thread_atexit_win32.cpp: thread_local destructors that run while their storage is still there
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

/* Why this file exists
 *
 * GCC on MinGW keeps thread_local variables in emulated TLS, which winpthreads frees when a thread exits (a pthread
 * key destructor, run from its TLS callback in .CRT$XLF). libstdc++'s __cxa_thread_atexit runs the thread_local
 * destructors from another pthread key, in no fixed order with the first. In the release build (VideoLAN's image,
 * mingw-w64 12, static libstdc++ and winpthreads) the storage often went first, so Qt's thread_local objects (the
 * regular expression JIT stacks, the animation timers) were destroyed from freed memory: every Qt thread crashed as
 * it ended, on every close, and the process could then hang for good.
 *
 * This definition replaces libstdc++'s (the linker takes it before looking in the archive), the way newer
 * mingw-w64 does it: a list per thread in native Windows TLS, run from a TLS callback in .CRT$XLB, which Windows
 * calls before .CRT$XLF, while the emulated TLS is still intact. */

#ifdef _WIN32

#include <windows.h>
#include <cstdlib>

namespace {

struct Destructor
{
    void (*fn)(void *);
    void *object;
    Destructor *next;
};

DWORD tlsIndex = TLS_OUT_OF_INDEXES;
INIT_ONCE tlsOnce = INIT_ONCE_STATIC_INIT;

BOOL CALLBACK allocIndex(PINIT_ONCE, PVOID, PVOID *)
{
    tlsIndex = TlsAlloc();
    return tlsIndex != TLS_OUT_OF_INDEXES;
}

/* Last registered, first destroyed, as C++ requires; destructors may register more, which run too */
void runDestructors()
{
    if (tlsIndex == TLS_OUT_OF_INDEXES)
        return;
    while (auto *d = static_cast<Destructor *>(TlsGetValue(tlsIndex)))
    {
        TlsSetValue(tlsIndex, d->next);
        d->fn(d->object);
        free(d);
    }
}

void NTAPI tlsCallback(PVOID, DWORD reason, PVOID)
{
    /* Threads only: at process exit VLZ ends the process directly (bin/winvlc.c), and Qt is gone by then anyway */
    if (reason == DLL_THREAD_DETACH)
        runDestructors();
}

} // namespace

extern "C" int __cxa_thread_atexit(void (*fn)(void *), void *object, void * /* dso_handle */)
{
    if (!InitOnceExecuteOnce(&tlsOnce, allocIndex, nullptr, nullptr))
        return -1;
    auto *d = static_cast<Destructor *>(malloc(sizeof(Destructor)));
    if (!d)
        return -1;
    d->fn = fn;
    d->object = object;
    d->next = static_cast<Destructor *>(TlsGetValue(tlsIndex));
    TlsSetValue(tlsIndex, d);
    return 0;
}

/* Registered in the image's TLS callback table; "used" keeps it from being dropped */
extern "C" __attribute__((section(".CRT$XLB"), used)) const PIMAGE_TLS_CALLBACK vlz_thread_atexit_callback = tlsCallback;

#endif
