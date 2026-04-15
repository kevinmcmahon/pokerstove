#include "PokerHandEvaluator.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include <pokerstove/test/GoldenTestUtils.h>

using namespace pokerstove;

namespace
{

namespace pstest = pokerstove::test;

CardSet ParseCardSetField(const std::string& value)
{
    return pstest::isEmptyField(value) ? CardSet() : CardSet(value);
}

void ExpectRelation(const PokerEvaluation& left,
                    const PokerEvaluation& right,
                    const std::string& expected)
{
    if (expected == "gt")
        EXPECT_GT(left, right);
    else if (expected == "lt")
        EXPECT_LT(left, right);
    else if (expected == "eq")
        EXPECT_EQ(left, right);
    else
        FAIL() << "unsupported relation: " << expected;
}

}  // namespace

TEST(GoldenVectors, AllocatorsMatchFixtureMetadata)
{
    const std::vector<std::map<std::string, std::string>> records =
        pstest::readRecords("peval.golden");

    for (const std::map<std::string, std::string>& record : records)
    {
        if (pstest::requireField(record, "kind") != "alloc")
            continue;

        SCOPED_TRACE(pstest::requireField(record, "case"));
        const std::shared_ptr<PokerHandEvaluator> evaluator =
            PokerHandEvaluator::alloc(pstest::requireField(record, "game"));

        ASSERT_NE(nullptr, evaluator);
        EXPECT_EQ(static_cast<size_t>(std::stoul(pstest::requireField(record, "hand_size"))),
                  evaluator->handSize());
        EXPECT_EQ(static_cast<size_t>(std::stoul(pstest::requireField(record, "board_size"))),
                  evaluator->boardSize());
        EXPECT_EQ(static_cast<size_t>(std::stoul(pstest::requireField(record, "evaluation_size"))),
                  evaluator->evaluationSize());
        EXPECT_EQ(pstest::parseBool(pstest::requireField(record, "uses_suits")),
                  evaluator->usesSuits());
    }
}

TEST(GoldenVectors, DirectEvaluationsMatchFixtureMetadata)
{
    const std::vector<std::map<std::string, std::string>> records =
        pstest::readRecords("peval.golden");

    for (const std::map<std::string, std::string>& record : records)
    {
        if (pstest::requireField(record, "kind") != "eval")
            continue;

        SCOPED_TRACE(pstest::requireField(record, "case"));
        const std::shared_ptr<PokerHandEvaluator> evaluator =
            PokerHandEvaluator::alloc(pstest::requireField(record, "game"));
        ASSERT_NE(nullptr, evaluator);

        const PokerHandEvaluation evaluation =
            evaluator->evaluateHand(ParseCardSetField(pstest::requireField(record, "hand")),
                                    ParseCardSetField(pstest::requireField(record, "board")));

        EXPECT_EQ(pstest::parseBool(pstest::requireField(record, "highlow")),
                  evaluation.highlow());

        const std::string& highType = pstest::requireField(record, "high_type");
        if (!pstest::isEmptyField(highType))
            EXPECT_EQ(std::stoi(highType), evaluation.high().type());

        const std::string& highMajor = pstest::requireField(record, "high_major");
        if (!pstest::isEmptyField(highMajor))
            EXPECT_EQ(Rank(highMajor), evaluation.high().majorRank());

        const std::string& highMinor = pstest::requireField(record, "high_minor");
        if (!pstest::isEmptyField(highMinor))
            EXPECT_EQ(Rank(highMinor), evaluation.high().minorRank());

        if (evaluation.highlow())
            EXPECT_GT(evaluation.low(), PokerEvaluation());
        else
            EXPECT_EQ(PokerEvaluation(), evaluation.low());
    }
}

TEST(GoldenVectors, OrderingMatchesFixtureRelations)
{
    const std::vector<std::map<std::string, std::string>> records =
        pstest::readRecords("peval.golden");

    for (const std::map<std::string, std::string>& record : records)
    {
        if (pstest::requireField(record, "kind") != "compare")
            continue;

        SCOPED_TRACE(pstest::requireField(record, "case"));
        const std::shared_ptr<PokerHandEvaluator> evaluator =
            PokerHandEvaluator::alloc(pstest::requireField(record, "game"));
        ASSERT_NE(nullptr, evaluator);

        const CardSet board = ParseCardSetField(pstest::requireField(record, "board"));
        const PokerEvaluation left =
            evaluator->evaluateHand(CardSet(pstest::requireField(record, "hand_a")),
                                    board).high();
        const PokerEvaluation right =
            evaluator->evaluateHand(CardSet(pstest::requireField(record, "hand_b")),
                                    board).high();

        ExpectRelation(left, right, pstest::requireField(record, "expect"));
    }
}
