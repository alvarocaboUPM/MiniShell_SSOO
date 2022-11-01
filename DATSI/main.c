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
*					Versión 2.0
*					  1/11/2022
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
#include <sys/stat.h>
#include <strings.h>
#include <stdbool.h>

/******	Variables Globales	***/
#define READ_END 0
#define WRITE_END 1

#define MAX_PATH 4096 //common linux maxsize
#define MAX_FILENAME 255 //ext2
#define MAX_USERNAME 32
//COLORES PARA EL PROMT
#define RED   "\033[31m"
#define GREEN "\033[32m"
#define ORANGE "\033[33m"
#define RESET "\033[0m"

static pid_t PID; //Identificador de proceso
static char CWD[MAX_PATH]; //Path de trabajo actual
static char* HOME; //Path del home 

/***Declaración de funciones*/

extern int obtain_order();		/* See parser.y for description */
int* countArgs(char ***argvv, int argvc);
//Visuals
void printWelcome();
void PrintPromtCWD();
void errorPrint(char* err);
void warningPrint(char* err);
void okPrint(char* err);

//Internal functions
int exeIC(char** argv, int argc);
int cdIC(char** argv, int argc);
int timeIC(char** argv, int argc);
int umaskIC(char** argv, int argc);
int readIC(char** argv, int argc);
/*DEBUGGING*/
void printIntArray(int *arr);
void printCharArray(char* arr);
int getArraySize(void* arr);

//Lista de funciones internas
const char* ICommands[4]={"cd","umask","time","read"};

/*Mapa de funciones internas para llamar en función
del argumento introducido*/
const static struct {
  const char *name; //Command name
  int (*func)(char **argv, int argc);
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
	char *filev[3] = { NULL, NULL, NULL }; //Posibles redirects
	int bg;
	int ret;
	//MY main parameters
	int bg_Pid = 0; //PID de la función de bg 
	PID = getpid(); //PID de la shell

	/*	SIGNALS	*/
	signal(SIGKILL, SIG_IGN); //Kill Process
	signal(SIGINT, SIG_IGN);   // Ctrl+C
    signal(SIGQUIT, SIG_IGN);  // Ctrl+shitft+\ 
	
	/*	BUFFERS E-S*/
	setbuf(stdout, NULL);			/* Unbuffered */
	setbuf(stdin, NULL);

	//Welcome message
	printWelcome();
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
	int total_N_commands = argcc[0];
	int current_N_commands=0;
	//Adding pipes
	int m_pipe[2];
	int inuse_fd=0;
	//Some flags
	bool in_pipe, isLast;
	in_pipe=isLast=false;

	//Si hay más de un comando se interpreta como un pipe
	in_pipe= total_N_commands>1;

	//================DEBUG=================
	//printf("%d\n", (total_N_commands));
	//printIntArray(argcc);

	int stdin_copy = dup(STDIN_FILENO);

	//Presenting the input commands
	while((command=argvv[current_N_commands])){
		//Gestión básica de arguemtos y el comando
		int n_args=argcc[current_N_commands+1] - 1; //Le quitamos el nombre del comando
		//fprintf(stderr, "#%d: \"%s\" using %d argument/s\n", current_N_commands, command[0], n_args);
		current_N_commands++; //current_N_commands <= argcc.length
	
		//Checks if a pipe has been requested, if so, it trys to create it
		
		if (in_pipe && pipe(m_pipe) < 0){
			errorPrint("No se ha podido crear el pipe");
		} 

		//Checks if we are @ the final command 
		isLast= current_N_commands == total_N_commands; 
		
		/*Now we have to discriminate wether the command is
			internal or is located in /usr/bin: */

		if (exeIC(command, n_args) == -1){
			pid_t pid;
			switch (pid = fork ()) {
				case - 1 : errorPrint("fork"); exit(1);
				/*****************
				*	 HIJO		 * 
				******************/
				case   0 : 
					//Case: Pipe requested (ESCRITORES)
					if(in_pipe){
						if(!isLast){
							char* mess= malloc(50);
							sprintf(mess, "Using pipe @ process %s -> : %d\n",command[0], getpid());
							warningPrint(mess);
						}
							
						//Solo manejamos el fd en uso
						if(inuse_fd!=0){
							//try dup2(inuse_fd) -> Stdin
   							if (dup2(inuse_fd, STDIN_FILENO) < 0) 
								errorPrint("dup 2[HIJO] -> STDIN");
                            if (!isLast && (close(inuse_fd) < 0))
								//Si es el último intentamos cerrar
								errorPrint("Pipe cerrada antes de acabar");
						}
						if (m_pipe[WRITE_END] != STDOUT_FILENO) {
                            if (!isLast && dup2(m_pipe[WRITE_END], STDOUT_FILENO) < 0) 
							errorPrint("dup 2[HIJO] -> STDOUT");  // stdout -> pipe
                        }	
						if (inuse_fd != STDIN_FILENO && close(inuse_fd) < 0) 
							errorPrint("closing last_pipe_fd when != STDIN_FILENO (0)");

						//Tries para cerrar el pipe
						if (close(m_pipe[WRITE_END]) < 0) 
							errorPrint("Cerrando [WRITE_END] en HIJO");
						if (close(m_pipe[READ_END]) < 0) 
							errorPrint("Cerrando [READ_END] en HIJO");
					}

					//Case: Normal execution
					int ret_status = execvp(command[0], command);
						//Catching the error message
						if (ret_status != 0) {
							char* error_message = malloc(50);
							sprintf(error_message, "'%s' exited, status(%d)", command[0], ret_status);
							errorPrint(error_message);
							free(error_message);
						}
						exit(EXIT_FAILURE);
				
				/*****************
				*	 PADRE		 * 
				******************/
				default: 
					//Case: Pipe requested (LECTOR)
					if(in_pipe){
						if (close(inuse_fd) < 0) 
							errorPrint("Cerrando el [WRITE_END] no cerrado del hijo"); 
						if (!isLast) {
							inuse_fd = dup(m_pipe[READ_END]);
						} else {
							// Reconfiguramos 
							dup2(stdin_copy, STDIN_FILENO);
							close(stdin_copy);
						}
						//try closing both ends
						if (close(m_pipe[WRITE_END]) < 0) 
							errorPrint("Cerrando [WRITE_END] en padre");
						if (close(m_pipe[READ_END]) < 0) 
							errorPrint("Cerrando [READ_END] en padre");
					}
					waitpid(pid, NULL, 0); //espera por su hijo
				}
			}
		//Si el command introducido no está en usr/bin o no está
		//implementado en la shell, saltará un error de no implementado
		}
  #endif
  
	}
	exit(0);
	return 0;
}

	/*FUNCIONES AUXILIARES*/
void printWelcome(){
	puts(
    " __   __ ___ __    _ ___ _______ __   __ _______ ___     ___       _______ __   __   _______ ___    __   __ _______ ______   _______ \n"
	"|  |_|  |   |  |  | |   |       |  | |  |       |   |   |   |     |  _    |  | |  | |   _   |   |  |  | |  |   _   |    _ | |       |\n"
	"|       |   |   |_| |   |  _____|  |_|  |    ___|   |   |   |     | |_|   |  |_|  | |  |_|  |   |  |  |_|  |  |_|  |   | || |   _   |\n"
	"|       |   |       |   | |_____|       |   |___|   |   |   |     |       |       | |       |   |  |       |       |   |_||_|  | |  |\n"
	"|       |   |  _    |   |_____  |       |    ___|   |___|   |___  |  _   ||_     _| |       |   |__|       |       |    __  |  |_|  |\n"
	"| ||_|| |   | | |   |   |_____| |   _   |   |___|       |       | | |_|   | |   |   |   _   |       |     ||   _   |   |  | |       |\n"
	"|_|   |_|___|_|  |__|___|_______|__| |__|_______|_______|_______| |_______| |___|   |__| |__|_______||___| |__| |__|___|  |_|_______|\n");

}
/*
	Escribe el error en rojo utilizando perror
*/
void errorPrint(char *err) {
	fprintf(stderr, RED);
	perror(err);
	fprintf(stderr, RESET); 
}

/*
	Escribe el warning en naranja
*/
void warningPrint(char *err) {
	fprintf(stderr, ORANGE);
	fprintf(stderr, "%s", err);
	fprintf(stderr, RESET); 
}

/*
	Escribe un texto en verde
*/
void okPrint(char *err) {
	fprintf(stderr, GREEN);
	fprintf(stderr, "%s", err);
	fprintf(stderr, RESET); 
}

/* Función que imprime el promt con la estructura
   username@CWD$ 
*/
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


/* Función que interpreta el mapa de argvv 
   y devuelve el número de comandos distintos (sizeof(res)) y 
   el número de arguementos de cada uno*/
int* countArgs(char ***argvv, int argvc){

	/*EL PRIMER ELEMENTO VA SER EL LENGTH*/

	int* res = malloc(argvc+1); 
	int i=0;
	res[0]=0;

	while(argvv[i]!=NULL){
		int count=0; char* test;
		while((test=argvv[i][count])){
			//fprintf(stderr, "\t%d : %s\n", count, test);
			count++;
		}
		res[i+1]=count;
		//fprintf(stderr, "\tRes[%d]: %d\n", i, res[i]);
		i++;
		res[0]=i;
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
	mode_t current_mask, new_mask;
	//Args control
	switch (argc)
	{
	case 0:
		//Prints current umask
		current_mask = umask(022);    // change it to whatever but save the old one on return
		umask(current_mask);        // change it back
		fprintf(stdout, "%o\n", current_mask); 
		break;
	case 1: ; //Solves a label error 
		char* error;
		new_mask = (mode_t)strtol(argv[1], &error, 8);
		printf("%s", error);
		if(*error=='\0'){
			umask(new_mask);
			printf("%o\n", new_mask); 
		}else{
			errorPrint("Please imput a valid Base-8 number");
			return -1;
		}
		break;

	default:
		errorPrint("Cant have more than 1 argument for 'Umask'");
		return -1;
	}
}

int readIC(char** argv, int argc){
	return -1;
}


/* DEBUGGIN TOOLS */

void printIntArray(int *arr){
	puts("\nPriting Int array: \n");
	int i;
	printf("[");
	
	for(i=0; i<sizeof(arr)/sizeof(int); i++){
		printf("%d, ", arr[i]);
	}
	printf("]\n");
}

void printCharArray(char* arr){
	puts("\nPriting Char array: \n");
	int i;
	printf("[");
	
	for(i=0; i<sizeof(arr)/sizeof(int); i++){
		printf("%c, ", arr[i]);
	}
	printf("]\n");
}
