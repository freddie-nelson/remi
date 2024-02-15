#pragma once

#include <glm/glm.hpp>

namespace Core
{
    /**
     * Represents a transform.
     *
     * This is the model matrix of the mesh.
     *
     * The transform is the combined rotation, scale, shear, translation and zIndex matrix. See [here](https://en.wikipedia.org/wiki/Transformation_matrix#Examples_in_2_dimensions) for an explanation.
     *
     * The default zIndex is 0. A higher zIndex means the mesh will be rendered above meshes with a lower zIndex.
     * A higher zIndex is 'closer' to the camera. 0 is the lowest zIndex and will be rendered below everything, whereas `Config::MAX_Z_INDEX` is the largest z index and will be rendered above everything.
     *
     * This can be used as an entity component.
     */
    class Transform
    {
    public:
        /**
         * Creates a transform instance.
         *
         * The default transform is the identity transform.
         *
         * @param translation The translation of the mesh.
         * @param scale The scale of the mesh.
         * @param shear The shear of the mesh.
         * @param rotation The rotation of the mesh.
         * @param zIndex The zIndex of the mesh.
         */
        Transform(glm::vec2 translation = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(1.0f), glm::vec2 shear = glm::vec2(0.0f), float rotation = 0.0f, unsigned int zIndex = 0);

        /**
         * Creates a transform instance.
         *
         * @param mat The transform matrix of the mesh.
         */
        Transform(const glm::mat4 &mat);

        /**
         * Moves the mesh forward by the given amount.
         *
         * This will increment the zIndex of the mesh by the given amount.
         *
         * @param amount The amount to move the mesh forward by.
         */
        void moveForward(unsigned int amount = 1);

        /**
         * Moves the mesh backward by the given amount.
         *
         * This will decrement the zIndex of the mesh by the given amount.
         *
         * @param amount The amount to move the mesh backward by.
         */
        void moveBackward(unsigned int amount = 1);

        /**
         * Sets the zIndex of the mesh.
         *
         * @param zIndex The zIndex of the mesh.
         */
        void setZIndex(unsigned int zIndex);

        /**
         * Gets the zIndex of the mesh.
         *
         * @returns the zIndex of the mesh.
         */
        unsigned int getZIndex() const;

        /**
         * Moves the mesh by the given amount.
         *
         * Adds the move vector to the translation of the mesh.
         *
         * @param move The amount to move the mesh by.
         */
        void move(glm::vec2 move);

        /**
         * Translates the mesh by the given amount.
         *
         * Adds the translation vector to the translation of the mesh.
         *
         * @param translation The amount to translate the mesh by.
         */
        void translate(glm::vec2 translation);

        /**
         * Sets the translation of the mesh.
         *
         * @param translation The translation of the mesh.
         */
        void setTranslation(glm::vec2 translation);

        /**
         * Gets the translation of the mesh.
         *
         * @returns the translation of the mesh.
         */
        const glm::vec2 &getTranslation() const;

        /**
         * Scales the mesh by the given amount.
         *
         * The scale of the mesh is multiplied by the scale vector.
         *
         * The x and y components of the scale vector represent the scale in the x and y directions respectively.
         *
         * e.g. scale(glm::vec2(2.0f, 1.0f)) will double the scale of the mesh in the x direction.
         *
         * @param scale The amount to scale the mesh by.
         */
        void scale(glm::vec2 scale);

        /**
         * Scales the mesh by the given amount.
         *
         * The scale of the mesh is multiplied by the scale scalar.
         *
         * e.g. scale(2.0f) will double the scale of the mesh.
         *
         * @param scale The amount to scale the mesh by.
         */
        void scale(float scale);

        /**
         * Sets the scale of the mesh.
         *
         * The x and y components of the scale vector represent the scale in the x and y directions respectively.
         *
         * @param scale The scale of the mesh.
         */
        void setScale(glm::vec2 scale);

        /**
         * Gets the scale of the mesh.
         *
         * @returns the scale of the mesh.
         */
        const glm::vec2 &getScale() const;

        /**
         * Sets the shear of the mesh.
         *
         * The x and y components of the shear vector represent the sheer in the x and y directions respectively.
         *
         * @param shear The shear of the mesh.
         */
        void setShear(glm::vec2 shear);

        /**
         * Gets the shear of the mesh.
         *
         * @returns the shear of the mesh.
         */
        const glm::vec2 &getShear() const;

        /**
         * Rotates the mesh by the given amount.
         *
         * The rotation is in radians.
         *
         * A positive angle will rotate the mesh counter-clockwise.
         *
         * @param angle The amount to rotate the mesh by.
         */
        void rotate(float angle);

        /**
         * Sets the rotation of the mesh.
         *
         * The rotation is in radians.
         *
         * @param angle The rotation angle of the mesh.
         */
        void setRotation(float angle);

        /**
         * Gets the rotation of the mesh.
         *
         * The rotation is in radians.
         *
         * @returns the rotation of the mesh.
         */
        float getRotation() const;

        /**
         * The number of times each property has changed.
         *
         * This can be used to determine what properties have changed at runtime.
         */
        struct PropertyChanges
        {
            size_t zIndexChanges = 0;
            size_t translationChanges = 0;
            size_t scaleChanges = 0;
            size_t shearChanges = 0;
            size_t rotationChanges = 0;
        };

        /**
         * Gets the number of times each property has changed.
         *
         * This can be used to determine what properties have changed at runtime.
         *
         * @returns The number of times each property has changed.
         */
        const PropertyChanges &getPropertyChanges() const;

        /**
         * Gets the transformation matrix of the mesh.
         *
         * The transformation matrix is the combined rotation, scale, sheer and translation matrix.
         *
         * The transformation matrix includes the zIndex translation of the mesh.
         *
         * This only recomputes the transformation matrix when the transform's properties have changed.
         *
         * @returns the transformation matrix of the mesh.
         */
        const glm::mat4 &getTransformationMatrix() const;

        /**
         * Sets the transformation matrix of the mesh.
         *
         * @param mat The transformation matrix of the mesh.
         */
        void setTransformationMatrix(const glm::mat4 &mat);

    private:
        unsigned int zIndex = 0;

        glm::vec2 translation = glm::vec2(0.0f);
        glm::vec2 scaleVec = glm::vec2(1.0f);
        glm::vec2 shear = glm::vec2(0.0f);
        float rotation = 0;

        PropertyChanges propertyChanges;

        /**
         * Indicates that the transformation matrix is dirty and needs to be recomputed.
         */
        mutable bool isTransformDirty = true;
        mutable glm::mat4 transformationMatrix = glm::mat4(1.0f);
    };
}