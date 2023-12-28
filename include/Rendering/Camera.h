#pragma once

#include "../Config.h"
#include "../Core/AABB/AABB.h"

#include <glm/glm.hpp>
#include <utility>

namespace Rendering
{
    /**
     * Represents an orthographic camera.
     *
     * The camera points towards the postive z axis.
     *
     * NOTE:
     * Reducing the camera's far plane below `Config::MAX_Z_INDEX + 1` will cause the camera to clip z indexes starting at 0 and going up to `Config::MAX_Z_INDEX`.
     * Whereas increasing the camera's near plane above 0 will cause the camera to clip z indexes starting at `Config::MAX_Z_INDEX` and going down to 0.
     *
     * You can think of the clipping as happening in the reverse direction to the z index. Lower z indexes are clipped by the far plane and higher z indexes are clipped by the near plane.
     */
    class Camera
    {
    public:
        /**
         * Creates a camera instance.
         *
         * @param width The width of the camera viewport.
         * @param height The height of the camera viewport.
         * @param near The z cut off for the near plane, by default this is set to `0.0f`.
         * @param far The z cut off for the far plane, by default this is set to `Config::MAX_Z_INDEX + 1.0f`.
         */
        Camera(float width, float height, float near = 0.0f, float far = Config::MAX_Z_INDEX + 1.0f);

        /**
         * Sets the width of the camera viewport.
         *
         * @param width The width of the camera viewport.
         */
        void setWidth(float width);

        /**
         * Gets the width of the camera viewport.
         *
         * @returns the width of the camera viewport.
         */
        float getWidth() const;

        /**
         * Sets the height of the camera viewport.
         *
         * @param height The height of the camera viewport.
         */
        void setHeight(float height);

        /**
         * Gets the height of the camera viewport.
         *
         * @returns the height of the camera viewport.
         */
        float getHeight() const;

        /**
         * Sets the size of the camera viewport.
         *
         * @param width The width of the camera viewport.
         * @param height The height of the camera viewport.
         */
        void setViewportSize(float width, float height);

        /**
         * Gets the size of the camera viewport.
         *
         * @returns the size of the camera viewport.
         */
        std::pair<float, float> getViewportSize() const;

        /**
         * Sets the z cut off for the near plane.
         *
         * @param near The z cut off for the near plane.
         */
        void setNear(float near);

        /**
         * Gets the z cut off for the near plane.
         *
         * @returns the z cut off for the near plane.
         */
        float getNear() const;

        /**
         * Sets the z cut off for the far plane.
         *
         * @param far The z cut off for the far plane.
         */
        void setFar(float far);

        /**
         * Gets the z cut off for the far plane.
         *
         * @returns the z cut off for the far plane.
         */
        float getFar() const;

        /**
         * Gets the view projection matrix.
         *
         * @returns the view projection matrix.
         */
        glm::mat4 getViewProjectionMatrix() const;

        /**
         * Gets the AABB of the camera.
         *
         * This is the AABB of the camera's viewport.
         *
         * @returns the AABB of the camera.
         */
        const Core::AABB &getAABB() const;

    private:
        float width;
        float height;
        float near;
        float far;

        mutable bool isViewProjectionMatrixDirty = true;
        mutable glm::mat4 viewProjectionMatrix;

        Core::AABB aabb;

        /**
         * Updates the camera's AABB.
         */
        void updateAABB();
    };
}