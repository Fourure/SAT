#ifndef Parsers
#define Parsers
#include "Formule.hpp"
#include <string>

extern formule* litFormuleString(string input);
extern formule* litFormuleFichier(const char * filename);

#endif
