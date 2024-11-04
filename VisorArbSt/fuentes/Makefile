ifeq ($(shell pkg-config --cflags gtk+-2.0),) 
  $(error Package gtk+-2.0 necesario para compilar to compile) 
endif 
CXXFLAGSS += $(shell pkg-config --cflags gtk+-2.0)
LDLIBS += $(shell pkg-config --libs gtk+-2.0)

CFLAGSS=-I<.>
OBJETOS= Ttoken.o ALexicoG.o Reglas.o AnalizadorP.o AnalizadorG.o Simbolos.o ListaReglas.o Nodo.o Controlador.o 
FUENTES= Ttoken.h ALexicoG-h Reglas.h AnalizadorP.h AnalizadorG.h Simbolos.h ListaReglas.h Nodo.h Controlador.h 

MiEjecutable: $(OBJETOS)
	g++ $(OBJETOS) $(CXXFLAGSS) VisualizadorArbolesSintacticos.cpp -o VisualizadorArbolesSintacticos $(LDLIBS)

depend:
	makedepend $(CFLGAS) $(FUENTES) 



