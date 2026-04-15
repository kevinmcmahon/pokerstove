#include "PokerEvaluation.h"
#include "CardSet.h"
#include <gtest/gtest.h>

using namespace pokerstove;


TEST(PokerEvaluation, High)
{
    CardSet cards("AcAs8c8hKd");    // dead man's hand

    PokerEvaluation eval = cards.evaluateHigh();
    EXPECT_EQ(eval.type(), TWO_PAIR);
    EXPECT_EQ(eval.majorRank(), Rank::Ace());
    EXPECT_EQ(eval.minorRank(), Rank::Eight());
    EXPECT_EQ("Two Pair", eval.handType());
    EXPECT_EQ("Two pair: Aces and 8s, K kicker", eval.toStringPretty());
}

TEST(PokerEvaluation, EmptyEvaluationPrintsAsEmptyString)
{
    PokerEvaluation eval;

    EXPECT_EQ(0, eval.code());
    EXPECT_EQ("", eval.str());
}
