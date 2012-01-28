#include "Solveur.hpp"
#include <iostream>
#include <sstream>

using namespace std;

/*-------------------------------------------------------------------------------------------------------*/
/*                        Fonction d'affichage des différents éléments                                   */
/*-------------------------------------------------------------------------------------------------------*/

void affiche_interpretation(const interpretation& valeurs){
    int i;
    for (i=0;i<(int)valeurs.size();i++)
        cout << i << " => " << valeurs[i] << "|";
    cout << endl;
}

void affiche_index(const indexation& index){
    int i=1;
    vector<pair<list<clause*>, int> >::const_iterator it;
    
    cout << "   Positif :" << endl;
    for(it=index.positif.begin();it != index.positif.end();it++){
        cout << "       Var : " << i << " in " << (*it).second << " clause : " << list2string((*it).first) << endl;
        i++;
    }
    
    i=1;
    cout << "   Negatif :" << endl;
    for(it=index.negatif.begin();it != index.negatif.end();it++){
        cout << "       Var : " << -i << " in " << (*it).second << " clause : " << list2string((*it).first) << endl;
        i++;
    }
}

extern string list2string(const list<clause*>& l){
    list<clause*>::const_iterator lit = l.begin();
    ostringstream out;
    
    while(lit != l.end()){
        out << "(" << clause2string(*lit) << ") ";
        lit++;
    }
    return out.str();
}

string map2string (const map<string,int>& correspondance){
    map<string,int>::const_iterator it;
    ostringstream out;
    
    for(it=correspondance.begin();it!=correspondance.end();it++)
        out << "    " << it->first << " => " << it->second << endl;
    
    return out.str();
}

string clause2string(const clause * c){
    vector<int>::const_iterator lit = c->second.begin();
    ostringstream out;
    //out << c->first << " | ";
    while(lit != c->second.end()){
        out << *lit;
        lit++;
        if(lit != c->second.end())
            out << " \\/ ";
    }
    return out.str();
}

string formule2string(const formule_conj * f) {
    formule_conj::const_iterator lit = f->begin();
    ostringstream out;
    while (lit != f->end()){
        out << "( " << clause2string(*lit);
        lit++;
        if(lit != f->end())
            out << " ) /\\ ";
        else
            out << " )";
    }
    return out.str();
}

extern string interpretation2string(const interpretation& inter){
    interpretation::const_iterator it; 
    ostringstream out;
    int i=1;
    for (it=inter.begin(); it!=inter.end(); it++) {
        out << i << ":" << *it << " | ";
        i++;
    }
    return out.str();
}

/*-------------------------------------------------------------------------------------------------------*/
/*                                  Numérotation des variables                                           */
/*-------------------------------------------------------------------------------------------------------*/

void numerote(const formule* f,map<string,int>& correspondance) {
    switch (f->op) {
        case o_variable:
            correspondance.insert(pair<string,int>(*(f->nom),(int)correspondance.size() + 1));
            break;
        case o_non:
            numerote(f->arg,correspondance);
            break;
        default:
            numerote(f->arg1,correspondance);
            numerote(f->arg2,correspondance);
            break;
    }
}

/*-------------------------------------------------------------------------------------------------------*/
/*                         Fonctions de création de formule conjonctives                                 */
/*-------------------------------------------------------------------------------------------------------*/

clause * ou(int arg1, clause * arg2){
    arg2->second.push_back(arg1);
    return arg2;
}

clause * ou(clause * arg1,clause * arg2){
    vector<int>::const_iterator it;
    for(it = arg2->second.begin(); it != arg2->second.end() ;it++)
        arg1->second.push_back(*it);
    return arg1;
}

clause * ou(int valeur){
    clause * res = new clause;
    res = ou(valeur,res);
    return res;
}

clause * ou(int val1, int val2){
    return ou(val1,ou(val2));
}

clause * ou(int val1, int val2, int val3){
    return ou(val1 , ou(val2, ou(val3)));
}

formule_conj * et(formule_conj * arg1, formule_conj * arg2){
    formule_conj::const_iterator it;
    for(it=arg2->begin(); it != arg2->end() ;it++)
        arg1->push_back(*it);
    
    delete(arg2);
    return arg1;
}

formule_conj * et(clause *  arg1, formule_conj * arg2){
    arg2->push_back(arg1);
    return arg2;
}

/*-------------------------------------------------------------------------------------------------------*/
/*                                Traduction en forme conjonctive                                        */
/*-------------------------------------------------------------------------------------------------------*/

triplet * forme_conjonctive(const formule * f,const map<string,int>& correspondance){
    return forme_conjonctive(f,correspondance,(int)correspondance.size() + 1);
}

triplet * forme_conjonctive(const formule * f,const map<string,int>& correspondance, int suivant){
    triplet * res= new triplet();
    triplet *B_prime,*B_second;
    int p,p_prime,p_second;
    formule_conj * C_prime,* C_second;
    
    switch (f->op) {
        case o_variable:
            res->forme_conj=new formule_conj();
            res->var_repr=correspondance.find(*(f->nom))->second;
            res->suivant=suivant;
            break;
            
        case o_non:
            B_prime = forme_conjonctive(f->arg,correspondance,suivant);
            p=B_prime->suivant;
            p_prime=B_prime->var_repr;
            C_prime=B_prime->forme_conj;
            
            res->forme_conj = et(ou(-p, -p_prime) , et(ou(p,p_prime), C_prime));
            
            res->var_repr=p;
            res->suivant=p+1;
            delete(B_prime);
            break;
            
        case o_ou:
            B_prime = forme_conjonctive(f->arg1,correspondance,suivant);
            B_second = forme_conjonctive(f->arg2,correspondance,B_prime->suivant);
            
            p = B_second->suivant;
            p_prime = B_prime->var_repr;
            p_second = B_second->var_repr;
            C_prime = B_prime->forme_conj;
            C_second = B_second->forme_conj;
            
            res->forme_conj = et(ou(-p,p_prime,p_second), et(ou(p,-p_prime),et(ou(p,-p_second),et(C_prime,C_second))));
            res->var_repr = p;
            res->suivant= p+1;
            
            delete(B_prime);
            delete(B_second);
            break;
            
        case o_et:
            B_prime = forme_conjonctive(f->arg1,correspondance,suivant);
            B_second = forme_conjonctive(f->arg2,correspondance,B_prime->suivant);
            
            p = B_second->suivant;
            p_prime = B_prime->var_repr;
            p_second = B_second->var_repr;
            C_prime = B_prime->forme_conj;
            C_second = B_second->forme_conj;
            
            res->forme_conj = et(ou(p,-p_prime,-p_second), et(ou(-p,p_prime),et(ou(-p,p_second),et(C_prime,C_second))));
            res->var_repr = p;
            res->suivant= p+1;
            
            delete(B_prime);
            delete(B_second);
            break;
            
        case o_implique:
            B_prime = forme_conjonctive(f->arg1,correspondance,suivant);
            B_second = forme_conjonctive(f->arg2,correspondance,B_prime->suivant);
            
            p = B_second->suivant;
            p_prime = B_prime->var_repr;
            p_second = B_second->var_repr;
            C_prime = B_prime->forme_conj;
            C_second = B_second->forme_conj;
            
            res->forme_conj = et(ou(-p,-p_prime,p_second),et(ou(p,p_prime),et(ou(p,-p_second),et(C_prime,C_second))));
            res->var_repr = p;
            res->suivant= p+1;
            
            delete(B_prime);
            delete(B_second);
            break;
            
        case o_equivaut:
            B_prime = forme_conjonctive(f->arg1,correspondance,suivant);
            B_second = forme_conjonctive(f->arg2,correspondance,B_prime->suivant);
            
            p = B_second->suivant;
            p_prime = B_prime->var_repr;
            p_second = B_second->var_repr;
            C_prime = B_prime->forme_conj;
            C_second = B_second->forme_conj;
            
            res->forme_conj = et(ou(-p,-p_prime,p_second),et(ou(-p,p_prime,-p_second),et(ou(p,p_prime,p_second),et(ou(p,-p_prime,-p_second),et(C_prime,C_second)))));
            res->var_repr = p;
            res->suivant= p+1;
            
            delete(B_prime);
            delete(B_second);
            break;
            
        default:
            break;
    }
    return res;
}

/*-------------------------------------------------------------------------------------------------------*/
/*                                   Indexation des clauses                                              */
/*-------------------------------------------------------------------------------------------------------*/

indexation indexeClauses(const formule_conj *f,int nb_variable){
    
    indexation index;
    index.positif.resize(nb_variable);
    index.negatif.resize(nb_variable);
    
    formule_conj::const_iterator it_fc;
    vector<int>::const_iterator it_c;
    
    for (it_fc=f->begin(); it_fc != f->end(); it_fc++) {
        for (it_c=(*it_fc)->second.begin(); it_c!=(*it_fc)->second.end(); it_c++) {
            if (*it_c>0) {
                (index.positif)[(*it_c)-1].first.push_back(*it_fc);
                (index.positif)[(*it_c)-1].second++;
            }
            else {
                (index.negatif)[(-(*it_c))-1].first.push_back(*it_fc);
                (index.negatif)[(-(*it_c))-1].second++;
            }
        }
    }
    
    return index;
    
}

/*-------------------------------------------------------------------------------------------------------*/
/*                                    Propagation Unaire                                                 */
/*-------------------------------------------------------------------------------------------------------*/

bool cherche(formule_conj * fc,int var,interpretation& valeurs,indexation& index){
    list<int> tmp;
    vector<clause*> clause_deduite;
    list<int>::const_iterator it;
    vector<clause*>::const_iterator it_c;
    
    if(var<1)
        return true;
    
    //La valeurs de var peut avoir déjà été affectée par la propagation. Dans ce cas il n'est pas nécéssaire de "l'évaluer" à nouveau.
    
    if (valeurs[var-1]==0) {
        
        valeurs[var-1]=1;
        if(!propage(var,valeurs,index,fc,tmp,clause_deduite))
            if(cherche(fc,var-1,valeurs,index))
                return true;
        
        //Remise à zéro des variables propagées
        for(it=tmp.begin();it != tmp.end();it++)
            valeurs[*it -1]=0;
        tmp.clear();
        
        //Remise à faux des clauses déduite
        for (it_c=clause_deduite.begin(); it_c!=clause_deduite.end(); it_c++) {
            backtrack_monotone(index, *it_c);
            (*it_c)->first=false;
        }
        clause_deduite.clear();
        
        valeurs[var-1]=-1;
        if(!propage(var,valeurs,index,fc,tmp,clause_deduite))
            if(cherche(fc,var-1,valeurs,index))
                return true;
        
        //Remise à zéro des variables propagées
        valeurs[var-1]=0;
        for(it=tmp.begin();it != tmp.end();it++)
            valeurs[*it -1]=0;
        
        //Remise à faux des clauses déduite
        for (it_c=clause_deduite.begin(); it_c!=clause_deduite.end(); it_c++) {
            backtrack_monotone(index, *it_c);
            (*it_c)->first=false;
        }
        
        return false;
    }
    else
        return cherche(fc, var-1, valeurs, index);
}

extern int clause_est_satisfaite(clause * c,interpretation& valeurs,int& litteral){
    int tmp,res=-1;
    bool i=true;
    vector<int>::const_iterator lit;
    
    litteral=0;
    
    for(lit=c->second.begin();lit != c->second.end();lit++){
        tmp=valeurs[abs(*lit)-1]*(*lit);
        
        if(tmp>0)
            return 1;
        if(tmp==0){
            if (i) {
                litteral=(*lit);
                i=false;
            }
            else
                litteral=0;
            res=0;
        }
    }
    return res;
}

//Propage retourne vrai si l'indexation renvoie UNSAT sur la formule. Sinon elle renvoie vrai.
bool propage(int var, interpretation& valeurs, indexation& index, formule_conj * fc , list<int>& deduite, vector<clause*>& clause_deduite){
    list<clause*> * tmp; //Pointeur pour éviter une copie qui peut être lourde quand la clause est grande.
    int litteral=0;
    
    if(valeurs[var-1]==1)
        tmp = &((index.negatif)[var-1].first);
    else
        tmp = &((index.positif)[var-1].first);
    
    list<clause*>::const_iterator it;
    for(it=tmp->begin();it!=tmp->end();it++){
        if (!(*it)->first) {
            switch (clause_est_satisfaite(*it, valeurs, litteral)) {
                case -1:
                    return true;
                    break;
                    
                case 1 :
                    monotone(valeurs, index, *it, deduite, clause_deduite);
                    break;
                    
                default:
                    if(litteral!=0){
                        //cout << var << " implique " << litteral << " dans " << clause2string(*it) << endl;
                        deduite.push_back(abs(litteral));
                        if(litteral>0){
                            valeurs[abs(litteral)-1]=1;
                        }
                        else {
                            valeurs[abs(litteral)-1]=-1;
                        }
                        if (propage(abs(litteral), valeurs, index, fc, deduite, clause_deduite)) {
                            return true;
                        }
                    }
                    break;
            }
        }
    }
    return false;
}

/*-------------------------------------------------------------------------------------------------------*/
/*                                        Variables monotones                                            */
/*-------------------------------------------------------------------------------------------------------*/

void monotone(interpretation& valeurs, indexation& index, clause* c,list<int>& deduite, vector<clause*>& clause_deduite){
    c->first = true;
    clause_deduite.push_back(c);
    vector<int>::const_iterator it;
    for (it=c->second.begin(); it != c->second.end(); it++) {
        if (*it<0) {
            if(--index.negatif[-(*it)-1].second==0 && valeurs[-(*it)-1]==0){//On vient de trouver une variable monotone
                valeurs[-(*it)-1]=1; //On mes à vrai puisqu'il n'y a plus de non var.
                deduite.push_back(-(*it));
                list<clause*>::const_iterator it_c;
                for (it_c=index.positif[-(*it) -1].first.begin(); it_c!=index.positif[-(*it) -1].first.end(); it_c++) {
                    if (!(*it_c)->first) {
                        monotone(valeurs, index, *it_c, deduite, clause_deduite);
                    }
                }
            }
        }
        else {
            if(--index.positif[*it-1].second==0 && valeurs[(*it)-1]==0){//
                valeurs[(*it)-1]=-1; //On mes à faux puisqu'il n'y a plus de var.
                deduite.push_back(*it);
                list<clause*>::const_iterator it_c;
                for (it_c=index.negatif[(*it) -1].first.begin(); it_c!=index.negatif[(*it) -1].first.end(); it_c++) {
                    if (!(*it_c)->first) {
                        monotone(valeurs, index, *it_c, deduite, clause_deduite);
                    }
                }
            }
        }
    }
}

void backtrack_monotone(indexation& index, clause* c){
    vector<int>::const_iterator it;
    for (it=c->second.begin(); it != c->second.end(); it++) {
        if (*it<0) {
            index.negatif[-(*it)-1].second++;
        }
        else {
            index.positif[*it-1].second++;
        }
    }
}


