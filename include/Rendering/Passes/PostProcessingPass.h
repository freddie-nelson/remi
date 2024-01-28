#pragma once

#include "RenderPass.h"
#include "../Shader/Shader.h"

#include <glm/glm.hpp>
#include <string>

namespace Rendering
{
    /**
     * Represents a post processor.
     *
     * A post processor is a shader that is applied to the screen after rendering.
     *
     * The shader must be created in the constructor of a class derived from the post processor.
     *
     * The fragment shader must contain the following uniforms:
     * - sampler2D uRenderTexture
     *
     * The fragment shader may contain the following uniforms:
     * - vec2 uResolution
     *
     * The fragment shader must also contain the following inputs:
     * - vec2 vTexCoord
     *
     * The fragment shader must also contain the following outputs:
     * - vec4 FragColor
     */
    class PostProcessingPass : public RenderPass
    {
    public:
        /**
         * Creates a PostProcessingPass instance.
         */
        PostProcessingPass() = default;

        /**
         * Destroys the PostProcessingPass instance.
         */
        virtual ~PostProcessingPass() = default;

        /**
         * Executes the post processor.
         *
         * @param input The input to the post processor.
         */
        virtual RenderPassInput *execute(RenderPassInput *input) override;

        /**
         * Gets the shader attached to this post processor.
         *
         * @returns The shader attached to this post processor.
         */
        const Shader &getShader() const;

        /**
         * Sets a uniform for the shader attached to this post processor.
         *
         * This will replace any existing uniform with the same name.
         *
         * @param uniform The uniform to set.
         */
        void uniform(UniformBase *uniform);

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "PostProcessingPass";
        };

    protected:
        /**
         * Whether or not to output the result of the post processor to the screen instead of the render target.
         */
        bool outputToScreen = false;

        std::unordered_map<std::string, UniformBase *> uniforms;

        /**
         * Creates the shader for the post processor.
         *
         * @param fragmentShader The fragment shader of the post processor.
         */
        void createShader(std::string fragmentShader);

    private:
        Shader shader;

        static const std::string vertexShader;
        static const std::vector<glm::vec2> vertices;
        static const std::vector<glm::vec2> texCoords;
        static const std::vector<unsigned int> indices;
    };
}