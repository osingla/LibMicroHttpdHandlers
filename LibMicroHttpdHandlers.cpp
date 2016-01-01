
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "Logg.h"

#include "LibMicroHttpdHandlers.h"


/**
 * TODO
 */
LibMicroHttpdHandlers::LibMicroHttpdHandlers() {

    // Current number of pairs {url,what}
    nb_pairs = 0;
    pairs = NULL;

}   // LibMicroHttpdHandlers:LibMicroHttpdHandlers

/**
 * TODO
 */
LibMicroHttpdHandlers::~LibMicroHttpdHandlers() {
}   // LibMicroHttpdHandlers:~LibMicroHttpdHandlers

/**
 * TODO
 */
int LibMicroHttpdHandlers::qsort_cmp_pair(const void *p1, const void *p2) {
	pair_t *q1 = (pair_t *)p1;
	pair_t *q2 = (pair_t *)p2;

#if 0
	DEBUG("p1=%p p2=%p", p1, p2);

	switch (q1->type) {
		case TYPE_FILE :
			DEBUG("%-24s -> %s", q1->url, (const char *)q1->what);
			break;
		case TYPE_GET :
			DEBUG("%-24s -> %s", q1->url, (q1->fnct_name) ? q1->fnct_name : "");
			break;
	}

	switch (q2->type) {
		case TYPE_FILE :
			DEBUG("%-24s -> %s", q2->url, (const char *)q2->what);
			break;
		case TYPE_GET :
			DEBUG("%-24s -> %s", q2->url, (q2->fnct_name) ? q2->fnct_name : "");
			break;
	}
#endif
	return strcmp(q1->url, q2->url);
}	// LibMicroHttpdHandlers::qsort_cmp_pair

/**
 * TODO
 */
int LibMicroHttpdHandlers::set_file_handler(const char *url, const void *what) {

	// Add this new entry into the table
	if (!pairs)
		pairs = (pair_t *)malloc(sizeof(pair_t));
	else
		pairs = (pair_t *)realloc(pairs, sizeof(pair_t) * (nb_pairs+1));
	pair_t *p = &pairs[nb_pairs++];
	p->type = TYPE_FILE;
	p->url = strdup(url);
	p->what = (const void *)strdup((const char *)what);
	p->fnct_name = NULL;

	// Sort the handlers
	qsort(pairs, nb_pairs, sizeof(pair_t), qsort_cmp_pair);

    return 0;
}   // LibMicroHttpdHandlers:set_file_handler

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
	p->what = (const void *)pf;
	p->fnct_name = (fnct_name) ? strdup(fnct_name) : NULL;

	// Sort the handlers
	qsort(pairs, nb_pairs, sizeof(pair_t), qsort_cmp_pair);

    return 0;
}   // LibMicroHttpdHandlers:set_get_handler

/**
 * TODO
 */
const void *LibMicroHttpdHandlers::search_url(const char *url) {

	pair_t *p = (pair_t *)bsearch(url, pairs, nb_pairs, sizeof(pair_t), qsort_cmp_pair);
	if (!p)
		return NULL;

    return NULL;
}   // LibMicroHttpdHandlers:search_url

/**
 * TODO
 */
void LibMicroHttpdHandlers::dump_handlers( ) {

	DEBUG("%d handlers", nb_pairs);
	for (int n = 0; n < nb_pairs; n++ ) {
		pair_t *p = &pairs[n];
		switch (p->type) {
			case TYPE_FILE :
				DEBUG("%4d: %-24s -> %s", n, p->url, (const char *)p->what);
				break;
			case TYPE_GET :
				DEBUG("%4d: %-24s -> %s", n, p->url, (p->fnct_name) ? p->fnct_name : "");
				break;
		}
	}

}   // LibMicroHttpdHandlers:search_url
