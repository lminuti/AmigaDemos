#include <stdio.h>
#include "libraries/dosextens.h"
#include "proto/dos.h"

void Writef(BPTR fh, const char *fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    Write(fh, buffer, strlen(buffer));
}

int main()
{
    BPTR fh;
    
    fh = Open("*", MODE_OLDFILE);
    
    Writef(fh, "%ls", "Hello, Amiga!\n");
    
    Close(fh);
    
    return 0;
}
