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
         */
        Renderable(bool isVisible, bool isStatic);

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
         * If this is set to true, then renderer will assume the entity has not been transformed and has not had their mesh modified since the last frame.
         *
         * This is useful for entities that do not move, and who's geometry does not change, such as the background or floor.
         *
         * The entity should not be moved, rotated or scaled. The world transform should not be changed at all.
         *
         * The mesh should not be changed either. The texture/material can be changed safely.
         *
         * Once an entity has been created as static, it should not be changed to dynamic, and vice versa.
         *
         * This should be based on the world transform of the entity, not the local transform. i.e. If the parent of the entity is static but the entity can move, then the entity should **not** be marked as static.
         * Or if the entity is does not move but the parent does, then the entity should **not** be marked as static.
         */
        bool isStatic = false;
    };
}