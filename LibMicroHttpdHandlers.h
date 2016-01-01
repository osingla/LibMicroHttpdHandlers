

typedef struct LibMicroHttpdHandlers {

public:

	typedef enum  {
		TYPE_FILE = 1,			///< File handler
		TYPE_GET,				///< GET handler
	} type_t;								///< TODO

	typedef int (*pf_t)(
		void *cls,
		struct MHD_Connection *connection,
		char const *url);					///< TODO

	LibMicroHttpdHandlers();                ///< Constructor
    ~LibMicroHttpdHandlers();               ///< Destructor

    int set_file_handler(
        const char *url,
        const void *what);                 	///< Add a new handler

#define SET_GET_HANDLER(url, pf) set_get_handler(url, pf, #pf)
    int set_get_handler(
        const char *url,
		pf_t pf,
		char const *fnct_name = NULL); 		///< Add a new handler

    void dump_handlers();					///< TODO

private:

    typedef struct {
        type_t type;						///< TYPE_FILE
        const char *url;					///< TODO
        const void *what;                   ///< Add a new handler (file or function)
        const char *fnct_name;				///< TODO
    } pair_t;

    int nb_pairs;                           ///< Current number of pairs {url,what}
    pair_t *pairs;                          ///< TODo

    static int qsort_cmp_pair(
    	const void *p1,
		const void *p2);					///< TODO

    void const *search_url(
        const char *url);                  	///< Search for a given URL

} LibMicroHttpdHandlers;
