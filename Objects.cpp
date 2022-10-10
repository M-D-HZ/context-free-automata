#include <algorithm>
#include "Objects.h"
#include "fstream"
#include "vector"

using namespace std;

Objects::Objects(const string &naam, bool isVariable) : Naam(naam), isVariable(isVariable) {

}

const string &Objects::getNaam() const {
    return Naam;
}

void Objects::setNaam(const string &naam) {
    Naam = naam;
}

bool Objects::isVariable1() const {
    return isVariable;
}

void Objects::setIsVariable(bool isVariable) {
    Objects::isVariable = isVariable;
}

const vector<vector<Objects *>> &Objects::getProduction() const {
    return Production;
}

void Objects::setProduction(const vector<vector<Objects *>> &production) {
    Production = production;
}

void Objects::addProductionRule(vector<Objects *> Rule) {
    if (this->isVariable){
        Production.push_back(Rule);
    }
}

void Objects::ProductionPrint(string VarName) {
    sort(Production.begin(), Production.end());
    for(auto i:Production){
        cout << "    " + VarName + " -> `";
        for (auto j:i){
            if (j != i.back()){
                cout << j->Naam + " ";
            }
            else{
                cout << j->Naam;
            }
        }

        cout << "`" << endl;
    }

}
