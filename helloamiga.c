#include "libraries/dosextens.h"
#include "proto/dos.h"

int main()
{
    BPTR fh;
    
    fh = Open("*", MODE_OLDFILE);
    
    Write(fh, "Hello, Amiga!\n", 15);
    
    Close(fh);
    
    return 0;
}
