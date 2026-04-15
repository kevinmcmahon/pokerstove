#include "SimpleDeck.hpp"

#include <gtest/gtest.h>

using namespace pokerstove;
using namespace std;

TEST(SimpleDeck, DefaultDeckStartsOrderedAndFull)
{
    SimpleDeck deck;

    EXPECT_EQ(52, deck.size());
    EXPECT_EQ(CardSet("2c"), deck[0]);
    EXPECT_EQ(CardSet("As"), deck[51]);
    EXPECT_EQ(CardSet(), deck.dead());
}

TEST(SimpleDeck, remove_card)
{
    SimpleDeck d;
    EXPECT_EQ(d.size(), 52);

    CardSet clubThree = CardSet("3c");
    CardSet clubAce = CardSet("Ac");
    CardSet fullDeck;

    d.remove(clubThree);
    EXPECT_EQ(d.size(), 51);
    EXPECT_EQ(d.dead(), clubThree);

    d.remove(clubAce);
    EXPECT_EQ(d.size(), 50);
    EXPECT_EQ(d.dead().size(), 2);
    EXPECT_EQ(d.dead(), clubThree | clubAce);
    EXPECT_FALSE(d.deal(50).contains(clubAce));
}

TEST(SimpleDeck, shuffle_preserves_card_set_and_resets_boundary)
{
    SimpleDeck deck;
    deck.remove(CardSet("AcKd"));
    deck.deal(5);

    deck.shuffle();

    CardSet all;
    all.fill();

    EXPECT_EQ(52, deck.size());
    EXPECT_EQ(CardSet(), deck.dead());
    EXPECT_EQ(all, deck.deal(52));
}

TEST(SimpleDeck, deal_too_many_cards_throws)
{
    SimpleDeck deck;

    EXPECT_THROW(deck.deal(53), std::out_of_range);
    EXPECT_EQ(52, deck.size());
}

TEST(SimpleDeck, deal_zero_cards_returns_empty_set)
{
    SimpleDeck deck;

    EXPECT_EQ(CardSet(), deck.deal(0));
    EXPECT_EQ(52, deck.size());
}

TEST(SimpleDeck, deal_all_cards_exhausts_the_deck)
{
    SimpleDeck deck;
    CardSet all;
    all.fill();

    EXPECT_EQ(all, deck.deal(52));
    EXPECT_EQ(0, deck.size());
}

TEST(SimpleDeck, deal_after_exhaustion_throws)
{
    SimpleDeck deck;
    deck.deal(52);

    EXPECT_THROW(deck.deal(1), std::out_of_range);
}

TEST(SimpleDeck, reset_restores_dealt_cards_without_reordering)
{
    SimpleDeck deck;

    EXPECT_EQ(CardSet("As"), deck.deal(1));
    EXPECT_EQ(51, deck.size());

    deck.reset();

    EXPECT_EQ(52, deck.size());
    EXPECT_EQ(CardSet("As"), deck.deal(1));
}

TEST(SimpleDeck, remove_does_not_double_count_already_dead_cards)
{
    SimpleDeck deck;

    deck.remove(CardSet("AcKd"));
    EXPECT_EQ(50, deck.size());

    const CardSet dealt = deck.deal(1);
    EXPECT_EQ(49, deck.size());

    deck.remove(CardSet("AcQh"));

    EXPECT_EQ(48, deck.size());
    EXPECT_TRUE(deck.dead().contains(CardSet("AcKdQh")));
    EXPECT_TRUE(deck.dead().contains(dealt));
}

TEST(SimpleDeck, peek_uses_deck_position_bitmasks)
{
    SimpleDeck deck;

    EXPECT_EQ(CardSet("2c"), deck.peek(UINT64_C(0x1)));
    EXPECT_EQ(CardSet("3c"), deck.peek(UINT64_C(0x2)));
    EXPECT_EQ(CardSet("2c3c"), deck.peek(UINT64_C(0x3)));
}

TEST(SimpleDeck, dead_includes_removed_and_dealt_cards)
{
    SimpleDeck deck;

    deck.remove(CardSet("AcKd"));
    const CardSet dealt = deck.deal(2);

    const CardSet dead = deck.dead();
    EXPECT_EQ(4, dead.size());
    EXPECT_TRUE(dead.contains(CardSet("AcKd")));
    EXPECT_TRUE(dead.contains(dealt));
}
