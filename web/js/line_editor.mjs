// Line editing for a terminal with no shell behind it: typing, backspace,
// history, paste, Ctrl+C and Ctrl+U. It is given a `write` function and an
// `onSubmit` callback and knows nothing else, so it behaves the same in
// the browser and in the Node tests.
//
// Deliberately small: no cursor movement inside the line. The matcher
// shrugs off typos, so retyping a question is cheaper than editing one.
export class LineEditor {
  #write;
  #onSubmit;
  #prompt;
  #maxLength;
  #buffer = '';
  #history = [];
  #browsing = -1; // index into #history while using Up/Down, -1 otherwise
  #draft = '';

  constructor({ write, onSubmit, prompt = '> ', maxLength = 300 }) {
    this.#write = write;
    this.#onSubmit = onSubmit;
    this.#prompt = prompt;
    this.#maxLength = maxLength;
  }

  get buffer() {
    return this.#buffer;
  }

  // Feed whatever the terminal reports: one key, an escape sequence, or a paste.
  handle(data) {
    let i = 0;
    while (i < data.length) {
      const ch = data[i];
      if (ch === '\x1b') {
        const match = /^\x1b(?:\[[0-9;?]*[ -/]*[@-~]|O[@-~])/.exec(data.slice(i));
        const sequence = match ? match[0] : ch;
        this.#escape(sequence);
        i += sequence.length;
      } else if (ch === '\r' || ch === '\n') {
        this.#submit();
        i += ch === '\r' && data[i + 1] === '\n' ? 2 : 1;
      } else {
        this.#key(ch);
        i += 1;
      }
    }
  }

  #key(ch) {
    if (ch === '\x7f' || ch === '\b') return this.#backspace();
    if (ch === '\x15') return this.#eraseLine();
    if (ch === '\x03') return this.#cancel();
    if (ch < ' ') return; // other control characters (tab, bell...) do nothing
    if (this.#buffer.length >= this.#maxLength) return;
    this.#buffer += ch;
    this.#write(ch);
  }

  #escape(sequence) {
    if (sequence === '\x1b[A' || sequence === '\x1bOA') this.#older();
    else if (sequence === '\x1b[B' || sequence === '\x1bOB') this.#newer();
    // Everything else (arrows left/right, Home, End, Delete...) is ignored.
  }

  #backspace() {
    if (this.#buffer === '') return;
    this.#buffer = Array.from(this.#buffer).slice(0, -1).join('');
    this.#write('\b \b');
  }

  #eraseLine() {
    this.#write('\b \b'.repeat(Array.from(this.#buffer).length));
    this.#buffer = '';
  }

  #cancel() {
    this.#buffer = '';
    this.#browsing = -1;
    this.#write('^C\r\n' + this.#prompt);
  }

  #submit() {
    this.#write('\r\n');
    const line = this.#buffer;
    this.#buffer = '';
    this.#browsing = -1;
    if (line.trim() !== '' && line !== this.#history.at(-1)) this.#history.push(line);
    this.#onSubmit(line);
  }

  #replaceLine(text) {
    this.#eraseLine();
    this.#buffer = text;
    this.#write(text);
  }

  #older() {
    if (this.#history.length === 0) return;
    if (this.#browsing === -1) {
      this.#draft = this.#buffer;
      this.#browsing = this.#history.length - 1;
    } else if (this.#browsing > 0) {
      this.#browsing -= 1;
    }
    this.#replaceLine(this.#history[this.#browsing]);
  }

  #newer() {
    if (this.#browsing === -1) return;
    if (this.#browsing < this.#history.length - 1) {
      this.#browsing += 1;
      this.#replaceLine(this.#history[this.#browsing]);
    } else {
      this.#browsing = -1;
      this.#replaceLine(this.#draft);
    }
  }
}
