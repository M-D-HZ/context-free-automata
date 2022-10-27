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

    bool IsGen(Objects* &C);

    bool HasTermProd(Objects* C);

    Objects* FindObject(const string& name);

    void EliminateProd(Objects* C);

    bool Reachable(Objects* C);

    void BreakBodies(Objects* &C, int &amount);

    Objects* ReplaceBodies(vector<Objects*> C, Objects* S, int count);

    void BadBodies(Objects* &C, vector<Objects*> &N);

    Objects* ReplaceBody(Objects* C, vector<Objects*> &N);

    void toCNF();

    void EliminateEpsilon(Objects* &C);

    bool IsNullable(Objects* C);

    int EliminateUnit(vector<pair<Objects*,Objects*>> Units, int amount);

    void UnitPairs(vector<pair<Objects*,Objects*>> &UP, Objects* C, Objects* D=NULL);

    bool NoDuplicates(vector<pair<Objects*,Objects*>> UP, pair<Objects*,Objects*> k);
    bool NoDuplicateProd(vector<Objects*> production,Objects* C);

};



#endif //MEB_TOI_CFG_H
