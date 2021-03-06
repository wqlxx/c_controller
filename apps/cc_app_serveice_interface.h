
struct ofpapp_flow_mod {
	uint8_t type;
	uint8_t data[0];
}

enum ofp_app_type {

	OFPAPP_HELLO,
	OFPAPP_ECHO_REPLY,
	OFPAPP_ECHO_REQUEST,
	OFPAPP_ERROR,
	OFPAPP_FEATURE_REQUEST,
	OFPAPP_FLOW_MOD,
	OFPAPP_FLOW_STATS_REQUEST,
	OFPAPP_GET_CONFIG_REQUEST,
	OFPAPP_GET_DESC_STATS_REQUEST,
	OFPAPP_SET_CONFIG,
	OFPAPP_PORT_MOD,
	OFPAPP_PORT_STATS_REQUEST,
	OFPAPP_STATS_REQUEST,
	OFPAPP_TABLE_STATS_REQUEST,
	OFPAPP_VENDOR_STATS_REQUEST,
	OFPAPP_QUEUE_STATS_REQUEST,
	OFPAPP_AGGREGATE_STATS_REQUEST,
	OFPAPP_PACKET_OUT,
	
};

