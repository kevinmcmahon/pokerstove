#include "combinations.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <vector>

using namespace pokerstove;

namespace
{

std::vector<size_t> snapshot(const combinations& cards)
{
    return std::vector<size_t>(cards.begin(), cards.end());
}

}  // namespace

TEST(Combinations, holdem)
{
    // if we run though all the 2 card combos we get 52c2 hands
    size_t count = 0;
    combinations cards(52, 2);
    do {
        count += 1;
    } while (cards.next());

    EXPECT_EQ(count, static_cast<size_t>(choose(52, 2)));
}

TEST(Combinations, ChooseHandlesOutOfRangeAndEdgeCases)
{
    EXPECT_EQ(1u, static_cast<size_t>(choose(5, 0)));
    EXPECT_EQ(5u, static_cast<size_t>(choose(5, 1)));
    EXPECT_EQ(10u, static_cast<size_t>(choose(5, 2)));
    EXPECT_EQ(0u, static_cast<size_t>(choose(4, 5)));
}

TEST(Combinations, ZeroCardCombinationsEnumerateExactlyOnce)
{
    combinations cards(4, 0);
    size_t count = 1;

    while (cards.next())
        ++count;

    EXPECT_EQ(1u, count);
}

TEST(Combinations, EnumeratesUniqueOrderedCombinations)
{
    combinations cards(5, 3);
    std::set<std::vector<size_t>> seen;

    do
    {
        const std::vector<size_t> current = snapshot(cards);
        EXPECT_TRUE(std::is_sorted(current.begin(), current.end()));
        EXPECT_TRUE(seen.insert(current).second);
    } while (cards.next());

    EXPECT_EQ(10u, seen.size());
}

TEST(Combinations, GetMaskReflectsCurrentCombination)
{
    combinations cards(5, 3);

    EXPECT_EQ(UINT64_C(0x7), cards.getMask());
    ASSERT_TRUE(cards.next());
    EXPECT_EQ(UINT64_C(0xB), cards.getMask());
}

TEST(Combinations, ResetRestoresInitialCombination)
{
    combinations cards(5, 3);

    ASSERT_TRUE(cards.next());
    ASSERT_TRUE(cards.next());
    cards.reset();

    EXPECT_EQ((std::vector<size_t>{0, 1, 2}), snapshot(cards));
}
