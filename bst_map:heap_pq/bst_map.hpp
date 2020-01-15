// Submitter: jiel30(Jie, Li)
// Partner  : junfenl3(Junfeng, Li)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming
#ifndef BST_MAP_HPP_
#define BST_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"
#include "array_queue.hpp"   //For traversal


namespace ics {


#ifndef undefinedltdefined
#define undefinedltdefined
    template<class T>
    bool undefinedlt (const T& a, const T& b) {return false;}
#endif /* undefinedltdefined */

//Instantiate the templated class supplying tlt(a,b): true, iff a has higher priority than b.
//If tlt is defaulted to undefinedlt in the template, then a constructor must supply cgt.
//If both tlt and clt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedlt value supplied by tlt/clt is stored in the instance variable gt.
    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b) = undefinedlt<KEY>> class BSTMap {
    public:
        typedef pair<KEY,T> Entry;
        typedef bool (*ltfunc) (const KEY& a, const KEY& b);

        //Destructor/Constructors
        ~BSTMap();

        BSTMap          (bool (*clt)(const KEY& a, const KEY& b) = undefinedlt<KEY>);
        BSTMap          (const BSTMap<KEY,T,tlt>& to_copy, bool (*clt)(const KEY& a, const KEY& b) = undefinedlt<KEY>);
        explicit BSTMap (const std::initializer_list<Entry>& il, bool (*clt)(const KEY& a, const KEY& b) = undefinedlt<KEY>);

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template <class Iterable>
        explicit BSTMap (const Iterable& i, bool (*clt)(const KEY& a, const KEY& b) = undefinedlt<KEY>);


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
        BSTMap<KEY,T,tlt>& operator = (const BSTMap<KEY,T,tlt>& rhs);
        bool operator == (const BSTMap<KEY,T,tlt>& rhs) const;
        bool operator != (const BSTMap<KEY,T,tlt>& rhs) const;

        template<class KEY2,class T2, bool (*lt2)(const KEY2& a, const KEY2& b)>
        friend std::ostream& operator << (std::ostream& outs, const BSTMap<KEY2,T2,lt2>& m);



        class Iterator {
        public:
            //Private constructor called in begin/end, which are friends of BSTMap<T>
            ~Iterator();
            Entry       erase();
            std::string str  () const;
            BSTMap<KEY,T,tlt>::Iterator& operator ++ ();
            BSTMap<KEY,T,tlt>::Iterator  operator ++ (int);
            bool operator == (const BSTMap<KEY,T,tlt>::Iterator& rhs) const;
            bool operator != (const BSTMap<KEY,T,tlt>::Iterator& rhs) const;
            Entry& operator *  () const;
            Entry* operator -> () const;
            friend std::ostream& operator << (std::ostream& outs, const BSTMap<KEY,T,tlt>::Iterator& i) {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }
            friend Iterator BSTMap<KEY,T,tlt>::begin () const;
            friend Iterator BSTMap<KEY,T,tlt>::end   () const;

        private:
            //If can_erase is false, the value has been removed from "it" (++ does nothing)
            ArrayQueue<Entry> it;                 //Queue for the nodes with key/value pairs; use for iterator with dequeue
            BSTMap<KEY,T,tlt>* ref_map;
            int               expected_mod_count;
            bool              can_erase = true;

            //Called in friends begin/end
            Iterator(BSTMap<KEY,T,tlt>* iterate_over, bool from_begin);
            Iterator(BSTMap<KEY,T,tlt>* iterate_over);
        };


        Iterator begin () const;
        Iterator end   () const;


    private:
        class TN {
        public:
            TN ()                     : left(nullptr), right(nullptr){}
            TN (const TN& tn)         : value(tn.value), left(tn.left), right(tn.right){}
            TN (Entry v, TN* l = nullptr,
                TN* r = nullptr) : value(v), left(l), right(r){}

            Entry value;
            TN*   left;
            TN*   right;
        };

        bool (*lt) (const KEY& a, const KEY& b); //lt is used for searching BST nodes (defined in template or constructor)
        TN* map       = nullptr;                 //Pointer to note that is root of BST
        int used      = 0;                       //Cache the # of nodes with key/value pairs in BST
        int mod_count = 0;                       //Used to sense for concurrent modification

        //Helper methods (find_key written iteratively, the rest recursively)
        TN*   find_key            (TN*  root, const KEY& key)                 const; //Returns reference to key's node or nullptr
        bool  has_value           (TN*  root, const T& value)                 const; //Returns whether value is is root's tree
        TN*   copy                (TN*  root)                                 const; //Copy the keys/values in root's tree (identical structure)
        void  copy_to_queue       (TN* root, ArrayQueue<Entry>& q)            const; //Fill queue with root's tree value
        bool  equals              (TN*  root, const BSTMap<KEY,T,tlt>& other) const; //Returns whether root's keys/value are all in other
        std::string string_rotated(TN* root, std::string indent)              const; //Returns string representing root's tree

        T     insert              (TN*& root, const KEY& key, const T& value);       //Put key->value, returning key's old value (or new one's, if key absent)
        T&    find_addempty       (TN*& root, const KEY& key);                       //Return reference to key's value (adding key->T() first, if key absent)
        Entry remove_closest      (TN*& root);                                       //Helper for remove
        T     remove              (TN*& root, const KEY& key);                       //Remove key->value from root's tree
        void  delete_BST          (TN*& root);                                       //Deallocate all TN in tree; root == nullptr
        void  copy_map              (TN*& self, TN*& new_one);
    };





////////////////////////////////////////////////////////////////////////////////
//
//BSTMap class and related definitions

//Destructor/Constructors

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::~BSTMap() {
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::BSTMap(bool (*clt)(const KEY& a, const KEY& b))
    :lt(tlt != (ltfunc)undefinedlt<KEY> ? tlt : clt) {
//        std::cout <<"successful call it\n";
        if (lt == (ltfunc)undefinedlt<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: neither specified");
        if (tlt != (ltfunc)undefinedlt<KEY> && clt != (ltfunc)undefinedlt<KEY> && tlt != clt)
        {
            throw TemplateFunctionError("BSTMap::default constructor: both specified and different");
        }
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::BSTMap(const BSTMap<KEY,T,tlt>& to_copy, bool (*clt)(const KEY& a, const KEY& b))
    {
        if(tlt != (ltfunc)undefinedlt<KEY>)
        {
            lt = tlt;
        }
        else
        {
            lt = clt;
        }
        if (lt == (ltfunc)undefinedlt<KEY>)
            lt = to_copy.lt;// throw TemplateFunctionError("BSTMap::default constructor: neither specified");
        if (tlt != (ltfunc)undefinedlt<KEY> && clt != (ltfunc)undefinedlt<KEY> && tlt != clt)
        {
            throw TemplateFunctionError("BSTMap::default constructor: both specified and different");
        }
        if(lt != to_copy.lt)
        {
            lt = to_copy.lt;
            for (auto element : to_copy)
            {
                put(element.first,element.second);
            }
        }
        else
        {
            map = copy(to_copy.map);
            used = to_copy.size();
            mod_count = to_copy.mod_count;
        }
    }
//    void  BSTMap<KEY,T,tlt>::copy_map(TN*& self, TN*& new_one)
//    {
//        if(new_one == NULL)
//        {
//            return;
//        }
//        self->value = new_one->value;
//        TN* a = new TN;
//        self -> left = a;
//        self->left = copy_map(a,new_one->left);
//        TN* b = new TN;
//        self -> right = b;
//        self->right = copy_map(b,new_one->right);
//    }

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::BSTMap(const std::initializer_list<Entry>& il, bool (*clt)(const KEY& a, const KEY& b))
    {
        if(tlt != (ltfunc)undefinedlt<KEY>)
        {
            lt = tlt;
        }
        else
        {
            lt = clt;
        }
        if (lt == (ltfunc)undefinedlt<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: neither specified");
        if (tlt != (ltfunc)undefinedlt<KEY> && clt != (ltfunc)undefinedlt<KEY> && tlt != clt)
        {
            throw TemplateFunctionError("BSTMap::default constructor: both specified and different");
        }
        for(auto element:il)
        {
            put(element.first,element.second);
        }
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    template <class Iterable>
    BSTMap<KEY,T,tlt>::BSTMap(const Iterable& i, bool (*clt)(const KEY& a, const KEY& b))
    {
        if(tlt != (ltfunc)undefinedlt<KEY>)
        {
            lt = tlt;
        }
        else
        {
            lt = clt;
        }
        if (lt == (ltfunc)undefinedlt<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: neither specified");
        if (tlt != (ltfunc)undefinedlt<KEY> && clt != (ltfunc)undefinedlt<KEY> && tlt != clt)
        {
            throw TemplateFunctionError("BSTMap::default constructor: both specified and different");
        }
        for(auto element:i)
        {
            put(element.first,element.second);
        }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::empty() const {
        return used == 0;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    int BSTMap<KEY,T,tlt>::size() const {
        return used;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::has_key (const KEY& key) const {
        if(find_key(map, key) == nullptr){
            return false;
        }
        return true;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::has_value (const T& value) const {
        return this->has_value(map, value);
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    std::string BSTMap<KEY,T,tlt>::str() const {
        std::ostringstream answer;
        answer << "bst_map[\n";
        answer << string_rotated(map,"");
        answer << "](used=" << used << ",mod_count=" << mod_count << ")";
        return answer.str();
//        return "";

    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    T BSTMap<KEY,T,tlt>::put(const KEY& key, const T& value) {
        return insert(map, key, value);
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    T BSTMap<KEY,T,tlt>::erase(const KEY& key) {
//    if(remove(map, key) == KeyError("")){
//        throw KeyError("");
//    }
        if(this->has_key(key)){
            --used;
            ++mod_count;
            return remove(map,key);
        }
        return remove(map,key);
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    void BSTMap<KEY,T,tlt>::clear() {
        map = nullptr;
        used = 0;
        mod_count = 0;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    template<class Iterable>
    int BSTMap<KEY,T,tlt>::put_all(const Iterable& i) {
        int count = 0;
        for(auto m : i)
        {
            count ++;
            put(m.first,m.second);
        }
        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    T& BSTMap<KEY,T,tlt>::operator [] (const KEY& key) {
        if(this->has_key(key)){
            return find_key(map,key)->value.second;
        }
        TN* news = new TN(Entry(key, T()));
        put(news->value.first, news->value.second);
        delete news;
        return find_key(map,key)->value.second;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    const T& BSTMap<KEY,T,tlt>::operator [] (const KEY& key) const {
        if(this->has_key(key)){
            return find_key(map,key)->value.second;
        }
        throw KeyError("..");
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>& BSTMap<KEY,T,tlt>::operator = (const BSTMap<KEY,T,tlt>& rhs) {
        //dengyu
        if(lt != rhs.lt)
        {
            lt = rhs.lt;
            for (auto items:rhs)
            {
                put(items.first,items.second);
            }
        }
        else
        {
            map = copy(rhs.map);
            used = rhs.size();
            mod_count = rhs.mod_count;
        }
        return *this;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::operator == (const BSTMap<KEY,T,tlt>& rhs) const {
        if(this == &rhs){
            return true;
        }
        if(used != rhs.size()){
            return false;
        }
        if(this->lt != rhs.lt){
            return false;
        }
        if(this->map == NULL && rhs.map == NULL)
        {
            return true;
        }
        ArrayQueue<Entry> q;
        copy_to_queue(map,q);
        for (auto element : q)
        {
            if(!rhs.has_key(element.first) || (*this)[element.first] != rhs[element.first])
            {
                return false;
            }
        }
        return true;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::operator != (const BSTMap<KEY,T,tlt>& rhs) const {
        if(this == &rhs){
            return false;
        }
        if(used != rhs.size()){
            return true;
        }
        if(this->lt != rhs.lt){
            return true;
        }
        if(this->map == NULL && rhs.map == NULL)
        {
            return false;
        }
        ArrayQueue<Entry> q;
        copy_to_queue(map,q);
        for (auto element : q)
        {
            if(!rhs.has_key(element.first) || (*this)[element.first] != rhs[element.first])
            {
                return true;
            }
        }
        return false;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    std::ostream& operator << (std::ostream& outs, const BSTMap<KEY,T,tlt>& m) {
        outs << "map[";
        int a = 0;
        for(auto x: m)
        {
            if(a == 0)
            {
                outs << x.first << "->" << x.second;
                a++;
            }
            else
            {
                outs << "," << x.first << "->" << x.second;
            }
        }
        outs << "]";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    auto BSTMap<KEY,T,tlt>::begin () const -> BSTMap<KEY,T,tlt>::Iterator {
        return Iterator(const_cast<BSTMap<KEY,T,tlt>*>(this),true);
    }

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    auto BSTMap<KEY,T,tlt>::end () const -> BSTMap<KEY,T,tlt>::Iterator {
        return Iterator(const_cast<BSTMap<KEY,T,tlt>*>(this));
    }

////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    typename BSTMap<KEY,T,tlt>::TN* BSTMap<KEY,T,tlt>::find_key (TN* root, const KEY& key) const {
        for(TN* c = root; c!= nullptr; c=(lt(key, c->value.first) ? c->left : c->right)){
            if(c->value.first == key){
                return c;
            }
        }
        return nullptr;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::has_value (TN* root, const T& value) const {
        if(root == nullptr){
            return false;
        }
        if(root->value.second == value){
            return true;
        }
        else{
            return this->has_value(root->left, value) or this->has_value(root->right, value);
        }
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    typename BSTMap<KEY,T,tlt>::TN* BSTMap<KEY,T,tlt>::copy (TN* root) const {
        if(root == NULL)
        {
            return NULL;
        }
        else
        {
//            std::cout << "try to create\n";
            return new TN(root->value, copy(root->left), copy(root->right));
        }
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    void BSTMap<KEY,T,tlt>::copy_to_queue (TN* root, ArrayQueue<Entry>& q) const {
        if(root == NULL)
        {
            return;
        }
        q.enqueue(root->value);
        copy_to_queue(root->left,q);
        copy_to_queue(root->right,q);
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::equals (TN* root, const BSTMap<KEY,T,tlt>& other) const {
//    if(root->value == other.map->value)
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    std::string BSTMap<KEY,T,tlt>::string_rotated(TN* root, std::string indent) const {
//        std::ostringstream answer;
//        if(root ->right != NULL)
//        {
//            answer << ".." << string_rotated(root -> right,"");
//        }
//        if(root -> left != NULL)
//        {
//            answer << string_rotated(root -> left,"");
//        }
//        if(root->right == NULL|| root->left == NULL)
//        {
//            answer << root->value.first << "->" << root->value.second << "\n";
//            return answer.str();
//        }
//        return answer.str();
        std::ostringstream answer;
        if (root == nullptr)
            return answer.str();
        else {
            answer << string_rotated(root->right, indent+"..");
            answer << indent << root->value.first << "->" << root->value.second << std::endl;
            answer << string_rotated(root->left, indent+"..");
        }
        return answer.str();
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    T BSTMap<KEY,T,tlt>::insert (TN*& root, const KEY& key, const T& value) {
        if(root == nullptr){
            used++;
            ++mod_count;
            root = new TN(Entry(key,value));
            return root->value.second;
        }
        if(lt(key,root->value.first)){
            return insert(root->left, key, value);
        }
        else if(root->value.first == key){
            T old_value = root->value.second;
            root->value.second = value;
            ++mod_count;
            return old_value;
        }
        else{
            return insert(root->right, key, value);
        }
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    T& BSTMap<KEY,T,tlt>::find_addempty (TN*& root, const KEY& key) {
//    if(this->has_key(key)){
//        return find_key(map,key)->value.second;
//    }
//    TN* news = new TN(Entry(key, T()));
//    put(news->value.first, news->value.second);
//    T& to_return = news->value.second;
//    delete news;
//    used++;
//    ++mod_count;
//    std::cout << to_return << std::endl;
//    return to_return;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    pair<KEY,T> BSTMap<KEY,T,tlt>::remove_closest(TN*& root) {
        if (root->right != nullptr)
            return remove_closest(root->right);
        else{
            Entry to_return = root->value;
            TN* to_delete = root;
            root = root->left;
            delete to_delete;
            return to_return;
        }
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    T BSTMap<KEY,T,tlt>::remove (TN*& root, const KEY& key) {
        if (root == nullptr) {
            std::ostringstream answer;
            answer << "BSTMap::erase: key(" << key << ") not in Map";
            throw KeyError(answer.str());
        }else
        if (key == root->value.first) {
            T to_return = root->value.second;
            if (root->left == nullptr) {
                TN* to_delete = root;
                root = root->right;
                delete to_delete;
            }else if (root->right == nullptr) {
                TN* to_delete = root;
                root = root->left;
                delete to_delete;
            }else
                root->value = remove_closest(root->left);
            return to_return;
        }else
            return remove( (lt(key,root->value.first) ? root->left : root->right), key);
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    void BSTMap<KEY,T,tlt>::delete_BST (TN*& root) {
    }






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::Iterator::Iterator(BSTMap<KEY,T,tlt>* iterate_over, bool from_begin)
    :ref_map(iterate_over), expected_mod_count(ref_map->mod_count){
        iterate_over->copy_to_queue(iterate_over->map,it);
    }
    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::Iterator::Iterator(BSTMap<KEY,T,tlt>* iterate_over)
    :ref_map(iterate_over), expected_mod_count(ref_map->mod_count)
    {
        iterate_over->copy_to_queue(iterate_over->map,it);
        it.clear();
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    BSTMap<KEY,T,tlt>::Iterator::~Iterator()
    {}


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    auto BSTMap<KEY,T,tlt>::Iterator::erase() -> Entry {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::erase");
        if (!can_erase)
            throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor already erased");
        if (it.size() == 0)
            throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor beyond data structure");
        can_erase = false;
        auto to_return = it.peek();
//        std::cout << "finish setting\n";
        ref_map->erase(it.peek().first);
        it.dequeue();
        expected_mod_count = ref_map->mod_count;
//        std::cout << "finish erase\n";
        return to_return;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    std::string BSTMap<KEY,T,tlt>::Iterator::str() const {
        std::ostringstream answer;
        answer << ref_map->str() << "(expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
        return answer.str();
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    auto  BSTMap<KEY,T,tlt>::Iterator::operator ++ () -> BSTMap<KEY,T,tlt>::Iterator& {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::operator ++");

        if (it.size() == 0)
            return *this;

        if (can_erase)
            it.dequeue();         //decreasing priority goes backward in array, towards 0
        else
            can_erase = true;  //current already indexes "one beyond" deleted value

        return *this;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    auto BSTMap<KEY,T,tlt>::Iterator::operator ++ (int) -> BSTMap<KEY,T,tlt>::Iterator {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::operator ++(int)");

        if (it.size() == 0)
            return *this;

        Iterator to_return(*this);
        if (can_erase)
            it.dequeue();         //decreasing priority goes backward in array, towards 0
        else
            can_erase = true;  //current already indexes "one beyond" deleted value

        return to_return;
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::Iterator::operator == (const BSTMap<KEY,T,tlt>::Iterator& rhs) const {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if (rhsASI == 0)
            throw IteratorTypeError("BSTMap::Iterator::operator ==");
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::operator ==");
        if (ref_map != rhsASI->ref_map)
            throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator ==");

        if(it.size() == 0 && rhsASI->it.size() == 0)
        {
            return true;
        }
        return it.peek() == rhsASI->it.peek();
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    bool BSTMap<KEY,T,tlt>::Iterator::operator != (const BSTMap<KEY,T,tlt>::Iterator& rhs) const {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if (rhsASI == 0)
            throw IteratorTypeError("BSTMap::Iterator::operator !=");
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::operator !=");
        if (ref_map != rhsASI->ref_map)
            throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator !=");
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


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    pair<KEY,T>& BSTMap<KEY,T,tlt>::Iterator::operator *() const {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::operator *");
        if (!can_erase || it.size() == 0) {
            std::ostringstream where;
            where << 0 << " when size = " << ref_map->size();
            throw IteratorPositionIllegal("BSTMap::Iterator::operator * Iterator illegal: "+where.str());
        }

        return it.peek();
    }


    template<class KEY,class T, bool (*tlt)(const KEY& a, const KEY& b)>
    pair<KEY,T>* BSTMap<KEY,T,tlt>::Iterator::operator ->() const {
        if (expected_mod_count !=  ref_map->mod_count)
            throw ConcurrentModificationError("BSTMap::Iterator::operator ->");
        if (!can_erase || it.size() == 0) {
            std::ostringstream where;
            where << 0 << " when size = " << ref_map->size();
            throw IteratorPositionIllegal("BSTMap::Iterator::operator -> Iterator illegal: "+where.str());
        }

        return &it.peek();
    }


}

#endif /* BST_MAP_HPP_ */
