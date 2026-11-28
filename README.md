# VLZ website

This is the `landing` branch of [TriptoAfsin/vlz](https://github.com/TriptoAfsin/vlz). It holds **only** the website at <https://vlz.t21.dev>. The app (a fork of VLC) lives on `main`.

This branch is an orphan. Never merge it with `main`.

## Stack

- React 19 + TypeScript + Vite, prerendered to static HTML at build time (`src/entry-server.tsx` → `scripts/prerender.mjs`), then hydrated.
- Plain CSS with tokens in `src/styles/tokens.css`, taken from the Dark theme in `DESIGN.md` on `main`; the accent is the orange of VLZ's own icon. Type is the system face (Segoe UI Variable on Windows), as `DESIGN.md` asks, so the page loads no fonts and makes no third-party requests.
- Cloudflare Workers static assets (`wrangler.jsonc`), custom domain `vlz.t21.dev`.

## Commands

```bash
npm install
npm run dev       # local dev server
npm run build     # fetch latest release, typecheck, build, prerender
npm run check     # verify SEO tags, OG image, sitemap, alt text in dist/
npm run og        # re-render public/og.png from the release data
npm run deploy    # build and `wrangler deploy` (needs `wrangler login`)
```

## Content that comes from the app

- **Release data.** `scripts/release.mjs` asks the GitHub API for the latest release at build time and writes `src/release.json`: the version, date, and the setup, portable and checksum links and sizes. If GitHub is unreachable, the committed `release.json` is used. VLZ ships **Windows x64 only**; the page tells macOS and Linux visitors so instead of offering a file.
- **Copy.** Everything on the page comes from `README.md` and `ROADMAP.md` on `main`. Do not add a feature or a number that is not there.
- **Trademarks.** VLZ is not affiliated with or endorsed by VideoLAN. The README's disclaimer stays visible on the page (hero, credits and footer). Use only VLZ's own name and icon (`extras/vlz/brand/`), never VLC's cone. Keep the credit to VideoLAN and the GPLv2+/LGPLv2.1+ links (`COPYING`, `COPYING.LIB`).
- **Images.** `public/screenshots/*.webp` are WebP copies of `extras/vlz/screenshots/*.png` on `main` (1860px and 930px wide). These are **design previews (HTML mock-ups), not the running app**, and the page labels them that way. Replace them with real screenshots when the interface is built. To re-export:
  `ffmpeg -i library.png -c:v libwebp -quality 84 library.webp`
  `ffmpeg -i library.png -vf scale=930:-2 -c:v libwebp -quality 84 library-930.webp`
- **Icons.** `public/icon.svg`, `favicon.ico`, `favicon-32.png` and `icon-512.png` are copied from `extras/vlz/brand/`; `icon-192.png` and `apple-touch-icon.png` are scaled from the 512px icon.

## Deploying

Workers Builds: connect this repository, set the production branch to `landing`, build command `npm run build`, deploy command `npx wrangler deploy`. A push here deploys. Re-run the build after each app release so the version and download links update (or trigger a deploy hook from the release workflow on `main`).

## SEO

`index.html` carries the title, description, canonical, Open Graph and Twitter tags and a JSON-LD `SoftwareApplication`; `scripts/prerender.mjs` fills in the version, and writes `404.html` and `sitemap.xml`. `public/robots.txt` points at the sitemap. `npm run check` fails if any of that is missing.
