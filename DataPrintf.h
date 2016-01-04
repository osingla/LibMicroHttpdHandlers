#ifndef __DATA_PRINTF_H_
#define __DATA_PRINTF_H_

typedef struct DataPrintf {

public:

	DataPrintf( );							///< Constructor
	~DataPrintf( );							///< Destructor

	const int alloc_size = 131072;			///< Chunk size allocated

	void printf(
		char const *fmt, ... );				///< Add new html string

	void printf(
		char const *fmt,
		va_list ap );						///< Add new html string

	void add(char const *buffer);			///< Add a new string

	inline int get_strlen() {
		return data_len; }					///< TODO

	inline char const *get_data() {
		return (char const *)data; }		///< TODO

	int send(
		struct MHD_Connection *connection,
		char const *mime_type);				///< TODO

private:

	char *temp_str;							///< Temporary buffer
	char *data;								///< data to display
	int data_len;							///< Current length (strlen)
	int data_len_alloctd;					///< Length allocated (malloc)

} DataPrintf;

#endif /* __DATA_PRINTF_H_ */
