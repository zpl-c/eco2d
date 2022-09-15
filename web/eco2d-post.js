// Hack to enforce CLOCK_REALTIME, which is significantly faster for our purposes.
_emscripten_get_now = () => Date.now();
