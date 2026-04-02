#include "wendfyr/services/event_bus.hpp"

#include <algorithm>

namespace wendfyr::services
{
    SubscriptionId EventBus::subscribe(EventCallback callback)
    {
        _subscribers.push_back(Subscription{id, std::move(callback)});
        return _next_id++;
    }

    void EventBus::unsubscribe(SubscriptionId id)
    {
        auto it{std::remove_if(_subscribers.begin(), _subscribers.end(),
                               [](const Subscription& sub) { return sub.id == id; })};

        _subscribers.erase(it, _subscribers.end());
    }

    void EventBus::publish(const domain::events::Event& event)
    {
        for (const auto& sub : _subscribers)
        {
            sub.callback(event);
        }
    }

    std::size_t subscriberCount() const noexcept
    {
        return _subsribers.size();
    }
};  // namespace wendfyr::services