#include "../../../include/Rendering/Texture/AnimatedTexture.h"

#include <math.h>
#include <stdexcept>
#include <iostream>

Rendering::AnimatedTexture::AnimatedTexture(const std::vector<std::string> &frames, float duration, AnimationMode mode, bool flip) : mode(mode)
{
    setDuration(duration);
    setFrames(frames, flip);
}

Rendering::AnimatedTexture::AnimatedTexture(const std::vector<Texture *> &frames, float duration, AnimationMode mode) : mode(mode)
{
    setDuration(duration);
    setFrames(frames);
}

Rendering::AnimatedTexture::~AnimatedTexture()
{
    destroyFrames();
}

void Rendering::AnimatedTexture::setFrames(const std::vector<std::string> &frames, bool flip)
{
    if (frames.size() == 0)
    {
        throw std::invalid_argument("AnimatedTexture (setFrames): frames must not be empty.");
    }

    destroyFrames();

    for (auto &frame : frames)
    {
        this->frames.push_back(new Texture(frame, flip));
    }

    updateTransparent();
}

void Rendering::AnimatedTexture::setFrames(const std::vector<Texture *> &frames)
{
    if (frames.size() == 0)
    {
        throw std::invalid_argument("AnimatedTexture (setFrames): frames must not be empty.");
    }

    destroyFrames();

    for (auto &frame : frames)
    {
        this->frames.push_back(new Texture(*frame));
    }

    updateTransparent();
}

const std::vector<Rendering::Texture *> &Rendering::AnimatedTexture::getFrames() const
{
    return frames;
}

const Rendering::Texture &Rendering::AnimatedTexture::getFrame(unsigned int index) const
{
    return *frames[index];
}

const Rendering::Texture &Rendering::AnimatedTexture::getCurrentFrame() const
{
    return *frames[getCurrentFrameIndex()];
}

size_t Rendering::AnimatedTexture::getCurrentFrameIndex() const
{
    size_t index = static_cast<size_t>(std::floor((time / duration) * frames.size()));

    // if the time >= duration then the index will be out of bounds
    // so we need to clamp it to the last frame
    if (index >= frames.size())
    {
        index = frames.size() - 1;
    }

    return index;
}

float Rendering::AnimatedTexture::getDuration() const
{
    return duration;
}

void Rendering::AnimatedTexture::setDuration(float duration)
{
    if (duration <= 0.0f)
    {
        throw std::invalid_argument("AnimatedTexture (setDuration): duration cannot be less than or equal to 0.");
    }

    this->duration = duration;
}

float Rendering::AnimatedTexture::getFrameDuration() const
{
    return duration / frames.size();
}

size_t Rendering::AnimatedTexture::getFrameCount() const
{
    return frames.size();
}

void Rendering::AnimatedTexture::setMode(AnimationMode mode)
{
    this->mode = mode;
}

Rendering::AnimatedTexture::AnimationMode Rendering::AnimatedTexture::getMode() const
{
    return mode;
}

float Rendering::AnimatedTexture::getTime() const
{
    return time;
}

void Rendering::AnimatedTexture::setTime(float time)
{
    this->time = time;
}

void Rendering::AnimatedTexture::step(float dt)
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
}

void Rendering::AnimatedTexture::pause()
{
    paused = true;
}

void Rendering::AnimatedTexture::play()
{
    paused = false;
}

bool Rendering::AnimatedTexture::isTransparent() const
{
    return transparent;
}

void Rendering::AnimatedTexture::updateTransparent()
{
    transparent = false;

    for (auto &frame : frames)
    {
        if (frame->isTransparent())
        {
            transparent = true;
            return;
        }
    }
}

void Rendering::AnimatedTexture::destroyFrames()
{
    for (auto &frame : frames)
    {
        delete frame;
    }

    frames.clear();
}
