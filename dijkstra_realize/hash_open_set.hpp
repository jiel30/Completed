#ifndef HASH_OPEN_SET_HPP_
#define HASH_OPEN_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"


namespace ics {


#ifndef undefinedhashdefined
#define undefinedhashdefined
template<class T>
int undefinedhash (const T& a) {return 0;}
#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
template<class T, int (*thash)(const T& a) = undefinedhash<T>> class HashOpenSet {
  public:
    typedef ics::pair<int,T> HashedElement;   //Stores hash(element) and element see the h_element member
    typedef int (*hashfunc) (const T& a);

    //Destructor/Constructors
    ~HashOpenSet ();

    HashOpenSet (double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);
    explicit HashOpenSet (int initial_bins, double the_load_threshold = 1.0, int (*chash)(const T& k) = undefinedhash<T>);
    HashOpenSet (const HashOpenSet<T,thash>& to_copy, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);
    explicit HashOpenSet (const std::initializer_list<T>& il, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HashOpenSet (const Iterable& i, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);


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
    HashOpenSet<T,thash>& operator = (const HashOpenSet<T,thash>& rhs);
    bool operator == (const HashOpenSet<T,thash>& rhs) const;
    bool operator != (const HashOpenSet<T,thash>& rhs) const;
    bool operator <= (const HashOpenSet<T,thash>& rhs) const;
    bool operator <  (const HashOpenSet<T,thash>& rhs) const;
    bool operator >= (const HashOpenSet<T,thash>& rhs) const;
    bool operator >  (const HashOpenSet<T,thash>& rhs) const;

    template<class T2, int (*hash2)(const T2& a)>
    friend std::ostream& operator << (std::ostream& outs, const HashOpenSet<T2,hash2>& s);



  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of HashOpenSet<T,thash>
        ~Iterator();
        T           erase();
        std::string str  () const;
        HashOpenSet<T,thash>::Iterator& operator ++ ();
        HashOpenSet<T,thash>::Iterator  operator ++ (int);
        bool operator == (const HashOpenSet<T,thash>::Iterator& rhs) const;
        bool operator != (const HashOpenSet<T,thash>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const HashOpenSet<T,thash>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator HashOpenSet<T,thash>::begin () const;
        friend Iterator HashOpenSet<T,thash>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        int                 current; //Bin Index
        HashOpenSet<T,thash>*   ref_set;
        int                 expected_mod_count;
        bool                can_erase = true;

        //Called in friends begin/end
        Iterator(HashOpenSet<T,thash>* iterate_over, int initial);
    };


    Iterator begin () const;
    Iterator end   () const;


private:
  int (*hash)(const T& k);          //Hashing function used (from template or constructor)

  const static int bin_empty  = -1;   //Special values that can be stored in the bin array,
  const static int was_occupied = -2; //  along with non-negative indexes into the h_element array
  int*           bin       = nullptr; //Non-contiguous int array: bin[i] is either -1, -2, or an index into the h_element array
  HashedElement* h_element = nullptr; //Contiguous HashedElement array storing hash(element) and element as pair
                                      //bin/h_element are allocated in constructors and in ensure_lengths
  double load_threshold;              //used/bins must always be <= load_threshold (see ensure_lengths)
  int bins             = 1;           //Allocated length of bin array (>= 1, so hash_compress doesn't % 0)
  int elements         = 1;           //Allocated length of h_element array (can be < bins, if load_factor < 1)
  int used             = 0;           //# of elements in this set; used <= elements (see ensure_lengths)
                                      //  bins-used = # of bin_empty/was_occupied values in bin array
  int mod_count        = 0;           //For sensing concurrent modification


  //Helper methods
  //Given element, compute/return its hashed value and which bin it compresses to (%bins in range [0,bins-1])
  void hash_compress   (const T& element, int& hashed, int& bin_index)  const;
  int  find_element    (const T& element)  const; //Returns index to element's bin or -1 if  element is not in any bin
  int  next_unoccupied (int bin_index)   const;   //this bin_index (if unoccupied) or next bin_index beyond that
                                                  //   is unoccupied; advances using linear probing
  void ensure_lengths  (int new_used);            //Reallocate bin/h_element if load factor > load_threshold/more elements needed
};





//HashOpenSet class and related definitions

////////////////////////////////////////////////////////////////////////////////
//
//Destructor/Constructors

template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::~HashOpenSet() {
  delete [] bin;
  delete [] h_element;
}


template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::HashOpenSet(double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash<T> ? thash : chash), load_threshold(the_load_threshold) {
  if (hash == (hashfunc)undefinedhash<T>)
    throw TemplateFunctionError("HashOpenSet::default constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
    throw TemplateFunctionError("HashOpenSet::default constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenSet::default constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  bin       = new int[bins];
  h_element = new HashedElement[elements];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;
}


template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::HashOpenSet(int initial_bins, double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash<T> ? thash : chash), bins(initial_bins), load_threshold(the_load_threshold) {
  if (hash == (hashfunc)undefinedhash<T>)
    throw TemplateFunctionError("HashOpenSet::length constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
    throw TemplateFunctionError("HashOpenSet::length constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenSet::length constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  if (bins < 1)
    bins = 1;
  bin       = new int[bins];
  h_element = new HashedElement[elements];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;
}


template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::HashOpenSet(const HashOpenSet<T,thash>& to_copy, double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash<T> ? thash : chash), load_threshold(the_load_threshold), bins(to_copy.bins), elements(to_copy.elements) {
  if (hash == (hashfunc)undefinedhash<T>)
    hash = to_copy.hash;//throw TemplateFunctionError("HashOpenSet::copy constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
    throw TemplateFunctionError("HashOpenSet::copy constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenSet::length constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  if (hash == to_copy.hash && (double)to_copy.size()/to_copy.bins <= the_load_threshold) {
    used       = to_copy.used;
    bin        = new int[bins];
    h_element  = new HashedElement[elements];
    for (int b=0; b<bins; ++b)
      bin[b] = to_copy.bin[b];
    for (int e=0; e<used; ++e)
      h_element[e] = to_copy.h_element[e];
  }else {
    bins      = std::max(1,int(to_copy.size()/load_threshold));
    elements  = to_copy.used;
    bin       = new int[bins];
    h_element  = new HashedElement[elements];
    for (int b=0; b<bins; ++b)
      bin[b] = bin_empty;
    for (int e=0; e<elements; ++e)
      insert(to_copy.h_element[e].second);
  }
}


template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::HashOpenSet(const std::initializer_list<T>& il, double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash<T> ? thash : chash), load_threshold(the_load_threshold), bins(std::max(1,int(il.size()/the_load_threshold))) {
  if (hash == (hashfunc)undefinedhash<T>)
    throw TemplateFunctionError("HashOpenSet::initializer_list constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
    throw TemplateFunctionError("HashOpenSet::initializer_list constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenSet::initializer_list constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  bin       = new int[bins];
  h_element = new HashedElement[elements];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;

  for (const T& s_element : il)
    insert(s_element);
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
HashOpenSet<T,thash>::HashOpenSet(const Iterable& i, double the_load_threshold, int (*chash)(const T& a))
: hash(thash != (hashfunc)undefinedhash<T> ? thash : chash), load_threshold(the_load_threshold), bins(std::max(1,int(i.size()/the_load_threshold))) {
  if (hash == (hashfunc)undefinedhash<T>)
    throw TemplateFunctionError("HashOpenSet::Iterable constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
    throw TemplateFunctionError("HashOpenSet::Iterable constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenSet::Iterable constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  bin       = new int[bins];
  h_element = new HashedElement[elements];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;

  for (const T& s_element : i)
    insert(s_element);
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::empty() const {
  return used == 0;
}


template<class T, int (*thash)(const T& a)>
int HashOpenSet<T,thash>::size() const {
  return used;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::contains (const T& element) const {
  return find_element(element) != -1;
}

template<class T, int (*thash)(const T& a)>
std::string HashOpenSet<T,thash>::str() const {
  std::ostringstream answer;
  answer << "HashOpenSet[";
  if (bins != 0) {
    answer << std::endl;
    for (int b=0; b<bins; ++b) {
      answer << "  bin[" << b << "] = ";
      if (bin[b] < -2)
        answer << "unknown: error" << std::endl;
      else if (bin[b] == bin_empty)
        answer << "empty" << std::endl;
      else if (bin[b] == was_occupied)
        answer << "was occupied" << std::endl;
      else
        answer << bin[b] << "; with value " << h_element[bin[b]].first << " : "
               << h_element[bin[b]].second << std::endl;
    }
  }
  if (bins != 0 || elements != 0)
    answer << "--------------------" << std::endl;
  if (elements != 0) {
    for (int e=0; e<elements; ++e){
      answer << "  h_element[" << e << "] = ";
      if (e < used)
        answer << h_element[e].first << " : "
               << h_element[e].second << std::endl;
      else
        answer << "unused" << std::endl;
    }
  }
  answer  << "](load_threshold=" << load_threshold << ",bins=" << bins << ",elements=" << elements << ",used=" <<used <<",mod_count=" << mod_count << ")";
  return answer.str();
}


template<class T, int (*thash)(const T& a)>
template <class Iterable>
bool HashOpenSet<T,thash>::contains_all(const Iterable& i) const {
  for (const T& v : i)
    if (!contains(v))
      return false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, int (*thash)(const T& a)>
int HashOpenSet<T,thash>::insert(const T& element) {
  int c = find_element(element);
  if (c != -1)
      return 0;

  ensure_lengths(used+1);
  int element_hash, bin_index;
  hash_compress(element,element_hash,bin_index);
  bin_index = next_unoccupied(bin_index);
  bin[bin_index] = used;
  h_element[used] = HashedElement(element_hash,element);

  ++used;
  ++mod_count;
  return 1;
}


template<class T, int (*thash)(const T& a)>
int HashOpenSet<T,thash>::erase(const T& element) {
  int c = find_element(element);
  if (c == -1)
    return 0;

  //Keep h_element contiguous
  int bin_index     = find_element(h_element[used-1].second); //will rehash, even though h_element[used-1].first is the hash value
  bin[bin_index]    = bin[c];
  h_element[bin[c]] = h_element[used-1];
  bin[c]            = was_occupied;

  --used;
  ++mod_count;
  return 1;
}


template<class T, int (*thash)(const T& a)>
void HashOpenSet<T,thash>::clear() {
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;

  used = 0;
  ++mod_count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashOpenSet<T,thash>::insert_all(const Iterable& i) {
  int count = 0;
  for (const T& v : i)
    count += insert(v);

  return count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashOpenSet<T,thash>::erase_all(const Iterable& i) {
  int count = 0;
  for (const T& v : i)
    count += erase(v);
  return count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashOpenSet<T,thash>::retain_all(const Iterable& i) {
  HashOpenSet<T,thash> s(i);

  int count = 0;
  for (int e=0; e<used; ++e)
    if (!s.contains(h_element[e].second)) {

      int bin_erase = find_element(h_element[e].second);
      int bin_end   = find_element(h_element[used-1].second);

      bin[bin_end]              = bin[bin_erase];
      h_element[bin[bin_erase]] = h_element[used-1];
      bin[bin_erase]            = was_occupied;
      --used;

      ++count;
      --e;      // process new element in same bin index
    }
  
  if (count > 0)
    ++mod_count;

  return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>& HashOpenSet<T,thash>::operator = (const HashOpenSet<T,thash>& rhs) {
  if (this == &rhs)
    return *this;

  load_threshold = rhs.load_threshold;
  hash = rhs.hash;
  used = rhs.used;

  if (bins != rhs.bins) {
    delete [] bin;
    bins = rhs.bins;
    bin  = new int[bins];
  }
  if (elements != rhs.elements) {
    delete [] h_element;
    elements  = rhs.elements;
    h_element = new HashedElement[elements];
  }

  for (int b=0; b<rhs.bins; ++b)
    bin[b] = rhs.bin[b];
  for (int e=0; e<used; ++e)
    h_element[e] = rhs.h_element[e];

// Old specification; not using both rhs.hash and rhs.load_threshold
//  if (hash == rhs.hash) {
//    //Use a copy of the rhs data
//    bins      = rhs.bins;
//    elements  = rhs.elements;
//    used      = rhs.used;
//    bin       = new int[bins];
//    h_element = new HashedElement[elements];
//
//    for (int b=0; b<rhs.bins; ++b)
//      bin[b] = rhs.bin[b];
//    for (int e=0; e<used; ++e)
//      h_element[e] = rhs.h_element[e];
//
//  }else{
//    //Copy element pairs into with rehashing
//    bins      = std::max(1,int(rhs.size()/load_threshold));
//    elements  = rhs.used;
//    used      = 0;
//    bin       = new int[bins];
//    h_element = new HashedElement[elements];
//    for (int b=0; b<bins; ++b)
//      bin[b] = bin_empty;
//
//    for (int e=0; e<rhs.used; ++e)
//      insert(rhs.h_element[e].second);
//  }
  ++mod_count;
  return *this;

//  if (hash == rhs.hash && (double)rhs.size()/rhs.bins <= load_threshold) {
//    //Use a copy of the rhs data
//    delete [] set;
//    delete [] state;
//    bins  = rhs.bins;
//    used  = rhs.used;
//    set   = new T        [bins];
//    state = new bin_state[bins];
//
//    for (int b=0; b<rhs.bins; ++b) {
//      state[b] = rhs.state[b];
//      if (state[b] == bs_occupied)
//        set[b] = rhs.set[b];
//    }
//  }else {
//    //Copy with rehashing
//    delete[] set;
//    delete[] state;
//    bins = std::max(1,int(rhs.size()/load_threshold));
//    set   = new T        [bins];
//    state = new bin_state[bins];
//    for (int b=0; b<bins; ++b)
//      state[b] = bs_empty;
//
//    for (int b=0; b<rhs.bins; ++b) {
//      if (rhs.state[b] == bs_occupied)
//        insert(rhs.set[b]);
//    }
//  }
  ++mod_count;
  return *this;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::operator == (const HashOpenSet<T,thash>& rhs) const {
  if (this == &rhs)
    return true;
  if (used != rhs.size())
    return false;

  for (int e=0; e<used; ++e) {
      int rhs_c = rhs.find_element(h_element[e].second);
      if  (rhs_c == -1)
        return false;
    }

  return true;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::operator != (const HashOpenSet<T,thash>& rhs) const {
  return !(*this == rhs);
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::operator <= (const HashOpenSet<T,thash>& rhs) const {
  if (this == &rhs)
    return true;
  if (used > rhs.size())
    return false;

  for (int e=0; e<used; ++e)
    if (!rhs.contains(h_element[e].second))
      return false;

  return true;
}

template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::operator < (const HashOpenSet<T,thash>& rhs) const {
  if (this == &rhs)
    return false;
  if (used >= rhs.size())
    return false;

  for (int e=0; e<used; ++e)
    if (!rhs.contains(h_element[e].second))
      return false;

  return true;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::operator >= (const HashOpenSet<T,thash>& rhs) const {
  return rhs <= *this;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::operator > (const HashOpenSet<T,thash>& rhs) const {
  return rhs < *this;
}


template<class T, int (*thash)(const T& a)>
std::ostream& operator << (std::ostream& outs, const HashOpenSet<T,thash>& s) {
  outs  << "set[";

  int printed = 0;
  for (int e=0; e<s.used; ++e)
    outs << (printed++ == 0? "" : ",") << s.h_element[e].second;

  outs << "]";
  return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T, int (*thash)(const T& a)>
auto HashOpenSet<T,thash>::begin () const -> HashOpenSet<T,thash>::Iterator {
  return Iterator(const_cast<HashOpenSet<T,thash>*>(this),0);
}


template<class T, int (*thash)(const T& a)>
auto HashOpenSet<T,thash>::end () const -> HashOpenSet<T,thash>::Iterator {
  return Iterator(const_cast<HashOpenSet<T,thash>*>(this),used);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, int (*thash)(const T& a)>
void HashOpenSet<T,thash>::hash_compress (const T& element, int& hashed, int& bin_index) const {
  hashed    = hash(element);
  bin_index = abs(hashed) % bins;
}


template<class T, int (*thash)(const T& a)>
int HashOpenSet<T,thash>::find_element (const T& element) const {
  int element_hash, bin_index;
  hash_compress(element,element_hash,bin_index);
  int bin_index_original = bin_index;
  do {
    if (bin[bin_index] == bin_empty)
      return -1;
    if (bin[bin_index] > bin_empty  &&                     //indexes occupied bin_index, and
        h_element[bin[bin_index]].first == element_hash &&   //..with same hash code, and -fast int check to avoid == key
        h_element[bin[bin_index]].second == element)         //..wisth same element
      return bin_index;
    bin_index = ++bin_index % bins; //linear probing;
  } while (bin_index != bin_index_original);

  return -1;
}

template<class T, int (*thash)(const T& a)>
  int HashOpenSet<T,thash>::next_unoccupied (int bin_index) const {
    for (/*bin*/; bin[bin_index] >= 0; bin_index = ++bin_index % bins) //linear probing
      ;

    //load_threshold <= 1 guarantees something is empty/was_occupied
    return bin_index;
  }


template<class T, int (*thash)(const T& a)>
void HashOpenSet<T,thash>::ensure_lengths(int new_used) {
  // Check new_used load_threshold and elements

  if (double(new_used)/double(bins) > load_threshold) {
    delete [] bin;

    int old_bins  = bins;                //bins always is >= 1; doubling always increases
    bins  = 2*old_bins;
    bin   = new int[bins];

    for (int b=0; b<bins; ++b)
      bin[b] = bin_empty;

    for (int e=0; e<used; ++e) {
      int bin_index = next_unoccupied(abs(h_element[e].first) % bins);
      bin[bin_index] = e;
    }
  }

  if (new_used > elements) {
    int old_entries = elements;             //entries always >= 0
    HashedElement* old_h_element = h_element;
    elements = std::max(1,2*old_entries);   //handle increase in case of entries = 0
    h_element = new HashedElement[elements];
    for (int e=0; e <used; ++e)
      h_element[e] = old_h_element[e];
    delete [] old_h_element;
  }

}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::Iterator::Iterator(HashOpenSet<T,thash>* iterate_over, int initial)
: ref_set(iterate_over), expected_mod_count(ref_set->mod_count) {
  current = initial;
}


template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::Iterator::~Iterator()
{}


template<class T, int (*thash)(const T& a)>
T HashOpenSet<T,thash>::Iterator::erase() {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HashOpenSet::Iterator::erase Iterator cursor already erased");
  if (current < 0 || current >= ref_set->bins)
    throw CannotEraseError("HashOpenSet::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  T to_return = ref_set->h_element[current].second;

  int bin_erase = ref_set->find_element(ref_set->h_element[current].second);
  int bin_end   = ref_set->find_element(ref_set->h_element[ref_set->used-1].second);

  ref_set->bin[bin_end]                       = ref_set->bin[bin_erase];
  ref_set->h_element[ref_set->bin[bin_erase]] = ref_set->h_element[ref_set->used-1];
  ref_set->bin[bin_erase]                     = ref_set->was_occupied;

  --ref_set->used;
  ++ref_set->mod_count;
  expected_mod_count = ref_set->mod_count;

  return to_return;
}


template<class T, int (*thash)(const T& a)>
std::string HashOpenSet<T,thash>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_set->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}


template<class T, int (*thash)(const T& a)>
auto  HashOpenSet<T,thash>::Iterator::operator ++ () -> HashOpenSet<T,thash>::Iterator& {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::operator ++");

  if (current >= ref_set->used)
    return *this;

  if (can_erase)
    ++current;

  can_erase = true;

  return *this;
}


template<class T, int (*thash)(const T& a)>
auto  HashOpenSet<T,thash>::Iterator::operator ++ (int) -> HashOpenSet<T,thash>::Iterator {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::operator ++(int)");

  Iterator to_return(*this);

  if (current >= ref_set->used)
    return *this;

  if (can_erase)
    ++current;

  can_erase = true;

  return to_return;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::Iterator::operator == (const HashOpenSet<T,thash>::Iterator& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashOpenSet::Iterator::operator ==");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::operator ==");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashOpenSet::Iterator::operator ==");

  return this->current == rhsASI->current;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::Iterator::operator != (const HashOpenSet<T,thash>::Iterator& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashOpenSet::Iterator::operator !=");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::operator !=");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashOpenSet::Iterator::operator !=");

  return this->current != rhsASI->current;
}

template<class T, int (*thash)(const T& a)>
T& HashOpenSet<T,thash>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::operator *");
  if (!can_erase || current < 0 || current >= ref_set->bins)
    throw IteratorPositionIllegal("HashOpenSet::Iterator::operator * Iterator illegal: exhausted");

  return ref_set->h_element[current].second;
}

template<class T, int (*thash)(const T& a)>
T* HashOpenSet<T,thash>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashOpenSet::Iterator::operator *");
  if (!can_erase || current < 0 || current >= ref_set->bins)
    throw IteratorPositionIllegal("HashOpenSet::Iterator::operator * Iterator illegal: exhausted");

  return &(ref_set->set[current].second);
}

}

#endif /* HASH_OPEN_SET_HPP_ */
