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
    int undefinedhash(const T &a) { return 0; }

#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
    template<class T, int (*thash)(const T &a) = undefinedhash<T>>
    class HashOpenSet {
    public:
        typedef ics::pair<int, T> HashedElement;   //Stores hash(element) and element see the h_element member
        typedef int (*hashfunc)(const T &a);

        //Destructor/Constructors
        ~HashOpenSet();

        HashOpenSet(double the_load_threshold = 1.0, int (*chash)(const T &a) = undefinedhash<T>);

        explicit HashOpenSet(int initial_bins, double the_load_threshold = 1.0,
                             int (*chash)(const T &k) = undefinedhash<T>);

        HashOpenSet(const HashOpenSet<T, thash> &to_copy, double the_load_threshold = 1.0,
                    int (*chash)(const T &a) = undefinedhash<T>);

        explicit HashOpenSet(const std::initializer_list<T> &il, double the_load_threshold = 1.0,
                             int (*chash)(const T &a) = undefinedhash<T>);

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template<class Iterable>
        explicit
        HashOpenSet(const Iterable &i, double the_load_threshold = 1.0, int (*chash)(const T &a) = undefinedhash<T>);


        //Queries
        bool empty() const;

        int size() const;

        bool contains(const T &element) const;

        std::string str() const; //supplies useful debugging information; contrast to operator <<

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template<class Iterable>
        bool contains_all(const Iterable &i) const;


        //Commands
        int insert(const T &element);

        int erase(const T &element);

        void clear();

        //Iterable class must support "for" loop: .begin()/.end() and prefix ++ on returned result

        template<class Iterable>
        int insert_all(const Iterable &i);

        template<class Iterable>
        int erase_all(const Iterable &i);

        template<class Iterable>
        int retain_all(const Iterable &i);


        //Operators
        HashOpenSet<T, thash> &operator=(const HashOpenSet<T, thash> &rhs);

        bool operator==(const HashOpenSet<T, thash> &rhs) const;

        bool operator!=(const HashOpenSet<T, thash> &rhs) const;

        bool operator<=(const HashOpenSet<T, thash> &rhs) const;

        bool operator<(const HashOpenSet<T, thash> &rhs) const;

        bool operator>=(const HashOpenSet<T, thash> &rhs) const;

        bool operator>(const HashOpenSet<T, thash> &rhs) const;

        template<class T2, int (*hash2)(const T2 &a)>
        friend std::ostream &operator<<(std::ostream &outs, const HashOpenSet<T2, hash2> &s);


    public:
        class Iterator {
        public:
            //Private constructor called in begin/end, which are friends of HashOpenSet<T,thash>
            ~Iterator();

            T erase();

            std::string str() const;

            HashOpenSet<T, thash>::Iterator &operator++();

            HashOpenSet<T, thash>::Iterator operator++(int);

            bool operator==(const HashOpenSet<T, thash>::Iterator &rhs) const;

            bool operator!=(const HashOpenSet<T, thash>::Iterator &rhs) const;

            T &operator*() const;

            T *operator->() const;

            friend std::ostream &operator<<(std::ostream &outs, const HashOpenSet<T, thash>::Iterator &i) {
              outs << i.str(); //Use the same meaning as the debugging .str() method
              return outs;
            }

            friend Iterator HashOpenSet<T, thash>::begin() const;

            friend Iterator HashOpenSet<T, thash>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            int current; //h_element Index
            HashOpenSet<T, thash> *ref_set;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator(HashOpenSet<T, thash> *iterate_over, int initial);
        };


        Iterator begin() const;

        Iterator end() const;


    private:
        int (*hash)(const T &k);          //Hashing function used (from template or constructor)

        const static int bin_empty = -1;   //Special values that can be stored in the bin array,
        const static int was_occupied = -2; //  along with non-negative indexes into the h_element array
        int *bin = nullptr; //Non-contiguous int array: bin[i] is either -1, -2, or an index into the h_element array
        HashedElement *h_element = nullptr; //Contiguous HashedElement array storing hash(element) and element as pair
        //bin/h_element are allocated in constructors and in ensure_lengths
        double load_threshold;              //used/bins must always be <= load_threshold (see ensure_lengths)
        int bins = 1;           //Allocated length of bin array (>= 1, so hash_compress doesn't % 0)
        int elements = 1;           //Allocated length of h_element array (can be < bins, if load_factor < 1)
        int used = 0;           //# of elements in this set; used <= elements (see ensure_lengths)
        //  bins-used = # of bin_empty/was_occupied values in bin array
        int mod_count = 0;           //For sensing concurrent modification


        //Helper methods
        //Given element, compute/return its hashed value and which bin it compresses to (%bins in range [0,bins-1])
        void hash_compress(const T &element, int &hashed, int &bin_index) const;

        int find_element(const T &element) const; //Returns index to element's bin or -1 if  element is not in any bin
        int next_unoccupied(int bin_index) const;   //this bin_index (if unoccupied) or next bin_index beyond that
        //   is unoccupied; advances using linear probing
        void ensure_lengths(
                int new_used);            //Reallocate bin/h_element if load factor > load_threshold/more elements needed
    };





//HashOpenSet class and related definitions

////////////////////////////////////////////////////////////////////////////////
//
//Destructor/Constructors

    template<class T, int (*thash)(const T &a)>
    HashOpenSet<T, thash>::~HashOpenSet() {
    }


    template<class T, int (*thash)(const T &a)>
    HashOpenSet<T, thash>::HashOpenSet(double the_load_threshold, int (*chash)(const T &element))
    :hash (thash != (hashfunc)undefinedhash<T> ? thash : chash)
    {
        if(the_load_threshold > 1.){
            throw IcsError("");
        }
        if(hash == (hashfunc)undefinedhash<T>){
            throw TemplateFunctionError("HashOpenSet::default constructor: neither specified");
        }
        if(thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash){
            throw TemplateFunctionError("HashOpenSet::default constructor: both specified and different");
        }
        load_threshold = the_load_threshold;
        bin = new int[bins];
        h_element = new HashedElement[elements];
        for(int i = 0; i < elements; ++i){
            bin[i] = -1;
        }
    }


    template<class T, int (*thash)(const T &a)>
    HashOpenSet<T, thash>::HashOpenSet(int initial_bins, double the_load_threshold, int (*chash)(const T &element))
    :hash (thash != (hashfunc)undefinedhash<T> ? thash : chash)
    {
        if(the_load_threshold > 1.){
            throw IcsError("");
        }
        if(hash == (hashfunc)undefinedhash<T>)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
        }
        if(thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
        }
        load_threshold = the_load_threshold;
        if(initial_bins < 0)
        {
            bins = 0;
        }
        else
        {
            bins = initial_bins;
        }
        bin = new int[bins];
        h_element = new HashedElement[elements];
        for(int aa = 0; aa < elements; ++aa)
        {
            bin[aa] = -1;
        }
    }

    template<class T, int (*thash)(const T &a)>
    HashOpenSet<T, thash>::HashOpenSet(const HashOpenSet<T, thash> &to_copy, double the_load_threshold,int (*chash)(const T &element))
    :hash (thash != (hashfunc)undefinedhash<T> ? thash : chash)
    {
        if(the_load_threshold > 1.){
            throw IcsError("");
        }
        if(hash == (hashfunc)undefinedhash<T>)
        {
            hash = to_copy.hash;
//            throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
        }
        if(thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
        }


        load_threshold = the_load_threshold;



        if(hash == to_copy.hash){
            bins = to_copy.bins;
            elements = to_copy.elements;
            used = to_copy.used;
            bin = new int[bins];
            for(int i = 0; i < bins; ++i){
                bin[i] = -1;
            }
            h_element = new HashedElement[elements];
            for (int i = 0; i < bins; i ++) bin[i] = to_copy.bin[i];
            for (int i = 0; i < used; i ++) h_element[i] = to_copy.h_element[i];
        }
        else{
            bin = new int[bins];
            for(int i = 0; i < bins; ++i){
                bin[i] = -1;
            }
            h_element = new HashedElement[elements];
            for(int i = 0; i < to_copy.used; ++i){
                insert(to_copy.h_element[i].second);
            }
//            insert_all(to_copy);
        }
    }


    template<class T, int (*thash)(const T &a)>
    HashOpenSet<T, thash>::HashOpenSet(const std::initializer_list<T> &il, double the_load_threshold,int (*chash)(const T &element))
    :hash (thash != (hashfunc)undefinedhash<T> ? thash : chash)
    {
        if(the_load_threshold > 1.){
            throw IcsError("");
        }
        if(hash == (hashfunc)undefinedhash<T>)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
        }
        if(thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
        }
        load_threshold = the_load_threshold;
        bin = new int[bins];
        for(int i = 0; i < bins; ++i){
            bin[i] = -1;
        }
        h_element = new HashedElement[elements];
        for(const auto aa:il)
        {
            insert(aa);
        }
    }


    template<class T, int (*thash)(const T &a)>
    template<class Iterable>
    HashOpenSet<T, thash>::HashOpenSet(const Iterable &i, double the_load_threshold, int (*chash)(const T &a))
    :hash (thash != (hashfunc)undefinedhash<T> ? thash : chash)
    {
        if(the_load_threshold > 1.){
            throw IcsError("");
        }
        if(hash == (hashfunc)undefinedhash<T>)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: neither specified");
        }
        if(thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
        {
            throw TemplateFunctionError("HashOpenTable::default constructor: both specified and different");
        }
        load_threshold = the_load_threshold;
        bin = new int[bins];
        for(int i = 0; i < bins; ++i){
            bin[i] = -1;
        }
        h_element = new HashedElement[elements];
        for(const auto aa:i)
        {
            insert(aa);
        }
    }

////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::empty() const {return used == 0;
    }


    template<class T, int (*thash)(const T &a)>
    int HashOpenSet<T, thash>::size() const {return used;
    }


    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::contains(const T &element) const {
        int hashed, bin_idx;
        hash_compress(element, hashed, bin_idx);
        for(int i = 0; i < bins; ++i){
            int new_bin_idx = (bin_idx + i) % bins;
            if (bin[new_bin_idx] == bin_empty){
                return false;
            }
            if (bin[new_bin_idx] == was_occupied){
                continue;
            }
            if (h_element[bin[new_bin_idx]].first == hashed) {
                return true;
            }
        }
        return false;
    }


    template<class T, int (*thash)(const T &a)>
    std::string HashOpenSet<T, thash>::str() const {
        std::ostringstream answer;
        answer << "set s = HashOpenSet[\n";
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
                answer << "\tbin[" << aa << "] = " << bin[aa] << ";" << "\n";
            }
        }
        answer << "--------------------\n";
        for (int aa = 0; aa < elements; aa++)
        {
            if(aa < used)
            {
                answer << "\th_elements[" << aa << "] = " << h_element[aa].first << " : " << h_element[aa].second  <<  "\n";
            }
            else
            {
                answer << "\th_elements[" << aa << "] = unused\n";
            }
        }
        answer << "](load_threshold=" << load_threshold << ",bins=" << bins << ",elements=" << elements;
        answer << ",used=" << used << ",mod_count=" << mod_count << "\n";
        return answer.str();
    }


    template<class T, int (*thash)(const T &a)>
    template<class Iterable>
    bool HashOpenSet<T, thash>::contains_all(const Iterable &i) const {
        for(const T& v:i){
            if(!contains(v)){
                return false;
            }
        }
        return true;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template<class T, int (*thash)(const T &a)>
    int HashOpenSet<T, thash>::insert(const T &element) {
        ensure_lengths(used+1);
        int bin_idx = find_element(element);
        if (bin_idx >= 0) {
            return 0;
        } else {
            int hashcode, bin_idx;
            hash_compress(element, hashcode, bin_idx);
            bin_idx = next_unoccupied(bin_idx);
            h_element[used] = HashedElement(hashcode, element);
            bin[bin_idx] = used;
            used++;
            ++mod_count;
            return 1;
        }
    }


    template<class T, int (*thash)(const T &a)>
    int HashOpenSet<T, thash>::erase(const T &element) {
        int bin_idx = find_element(element);
        if (bin_idx < 0) return 0;
        else{
            int h_elem_idx = bin[bin_idx];
            if(h_elem_idx != used-1){
                bin[bin_idx] = was_occupied;
                h_element[h_elem_idx] = h_element[used - 1];
                int bin_idx2 = find_element(h_element[h_elem_idx].second);
                bin[bin_idx2] = h_elem_idx;
                used --;
                ++mod_count;
                return 1;
            }
            else{
                bin[bin_idx] = was_occupied;
                h_element[h_elem_idx] = h_element[used - 1];
                used --;
                ++mod_count;
                return 1;
            }
        }

    }


    template<class T, int (*thash)(const T &a)>
    void HashOpenSet<T, thash>::clear() {
        used  = 0;
        ++mod_count;
        for (int  i = 0; i < bins; i ++) bin[i] = bin_empty;
    }


    template<class T, int (*thash)(const T &a)>
    template<class Iterable>
    int HashOpenSet<T, thash>::insert_all(const Iterable &i) {
        int count = 0;
        for (const T& v : i) {
            count += insert(v);
        }

        return count;
    }


    template<class T, int (*thash)(const T &a)>
    template<class Iterable>
    int HashOpenSet<T, thash>::erase_all(const Iterable &i) {
        int count = 0;
        for (const T& v : i)
            count += erase(v);

        return count;
    }


    template<class T, int (*thash)(const T &a)>
    template<class Iterable>
    int HashOpenSet<T, thash>::retain_all(const Iterable &i) {
        HashOpenSet s(i);
        int count = 0;
        for (int i=0; i<used; i++)
            if (!s.contains(h_element[i].second)) {
                erase(h_element[i].second);
                --i;
                ++count;
            }

        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template<class T, int (*thash)(const T &a)>
    HashOpenSet<T, thash> &HashOpenSet<T, thash>::operator=(const HashOpenSet<T, thash> &rhs) {
        if(this == &rhs){
            return *this;
        }
        ensure_lengths(rhs.used);
        hash = rhs.hash;
        bins = rhs.bins;
        elements = rhs.elements;
        used = rhs.used;
        load_threshold = rhs.load_threshold;

        bin = new int[bins];
        h_element = new HashedElement[elements];

        for (int i = 0; i < bins; i ++) bin[i] = rhs.bin[i];
        for (int i = 0; i < used; i ++) h_element[i] = rhs.h_element[i];

        ++mod_count;
        return *this;
    }


    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::operator==(const HashOpenSet<T, thash> &rhs) const {
        if (used != rhs.size()) return false;
        for (int i = 0; i < used; i ++) {
            const auto& elem = h_element[i].second;
            if (!rhs.contains(elem)) return false;
        }
        return true;
    }


    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::operator!=(const HashOpenSet<T, thash> &rhs) const {
      return !(*this == rhs);
    }


    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::operator<=(const HashOpenSet<T, thash> &rhs) const {
        if (used > rhs.size()) return false;
        for (int i = 0; i < used; i ++) {
            const auto& elem = h_element[i].second;
            if (!rhs.contains(elem)) return false;
        }
        return true;
    }

    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::operator<(const HashOpenSet<T, thash> &rhs) const {
        if (used >= rhs.size()) return false;
        for (int i = 0; i < used; i ++) {
            const auto& elem = h_element[i].second;
            if (!rhs.contains(elem)) return false;
        }
        return true;
    }


    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::operator>=(const HashOpenSet<T, thash> &rhs) const {
        return rhs <= *this;
    }


    template<class T, int (*thash)(const T &a)>
    bool HashOpenSet<T, thash>::operator>(const HashOpenSet<T, thash> &rhs) const {
        return rhs < *this;
    }


    template<class T, int (*thash)(const T &a)>
    std::ostream &operator<<(std::ostream &outs, const HashOpenSet<T, thash> &s) {
        outs << "set[";
        if(!s.empty())
        {
            outs << s.h_element[0].second;
            for(int aa = 1; aa < s.used; aa++)
            {
                outs << "," << s.h_element[aa].second;
            }
        }
        outs << "]";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template<class T, int (*thash)(const T &a)>
    auto HashOpenSet<T, thash>::begin() const -> HashOpenSet<T, thash>::Iterator {return Iterator(const_cast<HashOpenSet<T,thash>*>(this),0);
    }


    template<class T, int (*thash)(const T &a)>
    auto HashOpenSet<T, thash>::end() const -> HashOpenSet<T, thash>::Iterator {return Iterator(const_cast<HashOpenSet<T,thash>*>(this),used);
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, int (*thash)(const T &a)>
void HashOpenSet<T, thash>::hash_compress(const T &element, int &hashed, int &bin_index) const {
    hashed = abs((*hash)(element));
    bin_index = hashed % bins;
}


template<class T, int (*thash)(const T &a)>
int HashOpenSet<T, thash>::find_element(const T &element) const {
    int hashcode, bin_index;
    hash_compress(element, hashcode, bin_index);
    for(int i = 0; i < bins; ++i){
        int new_bin_idx = (bin_index + i) % bins;
        if (bin[new_bin_idx] == bin_empty) return -1;
        if (bin[new_bin_idx] == was_occupied) continue;
        else{
            if (h_element[bin[new_bin_idx]].first == hashcode) {
                return new_bin_idx;
            }
            continue;
        }
    }
    return -1;
}

template<class T, int (*thash)(const T &a)>
int HashOpenSet<T, thash>::next_unoccupied(int bin_index) const {
    for(int i = 0; i < bins; ++i) {
        int new_bin_idx = (bin_index + i) % bins;
        if (bin[new_bin_idx] < 0) return new_bin_idx;
    }
    return -1;
}


template<class T, int (*thash)(const T &a)>
void HashOpenSet<T, thash>::ensure_lengths(int new_used) {
    if (new_used * 1.0 / bins >= load_threshold) {
        int ori_bin_count = bins;
        int* ori_bin = bin;

        bins = int(new_used / load_threshold + 1) ;  //  8  0.8
        bin = new int[bins];

        for (int i = 0; i < bins; i ++) bin[i] = bin_empty;
        for (int i = 0; i < ori_bin_count; i ++) {
            if (ori_bin[i] >= 0) {
                int loc = next_unoccupied(h_element[ori_bin[i]].first % bins);
                if (loc == -1) throw;
                bin[loc] = ori_bin[i];  // crucial!!!
            }
        }
        delete[] ori_bin;
    }
    if (new_used > elements) {
        HashedElement* ori_h_elem = h_element;
        int ori_elem = elements;

        elements = new_used + 1;
        h_element = new HashedElement[elements];
        for (int i = 0; i < used; i ++) {
            h_element[i] = ori_h_elem[i];
        }
        delete[] ori_h_elem;
    }
    else{
        return;
    }
}




////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, int (*thash)(const T& a)>
HashOpenSet<T,thash>::Iterator::Iterator(HashOpenSet<T,thash>* iterate_over, int initial) {
    current = initial;
    ref_set = iterate_over;
    expected_mod_count = ref_set->mod_count;
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
    if (current < 0 || current >= ref_set->used)
        throw CannotEraseError("HashOpenSet::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;
    T to_return = ref_set->h_element[current].second;
    ref_set->erase(to_return);
    expected_mod_count = ref_set->mod_count;
    return to_return;
//    return ref_set->h_element[0].second;
}


template<class T, int (*thash)(const T& a)>
std::string HashOpenSet<T,thash>::Iterator::str() const {return "";
}


template<class T, int (*thash)(const T& a)>
auto  HashOpenSet<T,thash>::Iterator::operator ++ () -> HashOpenSet<T,thash>::Iterator& {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashOpenSet::Iterator::operator ++");

    if (current >= ref_set->used)
        return *this;

    if (can_erase)
        ++current;
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return *this;
}


template<class T, int (*thash)(const T& a)>
auto  HashOpenSet<T,thash>::Iterator::operator ++ (int) -> HashOpenSet<T,thash>::Iterator {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashOpenSet::Iterator::operator ++(int)");

    if (current >= ref_set->used)
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        ++current;
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

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

    return current == rhsASI->current;
}


template<class T, int (*thash)(const T& a)>
bool HashOpenSet<T,thash>::Iterator::operator != (const HashOpenSet<T,thash>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("HashOpenSet::Iterator::operator ==");
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashOpenSet::Iterator::operator ==");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("HashOpenSet::Iterator::operator ==");

    return current != rhsASI->current;
}

template<class T, int (*thash)(const T& a)>
T& HashOpenSet<T,thash>::Iterator::operator *() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArraySet::Iterator::operator *");
    if (!can_erase || current < 0 || current >= ref_set->used) {
        std::ostringstream where;
        where << current << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("ArraySet::Iterator::operator * Iterator illegal: "+where.str());
    }

//    return ref_set->set[current];
    return ref_set->h_element[current].second;
}

template<class T, int (*thash)(const T& a)>
T* HashOpenSet<T,thash>::Iterator::operator ->() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArraySet::Iterator::operator *");
    if (!can_erase || current < 0 || current >= ref_set->used) {
        std::ostringstream where;
        where << current << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("ArraySet::Iterator::operator * Iterator illegal: "+where.str());
    }

//    return ref_set->set[current];
    return ref_set->h_element[current].second;
}

}

#endif /* HASH_OPEN_SET_HPP_ */
