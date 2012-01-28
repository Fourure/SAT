#ifndef Formule
#define Formule
#include <string>
#include <map>
#include <list>
#include <vector>

using namespace std;

//STRUCTURE :
enum operateur {
  o_variable,
  o_non,
  o_et,
  o_ou,
  o_implique,
  o_equivaut
};

struct formule {
  operateur op;
  union {
    string * nom;
    struct {
      formule * arg1;
      formule * arg2;
    };
    formule * arg;
  };
};

//FONCTIONS :

extern bool is_binary(operateur);
extern int priority_level(operateur);
extern operateur operateur_for_level(int lvl);

//Fonction d'affichage des différents éléments
extern string operateur2string(operateur op);
extern string formule2string(formule * f);

//Fonctions de création de formule
extern formule * mkbinop(operateur op, formule* arg1, formule* arg2);
extern formule * var(string * nom);
extern formule * var(string nom);
extern formule * non(formule * arg);
extern formule * et(formule* arg1, formule* arg2);
extern formule * ou(formule* arg1, formule* arg2);
extern formule * impl(formule* arg1, formule* arg2);
extern formule * equiv(formule* arg1, formule* arg2);


#define START_LVL 5
#endif
