#include <iostream>
#include <stdexcept>

template <typename Val>
class Cartesian;

template <typename Val>
struct CNode {
    friend class Cartesian<Val>;
private:
    CNode *_left, *_right;//, *_par;
    Val _val;
    Val _sum;
    unsigned _prior;
    unsigned _size;
    bool _reversed;
    bool _added, _assigned;
    Val _addVal, _assignVal;

    unsigned _increasing_tail; // размер наибольшего нестрого возрастающего куска в конце
    unsigned _decreasing_tail; // размер наибольшего нестрого (ибо next и prev должны быть обратными операциями) убывающего куска в конце
    unsigned _increasing_head; // в начале
    unsigned _decreasing_head;
    Val _val_leftest, _val_rightest; // крайние значения поддерева

    explicit CNode(Val val) :
            _val(val),
            _prior(unsigned(rand())),
            _size(1),
            _sum(val),
            _left(nullptr),
            _right(nullptr),
            //_par(nullptr),
            _reversed(false),
            _added(false),
            _assigned(false),

            _increasing_tail(1),
            _decreasing_head(1),
            _decreasing_tail(1),
            _increasing_head(1),
            _val_leftest(val),
            _val_rightest(val)
    {}


    void reverse() {
        _reversed = false;
        std::swap(_left, _right);
        if (_left)
            _left->_reversed ^= true;
        if (_right)
            _right->_reversed ^= true;
        std::swap(_increasing_head, _decreasing_tail);
        std::swap(_decreasing_head, _increasing_tail);
        std::swap(_val_leftest, _val_rightest);
    }

    void push() {
        if (_reversed)
            reverse();

        if (_assigned) {
            _assigned = false;
            _val = _assignVal;
            _sum = _assignVal * _size;
            _val_leftest = _val_rightest = _assignVal;
            if (_left) {
                _left->_added = false;
                _left->_assigned = true;
                _left->_assignVal = _assignVal;
            }
            if (_right) {
                _right->_added = false;
                _right->_assigned = true;
                _right->_assignVal = _assignVal;
            }
            _increasing_tail = _decreasing_head = _size;
            _increasing_head = _decreasing_tail = _size;
        }

        if (_added) {
            _added = false;
            _val += _addVal;
            _sum += _addVal * _size;
            _val_leftest += _addVal;
            _val_rightest += _addVal;
            if (_left) {
                if (_left->_added)
                    _left->_addVal += _addVal;
                else {
                    _left->_added = true;
                    _left->_addVal = _addVal;
                }
            }
            if (_right) {
                if (_right->_added)
                    _right->_addVal += _addVal;
                else {
                    _right->_added = true;
                    _right->_addVal = _addVal;
                }
            }
        }
    }


    void recalc() {
        _size = (_left ? _left->_size : 0) + (_right ? _right->_size : 0) + 1;
        if (_left) {
            _left->push();
            _val_leftest = _left->_val_leftest;
        } else
            _val_leftest = _val;
        if (_right) {
            _right->push();
            _val_rightest = _right->_val_rightest;
        } else
            _val_rightest = _val;
        _sum = (_left ? _left->_sum : Val()) + (_right ? _right->_sum : Val()) + _val;

        _increasing_head = _left ? _left->_increasing_head : 0;
        if (!_left || (_left->_increasing_head == _left->_size
                       && _left->_val_rightest <= _val)) {
            _increasing_head += 1;
            if (_right && _val <= _right->_val_leftest)
                _increasing_head += _right->_increasing_head;
        }

        _decreasing_head = _left ? _left->_decreasing_head : 0;
        if (!_left || (_left->_decreasing_head == _left->_size
                       && _left->_val_rightest >= _val)) {
            _decreasing_head += 1;
            if (_right && _val >= _right->_val_leftest)
                _decreasing_head += _right->_decreasing_head;
        }

        _increasing_tail = _right ? _right->_increasing_tail : 0;
        if (!_right || (_right->_increasing_tail == _right->_size
                        && _val <= _right->_val_leftest)) {
            _increasing_tail += 1;
            if (_left && _left->_val_rightest <= _val)
                _increasing_tail += _left->_increasing_tail;
        }

        _decreasing_tail = _right ? _right->_decreasing_tail : 0;
        if (!_right || (_right->_decreasing_tail == _right->_size
                        && _val >= _right->_val_leftest)) {
            _decreasing_tail += 1;
            if (_left && _left->_val_rightest >= _val)
                _decreasing_tail += _left->_decreasing_tail;
        }
    }


/// THIS IS A NEW FUNCTION
/// summation and permutation added

    template <typename U, class... Args>
    static void doSthInTheMiddle(CNode*& n, unsigned l, unsigned r, U f, Args&... args) {
        CNode *left, *mid, *right;
        split(n, r+1, mid, right);
        split(mid, l, left, mid);
        f(mid, args...);
        mid = merge(left, mid);
        n = merge(mid, right);
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
        n->push();
        if (pos <= size(n->_left)) {
            rres = n;
            if (n->_right)
                n->_right->push();
            split(n->_left, pos, lres, n->_left);
        } else {
            lres = n;
            if (n->_left)
                n->_left->push();
            split(n->_right, pos - size(n->_left) - 1, lres->_right, rres);
        }
        if (lres) {
            //lres->_par = nullptr;
            lres->recalc();
        }
        if (rres) {
            //rres->_par = nullptr;
            rres->recalc();
        }
    }


    static void splitKey(CNode* n, Val val, CNode*& lres, CNode*& rres, bool increasing) {
        if (!n) {
            lres = nullptr;
            rres = nullptr;
            return;
        }
        n->push();
        if ((increasing && val <= n-> _val)
            || (!increasing && val >= n->_val)) {
            rres = n;
            if (n->_right)
                n->_right->push();
            splitKey(n->_left, val, lres, n->_left, increasing);
        } else {
            lres = n;
            if (n->_left)
                n->_left->push();
            splitKey(n->_right, val, lres->_right, rres, increasing);
        }
        if (lres) {
            //lres->_par = nullptr;
            lres->recalc();
        }
        if (rres) {
            //rres->_par = nullptr;
            rres->recalc();
        }
    }

    static CNode* merge(CNode<Val>* lnode, CNode<Val>* rnode) {
        if (!lnode) {
            if (rnode)
                rnode->push();
            return rnode;
        }
        if (!rnode) {
            lnode->push();
            return lnode;
        }
        lnode->push();
        rnode->push();
        CNode<Val>* res;
        if (lnode->_prior < rnode->_prior) {
            res = lnode;
            res->_right = merge(res->_right, rnode);
        } else {
            res = rnode;
            res->_left = merge(lnode, res->_left);
        }
        //res->_par = nullptr;
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
public:
    static void inorder(CNode* n) {
        if (!n)
            return;
        n->push();
        n->recalc();
        inorder(n->_left);
        std::cout << n->_val << " ";
        inorder(n->_right);
    }
};

template <typename Val>
class Cartesian {
    typedef CNode<Val>* NodePtr;
    typedef CNode<Val> Node;
    NodePtr _root;
public:
    Cartesian() {
        _root = nullptr;
    }

    Cartesian(Val* begin, Val* end) {
        _root = nullptr;
        for (auto iter = begin; iter != end; ++iter)
            insert(*iter, unsigned(-1));
    }

    void insert(Val val, unsigned pos = unsigned(-1)) {
        NodePtr newnode = new Node(val);
        if (!_root) {
            _root = newnode;
            return;
        }
        NodePtr tmp;
        Node::split(_root, pos, tmp, _root);
        tmp = Node::merge(tmp, newnode);
        _root = Node::merge(tmp, _root);
    }

    void erase(unsigned pos) {
        if (pos >= Node::size(_root))
            throw std::out_of_range("While calling erase");
        NodePtr left, mid, right;
        Node::split(_root, pos, left, mid);
        Node::split(mid, 1, mid, right);
        delete mid;
        _root = Node::merge(left, right);
    }

    void add(unsigned l, unsigned r, Val val) {
        if (_root) {
            //Node::add(_root, l, r, val);
            auto addNode = [](Node *&n, Val addVal) {
                if (n->_added)
                    n->_addVal += addVal;
                else
                    n->_addVal = addVal;
                n->_added = true;
            };
            Node::doSthInTheMiddle(_root, l, r, addNode, val);
        }
    }

    void assign(unsigned l, unsigned r, Val val) {
        if (_root) {
            //Node::assign(_root, l, r, val);
            auto assignNode = [](Node *&n, Val assignVal) {
                n->_assigned = true;
                n->_assignVal = assignVal;
            };
            Node::doSthInTheMiddle(_root, l, r, assignNode, val);
        }
    }

    Val sum(unsigned l, unsigned r) {
        Val res=Val();
        if (_root) {
            auto sumInNode = [](Node* n, Val& res) {
                res = n->_sum;
            };
            Node::doSthInTheMiddle(_root, l, r, sumInNode, res);
        }
        return res;
    }

    void permutation(unsigned l, unsigned r, bool next) {
        if (l >= Node::size(_root) || r >= Node::size(_root))
            throw std::out_of_range("Out of range index while calling next_permutation");

        auto permutationNode = [](Node* perm, bool next) {
            if (!perm)
                return;
            perm->push();
            size_t suffix = next ? perm->_decreasing_tail : perm->_increasing_tail;
            if (suffix == perm->_size) {
                perm->_reversed ^= true;
                if (perm->_reversed)
                    perm->reverse();
            } else {
                NodePtr left, leftmid, mid, rightmid, right;
                Node::split(perm, perm->_size - suffix, left, mid);
                Node::split(left, left->_size - 1, left, leftmid);
                Node::splitKey(mid, leftmid->_val, mid, right, !next);
                Node::split(mid, mid->_size - 1, mid, rightmid);
                mid = Node::merge(mid, leftmid);
                mid = Node::merge(mid, right);
                mid->_reversed ^= true;
                if (mid->_reversed)
                    mid->reverse();
                left = Node::merge(left, rightmid);
                perm = Node::merge(left, mid);
            }
        };
        Node::doSthInTheMiddle(_root, l, r, permutationNode, next);
    }


    void next_permutation(unsigned l, unsigned r) {
        permutation(l, r, true);
    }

    void prev_permutation(unsigned l, unsigned r) {
        permutation(l, r, false);
    }

    NodePtr root() const {
        return _root;
    }

    ~Cartesian() {
        Node::destroy(_root);
    }
};

int main() {
    Cartesian<long long> tree;

    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);

    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        long long x;
        std::cin >> x;
        tree.insert(x);
    }

    int m;
    std::cin >> m;
    for (int i = 0; i < m; ++i) {
        int op;
        std::cin >> op;
        unsigned l, r, pos;
        long long x;
        switch (op) {
            case 1:
                std::cin >> l >> r;
                std::cout << tree.sum(l, r) << std::endl;
                break;
            case 2:
                std::cin >> x >> pos;
                tree.insert(x, pos);
                break;
            case 3:
                std::cin >> pos;
                tree.erase(pos);
                break;
            case 4:
                std::cin >> x >> l >> r;
                tree.assign(l, r, x);
                break;
            case 5:
                std::cin >> x >> l >> r;
                tree.add(l, r, x);
                break;
            case 6:
                std::cin >> l >> r;
                tree.next_permutation(l, r);
                break;
            case 7:
                std::cin >> l >> r;
                tree.prev_permutation(l, r);
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }
    CNode<long long>::inorder(tree.root());
    std::cout << std::endl;
    return 0;
}