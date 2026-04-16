#pragma once

#include <string>
#include <string_view>


class zstring_view : public std::string_view {
public:
    zstring_view() = delete;
    zstring_view(const zstring_view& str) = default;
 
    constexpr zstring_view(const char* str) noexcept : std::string_view{str} {}
    zstring_view(const std::string& str) noexcept : std::string_view{str} {}
    zstring_view(const char* str, const size_t size) : std::string_view{str, size} {
        if (str[size] != '\0') {
            throw std::runtime_error("`str` should be null terminated at size");
        }
    }
 
    zstring_view& operator=(std::string_view) = delete;
    zstring_view& operator=(const zstring_view&) = default;
 
    void remove_suffix(std::size_t) = delete;
    void swap(std::string_view&) = delete;
    void swap(zstring_view& other) noexcept { std::string_view::swap(other); }
 
    [[nodiscard]] constexpr const char* c_str() const noexcept { return std::string_view::data(); }
};
