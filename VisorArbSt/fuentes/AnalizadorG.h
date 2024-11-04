#ifndef ANALIZADORGH
#define ANALIZADORGH

#include "ALexicoG.h"
#include "Reglas.h"
#include "AnalizadorP.h"
using namespace std;

class AnalizadorG {
    private:
        //Numero de errores en la gramatica
        int error;
        //Ultimo token leido
        TokenGramatica TLeido;
        // Analizador Lexico
        ALexicoG * Lex;
        //Coleccion de elementos que forman los simbolos Terminales
        vector<string> ElemsTerminal;
        //Coleccion de elementos que forman los simbolos NoTerminales
        vector<string> ElemsNoTerminal;
        //Coleccion de reglas de la gramatica
        vector<Reglas> ListReglas;
        //Colección donde alamacenar los errores encontrados
        vector<string> * ListaError;
        //Elemento que es el axioma
        string SimbAxioma;
        //Numero de reglas de la gramatica
        int nReglas;
        //Flags que indican que se esta declarando conjuntos o que nos encontramos con el antecedente de la regla
        int decConjuntos;
        int antecedente;


    void TERM();
    void AXIOM();
    void INI ();
    void SIGCONJUNTO();
    void SIGCONJUNTO2();
    void SIGCONJUNTO3();
    void FUERACONJ ();
    void CONJUNTOS();
    void NOTERM();
    void FINFICHERO();
    void IDTERM();
    void CONJTERMINI();
    void CONJTERM();
    void IDNOTERM();
    void NOTERMINI();
    void CONJNOTERM();
    void PROD();
    void PRODUCC();
    void REGLA();
    void CONSEC();
    void SIGCONSEC();
    //funciones que insertan los errores en el formato adecuado
    void InsertarError(const char * textoError, int linea);
    void InsertarError(string textoError, int linea);
    //Comprueba si elem esta en la colección de Terminales
    bool esTerminal(string elem);
    //Comprueba si elem esta en la colección de NoTerminales
    bool esNoTerminal(string elem);


    public:
    //Constructor del método
    AnalizadorG(vector<string> * ListaErrores);
    // Analiza el fichero de la gramatica y devuelve si es correcto o no
    bool comprobarFormato(istream * fichero);
    //Devuelve la colección de terminales
    vector<string> GetTerminal();
    //Devuelve la conlección de no Terminales
    vector<string> GetNoTerminal();
    //Devuelve el axioma
    string GetAxioma();
    //Devuelve la referencia a la lista de reglas
    vector<Reglas>* GetListReglas();
    //Devuelve el número total de reglas de la gramática
    int getReglas();
    //Destructor
    ~AnalizadorG();
};


#endif
