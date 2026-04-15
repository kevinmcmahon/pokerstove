#include "CardDistribution.h"
#include "ShowdownEnumerator.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include <pokerstove/test/GoldenTestUtils.h>

using namespace pokerstove;

namespace
{

namespace pstest = pokerstove::test;

CardSet ParseCardSetField(const std::string& value)
{
    return pstest::isEmptyField(value) ? CardSet() : CardSet(value);
}

std::vector<CardDistribution> ParseDistributions(const std::string& input)
{
    std::vector<CardDistribution> dists;
    for (const std::string& value : pstest::parseList(input, ';'))
    {
        CardDistribution dist;
        if (!dist.parse(value))
            throw std::runtime_error("invalid distribution fixture: " + value);
        dists.push_back(dist);
    }
    return dists;
}

}  // namespace

TEST(GoldenVectors, DistributionsMatchFixtureContract)
{
    const std::vector<std::map<std::string, std::string>> records =
        pstest::readRecords("penum.golden");

    for (const std::map<std::string, std::string>& record : records)
    {
        if (pstest::requireField(record, "kind") != "distribution")
            continue;

        SCOPED_TRACE(pstest::requireField(record, "case"));
        CardDistribution dist;
        ASSERT_TRUE(dist.parse(pstest::requireField(record, "input")));
        EXPECT_EQ(pstest::requireField(record, "expected_str"), dist.str());
        EXPECT_DOUBLE_EQ(std::stod(pstest::requireField(record, "total_weight")),
                         dist.weight());
        EXPECT_EQ(static_cast<size_t>(std::stoul(pstest::requireField(record, "size"))),
                  dist.size());

        CardDistribution roundTrip;
        ASSERT_TRUE(roundTrip.parse(dist.str()));
        EXPECT_EQ(dist.str(), roundTrip.str());
        EXPECT_DOUBLE_EQ(dist.weight(), roundTrip.weight());
    }
}

TEST(GoldenVectors, ShowdownsMatchFixtureContract)
{
    const std::vector<std::map<std::string, std::string>> records =
        pstest::readRecords("penum.golden");
    ShowdownEnumerator showdown;

    for (const std::map<std::string, std::string>& record : records)
    {
        if (pstest::requireField(record, "kind") != "showdown")
            continue;

        SCOPED_TRACE(pstest::requireField(record, "case"));
        const std::shared_ptr<PokerHandEvaluator> evaluator =
            PokerHandEvaluator::alloc(pstest::requireField(record, "game"));
        ASSERT_NE(nullptr, evaluator);

        const std::vector<CardDistribution> dists =
            ParseDistributions(pstest::requireField(record, "dists"));
        const std::vector<double> expectedWins =
            pstest::parseDoubles(pstest::requireField(record, "win"));
        const std::vector<double> expectedTies =
            pstest::parseDoubles(pstest::requireField(record, "tie"));

        ASSERT_EQ(dists.size(), expectedWins.size());
        ASSERT_EQ(dists.size(), expectedTies.size());

        const std::vector<EquityResult> results =
            showdown.calculateEquity(
                dists,
                ParseCardSetField(pstest::requireField(record, "board")),
                evaluator);

        ASSERT_EQ(dists.size(), results.size());

        double totalShares = 0.0;
        for (size_t i = 0; i < results.size(); ++i)
        {
            EXPECT_NEAR(expectedWins[i], results[i].winShares, 1e-12) << i;
            EXPECT_NEAR(expectedTies[i], results[i].tieShares, 1e-12) << i;
            totalShares += results[i].shares();
        }

        std::vector<EquityResult> normalized = results;
        EquityResult::normalize(normalized);
        double totalEquity = 0.0;
        for (const EquityResult& result : normalized)
            totalEquity += result.equity;

        if (totalShares == 0.0)
            EXPECT_DOUBLE_EQ(0.0, totalEquity);
        else
            EXPECT_NEAR(1.0, totalEquity, 1e-12);
    }
}
