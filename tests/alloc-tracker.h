#pragma once

#include <concepts>
#include <stdint.h>
#include <gtest/gtest.h>

size_t get_alloc_count();

void assert_no_alloc(std::invocable auto&& fn) {
    size_t before = get_alloc_count();

    fn();

    size_t after = get_alloc_count();
    EXPECT_EQ(before, after) << "Dynamic memory allocated detected";
}
