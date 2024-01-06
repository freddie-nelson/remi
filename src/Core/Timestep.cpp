#include "../../include/Core/Timestep.h"

Core::Timestep::Timestep(Time time)
    : time(time)
{
    this->time = time;
    this->lastTime = time;

    update(time);
}

void Core::Timestep::update(Time time)
{
    this->lastTime = this->time;
    this->time = time;

    microseconds = time - lastTime;
    milliseconds = microseconds / 1000.0;
    seconds = milliseconds / 1000.0;
}

double Core::Timestep::getSeconds() const
{
    return seconds;
}

double Core::Timestep::getMilliseconds() const
{
    return milliseconds;
}

double Core::Timestep::getMicroseconds() const
{
    return microseconds;
}