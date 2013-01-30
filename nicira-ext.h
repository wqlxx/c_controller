/*
 * Distributed under the terms of the GNU GPL version 2.
 * Copyright (c) 2008 Nicira Networks
 */

#ifndef OPENFLOW_NICIRA_EXT_H
#define OPENFLOW_NICIRA_EXT_H 1

#include "openflow/openflow.h"

#define NICIRA_OUI_STR "002320"

/* The following vendor extensions, proposed by Nicira Networks, are not yet
 * ready for standardization (and may never be), so they are not included in
 * openflow.h. */

#define NX_VENDOR_ID 0x00002320

enum nicira_type {
    /* Switch status request.  The request body is an ASCII string that
     * specifies a prefix of the key names to include in the output; if it is
     * the null string, then all key-value pairs are included. */
    NXT_STATUS_REQUEST,

    /* Switch status reply.  The reply body is an ASCII string of key-value
     * pairs in the form "key=value\n". */
    NXT_STATUS_REPLY,

    /* Configure an action.  Most actions do not require configuration
     * beyond that supplied in the actual action call. */
    NXT_ACT_SET_CONFIG,

    /* Get configuration of action. */
    NXT_ACT_GET_CONFIG,

    /* Remote command execution.  The request body is a sequence of strings
     * delimited by null bytes.  The first string is a command name.
     * Subsequent strings are command arguments. */
    NXT_COMMAND_REQUEST,

    /* Remote command execution reply, sent when the command's execution
     * completes.  The reply body is struct nx_command_reply. */
    NXT_COMMAND_REPLY,

    /* Configure whether Flow End messages should be sent. */
    NXT_FLOW_END_CONFIG,

    /* Sent by switch when a flow ends.  These messages are turned into
     * ofp_flow_removed and NetFlow messages in user-space. */
    NXT_FLOW_END
};

struct nicira_header {
    struct ofp_header header;
    uint32_t vendor;            /* NX_VENDOR_ID. */
    uint32_t subtype;           /* One of NXT_* above. */
};
OFP_ASSERT(sizeof(struct nicira_header) == sizeof(struct ofp_vendor_header) + 4);


enum nx_snat_command {
    NXSC_ADD,
    NXSC_DELETE
};

/* Configuration for source-NATing */
struct nx_snat_config {
    uint8_t command;        /* One of NXSC_*. */
    uint8_t pad[3];
    uint16_t port;          /* Physical switch port. */
    uint16_t mac_timeout;   /* Time to cache MAC addresses of SNAT'd hosts
                               in seconds.  0 uses the default value. */

    /* Range of IP addresses to impersonate.  Set both values to the
     * same to support a single address.  */
    uint32_t ip_addr_start; 
    uint32_t ip_addr_end;

    /* Range of transport ports that should be used as new source port.  A
     * value of zero, let's the switch choose.*/
    uint16_t tcp_start;
    uint16_t tcp_end;
    uint16_t udp_start;
    uint16_t udp_end;

    /* MAC address to use for ARP requests for a SNAT IP address that 
     * comes in on a different interface than 'port'.  A value of all 
     * zeros silently drops those ARP requests.  Requests that arrive 
     * on 'port' get a response with the mac address of the datapath 
     * device. */
    uint8_t mac_addr[OFP_ETH_ALEN];
    uint8_t pad2[2];
};
OFP_ASSERT(sizeof(struct nx_snat_config) == 32);

/* Action configuration.  Not all actions require separate configuration. */
struct nx_act_config {
    struct nicira_header header;
    uint16_t type;          /* One of OFPAT_* */
    uint8_t pad[2];
    union {
        struct nx_snat_config snat[0];
    };                      /* Array of action configurations.  The number 
                               is inferred from the length field in the 
                               header. */
};
OFP_ASSERT(sizeof(struct nx_act_config) == 20);


enum nx_action_subtype {
    NXAST_SNAT                      /* Source-NAT */
};

/* Action structure for NXAST_SNAT. */
struct nx_action_snat {
    uint16_t type;                  /* OFPAT_VENDOR. */
    uint16_t len;                   /* Length is 8. */
    uint32_t vendor;                /* NX_VENDOR_ID. */
    uint16_t subtype;               /* NXAST_SNAT. */
    uint16_t port;                  /* Output port--it must be previously 
                                       configured. */
    uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct nx_action_snat) == 16);

/* Header for Nicira-defined actions. */
struct nx_action_header {
    uint16_t type;                  /* OFPAT_VENDOR. */
    uint16_t len;                   /* Length is 8. */
    uint32_t vendor;                /* NX_VENDOR_ID. */
    uint16_t subtype;               /* NXAST_*. */
    uint8_t pad[6];
};
OFP_ASSERT(sizeof(struct nx_action_header) == 16);

/* Status bits for NXT_COMMAND_REPLY. */
enum {
    NXT_STATUS_EXITED = 1 << 31,   /* Exited normally. */
    NXT_STATUS_SIGNALED = 1 << 30, /* Exited due to signal. */
    NXT_STATUS_UNKNOWN = 1 << 29,  /* Exited for unknown reason. */
    NXT_STATUS_COREDUMP = 1 << 28, /* Exited with core dump. */
    NXT_STATUS_ERROR = 1 << 27,    /* Command could not be executed. */
    NXT_STATUS_STARTED = 1 << 26,  /* Command was started. */
    NXT_STATUS_EXITSTATUS = 0xff,  /* Exit code mask if NXT_STATUS_EXITED. */
    NXT_STATUS_TERMSIG = 0xff,     /* Signal number if NXT_STATUS_SIGNALED. */
};

/* NXT_COMMAND_REPLY. */
struct nx_command_reply {
    struct nicira_header nxh;
    uint32_t status;            /* Status bits defined above. */
    /* Followed by any number of bytes of process output. */
};
OFP_ASSERT(sizeof(struct nx_command_reply) == 20);

enum nx_flow_end_reason {
    NXFER_IDLE_TIMEOUT,         /* Flow idle time exceeded idle_timeout. */
    NXFER_HARD_TIMEOUT,         /* Time exceeded hard_timeout. */
    NXFER_DELETE,               /* Flow was removed by delete command. */
    NXFER_EJECT                 /* Flow was ejected. */
};

struct nx_flow_end_config {
    struct nicira_header header;
    uint8_t enable;           /* Set to 1 to enable Flow End message
                                 generation.  0 to disable.  */
    uint8_t pad[3];
};
OFP_ASSERT(sizeof(struct nx_flow_end_config) == 20);

struct nx_flow_end {
    struct nicira_header header;
    struct ofp_match match;   /* Description of fields. */
    uint64_t cookie;          /* Opaque controller-issued identifier. */

    uint16_t priority;        /* Priority level of flow entry. */
    uint8_t reason;           /* One of NXFER_*. */

    uint8_t tcp_flags;        /* Union of seen TCP flags. */
    uint8_t ip_tos;           /* IP TOS value. */

    uint8_t send_flow_exp;    /* Send flow expiry to controller. */

    uint16_t idle_timeout;    /* Idle time before discarding (seconds). */

    uint64_t init_time;       /* Time flow started in milliseconds. */
    uint64_t used_time;       /* Time entry was last used in milliseconds. */
    uint64_t end_time;        /* Time flow ended in milliseconds. */

    uint64_t packet_count;
    uint64_t byte_count;
};
OFP_ASSERT(sizeof(struct nx_flow_end) == 112);

#endif /* openflow/nicira-ext.h */

