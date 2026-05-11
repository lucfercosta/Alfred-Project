#include "searchengine.h"

#include <algorithm>



/*
 * normalizeQueryForMatching
 *
 * PURPOSE:
 * - Standardizes user input for consistent matching
 * - Removes case sensitivity issues
 *
 * Example:
 * "Fire Fox" → "fire fox"
 */
QString SearchEngine::normalizeQueryForMatching(const QString& rawQuery)
{
    return rawQuery.toLower();
}


/*
 * splitQueryIntoTokens
 *
 * PURPOSE:
 * - Breaks query into individual words (tokens)
 * - Each token is matched separately against items
 *
 * Example:
 * "fire fox" → ["fire", "fox"]
 */
QVector<QString> SearchEngine::splitQueryIntoTokens(const QString& normalizedQuery)
{
    return normalizedQuery.split(" ").toVector(); // turns "fire fox" → ["fire", "fox"]
}


/*
 * calculateRelevanceScore
 *
 * PURPOSE:
 * - Computes how well an item matches the query
 * - Uses simple heuristic scoring (not ML)
 *
 * SCORING RULES:
 * +100 → starts with token (strong match)
 * +50  → contains token (weak match)
 *
 * STRATEGY:
 * - Each query token contributes independently
 * - Final score is sum of all token matches
 */
int SearchEngine::calculateRelevanceScore(const IndexedItem& indexedItem, const QVector<QString>& queryTokens)
{
    // Normalize item name for case-insensitive comparison
    QString normalizedIndexedItemName = normalizeQueryForMatching(indexedItem.displayName);
    int totalScore = 0;

    // Evaluate each token separately
    for (const QString& queryToken : queryTokens)
    {
        // Limit token size to reduce noise / garbage input
        QString limitedLengthToken = queryToken.left(10);

        int currentTokenScore = 0;

        /*
         * Strong match:
         * Item name starts with query token
         * Example: "firefox" matches "fire"
         */
        if (normalizedIndexedItemName.startsWith(limitedLengthToken))
        {
            currentTokenScore += 100;
        }

        /*
         * Weak match:
         * Token appears anywhere in string
         */
        else if (normalizedIndexedItemName.contains(limitedLengthToken))
        {
            currentTokenScore += 50;
        }

        totalScore += currentTokenScore;
    }

    return totalScore;
}



/*
 * findMatchingItems (CORE SEARCH PIPELINE)
 *
 * PURPOSE:
 * This is the main search algorithm.
 *
 * PIPELINE:
 * 1. Validate input
 * 2. Normalize rawQuery
 * 3. Tokenize rawQuery
 * 4. Score all indexed items
 * 5. Apply learning boost UI (usage frequency)
 * 6. Apply type priority boost
 * 7. Sort results
 * 8. Return top N results
 */
QVector<IndexedItem> SearchEngine::findMatchingItems(const QString& rawQuery, const QVector<IndexedItem>& indexedItems, int maxResults, const QMap<QString,int>& itemSelectionFrequency)
{
    // If query is empty → no search needed
    if (rawQuery.isEmpty())
        return {};

    // Step 1: normalize input
    QString normalizedQuery = normalizeQueryForMatching(rawQuery);

    // Step 2: tokenize query
    QVector<QString> normalizedTokens = splitQueryIntoTokens(normalizedQuery);

    // Key&Value pair e.g.: "/usr/bin/firefox" → 12
    QVector<std::pair<IndexedItem,int>> scoredItems;

    // Step 3: score every indexed item
    for (const IndexedItem& indexedItem : indexedItems)
    {
        // Base score from text matching
        int baseRelevanceScore = calculateRelevanceScore(indexedItem, normalizedTokens);

        // Skip irrelevant items
        if (baseRelevanceScore <= 0)
            continue;

        /*
         * Learning system:
         * Items frequently used by the user get boosted
         */

        // If there's a value on the pair, it returns itemLocation and score, else returns itemLocation and 0
        int frequencyBoost = itemSelectionFrequency.value(indexedItem.itemLocation, 0) * 10;


        /*
         * Type priority system:
         * Applications are most important
         * Files second
         * Directories last
         */
        int typeBoost = 0;

        switch (indexedItem.itemType)
        {
        case IndexedItemType::Application:
            typeBoost = 3000; // highest priority
            break;

        case IndexedItemType::File:
            typeBoost = 2000; // medium priority
            break;

        case IndexedItemType::Directory:
            typeBoost = 0; // lowest priority
            break;
        }

        /*
         * Final score = relevance + learning + type priority
         */

        // Adds item to the end of the list
        scoredItems.push_back({indexedItem, baseRelevanceScore + frequencyBoost + typeBoost});
    }

    /*
     * Step 4: sort results by score (descending)
     * Highest score = most relevant result
     */

    // Compares item values (.second) in pairs and rearrange scoredItems using internal algorithm (sort)
    std::sort(scoredItems.begin(), scoredItems.end(), [](const auto& indexedItemA, const auto& indexedItemB)
    {
        // Decides which item should come first on the list
        return indexedItemA.second > indexedItemB.second;
    });

    /*
     * Step 5: extract top results
     */
    QVector<IndexedItem> topResults;

    // Adds a number N of already sorted items to topResults (only key .first)
    for (int i = 0; i < std::min(maxResults, int(scoredItems.size())); i++)
    {
        topResults.append(scoredItems[i].first);
    }

    return topResults;
}
