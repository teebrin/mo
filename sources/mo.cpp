#include "y/mo.h"

#include <iostream>

namespace y::mo {
Mo::Mo(mio::mmap_source mmap) : mmap_(std::move(mmap)) {
    if (magic_number() != 0x950412de) {
        throw std::runtime_error(
            std::format("Not an MO file: expecting 0x950412de magic number, got 0x{:x}", magic_number()));
    }
    if (format_revision() != 0) {
        throw std::runtime_error(
            std::format("Expecting format_revision 0, got {}", format_revision()));
    }
    if (hash_size() < 2) {
        throw std::runtime_error("Only hash based files are supported");
    }
}

ptrdiff_t Mo::find_original_index(const std::string_view context, const std::string_view original) const {
    const auto hash = hash_string(context, original);
    const auto increment = 1 + hash % (hash_size() - 2);
    auto limit = hash_size();
    const auto matches = [&](const std::string_view stored) {
        if (context.empty()) return stored == original;
        return stored.size() == context.size() + 1 + original.size() &&
               stored.starts_with(context) &&
               stored[context.size()] == '\x04' &&
               stored.substr(context.size() + 1) == original;
    };
    for (auto hash_index = hash % hash_size();
         const auto string_index_plus_one = original_index(hash_index);
         hash_index = (hash_index + increment) % hash_size()) {
        if (auto string_index = string_index_plus_one - 1; matches(unchecked_original_string(string_index))) {
            return string_index;
        }
        if (--limit == 0) { break; }
    }
    return -1;
}

unsigned long Mo::hash_string(const std::string_view context, const std::string_view original) {
    unsigned long hash = 0;
    const auto hash_char = [&hash](const char c) {
        hash <<= 4;
        hash += static_cast<unsigned char>(c);
        const unsigned long g = hash & 0xf0000000;
        if (g != 0) {
            hash ^= g >> 24;
            hash ^= g;
        }
    };
    if (!context.empty()) {
        std::ranges::for_each(context, hash_char);
        hash_char('\x04');
    }
    std::ranges::for_each(original, hash_char);
    return hash;
}
}
