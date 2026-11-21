// Renders the installer's side panel (welcome and finish pages) from the VLX icon:
// extras/package/win32/NSIS/vlc_branding.bmp, 164 x 314, 24-bit, as NSIS expects.
//
//   node extras/vlx/scripts/installer-art.mjs
//
// Needs Node and Playwright (set PLAYWRIGHT_CORE to its playwright-core folder if `playwright-core` isn't
// resolvable from here). Also writes a PNG preview next to the BMP's source in extras/vlx/brand/.
import { createRequire } from 'node:module';
import { writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const require = createRequire(import.meta.url);
const { chromium } = require(process.env.PLAYWRIGHT_CORE || 'playwright-core');
const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..', '..');
const W = 164, H = 314;

// The icon's three "echo" triangles, scaled up and bleeding off the bottom left, on the icon's orange.
const tri = (x, fill) =>
  `<path d="M${x} 80v96a12 12 0 0 0 18 10.44L${x + 89.2} 134a12 12 0 0 0 0 -12L${x + 18} 69.56A12 12 0 0 0 ${x} 80z" fill="${fill}"/>`;
const svg = `<svg xmlns="http://www.w3.org/2000/svg" width="${W}" height="${H}" viewBox="0 0 ${W} ${H}">
  <defs><linearGradient id="bg" x1="0" y1="0" x2="0" y2="1">
    <stop offset="0" stop-color="#ff8a33"/><stop offset="1" stop-color="#f26a0a"/></linearGradient></defs>
  <rect width="${W}" height="${H}" fill="url(#bg)"/>
  <g transform="translate(-58 78) scale(1.2)">${tri(52, '#ff9a4d')}${tri(80, '#ffc28e')}${tri(108, '#fff6ec')}</g>
  <g transform="translate(22 26) scale(0.21)">
    <path d="M128 8C40 8 8 40 8 128s32 120 120 120 120-32 120-120S216 8 128 8z" fill="#fff6ec"/>
    ${tri(52, '#ffc28e')}${tri(80, '#ff9a4d')}${tri(108, '#ff7a1a')}
  </g>
  <text x="84" y="61" font-family="Segoe UI, Arial, sans-serif" font-weight="700" font-size="26" fill="#fff6ec">VLX</text>
</svg>`;

// Playwright's own Chromium if downloaded, else the Edge or Chrome already on the PC
const browser = await chromium.launch().catch(() => chromium.launch({ channel: 'msedge' }))
  .catch(() => chromium.launch({ channel: 'chrome' }));
const page = await browser.newPage({ viewport: { width: W, height: H } });
const pixels = await page.evaluate(async ({ svg, W, H }) => {
  const img = new Image();
  img.src = 'data:image/svg+xml;base64,' + btoa(svg);
  await img.decode();
  const c = document.createElement('canvas');
  c.width = W; c.height = H;
  const g = c.getContext('2d');
  g.drawImage(img, 0, 0);
  return { rgba: Array.from(g.getImageData(0, 0, W, H).data), png: c.toDataURL('image/png') };
}, { svg, W, H });
await browser.close();

// 24-bit bottom-up BMP, rows padded to 4 bytes
const row = Math.ceil((W * 3) / 4) * 4;
const size = 54 + row * H;
const bmp = Buffer.alloc(size);
bmp.write('BM', 0);
bmp.writeUInt32LE(size, 2);
bmp.writeUInt32LE(54, 10);
bmp.writeUInt32LE(40, 14);
bmp.writeInt32LE(W, 18);
bmp.writeInt32LE(H, 22);
bmp.writeUInt16LE(1, 26);
bmp.writeUInt16LE(24, 28);
bmp.writeUInt32LE(row * H, 34);
bmp.writeInt32LE(2835, 38);
bmp.writeInt32LE(2835, 42);
for (let y = 0; y < H; y++) {
  const off = 54 + (H - 1 - y) * row;
  for (let x = 0; x < W; x++) {
    const i = (y * W + x) * 4;
    bmp[off + x * 3] = pixels.rgba[i + 2];
    bmp[off + x * 3 + 1] = pixels.rgba[i + 1];
    bmp[off + x * 3 + 2] = pixels.rgba[i];
  }
}
writeFileSync(join(ROOT, 'extras/package/win32/NSIS/vlc_branding.bmp'), bmp);
writeFileSync(join(ROOT, 'extras/vlx/brand/installer-side.png'), Buffer.from(pixels.png.split(',')[1], 'base64'));
console.log('wrote vlc_branding.bmp and brand/installer-side.png');
