// Submitter: jiel30(Jie, Li)
// Partner  : junfenl3(Junfeng, Li)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef HEAP_PRIORITY_QUEUE_HPP_
#define HEAP_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include <utility>              //For std::swap function
#include "array_stack.hpp"      //See operator <<


namespace ics {


#ifndef undefinedgtdefined
#define undefinedgtdefined
template<class T>
bool undefinedgt (const T& a, const T& b) {return false;}
#endif /* undefinedgtdefined */

//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to undefinedgt in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedgt value supplied by tgt/cgt is stored in the instance variable gt.
template<class T, bool (*tgt)(const T& a, const T& b) = undefinedgt<T>> class HeapPriorityQueue {
  public:
    typedef bool (*gtfunc) (const T& a, const T& b);
        
    //Destructor/Constructors
    ~HeapPriorityQueue();

    HeapPriorityQueue(bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    explicit HeapPriorityQueue(int initial_length, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    HeapPriorityQueue(const HeapPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    explicit HeapPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HeapPriorityQueue (const Iterable& i, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);


    //Queries
    bool empty      () const;
    int  size       () const;
    T&   peek       () const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<


    //Commands
    int  enqueue (const T& element);
    T    dequeue ();
    void clear   ();

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    int enqueue_all (const Iterable& i);


    //Operators
    HeapPriorityQueue<T,tgt>& operator = (const HeapPriorityQueue<T,tgt>& rhs);
    bool operator == (const HeapPriorityQueue<T,tgt>& rhs) const;
    bool operator != (const HeapPriorityQueue<T,tgt>& rhs) const;

    template<class T2, bool (*gt2)(const T2& a, const T2& b)>
    friend std::ostream& operator << (std::ostream& outs, const HeapPriorityQueue<T2,gt2>& pq);



    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of HeapPriorityQueue<T,tgt>
        ~Iterator();
        T           erase();
        std::string str  () const;
        HeapPriorityQueue<T,tgt>::Iterator& operator ++ ();
        HeapPriorityQueue<T,tgt>::Iterator  operator ++ (int);
        bool operator == (const HeapPriorityQueue<T,tgt>::Iterator& rhs) const;
        bool operator != (const HeapPriorityQueue<T,tgt>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const HeapPriorityQueue<T,tgt>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }

        friend Iterator HeapPriorityQueue<T,tgt>::begin () const;
        friend Iterator HeapPriorityQueue<T,tgt>::end   () const;

      private:
        //If can_erase is false, the value has been removed from "it" (++ does nothing)
        HeapPriorityQueue<T,tgt>  it;                 //copy of HPQ (from begin), to use as iterator via dequeue
        HeapPriorityQueue<T,tgt>* ref_pq;
        int                       expected_mod_count;
        bool                      can_erase = true;

        //Called in friends begin/end
        //These constructors have different initializers (see it(...) in first one)
        Iterator(HeapPriorityQueue<T,tgt>* iterate_over, bool from_begin);    // Called by begin
        Iterator(HeapPriorityQueue<T,tgt>* iterate_over);                     // Called by end
    };


    Iterator begin () const;
    Iterator end   () const;


  private:
    bool (*gt) (const T& a, const T& b); //The gt used by enqueue (from template or constructor)
    T*  pq;                              //Array stores a binary heap, accordingto the heap order/structure property
    int length    = 0;                   //Physical length of array: must be >= .size()
    int used      = 0;                   //Cache the amount of array used, with invariant: 0 <= used <= length
    int mod_count = 0;                   //Allows the sensing of concurrent modification


    //Helper methods
    void ensure_length  (int new_length);
    int  left_child     (int i) const;         //Useful abstractions for heaps as arrays
    int  right_child    (int i) const;
    int  parent         (int i) const;
    bool is_root        (int i) const;
    bool in_heap        (int i) const;
    void percolate_up   (int i);
    void percolate_down (int i);
    void heapify        ();                   // Percolate down all value is array (from indexes used-1 to 0): O(N)
  };





////////////////////////////////////////////////////////////////////////////////
//
//HeapPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::~HeapPriorityQueue() {
    delete[] pq;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::HeapPriorityQueue(bool (*cgt)(const T& a, const T& b)){
    if(tgt != (gtfunc)undefinedgt<T>)
    {
        gt = tgt;
    }
    else
    {
        gt = cgt;
    }
    if (gt == (gtfunc)undefinedgt<T>)
        throw TemplateFunctionError("HeapPriorityQueue::default constructor: neither specified");
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::default constructor: both specified and different");

    pq = new T[length];
}

template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::HeapPriorityQueue(int initial_length, bool (*cgt)(const T& a, const T& b))
: gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(initial_length) {
    if (gt == (gtfunc)undefinedgt<T>)
        throw TemplateFunctionError("HeapPriorityQueue::length constructor: neither specified");
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::length constructor: both specified and different");

    if (length < 0)
        length = 0;
    pq = new T[length];
}


template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::HeapPriorityQueue(const HeapPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b))
: gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(to_copy.length) {
    if (gt == (gtfunc)undefinedgt<T>)
        gt = to_copy.gt;//throw TemplateFunctionError("ArrayPriorityQueue::copy constructor: neither specified");
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::copy constructor: both specified and different");

    pq = new T[length];

    if (gt == to_copy.gt) {
        used = to_copy.used;
        for (int i=0; i<to_copy.used; ++i)
            pq[i] = to_copy.pq[i];
    }else
        for (int i=0; i<to_copy.used; ++i)
            enqueue(to_copy.pq[i]);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::HeapPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
:gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(il.size()) {
    if (gt == (gtfunc)undefinedgt<T>)
    throw TemplateFunctionError("HeapPriorityQueue::initializer_list constructor: neither specified");
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
    throw TemplateFunctionError("HeapPriorityQueue::initializer_list constructor: both specified and different");

    pq = new T[length];
    for (const T& pq_elem : il)
    enqueue(pq_elem);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template<class Iterable>
HeapPriorityQueue<T,tgt>::HeapPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
: gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(i.size()) {
    if (gt == (gtfunc)undefinedgt<T>)
        throw TemplateFunctionError("HeapPriorityQueue::Iterable constructor: neither specified");
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::Iterable constructor: both specified and different");

    pq = new T[length];
    int aa = 0;
    for(auto a : i)
    {
        enqueue(a);
        aa++;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::empty() const {
    return used == 0;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int HeapPriorityQueue<T,tgt>::size() const {
    return used;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& HeapPriorityQueue<T,tgt>::peek () const {
    if (empty())
        throw EmptyError("HeapPriorityQueue::peek");

    return pq[0];
}

template<class T, bool (*tgt)(const T& a, const T& b)>
std::string HeapPriorityQueue<T,tgt>::str() const {
    std::ostringstream answer;
    answer << "HeapPriorityQueue[";
    T temp[size()];
    int a = 0;
    for(auto x = begin(); x != end(); x++) {
        temp[a] = *x;
        a++;
    }
    int i = 1;
    for(int aa = size() - 1; aa >=0; aa--) {
        if (aa == size() - 1) {
            answer << "0:" << temp[aa];
        } else {
            answer << "," << i << ";" << temp[aa];
            i++;
        }
    }

    answer << "](length=" << length << ",used=" << used << ",mod_count=" << mod_count << ")";
    return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, bool (*tgt)(const T& a, const T& b)>
int HeapPriorityQueue<T,tgt>::enqueue(const T& element) {
    this->ensure_length(used + 1);
    used ++;
    pq[used - 1] = element;
    percolate_up(used - 1);
    ++mod_count;
    return 1;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T HeapPriorityQueue<T,tgt>::dequeue() {
    if (this->empty())
        throw EmptyError("HeapPriorityQueue::dequeue");
    ++mod_count;
    used --;
    T to_return = pq[0];
    pq[0] = pq[used];
    percolate_down(0);
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void HeapPriorityQueue<T,tgt>::clear() {
    used = 0;
    ++mod_count;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template <class Iterable>
int HeapPriorityQueue<T,tgt>::enqueue_all (const Iterable& i) {
    int count = 0;
    for (const T& v : i)
        count += enqueue(v);

    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>& HeapPriorityQueue<T,tgt>::operator = (const HeapPriorityQueue<T,tgt>& rhs) {
    if (this == &rhs)
        return *this;
    gt = rhs.gt;
    this->ensure_length(rhs.used);
    used = rhs.used;
    for (int i=0; i<used; ++i)
        pq[i] = rhs.pq[i];

    ++mod_count;
    return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::operator == (const HeapPriorityQueue<T,tgt>& rhs) const {
    if (this == &rhs)
        return true;

    if (gt != rhs.gt) //For PriorityQueues to be equal, they need the same gt function, and values
        return false;

    if (used != rhs.size())
        return false;
//    std::cout << "passed first part\n";
    HeapPriorityQueue<T,tgt>::Iterator rhs_i = rhs.begin();
    HeapPriorityQueue<T,tgt>::Iterator b = this->begin();
    HeapPriorityQueue<T,tgt>::Iterator e = this->end();
//    std::cout << "finish set begin and end" << std::endl;
//    while(b != e)
//    {
////        std::cout << "finish compare" << std::endl;
//        if(*b == *rhs_i)
//        {
////            std::cout << "not equal" << std::endl;
//            return false;
//        }
//        rhs_i ++;
//        b ++;
//    }
    for(auto aa = begin(); aa != end(); aa++)
    {
        if(*aa != *rhs_i)
        {
            return false;
        }
        rhs_i ++;
    }

    return true;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::operator != (const HeapPriorityQueue<T,tgt>& rhs) const {
    return !(*this == rhs);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::ostream& operator << (std::ostream& outs, const HeapPriorityQueue<T,tgt>& p) {
    outs << "priority_queue[";
    T temp[p.size()];
    int a = 0;
    for(auto x:p)
    {
        temp[a] = x;
        a++;
    }
    for(int aa = p.size() - 1; aa >=0; aa--)
    {
        if(aa == p.size() - 1)
        {
            outs << temp[aa];
        }
        else
        {
            outs << "," << temp[aa];
        }
    }
    outs << "]:highest";

    return outs;

}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
auto HeapPriorityQueue<T,tgt>::begin () const -> HeapPriorityQueue<T,tgt>::Iterator {
    return Iterator(const_cast<HeapPriorityQueue<T,tgt>*>(this),true);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto HeapPriorityQueue<T,tgt>::end () const -> HeapPriorityQueue<T,tgt>::Iterator {
    return Iterator(const_cast<HeapPriorityQueue<T,tgt>*>(this));
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, bool (*tgt)(const T& a, const T& b)>
void HeapPriorityQueue<T,tgt>::ensure_length(int new_length) {
    if (length >= new_length)
        return;
    T* old_pq = pq;
    length = std::max(new_length,2*length);
    pq = new T[length];
    for (int i=0; i<used; ++i)
        pq[i] = old_pq[i];

    delete [] old_pq;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int HeapPriorityQueue<T,tgt>::left_child(int i) const
{
    return i*2 + 1;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
int HeapPriorityQueue<T,tgt>::right_child(int i) const
{
    return i * 2 + 2;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
int HeapPriorityQueue<T,tgt>::parent(int i) const
{
    return (i-1)/2;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::is_root(int i) const
{
    return(i == 0);
}

template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::in_heap(int i) const
{
    return(i >= 0 && i <= (used - 1));
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void HeapPriorityQueue<T,tgt>::percolate_up(int i) {
    int p = parent(i);
    if(is_root(i))
    {
        return;
    }
    if(! in_heap(p))
    {
//        std::cout << "hahaha" << p << std::endl;
        return;
    }
    if(gt(pq[i],pq[p]))
    {
        T a = pq[i];
        pq[i] = pq[p];
        pq[p] = a;
        percolate_up(p);
    }
    else
    {
        return;
    }
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void HeapPriorityQueue<T,tgt>::percolate_down(int i) {
    int left = left_child(i);
    int right = right_child(i);
    int compare;
    if(!in_heap(left))
    {
        return;
    }
    if(in_heap(left) || not in_heap(right))
    {
        compare = left;
    }
    if(in_heap(left) && in_heap(right) && gt(pq[left],pq[right]))
    {
        compare = left;
    }
    if(in_heap(left) && in_heap(right)&& ! gt(pq[left],pq[right]))
    {
        compare = right;
    }
    if(in_heap(compare)&& gt(pq[compare],pq[i]))
    {
        T a = pq[i];
        pq[i] = pq[compare];
        pq[compare] = a;
        percolate_down(compare);
    }

}


template<class T, bool (*tgt)(const T& a, const T& b)>
void HeapPriorityQueue<T,tgt>::heapify() {
for (int i = used-1; i >= 0; --i)
  percolate_down(i);
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::Iterator::Iterator(HeapPriorityQueue<T,tgt>* iterate_over, bool from_begin)
:ref_pq(iterate_over), expected_mod_count(ref_pq->mod_count),it(*iterate_over)
{
}


template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::Iterator::Iterator(HeapPriorityQueue<T,tgt>* iterate_over)
:ref_pq(iterate_over), expected_mod_count(ref_pq->mod_count),it(*iterate_over)
{
    it.clear();
//    it = *iterate_over;
//    it.used = 0;
//    ref_pq = iterate_over;
//    expected_mod_count = ref_pq->mod_count;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
HeapPriorityQueue<T,tgt>::Iterator::~Iterator()
{}


template<class T, bool (*tgt)(const T& a, const T& b)>
T HeapPriorityQueue<T,tgt>::Iterator::erase() {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("HeapPriorityQueue::Iterator::erase Iterator cursor already erased");
    if (it.size() == 0)
        throw CannotEraseError("HeapPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;
    T to_return = it.dequeue();
    int index = 0;
    for (int aa = 0; aa < ref_pq->size(); aa++)
    {
        if(to_return == ref_pq->pq[aa])
        {
            index = aa;
            break;
        }
    }
    ref_pq->pq[index] = ref_pq->pq[ref_pq->size()-1];
    ref_pq->used --;
    ref_pq->mod_count --;
    ref_pq->percolate_down(index);
    expected_mod_count = ref_pq->mod_count;
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string HeapPriorityQueue<T,tgt>::Iterator::str() const {
    std::ostringstream answer;
    answer << ref_pq->str() << "/current=" << it.peek() << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
    return answer.str();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto HeapPriorityQueue<T,tgt>::Iterator::operator ++ () -> HeapPriorityQueue<T,tgt>::Iterator& {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ++");

    if (it.size() == 0)
        return *this;

    if (can_erase)
        it.dequeue();         //decreasing priority goes backward in array, towards 0
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto HeapPriorityQueue<T,tgt>::Iterator::operator ++ (int) -> HeapPriorityQueue<T,tgt>::Iterator {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ++(int)");

    if (it.size() == 0)
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        it.dequeue();         //decreasing priority goes backward in array, towards 0
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::Iterator::operator == (const HeapPriorityQueue<T,tgt>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("HeapPriorityQueue::Iterator::operator ==");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ==");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("HeapPriorityQueue::Iterator::operator ==");

    if(it.size() == 0 && rhsASI->it.size() == 0)
    {
        return true;
    }
    return it.peek() == rhsASI->it.peek();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool HeapPriorityQueue<T,tgt>::Iterator::operator != (const HeapPriorityQueue<T,tgt>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("HeapPriorityQueue::Iterator::operator !=");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator !=");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("HeapPriorityQueue::Iterator::operator !=");
    if(it.size() == 0 || rhsASI->it.size() == 0)
    {
        if(it.size() == rhsASI->it.size())
        {
            return false;
        }
        return true;
    }
    return it.peek() != rhsASI->it.peek();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& HeapPriorityQueue<T,tgt>::Iterator::operator *() const {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator *");
    if (!can_erase || it.size() == 0) {
        std::ostringstream where;
        where << 0 << " when size = " << ref_pq->size();
        throw IteratorPositionIllegal("HeapPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
    }

    return it.peek();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T* HeapPriorityQueue<T,tgt>::Iterator::operator ->() const {
    if (expected_mod_count !=  ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ->");
    if (!can_erase || it.size() == 0) {
        std::ostringstream where;
        where << 0 << " when size = " << ref_pq->size();
        throw IteratorPositionIllegal("HeapPriorityQueue::Iterator::operator -> Iterator illegal: "+where.str());
    }

    return &it.peek();
}

}

#endif /* HEAP_PRIORITY_QUEUE_HPP_ */
