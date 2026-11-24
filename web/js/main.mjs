import { Terminal } from '../vendor/xterm/xterm.mjs';
import { FitAddon } from '../vendor/xterm/addon-fit.mjs';
import { startApp } from './app_core.mjs';

const theme = getComputedStyle(document.documentElement);
const token = (name) => theme.getPropertyValue(name).trim();

const term = new Terminal({
  fontFamily: token('--mono'),
  fontSize: 15,
  lineHeight: 1.25,
  cursorBlink: !matchMedia('(prefers-reduced-motion: reduce)').matches,
  screenReaderMode: true,
  scrollback: 2000,
  theme: {
    background: token('--term-bg'),
    foreground: token('--term-fg'),
    cursor: token('--accent'),
    green: token('--ansi-green'),
    yellow: token('--ansi-yellow'),
    cyan: token('--ansi-cyan'),
    red: token('--ansi-red'),
  },
});
const fit = new FitAddon();
term.loadAddon(fit);
term.open(document.getElementById('terminal'));
fit.fit();
new ResizeObserver(() => fit.fit()).observe(document.getElementById('terminal'));
term.write('\x1b[?45h'); // let Backspace cross a wrapped line

// Remembered names live in this browser's localStorage and nowhere else.
// If storage is unavailable (some private modes) the name just isn't remembered.
const storage = {
  get(key) {
    try {
      return localStorage.getItem(key);
    } catch {
      return null;
    }
  },
  set(key, value) {
    try {
      localStorage.setItem(key, value);
    } catch {
      /* not remembered; nothing else to do */
    }
  },
};

function download(name, contents, type) {
  const url = URL.createObjectURL(new Blob([contents], { type }));
  const link = Object.assign(document.createElement('a'), { href: url, download: name });
  document.body.append(link);
  link.click();
  link.remove();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}

try {
  await startApp({
    term,
    createModule: async () => (await import('../wasm/dotmate.mjs')).default(),
    storage,
    download,
  });
  if (!matchMedia('(pointer: coarse)').matches) term.focus(); // don't pop a phone keyboard open on load
} catch (error) {
  const box = document.getElementById('load-error');
  box.textContent =
    'The demo could not start. It needs a current version of Chrome, Firefox, Safari or Edge ' +
    `with WebAssembly turned on. (${error?.message ?? error})`;
  box.hidden = false;
}
