#include "../../include/Rendering/SDLRenderer.h"

#include <iostream>
#include <math.h>

Rendering::SDLRenderer::SDLRenderer(std::string windowTitle, int windowWidth, int windowHeight) : Renderer(windowTitle, windowWidth, windowHeight)
{
}

Rendering::SDLRenderer::~SDLRenderer()
{
    destroy();
}

int Rendering::SDLRenderer::init()
{
    // init sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Failed to initialize SDL." << std::endl;
        return 1;
    }

    window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    return 0;
}

void Rendering::SDLRenderer::destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Rendering::SDLRenderer::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        }
    }

    return false;
}

void Rendering::SDLRenderer::clear()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Rendering::SDLRenderer::present()
{
    SDL_RenderPresent(renderer);
}

void Rendering::SDLRenderer::line(glm::vec2 start, glm::vec2 end, const Color &color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
}

void Rendering::SDLRenderer::circle(const Circle &circle, const Color &color)
{
    const int cx = std::floor(circle.centre[0]);
    const int cy = std::floor(circle.centre[1]);
    const int radius = std::round(circle.radius);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // draw outline of circle
    // code from https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - radius * 2;

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        else
        {
            x--;
            dx += 2;
            err += dx - radius * 2;
        }
    }
};

void Rendering::SDLRenderer::rect(const Rect &rect, const Color &color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect sdlRect{
        x : static_cast<int>(rect.topLeft.x),
        y : static_cast<int>(rect.topLeft.y),
        w : static_cast<int>(rect.w),
        h : static_cast<int>(rect.h)
    };

    SDL_RenderDrawRect(renderer, &sdlRect);
};

void Rendering::SDLRenderer::polygon(const std::vector<glm::vec2> &vertices, const Color &color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int i = 0; i < vertices.size(); i++)
    {
        int j = (i + 1) % vertices.size();

        SDL_RenderDrawLine(renderer, vertices[i].x, vertices[i].y, vertices[j].x, vertices[j].y);
    }
};