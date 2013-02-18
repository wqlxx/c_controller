#include "cc_create_of_msg.h"

/*func cc_create_of_header*/
/*parm: 
 *type: one of the OFPT_
 *len : length including this ofp_header
 *xid : transaction id
 */
buffer*
cc_create_of_header(const uint8_t type,const uint16_t len,const uint32_t xid)
{
	buffer* buf = alloc_buffer();
	if(buf == NULL)
	{
		log_err_for_cc("create of header failed");
		perror("craete error");
		return CC_ERROR;
	}

	struct ofp_header* header = append_back_buffer( buffer, len );
	memset(header,0,len);

	header->version = OFP_VERSION;
	header->type = type;
	header->length = htons(len);
	header->xid = htons(xid);

	return buf;
}
