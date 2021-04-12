//
//  main.cpp
//  EadsLab3
//
//  Created by Beste Baydur on 18.12.2020.
//

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

template<typename Key, typename Info>
class Dictionary {
public:
    Dictionary() : root(nullptr) {

    }

    Dictionary(const Dictionary& copy) : root(nullptr) {
        if (copy.root != nullptr) {
            root = new Node(*copy.root);
        }
    }

    ~Dictionary() {
        clear();
    }

    Dictionary& operator =(const Dictionary& that) {
        if (this != &that) {
            clear();
            if (that.root != nullptr) {
                root = new Node(that.root);
            }
        }
        return *this;
    }

    void print(ostream& os) {
        int max = getHeight();
        for (int i = 0; i < max; ++i) {
            printRow(os, i, max);
        }
    }

    int count() const {
        return count(root);
    }

    void clear() {
        if (root != nullptr) {
            delete root;
            root = nullptr;
        }
    }

    void insert(const Key& k, const Info& i) {
        root = insert(root, k, i);
    }

    void remove(const Key& k) {
        root = remove(root, k);
    }

    bool hasKey(const Key& k) {
        return findNode(k) != nullptr;
    }

    bool tryGetInfo(const Key& k, Info& i) {
        Node* n = findNode(k);
        if (n != nullptr) {
            i = n->info;
            return true;
        }
        return false;
    }
    
    Info& getInfo(const Key& k) {
        return findNode(k)->info;
    }

private:
    struct Node {
        Key key;
        Info info;
        Node* nodeL;
        Node* nodeR;

        Node(const Key& k, const Info& i, Node* l = nullptr, Node* r = nullptr)
            : key(k), info(i), nodeL(l), nodeR(r)
        {
        }

        Node(const Node& copy) : Node(copy.key, copy.info) {
            if (copy.nodeL != nullptr) {
                nodeL = new Node(*(copy.nodeL));
            }
            if (copy.nodeR != nullptr) {
                nodeR = new Node(*(copy.nodeR));
            }
        }

        ~Node() {
            if (nodeL != nullptr) {
                delete nodeL;
            }
            if (nodeR != nullptr) {
                delete nodeR;
            }
        }

        int getType() const {
            int type = 0;
            if (nodeL != nullptr) type += 1;
            if (nodeR != nullptr) type += 2;
            return type;
        }
    };

    void printRow(ostream& os, int row, int max) {
        vector<Node*> nodes;
        printRow(nodes, root, row, 0);
        int width = (max - row) * 3;
        os << string(width, ' ');
        for (typename vector<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            Node* node = *it;
            if (node != nullptr) {
                os << node->key;
            }
            else {
                os << "N";
            }
            os << string((int)ceil(1.5 * (max - row)), ' ');
        }
        os << endl << endl;
    }

    void printRow(vector<Node*>& nodes, Node* n, int row, int lvl) {
        if (lvl == row) {
            nodes.push_back(n);
        }
        else {
            if (n != nullptr) {
                printRow(nodes, n->nodeL, row, lvl + 1);
                printRow(nodes, n->nodeR, row, lvl + 1);
            }
        }
    }

    int count(Node* n) const {
        if (n == nullptr)
            return 0;
        return count(n->nodeL) + count(n->nodeR) + 1;
    }

    int getHeight() const {
        return getHeight(root);
    }

    int getHeight(Node* n) const {
        if (n == nullptr)
            return 0;
        return max(getHeight(n->nodeL), getHeight(n->nodeR)) + 1;
    }
    
    int getBalance(Node* n) const {
        if (n == nullptr) {
            return 0;
        }
        return getHeight(n->nodeL) - getHeight(n->nodeR);
    }

    Node* rotateL(Node* n) {
        Node* l = n->nodeR;
        Node* r = l->nodeL;
        l->nodeL = n;
        n->nodeR = r;
        return l;
    }

    Node* rotateR(Node* n) {
        Node* r = n->nodeL;
        Node* l = r->nodeR;
        r->nodeR = n;
        n->nodeL = l;
        return r;
    }

    Node* insert(Node* n, const Key& k, const Info& i) {
        if (n == nullptr) {
            n = new Node(k, i);
            return n;
        }

        if (k < n->key) {
            n->nodeL = insert(n->nodeL, k, i);
        }
        else if (k > n->key) {
            n->nodeR = insert(n->nodeR, k, i);
        }
        else {
            throw runtime_error("Duplicate keys are forbidden");
        }

        int balance = getBalance(n);

        if (balance > 1) {
            if (k < n->nodeL->key) {
                return rotateR(n);
            }
            if (k > n->nodeL->key) {
                n->nodeL = rotateL(n->nodeL);
                return rotateR(n);
            }
        }

        if (balance < -1) {
            if (k > n->nodeR->key) {
                return rotateL(n);
            }
            if (k < n->nodeR->key) {
                n->nodeR = rotateR(n->nodeR);
                return rotateL(n);
            }
        }

        return n;
    }

    Node* remove(Node* n, const Key& k) {
        if (n == nullptr) {
            return n;
        }

        if (n->key < k) {
            n->nodeL = remove(n->nodeL, k);
        }
        else if (n->key > k) {
            n->nodeR = remove(n->nodeR, k);
        }
        else {
            Node* temp = nullptr;
            switch (n->getType()) {
                case 3:
                    temp = n->nodeR;
                    while (temp->nodeL != nullptr) {
                        temp = temp->nodeL;
                    }
                    n->key = temp->key;
                    n->info = temp->info;
                    n->nodeR = remove(n->nodeR, temp->key);
                    temp = nullptr;
                    break;
                case 2:
                    temp = n->nodeR;
                    break;
                case 1:
                    temp = n->nodeL;
                    break;
                case 0:
                    delete n;
                    n = nullptr;
                    break;
            }
            if (temp != nullptr) {
                n->key = temp->key;
                n->info = temp->info;
                n->nodeL = temp->nodeL;
                n->nodeR = temp->nodeR;
                temp->nodeL = nullptr;
                temp->nodeR = nullptr;
                delete temp;
            }
        }

        if (n == nullptr) {
            return n;
        }

        int balance = getBalance(n);

        if (balance > 1) {
            if (getBalance(n->nodeL) >= 0) {
                return rotateR(n);
            }
            if (getBalance(n->nodeL) < 0) {
                n->nodeL = rotateL(n->nodeL);
                return rotateR(n);
            }
        }
        if (balance < -1) {
            if (getBalance(n->nodeR) <= 0) {
                return rotateL(n);
            }
            if (getBalance(n->nodeR) > 0) {
                n->nodeR = rotateR(n->nodeR);
                return rotateL(n);
            }
        }

        return n;
    }

    Node* findNode(const Key& k) {
        return findNode(root, k);
    }

    Node* findNode(Node* n, const Key& k) {
        if (n == nullptr || n->key == k) {
            return n;
        }
        if (n->key < k) {
            return findNode(n->nodeR, k);
        }
        else {
            return findNode(n->nodeL, k);
        }
    }

    Node* root;
};

template<typename Key, typename Info>
void drawInsert(Dictionary<Key, Info>& d, const Key& k, const Info& i) {
    cout << "Added '" << k << "'" << endl;
    d.insert(k, i);
    d.print(cout);
}

void draw_tree() {
    // See if our tree is self-balancing by visualizing
    Dictionary<int, int> coutTree;
    drawInsert(coutTree, 12, 0);
    drawInsert(coutTree, 8, 0);
    drawInsert(coutTree, 11, 0);
    drawInsert(coutTree, 4, 0);
    drawInsert(coutTree, 10, 0);
    drawInsert(coutTree, 18, 0);
    drawInsert(coutTree, 17, 0);
    drawInsert(coutTree, 5, 0);
}

int main() {
    draw_tree();

    Dictionary<string, int> test;

    test.insert("one", 1);
    test.insert("two", 2);
    test.insert("three", 3);
    test.insert("four", 4);
    test.insert("five", 5);

    assert(test.hasKey("one"));
    assert(test.hasKey("two"));
    assert(test.hasKey("three"));
    assert(test.hasKey("four"));
    assert(test.hasKey("five"));

    test.remove("three");
    assert(!test.hasKey("three"));

    test.clear();
    test.insert("fifty five", 55);
    test.insert("thirty nine", 39);

    int value;
    assert(test.tryGetInfo("fifty five", value));
    assert(value == 55);
    assert(test.tryGetInfo("thirty nine", value));
    assert(value == 39);

    Dictionary<string, int> test2 = test;

    test.remove("fifty five");
    test.remove("thirty nine");
    assert(!test.hasKey("fifty five"));
    assert(!test.hasKey("thirty nine"));

    assert(test2.tryGetInfo("fifty five", value));
    assert(value == 55);
    assert(test2.tryGetInfo("thirty nine", value));
    assert(value == 39);

    return 0;
}

