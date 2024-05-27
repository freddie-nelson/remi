#include "../../include/Physics/Collider2D.h"

#include <stdexcept>
#include <iostream>

Physics::Collider2D::Collider2D(ColliderShape2D *shape)
{
    this->shape = shape->clone();
}

Physics::Collider2D::Collider2D(const Collider2D &other)
{
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
    filter = other.filter;
    // fixtures = other.fixtures;
}

Physics::Collider2D::Collider2D(const Collider2D &&other)
{
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
    filter = other.filter;
    // fixtures = other.fixtures;
}

Physics::Collider2D &Physics::Collider2D::operator=(const Physics::Collider2D &other)
{
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
    filter = other.filter;
    // fixtures = other.fixtures;

    return *this;
}

Physics::Collider2D &Physics::Collider2D::operator=(const Physics::Collider2D &&other)
{
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
    filter = other.filter;
    // fixtures = other.fixtures;

    return *this;
}

Physics::Collider2D::~Collider2D()
{
    if (shape != nullptr)
    {
        delete shape;
    }

    shape = nullptr;
    fixtures = nullptr;
}

const Physics::ColliderShape2D *Physics::Collider2D::getShape() const
{
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

void Physics::Collider2D::setCollisionCategory(uint16_t category)
{
    filter.category = category;

    updateCollisionFilter();
}

uint16_t Physics::Collider2D::getCollisionCategory() const
{
    return filter.category;
}

void Physics::Collider2D::setCollisionMask(uint16_t mask)
{
    filter.mask = mask;

    updateCollisionFilter();
}

uint16_t Physics::Collider2D::getCollisionMask() const
{
    return filter.mask;
}

void Physics::Collider2D::setCollisionGroup(int16_t group)
{
    filter.group = group;

    updateCollisionFilter();
}

int16_t Physics::Collider2D::getCollisionGroup() const
{
    return filter.group;
}

void Physics::Collider2D::setCollisionFilter(uint16_t category, uint16_t mask, int16_t group)
{
    filter.category = category;
    filter.mask = mask;
    filter.group = group;

    updateCollisionFilter();
}

void Physics::Collider2D::setCollisionFilter(CollisionFilter filter)
{
    this->filter = filter;

    updateCollisionFilter();
}

Physics::CollisionFilter &Physics::Collider2D::getCollisionFilter()
{
    return filter;
}

const Physics::CollisionFilter &Physics::Collider2D::getCollisionFilter() const
{
    return filter;
}

std::vector<b2Fixture *> *Physics::Collider2D::getFixtures()
{
    return fixtures;
}

void Physics::Collider2D::setFixtures(std::vector<b2Fixture *> *fixtures)
{
    this->fixtures = fixtures;
}

void Physics::Collider2D::updateCollisionFilter()
{
    if (fixtures != nullptr)
    {
        for (b2Fixture *fixture : *fixtures)
        {
            b2Filter f = fixture->GetFilterData();
            f.categoryBits = filter.category;
            f.maskBits = filter.mask;
            f.groupIndex = filter.group;

            fixture->SetFilterData(f);
        }
    }
}
