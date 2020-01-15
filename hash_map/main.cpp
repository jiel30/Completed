//
//  HashMap.cpp
//  test
//
//  Name: Jie Li
//  ID: 52536988
//  Created by Li Jie on 8/4/19.
//  Copyright © 2019 Li Jie. All rights reserved.
//

#include "HashMap.hpp"
#include <iostream>
using namespace std;

int main()
{
	std::string cutspace(std::string a);
	std::string cutafter(std::string a);
    HashMap h;  //create the hashmap h
    bool v = false;
    bool debug = false;
    while (true)
    {
    	v = false;
    	string first;
    	getline(cin,first);
    	if (first == "CLEAR")
    	{
    		v = true;
    		h.clear();
    		cout << "CLEARED" << endl;
    	}
    	if (first == "QUIT")
    	{
    		cout << "GOODBYE" << endl;
    		break;   //if the input is QUIT, the while loop will break
    	}
    	if (first == "ALLACCOUNT")
    	{
    		v = true;
    		if (debug == true)
    		{
    			cout << "all account: " << endl;
    			h.PrintAll();
    			cout << "end" << endl;
    		}
    		else
    		{
    			cout << "INVAILD" << endl;
    		}
    	}
    	if (first == "ACCOUNTNUMBER")
    	{
    		v = true;
    		if (debug == true)
    		{
    	    	cout << h.size() << endl;
    	    }
    	    else
    	    {
    	    	cout << "INVAILD" << endl;
    	    }
    	}
    	if (first == "DEBUG ON")
    	{
    		v = true;
    		if (debug == false)
    		{
    			debug = true;
    			cout << "ON NOW" << endl;
    		}
    		else
    		{
    			cout << "ON ALREADY" << endl;
    		}
    	}
    	if (first == "DEBUG OFF")
    	{
    		v = true;
    		if (debug == false)
    		{
    			cout << "OFF ALREADY" << endl;
    		}
    		else
    		{
    			debug = false;
    			cout << "OFF NOW" << endl;
    		}
    	}
    	if (first == "LOGIN COUNT" and debug == true)
    	{
    		v = true;
    		cout << h.size() << endl;
    	}
    	if (first == "BUCKET COUNT" and debug == true)
    	{
    		v = true;
    		cout << h.bucketCount() << endl;
    	}
    	if (first == "LOAD FACTOR" and debug == true)
    	{
    		v = true;
    		cout << h.loadFactor() << endl;
    	}
    	if (first == "MAX BUCKET SIZE" and debug == true)
    	{
    		v = true;
    		cout << h.maxBucketSize() << endl;
    	}
    	if (first.length() < 8 and v == false)
    	{
    		cout << "INVALID" << endl;
    		v = true;
    	}
    	if (first.substr(0,6) == "REMOVE" and v == false)
    	{
    		v = true;
    		string second = cutspace(first);
    		string yyy = cutafter(first);
    		if (second == "" or yyy != "REMOVE")
    		{
    			cout << "INVALID" << endl;
    		}
    		else
    		{
    			if (cutspace(second) != "")
    			{
    				cout << "INVALID" << endl;
    			}
    			else
    			{
    				if (h.contains(second))
    				{
    					h.remove(second);
    				    cout << "REMOVED" << endl;
    				 }
    				 else
    				 {
    					 cout << "NONEXISTENT" << endl;
    				 }
    			}
    		}
    	}
    	if (v == false and first.substr(0,5) == "LOGIN")
    	{
    		v = true;
    		string second = cutspace(first);
    		string third = cutspace(second);
    		string xxx = cutafter(second);
    		string yyy = cutafter(first);
    		if (second == "" or third == "" or cutspace(third) != "" or yyy != "LOGIN")
    		{
    			cout << "INVALID" << endl;
    		}
    		else
    		{
    			if (h.check(xxx,third))
    			{
    				cout << "SUCCEEDED" << endl;
    			}
    			else
    			{
    				cout << "FAILED" << endl;
    			}
    		}
    	}
    	if (v == false and first.substr(0,6) == "CREATE")
    	{
    		v = true;
    		string second = cutspace(first);
    		string third = cutspace(second);
    		string xxx = cutafter(second);
    		string yyy = cutafter(first);
    		if (second == "" or third == "" or cutspace(third) != "" or yyy != "CREATE")
    		{
    			cout << "INVALID" << endl;
    		}
    		else
    		{
    			if (h.contains(xxx))
    			{
    				cout << "EXISTS" << endl;
    			}
    			else
    			{
    				h.add(xxx,third);
    				if (h.loadFactor() > 0.8)
    				{
    					h.rearrange();
    				}
    				cout << "CREATED" << endl;
    			}
    		}
    	}
    	if (v == false)
    	{
    		cout << "INVAILD" << endl;
    	}
    }
    return 0;
}
