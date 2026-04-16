#ifndef Y_MO_H
#define Y_MO_H


#include <mio/mmap.hpp>
#include <zstring_view.hpp>

namespace y::mo {
class Mo {
public:
    class Value {
    public:
        Value(const Mo* mo, ptrdiff_t index) : mo_(mo), index_(index) {
        }

        [[nodiscard]] auto index() const { return index_; }

        [[nodiscard]] zstring_view original() const {
            return mo_->original_string(index_);
        }

        [[nodiscard]] zstring_view translation() const {
            return mo_->translation_string(index_);
        }

    private:
        const Mo* mo_;
        ptrdiff_t index_;
    };

    class Iterator {
    public:
        Iterator(const Mo* mo, ptrdiff_t index) : mo_(mo), index_(index) {
        }

        Value operator*() const {
            return Value{mo_, index_};
        }

        Iterator& operator++() {
            ++index_;
            return *this;
        }

        auto operator<=>(const Iterator&) const = default;

    private:
        const Mo* mo_;
        ptrdiff_t index_;
    };

    explicit Mo(mio::mmap_source mmap);

    [[nodiscard]] uint32_t magic_number() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data());
    }

    [[nodiscard]] uint32_t format_revision() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + 4);
    }

    [[nodiscard]] size_t size() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + 8);
    }

    [[nodiscard]] uint32_t original_offset() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + 12);
    }

    [[nodiscard]] uint32_t translation_offset() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + 16);
    }

    [[nodiscard]] uint32_t hash_size() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + 20);
    }

    [[nodiscard]] uint32_t hash_offset() const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + 24);
    }

    [[nodiscard]] zstring_view original_string(ptrdiff_t index) const {
        auto* entry = mmap_.data() + original_offset() + index * 8;
        const uint32_t size = *reinterpret_cast<const uint32_t*>(entry);
        const uint32_t data_offset = *reinterpret_cast<const uint32_t*>(entry + 4);
        return {mmap_.data() + data_offset, size};
    }

    [[nodiscard]] zstring_view translation_string(ptrdiff_t index) const {
        auto* entry = mmap_.data() + translation_offset() + index * 8;
        const uint32_t size = *reinterpret_cast<const uint32_t*>(entry);
        const uint32_t data_offset = *reinterpret_cast<const uint32_t*>(entry + 4);
        return {mmap_.data() + data_offset, size};
    }

    [[nodiscard]] ptrdiff_t find_original_index(std::string_view str) const;

    static unsigned long hash_string(std::string_view str);

    [[nodiscard]] Iterator begin() const { return Iterator{this, 0}; }
    [[nodiscard]] Iterator end() const { return Iterator{this, static_cast<ptrdiff_t>(size())}; }

private:
    [[nodiscard]] ptrdiff_t original_index(const unsigned long hash_index) const {
        return *reinterpret_cast<const uint32_t*>(mmap_.data() + hash_offset() + hash_index * 4);
    }

    mio::mmap_source mmap_;
};
}

#endif // Y_MO_H
