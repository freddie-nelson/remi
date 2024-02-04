#pragma once

#include "../Renderer.h"
#include "../../ECS/Registry.h"
#include "../../Utility/TypeHelpers.h"
#include "../RenderTarget.h"
#include "../Texture/TextureManager.h"
#include "../../Core/SpaceTransformer.h"

#include <string>

namespace Rendering
{
    /**
     * Represents the input to a render pass.
     */
    struct RenderPassInput
    {
        /**
         * The renderer to use.
         */
        Renderer *renderer;

        /**
         * The registry to read data from.
         */
        const ECS::Registry *registry;

        /**
         * The camera to render with.
         */
        ECS::Entity camera;

        /**
         * The render target to render to.
         */
        const RenderTarget *renderTarget;

        /**
         * The texture manager to use.
         */
        TextureManager *textureManager;

        /**
         * The space transformer.
         */
        Core::SpaceTransformer *spaceTransformer;

        /**
         * Destroys the render pass input.
         */
        virtual ~RenderPassInput() = default;

        /**
         * Gets the data from the input.
         *
         * @returns The data from the input.
         */
        virtual void *getData() = 0;

        /**
         * Gets the type of the data.
         *
         * @returns The type of the data.
         */
        virtual blz::TypeId getDataType() = 0;

        /**
         * Gets the name of the data type.
         *
         * @returns The name of the data type.
         */
        virtual std::string getDataTypeName() = 0;
    };

    /**
     * Represents the input to a render pass.
     *
     * This is a typed version of the input.
     *
     * The input data to a render pass which does not require data should be set to 0.
     */
    template <typename T>
    struct RenderPassInputTyped : public RenderPassInput
    {
        /**
         * Creates a new render pass input.
         */
        RenderPassInputTyped() = default;

        /**
         * Creates a new render pass input.
         *
         * Will copy all fields from the given input, except from the data field.
         *
         * @param input The input to copy fields from.
         * @param data The data to store.
         */
        RenderPassInputTyped(RenderPassInput *input, T *data)
        {
            renderer = input->renderer;
            registry = input->registry;
            camera = input->camera;
            renderTarget = input->renderTarget;
            textureManager = input->textureManager;
            spaceTransformer = input->spaceTransformer;

            this->data = data;
        }

        /**
         * The data.
         */
        T *data;

        /**
         * Destroys the render pass input.
         *
         * This will delete the data too.
         */
        virtual ~RenderPassInputTyped()
        {
            delete data;
        }

        /**
         * Gets the data from the input.
         *
         * @returns The data from the input.
         */
        void *getData()
        {
            return data;
        }

        /**
         * Gets the type of the data.
         *
         * @returns The type of the data.
         */
        blz::TypeId getDataType()
        {
            return blz::TypeInfoGenerator::id<T>;
        }

        /**
         * Gets the name of the data type.
         *
         * @returns The name of the data type.
         */
        std::string getDataTypeName()
        {
            return blz::TypeInfoGenerator::name<T>;
        }
    };

    /**
     * Represents a render pass.
     *
     * This is a single step in the render pipeline.
     */
    class RenderPass
    {
    public:
        RenderPass() = default;

        /**
         * Destroys the render pass.
         */
        virtual ~RenderPass() = default;

        /**
         * Executes the render pass.
         *
         * This will take the input from the previous pass and transform it in some way, to produce the output for the next pass.
         *
         * The input and output are implementation specific
         *
         * At the end of the execute function, the input should either be deleted or returned as the output.
         *
         * @param input The input to the render pass.
         *
         * @throws std::invalid_argument If the input is invalid.
         */
        virtual RenderPassInput *execute(RenderPassInput *input) = 0;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() = 0;

    protected:
        /**
         * Checks if the given input is valid.
         *
         * @param input The input to check.
         *
         * @tparam T The expected type of the input.
         *
         * @throws std::invalid_argument If the input is null.
         * @throws std::invalid_argument If the input is not of the expected type.
         */
        template <typename T>
        void checkInput(RenderPassInput *input)
        {
            if (input == nullptr)
            {
                throw std::invalid_argument("[" + getName() + "]: Input to render pass is null.");
            }

            if (!isType<T>(input))
            {
                throw std::invalid_argument("[" + getName() + "]: Input to render pass is of type '" + input->getDataTypeName() + "', expected '" + blz::TypeInfoGenerator::name<T> + "'.");
            }
        }

        /**
         * Checks if the input is of the given type.
         *
         * @param input The input to check.
         *
         * @tparam T The type to check for.
         *
         * @returns True if the input is of the given type, false otherwise.
         */
        template <typename T>
        bool isType(RenderPassInput *input)
        {
            return input->getDataType() == blz::TypeInfoGenerator::id<T>;
        }
    };
}