#include <iostream>
#include "CFG.h"

using namespace std;

int main() {
    //opdr 1
//    CFG cfg;
//    cfg.print();
//    return 0;

    CFG cfg("CFG.json");
    cfg.print();
    return 0;
}
