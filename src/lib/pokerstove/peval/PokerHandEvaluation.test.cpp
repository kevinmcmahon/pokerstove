#include "OmahaEightHandEvaluator.h"
#include "PokerHandEvaluation.h"

#include <gtest/gtest.h>

using namespace pokerstove;

TEST(PokerHandEvaluation, SizeAndEvalListReflectLowPresence)
{
    const PokerHandEvaluation highOnly(CardSet("AcAs8c8hKd").evaluateHigh());
    const PokerHandEvaluation highLow(
        CardSet("Ac2d3h4c5s").evaluateHigh(),
        CardSet("Ac2d3h4c5s").evaluateLowA5());

    EXPECT_FALSE(highOnly.highlow());
    EXPECT_EQ(1, highOnly.size());
    EXPECT_EQ(1u, highOnly.evals().size());

    EXPECT_TRUE(highLow.highlow());
    EXPECT_EQ(2, highLow.size());
    EXPECT_EQ(2u, highLow.evals().size());
}

TEST(PokerHandEvaluation, SharesHandleSinglePotResults)
{
    const PokerHandEvaluation madeHand(CardSet("AsKsQsJsTs").evaluateHigh());
    const PokerHandEvaluation bluffCatcher(CardSet("AcAd7c5d2h").evaluateHigh());

    EXPECT_DOUBLE_EQ(1.0, shares(madeHand, bluffCatcher));
    EXPECT_DOUBLE_EQ(0.0, shares(bluffCatcher, madeHand));
    EXPECT_DOUBLE_EQ(0.5, shares(madeHand, madeHand));
}

TEST(PokerHandEvaluation, SharesHandleQuarteredAndThreeQuarteredPots)
{
    OmahaEightHandEvaluator evaluator;
    const PokerHandEvaluation hero =
        evaluator.evaluateHand(CardSet("4c3sKhQd"), CardSet("Ac2d6cKcJs"));
    const PokerHandEvaluation villain =
        evaluator.evaluateHand(CardSet("4h5cQhKh"), CardSet("Ac2d6cKcJs"));

    EXPECT_DOUBLE_EQ(0.75, shares(hero, villain));
    EXPECT_DOUBLE_EQ(0.25, shares(villain, hero));
}
