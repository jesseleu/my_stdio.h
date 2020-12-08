#ifndef _MY_STDIO_H_
#define _MY_STDIO_H_
#define BUFSIZ 8192 // default buffer size
#define _IONBF 0    // unbuffered
#define _IOLBF 1    // line buffered
#define _IOFBF 2    // fully buffered
#define EOF -1      // end of file
#include<stddef.h>
class FILE 
{
 public:
  FILE() 
  {
     fd = 0;
     pos = 0;
     buffer = (char *) 0;
     size = 0;
     actual_size = 0;
     mode = _IONBF;
     flag = 0;
     bufown = false;
     lastop = 0;
     eof = false;
  }


  int fd;          // a Unix file descriptor of an opened file
  int pos;         // the current file position in the buffer
  char *buffer;    // an input or output file stream buffer
  int size;        // the buffer size
  int actual_size; // the actual buffer size when read( ) returns # bytes read smaller than size
  int mode;        // _IONBF, _IOLBF, _IOFBF
  int flag;        // O_RDONLY 
                   // O_RDWR 
                   // O_WRONLY | O_CREAT | O_TRUNC
                   // O_WRONLY | O_CREAT | O_APPEND
                   // O_RDWR   | O_CREAT | O_TRUNC
                   // O_RDWR   | O_CREAT | O_APPEND
  bool bufown;     // true if allocated by stdio.h or false by a user
  char lastop;     // 'r' or 'w' 
  bool eof;        // true if EOF is reached
};


int recursive_itoa(int arg);
char *itoa(const int arg);
int printf(const void *format,...);
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);
FILE *fopen(const char *path, const char *mode);
int fpurge(FILE *stream);
int fflush(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
char *fgets(char *str, int size, FILE *stream);
int fputs(const char *str, FILE *stream);
int feof(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int fclose(FILE *stream);
#include "stdio.cpp"
#endif
