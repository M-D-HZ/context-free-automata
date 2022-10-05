#ifndef MEB_TOI_OBJECTS_H
#define MEB_TOI_OBJECTS_H
#include <iostream>
#include <vector>

using namespace std;

class Objects {
    string Naam;
    bool isVariable;                            // Variable or Terminal
    vector<vector<Objects*>> Production;        // Production Rules

public:
/// CONSTRUCTOR

    Objects(const string &naam, bool isVariable);

/// SETTERS AND GETTERS

    const string &getNaam() const;

    void setNaam(const string &naam);

    bool isVariable1() const;

    void setIsVariable(bool isVariable);

    const vector<vector<Objects *>> &getProduction() const;

    void setProduction(const vector<vector<Objects *>> &production);

/// EXTRA FUNCTIONALITY

    void addProductionRule(vector<Objects*> Rule);

    void ProductionPrint(string VarName);
};


#endif //MEB_TOI_OBJECTS_H
