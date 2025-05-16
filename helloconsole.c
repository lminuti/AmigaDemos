#include "libraries/dosextens.h"
#include "proto/dos.h"

int main()
{
    BPTR fh;
    
    fh = Open("CON:10/10/500/150/New Window", MODE_NEWFILE);
    
    Write(fh, "Hello, new Amiga console!\n", 27);
    Delay(300);
    Close(fh);
    
    return 0;
}
