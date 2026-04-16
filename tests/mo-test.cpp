#include <filesystem>
#include <gtest/gtest.h>
#include <y/mo.h>

TEST(Mo, MagicNumber) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    EXPECT_EQ(mo.magic_number(), 0x950412de);
}

TEST(Mo, FormatRevision) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    EXPECT_EQ(mo.format_revision(), 0);
}

TEST(Mo, HashCount) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    EXPECT_GE(mo.hash_size(), mo.size());
}

TEST(Mo, FindOriginal) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    for (ptrdiff_t i = 0; i < mo.size(); ++i) {
        EXPECT_EQ(mo.find_original_index(mo.original_string(i)), i);
    }
}

TEST(Mo, OriginalString) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    EXPECT_EQ(mo.original_string(0), "");
}

TEST(Mo, Iterator) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};

    for (const auto& item : mo) {
        ASSERT_EQ(mo.find_original_index(item.original()), item.index());
        ASSERT_EQ(mo.translation_string(item.index()), item.translation());
        ASSERT_EQ(mo.original_string(item.index()), item.original());
    }
}
