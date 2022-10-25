#include "CFG.h"
#include "json.hpp"
#include "iostream"
#include "fstream"

using namespace std;
using json = nlohmann::json;

bool Compare(Objects* a, Objects* b){return (*a < *b);}

bool CompareVec(vector<Objects*> a, vector<Objects*> b){
    for (int i = 0; i < min(a.size(),b.size()); ++i) {
        if (a[i] != b[i]){
            return *a[i] < *b[i];
        }
        if (a[i] == b[i] && i == min(a.size(),b.size())-1){
            return min(a.size(),b.size()) == b.size();
        }
    }
    return false;
}

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
        sort(k.begin(),k.end(), CompareVec);
        i->setProduction(k);
        i->ProductionPrint(i->getNaam());
    }
    cout << "}" << endl;
    cout << "S = " + Startsymbol->getNaam();
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
        i->EliminateEps();
        vector<vector<Objects*>> k = i->getProduction();
        sort(k.begin(),k.end());
        k.erase(unique(k.begin(),k.end()),k.end());
        i->setProduction(k);
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
    vector<pair<Objects*,Objects*>> UnitPairen;
    for (auto i: Variables){
        UnitPairen.push_back({i,i});
        UnitPairs(UnitPairen,i);
    }
    EliminateUnit(UnitPairen);
    for (auto i: Variables){
        i->EliminateSingles();
    }
    print();
    cout << endl << endl;
    cout << "-------------------------------------" << endl << endl;

    vector<Objects*> Generating;
    vector<Objects*> reachable;

    for (int i = 0; i < Variables.size(); ++i) {
        if (IsGen(Variables[i])){
            Generating.push_back(Variables[i]);
        }
        else{
            EliminateProd(Variables[i]);
            i--;
        }
    }
    for (auto i:Terminals){
        Generating.push_back(i);
    }
    for (auto i:Generating){
        if (Reachable(i) || i == Startsymbol){
            reachable.push_back(i);
        }
        else{
            EliminateProd(i);
        }
    }
    print();
    cout << endl << endl;
    cout << "-------------------------------------" << endl << endl;


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

void CFG::EliminateEpsilon(Objects* &C) {
    vector<vector<Objects*>> temp = C->getProduction();
    for (auto i:temp){
        vector<Objects*> NProd;
        vector<Objects*> ItProd = i;
        for (int j = 0; j < i.size(); ++j) {
            if (!i[j]->isVariable1()){
                NProd.push_back(i[j]);
            }
            else if(IsNullable(i[j])){
                ItProd.erase(ItProd.begin() + j);
                C->addProductionRule(ItProd);
                ItProd = i;
                continue;
            }
            else{
                NProd.push_back(i[j]);
            }
        }
        if (!NProd.empty()){
            C->addProductionRule(NProd);
        }
    }
    temp.clear();
}

void CFG::EliminateUnit(vector<pair<Objects*,Objects*>> Units) {
    for (auto i:Units){
        if (i.first == i.second){
            continue;
        }
        if (i.second->getNaam() == "B" && i.first->getNaam() == "D"){
            cout << endl;
        }
        for (auto j:i.second->getProduction()){
            if (j.size() == 1 && j[0]->isVariable1()){
                for (auto k:j[0]->getProduction()) {
                    if (NoDuplicateProd(k,i.first)){
                        i.first->addProductionRule(k);
                    }
                }
                continue;
            }
            if (NoDuplicateProd(j,i.first)){
                i.first->addProductionRule(j);
            }
        }
    }
}

void CFG::UnitPairs(vector<pair<Objects*,Objects*>> &UP, Objects* C, Objects* D) {
    pair<Objects*,Objects*> Unit;
    vector<vector<Objects*>> temp;
    if (D != NULL){
        temp = D->getProduction();
    }
    else{
        temp = C->getProduction();
    }
    for (auto i:temp){
        if (i.size() ==  1 && i[0]->isVariable1()){
            Unit.first = C;
            Unit.second = i[0];
            if (NoDuplicates(UP,Unit)){
                UP.push_back(Unit);
            }
            UnitPairs(UP,C,i[0]);
        }
    }
}

bool CFG::NoDuplicates(vector<pair<Objects*,Objects*>> UP, pair<Objects*,Objects*> k) {
    for (int i = 0; i < UP.size(); ++i) {
        if (UP[i].first == k.first && UP[i].second == k.second){
            return false;
        }
    }
    return true;
}

bool CFG::NoDuplicateProd(vector<Objects*> production,Objects* C) {
    for (auto i:C->getProduction()){
        if (i == production){
            return false;
        }
    }
    return true;
}

bool CFG::IsGen(Objects* &C) {
    if (HasTermProd(C)){
        return true;
    }
    for (auto i:C->getProduction()){
        if (i.size() > 1){
            for (auto j:i){
                if (j->getNaam() == C->getNaam()){
                    break;
                }
            }
            for (auto j:i){
                if (!HasTermProd(j)){
                    return false;
                }
            }
        }
//        if (i.size() == 1 && i[0]->isVariable1()){
//            IsGen(i[0]);
//        }
    }
    return false;
}

bool CFG::HasTermProd(Objects* C) {
    for (auto i:C->getProduction()){
        for (auto j:i){
            if (j->isVariable1()){
                continue;
            }
            else{
                return true;
            }
        }
    }
    return false;
}

void CFG::EliminateProd(Objects* C) {
    for (auto i:Variables){
        if (i->getNaam() != C->getNaam()){
            i->EliminateProd(C);
        }
    }
    for (int i = 0; i < Variables.size(); ++i) {
        if (Variables[i]->getNaam() == C->getNaam()){
            Variables.erase(Variables.begin()+i);
            break;
        }
    }
}

bool CFG::Reachable(Objects *C) {
    for (auto i:Variables){
        for (auto j:i->getProduction()){
            for (auto k:j){
                if (k->getNaam() == C->getNaam()){
                    return true;
                }
            }
        }
    }
    return false;
}




