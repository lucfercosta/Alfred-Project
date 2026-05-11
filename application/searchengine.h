#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "../data/IndexedItem.h"

#include <QVector>
#include <QString>
#include <QMap>



/*
 * SearchEngine
 *
 * This class belongs to the APPLICATION LAYER (logic layer).
 *
 * Responsibility:
 * - Take user input (search query)
 * - Process and normalize it
 * - Compare it against indexed items
 * - Score each item based on relevance
 * - Return a ranked list of results
 *
 * This is the CORE of the search system (ranking + matching logic)
 */
class SearchEngine {
public:

    /*
     * normalizeQueryForMatching
     *
     * Purpose:
     * - Converts input text into a standard format
     * - Ensures case-insensitive matching
     *
     * Example:
     * "Fire Fox" → "fire fox"
     */
    QString normalizeQueryForMatching(const QString& rawQuery);

    /*
     * splitQueryIntoTokens
     *
     * Purpose:
     * - Breaks a query into individual words (tokens)
     * - Each token is used for matching against item names
     *
     * Example:
     * "fire fox" → ["fire", "fox"]
     */
    QVector<QString> splitQueryIntoTokens(const QString& normalizedQuery); // split query into words




    /*
     * calculateRelevanceScore
     *
     * Purpose:
     * - Calculates how relevant an item is to the query
     *
     * Factors used:
     * - Exact match (highest score)
     * - Prefix match (high score)
     * - Partial match (lower score)
     * - Token coverage (how many words match)
     *
     * Input:
     * - indexedItem: item being evaluated
     * - queryTokens: processed search terms
     *
     * Output:
     * - integer score representing relevance
     */
    int calculateRelevanceScore(const IndexedItem& indexedItem, const QVector<QString>& queryTokens);


    /*
     * findMatchingItems (MAIN SEARCH FUNCTION)
     *
     * Purpose:
     * This is the CORE pipeline of the search system:
     *
     * 1. Normalize query
     * 2. Split into tokens
     * 3. Compare against all indexed items
     * 4. Calculate relevance score for each item
     * 5. Apply usage-based boost (learning system)
     * 6. Sort results by score
     * 7. Return top N results
     *
     * Inputs:
     * - query: raw user input
     * - indexedItems: full dataset from filesystem index
     * - maxResults: limit of results shown in UI
     * - itemSelectionFrequency:
     *      map that tracks how often each item is used
     *      (used for adaptive ranking / learning)
     *
     * Output:
     * - ranked list of best matching items
     */
    QVector<IndexedItem> findMatchingItems(const QString& query, const QVector<IndexedItem>& indexedItems, int maxResults, const QMap<QString,int>& itemSelectionFrequency);
};

#endif
