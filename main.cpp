#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <fstream>
#include <map>

using namespace std;

int main(int argc, char **argv) {

    if (argc > 2) {
        cerr << "Too many arguments" << endl;
        exit(1);
    }

    // QWERTY -> dvorak map
    map<char, char> emap;
    emap['A'] = 'A';
    emap['B'] = 'N';
    emap['C'] = 'I';
    emap['D'] = 'H';
    emap['E'] = 'D';
    emap['F'] = 'Y';
    emap['G'] = 'U';
    emap['H'] = 'J';
    emap['I'] = 'G';
    emap['J'] = 'C';
    emap['K'] = 'V';
    emap['L'] = 'P';
    emap['M'] = 'M';
    emap['N'] = 'L';
    emap['O'] = 'S';
    emap['P'] = 'R';
    emap['Q'] = 'X';
    emap['R'] = 'O';
    emap['S'] = ';';
    emap['T'] = 'K';
    emap['U'] = 'F';
    emap['V'] = '.';
    emap['W'] = ',';
    emap['X'] = 'B';
    emap['Y'] = 'T';
    emap['Z'] = '/';

    // dvorak -> QWERTY map
    map<char, char> dmap;
    dmap['A'] = 'A';
    dmap['N'] = 'B';
    dmap['I'] = 'C';
    dmap['H'] = 'D';
    dmap['D'] = 'E';
    dmap['Y'] = 'F';
    dmap['U'] = 'G';
    dmap['J'] = 'H';
    dmap['G'] = 'I';
    dmap['C'] = 'J';
    dmap['V'] = 'K';
    dmap['P'] = 'L';
    dmap['M'] = 'M';
    dmap['L'] = 'N';
    dmap['S'] = 'O';
    dmap['R'] = 'P';
    dmap['X'] = 'Q';
    dmap['O'] = 'R';
    dmap[';'] = 'S';
    dmap['K'] = 'T';
    dmap['F'] = 'U';
    dmap['.'] = 'V';
    dmap[','] = 'W';
    dmap['B'] = 'X';
    dmap['T'] = 'Y';
    dmap['/'] = 'Z';

    if (argc == 2) {
        fstream file(argv[1]);
        string top;
        getline(file, top);
        if (top == "#encrypt") {
            // Initial file encryption
            char c;
            string outString;
            while (file >> c) outString += emap[c];
            file.close();

            // Write encrypted text back to same file without #encrypt header
            file.open(argv[1], fstream::out | fstream::trunc);
            file << outString;
            file.close();
        } else {
            // We are opening an encrypted file
            // Go back to the beginning
            file.clear();
            file.seekg(0, ios::beg);

            // Decrypt file contents
            char c;
            string outString;
            while (file >> c) outString += dmap[c];
            file.close();

            // Write decrypted text back to same file
            file.open(argv[1], fstream::out | fstream::trunc);
            file << outString;
            file.close();

            // Open decrypted file in gedit in a separate process
            int f = fork();

            if (f < 0) {
                cerr << errno << endl;
                exit(1);
            }

            if (f == 0) execlp("gedit", "gedit", argv[1], nullptr);

            if (f > 0) {
                wait(nullptr);

                file.open(argv[1]);

                // Initial file encryption
                outString = "";
                while (file >> c) outString += emap[c];
                file.close();

                // Write encrypted text back to same file without #encrypt header
                file.open(argv[1], fstream::out | fstream::trunc);
                file << outString;
                file.close();
            }
        }
    }

    return 0;
}
