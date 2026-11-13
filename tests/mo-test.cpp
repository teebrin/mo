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

TEST(Mo, Translate) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/django.mo"}};
    const auto entry = *mo.begin();
    bool called = false;
    const auto fallback = [&] { called = true; return std::string{"fallback"}; };
    static_assert(std::same_as<decltype(mo.translate("missing", fallback)), std::string>);
    EXPECT_EQ(mo.translate(entry.original(), fallback), entry.translation());
    EXPECT_FALSE(called);
    EXPECT_EQ(mo.translate("__missing_translation__", fallback), "fallback");
    EXPECT_TRUE(called);
    EXPECT_EQ(mo.translate("__missing_translation__"), "__missing_translation__");
    EXPECT_EQ(mo.translate("__missing_context__", "__missing_translation__"), "__missing_translation__");
    EXPECT_EQ(mo.translate("__missing_context__", "__missing_translation__", fallback), "fallback");
    const y::mo::Mo contextual{mio::mmap_source{"../../tests/samples/context.mo"}};
    EXPECT_EQ(contextual.translate("menu", "Open"), "Ouvrir");
    EXPECT_EQ(contextual.translate("menu", "Open", fallback), "Ouvrir");
}

TEST(Mo, TranslateOptionalFallback) {
    const y::mo::Mo mo{mio::mmap_source{"../../tests/samples/context.mo"}};
    const auto fallback = []() -> std::optional<zstring_view> { return std::nullopt; };
    static_assert(std::same_as<decltype(mo.translate("missing", fallback)), std::optional<zstring_view>>);
    EXPECT_EQ(mo.translate("missing", fallback), std::nullopt);
    EXPECT_EQ(mo.translate("missing", y::mo::nullopt_fallback), std::nullopt);
    EXPECT_EQ(mo.translate("other", "Open", fallback), std::nullopt);
    const auto translated = mo.translate("menu", "Open", fallback);
    ASSERT_TRUE(translated.has_value());
    EXPECT_EQ(*translated, "Ouvrir");
    const auto built_in = mo.translate("menu", "Open", y::mo::nullopt_fallback);
    ASSERT_TRUE(built_in.has_value());
    EXPECT_EQ(*built_in, "Ouvrir");
}
