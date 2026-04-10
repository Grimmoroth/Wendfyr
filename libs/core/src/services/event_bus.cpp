#include "wendfyr/services/event_bus.hpp"

#include <algorithm>

namespace wendfyr::services
{
    SubscriptionId EventBus::subscribe(EventCallback callback)
    {
        auto id = _next_id++;
        _subscribers.push_back(Subscription{id, std::move(callback)});
        return id;
    }

    void EventBus::unsubscribe(SubscriptionId id)
    {
        auto it{std::remove_if(_subscribers.begin(), _subscribers.end(),
                               [id](const Subscription& sub) { return sub.id == id; })};

        _subscribers.erase(it, _subscribers.end());
    }

    void EventBus::publish(const domain::events::Event& event) const
    {
        for (const auto& sub : _subscribers)
        {
            sub.callback(event);
        }
    }

    std::size_t EventBus::subscriberCount() const noexcept
    {
        return _subscribers.size();
    }
};  // namespace wendfyr::services