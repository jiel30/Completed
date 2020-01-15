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
template<class KEY,class T, int (*thash)(const KEY& a) = undefinedhash<KEY>> class HashOpenMap {
  public:
//    int  find_key        (const KEY& key)  const;  //Returns index to key's bin or -1 if  key is not in any bin
    void print_bin();
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
        int                       current; //h_entry Index
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
//    delete bin;
//    delete h_entry;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(double the_load_threshold, int (*chash)(const KEY& k))
:hash (thash != (hashfunc)undefinedhash<KEY> ? thash : chash)
{
    if(the_load_threshold > 1 || the_load_threshold <=0)
    {
        throw TemplateFunctionError("HashOpenTable::the_load_threshold should be bigger than 1");
    }
    if(hash == (hashfunc)undefinedhash<KEY>)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
    }
    if(thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
    }
    load_threshold = the_load_threshold;
    bin = new int[bins];
    h_entry = new HashedEntry[entries];
    for(int aa = 0; aa < entries; ++aa)
    {
        bin[aa] = bin_empty;
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(int initial_bins, double the_load_threshold, int (*chash)(const KEY& k))
:hash (thash != (hashfunc)undefinedhash<KEY> ? thash : chash)
{
    if(the_load_threshold > 1 || the_load_threshold <=0)
    {
        throw TemplateFunctionError("HashOpenTable::the_load_threshold should be bigger than 1");
    }
    if(hash == (hashfunc)undefinedhash<KEY>)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
    }
    if(thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
    }
    load_threshold = the_load_threshold;
    if(initial_bins < 0)
    {
        bins = 1;
    }
    else
    {
        bins = initial_bins;
    }
    bin = new int[bins];
    h_entry = new HashedEntry[entries];
    for(int aa = 0; aa < bins; ++aa)
    {
        bin[aa] = -1;
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(const HashOpenMap<KEY,T,thash>& to_copy, double the_load_threshold, int (*chash)(const KEY& a))
:hash (thash != (hashfunc)undefinedhash<KEY> ? thash : chash)
{
    if(the_load_threshold > 1 || the_load_threshold <=0)
    {
        throw TemplateFunctionError("HashOpenTable::the_load_threshold should be bigger than 1");
    }
    if(hash == (hashfunc)undefinedhash<KEY>)
    {
        hash = to_copy.hash;
    }
    if(thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
    }
    if(hash == to_copy.hash)
    {
        bins = to_copy.bins;
        entries = to_copy.entries;
        h_entry = new HashedEntry[entries];
        bin = new int[bins];
        used = to_copy.used;
        for(int aa = 0; aa < bins; aa++)
        {
            bin[aa] = -1;
        }
        for (int aa = 0; aa < bins; ++aa)
        {
            bin[aa] = to_copy.bin[aa];
        }
        for (int aa = 0; aa < used; ++aa)
        {
            h_entry[aa] = to_copy.h_entry[aa];
        }
        mod_count = 0;
    }
    else
    {
        h_entry = new HashedEntry[entries];
        bin = new int[bins];
        for (int aa = 0; aa < bins; aa++)
        {
            bin[aa] = -1;
        }
        for (int aa = 0; aa < to_copy.used; aa++)
        {
            put(to_copy.h_entry[aa].second.first,to_copy.h_entry[aa].second.second);
        }
        mod_count = 0;
    }
}

template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::HashOpenMap(const std::initializer_list<Entry>& il, double the_load_threshold, int (*chash)(const KEY& k))
:hash (thash != (hashfunc)undefinedhash<KEY> ? thash : chash)
{
    if(the_load_threshold > 1 || the_load_threshold <=0)
    {
        throw TemplateFunctionError("HashOpenTable::the_load_threshold should be bigger than 1");
    }
    if(hash == (hashfunc)undefinedhash<KEY>)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
    }
    if(thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
    }
    load_threshold = the_load_threshold;
    bin = new int[bins];
    h_entry = new HashedEntry[entries];
    for(int aa = 0; aa < bins; aa++)
    {
        bin[aa] = -1;
    }
    for(const auto aa:il)
    {
        put(aa.first,aa.second);
        mod_count --;
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template <class Iterable>
HashOpenMap<KEY,T,thash>::HashOpenMap(const Iterable& i, double the_load_threshold, int (*chash)(const KEY& k))
:hash (thash != (hashfunc)undefinedhash<KEY> ? thash : chash)
{
    if(the_load_threshold > 1 || the_load_threshold <=0)
    {
        throw TemplateFunctionError("HashOpenTable::the_load_threshold should be bigger than 1");
    }
    if(hash == (hashfunc)undefinedhash<KEY>)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
    }
    if(thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
    {
        throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
    }
    load_threshold = the_load_threshold;
    bin = new int[bins];
    h_entry = new HashedEntry[entries];
    for(int aa = 0; aa < bins; aa++)
    {
        bin[aa] = -1;
    }
    for(const auto aa:i)
    {
        put(aa.first,aa.second);
        mod_count --;
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::empty() const {
    return used == 0;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashOpenMap<KEY,T,thash>::size() const {return used;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::has_key (const KEY& key) const {
    int check = find_key(key);
    return check != -1;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::has_value (const T& value) const {
    T va;
    for(int aa = 0; aa < used; aa++)
    {
        va = h_entry[aa].second.second;
        if(va == value)
        {
            return true;
        }
    }
    return false;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashOpenMap<KEY,T,thash>::str() const {
    std::ostringstream answer;
    answer << "map m = HashOpenMap[\n";
    for(int aa = 0; aa < bins; aa++)
    {
        if(bin[aa] == -1)
        {
            answer << "\tbin[" << aa << "] = empty\n";
        }
        if(bin[aa] == -2)
        {
            answer << "\tbin[" << aa << "] = was occupied\n";
        }
        if(bin[aa] != -1 && bin[aa] != -2)
        {
            answer << "\tbin[" << aa << "] = " << bin[aa] << ";\n";
        }
    }
    answer << "--------------------\n";
    for (int aa = 0; aa < entries; aa++)
    {
        if(aa < used)
        {
            answer << "\th_entry[" << aa << "] = " << h_entry[aa].first << " : " << h_entry[aa].second.first << " -> " << h_entry[aa].second.second << "\n";
        }
        else
        {
            answer << "\th_entry[" << aa << "] = unused\n";
        }
    }
    answer << "](load_threshold=" << load_threshold << ",bins=" << bins << ",entries=" << entries;
    answer << ",used=" << used << ",mod_count=" << mod_count << "\n";
    return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class KEY,class T, int (*thash)(const KEY& a)>
T HashOpenMap<KEY,T,thash>::put(const KEY& key, const T& value) {
    mod_count ++;
    int new_used = used + 1;
    int hashed;
    int bin_index;
    ensure_lengths(new_used);
    hash_compress(key,hashed,bin_index);
    Entry e(key,value);
    HashedEntry he(hashed,e);
    int find_it = find_key(key);
    if(find_it != -1)
    {
        T before = h_entry[bin[find_it]].second.second;
        h_entry[bin[find_it]] = he;
        return before;
    }
    used ++;
    h_entry[used - 1] = he;
    int index = next_unoccupied(bin_index);
    bin[index] = used - 1;
//    std::cout << "initial bin_index: " << bin_index;
//    std::cout << " key: " << key << " index: " << used - 1 << std::endl;
    return value;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
T HashOpenMap<KEY,T,thash>::erase(const KEY& key) {
    int index = find_key(key);
    int bin_index;
    if(index == -1)
    {
        std::ostringstream answer;
        answer << "ArrayMap::erase: key(" << key << ") not in Map";
        throw KeyError(answer.str());
    }
    used --;
    mod_count ++;
    int h_index = bin[index];
    T old_value = h_entry[h_index].second.second;
    if(h_index != used)
    {
        h_entry[h_index] = h_entry[used];
        int a = find_key(h_entry[h_index].second.first);
        bin[a] = h_index;
    }
    bin[index] = -2;

//    for(int aa = h_index; aa < used; aa++)
//    {
//        bin_index = find_key(h_entry[aa].second.first);
//        bin[bin_index] = bin[bin_index] - 1;
//        h_entry[aa] = h_entry[aa + 1];
//    }
    return old_value;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashOpenMap<KEY,T,thash>::clear() {
    used = 0;
    mod_count ++;
    for (int aa = 0; aa < bins; aa++)
    {
        bin[aa] = -1;
    }
    HashedEntry* temp = new HashedEntry[entries];
//    delete h_entry;
    h_entry = temp;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template<class Iterable>
int HashOpenMap<KEY,T,thash>::put_all(const Iterable& i) {
    int count = 0;
    for (auto aa: i)
    {
        put(aa.first,aa.second);
        count ++;
    }
    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class KEY,class T, int (*thash)(const KEY& a)>
T& HashOpenMap<KEY,T,thash>::operator [] (const KEY& key) {
    int index = find_key(key);
    if(index != -1)
    {
        return h_entry[bin[index]].second.second;
    }
//    ensure_lengths(used + 1);
//    int hashed;
//    int bin_index;
//    hash_compress(key,hashed,bin_index);
//    Entry e = Entry(key,T());
//    HashedEntry he = HashedEntry(hashed,e);
//    int i = next_unoccupied(bin_index);
//    bin[i] = used;
//    used ++;
//    h_entry[used - 1] = he;
    put(key,T());
    return h_entry[used - 1].second.second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
const T& HashOpenMap<KEY,T,thash>::operator [] (const KEY& key) const {
    int index = find_key(key);
    if(index != -1)
    {
        return h_entry[bin[index]].second.second;
    }
    std::ostringstream answer;
    answer << "ArrayMap::operator []: key(" << key << ") not in Map";
    throw KeyError(answer.str());
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>& HashOpenMap<KEY,T,thash>::operator = (const HashOpenMap<KEY,T,thash>& rhs) {
    if(this == &rhs)
    {
        return *this;
    }
    hash = rhs.hash;
    load_threshold = rhs.load_threshold;
    ensure_lengths(rhs.used);
    used = rhs.used;
    bins = rhs.bins;
    entries = rhs.entries;
//        delete bin;
//        delete h_entry;
    bin = new int[bins];
    h_entry = new HashedEntry[entries];
    for(int aa = 0; aa < bins; aa++)
    {
        bin[aa] = rhs.bin[aa];
    }
    for (int aa = 0; aa < used; aa++)
    {
        h_entry[aa] = rhs.h_entry[aa];
    }
    mod_count ++;

    return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::operator == (const HashOpenMap<KEY,T,thash>& rhs) const {
    int index;
    if(this == &rhs)
    {
        return true;
    }
    if(used != rhs.used)
    {
        return false;
    }
    for(int aa = 0; aa < rhs.size(); aa ++)
    {
        index = find_key(rhs.h_entry[aa].second.first);
        if(index == -1 || rhs.h_entry[aa].second.second != h_entry[bin[index]].second.second || rhs.h_entry[aa].second.first != h_entry[bin[index]].second.first)
        {
            return false;
        }
    }
//    std::cout << "finish ==\n";
    return true;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::operator != (const HashOpenMap<KEY,T,thash>& rhs) const {return !(*this == rhs);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::ostream& operator << (std::ostream& outs, const HashOpenMap<KEY,T,thash>& m) {
    outs << "map[";
    if(!m.empty())
    {
        outs << m.h_entry[0].second.first << "->" << m.h_entry[0].second.second;
        for(int aa = 1; aa < m.used; aa++)
        {
            outs << "," << m.h_entry[aa].second.first << "->" << m.h_entry[aa].second.second;
        }
    }
    outs << "]";
    return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashOpenMap<KEY,T,thash>::begin () const -> HashOpenMap<KEY,T,thash>::Iterator {return Iterator(const_cast<HashOpenMap<KEY,T,thash>*>(this),0);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashOpenMap<KEY,T,thash>::end () const -> HashOpenMap<KEY,T,thash>::Iterator {return Iterator(const_cast<HashOpenMap<KEY,T,thash>*>(this),used);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class KEY,class T, int (*thash)(const KEY& a)>
void HashOpenMap<KEY,T,thash>::hash_compress (const KEY& key, int& hashed, int& bin_index) const {
    hashed = hash(key);
    if(hashed < 0)
    {
        hashed = hashed * -1;
    }
    bin_index = hashed % bins;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashOpenMap<KEY,T,thash>::find_key (const KEY& key) const {

    int hashed;
    int bin_index;
    hash_compress(key,hashed,bin_index);
    int check_time = 0;
    int end_of_bin = bins - 1;
//    std::cout << hashed << std:: endl;
//    std::cout << "first_bin number: " << bin_index << std::endl;
//    std::cout << "bin mouduan: " << end_of_bin << std::endl;
//    for (int aa = 0; aa < bins ; aa++)
//    {
//        std::cout << bin[aa] << "\t";
//    }
//    std::cout << std::endl;
    for(int aa = 0; aa < bins; aa++)
    {
//        std::cout << "bin_index: " << bin_index << std::endl;
        if(bin[bin_index] == -1)
        {
            return -1;
        }
//        std::cout << h_entry[bin[bin_index]].first << std::endl;
        if(h_entry[bin[bin_index]].first == hashed)
        {
//            std::cout << "enter hashed\n";
//            std::cout << "key inside is: " << h_entry[bin[bin_index]].second.first << std::endl;
            if(h_entry[bin[bin_index]].second.first == key)
            {
                return bin_index;
            }
        }
        if(bin_index == end_of_bin)
        {
            bin_index = 0;
        }
        else
        {
            bin_index ++;
        }
    }
    return -1;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashOpenMap<KEY,T,thash>::next_unoccupied (int bin_index) const {
    int end_of_bin = bins - 1;
    while(1)
    {
        if(bin[bin_index] == -1 || bin[bin_index] == -2)
        {
            return bin_index;
        }
        if(bin_index == end_of_bin)
        {
            bin_index = 0;
        }
        else
        {
            bin_index ++;
        }
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashOpenMap<KEY,T,thash>::ensure_lengths(int new_used) {
    int hashed;
    int bin_index;
    if(new_used / bins >= load_threshold)
    {
        bins = bins * 2;
        bin = new int[bins];
        for(int aa = 0; aa < bins ; aa++) {
            bin[aa] = -1;
        }
        for (int aa = 0; aa < used; aa++)
        {
            hash_compress(h_entry[aa].second.first,hashed,bin_index);
            bin_index = next_unoccupied(bin_index);
            bin[bin_index] = aa;
        }
    }
    if(entries < new_used)
    {
        entries = entries*2;
        HashedEntry* t = new HashedEntry[entries];
        for (int aa = 0; aa < used; aa++)
        {
            t[aa] = h_entry[aa];
        }
//        delete h_entry;
        h_entry = t;
    }
}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::Iterator::Iterator(HashOpenMap<KEY,T,thash>* iterate_over, int initial)
{
    current = initial;
    ref_map = iterate_over;
    expected_mod_count = ref_map->mod_count;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashOpenMap<KEY,T,thash>::Iterator::~Iterator()
{
}


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
    ref_map->erase(ref_map->h_entry[current].second.first);
    expected_mod_count = ref_map->mod_count;
    return to_return;
//    return ref_map->h_entry[0].second;
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
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashOpenMap<KEY,T,thash>::Iterator::operator ++ (int) -> HashOpenMap<KEY,T,thash>::Iterator {
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("HashOpenMap::Iterator::operator ++(int)");

    if (current >= ref_map->used)
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        ++current;
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

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

    return current == rhsASI->current;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashOpenMap<KEY,T,thash>::Iterator::operator != (const HashOpenMap<KEY,T,thash>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("HashOpenMap::Iterator::operator ==");
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("HashOpenMap::Iterator::operator ==");
    if (ref_map != rhsASI->ref_map)
        throw ComparingDifferentIteratorsError("HashOpenMap::Iterator::operator ==");

    return current != rhsASI->current;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>& HashOpenMap<KEY,T,thash>::Iterator::operator *() const {
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("HashOpenMap::Iterator::operator *");
    if (!can_erase || current < 0 || current >= ref_map->used) {
        std::ostringstream where;
        where << current << " when size = " << ref_map->size();
        throw IteratorPositionIllegal("HashOpenMap::Iterator::operator * Iterator illegal: "+where.str());
    }
    return ref_map->h_entry[current].second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>* HashOpenMap<KEY,T,thash>::Iterator::operator ->() const {
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("HashOpenMap::Iterator::operator ->");
    if (!can_erase || current < 0 || current >= ref_map->used) {
        std::ostringstream where;
        where << current << " when size = " << ref_map->size();
        throw IteratorPositionIllegal("HashOpenMap::Iterator::operator -> Iterator illegal: "+where.str());
    }
    return &(ref_map->h_entry[current].second);
}


}

#endif /* HASH_OPEN_MAP_HPP_ */
