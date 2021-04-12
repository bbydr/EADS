//
//  main.cpp
//  EadsLab2
//
//  Created by Beste Baydur on 3.12.2020.
//

#include <iostream>
#include <iterator>
using namespace std;

template<typename Key, typename Info>
class Ring {
private:
    struct Node {
        Node(Key k, Info i, Node* n, Node* p) {
            key = k;
            info = i;
            next = n;
            prev = p;
        }

        Key key;
        Info info;
        Node* next;
        Node* prev;
    };
    Node* root;

public:
    // Base class for my iterators
    class RingIterator : public iterator<bidirectional_iterator_tag, Key> {
    public:
        RingIterator(const RingIterator& copy)
            : node(copy.node) { }

        RingIterator& operator = (const RingIterator& what) {
            if (this != &what) {
                node = what.node;
            }
            return *this;
        }

        bool isValid(void) const {
            return node != nullptr;
        }

        RingIterator operator ++(int) {
            RingIterator prev = *this;
            ++(*this);
            return prev;
        }

        RingIterator& operator ++(void) {
            if (node != nullptr) {
                node = node->next;
            }
            return *this;
        }

        RingIterator operator --(int) {
            RingIterator prev = *this;
            --(*this);
            return prev;
        }

        RingIterator& operator --(void) {
            if (node != nullptr) {
                node = node->prev;
            }
            return *this;
        }

        bool operator ==(const RingIterator& what) {
            return node == what.node;
        }

        bool operator !=(const RingIterator& what) {
            return node != what.node;
        }

    private:
        RingIterator(Node* n) : node(n) { }
        Node* node;

        friend class Ring<Key, Info>;
    };

    // A read-write iterator subclass
    class Iterator : public RingIterator {
    public:
        Iterator(void) : RingIterator(nullptr) { }
        Iterator(const Iterator& copy)
            : RingIterator(copy.node) { }

        const Key& getKey(void) const {
            return this->node->key;
        }

        const Info& getInfo(void) const {
            return this->node->info;
        }

        Info setInfo(Info value) {
            swap(this->node->info, value);
            return value;
        }

        Info& operator *(void) { return this->node->info; }

    protected:
        Iterator(Node* n) : RingIterator(n) { }
        friend class Ring<Key, Info>;
    };

    // A read-only iterator subclass
    class ConstIterator : public RingIterator {
    public:
        ConstIterator(void) : RingIterator(nullptr) { }
        ConstIterator(const ConstIterator& copy)
            : RingIterator(copy.node) { }

        const Key& getKey(void) const {
            return this->node->key;
        }

        const Info& getInfo(void) const {
            return this->node->info;
        }

        const Info& operator *(void) { return this->node->info; }

    protected:
        ConstIterator(Node* n) : RingIterator(n) { }
        friend class Ring<Key, Info>;
    };

    Ring(void) : root(nullptr) { }

    Ring(const Ring& copy) : root(nullptr) {
        copyFrom(copy);
    }

    Ring(Ring&& move) noexcept : root(nullptr) {
        swap(move.root, root);
    }

    ~Ring(void) {
        clear();
    }

    Ring& operator = (const Ring& what) {
        if (this != &what) {
            clear();
            copyFrom(what);
        }
        return *this;
    }

    void print(void) const {
        if (!isEmpty()) {
            ConstIterator iter = any();
            do {
                cout << "[" << iter.getKey() << "," << iter.getInfo() << "]" << endl;
            } while (++iter != any());
        }
    }

    void clear(void) {
        if (!isEmpty()) {
            Iterator iter = any();
            do {
                auto node = iter++.node;
                delete node;
            } while (iter != any());
            root = nullptr;
        }
    }

    bool isEmpty(void) const {
        return root == nullptr;
    }

    // Counts how many elements it has
    int count(void) const {
        int result = 0;
        if (!isEmpty()) {
            ConstIterator iter = any();
            do {
                ++result;
            } while (++iter != any());
        }
        return result;
    }

    // Try to find and remove a node with the given key
    bool remove(Key key) {
        return remove(find(key));
    }

    // Removes the entry, pointed by the iterator (it may rotate if iter points to any())
    bool remove(Iterator iter, bool clockwise = true) {
        if (iter.isValid()) {
            Node* node = iter.node;
            Node* next = node->next;
            Node* prev = node->prev;
            next->prev = prev;
            prev->next = next;
            if (node == root) {
                if (clockwise) {
                    root = next == node ? nullptr : next;
                }
                else {
                    root = prev == node ? nullptr : prev;
                }
            }
            delete node;
            return true;
        }
        return false;
    }

    // Checks if a node with the given key exists
    bool exists(Key key) const {
        return find(key).isValid();
    }

    // Inserts a new entry (unique keys)
    void insert(Key key, Info info, bool clockwise = true) {
        if (root == nullptr) {
            root = new Node(key, info, nullptr, nullptr);
            root->next = root->prev = root;
        } else {
            if (exists(key)) {
                throw runtime_error("An entry with the given key is already present!");
            }
            Node* prev = root->prev;
            Node* next = root->next;
            if (clockwise) {
                Node* node = new Node(key, info, root, prev);
                prev->next = node;
                root->prev = node;
                root = node;
            }
            else {
                Node* node = new Node(key, info, next, root);
                next->prev = node;
                root->next = node;
                root = node;
            }
        }
    }

    // Copies and inserts nodes from the provided ring
    void copyFrom(const Ring<Key, Info>& from) {
        if (!from.isEmpty()) {
            ConstIterator iter = from.any();
            do {
                insert(iter.getKey(), iter.getInfo());
            } while (++iter != from.any());
        }
    }

    // What happens to be the current "middle" of the ring (read-only iterator)
    ConstIterator any(void) const {
        return ConstIterator(root);
    }

    // What happens to be the current "middle" of the ring (normal iterator)
    Iterator any(void) {
        return Iterator(root);
    }

    // Indexing, relative to any() (read-only iterator)
    ConstIterator at(int index) const {
        ConstIterator iter = any();
        do {
            ++iter;
        } while (--index > 0);
        return iter;
    }

    // Indexing, relative to any() (normal iterator)
    Iterator at(int index) {
        Iterator iter = any();
        do {
            ++iter;
        } while (--index > 0);
        return iter;
    }

    // Finds the first occurrence of the key (++)
    Iterator find(Key key) {
        if (!isEmpty()) {
            Iterator iter = any();
            do {
                if (iter.getKey() == key) {
                    return iter;
                }
            } while (++iter != any());
        }
        return Iterator(nullptr);
    }

    // Finds the first occurrence of the key (++, read-only)
    ConstIterator find(Key key) const {
        if (!isEmpty()) {
            ConstIterator iter = any();
            do {
                if (iter.getKey() == key) {
                    return iter;
                }
            } while (++iter != any());
        }
        return ConstIterator(nullptr);
    }
};

int main(int argc, char** argv) {
    Ring<int, string> A;
    Ring<int, string> B;

    A.insert(11, "Just");
    A.insert(3, "Another");
    A.insert(6, "Brick");
    A.insert(7, "In");
    A.insert(8, "The");
    A.insert(1, "Wall");

    B = A;
    B.remove(B.at(2));
    B.remove(B.find(1));
    // Expected output: "11,Just 3,Another 7,In 8,The"
    
    B.print();
    // I got the expected output
    return 0;
}
