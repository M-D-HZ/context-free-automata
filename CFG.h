#ifndef MEB_TOI_CFG_H
#define MEB_TOI_CFG_H

#include <iostream>
#include <vector>
#include "Objects.h"

using namespace std;

class CFG {
    vector<Objects*> Terminals;
    vector<Objects*> Variables;
    vector<Objects*> Objecten;
    Objects* Startsymbol;
    string Result;
public:
/// CONSTRUCTER

    CFG();
    CFG(const string& inputfile);

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

    Objects* FindObject(const string& name);

    void toCNF();

    void EliminateEpsilon(Objects* C);

    bool IsNullable(Objects* C);

    void EliminateUnit(Objects* C,Objects* D);

};



#endif //MEB_TOI_CFG_H
