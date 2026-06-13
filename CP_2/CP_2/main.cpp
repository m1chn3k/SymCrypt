#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>

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
    {"ы", 26}, {"Ы", 26}, {"ь", 27}, {"Ь", 27}, {"ъ", 27}, {"Ъ", 27},
    {"э", 28}, {"Э", 28}, {"ю", 29}, {"Ю", 29}, {"я", 30}, {"Я", 30}
};

string rev_alf[] = { "а","б","в","г","д","е","ж","з","и","й","к","л","м","н","о","п",
                    "р","с","т","у","ф","х","ц","ч","ш","щ","ы","ь","э","ю","я" };

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

double calc_ic(const vector<int>& txt) {
    long long f[32] = { 0 };
    for (int x : txt) {
        if (x >= 0 && x < 32) f[x]++;
    }
    double ic = 0;
    int n = txt.size();
    if (n <= 1) return 0;
    for (int i = 0; i < 32; i++) {
        ic += (double)f[i] * (f[i] - 1);
    }
    return ic / ((double)n * (n - 1));
}

vector<int> crypt(const vector<int>& txt, const vector<int>& key, int mode) {
    vector<int> res(txt.size());
    int kl = key.size();
    if (kl == 0) return res;
    for (size_t i = 0; i < txt.size(); ++i) {
        res[i] = (txt[i] + mode * key[i % kl] + 32) % 32;
    }
    return res;
}

int main() {
    vector<int> small_t = read_file("text.txt");
    if (small_t.empty()) {
        cout << "1000-7" << endl;
        return 1;
    }

    cout << fixed << setprecision(5);
    cout << "step 1: ic open (small) = " << calc_ic(small_t) << "\n";

    int r_arr[] = { 2, 3, 4, 5, 10, 15, 20 };
    for (int r : r_arr) {
        vector<int> key(r);
        for (int i = 0; i < r; i++) key[i] = i % 32;
        vector<int> cipher = crypt(small_t, key, 1);
        cout << "r=" << r << " ic=" << calc_ic(cipher) << "\n";
    }
    cout << "\n";

    vector<int> var7 = read_file("var7.txt");
    if (var7.empty()) {
        cout << "dead inside" << endl;
        return 1;
    }

    cout << "step 2:\n";
    int best_r = 2;
    double max_ic = 0;
    for (int r = 2; r <= 30; ++r) {
        double avg_ic = 0;
        for (int i = 0; i < r; ++i) {
            vector<int> block;
            for (size_t j = i; j < var7.size(); j += r) {
                block.push_back(var7[j]);
            }
            avg_ic += calc_ic(block);
        }
        avg_ic /= r;
        cout << "r=" << r << " ic=" << avg_ic << "\n";
        if (avg_ic > max_ic) {
            max_ic = avg_ic;
            best_r = r;
        }
    }
    cout << "\nbest r = " << best_r << "\n\n";

    vector<int> book = read_file("Anno_Hideaki_Evangelion__Ranobe.txt");
    vector<double> p(32, 0);
    if (!book.empty()) {
        for (int x : book) {
            if (x >= 0 && x < 32) p[x]++;
        }
        for (int i = 0; i < 32; i++) p[i] /= book.size();
    }

    cout << "step 3:\n";
    vector<int> k_freq(best_r), k_mi(best_r);
    for (int i = 0; i < best_r; ++i) {
        long long f[32] = { 0 };
        int len = 0;
        for (size_t j = i; j < var7.size(); j += best_r) {
            if (var7[j] >= 0 && var7[j] < 32) {
                f[var7[j]]++;
                len++;
            }
        }

        if (len == 0) continue;

        int top_c = 0;
        for (int v = 1; v < 32; ++v) {
            if (f[v] > f[top_c]) top_c = v;
        }
        k_freq[i] = (top_c - 14 + 32) % 32;

        if (!book.empty()) {
            int best_g = 0;
            double max_m = -1;
            for (int g = 0; g < 32; ++g) {
                double m = 0;
                for (int t = 0; t < 32; ++t) {
                    m += p[t] * ((double)f[(t + g) % 32] / len);
                }
                if (m > max_m) {
                    max_m = m;
                    best_g = g;
                }
            }
            k_mi[i] = best_g;
        }
    }

    cout << "key (freq): ";
    for (int x : k_freq) cout << rev_alf[x];
    cout << "\nkey (mi):   ";
    if (!book.empty()) {
        for (int x : k_mi) cout << rev_alf[x];
    }
    else {
        cout << "need book.txt";
    }
    cout << "\n\n";

    vector<int> final_key = book.empty() ? k_freq : k_mi;
    vector<int> dec = crypt(var7, final_key, -1);
    ofstream out("decrypted.txt");
    for (int c : dec) out << rev_alf[c];
    out.close();

    return 0;
}