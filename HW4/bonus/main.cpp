#include <iostream>
#include <chrono>
#include "SaliencyDetect.h"

using namespace std;
using namespace chrono;


int main(int argc, const char** argv) {
    if (argc > 1) {
        int u = 4;
        char outFile[1000];
        strcpy(outFile, "target.png");
        for (int i = 2; i < argc - 1; ++i) {
            if (strcmp(argv[i], "-u") == 0) {
                u = atoi(argv[i+1]);
                ++i;
            } else if (strcmp(argv[i], "-o") == 0) {
                strcpy(outFile, argv[i+1]);
                ++i;
            }
        }
        printf("out=%s, u=%i\n", outFile, u);
        auto start = system_clock::now();
        exec(argv[1], outFile, u);
        auto end = system_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "running time: " << double(duration.count()) * microseconds::period::num / microseconds::period::den << "s"<<endl;
    } else {
        cout << "Instruction: <filename> [-u region size] [-o output file name]" << endl;
    }
    system("pause");
    return 0;
}