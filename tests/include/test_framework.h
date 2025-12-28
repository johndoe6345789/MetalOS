#ifndef METALOS_TEST_FRAMEWORK_H
#define METALOS_TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple test framework for MetalOS
// Inspired by minimalism - no external dependencies

// Test statistics
typedef struct {
    int total;
    int passed;
    int failed;
} TestStats;

static TestStats test_stats = {0, 0, 0};

// Color codes for output
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"

// Test macros
#define TEST(name) \
    static void test_##name(void); \
    static void run_##name(void) { \
        printf(COLOR_BLUE "[ RUN  ] " COLOR_RESET #name "\n"); \
        test_stats.total++; \
        test_##name(); \
    } \
    static void test_##name(void)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            printf(COLOR_RED "[FAILED] " COLOR_RESET "%s:%d: Assertion failed: %s\n", \
                   __FILE__, __LINE__, #condition); \
            test_stats.failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            printf(COLOR_RED "[FAILED] " COLOR_RESET "%s:%d: Expected %lld, got %lld\n", \
                   __FILE__, __LINE__, (long long)(b), (long long)(a)); \
            test_stats.failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_NE(a, b) \
    do { \
        if ((a) == (b)) { \
            printf(COLOR_RED "[FAILED] " COLOR_RESET "%s:%d: Values should not be equal: %lld\n", \
                   __FILE__, __LINE__, (long long)(a)); \
            test_stats.failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_NULL(ptr) ASSERT_EQ(ptr, NULL)
#define ASSERT_NOT_NULL(ptr) ASSERT_NE(ptr, NULL)

#define ASSERT_STR_EQ(a, b) \
    do { \
        if (strcmp((a), (b)) != 0) { \
            printf(COLOR_RED "[FAILED] " COLOR_RESET "%s:%d: Strings not equal\n", \
                   __FILE__, __LINE__); \
            printf("  Expected: \"%s\"\n", (a)); \
            printf("  Got:      \"%s\"\n", (b)); \
            test_stats.failed++; \
            return; \
        } \
    } while(0)

#define TEST_PASS() \
    do { \
        test_stats.passed++; \
        printf(COLOR_GREEN "[ PASS ] " COLOR_RESET "\n"); \
    } while(0)

// Run all tests
#define RUN_TEST(name) run_##name()

// Initialize test framework
static inline void test_init(const char* suite_name) {
    printf("\n");
    printf(COLOR_BLUE "========================================\n" COLOR_RESET);
    printf(COLOR_BLUE "  MetalOS Test Suite: %s\n" COLOR_RESET, suite_name);
    printf(COLOR_BLUE "========================================\n" COLOR_RESET);
    test_stats.total = 0;
    test_stats.passed = 0;
    test_stats.failed = 0;
}

// Print test results
static inline int test_summary(void) {
    printf("\n");
    printf(COLOR_BLUE "========================================\n" COLOR_RESET);
    printf("  Total tests:  %d\n", test_stats.total);
    printf(COLOR_GREEN "  Passed:       %d\n" COLOR_RESET, test_stats.passed);
    if (test_stats.failed > 0) {
        printf(COLOR_RED "  Failed:       %d\n" COLOR_RESET, test_stats.failed);
    } else {
        printf("  Failed:       %d\n", test_stats.failed);
    }
    printf(COLOR_BLUE "========================================\n" COLOR_RESET);
    
    if (test_stats.failed == 0) {
        printf(COLOR_GREEN "\n✓ All tests passed!\n" COLOR_RESET);
        return 0;
    } else {
        printf(COLOR_RED "\n✗ Some tests failed!\n" COLOR_RESET);
        return 1;
    }
}

#endif // METALOS_TEST_FRAMEWORK_H
