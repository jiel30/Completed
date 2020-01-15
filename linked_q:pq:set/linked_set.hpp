// Submitter: jiel30(Jie, Li)
// Partner  : junfenl3(Junfeng, Li)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef LINKED_SET_HPP_
#define LINKED_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"


namespace ics {


template<class T> class LinkedSet {
  public:
    //Destructor/Constructors
    ~LinkedSet();

    LinkedSet          ();
    explicit LinkedSet (int initialLength);
    LinkedSet          (const LinkedSet<T>& to_copy);
    explicit LinkedSet (const std::initializer_list<T>& il);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit LinkedSet (const Iterable& i);


    //Queries
    bool empty      () const;
    int  size       () const;
    bool contains   (const T& element) const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    bool contains_all (const Iterable& i) const;


    //Commands
    int  insert (const T& element);
    int  erase  (const T& element);
    void clear  ();

    //Iterable class must support "for" loop: .begin()/.end() and prefix ++ on returned result

    template <class Iterable>
    int insert_all(const Iterable& i);

    template <class Iterable>
    int erase_all(const Iterable& i);

    template<class Iterable>
    int retain_all(const Iterable& i);


    //Operators
    LinkedSet<T>& operator = (const LinkedSet<T>& rhs);
    bool operator == (const LinkedSet<T>& rhs) const;
    bool operator != (const LinkedSet<T>& rhs) const;
    bool operator <= (const LinkedSet<T>& rhs) const;
    bool operator <  (const LinkedSet<T>& rhs) const;
    bool operator >= (const LinkedSet<T>& rhs) const;
    bool operator >  (const LinkedSet<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedSet<T2>& s);



  private:
    class LN;

  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of LinkedSet<T>
        ~Iterator();
        T           erase();
        std::string str  () const;
        LinkedSet<T>::Iterator& operator ++ ();
        LinkedSet<T>::Iterator  operator ++ (int);
        bool operator == (const LinkedSet<T>::Iterator& rhs) const;
        bool operator != (const LinkedSet<T>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const LinkedSet<T>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator LinkedSet<T>::begin () const;
        friend Iterator LinkedSet<T>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        LN*           current;  //if can_erase is false, this value is unusable
        LinkedSet<T>* ref_set;
        int           expected_mod_count;
        bool          can_erase = true;

        //Called in friends begin/end
        Iterator(LinkedSet<T>* iterate_over, LN* initial);
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
        LN* next   = nullptr;
    };


    LN* front     = new LN();
    LN* trailer   = front;        //Invariant: always points to the trailer node of list
    int used      =  0;           //Cache the node count in this linked list
    int mod_count =  0;           //Used to sense for concurrent modification

    //Helper methods
    int  erase_at   (LN* p);
    void delete_list(LN*& front);  //Deallocate all LNs (but trailer), and set front's argument to trailer;
};





////////////////////////////////////////////////////////////////////////////////
//
//LinkedSet class and related definitions

//Destructor/Constructors

template<class T>
LinkedSet<T>::~LinkedSet() {
    delete_list(front);
}


template<class T>
LinkedSet<T>::LinkedSet() {
//    LinkedSet* set = new LinkedSet();
    front = new LN;
    front -> next = nullptr;
    trailer = front;
}


template<class T>
LinkedSet<T>::LinkedSet(const LinkedSet<T>& to_copy) : used(to_copy.used) {
    mod_count = to_copy.mod_count;
    front = new LN(to_copy.front -> value);
    LN* current = front;
    LN* track = to_copy.front->next;
    while(track != to_copy.trailer)
    {
        LN* a = new LN(track->value,NULL);
        current -> next = a;
        current = current -> next;
        track = track->next;
    }
    trailer = new LN;
    trailer -> next = NULL;
    current -> next = trailer;
}


template<class T>
LinkedSet<T>::LinkedSet(const std::initializer_list<T>& il) {
    for(auto aa : il)
    {
        insert(aa);
    }

}


template<class T>
template<class Iterable>
LinkedSet<T>::LinkedSet(const Iterable& i) {
    for(const T& v : i){
        insert(v);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T>
bool LinkedSet<T>::empty() const {
    return used == 0;
}


template<class T>
int LinkedSet<T>::size() const {
    return used;
}


template<class T>
bool LinkedSet<T>::contains (const T& element) const {
    LN* x = front;
    while(x != trailer){
        if(x->value == element){
            return true;
        }
        x = x->next;
    }

    return false;
}


template<class T>
std::string LinkedSet<T>::str() const {
    std::ostringstream answer;
    answer << "LinkedSet[";
    int count = 1;
    LN* x = front;
    if(used == 0){
        answer << "]";
//        answer << "(length=" << used << ",front=" << front->value << ",rear=" << rear->value << ",mod_count=" << mod_count << ")";
        answer << "(length=" << used <<  ",mod_count=" << mod_count << ")";
        return answer.str();
    }
    while(x != trailer){
        answer << count << ":";
        answer << x->value;
        if(x->next != trailer){
            answer << ",";
        }
        x = x->next;
        ++count;
    }
    answer << "]";
    answer << "(length=" << used << ",front=" << front->value << ",mod_count=" << mod_count << ")";
    return answer.str();
}


template<class T>
template<class Iterable>
bool LinkedSet<T>::contains_all (const Iterable& i) const {
    for (const T& v : i)
        if (!contains(v))
            return false;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands


template<class T>
int LinkedSet<T>::insert(const T& element) {
    LN* tmp = front;
    while (tmp != trailer) {
        if (tmp->value == element) {
            return 0;
        }
        tmp = tmp->next;
    }
    front = new LN(element, front);
    ++used;
    ++mod_count;
    return 1;
}


template<class T>
int LinkedSet<T>::erase(const T& element) {
    if (used == 0) return 0;
    LN* x = front;

    if(front->value == element){
        front = front->next;
        --used;
        ++mod_count;
        delete x;
        return 1;
    }
    while(x->next != trailer){
        if(x->next->value == element){
            return erase_at(x);
        }
        x = x->next;
    }

    return 0;
}


template<class T>
void LinkedSet<T>::clear() {
    if(front == trailer){
        return;
    }
    while(front != trailer){
        LN* s = front;
        front = front->next;
        delete s;
    }
    front = trailer;
    used = 0;
    ++mod_count;
}


template<class T>
template<class Iterable>
int LinkedSet<T>::insert_all(const Iterable& i) {
    int count = 0;
    for(const T& v : i){
        count += insert(v);
    }

    return count;
}


template<class T>
template<class Iterable>
int LinkedSet<T>::erase_all(const Iterable& i) {
    int count = 0;
    for(const T& v : i){
        count += erase(v);
    }

    return count;
}


template<class T>
template<class Iterable>
int LinkedSet<T>::retain_all(const Iterable& i) {
    if(used == 0)
    {
        return 0;
    }
    int count = 0;
    LN* temp = front;
    bool find_it = false;
//    std::cout << i << std::endl;
    while(temp != trailer)
    {
        for(auto aa: i)
        {
//            std::cout << aa << "\n";
            if(temp->value == aa)
            {
//                std::cout << "find: " << aa << "\t...";
                find_it = true;
                break;
            }
        }
        if(find_it == false)
        {
            std::cout << "not find: " << temp->value << std::endl;
            LN* a = temp;
            temp = temp-> next;
            erase(a->value);
            count ++;
            find_it = false;
            continue;
        }
        temp = temp->next;
        find_it = false;
    }
    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T>
LinkedSet<T>& LinkedSet<T>::operator = (const LinkedSet<T>& rhs) {
    if (used > rhs.used) {
        while (used > rhs.used) {
            LN* tmp = front;
            front = front->next;
            delete tmp;
            --used;
        }
    }
    LN* tmp = front;
    for (auto& v: rhs) {
        if (tmp == trailer) {
            insert(v);
        }
        else {
            tmp->value = v;
            tmp = tmp->next;
        }
    }
    mod_count = 0;
    return *this;
}


template<class T>
bool LinkedSet<T>::operator == (const LinkedSet<T>& rhs) const {
    if (this == &rhs)
        return true;

    if (used != rhs.size())
        return false;

    auto i = front;
    while (i != trailer) {
        if(!rhs.contains(i->value)){
            return false;
        }
        i = i->next;
    }

    return true;
}


template<class T>
bool LinkedSet<T>::operator != (const LinkedSet<T>& rhs) const {
    return !(rhs == *this);
}


template<class T>
bool LinkedSet<T>::operator <= (const LinkedSet<T>& rhs) const {
    if (this == &rhs)
        return true;

    if (used > rhs.size())
        return false;

    auto i = front;
    while (i != trailer) {
        if(!rhs.contains(i->value)){
            return false;
        }
        i = i->next;
    }

    return true;
}


template<class T>
bool LinkedSet<T>::operator < (const LinkedSet<T>& rhs) const {
    if (this == &rhs)
        return false;

    if (used >= rhs.size())
        return false;

    auto i = front;
    while (i != trailer) {
        if(!rhs.contains(i->value)){
            return false;
        }
        i = i->next;
    }
    return true;
}


template<class T>
bool LinkedSet<T>::operator >= (const LinkedSet<T>& rhs) const {
    return rhs <= *this;
}


template<class T>
bool LinkedSet<T>::operator > (const LinkedSet<T>& rhs) const {
    return rhs < *this;
}


template<class T>
std::ostream& operator << (std::ostream& outs, const LinkedSet<T>& s) {
    outs << "set[";
    auto tmp = s.front;
    while(tmp != s.trailer){
        outs << tmp->value;
        if(tmp->next != s.trailer){
            outs << ",";
        }
        tmp = tmp->next;
    }

    outs << "]";
    return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T>
auto LinkedSet<T>::begin () const -> LinkedSet<T>::Iterator {
    return Iterator(const_cast<LinkedSet<T>*>(this),front);
}


template<class T>
auto LinkedSet<T>::end () const -> LinkedSet<T>::Iterator {
    return Iterator(const_cast<LinkedSet<T>*>(this), trailer);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T>
int LinkedSet<T>::erase_at(LN* p) {
    LN* x = p->next;
    p->next = p->next->next;
    delete x;
    --used;
    ++mod_count;
    return 1;
}


template<class T>
void LinkedSet<T>::delete_list(LN*& front) {
    if (front == trailer) return;
    delete_list(front->next);
    delete front;
}





////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T>
LinkedSet<T>::Iterator::Iterator(LinkedSet<T>* iterate_over, LN* initial):
        current(initial), ref_set(iterate_over), expected_mod_count(ref_set->mod_count)
{
}


template<class T>
LinkedSet<T>::Iterator::~Iterator()
{}


template<class T>
T LinkedSet<T>::Iterator::erase() {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor already erased");

    T to_return = this->current->value;

    auto itbegin = this->ref_set->begin();

    if (*this == itbegin) {
        auto tp = this->current->next;
        delete this->current;
        this->current = tp;
        this->ref_set->front = tp;
    } else {
        while (itbegin.current->next != this->current) ++itbegin;
        itbegin.current->next = current->next;
        delete current;
        current = itbegin.current->next;
    }
    --ref_set->used;

    can_erase = false;
    expected_mod_count ++;
    ref_set->mod_count ++;
    return to_return;
}


template<class T>
std::string LinkedSet<T>::Iterator::str() const {
    std::ostringstream answer;
    if(this->current == ref_set->trailer){
        answer << ref_set->str() << "(current=end" << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
        return answer.str();
    }
    answer << ref_set->str() << "(current=" << current->value << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
    return answer.str();
}


template<class T>
auto LinkedSet<T>::Iterator::operator ++ () -> LinkedSet<T>::Iterator& {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator ++");
    if (current == ref_set->trailer)
//      throw EmptyError("The iterator is null, can't move");
        return *this;
    if (can_erase)  //  very tricky. it will move to next node if it is erased, so when ++it, it don't need to move. TODO!
        current = current->next;
    else
        can_erase = true;
    return *this;
}


template<class T>
auto LinkedSet<T>::Iterator::operator ++ (int) -> LinkedSet<T>::Iterator {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator ++(int)");
    if (current == ref_set->trailer)
//      throw EmptyError("The iterator is null, can't move");
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        current = current->next;
    else
        can_erase = true;

    return to_return;
}


template<class T>
bool LinkedSet<T>::Iterator::operator == (const LinkedSet<T>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    return current == rhsASI->current;
}


template<class T>
bool LinkedSet<T>::Iterator::operator != (const LinkedSet<T>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    return current != rhsASI->current;
}


template<class T>
T& LinkedSet<T>::Iterator::operator *() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("*");

    if (current == ref_set->trailer)
        throw IteratorPositionIllegal("this is the end!");
    if (can_erase)
        return current->value;
    else
        throw IteratorPositionIllegal("the variable can_erase is false, it means you have delete this node");
}


template<class T>
T* LinkedSet<T>::Iterator::operator ->() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("*");
    if (current == ref_set->trailer)
        throw IteratorPositionIllegal("this is the end!");
    if (can_erase)
        return &(current->value);
    else
        throw IteratorPositionIllegal("the variable can_erase is false, it means you have delete this node");
}


}

#endif /* LINKED_SET_HPP_ */
