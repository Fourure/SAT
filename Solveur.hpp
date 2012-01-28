#ifndef Solveur
#define Solveur

#include <string>
#include <map>
#include <list>
#include <vector>
#include "Formule.hpp"

typedef pair<bool,vector<int> > clause;
typedef vector<clause *> formule_conj;

typedef vector<int> interpretation;

struct triplet {
    formule_conj * forme_conj;
    int var_repr;
    int suivant;
};

struct indexation {
    vector< pair<list<clause*>,int> > positif;
    vector< pair<list<clause*>,int> > negatif;
};

//Fonction d'affichage des différents éléments
extern void affiche_interpretation(const interpretation& valeurs);
void affiche_index(const indexation& index);
extern string list2string(const list<clause*>& l);
extern string map2string (const map<string,int>& correspondance);
extern string clause2string(const clause * c);
extern string formule2string(const formule_conj * f);
extern string interpretation2string(const interpretation& i);

//Numérotation des variables
extern void numerote(const formule* f,map<string,int>& corres);

//Fonctions de création de formule conjonctives
extern clause * ou(int arg1, clause * arg2);
extern clause * ou(clause * arg1,clause * arg2);
extern clause * ou(int valeur);
extern clause * ou(int val1, int val2);
extern clause * ou(int val1, int val2, int val3);
extern formule_conj * et(formule_conj * arg1, formule_conj * arg2);
extern formule_conj * et(clause *  arg1, formule_conj * arg2);

//Traduction en forme conjonctive
extern triplet * forme_conjonctive(const formule * f,const map<string,int>& correspondance);
extern triplet * forme_conjonctive(const formule * f,const map<string,int>& correspondance, int suivant);

//Indexation des clauses
extern indexation indexeClauses(const formule_conj *f,int nb_variable);

//Propagation Unaire
bool cherche(formule_conj * fc,int id_var,interpretation& valeurs, indexation& index);
extern int clause_est_satisfaite(clause * c,interpretation& valeurs,int& i);
extern bool propage(int variable, interpretation& valeurs, indexation& index, formule_conj * fc , list<int>& deduite, vector<clause*>& clause_deduite);

//Variable monotones
void monotone(interpretation& valeurs ,indexation& index, clause* c, list<int>& deduite, vector<clause*>& clause_deduite);
void backtrack_monotone(indexation& index, clause* c);

#endif
