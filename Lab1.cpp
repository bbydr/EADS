#include <iostream>
#include <string>
using namespace std;

template<typename TKey, typename TInfo>
class Sequence {
public:
    
    // A linked-list node
    class Node {
        friend class Sequence<TKey, TInfo>;

    public:
        // Simple constructor
        Node(const TKey& key, const TInfo& info, Node* next = nullptr) : Key(key), Info(info), Next(next) { }

        // Copy constructor
        Node(const Node& copy) : Key(copy.Key), Info(copy.Info), Next(copy.Next) { }
        
        // Move constructor
        Node(Node&& move) : Key(), Info(), Next(nullptr) {
            swap(Key, move.Key);
            swap(Info, move.Info);
            swap(Next, move.Next);
        }

        // Overloading assignment operator
        Node& operator= (const Node& assign) {
            if (this != &assign) {
                Key = assign.Key;
                Info = assign.Info;
                Next = assign.Next;
            }
            return *this;
        }

        TKey GetKey(void) const {
            return Key;
        }

        TInfo& GetInfo(void) const {
            return Info;
        }

        Node* GetNext(void) const {
            return Next;
        }

    private:
        ~Node(void) {
            Next = nullptr;
        }

        TKey Key;
        TInfo Info;

        // Single-linked list
        Node* Next;
    };

    // Default constructor
    Sequence(void) : Head(nullptr) {

    }

    // Copy constructor
    Sequence(const Sequence& copy) : Head(nullptr) {
        CopyFrom(copy, 0, copy.Count());
    }

    // Move constructor
    Sequence(Sequence&& move) noexcept : Head(nullptr) {
        swap(Head, move.Head);
    }

    // Destructor
    ~Sequence(void) {
        Clear();
    }

    // Prints this sequence
    void Print(void) const {
        if (Head == nullptr) {
            cout << "[Empty]" << endl;
        } else {
            cout << "[" << Count() << " Elements]" << endl;
            Node* i = Head;
            while (i != nullptr) {
                cout << "[" << i->Key << ", " << i->Info << "]" << endl;
                i = i->Next;
            }
        }
    }

    // Overloading assignment operator
    Sequence& operator= (const Sequence& assign) {
        if (this != &assign) {
            Clear();
            CopyFrom(assign, 0, assign.Count());
        }
        return *this;
    }

    Sequence operator+(const Sequence& other) const {
        Sequence result;
        result.CopyFrom(*this, 0, Count());
        result.CopyFrom(other, 0, other.Count());
        return result;
    }

    Sequence& operator+=(const Sequence& other) const {
        CopyFrom(other, 0, other.Count());
        return *this;
    }

    // Gets the first element of the sequence
    Node* GetFirst(void) {
        return Head;
    }

    // Returns the first element with the given key
    Node* FindFirst(const TKey& key) {
        if (Head == nullptr) {
            return nullptr;
        } else {
            Node* i = Head;
            while (i != nullptr) {
                if (i->Key == key) {
                    return i;
                }
                i = i->Next;
            }
            return nullptr;
        }
    }

    // Gets the last element of the sequence
    Node* GetLast(void) {
        Node* prev;
        return GetLast(prev);
    }

    // Gets the last element of the sequence + the previous one
    Node* GetLast(Node*& prev) {
        if (Head == nullptr) {
            prev = nullptr;
            return nullptr;
        } else {
            prev = nullptr;
            Node* i = Head;
            while (i->Next != nullptr) {
                prev = i;
                i = i->Next;
            }
            return i;
        }
    }

    // Node at the provided index
    Node* At(const size_t index) const {
        if (index < 0 || index >= Count()) {
            throw "Index out of range";
        }
        Node* i = Head;
        for (size_t j = 0; j < index; j++) {
            i = i->Next;
        }
        return i;
    }

    // Remove all elements
    void Clear(void) {
        if (Head != nullptr) {
            Node* i = Head;
            Node* j = nullptr;
            while (i != nullptr) {
                j = i;
                i = i->Next;
                delete j;
            }
            Head = nullptr;
        }
    }

    // Checks whether it is empty
    bool IsEmpty(void) const {
        return Head == nullptr;
    }

    // Removes first element with a given key
    bool Remove(const TKey& key) {
        Node* i = Head;
        Node* j = nullptr;
        while (i != nullptr) {
            if (i->Key == key) {
                if (j == nullptr) {
                    Head = i->Next;
                }
                else {
                    j->Next = i->Next;
                }
                delete i;
                return true;
            }
            j = i;
            i = i->Next;
        }
        return false;
    }

    // Adds an element with the given key and info to the front
    void PushFront(const TKey& key, const TInfo& info) {
        if (Head == nullptr) {
            Head = new Node(TKey(key), TInfo(info));
        }
        else {
            GetLast()->Next= new Node(TKey(key), TInfo(info));
        }
    }

    // Adds an element with the given key and info to the back
    void PushBack(const TKey& key, const Info& info) {
        Head = new Node(TKey(key), TInfo(info), Head);
    }

    // Removes an element in the back
    bool PopBack(void) {
        if (Head != nullptr) {
            auto Next = Head->Next;
            delete Head;
            Head = Next;
            return true;
        }
        return false;
    }

    // Removes an element in the front
    bool PopFront(void) {
        Node* prev;
        Node* node = GetLast(prev);
        if (node != nullptr) {
            prev->Next = nullptr;
            delete node;
            return true;
        }
        else {
            return false;
        }
    }

    // How many elements does it have?
    size_t Count(void) const {
        Node* i = Head;
        size_t j = 0;
        while (i != nullptr) {
            ++j;
            i = i->Next;
        }
        return j;
    }

    // Is there at least one element with a given key?
    bool Contains(const TKey& key) const {
        Node* i = Head;
        while (i != nullptr) {
            if (i->Key == key) {
                return true;
            }
            i = i->Next;
        }
        return false;
    }

    Sequence SubSequence(const size_t offset, const size_t count) const {
        Sequence result;
        result.CopyFrom(*this, offset, count);
        return result;
    }

    // Copy [count] elements from another sequence, starting at [offset]
    void CopyFrom(const Sequence& from, const size_t offset, const size_t count) {
        if (from.Count() < offset + count) {
            throw "The provided sequence doesn't have enough elements to copy";
        }

        Node* j = from.At(offset);
        Node* i = nullptr;

        if (Head == nullptr) {
            i = Head = new Node(j->Key, j->Info);
        }
        else {
            i = GetLast()->Next = new Node(j->Key, j->Info);
        }

        j = j->Next;

        for (size_t k = 1; k < count; k++) {
            i->Next = new Node(j->Key, j->Info);
            i = i->Next;
            j = j->Next;
        }
    }

private:
    // Single-linked list
    Node* Head;
};


// Helper method for making sure our arguments fit the no-exception idea
template<typename Key, typename Info>
void clamp_args_produce(const Sequence<Key, Info>& seq1, int& start, int& len, int& limit) {
    limit = max(0, limit);
    start = max(0, start);
    len = max(0, len);

    if (len > 0) {
        auto maximum = start + len;
        auto clamped = min(maximum, (int)seq1.Count());
        len = min(clamped - start, limit);
        limit = max(0, limit - len);
    }
}


// Concatenates two sequences
template<typename Key, typename Info>
Sequence<Key, Info> produce(
        const Sequence<Key, Info>& seq1, int start1, int dl1,
        const Sequence<Key, Info>& seq2, int start2, int dl2,
        int limit)
{
    Sequence<Key, Info> result;

    clamp_args_produce(seq1, start1, dl1, limit);
    clamp_args_produce(seq2, start2, dl2, limit);

    result.CopyFrom(seq1, start1, dl1);
    result.CopyFrom(seq2, start2, dl2);

    return result;
}


int main(int argc, char** argv) {
    Sequence<int, string> A;
    Sequence<int, string> B;

    A.PushFront(13, "It's");
    A.PushFront(15, "Time");
    A.PushFront(3, "For");
    A.PushFront(3, "A");
    A.PushFront(21, "Show");

    B.PushFront(111, "Here");
    B.PushFront(3324, "I");
    B.PushFront(355, "Am");
    B.PushFront(555, "Rock");
    B.PushFront(99, "You");
    B.PushFront(77, "Like");
    B.PushFront(88, "A");
    B.PushFront(52, "Hurricane");

    Sequence<int, string> C = produce(A, 2, 2, B, 3, 4, 6);
    Sequence<int, string> D = produce(A, 2, 2, B, 3, 4, 3);

    cout << endl << "Sequence A: ";
    A.Print();
    cout << endl << "Sequence B: ";
    B.Print();
    cout << endl << "Sequence C: ";
    C.Print();
    cout << endl << "Sequence D: ";
    D.Print();
    return 0;
}
