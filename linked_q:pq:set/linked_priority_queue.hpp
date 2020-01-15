// Submitter: jiel30(Jie, Li)
// Partner  : junfenl3(Junfeng, Li)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
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
template<class T, bool (*tgt)(const T& a, const T& b) = undefinedgt<T>> class LinkedPriorityQueue {
  public:
    typedef bool (*gtfunc) (const T& a, const T& b);
    //Destructor/Constructors
    ~LinkedPriorityQueue();

    LinkedPriorityQueue          (bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    LinkedPriorityQueue          (const LinkedPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    explicit LinkedPriorityQueue (const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit LinkedPriorityQueue (const Iterable& i, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);


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
    LinkedPriorityQueue<T,tgt>& operator = (const LinkedPriorityQueue<T,tgt>& rhs);
    bool operator == (const LinkedPriorityQueue<T,tgt>& rhs) const;
    bool operator != (const LinkedPriorityQueue<T,tgt>& rhs) const;

    template<class T2, bool (*gt2)(const T2& a, const T2& b)>
    friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T2,gt2>& pq);



  private:
    class LN;

  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of LinkedPriorityQueue<T,tgt>
        ~Iterator();
        T           erase();
        std::string str  () const;
        LinkedPriorityQueue<T,tgt>::Iterator& operator ++ ();
        LinkedPriorityQueue<T,tgt>::Iterator  operator ++ (int);
        bool operator == (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const;
        bool operator != (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T,tgt>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator LinkedPriorityQueue<T,tgt>::begin () const;
        friend Iterator LinkedPriorityQueue<T,tgt>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        LN*             prev;            //initialized to point at header node
        LN*             current;         //invariant current == prev->next
        LinkedPriorityQueue<T,tgt>* ref_pq;
        int             expected_mod_count;
        bool            can_erase = true;

        //Called in friends begin/end
        Iterator(LinkedPriorityQueue<T,tgt>* iterate_over, LN* initial);
    };


    Iterator begin () const;
    Iterator end   () const;


  private:
    class LN {
      public:
        LN ()                      {}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next = nullptr;
    };


    bool (*gt) (const T& a, const T& b); // The gt used by enqueue (from template or constructor)
    LN* front;
  int used      =  0;                    //Cache the node count in this linked list
  int mod_count =  0;                    //Used to sense for concurrent modification

    //Helper methods
    void delete_list(LN*& front);        //Deallocate all LNs, and set front's argument to nullptr;
//    void swap(LN*& a, LN*& b);            //swap two LN''s value
};





////////////////////////////////////////////////////////////////////////////////
//
//LinkedPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::~LinkedPriorityQueue() {
    delete_list(front);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(bool (*cgt)(const T& a, const T& b))
: gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt) {
    if (gt == (gtfunc)undefinedgt<T>)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::default constructor: neither specified");
    }
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::default constructor: both specified and different");
    }
    front = new LN;
    front -> next = nullptr;

}



template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const LinkedPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b))
:gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt)
{
    if (gt == (gtfunc)undefinedgt<T>)
    {
        gt = to_copy.gt;//throw TemplateFunctionError("LinkedPriorityQueue::copy constructor: neither specified");
    }
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::copy constructor: both specified and different");
    }
    LN* current;
    front = new LN;
    front -> next = nullptr;
    current = front;
    LN* tc = to_copy.front-> next;
    if (gt == to_copy.gt) {
        used = to_copy.used;
        while(tc != nullptr)
        {
            LN* a = new LN;
            a -> value = tc -> value;
            current -> next = a;
            tc = tc-> next;
            current = current -> next;
        }
    }else
    {
        LN* tb = to_copy.front-> next;
        while(tb != nullptr)
        {
            enqueue(tb->value);
            tb = tb-> next;
        }
    }

}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
:gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt)
{
    if (gt == (gtfunc)undefinedgt<T>)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::initializer_list constructor: neither specified");
    }
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::initializer_list constructor: both specified and different");
    }
    front = new LN;
    front -> next = nullptr;
    for (auto pq_elem : il)
    {
        enqueue(pq_elem);
    }
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template<class Iterable>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
:gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt)
{
    if (gt == (gtfunc)undefinedgt<T>)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::Iterable constructor: neither specified");
    }
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
    {
        throw TemplateFunctionError("LinkedPriorityQueue::Iterable constructor: both specified and different");
    }
    front = new LN;
    front -> next = nullptr;
    for (auto v : i) {
        enqueue(v);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::empty() const {
    return used == 0;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int LinkedPriorityQueue<T,tgt>::size() const {
    return used;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& LinkedPriorityQueue<T,tgt>::peek () const {
    if (empty()) {
        throw EmptyError("LinkedPriorityQueue::peek");
    }
    return front-> next -> value;
}

//    outs << "priority_queue[";
//    auto tmp = pq.front->next;
//    T check[pq.used];
//    int count = 0;
//    while(tmp != nullptr)
//{
//    check[count] = tmp -> value;
//    tmp = tmp -> next;
//    count ++;
//}
//for (int aa = 0; aa < pq.used; aa++)
//{
//outs << check[pq.used - 1 - aa];
//if(aa != pq.used - 1)
//{
//outs << ",";
//}
//}
template<class T, bool (*tgt)(const T& a, const T& b)>
std::string LinkedPriorityQueue<T,tgt>::str() const {
    std::ostringstream answer;
    answer << "LinkedPriorityQueue[";
    if(used == 0)
    {
        answer << "](used=" << used << ",mod_count=" << mod_count << ")";
        return answer.str();
    }
    int count = 1;
    LN* x = front -> next;
    while(x != nullptr){
        answer << count << ":";
        answer << x->value;
        if(x->next != nullptr){
            answer << ",";
        }
        x = x->next;
        ++count;
    }
    answer << "](used=" << used << ",mod_count=" << mod_count << ")";
    return answer.str();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int LinkedPriorityQueue<T,tgt>::enqueue(const T& element) {
    used ++;
    LN* temp = this -> front -> next;
    LN* e = new LN;
    e->value = element;
    e -> next = temp;
    this -> front -> next = e;
    LN* compare = e;
    for (int aa = 0; aa < used -1; aa++)
    {
        if(gt(compare -> next->value,compare->value))
        {
//            swap(compare,compare->next);
            T c_value = compare->value;
            T n_value = compare->next -> value;
            compare -> value = n_value;
            compare -> next -> value = c_value;
            compare = compare -> next;
        }
        else
        {
            break;
        }
    }
    ++mod_count;
    return 1;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T LinkedPriorityQueue<T,tgt>::dequeue() {
    if(used == 0)
    {
        throw EmptyError("LinkedPriorityQueue::dequeue");
    }
    ++mod_count;
    LN* d = front-> next;
    T v = d -> value;
    front -> next = d -> next;
    delete d;
    used --;
    mod_count ++;
    return v;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void LinkedPriorityQueue<T,tgt>::clear() {
    used = 0;
    mod_count ++;
    while(front -> next != nullptr)
    {
        LN* temp = front -> next;
        front -> next = temp -> next;
        delete temp;
    }
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template <class Iterable>
int LinkedPriorityQueue<T,tgt>::enqueue_all (const Iterable& i) {
    int count = 0;
    for (auto aa:i)
    {
//        std::cout << aa << " ";
        count +=1;
        enqueue(aa);
        mod_count --;
    }
    mod_count ++;
    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>& LinkedPriorityQueue<T,tgt>::operator = (const LinkedPriorityQueue<T,tgt>& rhs) {

    if(this == &rhs)
    {
        mod_count++;
        return *this;
    }
    if(rhs.used == 0)
    {
        clear();
        return *this;
    }
    gt = rhs.gt;
    while(rhs.used < used)
    {
        dequeue();
        mod_count --;
    }

    if(rhs.used == used)
    {
        LN* temp = front -> next;
        LN* tbc = rhs.front-> next;
        while(tbc != nullptr)
        {
            temp->value = tbc->value;
            tbc = tbc-> next;
            temp = temp-> next;
        }
    }
    else
    {
        LN* temp = front -> next;
        LN* tbc = rhs.front-> next;
        while(tbc != nullptr)
        {
            if(temp != NULL)
            {
                temp->value = tbc->value;
                tbc = tbc-> next;
                temp = temp-> next;
            }
            else
            {
                enqueue(tbc->value);
//                std::cout << "enqueue: " << tbc -> value << std::endl;
                tbc = tbc-> next;
            }
        }
    }

    return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::operator == (const LinkedPriorityQueue<T,tgt>& rhs) const {
    if(this == &rhs)
    {
        return true;
    }
    if(gt != rhs.gt)
    {
        return false;
    }
    if(used != rhs.size())
    {
        return false;
    }
    auto a = front -> next;
    auto b = rhs.front -> next;
    while(true) {
        if (a == nullptr && b == nullptr)
        {
            return true;
        }
        if (a->value == b->value) {
            a = a->next;
            b = b->next;
        }
        else
        {
            return false;
        }
    }
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::operator != (const LinkedPriorityQueue<T,tgt>& rhs) const {
    return !(*this == rhs);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T,tgt>& pq) {
    outs << "priority_queue[";
    auto tmp = pq.front->next;
    T check[pq.used];
    int count = 0;
    while(tmp != nullptr)
    {
        check[count] = tmp -> value;
        tmp = tmp -> next;
        count ++;
    }
    for (int aa = 0; aa < pq.used; aa++)
    {
        outs << check[pq.used - 1 - aa];
        if(aa != pq.used - 1)
        {
            outs << ",";
        }
    }

    outs << "]:highest";
    return outs;

}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::begin () const -> LinkedPriorityQueue<T,tgt>::Iterator {
    return Iterator(const_cast<LinkedPriorityQueue<T,tgt>*>(this),front->next);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::end () const -> LinkedPriorityQueue<T,tgt>::Iterator {
    return Iterator(const_cast<LinkedPriorityQueue<T,tgt>*>(this),nullptr);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, bool (*tgt)(const T& a, const T& b)>
void LinkedPriorityQueue<T,tgt>::delete_list(LN*& front) {
//    if(front == nullptr)
//    {
//        return;
//    }
//    delete_list(front -> next);
//    delete front;
    if(front -> next != nullptr)
    {
        LN* temp = front -> next;
        front -> next = front -> next -> next;
        delete temp;
    }
}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::Iterator::Iterator(LinkedPriorityQueue<T,tgt>* iterate_over, LN* initial)
{
    prev = iterate_over->front;
    current = initial;
    expected_mod_count = iterate_over->mod_count;
    ref_pq = iterate_over;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::Iterator::~Iterator()
{}


template<class T, bool (*tgt)(const T& a, const T& b)>
T LinkedPriorityQueue<T,tgt>::Iterator::erase() {
    if (expected_mod_count != ref_pq->mod_count)
    {
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::erase");
    }
    if (!can_erase)
    {
        throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");
    }
    if (current == nullptr)
        throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor beyond data structure");


    //----------------------------------------------------------------------------------------------
    T to_return = this->current->value;
    auto itbegin = this->ref_pq->begin();

    if (this->ref_pq->used == 1) {

        delete this->ref_pq->front->next;
        this->ref_pq->front -> next = nullptr;
        this->current = nullptr;
    }
    else if (this->current == ref_pq->front-> next) {

        this->current = this->current->next;
        delete this->ref_pq->front->next;
        this->ref_pq->front -> next = this->current;
    }
    else {

        while (itbegin.current->next != this->current) {
            itbegin ++;
        }
        prev = itbegin.current;
        itbegin.current->next = this->current->next;

        if (this->current == nullptr) {
            delete this->current;
            this->current = nullptr;
        }
        else {
            delete this->current;
            this->current = prev->next;
        }
    }
    --this->ref_pq->used;
    can_erase = false;
    expected_mod_count ++;
    ref_pq->mod_count ++;
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string LinkedPriorityQueue<T,tgt>::Iterator::str() const {
    std::ostringstream answer;
    if(ref_pq-> front -> next == nullptr)
    {
        answer << ref_pq->str() << "/current=" << "nullptr" << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
    } else
    {
        answer << ref_pq->str() << "/current=" << current->value << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
    }
    return answer.str();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::Iterator::operator ++ () -> LinkedPriorityQueue<T,tgt>::Iterator& {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++");
    if (current == nullptr)
        return *this;
    if (can_erase)  //  very tricky. it will move to next node if it is erased, so when ++it, it don't need to move. TODO!
    {
        prev = current;
        current = current->next;
    }
    else
    {
        can_erase = true;
    }
    return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::Iterator::operator ++ (int) -> LinkedPriorityQueue<T,tgt>::Iterator {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++(int)");
    if (current == nullptr)
//      throw EmptyError("The iterator is null, can't move");
        return *this;
    Iterator to_return(*this);
    if (can_erase) {
        prev = current;
        current = current->next;
        return to_return;
    }
    can_erase = true;
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::Iterator::operator == (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("ArrayPriorityQueue::Iterator::operator ==");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("ArrayPriorityQueue::Iterator::operator ==");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("ArrayPriorityQueue::Iterator::operator ==");

    return current == rhsASI->current;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::Iterator::operator != (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator !=");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator !=");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator !=");

    return current != rhsASI->current;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
T& LinkedPriorityQueue<T,tgt>::Iterator::operator *() const {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("*");

    if (current == nullptr)
        throw IteratorPositionIllegal("this is nullptr!");
    if (can_erase)
        return current->value;
    else
        throw IteratorPositionIllegal("the variable can_erase is false, it means you have delete this node");
}

template<class T, bool (*tgt)(const T& a, const T& b)>
T* LinkedPriorityQueue<T,tgt>::Iterator::operator ->() const {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("*");
    if (current == nullptr)
        throw IteratorPositionIllegal("this is nullptr!");
    if (can_erase)
        return &(current->value);
    else
        throw IteratorPositionIllegal("the variable can_erase is false, it means you have delete this node");
}


}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */
