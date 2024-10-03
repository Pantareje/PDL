\documentclass{article}
\usepackage[margin=1in]{geometry}
\usepackage{graphicx} % Para insertar imagenes
\usepackage{tikz} % Para los nodos y cosillas del AFD
\usetikzlibrary{automata, positioning}

\title{\textbf{Memoria: Analizador léxico}}
\author{Carmen Toribio Pérez, 22M009\\Sergio Gil Atienza, 22M046\\María Moronta Carrión, 22M111}
\date{}

\begin{document}

\maketitle

\section{Introducción}
Hola aquí hay que poner unpoquillo de texto. "La practica consiste de mimimimi... Hemos decidido usar C++ porque mimimimi..."

\section{Gramática}
Aquí ponemos algo blablabla. \\
Aquí ponemos GR = {todas las cosas de la gramatica} y definimos todo enplan serio fuap.

\section{Autómata Finito Determinista}
Aquí ponemos algo de texto tmb para que quede épico.\\
\begin{center}
\begin{tikzpicture}[scale=0.15]
\tikzstyle{every node}+=[inner sep=0pt]
\draw [black] (38.4,-29.3) circle (3);
\draw (38.4,-29.3) node {$0$};
\draw [black] (38.4,-29.3) circle (2.4);
\draw [black] (7.3,-18.4) circle (3);
\draw (7.3,-18.4) node {$1$};
\draw [black] (6.2,-5.8) circle (3);
\draw (6.2,-5.8) node {$2$};
\draw [black] (6.2,-5.8) circle (2.4);
\draw [black] (15.1,-13.4) circle (3);
\draw (15.1,-13.4) node {$3$};
\draw [black] (13.9,-3.7) circle (3);
\draw (13.9,-3.7) node {$4$};
\draw [black] (13.9,-3.7) circle (2.4);
\draw [black] (31.2,-14.6) circle (3);
\draw (31.2,-14.6) node {$5$};
\draw [black] (31.2,-3.7) circle (3);
\draw (31.2,-3.7) node {$6$};
\draw [black] (42.8,-6.5) circle (3);
\draw (42.8,-6.5) node {$7$};
\draw [black] (42.8,-6.5) circle (2.4);
\draw [black] (49.3,-13.4) circle (3);
\draw (49.3,-13.4) node {$8$};
\draw [black] (66.5,-9.8) circle (3);
\draw (66.5,-9.8) node {$9$};
\draw [black] (66.5,-9.8) circle (2.4);
\draw [black] (73.3,-26.4) circle (3);
\draw (73.3,-26.4) node {$10$};
\draw [black] (73.3,-26.4) circle (2.4);
\draw [black] (57.8,-38.9) circle (3);
\draw (57.8,-38.9) node {$11$};
\draw [black] (70.8,-35.9) circle (3);
\draw (70.8,-35.9) node {$12$};
\draw [black] (70.8,-35.9) circle (2.4);
\draw [black] (56.9,-47.8) circle (3);
\draw (56.9,-47.8) node {$13$};
\draw [black] (67.5,-48.5) circle (3);
\draw (67.5,-48.5) node {$14$};
\draw [black] (67.5,-48.5) circle (2.4);
\draw [black] (18.3,-43.6) circle (3);
\draw (18.3,-43.6) node {$15$};
\draw [black] (13.9,-51.8) circle (3);
\draw (13.9,-51.8) node {$16$};
\draw [black] (4.7,-47.2) circle (3);
\draw (4.7,-47.2) node {$17$};
\draw [black] (29.6,-55.5) circle (3);
\draw (29.6,-55.5) node {$18$};
\draw [black] (29.6,-55.5) circle (2.4);
\draw [black] (12.6,-31.6) circle (3);
\draw (12.6,-31.6) node {$19$};
\draw [black] (12.6,-31.6) circle (2.4);
\draw [black] (35.41,-29.054) arc (-96.15072:-122.4786:59.606);
\fill [black] (9.79,-20.07) -- (10.2,-20.93) -- (10.73,-20.08);
\draw (21.34,-26.57) node [below] {$l$};
\draw [black] (36.458,-27.028) arc (248.25582:-39.74418:2.25);
\draw (35.31,-22.12) node [above] {$del$};
\fill [black] (39.02,-26.38) -- (39.78,-25.82) -- (38.85,-25.45);
\draw [black] (42.9,-21.9) -- (39.96,-26.74);
\fill [black] (39.96,-26.74) -- (40.8,-26.31) -- (39.95,-25.79);
\draw [black] (7.04,-15.41) -- (6.46,-8.79);
\fill [black] (6.46,-8.79) -- (6.03,-9.63) -- (7.03,-9.54);
\draw (6.12,-12.17) node [left] {$o.c.$};
\draw [black] (7.94,-21.319) arc (40.10787:-247.89213:2.25);
\draw (3.61,-25.66) node [below] {$l,\mbox{ }d,\mbox{ }\_$};
\fill [black] (5.37,-20.68) -- (4.44,-20.82) -- (5.08,-21.58);
\draw [black] (35.659,-28.081) arc (-115.2029:-133.41663:70.465);
\fill [black] (17.23,-15.51) -- (17.47,-16.42) -- (18.16,-15.69);
\draw (24.83,-23.03) node [below] {$d$};
\draw [black] (17.112,-11.19) arc (165.42106:-122.57894:2.25);
\draw (22.13,-10.07) node [right] {$d$};
\fill [black] (18.08,-13.65) -- (18.73,-14.34) -- (18.98,-13.37);
\draw [black] (14.73,-10.42) -- (14.27,-6.68);
\fill [black] (14.27,-6.68) -- (13.87,-7.53) -- (14.86,-7.41);
\draw (15.17,-8.42) node [right] {$o.c.$};
\draw [black] (36.047,-27.446) arc (-133.65734:-174.15181:15.848);
\fill [black] (31.22,-17.6) -- (30.81,-18.44) -- (31.8,-18.34);
\draw (32.05,-24.04) node [left] {$'$};
\draw [black] (28.864,-16.463) arc (-23.69198:-311.69198:2.25);
\draw (23.86,-16.4) node [left] {$o.c.$};
\fill [black] (28.3,-13.88) -- (27.77,-13.1) -- (27.37,-14.02);
\draw [black] (28.826,-12.819) arc (-141.47856:-218.52144:5.89);
\fill [black] (28.83,-5.48) -- (27.94,-5.8) -- (28.72,-6.42);
\draw (27.04,-9.15) node [left] {$\backslash$};
\draw [black] (31.2,-6.7) -- (31.2,-11.6);
\fill [black] (31.2,-11.6) -- (31.7,-10.8) -- (30.7,-10.8);
\draw (31.7,-9.15) node [right] {$cesc$};
\draw [black] (41.24,-9.056) arc (-37.41152:-72.73715:14.277);
\fill [black] (41.24,-9.06) -- (40.36,-9.39) -- (41.15,-10);
\draw (38.8,-12.59) node [below] {$'$};
\draw [black] (48.233,-16.203) arc (-23.31516:-45.54881:34.871);
\fill [black] (48.23,-16.2) -- (47.46,-16.74) -- (48.38,-17.14);
\draw (45.57,-23.47) node [right] {$+$};
\draw [black] (51.777,-11.713) arc (119.84259:83.80039:19.447);
\fill [black] (63.55,-9.25) -- (62.81,-8.66) -- (62.7,-9.66);
\draw (56.84,-8.96) node [above] {$=$};
\draw [black] (63.934,-11.35) arc (-62.70739:-93.64964:22.333);
\fill [black] (63.93,-11.35) -- (62.99,-11.27) -- (63.45,-12.16);
\draw (59.5,-14.04) node [below] {$o.c.$};
\draw [black] (40.965,-27.747) arc (118.831:70.66911:36.333);
\fill [black] (70.51,-25.29) -- (69.92,-24.55) -- (69.59,-25.5);
\draw (55.37,-22.81) node [above] {$-$};
\draw [black] (54.811,-38.667) arc (-97.84966:-134.80692:25.367);
\fill [black] (54.81,-38.67) -- (54.09,-38.06) -- (53.95,-39.05);
\draw (45.8,-36.78) node [below] {$\&$};
\draw [black] (68.375,-37.656) arc (-60.37001:-93.64076:13.669);
\fill [black] (68.38,-37.66) -- (67.43,-37.62) -- (67.93,-38.49);
\draw (65.51,-39.69) node [below] {$\&$};
\draw [black] (54.207,-46.479) arc (-118.50722:-151.49278:36.082);
\fill [black] (54.21,-46.48) -- (53.74,-45.66) -- (53.27,-46.54);
\draw (44.83,-40.76) node [below] {$|$};
\draw [black] (64.579,-49.159) arc (-83.54324:-104.01316:13.736);
\fill [black] (64.58,-49.16) -- (63.73,-48.75) -- (63.84,-49.75);
\draw (62.04,-49.78) node [below] {$|$};
\draw [black] (20.506,-41.567) arc (131.66584:119.19351:86.147);
\fill [black] (20.51,-41.57) -- (21.44,-41.41) -- (20.77,-40.66);
\draw (27.04,-35.23) node [above] {$/$};
\draw [black] (6.881,-45.141) arc (132.06374:103.88692:66.499);
\fill [black] (35.47,-29.95) -- (34.58,-29.66) -- (34.82,-30.63);
\draw (19.45,-35.28) node [above] {$/$};
\draw [black] (19.291,-46.387) arc (2.91612:-59.35083:5.157);
\fill [black] (16.77,-51.09) -- (17.71,-51.11) -- (17.2,-50.25);
\draw (19.36,-50.26) node [right] {$*$};
\draw [black] (10.918,-51.615) arc (-102.36433:-130.76577:9.753);
\fill [black] (6.64,-49.47) -- (6.92,-50.38) -- (7.57,-49.62);
\draw (7.65,-51.32) node [below] {$*$};
\draw [black] (7.684,-47.339) arc (78.23355:48.63635:9.442);
\fill [black] (12,-49.5) -- (11.73,-48.59) -- (11.07,-49.34);
\draw (12.03,-47.63) node [above] {$o.c.$};
\draw [black] (2.839,-44.862) arc (246.26123:-41.73877:2.25);
\draw (2.65,-40.02) node [above] {$*$};
\fill [black] (5.42,-44.3) -- (6.2,-43.77) -- (5.29,-43.37);
\draw [black] (35.41,-29.57) -- (15.59,-31.33);
\fill [black] (15.59,-31.33) -- (16.43,-31.76) -- (16.34,-30.76);
\draw (25.21,-29.8) node [above] {$eof$};
\draw [black] (28.368,-52.768) arc (-160.31098:-236.82114:18.77);
\fill [black] (28.37,-52.77) -- (28.57,-51.85) -- (27.63,-52.18);
\draw (27.48,-39.76) node [left] {$,$};
\draw [black] (29.231,-52.524) arc (-175.91406:-221.21806:28.862);
\fill [black] (29.23,-52.52) -- (29.67,-51.69) -- (28.68,-51.76);
\draw (29.89,-40.57) node [left] {$;$};
\draw [black] (30.052,-52.534) arc (170.14119:152.72669:71.764);
\fill [black] (30.05,-52.53) -- (30.68,-51.83) -- (29.7,-51.66);
\draw (31.96,-41.26) node [left] {$($};
\draw [black] (37.44,-32.14) -- (30.56,-52.66);
\fill [black] (30.56,-52.66) -- (31.28,-52.06) -- (30.34,-51.74);
\draw (34.77,-43.11) node [right] {$)$};
\draw [black] (38.432,-32.299) arc (-1.66748:-35.46464:37.795);
\fill [black] (31.44,-53.13) -- (32.31,-52.77) -- (31.49,-52.19);
\draw (37.25,-43.94) node [right] {$\{$};
\draw [black] (39.338,-32.147) arc (14.16948:-51.3016:21.117);
\fill [black] (32.07,-53.8) -- (33,-53.69) -- (32.38,-52.91);
\draw (39.65,-44.75) node [right] {$\}$};
\draw [black] (41.285,-28.477) arc (104.87742:84.62269:82.844);
\fill [black] (70.32,-26.06) -- (69.57,-25.49) -- (69.48,-26.49);
\draw (55.55,-25.41) node [above] {$=$};
\draw [black] (70.686,-27.871) arc (-62.84962:-107.65027:38.794);
\fill [black] (70.69,-27.87) -- (69.75,-27.79) -- (70.2,-28.68);
\draw (56.34,-32.59) node [below] {$<$};
\draw [black] (70.362,-27.007) arc (-78.97548:-91.52441:132.966);
\fill [black] (70.36,-27.01) -- (69.48,-26.67) -- (69.67,-27.65);
\draw (56.09,-29.58) node [below] {$>$};
\draw [black] (16.693,-52.862) arc (96.92389:-191.07611:2.25);
\draw (20.58,-57.23) node [below] {$o.c.$};
\fill [black] (14.76,-54.66) -- (14.36,-55.52) -- (15.35,-55.4);
\end{tikzpicture}
\end{center}

Y aquí ponemos pues lo que sea también (nose escribir).

\section{Acciones semánticas}
Mucho texto y la transición a la que corresponde cada acción

\section{Gestión de errores}
Explicando los casos de error posibles, los códigos de error asignados a cada caso etc.

\section{Tabla de Símbolos}
Solo tiene identificadores y su posicion, de momento. "muchotexto mimimi"

\section{Funcionamiento del programa??}
Breve explicación de lo que hace nuestro programa.
\end{document}