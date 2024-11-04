#ifndef ANALIZADORPH
#define ANALIZADORPH

#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>


using namespace std;

class AnalizadorP {
    private:
    //Modo del parse
    char modo;
    //Lista de reglas que se aplican
    vector<int> listaParse;
    //Numero maximo de reglas de la gramatica y errores encontrados
    int nmax,nerrores;
    //Descriptor del fichero del parse
    istream *ficheroP;
    //Lee el siguiente caracter
    char Leer_Caracter();
    //Comprueba si el caracter es un digito
    bool  Es_numero (char caracter);
    //Lista donde se almacena la descripción del error encontrado
    vector<string> * ListaError;

    public:
    //Constructor, recibe el número de reglas de la gramática y la referencia a la lista donde almacenar los errores
    AnalizadorP (int maxReglas,vector<string> * ListaErrores);
    //Devuelve el modo (A=Ascendente, D= Descendente)
    char getModo ();
    //Devuelve si ha habido o no, errores
    bool getNerrores();
    //Analiza el parse que contiene el archivo cuyo descriptor se recibe como parámetro
    vector<int> analizarParse (istream * fich);
};
#endif
