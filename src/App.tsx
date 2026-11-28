import { useEffect, useRef, useState } from "react";
import { RELEASE, SITE, GOALS, PREVIEWS, ROADMAP, FUTURE, BUILD_CMD, installerMB, releaseDate, mb, type Asset } from "./content";
import { Mark, GitHub, ArrowRight, Download, Copy, Check } from "./components/Icons";
import { useDownload, useSystem, osName } from "./download";

function Nav() {
  const [scrolled, setScrolled] = useState(false);
  useEffect(() => {
    const on = () => setScrolled(window.scrollY > 8);
    on();
    window.addEventListener("scroll", on, { passive: true });
    return () => window.removeEventListener("scroll", on);
  }, []);
  return (
    <header className="nav" data-scrolled={scrolled}>
      <div className="nav-inner">
        <a className="brand" href="#top" aria-label="VLZ, back to top">
          <Mark /> <span>VLZ</span>
        </a>
        <nav aria-label="Sections" className="nav-links">
          <a href="#goals">Goals</a>
          <a href="#previews">Design</a>
          <a href="#roadmap">Roadmap</a>
          <a href="#credits">Credits</a>
        </nav>
        <a className="icon-link" href={SITE.repo} aria-label="VLZ on GitHub">
          <GitHub />
        </a>
        <a className="btn btn-solid btn-sm" href="#download">
          Download
        </a>
      </div>
    </header>
  );
}

/** The README's disclaimer, word for word. It stays visible on the page. */
function Disclaimer({ className = "" }: { className?: string }) {
  return (
    <p className={`disclaimer ${className}`}>
      VLZ is <b>not</b> affiliated with or endorsed by VideoLAN. “VLC” and the cone logo are trademarks of VideoLAN.
      VLZ uses its own name and icon for that reason.
    </p>
  );
}

function Hero() {
  const dl = useDownload();
  const other = dl.os === "mac" || dl.os === "linux";
  return (
    <section className="hero wrap" id="top" aria-labelledby="hero-title">
      <div className="hero-copy">
        <a className="release-pill" href={RELEASE.page}>
          <span className="dot" aria-hidden="true" />v{RELEASE.version} · Windows x64 · GPLv2 or later
        </a>
        <h1 id="hero-title" className="hero-title">
          <span className="hero-name">
            <Mark size={64} />
            VLZ
          </span>
          <span className="hero-line">A fork of VLC media player with a more modern, fluid interface.</span>
        </h1>
        <p className="hero-sub">
          A personal, open source fork of <a href={SITE.vlc}>VLC media player</a> with a more modern, fluid UI and tuned
          defaults. Everything that makes VLC great (the engine, the codecs, “plays everything”) is VLC's work. VLZ only
          reskins and tunes it.
        </p>
        <div className="cta-row">
          <a className="btn btn-solid" href={dl.href}>
            <Download />
            {dl.label}
            {dl.size && <span className="btn-meta">{dl.size} MB</span>}
          </a>
          <a className="btn btn-ghost" href={SITE.repo}>
            <GitHub /> View on GitHub
          </a>
        </div>
        {other ? (
          <p className="cta-note" data-os={dl.os}>
            You're on {osName(dl.os!)}. VLZ is made for Windows: there are no macOS or Linux builds.
          </p>
        ) : (
          dl.href !== "#download" && (
            <p className="cta-note">
              Windows x64 setup. <a href="#download">Portable and checksums</a>
            </p>
          )
        )}
      </div>

      <div className="spec-card">
        <dl className="specs">
          <div className="spec">
            <dt>Latest release</dt>
            <dd>
              <span className="mono">{RELEASE.version}</span>
            </dd>
          </div>
          <div className="spec">
            <dt>Released</dt>
            <dd>{releaseDate}</dd>
          </div>
          <div className="spec">
            <dt>Platform</dt>
            <dd>Windows x64 only</dd>
          </div>
          <div className="spec">
            <dt>Setup</dt>
            <dd>
              <span className="mono">{installerMB}</span> MB
            </dd>
          </div>
          {RELEASE.assets.windowsPortable && (
            <div className="spec">
              <dt>Portable (.7z)</dt>
              <dd>
                <span className="mono">{mb(RELEASE.assets.windowsPortable.size)}</span> MB
              </dd>
            </div>
          )}
          <div className="spec">
            <dt>Licence</dt>
            <dd>GPLv2+ · libVLC LGPLv2.1+</dd>
          </div>
        </dl>
        <Disclaimer className="source" />
      </div>
    </section>
  );
}

function Goals() {
  return (
    <section className="wrap principles" id="goals" aria-label="Goals">
      {GOALS.map((g) => (
        <div className="rise" key={g.title}>
          <h2>{g.title}</h2>
          <p>{g.body}</p>
        </div>
      ))}
    </section>
  );
}

function Shot({ name, alt, sizes = "(min-width: 1240px) 590px, (min-width: 761px) 48vw, 100vw" }: { name: string; alt: string; sizes?: string }) {
  return (
    <img
      src={`/screenshots/${name}.webp`}
      srcSet={`/screenshots/${name}-930.webp 930w, /screenshots/${name}.webp 1860w`}
      sizes={sizes}
      width={1860}
      height={1164}
      alt={alt}
      loading="lazy"
      decoding="async"
    />
  );
}

function Previews() {
  return (
    <section className="wrap previews" id="previews" aria-labelledby="prev-title">
      <div className="section-head rise">
        <h2 id="prev-title">Where the interface is going.</h2>
        <p>
          Early. The design is locked in <a href={SITE.design}>DESIGN.md</a>: the Lean direction, the Ripple echo icon and
          a fully customizable player. These are design previews (HTML mock-ups), not screenshots of the running app.
        </p>
      </div>
      <div className="prev-grid">
        {PREVIEWS.map((p) => (
          <figure className="prev rise" key={p.shot}>
            <div className="prev-shot">
              <Shot name={p.shot} alt={p.alt} />
            </div>
            <figcaption>
              {p.title} <span className="tag">Design preview</span>
            </figcaption>
          </figure>
        ))}
      </div>
      <figure className="prev prev-wide rise">
        <div className="prev-shot">
          <Shot name="library-paper-emerald" sizes="(min-width: 1240px) 1200px, 100vw" alt="Design preview of the library in the Paper theme with an emerald accent" />
        </div>
        <figcaption>
          Themes and accents are yours to choose. The library in the Paper theme with the Emerald accent.{" "}
          <span className="tag">Design preview</span>
        </figcaption>
      </figure>
    </section>
  );
}

function Roadmap() {
  return (
    <section className="wrap roadmap" id="roadmap" aria-labelledby="road-title">
      <div className="section-head rise">
        <h2 id="road-title">Roadmap.</h2>
        <p>
          Found by auditing VLC 4.0-dev at fork time, ordered by leverage against diff size. All of it is planned.{" "}
          <a href={SITE.roadmap}>Full roadmap</a>
        </p>
      </div>
      <div className="road-grid">
        {ROADMAP.map((r) => (
          <div className="road-col rise" key={r.title}>
            <h3>{r.title}</h3>
            <ul>
              {r.items.map((i) => (
                <li key={i}>{i}</li>
              ))}
            </ul>
          </div>
        ))}
      </div>
      <p className="road-later rise">
        <b>Later.</b> Features are not planned yet. Ideas so far: {FUTURE.charAt(0).toLowerCase() + FUTURE.slice(1)}
      </p>
    </section>
  );
}

function CopyLine({ text }: { text: string }) {
  const [done, setDone] = useState(false);
  const t = useRef<number>(undefined);
  const copy = async () => {
    try {
      await navigator.clipboard.writeText(text);
      setDone(true);
      window.clearTimeout(t.current);
      t.current = window.setTimeout(() => setDone(false), 1600);
    } catch {
      /* clipboard refused; the text stays selectable */
    }
  };
  return (
    <div className="copy-line">
      <code>{text}</code>
      <button type="button" className="copy-btn" onClick={copy} aria-label={done ? "Copied" : "Copy command"} data-done={done}>
        <span className="copy-icons" aria-hidden="true">
          <Copy />
          <Check />
        </span>
      </button>
    </div>
  );
}

function DlLink({ a, label }: { a?: Asset; label: string }) {
  if (!a) return null;
  return (
    <a className="dl" href={a.url}>
      <span className="dl-label">{label}</span>
      <span className="dl-meta mono">{mb(a.size)} MB</span>
      <ArrowRight />
    </a>
  );
}

function Downloads() {
  const a = RELEASE.assets;
  const os = useSystem();
  const other = os === "mac" || os === "linux";
  return (
    <section className="wrap downloads" id="download" aria-labelledby="dl-title">
      <div className="section-head rise">
        <h2 id="dl-title">Download VLZ.</h2>
        <p>
          Version {RELEASE.version}, released {releaseDate}. Windows x64 builds come from GitHub Actions.{" "}
          <a href={RELEASE.page}>Release notes</a>
        </p>
      </div>
      <div className="dl-grid">
        <div className="dl-col rise" {...(os === "windows" ? { "data-yours": "" } : {})}>
          <h3>
            Windows x64
            {os === "windows" && <span className="yours-tag">Your system</span>}
          </h3>
          <DlLink a={a.windows} label="Setup (.exe)" />
          <DlLink a={a.windowsPortable} label="Portable (.7z)" />
          <p className="dl-note">Not code-signed, so Windows SmartScreen may warn on first run.</p>
        </div>
        <div className="dl-col dl-none rise" {...(other ? { "data-yours": "" } : {})}>
          <h3>
            macOS and Linux
            {other && <span className="yours-tag">Your system</span>}
          </h3>
          <p>There are no Linux or macOS builds. VLZ is made for Windows.</p>
          <p className="dl-note">
            On those systems, <a href={SITE.vlc}>VLC media player</a> itself is the one to use.
          </p>
        </div>
        <div className="dl-col rise">
          <h3>Build it yourself</h3>
          <p>
            VLZ builds the same way VLC does: a mingw-w64 toolchain inside WSL, msys2 or Docker. Start with{" "}
            <a href={SITE.build}>BUILD-win32.md</a>.
          </p>
          <CopyLine text={BUILD_CMD} />
        </div>
      </div>
      {a.checksums && (
        <p className="checksums">
          Check what you downloaded against the SHA-256 of each file in <a href={a.checksums.url}>{a.checksums.name}</a>.
        </p>
      )}
    </section>
  );
}

function Credits() {
  return (
    <section className="wrap credits" id="credits" aria-labelledby="cred-title">
      <div className="section-head rise">
        <h2 id="cred-title">Credits and licence.</h2>
      </div>
      <div className="cred-grid">
        <div className="rise">
          <p>
            All credit for the media engine, the codecs and the original interface goes to the{" "}
            <a href={SITE.videolan}>VideoLAN</a> team and VLC's contributors (see <a href={SITE.authors}>AUTHORS</a> and{" "}
            <a href={SITE.thanks}>THANKS</a>). The original VLC README is kept at{" "}
            <a href={SITE.vlcReadme}>README.VLC.md</a>.
          </p>
          <Disclaimer />
        </div>
        <div className="rise">
          <p>
            VLZ keeps VLC's licensing: the application is <b>GPLv2 or later</b> (<a href={SITE.copying}>COPYING</a>), and
            libVLC is <b>LGPLv2.1 or later</b> (<a href={SITE.copyingLib}>COPYING.LIB</a>). Every VLZ change is released
            under the same terms.
          </p>
          <p>
            VLZ follows pinned VLC 4.0-dev commits until VLC 4.0 ships, then release tags. The source is on{" "}
            <a href={SITE.repo}>GitHub</a>.
          </p>
        </div>
      </div>
    </section>
  );
}

function Footer() {
  return (
    <footer className="footer">
      <div className="wrap footer-inner">
        <p className="footer-statement">
          <Mark size={32} /> VLZ
        </p>
        <p className="footer-by">
          By <a href="https://github.com/TriptoAfsin">TriptoAfsin</a>. Built on <a href={SITE.vlc}>VLC media player</a> by{" "}
          <a href={SITE.videolan}>VideoLAN</a>. Not affiliated with or endorsed by VideoLAN; “VLC” and the cone logo are
          trademarks of VideoLAN.
        </p>
        <nav className="footer-links" aria-label="Footer">
          <a href={SITE.repo}>GitHub</a>
          <a href={SITE.releases}>Releases</a>
          <a href={SITE.roadmap}>Roadmap</a>
          <a href={SITE.copying}>GPLv2 licence</a>
          <a href={SITE.copyingLib}>LGPLv2.1 licence</a>
          <a href="https://t21.dev">t21.dev</a>
        </nav>
      </div>
    </footer>
  );
}

export default function App() {
  return (
    <>
      <a className="skip" href="#main">
        Skip to content
      </a>
      <Nav />
      <main id="main">
        <Hero />
        <Goals />
        <Previews />
        <Roadmap />
        <Downloads />
        <Credits />
      </main>
      <Footer />
    </>
  );
}
