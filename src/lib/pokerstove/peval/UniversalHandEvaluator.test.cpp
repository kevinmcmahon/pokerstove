#include "UniversalHandEvaluator.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace pokerstove;
using namespace std;

TEST(UniversalHandEvaluator, Construct)
{
    UniversalHandEvaluator eval(4,4,3,5,2,&CardSet::evaluateHigh, NULL);
    EXPECT_EQ(true, eval.usesSuits());
    EXPECT_EQ(5, eval.boardSize());
}

TEST(UniversalHandEvaluator, FiveCardOmaha)
{
    UniversalHandEvaluator oeval(5,5,3,5,2,&CardSet::evaluateHigh, NULL);
    CardSet hand("2d3dKhQd8h");
    CardSet board("2c3c4c");
    PokerEvaluation eval = oeval.eval(hand, board);
    EXPECT_EQ(TWO_PAIR, eval.type());
    EXPECT_EQ(Rank("3"), eval.majorRank());
    EXPECT_EQ(Rank("2"), eval.minorRank());
}

TEST(UniversalHandEvaluator, SixCardOmaha)
{
    UniversalHandEvaluator oeval(6,6,3,5,2,&CardSet::evaluateHigh, NULL);
    CardSet hand("2d3dKhQd8h8d");
    CardSet board("2c3c4c");
    PokerEvaluation eval = oeval.eval(hand, board);
    EXPECT_EQ(TWO_PAIR, eval.type());
    EXPECT_EQ(Rank("3"), eval.majorRank());
    EXPECT_EQ(Rank("2"), eval.minorRank());
}

TEST(UniversalHandEvaluator, RejectsNullPrimaryEvaluator)
{
    EXPECT_THROW(
        UniversalHandEvaluator(4, 4, 3, 5, 2, evalFunction(NULL), NULL),
        std::invalid_argument);
}

TEST(UniversalHandEvaluator, RejectsInvalidPocketCardCounts)
{
    UniversalHandEvaluator eval(4,4,3,5,2,&CardSet::evaluateHigh, NULL);

    EXPECT_THROW(eval.evaluateHand(CardSet("AcKdQs"), CardSet("2c3d4h")),
                 std::invalid_argument);
    EXPECT_THROW(eval.evaluateHand(CardSet("AcKdQsJhTs"), CardSet("2c3d4h")),
                 std::invalid_argument);
}

TEST(UniversalHandEvaluator, RejectsInvalidBoardCardCounts)
{
    UniversalHandEvaluator eval(4,4,3,5,2,&CardSet::evaluateHigh, NULL);
    const CardSet hand("AcKdQsJh");

    EXPECT_THROW(eval.evaluateHand(hand, CardSet("2c3d")), std::invalid_argument);
    EXPECT_THROW(eval.evaluateHand(hand, CardSet("2c3d4h5s6c7d")),
                 std::invalid_argument);
}

TEST(UniversalHandEvaluator, FillSubsetsReturnsEmptyCandidateWhenSubsetTooLarge)
{
    UniversalHandEvaluator eval(4,4,3,5,2,&CardSet::evaluateHigh, NULL);
    std::vector<CardSet> candidates;

    eval.fillSubsets(candidates, 5, CardSet("AcKd"));

    ASSERT_EQ(1u, candidates.size());
    EXPECT_EQ(CardSet(), candidates[0]);
}

TEST(UniversalHandEvaluator, FillSubsetsReturnsOriginalSetForZeroSubset)
{
    UniversalHandEvaluator eval(4,4,3,5,2,&CardSet::evaluateHigh, NULL);
    std::vector<CardSet> candidates;

    eval.fillSubsets(candidates, 0, CardSet("AcKd"));

    ASSERT_EQ(1u, candidates.size());
    EXPECT_EQ(CardSet("AcKd"), candidates[0]);
}

TEST(UniversalHandEvaluator, HighLowEvaluationTracksQualifiedLow)
{
    UniversalHandEvaluator eval(
        1, 7, 0, 0, 0, &CardSet::evaluateHigh, &CardSet::evaluate8LowA5);

    const PokerHandEvaluation qualifying =
        eval.evaluateHand(CardSet("Ac2d3h4c5s7d8c"), CardSet());
    const PokerHandEvaluation noLow =
        eval.evaluateHand(CardSet("AcKdQhJsTc9d8c"), CardSet());

    EXPECT_TRUE(qualifying.highlow());
    EXPECT_GT(qualifying.low(), PokerEvaluation());
    EXPECT_FALSE(noLow.highlow());
}
