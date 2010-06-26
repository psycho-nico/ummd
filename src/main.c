/*
 *  UMMD - Micro MultiMedia Daemon
 *
 *  Copyright (C) 2010 Nicolas Thill <nicolas.thill@gmail.com>
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
 
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "autoconf.h"

#include "conf.h"
#include "core.h"
#include "util/log.h"

static char *me;

my_conf_t *my_conf;
my_core_t *my_core;

char my_usage[] =
	"Usage: %1$s [OPTIONS]\n"
	"  Options:\n"
	"    -C FILE   read configuration from FILE\n"
	"                (default: " MY_CFG_DIR "/%1$s.conf)\n"
	"    -L SPEC   use SPEC for logging, where SPEC can be:\n"
	"                - file:PATH   use regular file specified by PATH\n"
	"                - stderr      standard error\n"
	"                - stdout      standard output\n"
	"                - syslog      system logger\n"
#ifdef MY_DEBUGGING
	"                (default: stderr)\n"
#else
	"                (default: syslog)\n"
#endif
	"    -P FILE   store process id in FILE\n"
	"                (default: " MY_RUN_DIR "/%1$s.pid)\n"
	"\n"
	"    -?, -h    display usage information and exit\n"
	"    -V        display version information and exit\n"
	"\n";

char my_version[] =
	"Version: %s %s\n"
	"Copyright (C) 2010 Nicolas Thill <nicolas.thill@gmail.com>\n"
	"License: GPLv2, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>.\n"
	"\n";

static int my_show_usage(void)
{
	fprintf(stdout, my_usage, me);
}

static int my_show_version(void)
{
	fprintf(stdout, my_version, me, PACKAGE_VERSION);
}

static void my_handle_children(int sig)
{
	pid_t pid;
	int status;
	
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		my_log(MY_LOG_NOTICE, "child (pid: %d) exited with status %d");
	}
}

static void my_handle_shutdown(int sig)
{
	MY_DEBUG("received signal '%s'", strsignal(sig));
	my_core_stop(my_core);
}

static void my_install_sig_handlers(void)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = my_handle_children;
	sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
	sigaction(SIGCHLD, &sa, NULL);

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = my_handle_shutdown;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

int main(int argc, char *argv[])
{
	int opt_c;
	char buf[FILENAME_MAX];

	me = strrchr(argv[0], '/');
	if (me == NULL ) {
		me = argv[0];
	} else {
		me++;
	}
	my_log_init(me);

	my_conf = my_conf_create();

	while ((opt_c = getopt(argc, argv, "C:L:Vh?")) != -1)
		switch (opt_c) {
		case 'C':
			my_conf->cfg_file = optarg;
			break;
		case 'L':
			my_conf->log_file = optarg;
			break;
		case 'P':
			my_conf->pid_file = optarg;
			break;
		case 'V':
			my_show_version();
			exit(0);
			break;
		case '?':
		case 'h':
			my_show_usage();
			exit(0);
			break;
		default:
			MY_ERROR("unknow argument '-%c'", (int)opt_c);
			my_show_usage();
			exit(1);
	}

	my_conf->log_level = -1;

	if (my_conf->cfg_file == NULL) {
		snprintf(buf, sizeof(buf), "%s/%s.conf", MY_CFG_DIR, me);
		my_conf->cfg_file = strdup(buf);
	}

	my_conf_parse(my_conf);

	if (my_conf->log_file == NULL) {
#ifdef MY_DEBUGGING
		my_conf->log_file = "stderr";
#else
		my_conf->log_file = "syslog";
#endif
	}

	if (my_conf->log_level < 0) {
		my_conf->log_level = MY_LOG_NOTICE;
	}

	if (my_conf->pid_file == NULL) {
		snprintf(buf, sizeof(buf), "%s/%s.pid", MY_RUN_DIR, me);
		my_conf->pid_file = strdup(buf);
	}

	if (my_log_open(my_conf->log_file, my_conf->log_level)) {
		exit(1);
	}

	my_log(MY_LOG_NOTICE, "started");

	my_core = my_core_create();
	if (my_core) {
		my_core_init(my_core, my_conf);
		my_install_sig_handlers();
		my_core_loop(my_core);
		my_core_destroy(my_core);
	}

	my_log(MY_LOG_NOTICE, "ended");

	my_log_close();

	return 0;
}
