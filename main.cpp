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

//  opdr 3 (toCNF)
//    CFG cfg("input-cnf1.json");
//    cfg.toCNF();
//    return 0;
//  opdr 3,2
//    CFG cfg("Second.json");
//    cfg.toCNF();
//    return 0;
//  opdr 4(CYK)
    CFG cfg("CYKINP.json");
    cfg.accepts("baaba"); //moet true geven
    cfg.accepts("abba"); //moet false geven
    return 0;

}
