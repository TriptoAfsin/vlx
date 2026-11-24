/*****************************************************************************
 * mediacontrols_win32.cpp: Windows media controls (System Media Transport Controls)
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

#include "mediacontrols_win32.hpp"

#include "qt.hpp"
#include "player/player_controller.hpp"
#include "playlist/playlist_controller.hpp"
#include "vlzsettings.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QUrl>

#include <algorithm>
#include <functional>
#include <QElapsedTimer>
#include <QPointer>
#include <QWindow>

/* MinGW's windows.foundation.h declares IReference<boolean> after IReference<BYTE>, the same C++ type there (boolean is
 * unsigned char), and older versions then fail on the second UUID. Skip the duplicate; fixed headers use the same guard. */
#define ____FIReference_1_boolean_INTERFACE_DEFINED__
#include <windows.h>
#include <roapi.h>
#include <winstring.h>
#include <shcore.h>
#include <systemmediatransportcontrolsinterop.h>
#include <windows.media.h>
#include <windows.storage.streams.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
namespace WM = ABI::Windows::Media;
namespace WF = ABI::Windows::Foundation;
namespace WSS = ABI::Windows::Storage::Streams;

/* Windows 8+ exports that VLC's Windows 7 target hides in the headers */
extern "C" HRESULT WINAPI CreateRandomAccessStreamOnFile(PCWSTR filePath, DWORD accessMode, REFIID riid, void **ppv);

/* WinRT interfaces declared here (see below). They live in a named namespace on purpose: in an anonymous one, GCC
 * sees that nothing in this file implements them and, at -O3, turns calls through them into unreachable code. */
namespace vlz_winrt {

/* Windows.Media.IVideoDisplayProperties, which this MinGW only forward-declares: the methods in Windows' order. The
 * object comes from the display updater's typed getter, so only this order matters. */
struct VideoDisplayProperties : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_Title(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Title(HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Subtitle(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Subtitle(HSTRING value) = 0;
};

/* Three interfaces that older MinGW headers (VideoLAN's build image) lack, declared here so every header version
 * builds: the methods in Windows' order, with their IIDs. Enum arguments are 32-bit integers in the ABI. */
struct StreamReferenceStatics : public IInspectable /* Windows.Storage.Streams.IRandomAccessStreamReferenceStatics */
{
    /* The result is an IRandomAccessStreamReference, which those headers only forward-declare */
    virtual HRESULT STDMETHODCALLTYPE CreateFromFile(IUnknown *file, IUnknown **out) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFromUri(IUnknown *uri, IUnknown **out) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFromStream(WSS::IRandomAccessStream *stream, IUnknown **out) = 0;
};
const IID kStreamReferenceStaticsIid = { 0x857309dc, 0x3fbf, 0x4e7d, { 0x98, 0x6f, 0xef, 0x3b, 0x1a, 0x07, 0xa9, 0x64 } };

struct TimelineProperties : public IInspectable /* Windows.Media.ISystemMediaTransportControlsTimelineProperties */
{
    virtual HRESULT STDMETHODCALLTYPE get_StartTime(WF::TimeSpan *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartTime(WF::TimeSpan value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EndTime(WF::TimeSpan *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EndTime(WF::TimeSpan value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MinSeekTime(WF::TimeSpan *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MinSeekTime(WF::TimeSpan value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxSeekTime(WF::TimeSpan *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxSeekTime(WF::TimeSpan value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Position(WF::TimeSpan *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Position(WF::TimeSpan value) = 0;
};
const IID kTimelinePropertiesIid = { 0x5125316a, 0xc3a2, 0x475b, { 0x85, 0x07, 0x93, 0x53, 0x4d, 0xc8, 0x8f, 0x15 } };

struct TransportControls2 : public IInspectable /* Windows.Media.ISystemMediaTransportControls2, up to the one VLZ uses */
{
    virtual HRESULT STDMETHODCALLTYPE get_AutoRepeatMode(INT32 *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AutoRepeatMode(INT32 value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ShuffleEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ShuffleEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PlaybackRate(DOUBLE *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PlaybackRate(DOUBLE value) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateTimelineProperties(TimelineProperties *timeline) = 0;
};
const IID kTransportControls2Iid = { 0xea98d2f6, 0x7f3c, 0x4af2, { 0xa5, 0x86, 0x72, 0x88, 0x98, 0x08, 0xef, 0xb1 } };

} // namespace vlz_winrt
using namespace vlz_winrt;

namespace {

/* An HSTRING for the WinRT calls, released when it goes out of scope. */
class HString
{
public:
    explicit HString(const QString &s)
    {
        const std::wstring w = s.toStdWString();
        WindowsCreateString(w.c_str(), UINT32(w.size()), &h);
    }
    explicit HString(const WCHAR *s) { WindowsCreateString(s, UINT32(wcslen(s)), &h); }
    ~HString() { if (h) WindowsDeleteString(h); }
    HString(const HString &) = delete;
    HString &operator=(const HString &) = delete;
    operator HSTRING() const { return h; }
private:
    HSTRING h = nullptr;
};

using ButtonHandlerBase = WF::ITypedEventHandler<WM::SystemMediaTransportControls *,
                                                 WM::SystemMediaTransportControlsButtonPressedEventArgs *>;

/* Windows calls this on a thread of its own; the button goes to the UI thread, where the playlist lives. */
// ITypedEventHandler<SystemMediaTransportControls*, SystemMediaTransportControlsButtonPressedEventArgs*>. The headers
// declare it, but no MinGW import library defines it.
const IID kButtonHandlerIid = { 0x0557e996, 0x7b23, 0x5bae, { 0xaa, 0x81, 0xea, 0x0d, 0x67, 0x11, 0x43, 0xa4 } };

class ButtonHandler : public ButtonHandlerBase
{
public:
    explicit ButtonHandler(std::function<void(WM::SystemMediaTransportControlsButton)> f) : m_f(std::move(f)) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **out) override
    {
        if (!out)
            return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IAgileObject
            || riid == kButtonHandlerIid)
        {
            *out = static_cast<ButtonHandlerBase *>(this);
            AddRef();
            return S_OK;
        }
        *out = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refs); }
    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG refs = InterlockedDecrement(&m_refs);
        if (refs == 0)
            delete this;
        return refs;
    }
    HRESULT STDMETHODCALLTYPE Invoke(WM::ISystemMediaTransportControls *,
                                     WM::ISystemMediaTransportControlsButtonPressedEventArgs *args) override
    {
        WM::SystemMediaTransportControlsButton button;
        if (args && SUCCEEDED(args->get_Button(&button)))
            m_f(button);
        return S_OK;
    }

private:
    virtual ~ButtonHandler() = default;
    LONG m_refs = 1;
    std::function<void(WM::SystemMediaTransportControlsButton)> m_f;
};

WF::TimeSpan span(vlc_tick_t t) { return WF::TimeSpan{ INT64(t) * 10 }; } /* µs -> 100 ns */

} // namespace

struct vlz::MediaControlsWin32::Private
{
    qt_intf_t *p_intf = nullptr;
    ComPtr<WM::ISystemMediaTransportControls> smtc;
    EventRegistrationToken buttonToken {};
    QElapsedTimer sinceTimeline;

    void updateStatus()
    {
        PlayerController *player = THEMIM;
        WM::MediaPlaybackStatus status = WM::MediaPlaybackStatus_Closed;
        switch (player->getPlayingState())
        {
        case PlayerController::PLAYING_STATE_STARTED:
        case PlayerController::PLAYING_STATE_PLAYING: status = WM::MediaPlaybackStatus_Playing; break;
        case PlayerController::PLAYING_STATE_PAUSED: status = WM::MediaPlaybackStatus_Paused; break;
        case PlayerController::PLAYING_STATE_STOPPING:
        case PlayerController::PLAYING_STATE_STOPPED: status = WM::MediaPlaybackStatus_Stopped; break;
        }
        smtc->put_PlaybackStatus(status);
        /* Shown only while there is something to show, and while Settings has it on */
        VLZSettings *settings = VLZSettings::instance();
        smtc->put_IsEnabled((!settings || settings->mediaControls())
                            && status != WM::MediaPlaybackStatus_Stopped && status != WM::MediaPlaybackStatus_Closed);
        updateTimeline(true);
    }

    void updateMetadata()
    {
        PlayerController *player = THEMIM;
        ComPtr<WM::ISystemMediaTransportControlsDisplayUpdater> updater;
        if (FAILED(smtc->get_DisplayUpdater(&updater)))
            return;
        updater->ClearAll();
        const QString title = !player->getTitle().isEmpty() ? player->getTitle() : player->getName();
        if (player->hasVideoOutput())
        {
            updater->put_Type(WM::MediaPlaybackType_Video);
            WM::IVideoDisplayProperties *raw = nullptr;
            if (SUCCEEDED(updater->get_VideoProperties(&raw)) && raw)
            {
                auto *video = reinterpret_cast<VideoDisplayProperties *>(raw);
                video->put_Title(HString(title));
                video->put_Subtitle(HString(player->getArtist()));
                video->Release();
            }
        }
        else
        {
            updater->put_Type(WM::MediaPlaybackType_Music);
            ComPtr<WM::IMusicDisplayProperties> music;
            if (SUCCEEDED(updater->get_MusicProperties(&music)))
            {
                music->put_Title(HString(title));
                music->put_Artist(HString(player->getArtist()));
                music->put_AlbumArtist(HString(player->getAlbum()));
            }
        }
        setThumbnail(updater.Get(), player->getArtwork());
        updater->Update();
    }

    /* The cover, from VLC's art (a local file once fetched or extracted) */
    void setThumbnail(WM::ISystemMediaTransportControlsDisplayUpdater *updater, const QUrl &art)
    {
        if (!art.isLocalFile())
            return;
        const std::wstring path = QDir::toNativeSeparators(art.toLocalFile()).toStdWString();
        ComPtr<WSS::IRandomAccessStream> stream;
        if (FAILED(CreateRandomAccessStreamOnFile(path.c_str(), 0 /* FileAccessMode_Read */,
                                                  IID_PPV_ARGS(&stream))))
            return;
        ComPtr<StreamReferenceStatics> statics;
        if (FAILED(RoGetActivationFactory(HString(L"Windows.Storage.Streams.RandomAccessStreamReference"),
                                          kStreamReferenceStaticsIid,
                                          reinterpret_cast<void **>(statics.ReleaseAndGetAddressOf()))))
            return;
        ComPtr<IUnknown> reference;
        if (SUCCEEDED(statics->CreateFromStream(stream.Get(), &reference)))
            updater->put_Thumbnail(reinterpret_cast<WSS::IRandomAccessStreamReference *>(reference.Get()));
    }

    /* Windows 11 shows a progress bar: length and position, refreshed at most once a second while playing */
    void updateTimeline(bool force)
    {
        if (!force && sinceTimeline.isValid() && sinceTimeline.elapsed() < 1000)
            return;
        sinceTimeline.restart();
        ComPtr<TransportControls2> smtc2;
        if (FAILED(smtc->QueryInterface(kTransportControls2Iid, reinterpret_cast<void **>(smtc2.ReleaseAndGetAddressOf()))))
            return;
        ComPtr<IInspectable> instance;
        if (FAILED(RoActivateInstance(HString(L"Windows.Media.SystemMediaTransportControlsTimelineProperties"), &instance)))
            return;
        ComPtr<TimelineProperties> timeline;
        if (FAILED(instance->QueryInterface(kTimelinePropertiesIid, reinterpret_cast<void **>(timeline.ReleaseAndGetAddressOf()))))
            return;
        PlayerController *player = THEMIM;
        const vlc_tick_t length = player->getLength().valid() ? player->getLength().toVLCTick() : 0;
        const vlc_tick_t time = player->getTime().valid() ? player->getTime().toVLCTick() : 0;
        timeline->put_StartTime(span(0));
        timeline->put_MinSeekTime(span(0));
        timeline->put_EndTime(span(length));
        timeline->put_MaxSeekTime(span(length));
        timeline->put_Position(span(std::min(time, length)));
        smtc2->UpdateTimelineProperties(timeline.Get());
    }
};

vlz::MediaControlsWin32::MediaControlsWin32(qt_intf_t *p_intf, QWindow *window, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Private>())
{
    d->p_intf = p_intf;
    /* Set up even when Settings has it off, so turning it on applies at once; off, it is never enabled */
    if (!p_intf || !window)
        return;

    ComPtr<ISystemMediaTransportControlsInterop> interop;
    HRESULT hr = RoGetActivationFactory(HString(L"Windows.Media.SystemMediaTransportControls"), IID_PPV_ARGS(&interop));
    if (SUCCEEDED(hr))
        hr = interop->GetForWindow(reinterpret_cast<HWND>(window->winId()), IID_PPV_ARGS(&d->smtc));
    if (FAILED(hr) || !d->smtc)
    {
        msg_Dbg(p_intf, "Windows media controls unavailable (0x%lx)", hr);
        d->smtc.Reset();
        return;
    }

    d->smtc->put_IsPlayEnabled(true);
    d->smtc->put_IsPauseEnabled(true);
    d->smtc->put_IsStopEnabled(true);
    d->smtc->put_IsNextEnabled(true);
    d->smtc->put_IsPreviousEnabled(true);

    QPointer<QObject> guard(this);
    auto *handler = new ButtonHandler([guard, p_intf](WM::SystemMediaTransportControlsButton button) {
        QMetaObject::invokeMethod(qApp, [guard, p_intf, button]() {
            if (!guard)
                return;
            vlc::playlist::PlaylistController *playlist = THEMPL;
            switch (button)
            {
            case WM::SystemMediaTransportControlsButton_Play:
                // play() only starts a stopped playlist; a paused one resumes with the toggle.
                if (THEMIM->getPlayingState() == PlayerController::PLAYING_STATE_PAUSED)
                    playlist->togglePlayPause();
                else
                    playlist->play();
                break;
            case WM::SystemMediaTransportControlsButton_Pause: playlist->pause(); break;
            case WM::SystemMediaTransportControlsButton_Stop: playlist->stop(); break;
            case WM::SystemMediaTransportControlsButton_Next: playlist->next(); break;
            case WM::SystemMediaTransportControlsButton_Previous: playlist->prev(); break;
            default: break;
            }
        }, Qt::QueuedConnection);
    });
    d->smtc->add_ButtonPressed(handler, &d->buttonToken);
    handler->Release();

    PlayerController *player = THEMIM;
    connect(player, &PlayerController::playingStateChanged, this, [this]() { d->updateStatus(); });
    connect(player, &PlayerController::currentMetaChanged, this, [this]() { d->updateMetadata(); });
    connect(player, &PlayerController::nameChanged, this, [this]() { d->updateMetadata(); });
    connect(player, &PlayerController::hasVideoOutputChanged, this, [this]() { d->updateMetadata(); });
    connect(player, &PlayerController::lengthChanged, this, [this]() { d->updateTimeline(true); });
    connect(player, &PlayerController::timeChanged, this, [this]() { d->updateTimeline(false); });
    if (VLZSettings *settings = VLZSettings::instance())
        connect(settings, &VLZSettings::mediaControlsChanged, this, [this]() { d->updateStatus(); });
    d->updateStatus();
    d->updateMetadata();
    msg_Dbg(p_intf, "Windows media controls on");
}

vlz::MediaControlsWin32::~MediaControlsWin32()
{
    if (d->smtc)
    {
        d->smtc->remove_ButtonPressed(d->buttonToken);
        d->smtc->put_IsEnabled(false);
    }
}
