
/* **
 * Return the base  of a filename (last part of the filename)
 *
 * @param[in] filename Filename (as returned by __FILE__)
 * @return base of the filename
 */
static inline const char * __file__( const char *filename ) {
	char const *p = strrchr( filename, '/' );
	if ( p )
		return p+1;
	else
		return filename;
}   // __file__

/* **
 * Send message to the Linux system logger
 *
 * @param priority LOG_DEBUG, LOG_ERR, etc.
 * @param[in] fmt sprintf like format
 * @param ... Additional optional arguments
 * @return Always -1 (means usually error condition)
 */
static inline int SYSLOG( int priority, const char *fmt, ... ) {
	va_list ap;
	va_start( ap, fmt );
	vsyslog( priority, fmt, ap );
	va_end( ap );
	return -1;
}   // SYSLOG

#define DEBUG(fmt, ...) \
	SYSLOG( LOG_INFO, "%22s %4d: " fmt, __file__(__FILE__), __LINE__, ## __VA_ARGS__ )

