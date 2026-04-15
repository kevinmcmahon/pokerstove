#include "Odometer.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(Odometer, GeneratesDocumentedSequence)
{
    Odometer odometer({2, 4, 3});
    const std::vector<std::string> expected = {
        "000", "001", "002", "010", "011", "012",
        "020", "021", "022", "030", "031", "032",
        "100", "101", "102", "110", "111", "112",
        "120", "121", "122", "130", "131", "132",
    };

    std::vector<std::string> actual;
    actual.push_back(odometer.str());
    while (odometer.next())
        actual.push_back(odometer.str());

    EXPECT_EQ(expected, actual);
}

TEST(Odometer, NextStopsAfterLastTuple)
{
    Odometer odometer({1, 1});

    EXPECT_EQ("00", odometer.str());
    EXPECT_FALSE(odometer.next());
}

TEST(Odometer, StrReflectsCurrentState)
{
    Odometer odometer({2, 2});

    EXPECT_EQ("00", odometer.str());
    ASSERT_TRUE(odometer.next());
    EXPECT_EQ("01", odometer.str());
    ASSERT_TRUE(odometer.next());
    EXPECT_EQ("10", odometer.str());
}

TEST(Odometer, SingletonExtentStartsAtZeroAndEndsImmediately)
{
    Odometer odometer({1});

    EXPECT_EQ(1u, odometer.size());
    EXPECT_EQ("0", odometer.str());
    EXPECT_FALSE(odometer.next());
}
