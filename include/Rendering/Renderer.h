#pragma once

#include "Color.h"
#include "./Mesh/Mesh.h"

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace Rendering
{

    class Renderer
    {
    public:
        Renderer(SDL_Renderer *sdlRenderer, int windowWidth, int windowHeight);
        ~Renderer();

        void clear();
        void present();

        void mesh(const Mesh2D &mesh, const Color &color);

    private:
        int windowWidth;
        int windowHeight;

        SDL_Renderer *sdlRenderer;
    };
}