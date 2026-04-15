#include "PokerHandEvaluator.h"
#include "BadugiHandEvaluator.h"
#include "DeuceToSevenHandEvaluator.h"
#include "DrawHighHandEvaluator.h"
#include "HoldemHandEvaluator.h"
#include "OmahaEightHandEvaluator.h"
#include "OmahaHighHandEvaluator.h"
#include "RazzHandEvaluator.h"
#include "StudEightHandEvaluator.h"
#include "StudHandEvaluator.h"
#include "UniversalHandEvaluator.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace pokerstove;

namespace
{

enum class AllocKind
{
    Badugi,
    DeuceToSeven,
    DrawHigh,
    Holdem,
    OmahaEight,
    OmahaHigh,
    Razz,
    Stud,
    StudEight,
    Universal,
};

template <typename Evaluator>
std::vector<EquityResult> normalizedShowdown(Evaluator& evaluator,
                                             const std::vector<CardSet>& hands,
                                             const CardSet& board = CardSet())
{
    std::vector<PokerHandEvaluation> evals(hands.size());
    std::vector<EquityResult> results(hands.size());
    evaluator.evaluateShowdown(hands, board, evals, results, 1.0);
    EquityResult::normalize(results);
    return results;
}

void expectEvaluatorKind(const std::shared_ptr<PokerHandEvaluator>& evaluator,
                         AllocKind kind)
{
    switch (kind)
    {
        case AllocKind::Badugi:
            EXPECT_NE(nullptr, dynamic_cast<BadugiHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::DeuceToSeven:
            EXPECT_NE(nullptr, dynamic_cast<DeuceToSevenHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::DrawHigh:
            EXPECT_NE(nullptr, dynamic_cast<DrawHighHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::Holdem:
            EXPECT_NE(nullptr, dynamic_cast<HoldemHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::OmahaEight:
            EXPECT_NE(nullptr, dynamic_cast<OmahaEightHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::OmahaHigh:
            EXPECT_NE(nullptr, dynamic_cast<OmahaHighHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::Razz:
            EXPECT_NE(nullptr, dynamic_cast<RazzHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::Stud:
            EXPECT_NE(nullptr, dynamic_cast<StudHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::StudEight:
            EXPECT_NE(nullptr, dynamic_cast<StudEightHandEvaluator*>(evaluator.get()));
            break;
        case AllocKind::Universal:
            EXPECT_NE(nullptr, dynamic_cast<UniversalHandEvaluator*>(evaluator.get()));
            break;
    }
}

}  // namespace

TEST(PokerHandEvaluator, AllocSupportsDocumentedGameIdentifiers)
{
    struct AllocCase
    {
        const char* id;
        size_t handSize;
        size_t boardSize;
        size_t evaluationSize;
        bool usesSuits;
        AllocKind kind;
    };

    const std::vector<AllocCase> cases = {
        {"h", 2, 5, 1, true, AllocKind::Holdem},
        {"O", 4, 5, 1, true, AllocKind::OmahaHigh},
        {"o", 4, 5, 1, true, AllocKind::OmahaHigh},
        {"o8", 4, 5, 2, true, AllocKind::OmahaEight},
        {"o/8", 4, 5, 2, true, AllocKind::OmahaEight},
        {"omaha", 4, 5, 1, true, AllocKind::OmahaHigh},
        {"OMAHA/8", 4, 5, 2, true, AllocKind::OmahaEight},
        {"o5", 4, 5, 1, true, AllocKind::Universal},
        {"o6", 4, 5, 1, true, AllocKind::Universal},
        {"o5/8", 4, 5, 2, true, AllocKind::Universal},
        {"o6/8", 4, 5, 2, true, AllocKind::Universal},
        {"k", 5, 0, 1, true, AllocKind::Universal},
        {"l", 5, 0, 1, true, AllocKind::Universal},
        {"3", 3, 0, 1, true, AllocKind::Universal},
        {"r", 7, 0, 1, false, AllocKind::Razz},
        {"s", 7, 0, 1, true, AllocKind::Stud},
        {"q", 7, 0, 2, true, AllocKind::Universal},
        {"d", 5, 0, 1, true, AllocKind::DrawHigh},
        {"D", 5, 0, 1, true, AllocKind::DrawHigh},
        {"t", 5, 0, 1, true, AllocKind::DeuceToSeven},
        {"T", 5, 0, 1, true, AllocKind::Universal},
        {"e", 7, 0, 2, true, AllocKind::StudEight},
        {"b", 4, 0, 1, true, AllocKind::Badugi},
    };

    for (const AllocCase& testCase : cases)
    {
        const std::shared_ptr<PokerHandEvaluator> evaluator =
            PokerHandEvaluator::alloc(testCase.id);

        ASSERT_NE(nullptr, evaluator) << testCase.id;
        EXPECT_EQ(testCase.handSize, evaluator->handSize()) << testCase.id;
        EXPECT_EQ(testCase.boardSize, evaluator->boardSize()) << testCase.id;
        EXPECT_EQ(testCase.evaluationSize, evaluator->evaluationSize()) << testCase.id;
        EXPECT_EQ(testCase.usesSuits, evaluator->usesSuits()) << testCase.id;
        expectEvaluatorKind(evaluator, testCase.kind);
    }
}

TEST(PokerHandEvaluator, InvalidGame)
{
    EXPECT_EQ(nullptr, PokerHandEvaluator::alloc("x"));
    EXPECT_EQ(nullptr, PokerHandEvaluator::alloc(""));
}

TEST(PokerHandEvaluator, NormalizeUsesFixedTotalShares)
{
    std::vector<EquityResult> results(2);
    results[0].tieShares = 0.5;
    results[1].tieShares = 0.5;

    EquityResult::normalize(results);

    EXPECT_DOUBLE_EQ(0.5, results[0].equity);
    EXPECT_DOUBLE_EQ(0.5, results[1].equity);
}

TEST(PokerHandEvaluator, NormalizeLeavesZeroTotalsUnchanged)
{
    std::vector<EquityResult> results(2);
    results[0].equity = 0.4;
    results[1].equity = 0.6;

    EquityResult::normalize(results);

    EXPECT_DOUBLE_EQ(0.4, results[0].equity);
    EXPECT_DOUBLE_EQ(0.6, results[1].equity);
}

TEST(PokerHandEvaluator, EvaluateShowdownAppliesWeightsAndAccumulatesResults)
{
    HoldemHandEvaluator evaluator;
    const std::vector<CardSet> hands = {CardSet("AcAs"), CardSet("KhQh")};
    const CardSet board("2c3d4h5s9c");
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<EquityResult> results(2);

    evaluator.evaluateShowdown(hands, board, evals, results, 0.25);
    EXPECT_DOUBLE_EQ(0.25, results[0].winShares);
    EXPECT_DOUBLE_EQ(0.0, results[0].tieShares);
    EXPECT_DOUBLE_EQ(0.0, results[1].shares());

    evaluator.evaluateShowdown(hands, board, evals, results, 0.25);
    EXPECT_DOUBLE_EQ(0.5, results[0].winShares);
    EXPECT_DOUBLE_EQ(0.0, results[0].tieShares);
    EXPECT_DOUBLE_EQ(0.0, results[1].shares());
}

TEST(PokerHandEvaluator, EvaluateShowdownAppliesWeightsToTies)
{
    HoldemHandEvaluator evaluator;
    const std::vector<CardSet> hands = {CardSet("2d2h"), CardSet("3d3h")};
    const CardSet board("AcKcQcJcTc");
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<EquityResult> results(2);

    evaluator.evaluateShowdown(hands, board, evals, results, 0.5);

    EXPECT_DOUBLE_EQ(0.0, results[0].winShares);
    EXPECT_DOUBLE_EQ(0.25, results[0].tieShares);
    EXPECT_DOUBLE_EQ(0.0, results[1].winShares);
    EXPECT_DOUBLE_EQ(0.25, results[1].tieShares);
}

TEST(PokerHandEvaluator, EvaluateShowdownHandlesLargeTies)
{
    HoldemHandEvaluator evaluator;
    const std::vector<CardSet> hands = {
        CardSet("2d2h"), CardSet("3d3h"), CardSet("4d4h"), CardSet("5d5h"),
        CardSet("6d6h"), CardSet("7d7h"), CardSet("8d8h"), CardSet("9d9h"),
        CardSet("TdTh"), CardSet("JdJh"), CardSet("QdQh"),
    };
    CardSet board("AcKcQcJcTc");
    std::vector<PokerHandEvaluation> evals(hands.size());
    std::vector<EquityResult> results(hands.size());

    evaluator.evaluateShowdown(hands, board, evals, results, 1.0);

    for (const EquityResult& result : results)
    {
        EXPECT_DOUBLE_EQ(0.0, result.winShares);
        EXPECT_DOUBLE_EQ(1.0 / 11.0, result.tieShares);
    }
}

TEST(PokerHandEvaluator, EvaluateShowdownSplitsHighAndLowWhenOnlyOneHandQualifiesLow)
{
    OmahaEightHandEvaluator evaluator;
    const std::vector<CardSet> hands = {
        CardSet("3d4hKhQd"),
        CardSet("AcKcQhTs"),
    };
    const CardSet board("2c5c6cJhTd");
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<EquityResult> results(2);

    evaluator.evaluateShowdown(hands, board, evals, results, 1.0);

    EXPECT_DOUBLE_EQ(0.5, results[0].winShares);
    EXPECT_DOUBLE_EQ(0.0, results[0].tieShares);
    EXPECT_DOUBLE_EQ(0.5, results[1].winShares);
    EXPECT_DOUBLE_EQ(0.0, results[1].tieShares);
}

TEST(PokerHandEvaluator, EvaluateShowdownUsesSinglePotWhenNoLowQualifies)
{
    OmahaEightHandEvaluator evaluator;
    const std::vector<CardSet> hands = {
        CardSet("2c3cKhQd"),
        CardSet("4c5cAhTc"),
    };
    const CardSet board("AcKcQhJsTd");
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<EquityResult> results(2);

    evaluator.evaluateShowdown(hands, board, evals, results, 1.0);

    EXPECT_DOUBLE_EQ(0.0, results[0].winShares);
    EXPECT_DOUBLE_EQ(0.5, results[0].tieShares);
    EXPECT_DOUBLE_EQ(0.0, results[1].winShares);
    EXPECT_DOUBLE_EQ(0.5, results[1].tieShares);
}

TEST(PokerHandEvaluator, EvaluateEquityMatchesNormalizedShowdownForHighOnlyGames)
{
    HoldemHandEvaluator evaluator;
    const CardSet first("AcAs");
    const CardSet second("KhQh");
    const CardSet board("2c3d4h5s9c");

    const std::vector<EquityResult> results =
        normalizedShowdown(evaluator, {first, second}, board);

    EXPECT_DOUBLE_EQ(results[0].equity,
                     evaluator.evaluateEquity(first, second, board));
    EXPECT_DOUBLE_EQ(results[1].equity,
                     evaluator.evaluateEquity(second, first, board));
}

TEST(PokerHandEvaluator, EvaluateEquityMatchesNormalizedShowdownForHighLowGames)
{
    OmahaEightHandEvaluator evaluator;
    const CardSet first("4c3sKhQd");
    const CardSet second("4h5cQhKh");
    const CardSet board("Ac2d6cKcJs");

    const std::vector<EquityResult> results =
        normalizedShowdown(evaluator, {first, second}, board);

    EXPECT_DOUBLE_EQ(results[0].equity,
                     evaluator.evaluateEquity(first, second, board));
    EXPECT_DOUBLE_EQ(results[1].equity,
                     evaluator.evaluateEquity(second, first, board));
}
