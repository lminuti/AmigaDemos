#ifdef DEBUG 
#include <stdio.h>
#endif

#include "exec/types.h"
#include "intuition/intuition.h"
#include "libraries/dosextens.h"
#include "graphics/gfxmacros.h"

#include "proto/dos.h"
#include "proto/intuition.h"
#include "proto/exec.h"
#include "proto/graphics.h"
#include "proto/diskfont.h"

#define BALL_COUNT 3

BPTR console;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct Window *myWindow;
struct TextFont *font;

int line_distance = 15;

struct TextAttr MyFont =
{
    "topaz.font", /* Font Name */
    TOPAZ_SIXTY, /* Font Height */ 
    FS_NORMAL, /* Style */ 
    FPF_ROMFONT /* Preferences */
};

struct Point {
    int x, y;
    int dx, dy;
    int rx, ry;
    int color;
};

struct Line {
    struct Point p1;
    struct Point p2;
};

struct Point points[BALL_COUNT] = {
    { 25, 30, 5, 4, 20, 10, 1 },
    { 250, 50, 3, 2, 20, 10, 2 },
    { 100, 150, -4, -3, 20, 10, 3 }
};

struct Line line = { { 35, 20, 6, 4, 1, 1 }, { 120, 30, 8, 6, 1, 1 } };
struct Line eraserLine = { { 35, 20, 6, 4, 1, 1 }, { 120, 30, 8, 6, 1, 1 } };

#ifndef DEBUG
void printf(const char *fmt, ...) {}
#endif

struct NewWindow myNewWindow = {
    30, 30, // Top and Left
    600, 200, // Width and Height
    -1, // Detail Pen: penna usata per il bordo
    -1, // Block Pen: penna usata per disegnare i gadget
    CLOSEWINDOW | INTUITICKS, // Flag IDCMP (eventi)
    SMART_REFRESH | WINDOWCLOSE | WINDOWDRAG | WINDOWDEPTH | ACTIVATE | GIMMEZEROZERO, // Flags
    //SMART_REFRESH | WINDOW_NORMAL_FLAGS | ACTIVATE | GIMMEZEROZERO | NOCAREREFRESH, // Flags
    NULL, // First Gadget
    NULL, // Check Mark
    "Bounce", // Window Title
    NULL, // Pointer to the screen
    NULL, // Puntatore alla bitmap se si usa una SuperBitmap
    100, 100, // Min Width and Height
    620, 200, // Max Width and Height
    WBENCHSCREEN // Tipo di schermo
};

void draw_point(struct Window *window, struct Point *point)
{
    struct RastPort *rp = window->RPort;
    SetDrMd(rp, JAM1);
    SetAPen(rp, point->color);
    DrawEllipse(rp, point->x, point->y, point->rx, point->ry);
}

void erase_point(struct Window *window, struct Point *point)
{
    struct RastPort *rp = window->RPort;
    SetDrMd(rp, JAM1);
    SetAPen(rp, 0);
    DrawEllipse(rp, point->x, point->y, point->rx, point->ry);
}

void draw_line(struct Window *window, struct Line *line)
{
    struct RastPort *rp = window->RPort;
    SetDrMd(rp, COMPLEMENT);
    Move(rp, line->p1.x, line->p1.y);
    Draw(rp, line->p2.x, line->p2.y);
}

void draw_window(struct Window *window)
{
    // struct RastPort *rp = window->RPort;
    // SetFont(myWindow->RPort, font);
    // Move(myWindow->RPort, 20, 20); 
    // Text(myWindow->RPort, "Hello World", 11);    
}

void move_point(struct Point *point)
{
    point->x += point->dx;
    point->y += point->dy;

    // Check for collision with window borders
    if (point->x - point->rx < myWindow->BorderLeft || point->x + point->rx > myWindow->Width - myWindow->BorderLeft - myWindow->BorderRight) {
        point->dx = -point->dx; // Reverse direction on X axis
    }
    if (point->y - point->ry < 0 || point->y + point->ry > myWindow->Height - myWindow->BorderTop - myWindow->BorderBottom) {
        point->dy = -point->dy; // Reverse direction on X axis
    }
}

void move_points(struct Point *points, int count)
{
    for (int i = 0; i < count; i++) {
        erase_point(myWindow, &points[i]);
        move_point(&points[i]);
        draw_point(myWindow, &points[i]);
    }
}

void handle_ticks(struct Window *window)
{
    //move_points(points, BALL_COUNT);

    if (line_distance <= 0) {
        draw_line(window, &eraserLine);
        move_point(&eraserLine.p1);
        move_point(&eraserLine.p2);
    } else {
        line_distance--;
    }

    move_point(&line.p1);
    move_point(&line.p2);
    draw_line(window, &line);
}

LONG handle_message(struct IntuiMessage *msg)
{
    LONG msgClass = msg->Class;
    //write_message("Message class: %ld\n", msgClass);
    switch (msgClass) {
        case CLOSEWINDOW:
            return 0;
        case NEWSIZE:
            return 1;
        case INTUITICKS:
            handle_ticks(myWindow);
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
        //printf("Failed to open Intuition library\n");
        Close(console);
        return 1;
    }

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (GfxBase == NULL) {
        //printf("Failed to open graphics library\n");
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
        
    //write_message("Hello, Amiga!\n");
    
    myWindow = OpenWindow(&myNewWindow);
    if (myWindow == NULL) {
        printf("Failed to open windows\n");
        CloseLibrary((struct Library *)IntuitionBase);
        Close(console);
        return 1;
    }

    draw_window(myWindow);
    //move_points(points, BALL_COUNT);
    draw_line(myWindow, &line);
    
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