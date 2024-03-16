#include "../../../include/Rendering/Texture/AnimatedTexture.h"

#include <math.h>
#include <stdexcept>
#include <iostream>

Rendering::AnimatedTexture::AnimatedTexture(const std::vector<std::string> &frames, bool flip)
{
    setFrames(frames, flip);
}

Rendering::AnimatedTexture::AnimatedTexture(const std::vector<Texture *> &frames)
{
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

size_t Rendering::AnimatedTexture::getFrameCount() const
{
    return frames.size();
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
