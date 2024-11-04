#include "AnalizadorP.h"

using namespace std;
//Constructor, carga el numero de reglas, inicializa el numero de errores y recibe la listadonde almacenar los error
AnalizadorP :: AnalizadorP ( int maxReglas,vector<string> * ListaErrores){
    nmax = maxReglas;
    nerrores = 0;
    ListaError = ListaErrores;
}

//Devuelve el modo en el que será construido el parse
char AnalizadorP :: getModo (){
    return modo;
}
//Devuelve si se ha encontrado algún error
bool AnalizadorP :: getNerrores(){
    return nerrores;
}

//Lee un caracter o devuelve nulo en caso de llegar al final de fichero
char AnalizadorP :: Leer_Caracter() {
	char car;
	if (ficheroP->eof())
		 return '\0';
	else {
        	ficheroP->get(car);
		if (ficheroP->eof())
			return '\0';
		else
       			return car;
	     }
}
//Comprueba si el caracter es un digito
bool AnalizadorP :: Es_numero (char caracter) {
    return (caracter >= '0' && caracter <= '9');
}

//Analiza el parse que contiene el archivo cuyo descriptor se recibe como parámetro
vector<int> AnalizadorP :: analizarParse (istream * fich){
    ficheroP = fich;
    string errorEncontrado;
    char carAux;
    char carAux2[3];
    char buffer [5];
    int numAux;
    carAux = Leer_Caracter();
    //Lee delimitadores que pueda haber inicialmente

    while (carAux == '\n' || carAux == '\r'|| carAux== ' ' or carAux == '\t'){
        carAux = Leer_Caracter();
    }
    //Comprueba que el primer caracter no delimitador es una A o D
    if (carAux== 'A' ){
        modo ='A';
    }
    else if(carAux== 'D'){
        modo ='D';
    }
    else {  //Encontrado algo que no es una A o D
        nerrores++;
        errorEncontrado ="El fichero del parse debe empezar por A o D";
        ListaError->push_back (errorEncontrado);
    }
    //Leemos todas las letras de la posible  palabra hasta encontrar un delimitador
    carAux = Leer_Caracter();
    while (!(carAux == '\n' || carAux == '\r'||  carAux == '\0' ||carAux== ' ' or carAux == '\t')){
        carAux = Leer_Caracter();
    }
    //Leemos todos los delimitadores hasta el primer número

    while (carAux == '\n' || carAux == '\r'|| carAux== ' ' or carAux == '\t'){
        carAux = Leer_Caracter();
    }
    //Comprobamos que no llega un final de fichero antes del parse
    if (carAux == '\0'){
        nerrores++;
        errorEncontrado ="final de fichero inesperado, se esperaba el parse";
        ListaError->push_back (errorEncontrado);
    }
    //Se empizan a Leer numeros

    while (carAux != '\0'){
        numAux=0;
        carAux2[2]= '\0';
        while (Es_numero(carAux)){//Almacena el numero del parse
            carAux2[0]= carAux;
            carAux2[1]='\0';
            numAux = numAux * 10;
            numAux = numAux + atoi (carAux2);
            carAux = Leer_Caracter();
        }
        if (numAux >nmax){
            nerrores++;
            sprintf(buffer,"%d",numAux);
            errorEncontrado = "No existe la regla numero ";
            errorEncontrado =errorEncontrado+buffer;
            ListaError->push_back (errorEncontrado);
        }
        else {
            listaParse.push_back(numAux);   //Insertamos el número de regla
        }
        //Comprobamos que si no es un nuemro lo que se encuentra es un delimitador
        while (!(carAux == '\n' || carAux == '\r'|| carAux== ' ' or carAux == '\t')){
            nerrores++;
            errorEncontrado ="Una vez se ha escrito la primera regla del parse solo se admiten números y delimitadores";
            ListaError->push_back (errorEncontrado);
            carAux = Leer_Caracter();
        }
        while ((carAux == '\n' || carAux == '\r'|| carAux== ' ' or carAux == '\t')){
            carAux = Leer_Caracter();
        }

    }

return listaParse;
}
