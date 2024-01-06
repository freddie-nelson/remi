#pragma once

#include <chrono>

namespace Core
{
    using Time = uint64_t;

    inline uint64_t timeSinceEpochMillisec()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    inline uint64_t timeSinceEpochMicrosec()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    /**
     * A timestep.
     *
     * A timestep is used to calculate the delta time between frames.
     */
    class Timestep
    {
    public:
        /**
         * Creates a new timestep.
         *
         * @param time The time in microseconds.
         */
        Timestep(Time time = timeSinceEpochMicrosec());

        /**
         * Updates the timestep.
         *
         * @param time The time in microseconds.
         */
        void update(Time time = timeSinceEpochMicrosec());

        /**
         * Gets the timestep in seconds.
         *
         * @returns The timestep in seconds.
         */
        double getSeconds() const;

        /**
         * Gets the timestep in milliseconds.
         *
         * @returns The timestep in milliseconds.
         */
        double getMilliseconds() const;

        /**
         * Gets the timestep in microseconds.
         *
         * @returns The timestep in microseconds.
         */
        double getMicroseconds() const;

    private:
        Time time;
        Time lastTime;

        double seconds;
        double milliseconds;
        double microseconds;
    };
}