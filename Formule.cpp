#include "Formule.hpp"
#include <iostream>
#include <sstream>

using namespace std;

bool is_binary(operateur op) {
    switch (op) {
        case o_et:
        case o_ou:
        case o_equivaut:
        case o_implique:
            return true;
        default:
            return false;
    }
}

int priority_level(operateur op) {
    switch (op) {
        case o_variable:
            return -1;
        case o_non:
            return 1;
        case o_et:
            return 2;
        case o_ou:
            return 3;
        case o_implique:
            return 4;
        default:
            return 5;
    }
}

operateur operateur_for_level(int lvl) {
    switch (lvl) {
        case 1:
            return o_non;
        case 2:
            return o_et;
        case 3:
            return o_ou;
        case 4:
            return o_implique;
        case 5:
            return o_equivaut;
        default:
            return o_variable;
    }
}


//Fonction d'affichage des diffŽrents ŽlŽments
string operateur2string(operateur op) {
    switch (op) {
        case o_variable:
            return "var";
        case o_non:
            return "~";
        case o_et:
            return "/\\";
        case o_ou:
            return "\\/";
        case o_implique:
            return "=>";
        default:
            return "<=>";
    }
}

string formule2string(formule * f) {
    switch (f->op) {
        case o_variable:
            return *(f->nom);
        case o_non:
            return "~" + formule2string(f->arg);
        default:
            return "("+formule2string(f->arg1)
            +" "+operateur2string(f->op)+" "
            +formule2string(f->arg2)+")";
    }
}


//Fonctions de crŽation de formule
formule * mkbinop(operateur op, formule* arg1, formule* arg2) {
    formule*res = new formule();
    res->op = op;
    res->arg1 = arg1;
    res->arg2 = arg2;
    return res;
}

formule * var(string * nom) {
    formule*res = new formule();
    res->op = o_variable;
    res->nom = nom;
    return res;
}

formule * var(string nom)  {
    formule*res = new formule();
    res->op = o_variable;
    res->nom = new string(nom);
    return res;
}

formule * non(formule * arg) {
    formule * res = new formule();
    res->op = o_non;
    res->arg = arg;
    return res;
}

formule * et(formule* arg1, formule* arg2) {
    return mkbinop(o_et,arg1,arg2);
}

formule * ou(formule* arg1, formule* arg2) {
    return mkbinop(o_ou,arg1,arg2);
}

formule * impl(formule* arg1, formule* arg2) {
    return mkbinop(o_implique,arg1,arg2);
}

formule * equiv(formule* arg1, formule* arg2) {
    return mkbinop(o_equivaut,arg1,arg2);
}