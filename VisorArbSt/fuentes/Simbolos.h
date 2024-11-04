#ifndef SIMBOLOSH
#define SIMBOLOSH
#include <vector>
#include <string>

using namespace std;
class Simbolos {
    private:
        //String que representa al axioma en caso de que el simbolo fuese el axioma
        string Axioma;
        //Coleccion de string con todo los simbolos de un mismo tipo
        vector<string> ListaSimbolos;
    public:
    void putListaSimbolos (vector<string> ListaSimb);
    Simbolos();
    Simbolos (string axiom);
    Simbolos (vector<string> ListaSimb);
    string extraerAxioma();
    bool buscarSimb(string simb);
};
#endif
