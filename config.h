/* See LICENSE file for copyright and license details. */

/* appearance */
static const char *fonts[] = {
	"monospace:size=10"
};
static const char dmenufont[]       = "monospace:size=10";
static const char normbordercolor[] = "#000000";
static const char normbgcolor[]     = "#222222";
static const char normfgcolor[]     = "#bbbbbb";
static const char selbordercolor[]  = "#ffff44";
static const char selbgcolor[]      = "#005577";
static const char selfgcolor[]      = "#eeeeee";
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   prefer_wide  monitor */
        { "Gimp",     NULL,       NULL,       0,            1,           0,           -1 },
        // Any substring will match:
        { "irefox",   NULL,       NULL,       0,            0,           1,           -1 },
        { "hrome",    NULL,       NULL,       0,            0,           1,           -1 },
        { "Tkdiff",   NULL,       NULL,       0,            0,           1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#include "wide_layouts.h"   // contains tcl, wide_tile
static const Layout layouts[] = {
    // Note, the first entry is the default:

    /* symbol     arrange function */
    { "+|+",      wide_tile },  /* four column 4K layout */
    { "[]=",      tile },    /* original recipe two-column tiling */
    { "><>",      NULL },    /* no layout function means floating behavior */
    { "|||",      tcl },     /* three column layout */
    { "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask

// 'view' selects a new tag to view
// 'tag' sets the tag for the active window
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "/bin/sh", "-c", "~/scripts/xterm", NULL };
// Get the work laptop display back on:
static const char *xrandr_auto_cmd[] = { "/usr/bin/xrandr", "--output", "eDP-1", "--auto", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
    //	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
  // JPC added -- recover laptop display when it keels:
        { MODKEY,                       XK_q,      spawn,          { .v = xrandr_auto_cmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_e,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_n,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_comma,  incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_period, incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_l,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_u,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_i,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_minus,  killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_p,      setlayout,      {.v = &layouts[3]} },
        { MODKEY,                       XK_w,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	// Views ALL tags simultaneously:
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	// Tags the active window to all tags:
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	// JPC what is 'focusmon'? Again, this is probably multi-monitor support.
	//	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	//	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	// JPC what is 'tagmon'?
	// From the code -- it's literally changing to a different monitor.
	//  { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	//  { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

        // JPC added:
	{ MODKEY,                       XK_Up,     viewrot, {.i = -1} },
	{ MODKEY,                       XK_Left,   viewrot, {.i = -1} },
	{ MODKEY,                       XK_Down,   viewrot, {.i = 1} },
	{ MODKEY,                       XK_Right,  viewrot, {.i = 1} },
        { MODKEY,                       XK_End,    toggle_prefer_wide, {0}},

	{ MODKEY|ShiftMask,             XK_Up,     tagrot,  {.i = -1} },
	{ MODKEY|ShiftMask,             XK_Left,   tagrot,  {.i = -1} },
	{ MODKEY|ShiftMask,             XK_Down,   tagrot,  {.i = 1} },
	{ MODKEY|ShiftMask,             XK_Right,  tagrot,  {.i = 1} },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

