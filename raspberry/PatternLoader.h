//
//  Pattern.h
//  TonicStandaloneDemo
//
//  Created by Halldór Eldjárn on 22/11/14.
//
//

#ifndef __TonicStandaloneDemo__Pattern__
#define __TonicStandaloneDemo__Pattern__

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

const int MAX_NO_INSTRUMENTS = 4;
const int MAX_NO_PARAMETERS = 7;
const int MAX_LEN_SEQUENCE = 16;

typedef vector<vector<string> > StringMatrix;
typedef vector<vector<int> > IntMatrix;

class PatternLoader {
private:
    int write(string, string);
    StringMatrix read(string, int, int);
    string packData(StringMatrix);
    string packData(IntMatrix);
public:
    int writeKit(StringMatrix, int);
    int writePattern(StringMatrix, int);
    int writeKit(IntMatrix, int);
    int writePattern(IntMatrix, int);
    
    StringMatrix readKit(int);
    StringMatrix readPattern(int);
};

#endif /* defined(__TonicStandaloneDemo__Pattern__) */
