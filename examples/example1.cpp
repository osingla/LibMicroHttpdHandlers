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
 * TODO
 */
static void http_request_completed( void *cls, struct MHD_Connection *connection,
	void **con_cls, enum MHD_RequestTerminationCode toe ) {


	if ( (con_cls == NULL) || (*con_cls == NULL) )
		return;

//  DEBUG("... completed %s : toe=%d", __FUNCTION__, toe);

}   // http_request_completed

/**
 * TODO
 */
static int http_answer_to_connection( void *cls, struct MHD_Connection *connection,
	const char *url, const char *method, const char *version,
	const char *upload_data, size_t *upload_data_size, void **con_cls ) {

    DEBUG("%s - %s - %s", method, url, version);

	if (!strcmp(method, "GET")) {

		bool is_a_file;
		void const *what = handlers.search_get_handler(url, &is_a_file);
		DEBUG("[%s]", what);
		if (what) {
			if (is_a_file)
				return handlers.send_file(connection, url, (const char *)what);
		}

	}

    return MHD_NO;
}								// Httpd::http_answer_to_connection

int main( int argc, char *argv[] ) {

	DEBUG("");
    DEBUG("------- Starting test program -------");
    
    handlers.set_file_get_handler("/", "examples/example1/index.html");
    handlers.set_file_get_handler("/LinuxFromScratch.css", "examples/example1/LinuxFromScratch.css");
    handlers.set_file_get_handler("/LinuxFromScratch.js", "examples/example1/LinuxFromScratch.css");
    handlers.set_file_get_handler("/image1.png", "html1/image1.png");
    handlers.set_file_get_handler("/favicon.ico", "examples/example1/favicon.ico");
    handlers.SET_GET_HANDLER("/test", test_page);

    handlers.dump_handlers();

	http_daemon = MHD_start_daemon(
		MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG,
		HTTP_PORT,
		NULL, NULL,
		http_answer_to_connection, NULL,
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

