// Renders public/og.png (1200x630) from an SVG with resvg: VLZ's own icon, the name,
// the latest version and the VideoLAN disclaimer. Run `npm run og` after a release.
// No app screenshot: the README's images are design mock-ups, not the running app.
import { readFile, writeFile, mkdir } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { createRequire } from "node:module";
import { Resvg } from "@resvg/resvg-js";
import wawoff2 from "wawoff2";

const require = createRequire(import.meta.url);
const root = new URL("..", import.meta.url);
const release = JSON.parse(await readFile(new URL("src/release.json", root), "utf8"));

// resvg needs TTF; fontsource ships WOFF2. Geist stands in for Segoe UI so the render is the same on any machine.
const dir = join(tmpdir(), "vlz-og-fonts");
await mkdir(dir, { recursive: true });
const fonts = [];
for (const f of [
  "geist/files/geist-latin-500-normal",
  "geist/files/geist-latin-700-normal",
  "geist-mono/files/geist-mono-latin-400-normal",
]) {
  const src = require.resolve(`@fontsource/${f}.woff2`);
  const out = join(dir, f.split("/").pop() + ".ttf");
  await writeFile(out, await wawoff2.decompress(await readFile(src)));
  fonts.push(out);
}

// The icon's paths, from public/icon.svg (extras/vlz/brand/vlz-icon.svg on main).
const icon = (await readFile(new URL("public/icon.svg", root), "utf8")).match(/<path[^>]*\/>/g).join("\n    ");

const svg = `<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="630" viewBox="0 0 1200 630">
  <rect width="1200" height="630" fill="#141415"/>
  <rect x="0" y="0" width="1200" height="4" fill="#ff7a1a"/>
  <g transform="translate(770 135) scale(1.4)">
    ${icon}
  </g>
  <text x="70" y="250" font-family="Geist" font-weight="700" font-size="190" fill="#ececee" letter-spacing="-8">VLZ</text>
  <text font-family="Geist" font-weight="500" font-size="40" fill="#ececee" letter-spacing="-1">
    <tspan x="74" y="338">A fork of VLC media player</tspan>
    <tspan x="74" y="388">with a more modern,</tspan>
    <tspan x="74" y="438">fluid interface.</tspan>
  </text>
  <text x="74" y="530" font-family="Geist Mono" font-size="21" fill="#a1a1a7">vlz.t21.dev · v${release.version} · Windows x64</text>
  <text x="74" y="572" font-family="Geist" font-weight="500" font-size="19" fill="#8a8a91">Not affiliated with or endorsed by VideoLAN. “VLC” is a VideoLAN trademark.</text>
</svg>`;

const png = new Resvg(svg, {
  fitTo: { mode: "width", value: 1200 },
  font: { fontFiles: fonts, loadSystemFonts: false, defaultFontFamily: "Geist" },
})
  .render()
  .asPng();
await writeFile(new URL("public/og.png", root), png);
console.log(`og: public/og.png (${(png.length / 1024).toFixed(0)} KB, v${release.version})`);
