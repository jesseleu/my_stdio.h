#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include<stddef.h>
#include "stdio.h"
using namespace std;
char decimal[100];

int recursive_itoa(int arg)
{
    int div = arg / 10;
    int mod = arg % 10;
    int index = 0;
    if (div > 0)
    {
        index = recursive_itoa(div);
    }
    decimal[index] = mod + '0';
    return ++index;
}

char *itoa(const int arg)
{
    bzero(decimal, 100);
    int order = recursive_itoa(arg);
    char *new_decimal = new char[order + 1];
    bcopy(decimal, new_decimal, order + 1);
    return new_decimal;
}

int printf(const void *format, ...)
{
    va_list list;
    va_start(list, format);
    
    char *msg = (char *)format;
    char buf[1024];
    int nWritten = 0;
    
    int i = 0, j = 0, k = 0;
    while (msg[i] != '\0')
    {
        if (msg[i] == '%' && msg[i + 1] == 'd')
        {
            buf[j] = '\0';
            nWritten += write(1, buf, j);
            j = 0;
            i += 2;
            
            int int_val = va_arg(list, int);
            char *dec = itoa(abs(int_val));
            if (int_val < 0)
            {
                nWritten += write(1, "-", 1);
            }
            nWritten += write(1, dec, strlen(dec));
            delete dec;
        }
        else
        {
            buf[j++] = msg[i++];
        }
    }
    if (j > 0)
    {
        nWritten += write(1, buf, j);
    }
    va_end(list);
    return nWritten;
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
    if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF)
    {
        return -1;
    }
    stream->mode = mode;
    stream->pos = 0;
    if (stream->buffer != (char *)0 && stream->bufown == true)
    {
        delete stream->buffer;
    }
    
    switch (mode)
    {
        case _IONBF:
            stream->buffer = (char *)0;
            stream->size = 0;
            stream->bufown = false;
            break;
        case _IOLBF:
        case _IOFBF:
            if (buf != (char *)0)
            {
                stream->buffer = buf;
                stream->size   = (int) size;
                stream->bufown = false;
            }
            else
            {
                stream->buffer = new char[BUFSIZ];
                stream->size = BUFSIZ;
                stream->bufown = true;
            }
            break;
    }
    return 0;
}

void setbuf(FILE *stream, char *buf)
{
    setvbuf(stream, buf, (buf != (char *)0) ? _IOFBF : _IONBF , BUFSIZ);
}

FILE *fopen(const char *path, const char *mode)
{
    FILE *stream = new FILE();
    setvbuf(stream, (char *)0, _IOFBF, BUFSIZ);
    
    // fopen( ) mode
    // r or rb = O_RDONLY
    // w or wb = O_WRONLY | O_CREAT | O_TRUNC
    // a or ab = O_WRONLY | O_CREAT | O_APPEND
    // r+ or rb+ or r+b = O_RDWR
    // w+ or wb+ or w+b = O_RDWR | O_CREAT | O_TRUNC
    // a+ or ab+ or a+b = O_RDWR | O_CREAT | O_APPEND
    
    switch(mode[0])
    {
        case 'r':
            if (mode[1] == '\0')            // r
            {
                stream->flag = O_RDONLY;
            }
            else if (mode[1] == 'b')
            {
                if (mode[2] == '\0')          // rb
                {
                    stream->flag = O_RDONLY;
                }
                else if (mode[2] == '+')      // rb+
                {
                    stream->flag = O_RDWR;
                }
            }
            else if (mode[1] == '+')        // r+  r+b
            {
                stream->flag = O_RDWR;
            }
            break;
        case 'w':
            if (mode[1] == '\0')            // w
            {
                stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
            }
            else if (mode[1] == 'b')
            {
                if (mode[2] == '\0')          // wb
                {
                    stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
                }
                else if (mode[2] == '+')      // wb+
                {
                    stream->flag = O_RDWR | O_CREAT | O_TRUNC;
                }
            }
            else if (mode[1] == '+')        // w+  w+b
            {
                stream->flag = O_RDWR | O_CREAT | O_TRUNC;
            }
            break;
        case 'a':
            if (mode[1] == '\0')            // a
            {
                stream->flag = O_WRONLY | O_CREAT | O_APPEND;
            }
            else if (mode[1] == 'b')
            {
                if (mode[2] == '\0')          // ab
                {
                    stream->flag = O_WRONLY | O_CREAT | O_APPEND;
                }
                else if (mode[2] == '+')      // ab+
                {
                    stream->flag = O_RDWR | O_CREAT | O_APPEND;
                }
            }
            else if (mode[1] == '+')        // a+  a+b
            {
                stream->flag = O_RDWR | O_CREAT | O_APPEND;
            }
            break;
    }
    
    mode_t open_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    
    if ((stream->fd = open(path, stream->flag, open_mode)) == -1)
    {
        delete stream;
        printf("fopen failed\n");
        stream = NULL;
    }
    
    return stream;
}

int fpurge(FILE *stream)
{
    for(int i = 0; i < BUFSIZ; i++)
    {
        stream->buffer[i] = '\0';
    }
    return 0;
}

int fflush(FILE *stream)
{
    if(stream->buffer[0] != 0 && stream->lastop == 'w')
    {
        fwrite(stream->buffer, stream->size, 1, stream);
    }
    fpurge(stream);
    
    return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    char* p = (char*) ptr;
    stream->lastop = 'r';
    size_t readSize = size * nmemb;
    size_t bytesread = 0;
    
    if(!stream->bufown)
    {
        stream->buffer = new char[BUFSIZ];
        stream->bufown = true;
        fpurge(stream);
    }
    if(stream->eof)
    {
        return 0;
    }
    if(stream->pos == stream->size  || (stream->buffer[0] == 0))
    {
        fpurge(stream);
        bytesread = read(stream->fd, stream->buffer, BUFSIZ);
        stream->buffer[bytesread] = 0;
        if(bytesread > 0)
        {
            stream->size = (int)bytesread;
            stream->pos = 0;
            
        }
        else
        {
            stream->eof = true;
            fpurge(stream);
            return 0;
        }
    }
    if(stream->size - stream->pos < readSize)
    {
        int actualRead = stream->size - stream->pos;
        memcpy(p, &stream->buffer[stream->pos], actualRead + 1);
        p[actualRead] = 0;
        stream->pos += actualRead  ;
        return actualRead;
    }
    else
    {
        memcpy(p, &stream->buffer[stream->pos], readSize + 1);
        p[readSize + 1] = 0;
        stream->pos += readSize ;
        return readSize;
    }
    
    
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if( ! stream->bufown)
    {
        stream->buffer = new char[BUFSIZ];
        stream->bufown = true;
    }
    
    const  char *p = (const char *) ptr;
    size_t writeSize = size * nmemb;
    int totalWrite = 0 ;
    stream->pos = 0;
    fpurge(stream);
    while(BUFSIZ + 1 <  writeSize)
    {
        fpurge(stream);
        stream->pos = 0;
        memcpy(stream->buffer, (void*)p, BUFSIZ + 1);
        totalWrite += BUFSIZ;
        p+= BUFSIZ + 1;
        
        stream->buffer[BUFSIZ + 1] = 0;
        write(stream->fd, stream->buffer, BUFSIZ + 1);
        writeSize -= BUFSIZ + 1;
        
        
    }
    
    fpurge(stream);
    stream->pos = 0;
    memcpy(stream->buffer, (void*)p, writeSize);
    stream->buffer[BUFSIZ  + 1] = 0;
    stream->pos += BUFSIZ;
    
    p += writeSize + 1;
    write(stream->fd, stream->buffer, writeSize);
    fpurge(stream);
    stream->pos = 0;
    
    return writeSize;
    
}

int fgetc(FILE *stream)
{
    if(stream->eof)
    {
        return - 1;
    }
    size_t bytesread = 0;
    char res = EOF;
    stream->lastop = 'r';
    if(!stream->bufown)
    {
        stream->buffer = new char[BUFSIZ];
        stream->bufown = true;
        fpurge(stream);
    }
    if(stream->pos == stream->size || (stream->buffer[0] == 0))//fill the buffer
    {
        fpurge(stream);
        stream->pos = 0;
        bytesread = read(stream->fd, stream->buffer, BUFSIZ);
        stream->buffer[bytesread] = 0;
        if(bytesread > 0)
        {
            stream->size = (int)bytesread;
            stream->pos = 0;
            
        }
        else
        {
            stream->eof = true;
            fpurge(stream);
            return EOF;
        }
    }
    res = stream->buffer[stream->pos++];
    return res;
}

int fputc(int c, FILE *stream)
{
    stream->lastop = 'w';
    if( ! stream->bufown)
    {
        stream->buffer = new char[BUFSIZ];
        stream->bufown = true;
    }
    if(BUFSIZ - stream->pos <  1)
    {
        write(stream->fd, stream->buffer, BUFSIZ);
        fpurge(stream);
        stream->buffer[0] = (char) c;
        stream->pos = 1;
        return c;
    }
    else
    {
        stream->buffer[stream->pos] = (char)c;
        stream->pos += 1;
        stream->buffer[stream->pos] = 0;
        if( stream->pos == stream->size)
        {
            write(stream->fd, stream->buffer, BUFSIZ);
            stream->pos = 0;
            fpurge(stream);
        }
        return c;
    }
}

char *fgets(char *str, int size, FILE *stream)
{
    int c =  0;
    int index = 0;
    stream->lastop = 'r';
    if (size <= 0)
    {
        return str;
    }
    while(index < size - 1 && (c = fgetc(stream)) != EOF)
    {
        str[index++] = c;
        if (c == '\n')
        {
            break;
        }
    }
    str[index] = 0;
    
    if (index > 0)
    {
        return str;
    }
    else
    {
        return NULL;
    }
}
int fputs(const char *str, FILE *stream)
{
    int index = 0;
    stream->lastop = 'w';
    while(str[index] != 0)
    {
        fputc(str[index], stream);
        index++;
    }
    return 0;
}

int feof(FILE *stream)
{
    return stream->eof == true;
}

int fseek(FILE *stream, long offset, int whence)
{
    fflush(stream);
    long newPos = lseek(stream->fd, offset, whence);
    stream->pos = 0;
    int res = (newPos <  0);
    if(res == 0)
    {
        stream->eof = false ;
    }
    return res;
}

int fclose(FILE *stream)
{
    if (stream == NULL)
    {
        return -1;
    }
    
    fflush(stream);
    stream->pos = 0;
    stream->eof = false;
    int res = close(stream->fd);
    if(stream->bufown && stream->buffer == 0)
    {
        delete stream->buffer;
    }
    delete stream;
    return res;
    
}

