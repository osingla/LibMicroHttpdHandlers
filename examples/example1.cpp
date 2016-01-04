#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" {
#include <microhttpd.h>
}

#include "../Logg.h"

#include "LibMicroHttpdHandlers.h"
#include "DataPrintf.h"

#define HTTP_PORT 8088

static struct MHD_Daemon *http_daemon = NULL;

static LibMicroHttpdHandlers handlers;     ///< Hash table that associates URL to a file to send


/**
 * libmicrohttpd logger
 */
static void httpd_log( void *arg, const char *fmt, va_list ap ) {
	vsyslog( LOG_INFO, fmt, ap );
}   // httpd_log

/**
 * handle for	handle for	/configuration/get-audio-video/values.xml
 */
static int test_page( void *cls, struct MHD_Connection *connection, char const *url ) {

    return MHD_NO;
}	// test_page

/**
 * handle for /gps-geofence/get-values.xml
 */
int http_geofence_get_values_xml( void *cls, struct MHD_Connection *connection, char const *url ) {

	DataPrintf xml;

	xml.printf("<xmlstatus>");
	xml.printf(" <values ");
	xml.printf("   Lat=%lf", (double)35.780421);
	xml.printf("   Lon=%lf", (double)-78.639214);
	xml.printf(" />");
	xml.printf("</xmlstatus>");

	return xml.send(connection, "text/xml");
}								// http_geofence_get_values_xml

/**
 * TODO
 */
static void http_request_completed( void *cls, struct MHD_Connection *connection,
	void **con_cls, enum MHD_RequestTerminationCode toe ) {

	if ( (con_cls == NULL) || (*con_cls == NULL) )
		return;

//  DEBUG("... completed %s : toe=%d", __FUNCTION__, toe);

}   // http_request_completed

int main( int argc, char *argv[] ) {

	DEBUG("");
    DEBUG("------- Starting test program -------");
    
    handlers.set_file_get_handler("/", "examples/example1/index.html");
    handlers.set_file_get_handler("/LinuxFromScratch.css", "examples/example1/LinuxFromScratch.css");
    handlers.set_file_get_handler("/LinuxFromScratch.js", "examples/example1/LinuxFromScratch.css");
    handlers.set_file_get_handler("/image1.png", "html1/image1.png");
    handlers.set_file_get_handler("/favicon.ico", "examples/example1/favicon.ico");
    handlers.SET_GET_HANDLER("/test", test_page);
    handlers.SET_GET_HANDLER("/get-gps-pos.xml", http_geofence_get_values_xml);
    handlers.set_dir_get_handler("/dojo", "examples/dojo/dojox/gesture");

    handlers.dump_handlers();

	http_daemon = MHD_start_daemon(
		MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG,
		HTTP_PORT,
		NULL, NULL,
		handlers.http_answer_to_connection, &handlers,
		MHD_OPTION_NOTIFY_COMPLETED, http_request_completed, NULL,	// function called whenever a request has been completed
		MHD_OPTION_CONNECTION_TIMEOUT, 0,							// no timeout
		MHD_OPTION_EXTERNAL_LOGGER, httpd_log, NULL,
		MHD_OPTION_END );
	if (http_daemon == NULL) {
		DEBUG( "Unable to start http service - %m" );
        exit(-1);
    }

    for (;;) {
        sleep(1);
    }

    MHD_stop_daemon( http_daemon );

   return 0;
}   // main

