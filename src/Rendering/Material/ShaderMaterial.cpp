#include "../../../include/Rendering/Material/ShaderMaterial.h"

#include <iostream>

std::unordered_map<std::string, Rendering::ShaderMaterial::FragShaderKey> Rendering::ShaderMaterial::fragShaderToKey = std::unordered_map<std::string, Rendering::ShaderMaterial::FragShaderKey>();

std::unordered_map<Rendering::ShaderMaterial::FragShaderKey, std::string> Rendering::ShaderMaterial::keyToFragShader = std::unordered_map<Rendering::ShaderMaterial::FragShaderKey, std::string>();

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

Rendering::ShaderMaterial &Rendering::ShaderMaterial::operator=(const ShaderMaterial &m)
{
    Material::operator=(m);

    setFragmentShader(m.getFragmentShader());

    return *this;
}