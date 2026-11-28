import release from "./release.json";

const GH = "https://github.com/TriptoAfsin/vlz";

export const SITE = {
  url: "https://vlz.t21.dev",
  repo: GH,
  readme: `${GH}#readme`,
  releases: `${GH}/releases`,
  design: `${GH}/blob/main/DESIGN.md`,
  roadmap: `${GH}/blob/main/ROADMAP.md`,
  build: `${GH}/blob/main/doc/BUILD-win32.md`,
  verify: `${GH}/blob/main/VERIFY.md`,
  upstream: `${GH}/blob/main/UPSTREAM.md`,
  copying: `${GH}/blob/main/COPYING`,
  copyingLib: `${GH}/blob/main/COPYING.LIB`,
  authors: `${GH}/blob/main/AUTHORS`,
  thanks: `${GH}/blob/main/THANKS`,
  vlcReadme: `${GH}/blob/main/README.VLC.md`,
  vlc: "https://www.videolan.org/vlc/",
  videolan: "https://www.videolan.org",
};

export type Asset = { name: string; url: string; size: number };
type Assets = Partial<Record<"windows" | "windowsPortable" | "checksums", Asset>>;
export const RELEASE = release as { version: string; date: string; page: string; assets: Assets };

/** Bytes to MB the way Windows Explorer shows it (MiB), one decimal. */
export const mb = (bytes: number) => (bytes / 1048576).toFixed(1);

export const installerMB = RELEASE.assets.windows ? mb(RELEASE.assets.windows.size) : "47.9";

export const releaseDate = new Date(RELEASE.date + "T00:00:00Z").toLocaleDateString("en-GB", {
  day: "numeric",
  month: "long",
  year: "numeric",
  timeZone: "UTC",
});

// From the README's "Goals" on `main`.
export const GOALS: { title: string; body: string }[] = [
  {
    title: "UI first.",
    body: "A calmer, more modern Qt/QML interface: consistent spacing, softer surfaces, smooth transitions, a refreshed player view and media library.",
  },
  {
    title: "Performance second.",
    body: "Faster startup and a snappier UI through build flags and defaults (LTO, lazy-loaded views), and by not building rarely used modules.",
  },
  {
    title: "Low upkeep.",
    body: "Small, contained diffs, so a new VLC release merges in with little work.",
  },
];

// The README's "Screenshots (design previews)": HTML mock-ups, not the running app.
export const PREVIEWS: { shot: string; title: string; alt: string }[] = [
  { shot: "player-video", title: "Video", alt: "Design preview of the player with a video: floating control island and the up-next queue" },
  {
    shot: "player-audio",
    title: "Audio",
    alt: "Design preview of the player with an audio file: cover art, track details, waveform seek bar and track list",
  },
  { shot: "library", title: "Library", alt: "Design preview of the library: resume card and a sortable table" },
  { shot: "settings-search", title: "Settings search", alt: "Design preview of settings: searching “gpu” jumps to Hardware decoding" },
];

// From ROADMAP.md on `main`. Status there: planned.
export const ROADMAP: { title: string; items: string[] }[] = [
  {
    title: "1. Interface",
    items: [
      "A new palette: neutral ramp, accent, hover and selection states",
      "Rounder corners, retuned durations and easing",
      "Page transitions, and a crossfade between library and player",
      "Control bar auto-hide with a fade and a short slide",
      "Grid hover with a rounded background, a small lift and a shadow",
      "Windows 11 Mica backdrop and rounded window corners",
      "A bundled UI font, and a longer mouse-hide timeout",
    ],
  },
  {
    title: "2. Branding",
    items: [
      "VLZ's own icons, in-app artwork and window title",
      "Binary names, config paths and plugins stay as VLC has them, so settings and rebases keep working",
    ],
  },
  {
    title: "3. Performance",
    items: [
      "QML compiled ahead of time, for a faster cold start",
      "Release builds with LTO and -O3",
      "The player view loaded in the background, for a faster first switch",
      "Network discovery off by default, and the first library scan after the first frame",
    ],
  },
];

export const FUTURE = "A command palette (Ctrl K), resume-position sync, a better subtitle search flow.";

export const BUILD_CMD = "./extras/package/win32/build.sh -a x86_64 -p -m";
