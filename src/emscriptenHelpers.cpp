#include "../include/emscriptenHelpers.h"
#include <functional>

#ifdef __EMSCRIPTEN__
std::function<void()> mainLoop;

void mainLoopWrapper()
{
    mainLoop();
}

void blz::emscriptenSetMainLoop(std::function<void(void)> &cb, int fps, bool simulateInfiniteLoop)
{
    mainLoop = cb;
    emscripten_set_main_loop(mainLoopWrapper, fps, static_cast<int>(simulateInfiniteLoop));
}
#endif