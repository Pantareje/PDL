#include "Simbolos.h"

Simbolos :: Simbolos(){
    Axioma = "";
}
//Constructor para el axioma
Simbolos :: Simbolos (string axiom){
    Axioma = axiom;
}
//Constructor para los terminales y no Terminales
Simbolos :: Simbolos (vector<string> ListaSimb){
    ListaSimbolos= ListaSimb;
}
// Introduce la lista de simbolos en el vector
void Simbolos :: putListaSimbolos (vector<string> ListaSimb){
    ListaSimbolos= ListaSimb;
}
// Devuelve el axioma del simbolo
string Simbolos :: extraerAxioma(){
    return Axioma;
}
// Devuelve si el elemento se encuentra dentro de la lista de simbolos
bool Simbolos :: buscarSimb(string simb){
    bool encontrado=false;
    unsigned int i=0;
    while (i < ListaSimbolos.size() && !encontrado){
        encontrado = (ListaSimbolos.at(i) == simb);
        i++;
    }
    return encontrado;
}

