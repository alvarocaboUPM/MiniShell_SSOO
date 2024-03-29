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
#include <signal.h> //Handlers de señales
#include <unistd.h> //PID operations
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <inttypes.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <strings.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
// Permisos open
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

#pragma GCC diagnostic ignored "-Wformat" // Para el formato de time

//========DECLARACIÓN DE FUNCIONES==========

extern int obtain_order(); /* See parser.y for description */
int *countArgs(char ***argvv, int argvc);
void metacharsHandler(char *command[]);
int metaUser(char *cursor);
int metaVar(char *cursor);
int followsFormat(char *pos, int *follows_format);
int redirHandler(char **filev);
int redir(int i, char **filev, int *original);
// Visuals

void printWelcome();
void PrintPromtCWD();
int errorPrint(char *err);
void warningPrint(char *err);
void okPrint(char *err);

int exeIC(char **argv, int argc, char **redirs, int *status);
int cdIC(char **argv, int argc);
int timeIC(char **argv, int argc);
void start_clock();
int printTMS(struct tms t);
int littleDad(char *argv[]);

int umaskIC(char **argv, int argc);
int readIC(char **argv, int argc);
/*DEBUGGING*/

void printIntArray(int *arr);
void printCharArray(char *arr);
int getArraySize(void *arr);
int getFDsOpen();
void printFlags();
char *printRedir(int redir);

// Lista de funciones internas
const char *ICommands[4] = {"cd", "umask", "time", "read"};

/**
 * @brief Mapa de funciones internas para llamar en función
	del argumento introducido
 */
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

// Mapa de flags de estado

static bool in_pipe = false;
static bool isLast = false;
static bool isFirst = false;

// Estructuras de timing
static clock_t TIME;

/******* Main ********/

int main(void)
{
	// GIVEN main parameters
	char ***argvv = NULL;
	int argvc;

	char *filev[3] = {NULL, NULL, NULL}; // Posibles redirects
	int bg;								 // Flag for background needed (0/1)
	int ret;
	// MY main parameters
	int bg_Pid = 0; // PID de la función de bg
	PID = getpid(); // PID de la shell

	// SIGNALS
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;

	// Ignored signals
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	// Acciones provocadas por el usuario
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);

	// BUFFERS E-S
	setbuf(stdout, NULL); /* Unbuffered */
	setbuf(stdin, NULL);

	// Welcome message
	printWelcome();

	// Guarda el momento de inicio del minishell
	start_clock();

	/* BUCLE INFINITO*/
	while (1)
	{
		// Necesitamos obtener el dir y el PID del calling process
		getcwd(CWD, sizeof(CWD));
		// Promt
		printf("\nmsh> ");
		ret = obtain_order(&argvv, filev, &bg);
		if (ret == 0) /* EOF */
		{
			fprintf(stderr, "\nBye!\n");
			break;
		}
		if (ret == -1)
			continue;	 /* Syntax error */
		argvc = ret - 1; /* Line */
		if (argvc == 0)
			continue; /* Empty line */

#if 1
		/*YOU CAN COMMENT THIS SECTION BY SWITCHING THE IF*/

		int*argcc = countArgs(argvv, argvc);
		char **command = NULL;
		int atCommand = 0; // Numero de comandos registrados

		// Si hay más de un comando se interpreta como un pipe
		in_pipe = argvc > 1;

		// Adding pipes
		int fd[2];
		int inuse_fd;
		int stdin_copy;

		if (in_pipe)
		{
			// Guardamos el contenido de la entrada estandar
			inuse_fd = 0;
			stdin_copy = dup(STDIN_FILENO);
		}

		// Handling individual commands
		while ((command = argvv[atCommand]))
		{
			int n_args = argcc[atCommand] - 1; // Le quitamos el nombre del comando

			// Updates flags
			isLast = atCommand == argvc - 1;
			isFirst = atCommand == 0;
			// Handles redirect
			int original = 0;
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
			// Metachars
			metacharsHandler(command);

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

			// Checks if a pipe has been requested, if so, it trys to create it
			if (in_pipe && pipe(fd) < 0)
				errorPrint("No se ha podido crear el pipe");

			/*Now we have to discriminate wether the command is
				internal or is located in /usr/bin: */

			int *ret_status = malloc(sizeof(int));
			if (!bg && isLast &&
				exeIC(command, n_args, filev, ret_status) == 0)
				continue;

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
				if (isFirst && filev[STDIN_FILENO] &&
					redir(STDIN_FILENO, filev, &original) != STDIN_FILENO)
					return errorPrint("Error al redirigir la entrada estandar en hijo");

				if (isLast && filev[STDOUT_FILENO] &&
					redir(STDOUT_FILENO, filev, &original) != STDOUT_FILENO)
					return errorPrint("Error al redirigir la salida estandar en hijo");

				if (filev[STDERR_FILENO] &&
					redir(STDERR_FILENO, filev, &original) != STDERR_FILENO)
					return errorPrint("Error al redirigir la salida de error estandar en hijo");

				// Case bg; do not ignore signals if not bg
				if (!bg)
				{
					sa.sa_handler = SIG_DFL;
					sigaction(SIGINT, &sa, NULL);
					sigaction(SIGQUIT, &sa, NULL);
				}

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
					if (inuse_fd != STDIN_FILENO)
					{
						// try dup2(inuse_fd) -> Stdin
						if (dup2(inuse_fd, STDIN_FILENO) < 0)
							errorPrint("dup 2[HIJO] -> STDIN");
						if (!isLast && (close(inuse_fd) < 0))
							// Cerramos el descriptor porque hemos acabado con él
							errorPrint("Pipe cerrada antes de acabar");
					}
					if (fd[WRITE_END] != STDOUT_FILENO)
					{
						if (!isLast && dup2(fd[WRITE_END], STDOUT_FILENO) < 0)
							errorPrint("dup 2[HIJO] -> STDOUT"); // stdout -> pipe
					}
					if (inuse_fd != STDIN_FILENO && close(inuse_fd) < 0)
						errorPrint("closing last_pipe_fd when != STDIN_FILENO (0)");

					// Tries para cerrar el pipe
					if (close(fd[WRITE_END]) < 0)
						errorPrint("Cerrando [WRITE_END] en HIJO");
					if (close(fd[READ_END]) < 0)
						errorPrint("Cerrando [READ_END] en HIJO");
				}

				// Case: Normal execution
				if (exeIC(command, n_args, filev, ret_status) == -1)
				{
					//command[n_args+1]=NULL;
					*ret_status = execvp((const char *)command[0], (char *const *)command);
				}
				// Catching the error message
				if (*ret_status != 0)
				{
					char *error_message = malloc(50);
					sprintf(error_message, "'%s' exited, status(%d)", command[0], *ret_status);
					errorPrint(error_message);
					free(error_message);
				}
				exit(1);
				break;

				/***************
				 *	 PADRE	   *
				 ***************/

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
						close(stdin_copy);
					}
					else
						// Duplicamos en un nuevo fd el lado lector
						inuse_fd = dup(fd[READ_END]);

					// try closing both ends
					if (close(fd[WRITE_END]) < 0)
						errorPrint("Cerrando [WRITE_END] en padre");
					if (close(fd[READ_END]) < 0)
						errorPrint("Cerrando [READ_END] en padre");
				}

				if (isLast)
				{
					if (!bg)
					{
						int ret_status;
						/*Supendemos padre hasta que Hijo.status -> Terminated
							sin fijarnos en el return status del hijo
						*/
						while (pid != wait(&ret_status))
							continue;

						char stat_str[4];
						sprintf(stat_str, "%d", ret_status); // toString
						setenv("status", stat_str, 1);
					}
					else
					{
						bg_Pid = pid;
						fprintf(stdout, "[%d]\n", pid);
						char backgr_pid_str[7];
						sprintf(backgr_pid_str, "%d", bg_Pid);
						setenv("bgpid", backgr_pid_str, 1);
					}
				}
			}
			// Restaura los redirects
			if (isLast && hasRedirects != -1)
			{
				// restore
				dup2(original, hasRedirects);
			}
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

/**
 * @brief Función que imprime el promt con la estructura
 * username"@"CWD$
 * @param CWD Current directory
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

/**
 * @brief Gets the number of arguments on every imput command
 *
 * @param argvv Array de comandos
 * @param argvc Longitud del array
 * @return Númemo de argumentos de cada comando
 */
int *countArgs(char ***argvv, int argvc)
{
	int *res = malloc(sizeof(int) * argvc);
	int i = 0;

	while (argvv[i] != NULL)
	{
		int count = 0;
		char *test;
		while ((test = argvv[i][count]))
		{
			//fprintf(stderr, "\t%d : %s\n", count, test);
			count++;
		}
		res[i] = count;
		//fprintf(stderr, "\tRes[%d]: %d\n", i, res[i]);
		i++;
	}
	return res;
}

/**
 * @brief Detects any valid metachars in the given command
 * and 'translates' them to a valid path or argument for the minishell
 *
 * @param argv Input command
 */
void metacharsHandler(char *command[])
{
	for (int i = 1; command[i]; i++)
	{
		if (command[i][0] == '~' && metaUser(command[i]) < 0)
			return;
		if (strchr(command[i], '$') != NULL && metaVar(command[i]) < 0)
			return;
	}
}

/**
 * @brief
 *
 * @param cursor
 * @return 0 if found | -1 for error
 */
int metaUser(char *cursor)
{
	char *possible_user = cursor + 1; // Quitamos el '~'
	// no user given, use the current user
	if (possible_user[0] == '\0')
		strcpy(possible_user, getenv("USER"));

	if (!followsFormat(possible_user, NULL))
	{
		errorPrint("Given user doesn't follow format");
		return -1;
	}

	struct passwd *user = getpwnam(possible_user);
	if (user == NULL)
	{
		errno = EINVAL;
		// make a formatted error message
		char *error_message = malloc(sizeof(char) * (strlen(possible_user) + strlen("No existe el usuario ") + 1));
		strcpy(error_message, "No existe el usuario -> ");
		strcat(error_message, possible_user);
		errorPrint(error_message);
		return -1;
	}
	// get the user's home directory
	strncpy(cursor,user->pw_dir,strlen(user->pw_dir));

	return 0;
}

/**
 * @brief Swaps the given command with the env
 * variable value
 *
 * @param cursor
 * @return 0 if found | 1 if not found | -1 for error
 */
int metaVar(char *cursor)
{
	char *pos;
	if ((pos = strchr(cursor, '$') + 1) == NULL)
	{
		return 1;
	}

	char *res = malloc(sizeof(char) * MAX_PATH);
	res[0] = '\0';

	int offset = pos - cursor - 1;

	// check if pos is further into the string than command[n_arg]
	if (offset != 0)
	{
		strncat(res, cursor, offset);
	}
	int var_size;
	// Check format
	if (!(followsFormat(pos, &var_size)))
	{
		errorPrint("Given user doesn't follow format");
		return -1;
	}

	// Allocating memory for expanding var
	char *var = malloc(sizeof(char) * var_size);
	var[var_size] = '\0';
	// Copying pos name @ var
	strncpy(var, pos, var_size);

	// Retriving it from enviromment
	char *var_value = getenv(var);
	if (var_value == NULL)
	{
		errorPrint("Variable de entorno undefined");
		return -1;
	}
	strcat(res, var_value);

	// Concatenate the rest of the command
	offset = strlen(pos) - strlen(var);
	if (offset > 0)
	{
		strncat(res, (pos + strlen(var)), offset);
	}
	strcpy(cursor, res);
	free(var);
	free(res);
	return 0;
}

int followsFormat(char *pos, int *follows_format)
{
	if (follows_format == NULL)
	{
		follows_format = malloc(sizeof(int));
	}
	char *format = "%*[_a-zA-Z0-9]%n";
	sscanf(pos, format, follows_format);
	return *follows_format;
}

/*===========REDIRECTS============*/

/**
 * @brief Detects if any of the filev components
 * has been requested
 *
 * @param filev[3]
 * @return Index of the redirected fd
 * @throw -1 if non has been requested
 */
int redirHandler(char **filev)
{
	int i = 0;
	while (i <= 3)
	{
		if (i == 3)
			return -1;
		if (filev[i] != NULL)
		{
			break;
		}
		i++;
	}
	return i;
}

/**
 * @brief Implemets the logic of redirect
 * 
 * @param i Requested redirect returned by redirHandler
 * @param filev 
 * @param original 
 * @return The index given in case of success | -1 if something wrong
 */
int redir(int i, char **filev, int *original)
{
	/* 0 = < STDIN	|| 1 = > STDOUT || 2 = >& STDERR */
	int fd;
	if (i == STDIN_FILENO)
	{
		fd = open(filev[i], O_RDONLY, 0666); // Solo leemos
	}
	else
	{
		fd = open(filev[i], O_CREAT | O_TRUNC | O_WRONLY, 0666);
	}

	*original = dup(i); // Duplicamos para poder restaurar

	// Control de errores
	if (fd < 3)
		return errorPrint("No se pudo redirigir");

	if (dup2(fd, i) < 0)
		return errorPrint("Error en el dup2 de redir");

	if (close(fd) < 0)
		return errorPrint("Error al cerrar el archivo de redirección");

	return i;
}

/*INTERNAL COMMANDS SELECT AND IMPLEMENTATION*/

/**
 *  @brief Executes internal commands
 *
 *	@param argv Array de comandos
 *	@param argc Numero de argumentos
 *	@param redirs 3 posibles nombres de archivos para redirigir
 *	@param status Dirección de memoria que guarda la salida del programa
 *	@result 0 En caso de éxito
 *	@exception -1 en caso de que no sea un comando interno válido
 */
int exeIC(char **argv, int argc, char **redirs, int *status)
{

	int redAt = redirHandler(redirs), og;

	for (int i = 0; i < 4; i++)
	{
		if (!strcmp(function_map[i].name, argv[0]) && function_map[i].func)
		{
			
			if (redAt > -1 && redir(redAt, redirs, &og) != redAt)
				errorPrint("Error en la redirección");

			// Ejecuta la función
			*status = function_map[i].func(argv, argc);
			//Restauramos
			if(redAt> -1)
				dup2(og, redAt);
			return 0;
		}
	}

	return -1;
}

/* Used the --help section of these command to complete the
	closest implementation for this minishell */

/**
 * @brief
 *
 * @param [dir] (argc <=2)
 *		if length(argv) == 1 -> dir = $HOME
 * @return 0 if $PWD changed, -1 otherwise
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

	if (PID != getpid())
	{
		warningPrint("ATENCIÓN: La función se ejecuta en un subshell\n");
	}

	// Check for permisions and other errors when we changhe $PWD
	if (chdir(dir) == 0)
	{
		// get CWD again
		if (!getcwd(CWD, MAX_PATH))
		{
			errorPrint("Error al obtener el dir actual");
			return -3;
		}
		fprintf(stdout, "%s\n", CWD);
		return 0;
	}
	else
	{
		errorPrint("Can't access directory");
		return -2;
	}
}
/**
 * @brief Returns exeution time of the given command
 *
 * @param argv Array of arguments (Sin 'time')
 * @param argc Number of arguments (-1)
 * @return 0 if no err found
 * @throw -1 for failure
 */
int timeIC(char **argv, int argc)
{
#ifndef CLK_TCK
	int CLK_TCK = sysconf(_SC_CLK_TCK);
#endif
	clock_t time, userticks, systticks;
	struct tms t;
	int pid;

	// Si no hay argumentos, tiempo minishell e hijos
	if (argc == 0)
	{
		printTMS(t);
		return 0;
	}
	// Iniciar contadores
	userticks = 0;
	systticks = 0;

	if ((time = -times(&t)) == -1)
	{
		errorPrint("Error al comprobar los tiempos");
		return -1;
	}

	userticks = -t.tms_utime - t.tms_cutime;
	systticks = -t.tms_stime - t.tms_cstime;
	signal(SIGINT, SIG_IGN);  /* <<=== Ignorar se�ales de      */
	signal(SIGQUIT, SIG_IGN); /* <<=== terminaci�n interactiva */
	// Ejecución controlada
	if ((pid = littleDad(++argv)) < 0)
		return -1;
	time += times(&t);
	userticks += t.tms_utime + t.tms_cutime;
	systticks += t.tms_stime + t.tms_cstime;

	printf("%d.%03du ", (int)userticks / CLK_TCK);
	printf("%d.%03ds ", (int)systticks / CLK_TCK);
	printf("%ds.%03dr\n", (int)time / CLK_TCK);

	return 0;
}

/**
 * @brief Starts a clock at the begining of
 * the minishell execution
 */
void start_clock()
{
	TIME = times(NULL);
}

/**
 * @brief Prints the t parent values formatted
 * as requested
 *
 * @param t tms struct to be printed
 */
int printTMS(struct tms t)
{
	clock_t time2;
	if ((time2 = times(&t)) < 0)
	{
		return -1;
	}

	printf("%d.%03du ", t.tms_utime);
	printf("%d.%03ds ", t.tms_stime);
	printf("%ds.%03dr\n", (time2 - TIME));

	return 0;
}

/**
 * @brief Acts as a little main function
 * that handles the forked process and executes the given command
 *
 * @param argv Command to be executed
 * @return pid of the child process
 * @throw -1 if error
 */
int littleDad(char *argv[])
{

	int status;
	int pid = fork();

	// Hijo ejecuta
	if (pid == 0)
	{
		if (execvp(argv[0], argv) < 0)
			errorPrint("Error al ejecutar el comando en time");
		exit(1);
	}

	// Padre espera y devuelve el status
	pid = waitpid(pid, &status, 0);
	if (status != 0)
	{
		char buff[50];
		sprintf(buff, "Bad ending status -> %d", status);
		errorPrint(buff);
	}
	return (pid == -1) ? -1 : pid;
}

/**
 * @brief Emulates umask command in unix
 *
 * @param argv Arguements of umask + command itself
 * @param argc Number of arguments
 * @if OK @return 0
 * @else @return -1
 */
int umaskIC(char **argv, int argc)
{
	mode_t tmp_mask, new_mask;
	// Args control
	switch (argc)
	{
	case 0:
		// Prints current umask
		tmp_mask = umask(0); // change it to whatever but save the old one on return
		umask(tmp_mask);	 // change it back
		fprintf(stdout, "%o\n", tmp_mask);
		break;
	case 1:; // DONT REMOVE Solves a label error
		char *error;
		new_mask = (mode_t)strtol(argv[1], &error, 8);

		if (*error != '\0')
		{
			errorPrint(strcat(error, "Please imput a valid Base-8 number"));
			return -1;
		}
		tmp_mask = umask(new_mask);
		// printf("0%03o -> 0%03o\n", tmp_mask, new_mask);
		printf("%o", new_mask);
		break;

	default:
		errorPrint("Cant have more than 1 argument for 'Umask'");
		return -1;
	}
	return 0;
}

int readIC(char **argv, int argc)
{
	return -1;
}

/* DEBUGGIN TOOLS */

void printIntArray(int *arr)
{
	warningPrint("\nPriting int array -> ");
	int i, x;
	printf("[");

	for (i = 0; (x = arr[i]); i++)
	{
		printf("%d, ", x);
	}

	printf("]\n");
}

void printCharArray(char *arr)
{
	warningPrint("\nPriting Char array -> ");
	int i;
	char c;
	printf("%c", '"');

	for (i = 0; (c = arr[i]); i++)
	{
		printf("%c", c);
	}
	printf("%c\n", '"');
}

int getFDsOpen()
{
	DIR *dp = opendir("/proc/self/fd");
	if (dp == NULL)
	{
		printf("NULL");
		return -1;
	}
	struct dirent *de;
	int count = -2; // '.', '..', dp

	if (dp == NULL)
		return -1;

	while ((de = readdir(dp)) != NULL)
	{
		if (count >= 0)
		{
			printf("%s, ", de->d_name);
		}
		count++;
	}

	(void)closedir(dp);
	return count;
}

void printFlags()
{
	printf("isFirst-> %s\n", isFirst ? "true" : "false");
	printf("isLast->%s\n", isLast ? "true" : "false");
	printf("in_pipe %s\n", in_pipe ? "true" : "false");
}

char *printRedir(int redir)
{
	char *result;
	switch (redir)
	{
	case 0:
		result = "IN";
		break;
	case 1:
		result = "OUT";
		break;
	default:
		result = "ERR";
		break;
	}
	return result;
}