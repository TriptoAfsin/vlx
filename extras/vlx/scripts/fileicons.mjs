// Generates VLX file-type icons: extras/vlx/brand/filetypes/<style>-<look>/<name>.ico
// Styles: band (Page and band), echo. Looks: light, dark. See DESIGN.md › File icons.
//
//   node extras/vlx/scripts/fileicons.mjs
//
// Needs Node and Playwright (any install: set PLAYWRIGHT_CORE to its playwright-core folder if `playwright-core`
// isn't resolvable) with Microsoft Edge or Chromium. Outputs are committed, so builds need neither.
import { createRequire } from 'node:module';
import { mkdirSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const require = createRequire(import.meta.url);
const { chromium } = require(process.env.PLAYWRIGHT_CORE || 'playwright-core');
const OUT = join(dirname(fileURLToPath(import.meta.url)), '..', 'brand', 'filetypes');

const C = {
  video: ['#ff7a1a', '#ffb27a'], audio: ['#10b981', '#7fdcbc'], playlist: ['#4f7cff', '#a9bfff'],
  subtitles: ['#8b5cf6', '#c7b2fb'], disc: ['#64748b', '#b8c2cf'],
  page: '#ffffff', pageEdge: '#d9d9de', fold: '#ececf0', dpage: '#26272c', dpageEdge: '#4a4b54', dfold: '#34353c',
  ink: '#2a2b30', dark: '#1d1e22', dark2: '#2c2d33', darkEdge: '#4a4b54', white: '#fffaf4',
};
// name -> [kind, label]. Named extensions get their own label; the kind names are fallbacks for every other type.
const ICONS = {
  mkv: ['video', 'MKV'], mp4: ['video', 'MP4'], avi: ['video', 'AVI'], mov: ['video', 'MOV'], webm: ['video', 'WEBM'],
  wmv: ['video', 'WMV'], flv: ['video', 'FLV'], m4v: ['video', 'M4V'], mpg: ['video', 'MPG'], mpeg: ['video', 'MPEG'],
  ts: ['video', 'TS'], m2ts: ['video', 'M2TS'], '3gp': ['video', '3GP'], ogv: ['video', 'OGV'],
  mp3: ['audio', 'MP3'], flac: ['audio', 'FLAC'], m4a: ['audio', 'M4A'], wav: ['audio', 'WAV'], ogg: ['audio', 'OGG'],
  opus: ['audio', 'OPUS'], aac: ['audio', 'AAC'], wma: ['audio', 'WMA'],
  m3u: ['playlist', 'M3U'], m3u8: ['playlist', 'M3U8'], xspf: ['playlist', 'XSPF'], pls: ['playlist', 'PLS'],
  srt: ['subtitles', 'SRT'], ass: ['subtitles', 'ASS'], vtt: ['subtitles', 'VTT'], iso: ['disc', 'ISO'],
  video: ['video', 'VIDEO'], audio: ['audio', 'AUDIO'], playlist: ['playlist', 'LIST'], subtitles: ['subtitles', 'SUBS'], disc: ['disc', 'DISC'],
};
const SIZES = [16, 20, 24, 32, 40, 48, 64, 96, 128, 256];
const SMALL = 40; // at or below this, no label and a bigger glyph

function glyph(kind, cy, s, f) {
  let g;
  if (kind === 'video') g = `<rect x="-11" y="-8" width="22" height="16" rx="3" fill="${f}"/><path d="M-3 -4.5v9l7.5-4.5z" fill="${C.white}"/>`;
  else if (kind === 'audio') g = `<path d="M-2 6V-9l9-2v13" fill="none" stroke="${f}" stroke-width="2.6" stroke-linejoin="round"/><circle cx="-5.2" cy="6" r="3.8" fill="${f}"/><circle cx="3.8" cy="2.5" r="3.8" fill="${f}"/>`;
  else if (kind === 'playlist') g = `<path d="M-11 -7h14M-11 -1h14M-11 5h9" stroke="${f}" stroke-width="2.6" stroke-linecap="round"/><path d="M5 1v10l8-5z" fill="${f}"/>`;
  else if (kind === 'subtitles') g = `<rect x="-12" y="-8" width="24" height="16" rx="3" fill="none" stroke="${f}" stroke-width="2.4"/><path d="M-7 -1h6M3 -1h4M-7 3.5h9M5 3.5h2" stroke="${f}" stroke-width="2.2" stroke-linecap="round"/>`;
  else g = `<circle r="11" fill="${f}"/><circle r="3.4" fill="${C.white}"/>`;
  return `<g transform="translate(32 ${cy}) scale(${s})">${g}</g>`;
}
const PAGE = 'M15 4h24l12 12v42a3 3 0 0 1-3 3H15a3 3 0 0 1-3-3V7a3 3 0 0 1 3-3z';
const FOLD = 'M39 4v9a3 3 0 0 0 3 3h9';
const page = (fill, edge, fold, w = 1.2) => `<path d="${PAGE}" fill="${fill}" stroke="${edge}" stroke-width="${w}"/><path d="${FOLD}" fill="${fold}" stroke="${edge}" stroke-width="${w}"/>`;
const label = (t, x, y, fill, size) => `<text x="${x}" y="${y}" text-anchor="middle" font-family="Segoe UI, Inter, sans-serif" font-weight="800" font-size="${size}" fill="${fill}">${t}</text>`;
function tri(x, y, w, h, r) {
  const t = y - h / 2, tip = x + w;
  return `M${x} ${t + r}v${h - 2 * r}a${r} ${r} 0 0 0 ${r * 1.5} ${r * 0.87}L${tip - r * 0.9} ${y + r * 0.5}a${r} ${r} 0 0 0 0 ${-r}L${x + r * 1.5} ${t + r * 0.13}A${r} ${r} 0 0 0 ${x} ${t + r}z`;
}
const fontFor = t => (t.length >= 5 ? 6.8 : t.length === 4 ? 8 : 9);

function svg(style, look, kind, text, small) {
  const [c1, c2] = C[kind];
  const dark = look === 'dark';
  let body;
  if (style === 'band') {
    body = (dark ? page(C.dpage, C.dpageEdge, C.dfold) : page(C.page, C.pageEdge, C.fold)) +
      glyph(kind, small ? 36 : 30, small ? 1.25 : 1, c1) +
      (small ? '' : `<rect x="12" y="44" width="39" height="13" fill="${c1}"/>` + label(text, 31.5, 53.6, C.white, fontFor(text)));
  } else {
    const y = small ? 34 : 28;
    body = (dark ? page(C.dark, C.darkEdge, C.dark2, 1.4) : page(C.page, C.pageEdge, C.fold)) +
      `<path d="${tri(small ? 16 : 18, y, small ? 20 : 18, small ? 25 : 22, 2.6)}" fill="${c1}" opacity="${dark ? 0.45 : 0.3}"/>` +
      `<path d="${tri(small ? 22 : 23, y, small ? 20 : 18, small ? 25 : 22, 2.6)}" fill="${c1}" opacity="${dark ? 0.75 : 0.6}"/>` +
      `<path d="${tri(small ? 28 : 28, y, small ? 20 : 18, small ? 25 : 22, 2.6)}" fill="${dark ? c2 : c1}"/>` +
      (small ? '' : label(text, 31.5, 53, dark ? C.white : C.ink, fontFor(text)));
  }
  return body;
}

// ICO with PNG entries (Vista+), one per size.
function ico(pngs) {
  const header = Buffer.alloc(6 + 16 * pngs.length);
  header.writeUInt16LE(0, 0); header.writeUInt16LE(1, 2); header.writeUInt16LE(pngs.length, 4);
  let offset = header.length;
  pngs.forEach(([size, buf], i) => {
    const o = 6 + 16 * i;
    header.writeUInt8(size >= 256 ? 0 : size, o); header.writeUInt8(size >= 256 ? 0 : size, o + 1);
    header.writeUInt8(0, o + 2); header.writeUInt8(0, o + 3);
    header.writeUInt16LE(1, o + 4); header.writeUInt16LE(32, o + 6);
    header.writeUInt32LE(buf.length, o + 8); header.writeUInt32LE(offset, o + 12);
    offset += buf.length;
  });
  return Buffer.concat([header, ...pngs.map(p => p[1])]);
}

const browser = await chromium.launch({ channel: process.env.PLAYWRIGHT_CHANNEL || 'msedge' });
const page_ = await browser.newPage({ viewport: { width: 300, height: 300 } });
const manifest = {};
let count = 0;
for (const style of ['band', 'echo']) {
  for (const look of ['light', 'dark']) {
    const dir = join(OUT, `${style}-${look}`);
    mkdirSync(dir, { recursive: true });
    for (const [name, [kind, text]] of Object.entries(ICONS)) {
      const pngs = [];
      for (const size of SIZES) {
        const inner = svg(style, look, kind, text, size <= SMALL);
        await page_.setContent(`<html><body style="margin:0;background:transparent"><svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="${size}" height="${size}" shape-rendering="geometricPrecision">${inner}</svg></body></html>`);
        pngs.push([size, await (await page_.$('svg')).screenshot({ omitBackground: true })]);
      }
      writeFileSync(join(dir, `${name}.ico`), ico(pngs));
      manifest[name] = kind;
      count++;
    }
  }
}
writeFileSync(join(OUT, 'manifest.json'), JSON.stringify({ sizes: SIZES, icons: manifest }, null, 2) + '\n');
await browser.close();
console.log(`wrote ${count} icons to ${OUT}`);
