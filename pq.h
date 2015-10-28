#include <functional>
#include <algorithm>

using namespace std;

template <typename T, typename Comp=std::less<T> >
class PQ {
public:
    const unsigned int d; // the maximum number of children allowed
private:
    std::vector<T> heap;

    // moveUp support methods
    bool isRoot(unsigned int index) const {  return (0 == index); }
    // -> These methods assume that the index does not correspond with the root
    //    or that the PQ is not empty
    unsigned int parent(unsigned int index) const { return (index-1)/d; }
    unsigned int lastIndex() const { return (heap.size()-1);}
    unsigned int lastParent() const { return parent(lastIndex()); }

    // moveDown support methods
    bool isLeaf(unsigned int index) const {
        return ((0 == index && heap.size() == 1) || index > lastParent());
    }
    // ->these methods assume that the index does not correspond with a leaf
    unsigned int firstChild(unsigned int index) const { return (index*d + 1); }
    unsigned int lastChild(unsigned int index) const {
        return ((index == lastParent() ? lastIndex() : d*(index+1)));
    }
    unsigned int maxChild(unsigned int index) const {
        return (max_element(heap.begin()+firstChild(index),
            heap.begin()+lastChild(index)+1, Comp()) - heap.begin());
    }

    void moveUp(unsigned int index) {
        unsigned int prnt;
        Comp cmp;
        // while not at the root and the parent's value is smaller
        while (!isRoot(index) && cmp(heap[prnt=parent(index)], heap[index])) {
            swap(heap[prnt], heap[index]);
            index = prnt;
        }
    }
    void moveDown(unsigned int index) {
        unsigned int theMaxChild;
        Comp cmp;
        // while not at a leaf and a child's value is larger
        while (!isLeaf(index) && cmp(heap[index],heap[theMaxChild=maxChild(index)])) {
            swap(heap[theMaxChild], heap[index]);
            index = theMaxChild;
        }
    }

public:
    PQ(unsigned int numChildren=4) : d(numChildren) { ; }
    template <typename Iter>
    PQ(Iter begin, Iter end, unsigned int numChildren=4)  : d(numChildren), heap(begin, end) {
        // moveDown each element
        for (auto i = heap.begin(); i != heap.end(); ++i) {
            moveDown(i - heap.begin()); // convert iter to index
        }
    }

    unsigned int size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
    const T &top() const { return heap[0]; }

    void push(const T &value) {
        heap.push_back(value);
        moveUp(lastIndex());
    }

    void pop() {
        // simple, trivial cases
        switch (size()) {
        case 0: return;
        case 1:
            heap.pop_back();
            return;
        }

        // replace the top value with the value stored at the end
        // then restore heap order
        heap[0] = heap[lastIndex()];
        heap.pop_back();
        moveDown(0);
    }
};
