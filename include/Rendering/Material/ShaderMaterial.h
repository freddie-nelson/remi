#pragma once

#include "Material.h"
#include "../Shader/Uniform.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Rendering
{

    /**
     * Represents an special type of material that contains a fragment shader.
     *
     * The fragment shader will be used in place of the default fragment shader used by the renderer.
     *
     * The fragment shader must contain the following uniforms:
     * - sampler2D uTextures[__MAX_TEXTURE_UNITS__]
     *
     * The fragment shader must also contain the following inputs:
     * - flat uint vTextureUnit
     * - vec2 vTexCoord
     * - vec4 vColor
     * - vec2 vUv
     *
     * The fragment shader must also contain the following outputs:
     * - vec4 FragColor
     *
     * The renderer will crash if this is not the case.
     *
     * When the fragment shader is set, it will be assigned a key. This is used by the renderer to refer to the shader generated for the material.
     */
    class ShaderMaterial : public Material
    {
    public:
        using FragShaderKey = size_t;

        /**
         * Creates a ShaderMaterial instance.
         *
         * @param fragmentShader The fragment shader of the material.
         */
        ShaderMaterial(std::string fragmentShader, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f), Texture *texture = nullptr);

        /**
         * Creates a ShaderMaterial instance.
         *
         * @param fragmentShader The fragment shader of the material.
         * @param texture The texture of the material.
         */
        ShaderMaterial(std::string fragmentShader, Texture *texture);

        /**
         * Creates a ShaderMaterial instance.
         *
         * @param m The ShaderMaterial to copy.
         */
        ShaderMaterial(const ShaderMaterial &m);

        /**
         * Gets the fragment shader of the material.
         *
         * @return The fragment shader of the material.
         */
        const std::string &getFragmentShader() const;

        /**
         * Sets the fragment shader of the material.
         *
         * @param fragmentShader The fragment shader of the material.
         */
        void setFragmentShader(std::string fragmentShader);

        /**
         * Gets the key of the fragment shader.
         *
         * @returns The key of the fragment shader.
         */
        FragShaderKey getFragmentShaderKey() const;

        /**
         * Sets the uniform for the shader attached to this material.
         *
         * This will set the uniform for all shader materials that use the same fragment shader.
         *
         * @param uniform The uniform to set.
         */
        void uniform(UniformBase *uniform);

        /**
         * Gets the uniforms for the shader attached to this material.
         *
         * @returns The uniforms for the shader attached to this material.
         */
        const std::unordered_map<std::string, UniformBase *> &getUniforms() const;

        /**
         * Copies the given ShaderMaterial into this ShaderMaterial.
         *
         * @param m The ShaderMaterial to copy.
         */
        ShaderMaterial &operator=(const ShaderMaterial &m);

    private:
        static std::unordered_map<std::string, FragShaderKey> fragShaderToKey;

        static std::unordered_map<FragShaderKey, std::string> keyToFragShader;

        static std::unordered_map<FragShaderKey, std::unordered_map<std::string, UniformBase *>> uniforms;

        /**
         * The next key to assign to a fragment shader.
         *
         * Should start at 1 as 0 is reserved for the default shader.
         */
        static FragShaderKey nextKey;

        FragShaderKey fragShaderKey;
    };
}