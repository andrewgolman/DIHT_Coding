#include <iostream>
#include <vector>
#include <complex>
using std::vector;
using std::string;


vector<int> getDecimalPolyn(string &a);
void getPows10 (int loglog);
typedef std::complex<double> comp;
vector< vector<int> > pows10;
int SHIFT = 4;
int LASTBITS = (1 << SHIFT) - 1;

int GO_TRIVIAL = 4;

vector<int> trivialConversion(vector<int> &a) {
    vector<int> res;
    long long num = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        num += a[i];
        num *= 10;
    }
    num /= 10;
    while (num > 0) {
        res.insert(res.begin(), num & LASTBITS);
        num >>= SHIFT;
    }
    return res;
}

size_t bitRev(int i, size_t n) {
    int res = 0;
    while (n > 0) {
        res += (i & 1);
        res <<= 1;
        i >>= 1;
        n >>= 1;
    }
    res >>= 2;
    return res;
}
void fft (vector<comp>& a, bool direct) {
    size_t n = a.size();
    for (size_t i = 0; i < n; ++i)
        if (i < bitRev(i, n))
            std::swap(a[i], a[bitRev(i, n)]);
    for (size_t l = 2; l <= n; l <<= 1) {
        double angle = 2*M_PI/l;
        if (!direct)
            angle *= -1;
        comp firstRoot(cos(angle), sin(angle));
        for (size_t i = 0; i <= n - l; i += l) {
            comp curRoot(1);
            comp a0, a1;
            for (size_t j = 0; j < l / 2; ++j) {
                a0 = a[i + j], a1 = a[i + j + l / 2] * curRoot;
                a[i + j] = a0 + a1;
                a[i + j + l / 2] = a0 - a1;
                curRoot *= firstRoot;
            }
        }
    }
    if (!direct)
        for (size_t i = 0; i < n; ++i)
            a[i] /= n;
}

void multy(vector<int> &a, vector<int> &b) {
    size_t sz = 1;
    while (sz < std::max(a.size(), b.size()))
        sz <<= 1;
    sz <<= 1;
    vector<comp> fa(sz - a.size(), 0), fb(sz - b.size(), 0);
    for (size_t i = 0; i < a.size(); ++i)
        fa.push_back(a[i]);
    for (size_t i = 0; i < b.size(); ++i)
        fb.push_back(b[i]);
    fft(fa, true);
    fft(fb, true);
    for (size_t i = 0; i < sz; ++i)
        fa[i] *= fb[i];
    fft(fa, false);
    a.resize(sz);
    for (size_t i = 0; i < sz; ++i)
        a[i] = int(fa[i].real() + 0.5);
    for (int i = sz - 1; i > 0; --i)
        if (a[i] > LASTBITS) {
            a[i - 1] += (a[i] >> SHIFT);
            a[i] &= LASTBITS;
        }
    a.pop_back();
    a.insert(a.begin(), 0);
    if (a[1] > LASTBITS) {
        a[0] += (a[1] >> SHIFT);
        a[1] &= LASTBITS;
    }
    int del = -1;
    while (del < int(a.size()) && !a[++del]);
    if (del > 0)
        a.erase(a.begin(), a.begin() + del);
}

void recurse(vector<int> &a, int log) {
    if (a.size() <= GO_TRIVIAL) {
        a = trivialConversion(a);
        return;
    }
    size_t hsz = (a.size() & ((1 << log)  - 1));
    if (!hsz)
        hsz = a.size() / 2;
    vector<int> first(a.begin(), a.begin() + hsz);
    vector<int> second(a.begin() + hsz, a.end());
    int logfirst = 0, fsz = 1;
    while (fsz < first.size()) {
        fsz <<= 1;
        ++logfirst;
    }
    recurse(first, logfirst - 1);
    recurse(second, log - 1);
    a.clear();
    if (!first.size()) {
        a = second;
        return;
    }
    multy(first, pows10[log]);
    a.resize(first.size(), 0);
    second.insert(second.begin(), first.size() - second.size(), 0);
    for (int i = a.size() - 1; i > 0; --i) {
        a[i] += first[i] + second[i];
        a[i - 1] += (a[i] >> SHIFT);
        a[i] &= LASTBITS;
    }
    a[0] += first[0] + second[0];
    if (a[0] > LASTBITS) {
        a.insert(a.begin(), 0);
        a[0] = (a[1] >> SHIFT);
        a[1] &= LASTBITS;
    }
}

string ntos(int n) {
    string s;
    while (n > 0) {
        s.insert(s.begin(), n % 10 + '0');
        n /= 10;
    }
    return s;
}

string getTest(int n) {
    string res;
    for (int i = 0; i < n; ++i) {
        int k = rand() % 10;
        res.push_back('0' + k);
    }
    return res;
}

int main() {
    string s;
    std::cin >> s;
    if (s == "0") {
        printf("0");
        return 0;
    }
    size_t sz = 1;
    int log = 0;
    while (sz < s.size()) {
        sz <<= 1;
        ++log;
    }
    vector<int> a = getDecimalPolyn(s);
    getPows10(log + 1);
    recurse(a, log - 1);
    vector<int> res;
    for (int i = 0; i < a.size(); ++i) {
        for (int j = SHIFT - 1; j >= 0; --j)
            res.push_back((a[i] >> j) & 1);
    }
    int i = -1;
    while (!res[++i]);
    for (; i < res.size(); ++i)
        printf("%d", res[i]);
    return 0;
}

vector<int> getDecimalPolyn(string &a) {
    vector<int> res;
    for (int i = 0; i < a.size(); ++i)
        res.push_back(a[i] - '0');
    return res;
}

void getPows10 (int loglog) {
    vector<int> cur(2);
    cur[0] = 1, cur[1] = 0;
    cur = trivialConversion(cur);
    pows10.clear();
    pows10.push_back(cur);
    for (int i = 1; i < loglog - 1; ++i) {
        cur.insert(cur.begin(), cur.size(), 0);
        multy(cur, cur);
        pows10.push_back(cur);
    }
}
