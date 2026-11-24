# dotMate (2026 rewrite)

A modern C++20 rewrite of my 2019 college project, a conversational
menstrual-health FAQ that uses fuzzy string matching instead of a rigid
menu, plus a period-tracking calendar. It runs as a terminal app and,
compiled to WebAssembly, as a live demo in the browser.

## Terminal app

Requires CMake 3.16+ and a C++20 compiler (tested with GCC 13 on Linux
and GCC 16 / MSYS2 MinGW on Windows).

```sh
mkdir build && cd build
cmake ..
make -j4
./dotmate          # the app
./dotmate_tests    # the unit tests
```

The knowledge base lives in `data/menstrual_qa.json` and is copied next
to the built binary. Ask a question, type `calendar` to estimate your
next period, or `exit` to leave.

## Browser demo

`web/` is a complete static site: a page, a terminal emulator
(xterm.js, vendored in `web/vendor/`), and the compiled program in
`web/wasm/`. Serve it over HTTP (modules and WebAssembly don't load from
`file://`):

```sh
python3 -m http.server -d web 8000     # then open http://localhost:8000
```

When the page is opened inside an iframe it shows only the terminal (`js/embed.js` adds an `embedded` class that the page's CSS keys off), so the same folder works as a standalone page and as an embed.

### Rebuilding the WebAssembly module

Only needed after changing C++ or `data/menstrual_qa.json`. Install
[Emscripten](https://emscripten.org/docs/getting_started/downloads.html)
(emsdk), activate it (`source emsdk_env.sh`, or run it from an emsdk
shell / WSL on Windows), then:

```sh
scripts/build-wasm.sh
```

That builds the module, runs the unit tests against the WebAssembly
build under Node, and copies `dotmate.mjs` + `dotmate.wasm` into
`web/wasm/`.

## Tests

```sh
cd build && ctest --output-on-failure             # native unit tests
scripts/build-wasm.sh                             # same tests, compiled to WebAssembly, run under Node
node --test tests/web/*.test.mjs                  # browser-side logic + the real WASM, via a fake terminal
```

The last one needs `web/wasm/` to exist (it ships prebuilt). Node 20+.

## Layout

```
include/dotmate/   Public headers (the API of each class)
src/               Implementation; main.cpp is the terminal driver,
                   wasm_bindings.cpp the browser one
tests/             C++ unit tests + a tiny zero-dependency harness
tests/web/         Node tests for the browser-side JavaScript
data/              menstrual_qa.json -- the Q&A content, with sources
web/               The deployable site (index.html, js/, vendor/, wasm/, _headers)
scripts/           build-wasm.sh
third_party/       Vendored nlohmann/json (single header, MIT licence)
```

`Matcher`, `CycleTracker` and `Session` do no I/O of their own and are
unit tested directly; everything they need from the outside world
(text output, saving a file, remembered names) comes in through small
interfaces that the terminal app, the tests and the browser build each
implement.

## Privacy

By default nothing is written anywhere about who used the app or what
they asked. If you opt in to being remembered, only your display name
and a non-reversible hash of it are stored -- in `returning_users.json`
for the terminal app, in this browser's `localStorage` for the demo --
never a transcript of any question or answer. The browser demo makes no
network requests after the page loads.
