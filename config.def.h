/* See LICENSE file for copyright and license details. */

#include "dracula-colors.h"
#include "movestack.c"
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 3; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1;            /* 0 means bottom bar */
static const char *fonts[] = {"Hack:pixelsize=12:antialias=true:autohint=true"};
static const char dmenufont[] =
    "Hack:pixelsize=12:antialias=true:autohint=true";
static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_cyan[] = "#005577";
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {dracul_foreground, dracul_background, dracul_purple},
    [SchemeSel] = {dracul_foreground, dracul_selection, dracul_green},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     iscentered   isfloating
       monitor */
    {"Gimp", NULL, NULL, 0, 0, 1, -1},
    {"Firefox", NULL, NULL, 1, 0, 0, -1},
    {"Mixer", NULL, NULL, 0, 1, 1, -1},
    {NULL, NULL, "mpvfloat", 0, 0, 1, -1},
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[]=", tile}, /* first entry is default */
    {"><>", NULL}, /* no layout function means floating behavior */
    {"[M]", monocle},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}}, {                      \
    MODKEY | ControlMask | ShiftMask, KEY, toggletag, { .ui = 1 << TAG }       \
  }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "dmenu_run", "-m",      dmenumon, "-fn",    dmenufont, "-nb",     col_gray1,
    "-nf",       col_gray3, "-sb",    col_cyan, "-sf",     col_gray4, NULL};
static const char *termcmd[] = {"st", NULL};
static const char *rofiruncmd[] = {
    "rofi",        "-theme",   "gruvbox-dark", "-show", "combi",
    "-combi-modi", "run,drun", "-modi",        "combi", NULL};
static const char *session_menu_cmd[] = {"session-menu", NULL};
static const char *pulsemixer_cmd[] = {"st", "-c",         "Mixer",
                                       "-e", "pulsemixer", NULL};
static const char *rofi_surfraw_cmd[] = {"rofi-surfraw", NULL};
static const char *ec_cmd[] = {"ec", NULL};
static const char *org_capture_cmd[] = {"org-capture", NULL};
static const char *keyboard_layout_cmd[] = {"keyboard-layout", NULL};
static const char *displayselect_cmd[] = {"displayselect", NULL};
static const char *ranger_cmd[] = {"st", "-e", "ranger", NULL};
static const char *capture_upload_cmd[] = {"capture_upload", NULL};
static const char *capture_cmd[] = {"capture", NULL};
static const char *pamixer_inc_fast_cmd[] = {"pamixer", "--allow-boost", "-i",
                                             "15", NULL};
static const char *pamixer_inc_cmd[] = {"pamixer", "--allow-boost", "-i", "5",
                                        NULL};
static const char *pamixer_dec_fast_cmd[] = {"pamixer", "--allow-boost", "-d",
                                             "15", NULL};
static const char *pamixer_dec_cmd[] = {"pamixer", "--allow-boost", "-d", "5",
                                        NULL};
static const char *pamixer_mute_cmd[] = {"pamixer", "--allow-boost", "-t",
                                         NULL};
static const char *mpc_play_cmd[] = {"mpc", "-q", "toggle", NULL};
static const char *mpc_prev_cmd[] = {"mpc", "-q", "prev", NULL};
static const char *mpc_next_cmd[] = {"mpc", "-q", "next", NULL};

static Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_Down, focusstack, {.i = +1}},
    {MODKEY, XK_Up, focusstack, {.i = -1}},
    {MODKEY | ControlMask | ShiftMask, XK_Down, incnmaster, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_Up, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_Down, movestack, {.i = +1}},
    {MODKEY | ShiftMask, XK_Up, movestack, {.i = -1}},
    {MODKEY | ShiftMask, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ShiftMask, XK_q, killclient, {0}},
    {MODKEY | ShiftMask, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY | ShiftMask, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY | ShiftMask, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_f, fullscreen, {0}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_Left, focusmon, {.i = -1}},
    {MODKEY, XK_Right, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_Left, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_Right, tagmon, {.i = +1}},
    TAGKEYS(XK_1, 0),
    TAGKEYS(XK_2, 1),
    TAGKEYS(XK_3, 2),
    TAGKEYS(XK_4, 3),
    TAGKEYS(XK_5, 4),
    TAGKEYS(XK_6, 5),
    TAGKEYS(XK_7, 6),
    TAGKEYS(XK_8, 7),
    TAGKEYS(XK_9, 8),
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_d, spawn, {.v = rofiruncmd}},
    {MODKEY | ShiftMask, XK_e, spawn, {.v = session_menu_cmd}},
    {MODKEY, XK_m, spawn, {.v = pulsemixer_cmd}},
    {MODKEY, XK_grave, spawn, {.v = ec_cmd}},
    {MODKEY | ShiftMask, XK_grave, spawn, {.v = org_capture_cmd}},
    {MODKEY, XK_s, spawn, {.v = rofi_surfraw_cmd}},
    {MODKEY, XK_F8, spawn, {.v = keyboard_layout_cmd}},
    {MODKEY, XK_F7, spawn, {.v = displayselect_cmd}},
    {MODKEY, XK_F1, spawn, {.v = ranger_cmd}},
    {MODKEY | ShiftMask, XK_Print, spawn, {.v = capture_upload_cmd}},
    {MODKEY, XK_Print, spawn, {.v = capture_cmd}},

    {MODKEY, XF86XK_AudioRaiseVolume, spawn, {.v = pamixer_inc_fast_cmd}},
    {MODKEY | ShiftMask,
     XF86XK_AudioRaiseVolume,
     spawn,
     {.v = pamixer_inc_cmd}},

    {MODKEY, XF86XK_AudioLowerVolume, spawn, {.v = pamixer_dec_fast_cmd}},
    {MODKEY | ShiftMask,
     XF86XK_AudioLowerVolume,
     spawn,
     {.v = pamixer_dec_cmd}},

    {MODKEY | ShiftMask, XF86XK_AudioMute, spawn, {.v = pamixer_mute_cmd}},

    {MODKEY, XF86XK_AudioPlay, spawn, {.v = mpc_play_cmd}},

    {MODKEY, XF86XK_AudioPrev, spawn, {.v = mpc_prev_cmd}},
    {MODKEY, XF86XK_AudioNext, spawn, {.v = mpc_next_cmd}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument
     */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};

static const char *dwmfifo = "/tmp/dwm.fifo";
static Command commands[] = {
    {"dmenu", spawn, {.v = dmenucmd}},
    {"term", spawn, {.v = termcmd}},
    {"quit", quit, {0}},
    {"restart", quit, {.i = 1}},
    {"togglebar", togglebar, {0}},
    {"focusstack+", focusstack, {.i = +1}},
    {"focusstack-", focusstack, {.i = -1}},
    {"incnmaster+", incnmaster, {.i = +1}},
    {"incnmaster-", incnmaster, {.i = -1}},
    {"setmfact+", setmfact, {.f = +0.05}},
    {"setmfact-", setmfact, {.f = -0.05}},
    {"zoom", zoom, {0}},
    {"view", view, {0}},
    {"killclient", killclient, {0}},
    {"setlayout-tiled", setlayout, {.v = &layouts[0]}},
    {"setlayout-float", setlayout, {.v = &layouts[1]}},
    {"setlayout-mono", setlayout, {.v = &layouts[2]}},
    {"togglelayout", setlayout, {0}},
    {"togglefloating", togglefloating, {0}},
    {"viewall", view, {.ui = ~0}},
    {"tag", tag, {.ui = ~0}},
    {"focusmon+", focusmon, {.i = +1}},
    {"focusmon-", focusmon, {.i = -1}},
    {"tagmon+", tagmon, {.i = +1}},
    {"tagmon-", tagmon, {.i = -1}},
    {"view1", view, {.ui = 1 << 0}},
    {"view2", view, {.ui = 1 << 1}},
    {"view3", view, {.ui = 1 << 2}},
    {"view4", view, {.ui = 1 << 3}},
    {"view5", view, {.ui = 1 << 4}},
    {"view6", view, {.ui = 1 << 5}},
    {"view7", view, {.ui = 1 << 6}},
    {"view8", view, {.ui = 1 << 7}},
    {"view9", view, {.ui = 1 << 8}},
    {"toggleview1", toggleview, {.ui = 1 << 0}},
    {"toggleview2", toggleview, {.ui = 1 << 1}},
    {"toggleview3", toggleview, {.ui = 1 << 2}},
    {"toggleview4", toggleview, {.ui = 1 << 3}},
    {"toggleview5", toggleview, {.ui = 1 << 4}},
    {"toggleview6", toggleview, {.ui = 1 << 5}},
    {"toggleview7", toggleview, {.ui = 1 << 6}},
    {"toggleview8", toggleview, {.ui = 1 << 7}},
    {"toggleview9", toggleview, {.ui = 1 << 8}},
    {"tag1", tag, {.ui = 1 << 0}},
    {"tag2", tag, {.ui = 1 << 1}},
    {"tag3", tag, {.ui = 1 << 2}},
    {"tag4", tag, {.ui = 1 << 3}},
    {"tag5", tag, {.ui = 1 << 4}},
    {"tag6", tag, {.ui = 1 << 5}},
    {"tag7", tag, {.ui = 1 << 6}},
    {"tag8", tag, {.ui = 1 << 7}},
    {"tag9", tag, {.ui = 1 << 8}},
    {"toggletag1", toggletag, {.ui = 1 << 0}},
    {"toggletag2", toggletag, {.ui = 1 << 1}},
    {"toggletag3", toggletag, {.ui = 1 << 2}},
    {"toggletag4", toggletag, {.ui = 1 << 3}},
    {"toggletag5", toggletag, {.ui = 1 << 4}},
    {"toggletag6", toggletag, {.ui = 1 << 5}},
    {"toggletag7", toggletag, {.ui = 1 << 6}},
    {"toggletag8", toggletag, {.ui = 1 << 7}},
    {"toggletag9", toggletag, {.ui = 1 << 8}},
};
