
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {
#include <microhttpd.h>
}

#include "Logg.h"

#include "DataPrintf.h"


/**
 * Constructor
 */
DataPrintf::DataPrintf( ) : data(NULL), data_len(0), data_len_alloctd(0) {
	data_len_alloctd = alloc_size;
	data_len = 0;
	data = (char *)malloc(data_len_alloctd);
	strcpy(data, "");
	temp_str = (char *)malloc(data_len_alloctd);
}								// DataPrintf::DataPrintf

/**
 * Destructor
 */
DataPrintf::~DataPrintf( ) {
	free(temp_str);
}								// DataPrintf::~DataPrintf

/**
 * TODO
 */
void DataPrintf::printf(char const *fmt, ...) {

	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf(temp_str, alloc_size-8, fmt, ap);
	va_end(ap);
	strcat(temp_str, "\r\n");
	len += 2;

	if (len + data_len >= data_len_alloctd) {
		data_len_alloctd += alloc_size;
		data = (char *)realloc(data, data_len_alloctd);
	}

	strcat(data, temp_str);
	data_len += len;
//	if (data_len != strlen(data))
//		ERROR( "!!! %d  %d  %d ", data_len, strlen(data), len );
}	// DataPrintf::printf

/**
 * TODO
 */
int DataPrintf::send(struct MHD_Connection *connection, char const *mime_type) {

	struct MHD_Response *response = MHD_create_response_from_data(
		data_len, data,
		MHD_YES,		// must free - if true: MHD should free data when done;
		MHD_NO);		// must copy - if true: MHD allocates a block of memory and use it to make a copy of data
//	if ( !response )
//		return MHD_NO;
	MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, mime_type);
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	return ret;
}	// DataPrintf::send
