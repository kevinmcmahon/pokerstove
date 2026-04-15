#include "CardDistribution.h"

#include <gtest/gtest.h>

using namespace pokerstove;

TEST(CardDistribution, DefaultConstructorCreatesRandomEmptyDistribution)
{
    CardDistribution dist;

    ASSERT_EQ(1u, dist.size());
    EXPECT_EQ(CardSet(), dist[0]);
    EXPECT_DOUBLE_EQ(1.0, dist[CardSet()]);
    EXPECT_DOUBLE_EQ(1.0, dist.weight());
}

TEST(CardDistribution, ParseDotCreatesSpecialEmptyDistribution)
{
    CardDistribution dist;

    ASSERT_TRUE(dist.parse("."));
    ASSERT_EQ(1u, dist.size());
    EXPECT_EQ(CardSet(), dist[0]);
    EXPECT_DOUBLE_EQ(1.0, dist[CardSet()]);
}

TEST(CardDistribution, ParseWeightedHands)
{
    CardDistribution dist;

    ASSERT_TRUE(dist.parse("AcAs=0.25,KhQh=0.75"));
    ASSERT_EQ(2u, dist.size());
    EXPECT_DOUBLE_EQ(0.25, dist[CardSet("AcAs")]);
    EXPECT_DOUBLE_EQ(0.75, dist[CardSet("KhQh")]);
    EXPECT_DOUBLE_EQ(1.0, dist.weight());
    EXPECT_EQ("AcAs=0.250,KhQh=0.750", dist.str());
}

TEST(CardDistribution, ParseRejectsInvalidInput)
{
    CardDistribution dist;

    EXPECT_FALSE(dist.parse("AcAs="));
    EXPECT_FALSE(dist.parse("AcA"));
    EXPECT_FALSE(dist.parse("AcAc"));
    EXPECT_FALSE(dist.parse("Xx"));
}

TEST(CardDistribution, FillGeneratesAllCombinations)
{
    CardDistribution dist;

    dist.fill(CardSet("AcKdQs"), 2);

    ASSERT_EQ(3u, dist.size());
    EXPECT_DOUBLE_EQ(1.0, dist[CardSet("AcKd")]);
    EXPECT_DOUBLE_EQ(1.0, dist[CardSet("AcQs")]);
    EXPECT_DOUBLE_EQ(1.0, dist[CardSet("KdQs")]);
    EXPECT_DOUBLE_EQ(3.0, dist.weight());
}

TEST(CardDistribution, RemoveCardsZerosOnlyIntersectingHands)
{
    CardDistribution dist;
    ASSERT_TRUE(dist.parse("AcAs=0.25,KdKh=0.75,QcQh=1.0"));

    dist.removeCards(CardSet("AsQh"));

    EXPECT_DOUBLE_EQ(0.0, dist[CardSet("AcAs")]);
    EXPECT_DOUBLE_EQ(0.75, dist[CardSet("KdKh")]);
    EXPECT_DOUBLE_EQ(0.0, dist[CardSet("QcQh")]);
    EXPECT_DOUBLE_EQ(0.75, dist.weight());
}

TEST(CardDistribution, IndexOutOfBoundsThrows)
{
    CardDistribution dist(CardSet("AcAs"));

    EXPECT_THROW(dist[1], std::runtime_error);
}

TEST(CardDistribution, MissingHandLookupReturnsZero)
{
    CardDistribution dist(CardSet("AcAs"));

    EXPECT_DOUBLE_EQ(0.0, dist[CardSet("KhQh")]);
}
