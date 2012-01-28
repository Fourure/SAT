#include <iostream>
#include <sstream>
#include "Formule.hpp"
#include "Parsers.hpp"
#include "Solveur.hpp"

using namespace std;

int main(int argc, char** argv) {
    
    formule * une_formule;
    formule_conj * une_formule_conjonctive;
    
    string lue;
    
    map<string,int> correspondance;
    interpretation i;
    indexation index;
    
    if (argc >= 2) {
        lue = string(argv[1]);
        une_formule = litFormuleFichier(argv[1]);
    } else {
        stringbuf buffer;
        cin.get(buffer);
        lue = buffer.str();
        une_formule = litFormuleString(lue);
        
    }
    
    //*
    cout << endl << "Analyse :" << endl;
    cout << formule2string(une_formule) << endl;
    //*/
    
    
    //Numérotation des variables.
    numerote(une_formule,correspondance);
    
    //*
    cout << endl << "Correspondance :\n";
    cout << map2string(correspondance) << endl;
    //*/
    
    triplet * res=forme_conjonctive(une_formule,correspondance);
    une_formule_conjonctive= et(ou(res->var_repr),res->forme_conj);
    
    //*
    cout << "Formule conjonctive equisatisfiable : " << endl;
    cout << formule2string(une_formule_conjonctive) << endl;
    //*/
    
    i.resize(res->var_repr);
    
    index=indexeClauses(une_formule_conjonctive,res->var_repr);
    
    //cout << endl << "Nombre de clause = " << une_formule_conjonctive->size() << endl << endl;
    
    /*
    cout << "Index :" << endl;
    affiche_index(index);
    cout << endl;
    //*/

    if(cherche(une_formule_conjonctive,res->var_repr,i, index))
        cout <<"\033[0;32mLa formule est satisfiable\033[0m" << endl;
    else
        cout <<"\033[0;31mLa formule n'est PAS satisfiable\033[0m" << endl;
    
    delete (res);
    
    return 0;
}
