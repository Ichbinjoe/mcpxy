#ifndef MCPXY_TYPESTEST_H
#define MCPXY_TYPESTEST_H

#include "proto/Types.h"

#include <gtest/gtest.h>

namespace mcpxy::types {
    using namespace std;

    TEST(BoolTypesTest, Get) {
        dataspace<1> f{0x00};
        EXPECT_FALSE(boolOf(f));
        dataspace<1> t{0x01};
        EXPECT_TRUE(boolOf(t));
    }

    TEST(BoolTypesTest, Set) {
        dataspace<1> d{0xff};
        setBool(d, false);
        EXPECT_EQ(0x00, d[0]);
        setBool(d, true);
        EXPECT_EQ(0x01, d[0]);
    }
}

#endif //MCPXY_TYPESTEST_H
