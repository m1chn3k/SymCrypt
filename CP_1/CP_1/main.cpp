#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <Windows.h>

using namespace std;

unordered_map<string, int> alf_with = {
    {"а", 0},  {"А", 0},  {"б", 1},  {"Б", 1},  {"в", 2},  {"В", 2},
    {"г", 3},  {"Г", 3},  {"д", 4},  {"Д", 4},  {"е", 5},  {"Е", 5},
    {"ё", 5},  {"Ё", 5},  {"ж", 6},  {"Ж", 6},  {"з", 7},  {"З", 7},
    {"и", 8},  {"И", 8},  {"й", 9},  {"Й", 9},  {"к", 10}, {"К", 10},
    {"л", 11}, {"Л", 11}, {"м", 12}, {"М", 12}, {"н", 13}, {"Н", 13},
    {"о", 14}, {"О", 14}, {"п", 15}, {"П", 15}, {"р", 16}, {"Р", 16},
    {"с", 17}, {"С", 17}, {"т", 18}, {"Т", 18}, {"у", 19}, {"У", 19},
    {"ф", 20}, {"Ф", 20}, {"х", 21}, {"Х", 21}, {"ц", 22}, {"Ц", 22},
    {"ч", 23}, {"Ч", 23}, {"ш", 24}, {"Ш", 24}, {"щ", 25}, {"Щ", 25},
    {"ъ", 26}, {"Ъ", 26}, {"ы", 27}, {"Ы", 27}, {"ь", 28}, {"Ь", 28},
    {"э", 29}, {"Э", 29}, {"ю", 30}, {"Ю", 30}, {"я", 31}, {"Я", 31},
    {" ", 32}
};

unordered_map<string, int> alf_no = {
    {"а", 0},  {"А", 0},  {"б", 1},  {"Б", 1},  {"в", 2},  {"В", 2},
    {"г", 3},  {"Г", 3},  {"д", 4},  {"Д", 4},  {"е", 5},  {"Е", 5},
    {"ё", 5},  {"Ё", 5},  {"ж", 6},  {"З", 7},  {"з", 7},  {"З", 7},
    {"и", 8},  {"И", 8},  {"й", 9},  {"Й", 9},  {"к", 10}, {"К", 10},
    {"л", 11}, {"Л", 11}, {"м", 12}, {"М", 12}, {"н", 13}, {"Н", 13},
    {"о", 14}, {"О", 14}, {"п", 15}, {"П", 15}, {"р", 16}, {"Р", 16},
    {"с", 17}, {"С", 17}, {"т", 18}, {"Т", 18}, {"у", 19}, {"У", 19},
    {"ф", 20}, {"Ф", 20}, {"х", 21}, {"Х", 21}, {"ц", 22}, {"Ц", 22},
    {"ч", 23}, {"Ч", 23}, {"ш", 24}, {"Ш", 24}, {"щ", 25}, {"Щ", 25},
    {"ъ", 26}, {"Ъ", 26}, {"ы", 27}, {"Ы", 27}, {"ь", 28}, {"Ь", 28},
    {"э", 29}, {"Э", 29}, {"ю", 30}, {"Ю", 30}, {"я", 31}, {"Я", 31}
};

string rev_with[] = {
    "а","б","в","г","д","е","ж","з","и","й","к","л","м","н","о","п",
    "р","с","т","у","ф","х","ц","ч","ш","щ","ъ","ы","ь","э","ю","я"," "
};

string rev_no[] = {
    "а","б","в","г","д","е","ж","з","и","й","к","л","м","н","о","п",
    "р","с","т","у","ф","х","ц","ч","ш","щ","ъ","ы","ь","э","ю","я"
};

double calc_h(const vector<long long>& f, int n) {
    long long total = 0;
    for (long long c : f) total += c;
    if (total == 0) return 0.0;
    double h = 0.0;
    for (long long c : f) {
        if (c > 0) {
            double p = static_cast<double>(c) / total;
            h -= p * log2(p);
        }
    }
    return h / n;
}

void print_top_monograms(const vector<long long>& f, string rev[], int size) {
    vector<pair<long long, string>> temp;
    for (int i = 0; i < size; ++i) {
        if (f[i] > 0) temp.push_back({ f[i], rev[i] });
    }
    sort(temp.rbegin(), temp.rend());

    cout << "Top letters (sorted by frequency):\n";
    for (const auto& p : temp) {
        cout << "'" << p.second << "': " << p.first << "  ";
    }
    cout << "\n\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    ifstream file("Anno_Hideaki_Evangelion__Ranobe.txt", ios::binary);
    if (!file.is_open()) {
        cout << "1000-7" << endl;
        return 1;
    }

    vector<string> txt_w;
    vector<string> txt_n;
    char ch;
    string prev = "";

    while (file.get(ch)) {
        string sym = "";
        unsigned char uc = static_cast<unsigned char>(ch);
        if (uc == 0xD0 || uc == 0xD1) {
            sym += ch;
            char nextCh;
            if (file.get(nextCh)) sym += nextCh;
        }
        else if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') {
            sym = " ";
        }
        else {
            continue;
        }

        if (alf_with.count(sym)) {
            if (sym == " " && prev == " ") continue;
            txt_w.push_back(sym);
            if (sym != " ") txt_n.push_back(sym);
            prev = sym;
        }
    }
    file.close();

    if (txt_w.empty()) {
        cout << "dead inside" << endl;
        return 1;
    }

    vector<long long> f1_w(33, 0), f2_w_o(33 * 33, 0), f2_w_no(33 * 33, 0);
    for (size_t i = 0; i < txt_w.size(); ++i) {
        f1_w[alf_with[txt_w[i]]]++;
        if (i < txt_w.size() - 1) {
            int id1 = alf_with[txt_w[i]];
            int id2 = alf_with[txt_w[i + 1]];
            f2_w_o[id1 * 33 + id2]++;
            if (i % 2 == 0) f2_w_no[id1 * 33 + id2]++;
        }
    }

    vector<long long> f1_n(32, 0), f2_n_o(32 * 32, 0), f2_n_no(32 * 32, 0);
    for (size_t i = 0; i < txt_n.size(); ++i) {
        f1_n[alf_no[txt_n[i]]]++;
        if (i < txt_n.size() - 1) {
            int id1 = alf_no[txt_n[i]];
            int id2 = alf_no[txt_n[i + 1]];
            f2_n_o[id1 * 32 + id2]++;
            if (i % 2 == 0) f2_n_no[id1 * 32 + id2]++;
        }
    }

    cout << fixed << setprecision(5);

    cout << "=== WITH SPACES ===\n";
    print_top_monograms(f1_w, rev_with, 33);
    double h1_w = calc_h(f1_w, 1);
    double h2_w_o = calc_h(f2_w_o, 2);
    double h2_w_no = calc_h(f2_w_no, 2);
    cout << "h1: " << h1_w << " r: " << (1.0 - (h1_w / log2(33))) * 100.0 << "%\n";
    cout << "h2 (overlap): " << h2_w_o << " r: " << (1.0 - (h2_w_o / log2(33))) * 100.0 << "%\n";
    cout << "h2 (no overlap): " << h2_w_no << " r: " << (1.0 - (h2_w_no / log2(33))) * 100.0 << "%\n\n";

    cout << "=== NO SPACES ===\n";
    print_top_monograms(f1_n, rev_no, 32);
    double h1_n = calc_h(f1_n, 1);
    double h2_n_o = calc_h(f2_n_o, 2);
    double h2_n_no = calc_h(f2_n_no, 2);
    cout << "h1: " << h1_n << " r: " << (1.0 - (h1_n / log2(32))) * 100.0 << "%\n";
    cout << "h2 (overlap): " << h2_n_o << " r: " << (1.0 - (h2_n_o / log2(32))) * 100.0 << "%\n";
    cout << "h2 (no overlap): " << h2_n_no << " r: " << (1.0 - (h2_n_no / log2(32))) * 100.0 << "%\n";

    ofstream csv("matrix.csv");
    for (int i = 0; i < 33; ++i) {
        for (int j = 0; j < 33; ++j) {
            csv << f2_w_o[i * 33 + j] << (j == 32 ? "" : ",");
        }
        csv << "\n";
    }
    csv.close();

    return 0;
}