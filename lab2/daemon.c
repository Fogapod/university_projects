#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "logger.h"
#include "daemon.h"

void sig_handler(int sig);
void set_pid_file(pid_t pid);

void daemonize(void)
{
	struct sigaction newSigAction;
	newSigAction.sa_handler = sig_handler;
	sigaction(SIGTERM, &newSigAction, NULL);
	sigaction(SIGKILL, &newSigAction, NULL);

	pid_t pid = fork();

	if (pid == -1)
    {
        log_info("Failed starting daemon process");
        exit(3);
    }
    else if (pid > 0)
	{
		log_debug("Daemon process started (%d)", pid);
		set_pid_file(pid);
		exit(0);
	}

	umask(0);
	setsid();

	chdir(ROOT_DIR);
	close(STDIN_FILENO);
	// close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void sig_handler(int sig)
{
	log_trace("Recieved signal: %s", strsignal(sig));

	switch (sig)
	{
		case SIGKILL:
		case SIGTERM:
			log_trace("Exiting");
			exit(0);
		default:
			log_debug("Unknown signal!");
    }
}

void set_pid_file(pid_t pid)
{
	FILE *f = fopen(PID_FILE, "w");

	if (f)
	{
		log_trace("Writing to %s ...", PID_FILE);
		fprintf(f, "%u", pid);
		fclose(f);
	}
	else
	{
		log_info("Can't write to %s! Daemon not started", PID_FILE);
        exit(3);
	}
}
