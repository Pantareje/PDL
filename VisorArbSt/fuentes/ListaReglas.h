#ifndef LISTAREGLASH
#define LISTAREGLASH
#include <vector>
using namespace std;
class ListaReglas{
    private:
        //Coleccion con el orden de aplicaciond de las reglas
        vector<int> ListaParse;
    public:
        //Constructor, almacena la lista que le pasan
        ListaReglas (vector<int> lista);
        //Devuelve la lista del parse
        vector<int> dameLista();
        //Devuelve la lista del parse invertida
        vector<int> dameListaInvertida();



};
#endif
