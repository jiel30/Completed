// HashMap.hpp
// Name: Jie Li
// ID: 52536988
// ICS 45C Fall 2016
// Project #3: Maps and Legends
//
// This header file contains a declaration for a HashMap class, which
// is a separately-chained hash table implementation of a map (i.e.,
// a collection of key/value pairs), where the keys and values are
// strings.  This HashMap consists of a dynamically-allocated array
// of "buckets", each of which is a pointer to the first node in a
// linked list of key/value pairs that have hashed to that bucket.
//
// You may not make changes to the members of this class as they are
// declared now, since we will be running unit tests against your class
// that will expect these declarations not to have changed (and that
// its member functions are implemented as specified below).  However,
// you are free to add whatever additional members you feel are necessary,
// including new member variables or member functions.

#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <functional>
#include <string>
#include <iostream>
using namespace std;

//the dafault hash function for HashMap
unsigned int defaulthf(std::string address)
{
    unsigned int a = address.length() * address.length() * 2;
    return a;
}

class HashMap
{
public:
    // This is called a "typedef", which is a way to give a synonym to an
    // existing type.  In this case, we're introducing HashFunction as a
    // new name for a type representing a function that takes a reference
    // to a const string and returns an unsigned int.  Given that, a
    // variable or parameter of type HashFunction can be assigned such
    // a function.
    //
    // Hash functions must conform to these properties:
    //
    // (1) Given a particular string s repeatedly, they must always
    //     return the same hash value.
    // (2) They do not take the number of buckets into account (as they
    //     do not receive a parameter that tells them how many buckets
    //     there are).  Any unsigned int value is fair game as a result.
    //     It will be the job of the HashMap class to reduce the results
    //     to the range of available bucket indices (e.g., by using the
    //     % operator).
    typedef std::function<unsigned int(const std::string&)> HashFunction;

    // This constant specifies the number of buckets that a HashMap will
    // have when it is initially constructed.
    unsigned int initialBucketCount = 10;


public:

    // the check function takes id and password to find if it matches the value we have
    bool check(const std::string& key, const std::string& value)const;

    //the rearrange function is used when there are too many key in the buckets, it will enlarge the buckets and rearrange keys and values
    void rearrange ();


    // This constructor initializes the HashMap to use whatever default
    // hash function you'd like it to use.  A little research online will
    // yield some good ideas about how to write a good hash function for
    // strings; don't just return zero or, say, the length of the string.
    HashMap();

    // This constructor instead initializes the HashMap to use a particular
    // hash function instead of the default.  (We'll use this in our unit
    // tests to control the scenarios more carefully.)
    HashMap(HashFunction hasher);

    // The "Big Three" need to be implemented appropriately, so that HashMaps
    // can be created, destroyed, copied, and assigned without leaking
    // resources, interfering with one another, or causing crashes or
    // undefined behavior.
    HashMap(const HashMap& hm);
    ~HashMap();
    HashMap& operator=(const HashMap& hm);

    // add() takes a key and a value.  If the key is not already stored in
    // this HashMap, the key/value pair is added; if the key is already
    // stored, the function has no effect.
    //
    // If adding the new key/value pair will cause the load factor of this
    // HashMap to exceed 0.8, the following must happen:
    //
    // (1) The number of buckets should be increased by doubling it and
    //     adding 1 (i.e., if there were 10 buckets, increase it to
    //     2 * 10 + 1 = 21).
    // (2) All key/value pairs should be rehashed into their new buckets,
    //     important because changing the number of buckets will likely
    //     change which bucket a particular key hashes to (especialy if
    //     you're using % to determine the index of that bucket).
    void add(const std::string& key, const std::string& value);

    // remove() takes a key and removes it (and its associated value) from
    // this HashMap if it is already present; if not, the function has no
    // effect.
    void remove(const std::string& key);

    // contains() returns true if the given key is in this HashMap, false
    // if not.
    bool contains(const std::string& key) const;

    // value() returns the value associated with the given key in this HashMap
    // if the key is stored in this HashMap; if not, the empty string is
    // returned.  (Going forward, we'll discover that throwing an exception
    // is a better way to handle the scenario where the key is not present,
    // but we'll conquer that at a later date.)
    std::string value(const std::string& key) const;

    // size() returns the number of key/value pairs stored in this HashMap.
    unsigned int size() const;

    // bucketCount() returns the number of buckets currently allocated in
    // this HashMap.
    unsigned int bucketCount() const;

    // loadFactor() returns the proportion of the number of key/value pairs
    // to the number of buckets, a measurement of how "full" the HashMap is.
    // For example, if there are 20 key/value pairs and 50 buckets, we would
    // say that the load factor is 20/50 = 0.4.
    double loadFactor() const;

    // maxBucketSize() returns the number of key/value pairs stored in this
    // HashMap's largest bucket.
    unsigned int maxBucketSize() const;

    void clear();
    void PrintAll();




private:
    // This structure describes the nodes that make up the linked lists in
    // each of this HashMap's buckets.
    struct Node
    {
        std::string key;
        std::string value;
        Node* next;
    };


    // Store the hash function (either the default hash function or the one
    // passed to the constructor as a parameter) in this member variable.
    // When you want to hash a key, call this member variable (i.e., follow
    // it with parentheses and a parameter) just like you would any other
    // function.
    HashFunction hasher;
    Node ** buckets;
    unsigned int totalbuckets;
    unsigned int loginnumber;
    // You will no doubt need to add at least a few more private members

    //this function is used to set all variable for HashMap
    void setup();
    void hashcopy(Node** target, Node** source, unsigned int size);
};

HashMap::HashMap()
{
    hasher = defaulthf;  //set hasher to the default hashfunction
    setup();       //use setup function to set up other element
}
HashMap::HashMap(HashFunction hasher)
{
    this -> hasher = hasher;     //set hasher as the input one
    setup();
}
void HashMap::setup(void)
{
    totalbuckets = initialBucketCount;    //initial bucket is 10
    loginnumber = 0;                     //no login account at the first time
    buckets = new Node* [totalbuckets];   //start at 10 array
    for (int aa = 0; aa < totalbuckets; aa++)
    {
        buckets[aa] = NULL;              //set all cells to NULL
    }
}
bool HashMap::contains(const std::string &key) const
{
    bool determine = false;            //use this variable to decide whether we find the key or not
    int number = hasher(key);   //use hash function to fund its hash number
    int index = number % totalbuckets;    // use % to get the index in the array
    if (buckets[index] == NULL)
    {
        return determine;              //if the element in the cell is NULL, we will say it is not in array
    }
    else
    {
        Node * current = buckets[index];     //create one pointer to trace the process
        while (current != NULL)
        {
            if (current -> key == key)
            {
                determine = true;          //once we find the same key, we will change the determine to true and return it
                return determine;
            }
            current = current -> next;     //make the pointer points to the next address
        }
    }
    return determine;
}
bool HashMap::check(const std::string& key, const std::string& value) const
{
	bool determine = false;            //use this variable to decide whether we find the key or not
	int number = hasher(key);   //use hash function to fund its hash number
	int index = number % totalbuckets;    // use % to get the index in the array
	if (buckets[index] == NULL)
	{
	    return determine;              //if the element in the cell is NULL, we will say it is not in array
	}
	else
	{
	    Node * current = buckets[index];     //create one pointer to trace the process
	    while (current != NULL)
	    {
	        if (current -> key == key and current -> value == value)
	        {
	            determine = true;          //once we find the same key, we will change the determine to true and return it
	            return determine;
	        }
	        current = current -> next;     //make the pointer points to the next address
	    }
	}
	 return determine;

}
void HashMap::add(const std::string &key, const std::string &value)
{
    int number = hasher(key);     //find the hash number
    int index = number % totalbuckets;      //get the index
    if (not contains(key))                     //check if we already have the key
    {
        Node * a = new Node;   //create a new pointer which is the first node for the linked lists
        a -> key = key;
        a -> value = value;
        a -> next = buckets[index];
        buckets[index] = a;    //put the key and value in the first of the linked lists
    }
    loginnumber ++;
}
void HashMap::remove(const std::string &key)
{
    int number = hasher(key);  //find hash number
    int index = number % totalbuckets;  //get the index
    if (not contains(key))   //if we do not contain this key, we will just stop
    {
    	return;
    }
    if (buckets[index] -> key == key)   //find if the first element of the linked list is the one we want to remove
    {
        Node * a = buckets[index];  //create one pointer of the first element of the linked list
        buckets[index] = a -> next;  //make the second element to be the first
        delete a;  //delete the first linked list
        loginnumber = loginnumber - 1;
        return;
    }
    else
    {
    	if (buckets[index] != NULL)
    	{
    		Node* current = buckets[index];    //create the current to trace the flow
    		Node * n = current -> next;    //create the n which is next of current, so after we delete one, we can link the list again
    		while (n != NULL)   //since the first element of the buckets cannot be the one we are looking for, so I use n to determine
    		{
    			if (n -> key == key)
    			{
    				current -> next = n -> next;
    				delete n;
    				loginnumber = loginnumber - 1;
    			}
    			current = current -> next;
    			n = n -> next;
    		}
    	}
    }
}

std::string HashMap::value(const std::string& key) const
{
    int number = hasher(key);   //find hash number
    int index = number % totalbuckets;   //find the index
    Node * current = buckets[index];   //create the node to trace the process
    while (current != NULL)
    {
        if (current -> key == key)   //check whether we find the key
        {
            return current -> value;   // get the value
        }
        current = current -> next;   //make current be the next element in chained lists
    }
    return "";   //if we do not find any, return a empty string
}

unsigned int HashMap::size() const
{
	return loginnumber;   //since we will update the number of login after we add or remove, we can return it directly
}

unsigned int HashMap::bucketCount() const
{
    return totalbuckets;   //since we will update the number of bucket after we add more, we can return it directly
}

double HashMap::loadFactor() const
{
    double s = size();    //get the size
    double b = bucketCount();    //get the number of bucket
    return double(s)/double(b);  //transfer int to double and divide them
    //use the pair size to divide the number of bucket to get the loadfactor
}


void HashMap::rearrange ()
{
    unsigned int oldnumber = totalbuckets;    //we need the old number to control the loop of old buckets
    totalbuckets = oldnumber *2 + 1;   //get the size of the new buckets
    Node ** new_buckets = new Node * [totalbuckets];   //create the new buckets
    loginnumber = 0;   //reset the login number
    for (int aa = 0; aa < totalbuckets; aa++)
    {
    	new_buckets[aa] = NULL;   //set up the new buckets
    }
    Node **copy = buckets;   //create a copy of old buckets, so we can put the old element back
    HashMap::buckets = new_buckets;
    for (int aa = 0; aa < oldnumber; aa++)
    {
    	Node * current = new Node;
    	current = copy[aa];
    	while(current != NULL)
    	{
    		add(current -> key, current -> value);   //add the element in old buckets back to the new one
    		Node * c = current;
    		current = current -> next;
    		delete c;   //delete the element in old
    	}
    	delete current;
    }
    delete[] copy;
}

void HashMap::clear(void)
{
	for (int aa = 0; aa < totalbuckets; aa++)   // check each bucket
	{
		Node * current = buckets[aa];  //get the element in linked list
		buckets[aa] = NULL;
		while (current != NULL)   // check each element in the linked list and delete them later
		{
			Node * c = current;
			current = current -> next;
			loginnumber -= 1;
			delete c;
		}
	}
}

void HashMap::PrintAll()
{
	for (int aa = 0; aa < totalbuckets; aa++)  //check all buckets for information
	{
		Node * current = buckets[aa];
		while (current != NULL)
		{
			std::cout << "User ID: " << current -> key << " Password: " << current -> value << std::endl;
			current = current -> next;
		}
	}
}

unsigned int HashMap::maxBucketSize() const
{
	unsigned int det = 0;   //set the int to record the largest buckets
	unsigned int total = 0;  //set this int to record the number of element in one linked list
	Node * current;
	for (int aa = 0; aa < totalbuckets; aa++)
	{
		current = buckets[aa];
		while (current != NULL)
		{
			total ++;  //count the number of element in the linked list
			current = current -> next;
		}
		if (total > det)  //check if the new number is bigger than the previous one
		{
			det = total;
		}
		total = 0;   //reset the total
	}
	return det;
}

void HashMap::hashcopy(Node** target, Node** source, unsigned int size)
{
	for (int aa = 0; aa < size; aa++)
	{
		Node * current = new Node;
		if (source[aa] != NULL)
		{
			current-> key = source[aa] -> key;
			current -> value = source[aa] -> value;
			current -> next = NULL;
		}
		else
		{
			current = NULL;
		}
		Node * trace = source[aa];
		target[aa] = current;
		while (current != NULL)
		{
			trace = trace -> next;
			if (trace != NULL)
			{
				Node * n = new Node;
				n -> key = trace -> key;
				n -> value = trace -> value;
				n -> next = NULL;
				current -> next = n;
				current = current -> next;
			}
			else
			{
				current = current -> next;
			}
		}

	}
}

HashMap::HashMap(const HashMap& hm)
{
	Node ** buckets = new Node* [hm.totalbuckets];
	this -> hasher = hm.hasher;
	this-> totalbuckets = hm.totalbuckets;
	this-> loginnumber = hm.loginnumber;
	hashcopy(this-> buckets, hm.buckets, hm.totalbuckets);
}

HashMap::~HashMap()
{
	clear();
	delete[] buckets;
}

HashMap& HashMap::operator=(const HashMap& hm)
{
	if (this != &hm)
	{
		this->clear();
		delete buckets;
		totalbuckets = hm.totalbuckets;
		hasher = hm.hasher;
		loginnumber = hm.loginnumber;
		hashcopy(buckets,hm.buckets,hm.totalbuckets);
	}
	return *this;
}

//this function is going to get a string which contains spaces, and find the first
//space. Cutting the space and previous string and return the rest
//if it does not find any, it will return a empty string

std::string cutspace(std::string a)
{
	std::string::size_type position;
	position = a.find(" ");
	if (position == a.npos)
	{
		return "";
	}
	else
	{
		return a.substr(position + 1);
	}
}

//this function is used to get the string before the first space
//it will return empty string if there is no space

std::string cutafter(std::string a)
{
	std::string::size_type position1;
	position1 = a.find(" ");
	if (position1 == a.npos)
	{
		return "";
	}
	else
	{
		return a.substr(0, position1);
	}
}

#endif // HASHMAP_HPP
