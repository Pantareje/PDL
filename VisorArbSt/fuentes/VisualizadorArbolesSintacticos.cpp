#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <list>
#include <gtk/gtk.h>
#include "Controlador.h"
#include "Nodo.h"
using namespace std;


enum {
        COL_DATA,
        COL_TOTAL
};
enum
{
        COLUMN_PRODUCT,
        NUM_COLUMNS
};


// Estructura con los widgets, estado y controlador que mantienen al sistema
typedef struct
{
    GtkWidget *elem1;
    GtkWidget *elem2;
    GtkWidget *elem3;
    GtkWidget *elem4;
    GtkWidget *elem5;
    GtkWidget *elem6;
    GtkWidget *elem7;
    GtkWidget *elem8;
    GtkWidget *elem9;
 //   GtkWidget *elem10;
 //   GtkWidget *elem11;
    GtkWidget *elem12;
    GtkWidget *elem13;
    GtkWidget *elem14;
    GtkWidget *elem15;
    GtkWidget *elem16;
    int *estado;
    char dir[150];
    Controlador *elemControlador;

} CjtoDataWidget;

typedef struct
{
    GtkWidget *elem1;
    GtkWidget *elem2;
    GtkWidget *elem3;
    GtkWidget *elem4;
    int *estado;


} CjtoWidgetBuscar;


//* *   *   *   *   *   *   *   *   *   *   *   *   **  *   *   *   *




void SelArchivoGramatica(GtkButton *widget, CjtoDataWidget* data);
void SelArchivoParse(GtkButton *widget, CjtoDataWidget* data);
void ConstArbol(GtkButton *widget, CjtoDataWidget* data);
void ExpandirArbol(GtkButton *widget, gpointer data);
void ContraerArbol(GtkButton *widget, gpointer data);
void verFormatos();


//Captura las teclas que se pulsan y comprueban si activana alguna fucnionalidad
bool capturaTeclas (GtkWidget * window,GdkEventKey*	pKey,CjtoDataWidget* data){
 if (pKey->type == GDK_KEY_PRESS){

        switch (pKey->keyval)
		{

            case (65470) ://F1
                verFormatos();
			break;
			case (65471) ://F2
                if (gtk_widget_get_sensitive (data->elem7) ){//Comprueba que la accion esta habilitada
                    SelArchivoGramatica(GTK_BUTTON(data->elem7),data);
                }
			break;
			case (65472) : //F3
            if (gtk_widget_get_sensitive (data->elem8) ){//Comprueba que la accion esta habilitada
                    SelArchivoParse(GTK_BUTTON(data->elem8),data);
                }
            break;
            case (65473) : //F4
            if (gtk_widget_get_sensitive (data->elem9) ){//Comprueba que la accion esta habilitada
                    ConstArbol(GTK_BUTTON(data->elem9),data);
            }
            break;
            case (65474) : //F5
            //No utiliza el boton que se le pasa
                ExpandirArbol(GTK_BUTTON(data->elem9),data->elem1);
            break;
            case (65475) : //F6
            //No utiliza el boton que se le pasa
               ContraerArbol(GTK_BUTTON(data->elem9),data->elem1);
            break;
		}
	}

	return FALSE;
}

//  Muestra una ventana conInformacion sobre los formatos y el programa en general

void verFormatos(){
    GtkWidget *ventana,*notebook,*textViewG,*textViewP,*textViewM,*sw,*sw1,*sw2;
    GtkWidget *labelGramatica,*labelParse,*labelManual;
    GtkTextBuffer *buffer,*buffer1,*buffer2;
    string  textoAmostrar;

    // Establecemos las scrollBar que contendrán al texto
    sw = gtk_scrolled_window_new (NULL, NULL);
                gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);

    sw1 = gtk_scrolled_window_new (NULL, NULL);
                gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw1),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw1),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);
    sw2 = gtk_scrolled_window_new (NULL, NULL);
                gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw2),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw2),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);

    //formato del Parse
    textViewP = gtk_text_view_new();
    gtk_text_view_set_editable (GTK_TEXT_VIEW(textViewP),FALSE);
    textoAmostrar = "El formato del fichero donde se almacenará el parse es bastante más sencillo que la gramática,\núnicamente contendrá dos cosas, primero una palabra que empiece por A si nos referimos a\nun analizador sintáctico ascendente o una palabra que empiece por D si nos referimos a un\nparse de un analizador sintáctico descendente. Y segundo (previo delimitador), una línea con\nun conjunto de números que representarán las reglas a ejecutar. Estos números estarán\n";
    textoAmostrar = textoAmostrar+"separados por delimitadores (espacios en blanco, tabuladores y saltos de línea) y se dará por\nconcluido el parse cuando se lea el final del fichero.\n";
    textoAmostrar = textoAmostrar+"\nPor tanto, la estructura del fichero del parse sería la siguiente:\n\n";
    textoAmostrar = textoAmostrar+"Ascendente 2 21 1 4 7 11 27 8";
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewP));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer),textoAmostrar.c_str(),-1);
    gtk_text_view_set_buffer (GTK_TEXT_VIEW(textViewP),GTK_TEXT_BUFFER(buffer));
    gtk_text_view_set_pixels_inside_wrap(GTK_TEXT_VIEW(textViewP),5);


    //Formato de la gramática
    textViewG = gtk_text_view_new();
    gtk_text_view_set_editable (GTK_TEXT_VIEW(textViewG),FALSE);

    textoAmostrar="Una gramática consta de cuatro componentes principales:\n   -Conjunto de símbolos terminales.\n   -Conjunto de símbolos no terminales.\n   -Un axioma o símbolo inicial.\n   -Un conjunto de producciones.\n";

    textoAmostrar = textoAmostrar + "\nCada conjunto será identificado mediante una palabra específica, que serán";
    textoAmostrar = textoAmostrar +"\nTerminales, NoTerminales, Axioma y Producciones. Estas palabras no son reservadas y";
    textoAmostrar = textoAmostrar +"\npueden ser utilizadas para representar otros símbolos.\n\nPara que el fichero pueda ser analizado en una sola pasada, los conjuntos de símbolos";

    buffer1 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewG));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer1),textoAmostrar.c_str(),-1);

    textoAmostrar = "\nterminales, no terminales y el axioma deben aparecer antes que el conjunto de\nproducciones.\n\nLa representación de los diferentes conjuntos será:\n";
    textoAmostrar = textoAmostrar +"   -Conjuntos de símbolos terminales: Se indicará con la palabra Terminales, seguida\n    de un símbolo igual ( = ) y un símbolo de llave abierta ( { ). A continuación\n    aparecerá la lista de símbolos terminales separados por delimitadores (espacios en\n    blanco, tabuladores y saltos de línea). La finalización del conjunto de símbolos\n     terminales se considerará cuando aparezca un símbolo ( } ) seguido de un salto de\n";
    textoAmostrar = textoAmostrar +"    línea. Por tanto, si se va a utilizar el símbolo de llave cerrada ( } ) no se pude poner\n    como último símbolo de una línea del conjunto, si éste se va a separar en varias\n    líneas.";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    textoAmostrar =  "\n\n   - Conjuntos de símbolos no terminales: Se indicará con la palabra NoTerminales,\n    seguida de un símbolo igual ( = ) y un símbolo de llave abierta ( { ). A continuación\n    aparecerá la lista de símbolos no terminales separados por delimitadores.Para\n    finalizar el conjunto se utilizará un símbolo de llave cerrada ( } ). No puede\n    terminales.";
    textoAmostrar = textoAmostrar +"\n\n   - Axioma: Se indicará con la palabra Axioma, seguida de un símbolo igual ( = ) y el\n    símbolo no terminal que se utilizará como axioma. Este símbolo deber estar\n    contenido en el conjunto de símbolos no terminales.";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    textoAmostrar = "\n\n   - Producciones: Se indicará con la palabra Producciones, seguida de un símbolo igual\n    ( = ) y un símbolo de llave abierta ( { ). A partir de la siguiente línea se escribirán las\n    diferentes reglas, una por línea (pudiendo aparecer líneas en blanco). Para finalizar\n    el conjunto de producciones, se pondrá un símbolo de llave cerrada ( } ) en una\n    línea nueva.";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    textoAmostrar ="\n\nEl formato de los símbolos será:\n\n - Símbolos no terminales: Combinación de letras, números y símbolos de guión bajo\n    ( _ ).\n\n   - Símbolos terminales: Combinación de letras, números o símbolos, excepto\n    delimitadores. Además, para representar el símbolo lambda, se utilizará la palabra\n    reservada lambda.";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    textoAmostrar="\n\nCada regla estará formada por un antecedente (que, para que la gramática sea tipo 2\ndeberá estar formado únicamente por un símbolo no terminal), seguida por uno o\nvarios delimitadores (que no sean saltos de línea). Después aparecerá una flecha (->),\nseguida de uno o varios delimitadores y después aparecerá el consecuente de la regla,\n";
    textoAmostrar = textoAmostrar + "que estará formado por uno o varios símbolos terminales o no terminales separados\npor espacios en blanco o tabuladores.";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    textoAmostrar="\n\nEl fichero podrá contener comentarios colocados entre los distintos conjuntos, nunca\ndentro de ellos, aunque se podrán escribir comentarios detrás de cada regla del\nconjunto de producciones, en la misma línea que éstas. Los comentarios se iniciarán\n con el símbolo //// y terminará en el final de la línea.";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    textoAmostrar="\n\nPor tanto, la estructura del fichero de la gramática sería la siguiente:\n\n\nTerminales = { term1 term2 term3 ......... termN }\nNoTerminales = { nterm1 nterm2 nterm3 ....... ntermM }\n//// Comentario entre conjuntos\nAxioma = ntermX\nProducciones = {\nAnt1 -> Cons1A Cons1B ... Cons1I\n";
    textoAmostrar= textoAmostrar + "\n//// Comentario\nAnt2 -> Cons2A Cons2B ... Cons2J\n//// Comentario\n. . .\nAntK -> ConsKA ConsKB ... ConsKL\n//// Comentario\n}";
    gtk_text_buffer_insert_at_cursor(buffer1, textoAmostrar.c_str(),textoAmostrar.size());

    gtk_text_view_set_buffer (GTK_TEXT_VIEW(textViewG),GTK_TEXT_BUFFER(buffer1));
    gtk_text_view_set_pixels_inside_wrap(GTK_TEXT_VIEW(textViewG),5);




    textViewM = gtk_text_view_new();
    gtk_text_view_set_editable (GTK_TEXT_VIEW(textViewM),FALSE);
    textoAmostrar = "El funcionamiento de esta aplicación es el siguiente:\n\n   - Primero se cargara un archivo que contendrá la gramática, si se produce un\n    error se mostrará en el panel de Informe.\n";
    textoAmostrar = textoAmostrar + "   - Una vez cargada la gramática podrá o bien cargar otra gramática o cargar\n    un parse para esa gramática, si se producen errores se mostrará en el panel\n   Informe.\n";
    textoAmostrar = textoAmostrar + "   - Si la gramática y el parse han sido cargados correctamente, podrá seleccionar\n    la opción Construir árbol para construirlo, si se producen errores  durante\n  la construcción se mostrará en el panel Informe.\n";
    textoAmostrar = textoAmostrar + "   - La opción Eliminar árbol, le permitira borrar el árbol construido y cerrar la\n    gramática y el parse cargados.\n";
    textoAmostrar = textoAmostrar + "   - Si con el árbol construido selecciona la opción cargar gramática o \n    parse y selecciona un archivo, el árbol será eliminado.\n";
    textoAmostrar = textoAmostrar + "   - Si se selecciona cargar gramática cuando ya ha sido cargado un parse este se\n    cerrará.";
    textoAmostrar = textoAmostrar +"\n   - Solo podrá seleccionar archivos con extension .txt.";
    textoAmostrar = textoAmostrar + "\n\nNota:\n\n La aplicación no permite caracteres que no sean ASCI I.";
    buffer2 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textViewM));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer2),textoAmostrar.c_str(),-1);
    gtk_text_view_set_buffer (GTK_TEXT_VIEW(textViewM),GTK_TEXT_BUFFER(buffer2));
    gtk_text_view_set_pixels_inside_wrap(GTK_TEXT_VIEW(textViewM),5);




// Insertamos el cuadro de texto en los scroolbar

    gtk_container_add ( GTK_CONTAINER(sw), textViewG);
    gtk_container_add ( GTK_CONTAINER(sw1), textViewP);
    gtk_container_add ( GTK_CONTAINER(sw2), textViewM);
    //Establecemos el titulos de las páginas
    labelGramatica = gtk_label_new ("Formato de la Gramática");
    labelParse = gtk_label_new ("Formato del Parse");
    labelManual = gtk_label_new ("Manual");

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    //Insertamos las paginas
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), sw, labelGramatica);
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), sw1, labelParse);
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), sw2, labelManual);

    ventana = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_border_width (GTK_CONTAINER (ventana), 10);
    gtk_container_add ( GTK_CONTAINER(ventana), notebook);

    gtk_window_set_title (GTK_WINDOW (ventana), "Ver ayuda");
    gtk_window_set_default_size (GTK_WINDOW (ventana), 665, 300);
    gtk_widget_show_all ((ventana) );
}





//El nodo se mostrará asi mismo y asu hijos
void derivaHijos(list <Nodo> ListaAux, list<Nodo>::iterator Elemento,GtkTreeStore *modelAux,GtkTreeIter padre){
    GtkTreeIter child;
    char buffer [5];
    string elemsimb;
    list<Nodo>::iterator k;
    vector < list<Nodo> ::iterator> hijosAux;
    hijosAux = Elemento->dameHijos();


    for (unsigned int i=0;i<hijosAux.size();i++){
        //Obtenemos el nodo y su simbolo
        k = hijosAux.at(i);
        elemsimb = k->getSimbolo() ;
        if (0==k->dameReglaDeriv()){
            //Es nodo hoja (no deriva en ninguna regla
            gtk_tree_store_append (modelAux, &child, &padre);
            gtk_tree_store_set (modelAux, &child,COL_DATA, elemsimb.c_str(),-1);
        }
        else{
            //Es nodo NoTerminal, mostraremos la regla por la uqe deriva
            sprintf(buffer,"%d",k->dameReglaDeriv());
            elemsimb = elemsimb + "     ("+buffer+")";
            gtk_tree_store_append (modelAux, &child, &padre);
            gtk_tree_store_set (modelAux, &child,COL_DATA, elemsimb.c_str(),-1);
        }
        if (k->tieneHijos()){//Si tiene hijos les indica que se muestren
            derivaHijos(ListaAux,k,modelAux,child);
        }
    }
}

//Muestra el árbol por pantalla
GtkTreeModel *create_model (GtkTreeStore* model,list <Nodo> ListaAux)
{
        char buffer [5];
        GtkTreeIter top;
        list<Nodo>  bubu;
        string elemsimb;
        list<Nodo>::iterator j,k;
        vector < list<Nodo> ::iterator> hijosAux;
       j=ListaAux.begin();


    gtk_tree_store_append (model, &top, NULL);
    elemsimb = j->getSimbolo() ;
    sprintf(buffer,"%d",j->dameReglaDeriv());
    elemsimb = elemsimb + "     ("+buffer+")";
    gtk_tree_store_set (model, &top,COL_DATA,  elemsimb.c_str(),-1);

    if (j->tieneHijos()){
        //Llanma a que cada hijo se muestre asi mismo y a sus hijos
        derivaHijos(ListaAux, j,model, top);
    }
    return GTK_TREE_MODEL (model);
}

// Dependiendo de si es visble el widget que se le pasa como parametro lo oculta o lo muestra
//Oculta o muestra el panel de informe
void OcultarMostrarInforme(GtkButton *widget, gpointer  data){
    if (gtk_widget_get_visible (GTK_WIDGET(data))){
        gtk_button_set_label (widget, "Mostrar Informe");
        gtk_widget_set_visible (GTK_WIDGET(data),FALSE);
    }
    else {
        gtk_button_set_label (widget, "Ocultar Informe");
        gtk_widget_set_visible (GTK_WIDGET(data),TRUE);
    }
}

// Oculta o muestra el panel de archivos
void OcultarMostrarArchivos(GtkButton *widget, gpointer  data){
    if (gtk_widget_get_visible (GTK_WIDGET(data))){
        gtk_button_set_label (widget, "Mostrar Archivos");
        gtk_widget_set_visible (GTK_WIDGET(data),FALSE);
    }
    else {
        gtk_button_set_label (widget, "Ocultar Archivos");
        gtk_widget_set_visible (GTK_WIDGET(data),TRUE);
    }

}


// Oculta el arbol y lo muestra dependiendo de como este
void OcultarMostrarArbol(GtkButton *widget, gpointer  data){

    if (gtk_widget_get_visible (GTK_WIDGET(data))){
        gtk_button_set_label (widget, "Mostrar Árbol");
        gtk_widget_set_visible (GTK_WIDGET(data),FALSE);
    }
    else {
        gtk_button_set_label (widget, "Ocultar Árbol");
        gtk_widget_set_visible (GTK_WIDGET(data),TRUE);
    }

}

// Iniciará la funcionalidad de cargar gramática
void SelArchivoGramatica(GtkButton *widget, CjtoDataWidget* data){
    char *filename;
    char line[2500];
    int nlineasAux =0;
    string errorAux;
    vector <string> *ListaAux;
    GtkWidget *pFileSelection;
    GtkListStore *model,*model1;
    GtkTreeIter iter;
    bool gram;
  //  GtkWidget *pDialog;
    /* Creación de la ventana de selección */
    pFileSelection = gtk_file_chooser_dialog_new("Abrir fichero...",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                      NULL);
  //Se situa en el ultimo directorio
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(pFileSelection),data->dir);

    /* Limitar las acciones a esta ventana */
    gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

    //Crea el filtro para los archivos a buscar
    GtkFileFilter *filter = gtk_file_filter_new ();
    gtk_file_filter_add_pattern (filter, "*.txt");
    //Añade el filtro al selector de archivos
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (pFileSelection),filter);
    if (gtk_dialog_run (GTK_DIALOG (pFileSelection)) == GTK_RESPONSE_ACCEPT)
          {
            //Borramos el árbol
            gtk_tree_store_clear (GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem1))));
            //Elimina los datos que pudiesen tener las lista que muestran los ficheros
            gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem3))));
            gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem2))));
            model =GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem2)));

            gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4))));
            model1 =GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4)));
            //Almacena en filename el nombre enviado
            filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileSelection));
            strncpy(data->dir,gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileSelection)),149);
            data->dir[149]='\0';

            gtk_entry_set_text (GTK_ENTRY(data->elem5), filename);  //Inserto la ruta como nombre del fichero de la gramatica
            gtk_entry_set_text (GTK_ENTRY(data->elem6),""); //Borro el nombre del archivo del parse

            gtk_notebook_set_current_page (GTK_NOTEBOOK(data->elem16),0);   //Pongo la pagina de la gramatica en el notebook
            //    Lee el fichero y lo va insertando en los TreeView (listStore) de la gramatica
            ifstream f(filename, ifstream::in);
            if (!f){
                gtk_list_store_append (model1, &iter);
                gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"no se puede abrir el archivo", -1);
            }
            else {

                if (*data->estado ==1){
                    gram=data->elemControlador ->CargarGramatica(filename);
                }
                else{
                    gram=data->elemControlador ->CambiarGramatica(filename);
                }
                    gtk_widget_set_sensitive(GTK_WIDGET(data->elem9),FALSE);//Desactiva construir arbol
                    gtk_widget_set_sensitive(GTK_WIDGET(data->elem12),FALSE);//Desactiva eliminar arbol
                    gtk_widget_set_sensitive(GTK_WIDGET(data->elem15),FALSE);//Desactiva construir arbol del menu
                    if (gram)
                    {//Gramatica correcta

                        *data->estado =2;
                        //Desactivamos los botones que se tengan que desactivar, e activamos otros
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem8),TRUE); //Activamos cargar parse
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem14),TRUE); //Activamos cargar parse del menu
                        //Mostramos el informe
                        gtk_list_store_append (model1, &iter);
                        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Gramática cargada correctamente", -1);
                    }
                    else{
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem8),FALSE);    //Desactivamos cargar parse
                        //Error, estado el inicial
                        *data->estado =1;
                        ListaAux = data->elemControlador->DameListaErrores();

                        gtk_list_store_append (model1, &iter);
                        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Detectado errores en el fichero de la gramática", -1);

                        for (unsigned int i=0;i<ListaAux->size();i++){

                            gtk_list_store_append (model1, &iter);
                            errorAux = ListaAux -> at(i);
                            gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,errorAux.c_str(), -1);
                        }
                    }
                nlineasAux=0;
                 while (!f.eof() && (nlineasAux< 2000)){
                     //Mostramos el archivo de la gramatica en el panel de archivo
                    f.getline(line, 2499, '\n');
                    nlineasAux++;
                    gtk_list_store_append (model, &iter);

                    gtk_list_store_set (model, &iter,
                                    COLUMN_PRODUCT,
                                    line,
                                    -1);
                   }
            }
            //Libera filename
            free (filename);
            }
        gtk_widget_destroy(pFileSelection);
}



// Iniciará la funcionalidad de cargar parse
void SelArchivoParse(GtkButton *widget, CjtoDataWidget* data){
    char *filename;
    char line[2500];
    int nlineasAux =0;
    vector <string> *ListaAux;
    string errorAux;
   GtkWidget *pFileSelection;
   GtkListStore *model,*model1;
    GtkTreeIter iter;
    bool pars;
    /* Creación de la ventana de selección */
    pFileSelection = gtk_file_chooser_dialog_new("Abrir fichero...",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                      NULL);
    //Se situa en la ultima ruta abierta
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(pFileSelection),data->dir);
    /* Limitar las acciones a esta ventana */
    gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

    //Crea el filtro para los archivos a buscar
    GtkFileFilter *filter = gtk_file_filter_new ();
    gtk_file_filter_add_pattern (filter, "*.txt");

    //Añade el filtro al selector de archivos
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (pFileSelection),filter);

    if (gtk_dialog_run (GTK_DIALOG (pFileSelection)) == GTK_RESPONSE_ACCEPT)
          { //Borramos el árbol
            gtk_tree_store_clear (GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem1))));
            //Elimina los datos que pudiese tener la lista
            gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem3))));
            model =GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem3)));
            //Elimina los datos que hubiese en el panel informe
            gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4))));
            model1 =GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4)));
            //Almacena en filename el nombre enviado
            filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileSelection));
            //Almacenamos la ruta del archivo
            strncpy(data->dir,gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pFileSelection)),149);
            data->dir[149]='\0';


            gtk_entry_set_text (GTK_ENTRY(data->elem6), filename);  //Muestra la ruta como nombre del archivo del parse
            gtk_notebook_set_current_page (GTK_NOTEBOOK(data->elem16),1);   //Pongo la pagina del parse

            //    Lee el fichero y lo va insertando en los TreeView (listStore) de la gramatica
            ifstream f(filename, ifstream::in);
            if (!f){
                gtk_list_store_append (model1, &iter);
                gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"no se puede abrir el archivo", -1);
            }
            else {
                //estado 2->  (gramatica ya cargado)
                if (*data->estado ==2){
                    // No hay parse
                    pars=data->elemControlador -> CargarParse(filename);
                }
                else{
                    //Indica que ya existe un parse
                    pars=data->elemControlador ->CambiarParse(filename);
                }
                    if (pars)
                    {
                        //Indico que el parse se ha cargado
                         *data->estado =3;
                        //Desactivamos los botones que se tengan que desactivar, e activamos otros
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem9),TRUE); //Activamos construir árbol
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem12),FALSE);//Desactiva eliminar arbol
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem15),TRUE); //Activamos construir árbol del menú
                        //Mostramos el informe

                        gtk_list_store_append (model1, &iter);
                        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Gramática cargada correctamente", -1);
                        gtk_list_store_append (model1, &iter);
                        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Parse cargado correctamente", -1);

                    }
                    else{
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem9),FALSE);//Desactiva construir arbol
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem12),FALSE);//Desactiva eliminar arbol
                        gtk_widget_set_sensitive(GTK_WIDGET(data->elem15),FALSE);//Desactiva construir arbol del menu

                        //Parse incorrecto, por lo tanto el estado es con la gramatica correcta
                         *data->estado =2;
                        ListaAux = data->elemControlador->DameListaErrores();

                        gtk_list_store_append (model1, &iter);
                        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Gramática cargada correctamente", -1);
                        gtk_list_store_append (model1, &iter);
                        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Detectado errores en el fichero del parse", -1);

                        for (unsigned int i=0;i<ListaAux->size();i++){

                            gtk_list_store_append (model1, &iter);
                            errorAux = ListaAux -> at(i);
                            gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,errorAux.c_str(), -1);
                        }
                    }

                nlineasAux=0;
                 while (!f.eof()&& (nlineasAux< 2000)){
                     //Mostramos el parse en el panel del archivo
                    f.getline(line, 2499, '\n');
                     nlineasAux ++;
                    gtk_list_store_append (model, &iter);

                    gtk_list_store_set (model, &iter,
                                    COLUMN_PRODUCT,
                                    line,
                                    -1);
                   }
            }
            //Libera filename
           free (filename);
            }
        gtk_widget_destroy(pFileSelection);
}


// Expande el arbol que se le pasa como parametro data
void ExpandirArbol(GtkButton *widget, gpointer data){
    gtk_tree_view_expand_all (GTK_TREE_VIEW (data));
}

//Iniciará la funcionalidad EliminarArbol
//Limpiará todos los paneles y pondrá el estado inicial
void EliminarArb(GtkButton *widget, CjtoDataWidget* data){
    gtk_tree_store_clear (GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem1)))); //Borramos el arbol
    gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem2)))); //Borramos el view del archivo de la gramatica
    gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem3)))); //Borramos el view del archivo del parse
    gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4)))); //Borramos el view del informe
    gtk_entry_set_text (GTK_ENTRY(data->elem5), "");
    gtk_entry_set_text (GTK_ENTRY(data->elem6), "");
    gtk_widget_set_sensitive(GTK_WIDGET(data->elem7),TRUE);    //Activamos el boton de cargar gramatica
    gtk_widget_set_sensitive(GTK_WIDGET(data->elem8),FALSE);    //Descativamos el boton de cargar parse
    gtk_widget_set_sensitive(GTK_WIDGET(data->elem14),FALSE);    //Desactivamos el boton de menu cargar parse
    gtk_widget_set_sensitive(GTK_WIDGET(widget),FALSE);
    //data->archBuscar = "V"; //Indicamos estado inicial

     *data->estado =1;
    data->elemControlador ->EliminarArbol();
}


//Contrae al árbol sintáctico
void ContraerArbol(GtkButton *widget, gpointer data){
    gtk_tree_view_collapse_all (GTK_TREE_VIEW (data));
}


//Iniciará la funcionalidad construir arbol, solo esterá disponible cuando se cumplan los PRe impuestos en el controlador
void ConstArbol(GtkButton *widget, CjtoDataWidget* data){
    GtkTreeStore *model;
    GtkListStore *model1;
    GtkTreeIter iter;
    string errorAux;
    list <Nodo> ListaAux;
    bool arb;
    vector <string> *ListaAuxError;
    //Limpiamos el panel de informe
    gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4))));
    model1 =GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem4)));
    //Limpiamos el panel del árbol
    model =GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(data->elem1)));

    arb = data->elemControlador-> ConstruirArbol();     //Construimos la estructura del árbol

    if (arb){
        ListaAux = data->elemControlador -> DameLista();
        create_model (model,ListaAux);
        gtk_list_store_append (model1, &iter);
        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Gramática cargada correctamente", -1);
        gtk_list_store_append (model1, &iter);
        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Parse cargado correctamente", -1);

        gtk_list_store_append (model1, &iter);
        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Árbol sintáctico construido correctamente", -1);

        gtk_tree_view_expand_all (GTK_TREE_VIEW (data->elem1));
        gtk_widget_set_sensitive(GTK_WIDGET(widget),FALSE); //Desactivamos construir arbol
        gtk_widget_set_sensitive(GTK_WIDGET(data->elem15),FALSE); //Desactivamos construir arbol
        gtk_widget_set_sensitive(GTK_WIDGET(data->elem12),TRUE); //Activamos eliminar arbol
    }
    else{//Error construyendo el árbol
        gtk_list_store_append (model1, &iter);
        gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,"Error al construir el árbol sintáctico", -1);

        //Ha habido un fallo al construir el árbol
        //Mostramos la lista de errores
        ListaAuxError = data->elemControlador->DameListaErrores();
        for (unsigned int i=0;i<ListaAuxError->size();i++){
            gtk_list_store_append (model1, &iter);
            errorAux = ListaAuxError -> at(i);
            gtk_list_store_set (model1, &iter,COLUMN_PRODUCT,errorAux.c_str(), -1);
        }
    }
}


//Creamos la estructura que tendrá al árbol sintáctico
GtkWidget * create_view_and_model ()
{
        GtkWidget *view;
        GtkTreeModel *model;
        GtkTreeViewColumn *col;
        GtkCellRenderer *renderer;

        view = gtk_tree_view_new ();

        col = gtk_tree_view_column_new ();
        gtk_tree_view_column_set_title (col, "Arbol Sintáctico");

        gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

        renderer = gtk_cell_renderer_text_new ();
        gtk_tree_view_column_pack_start (col, renderer, TRUE);
        gtk_tree_view_column_add_attribute  (col, renderer, "text",
                                            COL_DATA);

        model = GTK_TREE_MODEL(gtk_tree_store_new (COL_TOTAL, G_TYPE_STRING));
        //Muestra las lineas entre los hijos y los padres
        gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW (view),TRUE);

        gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
        //Expandimos todo el árbol
        gtk_tree_view_expand_all (GTK_TREE_VIEW (view));

        g_object_unref (model);

        return view;
}

gboolean
app_quit (GtkWidget *widget, GdkEvent *event,
          gpointer data)
{
        gtk_main_quit ();
        return TRUE;
}
//Creamos la columna que tendrá al archivo
static void
add_columns (GtkTreeView *treeview)
{
        GtkCellRenderer *renderer;
        renderer = gtk_cell_renderer_text_new ();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                     -1, "Archivo", renderer,
                                                     "text", COLUMN_PRODUCT,
                                                     NULL);
}
//Creamos la columna que tendrá al informe
static void
add_columns_inf (GtkTreeView *treeview)
{
        GtkCellRenderer *renderer;
        renderer = gtk_cell_renderer_text_new ();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                     -1, "Informe", renderer,
                                                     "text", COLUMN_PRODUCT,
                                                     NULL);
}

//Resaltará la linea del panel de fichero de la gramatica correspondiente a un error de la gramatica
void ResaltarLinea (GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,CjtoWidgetBuscar *data){
        GtkTreeIter iter;
        GtkTreeSelection *selection;
        selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (data->elem1));
        GtkTreeModel *model;
        GtkTreePath *miPath;
        gchar *author;
        int lin=0,i=6;
        char auxChar[2];
        string prueba;
        if (*data->estado ==1){
            gtk_notebook_set_current_page (GTK_NOTEBOOK(data->elem4),0);

        if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
                gtk_tree_model_get (model, &iter, COLUMN_PRODUCT, &author, -1);
                prueba= author;
                //El numero de linea empieza en la pos 6
                if (prueba[i] >= '0' && prueba[i] <= '9'){//Comrpueba que es el numero de linea
                    while (prueba[i]!=':' ){
                        auxChar[0] = prueba[i];
                        lin= lin*10 + atoi(auxChar);
                        i++;
                    }
                }
               free (author);
        }
        if (prueba[6] >= '0' && prueba[6] <= '9'){//Comprobamos que es un número
            miPath = gtk_tree_path_new_first ();
            for (int j=1;j<lin;j++){
                gtk_tree_path_next(miPath);
            }
            gtk_tree_view_set_cursor (GTK_TREE_VIEW (data->elem2),miPath, NULL,FALSE);
        }
    }
}



//Creamos el modelo del arbol
GtkWidget * creaModeloTree (void){
        GtkWidget *treeview;
        GtkTreeModel *model;
     model = GTK_TREE_MODEL(gtk_list_store_new (NUM_COLUMNS, G_TYPE_STRING));
        treeview = gtk_tree_view_new_with_model (model);
        g_object_unref (G_OBJECT (model));

        add_columns (GTK_TREE_VIEW (treeview));
        return treeview;
}


//Crea el ListStore para el informe
GtkWidget * do_cells (void){
                GtkWidget *treeview;
                GtkTreeModel *model;
                /* crear el modelo */
        model = GTK_TREE_MODEL(gtk_list_store_new (NUM_COLUMNS, G_TYPE_STRING));

                /* crear tree view */
                treeview = gtk_tree_view_new_with_model (model);
                g_object_unref (G_OBJECT (model));

                add_columns_inf (GTK_TREE_VIEW (treeview));

                return treeview;
}



int
main (int argc, char **argv)
{

        GtkWidget *window;
        GtkWidget *view,*viewInforme,*viewGramatica,*viewParse;
        GtkWidget *sw,*sw2,*sw3,*sw4;
        GtkWidget *vbox,*vbox2,*hbox,*hbox1,*hbox2,*hboxNarchivoG,*hboxNarchivoP;
        GtkWidget *buttonOcultArbol,*buttonCGramatica,*bExpandirArbol,*buttonBuscar,*buttonContraerArbol,*buttonCParse,*buttonOcultInforme,*buttonOcultArchivos;
        GtkWidget *buttonConstruir,*buttonCambiarG,*buttonCambiarP,*buttonEliminarArbol;
        GtkWidget *menuBar,*menu,*menuVer,*menuAyuda;
        GtkWidget *MCargarGramatica,*MCargarParse,*Salir,*Archivo_item,*Ayuda_item,*Ver_item;
        GtkWidget *MContraerArbol,*MExpandirArbol,*MConstruir;
        GtkWidget *MAyudaFormatos;
        GtkWidget *nArchivoG,*nArchivoP;
        GtkWidget *vpaned,*hpaned;
        GtkWidget *NotebookArchivos;
        CjtoDataWidget *dataWidget;

CjtoWidgetBuscar *dataBuscar;
//char *estadoGram;
int estadoGram;
Controlador * ControlAux;
// Creo el comtrolador principal del programa
ControlAux = new Controlador;



        gtk_init (&argc, &argv);

        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

        g_signal_connect (window, "delete_event",
                          G_CALLBACK (app_quit), NULL);


//* *   *   *   *   *   *   MENU       *   *   *   *

    menuBar = gtk_menu_bar_new();
    menu = gtk_menu_new();
    menuVer = gtk_menu_new();
    menuAyuda = gtk_menu_new();
    //Establecemos las opciones del menu
    MCargarGramatica = gtk_menu_item_new_with_label ("Cargar Gramática      F2");
    MCargarParse = gtk_menu_item_new_with_label ("Cargar Parse      F3");
    MConstruir = gtk_menu_item_new_with_label ("Construir árbol      F4");
    Salir = gtk_menu_item_new_with_label ("Salir");
    MAyudaFormatos = gtk_menu_item_new_with_label ("Ver ayuda      F1");
    MExpandirArbol = gtk_menu_item_new_with_label ("Expandir árbol      F5");
    MContraerArbol = gtk_menu_item_new_with_label ("Contraer árbol      F6");



//* *   *   *   *   *   *   *   *   *   *   *   *   *       **
//Añade los objetos que tendrán los submenus
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), MCargarGramatica);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), MCargarParse);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), MConstruir);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), Salir);
    gtk_menu_shell_append (GTK_MENU_SHELL (menuAyuda), MAyudaFormatos);
    gtk_menu_shell_append (GTK_MENU_SHELL (menuVer), MExpandirArbol);
    gtk_menu_shell_append (GTK_MENU_SHELL (menuVer), MContraerArbol);





    Archivo_item = gtk_menu_item_new_with_label("Archivo");
    Ver_item =gtk_menu_item_new_with_label("Ver");
    Ayuda_item = gtk_menu_item_new_with_label("Ayuda");

    gtk_widget_show(Archivo_item);

    //Añade los submenus
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (Archivo_item), menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (Ver_item), menuVer);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (Ayuda_item), menuAyuda);


    gtk_menu_bar_append( GTK_MENU_BAR (menuBar), Archivo_item );
    gtk_menu_bar_append( GTK_MENU_BAR (menuBar), Ver_item );
    gtk_menu_bar_append( GTK_MENU_BAR (menuBar), Ayuda_item );






        vbox = gtk_vbox_new (FALSE, 5);
        vbox2 = gtk_vbox_new (FALSE, 5);
        hbox = gtk_hbox_new (FALSE,5);
        hbox1 =gtk_hbox_new (FALSE,5);
        //Establece la hbox que contendra a la ruta del archivo de la grmatica
        hboxNarchivoG = gtk_hbox_new(FALSE,10);
        //Establece la hbox que contendra a la ruta del archivo del parse
        hboxNarchivoP = gtk_hbox_new(FALSE,52);

        //Contenedor general -> vbox
        gtk_container_add (GTK_CONTAINER (window), vbox);
        gtk_container_set_border_width(GTK_CONTAINER(vbox),2);
        //gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
        //  *   *   *   **  *   **  *
        //Menu
        gtk_box_pack_start (GTK_BOX (vbox),menuBar, FALSE,FALSE,1);
       // gtk_box_pack_start (GTK_BOX (vbox),barraHerramientas, FALSE,FALSE,1);
        //  *   *   *   **  *   *   *   *

        //eSTO ya estaba eliminado// gtk_container_add (GTK_CONTAINER (vbox),hbox);

        //gtk_container_add (GTK_CONTAINER (vbox),vbox2);
        gtk_box_pack_start (GTK_BOX (vbox),vbox2, FALSE,FALSE,1);
        // gtk_container_add (GTK_CONTAINER (vbox2),hbox);
        gtk_box_pack_start (GTK_BOX (vbox2),hbox, FALSE,FALSE,1);
        gtk_box_pack_start (GTK_BOX (vbox2),hbox1, FALSE,FALSE,1);

     //   gtk_container_add (GTK_CONTAINER (vbox2),hbox);



        buttonCGramatica = gtk_button_new_with_label ("Cargar Gramática");
        gtk_box_pack_start (GTK_BOX (hbox), buttonCGramatica, FALSE, TRUE, 5);
        gtk_widget_show (buttonCGramatica);

        //Botón para cargar el parse
        buttonCParse = gtk_button_new_with_label ("Cargar Parse");
        gtk_box_pack_start (GTK_BOX (hbox), buttonCParse, FALSE, TRUE,5);
        gtk_widget_show (buttonCParse);

        //Boton para construir el árbol
        buttonConstruir = gtk_button_new_with_label ("Construir Árbol");
        gtk_box_pack_start (GTK_BOX (hbox), buttonConstruir, FALSE, TRUE, 5);
        gtk_widget_show (buttonConstruir);

        //Boton para Cambiar el archivo de la gramática
        buttonCambiarG = gtk_button_new_with_label ("Cambiar gramática");
       /* gtk_box_pack_start (GTK_BOX (hbox), buttonCambiarG, FALSE, TRUE, 5);
        gtk_widget_show (buttonCambiarG);
*/
        //Boton para cambiar el archivo del parse
        buttonCambiarP = gtk_button_new_with_label ("Cambiar parse");
   /*     gtk_box_pack_start (GTK_BOX (hbox), buttonCambiarP, FALSE, TRUE, 5);
        gtk_widget_show (buttonCambiarP);*/


        //Boton para Eliminar el árbol
        buttonEliminarArbol = gtk_button_new_with_label ("Eliminar árbol");
        gtk_box_pack_start (GTK_BOX (hbox), buttonEliminarArbol, FALSE, TRUE, 5);
        gtk_widget_show (buttonEliminarArbol);


        bExpandirArbol = gtk_button_new_with_label ("expandirArbol");
        gtk_box_pack_start (GTK_BOX (hbox1), bExpandirArbol, FALSE, TRUE,5);
        gtk_widget_show (bExpandirArbol);

        buttonContraerArbol = gtk_button_new_with_label ("ContraerArbol");
        gtk_box_pack_start (GTK_BOX (hbox1), buttonContraerArbol, FALSE, TRUE,5);
        gtk_widget_show (buttonContraerArbol);

gtk_widget_set_size_request(hbox,20,28);
gtk_widget_set_size_request(hbox1,20,28);



    //Boton para ocultar Arbol
        buttonOcultArbol = gtk_button_new_with_label ("Ocultar Árbol");
        gtk_box_pack_start (GTK_BOX (hbox1), buttonOcultArbol, FALSE, TRUE,5);
        gtk_widget_show (buttonOcultArbol);
    //Boton para ocultar Archivos
        buttonOcultArchivos = gtk_button_new_with_label ("Ocultar Ventana Archivos");
        gtk_box_pack_start (GTK_BOX (hbox1), buttonOcultArchivos, FALSE, TRUE, 5);
        gtk_widget_show (buttonOcultArchivos);
    //Boton para ocultar Informe
        buttonOcultInforme = gtk_button_new_with_label ("Ocultar Ventana Informe");
        gtk_box_pack_start (GTK_BOX (hbox1), buttonOcultInforme, FALSE, TRUE, 5);
        gtk_widget_show (buttonOcultInforme);
    //Boton para buscar
        buttonBuscar = gtk_button_new_with_label ("Buscar");
        /*gtk_box_pack_start (GTK_BOX (hbox1), buttonBuscar, FALSE, TRUE, 5);
        gtk_widget_show (buttonBuscar);*/


gtk_widget_set_size_request(hboxNarchivoG,20,28);
gtk_widget_set_size_request(hboxNarchivoP,20,28);

        gtk_box_pack_start (GTK_BOX (vbox2),hboxNarchivoG, FALSE,TRUE,1);

//Hbox para el archivo de la gramatica
nArchivoG = gtk_entry_new ();
gtk_entry_set_editable  (GTK_ENTRY(nArchivoG), FALSE);    //Esta llamada es de codigo anticuado

        gtk_box_pack_start (GTK_BOX (hboxNarchivoG),
                                    gtk_label_new ("Archivo de la gramática"),
                                    FALSE, FALSE, 0);

        gtk_box_pack_start (GTK_BOX (hboxNarchivoG), nArchivoG, TRUE, TRUE, 2);
//Hbox para el archivo del parse
nArchivoP = gtk_entry_new ();
            gtk_entry_set_editable  (GTK_ENTRY(nArchivoP), FALSE);    //Esta llamada es de codigo anticuado

gtk_box_pack_start (GTK_BOX (vbox2),hboxNarchivoP, FALSE,FALSE,1);

       gtk_box_pack_start (GTK_BOX (hboxNarchivoP),
                                    gtk_label_new ("Archivo del parse"),
                                    FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (hboxNarchivoP), nArchivoP, TRUE, TRUE, 2);



        hbox2 = gtk_hbox_new (FALSE,4);
        //vpaned -> dividira el panel de (arbol, archivos) con el informe
        vpaned = gtk_vpaned_new ();
        gtk_container_add (GTK_CONTAINER(vbox), vpaned);
        hpaned = gtk_hpaned_new();


GtkWidget * labelGramatica,*labelParse;
//Crea las etiquetas que se mostraran como titulos de las páginas del notebook
labelGramatica = gtk_label_new ("Gramática");
labelParse = gtk_label_new ("Parse");

NotebookArchivos = gtk_notebook_new();
gtk_notebook_set_tab_pos (GTK_NOTEBOOK(NotebookArchivos), GTK_POS_TOP);




       // vpaned =gtk_vpaned_new ();

     //   gtk_container_add (GTK_CONTAINER (vbox),hbox2);

//scroll para el arbol sintáctico
        sw = gtk_scrolled_window_new (NULL, NULL);
                gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);
// scroll para el informe
        sw2 = gtk_scrolled_window_new (NULL, NULL);
                        gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw2),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw2),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);
//scroll para el archivo de gramatica
        sw3 = gtk_scrolled_window_new (NULL, NULL);
                        gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw3),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw3),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);
//scroll para el archivo del parse
        sw4 = gtk_scrolled_window_new (NULL, NULL);
                        gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw4),
                                                     GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw4),
                                                GTK_POLICY_AUTOMATIC,
                                                GTK_POLICY_AUTOMATIC);
//Mete en el hpaned al scrroll bar que contiene el arbol y el notebook con la gramatica y el parse
  gtk_paned_pack1        (GTK_PANED(hpaned),sw,TRUE,FALSE);
  gtk_paned_pack2        (GTK_PANED(hpaned),NotebookArchivos,TRUE,FALSE);

//Ponemos las dimensiones minimas de los paneles
    gtk_widget_set_size_request (GTK_WIDGET(hpaned),0,27);
    gtk_widget_set_size_request (GTK_WIDGET(sw2),0,22);
    gtk_paned_set_position   (GTK_PANED(hpaned),500);

    gtk_paned_set_position   (GTK_PANED(vpaned),400);


  gtk_paned_pack1        (GTK_PANED(vpaned),hpaned,TRUE,FALSE);
  gtk_paned_pack2        (GTK_PANED(vpaned),sw2,TRUE,FALSE);


    view = create_view_and_model ();
    viewInforme = do_cells();
    viewGramatica =creaModeloTree();
    viewParse =creaModeloTree();

    gtk_tree_view_expand_all (GTK_TREE_VIEW (view));

//Introducimos los  treeView en los scrollbar
    gtk_container_add (GTK_CONTAINER (sw), view);
    gtk_container_add (GTK_CONTAINER (sw2), viewInforme);
    gtk_container_add (GTK_CONTAINER(sw3),viewGramatica);
    gtk_container_add (GTK_CONTAINER(sw4),viewParse);

    //Añade los ficheros sroll de la gramatica y el parse en el notebook
    gtk_notebook_append_page (GTK_NOTEBOOK (NotebookArchivos), sw3, labelGramatica);
    gtk_notebook_append_page (GTK_NOTEBOOK (NotebookArchivos), sw4, labelParse);


gtk_widget_show(sw3);
gtk_widget_show(sw4);
gtk_notebook_next_page(GTK_NOTEBOOK (NotebookArchivos));
gtk_notebook_prev_page (GTK_NOTEBOOK (NotebookArchivos));


gtk_window_set_default_size (GTK_WINDOW (window), 1050, 750);
gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
//Estado inicial de las estructuras
//1 -> sin nada, 2-> gramatica cargada, 3-> parse cargado
//estadoGram ="V";

estadoGram = 1;


    dataBuscar = new (CjtoWidgetBuscar);
    dataBuscar ->elem1 = viewInforme;
    dataBuscar ->elem2 = viewGramatica;
    dataBuscar ->elem3 = viewParse;
    dataBuscar ->elem4 = NotebookArchivos;
    dataBuscar -> estado = &estadoGram;

    dataWidget = new (CjtoDataWidget);
    dataWidget ->elem1 = view;
    dataWidget ->elem2 = viewGramatica;
    dataWidget ->elem3 = viewParse;
    dataWidget ->elem4 = viewInforme;
    dataWidget ->elem5 = nArchivoG;
    dataWidget ->elem6 = nArchivoP;
    dataWidget ->elem7 = buttonCGramatica;
    dataWidget ->elem8 = buttonCParse;
    dataWidget ->elem9 = buttonConstruir;
   // dataWidget ->elem10 = buttonCambiarG;//
   // dataWidget ->elem11 = buttonCambiarP;//
    dataWidget ->elem12 = buttonEliminarArbol;
    dataWidget ->elem13 = MCargarGramatica;
    dataWidget ->elem14 = MCargarParse;
    dataWidget ->elem15 = MConstruir;
    dataWidget ->elem16 = NotebookArchivos;
    dataWidget -> estado = &estadoGram;
    strncpy(dataWidget ->dir,"cadena",7);

    dataWidget ->elemControlador =ControlAux;
//* *   *   **  *       *   **  *   *   *

g_signal_connect(G_OBJECT (window),"key_press_event",G_CALLBACK (capturaTeclas), dataWidget);

//* *   *   *   **  *   *   *   *   *   *
//Establecemos la sensibildad inicial de los botones
gtk_widget_set_sensitive(GTK_WIDGET(buttonCParse),FALSE);
gtk_widget_set_sensitive(GTK_WIDGET(buttonCambiarG),FALSE);
gtk_widget_set_sensitive(GTK_WIDGET(buttonCambiarP),FALSE);
gtk_widget_set_sensitive(GTK_WIDGET(buttonConstruir),FALSE);
gtk_widget_set_sensitive(GTK_WIDGET(buttonEliminarArbol),FALSE);
gtk_widget_set_sensitive(GTK_WIDGET(MCargarParse),FALSE);
gtk_widget_set_sensitive(GTK_WIDGET(MConstruir),FALSE);

//estblecemos las fucniones a las que lalma cada boton
    g_signal_connect (G_OBJECT (buttonOcultArbol), "clicked", G_CALLBACK (OcultarMostrarArbol), (gpointer) sw);
    g_signal_connect (G_OBJECT (bExpandirArbol), "clicked", G_CALLBACK (ExpandirArbol), (gpointer) view);
    g_signal_connect (G_OBJECT (buttonContraerArbol), "clicked", G_CALLBACK (ContraerArbol), (gpointer) view);
    g_signal_connect (G_OBJECT (buttonOcultInforme), "clicked", G_CALLBACK (OcultarMostrarInforme), (gpointer) sw2);

    g_signal_connect (G_OBJECT (buttonOcultArchivos), "clicked", G_CALLBACK (OcultarMostrarArchivos), (gpointer) NotebookArchivos);

    g_signal_connect (G_OBJECT (buttonCGramatica), "clicked", G_CALLBACK (SelArchivoGramatica), (gpointer) dataWidget);

    g_signal_connect (G_OBJECT (buttonCParse), "clicked", G_CALLBACK (SelArchivoParse), (gpointer) dataWidget);

    g_signal_connect (G_OBJECT (buttonConstruir), "clicked", G_CALLBACK (ConstArbol), (gpointer) dataWidget);

    g_signal_connect (G_OBJECT (buttonEliminarArbol), "clicked", G_CALLBACK (EliminarArb), (gpointer) dataWidget);

//Recibe la señal de  doble click en el treeStore de Informe
g_signal_connect(G_OBJECT (viewInforme), "row-activated", G_CALLBACK (ResaltarLinea), (gpointer) dataBuscar);


g_signal_connect (MCargarGramatica, "activate",
                              G_CALLBACK (SelArchivoGramatica),
                              (gpointer) dataWidget);
g_signal_connect (MCargarParse, "activate",
                              G_CALLBACK (SelArchivoParse),
                              (gpointer) dataWidget);
g_signal_connect (MConstruir, "activate",
                              G_CALLBACK (ConstArbol),
                              (gpointer) dataWidget);


g_signal_connect (MAyudaFormatos, "activate",
                              G_CALLBACK (verFormatos),
                              (gpointer) dataWidget);

g_signal_connect (Salir, "activate",
                              gtk_main_quit,NULL);
g_signal_connect (MExpandirArbol, "activate",
                              G_CALLBACK (ExpandirArbol),(gpointer) view);
g_signal_connect (MContraerArbol, "activate",
                              G_CALLBACK (ContraerArbol),(gpointer) view);

        gtk_window_set_title (GTK_WINDOW (window), "Visualizador de árboles sintácticos");

        gtk_widget_show_all (window);


        gtk_main ();

        return 0;
}


