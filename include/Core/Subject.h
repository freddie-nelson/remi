#pragma once

#include "Observer.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Core
{
    /**
     * Represents a subject.
     *
     * A subject is an object that can be observed by observers.
     *
     * The subject notifies all observers listening to an event when the event occurs.
     */
    template <typename T>
    class Subject
    {
    public:
        virtual ~Subject() = default;

        /**
         * Notifies all observers listening to the event.
         *
         * @param event The event to notify the observers of.
         */
        virtual void notifyObservers(std::string event, T data) const
        {
            if (!observers.contains(event))
            {
                return;
            }

            auto &eventObservers = observers.at(event);
            for (auto &eventObserver : eventObservers)
            {
                eventObserver->updateObserver(event, data);
            }
        }

        /**
         * Attaches an observer to the subject.
         *
         * @param event The event to attach the observer to.
         */
        virtual void attachObserver(std::string event, Observer<T> *observer)
        {
            if (!observers.contains(event))
            {
                observers.emplace(event, std::vector<Observer<T> *>());
            }

            auto &eventObservers = observers.at(event);
            eventObservers.push_back(observer);
        }

        /**
         * Detaches an observer from the subject.
         *
         * @param event The event to detach the observer from.
         */
        virtual void detachObserver(std::string event, Observer<T> *observer)
        {
            if (!observers.contains(event))
            {
                return;
            }

            auto &eventObservers = observers.at(event);
            for (auto it = eventObservers.begin(); it != eventObservers.end(); ++it)
            {
                if (*it == observer)
                {
                    eventObservers.erase(it);
                    return;
                }
            }
        }

        /**
         * Checks if the observer is attached to the subject.
         *
         * @param event The event to check if the observer is attached to.
         * @param observer The observer to check if it is attached to the event.
         */
        virtual bool hasObserver(std::string event, Observer<T> *observer) const
        {
            if (!observers.contains(event))
            {
                return false;
            }

            auto &eventObservers = observers.at(event);
            for (auto &eventObserver : eventObservers)
            {
                if (eventObserver == observer)
                {
                    return true;
                }
            }

            return false;
        }

    protected:
        std::unordered_map<std::string, std::vector<Observer<T> *>> observers;
    };
}