#pragma once

#include "Color.h"
#include "Shapes/Rect.h"
#include "Shapes/Circle.h"

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace Rendering
{

    class Renderer
    {
    public:
        Renderer(std::string windowTitle, int windowWidth, int windowHeight);
        ~Renderer();

        int init();
        void destroy();

        bool pollEvents();

        void clear();
        void present();

        void line(glm::vec2 start, glm::vec2 end, const Color &color);
        void circle(const Circle &circle, const Color &color);
        void rect(const Rect &rect, const Color &color);
        void polygon(const std::vector<glm::vec2> &vertices, const Color &color);

    private:
        SDL_Renderer *renderer;
        SDL_Window *window;

        std::string windowTitle;
        int windowWidth;
        int windowHeight;
    };
}