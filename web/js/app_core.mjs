import { LineEditor } from './line_editor.mjs';

const STORAGE_KEY = 'dotmate.returning_users.v1';

// Connects the compiled C++ conversation to a terminal. Everything the
// outside world provides is passed in, so the same code runs in the page
// (main.mjs) and in the Node tests (tests/web/app.test.mjs):
//   term          write(text) and onData(callback), like an xterm.js Terminal
//   createModule  async () => the Emscripten module
//   storage       get(key) / set(key, value), for the remembered-names JSON
//   download      (name, contents, mimeType) => void, for the calendar CSV
export async function startApp({ term, createModule, storage, download }) {
  const module = await createModule();
  const init = module.cwrap('dotmate_init', 'string', ['string']);
  const send = module.cwrap('dotmate_send_line', 'string', ['string']);
  let finished = false;

  // The C++ side writes "\n"; a terminal wants "\r\n".
  const print = (text) => term.write(text.replace(/\r?\n/g, '\r\n'));

  const render = (reply) => {
    if (!reply.ok) {
      print(`\x1b[31m${reply.error}\x1b[0m\n`);
      return;
    }
    print(reply.text);
    for (const file of reply.files) download(file.name, file.contents, 'text/csv');
    if (reply.store !== null) storage.set(STORAGE_KEY, reply.store);
    finished = reply.finished;
    if (finished) print('\n\x1b[2m[Session ended. Press Enter to start again.]\x1b[0m\n');
  };

  const begin = () => render(JSON.parse(init(storage.get(STORAGE_KEY) ?? '')));

  const editor = new LineEditor({
    write: (text) => term.write(text),
    onSubmit: (line) => {
      if (finished) {
        print('\n');
        begin();
      } else {
        render(JSON.parse(send(line)));
      }
    },
  });

  term.onData((data) => editor.handle(data));
  begin();
  return { editor };
}
