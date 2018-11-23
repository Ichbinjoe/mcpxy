#ifndef MCPXY_TYPESTEST_H
#define MCPXY_TYPESTEST_H

#include "proto/Types.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

    template<typename t, size_t s>
    t varintOf(initializer_list<int> v) {
        dataspace<s> ds;
        size_t p = 0;
        for (auto &val : v) {
            ds[p++] = (uint8_t)val;
        }
        t ty;
        ssize_t r = readVarint<t>(*(dataspace<0>*)ds, s, ty);
        if (r < 0)
            throw runtime_error("returned -1");

        return ty;
    }

    TEST(VarintTypesTest, GetInt) {
        EXPECT_EQ(0, (varintOf<int32_t, 5>({0x00})));
        EXPECT_EQ(1, (varintOf<int32_t, 5>({0x01})));
        EXPECT_EQ(2, (varintOf<int32_t, 5>({0x02})));
        EXPECT_EQ(127, (varintOf<int32_t, 5>({0x7f})));
        EXPECT_EQ(128, (varintOf<int32_t, 5>({0x80, 0x01})));
        EXPECT_EQ(255, (varintOf<int32_t, 5>({0xff, 0x01})));
        EXPECT_EQ(2147483647, (varintOf<int32_t, 5>({0xff, 0xff, 0xff, 0xff, 0x07})));
        EXPECT_EQ(-1, (varintOf<int32_t, 5>({0xff, 0xff, 0xff, 0xff, 0x0f})));
        EXPECT_EQ(-2147483648, (varintOf<int32_t, 5>({0x80, 0x80, 0x80, 0x80, 0x08})));
    }

    TEST(VarintTypesTest, GetLong) {
        EXPECT_EQ(0, (varintOf<int64_t, 10>({0x00})));
        EXPECT_EQ(1, (varintOf<int64_t, 10>({0x01})));
        EXPECT_EQ(2, (varintOf<int64_t, 10>({0x02})));
        EXPECT_EQ(127, (varintOf<int64_t, 10>({0x7f})));
        EXPECT_EQ(128, (varintOf<int64_t, 10>({0x80, 0x01})));
        EXPECT_EQ(255, (varintOf<int64_t, 10>({0xff, 0x01})));
        EXPECT_EQ(2147483647, (varintOf<int64_t, 10>({0xff, 0xff, 0xff, 0xff, 0x07})));
        EXPECT_EQ(-1, (varintOf<int64_t, 10>({0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01})));
        EXPECT_EQ(-2147483648, (varintOf<int64_t, 10>({0x80, 0x80, 0x80, 0x80, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x01})));
        EXPECT_EQ(-9223372036854775808, (varintOf<int64_t, 10>({0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01})));
        EXPECT_EQ(9223372036854775807, (varintOf<int64_t, 10>({0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})));
    }

    TEST(VarintTypesTest, SetInt) {
        uint8_t w[5];

        EXPECT_EQ(1, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)0)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x00, (int)w[1], (int)w[2], (int)w[3], (int)w[4]}));

        EXPECT_EQ(1, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)1)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x01, (int)w[1], (int)w[2], (int)w[3], (int)w[4]}));

        EXPECT_EQ(1, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)2)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x02, (int)w[1], (int)w[2], (int)w[3], (int)w[4]}));

        EXPECT_EQ(1, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)127)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x7f, (int)w[1], (int)w[2], (int)w[3], (int)w[4]}));

        EXPECT_EQ(2, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)128)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x80, 0x01, (int)w[2], (int)w[3], (int)w[4]}));

        EXPECT_EQ(2, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)255)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0x01, (int)w[2], (int)w[3], (int)w[4]}));

        EXPECT_EQ(5, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)2147483647)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0xff, 0xff, 0xff, 0x07}));

        EXPECT_EQ(5, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)-1)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0xff, 0xff, 0xff, 0x0f}));

        EXPECT_EQ(5, (writeVarint<int32_t>(*(dataspace<0>*)&w, (ssize_t)5, (int32_t)-2147483648)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x80, 0x80, 0x80, 0x80, 0x08}));
    }

    TEST(VarintTypesTest, SetLong) {
        uint8_t w[10];

        EXPECT_EQ(1, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)0)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x00, (int)w[1], (int)w[2], (int)w[3], (int)w[4], (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(1, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)1)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x01, (int)w[1], (int)w[2], (int)w[3], (int)w[4], (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(1, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)2)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x02, (int)w[1], (int)w[2], (int)w[3], (int)w[4], (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(1, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)127)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x7f, (int)w[1], (int)w[2], (int)w[3], (int)w[4], (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(2, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)128)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x80, 0x01, (int)w[2], (int)w[3], (int)w[4], (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(2, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)255)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0x01, (int)w[2], (int)w[3], (int)w[4], (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(5, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)2147483647)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0xff, 0xff, 0xff, 0x07, (int)w[5], (int)w[6], (int)w[7], (int)w[8], (int)w[9]}));

        EXPECT_EQ(10, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)-1)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01}));

        EXPECT_EQ(10, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)-2147483648)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x80, 0x80, 0x80, 0x80, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x01}));

        EXPECT_EQ(10, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)-9223372036854775808)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01}));

        EXPECT_EQ(9, (writeVarint<int64_t>(*(dataspace<0>*)&w, (ssize_t)10, (int64_t)9223372036854775807)));
        EXPECT_THAT(w, ::testing::ElementsAreArray({0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, (int)w[9]}));
    }


}

#endif //MCPXY_TYPESTEST_H
