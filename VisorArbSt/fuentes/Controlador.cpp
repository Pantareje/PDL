#include "Controlador.h"
//Constructor de la clase contorlador
Controlador :: Controlador (){
    modo='a';
    elementosDerivables=0;
    estructParse = false;
    ListaErrores = new vector<string>;
}

    //Carga la gramatica y devuelve si es correcta
    //PRE: No existe ninguna gramática cargada
    //POST: Crea un analizador para la gramatica y carga los terminales, noTErminales y lista de reglas
bool Controlador :: CargarGramatica (char *fichero){
    bool AnalisisCorrecto;

    ListaErrores ->clear(); //Limpia la lista de errores

    GAnalizador = new AnalizadorG(ListaErrores);    //Crea el analizador
    ifstream ficheroAb(fichero);    //Abre el fichero
    AnalisisCorrecto =GAnalizador -> comprobarFormato (&ficheroAb);
    if (AnalisisCorrecto){
        //Crea las estructuras
        Terminal = new Simbolos(GAnalizador -> GetTerminal());
        NoTerminal = new Simbolos(GAnalizador -> GetNoTerminal());
        Axioma = new Simbolos(GAnalizador -> GetAxioma());
        ListaDeReglas = GAnalizador->GetListReglas();
        gramaticaCargada = true;
        return true;
    }
    else{
        //Si ha habido error destruimos el analizador creado
        delete GAnalizador;
        return false;
    }
}

    //Carga el parse y devuelve si es correcto
    //PRE: Existe una gramatica cargada
    //POST: Crea el analizador del parse y carga la lista del parse
bool Controlador :: CargarParse (char *fichero){
    vector<int> parseDevuelto;

    ifstream ficheroAb(fichero);    //Abre el fichero
    ListaErrores ->clear();     //Limpia la lista de errores

    ParseAnalizador = new AnalizadorP (GAnalizador->getReglas(),ListaErrores);

    parseDevuelto =ParseAnalizador->analizarParse (&ficheroAb); //Carga el parse

    if (ParseAnalizador->getNerrores()){
        // Si ha habido error destruye lo creado
        return false;
        delete ParseAnalizador;
    }
    else {
        ListaParse = new ListaReglas (parseDevuelto);   //Crea la lista del parse
        ParseCargado = true;
        modo = ParseAnalizador ->getModo();
        estructParse = true;//Indicamos que se ha creado las estructuras del parse
        return true;
    }
}

    //Construye el árbol y devuelve si es correcto
    //PRE: Existen una gramatica y parse cargados
    //POST: Construirá una lista de nodos
bool Controlador :: ConstruirArbol (){
    string descFallo;
    elementosDerivables =0;
    ListaErrores ->clear();
    if (!(ParseCargado && gramaticaCargada)){
    }
    else if (modo == 'D'){
        ConstruirArbolDescendente();
    }
    else if (modo == 'A'){
        ConstruirArbolAscendente();
    }
   if (errorArbol){
    ListaNodos.clear();
   }
   else {
   }
   return !errorArbol;
}


    // construye el árbol cuando es Ascendente
void Controlador :: ConstruirArbolAscendente(){
    unsigned int i;

    bool fallo, encontrado,inicioLista;
    char buffer[5];
    //strings auxiliares
    string ante,ante2,descFallo;
    //Vector donde almacenaremos el consecuente de la regla
    vector<string> simbConsec;
    Nodo *nodoAux;

    Lreg = ListaParse->dameListaInvertida();    //Cargamos la lista del parse invertida
    simb = Axioma->extraerAxioma();     //Extraemos el axioma, (recorremos de arriba  abajo)
    elementosDerivables++;
    nodoAux = new Nodo(simb, false, true);      //Creamos el nodo del axioma
    nodoAux->insertarReglaDeriv(Lreg.at(0));    //Insertamos la regla por la que derivara
    ListaNodos.push_front(*nodoAux);            //Insertamos el nodo del axioma
    Nodo NodoDeriv;
    list<Nodo>::iterator j;
    fallo =false;
    unsigned int w;
    i=0;
    //Recorremos todo el parse hasta el final o que se encuentre un fallo
    while (i<Lreg.size() && (!fallo)){
        //Extraigo la regla a aplicar
        Reglas reglaAux = ListaDeReglas->at(Lreg.at(i) - 1);
        ante = reglaAux.getAnte();
        encontrado=false;


        j = ListaNodos.end();
        //Decrementamos el iterador ya que apùntaba al final de la lista donde no hay elementos
        j--;
        if (ListaNodos.size()==1){
            j=ListaNodos.begin();
        }
        inicioLista=false;
        //Busca el nodo de derecha a izaquierda
        //* *   *   *   *   *   *   *   *   *   *   *   *   **  *   *   *
        while ((!inicioLista) && (!encontrado)){
            if (j==ListaNodos.begin()){
                    inicioLista = true;
            }
            // Encontramos un elemento derivable
            if (j->esDerivable()){
                ante2=j->getSimbolo();
                encontrado = true;
                //Comprobamos si es el que buscamos
                if (ante2 != ante){
                    //Comprobamos si falla al intentar derivar el axioma
                    if (i==0){
                        //Todavía estamos con el axioma
                        descFallo = "Imposible alcanzar el axioma ";
                        ListaErrores->push_back(descFallo);
                    }
                    else{
                        descFallo = "Encontrado el elemento: ";
                        descFallo = descFallo + (ante2)+"     derivable por la derecha que no es el que debería:    "+(ante);
                        ListaErrores->push_back(descFallo);
                        descFallo = "Se había recorrido el siguiente parse: ";
                        //Concatenamos la lista del parse que se ha recorrido para mostrar el error producido
                         w=0;
                        sprintf(buffer,"%d",Lreg.at(w));
                        descFallo = descFallo + buffer;
                        for ( w=1;w<=i;w++){
                            sprintf(buffer,"%d",Lreg.at(w));
                            descFallo = descFallo +","+ buffer;
                        }
                        ListaErrores->push_back(descFallo);
                    }
                    fallo= true;
                }
                else{
                    //Decrementamos numero de derivables
                    elementosDerivables--;
                    //Quitamos la derivavilidad al nodo encontrado
                    j->quitarDerivable();
                    j->insertarReglaDeriv(Lreg.at(i));
                    simbConsec = reglaAux.getConsec();
                    insertarDerivacion( simbConsec,&j,j,i);
                }
            }
            else{
                j--;
            }
        }
        if (inicioLista && (!encontrado)){
            descFallo = "No se pudo construir el árbol, parse recorrido:   ";
            //Concatenamos la lista del parse que se ha recorrido para mostrar el error producido
            w=0;
            sprintf(buffer,"%d",Lreg.at(w));
            descFallo = descFallo + buffer;
            for ( w=1;w<=i;w++){
                sprintf(buffer,"%d",Lreg.at(w));
                descFallo = descFallo +","+ buffer;
            }
            ListaErrores->push_back(descFallo);
            fallo = true;
        }
        //* *   *   *   *       *   *   *   *   *   *       *   *   *   *
        //Nodo o encontrado o hallado error
        i++;
    }
    //Comprobamos que no queda ningunelemento derivable sin derivar
    if ((elementosDerivables) && (!fallo)){
        fallo = true;
        descFallo = "Existen nodos no Terminales que no han sido derivados después de aplicar todo el parse";
        ListaErrores->push_back(descFallo);
    }
    errorArbol=fallo;
}

//PRE: La gramatica y el parse son correctos y ya han sido cargados

void Controlador :: ConstruirArbolDescendente(){
    unsigned int i,w;

    bool fallo, encontrado;
    string ante,ante2,descFallo;
    vector<string> simbConsec;
    char buffer[5];

    Nodo *nodoAux;
    Lreg = ListaParse->dameLista();
    simb = Axioma->extraerAxioma();
    elementosDerivables++;
    nodoAux = new Nodo(simb, false, true);
    nodoAux->insertarReglaDeriv(Lreg.at(0));
    ListaNodos.push_front(*nodoAux);
    Nodo NodoDeriv;
    list<Nodo>::iterator j;
    j = ListaNodos.begin();
    fallo =false;
    i=0;
    //Recorremos todo el parse hasta el final o que se encuentre un fallo
    while (i<Lreg.size() && (!fallo)){
        //Extraigo la regla a aplicar, al empezar los parses desde indice 1 se restaura con la
        //resta para acceder al elemento que queremos
        Reglas reglaAux = ListaDeReglas->at(Lreg.at(i) - 1);
        ante = reglaAux.getAnte();
        encontrado=false;
        j = ListaNodos.begin();
        //Busca el nodo
        //* *   *   *   *   *   *   *   *   *   *   *   *   **  *   *   *
        //De izquierda a derecha
        while (j!=ListaNodos.end() && (!encontrado)){
            // Encontramos un elemento derivable
            if (j->esDerivable()){
                ante2=j->getSimbolo();
                encontrado = true;
                //Comprobamos si es el que buscamos
                if (ante2 != ante){
                    if (i==0){
                        //Todavía estamos con el axioma (primier elemento)
                        descFallo = "Imposible derivar el axioma ";
                        ListaErrores->push_back(descFallo);
                    }
                    else{
                        descFallo = "Encontrado el elemento: ";
                        descFallo = descFallo + (ante2)+"     derivable por la izquierda que no es el que debería:    "+(ante);
                        ListaErrores->push_back(descFallo);
                        descFallo = "Se había recorrido el siguiente parse: ";
                        //Concatenamos la lista del parse que se ha recorrido para mostrar el error producido
                         w=0;
                        sprintf(buffer,"%d",Lreg.at(w));
                        descFallo = descFallo + buffer;
                        for ( w=1;w<=i;w++){
                            sprintf(buffer,"%d",Lreg.at(w));
                            descFallo = descFallo +","+ buffer;
                        }
                        ListaErrores->push_back(descFallo);

                    }
                    fallo= true;
                }
                else{
                    //Decrementamos el numero de elemntos derivables
                    elementosDerivables--;
                    //Quitamos la derivavilidad al nodo encontrado
                    j->quitarDerivable();
                    j->insertarReglaDeriv(Lreg.at(i));

                    simbConsec = reglaAux.getConsec();

                    insertarDerivacion( simbConsec,&j,j,i);
                }
            }
            else{
                j++;
            }
        }
        if (j==ListaNodos.end() && (!encontrado)){
            descFallo = "No se pudo construir el árbol, parse recorrido:   ";
            //Concatenamos la lista del parse que se ha recorrido para mostrar el error producido
            w=0;
            sprintf(buffer,"%d",Lreg.at(w));
            descFallo = descFallo + buffer;
            for ( w=1;w<=i;w++){
                sprintf(buffer,"%d",Lreg.at(w));
                descFallo = descFallo +","+ buffer;
            }
            ListaErrores->push_back(descFallo);
            fallo = true;
        }
        //* *   *   *   *       *   *   *   *   *   *       *   *   *   *

        i++;
    }
    //Comprobamos que no queda ningunelemento derivable sin derivar
    if ((elementosDerivables) && (!fallo)){
        fallo = true;
        descFallo = "Existen nodos no Terminales que no han sido derivados después de aplicar todo el parse";
        ListaErrores->push_back(descFallo);
    }
    errorArbol=fallo;
}


//Inserta los nodos hijos en la lista del árbol
void Controlador :: insertarDerivacion( vector<string> elemRegla,list<Nodo>::iterator *j,list<Nodo>:: iterator w,int indiceRegla ){
    string simbElem;
    list<Nodo>::iterator k,h;
    Nodo *nodoAux;
    //j=h;
    k=*j;
    h=*j;
    //j-> Puntero donde se inserta a continuacion
    if (k!=ListaNodos.end()){
        k++;
    }

    for (unsigned int i=0;i<elemRegla.size();i++){
        simbElem = elemRegla.at(i);

        if (NoTerminal->buscarSimb(simbElem)){
            //Insertaremos un NoTerminal
            elementosDerivables++;
            nodoAux = new Nodo(simbElem, false,true);
            nodoAux->insertarReglaDeriv(0);
            if (k==ListaNodos.begin()){
            }

            ListaNodos.insert(k,1,*nodoAux);

            k--;    //Restauro el indice para que apunte al nodo insertado
            h->insertarHijo(k);     //Inserto el hijo al padre
            k++;

        }
        else{
            //Insertaremos un Terminal
            nodoAux = new Nodo(simbElem,true,false);
            nodoAux->insertarReglaDeriv(0);

            if (k==ListaNodos.end()){
           }

            ListaNodos.insert(k,1,*nodoAux);
            if (k!=ListaNodos.end()){
            }
            k--;        //Restauro el indice para que apunte al nodo insertado
            h->insertarHijo(k);     //Inserto el hijo al padre
            k++;
        }
    }
}


    //Cambia la gramatica actual
    //PRE: Existe una gramatica cargada actualmente
    //POST: Destruye los posibles arboles y parse cargados
bool Controlador::CambiarGramatica(char *fichero){
    ListaErrores ->clear();
    ListaNodos.clear();
    delete Terminal;
    delete NoTerminal;
    delete Axioma;
    delete GAnalizador;
    if (estructParse){
        delete ListaParse;
        delete ParseAnalizador;
    }
    estructParse =false;    //Indicamos que se han eliminado las estructuras del parse
    ListaDeReglas->clear();
    return CargarGramatica(fichero);
}

    //Cambia el parse
    //PRE: Existe un parse actualmente cargado
    //POST: Destruye el posibles arbol  cargado
bool  Controlador::CambiarParse(char *fichero){
    bool resultado;
    ListaErrores ->clear();
    ListaNodos.clear();
    delete ParseAnalizador;
    delete ListaParse;
    resultado =CargarParse(fichero);
    return resultado;
}

//Devuelve la lista de errores
vector<string>* Controlador::DameListaErrores(){
    return ListaErrores;
}


//Devuelve la lista de nodos
list<Nodo>  Controlador::DameLista(){
    return ListaNodos;
}

    //Elimina el arbol
    //PRE: Debe existir el árbol, el parse y la gramatica
    // POST: Inicializa el sistema
void Controlador :: EliminarArbol(){
    ListaErrores ->clear();
    ListaNodos.clear();
    delete Terminal;
    delete NoTerminal;
    delete Axioma;
    delete GAnalizador;
    delete ListaParse;
    delete ParseAnalizador;
    estructParse = false;
    ListaDeReglas->clear();
}

//Destructor de la clase
Controlador ::~Controlador(){
    delete Terminal;
    delete NoTerminal;
    delete Axioma;
  //   delete GAnalizador;
    delete ListaDeReglas;
    delete ListaParse;
    delete ParseAnalizador;


}


