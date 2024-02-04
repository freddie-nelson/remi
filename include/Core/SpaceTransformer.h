#pragma once

#include "../Rendering/Renderer.h"
#include "../ECS/Registry.h"
#include "Transform.h"

#include <glm/glm.hpp>
#include <unordered_map>

namespace Core
{
    /**
     * The space transformer.
     *
     * Can be used to convert points between coordinate systems.
     *
     * The coordinate systems available are:
     *
     * **Format: {space} (alternate name) [unit]**
     *
     * - Local (Entity) [meters]
     * - World [meters]
     * - View [pixels (but relative to camera's centre)]
     * - Clip [Normalized Device Coordinates (-1, 1)]
     * - Screen (Pixel) [pixels]
     */
    class SpaceTransformer
    {
    public:
        /**
         * Creates a space transformer instance.
         *
         * @param renderer The renderer to take the view and projection matrices from and the viewport transform.
         * @param registry The registry to read components/entities from.
         * @param pixelsPerMeter The number of pixels (screen space) per meter (world space).
         */
        SpaceTransformer(const Rendering::Renderer *renderer, const ECS::Registry *registry, unsigned int pixelsPerMeter);

        /**
         * The coordinate systems available for conversion.
         */
        enum Space
        {
            LOCAL,
            WORLD,
            VIEW,
            CLIP,
            SCREEN,
        };

        /**
         * Converts the given point between coordinate systems.
         *
         * This will not work for converting to or from local space. The overloaded `transform(const glm::vec2 &, const Core::Transform &, Spaces, Spaces)` should be used for this purpose.
         *
         * @param v The point to convert.
         * @param from The space to convert from, this is the current coordinate system the point lies in.
         * @param to The space to convert to.
         */
        glm::vec2 transform(const glm::vec2 &v, Space from, Space to) const;

        /**
         * Converts the given point between coordinate systems.
         *
         * @param v The point to convert.
         * @param localTransform The transform to convert to and from local space. Would usually be the entities transform component.
         * @param from The space to convert from, this is the current coordinate system the point lies in.
         * @param to The space to convert to.
         */
        glm::vec2 transform(const glm::vec2 &v, const Core::Transform &localTransform, Space from, Space to) const;

        /**
         * Converts the given point between coordinate systems.
         *
         * @param v The point to convert.
         * @param localTransform The transform to convert to and from local space. Would usually be the entities transform component.
         * @param from The space to convert from, this is the current coordinate system the point lies in.
         * @param to The space to convert to.
         */
        glm::vec2 transform(const glm::vec2 &v, const Core::Transform *localTransform, Space from, Space to) const;

        /**
         * Converts the given distance from pixels to meters.
         *
         * @param pixels The number of pixels to convert.
         *
         * @returns The number of meters the pixels represent.
         */
        float pixelsToMeters(float pixels) const;

        /**
         * Converts the given point from pixels to meters.
         *
         * @param pixels The point to convert.
         *
         * @returns The point in meters.
         */
        glm::vec2 pixelsToMeters(const glm::vec2 &pixels) const;

        /**
         * Converts the given distance from meters to pixels.
         *
         * @param meters The number of meters to convert.
         *
         * @returns The number of pixels the meters represent.
         */
        float metersToPixels(float meters) const;

        /**
         * Converts the given point from meters to pixels.
         *
         * @param meters The point to convert.
         *
         * @returns The point in pixels.
         */
        glm::vec2 metersToPixels(const glm::vec2 &pixels) const;

        /**
         * Gets the number of pixels per meter.
         *
         * @returns The number of pixels per meter.
         */
        float getPixelsPerMeter() const;

    private:
        const Rendering::Renderer *renderer;
        const ECS::Registry *registry;
        const unsigned int pixelsPerMeter;
        const float pixelsPerMeterFloat;

        /**
         * Converts to the next space up to get to goal space.
         *
         * @param s The current space.
         * @param goal The space we want to get to eventually.
         * @param v The point to convert.
         * @param localTransform The local transform.
         *
         * @returns The space `v` was converted to.
         */
        Space getNextSpace(Space s, Space goal, glm::vec2 &v, const Core::Transform *localTransform = nullptr) const;

        /**
         * Converts the given point from screen space to clip space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::CLIP
         */
        Space screenToClip(glm::vec2 &v) const;

        /**
         * Converts the given point from clip space to screen space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::SCREEN
         */
        Space clipToScreen(glm::vec2 &v) const;

        /**
         * Converts the given point from clip space to view space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::VIEW
         */
        Space clipToView(glm::vec2 &v) const;

        /**
         * Converts the given point from view space to clip space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::CLIP
         */
        Space viewToClip(glm::vec2 &v) const;

        /**
         * Converts the given point from view space to world space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::WORLD
         */
        Space viewToWorld(glm::vec2 &v) const;

        /**
         * Converts the given point from world space to view space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::VIEW
         */
        Space worldToView(glm::vec2 &v) const;

        /**
         * Converts the given point form world space to local space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::LOCAL
         */
        Space worldToLocal(glm::vec2 &v, const Core::Transform &localTransform) const;

        /**
         * Converts the given point from local space to world space.
         *
         * @param v The point to convert.
         *
         * @returns Spaces::WORLD
         */
        Space localToWorld(glm::vec2 &v, const Core::Transform &localTransform) const;
    };
}