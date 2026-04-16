#include <filesystem>
#include <gtest/gtest.h>
#include <y/mo.h>

#include "alloc-tracker.h"

TEST(Mo, MagicNumber) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    assert_no_alloc(
        [&] {
            EXPECT_EQ(mo.magic_number(), 0x950412de);
        });
}

TEST(Mo, FormatRevision) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    assert_no_alloc(
        [&] {
            EXPECT_EQ(mo.format_revision(), 0);
        });
}

TEST(Mo, HashCount) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    assert_no_alloc(
        [&] {
            EXPECT_GE(mo.hash_size(), mo.size());
        });
}

TEST(Mo, FindOriginal) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    assert_no_alloc(
        [&] {
            for (const auto& entry : mo) {
                EXPECT_EQ(mo.find_original_index(entry.original()), entry.index());
            }
        });
}

TEST(Mo, Iterator) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    assert_no_alloc(
        [&] {
            for (const auto& item : mo) {
                ASSERT_EQ(mo.find_original_index(item.original()), item.index());
                ASSERT_EQ(mo.original_string(item.index()), item.original());
                ASSERT_EQ(mo.translation_string(item.index()), item.translation());
            }
        });
}
