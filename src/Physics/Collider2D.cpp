#include "../../include/Physics/Collider2D.h"

#include <stdexcept>
#include <iostream>

Physics::Collider2D::Collider2D(ColliderShape2D *shape)
{
    this->shape = shape->clone();
}

Physics::Collider2D::Collider2D(const Collider2D &other)
{
    std::cout << "Collider2D copy constructor" << std::endl;
    std::cout << this << std::endl;
    if (other.shape == nullptr)
    {
        throw std::invalid_argument("Collider 2D (copy constructor): other collider shape is null.");
    }

    shape = other.shape->clone();
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitutionThreshold = other.restitutionThreshold;
    isSensor = other.isSensor;
    fixtures = nullptr;
    // fixtures = other.fixtures;
}

Physics::Collider2D::Collider2D(const Collider2D &&other)
{
    std::cout << "Collider2D move constructor" << std::endl;
    std::cout << this << std::endl;
    if (other.shape == nullptr)
    {
        throw std::invalid_argument("Collider 2D (move constructor): other collider shape is null.");
    }

    shape = other.shape->clone();
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitutionThreshold = other.restitutionThreshold;
    isSensor = other.isSensor;
    fixtures = nullptr;
    // fixtures = other.fixtures;
}

Physics::Collider2D &Physics::Collider2D::operator=(const Physics::Collider2D &other)
{
    std::cout << "Collider2D copy assignment" << std::endl;
    std::cout << this << std::endl;
    if (other.shape == nullptr)
    {
        throw std::invalid_argument("Collider 2D (copy assignment): other collider shape is null.");
    }

    if (this->shape != nullptr)
    {
        delete this->shape;
    }

    shape = other.shape->clone();
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitutionThreshold = other.restitutionThreshold;
    isSensor = other.isSensor;
    fixtures = nullptr;
    // fixtures = other.fixtures;

    return *this;
}

Physics::Collider2D &Physics::Collider2D::operator=(const Physics::Collider2D &&other)
{
    std::cout << "Collider2D move assignment" << std::endl;
    std::cout << this << std::endl;
    if (other.shape == nullptr)
    {
        throw std::invalid_argument("Collider 2D (move assignment): other collider shape is null.");
    }

    if (this->shape != nullptr)
    {
        delete this->shape;
    }

    shape = other.shape->clone();
    density = other.density;
    friction = other.friction;
    restitution = other.restitution;
    restitutionThreshold = other.restitutionThreshold;
    isSensor = other.isSensor;
    fixtures = nullptr;
    // fixtures = other.fixtures;

    return *this;
}

Physics::Collider2D::~Collider2D()
{
    std::cout << "Collider2D destructor" << std::endl;
    std::cout << this << std::endl;

    if (shape != nullptr)
    {
        std::cout << "Deleting shape" << std::endl;
        delete shape;
    }

    shape = nullptr;
    fixtures = nullptr;
}

const Physics::ColliderShape2D *Physics::Collider2D::getShape() const
{
    std::cout << "Collider2D getShape" << std::endl;
    std::cout << this << std::endl;
    return shape;
}

void Physics::Collider2D::setShape(ColliderShape2D *shape)
{
    if (this->shape != nullptr)
    {
        delete this->shape;
    }

    this->shape = shape->clone();
    fixtures = nullptr;
}

float Physics::Collider2D::getDensity() const
{
    return density;
}

void Physics::Collider2D::setDensity(float density)
{
    this->density = density;

    if (fixtures != nullptr)
    {
        for (b2Fixture *fixture : *fixtures)
        {
            fixture->SetDensity(density);
        }
    }
}

float Physics::Collider2D::getFriction() const
{
    return friction;
}

void Physics::Collider2D::setFriction(float friction)
{
    this->friction = friction;

    if (fixtures != nullptr)
    {
        for (b2Fixture *fixture : *fixtures)
        {
            fixture->SetFriction(friction);
        }
    }
}

float Physics::Collider2D::getRestitution() const
{
    return restitution;
}

void Physics::Collider2D::setRestitution(float restitution)
{
    this->restitution = restitution;

    if (fixtures != nullptr)
    {
        for (b2Fixture *fixture : *fixtures)
        {
            fixture->SetRestitution(restitution);
        }
    }
}

float Physics::Collider2D::getRestitutionThreshold() const
{
    return restitutionThreshold;
}

void Physics::Collider2D::setRestitutionThreshold(float restitutionThreshold)
{
    this->restitutionThreshold = restitutionThreshold;

    if (fixtures != nullptr)
    {
        for (b2Fixture *fixture : *fixtures)
        {
            fixture->SetRestitutionThreshold(restitutionThreshold);
        }
    }
}

bool Physics::Collider2D::getIsSensor() const
{
    return isSensor;
}

void Physics::Collider2D::setIsSensor(bool isSensor)
{
    this->isSensor = isSensor;

    if (fixtures != nullptr)
    {
        for (b2Fixture *fixture : *fixtures)
        {
            fixture->SetSensor(isSensor);
        }
    }
}

std::vector<b2Fixture *> *Physics::Collider2D::getFixtures()
{
    return fixtures;
}

void Physics::Collider2D::setFixtures(std::vector<b2Fixture *> *fixtures)
{
    this->fixtures = fixtures;
}
