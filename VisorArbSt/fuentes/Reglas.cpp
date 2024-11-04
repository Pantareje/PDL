#include "Reglas.h"

// Constructor, recibe el nuemro de regla y el antecedente de la regla
Reglas :: Reglas (int numero, string ante){
    numeroRegla = numero;
    Antecedente = ante;
}


//Añade un elemento a la lista de elementos del consecuente
void Reglas :: introducirElemConsec (string elemento){
    ElemsTerminal.push_back(elemento);
}
//Carga el elemento que forma el antecedente de la regla
void Reglas ::putAnte(string ante){
    Antecedente = ante;
}

//Devuelve el antecedente de la regla
string Reglas:: getAnte(){
    return Antecedente;
}
//Devuelve el vector que contiene todo los elementos del consecuente de la regla
vector<string> Reglas:: getConsec(){
    return ElemsTerminal;
}
