#include "../../../include/Rendering/Material/AnimatedMaterial.h"

#include <math.h>
#include <stdexcept>

Rendering::AnimatedMaterial::AnimatedMaterial(Color color, AnimatedTexture *texture, float duration, AnimationMode mode)
{
    setAnimatedTexture(texture);
    setColor(color);
    setDuration(duration);
    setMode(mode);
}

Rendering::AnimatedMaterial::AnimatedMaterial(AnimatedTexture *texture, float duration, AnimationMode mode)
{
    setAnimatedTexture(texture);
    setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    setDuration(duration);
    setMode(mode);
}

Rendering::AnimatedMaterial::~AnimatedMaterial()
{
}

void Rendering::AnimatedMaterial::step(float dt)
{
    if (paused)
    {
        return;
    }

    // step time
    if (mode == AnimationMode::REVERSE || mode == AnimationMode::ONCE_REVERSE || reversing)
    {
        time -= dt;
    }
    else
    {
        time += dt;
    }

    // looping behaviour
    switch (mode)
    {
    case AnimationMode::LOOP:
    case AnimationMode::REVERSE:
        if (time >= duration)
        {
            time = time - duration;
        }
        else if (time <= 0.0f)
        {
            time = duration + time;
        }
        break;
    case AnimationMode::PING_PONG:
        if (time >= duration)
        {
            reversing = true;
            time = duration - (time - duration);
        }
        else if (time <= 0.0f)
        {
            reversing = false;
            time = -time;
        }
        break;
    case AnimationMode::ONCE:
    case AnimationMode::ONCE_REVERSE:
        if (time >= duration)
        {
            time = duration;
        }
        else if (time <= 0.0f)
        {
            time = 0.0f;
        }
        break;

    default:
        // should never happen
        throw std::invalid_argument("AnimatedTexture (step): invalid animation mode.");
    }

    // set the texture to the current frame
    texture = &getCurrentFrame();
}

void Rendering::AnimatedMaterial::pause()
{
    paused = true;
}

void Rendering::AnimatedMaterial::play()
{
    paused = false;
}

void Rendering::AnimatedMaterial::restart()
{
    time = 0.0f;
    reversing = false;
    paused = false;
    texture = &getCurrentFrame();
}

void Rendering::AnimatedMaterial::setAnimatedTexture(AnimatedTexture *texture)
{
    if (texture == nullptr)
    {
        throw std::invalid_argument("AnimatedMaterial (setAnimatedTexture): texture cannot be null");
    }

    animatedTexture = texture;
    restart();
}

const Rendering::AnimatedTexture *Rendering::AnimatedMaterial::getAnimatedTexture() const
{
    return animatedTexture;
}

const Rendering::Texture &Rendering::AnimatedMaterial::getCurrentFrame() const
{
    return animatedTexture->getFrame(getCurrentFrameIndex());
}

size_t Rendering::AnimatedMaterial::getCurrentFrameIndex() const
{
    auto frameCount = animatedTexture->getFrameCount();

    size_t index = static_cast<size_t>(std::floor((time / duration) * frameCount));

    // if the time >= duration then the index will be out of bounds
    // so we need to clamp it to the last frame
    if (index >= frameCount)
    {
        index = frameCount - 1;
    }

    return index;
}

float Rendering::AnimatedMaterial::getDuration() const
{
    return duration;
}

void Rendering::AnimatedMaterial::setDuration(float duration)
{
    if (duration <= 0.0f)
    {
        throw std::invalid_argument("AnimatedTexture (setDuration): duration cannot be less than or equal to 0.");
    }

    this->duration = duration;
    restart();
}

float Rendering::AnimatedMaterial::getFrameDuration() const
{
    return duration / animatedTexture->getFrameCount();
}

void Rendering::AnimatedMaterial::setMode(AnimationMode mode)
{
    this->mode = mode;
}

Rendering::AnimationMode Rendering::AnimatedMaterial::getMode() const
{
    return mode;
}

float Rendering::AnimatedMaterial::getTime() const
{
    return time;
}

void Rendering::AnimatedMaterial::setTime(float time)
{
    this->time = time;
}

bool Rendering::AnimatedMaterial::isTransparent() const
{
    return Material::isTransparent() || animatedTexture->isTransparent();
}

bool Rendering::AnimatedMaterial::isPaused() const
{
    return paused;
}