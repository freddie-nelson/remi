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
         * Creates a renderable.
         *
         * @param isVisible Whether or not the entity is visible.
         * @param isStatic Whether or not the entity is static.
         * @parma noCulling Whether or not the entity should be culled.
         */
        Renderable(bool isVisible, bool isStatic, bool noCulling = false);

        /**
         * Whether or not the entity is visible.
         *
         * If this is set to false, the entity will not be rendered by Renderer::update.
         *
         * You can still render the entity manually by calling Renderer::entity, Renderer::batch or Renderer::instance.
         */
        bool isVisible = true;

        /**
         * Whether or not the entity is static.
         *
         * If this is set to true, then Renderer::update will assume the entity has not been transformed since the last frame.
         *
         * This is useful for entities that do not move, and who's geometry does not change, such as the background or floor.
         *
         * The entity can still rotate and be culled safely, however it should not be translated, scaled or sheared.
         *
         * The mesh should not be changed either. The texture/material can be changed safely.
         *
         * It's okay for entities to be changed from static to non-static and vice versa.
         *
         * This should be based on the world transform of the entity, not the local transform. i.e. If the parent of the entity is static but the entity can move, then the entity should not be marked as static.
         */
        bool isStatic = false;

        /**
         * Whether or not the entity should be culled.
         *
         * If this is set to true, the entity will never be culled during the culling pass.
         */
        bool noCulling = false;
    };
}