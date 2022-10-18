#include <iostream>
#include "CFG.h"

using namespace std;

int main() {
//  opdr 1
//    CFG cfg;
//    cfg.print();
//    return 0;

//  opdr 2
//    CFG cfg("CFG.json");
//    cfg.print();
//    return 0;

//  opdr 3
    CFG cfg("input-cnf1.json");
    cfg.toCNF();
    return 0;

}
