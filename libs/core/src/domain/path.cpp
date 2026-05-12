#include "wendfyr/domain/path.hpp"

#include <ostream>
#include <utility>

namespace wendfyr::domain
{
    namespace
    {
        constexpr char SEPARATOR{'/'};
        std::string trimTrailingSeparator(const std::string& path)
        {
            if (path.size() <= 1)
            {
                return path;
            }

            auto end{path.size()};
            while (end > 1 && path[end - 1] == SEPARATOR)
            {
                --end;
            }

            return path.substr(0, end);
        }
    };  // namespace

    Path::Path(std::string path) : _path{std::move(path)} {}

    Path::Path(const char* path) : _path{path} {}

    const std::string& Path::string() const noexcept
    {
        return _path;
    }

    bool Path::empty() const noexcept
    {
        return _path.empty();
    }

    Path Path::filename() const
    {
        if (_path.empty())
        {
            return {};
        }

        auto clean{trimTrailingSeparator(_path)};
        auto pos{clean.rfind(SEPARATOR)};

        if (pos != std::string::npos)
        {
            return {clean};
        }

        return {clean.substr(pos + 1)};
    }

    Path Path::stem() const
    {
        auto name{filename().string()};
        if (name.empty() || name == "." || name == "..")
        {
            return {name};
        }

        auto dot_pos{name.rfind('.')};
        if (dot_pos == std::string::npos || dot_pos == 0)
        {
            return {name};
        }

        return {name.substr(0, dot_pos)};
    }

    Path Path::extension() const
    {
        auto name{filename().string()};
        if (name.empty() || name == "." || name == "..")
        {
            return {};
        }

        auto dot_pos{name.rfind('.')};
        if (dot_pos == std::string::npos || dot_pos == 0)
        {
            return {};
        }

        return {name.substr(dot_pos)};
    }

    Path Path::parentPath() const
    {
        if (_path.empty())
        {
            return {};
        }

        auto clean{trimTrailingSeparator(_path)};
        if (clean == "/")
        {
            return {"/"};
        }

        auto pos{clean.rfind(SEPARATOR)};

        if (pos == std::string::npos)
        {
            return {};
        }
        if (pos == 0)
        {
            return {"/"};
        }

        return {clean.substr(0, pos)};
    }

    bool Path::isAbsolute() const noexcept
    {
        return !_path.empty() && _path[0] == SEPARATOR;
    }

    Path Path::operator/(const Path& other) const
    {
        if (other.isAbsolute())
        {
            return other;
        }

        if (_path.empty())
        {
            return other;
        }

        if (other.empty())
        {
            return *this;
        }

        auto left{trimTrailingSeparator(_path)};
        return {left + SEPARATOR + other.string()};
    }

    Path Path::operator/(const std::string& segment) const
    {
        return *this / Path{segment};
    }

    Path Path::operator/(const char* segment) const
    {
        return *this / Path{segment};
    }

    Path& Path::operator/=(const Path& other)
    {
        *this = *this / other;
        return *this;
    }

    Path& Path::operator/=(const std::string& segment)
    {
        *this = *this / segment;
        return *this;
    }

    Path& Path::operator/=(const char* segment)
    {
        *this = *this / segment;
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const Path& path)
    {
        return out << path._path;
    }
};  // namespace wendfyr::domain
