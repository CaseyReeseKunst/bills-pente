/*
 * src/xio/setup.h, part of Pente (game program)
 * Copyright (C) 1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Header file for routines to manage the setup window.
 */

/**********************************************************************
 * Types
 **********************************************************************/
typedef struct XioSetup_struct  {
  Xio  *xio;
  int  lastVol, vol;
  ButWin  *win;
  But  *title, *mainbox;
  But  *box1, *box2, *box3, *box4;
  But  *langs, *colorb, *colorl;
  But  *showThinkTxt, *showThinkBox;
  But  *svt1, *svoff, *svmax, *svbar, *svout, *svtoggle;
  But  *ttlab, *ttsize, *ttsizebox, *ttsizeout, *ttsizeup, *ttsizedown;
  But  *ttauto, *ttsizebg;
  But  *remote, *rmchk, *rmchklab, *rmsock, *rmsockin, *rmConn;
  But  *ok, *help;
} XioSetup;


/**********************************************************************
 * Functions available externally.
 **********************************************************************/
extern struct XioSetup_struct  *xioSetup_create(Xio *xio);
extern void  xioSetup_resize(struct XioSetup_struct *swin);
extern void  xioSetup_newVol(XioSetup *swin, int vol);
extern void  xioSetup_freshPortName(struct XioSetup_struct *swin);
#define  xioSetup_newRemoteEnable(s,e)  \
  do{if (s) butCb_set((s)->rmchk,(e),FALSE);}while(0)
#define  xioSetup_win(setup)  ((setup)->win)
extern void  xioSetup_remoteRelabel(XioSetup *swin);
