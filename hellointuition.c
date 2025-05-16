#include <stdio.h>
#include "exec/types.h"
#include "intuition/intuition.h"
#include "libraries/dosextens.h"

#include "proto/dos.h"
#include "proto/intuition.h"
#include "proto/exec.h"
#include "proto/graphics.h"
#include "proto/diskfont.h"

#define WINDOW_NORMAL_FLAGS WINDOWSIZING | WINDOWCLOSE | WINDOWDRAG | WINDOWDEPTH 

BPTR console;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct Window *myWindow;
struct TextFont *font;

struct TextAttr MyFont =
{
    "topaz.font", /* Font Name */
    TOPAZ_SIXTY, /* Font Height */ 
    FS_NORMAL, /* Style */ 
    FPF_ROMFONT /* Preferences */
};

void write_message(const char *fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    Write(console, buffer, strlen(buffer));
}

struct NewWindow myNewWindow = {
    30, 30, // Top and Left
    320, 200, // Width and Height
    -1, // Detail Pen: penna usata per il bordo
    -1, // Block Pen: penna usata per disegnare i gadget
    CLOSEWINDOW | NEWSIZE | REFRESHWINDOW, // Flag IDCMP (eventi)
    SIMPLE_REFRESH | WINDOW_NORMAL_FLAGS | ACTIVATE | GIMMEZEROZERO, // Flags
    //SMART_REFRESH | WINDOW_NORMAL_FLAGS | ACTIVATE | GIMMEZEROZERO | NOCAREREFRESH, // Flags
    NULL, // First Gadget
    NULL, // Check Mark
    "Hello, Amiga!", // Window Title
    NULL, // Pointer to the screen
    NULL, // Puntatore alla bitmap se si usa una SuperBitmap
    100, 100, // Min Width and Height
    620, 200, // Max Width and Height
    WBENCHSCREEN // Tipo di schermo
};

void draw_window(struct Window *window)
{
    struct RastPort *rp = window->RPort;
    SetFont(myWindow->RPort, font);
    Move(myWindow->RPort, 20, 20); 
    Text(myWindow->RPort, "Hello World", 11);    
}

LONG handle_message(struct IntuiMessage *msg)
{
    LONG msgClass = msg->Class;
    write_message("Message class: %ld\n", msgClass);
    switch (msgClass) {
        case CLOSEWINDOW:
            return 0;
        case NEWSIZE:
            return 1;
        case REFRESHWINDOW:
            BeginRefresh(myWindow);   /* Gestione corretta del refresh */
            draw_window(myWindow); /* Ridisegna la finestra */
            EndRefresh(myWindow, TRUE);
            return 1;
        default:
            return 1;
    }
}
    
int main(void)
{
    LONG result = 1;

    console = Open("*", MODE_OLDFILE);

    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
    if (IntuitionBase == NULL) {
        printf("Failed to open Intuition library\n");
        Close(console);
        return 1;
    }

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (GfxBase == NULL) {
        printf("Failed to open graphics library\n");
        CloseLibrary((struct Library *)IntuitionBase);
        Close(console);
        return 1;
    }

    MyFont.ta_Name = "emerald.font";
    MyFont.ta_YSize = 20;
    //MyFont.ta_YSize = TOPAZ_SIXTY;
    //MyFont.ta_YSize = TOPAZ_EIGHTY;
    font = OpenDiskFont(&MyFont);
    if (font == NULL) {
        printf("Failed to open font\n");
        CloseLibrary((struct Library *)GfxBase);
        CloseLibrary((struct Library *)IntuitionBase);
        Close(console);
        return 1;
    }

    struct IntuiMessage *msg;
        
    write_message("Hello, Amiga!\n");
    
    myWindow = OpenWindow(&myNewWindow);
    if (myWindow == NULL) {
        printf("Failed to open windows\n");
        CloseLibrary((struct Library *)IntuitionBase);
        Close(console);
        return 1;
    }

    draw_window(myWindow);
    
    while (result) {
        WaitPort(myWindow->UserPort);
        
        while ((msg = (struct IntuiMessage *)GetMsg(myWindow->UserPort)) != NULL) {
            result = handle_message(msg);
            ReplyMsg((struct Message *)msg);  /* Importante: Rilascia il messaggio */
            
            if (result == 0) {
                break;
            }
        }
    }
    
    CloseWindow(myWindow);
    CloseLibrary((struct Library *)IntuitionBase);
    CloseLibrary((struct Library *)GfxBase);
    CloseFont(font);
    Close(console);
    
    return 0;
}