/**
 * Copyright (c) 2012 Andrew Prock. All rights reserved.
 * $Id: PokerHandEvaluator.cpp 2649 2012-06-30 04:53:24Z prock $
 */
#include "PokerHandEvaluator.h"
#include <iostream>

using namespace std;
using namespace boost;
using namespace pokerstove;

PokerHandEvaluator::PokerHandEvaluator()
    : _subclassID("abstract-base-class")
    , _useSuits(true)
{}

PokerHandEvaluator::~PokerHandEvaluator() {}

/**
 * debugging util
 */
void display(const vector<CardSet>& hands, const CardSet& board, vector<EquityResult>& result)
{
    for (size_t i = 0; i < hands.size(); i++)
        cout << hands[i].str() << " ";
    for (size_t i = 0; i < result.size(); i++)
        cout << result[i].winShares << " ";
    cout << endl;
}

void PokerHandEvaluator::evaluateShowdown(const vector<CardSet>& hands,
                                          const CardSet& board,
                                          vector<PokerHandEvaluation>& evals,
                                          vector<EquityResult>& result,
                                          double weight) const
{
    // this is a special trick we use.  the hands vector could actually
    // contain hands [0..n],board because of the way we step through the
    // ParitionEnumerator, however, the size evals vector *must* be equal to
    // the number of hands, board or not.  So we use the size of the evals
    // here, not the size of the hand vector
    size_t hsize = evals.size();
    size_t nevals = 1;

    // gather all the evaluations
    for (size_t i = 0; i < hsize; i++)
    {
        // we track whether or not an eval is used in the nevals
        // variable to avoid looping through the low half of split
        // pot games when no one has a low.  This only covers games
        // which have one or two pots.
        evals[i] = evaluateHand(hands[i], board);
        if (nevals == 1 && evals[i].eval(1) > PokerEvaluation(0))
            nevals = 2;
    }

    // award share(s)
    for (size_t e = 0; e < nevals; e++)
    {
        // find the best eval, and adjust shares if there are ties
        PokerEvaluation maxeval = evals[0].eval(e);
        size_t winner = 0;
        size_t shares = 1;
        for (size_t i = 1; i < hsize; i++)
        {
            PokerEvaluation eval = evals[i].eval(e);
            if (eval > maxeval)
            {
                shares = 1;
                maxeval = eval;
                winner = i;
            }
            else if (eval == maxeval)
            {
                shares++;
            }
        }
        // award shares to the winner, or...
        if (shares == 1)
        {
            result[winner].winShares += weight / static_cast<double>(nevals);
        }
        // award shares to those who tie
        else
        {
            for (size_t i = 0; i < hsize; i++)
                if (evals[i].eval(e) == maxeval)
                    result[i].tieShares +=
                        weight / static_cast<double>(shares * nevals);
        }
    }
    // display (hands, board, result);
}

double PokerHandEvaluator::evaluateEquity(
    const pokerstove::CardSet& c1,
    const pokerstove::CardSet& c2,
    const pokerstove::CardSet& board)
{
    vector<EquityResult> results(2);
    vector<PokerHandEvaluation> evals;
    vector<CardSet> hands;
    hands.push_back(c1);
    hands.push_back(c2);
    evals.push_back(evaluate(c1, board));
    evals.push_back(evaluate(c2, board));

    evaluateShowdown(hands, board, evals, results, 1.0);
    EquityResult::normalize(results);
    return results[0].equity;
}
