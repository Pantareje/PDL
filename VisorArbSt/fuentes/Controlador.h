#ifndef CONTROLADORH
#define CONTROLADORH


#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>


#include "AnalizadorP.h"
#include "Reglas.h"
#include "AnalizadorG.h"
#include "Simbolos.h"
#include "Nodo.h"
#include "ListaReglas.h"

using namespace std;



class Controlador{
    private:
        //Objeto con los simbolos Terminales
        Simbolos *Terminal;
        //Objeto con los simbolos NoTerminales
        Simbolos *NoTerminal;
        //Objeto con el Axioma
        Simbolos *Axioma;
        //Texto con el simbolo
        string simb;
        bool ParseCargado, gramaticaCargada, errorArbol;
        bool estructParse;
        // Analizador para la gramatica
        AnalizadorG * GAnalizador;
        // Analizador para el parse
        AnalizadorP * ParseAnalizador;
        // Objeto que almacena el orden de las reglas a aplicar
        ListaReglas * ListaParse;
        // Colección con las diferentes reglas de la graamática
        vector<Reglas> *ListaDeReglas;
        //Colección con los errores que surjan
        vector<string> *ListaErrores;
        //Lista de Nodos
        list<Nodo> ListaNodos;


        //almacena el numero de elementos derivables que quedan en el arbol
        int elementosDerivables;

        char modo;
        //vector con el parse
        vector<int> Lreg;
        //vector con el parse que se ha utilizado
        vector<int> Lregaplicadas;
        // construye el árbol cuando es descendente
        void ConstruirArbolDescendente();
        // Construye el árbol cuando es ascendente
        void ConstruirArbolAscendente();
        //Inserta los nodos hijos que derivan de un nodo padre
        void  insertarDerivacion( vector<string> elemRegla,list<Nodo>:: iterator *j,list<Nodo>:: iterator w,int indiceRegla);


    public:
        Controlador();
        //Carga la gramatica y devuelve si es correcta
        //PRE: No existe ninguna gramática cargada
        bool CargarGramatica(char *fichero);

        //Carga el parse y devuelve si es correcto
        //PRE: Existe una gramatica cargada
        bool CargarParse(char *fichero);
        //Construye el árbol y devuelve si es correcto
        //PRE: Existen una gramatica y parse cargados
        bool ConstruirArbol ();
        //Cambia la gramatica actual
        //PRE: Existe una gramatica cargada actualmente
        //POST: Destruye los posibles arboles y parse cargados
        bool CambiarGramatica(char *fichero);
        //Cambia el parse
        //PRE: Existe un parse actualmente cargado
        //POST: Destruye el posibles arbol  cargado
        bool CambiarParse(char *fichero);
        //Elimina el arbol
        //PRE: Debe existir el árbol, el parse y la gramatica
        // POST: Inicializa el sistema
        void EliminarArbol();
        //Devuelve la lista de nodos
        list<Nodo> DameLista();
        //Devuelve la lista de errores
        vector<string>* DameListaErrores();
        ~Controlador();
      //  void  Auxmio(string ye, Nodo& nodoPadre,list<Nodo>::iterator j );

};

#endif
