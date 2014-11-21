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
        int writeKit(string, int);
        int writePattern(string, int);
        StringMatrix readKit(int);
};
    
    
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
int Pattern::writeKit(string params, int n)
{
    return write("kit", params, n);
}

// Writes pattern to "ptrnn.txt"
// Overwrites the current content of "ptrnn.txt"
int Pattern::writePattern(string pattern, int n)
{
    return write("ptrn", pattern, n);
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
    p.writeKit("1 2 3 4 5 6 7 8\n9 10 11 12 13 14 15 16", 2);

    StringMatrix s = p.readKit(2);

    for(int i = 0; i < s.size(); ++i)
    {
        for(int j = 0; j < s[i].size(); ++j)
        {
            cout << s[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}

