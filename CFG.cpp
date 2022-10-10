#include "CFG.h"
#include "json.hpp"
#include "iostream"
#include "fstream"
#include "iomanip"

using namespace std;
using json = nlohmann::json;

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
    this->sortVariables();
    this->sortTerminals();

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
