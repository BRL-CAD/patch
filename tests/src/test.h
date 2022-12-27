// SPDX-License-Identifier: BSD-3-Clause
// Copyright 2022 Shannon Booth <shannon.ml.booth@gmail.com>

#pragma once

#include <functional>
#include <iostream>
#include <patch/file.h>
#include <stdexcept>
#include <string>

#define EXPECT_TRUE(condition)                            \
    do {                                                  \
        if (!(condition)) {                               \
            std::cerr << "FAIL: condition is not true\n"; \
            throw std::runtime_error("Test failed");      \
        }                                                 \
    } while (false)

#define EXPECT_FALSE(condition)                            \
    do {                                                   \
        if ((condition)) {                                 \
            std::cerr << "FAIL: condition is not false\n"; \
            throw std::runtime_error("Test failed");       \
        }                                                  \
    } while (false)

#define EXPECT_NE(lhs, rhs)                                        \
    do {                                                           \
        /* NOLINTNEXTLINE(readability-container-size-empty) */     \
        if ((lhs) == (rhs)) {                                      \
            std::cerr << "FAIL: " << lhs << " == " << rhs << '\n'; \
            throw std::runtime_error("Test failed");               \
        }                                                          \
    } while (false)

#define EXPECT_EQ(lhs, rhs)                                        \
    do {                                                           \
        /* NOLINTNEXTLINE(readability-container-size-empty) */     \
        if ((lhs) != (rhs)) {                                      \
            std::cerr << "FAIL: " << lhs << " != " << rhs << '\n'; \
            throw std::runtime_error("Test failed");               \
        }                                                          \
    } while (false)

#define EXPECT_FILE_EQ(file, rhs)                                                         \
    do {                                                                                  \
        const auto file_data = Patch::File(file, std::ios_base::in).read_all_as_string(); \
        EXPECT_EQ(file_data, rhs);                                                        \
    } while (false)

#define EXPECT_FILE_BINARY_EQ(file, rhs)                                                                          \
    do {                                                                                                          \
        const auto file_data = Patch::File(file, std::ios_base::in | std::ios_base::binary).read_all_as_string(); \
        EXPECT_EQ(file_data, rhs);                                                                                \
    } while (false)

void register_test(std::string name, const std::function<void()>& test);
void register_test(std::string name, std::function<void(const char*)> test);

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_FUNCTION_NAME(name) test_##name

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_REGISTER_HELPER(name) HelperForRegisteringTest_##name

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PATCH_TEST(name)                                                \
    static void TEST_FUNCTION_NAME(name)(const char* patch_path);       \
    struct TEST_REGISTER_HELPER(name) {                                 \
        TEST_REGISTER_HELPER(name)                                      \
        ()                                                              \
        {                                                               \
            register_test(#name, TEST_FUNCTION_NAME(name));             \
        }                                                               \
    };                                                                  \
    static const TEST_REGISTER_HELPER(name) TEST_REGISTER_HELPER(name); \
    static void TEST_FUNCTION_NAME(name)(const char* patch_path)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST(name)                                                      \
    static void TEST_FUNCTION_NAME(name)();                             \
    struct TEST_REGISTER_HELPER(name) {                                 \
        TEST_REGISTER_HELPER(name)                                      \
        ()                                                              \
        {                                                               \
            register_test(#name, TEST_FUNCTION_NAME(name));             \
        }                                                               \
    };                                                                  \
    static const TEST_REGISTER_HELPER(name) TEST_REGISTER_HELPER(name); \
    static void TEST_FUNCTION_NAME(name)()
