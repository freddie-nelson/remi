#pragma once

namespace Rendering
{
    /**
     * Represents a renderable entity.
     *
     * This is data used by the renderer to determine how to render an entity.
     */
    struct Renderable
    {
        /**
         * Whether or not the entity is visible.
         *
         * If this is set to false, the entity will not be rendered by Renderer::update.
         */
        bool isVisible = true;

        /**
         * Whether or not the entity is static.
         *
         * If this is set to true, then Renderer::update will assume the entity has not been transformed since the last frame.
         *
         * This is useful for entities that do not move, such as the background.
         *
         * The entity can still rotate and be culled safely, however it should not be translated, scaled or sheared.
         */
        bool isStatic = false;
    };
}