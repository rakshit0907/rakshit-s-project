#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <set>
#include <tuple>

using namespace std;

class BookAnalyzer {
private:
    unordered_map<string, int> wordCount;
    static const set<string> stopWords;

public:
    // Function to clean and process the word, convert it to uppercase, and remove non-alphanumeric characters
    static string cleanWord(const string& word) {
        string cleaned;
        for (char ch : word) {
            if (isalnum(ch)) {
                cleaned += toupper(ch);
            }
        }
        return cleaned;
    }

    // Analyze a single book and count its word frequencies
    void analyzeBook(const string& filePath) {
        ifstream file(filePath);
        string word;

        while (file >> word) {
            string cleaned = cleanWord(word);
            if (!cleaned.empty() && stopWords.find(cleaned) == stopWords.end()) {
                wordCount[cleaned]++;
            }
        }
    }

    // Normalize the word frequencies and return a sorted vector of pairs (word, normalized frequency)
    vector<pair<string, double>> getNormalizedFrequencies() const {
        vector<pair<string, int>> wordVector(wordCount.begin(), wordCount.end());

        int totalWordCount = 0;
        for (const auto& entry : wordVector) {
            totalWordCount += entry.second;
        }

        vector<pair<string, double>> normalizedWords;
        for (const auto& entry : wordVector) {
            normalizedWords.emplace_back(entry.first, entry.second / static_cast<double>(totalWordCount));
        }

        // Sort words by frequency in descending order
        sort(normalizedWords.begin(), normalizedWords.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
            return b.second < a.second;
        });

        return normalizedWords;
    }

    // Compute the similarity score between this book's word frequencies and another book's frequencies
    static double computeSimilarity(const vector<pair<string, double>>& frequencies1, const vector<pair<string, double>>& frequencies2) {
        unordered_map<string, double> frequencyMap;

        // Fill frequency map with the first dataset
        for (const auto& pair : frequencies1) {
            frequencyMap[pair.first] = pair.second;
        }

        // Calculate similarity by matching common words
        double similarityScore = 0.0;
        for (const auto& pair : frequencies2) {
            if (frequencyMap.count(pair.first)) {
                similarityScore += frequencyMap[pair.first] * pair.second;
            }
        }

        return similarityScore;
    }
};

// Initialize stop words
const set<string> BookAnalyzer::stopWords = {"A", "AND", "AN", "OF", "IN", "THE"};

// Function to display the top N most similar book pairs
void displayTopPairs(const vector<tuple<double, size_t, size_t>>& similarityScores, const vector<string>& bookPaths, size_t topN) {
    cout << "Top " << topN << " Similar Book Pairs:\n";
    for (size_t i = 0; i < topN && i < similarityScores.size(); ++i) {
        double score = get<0>(similarityScores[i]);
        size_t book1 = get<1>(similarityScores[i]);
        size_t book2 = get<2>(similarityScores[i]);
        cout << bookPaths[book1] << " and " << bookPaths[book2] << " - Similarity: " << score << endl;
    }
}

int main() {
    // List of books
    vector<string> bookPaths = {
        "Cats by Moncrif.txt", "Foxes Book of Martyrs Part 1.txt", "Foxes Book of Martyrs Part 2.txt",
        "Foxes Book of Martyrs Part 3.txt", "Foxes Book of Martyrs Part 4.txt", "Foxes Book of Martyrs Part 5.txt",
        "Foxes Book of Martyrs Part 6.txt", "Gerards Herbal Vol. 1.txt", "Gerards Herbal Vol. 2.txt",
        "Gerards Herbal Vol. 3.txt", "Gerards Herbal Vol.4.txt", "Gil Blas.txt", "Gossip in a Library.txt",
        "Hudibras.txt", "King of the Beggars.txt", "Knocknagow.txt", "Les Chats par Moncrif.txt",
        "Lives and Anecdotes of Misers.txt", "Love and Madness - Herbert Croft.txt", 
        "Memoirs of Laetitia Pilkington V 1.txt", "Memoirs of Laetitia Pilkington V 2.txt",
        "Memoirs of Laetitia Pilkington V 3.txt", "Memoirs of Mrs Margaret Leeson - Peg Plunkett.txt",
        "Monro his Expedition.txt", "Mrs Beetons Book of Household Management.txt",
        "Out of the Hurly-Burly.txt", "Percys Reliques.txt", "Pompey The Little.txt",
        "Radical Pamphlets from the English Civil War.txt", "Scepsis Scientifica.txt",
        "The Anatomy of Melancholy Part 1.txt", "The Anatomy of Melancholy Part 2.txt",
        "The Anatomy of Melancholy Part 3.txt", "The Complete Cony-catching.txt",
        "The Consolation of Philosophy.txt", "The Devil on Two Sticks.txt",
        "The Diary of a Lover of Literature.txt", "The History Of Ireland - Geoffrey Keating.txt",
        "The History of the Human Heart.txt", "The Ingoldsby Legends.txt", "The Life of Beau Nash.txt",
        "The Life of John Buncle by Thomas Amory.txt", "The Life of King Richard III.txt",
        "The Life of Pico della Mirandola.txt", "The Martyrdom of Man.txt", "The Masterpiece of Aristotle.txt",
        "The Memoirs of Count Boruwlaski.txt", "The Metamorphosis of Ajax.txt",
        "The Newgate Calendar - Supplement 3.txt", "The Newgate Calendar Supplement.txt",
        "The Newgate Calendar V 1.txt", "The Newgate Calendar V 2.txt", "The Newgate Calendar V 3.txt",
        "The Newgate Calendar V 4.txt", "The Newgate Calendar V 5.txt", "The Newgate Calendar V 6.txt",
        "The Poems of Ossian.txt", "The Poetical Works of John Skelton.txt",
        "The Protestant Reformation.txt", "The Real Story of John Carteret Pilkington.txt",
        "The Rowley Poems.txt", "The Silver Fox.txt"
    };

    size_t bookCount = bookPaths.size();
    vector<BookAnalyzer> analyzers(bookCount);

    // Analyze each book
    for (size_t i = 0; i < bookCount; ++i) {
        analyzers[i].analyzeBook(bookPaths[i]);
    }

    // Create a similarity matrix and compute pairwise similarities
    vector<vector<double>> similarityMatrix(bookCount, vector<double>(bookCount, 0.0));
    for (size_t i = 0; i < bookCount; ++i) {
        auto freq1 = analyzers[i].getNormalizedFrequencies();
        for (size_t j = i + 1; j < bookCount; ++j) {
            auto freq2 = analyzers[j].getNormalizedFrequencies();
            double similarity = BookAnalyzer::computeSimilarity(freq1, freq2);
            similarityMatrix[i][j] = similarity;
            similarityMatrix[j][i] = similarity;  // Symmetric matrix
        }
    }

    // Store and sort similarity scores
    vector<tuple<double, size_t, size_t>> similarityScores;
    for (size_t i = 0; i < bookCount; ++i) {
        for (size_t j = i + 1; j < bookCount; ++j) {
            similarityScores.emplace_back(similarityMatrix[i][j], i, j);
        }
    }

    sort(similarityScores.rbegin(), similarityScores.rend());

    // Display top N similar book pairs
    displayTopPairs(similarityScores, bookPaths, 10);

    return 0;
}
