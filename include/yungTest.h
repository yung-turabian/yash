#ifndef UNITTEST_H
#define UNITTEST_H

#include <stdio.h>
#include <string.h>

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            fprintf(stderr, "Assertion failed: %s == %s, file %s, line %d\n", #expected, #actual, __FILE__, __LINE__); \
            return 1; \
        } \
    } while (0)

#define ASSERT_STR_EQ(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            fprintf(stderr, "Assertion failed: %s == %s, file %s, line %d\n", #expected, #actual, __FILE__, __LINE__); \
            return 1; \
        } \
    } while (0)

#define RUN_TEST(test) \
    do { \
        fprintf(stderr, "Running %s... ", #test); \
        if (test()) { \
            fprintf(stderr, "FAIL\n"); \
        } else { \
            fprintf(stderr, "PASS\n"); \
        } \
    } while (0)

#endif
