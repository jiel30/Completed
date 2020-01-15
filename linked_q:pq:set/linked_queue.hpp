// Submitter: jiel30(Jie, Li)
// Partner  : junfenl3(Junfeng, Li)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef LINKED_QUEUE_HPP_
#define LINKED_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"


namespace ics {


template<class T> class LinkedQueue {
  public:
    //Destructor/Constructors
    ~LinkedQueue();

    LinkedQueue          ();
    LinkedQueue          (const LinkedQueue<T>& to_copy);
    explicit LinkedQueue (const std::initializer_list<T>& il);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit LinkedQueue (const Iterable& i);


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
    LinkedQueue<T>& operator = (const LinkedQueue<T>& rhs);
    bool operator == (const LinkedQueue<T>& rhs) const;
    bool operator != (const LinkedQueue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedQueue<T2>& q);



  private:
    class LN;

  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of LinkedQueue<T>
        ~Iterator();
        T           erase();
        std::string str  () const;
        LinkedQueue<T>::Iterator& operator ++ ();
        LinkedQueue<T>::Iterator  operator ++ (int);
        bool operator == (const LinkedQueue<T>::Iterator& rhs) const;
        bool operator != (const LinkedQueue<T>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const LinkedQueue<T>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator LinkedQueue<T>::begin () const;
        friend Iterator LinkedQueue<T>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        LN*             prev = nullptr;  //if nullptr, current at front of list
        LN*             current;         //current == prev->next (if prev != nullptr)
        LinkedQueue<T>* ref_queue;
        int             expected_mod_count;
        bool            can_erase = true;

        //Called in friends begin/end
        Iterator(LinkedQueue<T>* iterate_over, LN* initial);
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


    LN* front     =  nullptr;
    LN* rear      =  nullptr;
    int used      =  0;            //Cache the node count in this linked list
    int mod_count =  0;            //Used to sense for concurrent modification

    //Helper methods
    void delete_list(LN*& front);  //Deallocate all LNs, and set front's argument to nullptr;
};





////////////////////////////////////////////////////////////////////////////////
//
//LinkedQueue class and related definitions

//Destructor/Constructors

template<class T>
LinkedQueue<T>::~LinkedQueue() {
  delete_list(front);
}


template<class T>
LinkedQueue<T>::LinkedQueue() {
    LN* queue;
}


template<class T>
LinkedQueue<T>::LinkedQueue(const LinkedQueue<T>& to_copy) {
    for(auto& v:to_copy){
        enqueue(v);
    }
}


template<class T>
LinkedQueue<T>::LinkedQueue(const std::initializer_list<T>& il) {
    for(const T& q_elem : il){
        enqueue(q_elem);
    }
}


template<class T>
template<class Iterable>
LinkedQueue<T>::LinkedQueue(const Iterable& i) {
    for(const T& v : i){
        enqueue(v);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T>
bool LinkedQueue<T>::empty() const {
    return used == 0;
}


template<class T>
int LinkedQueue<T>::size() const {
    return used;
}


template<class T>
T& LinkedQueue<T>::peek () const {
    if(this->empty()){
        throw EmptyError("LinkedQueue::peek");
    }
    return front->value;
}


template<class T>
std::string LinkedQueue<T>::str() const {
    std::ostringstream answer;
    answer << "LinkedQueue[";
    int count = 1;
    LN* x = front;
    if(used == 0){
        answer << "]";
//        answer << "(length=" << used << ",front=" << front->value << ",rear=" << rear->value << ",mod_count=" << mod_count << ")";
        answer << "(length=" << used <<  ",mod_count=" << mod_count << ")";
        return answer.str();
    }
    while(x != nullptr){
        answer << count << ":";
        answer << x->value;
        if(x->next != nullptr){
            answer << ",";
        }
        x = x->next;
        ++count;
    }
    answer << "]";
    answer << "(length=" << used << ",front=" << front->value << ",rear=" << rear->value << ",mod_count=" << mod_count << ")";
    return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T>
int LinkedQueue<T>::enqueue(const T& element) {
    LN* x = new LN(element, nullptr);
    if(front == nullptr and rear == nullptr){
        front = x;
        rear = x;
    }
    else{
        rear->next = x;
        rear = x;
    }
    ++used;
    ++mod_count;
    return 1;
}


template<class T>
T LinkedQueue<T>::dequeue() {
    if(used == 0){
        throw EmptyError("LinkedQueue:dequeue");
    }
    if (front == rear) {
      rear = nullptr;
    }
    LN* s = this->front;
    T answer = s->value;
    this->front = this->front->next;
    delete s;
    --used;
    ++mod_count;
    return answer;
}


template<class T>
void LinkedQueue<T>::clear() {
    if(front == nullptr){
        return;
    }
    while(front != nullptr){
        LN* s = front;
        front = front->next;
        delete s;
    }
    rear = nullptr;
    used = 0;
    ++mod_count;
//    front->next = rear;
}


template<class T>
template<class Iterable>
int LinkedQueue<T>::enqueue_all(const Iterable& i) {
    int count = 0;
    for(const T& v : i){
        count += enqueue(v);
    }

    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T>
LinkedQueue<T>& LinkedQueue<T>::operator = (const LinkedQueue<T>& rhs) {

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
      if (tmp == nullptr) {
        enqueue(v);
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
bool LinkedQueue<T>::operator == (const LinkedQueue<T>& rhs) const {
    if(this->front == nullptr and rhs.front == nullptr){
        return true;
    }
    else if(this->front == nullptr or rhs.front == nullptr){
        return false;
    }
    auto i = front;
    auto j = rhs.front;
    while(true){
        if(i->value == j->value){
          i = i->next;
          j = j->next;
          if (i == nullptr and j == nullptr)
            return true;
          else if (i == nullptr or j == nullptr)
            return false;
        }
        else{
            return false;
        }
    }
}


template<class T>
bool LinkedQueue<T>::operator != (const LinkedQueue<T>& rhs) const {
    if(*this == rhs){
        return false;
    }
    return true;
}


template<class T>
std::ostream& operator << (std::ostream& outs, const LinkedQueue<T>& q) {

    outs << "queue[";

    auto tmp = q.front;
    while(tmp != nullptr){
        outs << tmp->value;
        if(tmp->next != nullptr){
            outs << ",";
        }
        tmp = tmp->next;
    }

    outs << "]:rear";
    return outs;

}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T>
auto LinkedQueue<T>::begin () const -> LinkedQueue<T>::Iterator {
    return Iterator(const_cast<LinkedQueue<T>*>(this),front);
}

template<class T>
auto LinkedQueue<T>::end () const -> LinkedQueue<T>::Iterator {
    return Iterator(const_cast<LinkedQueue<T>*>(this), nullptr);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T>
void LinkedQueue<T>::delete_list(LN*& front) {
  if (front == nullptr) return;
  delete_list(front->next);
  delete front;
}





////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T>
LinkedQueue<T>::Iterator::Iterator(LinkedQueue<T>* iterate_over, LN* initial)
: current(initial), ref_queue(iterate_over), expected_mod_count(ref_queue->mod_count){
}


template<class T>
LinkedQueue<T>::Iterator::~Iterator()
{}


template<class T>
T LinkedQueue<T>::Iterator::erase() {
    if (expected_mod_count != ref_queue->mod_count)
        throw ConcurrentModificationError("LinkedQueue::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor already erased");

    T to_return = this->current->value;

    auto itbegin = this->ref_queue->begin();

    if (this->ref_queue->used == 1) {

      delete this->ref_queue->front;
      this->ref_queue->front = this->ref_queue->rear = nullptr;
      this->current = nullptr;
    }
    else if (this->current == ref_queue->front) {

      this->current = this->current->next;
      delete this->ref_queue->front;
      this->ref_queue->front = this->current;
    }
    else {

      while (itbegin.current->next != this->current) {
        itbegin ++;
      }
      prev = itbegin.current;
      itbegin.current->next = this->current->next;

      if (this->current == ref_queue->rear) {
        delete this->current;
        this->current = nullptr;
        this->ref_queue->rear = prev;
      }
      else {
        delete this->current;
        this->current = prev->next;
      }
    }
    --this->ref_queue->used;
    can_erase = false;
    expected_mod_count ++;
    ref_queue->mod_count ++;
    return to_return;

}


template<class T>
std::string LinkedQueue<T>::Iterator::str() const {
    std::ostringstream answer;
    if(this->current == nullptr){
        answer << ref_queue->str() << "(current=nullptr" << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
        return answer.str();
    }
    answer << ref_queue->str() << "(current=" << current->value << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
    return answer.str();
}


template<class T>
auto LinkedQueue<T>::Iterator::operator ++ () -> LinkedQueue<T>::Iterator& {
    if (expected_mod_count != ref_queue->mod_count)
        throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++");
    if (current == nullptr)
//    if (current->next == nullptr)
//      throw EmptyError("The iterator is null, can't move");
      return *this;
    if (can_erase)  //  very tricky. it will move to next node if it is erased, so when ++it, it don't need to move. TODO!
      current = current->next;
    else
      can_erase = true;
    return *this;
}


template<class T>
auto LinkedQueue<T>::Iterator::operator ++ (int) -> LinkedQueue<T>::Iterator {
    if (expected_mod_count != ref_queue->mod_count)
        throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++(int)");
    if (current == nullptr)
//    if (current->next == nullptr)
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
bool LinkedQueue<T>::Iterator::operator == (const LinkedQueue<T>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    return current == rhsASI->current;
}


template<class T>
bool LinkedQueue<T>::Iterator::operator != (const LinkedQueue<T>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    return current != rhsASI->current;
}


template<class T>
T& LinkedQueue<T>::Iterator::operator *() const {
   if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("*");

  if (current == nullptr)
      throw IteratorPositionIllegal("this is nullptr!");
    if (can_erase)
      return current->value;
    else
      throw IteratorPositionIllegal("the variable can_erase is false, it means you have delete this node");
}


template<class T>
T* LinkedQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("*");
  if (current == nullptr)
    throw IteratorPositionIllegal("this is nullptr!");
  if (can_erase)
    return &(current->value);
  else
    throw IteratorPositionIllegal("the variable can_erase is false, it means you have delete this node");
}


}

#endif /* LINKED_QUEUE_HPP_ */
