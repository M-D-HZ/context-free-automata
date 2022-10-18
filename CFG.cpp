#include "CFG.h"
#include "json.hpp"
#include "iostream"
#include "fstream"

using namespace std;
using json = nlohmann::json;

bool Compare(Objects* a, Objects* b){return (*a < *b);}

CFG::CFG() {

    Objects* BINDIGIT = new Objects("BINDIGIT", true);
    Objects* S = new Objects("S", true);

    this->addVariable(BINDIGIT);
    this->addVariable(S);

    Objects* Zero = new Objects("0", false);
    Objects* One = new Objects("1", false);
    Objects* a = new Objects("a", false);
    Objects* b = new Objects("b", false);

    this->addTerminal(Zero);
    this->addTerminal(b);
    this->addTerminal(One);
    this->addTerminal(a);

    vector<Objects*> NewRule = {Zero};
    BINDIGIT->addProductionRule(NewRule);
    NewRule = {One};
    BINDIGIT->addProductionRule(NewRule);

    NewRule = {};
    S->addProductionRule(NewRule);
    NewRule = {a,S,b,BINDIGIT};
    S->addProductionRule(NewRule);

    Startsymbol = S;
}

CFG::CFG(const string& inputfile) {

    ifstream input(inputfile);

    json j;
    input >> j;
    // tonen op het scherm
//    cout << setw(4) << j << endl;

    for (auto i:j["Variables"]) {
        Objects* variable = new Objects(i, true);
        Variables.push_back(variable);
        Objecten.push_back(variable);
    }


    for (auto i:j["Terminals"]) {
        Objects* terminal = new Objects(i, false);
        Terminals.push_back(terminal);
        Objecten.push_back(terminal);
    }

    for (auto i: j["Productions"]){
        vector<Objects*> newproduction;
        for (auto k:i["body"]){
            newproduction.push_back(FindObject(k));
        }
        FindObject(i["head"])->addProductionRule(newproduction);
    }
    Startsymbol = FindObject(j["Start"]);
}

const vector<Objects *> &CFG::getTerminals() const {
    return Terminals;
}

void CFG::setTerminals(const vector<Objects *> &terminals) {
    Terminals = terminals;
}

const vector<Objects *> &CFG::getVariables() const {
    return Variables;
}

void CFG::setVariables(const vector<Objects *> &variables) {
    Variables = variables;
}

Objects *CFG::getStartsymbol() const {
    return Startsymbol;
}

void CFG::setStartsymbol(Objects *startsymbol) {
    Startsymbol = startsymbol;
}

const string &CFG::getResult() const {
    return Result;
}

void CFG::setResult(const string &result) {
    Result = result;
}

void CFG::addTerminal(Objects *Terminal) {
    Terminals.push_back(Terminal);
}

void CFG::addVariable(Objects *Variable) {
    Variables.push_back(Variable);
}

void CFG::print() {
    sort(Variables.begin(),Variables.end(), Compare);
    sort(Terminals.begin(),Terminals.end(), Compare);
//    this->sortVariables();
//    this->sortTerminals();

    cout << "V = {";
    for (auto i:Variables) {
        if (i != Variables.back()){
            cout << i->getNaam() + ", ";
        }
        else{
            cout << i->getNaam() + "}" << endl;
        }
    }

    cout << "T = {";
    for (auto i:Terminals) {
        if (i != Terminals.back()){
            cout << i->getNaam() + ", ";
        }
        else{
            cout << i->getNaam() + "}" << endl;
        }
    }

    cout << "P = {" << endl;
    for (auto i:Variables) {
        vector<vector<Objects*>> k = i->getProduction();
        sort(k.begin(),k.end());
        i->setProduction(k);
        i->ProductionPrint(i->getNaam());
    }
    cout << "}" << endl;
    cout << "S = " + Startsymbol->getNaam();
}

void CFG::sortVariables() {
    for (int i = 0; i < Variables.size(); ++i) {
        if(Variables[i] == Variables.back()){
            return;
        }
        else if (Variables[i]->getNaam() > Variables[i+1]->getNaam()){
            Objects* temp = Variables[i];
            Variables[i] = Variables[i+1];
            Variables[i+1] = temp;
            sortVariables();
        }
    }
}

void CFG::sortTerminals() {
    for (int i = 0; i < Terminals.size(); ++i) {
        if(Terminals[i] == Terminals.back()){
            return;
        }
        else if (Terminals[i]->getNaam() > Terminals[i+1]->getNaam()){
            Objects* temp = Terminals[i];
            Terminals[i] = Terminals[i+1];
            Terminals[i+1] = temp;
            sortTerminals();
        }
    }
}

Objects *CFG::FindObject(const string& name) {
    for (auto i:Objecten){
        if (i->getNaam() == name){
            return i;
        }
    }
    return nullptr;
}

void CFG::toCNF() {
    cout << "Original CFG:" << endl << endl;
    print();
    cout << endl << endl;
    cout << "-------------------------------------" << endl << endl;

    /// Eliminating epsilon productions

    int OriginalAmount=0;
    int NewAmount =0;
    cout << ">> Eliminating epsilon productions" << endl;
    cout << " Nullables are {";
    for (auto i:Variables) {
       if(IsNullable(i) && i == Variables.back()){ cout << i->getNaam() << "}" << endl; }
       else if (IsNullable(i)){ cout << i->getNaam() << ", "; }
       for (auto j:i->getProduction()){
            OriginalAmount++;
            if (j.empty()){
                EliminateEpsilon(i);
            }
       }
    }
    for (auto i: Variables){
        vector<vector<Objects*>> k = i->getProduction();
        k.erase(unique(k.begin(),k.end()),k.end());
//        sort(k.begin(),k.end());
        i->setProduction(k);
        i->EliminateEps();
        for (auto j:i->getProduction()){
            NewAmount++;
        }
    }
    cout << " Created " << NewAmount << " Productions, original had " << OriginalAmount << endl << endl;
    print();
    cout << endl << endl;
    cout << "-------------------------------------" << endl << endl;

    /// Eliminating unit pairs
    OriginalAmount = NewAmount;

//    for (auto i: Variables){
//        for (auto j:i->getProduction()){
//            if (i->getNaam() == "D"){
//                cout << "gotcha you pig";
//            }
//            if (j.size() == 1){
//                EliminateUnit(i,j[0]);
//            }
//        }
//    }
//    for (auto i: Variables){
//        i->EliminateSingles();
//    }
//    print();

}

bool CFG::IsNullable(Objects* C) {
    for (auto i:C->getProduction()){
        if (i.empty()){
            return true;
        }
    }
    for (auto i:C->getProduction()){
        for (auto j:i) {
            if (j->isVariable1() && IsNullable(j)){
                return true;
            }
        }
    }
    return false;
}

void CFG::EliminateEpsilon(Objects* C) {
    vector<vector<Objects*>> test = C->getProduction();
    for (auto i:C->getProduction()){
        vector<Objects*> NProd;
        vector<Objects*> ItProd = i;
        for (int j = 0; j < i.size(); ++j) {
            if (!i[j]->isVariable1()){
                NProd.push_back(i[j]);
            }
            else if(IsNullable(i[j])){
                ItProd.erase(ItProd.begin() + j);
                test.push_back(ItProd);
                ItProd = i;
                continue;
            }
            else{
                NProd.push_back(i[j]);
            }
        }
        if (!NProd.empty()){
            test.push_back(NProd);
        }
    }
//    sort(test.begin(),test.end());
    C->setProduction(test);
}

void CFG::EliminateUnit(Objects* C , Objects* D) {
    for (auto i:D->getProduction()){
        if (i.size() > 1){
            C->addProductionRule(i);
        }
    }
}




