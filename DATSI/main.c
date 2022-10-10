/*-
 * main.c
 * Minishell C source
 * Shows how to use "obtain_order" input interface function.
 *
 * Copyright (c) 1993-2002-2019, Francisco Rosales <frosal@fi.upm.es>
 * Todos los derechos reservados.
 *
 * Publicado bajo Licencia de Proyecto Educativo Práctico
 * <http://laurel.datsi.fi.upm.es/~ssoo/LICENCIA/LPEP>
 *
 * Queda prohibida la difusión total o parcial por cualquier
 * medio del material entregado al alumno para la realización
 * de este proyecto o de cualquier material derivado de este,
 * incluyendo la solución particular que desarrolle el alumno.
 *
 * DO NOT MODIFY ANYTHING OVER THIS LINE
 * THIS FILE IS TO BE MODIFIED
 */

/*========================================================
* 				MINISHELL by Álvaro Cabo					
*					Versión 1.0
*					  7/10/2022
* ========================================================
*/

/************INCLUDES**********/

#include <stddef.h>			/* NULL */
#include <stdio.h>			/* setbuf, printf */
#include <stdlib.h>
//v1
#include <signal.h>	//Handlers de señales
#include <unistd.h> //PID operations
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <strings.h>
#include <stdbool.h>

/******	Variables Globales	***/
#define MAX_PATH 4096 //common linux maxsize
#define MAX_FILENAME 255 //ext2
#define MAX_USERNAME 32
//COLORES PARA EL PROMT
#define RED   "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

static pid_t PID; //Identificador de proceso
static char CWD[MAX_PATH]; //Path de trabajo actual
static char* HOME; //Path del home 

/***Declaración de funciones*/

extern int obtain_order();		/* See parser.y for description */
void PrintPromtCWD();
void errorPrint();
int* countArgs(char ***argvv, int argvc);
int cdIC(char** argv, int argc);
int timeIC(char** argv, int argc);
int umaskIC(char** argv, int argc);
int readIC(char** argv, int argc);

//Lista de funciones internas
const char** ICommands ={"cd", "umask","time", "read"};
/*Mapa de funciones internas para llamar en función
del argumento introducido*/
const static struct {
  const char *name; //Command name
  void (*func)(char **argv, int argc);
  } function_map [] = {
  { "cd", cdIC },
  { "time", timeIC },
  { "umask", umaskIC },
  { "read", readIC },
};

/******* Main ********/

int main(void)
{	
	/*	@PARAMS	*/

	//GIVEN main parameters
	char ***argvv = NULL;
	int argvc;
	char **argv = NULL;
	int argc;
	char *filev[3] = { NULL, NULL, NULL };
	int bg;
	int ret;
	//MY main parameters
	PID = getpid(); //PID de la shell

	/*	SIGNALS	*/
	signal(SIGKILL, SIG_IGN); //Kill Process
	signal(SIGINT, SIG_IGN);   // Ctrl+C
    signal(SIGQUIT, SIG_IGN);  // Ctrl+shitft+\ 
	
	/*	BUFFERS E-S*/
	setbuf(stdout, NULL);			/* Unbuffered */
	setbuf(stdin, NULL);

	/* BUCLE INFINITO*/
	while (1) {
		//Necesitamos obtener el dir y el PID del calling process
		getcwd(CWD, sizeof(CWD));
		PrintPromtCWD(CWD);
		ret = obtain_order(&argvv, filev, &bg);
		if (ret == 0){
			/* EOF */
			fprintf(stderr, "\nYou hitted ctrl-D | bye!\n");
			break;
		}
		if (ret == -1) continue;	/* Syntax error */
		argvc = ret - 1;			/* Line */
		if (argvc == 0) continue;	/* Empty line */

		#if 1
			/*YOU CAN COMMENT THIS SECTION BY SWITCHING THE IF*/
			char** command;
			int* argcc = countArgs(argvv, argvc);
			int N_commands=0; 
	
			while((command=argvv[N_commands])){
				int n_args=argcc[N_commands] - 1; //Le quitamos el nombre del comando
            	fprintf(stderr, "#%d: \"%s\" using %d argument/s\n", N_commands, command[0], n_args);
				N_commands++;

			/*Now we gotta discriminate wether the command is:
				internal: */
			if (exeIC(command, n_args) == -1){
                pid_t pid;
                switch (pid = fork ()) {
                    case - 1 : printError("fork de mandato"); exit(1);
                    case   0 : 
						/* proceso hijo */
                        execvp(command [0], command);
                        //Error in exec
                        printError("exec error"); exit(1);
		            default: /* padre */
                        /* establece el manejador */
                        waitpid(pid, NULL, 0); //espera por su hijo
                	}
            	}
			

			}
	#endif
	}
	exit(0);
	return 0;
}

/*FUNCIONES AUXILIARES*/

void errorPrint(char *err) {
    fprintf(stderr, RED);
    perror(err);
    fprintf(stderr, RESET); 
}

void PrintPromtCWD(char* CWD){
	//Prompt: user@PATH$
    char cd[2000];
    strcpy(cd, CWD);

	int i=0, size = (int)strlen(cd)-(int)strlen(getpwuid(getuid())->pw_dir);
	
	char* prompt= calloc(size, sizeof(char));
    char* p = strtok(cd, "/");
	
	while(p !=NULL){
        //printf("%s", p);
		if(i>1){
			strcat(prompt, p);
			strcat(prompt, "/");
			//fprintf(stderr, "\nP: %s\tPROMT: %s ", p, prompt);
		}
		p= strtok(NULL, "/");
		i++;
	}

	if(prompt == NULL) prompt="";
	
	prompt[strlen(prompt)-1]='\0'; //Remove last '/'
	//Imprime el Prompt
    fprintf(stderr, "\n%s@%s$ ", getpwuid(getuid())->pw_name,
									 prompt);
	
}

void printError(char *error) {
	//Imprime el error en rojo y reset
    fprintf(stderr, RED); 
    perror(error);
    fprintf(stderr, RESET);
}

int* countArgs(char ***argvv, int argvc){
	
	int* res = malloc(sizeof(int)*argvc); 
	int i=0;

	while(argvv[i]!=NULL){
		int count=0; char* test;
		while((test=argvv[i][count])){
			//fprintf(stderr, "\t%d : %s\n", count, test);
			count++;
		}
		res[i]=count;
		//fprintf(stderr, "\tRes[%d]: %d\n", i, res[i]);
		i++;
	}
	return res;
	
}

/*INTERNAL COMMANDS SELECT AND IMPLEMENTATION*/
int exeIC(char ** command, int argc)
{
	/*Given a command name returns its function*/

  for (int i = 0; i < (sizeof(function_map) / sizeof(function_map[0])); i++) {
    if (!strcmp(function_map[i].name, command[0]) && function_map[i].func) {
		//Ejecuta la función
      function_map[i].func(command, argc);
      return 0;
    }
  }
  return -1;
}

/* Used the --help section of these command to complete the 
	closest implementation for this minishell */

int cdIC(char ** command, int argc){
	/*
	@param: [dir] (argc <=2)
		* if length(command) == 1 -> dir = $HOME
	@return 0 if $PWD changed, -1 otherwise
	*/

	char* dir;

	switch (argc)
	{
	case 0:
		/* dir = $HOME */
		dir=getenv("HOME");
		break;
	case 1:
		dir=command[1];
		break;
	default:
		errorPrint("Cant have more than 1 argument for 'CD'");
		return -1;
	}

	//Check for permisions and other errors when we changhe $PWD
	if(chdir(dir)==0){
		fprintf(stdout, "%s\n", CWD);
	}else{
		errorPrint("Can't access directory");
		return -2;
	}
}
int timeIC(char** argv, int argc){
	return -1;
}
int umaskIC(char** argv, int argc){
	return -1;
}
int readIC(char** argv, int argc){
	return -1;
}