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

    vector<pair<string, double>
