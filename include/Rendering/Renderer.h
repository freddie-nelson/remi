#pragma once

#include "./Shapes/Circle.h"
#include "./Shapes/Rect.h"
#include "./Color.h"

#include <string>
#include <vector>

namespace Rendering
{
    class Renderer
    {
    public:
        Renderer(std::string windowTitle, int windowWidth, int windowHeight);
        virtual ~Renderer() = default;

        virtual int init() = 0;
        virtual void destroy() = 0;

        /**
         * Polls events from the renderer.
         *
         * @return True if the application should exit.
         */
        virtual bool pollEvents() = 0;

        virtual void clear() = 0;
        virtual void present() = 0;

        virtual void line(glm::vec2 start, glm::vec2 end, const Color &color) = 0;
        virtual void circle(const Circle &circle, const Color &color) = 0;
        virtual void rect(const Rect &rect, const Color &color) = 0;

        /**
         * Draws a polygon with the given vertices.
         *
         * @param vertices The vertices of the polygon. (must be in clockwise order)
         * @param color The color of the polygon.
         */
        virtual void polygon(const std::vector<glm::vec2> &vertices, const Color &color) = 0;

    protected:
        std::string windowTitle;
        int windowWidth;
        int windowHeight;
    };

}