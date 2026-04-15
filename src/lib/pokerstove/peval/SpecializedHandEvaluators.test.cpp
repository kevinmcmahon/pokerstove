#include "BadugiHandEvaluator.h"
#include "DrawHighHandEvaluator.h"
#include "RazzHandEvaluator.h"
#include "StudEightHandEvaluator.h"
#include "StudHandEvaluator.h"

#include <gtest/gtest.h>

using namespace pokerstove;

TEST(SpecializedHandEvaluators, DrawHighEvaluatesHandsAndTracksHandSize)
{
    DrawHighHandEvaluator eval;

    EXPECT_GT(eval.evaluate(CardSet("AsKsQsJsTs")).high(),
              eval.evaluate(CardSet("AcAd7c5d2h")).high());

    eval.setHandSize(4);
    EXPECT_EQ(4u, eval.handSize());
}

TEST(SpecializedHandEvaluators, RazzUsesAtoFiveLowAndIgnoresSuits)
{
    RazzHandEvaluator eval;

    EXPECT_FALSE(eval.usesSuits());
    EXPECT_GT(eval.evaluate(CardSet("As2d3h4c5s")).high(),
              eval.evaluate(CardSet("2c2d3c4d5h")).high());
}

TEST(SpecializedHandEvaluators, StudUsesHighHandEvaluation)
{
    StudHandEvaluator eval;

    EXPECT_GT(eval.evaluate(CardSet("AsKsQsJsTs9d8c")).high(),
              eval.evaluate(CardSet("AcAd7c5d2h3s4c")).high());
}

TEST(SpecializedHandEvaluators, StudEightProducesHighAndLowResults)
{
    StudEightHandEvaluator eval;

    const PokerHandEvaluation qualifying =
        eval.evaluate(CardSet("Ac2d3h4c5s7d8c"));
    const PokerHandEvaluation noLow =
        eval.evaluate(CardSet("AcKdQhJsTc9d8c"));

    EXPECT_TRUE(qualifying.highlow());
    EXPECT_GT(qualifying.low(), PokerEvaluation());
    EXPECT_FALSE(noLow.highlow());
}

TEST(SpecializedHandEvaluators, BadugiSupportsDrawTrackingAndThrowsForUnimplementedViews)
{
    BadugiHandEvaluator eval;
    const CardSet fourCardBadugi("Ac2d3h4s");

    EXPECT_GT(eval.evaluate(fourCardBadugi).high(),
              eval.evaluate(CardSet("Ac2c3h4s")).high());

    EXPECT_EQ(0u, eval.numDraws());
    eval.setNumDraws(2);
    EXPECT_EQ(2u, eval.numDraws());

    EXPECT_THROW(eval.evaluateRanks(fourCardBadugi), std::runtime_error);
    EXPECT_THROW(eval.evaluateSuits(fourCardBadugi), std::runtime_error);
}
