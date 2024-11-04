#include "AnalizadorG.h"
//Constructor, inicializa y carga la lista en la que insertar errores
AnalizadorG :: AnalizadorG(vector<string> * ListaErrores){
    error =0;
    decConjuntos = 0;    //Flag que indica si se estan analizando los conjuntos de terminales y no terminales
    antecedente = 0;//Flag que indica si se estan analizando el axioma
    nReglas =0;
    ListaError = ListaErrores;

}
//Producion INI *   *   *   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: INI(){
    FUERACONJ();
    CONJUNTOS();
    FUERACONJ();
    PROD();
    FINFICHERO();
}

// Produccion CONJUNTOS *   *   *   *   *   *   *   *   *   *   *   *
void  AnalizadorG :: CONJUNTOS(){
    if (TLeido.GetTipo() == Terminales){
        TERM();
        FUERACONJ();
        SIGCONJUNTO();
    }
    else if (TLeido.GetTipo() == NoTerminales){
        NOTERM();
        FUERACONJ();
        SIGCONJUNTO2();
    }
    else if (TLeido.GetTipo() == Axioma){
        AXIOM();
        FUERACONJ();
        SIGCONJUNTO3();
    }
    else {  //Error no se hallaba ninguno de los conjuntos que debían
        error++;
        InsertarError("Se esperaba las palabras Terminales, NoTerminales, Axioma o comentarios",Lex->GetLinea());
    }
}

// Produccion AXIOMA *   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: AXIOM(){
    TTokenGramatica Taux;

    if (!(TLeido.GetTipo() == Axioma)){
                InsertarError("Se esperaba la declaración del axioma ",Lex->GetLinea());
        error++;
    }
    TLeido = Lex->DameToken();
    if (!(TLeido.GetTipo() == Igual)){
        error++;
        InsertarError("Se esperaba el simbolo = ",Lex->GetLinea());
    }
    else{
        TLeido = Lex->DameToken();
    }
    Taux = TLeido.GetTipo();
    if (!((Taux == Terminales) || (Taux == NoTerminales)|| (Taux == IdSimbolo)||
             (Taux == Axioma)|| (Taux == Producciones))){
            error++;
            InsertarError("El axioma debe ser un simbolo no Terminal",Lex->GetLinea());
            TLeido = Lex->DameToken();
    }
    SimbAxioma = TLeido.getSimbolo();
    TLeido = Lex->DameToken();

}


// Produccion SIGCONJUNTO   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: SIGCONJUNTO(){
    if (TLeido.GetTipo() == NoTerminales){
        NOTERM();
        FUERACONJ();
        AXIOM();
    }
    else if (TLeido.GetTipo() == Axioma){
            AXIOM();
            FUERACONJ();
            NOTERM();
    }
    else {error++;
        InsertarError("Se esperaba el conjunto de NoTerminales, Axioma o comentarios",Lex->GetLinea());
        }
}

// Produccion SIGCONJUNTO2   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: SIGCONJUNTO2(){
    if (TLeido.GetTipo() == Terminales){
        TERM();
        FUERACONJ();
        AXIOM();
    }
    else if (TLeido.GetTipo() == Axioma){
        AXIOM();
        FUERACONJ();
        TERM();
    }
    else {error++;
    InsertarError("Se esperaba el conjunto de Terminales, Axioma o comentarios",Lex->GetLinea());
        }
}

// Produccion SIGCONJUNTO3   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: SIGCONJUNTO3(){

    if (TLeido.GetTipo() == NoTerminales){
        NOTERM();
        FUERACONJ();
        TERM();

    }
    else if (TLeido.GetTipo() == Terminales){
        TERM();
        FUERACONJ();
        NOTERM();
    }
    else {
        error++;
        InsertarError("Se esperaba el conjunto de Terminales,NoTerminales o comentarios",Lex->GetLinea());
    }

}

// Produccion FUERACONJ   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: FUERACONJ(){

    if (TLeido.GetTipo() == Comentario){
        TLeido = Lex->DameToken();
        if (!(TLeido.GetTipo() == SaltLinea)){
            error ++;
            TLeido = Lex->DameToken();
        }
        else {
            TLeido = Lex->DameToken();
            FUERACONJ();
        }
    }
    else if (TLeido.GetTipo() == SaltLinea){
        TLeido = Lex->DameToken();
        FUERACONJ();
    }
}

// Produccion TERM   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: TERM(){
    if (!(TLeido.GetTipo() == Terminales)){
        InsertarError("Se esperaba el conjunto de Terminales o comentarios",Lex->GetLinea());
        error++;
    }
    TLeido = Lex->DameToken();
    if (!(TLeido.GetTipo() == Igual)){
        InsertarError("Se esperaba el simbolo = ",Lex->GetLinea());
        error++;
    }
    else{
        TLeido = Lex->DameToken();
    }

    if (!(TLeido.GetTipo() == LlaveAbierta)){
        InsertarError("Se esperaba el simbolo { ",Lex->GetLinea());
        error++;
    }
    else{
        TLeido = Lex->DameToken();
    }
    decConjuntos =1;    //Activo el flag de declaracion de los terminales

    CONJTERMINI();

}

// Produccion CONJTERMINI   *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: CONJTERMINI(){

    if (TLeido.GetTipo() == SaltLinea){
        TLeido = Lex->DameToken();
        CONJTERM();
    }
    else {
        IDTERM();
        CONJTERM();
    }
}
// Produccion CONJTERM  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: CONJTERM(){


    if (TLeido.GetTipo() == SaltLinea){
        TLeido = Lex->DameToken();
        CONJTERM();
    }
    else if (TLeido.GetTipo() == LlaveCerrada){
        TLeido = Lex->DameToken();
        decConjuntos = 0;   //Desactivo el flag de declarion de terminales
    }
    else if (!(TLeido.GetTipo() == FinFichero)){
        IDTERM();
        CONJTERM();
    }
    else {
        InsertarError("Final de fichero Inesperado, posiblemente le falte }  ",Lex->GetLinea());
        error++;
    }
}

// Produccion IDTERM  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: IDTERM(){
    string errorAux;
    TTokenGramatica Taux;
    Taux = TLeido.GetTipo();
    if (!((Taux == Terminales) || (Taux == NoTerminales)|| (Taux == IdSimbolo)|| (Taux == IdSimbTerminal)||
             (Taux == Axioma)|| (Taux == Producciones)|| (Taux == Flecha)|| (Taux == LlaveAbierta)|| (Taux == Igual))){
                InsertarError("Recibido un elemento que no es un terminal",Lex->GetLinea());

        error++;
    }
    else if (decConjuntos){
        if (esNoTerminal(TLeido.getSimbolo()) || esTerminal(TLeido.getSimbolo())){   //Se comprueba que el elemnto no estubiese ya
            errorAux ="Se intenta declarar un elemento ya declarado:   ";
            errorAux = errorAux + TLeido.getSimbolo();
            InsertarError(errorAux,Lex->GetLinea());
            error++;
            //Ya esta puesto ese elemento
        }
        ElemsTerminal.push_back (TLeido.getSimbolo());
    }
    TLeido = Lex->DameToken();
}

// Produccion NOTERM  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: NOTERM(){
    if (!(TLeido.GetTipo() == NoTerminales)){
        InsertarError("Se esperaba la palabra NoTerminales",Lex->GetLinea());
        error++;
    }
    TLeido = Lex->DameToken();
    if (!(TLeido.GetTipo() == Igual)){
        error++;
        InsertarError("Se esperaba el símbolo = ",Lex->GetLinea());
    }
    else{
        TLeido = Lex->DameToken();
    }
    if (!(TLeido.GetTipo() == LlaveAbierta)){
        InsertarError("Se esperaba el símbolo { ",Lex->GetLinea());
        error++;
    }
    else{
        TLeido = Lex->DameToken();
    }
    decConjuntos =1;    //Activo flag de declaracion de No Terminales
    NOTERMINI();
}


// Produccion IDNOTERM  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: IDNOTERM(){
    string errorAux;
    TTokenGramatica Taux;
    Taux = TLeido.GetTipo();
    if (!((Taux == Terminales) || (Taux == NoTerminales)|| (Taux == IdSimbolo)||
             (Taux == Axioma)|| (Taux == Producciones))){
        InsertarError("Detectado un simbolo terminal en donde solo se permiten no terminales",Lex->GetLinea());
        error++;
    }
    else if (decConjuntos){
        if (esNoTerminal(TLeido.getSimbolo()) || esTerminal(TLeido.getSimbolo())){   //Se comprueba que el elemnto no estubiese ya
            InsertarError("Se intenta declarar un elemento ya declarado",Lex->GetLinea());
            //errorAux ="Se intenta declarar un elemento ya declarado:   ";
            //errorAux = errorAux + TLeido.getSimbolo();
            //InsertarError(errorAux,Lex->GetLinea());
            error++;
            //Ya esta puesto ese elemento
        }
        ElemsNoTerminal.push_back (TLeido.getSimbolo());
    }
    TLeido = Lex->DameToken();
}
// Produccion NOTERMINI  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: NOTERMINI(){

    if (TLeido.GetTipo() == SaltLinea){
        TLeido = Lex->DameToken();
        NOTERMINI();
    }
    else {
        IDNOTERM();
        CONJNOTERM();
    }
}

// Produccion CONJNOTERM  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: CONJNOTERM(){
    if (TLeido.GetTipo() == SaltLinea){
        TLeido = Lex->DameToken();
        CONJNOTERM();
    }
    else if (TLeido.GetTipo() == LlaveCerrada){
        TLeido = Lex->DameToken();
        decConjuntos = 0;   //Desactivo el falg de declaracion de No Terminales
    }
    else if (!(TLeido.GetTipo() == FinFichero)){
        IDNOTERM();
        CONJNOTERM();
    }
    else {
        InsertarError("Final de fichero Inesperado, posiblemente le falte }",Lex->GetLinea());
        error++;
    }
}


// Produccion FINFICHERO  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: FINFICHERO(){
    if (TLeido.GetTipo() == SaltLinea){
        TLeido = Lex->DameToken();
        FINFICHERO();
    }
    else if (TLeido.GetTipo() == Comentario){
        TLeido = Lex->DameToken();
        if (!(TLeido.GetTipo() == SaltLinea)){
            error++;
        }
        TLeido = Lex->DameToken();
        FINFICHERO();
    }
    else if(!(TLeido.GetTipo() == FinFichero)){
        error++;
        InsertarError("Despúes de las producciones solo se admiten comentarios",Lex->GetLinea());
    }
}


// Produccion PROD  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: PROD(){
    //Debe leer:     Producciones = { saltoLinea
    if (!(esNoTerminal(SimbAxioma))){
        error++;
        InsertarError("No se ha declarado el axioma como simbolo NoTerminal",Lex->GetLinea());
    }
    if (!(TLeido.GetTipo() == Producciones)){
        error++;
        InsertarError("Se esperaba la palabra Producciones",Lex->GetLinea());
    }
    TLeido = Lex->DameToken();
    if (!(TLeido.GetTipo() == Igual)){
        error++;
        InsertarError("Se esperaba el símbolo = ",Lex->GetLinea());
    }
    else{
        TLeido = Lex->DameToken();
    }
    if (!(TLeido.GetTipo() == LlaveAbierta)){
        error++;
        InsertarError("Se esperaba el símbolo { ",Lex->GetLinea());
    }
    else{
        TLeido = Lex->DameToken();
    }
    if (!(TLeido.GetTipo() == SaltLinea)){
        error++;
        InsertarError("Se esperaba un salto de linea ",Lex->GetLinea());
    }
    TLeido = Lex->DameToken();
    //Empiezan las producciones
    PRODUCC();
}



// Produccion PRODUCC  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: PRODUCC(){
    if ((TLeido.GetTipo() == SaltLinea)){
        TLeido = Lex->DameToken();
        PRODUCC();
    }
    else if  (TLeido.GetTipo() == LlaveCerrada){
        TLeido = Lex->DameToken();
        FINFICHERO();
    }
    else {
        if (TLeido.GetTipo() == FinFichero){
            InsertarError("Se esperaba el símbolo }  ",Lex->GetLinea());
            error++;
            return ;
        }
        REGLA();
        PRODUCC();
    }
}


// Produccion REGLA  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: REGLA(){

   // antecedente = 1; //Activo el flag que indica que viene el antecedente de la regla
    //* *   *   *   *   *
    //Cramos una nueva regla, comprobando q es un terminal
    if (!(esNoTerminal(TLeido.getSimbolo()))){
        error++;
        InsertarError("El antecedente debe ser un no terminal ",Lex->GetLinea());
        if(TLeido.GetTipo() == Comentario){
        InsertarError("Los comentarios solo se permiten después de la regla pero en la misma línea ",Lex->GetLinea());
        // Consume los tokens necesarios para proseguri con la comprobación
        TLeido = Lex->DameToken();
        TLeido = Lex->DameToken();
        }


            //Antecedente debe ser un elemento no Terminal
    }
    else {
            nReglas++;
            Reglas nueva_1=Reglas(nReglas,TLeido.getSimbolo());
            ListReglas.push_back (nueva_1);
    }
    TLeido = Lex->DameToken();
    //* *   *   *   *   *
 //   antecedente = 0;    //Desactivo el flag que indica que viene el antecedente de la regla
    if (!(TLeido.GetTipo() == Flecha)){
        InsertarError("Se esperaba  -> ",Lex->GetLinea());
        error++;
    }
    TLeido = Lex->DameToken();
    CONSEC();
}

// Produccion CONSEC  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: CONSEC(){
    TTokenGramatica Taux;
    string errorAux;

    Taux = TLeido.GetTipo();
    //Comprueba si es del tipo adecuado
    if ((Taux == Terminales) || (Taux == NoTerminales)|| (Taux == IdSimbolo)|| (Taux == IdSimbTerminal)||
            (Taux == Axioma)|| (Taux == Producciones)|| (Taux == Flecha)|| (Taux == LlaveAbierta)|| (Taux == Igual)||(Taux == LlaveCerrada)){
                //Comprueba si se ha declarado al elemento
        if (!(esTerminal(TLeido.getSimbolo()) || esNoTerminal(TLeido.getSimbolo()))) {
            error++;
            errorAux ="No se ha declarado este elemento   ";
            errorAux = errorAux + TLeido.getSimbolo();
            InsertarError(errorAux,Lex->GetLinea());

        }
        if (!ListReglas.empty()){
        ListReglas.back().introducirElemConsec(TLeido.getSimbolo());
        }
        TLeido = Lex->DameToken();
        SIGCONSEC();
    }

}

// Produccion CONSEC  *   *   *   *   *   *   *   *   *   *   *
void AnalizadorG :: SIGCONSEC(){
    TTokenGramatica Taux;
    string errorAux;
    if (TLeido.GetTipo() == SaltLinea){
        //NULL;
    }
    else if(TLeido.GetTipo() == Comentario){
        TLeido = Lex->DameToken();
        if (!(TLeido.GetTipo() == SaltLinea)){
            error ++;
            TLeido = Lex->DameToken();
        }

    }
    else if (TLeido.GetTipo() == FinFichero){
    }

    else {
        Taux = TLeido.GetTipo();
        //Compruebo si el elemento ha sido declarado
        if ((Taux == Terminales) || (Taux == NoTerminales)|| (Taux == IdSimbolo)|| (Taux == IdSimbTerminal)||
            (Taux == Axioma)|| (Taux == Producciones)|| (Taux == Flecha)|| (Taux == LlaveAbierta)|| (Taux == Igual)|| (Taux == LlaveCerrada)){
            if (!(esTerminal(TLeido.getSimbolo()) || esNoTerminal(TLeido.getSimbolo()))) {
                error++;
                errorAux ="No se ha declarado este elemento   ";
                errorAux = errorAux + TLeido.getSimbolo();
                InsertarError(errorAux,Lex->GetLinea());
            }

            if (!ListReglas.empty()){
            ListReglas.back().introducirElemConsec(TLeido.getSimbolo());
            }
        }
          TLeido = Lex->DameToken();
        SIGCONSEC();
    }
}


//POS: devuelve si el elmenento elem se encuentra en el vector ElemsTerminal

bool AnalizadorG ::esTerminal(string elem){
    unsigned int i=0;
    int encontrado =0;
    while (i< ElemsTerminal.size() && !encontrado){
        if  (ElemsTerminal.at(i)== elem){
        encontrado =1;
        }
        i++;
    }
    return encontrado;
}

//POS: devuelve si el elmento elemento se encuentra en el vector ElemsNoTerminal

bool AnalizadorG ::esNoTerminal(string elem){
    unsigned int i=0;
    int encontrado =0;
    while (i< ElemsNoTerminal.size() && !encontrado){
        if  (ElemsNoTerminal.at(i)== elem){
        encontrado =1;
        }
        i++;
    }
    return encontrado;
}
//Inserta un error en la lista de errores con el formatoo predeterminado
// Linea X: DescError
void AnalizadorG ::InsertarError(const char * textoError, int linea){
    char buffer [5];
    string errorEncontrado;
    sprintf(buffer,"%d",linea);
    errorEncontrado = "Linea ";
    errorEncontrado = errorEncontrado + buffer+": "+textoError;
    ListaError->push_back (errorEncontrado);
}
//Inserta un error en la lista de errores
void AnalizadorG ::InsertarError(string textoError, int linea){
    char buffer [5];
    string errorEncontrado;
    sprintf(buffer,"%d",linea);
    errorEncontrado = "Linea ";
    errorEncontrado = errorEncontrado + buffer+": "+textoError;
    ListaError->push_back (errorEncontrado);
}

//PRE: El fichero ha de de estar abierto
//Inicia el analisis de la gramatica, devolverá si si el formato es el correcto o no
bool AnalizadorG :: comprobarFormato(istream * fichero){
    Lex = new ALexicoG(fichero);
    TLeido = Lex->DameToken();
    INI();
    if (error > 0){
        return false;
    }
    else{
        return true;
    }
}

//Devuelve el numero de reglas que tiene la gramatica

int AnalizadorG ::getReglas(){
    return nReglas;
}



//Devuelve la colección de terminales
vector<string> AnalizadorG ::GetTerminal(){
return ElemsTerminal;
}
//Devuelve la conlección de no Terminales
vector<string> AnalizadorG ::GetNoTerminal(){
return ElemsNoTerminal;
}
//Devuelve el axioma
string AnalizadorG ::GetAxioma(){
return SimbAxioma;
}
//Devuelve la referencia a la lista de reglas
vector<Reglas>* AnalizadorG ::GetListReglas(){
return &ListReglas;
}
//Destructor de la clase
AnalizadorG::~AnalizadorG()
{
    delete Lex;
    ElemsNoTerminal.clear();
    ElemsTerminal.clear();
    ListReglas.clear();

}
