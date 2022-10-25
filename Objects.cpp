#include "Objects.h"
#include <iostream>
#include <vector>

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

void Objects::ProductionPrint(const string& VarName) {
//    Productionsort();
    for(auto i:Production){
        cout << "    " + VarName + " -> `";
        for (int j = 0; j < i.size(); ++j) {
            if (j != i.size()-1){
                cout << i[j]->Naam + " ";
            }
            else{
                cout << i[j]->Naam;
            }
        }

        cout << "`" << endl;
    }

}

void Objects::Productionsort() {

    for (int i = 0; i < Production.size(); ++i) {
        if(Production[i] == Production.back()){
            return;
        }
        else if (Production[i].size() < Production[i+1].size()){
            vector<Objects*> temp = Production[i];
            Production[i] = Production[i+1];
            Production[i+1] = temp;
            Productionsort();
        }
    }
}

void Objects::EliminateEps() {
    for (int i = 0; i < Production.size(); ++i) {
        if (Production[i].empty()){
            Production.erase(Production.begin() + i);
        }
    }
}

void Objects::EliminateSingles() {
    for (int i = 0; i < Production.size(); ++i) {
        if (Production[i].size() == 1 && Production[i][0]->isVariable1()){
            Production.erase(Production.begin() + i);
            i--;
        }
    }
}

bool Objects::operator<(const Objects &rhs) const {
    return Naam < rhs.Naam;
}

bool Objects::operator>(const Objects &rhs) const {
    return rhs < *this;
}

bool Objects::operator<=(const Objects &rhs) const {
    return !(rhs < *this);
}

bool Objects::operator>=(const Objects &rhs) const {
    return !(*this < rhs);
}

bool Objects::operator==(const Objects &rhs) const {
    return Naam == rhs.Naam &&
           isVariable == rhs.isVariable &&
           Production == rhs.Production;
}

bool Objects::operator!=(const Objects &rhs) const {
    return !(rhs == *this);
}

void Objects::EliminateProd(Objects *C) {
    vector<vector<Objects*>> temp = Production;
    for (int i = 0; i < temp.size(); ++i) {
        for (int j = 0; j < temp[i].size(); ++j) {
            if (temp[i][j]->getNaam() == C->getNaam()){
                Production.erase(Production.begin()+i);
            }
        }
    }
}


