#include "ShowdownEnumerator.h"

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include <pokerstove/peval/Card.h>
#include <pokerstove/peval/DrawHighHandEvaluator.h>
#include <pokerstove/peval/HoldemHandEvaluator.h>

using namespace pokerstove;

namespace
{

void ExpectResultsEq(const std::vector<EquityResult>& actual,
                     const std::vector<EquityResult>& expected)
{
    ASSERT_EQ(expected.size(), actual.size());
    for (size_t i = 0; i < actual.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(expected[i].winShares, actual[i].winShares) << i;
        EXPECT_DOUBLE_EQ(expected[i].tieShares, actual[i].tieShares) << i;
    }
}

std::vector<EquityResult>
manualFixedBoardEquity(const std::vector<CardDistribution>& dists,
                       const CardSet& board,
                       PokerHandEvaluator& evaluator)
{
    std::vector<EquityResult> results(dists.size());
    std::vector<PokerHandEvaluation> evals(dists.size());
    std::vector<CardSet> hands(dists.size());

    std::function<void(size_t, CardSet, double)> recurse =
        [&](size_t index, CardSet dead, double weight) {
            if (index == dists.size())
            {
                evaluator.evaluateShowdown(hands, board, evals, results, weight);
                return;
            }

            for (size_t i = 0; i < dists[index].size(); ++i)
            {
                const CardSet hand = dists[index][i];
                if (!dead.disjoint(hand))
                    continue;

                hands[index] = hand;
                recurse(index + 1,
                        dead | hand,
                        weight * dists[index][hand]);
            }
        };

    recurse(0, board, 1.0);
    return results;
}

std::vector<EquityResult>
manualSingleCardHoldemRollout(const std::vector<CardSet>& hands,
                              const CardSet& partialBoard)
{
    HoldemHandEvaluator evaluator;
    CardSet live;
    live.fill();
    live.remove(partialBoard);
    for (const CardSet& hand : hands)
        live.remove(hand);

    std::vector<EquityResult> results(hands.size());
    std::vector<PokerHandEvaluation> evals(hands.size());
    for (const Card& card : live.cards())
    {
        CardSet board = partialBoard;
        board.insert(card);
        evaluator.evaluateShowdown(hands, board, evals, results, 1.0);
    }

    return results;
}

std::vector<CardSet> dealDisjointCardSets(std::mt19937& rng,
                                          const std::vector<size_t>& sizes)
{
    std::vector<int> deck(STANDARD_DECK_SIZE);
    std::iota(deck.begin(), deck.end(), 0);
    std::shuffle(deck.begin(), deck.end(), rng);

    std::vector<CardSet> dealt;
    dealt.reserve(sizes.size());

    size_t offset = 0;
    for (const size_t size : sizes)
    {
        CardSet cards;
        for (size_t i = 0; i < size; ++i)
            cards.insert(Card(static_cast<uint8_t>(deck[offset + i])));
        offset += size;
        dealt.push_back(cards);
    }

    return dealt;
}

}  // namespace

TEST(ShowdownEnumerator, NullEvaluatorThrows)
{
    ShowdownEnumerator showdown;
    std::vector<CardDistribution> dists = {
        CardDistribution(CardSet("AcAs")),
        CardDistribution(CardSet("KhQh")),
    };

    EXPECT_THROW(showdown.calculateEquity(dists, CardSet("2c3d4h5s9c"), nullptr),
                 std::runtime_error);
}

TEST(ShowdownEnumerator, FixedBoardMatchesDirectShowdown)
{
    auto evaluator = std::make_shared<HoldemHandEvaluator>();
    ShowdownEnumerator showdown;
    std::vector<CardDistribution> dists = {
        CardDistribution(CardSet("AcAs")),
        CardDistribution(CardSet("KhQh")),
    };
    const CardSet board("2c3d4h5s9c");

    const std::vector<EquityResult> actual =
        showdown.calculateEquity(dists, board, evaluator);

    std::vector<EquityResult> expected(2);
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<CardSet> hands = {CardSet("AcAs"), CardSet("KhQh")};
    evaluator->evaluateShowdown(hands, board, evals, expected, 1.0);

    ExpectResultsEq(actual, expected);
}

TEST(ShowdownEnumerator, RollsOutMissingBoardCards)
{
    auto evaluator = std::make_shared<HoldemHandEvaluator>();
    ShowdownEnumerator showdown;
    std::vector<CardDistribution> dists = {
        CardDistribution(CardSet("AcAs")),
        CardDistribution(CardSet("KhQh")),
    };
    const CardSet board("2c3d4h5s");

    const std::vector<EquityResult> actual =
        showdown.calculateEquity(dists, board, evaluator);
    const std::vector<EquityResult> expected =
        manualSingleCardHoldemRollout({CardSet("AcAs"), CardSet("KhQh")}, board);

    ExpectResultsEq(actual, expected);
}

TEST(ShowdownEnumerator, AppliesDistributionWeights)
{
    auto evaluator = std::make_shared<HoldemHandEvaluator>();
    ShowdownEnumerator showdown;
    CardDistribution first;
    CardDistribution second;
    ASSERT_TRUE(first.parse("AcAs=0.25,KcKs=0.75"));
    ASSERT_TRUE(second.parse("QhQs=0.5,JhJs=0.5"));
    const std::vector<CardDistribution> dists = {first, second};
    const CardSet board("2c3d4h5s9c");

    const std::vector<EquityResult> actual =
        showdown.calculateEquity(dists, board, evaluator);
    const std::vector<EquityResult> expected =
        manualFixedBoardEquity(dists, board, *evaluator);

    ExpectResultsEq(actual, expected);
}

TEST(ShowdownEnumerator, SkipsOverlappingHands)
{
    auto evaluator = std::make_shared<HoldemHandEvaluator>();
    ShowdownEnumerator showdown;
    CardDistribution first(CardSet("AcAs"));
    CardDistribution second;
    ASSERT_TRUE(second.parse("AcKd=0.5,QhQs=0.5"));
    const std::vector<CardDistribution> dists = {first, second};
    const CardSet board("2c3d4h5s9c");

    const std::vector<EquityResult> actual =
        showdown.calculateEquity(dists, board, evaluator);

    std::vector<EquityResult> expected(2);
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<CardSet> hands = {CardSet("AcAs"), CardSet("QhQs")};
    evaluator->evaluateShowdown(hands, board, evals, expected, 0.5);

    ExpectResultsEq(actual, expected);
}

TEST(ShowdownEnumerator, SupportsNoBoardDrawGames)
{
    auto evaluator = std::make_shared<DrawHighHandEvaluator>();
    ShowdownEnumerator showdown;
    std::vector<CardDistribution> dists = {
        CardDistribution(CardSet("AsKsQsJsTs")),
        CardDistribution(CardSet("AcAd7c5d2h")),
    };

    const std::vector<EquityResult> actual =
        showdown.calculateEquity(dists, CardSet(), evaluator);

    std::vector<EquityResult> expected(2);
    std::vector<PokerHandEvaluation> evals(2);
    std::vector<CardSet> hands = {CardSet("AsKsQsJsTs"), CardSet("AcAd7c5d2h")};
    evaluator->evaluateShowdown(hands, CardSet(), evals, expected, 1.0);

    ExpectResultsEq(actual, expected);
}

TEST(ShowdownEnumerator, ReturnsZeroSharesWhenNoDisjointCombinationsExist)
{
    auto evaluator = std::make_shared<HoldemHandEvaluator>();
    ShowdownEnumerator showdown;
    const std::vector<CardDistribution> dists = {
        CardDistribution(CardSet("AcAs")),
        CardDistribution(CardSet("AcKd")),
    };

    const std::vector<EquityResult> actual =
        showdown.calculateEquity(dists, CardSet(), evaluator);

    ASSERT_EQ(2u, actual.size());
    EXPECT_DOUBLE_EQ(0.0, actual[0].shares());
    EXPECT_DOUBLE_EQ(0.0, actual[1].shares());
}

TEST(ShowdownEnumerator, RandomHoldemShowdownsNormalizeToOne)
{
    auto evaluator = std::make_shared<HoldemHandEvaluator>();
    ShowdownEnumerator showdown;
    std::mt19937 rng(1337);

    for (size_t i = 0; i < 64; ++i)
    {
        const std::vector<CardSet> cards = dealDisjointCardSets(rng, {2, 2, 5});
        const std::vector<CardDistribution> dists = {
            CardDistribution(cards[0]),
            CardDistribution(cards[1]),
        };

        std::vector<EquityResult> results =
            showdown.calculateEquity(dists, cards[2], evaluator);
        EquityResult::normalize(results);

        ASSERT_EQ(2u, results.size()) << i;
        EXPECT_NEAR(1.0, results[0].equity + results[1].equity, 1e-12) << i;
        EXPECT_DOUBLE_EQ(results[0].equity,
                         evaluator->evaluateEquity(cards[0], cards[1], cards[2])) << i;
        EXPECT_DOUBLE_EQ(results[1].equity,
                         evaluator->evaluateEquity(cards[1], cards[0], cards[2])) << i;
    }
}
