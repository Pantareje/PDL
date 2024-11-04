#include "ListaReglas.h"

//Constructor de la liste de reglas, recibe la lista de parse
ListaReglas :: ListaReglas (vector<int> lista){
    ListaParse = lista;
}
//Devuelve la lista del parse
vector<int> ListaReglas :: dameLista(){
    return ListaParse;
}
//Devuelve la lista del parse invertida
vector<int> ListaReglas ::dameListaInvertida(){
    vector<int> ListaAux;
    int i;
    for (i=ListaParse.size()-1;i>=0;i--){
        ListaAux.push_back(ListaParse.at(i));
    }
    return ListaAux;
}


