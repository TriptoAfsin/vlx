// Renders the preferences dialog's category icons (modules/gui/qt/pixmaps/prefs/spref_*.png) in the VLX style:
// a cream line glyph on the VLX icon's orange tile, 128 px (the dialog scales them down).
//
//   node extras/vlx/scripts/pref-icons.mjs
//
// Needs Node and Playwright (set PLAYWRIGHT_CORE to its playwright-core folder if needed); uses Playwright's
// Chromium, or the Edge or Chrome already installed.
import { createRequire } from 'node:module';
import { writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const require = createRequire(import.meta.url);
const { chromium } = require(process.env.PLAYWRIGHT_CORE || 'playwright-core');
const OUT = join(dirname(fileURLToPath(import.meta.url)), '..', '..', '..', 'modules', 'gui', 'qt', 'pixmaps', 'prefs');
const S = 128;

// Glyphs on a 24 grid, stroked.
const glyphs = {
  interface: '<rect x="3" y="4" width="18" height="16" rx="2.5"/><path d="M3 9h18M9 9v11"/>',
  audio: '<path d="M4 9.5h3.5L12 5.5v13l-4.5-4H4z"/><path d="M15.5 9.5a3.5 3.5 0 0 1 0 5M18 7a7 7 0 0 1 0 10"/>',
  video: '<rect x="3" y="5" width="18" height="14" rx="2.5"/><path d="M10 9.2v5.6l4.6-2.8z" fill="#fff6ec"/>',
  subtitles: '<rect x="3" y="5" width="18" height="14" rx="2.5"/><path d="M7 11.5h4M13.5 11.5H17M7 15h7M16.5 15h.5"/>',
  input: '<path d="M12 3.5l8.5 4.5L12 12.5 3.5 8z"/><path d="M3.5 12l8.5 4.5 8.5-4.5M3.5 16l8.5 4.5 8.5-4.5"/>',
  hotkeys: '<rect x="2.5" y="6" width="19" height="12" rx="2.5"/><path d="M6.5 10h.01M10 10h.01M14 10h.01M17.5 10h.01M7.5 14h9"/>',
  medialibrary: '<path d="M5 4.5v15M9 4.5v15"/><path d="M13 5.5l3.6-1 3.6 14-3.6 1z"/>',
};

const tile = (g) => `<svg xmlns="http://www.w3.org/2000/svg" width="${S}" height="${S}" viewBox="0 0 256 256">
  <defs><linearGradient id="t" x1="0" y1="0" x2="0" y2="1">
    <stop offset="0" stop-color="#ff8a33"/><stop offset="1" stop-color="#f26a0a"/></linearGradient></defs>
  <path d="M128 8C40 8 8 40 8 128s32 120 120 120 120-32 120-120S216 8 128 8z" fill="url(#t)"/>
  <g transform="translate(56 56) scale(6)" fill="none" stroke="#fff6ec" stroke-width="1.9"
     stroke-linecap="round" stroke-linejoin="round">${g}</g>
</svg>`;

const browser = await chromium.launch().catch(() => chromium.launch({ channel: 'msedge' }))
  .catch(() => chromium.launch({ channel: 'chrome' }));
const page = await browser.newPage();
for (const [name, g] of Object.entries(glyphs)) {
  const png = await page.evaluate(async ({ svg, S }) => {
    const img = new Image();
    img.src = 'data:image/svg+xml;base64,' + btoa(svg);
    await img.decode();
    const c = document.createElement('canvas');
    c.width = S; c.height = S;
    c.getContext('2d').drawImage(img, 0, 0);
    return c.toDataURL('image/png');
  }, { svg: tile(g), S });
  writeFileSync(join(OUT, `spref_${name}.png`), Buffer.from(png.split(',')[1], 'base64'));
  console.log('spref_' + name + '.png');
}
await browser.close();
