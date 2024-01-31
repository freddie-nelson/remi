#include "../../../include/Rendering/Material/Material.h"

#include <iostream>

Rendering::MaterialId Rendering::Material::nextId = 0;

Rendering::Texture *Rendering::Material::defaultTexture = new Rendering::Texture(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f), 32u, 32u);

Rendering::Material::Material(const Rendering::Material &m)
{
    color = m.color;
    texture = m.texture;
    animatedTexture = m.animatedTexture;
}

Rendering::Material::Material()
{
    setColor(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f));
    setTexture(defaultTexture);
}

Rendering::Material::Material(Rendering::Color color)
{
    setColor(color);
    setTexture(defaultTexture);
}

Rendering::Material::Material(Rendering::Texture *texture)
{
    setColor(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f));
    setTexture(texture);
}

Rendering::Material::Material(Rendering::AnimatedTexture *texture)
{
    setColor(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f));
    setTexture(texture);
}

Rendering::Material::Material(Rendering::Color color, Rendering::Texture *texture)
{
    setColor(color);
    setTexture(texture);
}

Rendering::Material::Material(Rendering::Color color, Rendering::AnimatedTexture *texture)
{
    setColor(color);
    setTexture(texture);
}

Rendering::Material::~Material()
{
}

const Rendering::Color &Rendering::Material::getColor() const
{
    return color;
}

void Rendering::Material::setColor(Rendering::Color color)
{
    this->color = color;
}

const Rendering::Texture *Rendering::Material::getTexture() const
{
    if (isAnimated())
    {
        return &animatedTexture->getCurrentFrame();
    }

    return texture;
}

Rendering::AnimatedTexture *Rendering::Material::getAnimatedTexture() const
{
    return animatedTexture;
}

bool Rendering::Material::isAnimated() const
{
    return animatedTexture != nullptr;
}

void Rendering::Material::setTexture(Rendering::Texture *texture)
{
    if (texture == nullptr)
    {
        this->animatedTexture = nullptr;
        this->texture = defaultTexture;
        return;
    }

    this->animatedTexture = nullptr;
    this->texture = texture;
}

void Rendering::Material::setTexture(Rendering::AnimatedTexture *texture)
{
    if (texture == nullptr)
    {
        this->animatedTexture = nullptr;
        this->texture = defaultTexture;
        return;
    }

    this->texture = nullptr;
    this->animatedTexture = texture;
}

bool Rendering::Material::isTransparent() const
{
    return color.a() < 1.0f || (isAnimated() ? animatedTexture->isTransparent() : texture->isTransparent());
}

Rendering::Material &Rendering::Material::operator=(const Rendering::Material &m)
{
    color = m.color;
    texture = m.texture;
    animatedTexture = m.animatedTexture;

    return *this;
}