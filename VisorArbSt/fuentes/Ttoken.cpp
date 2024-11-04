#include "Ttoken.h"
#include <stdio.h>

TokenGramatica :: TokenGramatica() {
    Token = Flecha;
    Simbolo = "";
}

// Constructor para todos aquellos token excepto los id de simbolos
TokenGramatica :: TokenGramatica (TTokenGramatica IdToken){
	Token = IdToken;
	Simbolo = "";
}
// Constructor par los IdSimbolos
TokenGramatica :: TokenGramatica (TTokenGramatica IdToken, string IdSimbolo){
	Token = IdToken;
	Simbolo = IdSimbolo;
}


TTokenGramatica TokenGramatica :: GetTipo(){
	return Token;
}

// Devuelve el simbolo del token
string TokenGramatica :: getSimbolo(){
    return Simbolo;
}
