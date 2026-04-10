#ifndef WENDFYR_PORTS_DRIVEN_I_CONFIG_SERVICE_HPP
#define WENDFYR_PORTS_DRIVEN_I_CONFIG_SERVICE_HPP

#include <cstdint>
#include <optional>
#include <string>

namespace wendfyr::ports::driven
{
    class IConfigService
    {
      public:
        virtual ~IConfigService();
        [[nodiscard]] virtual std::optional<std::string> getString(
            const std::string& key) const = 0;
        virtual void setString(const std::string& key, const std::string& value) = 0;

        [[nodiscard]] virtual int getInt(const std::string& key, int default_value) const
        {
            auto value{getString(key)};
            if (!value.has_value())
            {
                return default_value;
            }
            try
            {
                return std::stoi(value.value());
            }
            catch (const std::exception&)
            {
                return default_value;
            }
        }

        virtual void setInt(const std::string& key, int value)
        {
            setString(key, std::to_string(value));
        }

        [[nodiscard]] virtual bool setBool(const std::string& key, bool default_value) const
        {
            auto value = getString(key);
            if (!value.has_value())
            {
                return default_value;
            }

            return value.value() == "true" || value.value() == "1";
        }

        virtual void setBool(const std::string& key, bool value)
        {
            setString(key, value ? "true" : "false");
        }

        virtual void save() = 0;
        virtual void load() = 0;

        [[nodiscard]] virtual bool hasKey(const std::string& key) const = 0;

      protected:
        IConfigService() = default;
        IConfigService(const IConfigService&) = default;
        IConfigService(IConfigService&&) = default;
        IConfigService& operator=(const IConfigService&) = default;
        IConfigService& operator=(IConfigService&&) = default;
    };
};  // namespace wendfyr::ports::driven

#endif