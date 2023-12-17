#include "../../../include/Rendering/Material/Material.h"

Rendering::Texture *Rendering::Material::defaultTexture = new Rendering::Texture(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f), 1, 1);

Rendering::Material::Material()
{
    setColor(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f));
}

Rendering::Material::Material(Rendering::Color color)
{
    setColor(color);
}

Rendering::Material::Material(Rendering::Texture *texture)
{
    setTexture(texture);
}

Rendering::Material::Material(Rendering::Color color, Rendering::Texture *texture)
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
    return texture;
}

void Rendering::Material::setTexture(Rendering::Texture *texture)
{
    if (texture == nullptr)
    {
        this->texture = defaultTexture;
    }

    this->texture = texture;
}