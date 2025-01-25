#include <bits/stdc++.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

string normalizeWord(const string &word) {
    string result;
    for (char c : word) {
        if (isalnum(c)) {
            result += toupper(c);
        }
    }
    return result;
}

vector<pair<string, double>> getFrequentWords(const string &filename, const set<string> &excludedWords) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Unable to open file " << filename << endl;
        return {};
    }

    map<string, int> wordCounts;
    int totalWords = 0;
    string word;

    while (file >> word) {
        string normalized = normalizeWord(word);
        if (!normalized.empty() && !excludedWords.count(normalized)) {
            wordCounts[normalized]++;
            totalWords++;
        }
    }

    vector<pair<string, int>> sortedWords(wordCounts.begin(), wordCounts.end());
    sort(sortedWords.begin(), sortedWords.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    vector<pair<string, double>> frequentWords;
    for (size_t i = 0; i < min<size_t>(100, sortedWords.size()); ++i) {
        frequentWords.push_back({sortedWords[i].first, (double)sortedWords[i].second / totalWords});
    }

    return frequentWords;
}

double computeSimilarity(const vector<pair<string, double>> &file1, const vector<pair<string, double>> &file2) {
    unordered_map<string, double> wordFreqMap;
    for (const auto &entry : file1) {
        wordFreqMap[entry.first] = entry.second;
    }

    double similarity = 0.0;
    for (const auto &entry : file2) {
        if (wordFreqMap.count(entry.first)) {
            similarity += wordFreqMap[entry.first] * entry.second;
        }
    }

    return similarity;
}

vector<vector<double>> buildSimilarityMatrix(const vector<string> &files, const set<string> &excludedWords) {
    size_t numFiles = files.size();
    vector<vector<double>> similarityMatrix(numFiles, vector<double>(numFiles, 0));

    vector<vector<pair<string, double>>> allFrequencies;
    for (const string &file : files) {
        allFrequencies.push_back(getFrequentWords(file, excludedWords));
    }

    for (size_t i = 0; i < numFiles; ++i) {
        for (size_t j = i + 1; j < numFiles; ++j) {
            double sim = computeSimilarity(allFrequencies[i], allFrequencies[j]);
            similarityMatrix[i][j] = sim;
            similarityMatrix[j][i] = sim;
        }
    }

    return similarityMatrix;
}

vector<tuple<double, int, int>> getTopSimilarPairs(const vector<vector<double>> &matrix, int topN = 10) {
    priority_queue<tuple<double, int, int>> pq;
    size_t numFiles = matrix.size();

    for (size_t i = 0; i < numFiles; ++i) {
        for (size_t j = i + 1; j < numFiles; ++j) {
            pq.push({matrix[i][j], i, j});
        }
    }

    vector<tuple<double, int, int>> topPairs;
    for (int i = 0; i < topN && !pq.empty(); ++i) {
        topPairs.push_back(pq.top());
        pq.pop();
    }

    return topPairs;
}

vector<string> getBookFiles(const string &folderPath) {
    vector<string> files;
    for (const auto &entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".txt") {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

int main() {
    set<string> excludedWords = {"A", "AND", "AN", "OF", "IN", "THE"};
    string folderPath = "Book-Txt/Book-Txt/";

    vector<string> files = getBookFiles(folderPath);

    vector<vector<double>> similarityMatrix = buildSimilarityMatrix(files, excludedWords);
    vector<tuple<double, int, int>> topPairs = getTopSimilarPairs(similarityMatrix);

    cout << "Top Similar Book Pairs:\n";
    for (const auto &pair : topPairs) {
        double similarity;
        int book1, book2;
        tie(similarity, book1, book2) = pair;
        cout << fs::path(files[book1]).filename().string() << " and " 
             << fs::path(files[book2]).filename().string() << " have similarity: " 
             << fixed << setprecision(4) << similarity << endl;
    }

    return 0;
}
