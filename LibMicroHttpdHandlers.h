

typedef struct LibMicroHttpdHandlers {

public:

	typedef enum  {
		TYPE_GET = 1,				///< GET handler
		TYPE_PUT,					///< PUT handler
		TYPE_POST,					///< PUT handler
	} type_t;								///< TODO

	typedef int (*pf_t)(
		void *cls,
		struct MHD_Connection *connection,
		char const *url);					///< TODO

	LibMicroHttpdHandlers();                ///< Constructor
    ~LibMicroHttpdHandlers();               ///< Destructor

    int set_file_get_handler(
        const char *url,
        const void *what);                 	///< Add a new handler

#define SET_GET_HANDLER(url, pf) set_get_handler(url, pf, #pf)
    int set_get_handler(
        const char *url,
		pf_t pf,
		char const *fnct_name = NULL); 		///< Add a new handler

    void dump_handlers();					///< TODO

    void const *search_get_handler(
        const char *url,
		bool *is_a_file);             		///< Search for a given URL

    void const *search_put_handler(
        const char *url);             		///< Search for a given URL

    void const *search_post_handler(
        const char *url);             		///< Search for a given URL

    int send_file(
    	struct MHD_Connection *connection,
		const char *url,
		const char *fname);					///< Send a file

private:

    typedef struct {
        type_t type;						///< TYPE_GET | TYPE_PUT | TYPE_POST
        const char *url;					///< URL
        bool is_a_file;						///< If true, what is a file - if false, what is a function
        const void *what;                   ///< Add a new handler (file or function)
        const char *fnct_name;				///< TODO
    } pair_t;

    int nb_pairs;                           ///< Current number of pairs {url,what}
    pair_t *pairs;                          ///< TODo

    static int qsort_cmp_pair(
    	const void *p1,
		const void *p2);					///< TODO

    void const *search_handler(
        const char *url,
		type_t type,
		bool *is_a_file);             		///< Search for a given URL

    static char const *add_prefix(
    	type_t type);						///< return "GET:", "PUT:" or "POST:"

    static int filelen(
    	char const *filename);				///< Return file length

	typedef struct {
		char		*url;					///< TODO
		char		*fname;					///< TODO
		int			fd;						///< TODO
	} send_file_reader_t;

	static ssize_t send_file_reader_cb (
		void *cls,
		uint64_t pos,
		char *buf,
		size_t max );									///< TODO

	static void send_file_reader_done (
		void *cls );									///< TODO

	static int send_page(
		struct MHD_Connection *connection,
		const char *page, int status_code );		///< TODO

} LibMicroHttpdHandlers;
