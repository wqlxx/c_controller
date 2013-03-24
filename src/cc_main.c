/*
 * cc_of_msg_action functions.
 *
 * Author: qiang wang <wqlxx@yahoo.com.cn>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "cc_main.h"

void usage(const char* program_name)
{
    printf("%s: nox runtime\n"
	   "usage: %s [OPTIONS] [APP[=ARG[,ARG]...]] [APP[=ARG[,ARG]...]]...\n"
           "\nInterface options (specify any number):\n"
#ifdef HAVE_NETLINK
           "  -i nl:DP_ID             via netlink to local datapath DP_IDX\n"
#endif
           "  -i ptcp:[IP]:[PORT]     listen to TCP PORT on interface specified by IP\n"
           "                          (default: 0.0.0.0:%d)\n"
           "  -i pssl:[IP]:[PORT]:KEY:CERT:CONTROLLER_CA_CERT\n"
           "                          listen to SSL PORT on interface specified by IP\n"
           "                          (default: 0.0.0.0:%d)\n"
           "  -i pcap:FILE[:OUTFILE]  via pcap from FILE (for testing) write to OUTFILE\n"
           "  -i pcapt:FILE[:OUTFILE] same as \"pcap\", but delay packets based on pcap timestamps\n"
           "  -i pgen:                continuously generate packet-in events\n"
           "\nNetwork control options (must also specify an interface):\n"
           "  -u, --unreliable        do not reconnect to interfaces on error\n",
	   program_name, program_name, OFP_TCP_PORT, OFP_SSL_PORT);
    leak_checker_usage();
    printf("\nOther options:\n"
           "  -c, --conf=FILE         set configuration file\n"
           "  -d, --daemon            become a daemon\n"
           "  -l, --libdir=DIRECTORY  add a directory to the search path for application libraries\n"
           "  -p, --pid=FILE          set pid file\n"
           "  -n, --info=FILE         set controller info file\n"
	   "  -v, --verbose           make console log verbose (shows INFO messages -- use twice for DBG)\n"
#ifndef LOG4CXX_ENABLED
	   "  -v, --verbose=CONFIG    configure verbosity\n"
#endif
	   "  -h, --help              display this help message\n"
	   "  -V, --version           display version information\n");
    exit(EXIT_SUCCESS);
}

//struct cc_switch_proc cc_switch_proc={.num_sw=0};
static cc_socket* listen_socket;
//static list_element* sw_info_table;

/*
struct sw_info_app {
	pid_t sw_pid;
	uint64_t dpid; 
};
typedef struct sw_info_app sw_info_app;
*/

static int
cc_search_sw_info_table(pid_t pid);
{
	printf("switch exit!");
	return 0;
}


static void
sig_child(int signo)
{
	pid_t pid;
	int* status;
	pid = waitpid(-1,status,WNOHANG);
	cc_search_sw_info_table(pid);
}


int main(int argc,char **argv)
{

	//parse_argv(argc,argv);
	signal(SIGPIPE, SIG_IGN);
	/*handle the signal caused by child's exit*/
	signal(SIGCHLD, sig_child);
	//cc_init_of(&listen_socket);

	//cc_init_sw_info_table(sw_info_table);

	//cc_init_app();

	cc_init_of_socket(listen_socket);

	cc_polling(sw_info_table,listen_socket);

	cc_finalize_of_socket(listen_socket->fd);


	//cc_finalize_sw_info_table(sw_info_table);

	return 0;
}
