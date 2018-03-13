/*
TASK: http://informatics.mccme.ru/moodle/mod/statements/view3.php?id=7841&chapterid=111743#1

Implemented construction of Suffix Automaton and Suffix Tree (Ukkonen algorithm).
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

typedef std::vector<int> intv;

struct RefrainData {
    long long value;
    size_t length, begin;
    const intv str;
    RefrainData(const intv& str_) : str(str_) {}

    void init(long long value_, size_t length_, size_t begin_) {
        value = value_;
        length = length_;
        begin = begin_;
    }

    void print() {
        std::cout << value << std::endl;
        std::cout << length << std::endl;
        for (size_t i = 0; i < length; ++i)
            std::cout << str[begin + i] << " ";
        std::cout << std::endl;
    }
};

template <size_t alphabetSize>
struct SufAutomatonNode {
    size_t len;
    int link;
    size_t end;
    intv next;

    SufAutomatonNode(size_t len_, size_t end_) : len(len_), end(end_), next(intv(alphabetSize, -1)), link(-1)  {}
    SufAutomatonNode(const SufAutomatonNode& node) :
            link(node.link), next(node.next), len(node.len), end(node.end) {}

};

template <size_t alphabetSize>
class SufAutomaton {
    std::vector<SufAutomatonNode<alphabetSize>> nodes;
    size_t last;

    void expand(size_t ind, const intv& str) {
        int nextChar = str[ind];
        size_t newNode = nodes.size();
        nodes.emplace_back(nodes[last].len + 1, ind);
        size_t curNode = last;
        last = newNode;

        while (curNode != -1 && nodes[curNode].next[nextChar] == -1) {
            nodes[curNode].next[nextChar] = newNode;
            curNode = nodes[curNode].link;
        }
        if (curNode == -1) {
            nodes[newNode].link = 0;
            return;
        }

        size_t curNext = nodes[curNode].next[nextChar];
        if (nodes[curNode].len + 1 == nodes[curNext].len) {
            nodes[newNode].link = curNext;
            return;
        }

        size_t clone = nodes.size();
        nodes.emplace_back(nodes[curNext]);
        nodes[clone].len = nodes[curNode].len + 1;
        while (curNode != -1) {
            if (nodes[curNode].next[nextChar] == curNext)
                nodes[curNode].next[nextChar] = clone;
            else
                break;
            curNode = nodes[curNode].link;
        }
        nodes[newNode].link = nodes[curNext].link = clone;
    }

public:
    SufAutomaton(const intv& str) {
        nodes.emplace_back(0, 0);
        last = 0;
        for (size_t i = 0; i < str.size(); ++i)
            expand(i, str);
    }

    void refrain(RefrainData& refrainData) {
        std::vector<std::pair<int, size_t>> sortedNodes(nodes.size());
        for (size_t i = 0; i < nodes.size(); ++i)
            sortedNodes[i] = std::make_pair(-static_cast<int>(nodes[i].len), i);
        sort(sortedNodes.begin(), sortedNodes.end());

        std::vector<long long> visits(nodes.size(), 0);
        size_t curState = last;
        while (curState) {
            visits[curState] = 1;
            curState = nodes[curState].link;
        }

        for (auto i : sortedNodes) {
            for (int j : nodes[i.second].next)
                if (j != -1)
                    visits[i.second] += visits[j];
        }

        long long ansRefrain = 0;
        size_t ansState;
        for (size_t i = 0; i < nodes.size(); ++i)
            if (ansRefrain < nodes[i].len * visits[i]) {
                ansRefrain = nodes[i].len * visits[i];
                ansState = i;
            }

        refrainData.init(ansRefrain, nodes[ansState].len, nodes[ansState].end - nodes[ansState].len + 1);
    }
};

template <size_t alphabetSize>
struct SufTreeNode {
    size_t link;
    size_t begin, len;
    intv next;
    size_t leaves;
    long long dist;

    SufTreeNode(size_t begin, size_t len)
            : begin(begin), len(len), next(alphabetSize, 0), leaves(0) {}
};

template <size_t alphabetSize>
class SufTree {
    const size_t INF {10000000};
    std::vector<SufTreeNode<alphabetSize>> nodes;
    size_t strLen;
    size_t lastNotLeafNode, lastNotLeafPos;

    void goDown(size_t ind, const intv& str) {
        while (lastNotLeafPos > nodes[nodes[lastNotLeafNode].next[str[ind - lastNotLeafPos]]].len) {
            lastNotLeafNode = nodes[lastNotLeafNode].next[str[ind - lastNotLeafPos]];
            lastNotLeafPos -= nodes[lastNotLeafNode].len;
        }
    }

    void expand(size_t ind, const intv& str) {
        int newChar = str[ind];
        ++lastNotLeafPos;
        size_t last = 0;
        while (lastNotLeafPos > 0) {
            goDown(ind + 1, str);
            int nextGo = str[ind + 1 - lastNotLeafPos];
            int& nextV = nodes[lastNotLeafNode].next[nextGo];

            int nextChar = str[nodes[nextV].begin + lastNotLeafPos - 1];
            if (nextV == 0) {
                nextV = nodes.size();
                nodes.emplace_back(ind + 1 - lastNotLeafPos, INF);
                nodes[last].link = lastNotLeafNode;
                last = 0;
            } else if (nextChar == newChar) {
                nodes[last].link = lastNotLeafNode;
                return;
            } else {
                size_t newNode = nodes.size();
                nodes.emplace_back(nodes[nextV].begin, lastNotLeafPos - 1);
                nodes[newNode].next[newChar] = nodes.size();
                nodes.emplace_back(ind, INF);
                nodes[newNode].next[nextChar] = nextV;
                nodes[nextV].begin += lastNotLeafPos - 1;
                nodes[nextV].len -= lastNotLeafPos - 1;
                nextV = newNode;
                nodes[last].link = newNode;
                last = newNode;
            }

            if (lastNotLeafNode == 0)
                --lastNotLeafPos;
            else
                lastNotLeafNode = nodes[lastNotLeafNode].link;
        }
    }

    size_t dfsCountingLeaves(size_t curNode) {
        for (size_t i = 0; i < alphabetSize; ++i) {
            size_t nextNode = nodes[curNode].next[i];
            if (nextNode > 0)
                nodes[curNode].leaves += dfsCountingLeaves(nextNode);
        }
        if (!nodes[curNode].leaves)
            nodes[curNode].leaves = 1;
        return nodes[curNode].leaves;
    }

    void dfsCountingDist(size_t curNode) {
        for (size_t i = 0; i < alphabetSize; ++i) {
            size_t nextNode = nodes[curNode].next[i];
            if (nextNode > 0) {
                nodes[nextNode].dist = nodes[curNode].dist +
                        std::min(strLen - nodes[nextNode].begin - 1, nodes[nextNode].len);
                dfsCountingDist(nextNode);
            }
        }
    }

public:
    SufTree(intv str) {
        str.push_back(0);
        strLen = str.size();
        lastNotLeafNode = lastNotLeafPos = 0;
        nodes.emplace_back(0, 0);
        nodes[0].len = INF;
        for (size_t i = 0; i < strLen; ++i)
            expand(i, str);
    }

    void refrain(RefrainData& refrainData) {
        dfsCountingLeaves(0);
        nodes[0].dist = 0;
        dfsCountingDist(0);
        long long ansRefrain = 0;
        size_t ansNode;
        for (size_t i = 0; i < nodes.size(); ++i)
            if (ansRefrain < nodes[i].leaves * nodes[i].dist) {
                ansRefrain = nodes[i].leaves * nodes[i].dist;
                ansNode = i;
            }
        size_t ansLast = std::min(nodes[ansNode].begin + nodes[ansNode].len, strLen - 1) - 1;
        refrainData.init(ansRefrain, nodes[ansNode].dist, ansLast - nodes[ansNode].dist + 1);
    }
};


int main() {
    size_t sLen, alphabetSize;
    std::cin >> sLen >> alphabetSize;
    const size_t ALPHABETSIZE = 11;
    intv str(sLen);
    for (int &i : str)
        std::cin >> i;

    RefrainData r1(str), r2(str);
    SufAutomaton<ALPHABETSIZE>(str).refrain(r1);
    SufTree<ALPHABETSIZE>(str).refrain(r2);
    assert(r1.value == r2.value);
    (rand() % 2 ? r1 : r2).print();
    return 0;
}
