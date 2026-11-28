// Lucide-style strokes, inlined so the page loads no icon library.
const base = {
  width: 16,
  height: 16,
  viewBox: "0 0 24 24",
  fill: "none",
  stroke: "currentColor",
  strokeWidth: 2,
  strokeLinecap: "round" as const,
  strokeLinejoin: "round" as const,
  "aria-hidden": true,
};

/** VLZ's own icon, "Ripple, echo" (extras/vlz/brand/vlz-icon.svg on main). Never VLC's cone. */
export const Mark = ({ size = 22 }: { size?: number }) => (
  <svg width={size} height={size} viewBox="0 0 256 256" className="mark" aria-hidden="true">
    <path d="M128 8C40 8 8 40 8 128s32 120 120 120 120-32 120-120S216 8 128 8z" fill="#ff7a1a" />
    <path d="M52 80v96a12 12 0 0 0 18 10.44L141.2 134a12 12 0 0 0 0 -12L70 69.56A12 12 0 0 0 52 80z" fill="#ff9a4d" />
    <path d="M80 80v96a12 12 0 0 0 18 10.44L169.2 134a12 12 0 0 0 0 -12L98 69.56A12 12 0 0 0 80 80z" fill="#ffc28e" />
    <path d="M108 80v96a12 12 0 0 0 18 10.44L197.2 134a12 12 0 0 0 0 -12L126 69.56A12 12 0 0 0 108 80z" fill="#fff6ec" />
  </svg>
);
export const ArrowRight = () => (
  <svg {...base} className="i-arrow">
    <path d="M5 12h14M13 6l6 6-6 6" />
  </svg>
);
export const Download = () => (
  <svg {...base}>
    <path d="M12 4v11M7 10l5 5 5-5M5 20h14" />
  </svg>
);
export const Copy = () => (
  <svg {...base} className="i-copy">
    <rect x="9" y="9" width="11" height="11" rx="2" />
    <path d="M5 15V6a2 2 0 0 1 2-2h8" />
  </svg>
);
export const Check = () => (
  <svg {...base} className="i-check">
    <path d="m5 12 5 5 9-10" />
  </svg>
);
export const GitHub = () => (
  <svg width={16} height={16} viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
    <path d="M12 .5a11.5 11.5 0 0 0-3.64 22.41c.58.1.79-.25.79-.56v-2c-3.2.7-3.88-1.37-3.88-1.37-.53-1.33-1.29-1.69-1.29-1.69-1.04-.71.08-.7.08-.7 1.16.08 1.77 1.19 1.77 1.19 1.03 1.77 2.7 1.26 3.36.96.1-.75.4-1.26.73-1.55-2.55-.29-5.24-1.28-5.24-5.68 0-1.25.45-2.28 1.19-3.08-.12-.29-.52-1.46.11-3.05 0 0 .97-.31 3.17 1.18a11 11 0 0 1 5.77 0c2.2-1.49 3.17-1.18 3.17-1.18.63 1.59.23 2.76.11 3.05.74.8 1.19 1.83 1.19 3.08 0 4.41-2.69 5.38-5.25 5.67.41.36.78 1.06.78 2.14v3.17c0 .31.21.67.8.56A11.5 11.5 0 0 0 12 .5Z" />
  </svg>
);
