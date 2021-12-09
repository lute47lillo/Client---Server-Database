#if !defined(_UTIL_H_)
#define _UTIL_H_

size_t read_length(int fd);

void write_length(int fd, size_t length);

/*
 * Read exactly 'count' bytes into the buffer.
 */
void readn(int fd, void * buffer, size_t count);

/*
 * Write exactly 'count' bytes from the buffer.
 */
void writen(int fd, void * buffer, size_t count);

void error(char *msg);

#endif /* _UTIL_H_ */
