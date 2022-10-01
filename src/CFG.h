#ifndef MEB_TOI_CFG_H
#define MEB_TOI_CFG_H

#include <iostream>
#include <vector>
#include "../src/Objects.h"

using namespace std;

class CFG {
    vector<Objects*> Terminals;
    vector<Objects*> Variables;
    Objects* Startsymbol;
    string Result;
public:
/// CONSTRUCTER

    CFG();

/// SETTERS AND GETTERS

    const vector<Objects *> &getTerminals() const;

    void setTerminals(const vector<Objects *> &terminals);

    const vector<Objects *> &getVariables() const;

    void setVariables(const vector<Objects *> &variables);

    Objects *getStartsymbol() const;

    void setStartsymbol(Objects *startsymbol);

    const string &getResult() const;

    void setResult(const string &result);

/// EXTRA FUNCTIONS

    void addTerminal(Objects* Terminal);

    void addVariable(Objects* Variable);

    void print();

    void sortVariables();

    void sortTerminals();

};


#endif //MEB_TOI_CFG_H
