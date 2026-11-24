// When the page is shown inside an iframe (for example in a blog post),
// mark the document so the stylesheet can show just the terminal.
if (window.self !== window.top) document.documentElement.classList.add('embedded');
