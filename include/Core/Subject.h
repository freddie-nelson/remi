#pragma once

#include "Observer.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
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
        using FunctionObserverId = std::size_t;

        virtual ~Subject() = default;

        /**
         * Notifies all observers listening to the event.
         *
         * @param event The event to notify the observers of.
         */
        virtual void notifyObservers(std::string event, T data) const
        {
            auto eventObserversIt = observers.find(event);
            if (eventObserversIt != observers.end())
            {
                auto &eventObservers = eventObserversIt->second;
                for (auto &eventObserver : eventObservers)
                {
                    eventObserver->updateObserver(event, data);
                }
            }

            auto dataFunctionObserversIt = functionObservers.find(event);
            if (dataFunctionObserversIt != functionObservers.end())
            {
                auto &eventFunctionObservers = dataFunctionObserversIt->second;
                for (auto &eventFunctionObserver : eventFunctionObservers)
                {
                    eventFunctionObserver.callback(data);
                }
            }

            auto eventDataFunctionObserversIt = eventAwareFunctionObservers.find(event);
            if (eventDataFunctionObserversIt != eventAwareFunctionObservers.end())
            {
                auto &eventFunctionObservers = eventDataFunctionObserversIt->second;
                for (auto &eventFunctionObserver : eventFunctionObservers)
                {
                    eventFunctionObserver.callback(event, data);
                }
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
         * Attaches a callable observer that consumes only event data.
         *
         * @param event The event to attach the observer to.
         * @param observer The callable observer to attach.
         * @return A function observer id that can be used to detach capture lambdas.
         */
        template <typename Callable>
        requires(std::is_invocable_v<std::decay_t<Callable> &, T> && !std::is_invocable_v<std::decay_t<Callable> &, std::string, T>)
        FunctionObserverId attachObserver(std::string event, Callable &&observer)
        {
            using DecayedCallable = std::decay_t<Callable>;

            auto &eventObservers = functionObservers[event];
            if constexpr (std::is_convertible_v<DecayedCallable, void (*)(T)>)
            {
                eventObservers.push_back(DataFunctionObserverEntry{
                    .id = nextFunctionObserverId,
                    .callback = static_cast<void (*)(T)>(observer),
                });
            }
            else
            {
                eventObservers.push_back(DataFunctionObserverEntry{
                    .id = nextFunctionObserverId,
                    .callback = std::forward<Callable>(observer),
                });
            }

            return nextFunctionObserverId++;
        }

        /**
         * Attaches a callable observer that consumes event name and data.
         *
         * @param event The event to attach the observer to.
         * @param observer The callable observer to attach.
         * @return A function observer id that can be used to detach capture lambdas.
         */
        template <typename Callable>
        requires(std::is_invocable_v<std::decay_t<Callable> &, std::string, T>)
        FunctionObserverId attachObserver(std::string event, Callable &&observer)
        {
            using DecayedCallable = std::decay_t<Callable>;

            auto &eventObservers = eventAwareFunctionObservers[event];
            if constexpr (std::is_convertible_v<DecayedCallable, void (*)(std::string, T)>)
            {
                eventObservers.push_back(EventDataFunctionObserverEntry{
                    .id = nextFunctionObserverId,
                    .callback = static_cast<void (*)(std::string, T)>(observer),
                });
            }
            else
            {
                eventObservers.push_back(EventDataFunctionObserverEntry{
                    .id = nextFunctionObserverId,
                    .callback = std::forward<Callable>(observer),
                });
            }

            return nextFunctionObserverId++;
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
         * Detaches a callable observer by id.
         *
         * This is the reliable way to detach capture lambdas.
         *
         * @param event The event to detach the observer from.
         * @param observerId The function observer id returned by attachObserver.
         */
        virtual void detachObserver(std::string event, FunctionObserverId observerId)
        {
            auto dataFunctionObserversIt = functionObservers.find(event);
            if (dataFunctionObserversIt != functionObservers.end())
            {
                auto &eventObservers = dataFunctionObserversIt->second;
                eventObservers.erase(std::remove_if(eventObservers.begin(), eventObservers.end(), [&](const DataFunctionObserverEntry &entry)
                                                    { return entry.id == observerId; }),
                                     eventObservers.end());
            }

            auto eventDataFunctionObserversIt = eventAwareFunctionObservers.find(event);
            if (eventDataFunctionObserversIt != eventAwareFunctionObservers.end())
            {
                auto &eventObservers = eventDataFunctionObserversIt->second;
                eventObservers.erase(std::remove_if(eventObservers.begin(), eventObservers.end(), [&](const EventDataFunctionObserverEntry &entry)
                                                    { return entry.id == observerId; }),
                                     eventObservers.end());
            }
        }

        /**
         * Detaches a data-only function pointer observer.
         *
         * @param event The event to detach the observer from.
         * @param observer The function pointer observer to detach.
         */
        virtual void detachObserver(std::string event, void (*observer)(T))
        {
            auto dataFunctionObserversIt = functionObservers.find(event);
            if (dataFunctionObserversIt == functionObservers.end())
            {
                return;
            }

            auto &eventObservers = dataFunctionObserversIt->second;
            eventObservers.erase(std::remove_if(eventObservers.begin(), eventObservers.end(), [&](const DataFunctionObserverEntry &entry)
                                                {
                                                    auto target = entry.callback.template target<void (*)(T)>();
                                                    return target != nullptr && *target == observer;
                                                }),
                                 eventObservers.end());
        }

        /**
         * Detaches an event-aware function pointer observer.
         *
         * @param event The event to detach the observer from.
         * @param observer The function pointer observer to detach.
         */
        virtual void detachObserver(std::string event, void (*observer)(std::string, T))
        {
            auto eventDataFunctionObserversIt = eventAwareFunctionObservers.find(event);
            if (eventDataFunctionObserversIt == eventAwareFunctionObservers.end())
            {
                return;
            }

            auto &eventObservers = eventDataFunctionObserversIt->second;
            eventObservers.erase(std::remove_if(eventObservers.begin(), eventObservers.end(), [&](const EventDataFunctionObserverEntry &entry)
                                                {
                                                    auto target = entry.callback.template target<void (*)(std::string, T)>();
                                                    return target != nullptr && *target == observer;
                                                }),
                                 eventObservers.end());
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
        struct DataFunctionObserverEntry
        {
            FunctionObserverId id;
            std::function<void(T)> callback;
        };

        struct EventDataFunctionObserverEntry
        {
            FunctionObserverId id;
            std::function<void(std::string, T)> callback;
        };

        std::unordered_map<std::string, std::vector<Observer<T> *>> observers;
        std::unordered_map<std::string, std::vector<DataFunctionObserverEntry>> functionObservers;
        std::unordered_map<std::string, std::vector<EventDataFunctionObserverEntry>> eventAwareFunctionObservers;
        FunctionObserverId nextFunctionObserverId = 1;
    };
}