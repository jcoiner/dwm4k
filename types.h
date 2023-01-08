#ifndef TYPES_H_
#define TYPES_H_

#include <X11/Xlib.h>



typedef struct Monitor Monitor;
typedef struct Client Client;

struct Client {
    char name[256];
    float mina, maxa;
    int x, y, w, h;
    int oldx, oldy, oldw, oldh;
    int basew, baseh, incw, inch, maxw, maxh, minw, minh;
    int bw, oldbw;
    unsigned int tags;
    int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen, prefer_wide;
    Client *next;
    // "stack next", continues list begun by Monitor.stack
    Client *snext;
    Monitor *mon;
    Window win;
};

typedef struct {
    const char *symbol;
    void (*arrange)(Monitor *);
} Layout;

struct Monitor {
	char ltsymbol[16];
	float mfact;
	int nmaster;
	int num;
	int by;               /* bar geometry */
	int mx, my, mw, mh;   /* screen size */
	int wx, wy, ww, wh;   /* window area  */
        unsigned int seltags; // JPC: always 0 or 1. Indexes tagset[] below.
	unsigned int sellt;
	unsigned int tagset[2];
	int showbar;
	int topbar;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
	Window barwin;
	const Layout *lt[2];
};

#endif
