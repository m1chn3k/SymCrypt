#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <windows.h>

using namespace std;

unordered_map<string, int> alf = {
    {"а", 0}, {"А", 0}, {"б", 1}, {"Б", 1}, {"в", 2}, {"В", 2},
    {"г", 3}, {"Г", 3}, {"д", 4}, {"Д", 4}, {"е", 5}, {"Е", 5},
    {"ё", 5}, {"Ё", 5}, {"ж", 6}, {"Ж", 6}, {"з", 7}, {"З", 7},
    {"и", 8}, {"И", 8}, {"й", 9}, {"Й", 9}, {"к", 10}, {"К", 10},
    {"л", 11}, {"Л", 11}, {"м", 12}, {"М", 12}, {"н", 13}, {"Н", 13},
    {"о", 14}, {"О", 14}, {"п", 15}, {"П", 15}, {"р", 16}, {"Р", 16},
    {"с", 17}, {"С", 17}, {"т", 18}, {"Т", 18}, {"у", 19}, {"У", 19},
    {"ф", 20}, {"Ф", 20}, {"х", 21}, {"Х", 21}, {"ц", 22}, {"Ц", 22},
    {"ч", 23}, {"Ч", 23}, {"ш", 24}, {"Ш", 24}, {"щ", 25}, {"Щ", 25},
    {"ь", 26}, {"Ь", 26}, {"ъ", 26}, {"Ъ", 26}, {"ы", 27}, {"Ы", 27},
    {"э", 28}, {"Э", 28}, {"ю", 29}, {"Ю", 29}, {"я", 30}, {"Я", 30}
};

string rev_alf[] = {
    "а","б","в","г","д","е","ж","з","и","й","к","л","м","н","о","п",
    "р","с","т","у","ф","х","ц","ч","ш","щ","ь","ы","э","ю","я"
};

long long extGCD(long long a, long long b, long long& x, long long& y) {
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    long long x1, y1;
    long long d = extGCD(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

long long modInverse(long long a, long long m) {
    long long x, y;
    long long g = extGCD(a, m, x, y);
    if (g != 1) return -1;
    return (x % m + m) % m;
}

vector<long long> solveLinearCongruence(long long a, long long b, long long n) {
    vector<long long> res;
    a = (a % n + n) % n;
    b = (b % n + n) % n;

    long long x, y;
    long long d = extGCD(a, n, x, y);

    if (b % d != 0) return res;

    long long a1 = a / d;
    long long b1 = b / d;
    long long n1 = n / d;

    long long x0 = (b1 * modInverse(a1, n1)) % n1;
    if (x0 < 0) x0 += n1;

    for (long long i = 0; i < d; ++i) {
        res.push_back((x0 + i * n1) % n);
    }
    return res;
}

vector<int> read_file(string name) {
    vector<int> res;
    ifstream in(name, ios::binary);
    if (!in.is_open()) return res;

    char c;
    while (in.get(c)) {
        string s = "";
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc == 0xD0 || uc == 0xD1) {
            s += c;
            char nc;
            if (in.get(nc)) s += nc;
        }
        else {
            s += c;
        }

        if (alf.count(s)) {
            res.push_back(alf[s]);
        }
    }
    in.close();
    return res;
}

vector<int> get_top_5_bigrams(const vector<int>& txt) {
    map<int, int> freq;
    for (size_t i = 0; i + 1 < txt.size(); i += 2) {
        int X = txt[i] * 31 + txt[i + 1];
        freq[X]++;
    }

    vector<pair<int, int>> sorted_bg;
    for (auto const& pair : freq) {
        sorted_bg.push_back({ pair.second, pair.first });
    }
    sort(sorted_bg.rbegin(), sorted_bg.rend());

    vector<int> top5;
    for (size_t i = 0; i < 5 && i < sorted_bg.size(); ++i) {
        top5.push_back(sorted_bg[i].second);
        int X = sorted_bg[i].second;
        cout << rev_alf[X / 31] << rev_alf[X % 31] << " (" << X << ") - " << sorted_bg[i].first << "\n";
    }
    return top5;
}

vector<int> decrypt_affine(const vector<int>& cipher, long long a, long long b) {
    vector<int> plain;
    long long a_inv = modInverse(a, 961);
    if (a_inv == -1) return plain;

    for (size_t i = 0; i + 1 < cipher.size(); i += 2) {
        long long Y = cipher[i] * 31 + cipher[i + 1];
        long long X = (a_inv * (Y - b + 961)) % 961;
        plain.push_back(X / 31);
        plain.push_back(X % 31);
    }
    return plain;
}

bool is_readable(const vector<int>& dec) {
    if (dec.empty()) return false;

    long long f[31] = { 0 };
    for (int x : dec) {
        if (x >= 0 && x < 31) f[x]++;
    }

    int total = dec.size();
    double freq_o = (double)f[14] / total;
    double freq_a = (double)f[0] / total;
    double freq_e = (double)f[5] / total;

    if (freq_o < 0.07 || freq_a < 0.05 || freq_e < 0.05) return false;

    int bad_bigrams = 0;
    for (size_t i = 0; i + 1 < dec.size(); i++) {
        if ((dec[i] == 0 || dec[i] == 14 || dec[i] == 27 || dec[i] == 26) && dec[i + 1] == 26) {
            bad_bigrams++;
        }
    }

    if (bad_bigrams > 2) return false;

    return true;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    vector<int> var_txt = read_file("07.txt");
    if (var_txt.empty()) {
        cout << "1000-7\n";
        return 1;
    }

    cout << "Top 5 cipher bigrams:\n";
    vector<int> top_cipher = get_top_5_bigrams(var_txt);
    cout << "\n";

    vector<int> top_plain = { 545, 417, 572, 403, 168 };

    cout << "Bruteforcing keys...\n";

    bool found = false;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (i == j) continue;

            for (int k = 0; k < 5; k++) {
                for (int l = 0; l < 5; l++) {
                    if (k == l) continue;

                    long long dY = top_cipher[i] - top_cipher[j];
                    long long dX = top_plain[k] - top_plain[l];

                    vector<long long> a_candidates = solveLinearCongruence(dX, dY, 961);

                    for (long long a : a_candidates) {
                        long long tmp_x, tmp_y;
                        if (extGCD(a, 31, tmp_x, tmp_y) != 1) continue;

                        long long b = (top_cipher[i] - a * top_plain[k]) % 961;
                        if (b < 0) b += 961;

                        vector<int> dec = decrypt_affine(var_txt, a, b);

                        if (is_readable(dec)) {
                            cout << ">>> WIN! Key found: a = " << a << ", b = " << b << " <<<\n";
                            cout << "Decrypted text preview:\n";
                            for (size_t c = 0; c < min((size_t)100, dec.size()); ++c) {
                                cout << rev_alf[dec[c]];
                            }
                            cout << "...\n\n";

                            ofstream out("decrypted.txt");
                            for (int c : dec) out << rev_alf[c];
                            out.close();

                            found = true;
                        }
                    }
                }
            }
        }
    }

    if (!found) {
        cout << "Key not found. Mozhno poprobovat pomeyat 'ы' i 'ь' mestami.\n";
    }

    return 0;
}