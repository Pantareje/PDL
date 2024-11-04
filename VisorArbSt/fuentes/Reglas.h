#ifndef REGLASH
#define REGLASH
#include <vector>
#include <string>
using namespace std;
class Reglas {
    private:
        //Numero que identifica a la regla
        int numeroRegla;
        //Elemento que forma el antecedente de la regla
        string Antecedente;
        // Colección con todos los elementos que forman el consecuente
        vector<string> ElemsTerminal;
    public:
        Reglas (int numero, string ante);
        //Introduce el elemento en el consecuente de la regla
        void introducirElemConsec (string elemento);
        //Introduce el antecedenete en la regla
        void putAnte(string ante);
        //Devuelve el antecedente
        string getAnte();
        //Devuelve el consecuente
        vector<string> getConsec();

    };
#endif
