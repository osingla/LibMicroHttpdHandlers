
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "Logg.h"

extern "C" {
#include <microhttpd.h>
}

#include "LibMicroHttpdHandlers.h"


/**
 * TODO
 */
LibMicroHttpdHandlers::LibMicroHttpdHandlers() {

	// User data
	user_data = NULL;

    // Current number of pairs {url,what}
    nb_pairs = 0;
    pairs = NULL;

}   // LibMicroHttpdHandlers::LibMicroHttpdHandlers

/**
 * TODO
 */
LibMicroHttpdHandlers::~LibMicroHttpdHandlers() {
}   // LibMicroHttpdHandlers::~LibMicroHttpdHandlers

/**
 * TODO
 */
void LibMicroHttpdHandlers::set_user_data( void *user_data) {
	this->user_data = user_data;
}	//

/**
 * TODO
 */
void *LibMicroHttpdHandlers::get_user_data() {
	return user_data;
}	// LibMicroHttpdHandlers::get_user_data

/**
 * Return "GET:", "PUT:" or "POST:"
 */
char const *LibMicroHttpdHandlers::add_prefix(type_t type) {
	switch (type) {
		case TYPE_GET  : return "Get: ";
		case TYPE_PUT  : return "Put: ";
		case TYPE_POST : return "Post:";
	}
	return "";
}	// LibMicroHttpdHandlers::add_prefix

/**
 * TODO
 */
int LibMicroHttpdHandlers::qsort_cmp_pair(const void *p1, const void *p2) {
	pair_t *q1 = (pair_t *)p1;
	pair_t *q2 = (pair_t *)p2;

	char xurl1[8+strlen(q1->url)];
	strcpy(xurl1, add_prefix(q1->type));
	strcat(xurl1, q1->url);

	char xurl2[8+strlen(q2->url)];
	strcpy(xurl2, add_prefix(q2->type));
	strcat(xurl2, q2->url);

//	DEBUG("%s   %s", xurl1, xurl2);
	return strcmp(xurl1, xurl2);
}	// LibMicroHttpdHandlers::qsort_cmp_pair

/**
 * TODO
 */
int LibMicroHttpdHandlers::set_file_get_handler(const char *url, const void *what) {

	// If the page filename is relative to the current directory, make it absolute
	char *abs_what = (char *)what;
	if (abs_what[0] != '/') {
		char cwd[512];
		getcwd(cwd, sizeof(cwd)-1);
		int l = strlen(cwd);
		abs_what = (char *)malloc(l + strlen((char *)what) + 8 );
		strcpy(abs_what, cwd);
		if ((l > 0) && (cwd[l-1] != '/'))
			strcat(abs_what, "/");
		strcat(abs_what, (char *)what);
	}

	// Add this new entry into the table
	if (!pairs)
		pairs = (pair_t *)malloc(sizeof(pair_t));
	else
		pairs = (pair_t *)realloc(pairs, sizeof(pair_t) * (nb_pairs+1));
	pair_t *p = &pairs[nb_pairs++];
	memset(p, 0, sizeof(pair_t));
	p->type = TYPE_GET;
	p->url = strdup(url);
	p->is_a_file = true;
	p->what = (const void *)strdup((const char *)abs_what);
	p->fnct_name = NULL;

	// Sort the handlers
	qsort(pairs, nb_pairs, sizeof(pair_t), qsort_cmp_pair);

    return 0;
}   // LibMicroHttpdHandlers::set_file_get_handler

/**
 * TODO
 */
int LibMicroHttpdHandlers::add_dir_get_handler( char const *url, char const *dirname ) {

	DIR *d = opendir(dirname);
    if (d == NULL) {
    	DEBUG("Directory %s - %m", dirname);
    	return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
    	char *p = entry->d_name;
        if ( !strcmp( p, "." ) || !strcmp( p, ".." ) )
            continue;
        int l = strlen(p);
    	char new_url[strlen(url) + l + 80];
    	sprintf(new_url, "%s/%s", url, p);
    	char new_dirname[strlen(dirname) + l + 80];
    	sprintf(new_dirname, "%s/%s", dirname, p);
        if ((entry->d_type == DT_REG) && (access( new_dirname, F_OK ) == 0)) {
        	if (access( new_dirname, F_OK ) == 0) {
        		DEBUG( "%s   -->  %s", new_url, new_dirname );
        		set_file_get_handler(strdup(new_url), strdup(new_dirname));
//        		add(strdup(new_url), strdup(new_dirname));
        	}
        }
        else if ((entry->d_type == DT_DIR) || (entry->d_type == DT_LNK)) {
        	add_dir_get_handler(new_url, new_dirname);
        }
    }							// while

    closedir(d);
    return 0;
}	//  LibMicroHttpdHandlers::add_dir_get_handler

/**
 * TODO
 */
int LibMicroHttpdHandlers::set_get_handler(const char *url, pf_t pf, char const *fnct_name) {

	// Add this new entry into the table
	if (!pairs)
		pairs = (pair_t *)malloc(sizeof(pair_t));
	else
		pairs = (pair_t *)realloc(pairs, sizeof(pair_t) * (nb_pairs+1));
	pair_t *p = &pairs[nb_pairs++];
	p->type = TYPE_GET;
	p->url = strdup(url);
	p->is_a_file = false;
	p->what = (const void *)pf;
	p->fnct_name = (fnct_name) ? strdup(fnct_name) : NULL;

	// Sort the handlers
	qsort(pairs, nb_pairs, sizeof(pair_t), qsort_cmp_pair);

    return 0;
}   // LibMicroHttpdHandlers::set_get_handler

/**
 * TODO
 */
const void *LibMicroHttpdHandlers::search_get_handler(const char *url, bool *is_a_file) {

	return search_handler(url, TYPE_GET, is_a_file);
}   // LibMicroHttpdHandlers::search_get_handler

/**
 * TODO
 */
const void *LibMicroHttpdHandlers::search_handler(const char *url, type_t type, bool *is_a_file) {

	pair_t p;
	memset(&p, 0, sizeof(p));
	p.url = url;
	p.type = type;

	pair_t *q = (pair_t *)bsearch(&p, pairs, nb_pairs, sizeof(pair_t), qsort_cmp_pair);
	if (!q)
		return NULL;

	if (q->type == TYPE_GET) {
		if (!is_a_file)
			return NULL;
		*is_a_file = q->is_a_file;
		return q->what;
	}

    return NULL;
}   // LibMicroHttpdHandlers::search_handler

/**
 * TODO
 */
void LibMicroHttpdHandlers::dump_handlers( ) {

	DEBUG("%d handlers", nb_pairs);
	for (int n = 0; n < nb_pairs; n++ ) {
		pair_t *p = &pairs[n];
		switch (p->type) {
			case TYPE_GET :
				if (p->is_a_file)
					DEBUG("%4d: %-28s -> %s", n, p->url, (const char *)p->what);
				else
					DEBUG("%4d: %-28s -> %s", n, p->url, (p->fnct_name) ? p->fnct_name : "");
				break;
		}
	}

}   // LibMicroHttpdHandlers::search_url

/**
 * Return file length
 */
int LibMicroHttpdHandlers::filelen(char const *filename) {

	int fd = open( filename, O_RDONLY );
	if (fd == -1)
		return -1;
	int len = lseek( fd, 0, SEEK_END );
	close( fd );
	return len;

} // LibMicroHttpdHandlers::filelen

/**
 * TODO
 */
int LibMicroHttpdHandlers::send_page(struct MHD_Connection *connection, const char *page, int status_code) {

	struct MHD_Response *response = MHD_create_response_from_data( strlen(page), (void *) page, MHD_NO, MHD_YES );
	if ( !response )
		return MHD_NO;

	MHD_queue_response( connection, status_code, response );
	MHD_destroy_response( response );

	return MHD_YES;
}	// LibMicroHttpdHandlers::send_page

/**
 * TODO
 * 	MHD_ContentReaderCallback
 */
ssize_t LibMicroHttpdHandlers::send_file_reader_cb( void *cls, uint64_t pos, char *buf, size_t max ) {

	send_file_reader_t *t = (send_file_reader_t *)cls ;

	off_t o = lseek( t->fd, pos, SEEK_SET );
	if ( o == -1 ) {
		DEBUG( "--> %m" );
		return -1;
	}

	ssize_t s = read( t->fd, buf, max );
	if ( s == -1 ) {
		DEBUG( "--> %m" );
		return -1;
	}

//	static int cpt = 0;
//	DEBUG( "%4d: %s: fd=%d pos=%lld max=%d s=%d", cpt, t->url, t->fd, pos, max, s );
	return s;
}	// LibMicroHttpdHandlers::send_file_reader_cb

/**
 * TODO
 *	MHD_ContentReaderFreeCallback
 */
void LibMicroHttpdHandlers::send_file_reader_done( void *cls ) {
	send_file_reader_t *t = (send_file_reader_t *)cls ;

	close( t->fd );
	free( t->url );
	free( t->fname );
	free( t );
}	// LibMicroHttpdHandlers::send_file_reader_done

/**
 * Load a static file and send it
 */
int LibMicroHttpdHandlers::send_file( struct MHD_Connection *connection, const char *url, const char *fname ) {

	// Find the right mime type for this kind of file
	char const *mime_type = "text/plain";
	int l = strlen( fname );
	if ( (l >= 6) && !strcasecmp( &fname[l-5], ".html" ) )
		mime_type = "text/html";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".xml" ) )
		mime_type = "text/xml";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".ico" ) )
		mime_type = "image/x-icon";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".jpg" ) )
		mime_type = "image/jpeg";
	else if ( (l >= 5) && !strcasecmp( &fname[l-5], ".mjpg" ) )
		mime_type = "image/jpeg";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".gif" ) )
		mime_type = "image/gif";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".png" ) )
		mime_type = "image/png";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".css" ) )
		mime_type = "text/css";
	else if ( (l >= 4) && !strcasecmp( &fname[l-3], ".js" ) )
		mime_type = "application/x-javascript";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".wav" ) )
		mime_type = "audio/x-wav";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".avi" ) )
		mime_type = "audio/avi";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".asf" ) )
		mime_type = "video/x-ms-asf";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".mkv" ) )
		mime_type = "video/x-matroska";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".flv" ) )
		mime_type = "video/x-flv";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".m4v" ) )
		mime_type = "video/mp4";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".mp4" ) )
		mime_type = "video/mp4";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".m4a" ) )
		mime_type = "audio/mp4";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".aac" ) )
		mime_type = "audio/mpeg";
	else if ( (l >= 6) && !strcasecmp( &fname[l-5], ".opus" ) )
		mime_type = "audio/ogg";
	else if ( (l >= 4) && !strcasecmp( &fname[l-3], ".ts" ) )
		mime_type = "video/MP2T";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".exe" ) )
		mime_type = "application/octet-stream";
	else if ( (l >= 6) && !strcasecmp( &fname[l-5], ".conf" ) )
		mime_type = "text/plain";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".txt" ) )
		mime_type = "application/octet-stream";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".svg" ) )
		mime_type = "image/svg+xml";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".swf" ) )
		mime_type = "application/x-shockwave-flash";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".pdf" ) )
		mime_type = "application/pdf";
	else if ( (l >= 5) && !strcasecmp( &fname[l-4], ".gpx" ) )
		mime_type = "application/gpx+xml";
	else
		DEBUG( "*** Unknown mime type! - %s", fname );

	// Create a response object
	int flen = filelen(fname);
	if ( flen == -1 ) {
		DEBUG( "HTTP file %s - %m", fname );
#if 0
		char *html = html_page_redirect( "File Upload Error", "Invalid file to upload - %s", fname );
		struct MHD_Response *response = MHD_create_response_from_buffer( strlen(html), (void *)html, MHD_RESPMEM_MUST_FREE );
		if ( !response )
			return MHD_NO;
		int ret = MHD_queue_response( connection, MHD_HTTP_BAD_REQUEST, response );
		MHD_destroy_response( response );
		return ret;
#endif
	}
	if ( flen == 0 )
		return send_page(connection, "", MHD_HTTP_OK);
	send_file_reader_t *t = (send_file_reader_t *)malloc( sizeof( send_file_reader_t ) );
	t->url = strdup( url );
	t->fname = strdup( fname );
	t->fd = open( fname, O_RDONLY | O_NOATIME );
//	t->httpd = httpd;
#if 0
	if ( t->fd == -1 ) {
		WARNING( "HTTP file %s - %m", fname );
		char *html = html_page_redirect( "File Upload Error", "Invalid file to upload - %s", fname );
		struct MHD_Response *response = MHD_create_response_from_buffer( strlen(html), (void *)html, MHD_RESPMEM_MUST_FREE );
		if ( !response )
			return MHD_NO;
		int ret = MHD_queue_response( connection, MHD_HTTP_BAD_REQUEST, response );
		MHD_destroy_response( response );
		return ret;
	}
#endif
    struct MHD_Response *response = MHD_create_response_from_callback(
    	flen,
    	32 * 1024,
    	send_file_reader_cb, (void *)t,
   		send_file_reader_done );

	MHD_add_response_header( response, MHD_HTTP_HEADER_CONTENT_TYPE, mime_type );
	int ret = MHD_queue_response( connection, MHD_HTTP_OK, response );
	MHD_destroy_response( response );
	return ret;
}								// LibMicroHttpdHandlers::send_file

/**
 * TODO
 */
int LibMicroHttpdHandlers::http_answer_to_connection( void *cls, struct MHD_Connection *connection,
	const char *url, const char *method, const char *version,
	const char *upload_data, size_t *upload_data_size, void **con_cls ) {

	LibMicroHttpdHandlers *handlers = (LibMicroHttpdHandlers *)cls;

    DEBUG("%s - %s - %s", method, url, version);

	if (!strcmp(method, "GET")) {

		bool is_a_file;
		void const *what = handlers->search_get_handler(url, &is_a_file);
		DEBUG("[%s]", what);
		if (what) {
			if (is_a_file)
				return handlers->send_file(connection, url, (const char *)what);
		}

	}

    return MHD_NO;
}	// LibMicroHttpdHandlers::Httpd::http_answer_to_connection

