#ifndef ALEXICOGH
#define ALEXICOGH

#include <iostream>
#include <fstream>
#include "Ttoken.h"

class ALexicoG {
	private:
	istream * fichero;  //Descriptor del fichero de la gramatica
	int nLineas;    //Linea actual del fichero

	char car_leido; //Ultimo caracter leido del fichero

	//Lee un caracter o devuelve nulo en caso de llegar al final de fichero
	char Leer_Caracter();
	//Comprueba si el caracter es una letra o un numero
	bool EsAlfaNumerico (char caracter);
	//Comprueba si el caracacter es un delimitador (espacio en blanco tabulador)
	bool EsDelimitador (char caracter);
	//Comprueba si el caracter es un separador
	bool EsSeparador (char caracter);
	//Lee del fichero hasta encontrar un separador y lo concatena con el string que le pasan
	string Leer_Simbolo (string CaracteresIni);
	//Concatena el caracter con el string que le pasan
    string concatenar_Simbolo (string CaracteresIni, char caracter);

	public:
	//Constructor, recibe el fichero sobre el que debe extraer los tokens
	ALexicoG (istream * fich);
	//Salta de linea en el fichero
	void saltar_linea();
    //Devuelve el token que ha leido
	TokenGramatica DameToken ();

	int GetLinea();
};
#endif
