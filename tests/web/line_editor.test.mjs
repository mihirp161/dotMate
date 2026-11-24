import test from 'node:test';
import assert from 'node:assert/strict';
import { LineEditor } from '../../web/js/line_editor.mjs';

function setup(options = {}) {
  const out = [];
  const lines = [];
  const editor = new LineEditor({ write: (s) => out.push(s), onSubmit: (l) => lines.push(l), ...options });
  return { editor, lines, text: () => out.join('') };
}

test('typing is echoed and Enter submits the line', () => {
  const { editor, lines, text } = setup();
  editor.handle('hi');
  assert.equal(editor.buffer, 'hi');
  editor.handle('\r');
  assert.deepEqual(lines, ['hi']);
  assert.equal(text(), 'hi\r\n');
  assert.equal(editor.buffer, '');
});

test('backspace removes the last character and does nothing on an empty line', () => {
  const { editor, text } = setup();
  editor.handle('\x7f');
  assert.equal(text(), '');
  editor.handle('ab\x7f');
  assert.equal(editor.buffer, 'a');
  assert.equal(text(), 'ab\b \b');
});

test('a pasted block submits each line once, treating \\r\\n as a single break', () => {
  const { editor, lines } = setup();
  editor.handle('2026-01-01\r\n2026-01-29\n\r\n');
  assert.deepEqual(lines, ['2026-01-01', '2026-01-29', '']);
});

test('Ctrl+U clears the line; Ctrl+C abandons it and shows the prompt again', () => {
  const { editor, lines, text } = setup();
  editor.handle('abc\x15');
  assert.equal(editor.buffer, '');
  editor.handle('xyz\x03');
  assert.equal(editor.buffer, '');
  assert.deepEqual(lines, []);
  assert.ok(text().endsWith('^C\r\n> '));
});

test('Up and Down walk through history and restore the unsent draft', () => {
  const { editor } = setup();
  editor.handle('one\rtwo\r');
  editor.handle('dra');
  editor.handle('\x1b[A');
  assert.equal(editor.buffer, 'two');
  editor.handle('\x1b[A');
  assert.equal(editor.buffer, 'one');
  editor.handle('\x1b[A');
  assert.equal(editor.buffer, 'one');
  editor.handle('\x1b[B');
  assert.equal(editor.buffer, 'two');
  editor.handle('\x1b[B');
  assert.equal(editor.buffer, 'dra');
});

test('unsupported keys and escape sequences are ignored', () => {
  const { editor } = setup();
  editor.handle('a\x1b[D\x1b[C\x1b[3~\x1b[H\t');
  assert.equal(editor.buffer, 'a');
});

test('lines are capped at maxLength', () => {
  const { editor } = setup({ maxLength: 5 });
  editor.handle('abcdefgh');
  assert.equal(editor.buffer, 'abcde');
});
