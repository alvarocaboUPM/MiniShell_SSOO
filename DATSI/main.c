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
 * 			  MINISHELL by Álvaro Cabo
 *			  	Versión 2.0
 *			  	  1/11/2022
 * ========================================================
 */

/************INCLUDES**********/

#include <stddef.h> /* NULL */
#include <stdio.h>	/* setbuf, printf */
#include <stdlib.h>
// v1
#include <signal.h> //Handlers de señales
#include <unistd.h> //PID operations
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdbool.h>
#include <dirent.h>
//Permisos open
#define _GNU_SOURCE
#include <fcntl.h>

/******	Variables Globales	***/
#define READ_END 0
#define WRITE_END 1

#define MAX_PATH 4096	 // common linux maxsize
#define MAX_FILENAME 255 // ext2
#define MAX_USERNAME 32
// COLORES PARA EL PROMT
#define RED "\033[31m"
#define GREEN "\033[32m"
#define ORANGE "\033[33m"
#define RESET "\033[0m"

static pid_t PID;		   // Identificador de proceso
static char CWD[MAX_PATH]; // Path de trabajo actual
//static char *HOME;		   // Path del home

/***Declaración de funciones*/

extern int obtain_order(); /* See parser.y for description */
int* countArgs(char ***argvv, int argvc);
int redirHandler(char** filev);
int redir(int i, char** filev, int* original);
// Visuals
void printWelcome();
void PrintPromtCWD();
int errorPrint(char *err);
void warningPrint(char *err);
void okPrint(char *err);

int exeIC(char **argv, int argc, char ** redirs);
int cdIC(char **argv, int argc);
int timeIC(char **argv, int argc);
int umaskIC(char **argv, int argc);
int readIC(char **argv, int argc);
/*DEBUGGING*/
void printIntArray(int *arr);
void printCharArray(char *arr);
int getArraySize(void *arr);
int getFDsOpen();
void printFlags();
char* printRedir(int redir);

// Lista de funciones internas
const char *ICommands[4] = {"cd", "umask", "time", "read"};

/*Mapa de funciones internas para llamar en función
del argumento introducido*/
const static struct
{
	const char *name; // Command name
	int (*func)(char **argv, int argc);
} function_map[] = {
	{"cd", cdIC},
	{"time", timeIC},
	{"umask", umaskIC},
	{"read", readIC},
};

/*
Mapa de flags de estado
*/
static bool in_pipe= false;
static bool isLast= false;
static bool isFirst= false;

/******* Main ********/

int main(void)
{
	/*	@PARAMS	*/

	// GIVEN main parameters
	char ***argvv = NULL;
	int argvc;

	char *filev[3] = {NULL, NULL, NULL}; // Posibles redirects
	int bg;	// Flag for background needed (0/1)
	int ret;
	// MY main parameters
	int bg_Pid = 0; // PID de la función de bg
	PID = getpid(); // PID de la shell

	/*	SIGNALS	*/
	signal(SIGKILL, SIG_IGN); // Kill Process
	signal(SIGINT, SIG_IGN);  // Ctrl+C
	signal(SIGQUIT, SIG_IGN); // Ctrl+shitft+\ 

	/*	BUFFERS E-S*/
	setbuf(stdout, NULL); /* Unbuffered */
	setbuf(stdin, NULL);

	// Welcome message
	printWelcome();
	/* BUCLE INFINITO*/
	while (1)
	{
		// Necesitamos obtener el dir y el PID del calling process
		getcwd(CWD, sizeof(CWD));
		//PrintPromtCWD(CWD);
		printf("\nmsh> ");
		ret = obtain_order(&argvv, filev, &bg);
		if (ret == 0) /* EOF */
		{
			fprintf(stderr, "\nYou hitted ctrl-D | bye!\n");
			break;
		}
		if (ret == -1) continue;	 /* Syntax error */
		argvc = ret - 1; /* Line */
		if (argvc == 0) continue; /* Empty line */

#if 1
		/*YOU CAN COMMENT THIS SECTION BY SWITCHING THE IF*/
		int *argcc = countArgs(argvv, argvc);
		char **command = NULL;
		int atCommand = 0; //Numero de comandos registrados

		// Si hay más de un comando se interpreta como un pipe
		in_pipe = argvc > 1;

		// Adding pipes
		int m_pipe[2];
		int inuse_fd;
		int stdin_copy;

		if(in_pipe){
			//Guardamos el contenido de la entrada estandar
			inuse_fd = 0;
			stdin_copy = dup(STDIN_FILENO);
		}

		// Handling individual commands
		while ((command = argvv[atCommand])){
			int n_args = argcc[atCommand] - 1; // Le quitamos el nombre del comando

			// Updates flags
			isLast = atCommand == argvc-1;
			isFirst = atCommand == 0;
			//Handles redirect
			int original=0;
			int hasRedirects = redirHandler(filev);

			atCommand++; // atCommand <= argvc

			//========Debuggin station===========
			
			#if 0
				fprintf(stderr, "#%d: \"%s\" using %d argument/s\n", atCommand-1, command[0], n_args);
				okPrint("FDs Opened\n");
				printf("FDs-> %d\n", getFDsOpen());
				okPrint("FLAGS\n");
				printFlags();
				okPrint("REDIRS-> ");
				if(hasRedirects ==-1 ){
					puts("Nope");
				}else{
					printf("%s: %s", printRedir(hasRedirects), filev[hasRedirects]);
				}
				
				puts(" "
					 " ");
			#endif

			// Catch null
			if (command == NULL)
			{
				if (in_pipe) 
				{
					errorPrint("El segundo mandato es nulo");
					break;
				}
				continue;
			}

			// Checks if a piredirGetterpe has been requested, if so, it trys to create it
			if (in_pipe && pipe(m_pipe) < 0)
				errorPrint("No se ha podido crear el pipe");

			/*Now we have to discriminate wether the command is
				internal or is located in /usr/bin: */

			if(exeIC(command, n_args, filev)==-1){
				pid_t pid;
				switch (pid = fork())
				{
				case -1:
					errorPrint("fork");
					break;
				/*****************
				 *	 HIJO		 *
				 ******************/
				case 0:
					// Case redirects: 
					if(isFirst && hasRedirects == STDIN_FILENO
					&& redir(STDIN_FILENO, filev, &original) != STDIN_FILENO)
						return errorPrint("Error al redirigir la entrada estandar en hijo");
					
					if(isLast && hasRedirects == STDOUT_FILENO 
					&& redir(STDOUT_FILENO, filev, &original) != STDOUT_FILENO)
						return errorPrint("Error al redirigir la salida estandar en hijo");
				 	
					if(hasRedirects == STDERR_FILENO
					&& redir(STDERR_FILENO, filev, &original) != STDERR_FILENO)
						return errorPrint("Error al redirigir la salida de error estandar en hijo");

					// Case: Pipe requested (ESCRITORES)
					if (in_pipe)
					{
						if (!isLast)
						{
							char *mess = malloc(50);
							sprintf(mess, "Using pipe @ process %s -> : %d\n", command[0], getpid());
							warningPrint(mess);
						}

						// Solo manejamos el fd en uso
						if (inuse_fd != 0)
						{
							// try dup2(inuse_fd) -> Stdin
							if (dup2(inuse_fd, STDIN_FILENO) < 0)
								errorPrint("dup 2[HIJO] -> STDIN");
							if (!isLast && (close(inuse_fd) < 0))
								// Si es el último intentamos cerrar
								errorPrint("Pipe cerrada antes de acabar");
						}
						if (m_pipe[WRITE_END] != STDOUT_FILENO)
						{
							if (!isLast && dup2(m_pipe[WRITE_END], STDOUT_FILENO) < 0)
								errorPrint("dup 2[HIJO] -> STDOUT"); // stdout -> pipe
						}
						if (inuse_fd != STDIN_FILENO && close(inuse_fd) < 0)
							errorPrint("closing last_pipe_fd when != STDIN_FILENO (0)");

						// Tries para cerrar el pipe
						if (close(m_pipe[WRITE_END]) < 0)
							errorPrint("Cerrando [WRITE_END] en HIJO");
						if (close(m_pipe[READ_END]) < 0)
							errorPrint("Cerrando [READ_END] en HIJO");
					}

					// Case: Normal execution
					int ret_status = execvp(command[0], command);
					// Catching the error message
					if (ret_status != 0)
					{
						char *error_message = malloc(50);
						sprintf(error_message, "'%s' exited, status(%d)", command[0], ret_status);
						errorPrint(error_message);
						free(error_message);
					}
					//break; //No hace falta break pq exec termina el proceso hijo 

				/***************
				 *	 PADRE		 *
				 ****************/
				default:
					// Case: Pipe requested (LECTOR)
					if (in_pipe)
					{
						if (close(inuse_fd) < 0)
							errorPrint("Cerrando el [WRITE_END] no cerrado del hijo");
						if (isLast)
						{
							// Restauramos la entrada estandar
							dup2(stdin_copy, STDIN_FILENO);
							//printf("STDIN_COPY -> %d\n", stdin_copy);
							close(stdin_copy);
						}
						else
							//Duplicamos en un nuevo fd el lado lector
							inuse_fd = dup(m_pipe[READ_END]);
					
						// try closing both ends
						if (close(m_pipe[WRITE_END]) < 0)
							errorPrint("Cerrando [WRITE_END] en padre");
						if (close(m_pipe[READ_END]) < 0)
							errorPrint("Cerrando [READ_END] en padre");
					}

					if(isLast){
						if (!bg){	
							int ret_status;
							/*Supendemos padre hasta que Hijo.status -> Terminated
								sin fijarnos en el return status del hijo
							*/
							while (pid != wait(&ret_status)) 
										continue;
							
							char stat_str[4];
                            sprintf(stat_str, "%d", ret_status);
                            setenv("status", stat_str, 1);		
						}
						else{
							bg_Pid = pid;
							fprintf(stdout, "[%d]\n", pid);
							char backgr_pid_str[7];
							sprintf(backgr_pid_str, "%d", bg_Pid);
							setenv("bgpid", backgr_pid_str, 1);
						}
					}
				}
				//Restaura los redirects
				if(isLast && hasRedirects!=-1){
				 //restore
				 dup2(original, hasRedirects);
			}
		}
			// Si el command introducido no está en usr/bin o no está
			// implementado en la shell, saltará un error de no implementado
		}
#endif
	}
	exit(0);
	return 0;
}

/*FUNCIONES AUXILIARES*/
void printWelcome()
{
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
int errorPrint(char *err)
{
	fprintf(stderr, RED);
	perror(err);
	fprintf(stderr, RESET);
	return -1;
}

/*
	Escribe el warning en naranja
*/
void warningPrint(char *err)
{
	fprintf(stderr, ORANGE);
	fprintf(stderr, "%s", err);
	fprintf(stderr, RESET);
}

/*
	Escribe un texto en verde
*/
void okPrint(char *err)
{
	fprintf(stderr, GREEN);
	fprintf(stderr, "%s", err);
	fprintf(stderr, RESET);
}

/* Función que imprime el promt con la estructura
   username@CWD$
*/
void PrintPromtCWD(char *CWD)
{
	// Prompt: user@PATH$
	char cd[2000];
	strcpy(cd, CWD);

	int i = 0, size = (int)strlen(cd) - (int)strlen(getpwuid(getuid())->pw_dir);

	char *prompt = calloc(size, sizeof(char));
	char *p = strtok(cd, "/");

	while (p != NULL)
	{
		if (i > 1)
		{
			strcat(prompt, p);
			strcat(prompt, "/");
			//printf(stderr, "\nP: %s\tPROMT: %s ", p, prompt);
		}
		p = strtok(NULL, "/");
		i++;
	}

	if (prompt == NULL)
		prompt = "";

	prompt[strlen(prompt) - 1] = '\0'; // Remove last '/'
	// Imprime el Prompt
	fprintf(stderr, "\n%s@%s$ ", getpwuid(getuid())->pw_name,
			prompt);
}

/* 
   @param filev[3]
   @return Index of the redirected fd
*/
int redirHandler(char** filev){
	int i=0;
	while(i<=3){
		if(i==3) 
			return -1;
		if(filev[i]!=NULL){
			break;
		}
		i++;
	}
	return i;
}

/*
	Implemets the logic of redirect 
	@return The index given in case of success
	@throw -1 if something wrong
*/
int redir(int i, char** filev, int *original){
	/* 0 = < STDIN	|| 1 = > STDOUT || 2 = >& STDERR */
	int fd;
	if(i == STDIN_FILENO){
		fd = open(filev[i], O_RDONLY, 0666); //Solo leemos
	}else{
		fd = creat(filev[i], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		//fd = open(filev[i], O_CREAT | O_TRUNC | O_WRONLY, 0666);
	}
	
	*original = dup(i); //Duplicamos para poder restaurar

	//Control errores 1
	//printf("FD -> %d, ORIGINAL -> %d, NOM: %s\n", fd, *original, filev[i]);
	
	if(fd < 3)
		return errorPrint("No se pudo redirigir");
	
		
	if (dup2(fd, i) < 0)
		return errorPrint("Error en el dup2 de redir");

	if (close(fd) < 0)
		return errorPrint("Error al cerrar el archivo de redirección");

	return i;
}

/* 
   @param Array de comandos y su longitud
   @return Número de argumentos de cada comando
*/
int *countArgs(char ***argvv, int argvc)
{
	int *res = malloc(argvc);
	int i = 0;

	while (argvv[i] != NULL)
	{
		int count = 0;
		char *test;
		while ((test = argvv[i][count]))
		{
			// fprintf(stderr, "\t%d : %s\n", count, test);
			count++;
		}
		res[i] = count;
		// fprintf(stderr, "\tRes[%d]: %d\n", i, res[i]);
		i++;
	}
	return res;
}

/*INTERNAL COMMANDS SELECT AND IMPLEMENTATION*/

/*
	@param Command name, arguments and arguments.length
	@result Execution of the proper internal function
*/
int exeIC(char **argv, int argc, char** redirs){

	int redAt = redirHandler(redirs), og=0;

	for (int i = 0; i < 4; i++)
	{
		if (!strcmp(function_map[i].name, argv[0]) && function_map[i].func)
		{
			// Ejecuta la función
			if(redAt > -1 && redir(redAt, redirs, &og)!=redAt)
				errorPrint("Error en la redirección");
			function_map[i].func(argv, argc);
			dup2(og, redAt);
			return 0;
		}
	}

	return -1;
}

/* Used the --help section of these command to complete the
	closest implementation for this minishell */

/*
	@param [dir] (argc <=2)
	if length(argv) == 1 -> dir = $HOME
	@return 0 if $PWD changed, -1 otherwise
*/
int cdIC(char **argv, int argc)
{
	char *dir;

	switch (argc)
	{
	case 0:
		/* dir = $HOME */
		dir = getenv("HOME");
		break;
	case 1:
		dir = argv[1];
		break;
	default:
		errorPrint("Cant have more than 1 argument for 'CD'");
		return -1;
	}

	// Check for permisions and other errors when we changhe $PWD
	if (chdir(dir) == 0)
	{
		fprintf(stdout, "%s\n", CWD);
	}
	else
	{
		errorPrint("Can't access directory");
		return -2;
	}
}
int timeIC(char **argv, int argc)
{

	return -1;
}
int umaskIC(char **argv, int argc)
{
	mode_t current_mask, new_mask;
	// Args control
	switch (argc)
	{
	case 0:
		// Prints current umask
		current_mask = umask(022); // change it to whatever but save the old one on return
		umask(current_mask);	   // change it back
		fprintf(stdout, "%o\n", current_mask);
		break;
	case 1:; // Solves a label error
		char *error;
		new_mask = (mode_t)strtol(argv[1], &error, 8);
		printf("%s", error);
		if (*error == '\0')
		{
			umask(new_mask);
			printf("%o\n", new_mask);
		}
		else
		{
			errorPrint("Please imput a valid Base-8 number");
			return -1;
		}
		break;

	default:
		errorPrint("Cant have more than 1 argument for 'Umask'");
		return -1;
	}
}

int readIC(char **argv, int argc)
{
	return -1;
}


/* DEBUGGIN TOOLS */

void printIntArray(int *arr)
{
	puts("\nPriting Int array: \n");
	int i,x;
	printf("[");

	for (i = 0; (x=arr[i]); i++)
	{
		printf("%d, ", x);
	}
	
	printf("]\n");
}

void printCharArray(char *arr)
{
	puts("\nPriting Char array: \n");
	int i;
	printf("[");

	for (i = 0; i < sizeof(arr); i++)
	{
		printf("%c, ", arr[i]);
	}
	printf("]\n");
}

int getFDsOpen()
{
	DIR *dp = opendir("/proc/self/fd");
	if(dp==NULL){
		printf("NULL");
		return -1;
	}
	struct dirent *de;
	int count = -2; // '.', '..', dp

	if (dp == NULL)
		return -1;

	//char buffer = malloc(1000);
	while ((de = readdir(dp)) != NULL)
	{
		if (count >= 0){

			printf("%s, ", de->d_name);
		}
		count++;
	}

	(void)closedir(dp);
	return count;
}

void printFlags(){
	printf("isFirst-> %s\n", isFirst ? "true" : "false");
	printf("isLast->%s\n", isLast ? "true" : "false");
	printf("in_pipe %s\n", in_pipe ? "true" : "false");
}
char* printRedir(int redir){
	char* result;
	switch (redir)
	{
	case 0:
		result="IN";
		break;
	case 1:
		result="OUT";
		break;
	default:
		result="ERR";
		break;
	}
return result;
}