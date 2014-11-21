#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

const int MAX_NO_INSTRUMENTS = 8;
const int MAX_NO_PARAMETERS = 8;

typedef vector<vector<string> > StringMatrix;

class Pattern {
    private:
        int write(string, string, int);
    public:
        int writeKit(StringMatrix, int);
        int writePattern(StringMatrix, int);
        string packData(StringMatrix);
        StringMatrix readKit(int);
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
    
    
// Writes pattern to "fileNamen.txt"
// Overwrites the current content of "fileNamen.txt"
int Pattern::write(string fileName, string data, int n)
{
    fileName += to_string(n) + ".txt";
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
    return write("kit", s, n);
}

// Writes pattern to "ptrnn.txt"
// Overwrites the current content of "ptrnn.txt"
int Pattern::writePattern(StringMatrix pattern, int n)
{
    string s = packData(pattern);
    return write("ptrn", s, n);
}

// Reads the contents of "kitn.txt" and returns
// them as a 2D string vector
StringMatrix Pattern::readKit(int n)
{
    StringMatrix kit;
    
    ifstream file("kit" + to_string(n) + ".txt");
    string param;

    for(int i = 0; i < MAX_NO_INSTRUMENTS && !file.eof(); ++i)
    {
        kit.push_back(vector<string>());
        for(int j = 0; j < MAX_NO_PARAMETERS && !file.eof(); ++j)
        {
            file >> param;
            kit[i].push_back(param);
        }
    }

    return kit;
}


int main()
{
    Pattern p;

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

    StringMatrix sm2 = p.readKit(2);

    for(int i = 0; i < sm2.size(); ++i)
    {
        for(int j = 0; j < sm2[i].size(); ++j)
        {
            sm2[i][j] = to_string(stoi(sm2[i][j]) + 1);
        }
    }

    int i = p.writeKit(sm2, 3);

    return 0;
}

