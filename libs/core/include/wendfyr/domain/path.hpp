#ifndef WENDFYR_DOMAN_PATH_HPP
#define WENDFYR_DOMAIN_PATH_HPP

#include <string>

namespace wendfyr::domain
{
    class Path
    {
      public:
        Path() = default;
        Path(std::string path);
        Path(const char* path);

        [[nodiscard]] const std::string& string() const noexcept;
        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] Path filename() const;
        [[nodiscard]] Path stem() const;
        [[nodiscard]] Path extension() const;
        [[nodiscard]] Path parentPath() const;
        [[nodiscard]] bool isAbsolute() const noexcept;

        [[nodiscard]] Path operator/(const Path& other) const;
        [[nodiscard]] Path operator/(const std::string& segment) const;
        [[nodiscard]] Path operator/(const char* segment) const;

        Path& operator/=(const Path& other);
        Path& operator/=(const std::string& segment);
        Path& operator/=(const char* segment);

        [[nodiscard]] auto operator<=>(const Path& other) const noexcept =
            default;
        [[nodiscard]] bool operator==(const Path& other) const noexcept =
            default;

        friend std::ostream& operator<<(std::ostream& out,
                                        const Path& path);

      private:
        std::string _path;
    };
};  // namespace wendfyr::domain

template <>
struct std::hash<wendfyr::domain::Path>
{
    std::size_t operator()(
        const wendfyr::domain::Path& path) const noexcept
    {
        return std::hash<std::string>{}(path.string());
    }
};
#endif
