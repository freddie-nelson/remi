#include "../../../include/Rendering/Texture/AnimatedTexture.h"

#include <math.h>

Rendering::AnimatedTexture::AnimatedTexture(std::vector<std::string> frames, float duration, AnimationMode mode, bool flip) : duration(duration), mode(mode)
{
    setFrames(frames, flip);
}

Rendering::AnimatedTexture::AnimatedTexture(std::vector<Texture> frames, float duration, AnimationMode mode) : duration(duration), mode(mode)
{
    setFrames(frames);
}

Rendering::AnimatedTexture::~AnimatedTexture()
{
}

void Rendering::AnimatedTexture::setFrames(std::vector<std::string> frames, bool flip)
{
    this->frames.clear();

    for (auto &frame : frames)
    {
        this->frames.push_back(Texture(frame, flip));
    }
}

void Rendering::AnimatedTexture::setFrames(std::vector<Texture> frames)
{
    this->frames = frames;
}

const std::vector<Rendering::Texture> &Rendering::AnimatedTexture::getFrames()
{
    return frames;
}

const Rendering::Texture &Rendering::AnimatedTexture::getFrame(unsigned int index)
{
    return frames[index];
}

const Rendering::Texture &Rendering::AnimatedTexture::getCurrentFrame()
{
    return frames[getCurrentFrameIndex()];
}

size_t Rendering::AnimatedTexture::getCurrentFrameIndex()
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

float Rendering::AnimatedTexture::getDuration()
{
    return duration;
}

float Rendering::AnimatedTexture::getFrameDuration()
{
    return duration / frames.size();
}

size_t Rendering::AnimatedTexture::getFrameCount()
{
    return frames.size();
}

void Rendering::AnimatedTexture::setMode(AnimationMode mode)
{
    this->mode = mode;
}

Rendering::AnimatedTexture::AnimationMode Rendering::AnimatedTexture::getMode()
{
    return mode;
}

float Rendering::AnimatedTexture::getTime()
{
    return time;
}

void Rendering::AnimatedTexture::setTime(float time)
{
    this->time = time;
}

void Rendering::AnimatedTexture::step(float dt)
{
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
        if (time >= duration)
        {
            time = time - duration;
        }
        break;
    case AnimationMode::REVERSE:
        if (time <= 0.0f)
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
        break;
    }
}
