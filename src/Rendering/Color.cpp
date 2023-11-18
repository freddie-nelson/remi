#include "../../include/Rendering/Color.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>

bool Rendering::Color::isColorValid(glm::vec4 rgba)
{
    std::string names[4] = {"red", "green", "blue", "alpha"};

    for (int i = 0; i < 4; i++)
    {
        if (rgba[i] < 0.0f || rgba[i] > 1.0f)
        {
            throw std::invalid_argument("Invalid color: " + names[i] + " value must be between 0 and 1 inclusive.");
        }
    }

    return true;
}

bool Rendering::Color::isColorValidSafe(glm::vec4 rgba)
{
    try
    {
        return isColorValid(rgba);
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

Rendering::Color::Color(float r, float g, float b, float a)
{
    setColor(r, g, b, a);
}

Rendering::Color::Color(glm::vec4 rgba)
{
    setColor(rgba);
}

Rendering::Color::Color(glm::vec3 rgb)
{
    setColor(rgb);
}

Rendering::Color::Color(float rgb, float a)
{
    setColor(rgb, a);
}

void Rendering::Color::lighten(float amount)
{
    if (amount < 0.0f || amount > 1.0f)
    {
        throw std::invalid_argument("amount must be between 0 and 1 inclusive");
    }

    float a = rgba.a;
    rgba *= amount + 1.0f;
    rgba.a = a;

    clamp(0.0f, 1.0f);
}

void Rendering::Color::darken(float amount)
{
    if (amount < 0.0f || amount > 1.0f)
    {
        throw std::invalid_argument("amount must be between 0 and 1 inclusive");
    }

    float a = rgba.a;
    rgba *= 1.0f - amount;
    rgba.a = a;

    clamp(0.0f, 1.0f);
}

Rendering::Color Rendering::Color::blend(const Color &fg)
{
    Color blend(0.0f);

    if (fg.a() == 1.0f)
    {
        blend.setColor(fg.getColor());
    }
    else if (fg.a() == 0.0f)
    {
        blend.setColor(rgba);
    }
    else
    {
        // convert to 0 to 1 range
        float blendAlpha = 1 - (1 - fg.a()) * (1 - a());

        blend.setA(blendAlpha);
        blend.setR((fg.r() * fg.a() / blendAlpha) + (r() * a() * (1 - fg.a()) / blendAlpha));
        blend.setG((fg.g() * fg.a() / blendAlpha) + (g() * a() * (1 - fg.a()) / blendAlpha));
        blend.setB((fg.b() * fg.a() / blendAlpha) + (b() * a() * (1 - fg.a()) / blendAlpha));
    }

    return blend;
}

void Rendering::Color::clamp(float min, float max)
{
    clampR(min, max);
    clampG(min, max);
    clampB(min, max);
    clampA(min, max);
}

void Rendering::Color::clampRGB(float min, float max)
{
    clampR(min, max);
    clampG(min, max);
    clampB(min, max);
}

void Rendering::Color::clampR(float min, float max)
{
    validateClampMinMax(min, max);
    rgba.r = std::clamp(rgba.r, min, max);
}

void Rendering::Color::clampG(float min, float max)
{
    validateClampMinMax(min, max);
    rgba.g = std::clamp(rgba.g, min, max);
}

void Rendering::Color::clampB(float min, float max)
{
    validateClampMinMax(min, max);
    rgba.b = std::clamp(rgba.b, min, max);
}

void Rendering::Color::clampA(float min, float max)
{
    validateClampMinMax(min, max);
    rgba.a = std::clamp(rgba.a, min, max);
}

glm::vec4 Rendering::Color::getColor() const
{
    return rgba;
}

void Rendering::Color::setColor(float r, float g, float b, float a)
{
    rgba = glm::vec4(r, g, b, a);
    Color::isColorValid(rgba);
}

void Rendering::Color::setColor(glm::vec4 rgba)
{
    this->rgba = rgba;
    Color::isColorValid(rgba);
}

void Rendering::Color::setColor(glm::vec3 rgb)
{
    rgba = glm::vec4(rgb, 1.0f);
    Color::isColorValid(rgba);
}

void Rendering::Color::setColor(float rgb, float a)
{
    rgba = glm::vec4(rgb, rgb, rgb, a);
    Color::isColorValid(rgba);
}

void Rendering::Color::setR(float r)
{
    rgba.r = r;
    Color::isColorValid(rgba);
}

void Rendering::Color::setG(float g)
{
    rgba.g = g;
    Color::isColorValid(rgba);
}

void Rendering::Color::setB(float b)
{
    rgba.b = b;
    Color::isColorValid(rgba);
}

void Rendering::Color::setA(float a)
{
    rgba.a = a;
    Color::isColorValid(rgba);
}

void Rendering::Color::validateClampMinMax(float min, float max)
{
    if (min < 0.0f || min > 1.0f)
    {
        throw std::invalid_argument("min must be between 0 and 1 inclusive");
    }
    if (max < 0.0f || max > 1.0f)
    {
        throw std::invalid_argument("max must be between 0 and 1 inclusive");
    }
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max.");
    }
}