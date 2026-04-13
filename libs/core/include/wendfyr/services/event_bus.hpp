#ifndef WENDFYR_SERVICES_EVENT_BUS_HPP
#define WENDFYR_SERVICES_EVENT_BUS_HPP

#include "wendfyr/domain/events/event.hpp"

#include <cstddef>
#include <functional>
#include <vector>

namespace wendfyr::services
{
    using SubscriptionId = std::size_t;
    using EventCallback = std::function<void(const domain::events::Event&)>;

    class EventBus
    {
      public:
        SubscriptionId subscribe(EventCallback callback);
        void unsubscribe(SubscriptionId);
        void publish(const domain::events::Event& event) const;

        [[nodiscard]] std::size_t subscriberCount() const noexcept;

      private:
        struct Subscription
        {
            SubscriptionId id;
            EventCallback callback;
        };

        std::vector<Subscription> _subscribers;
        SubscriptionId _next_id{0};
    };
}  // namespace wendfyr::services

#endif