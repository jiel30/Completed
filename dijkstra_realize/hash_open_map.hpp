#ifndef HASH_OPEN_MAP_HPP_
#define HASH_OPEN_MAP_HPP_

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
template<class KEY, class T, int (*thash)(const KEY& a) = undefinedhash<KEY>> class HashOpenMap {
  public:
    typedef ics::pair<KEY,T>      Entry;         //Stores key and its associated value
    typedef ics::pair<int,Entry>  HashedEntry;   //Stores hash(key) and Entry (key/value); see the h_entry member
    typedef int (*hashfunc) (const KEY& a);

    //Destructor/Constructors
    ~HashOpenMap ();

    HashOpenMap          (double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);
    explicit HashOpenMap (int initial_bins, double the_load_threshold = 1.0, int (*chash)(const KEY& k) = undefinedhash<KEY>);
    HashOpenMap          (const HashOpenMap<KEY,T,thash>& to_copy, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);
    explicit HashOpenMap (const std::initializer_list<Entry>& il, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HashOpenMap (const Iterable& i, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);


    //Queries
    bool empty      () const;
    int  size       () const;
    bool has_key    (const KEY& key) const;
    bool has_value  (const T& value) const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<


    //Commands
    T    put   (const KEY& key, const T& value);
    T    erase (const KEY& key);
    void clear ();

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    int put_all(const Iterable& i);


    //Operators

    T&       operator [] (const KEY&);
    const T& operator [] (const KEY&) const;
    HashOpenMap<KEY,T,thash>& operator = (const HashOpenMap<KEY,T,thash>& rhs);
    bool operator == (const HashOpenMap<KEY,T,thash>& rhs) const;
    bool operator != (const HashOpenMap<KEY,T,thash>& rhs) const;

    template<class KEY2,class T2, int (*hash2)(const KEY2& a)>
    friend std::ostream& operator << (std::ostream& outs, const HashOpenMap<KEY2,T2,hash2>& m);


  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of HashOpenMap<T>
        ~Iterator();
        Entry       erase();
        std::string str  () const;
        HashOpenMap<KEY,T,thash>::Iterator& operator ++ ();
        HashOpenMap<KEY,T,thash>::Iterator  operator ++ (int);
        bool operator == (const HashOpenMap<KEY,T,thash>::Iterator& rhs) const;
        bool operator != (const HashOpenMap<KEY,T,thash>::Iterator& rhs) const;
        Entry& operator *  () const;
        Entry* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const HashOpenMap<KEY,T,thash>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator HashOpenMap<KEY,T,thash>::begin () const;
        friend Iterator HashOpenMap<KEY,T,thash>::end   () const;

      private:
        //If can_erase is false, must ++ to reach next value
        int                       current; //Bin Index
        HashOpenMap<KEY,T,thash>* ref_map;
        int                       expected_mod_count;
        bool                      can_erase = true;

        //Called in friends begin/end
        Iterator(HashOpenMap<KEY,T,thash>* iterate_over, int initial);
    };


    Iterator begin () const;
    Iterator end   () const;



private:
  int (*hash)(const KEY& k);          //Hashing function used (from template or constructor)

  const static int bin_empty  = -1;   //Special values that can be stored in the bin array,
  const static int was_occupied = -2; //  along with non-negative indexes into the h_entry array
  int*         bin     = nullptr;     //Non-contiguous int array: bin[i] is either -1, -2, or an index into the h_entry array
  HashedEntry* h_entry = nullptr;     //Contiguous HashedEntry array storing hash(key) and key/value as pair of pair
                                      //bin/h_entry are allocated in constructors and in ensure_lengths
  double load_threshold;              //used/bins must always be <= load_threshold (see ensure_lengths)
  int bins             = 1;           //Allocated length of bin array (>= 1, so hash_compress doesn't % 0)
  int entries          = 1;           //Allocated length of h_entry array (can be < bins, if load_factor < 1)
  int used             = 0;           //# of key/value associations in this map; used <= entries (see ensure_lengths)
                                      //  bins-used = # of bin_empty/was_occupied values in bin array
  int mod_count        = 0;           //For sensing concurrent modification


  //Helper methods
  //Given key, compute/return its hashed value and which bin it compresses to (%bins in range [0,bins-1])
  void hash_compress   (const KEY& key, int& hashed, int& bin_index)  const;
  int  find_key        (const KEY& key)  const;  //Returns index to key's bin or -1 if  key is not in any bin
  int  next_unoccupied (int bin_index)   const;  //this bin_index (if unoccupied) or next bin_index beyond that
                                                 //   is unoccupied; advances using linear probing
  void ensure_lengths  (int new_used);           //Reallocate bin/h_entry if load factor > load_threshold/more entries needed
};




////////////////////////////////////////////////////////////////////////////////
//
//HashOpenMap class and related definitions

//Destructor/Constructors

template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::~HashOpenMap() {
  delete [] bin;
  delete [] h_entry;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash), load_threshold(the_load_threshold) {
//std::cout << "in HashOpenMap default constructor " << (void*)thash << " and " << (void*)chash << std::endl;
//std::cout << "the_load_threshold = " << the_load_threshold << " and  undefinedhash = " << (void*)undefinedhash<KEY> << std::endl;
  if (hash == (hashfunc)undefinedhash<KEY>)
    throw TemplateFunctionError("HashOpenMap::default constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    throw TemplateFunctionError("HashOpenMap::default constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenMap::default constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  bin     = new int[bins];
  h_entry = new HashedEntry[entries];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(int initial_bins, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash), bins(initial_bins), load_threshold(the_load_threshold) {
  if (hash == (hashfunc)undefinedhash<KEY>)
    throw TemplateFunctionError("HashOpenMap::length constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    throw TemplateFunctionError("HashOpenMap::length constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenMap::length constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  if (bins < 1)
    bins = 1;

  bin     = new int[bins];
  h_entry = new HashedEntry[entries];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(const HashOpenMap<KEY,T,thash>& to_copy, double the_load_threshold, int (*chash)(const KEY& a))
: hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash), load_threshold(the_load_threshold), bins(to_copy.bins), entries(to_copy.entries) {
  if (hash == (hashfunc)undefinedhash<KEY>)
    hash = to_copy.hash;//throw TemplateFunctionError("HashOpenMap::copy constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    throw TemplateFunctionError("HashOpenMap::copy constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenMap::length constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  if (hash == to_copy.hash && (double)to_copy.size()/to_copy.bins <= the_load_threshold) {
    used     = to_copy.used;
    bin      = new int[bins];
    h_entry  = new HashedEntry[entries];
    for (int b=0; b<bins; ++b)
      bin[b] = to_copy.bin[b];
    for (int e = 0; e < used; ++e)
      h_entry[e] = to_copy.h_entry[e];
  }else{
    bins    = std::max(1,int(to_copy.size()/load_threshold));
    entries = to_copy.used;
    bin     = new int[bins];
    h_entry = new HashedEntry[entries];
    for (int b=0; b<bins; ++b)
      bin[b] = bin_empty;
    for (int e=0; e<entries; ++e)
      put(to_copy.h_entry[e].second.first,to_copy.h_entry[e].second.second);
  }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(const std::initializer_list<Entry>& il, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash), load_threshold(the_load_threshold), bins(std::max(1,int(il.size()/the_load_threshold))) {
  if (hash == (hashfunc)undefinedhash<KEY>)
    throw TemplateFunctionError("HashOpenMap::initializer_list constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    throw TemplateFunctionError("HashOpenMap::initializer_list constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenMap::initializer_list constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  bin     = new int[bins];
  h_entry = new HashedEntry[entries];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;

  for (const Entry& m_entry : il)
    put(m_entry.first,m_entry.second);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template <class Iterable>
HashOpenMap<KEY,T,thash>::HashOpenMap(const Iterable& i, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash), load_threshold(the_load_threshold), bins(std::max(1,int(i.size()/the_load_threshold))) {
  if (hash == (hashfunc)undefinedhash<KEY>)
    throw TemplateFunctionError("HashOpenMap::Iterable constructor: neither specified");
  if (thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    throw TemplateFunctionError("HashOpenMap::Iterable constructor: both specified and different");
  if (the_load_threshold > 1.) {
    std::ostringstream answer;
    answer << "HashOpenMap::Iterable constructor: the_load_threshold(" << the_load_threshold << ") > 1";
    throw IcsError(answer.str());
  }

  bin     = new int[bins];
  h_entry = new HashedEntry[entries];
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;

  for (const Entry& m_entry : i)
    put(m_entry.first,m_entry.second);
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::empty() const {
  return used == 0;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashOpenMap<KEY,T,thash>::size() const {
  return used;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::has_key (const KEY& key) const {
  return find_key(key) != -1;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::has_value (const T& value) const {
  for (int e=0; e<used; ++e)
    if (value == h_entry[e].second.second)
        return true;

  return false;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashOpenMap<KEY,T,thash>::str() const {
  std::ostringstream answer;
  answer << "HashOpenMap[";
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
        answer << bin[b] << "; with value " << h_entry[bin[b]].first << " : "
               << h_entry[bin[b]].second.first << " -> " <<h_entry[bin[b]].second.second << std::endl;
    }
  }
  if (bins != 0 || entries != 0)
      answer << "--------------------" << std::endl;
  if (entries != 0) {
    for (int e=0; e<entries; ++e){
      answer << "  h_entry[" << e << "] = ";
      if (e < used)
        answer << h_entry[e].first << " : "
               << h_entry[e].second.first << " -> " << h_entry[e].second.second << std::endl;
      else
        answer << "unused" << std::endl;
      }
  }
  answer  << "](load_threshold=" << load_threshold << ",bins=" << bins << ",entries=" << entries << ",used=" <<used <<",mod_count=" << mod_count << ")";
  return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class KEY,class T, int (*thash)(const KEY& a)>
T HashOpenMap<KEY,T,thash>::put(const KEY& key, const T& value) {
  T to_return;
  int c = find_key(key);
  if (c != -1) {
    to_return = h_entry[bin[c]].second.second;
    h_entry[bin[c]].second.second = value;
  }else{
    to_return = value;
    ensure_lengths(used+1); //Don't pre-increment used; used is needed in ensure_lengths
    int key_hash, bin_index;
    hash_compress(key,key_hash,bin_index);
    bin_index = next_unoccupied(bin_index);
    bin[bin_index] = used;
    h_entry[used] = HashedEntry(key_hash,Entry(key,value));
    ++used;
  }

  ++mod_count;
  return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
T HashOpenMap<KEY,T,thash>::erase(const KEY& key) {
  int c = find_key(key);
  if (c == -1) {
    std::ostringstream answer;
    answer << "HashOpenMap::erase: key(" << key << ") not in Map";
    throw KeyError(answer.str());
  }
  T to_return = h_entry[bin[c]].second.second;

  //Keep h_entry contiguous
  int bin_end     = find_key(h_entry[used-1].second.first); //will rehash, even though h_entry[used-1].first is the hash value
  bin[bin_end]    = bin[c];
  h_entry[bin[c]] = h_entry[used-1];
  bin[c]          = was_occupied;

  --used;
  ++mod_count;
  return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashOpenMap<KEY,T,thash>::clear() {
  for (int b=0; b<bins; ++b)
    bin[b] = bin_empty;

  used = 0;
  ++mod_count;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template<class Iterable>
int HashOpenMap<KEY,T,thash>::put_all(const Iterable& i) {
  int count = 0;
  for (const Entry& m_entry : i) {
    ++count;
    put(m_entry.first, m_entry.second);
  }

  return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class KEY,class T, int (*thash)(const KEY& a)>
T& HashOpenMap<KEY,T,thash>::operator [] (const KEY& key) {
  int c = find_key(key);
  if (c != -1)
    return h_entry[bin[c]].second.second;

  ensure_lengths(used+1); //Don't pre-increment used; used is needed in ensure_lengths
  int key_hash, bin_index;
  hash_compress(key,key_hash,bin_index);
  bin_index      = next_unoccupied(bin_index);
  bin[bin_index] = used;
  h_entry[used]  = HashedEntry(key_hash,Entry(key,T()));

  ++used;
  ++mod_count;
  return h_entry[used-1].second.second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
const T& HashOpenMap<KEY,T,thash>::operator [] (const KEY& key) const {
  int c = find_key(key);
  if (c != -1)
    return h_entry[bin[c]].second.second;

  std::ostringstream answer;
  answer << "HashOpenMap::operator []: key(" << key << ") not in Map";
  throw KeyError(answer.str());
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>& HashOpenMap<KEY,T,thash>::operator = (const HashOpenMap<KEY,T,thash>& rhs) {
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
  if (entries != rhs.entries) {
    delete [] h_entry;
    entries = rhs.entries;
    h_entry = new HashedEntry[entries];
  }

  for (int b=0; b<rhs.bins; ++b)
    bin[b] = rhs.bin[b];
  for (int e=0; e<used; ++e)
    h_entry[e] = rhs.h_entry[e];

// Old specification; not using both rhs.hash and rhs.load_threshold
//  if (hash == rhs.hash) {
//    //Use a copy of the rhs data
//    bins    = rhs.bins;
//    entries = rhs.entries;
//    used    = rhs.used;
//    bin     = new int[bins];
//    h_entry = new HashedEntry[entries];
//
//    for (int b=0; b<rhs.bins; ++b)
//      bin[b] = rhs.bin[b];
//    for (int e=0; e<used; ++e)
//      h_entry[e] = rhs.h_entry[e];
//
//  }else{
//    //Copy key/value pairs into with rehashing
//    bins      = std::max(1,int(rhs.size()/load_threshold));
//    entries = rhs.used;
//    used    = 0;
//    bin   = new int[bins];
//    h_entry = new HashedEntry[entries];
//    for (int b=0; b<bins; ++b)
//      bin[b] = bin_empty;
//
//    for (int e=0; e<rhs.used; ++e)
//      put(rhs.h_entry[e].second.first,rhs.h_entry[e].second.second);
//  }
  ++mod_count;
  return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::operator == (const HashOpenMap<KEY,T,thash>& rhs) const {
  if (this == &rhs)
    return true;
  if (used != rhs.size())
    return false;

  for (int e=0; e<used; ++e) {
    //Uses ! and ==, so != on T need not be defined
    int rhs_c = rhs.find_key(h_entry[e].second.first);
    if  (rhs_c == -1 || !(h_entry[e].second.second == rhs.h_entry[rhs.bin[rhs_c]].second.second))
      return false;
    }

  return true;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::operator != (const HashOpenMap<KEY,T,thash>& rhs) const {
  return !(*this == rhs);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::ostream& operator << (std::ostream& outs, const HashOpenMap<KEY,T,thash>& m) {
  outs << "map[";

  int printed = 0;
  for (int e=0; e<m.used; ++e)
      outs << (printed++ == 0? "" : ",") << m.h_entry[e].second.first << "->" << m.h_entry[e].second.second;

  outs << "]";
  return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashOpenMap<KEY,T,thash>::begin () const -> HashOpenMap<KEY,T,thash>::Iterator {
  return Iterator(const_cast<HashOpenMap<KEY,T,thash>*>(this),0);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashOpenMap<KEY,T,thash>::end () const -> HashOpenMap<KEY,T,thash>::Iterator {
  return Iterator(const_cast<HashOpenMap<KEY,T,thash>*>(this),used);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class KEY,class T, int (*thash)(const KEY& a)>
void HashOpenMap<KEY,T,thash>::hash_compress (const KEY& key, int& hashed, int& bin_index) const {
    hashed    = hash(key);
    bin_index = abs(hashed) % bins;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashOpenMap<KEY,T,thash>::find_key (const KEY& key) const {
  int key_hash, bin_index;
  hash_compress(key,key_hash,bin_index);
  int bin_index_original = bin_index;
  do {
    if (bin[bin_index] == bin_empty)
      return -1;
    if (bin[bin_index] > bin_empty  &&                 //indexes occupied bin_index, and
        h_entry[bin[bin_index]].first == key_hash &&   //..with same hash code, and -fast int check to avoid == key
        h_entry[bin[bin_index]].second.first == key)   //..wisth same key
        return bin_index;
      bin_index = ++bin_index % bins; //linear probing;
  } while (bin_index != bin_index_original);

  return -1;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashOpenMap<KEY,T,thash>::next_unoccupied (int bin_index) const {
  for (/*bin*/; bin[bin_index] >= 0; bin_index = ++bin_index % bins) //linear probing
    ;

  //load_threshold <= 1 guarantees something is empty/was_occupied
  return bin_index;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashOpenMap<KEY,T,thash>::ensure_lengths(int new_used) {
  // Check new_used load_threshold and entries

  if (double(new_used)/double(bins) > load_threshold) {
    delete [] bin;

    int old_bins  = bins;                //bins always is >= 1; doubling always increases
    bins  = 2*old_bins;
    bin   = new int[bins];

    for (int b=0; b<bins; ++b)
      bin[b] = bin_empty;

    for (int e=0; e<used; ++e) {
        int bin_index = next_unoccupied(abs(h_entry[e].first) % bins);
        bin[bin_index] = e;
    }
  }

  if (new_used > entries) {
      int old_entries = entries;             //entries always >= 0
      HashedEntry* old_h_entry = h_entry;
      entries = std::max(1,2*old_entries);   //handle increase in case of entries = 0
      h_entry = new HashedEntry[entries];
      for (int e=0; e <used; ++e)
        h_entry[e] = old_h_entry[e];
      delete [] old_h_entry;
  }

}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::Iterator::Iterator(HashOpenMap<KEY,T,thash>* iterate_over, int initial)
: ref_map(iterate_over), expected_mod_count(ref_map->mod_count) {
  current = initial;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::Iterator::~Iterator()
{}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashOpenMap<KEY,T,thash>::Iterator::erase() -> Entry {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HashOpenMap::Iterator::erase Iterator cursor already erased");
  if (current < 0 || current >= ref_map->used)
    throw CannotEraseError("HashOpenMap::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  Entry to_return = ref_map->h_entry[current].second;

  int ierase = ref_map->find_key(ref_map->h_entry[current].second.first);
  int bin_end   = ref_map->find_key(ref_map->h_entry[ref_map->used-1].second.first);

  ref_map->bin[bin_end]                     = ref_map->bin[ierase];
  ref_map->h_entry[ref_map->bin[ierase]] = ref_map->h_entry[ref_map->used-1];
  ref_map->bin[ierase]                   = ref_map->was_occupied;

  --ref_map->used;
  ++ref_map->mod_count;
  expected_mod_count = ref_map->mod_count;

  return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashOpenMap<KEY,T,thash>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_map->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashOpenMap<KEY,T,thash>::Iterator::operator ++ () -> HashOpenMap<KEY,T,thash>::Iterator& {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::operator ++");

  if (current >= ref_map->used)
    return *this;

  if (can_erase)
      ++current;

  can_erase = true;

  return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashOpenMap<KEY,T,thash>::Iterator::operator ++ (int) -> HashOpenMap<KEY,T,thash>::Iterator {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::operator ++(int)");

  Iterator to_return(*this);

  if (current >= ref_map->used)
    return to_return;

  if (can_erase)
      ++current;

  can_erase = true;

  return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::Iterator::operator == (const HashOpenMap<KEY,T,thash>::Iterator& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashOpenMap::Iterator::operator ==");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::operator ==");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("HashOpenMap::Iterator::operator ==");

  return this->current == rhsASI->current;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::Iterator::operator != (const HashOpenMap<KEY,T,thash>::Iterator& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashOpenMap::Iterator::operator !=");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::operator !=");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("HashOpenMap::Iterator::operator !=");

  return this->current != rhsASI->current;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>& HashOpenMap<KEY,T,thash>::Iterator::operator *() const {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::operator *");
  if (!can_erase || current < 0 || current >= ref_map->used)
    throw IteratorPositionIllegal("HashOpenMap::Iterator::operator * Iterator illegal");

  return ref_map->h_entry[current].second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>* HashOpenMap<KEY,T,thash>::Iterator::operator ->() const {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashOpenMap::Iterator::operator *");
  if (!can_erase || current < 0 || current >= ref_map->used)
    throw IteratorPositionIllegal("HashOpenMap::Iterator::operator -> Iterator illegal");

  return &(ref_map->h_entry[current].second);
}


}

#endif /* HASH_OPEN_MAP_HPP_ */
