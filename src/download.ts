// The download that fits the visitor's computer. Worked out in the browser after the page loads,
// so the prerendered page (and anyone without JavaScript) keeps the link to #download.
// VLZ ships Windows x64 only: macOS and Linux visitors are told so instead of being handed a file.
import { useEffect, useState } from "react";
import { RELEASE, mb } from "./content";

export type OS = "windows" | "mac" | "linux";

function detect(): OS | null {
  const ua = navigator.userAgent;
  if (/Android|iPhone|iPad|iPod|Mobile/i.test(ua)) return null;
  const os: OS | null = /Windows/i.test(ua) ? "windows" : /Mac OS X|Macintosh/i.test(ua) ? "mac" : /Linux|X11|CrOS/i.test(ua) ? "linux" : null;
  // An iPad asking for the desktop site says "Macintosh" but has a touch screen.
  if (os === "mac" && navigator.maxTouchPoints > 1) return null;
  return os;
}

export function useSystem(): OS | null {
  const [os, setOs] = useState<OS | null>(null);
  useEffect(() => setOs(detect()), []);
  return os;
}

const NAMES: Record<OS, string> = { windows: "Windows", mac: "macOS", linux: "Linux" };
export const osName = (os: OS) => NAMES[os];

/** The hero's main button: straight to the setup on Windows, else to #download. */
export function useDownload() {
  const os = useSystem();
  const file = RELEASE.assets.windows;
  if (os === "windows" && file) return { href: file.url, label: "Download for Windows", size: mb(file.size), os };
  return { href: "#download", label: os ? "Download for Windows" : "Download", size: null, os };
}
