#include "../../../include/Rendering/Material/ShaderMaterial.h"

#include <iostream>

std::unordered_map<std::string, Rendering::ShaderMaterial::FragShaderKey> Rendering::ShaderMaterial::fragShaderToKey = std::unordered_map<std::string, Rendering::ShaderMaterial::FragShaderKey>();

std::unordered_map<Rendering::ShaderMaterial::FragShaderKey, std::string> Rendering::ShaderMaterial::keyToFragShader = std::unordered_map<Rendering::ShaderMaterial::FragShaderKey, std::string>();

std::unordered_map<Rendering::ShaderMaterial::FragShaderKey, std::unordered_map<std::string, Rendering::UniformBase *>> Rendering::ShaderMaterial::uniforms = std::unordered_map<Rendering::ShaderMaterial::FragShaderKey, std::unordered_map<std::string, Rendering::UniformBase *>>();

Rendering::ShaderMaterial::FragShaderKey Rendering::ShaderMaterial::nextKey = 1;

Rendering::ShaderMaterial::ShaderMaterial(std::string fragmentShader, Color color, Texture *texture)
    : Material(color, texture)
{
    setFragmentShader(fragmentShader);
}

Rendering::ShaderMaterial::ShaderMaterial(std::string fragmentShader, Texture *texture)
    : Material(texture)
{
    setFragmentShader(fragmentShader);
}

Rendering::ShaderMaterial::ShaderMaterial(const ShaderMaterial &m)
    : Material(m)
{
    setFragmentShader(m.getFragmentShader());
    transparency = m.transparency;
    transparencySet = m.transparencySet;
}

const std::string &Rendering::ShaderMaterial::getFragmentShader() const
{
    return ShaderMaterial::keyToFragShader.at(fragShaderKey);
}

void Rendering::ShaderMaterial::setFragmentShader(std::string fragmentShader)
{
    if (ShaderMaterial::fragShaderToKey.contains(fragmentShader))
    {
        fragShaderKey = ShaderMaterial::fragShaderToKey[fragmentShader];
    }
    else
    {
        auto key = ShaderMaterial::nextKey++;

        ShaderMaterial::fragShaderToKey[fragmentShader] = key;
        ShaderMaterial::keyToFragShader[key] = fragmentShader;

        fragShaderKey = key;
    }
}

Rendering::ShaderMaterial::FragShaderKey Rendering::ShaderMaterial::getFragmentShaderKey() const
{
    return fragShaderKey;
}

void Rendering::ShaderMaterial::uniform(UniformBase *uniform)
{
    if (!uniforms.contains(fragShaderKey))
    {
        uniforms[fragShaderKey] = std::unordered_map<std::string, UniformBase *>();
    }

    uniforms[fragShaderKey][uniform->getName()] = uniform;
}

const std::unordered_map<std::string, Rendering::UniformBase *> &Rendering::ShaderMaterial::getUniforms() const
{
    if (!uniforms.contains(fragShaderKey))
    {
        uniforms[fragShaderKey] = std::unordered_map<std::string, UniformBase *>();
    }

    return uniforms.at(fragShaderKey);
}

void Rendering::ShaderMaterial::setTransparency(bool transparency)
{
    transparencySet = true;
    this->transparency = transparency;
}

bool Rendering::ShaderMaterial::isTransparent() const
{
    if (transparencySet)
    {
        return transparency;
    }

    return Material::isTransparent();
}

Rendering::ShaderMaterial &Rendering::ShaderMaterial::operator=(const ShaderMaterial &m)
{
    Material::operator=(m);

    setFragmentShader(m.getFragmentShader());
    transparencySet = m.transparencySet;
    transparency = m.transparency;

    return *this;
}