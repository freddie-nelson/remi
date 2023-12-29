#include "../../../include/Rendering/Material/Material.h"

Rendering::MaterialId Rendering::Material::nextId = 0;

Rendering::Texture *Rendering::Material::defaultTexture = new Rendering::Texture(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f), 32, 32);

Rendering::Material::Material(const Rendering::Material &m)
{
    color = m.color;
    texture = m.texture;
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

bool Rendering::Material::isTransparent() const
{
    return color.a() < 1.0f || texture->isTransparent();
}

Rendering::Material &Rendering::Material::operator=(const Rendering::Material &m)
{
    color = m.color;
    texture = m.texture;

    return *this;
}