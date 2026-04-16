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

ptrdiff_t Mo::find_original_index(const std::string_view str) const {
    const auto hash = hash_string(str);
    const auto increment = 1 + hash % (hash_size() - 2);
    auto limit = hash_size();
    for (auto hash_index = hash % hash_size();
         const auto string_index_plus_one = original_index(hash_index);
         hash_index = (hash_index + increment) % hash_size()) {
        if (auto string_index = string_index_plus_one - 1; original_string(string_index) == str) {
            return string_index;
        }
        if (--limit == 0) { break; }
    }
    throw std::out_of_range("original string not found");
}

unsigned long Mo::hash_string(const std::string_view str) {
    unsigned long hash = 0;
    for (const auto c : str) {
        hash <<= 4;
        hash += static_cast<unsigned char>(c);
        const unsigned long g = hash & 0xf0000000;
        if (g != 0) {
            hash ^= g >> 24;
            hash ^= g;
        }
    }
    return hash;
}
}
