INVOCACION '/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3 -2022b'
+ :
+ : '	 "Corrector de la práctica del Minishell. Versión 4.5.3"'
+ : '	 "Copyright © 1993-2020 Francisco Rosales «frosal@fi.upm.es»"'
+ :
+ : '	 FECHA: "Mon Oct 10 18:00:09 CEST 2022"'
+ : '	 GRUPO: "c200172"'
+ : '	 AUTORES: "c200172;alvaro.cabo@alumnos.upm.es"'
+ :
+ INTRODUCCION
+ :
+ : 'El presente texto es la traza de ejecución de las pruebas'
+ : 'realizadas sobre su práctica del Minishell.'
+ :
+ : 'Las líneas de este texto que comienzan por "+" son mandatos'
+ : 'de un Bourne Shell que tiene tal carácter como prompt.'
+ : 'El mandato ":" es el mandato nulo.'
+ :
+ : 'Para localizar rápidamente las partes de interés de este texto,'
+ : 'buscad la tira "########## ATENCIÓN ##########" .'
+ :
+ : Nota:
+ : '	 Esta utilidad se encuentra EN DESARROLLO.'
+ : '	 Los RESULTADOS obtenidos NO son DEFINITIVOS y, por tanto,'
+ : '	 la VALORACIÓN obtenida NO se entenderá como DEFINITIVA.'
+ :
+ DONDE_OBTENER_AYUDA
+ :
+ : 'La información básica necesaria para la realización de esta'
+ : 'práctica se encuentra en los siguientes documentos:'
+ : ' #	 Memoria de la práctica. Contiene información abundante'
+ : '	 y detallada sobre el comportamiento deseado, y muchas'
+ : '	 referencias a las funciones que se deben usar.'
+ : ' #	 Manual de UNIX. En caso de duda sobre como usar una'
+ : '	 función de librería o mandato, utilice el mandato man.'
+ : ' #	 Libros de C. La bibliografía sobre este lenguaje es muy'
+ : '	 extensa. Sea meticuloso y limpio a la hora de programar'
+ : '	 y no deje de comentar debidamente su propio código.'
+ :
+ :
+ PREPARACION_DEL_ENTORNO_DE_TRABAJO =====================================
+ :
+ Crear_nulo _______________________________________________ previo ______
+ :
+ '[' -f :.c ']'
+ : 'Ya existe.'
+ make :
make: ':' is up to date.
+ ./: Bien.
+ :
+ Crear_freefds ____________________________________________ previo ______
+ :
+ '[' -f freefds.c ']'
+ : 'Ya existe.'
+ make freefds
make: 'freefds' is up to date.
+ : Bien.
+ :
+ Crear_nofiles ____________________________________________ previo ______
+ :
+ '[' -f nofiles.c ']'
+ : 'Ya existe.'
+ make nofiles
make: 'nofiles' is up to date.
+ : Bien.
+ :
+ : 'Recordamos el numero de descriptores de fichero disponibles.'
++ ./nofiles
+ NOFILES=1024
+ :
+ Crear_killmyself _________________________________________ previo ______
+ :
+ '[' -f killmyself.c ']'
+ : 'Ya existe.'
+ make killmyself
make: 'killmyself' is up to date.
+ : Bien.
+ :
+ Crear_sigdfl _____________________________________________ previo ______
+ :
+ '[' -f sigdfl.c ']'
+ : 'Ya existe.'
+ make sigdfl
make: 'sigdfl' is up to date.
+ : Bien.
+ :
+ Eliminar_ficheros_temporales _____________________________ previo ______
+ :
+ : 'Borramos los ficheros temporales que se utilizan.'
+ :
+ rm -f .aux .aux2 .aux3 .void .doit .env .ls1
+ rm -f .rm .out .out2 .ddf .pwd .cd .home
+ :
+ Establecer_entorno _______________________________________ previo ______
+ :
+ : 'Antes de continuar, establecemos el entorno que será heredado por'
+ : 'todo mandato que se ejecute a partir de ahora.'
+ :
+ umask 0
+ PATH=/bin:/usr/bin:/usr/local/bin:.
+ HOME=/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
+ WHO=c200172
+ export PATH HOME WHO
+ :
+ Compilar_el_Minishell ____________________________________ previo ______
+ :
+ make msh
gcc -Wall -g   -c -o main.o main.c
main.c:68:26: warning: initialization of 'const char **' from incompatible pointer type 'char *' [-Wincompatible-pointer-types]
   68 | const char** ICommands ={"cd", "umask","time", "read"};
      |                          ^~~~
main.c:68:26: note: (near initialization for 'ICommands')
main.c:68:32: warning: excess elements in scalar initializer
   68 | const char** ICommands ={"cd", "umask","time", "read"};
      |                                ^~~~~~~
main.c:68:32: note: (near initialization for 'ICommands')
main.c:68:40: warning: excess elements in scalar initializer
   68 | const char** ICommands ={"cd", "umask","time", "read"};
      |                                        ^~~~~~
main.c:68:40: note: (near initialization for 'ICommands')
main.c:68:48: warning: excess elements in scalar initializer
   68 | const char** ICommands ={"cd", "umask","time", "read"};
      |                                                ^~~~~~
main.c:68:48: note: (near initialization for 'ICommands')
main.c:75:11: warning: initialization of 'void (*)(char **, int)' from incompatible pointer type 'int (*)(char **, int)' [-Wincompatible-pointer-types]
   75 |   { "cd", cdIC },
      |           ^~~~
main.c:75:11: note: (near initialization for 'function_map[0].func')
main.c:76:13: warning: initialization of 'void (*)(char **, int)' from incompatible pointer type 'int (*)(char **, int)' [-Wincompatible-pointer-types]
   76 |   { "time", timeIC },
      |             ^~~~~~
main.c:76:13: note: (near initialization for 'function_map[1].func')
main.c:77:14: warning: initialization of 'void (*)(char **, int)' from incompatible pointer type 'int (*)(char **, int)' [-Wincompatible-pointer-types]
   77 |   { "umask", umaskIC },
      |              ^~~~~~~
main.c:77:14: note: (near initialization for 'function_map[2].func')
main.c:78:13: warning: initialization of 'void (*)(char **, int)' from incompatible pointer type 'int (*)(char **, int)' [-Wincompatible-pointer-types]
   78 |   { "read", readIC },
      |             ^~~~~~
main.c:78:13: note: (near initialization for 'function_map[3].func')
main.c: In function 'main':
main.c:101:32: warning: multi-line comment [-Wcomment]
  101 |     signal(SIGQUIT, SIG_IGN);  // Ctrl+shitft+\
      |                                ^
main.c:135:8: warning: implicit declaration of function 'exeIC' [-Wimplicit-function-declaration]
  135 |    if (exeIC(command, n_args) == -1){
      |        ^~~~~
main.c:138:32: warning: implicit declaration of function 'printError' [-Wimplicit-function-declaration]
  138 |                     case - 1 : printError("fork de mandato"); exit(1);
      |                                ^~~~~~~~~~
main.c:91:6: warning: unused variable 'argc' [-Wunused-variable]
   91 |  int argc;
      |      ^~~~
main.c:90:9: warning: unused variable 'argv' [-Wunused-variable]
   90 |  char **argv = NULL;
      |         ^~~~
main.c: At top level:
main.c:196:6: warning: conflicting types for 'printError'
  196 | void printError(char *error) {
      |      ^~~~~~~~~~
main.c:138:32: note: previous implicit declaration of 'printError' was here
  138 |                     case - 1 : printError("fork de mandato"); exit(1);
      |                                ^~~~~~~~~~
main.c: In function 'cdIC':
main.c:270:1: warning: control reaches end of non-void function [-Wreturn-type]
  270 | }
      | ^
At top level:
main.c:54:14: warning: 'HOME' defined but not used [-Wunused-variable]
   54 | static char* HOME; //Path del home
      |              ^~~~
gcc -Wall -g  -o msh parser.o scanner.o main.o
+ : Bien.
+ :
+ :
+ PRINCIPIO_DE_LA_CORRECCION =============================================
+ :
+ LEVEL=0
+ :
+ case "$COMPLEX" in
+ Lineas_simples_y_fin_de_fichero ________________________________________
+ :
+ : 'Su Minishell debe aceptar líneas en blanco y mandatos simples'
+ : 'y debe terminar limpiamente cuando se cierre su entrada estándar.'
+ :
+ MSHPID=2295700
+ ./msh
+ wait 2295700

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ 

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ 

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$      

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ 		 

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /usr/bin/env
#0: "/usr/bin/env" using 0 argument/s
PWD=/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
HOME=/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
WHO=c200172
TMPDIR=/tmp
SHLVL=0
PATH=/bin:/usr/bin:/usr/local/bin:.
_=./msh

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/echo
#0: "/bin/echo" using 0 argument/s


jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ../../../../../../../../../../bin/echo
#0: "../../../../../../../../../../bin/echo" using 0 argument/s


jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ./:
#0: "./:" using 0 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ 

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ 
You hitted ctrl-D | bye!
+ EXIT=0
+ case $EXIT in
+ : 0
+ : Bien.
+ :
+ Ejecucion_del_Minishell ________________________________________________
+ :
+ : 'Preparamos una tubería por la que inyectar mandatos al Minishell'
+ : 'y lanzamos su ejecución.'
+ :
+ rm -f .pipe
+ mkfifo .pipe
+ MSHPID=2295709
+ exec
+ exec ./sigdfl ./msh
+ rm -f .pipe

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + :
+ LEVEL=5
+ :
+ Mandatos_sin_argumentos ________________________________________________
+ :
+ : 'Se ejecutan mandatos, que están en el PATH, sin argumentos.'
+ :
./:
#0: "./:" using 0 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ :
#0: ":" using 0 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ echo
#0: "echo" using 0 argument/s


jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + :
+ Mandatos_con_varios_argumentos _________________________________________
+ :
+ : 'Se ejecutan mandatos, que están en el PATH, CON varios argumentos.'
+ :
+ : '	NOTA:'
+ : '	El siguiente mandato: "/bin/kill -KILL ##"'
+ : '	aparecerá en multitud de ocasiones en el presente texto.'
+ : '	Se utiliza para sincronizar la ejecución de dos procesos'
+ : '	el Minishell y el Corrector que le inyecta mandatos.'
+ : '		IGNÓRELO'
+ Sincronizar
/bin/kill -KILL 2295724
#0: "/bin/kill" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + Sincronizar
rm -f .aux
#0: "rm" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ touch .aux
#0: "touch" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ :
#0: ":" using 0 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ : El mandato "touch" debe haber creado el fichero ".aux".
#0: ":" using 9 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ : Veamos si existe, usando ls.
#0: ":" using 5 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ :
#0: ":" using 0 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ls -l -a -1 -F -g -q -s .aux
#0: "ls" using 8 argument/s
0 -rw-rw-rw- 1 jail 0 Oct 10 18:00 .aux

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ :
#0: ":" using 0 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2295727
#0: "/bin/kill" using 2 argument/s
+ '[' -f .aux ']'
+ : 'Cierto. Bien.'

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + :
: Ahora + Sincronizar
lo borramos.
#0: ":" using 3 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ rm -f .aux
#0: "rm" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ls -l .aux
#0: "ls" using 2 argument/s
ls: cannot access '.aux': No such file or directory

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2295737
#0: "/bin/kill" using 2 argument/s
+ '[' -f .aux ']'
+ : 'Falso. Bien.'

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + rm -f .aux
+ :
+ Descriptores_de_fichero ________________________________________________
+ :
+ : 'El numero de descriptores de fichero usados por el Minishell'
+ : 'debe mantenerse estable durante toda la ejecución del mismo.'
+ :
+ : '«««   Se deben cerrar los que NO se vayan a usar.   »»»'
+ :
./freefds
#0: "./freefds" using 0 argument/s
+ Sincronizar
1019

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ./freefds
#0: "./freefds" using 0 argument/s
1019

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ./freefds .ddf
#0: "./freefds" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ cat .ddf
#0: "cat" using 1 argument/s
1019

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ./freefds
#0: "./freefds" using 0 argument/s
1019

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ./freefds
#0: "./freefds" using 0 argument/s
1019

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ./freefds .aux
#0: "./freefds" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ cat .aux
#0: "cat" using 1 argument/s
1019

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ cmp .ddf .aux
#0: "cmp" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2295743
#0: "/bin/kill" using 2 argument/s
+ cmp -s .ddf .aux

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + : 'Cierto. Bien.'
+ rm -f .aux
+ :
+ LEVEL=10
+ :
+ Redireccion_de_salida_a_fichero_nuevo __________________________________
+ :
+ : '«««   Se debe crear el fichero.   »»»'
+ :
+ Sincronizar
last -n 10 c200172 > .aux
#0: "last" using 3 argument/s

wtmp begins Thu Sep 22 01:00:32 2022

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ cat .aux
#0: "cat" using 1 argument/s
cat: .aux: No such file or directory

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2295755
#0: "/bin/kill" using 2 argument/s
+ '[' -f .aux ']'
+ : Falso.
+ ERROR_GRAVE

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + NOTA=10
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" NO se ha creado.'
+ : 'Utilice la llamada al sistema CREAT.'
+ :
+ Redireccion_de_salida_a_fichero_existente ______________________________
+ :
+ : '«««   Se debe truncar el fichero.   »»»'
+ :
touch+ Sincronizar
 .void
#0: "touch" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ cat .void > .aux
#0: "cat" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ cmp .void .aux
#0: "cmp" using 2 argument/s
cmp: .aux: No such file or directory

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2295760
#0: "/bin/kill" using 2 argument/s
+ cmp -s .void .aux

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" NO se ha truncado.'
+ : 'Utilice la llamada al sistema CREAT.'
+ rm -f .aux .void
+ :
+ Redireccion_de_entrada_a_fichero_no_existente __________________________
+ :
+ : 'El Minishell debe tolerar los errores cometidos por el usuario:'
+ : '«««   Se debe detectar, notificar y tolerar el error.      »»»'
+ : '«««   Se debe abortar la ejecución del resto de la linea.  »»»'
+ :
touch .aux
#0: "touch" using 1 argument/s
+ Sincronizar

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ rm -f .aux2
#0: "rm" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ rm -f .noexiste
#0: "rm" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ls -l .aux .aux2 .noexiste
#0: "ls" using 4 argument/s
ls: cannot access '.aux2': No such file or directory
ls: cannot access '.noexiste': No such file or directory
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ : NO debería borrarse el fichero ".aux".
#0: ":" using 6 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ : Debería crearse el fichero ".aux2".
#0: ":" using 5 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ rm -f .aux < .noexiste
#0: "rm" using 2 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ touch .aux2
#0: "touch" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ ls -l .aux .aux2 .noexiste
#0: "ls" using 4 argument/s
ls: cannot access '.aux': No such file or directory
ls: cannot access '.noexiste': No such file or directory
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux2

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2295767
#0: "/bin/kill" using 2 argument/s
+ '[' -f .aux2 ']'
+ : 'Cierto. Bien.'
+ rm -f .aux2

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + :
+ '[' -f .aux ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" se ha borrado.'
+ : 'No se controla adecuadamente el posible error devuelto'
+ : 'por la llamada al sistema OPEN.'
+ : 'El Minishell debe detectar y tolerar los errores derivados'
+ : 'de un uso incorrecto por parte del usuario y debe avisarle'
+ : 'del error para permitirle que lo corrija.'
+ :
+ Redireccion_de_entrada_a_fichero_existente _____________________________
+ :
+ : '«««   Se debe abrir para lectura.   »»»'
+ :
touch .aux+ Sincronizar

#0: "touch" using 1 argument/s

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ echo rm .aux > .doit
#0: "echo" using 2 argument/s
rm .aux

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ sh < .doit
#0: "sh" using 0 argument/s
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux
+ '[' -f .aux ']'
+ : Cierto.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" NO se ha borrado.'
+ : 'La redirección de entrada no asocia el fichero como'
+ : 'entrada estándar del mandato, o NO se abre para lectura.'
+ rm -f .aux
+ rm -f .doit
+ :
+ LEVEL=15
+ :
+ Redireccion_de_entrada_y_salida_simultaneamente ________________________
+ :
+ : '«««   Se deben poder hacer simultáneamente.   »»»'
+ :
+ Sincronizar
+ cmp -s .env .aux
+ : 'Cierto. Bien.'
+ rm -f .aux .env
+ :
+ Descriptores_de_fichero_tras_redirecciones _____________________________
+ :
+ : '«««   Se deben cerrar los que NO se usen.   »»»'
+ :
+ Sincronizar
1019
1019
1019
+ cmp -s .ddf .aux
+ : 'Cierto. Bien.'
+ rm -f .aux
+ :
+ LEVEL=20
+ :
+ Secuencia_sin_redirecciones_I __________________________________________
+ :
+ : '«««   Se debe esperar por el último mandato.   »»»'
+ :
+ cron_ON
+ CLOCK=64811
+ Sincronizar
kill: (2295808): No such process
+ TIMEOUT
+ : '########## ATENCIÓN ##########'
+ : 'Posible bloqueo (1) del Minishell.'
+ :
++ cron_OFF
+ '[' 10 -lt 9 ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'Una secuencia termina cuando termina el mandato mas a la'
+ : 'derecha en la misma.'
+ : 'El Minishell ha de esperar a que este proceso termine.'
+ :
+ Secuencia_sin_redirecciones_II _________________________________________
+ :
+ : '«««   Se debe conectar salida con entrada del siguiente.   »»»'
+ :
+ Sincronizar
kill: (2295856): No such process
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : 'Falso. Bien.'
+ :
+ Descriptores_de_fichero_tras_secuencias_I ______________________________
+ :
+ : '«««   Se deben cerrar los que NO se usen.   »»»'
+ :
+ Sincronizar
1019
1019
1019
+ cmp -s .ddf .aux
+ : 'Cierto. Bien.'
+ rm -f .aux
+ :
+ LEVEL=25
+ :
+ Secuencia_con_redireccion_de_salida ____________________________________
+ :
+ : '«««   Se debe redirigir la salida del último mandato.   »»»'
+ :
+ Sincronizar
rm: cannot remove '.ls1': No such file or directory
-rw-rw-rw- 1 jail jail 185 Oct 10 18:00 .aux
-rw-rw-rw- 1 jail jail 185 Oct 10 18:00 .ls1
+ cmp -s .aux .ls1
+ : 'Cierto. Bien.'
+ rm -f .aux .ls1
+ :
+ Secuencia_con_redireccion_de_entrada ___________________________________
+ :
+ : '«««   Se debe redirigir la entrada del primer mandato.   »»»'
+ :
+ Sincronizar
sh: 1: xua.: not found
+ TIMEOUT
+ : '########## ATENCIÓN ##########'
+ : 'Posible bloqueo (2) del Minishell.'
+ :
+ '[' -f .aux ']'
+ : Cierto.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" NO se ha borrado.'
+ : 'Las secuencias no funcionan con redirección de entrada.'
+ : 'La entrada de una secuencia es la entrada de su primer'
+ : mandato.
+ rm -f .aux
+ rm -f .rm
+ :
+ Secuencia_con_redireccion_de_entrada_y_salida __________________________
+ :
+ : '«««   Se debe redirigir la entrada del primer   »»»'
+ : '«««   mandato y la salida del último.           »»»'
+ :
+ Sincronizar
rev: stdin: 6: Invalid or incomplete multibyte or wide character
ls: cannot access '.aux': No such file or directory
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : 'Falso. Bien.'
+ rm -f .rm
+ :
+ Sincronizar
cat: .out: No such file or directory
+ cmp -s .out .out2
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".out" NO se ha creado O su contenido NO es correcto.'
+ : 'Las secuencias no funcionan con redirección de entrada y'
+ : salida.
+ : 'La salida de una secuencia es la salida de su último mandato.'
+ rm -f .out .out2
+ :
+ Descriptores_de_fichero_tras_secuencias_II _____________________________
+ :
+ : '«««   Se deben cerrar los que NO se usen.   »»»'
+ :
+ Sincronizar
1019
1019
1019
+ cmp -s .ddf .aux
+ : 'Cierto. Bien.'
+ rm -f .aux
+ rm -f .ddf
+ :
+ LEVEL=30
+ :
+ Mandatos_en_Background_I _______________________________________________
+ :
+ : '«««   Se deben ejecutar correctamente.   »»»'
+ :
+ Sincronizar
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux
+ Sincronizar
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : 'Falso. Bien.'
+ :
+ Mandatos_en_Background_II ______________________________________________
+ :
+ : '«««   No se debe esperar por ellos.   »»»'
+ :
+ cron_ON
+ CLOCK=64832
+ Sincronizar
++ cron_OFF
+ '[' 0 -lt 5 ']'
+ : 'Cierto. Bien.'
+ :
+ Mandatos_en_Background_III _____________________________________________
+ :
+ : '«««   Se debe esperar por el mandato en foreground.   »»»'
+ :
+ Sincronizar
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux
sleep 1
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : 'Falso. Bien.'
+ :
+ LEVEL=35
+ :
+ Senyales_I _____________________________________________________________
+ :
+ :
+ : 'El Minishell ha de manejar correctamente las señales generables'
+ : 'desde teclado que son SIGINT y SIGQUIT.'
+ :
+ : '«««   Deben ser ignoradas o capturadas por el Minishell.   »»»'
+ :
+ : 'Se arranca otro Minishell con el siguiente PID:'
+ PID=2296105
+ ./sigdfl ./msh
+ Sincronizar

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ /bin/kill -KILL 2296106
#0: "/bin/kill" using 2 argument/s
+ : 'Se intenta matar con las señales INT, QUIT y KILL en ese orden.'

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + kill -INT 2296105
+ kill -QUIT 2296105
+ kill -KILL 2296105
+ wait 2296105
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 1448: 2296105 Killed                  ./sigdfl $BIN < $PIPE
+ EXIT=137
+ case $EXIT in
+ : 137
+ : 'SIGKILL mata al Minishell, como debe ser. Bien.'
+ :
+ Senyales_II ____________________________________________________________
+ :
+ : '«««   Deben ser ignoradas por los procesos en background.   »»»'
+ :
+ Sincronizar
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux
+ Sincronizar
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : 'Falso. Bien.'
+ :
+ Senyales_III ___________________________________________________________
+ :
+ : '«««   Deben ser la acción por defecto para procesos en foreground.   »»»'
+ :
+ Sincronizar
-rw-rw-rw- 1 jail jail 0 Oct 10 18:00 .aux
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" se ha borrado.'
+ : 'SIGINT y SIGQUIT no tienen la acción por defecto para los'
+ : 'procesos lanzados en foreground y debe ser así.'
+ :
+ LEVEL=40
+ :
+ Interno_cd_I ___________________________________________________________
+ :
+ : 'Cuando se invoca en foreground y es el último:'
+ : '«««   Se debe ejecutar como llamada al sistema.   »»»'
+ :
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13
+ Sincronizar
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13
++ cd ..
++ pwd
++ cat /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.pwd
+ '[' /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13 = /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13 ']'
+ : 'Cierto. Bien.'
+ :
+ Interno_cd_II __________________________________________________________
+ :
+ : 'El mandato "cd" siempre:'
+ : '«««   Debe mostrar el "current working directory"   »»»'
+ : '«««   por la salida estándar con formato "%s\n".    »»»'
+ :
+ Sincronizar
cmp: EOF on /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.cd which is empty
+ cmp -s /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.pwd /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.cd
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'Debe poderse redirigir la salida y/o entrada estándar'
+ : 'de los mandatos especiales.'
+ rm -f .pwd .cd
+ :
+ Interno_cd_III _________________________________________________________
+ :
+ : 'Cuando se invoca "cd" sin argumentos:'
+ : '«««   Debe cambiar al directorio HOME.   »»»'
+ :
+ Sincronizar
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
++ cat /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.home
+ '[' /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172 = /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172 ']'
+ : 'Cierto. Bien.'
+ rm -f .home
+ :
+ Interno_cd_IV __________________________________________________________
+ :
+ : 'Si se intenta cambiar a un directorio no existente:'
+ : '«««   Se debe notificar el error.   »»»'
+ :
+ Sincronizar
sh: 115: cd: can't cd to ...
+ :
+ : 'Si se invoca con demasiados argumentos:'
+ : '«««   Se debe notificar el error.   »»»'
+ :
+ Sincronizar
sh: 117: cd: can't cd to a
+ :
+ LEVEL=45
+ :
+ Interno_umask_I ________________________________________________________
+ :
+ : 'Cuando se invoca "umask" sin argumentos:'
+ : '«««   Se debe mostrar la mascara de creación de ficheros,   »»»'
+ : '«««   por la salida estándar con formato "%o\n".            »»»'
+ :
0000
+ Sincronizar
0000
++ cat .aux
+ '[' 0000 -eq 0 ']'
+ : 'Cierto. Bien.'
+ rm -f .aux
+ :
+ Interno_umask_II _______________________________________________________
+ :
+ : 'Cuando se invoca "umask" sin argumentos:'
+ : '«««   No se debe modificar el valor actual de la mascara.   »»»'
+ :
+ Sincronizar
-rw-rw-rw- 1 jail jail 5 Oct 10 18:00 .aux
++ ls -l .aux
++ cut -c-10
+ '[' -rw-rw-rw- = -rw-rw-rw- ']'
+ : 'Cierto. Bien.'
+ :
+ Sincronizar
-rwxrwxrwx 1 jail jail 5 Oct 10 18:00 .aux
++ ls -l .aux
++ cut -c-10
+ '[' -rwxrwxrwx = -rwxrwxrwx ']'
+ : 'Cierto. Bien.'
+ :
+ Interno_umask_III ______________________________________________________
+ :
+ : 'Cuando se invoca "umask" con un argumento:'
+ : '«««   Se debe modificar el valor actual de la mascara      »»»'
+ : '«««   con el valor del argumento interpretado en base 8.   »»»'
+ :
+ Sincronizar
---------- 1 jail jail 5 Oct 10 18:00 .aux
-rwxr-x--x 1 jail jail 5 Oct 10 18:00 .aux
++ ls -l .aux
++ cut -c-10
+ '[' -rwxr-x--x = -rwxr-x--x ']'
+ : 'Cierto. Bien.'
+ :
+ Interno_umask_IV _______________________________________________________
+ :
+ : 'Cuando se invoca "umask" con un argumento erróneo:'
+ : '«««   Se debe notificar el error.                           »»»'
+ : '«««   No se debe modificar el valor actual de la mascara.   »»»'
+ :
+ Sincronizar
---------- 1 jail jail 5 Oct 10 18:00 .aux
sh: 143: umask: Illegal number: 888
-rwxr-x--x 1 jail jail 5 Oct 10 18:00 .aux
++ ls -l .aux
++ cut -c-10
+ '[' -rwxr-x--x = -rwxr-x--x ']'
+ : 'Cierto. Bien.'
+ :
+ Interno_umask_V ________________________________________________________
+ :
+ : 'Cuando se invoca "umask" con un numero de argumentos erróneo:'
+ : '«««   Se debe notificar el error.                           »»»'
+ : '«««   No se debe modificar el valor actual de la mascara.   »»»'
+ :
+ Sincronizar
---------- 1 jail jail 5 Oct 10 18:00 .aux
-rw-rw--w- 1 jail jail 5 Oct 10 18:00 .aux
++ ls -l .aux
++ cut -c-10
+ '[' -rw-rw--w- = -rwxr-x--x ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'En caso de detectar numero incorrecto de argumentos,'
+ : 'notifique el error.'
+ rm -f .aux
+ :
+ LEVEL=50
+ :
+ case "$TURN" in
+ Interno_time_I _________________________________________________________
+ :
+ : '«««   Debe poderse redirigir la salida estándar     »»»'
+ : '«««   de todo mandato interno. También de "time".   »»»'
+ :
+ Sincronizar
sh: 156: time: not found
-rw-rw--w- 1 jail jail 0 Oct 10 18:00 .aux
+ '[' -s .aux ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'No se puede redirigir su salida estándar.'
+ rm -f .aux
+ :
+ LEVEL=55
+ :
+ Interno_time_II ________________________________________________________
+ :
+ : 'Cuando se invoca "time" sin argumentos:'
+ : '«««   Se debe notificar el tiempo consumido.   »»»'
+ :
+ Sincronizar
sh: 159: time: not found
sh: 160: time: not found
+ '[' -s .aux ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'No se puede redirigir su salida estándar.'
+ rm -f .aux
+ :
+ Interno_time_III _______________________________________________________
+ :
+ : 'Cuando se invoca "time" con argumentos:'
+ : '«««   Se debe ejecutar el mandato indicado.                    »»»'
+ : '«««   Se debe notificar el tiempo consumido en su ejecución.   »»»'
+ :
+ Sincronizar
sh: 164: time: not found
-rw-rw--w- 1 jail jail 0 Oct 10 18:00 .aux
+ '[' -f .aux ']'
+ : Cierto.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'No ejecuta correctamente el mandato.'
+ rm -f .aux
+ :
+ LEVEL=60
+ :
+ Metacaracter_Dolar_I ___________________________________________________
+ :
+ : 'El metacarácter "$" con argumento "HOME" :'
+ : '«««   Se debe sustituir por el valor de la variable "HOME".   »»»'
+ :
+ Sincronizar
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
+ cmp -s .aux .home
+ : 'Cierto. Bien.'
+ rm -f .aux .home
+ :
+ Metacaracter_Dolar_II __________________________________________________
+ :
+ : 'Donde aparezca el metacarácter "$" con argumento:'
+ : '«««   Se debe sustituir correctamente la variable por su valor.   »»»'
+ :
+ echo /../home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.
+ Sincronizar
/../home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172/.
+ cmp .aux .aux2
+ : 'Cierto. Bien.'
+ rm -f .aux .aux2
+ :
+ LEVEL=65
+ :
+ Metacaracter_Tilde _____________________________________________________
+ :
+ : 'El metacarácter "~" sin argumento:'
+ : '«««   Se debe sustituir por el valor de la variable "HOME".   »»»'
+ :
+ Sincronizar
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
++ cat .home
++ cat .aux
+ '[' /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172 = /home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172 ']'
+ : 'Cierto. Bien.'
+ rm -f .aux .home
+ :
+ Interno_cd_HOME ________________________________________________________
+ :
+ : 'El mandato "cd ." :'
+ : '«««   Debe mostrar el valor de la variable "HOME".   »»»'
+ :
+ Sincronizar
/home/jail/home2/datsi/ssoo/soX/msh/home.2022b13/c200172
+ cmp -s .aux .home
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El mandato "cd" no muestra el valor correcto.'
+ rm -f .aux .home
+ :
+ LEVEL=70
+ :
+ Interno_read_I _________________________________________________________
+ :
+ : 'Cuando se invoca "read" con argumentos:'
+ : '«««   Se debe leer una sola línea de la entrada estándar.   »»»'
+ :
+ Sincronizar
: Debería crearse el fichero .out
-rw-rw--w- 1 jail jail 0 Oct 10 18:00 .out
-rw-rw--w- 1 jail jail 0 Oct 10 18:00 .out2
+ '[' -f .out ']'
+ : Cierto.
+ rm -f .out
+ '[' -f .out2 ']'
+ : Cierto.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".out2" se ha creado.'
+ : '"read" debe leer una línea de la entrada estándar.'
+ rm -f .out2
+ :
+ Interno_read_II ________________________________________________________
+ :
+ : 'Cuando se invoca "read" con argumentos:'
+ : '«««   Se debe asignar valor a las variables indicadas.   »»»'
+ :
+ Sincronizar
sh: 196: .aux: not found
$CMD $ARG
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" NO se ha creado.'
+ : 'Debe modificarse al valor actual de las variables con putenv.'
+ :
+ Interno_read_III _______________________________________________________
+ :
+ : 'Cuando se invoca "read" con argumentos en una secuencia:'
+ : '«««   Se deben poder ejecutar en una secuencia.       »»»'
+ : '«««   Debe tener efecto permanente si es el último.   »»»'
+ :
+ Sincronizar
rm: cannot remove '.aux': No such file or directory
$ARG
sh: 206: $ARG: not found
ls: cannot access '.aux': No such file or directory
+ '[' -f .aux ']'
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" NO se ha creado.'
+ : 'Debe poderse redirigir la salida y/o entrada estándar'
+ : 'de los mandatos especiales a/desde una secuencia.'
+ : 'Si este test le funciona cuando lo ejecuta desde un terminal,'
+ : 'pruebe a incluir entre las primeras líneas del programa:'
+ : '	setbuf(stdin, NULL);	'
+ :
+ LEVEL=75
+ :
+ Interno_read_IV ________________________________________________________
+ :
+ : 'Cuando se invoca "read" con mas argumentos que palabras:'
+ : '«««   Las variables de mas NO deben modificar su valor.   »»»'
+ :
+ Sincronizar
sh: 212: +: not found
sh: 214: 1: not found
read A B C D E
expr: syntax error: unexpected argument 'A'
++ cat .aux
+ '[' '' -eq 6 ']'
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 2462: [: : integer expression expected
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'Si aparecen mas nombres de variable que palabras,'
+ : 'las variables de mas NO deben modificar su valor.'
+ rm -f .aux
+ :
+ Interno_read_V _________________________________________________________
+ :
+ : 'Cuando se invoca "read" con mas palabras que argumentos:'
+ : '«««   Las últimas palabras asignadas al último argumento.   »»»'
+ :
+ Sincronizar
sh: 221: 1: not found
echo $A $B $C | wc -w > .aux
cat: .aux: No such file or directory
++ cat .aux
cat: .aux: No such file or directory
+ '[' '' -eq 5 ']'
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 2493: [: : integer expression expected
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'Si aparecen mas palabras que nombres de variable,'
+ : 'las últimas variables se deben asignar a la última variable.'
+ rm -f .aux
+ :
+ Interno_read_VI ________________________________________________________
+ :
+ : 'Cuando se invoca "read" sin argumentos:'
+ : '«««   Se debe leer la línea de entrada.   »»»'
+ : '«««   Se debe notificar el error.         »»»'
+ :
+ Sincronizar
sh: 227: read: arg count
-rw-rw--w- 1 jail jail 0 Oct 10 18:00 .aux
+ '[' -f .aux ']'
+ : Cierto.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'El fichero ".aux" se ha creado.'
+ : 'Si "read" se invoca sin argumentos, debe darse un error'
+ : 'pero la línea de entrada ha de ser consumida.'
+ rm -f .aux
+ :
+ LEVEL=80
+ :
+ Variables_especiales_I _________________________________________________
+ :
+ : 'Donde aparezca la variable especial "mypid" :'
+ : '«««   Se debe sustituir por el identificador   »»»'
+ : '«««   del propio proceso Minishell.            »»»'
+ :
+ Sincronizar

++ cat .aux
+ '[' '' -eq 2295709 ']'
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 2551: [: : integer expression expected
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'La variable especial "mypid" NO se sustituye por el valor correcto.'
+ rm -f .aux
+ :
+ Variables_especiales_II ________________________________________________
+ :
+ : 'Donde aparezca la variable especial "status" :'
+ : '«««   Se debe sustituir por el valor de terminación   »»»'
+ : '«««   del último mandato.                             »»»'
+ :
+ Sincronizar
cat: ...: No such file or directory
++ cat .aux
+ '[' '' -eq 0 ']'
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 2579: [: : integer expression expected
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'La variable especial "status" no mantiene el valor del mandato.'
+ rm -f .aux .aux2
+ :
+ Variables_especiales_III _______________________________________________
+ :
+ : 'Donde aparezca la variable especial "status" :'
+ : '«««   Se debe sustituir por el valor de terminación   »»»'
+ : '«««   de la última secuencia de mandatos              »»»'
+ : '«««   (el del último mandato de la secuencia).        »»»'
+ :
+ Sincronizar
cat: ...: No such file or directory
cat: ...: No such file or directory
++ cat .aux
+ '[' '' -eq 0 ']'
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 2611: [: : integer expression expected
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'La variable especial "status" no mantiene el valor de la secuencia.'
+ rm -f .aux .aux2 .aux3
+ :
+ LEVEL=85
+ :
+ Variables_especiales_IV ________________________________________________
+ :
+ : 'Donde aparezca la variable especial "status" :'
+ : '«««   Se debe sustituir por el valor de terminación   »»»'
+ : '«««   del último mandato interno                      »»»'
+ : '«««   (0 si fue correcto y no 0 si fue incorrecto).   »»»'
+ :
+ Sincronizar
sh: 246: cd: can't cd to ...
++ cat .aux
+ '[' '' -ne 0 ']'
/home/jail/home2/datsi/ssoo/soX/msh/src/check.msh-4.5.3: line 2642: [: : integer expression expected
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'La variable especial "status" no mantiene el valor del interno.'
+ rm -f .aux .aux2
+ :
+ Variables_especiales_V _________________________________________________
+ :
+ : 'Donde aparezca la variable especial "bgpid" :'
+ : '«««   Se debe sustituir por el identificador        »»»'
+ : '«««   del último proceso arrancado en background.   »»»'
+ :
echo $$ > .out ; /bin/kill -ALRM 2296288
+ Sincronizar
+ cmp -s .out .out2
+ : Falso.
+ ERROR_GRAVE
+ : '########## ATENCIÓN ##########'
+ :
+ : 'La variable especial "bgpid" NO se sustituye por el valor correcto.'
+ rm -f .out .out2 .doit
+ :
+ LEVEL=90
+ :
+ Expansion_de_comodines_I _______________________________________________
+ :
+ : 'Cuando aparezca un carácter "?" :'
+ : '«««   Si un fichero casa con el patrón indicado,              »»»'
+ : '«««   se debe sustituir el texto por el nombre del fichero.   »»»'
+ :
+ Sincronizar
.aux2
++ cat .out
+ '[' .aux2 = .aux2 ']'
+ : 'Cierto. Bien.'
+ rm -f .out .aux .aux2
+ :
+ Expansion_de_comodines_II ______________________________________________
+ :
+ : 'Cuando aparezca un carácter "?" :'
+ : '«««   Si el patrón no casa con ningún fichero,   »»»'
+ : '«««   NO se debe alterar el texto.               »»»'
+ :
+ Sincronizar
.aux?
++ cat .out
+ '[' '.aux?' = '.aux?' ']'
+ : 'Cierto. Bien.'
+ rm -f .out .aux
+ :
+ LEVEL=95
+ :
+ Expansion_de_comodines_III _____________________________________________
+ :
+ : 'Cuando aparezca un carácter "?" :'
+ : '«««   Si varios ficheros casan con el patrón indicado,             »»»'
+ : '«««   se debe expandir a tantos textos como nombres de ficheros.   »»»'
+ :
+ Sincronizar
.aux .aux2 .aux3 .aux
++ cat .out
+ '[' '.aux .aux2 .aux3 .aux' = '.aux .aux2 .aux3 .aux' ']'
+ : 'Cierto. Bien.'
+ rm -f .out .aux .aux2 .aux3
+ :
+ LEVEL=100
+ :
+ :
+ FIN_DE_LA_CORRECCION ===================================================
+ :
+ :
+ exit 10
+ case $NOTA in

jail@home2/datsi/ssoo/soX/msh/home.2022b13/c200172$ + : 10
+ : '*** ATENCIÓN ***'

You hitted ctrl-D | bye!
+ : 'Para APROBAR LA PARTE PRACTICA de la asignatura       '
+ : 'es IMPRESCINDIBLE obtener como mínimo 4.5 puntos.     '
+ : 'Debe usted llegar a una media de 4.5 entre esta       '
+ : 'práctica y las demás (si las hubiera).                '
+ :
+ exit 10

--K1eeXVyBHFw00ruk--