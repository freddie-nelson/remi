#pragma once

#include <string>

namespace Core
{
    /**
     * Represents an observer.
     *
     * An observer is an object that listens for events from a Subject.
     */
    template <typename T>
    class Observer
    {
    public:
        virtual ~Observer() = default;

        /**
         * Informs the observer of an event.
         *
         * @param event The event that occurred.
         * @param data The data associated with the event.
         */
        virtual void updateObserver(std::string event, T data) = 0;
    };
}