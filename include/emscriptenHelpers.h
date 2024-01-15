#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <functional>

namespace blz
{
    void emscriptenSetMainLoop(std::function<void()> &cb, int fps, bool simulateInfiniteLoop);
}

#endif
