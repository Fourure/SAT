#include <new>
#include <iostream>
#include <fstream>
#include <string>
#include "Parsers.hpp"

using namespace std;

namespace FormuleParser {
    
    struct parse_result {
        formule * f;
        int idx;
    };
    
    extern int skipSpaces (string input, int from);
    extern parse_result read_var(string input, int from);
    extern parse_result read_lvl(string input, int from, int lvl);
    extern formule * parseString(string input, int from);
    
    int skipSpaces (string input, int from) {
        bool notfound = true;
        while (from < (int)input.length() && notfound) {
            switch (input.at(from)) {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    from ++;
                    break;
                default:
                    notfound = false;
            }
        }
        return from;
    }
    
    parse_result read_var(string input, int from) {
        int to = from;
        while (to < (int)input.length()
               && ( ('a' <= input.at(to) && 'z' >= input.at(to))
                   || ('A' <= input.at(to) && 'Z' >= input.at(to))
                   || ('0' <= input.at(to) && '9' >= input.at(to))
                   || input.at(to) == '_' ) ) {
                   to++;
               }
        parse_result res;
        res.f = new formule();
        res.f->op = o_variable;
        res.f->nom = new string(input,from,to-from);
        res.idx = to;
        return res;
    }
    
    parse_result read_lvl(string input, int from, int lvl) {
        from = skipSpaces(input,from);
        if (from >= (int)input.length()) {
            throw "End of input reached before the parsing finished";
        }
        if (lvl <= 0) { // parens or variable
            if (input[from] == '(') {
                parse_result pr = read_lvl(input,from+1,START_LVL);
                pr.idx = skipSpaces(input, pr.idx);
                if (pr.idx >= (int)input.length()) {
                    throw "End of input reached before the parsing finished";
                } else if (input[pr.idx] != ')') {
                    throw "'(' at character "+pr.idx;
                }
                pr.idx = pr.idx+1;
                return pr;
            } else {
                return read_var(input, from);
            }
        } else {
            operateur op = operateur_for_level(lvl);
            string s_op = operateur2string(op);
            if (is_binary(op)) {
                parse_result pr1 = read_lvl(input,from,lvl-1);
                pr1.idx = skipSpaces(input,pr1.idx);
                if ( input.compare(pr1.idx, s_op.length(), s_op) == 0 ) {
                    parse_result pr2 = read_lvl(input,pr1.idx+(int)s_op.length(), lvl);
                    parse_result res;
                    res.f = new formule();
                    res.f -> op = op;
                    res.f -> arg1 = pr1.f;
                    res.f -> arg2 = pr2.f;
                    res.idx = pr2.idx;
                    return res;
                } else {
                    return pr1;
                }
            } else {
                if ( input.compare(from, s_op.length(), s_op) == 0 )  {
                    parse_result pr = read_lvl(input,from + (int)s_op.length(),lvl);
                    parse_result res;
                    res.idx = pr.idx;
                    res.f = new formule();
                    res.f->op = op;
                    res.f->arg = pr.f;
                    return res;
                } else {
                    return read_lvl(input,from,lvl-1);
                }
            }
        }
    }
    
    formule * parseString(string input, int from) {
        return read_lvl(input,from,START_LVL).f;
    }
    
}


formule * litFormuleString(string input) {
    return FormuleParser::parseString(input,0);
}

formule * litFormuleFichier(const char * filename) {
    ifstream file (filename);
    string line;
    string to_read;
    if (file.is_open()) {
        while ( file.good() ) {
            getline(file,line);
            to_read.append(line);
            to_read.append("\n");
        }
        file.close();
        return litFormuleString(to_read);
    } else {
        return NULL;
    }
}
