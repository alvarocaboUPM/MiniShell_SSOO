/* Times_C.c */
/*
 * Copyright 2005 datsi.fi.upm.es.
 * Universidad Politécnica de Madrid, Facultad de Informática
 *
 * Autor: Francisco Rosales
 * Versión: 1.0		Fecha: 05 Dic 2005
 * Descripción:
 *	Calcula los tiempos consumidos por la ejecución del
 *	mandato indicado.
 *	Utiliza las funciones auxiliares fork_command y wait_process.
 *	El padre ignora las señales generables por teclado.
 *	Presenta segundos y sub-segundos hasta la precisión establecida.
 * Uso:
 *	Times_C mandato [argumentos...]
 */
#define MYNAME	"Times_C"

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Lanza el mandato indicado.
// Retorna: -1 (si error) o PID del proceso hijo.
int fork_command(char * argv[])
{
	int pid = fork();

	if (pid != 0)
		return pid;
	execvp(argv[0], argv);
	perror(MYNAME);
	exit(1);
}

// Espera la terminación de un proceso.
// Retorna: -1 (si error) o estado de terminación.
int wait_process(int pid)
{
	int status;

	pid = waitpid(pid, &status, 0);
	return (pid == -1) ? -1 : status;
}



/* 
 * Cronometra la ejecución de un mandato.
 */
int chron_command(char * argv[]);

int main(int argc, char * argv[])
{
	int ret;

	ret = chron_command( NULL );
	if (ret == -1) {
		perror(MYNAME);
		return 1;
	}
	ret = chron_command(argv+1);
	if (ret == -1) {
		perror(MYNAME);
		return 1;
	}
	return 0;
}





#include <signal.h>
#include <time.h>
#include <sys/times.h>






#define precision	3

/* 
 * Cronometra la ejecución de un mandato.
 */
int chron_command(char * argv[])
{
	static clock_t startticks = 0;
	clock_t realticks, userticks, systticks;
	struct tms ticks;
	int pid, status;
#ifndef CLK_TCK
	int CLK_TCK = sysconf(_SC_CLK_TCK);
#endif

	if (argv == NULL) {
		startticks = times(NULL);
		return (int)startticks;
	}

	realticks = -startticks;
	userticks = 0;
	systticks = 0;

	if (argv[0] != NULL) {
		realticks = -times(&ticks);
		userticks = -ticks.tms_utime - ticks.tms_cutime;
		systticks = -ticks.tms_stime - ticks.tms_cstime;
		if ((pid = fork_command( argv )) == -1)
			return -1;
		signal(SIGINT,  SIG_IGN);   /* <<=== Ignorar señales de      */
		signal(SIGQUIT, SIG_IGN);   /* <<=== terminación interactiva */
		if ((status = wait_process(pid)) == -1)
			return -1;
	}
		realticks += times(&ticks);
		userticks += ticks.tms_utime + ticks.tms_cutime;
		systticks += ticks.tms_stime + ticks.tms_cstime;
		fprintf(stderr, MYNAME": »»» Real:%.*f\" User:%.*f\" Syst:%.*f\"\n",
			precision, (double)realticks/CLK_TCK,
			precision, (double)userticks/CLK_TCK,
			precision, (double)systticks/CLK_TCK);
	return 0;
}
