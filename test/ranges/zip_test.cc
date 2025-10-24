#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <nsl/ranges/zip.hh>
#include <vector>

TEST(zip_test, array_and_vector) {
    std::array int_array {1, 2, 3};
    std::vector float_vector {-1.0f, -2.0f, -3.0f};

    int expected = 1;
    for (auto [arr, vec] : nsl::zip(int_array, float_vector)) {
        EXPECT_EQ(arr, expected) << "Unexpected array element value";
        EXPECT_EQ(vec, -expected) << "Unexpected vector element value";
        expected++;
    }
}

TEST(zip_test, empty_zip) {
    for (auto it : nsl::zip()) {
        // Should be nothing to iterate over
        EXPECT_EQ(true, false) << "Invalid iteration over empty zip";
    }
}

TEST(zip_test, mismatched_sizes) {
    std::array int_array {1, 2};
    std::vector int_vector {1, 2, 3, 4};

    int index = 0;
    for (auto [arr, vec] : nsl::zip(int_array, int_vector)) {
        EXPECT_EQ(arr, vec) << "Unexpected mismatched values";
        index++;
    }
    EXPECT_EQ(index, std::min(int_array.size(), int_vector.size())) << "Iteration past bounds";
}

TEST(zip_test, operators) {
    std::array int_array {1, 2, 3, 4, 5, 6};
    std::vector int_vector {1, 2, 3, 4, 5};
    auto iterator = nsl::zip(int_array, int_vector).begin();

    EXPECT_EQ(*(++iterator), std::make_tuple(2, 2));
    EXPECT_EQ(*iterator, std::make_tuple(2, 2));

    EXPECT_EQ(*(iterator++), std::make_tuple(2, 2));
    EXPECT_EQ(*iterator, std::make_tuple(3, 3));

    EXPECT_EQ(*(--iterator), std::make_tuple(2, 2));
    EXPECT_EQ(*iterator, std::make_tuple(2, 2));

    EXPECT_EQ(*(iterator--), std::make_tuple(2, 2));
    EXPECT_EQ(*iterator, std::make_tuple(1, 1));

    EXPECT_EQ(iterator[3], std::make_tuple(4, 4));
}
