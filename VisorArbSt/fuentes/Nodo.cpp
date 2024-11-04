#include "Nodo.h"
//Constructor cuando se trata de un simbolo terminal
  Nodo::Nodo (string simb,bool term){
    simbolo=simb;
    esTerminal = term;
    elemDerivable = false;
}

Nodo:: Nodo (){
    esTerminal = true;
    elemDerivable = false;
}
//Constructor cuando se trata de un simbolo terminal
Nodo:: Nodo (string simb,bool term,bool derivable){
    simbolo=simb;
    esTerminal = term;
    elemDerivable = derivable;
}

//Inserta el parametro en su colección de hijos
void Nodo ::insertarHijo(list<Nodo>::iterator hijo){

    hijos.push_back(hijo);
}

//Quita la derivavilidad al nodo
void Nodo :: quitarDerivable(){
    elemDerivable = false;
}
//Devuelve el simbolo del nodo
string Nodo:: getSimbolo(){
    return simbolo;
}
//Comprueba si esDerivable
bool Nodo::esDerivable(){
    return elemDerivable;
}
// Comprueba si tiene hijos
bool Nodo:: tieneHijos(){
    return hijos.size();
}
//Inserta la regla por la que derivará
void Nodo ::insertarReglaDeriv(int numRegla){
    reglaDeriv = numRegla;
}
//Devuelve la regla por la uqe derivará
int Nodo::dameReglaDeriv(){
    return reglaDeriv;
}


vector < list<Nodo> ::iterator> Nodo:: dameHijos (){
    return hijos;
}

