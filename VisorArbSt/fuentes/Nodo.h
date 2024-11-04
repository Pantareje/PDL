#ifndef NODOH
#define NODOH
#include <string>
#include <vector>
#include <list>

using namespace std;
class Nodo{
    private:
        //simbolo que muestra el nodo
        string simbolo;
        // Indica si es un Terminal
        bool esTerminal;
        //Indica si es visible

        bool elemDerivable;
        // Indica la regla por la cual derivará, si no puede será un 0
        int reglaDeriv;
        vector < list<Nodo> ::iterator> hijos;

    public:
        Nodo (string simb,bool term, bool derivable );
        Nodo();
        Nodo (string simb,bool term);
        //Devuelve el simbolo del nodo
        string getSimbolo();
        //Comprueba si el nodo es derivable
        bool esDerivable();
        //Inserta un hijo en la colección de hijos
        void insertarHijo(list<Nodo>::iterator hijo);
        //Inserta la regla por la cual derivará
        void insertarReglaDeriv(int numRegla);
        // Devuelve la regla por la que derivará
        int dameReglaDeriv ();
        // El nodo deja de ser derivable
        void quitarDerivable();
        // Comprueba si el nodo tiene hijos
        bool tieneHijos();
        // Devuelve la colección de hijos
        vector < list<Nodo> ::iterator> dameHijos ();

};


#endif
