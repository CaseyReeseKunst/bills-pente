/*
 * src/xio/xio.h, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#ifndef  _XIO_H_
#define  _XIO_H_  1

#ifndef  XIO_DEBUG
#define  XIO_DEBUG  0
#endif

enum  {
  XIO_LANG_ENGLISH,
  XIO_LANG_FRENCH,
  XIO_LANG_ITALIAN,
  XIO_LANG_COUNT};

extern bool  xio_gameover, xio_paused, xio_stopped, xio_safe_shutdown;
extern bool  xio_color;
extern int  xio_turn, xio_turn_num;
extern bd_loc_t  xio_lastmove, xio_moveSelected;

#define  XIO_PIC_OUTLINE   (BUT_DCOLORS+ 0)  /* The edges of the pieces. */
#define  XIO_PIC_BLACK     (BUT_DCOLORS+ 1)  /* Used for board markings. */
#define  XIO_PIC_DIMOUTLINE (BUT_DCOLORS+ 2) /* Edges of dimmed pieces. */
#define  XIO_PIC_STAMP     (BUT_DCOLORS+3) /* The "Pente" stamp. */
#define  XIO_PIC_BUT1FG   (BUT_DCOLORS+4)  /* fg player 1's menu.  */
#define  XIO_PIC_PL1      (BUT_DCOLORS+5)  /* Player 1's pieces. */
#define  XIO_PIC_PL2      (BUT_DCOLORS+6)  /* Player 2's pieces. */
#define  XIO_PIC_DIMPL1   (BUT_DCOLORS+7)  /* Player 1's dimmed pieces. */
#define  XIO_PIC_DIMPL2   (BUT_DCOLORS+8)  /* Player 2's dimmed pieces. */
#define  XIO_PIC_MARK1    (BUT_DCOLORS+9)  /* Mark on Player 1's pieces. */
#define  XIO_PIC_MARK2    (BUT_DCOLORS+10)  /* Mark on Player 2's pieces. */
#define  XIO_PIC_BOARD(n) (BUT_DCOLORS+ 11+(n))  /* These three MUST go */
#define  XIO_PIC_HIBD(n)  (BUT_DCOLORS+267+(n))  /* in sequence. */
#define  XIO_PIC_LOBD(n)  (BUT_DCOLORS+523+(n))

#define  XIO_NPICS        (BUT_DCOLORS+779)


#define  XIO_COMPCHAR     "\1\1"
#define  XIO_PL1CHAR      "\1\2"
#define  XIO_PL2CHAR      "\1\3"
#define  XIO_PL1MARKCHAR  "\1\4"
#define  XIO_PL2MARKCHAR  "\1\5"

struct XioSetup_struct;
struct XioHelp_struct;
struct XioRemAcc_struct;
struct XioMsgwin_struct;

typedef struct Xio_struct {
  Abut  *abut;
  ButEnv  *env;
  int  openWindows;
  int  dpy_depth;
  int  color;  /* TrueColor, PseudoColor, or GrayScale */
  int  bgColor, litColor, shadColor;
  ButWin  *mainwin, *iconwin, *tiwin;
  int  buth;
  int  lang;
  GC   gc;
  But  *gridbuts[BD_LOC_MAX], *iconbuts[BD_LOC_MAX];
  But  *caps1[5], *caps2[5], *digits[3], *turnlabel[15];
  But  *undobut, *redobut, *helpbut, *setupbut, *startbut, *quitbut;
  But  *menu1, *menu2, *soundbut;
  But  *tiwinok, *tiwinbox, *tiwintext, *tiwinin, *tiwincan;
  But  *copbox, *copback, *copwords1, *copwords2, *copwords3, *copwords4;
  ButTimer  *coptimer;
  Pixmap  bgpic, ibgpic;
  But  *bgbut, *ibgbut;
  bool  wait_nonicon;  /* Window mapped; on any press, record this. */
  bool  restart_x;
  bd_t  iboard;
  bd_loc_t  markpos;
  bool  netMaster;
  ButLnet  *lnet;
  ButRnet  *rnet;
  int  rcMajor, rcMinor, rcBugFix;
  bool  remoteEnd;
  struct XioSetup_struct  *swin;
  struct XioHelp_struct  *hwin;
  struct xioRemote_struct  *rwin;
  struct XioRemAcc_struct  *remAcc;
  AbutMsg  *waitWindow;
  
  struct  {
    ClpEntry  *ceVol, *ceOldVol, *ceSilent, *ceErr;
    int  volume, oldVolume;
    bool  silent, err;
  } sound;
} Xio;


typedef struct  XioCompBroadcast_struct  {
  StdInt32  think, pos, player;
} XioCompBroadcast;


typedef struct xio_tb_struct  {
  int  align, font;
  char *text;
} xio_tb_t;

#define  XIO_ICONPIECE   3
#define  XIO_ICONBORDER  2
#define  XIO_ICONSIZE    (19*XIO_ICONPIECE+2*XIO_ICONBORDER)
extern void  xio_resize(Xio *xio, bd_t *board);
extern void  xio_new_bg(Xio *xio,
			Pixmap bp, int xoff, int size, int boxw);

/* From "xio.c" */
extern ButColor  *xio_setup_colors(void);
extern ButOut  xio_press_undo(But *info);
extern ButOut  xio_press_move(But *info);
extern ButOut  xio_press_redo(But *info);
extern ButOut  xio_press_start(But *info);
extern ButOut  xio_press_help(But *info);
extern ButOut  xio_press_setup(But *info);
extern ButOut  xio_press_quit(But *info);
extern ButOut  xio_change_menu(But *but, int value);
extern int  xio_shutdown(Display *dpy);
extern bool  xio_opendisp(Xio *xio);
extern void  xio_sound_check(void);


/* From "plasma.c" */
extern uchar  *xio_plasma(void);

/* From "mainwin.c" */
extern void  xio_m_create(Xio *xio, const char *name, const char *geometry,
			  bool iconic, bool color);
extern void  xio_draw_comp(void *packet, ButWin *win,
			   int x, int y, int w, int h);
extern void  xio_m_set_turn(Xio *xio, int turn, int oldturn);

/* From "pentebuts.c" */
extern void  xio_draw_pl1char(void *packet, ButWin *win,
			      int x, int y, int w, int h);
extern void  xio_draw_pl2char(void *packet, ButWin *win,
			      int x, int y, int w, int h);
extern void  xio_draw_pl1markchar(void *packet, ButWin *win,
				  int x, int y, int w, int h);
extern void  xio_draw_pl2markchar(void *packet, ButWin *win,
				  int x, int y, int w, int h);
extern But  *but_grid_create(ButOut (*func)(But *but), ButWin *win,
			     int layer, int flags, bd_loc_t pos);
extern void  but_grid_change(But *but, uint newpiece, bool marked);
extern bd_loc_t  but_grid_loc(But *but);
extern But  *but_cap_create(ButWin *win, int layer, int flags, int capnum,
			    int player);
extern void  but_cap_change(But *but, uint newcap);

/* From "cm2pm.c" */
extern Pixmap  cm2pm(Xio *xio, uchar *cmap, uint cmapw,uint cmaph,
		     uint w,uint h, int pic0, int npic);
extern Pixmap  cm2pm_chop(Xio *xio, uchar *cmap, uint cmapw,uint cmaph, uint w,
			  uint h, int pic0, int npic, int gridw, int buth);

/* From "remote.c" */
extern bool  xio_r_init(Xio *xio);
extern void  xio_r_disable(Xio *xio);
extern void  xio_r_create(Xio *xio);
extern void  xioRemote_newState(Xio *xio);
extern void  xioRemote_comp(Xio *xio, bool think, bd_loc_t loc, int player);
extern void  xioRemote_destroy(Xio *xio);

/* From "strings.c" */
extern const char  *xioStr_langlist[XIO_LANG_COUNT][XIO_LANG_COUNT+1];
extern char  *xioStr_mfonts[];
extern char  *xioStr_bfonts[];
extern char  *xioStr_language[];
extern char  *xioStr_turn[];
extern char  *xioStr_captures[];
extern char  *xioStr_undo[];
extern char  *xioStr_start[];
extern char  *xioStr_stop[];
extern char  *xioStr_continue[];
extern char  *xioStr_redo[];
extern char  *xioStr_help[];
extern char  *xioStr_quit[];
extern char  *xioStr_ok[];
extern char  *xioStr_cancel[];
extern char  *xioStr_netConn[];
extern char  *xioStr_machine[];
extern char  *xioStr_reject[];
extern char  *xioStr_noRemote[];

extern char  *xioStr_player1[];
extern char  *xioStr_player2[];
extern char  *xioStr_human[];
extern char  *xioStr_remote[];
extern char  *xioStr_comp1[];
extern char  *xioStr_comp2[];
extern char  *xioStr_comp3[];
extern char  *xioStr_comp4[];
extern char  *xioStr_comp5[];
extern char  *xioStr_comp6[];

extern char  *xioStr_notEnoughColors[];

extern char  *xioStr_copyright[];
extern char  *xioStr_nowarr[];
extern char  *xioStr_seehelp[];

extern char  *xioStr_setup[];
extern char  *xioStr_pentesetup[];
extern char  *xioStr_language[];
extern char  *xioStr_color[];
extern char  *xioStr_showThink[];
extern char  *xioStr_soundvol[];
extern char  *xioStr_off[];
extern char  *xioStr_max[];
extern char  *xioStr_transtab[];
extern char  *xioStr_size[];
extern char  *xioStr_autosize[];
extern char  *xioStr_netplay[];
extern char  *xioStr_enabled[];
extern char  *xioStr_socket[];
extern char  *xioStr_disabled[];
extern char  *xioStr_connect[], *xioStr_disconnect[];

extern char  *xioStr_netOffer[];
extern char  *xioStr_netReject[];
extern char  *xioStr_netWait[];
extern char  *xioStr_netDisconnect[];
extern char  *xioStr_netBusy[];
extern char  *xioStr_netClosed[];
extern char  *xioStr_noSockets[];

extern char  *xioStr_errProtSocket[];
extern char  *xioStr_errSockInUse[];
extern char  *xioStr_errLSockGeneric[];
extern char  *xioStr_errSockNumInvalid[];
extern char  *xioStr_errRSockGeneric[];
extern char  *xioStr_errRSockRefused[];
extern char  *xioStr_errHostTemp[];
extern char  *xioStr_errHostPerm[];

extern char  *xioStr_hmenu[];
extern char  *xioStr_phelp[];
extern char  *xioStr_ghelp[];
extern char  *xioStr_shelp[];
extern char  *xioStr_nhelp[];
extern char  *xioStr_cphelp[];
extern char  *xioStr_ahelp[];
extern char  *xioStr_chelp[];
extern xio_tb_t  *xioStr_proghelp[];
extern xio_tb_t  *xioStr_gamehelp[];
extern xio_tb_t  *xioStr_setuphelp[];
extern xio_tb_t  *xioStr_networkhelp[];
extern xio_tb_t  *xioStr_comphelp[];
extern xio_tb_t  *xioStr_abouthelp[];
extern xio_tb_t  *xioStr_copyinghelp[];

#endif  /* _XIO_H_ */
