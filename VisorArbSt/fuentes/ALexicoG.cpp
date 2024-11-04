#include <iostream>

#include "ALexicoG.h"
//#include "Ttoken.h"

using namespace std;


//Constructor
ALexicoG :: ALexicoG (istream * fich){
//Guarda la referencia al fichero, inicia el contador de líneas y lee el primer caracter
	fichero = fich;
	nLineas = 1;
	car_leido = Leer_Caracter();
}


//PRE: El fichero ya ha sido abierto
char ALexicoG :: Leer_Caracter() {
//Lee un caracter o devuelve nulo en caso de llegar al final de fichero
	char car;
	if (fichero->eof())
		 return '\0';
	else {
        	fichero->get(car);
		if (fichero->eof())
			return '\0';
		else
       			return car;
	     }
}

// Comprueba si el caracter es un digito
bool ALexicoG :: EsAlfaNumerico (char caracter) {
	char car_aux = tolower (caracter);

	if (caracter == -61){//Detecta la ñ, lee un caracter mas ya que la ñ son dos caracteres
	    car_leido = Leer_Caracter();

        return true;
    }

	return ((caracter >= '0' && caracter <= '9') or (car_aux >= 'a' && car_aux <= 'z'));
}

// Comprueba si el caracter es un delimitador
// Delimitadores Posibles : espacio en blanco y tabulador
bool ALexicoG :: EsDelimitador (char caracter){
	return (caracter== ' ' or caracter == '\t' or caracter == '\r');
}

bool ALexicoG :: EsSeparador (char caracter) {
    return (caracter == '\n' || caracter == '\r'|| caracter == '\0' || EsDelimitador(caracter));
}

// Devuelve el numero de linea
int ALexicoG :: GetLinea(){
	return nLineas;
}

// Recibe los caracteres iniciales del simbolo y sigue concatenando hasta que llegue un delimitador
string ALexicoG :: Leer_Simbolo (string CaracteresIni) {
   	string simbolo = CaracteresIni;

    while (! EsSeparador(car_leido)) {
        simbolo = concatenar_Simbolo(simbolo, car_leido);
        //simbolo = simbolo + car_leido;
        car_leido = Leer_Caracter();
    }   // Se leen todos los dígitos, letras y símbolos hasta un separador

    return simbolo;
}
//Concatena el caracter con el string que le pasan
string ALexicoG ::concatenar_Simbolo (string CaracteresIni, char caracter){
    string simbolo = CaracteresIni;

    if (caracter == -61){
        simbolo = simbolo + "ñ";
        car_leido = Leer_Caracter();// Tiro el siguiente caracter que lo corrompe la ñ
    }
    else {
        simbolo = simbolo + caracter;
    }
    return simbolo;
}
//Salta de linea en el fichero
void ALexicoG :: saltar_linea(){
    while (!(car_leido =='\n') or (car_leido =='\0')){
    car_leido = Leer_Caracter();
    }
}




//Retorna el token leido
TokenGramatica ALexicoG :: DameToken () {

	TokenGramatica  token;

	string simbolo;
	bool encontrado, esAlfaN;

   while (EsDelimitador(car_leido)) {
        	car_leido = Leer_Caracter();
    	}
	switch (car_leido){
		case ('\0'):	//Fin fichero
			token = TokenGramatica (FinFichero);
			break;
		case ('\n'):
			token = TokenGramatica (SaltLinea);

			nLineas ++;
			car_leido= Leer_Caracter();

			//Incremento linea
			break;
		case ('='):
			car_leido= Leer_Caracter ();
			if (EsSeparador(car_leido)){
				token = TokenGramatica (Igual,"=");	 //Genera Token Igual
			}
			else {
				simbolo = Leer_Simbolo ("="); //Se lee hasta encontrar un delimitador y se guarda en simbolo
				token = TokenGramatica (IdSimbTerminal,simbolo); // Genera Token IdSimbTerminal con el simbolo almacenado
			}
			break;
		case ('-'):
			car_leido= Leer_Caracter ();
			if (car_leido == '>'){
				car_leido= Leer_Caracter ();
				if (EsSeparador(car_leido)){
					token = TokenGramatica (Flecha,"->"); // Genera token Flecha
				}
				else {
					simbolo = Leer_Simbolo ("->"); //Se lee hasta encontrar un delimitador y se guarda en simbolo
					token = TokenGramatica (IdSimbTerminal,simbolo); // Genera Token IdSimbTerminal con simbolo almacenado
				}
			}
			else {
				if (EsSeparador(car_leido)){
					token = TokenGramatica (IdSimbTerminal,"-"); // Genera con el guion leido
				}
				else{

					simbolo = Leer_Simbolo ("-"); //Se lee hasta encontrar un delimitador y se guarda en simbolo
					token = TokenGramatica (IdSimbTerminal, simbolo); // Genera Token IdSimbTerminal con simbolo almacenado
				}
			}
			break;
		case ('/'):

			car_leido= Leer_Caracter ();
			if (car_leido == '/'){
				car_leido= Leer_Caracter ();
				if (car_leido == '/'){
					car_leido= Leer_Caracter ();
						if (car_leido == '/'){
							do {
							car_leido= Leer_Caracter ();
							}
							while (car_leido != '\0' and car_leido != '\r' and car_leido != '\n');
							token = TokenGramatica (Comentario);    //Genera token comentario
						}
						else {
							simbolo = Leer_Simbolo ("///");
							token = TokenGramatica (IdSimbTerminal,simbolo); // Genera token IdSimbTerminal con simbolo almacenado
						}
				}
				else {
					simbolo = Leer_Simbolo ("//");
					token = TokenGramatica (IdSimbTerminal,simbolo); // Genera token IdSimbTerminal con simbolo almacenado
				}
			}
			else {
				simbolo = Leer_Simbolo ("/");
				token = TokenGramatica (IdSimbTerminal,simbolo); // Genera token IdSimbTerminal con simbolo almacenado
			}
			break;
		case ('{'):
			car_leido= Leer_Caracter ();
			if (EsSeparador (car_leido)){
				token = TokenGramatica (LlaveAbierta,"{");  //Genera token llave abierta
			}
			else {
				simbolo = Leer_Simbolo ("{");
				token = TokenGramatica (IdSimbTerminal,simbolo); // Genera token IdSimbTerminal con simbolo almacenado
			}
			break;
		case ('}'):
			car_leido= Leer_Caracter ();
			encontrado= false;
			while (EsDelimitador (car_leido)){
				car_leido= Leer_Caracter ();
				encontrado = true;
			}
      			if (car_leido  == '\r' or car_leido  == '\0' or car_leido  == '\n'){
				token = TokenGramatica (LlaveCerrada,"}");  //Genera token LlaveCerrada
			}
			else {
				if (encontrado){
					token = TokenGramatica (IdSimbTerminal,"}");//Genera token LlaveCerrada
				}
				else {
					simbolo = Leer_Simbolo ("}");
					token = TokenGramatica (IdSimbTerminal,simbolo); // Genera token IdSimbTerminal con simbolo almacenado
				}
			}
			break;
		default:
			esAlfaN = true;
			while (! EsSeparador(car_leido)){

                simbolo = concatenar_Simbolo(simbolo, car_leido);
				//simbolo = simbolo + car_leido;
				if (!(EsAlfaNumerico (car_leido) or (car_leido == '_'))){
					esAlfaN = false;
				}
				car_leido= Leer_Caracter ();
			}
			if (esAlfaN) {
			    if (simbolo == "Terminales"){
                    token = TokenGramatica (Terminales,"Terminales"); //Genera token Terminales
                    }
                else {
                    if (simbolo == "NoTerminales"){
                        token = TokenGramatica (NoTerminales,"NoTerminales");   //Genera token NoTerminales
                    }
                    else{
                        if(simbolo == "Axioma"){
                            token = TokenGramatica (Axioma,"Axioma");   //Genera token Axioma
                        }
                        else {
                            if (simbolo == "Producciones"){
                            token = TokenGramatica (Producciones,"Producciones");   //Genera token Producciones
                            }
                            else {
                                token = TokenGramatica (IdSimbolo,simbolo); //Genera token Idsimbolo con lo que haya leido
                            }
                        }
                    }
                }
			}
			else {
				token = TokenGramatica (IdSimbTerminal,simbolo);    //Genera token IdSimbTerminal con lo que haya leido

			}

	}
    return token;

}
