/*
TASK: https://ejudge.lksh.ru/archive/2015/08/A/11/problems-11.html , problem D.
Implemented Push-Relabel optimized algorithm and Malhotra-Kumar-Maheshwari blocking flow algorithm 
*/

#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <cstdlib>
#include <queue>
#include <cassert>

const int INF = 2000000;
typedef std::vector<std::vector<int>> vvint;
typedef std::vector<int> vint;

struct Edge {
    size_t end;
    int flow;
    int capacity;
    int preflow;
    Edge(int e, int c) : end(e), flow(0), capacity(c) {};

    int residual() const {
        return capacity - flow;
    }
};


class Network {
private:
    std::vector<Edge> edges_;
    vvint adjList_;

public:
    Network(const vvint& g) {
        adjList_ = vvint(g.size());
        for (int i = 0; i < g.size(); ++i)
            for (int j = 0; j < g[i].size(); ++j)
                if (g[i][j] > 0) {
                    adjList_[i].push_back(edges_.size());
                    edges_.push_back(Edge(j, g[i][j]));
                    adjList_[j].push_back(edges_.size());
                    edges_.push_back(Edge(i, 0));
                }
    }

    std::vector<Edge>& edges() {
        return edges_;
    }

    const vint& incident(int v) const {
        return adjList_[v];
    }

    int size() const {
        return adjList_.size();
    }
};


class AlgoClass {
public:
    Network nw;
    AlgoClass(Network& nw_) : nw(nw_) {};
    virtual int maxFlow() = 0;
};

class PreFlow : AlgoClass {

    vint heights;
    std::list<size_t> vertices;
    vint pointers;
    vint overflow;
    std::list<size_t>::iterator currentVertex;

public:
    PreFlow(Network& nw_) : AlgoClass(nw_) {}

    int maxFlow() override {
        pointers.assign(nw.size(), 0);
        heights.assign(nw.size(), 0);
        overflow.assign(nw.size(), 0);
        overflow[0] = INF;
        heights[0] = nw.size();

        for (size_t i = 1; i < nw.size() - 1; ++i)
            vertices.push_back(i);
        currentVertex = vertices.begin();

        for (int i = 0; i < nw.incident(0).size(); ++i)
            push(0, nw.incident(0)[i]);

        while (currentVertex != vertices.end()) {
            size_t curHeight = heights[*currentVertex];
            discharge(*currentVertex);
            if (heights[*currentVertex] != curHeight)
                moveToFront();
            ++currentVertex;
        }

        return overflow.back();
    }

private:
    void moveToFront() {
        vertices.push_front(*currentVertex);
        vertices.erase(currentVertex);
        currentVertex = vertices.begin();
    }

    void discharge(int v) {
        while (overflow[v] > 0) {
            size_t next = pointers[v];
            if (next == nw.incident(v).size()) {
                relabel(v);
                pointers[v] = 0;
            } else {
                size_t nextEdgeNumber = nw.incident(v)[next];
                if (nw.edges()[nextEdgeNumber].residual() && heights[v] == heights[nw.edges()[nextEdgeNumber].end] + 1)
                    push(v, nextEdgeNumber);
                else
                    ++pointers[v];
            }
        }
    }

    void push(size_t v, size_t edgeNumber) {
        int flow = std::min(nw.edges()[edgeNumber].residual(), overflow[v]);
        nw.edges()[edgeNumber].flow += flow;
        nw.edges()[edgeNumber ^ 1].flow -= flow;
        overflow[v] -= flow;
        overflow[nw.edges()[edgeNumber].end] += flow;
    }

    void relabel(int v) {
        size_t minHeight = nw.size()+1;
        for (auto i = 0; i < nw.incident(v).size(); ++i) {
            Edge next = nw.edges()[nw.incident(v)[i]];
            if (next.residual() && heights[next.end] < minHeight)
                minHeight = heights[next.end];
        }
        heights[v] = minHeight + 1;
    }
};



class MKM : AlgoClass {
    friend class Network;

    vint layers;
    vint inCap, outCap;
    vint overflow;

public:
    MKM(Network& nw_) : AlgoClass(nw_) {};

    int maxFlow() override {
        int answer = 0;
        overflow.assign(nw.size(), 0);
        while (setLayers()) {
            setPotentials();
            while (ifReachable()) {
                int nextFlow = INF;
                int nextV = -1;
                for (int i = 0; i < nw.size(); ++i)
                    if (potential(i) < nextFlow && layers[i] >= 0) {
                        nextV = i;
                        nextFlow = potential(i);
                    }
                if (nextV == -1)
                    break;
                if (potential(nextV) > 0) {
                    answer += nextFlow;
                    pushFlow(nextV);
                }
                deleteVertex(nextV);
            }
        }
        return answer;
    }

private:
    int potential(int v) {
        return std::min(inCap[v], outCap[v]);
    }

    bool setLayers() {
        layers = vint(nw.size(), -INF);
        std::queue<int> q;
        q.push(0);
        layers[0] = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int i = 0; i < nw.incident(v).size(); ++i) {
                Edge e = nw.edges()[nw.incident(v)[i]];
                if (!e.residual())
                    continue;
                if (layers[e.end] == -INF) {
                    layers[e.end] = layers[v] + 1;
                    if (e.end == layers.size() - 1)
                        return true;
                    q.push(e.end);
                }
            }
        }
        return false;
    }

    void setPotentials() {
        inCap.assign(nw.size(), 0);
        outCap.assign(nw.size(), 0);

        for (int i = 0; i < nw.size(); ++i) {
            for (int j = 0; j < nw.incident(i).size(); ++j) {
                int to = nw.edges()[nw.incident(i)[j]].end;
                if (layers[i] + 1 == layers[to]) {
                    int cap = nw.edges()[nw.incident(i)[j]].residual();
                    inCap[to] += cap;
                    outCap[i] += cap;
                }
            }
        }
        inCap.front() = INF;
        outCap.back() = INF;
    }

    bool ifReachable() {
        std::queue<int> q;
        q.push(0);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int i = 0; i < nw.incident(v).size(); ++i) {
                Edge e = nw.edges()[nw.incident(v)[i]];
                if (!e.residual())
                    continue;
                if (e.end == layers.size()-1)
                    return true;
                if (layers[e.end] == layers[v] + 1) {
                    q.push(e.end);
                }
            }
        }
        return false;
    }

    void deleteVertex(int v) {
        if (layers[v] == -INF)
            return;
        int h = layers[v];
        layers[v] = -INF;
        for (int i = 0; i < nw.incident(v).size(); ++i) {
            int eout = nw.incident(v)[i], ein = nw.incident(v)[i] ^ 1;
            int to = nw.edges()[eout].end;
            if (layers[to] == h + 1) {
                inCap[to] -= nw.edges()[eout].residual();
                if (inCap[to] == 0)
                    deleteVertex(to);
            }
            else if (layers[to] == h - 1) {
                outCap[to] -= nw.edges()[ein].residual();
                if (outCap[to] == 0)
                    deleteVertex(to);
            }
        }
    }

    void pushFlow(int v) {
        int flow = potential(v);
        if (v != nw.size() - 1)
            pushUpDown(v, flow, 1);
        if (v != 0)
            pushUpDown(v, flow, 0);
    }

    void pushUpDown(int v0, int flow0, bool down) {
        overflow[v0] = flow0;
        std::queue<int> q;
        q.push(v0);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int i = 0; i < nw.incident(v).size() && overflow[v] > 0; ++i) {
                int en = nw.incident(v)[i];
                int to = nw.edges()[en].end;
                if (layers[to] != layers[v] + (down ? 1 : -1))
                    continue;
                if (!down)
                    en ^= 1;
                int flow = std::min(nw.edges()[en].residual(), overflow[v]);
                if (!flow)
                    continue;
                overflow[v] -= flow;
                overflow[to] += flow;
                nw.edges()[en].flow += flow;
                nw.edges()[en ^ 1].flow -= flow;
                if (down) {
                    outCap[v] -= flow;
                    inCap[to] -= flow;
                    if (to != nw.size() - 1)
                        q.push(to);
                }
                else {
                    inCap[v] -= flow;
                    outCap[to] -= flow;
                    if (to != 0)
                        q.push(to);
                }
            }
        }
    }
};





int main() {
    size_t n;
    std::cin >> n;
    vint score(n+1);
    for (int i = 1; i <= n; ++i)
        std::cin >> score[i];

    // source = 0
    // target = n+1

    vvint g(n+2, vint(n+2, 0));

    for (int i = 1; i <= n; ++i) {
        if (score[i] >= 0)
            g[0][i] = score[i];
        else
            g[i][n+1] = -score[i];
    }

    for (int k, i = 1; i <= n; ++i) {
        std::cin >> k;
        for (int l, j = 0; j < k; ++j) {
            std::cin >> l;
            g[i][l] = INF;
        }
    }

    Network nw(g);
    int ans1 = PreFlow(nw).maxFlow();
    int ans2 = MKM(nw).maxFlow();
    assert(ans1 == ans2);
    
    int ans = -ans1;
    
    for (int i = 1; i <= n; ++i) {
        if (score[i] > 0)
            ans += score[i];
    }
    std::cout << ans << std::endl;

}
