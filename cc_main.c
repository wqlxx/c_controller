#include <stdio.h>
#include "cc_log.h"
#include "cc_pares.h"
#include "cc_init_of.h"

	/*void usage(const char* program_name)
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
}*/

	/*
	
	*/

//struct cc_switch_proc cc_switch_proc={.num_sw=0};
struct cc_socket listen_socket;
sw_info global_sw_table[CC_MAX_NUM_SWITCH];

int main(int argc,char **argv)
{

	parse_argv(argc,argv);
	signal(SIGPIPE, SIG_IGN);
	//cc_init_of(&listen_socket);
	cc_init_listenfd(&listen_socket);
	while(1)
	{
		cc_recv_conn_from_switch(&listen_socket);
	}
	return 0;
}
