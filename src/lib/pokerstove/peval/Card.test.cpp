#include "Card.h"
#include "Rank.h"
#include "Suit.h"

#include <gtest/gtest.h>

using namespace pokerstove;

TEST(CardTest, ConstructorsExposeRankSuitAndCode)
{
    Card c1("Ac");
    const Card c2(Rank::Ace(), Suit::Clubs());
    const Card c3(12);

    EXPECT_EQ("Ac", c1.str());
    EXPECT_EQ(Rank::Ace(), c1.rank());
    EXPECT_EQ(Suit::Clubs(), c1.suit());
    EXPECT_EQ(c1, c2);
    EXPECT_EQ(c1, c3);
    EXPECT_EQ(c2.code(), c3.code());
}

TEST(CardTest, FromStringRejectsInvalidInputWithoutMutatingCard)
{
    Card card("As");

    EXPECT_FALSE(card.fromString("Xx"));
    EXPECT_EQ("As", card.str());
}
