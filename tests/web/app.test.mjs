// End-to-end: the real compiled WebAssembly module, driven through the
// real line editor, with a fake terminal standing in for xterm.js.
// Build the module first (scripts/build-wasm.sh).
import test from 'node:test';
import assert from 'node:assert/strict';
import { startApp } from '../../web/js/app_core.mjs';

const plain = (s) => s.replace(/\x1b\[[0-9;]*m/g, '');

async function launch(initialStorage = {}) {
  let listener = () => {};
  const term = {
    out: '',
    write(s) { this.out += s; },
    onData(cb) { listener = cb; },
    type(s) { listener(s); },
  };
  const store = new Map(Object.entries(initialStorage));
  const writes = [];
  const storage = {
    get: (k) => store.get(k) ?? null,
    set: (k, v) => { store.set(k, v); writes.push(v); },
  };
  const downloads = [];
  await startApp({
    term,
    createModule: async () => (await import('../../web/wasm/dotmate.mjs')).default(),
    storage,
    download: (name, contents, type) => downloads.push({ name, contents, type }),
  });
  return { term, store, writes, downloads };
}

test('starts with the greeting and a prompt', async () => {
  const { term } = await launch();
  assert.match(plain(term.out), /What should I call you\?/);
  assert.ok(term.out.endsWith('> '));
});

test('answers a typo-ridden question and only ever emits \\r\\n line breaks', async () => {
  const { term } = await launch();
  term.type('\r');
  term.type('Whatt is a periodd?\r');
  assert.match(plain(term.out), /A period is the monthly shedding of blood and tissue/);
  assert.match(plain(term.out), /\(source: Office on Women's Health\)/);
  assert.ok(!/(^|[^\r])\n/.test(term.out), 'found a bare \\n');
});

test('a question it cannot answer says so', async () => {
  const { term } = await launch();
  term.type('\rpurple elephants dancing on the moon\r');
  assert.match(plain(term.out), /I don't have a good answer for that one yet/);
});

test('the calendar flow estimates the next period and offers a CSV download', async () => {
  const { term, downloads } = await launch();
  term.type('\rcalendar\r2026-01-01\r2026-01-29\r2026-02-26\r\ry\r');
  assert.match(plain(term.out), /about 28 days/);
  assert.match(plain(term.out), /Estimated next period: 2026-03-26/);
  assert.match(plain(term.out), /Saved to period_calendar\.csv \(check your downloads\)/);
  assert.equal(downloads.length, 1);
  assert.equal(downloads[0].name, 'period_calendar.csv');
  assert.equal(downloads[0].type, 'text/csv');
  assert.match(downloads[0].contents, /\n26,,,X,/);
});

test('remembering a name stores only a hash and the name, then greets them back after a restart', async () => {
  const { term, store, writes } = await launch();
  term.type('Alex\ry\rWhat is a period?\rexit\r');
  assert.match(plain(term.out), /Take care, Alex!/);
  assert.match(plain(term.out), /Session ended\. Press Enter to start again\./);

  // Exactly one write, a {hash: {display_name}} object, nothing about the question.
  assert.equal(writes.length, 1);
  const saved = JSON.parse([...store.values()][0]);
  const [[hash, entry]] = Object.entries(saved);
  assert.match(hash, /^[0-9a-f]{16}$/);
  assert.deepEqual(entry, { display_name: 'Alex' });
  assert.ok(!writes[0].toLowerCase().includes('period'));

  term.out = '';
  term.type('\r');               // Enter restarts the session
  term.type('alex\r');           // same name, different case
  assert.match(plain(term.out), /Welcome back, Alex!/);
});

test('declining to be remembered writes nothing to storage', async () => {
  const { term, store } = await launch();
  term.type('Sam\rn\rexit\r');
  assert.equal(store.size, 0);
});
