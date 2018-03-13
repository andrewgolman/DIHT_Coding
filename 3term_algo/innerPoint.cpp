/* TASK: determine whether a point lies inside the polygon. The polygon might not be convex.

INPUT: number of polygon vertices and their coordinate pairs, then number of queries and
    a coordinate pair for each query
OUTPUT: one of three answers for each query: INSIDE, BORDER or OUTSIDE, due to point's position
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <random>
#include <ctime>

template <typename Val>
struct CNode;

template <typename Val>
class Cartesian;

struct Point;
struct Line;
struct Segment;
struct PointData;


template <typename Val>
struct CNode {
    friend class Cartesian<Val>;
private:
    CNode *_left, *_right;
    Val _val;
    unsigned _prior;
    unsigned _size;

    explicit CNode(Val val) :
            _val(val),
            _size(1),
            _prior(rand()),
            _left(nullptr),
            _right(nullptr)
    {}

    void recalc() {
        _size = (_left ? _left->_size : 0) + (_right ? _right->_size : 0) + 1;
    }

    static unsigned size(CNode* n) {
        return n ? n->_size : 0;
    }

    static void split(CNode* n, unsigned pos, CNode*& lres, CNode*& rres) {
        if (!n) {
            lres = nullptr;
            rres = nullptr;
            return;
        }
        if (pos <= size(n->_left)) {
            rres = n;
            split(n->_left, pos, lres, n->_left);
        } else {
            lres = n;
            split(n->_right, pos - size(n->_left) - 1, lres->_right, rres);
        }
        if (lres)
            lres->recalc();
        if (rres)
            rres->recalc();
    }

    static void splitKey(CNode* n, Val val, CNode*& lres, CNode*& rres) {
        if (!n) {
            lres = nullptr;
            rres = nullptr;
            return;
        }
        if (val <= n-> _val) {
            rres = n;
            splitKey(n->_left, val, lres, n->_left);
        } else {
            lres = n;
            splitKey(n->_right, val, lres->_right, rres);
        }
        if (lres)
            lres->recalc();
        if (rres)
            rres->recalc();
    }

    static CNode* merge(CNode<Val>* lnode, CNode<Val>* rnode) {
        if (!lnode)
            return rnode;
        if (!rnode)
            return lnode;
        CNode<Val>* res;
        if (lnode->_prior < rnode->_prior) {
            res = lnode;
            res->_right = merge(res->_right, rnode);
        } else {
            res = rnode;
            res->_left = merge(lnode, res->_left);
        }
        res->recalc();
        return res;
    }

    static void destroy(CNode* n) {
        if (!n)
            return;
        destroy(n->_left);
        destroy(n->_right);
        delete n;
    }
};

template <typename Val>
class Cartesian {
    typedef CNode<Val>* NodePtr;
    typedef CNode<Val> Node;
    NodePtr _root;
public:
    Cartesian() : _root(nullptr) {}

    unsigned size() const {
        return Node::size(_root);
    }

    unsigned insert(Val val) {
        NodePtr newnode = new Node(val);
        if (!_root) {
            _root = newnode;
            return 0;
        }
        NodePtr tmp;
        Node::splitKey(_root, val, tmp, _root);
        unsigned ans = Node::size(tmp);
        tmp = Node::merge(tmp, newnode);
        _root = Node::merge(tmp, _root);
        return ans;
    }

    void erase(Val val) {
        NodePtr left, mid, right;
        Node::splitKey(_root, val, left, mid);
        Node::split(mid, 1, mid, right);
        delete mid;
        _root = Node::merge(left, right);
    }

    Val at(unsigned pos) {
        NodePtr left, mid, right;
        Node::split(_root, pos, left, mid);
        Node::split(mid, 1, mid, right);
        Val ans = mid->_val;
        right = Node::merge(mid, right);
        _root = Node::merge(left, right);
        return ans;
    }

    ~Cartesian() {
        Node::destroy(_root);
    }
};

struct Point {
    long long x, y;
    bool compare(const Point& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
    Point operator-(const Point& p) const {
        return Point{x - p.x, y - p.y};
    }
};

struct Line {
    long long a, b, c;
    Line(const Point& p, const Point& q) {
        a = p.y - q.y;
        b = q.x - p.x;
        c = a * p.x + b * p.y;
    }
    long long applyEquation(const Point& p) const {
        return a * p.x + b * p.y - c;
    }
};


struct Segment {
    Point a, b;
    Segment() {};
    Segment(Point a0, Point b0) : a(a0), b(b0) {};

    static bool intersectX(long long a, long long b, long long c, long long d) {
        if (a > b)
            std::swap(a, b);
        if (c > d)
            std::swap(c, d);
        return std::max(a, c) <= std::min(b, d);
    }

    static long long vProduct(const Point& a, const Point& b, const Point& c) {
        Point ac = a - c;
        Point bc = b - c;
        return ac.x * bc.y - ac.y * bc.x;
    }

    bool intersect(const Segment& s) const {
        if (!intersectX(a.x, b.x, s.a.x, s.b.x))
            return false;
        if (!intersectX(a.y, b.y, s.a.y, s.b.y))
            return false;
        if (vProduct(a, b, s.a) * vProduct(a, b, s.b) > 0)
            return false;
        if (vProduct(s.a, s.b, a) * vProduct(s.a, s.b, b) > 0)
            return false;
        return true;
    }


    long long positionToLine(const Segment& s) const {
        Line line(a, b);
        long long p = (a.x <= s.a.x && s.a.x <= b.x) ? line.applyEquation(s.a) : 0;
        long long q = (a.x <= s.b.x && s.b.x <= b.x) ? line.applyEquation(s.b) : 0;
        return p + q;
    }

    bool compare (const Segment& s) const {
        long long pl1 = positionToLine(s);
        long long pl2 = s.positionToLine(*this);
        if (pl1 != 0)
            return pl1 > 0;
        if (pl2 != 0)
            return pl2 < 0;
        return a.x < s.a.x;
    }

    bool operator <= (const Segment& s) const {
        return !(s.compare(*this));
    }

    bool isVertical() const {
        return a.x == b.x;
    }
};

enum EEventType {EET_OPEN, EET_QUERY, EET_CLOSE};
enum EAnswers {EA_INSIDE, EA_BORDER, EA_OUTSIDE};

std::string eAnswersToString(EAnswers a) {
    if (a == EA_INSIDE)
        return "INSIDE";
    if (a == EA_BORDER)
        return "BORDER";
    return "OUTSIDE";
}

struct Event {
    Point p;
    EEventType type;
    size_t id;

    Event(const Point& p0, EEventType e, size_t n) : p(p0), type(e), id(n) {};

    bool operator < (const Event& e) const {
        if  (p.compare(e.p))
            return true;
        if (p.x == e.p.x && p.y == e.p.y)
            return type < e.type;
        return false;
    }
};

struct PointData {
    std::vector<Point> polygon;
    std::vector<Point> queries;

    void read() {
        size_t n, k;
        std::cin >> n;
        polygon.resize(n);
        for (size_t i = 0; i < n; ++i) {
            std::cin >> polygon[i].x >> polygon[i].y;
        }
        std::cin >> k;
        queries.resize(k);
        for (size_t i = 0; i < k; ++i) {
            std::cin >> queries[i].x >> queries[i].y;
        }
    }

    void constructEventsAndSegments(std::vector<Event>& events, std::vector<Segment>& segments) const {
        size_t n = polygon.size();
        for (size_t i = 0; i < n; ++i) {
            size_t ind1 = i, ind2 = (i+1) % n;
            if (polygon[ind2].compare(polygon[ind1]))
                std::swap(ind1, ind2); // events and segments are constructed in the same function to avoid making this check twice
            events.emplace_back(polygon[ind1], EET_OPEN, i);
            events.emplace_back(polygon[ind2], EET_CLOSE, i);
            segments.emplace_back(polygon[ind1], polygon[ind2]);
        }
        for (size_t i = 0; i < queries.size(); ++i) {
            events.emplace_back(queries[i], EET_QUERY, i);
        }
        sort(events.begin(), events.end());
    }

};

std::vector<EAnswers> solve(const PointData& pointData) {
    std::vector<Event> events;
    std::vector<Segment> segments;
    pointData.constructEventsAndSegments(events, segments);

    Cartesian<Segment> tree;
    std::vector<EAnswers> answers(pointData.queries.size());
    int inVertical = 0;

    for (Event e : events) {
        if (e.type == EET_OPEN) {
            if (segments[e.id].isVertical())
                ++inVertical;
            else
                tree.insert(segments[e.id]);
        }
        else if (e.type == EET_CLOSE) {
            if (segments[e.id].isVertical())
                --inVertical;
            else
                tree.erase(segments[e.id]);
        }
        else {
            Segment queryPoint{e.p, e.p};
            size_t pos = tree.insert(queryPoint);
            answers[e.id] = pos % 2 ? EA_INSIDE : EA_OUTSIDE;

            if (pos > 0) {
                Segment prev = tree.at(pos-1);
                if (queryPoint.intersect(prev))
                    answers[e.id] = EA_BORDER;
            }
            if (pos + 1 < tree.size()) {
                Segment next = tree.at(pos+1);
                if (queryPoint.intersect(next))
                    answers[e.id] = EA_BORDER;
            }
            if (inVertical)
                answers[e.id] = EA_BORDER;
            tree.erase(queryPoint);
        }
    }
    return answers;
}


void output(std::vector<EAnswers> answers) {
    for (EAnswers i : answers)
        std::cout << eAnswersToString(i) << std::endl;
}

int main() {
    srand(time(0));
    int tests;
    std::cin >> tests;
    for (int test = 0; test < tests; ++test) {
        PointData pointData;
        pointData.read();
        std::vector<EAnswers> answers = solve(pointData);
        output(answers);
    }
    return 0;
}

