#ifndef TTOKENH
#define TTOKENH


#include <string>

using namespace std;

//Tipos de tokens que tendrá la gramática
enum TTokenGramatica {
	Terminales,
	NoTerminales,
	Axioma,
	Producciones,
	Igual,
	LlaveAbierta,
	LlaveCerrada,
	SaltLinea,
	FinFichero,
	Del,
	Comentario,
	Flecha,
	lambda,
	IdSimbolo,
	IdSimbTerminal
};

class TokenGramatica {
private:
	// Tipo del token
	TTokenGramatica Token;
	string Simbolo;

public:

	// Constructor de un token que no es del tipo IdSimbolo o IdSimbTerminal
	TokenGramatica (TTokenGramatica IdToken);
	// Constructor de un token del tipo IdSimbolo o IdSimbTerminal
	TokenGramatica (TTokenGramatica IdToken, string IdSimbolo);
	TokenGramatica ();
	//Devuelve el tipo del token
	TTokenGramatica GetTipo();
    //Devuelve el simbolo del token
    string getSimbolo();
};
#endif
