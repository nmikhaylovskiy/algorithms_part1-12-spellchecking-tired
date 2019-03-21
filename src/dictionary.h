#ifndef  _DICTIONARY_H_ 
#define  _DICTIONARY_H_

#include  <iostream>
#include  <vector>
#include  <list>
#include  <algorithm>
#include  <string>
#include  <stdlib.h>
#include <fstream>

#include  "hashset.h"
#include  "hashset.cpp"

using namespace std;

class hash_function
{
public:

    unsigned int operator()( const string& s )  const { //моя любимая хэш функция, согласно варианту 24, sdbm, вязто из инет

        uint hash = 0;
        uint i = 0;


        for (i = 0; i < s.length(); i++)
        {
            hash = ((u_char)s[(int)i]) + (hash << 6) + (hash << 16) - hash;
        }
        return hash;

    }
};

class equality
{
public:
    equality() {}
    bool  operator()( const string& A, const string& B )  const 
    {
        return  (A == B);
    }
};

class Dictionary: public HashSet<string, hash_function, equality> {

public:
	Dictionary(string filename)
    {
        ifstream strm(filename);

        if (!strm)
            throw std::invalid_argument("Can't open the file?");

        std::string str;

        while (std::getline(strm, str))
        {
            if(str=="" || str.length()==0)
                continue;

            if (str[str.size()-1] == '\r')
                str.pop_back();

            insert(str);
        }
    }

};
#endif // _DICTIONARY_H_
