/* test_compare256.cc -- compare256 unit tests
 * Copyright (C) 2022 Nathan Moinvaziri
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
#  include "zbuild.h"
#  include "zutil_p.h"
#  include "cpu_features.h"
}

#include <gtest/gtest.h>

#define MAX_COMPARE_SIZE (256)

/* Ensure that compare256 returns the correct match length */
static inline void compare256_match_check(compare256_func compare256) {
    int32_t match_len, i;
    uint8_t *str1;
    uint8_t *str2;

    str1 = (uint8_t *)zng_alloc(MAX_COMPARE_SIZE);
    ASSERT_TRUE(str1 != NULL);
    memset(str1, 'a', MAX_COMPARE_SIZE);

    str2 = (uint8_t *)zng_alloc(MAX_COMPARE_SIZE);
    ASSERT_TRUE(str2 != NULL);
    memset(str2, 'a', MAX_COMPARE_SIZE);

    for (i = 0; i <= MAX_COMPARE_SIZE; i++) {
        if (i < MAX_COMPARE_SIZE)
            str2[i] = 0;

        match_len = compare256(str1, str2);
        EXPECT_EQ(match_len, i);

        if (i < MAX_COMPARE_SIZE)
            str2[i] = 'a';
    }

    zng_free(str1);
    zng_free(str2);
}

#define TEST_COMPARE256(name, func, support_flag) \
    TEST(compare256, name) { \
        if (!support_flag) { \
            GTEST_SKIP(); \
            return; \
        } \
        compare256_match_check(func); \
    }

TEST_COMPARE256(c, compare256_c, 1)

#ifdef UNALIGNED_OK
TEST_COMPARE256(unaligned_16, compare256_unaligned_16, 1)
#ifdef HAVE_BUILTIN_CTZ
TEST_COMPARE256(unaligned_32, compare256_unaligned_32, 1)
#endif
#if defined(UNALIGNED64_OK) && defined(HAVE_BUILTIN_CTZLL)
TEST_COMPARE256(unaligned_64, compare256_unaligned_64, 1)
#endif
#endif

#if defined(X86_SSE2) && defined(HAVE_BUILTIN_CTZ)
TEST_COMPARE256(unaligned_sse2, compare256_unaligned_sse2, x86_cpu_has_sse2)
#endif
#if defined(X86_AVX2) && defined(HAVE_BUILTIN_CTZ)
TEST_COMPARE256(unaligned_avx2, compare256_unaligned_avx2, x86_cpu_has_avx2)
#endif
