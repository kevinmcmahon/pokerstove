#include "PokerHand.h"

#include <gtest/gtest.h>

#include <stdexcept>

using namespace pokerstove;

TEST(PokerHand, StringConstructorPreservesInputOrder)
{
    EXPECT_EQ("AcKdQh", PokerHand("AcKdQh").str());
}

TEST(PokerHand, StringConstructorIgnoresSeparators)
{
    EXPECT_EQ("AcKdQh", PokerHand("[Ac] Kd/Qh").str());
}

TEST(PokerHand, AppendSkipsDuplicateCards)
{
    PokerHand hand("Ac");

    hand.append(Card("Ac"));
    hand.append(Card("Kd"));
    hand.append(CardSet("KdQh"));
    hand.append(PokerHand("QhJs"));

    EXPECT_EQ("AcKdQhJs", hand.str());
}

TEST(PokerHand, RemoveCompactsFrontMiddleAndTail)
{
    PokerHand hand("AcKdQhJsTc");

    hand.remove(Card("Ac"));
    EXPECT_EQ("KdQhJsTc", hand.str());

    hand.remove(Card("Js"));
    EXPECT_EQ("KdQhTc", hand.str());

    hand.remove(Card("Tc"));
    EXPECT_EQ("KdQh", hand.str());
}

TEST(PokerHand, PreflopStringNormalizesPairsAndSuiting)
{
    EXPECT_EQ("AA", PokerHand("AcAs").preflopstr());
    EXPECT_EQ("AKs", PokerHand("AcKc").preflopstr());
    EXPECT_EQ("AKo", PokerHand("KdAc").preflopstr());
    EXPECT_EQ("QJo", PokerHand("JdQh").preflopstr());
}

TEST(PokerHand, PreflopStringRejectsNonHoldemHands)
{
    EXPECT_THROW(PokerHand("AcKdQh").preflopstr(), std::runtime_error);
}

TEST(PokerHand, SortOrdersByRankThenSuit)
{
    PokerHand hand("Ac2dKhJs");

    hand.sort();

    EXPECT_EQ("2dJsKhAc", hand.str());
}

TEST(PokerHand, SortRanksOrdersDescendingByRank)
{
    PokerHand hand("2dAcKhJs");

    hand.sortRanks();

    EXPECT_EQ("AcKhJs2d", hand.str());
}

TEST(PokerHand, PushCardToFrontMovesSelectedCard)
{
    PokerHand hand("AcKdQhJs");

    hand.pushCardToFront(2);
    EXPECT_EQ("QhAcKdJs", hand.str());

    hand.pushCardToFront(10);
    EXPECT_EQ("QhAcKdJs", hand.str());
}

TEST(PokerHand, SortEvalMovesWheelAceToEnd)
{
    PokerHand hand("Ac2d3h4s5c");

    hand.sortEval();

    EXPECT_EQ("5c4s3h2dAc", hand.str());
}
