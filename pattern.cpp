#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

const int MAX_NO_INSTRUMENTS = 8;
const int MAX_NO_PARAMETERS = 8;
const int MAX_LEN_SEQUENCE = 16;

typedef vector<vector<string> > StringMatrix;
typedef vector<vector<int> > IntMatrix;

class Pattern {
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

string Pattern::packData(StringMatrix data)
{
    string s = "";
    for(int i = 0; i < data.size(); ++i)
    {
        for(int j = 0; j < data[i].size(); ++j)
        {
            s += data[i][j] + " ";
        }
        s += "\n";
    }
    return s;
}

string Pattern::packData(IntMatrix data)
{
    string s = "";
    for(int i = 0; i < data.size(); ++i)
    {
        for(int j = 0; j < data[i].size(); ++j)
        {
            s += to_string(data[i][j]) + " ";
        }
        s += "\n";
    }
    return s;
}
    
    
// Writes pattern to "fileNamen.txt"
// Overwrites the current content of "fileNamen.txt"
int Pattern::write(string fileName, string data)
{
    ofstream file(fileName);
    file << data;
    file.close();
    return 0;
}

// Writes parameters to "kitn.txt"
// Overwrites the current content of "kitn.txt"
int Pattern::writeKit(StringMatrix params, int n)
{
    string s = packData(params);
    return write("kit" + to_string(n) + ".txt", s);
}
int Pattern::writeKit(IntMatrix params, int n)
{
    string s = packData(params);
    return write("kit" + to_string(n) + ".txt", s);
}

// Writes pattern to "ptrnn.txt"
// Overwrites the current content of "ptrnn.txt"
int Pattern::writePattern(StringMatrix pattern, int n)
{
    string s = packData(pattern);
    return write("ptrn" + to_string(n) + ".txt", s);
}
int Pattern::writePattern(IntMatrix pattern, int n)
{
    string s = packData(pattern);
    return write("ptrn" + to_string(n) + ".txt", s);
}


StringMatrix Pattern::read(string fileName, int xlen, int ylen)
{
    StringMatrix res;
    string val;

    ifstream file(fileName);

    for (int i = 0; i < ylen; ++i)
    {
        res.push_back(vector<string>());
        for (int j = 0; j < xlen; ++j)
        {
            file >> val;
            res[i].push_back(val);
        }
    }
    file.close();
    return res;
}

// Reads the contents of "kitn.txt" and returns
// them as a 2D string vector
StringMatrix Pattern::readKit(int n)
{
    string fileName = "kit" + to_string(n) + ".txt";
    return read(fileName, MAX_NO_PARAMETERS, MAX_NO_INSTRUMENTS);
}

StringMatrix Pattern::readPattern(int n)
{
    string fileName = "ptrn" + to_string(n) + ".txt";
    return read(fileName, MAX_LEN_SEQUENCE, MAX_NO_INSTRUMENTS);
}


int main()
{
    Pattern p;

    // Test write kit
    StringMatrix sm1;
    for(int i = 0; i < MAX_NO_INSTRUMENTS; ++i)
    {
        sm1.push_back(vector<string>());
        for(int j = 0; j < MAX_NO_PARAMETERS; ++j)
        {
            sm1[i].push_back(to_string(i * j));
        }
    }

    p.writeKit(sm1, 2);

    sm1 = p.readKit(2);

    for(int i = 0; i < sm1.size(); ++i)
    {
        for(int j = 0; j < sm1[i].size(); ++j)
        {
            sm1[i][j] = to_string(stoi(sm1[i][j]) + 1);
        }
    }

    p.writeKit(sm1, 3);


    // Test write pattern
    StringMatrix sm2;
    for(int i = 0; i < MAX_NO_INSTRUMENTS; ++i)
    {
        sm2.push_back(vector<string>());
        for(int j = 0; j < MAX_LEN_SEQUENCE; ++j)
        {
            sm2[i].push_back(to_string(i * j));
        }
    }

    p.writePattern(sm2, 4);

    sm2 = p.readPattern(4);

    for(int i = 0; i < sm2.size(); ++i)
    {
        for(int j = 0; j < sm2[i].size(); ++j)
        {
            sm2[i][j] = to_string(stoi(sm2[i][j]) + 1);
        }
    }

    p.writePattern(sm2, 5);


    return 0;
}

