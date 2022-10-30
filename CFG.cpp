#include "CFG.h"
#include "json.hpp"
#include "iostream"
#include "fstream"
#include <vector>
#include <set>

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
    for (auto i:Variables){
        vector<string> temp;
        for (auto j:i->getProduction()) {
            string prod = "`";
            for (int k = 0; k < j.size(); ++k) {
                if (k == j.size()-1){
                    prod += j[k]->getNaam();
                }
                else{
                    prod += j[k]->getNaam() + " ";
                }
            }
            prod += "`";
            temp.push_back(prod);
        }
        sort(temp.begin(),temp.end());
        for (auto j: temp){
            cout << "  " + i->getNaam() + " -> " + j << endl;
        }
    }
//    for (auto i:Variables) {
//        vector<vector<Objects*>> k = i->getProduction();
//        sort(k.begin(),k.end(), CompareVec);
//        i->setProduction(k);
//        i->ProductionPrint(i->getNaam());
//    }
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
    cout << " >> Eliminating epsilon productions" << endl;
    cout << "  Nullables are {";
    for (auto i:Variables) {
       if(IsNullable(i) && i == Variables.back()){ cout << i->getNaam() ; }
       else if (IsNullable(i)){ cout << i->getNaam() << ", "; }
       for (auto j:i->getProduction()){
            OriginalAmount++;
            if (j.empty()){
                EliminateEpsilon(i);
            }
       }
    }
    cout << "}" << endl;
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
    cout << "  Created " << NewAmount << " productions, original had " << OriginalAmount << endl << endl;
    print();
    cout << endl << endl;
    cout << " >> Eliminating unit pairs" << endl;
    int uprod = 0;
    for (auto i:Variables){
        for (auto j:i->getProduction()){
            if (j.size() == 1 && j[0]->isVariable1()){
                uprod++;
            }
        }
    }
    cout << "  Found " << uprod << " unit productions" << endl;
    /// Eliminating unit pairs
    OriginalAmount = NewAmount;
    vector<pair<Objects*,Objects*>> UnitPairen;
    for (auto i: Variables){
        UnitPairen.push_back({i,i});
        UnitPairs(UnitPairen,i);
    }
    NewAmount = EliminateUnit(UnitPairen,NewAmount);
    for (auto i: Variables){
        NewAmount = i->EliminateSingles(NewAmount);
    }
    vector<vector<Objects*>> Unitsort;
    cout << "  Unit pairs: {";
    for (auto i:UnitPairen){
        Unitsort.push_back({i.first,i.second});
    }
    sort(Unitsort.begin(),Unitsort.end(), CompareVec);
    for (auto i:Unitsort){
        if (i != Unitsort.back()){
            cout << "(" << i[0]->getNaam() << ", " << i[1]->getNaam() << ")" << ", ";
        }
        else{
            cout << "(" << i[0]->getNaam() << ", " << i[1]->getNaam() << ")";
        }
    }
    cout << "}" << endl;
    cout << "  Created " << NewAmount << " new productions, original had " << OriginalAmount << endl << endl;
    print();
    cout << endl << endl;
    cout << " >> Eliminating useless symbols" << endl;
    vector<Objects*> Generating;
    vector<Objects*> reachable;
    int varsize = Variables.size();
    int Tersize = Terminals.size();
    OriginalAmount = NewAmount;

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
    cout << "  Generating symbols: {";
    for (auto i:Generating){
        if (i != Generating.back()){
            cout << i->getNaam() << ", ";
        }
        else{
            cout << i->getNaam();
        }
    }
    cout << "}" << endl;
    cout << "  Reachable symbols: {";
    for (auto i:reachable){
        if (i != reachable.back()){
            cout << i->getNaam() << ", ";
        }
        else{
            cout << i->getNaam();
        }
    }
    cout << "}" << endl;
    cout << "  Useful symbols: {";
    for (auto i:reachable){
        if (i != reachable.back()){
            cout << i->getNaam() << ", ";
        }
        else{
            cout << i->getNaam();
        }
    }
    int changeamount = 0;
    for (auto i:Variables) {
        for (auto j:i->getProduction()){
            changeamount++;
        }
    }
    cout << "}" << endl;
    cout << "  Removed " << varsize - Variables.size() << " variables, " << Tersize-Terminals.size() << " terminals and " << OriginalAmount-changeamount << " productions"<< endl << endl;
    print();
    cout << endl << endl;
    cout << " >> Replacing terminals in bad bodies" << endl;
    OriginalAmount = changeamount;
    vector<Objects*> NewVars;
    varsize = Variables.size();
    for (auto i:Variables){
        BadBodies(i,NewVars);
    }
    int test = Variables.size()-varsize;
    cout << "  Added " << test << " new variables: {";
    for (auto i:NewVars){
        if (i == NewVars.back()){
            cout << i->getNaam();
        }
        else{
            cout << i->getNaam() << ", " ;
        }
    }
    cout << "}"<< endl;
    changeamount = 0;
    for (auto i:Variables){
        for (auto j:i->getProduction()){
            changeamount++;
        }
    }
    cout << "  Created " << changeamount << " new productions, original had " << OriginalAmount << endl << endl;
    print();
    cout << endl << endl;
    varsize = Variables.size();
    int bodiesbroken = 0;
    for (auto i:Variables){
        int count = 1;
        BreakBodies(i,bodiesbroken,count);
    }
    cout << " >> Broke " << bodiesbroken << " bodies, added " << Variables.size()-varsize << " new variables" << endl;
    cout << ">>> Result CFG:" << endl<<endl;
    print();

}

bool CFG::IsNullable(Objects* C) {
    for (auto i:C->getProduction()){
        if (i.empty()){
            return true;
        }
    }
    for (auto i:C->getProduction()){
        for (auto j:i) {

            if (j->isVariable1() && j != C && IsNullable(j)){
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

int CFG::EliminateUnit(vector<pair<Objects*,Objects*>> Units, int amount) {
    for (auto i:Units){
        if (i.first == i.second){
            continue;
        }
        for (auto j:i.second->getProduction()){
            if (j.size() == 1 && j[0]->isVariable1()){
                for (auto k:j[0]->getProduction()) {
                    if (NoDuplicateProd(k,i.first)){
                        i.first->addProductionRule(k);
                        amount++;
                    }
                }
                continue;
            }
            if (NoDuplicateProd(j,i.first)){
                i.first->addProductionRule(j);
                amount++;
            }
        }
    }
    return amount;
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

void CFG::BadBodies(Objects* &C, vector<Objects*> &N) {
    vector<vector<Objects*>> temp = C->getProduction();
    for (auto& i:temp) {
        for (auto &j:i){
            if (!j->isVariable1() && i.size() > 1){
                j = ReplaceBody(j,N);
            }
        }
    }
    C->setProduction(temp);

}

Objects *CFG::ReplaceBody(Objects* C, vector<Objects*> &NE) {
    for (auto i:Variables){
        for (auto j:i->getProduction()){
            if (j.size() == 1 && j[0] == C){
                return i;
            }
        }
    }
    Objects* N = new Objects("_" + C->getNaam(), true);
    N->addProductionRule({C});
    Variables.push_back(N);
    NE.push_back(N);
    return N;
}

void CFG::BreakBodies(Objects* &C, int &bodies, int &count) {
    vector<vector<Objects*>> temp = C->getProduction();
    for (auto &i:temp){
        if (i.size() > 2){
            count++;
            Objects* N = new Objects(C->getNaam() + "_" + to_string(count) , true);
            N->addProductionRule({i[i.size()-2],i[i.size()-1]});
            Variables.push_back(N);
            i[i.size()-2] = N;
            bodies++;
            i.erase(i.begin()+i.size()-1);
            C->setProduction(temp);
            BreakBodies(C,bodies,count);
            return;
        }
    }

}

Objects *CFG::ReplaceBodies(vector<Objects*> C, Objects* S, int count) {
    Objects* N = new Objects(S->getNaam() + "_" + to_string(count) , true);
    N->addProductionRule({C[0],C[1]});
    Variables.push_back(N);
    return N;
}

bool CFG::accepts(string w) {
    map<string,vector<Objects*>> CYKTABLE;
    for (auto i:w){
        string k;k += i;
        CYKTABLE[k] = CYKBasis(k);
    }
    CYK(CYKTABLE,2,w);
    vector<int> tempmax;
    for (int rt = 0 ; rt < w.size()+1; ++rt) {
        int maxs = 0;
        for (int i = 1; i < w.size() - (rt - 1); ++i) {
            string k = w.substr(rt,i);
            set<Objects*> temp;
            copy(CYKTABLE[k].begin(),CYKTABLE[k].end(), inserter(temp,temp.end()));
//            cout << k << endl;
            if (temp.size() > maxs){
                maxs = temp.size();
            }
        }
//        cout << maxs << endl;
        maxs = maxs+2*(maxs-1);
        tempmax.push_back(maxs);
    }
    int index = 0;
    bool acceptss = false;
    for (int rt = w.size(); rt > 0; --rt) {
        for (int i = 0; i < w.size()-(rt-1); ++i) {
            string k = w.substr(i,rt);
            set<Objects*> temp;
            copy(CYKTABLE[k].begin(),CYKTABLE[k].end(), inserter(temp,temp.end()));
            cout << "| {";
            for (auto j:temp){
                if (rt == w.size() && j->getNaam() == Startsymbol->getNaam()){
                    acceptss = true;
                }
                if (j != CYKTABLE[k].back()){
                    cout << j->getNaam() << ", ";
                }
                else{
                    cout << j->getNaam();
                }
            }
            cout << "}  ";
            int currel = temp.size() + 2*(temp.size()-1);
            if (temp.empty()){
                currel = 0;
            }
            if (tempmax[index] - currel > 0){
                int k = 1;
                for (int j = 0; j <= tempmax[index] - currel-k; ++j) {
                    cout << " ";
                }
            }
            index++;
        }
        cout << "|"<< endl;
        index = 0;
    }
    if (acceptss){
        cout << "true" << endl;
    }
    else{
        cout << "false" << endl;
    }
    return acceptss;
}

vector<Objects*> CFG::CYKBasis(string a) {
    vector<Objects*> temp;
    for (auto i:Variables){
        for (auto j:i->getProduction()){
            if (j.size() == 1 && j[0]->getNaam() == a){
                temp.push_back(i);
            }
        }
    }
    return temp;
}

void CFG::CYK(map<string,vector<Objects*>> &CYKTABLE,int size, string w){
    int aks = w.size();
    if (size > w.size()){
        return;
    }
    for (int i = 0; i <  w.size()-1; ++i) {
        string k = w.substr(i,size);
        if (k == "aab"){
            cout << endl;
        }
        for (int j = 0; j < k.size()-1; ++j) {
            string pos = k.substr(0,j+1);
            string pos1 = k.substr(j+1,size-1);
            vector<Objects*> temp = CYKCarthesis(CYKTABLE[pos],CYKTABLE[pos1]);
            CYKTABLE[k].insert(CYKTABLE[k].end(),temp.begin(),temp.end());
        }
    }
    size++;
    CYK(CYKTABLE,size,w);
}

vector<Objects*> CFG::CYKCarthesis(vector<Objects*> a, vector<Objects*> b) {
    vector<Objects*> temp;
    for (auto i:a) {
        for (auto j:b){
            vector<Objects*> k = CYKAlgo({i,j});
            temp.insert(temp.end(),k.begin(),k.end());
        }
    }
    return temp;
}

vector<Objects *> CFG::CYKAlgo(vector<Objects*> a) {
    vector<Objects*> temp;
    for (auto i:Variables){
        for (auto j:i->getProduction()){
            if (j == a){
                temp.push_back(i);
            }
        }
    }
    return temp;
}




