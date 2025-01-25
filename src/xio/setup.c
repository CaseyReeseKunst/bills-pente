/*
 * src/xio/setup.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Routines to manage the setup window.
 */

#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <but/text.h>
#include <but/menu.h>
#include <but/plain.h>
#include <but/box.h>
#include <but/ctext.h>
#include <but/textin.h>
#include <but/slide.h>
#include <but/checkbox.h>
#include <but/net.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "../pente.h"
#include "xio.h"
#include "setup.h"
#include "sound.h"
#include "helpwin.h"
#include "remAcc.h"

#define  MAX_PORTNAME_LEN  50
static char  portName[MAX_PORTNAME_LEN+1];

static bool  wait_for_open = FALSE;

static ButOut  unmap(ButWin *win);
static ButOut  resize(ButWin *win);
static ButOut  destroy(ButWin *win);
static ButOut  ok_pressed(But *but);
static ButOut  connPressed(But *but);
static ButOut  lang_pressed(But *but, int value);
static ButOut  help_pressed(But *but);
static ButOut  color_pressed(But *but, int value);
static ButOut  remote_toggle(But *but, int value);
static ButOut  volset(But *but, int val, bool newPress);
static ButOut  newSocket(But *but, const char *value);
static ButOut  showThinkPressed(But *but, bool val);


XioSetup  *xioSetup_create(Xio *xio)  {
  XioSetup  *swin;
  static ButKey  ka_ok[] = {{XK_Return, 0,0}, {XK_KP_Enter, 0,0}, {0,0,0}};
  static ButKey  helpkeys[] = {{XK_H, 0,0}, {XK_h, 0,0}, {XK_Help, 0,0},
                               {XK_question, 0,0}, {0,0,0}};
  static ButKey  soundkeys[] = {{XK_S, 0,0}, {XK_s, 0,0}, {0,0,0}};
  ButWin  *win;
  char  geometry[100];
  int  w, h, minh;

  if (xio->swin && !wait_for_open)  {
    XRaiseWindow(butEnv_dpy(xio->env), butWin_xwin(xio->swin->win));
    return(xio->swin);
  }
  w = butWin_w(xio->mainwin);
  h = (int)(butWin_h(xio->mainwin)*0.61803399+0.5);
  swin = malloc(sizeof(XioSetup));
  if (w == 0)  {  /* Main window hasn't been opened! */
    wait_for_open = TRUE;
    return(swin);
  } else
    wait_for_open = FALSE;
  swin->xio = xio;
  ++xio->openWindows;
  clp_setBool(pe_clp, "x.setup", TRUE);
  minh = (but_h(xio->helpbut)*4+butEnv_stdBw(xio->env)*9) * 2;
  if (h < minh)
    h = minh;
  sprintf(geometry, "%dx%d%s", w, h, clp_getStr(pe_clp, "x.setup.geom"));
  win = swin->win = butWin_create(swin, xio->env, "Pente Setup", w, h,
				  unmap, NULL, resize, destroy);
  butWin_activate(win);
  swin->xio = xio;
  swin->mainbox = butBoxFilled_create(win, 0, BUT_DRAWABLE);
  butBoxFilled_setColors(swin->mainbox, xio->litColor, xio->shadColor,
			 xio->bgColor);
  swin->title = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_pentesetup[xio->lang],
			       butText_center);
  butText_setFont(swin->title, 1);  /* Bold. */

  swin->box1 = butBox_create(win, 1, BUT_DRAWABLE);
  butBox_setColors(swin->box1, xio->litColor, xio->shadColor);
  swin->langs = butMenu_downCreate(lang_pressed, xio, win, 1, 2,
				   BUT_DRAWABLE|BUT_PRESSABLE,
				   xioStr_language[xio->lang],
				   xioStr_langlist[xio->lang], xio->lang);
  swin->colorb = butCb_create(color_pressed, xio, win, 1,
			      BUT_PRESSABLE|BUT_DRAWABLE,
			      (xio->color != GrayScale));
  swin->colorl = butText_create(win, 1, BUT_DRAWABLE,
				xioStr_color[xio->lang], butText_left);
  swin->showThinkBox = butCb_create(showThinkPressed, xio, win, 1,
				    BUT_PRESSABLE|BUT_DRAWABLE,
				    clp_getBool(pe_clp, "showthink"));
  swin->showThinkTxt = butText_create(win, 1, BUT_DRAWABLE,
				      xioStr_showThink[xio->lang],
				      butText_right);

  swin->box2 = butBox_create(win, 1, BUT_DRAWABLE);
  butBox_setColors(swin->box2, xio->litColor, xio->shadColor);
  swin->svt1 = butText_create(win, 1, BUT_DRAWABLE,
			      xioStr_soundvol[xio->lang], butText_center);
  swin->svoff = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_off[xio->lang], butText_left);
  swin->svmax = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_max[xio->lang], butText_right);
  swin->svbar = butSlide_hCreate(volset, xio, win, 1,
				 BUT_DRAWABLE|BUT_PRESSABLE,
				 SND_MAXVOL, xio->sound.volume,
				 (SND_MAXVOL+9)/10);
  swin->svout = butText_create(win, 1, BUT_DRAWABLE,
			       "---%", butText_center);
  swin->svtoggle = butKeytrap_create(xioSound_toggle, xio, win,
				     BUT_DRAWABLE|BUT_PRESSABLE);
  but_setKeys(swin->svtoggle, soundkeys);
  xioSetup_newVol(swin, butSlide_get(swin->svbar));

  swin->box3 = butBox_create(win, 1, BUT_DRAWABLE);
  butBox_setColors(swin->box3, xio->litColor, xio->shadColor);
  swin->ttlab = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_transtab[xio->lang], butText_center);
  butText_setColor(swin->ttlab, BUT_FG, TRUE);
  swin->ttsize = butText_create(win, 1, BUT_DRAWABLE,
				xioStr_size[xio->lang], butText_left);
  butText_setColor(swin->ttsize, BUT_FG, TRUE);
  swin->ttsizedown = butAct_create(NULL, xio, win, 1, BUT_DRAWABLE,
				    "-", BUT_ALEFT|BUT_SRIGHT);
  swin->ttsizebox = butBox_create(win, 1, BUT_DRAWABLE);
  butBox_setColors(swin->ttsizebox, BUT_SHAD, BUT_LIT);
  swin->ttsizeout = butText_create(win, 2, BUT_DRAWABLE,
				   xioStr_disabled[xio->lang],
				   butText_right);
  butText_setColor(swin->ttsizeout, BUT_FG, TRUE);
  swin->ttsizebg = butPlain_create(win, 1, BUT_DRAWABLE, BUT_ENTERBG);
  swin->ttsizeup = butAct_create(NULL, xio, win, 1, BUT_DRAWABLE, "+",
				 BUT_SLEFT|BUT_ARIGHT);
  swin->ttauto = butCt_create(NULL, xio, win, 1, BUT_DRAWABLE,
			      xioStr_autosize[xio->lang]);

  swin->box4 = butBox_create(win, 1, BUT_DRAWABLE);
  butBox_setColors(swin->box4, xio->litColor, xio->shadColor);
  swin->remote = butText_create(win, 1, BUT_DRAWABLE,
				xioStr_netplay[xio->lang], butText_center);
  swin->rmchk = butCb_create(remote_toggle, xio, win, 1,
			     BUT_DRAWABLE | BUT_PRESSABLE,
			     clp_getBool(pe_clp, "x.remote.enabled"));
  swin->rmchklab = butText_create(win, 1, BUT_DRAWABLE,
				  xioStr_enabled[xio->lang], butText_left);
  swin->rmsock = butText_create(win, 1, BUT_DRAWABLE,
				xioStr_socket[xio->lang], butText_left);
  swin->rmConn = butCt_create(connPressed, xio, win, 1,
			      BUT_DRAWABLE|BUT_PRESSABLE, "");
  xioSetup_remoteRelabel(swin);
  swin->rmsockin = butTextin_create(newSocket, xio, win, 1,
				    BUT_DRAWABLE|BUT_PRESSABLE,
				    clp_getStr(pe_clp, "x.remote.lsock"),
				    MAX_PORTNAME_LEN);

  swin->ok = butCt_create(ok_pressed, xio, win, 1, BUT_DRAWABLE|BUT_PRESSABLE,
			  xioStr_ok[xio->lang]);
  but_setKeys(swin->ok, ka_ok);
  swin->help = butCt_create(help_pressed, NULL, win, 1,
			    BUT_DRAWABLE|BUT_PRESSABLE,
			    xioStr_help[xio->lang]);
  but_setKeys(swin->help, helpkeys);
  return(swin);
}


void  xioSetup_resize(XioSetup *swin)  {
  Xio  *xio;
  int  winw;
  int  w, h, minh;

  if (swin)  {
    xio = swin->xio;
    if (wait_for_open)
      xioSetup_create(xio);
    winw = butWin_w(xio->mainwin);
    w = winw;
    h = (int)(winw * 0.61803399 + 0.5);
    minh = (but_h(xio->helpbut)*4+butEnv_stdBw(xio->env)*9) * 2;
    if (h < minh)
      h = minh;
    butWin_resize(swin->win, winw, h);
  }
}


static ButOut  unmap(ButWin *win)  {
  clp_setBool(pe_clp, "x.setup", FALSE);
  butWin_destroy(win);
  return(0);
}


static ButOut  resize(ButWin *win)  {
  int  w, h, ax, ay, aw, ah, buth, i, j;
  XioSetup  *swin;
  Xio  *xio;
  int  bw;
  ButEnv  *env;

  swin = butWin_packet(win);
  env = butWin_env(win);
  bw = butEnv_stdBw(env);
  xio = butEnv_packet(env);
  buth = but_h(xio->helpbut);
  w = butWin_w(win);
  h = butWin_h(win);
  but_resize(swin->mainbox, 0,0, w,h);
  but_resize(swin->title, bw,bw, w-bw*2,buth+bw*2);

  ax = bw;
  ay = bw*3+buth;
  aw = w/2-bw,
  ah = h/2-(bw*3+buth);
  but_resize(swin->box1, ax, ay, aw, ah);
  but_resize(swin->langs, ax+bw*2, ay+bw*2, aw-bw*4, buth*2);
  but_resize(swin->colorb, ax+bw*2, ay+buth*2+bw*3, buth, buth);
  butText_resize(swin->colorl, ax+bw*3+buth, ay+buth*2+bw*3, buth);
  but_resize(swin->showThinkBox, ax+aw-bw*2-buth, ay+buth*2+bw*3, buth, buth);
  butText_resize(swin->showThinkTxt, ax+aw-bw*3-buth, ay+buth*2+bw*3, buth);

  ax = bw;
  ay = h/2;
  aw = w/2-bw,
  ah = h-(h/2)-(bw*3+buth);
  but_resize(swin->box2, ax,ay, aw,ah);
  but_resize(swin->svt1, ax+bw*2, ay+bw*2, aw-bw*4, buth);
  i = butText_resize(swin->svoff, ax+bw*2, ay+bw*3+buth, buth);
  j = butText_resize(swin->svmax, ax+aw-bw*2, ay+bw*3+buth, buth);
  but_resize(swin->svbar, ax+bw*3+i, ay+bw*3+buth, aw-bw*6-i-j, buth);
  but_resize(swin->svout, ax+bw*3, ay+bw*4+buth*2, aw-bw*6, buth);

  ax = w/2;
  ay = bw*3+buth;
  aw = w-w/2-bw,
  ah = h/2-(bw*3+buth);
  but_resize(swin->box3, ax,ay, aw,ah);
  but_resize(swin->ttlab, ax+bw*2, ay+bw*2, aw-bw*4, buth);
  i = butText_resize(swin->ttsize, ax+bw*2, ay+bw*3+buth, buth);
  i += bw * 3;
  but_resize(swin->ttsizedown, ax+i, ay+bw*3+buth, buth,buth);
  but_resize(swin->ttsizebox, ax+i+buth, ay+bw*3+buth,
	     aw-i-buth*2-bw*2, buth);
  but_resize(swin->ttsizeout, ax+i+buth+bw*2, ay+bw*3+buth+bw,
	     aw-i-buth*2-bw*6, buth-bw*2);
  but_resize(swin->ttsizebg, ax+i+buth+bw, ay+bw*3+buth+bw,
	     aw-i-buth*2-bw*4, buth-bw*2);
  but_resize(swin->ttsizeup, ax+aw-buth-bw*2, ay+bw*3+buth, buth,buth);
  but_resize(swin->ttauto, ax+bw*2,ay+bw*4+buth*2, aw-bw*4,buth);

  ax = w/2;
  ay = h/2;
  aw = w-w/2-bw,
  ah = h-h/2-(bw*3+buth);
  but_resize(swin->box4, ax,ay, aw,ah);
  but_resize(swin->remote, ax+bw*2, ay+bw*2, aw-bw*4, buth);
  but_resize(swin->rmchk, ax+bw*2, ay+bw*3+buth, buth, buth);
  but_resize(swin->rmchklab, ax+bw*3+buth, ay+bw*3+buth,
	     aw/2-bw*3-buth, buth);
  but_resize(swin->rmConn, ax+(aw+bw)/2, ay+bw*3+buth,
	     aw-(aw+bw)/2-bw*2, buth);
  i = butText_resize(swin->rmsock, ax+bw*2, ay+bw*4+buth*2, buth);
  but_resize(swin->rmsockin, ax+bw*3+i, ay+bw*4+buth*2, aw-bw*5-i, buth);

  but_resize(swin->ok, bw*2, h-buth-bw*2, (w-bw*5)/2, buth);
  but_resize(swin->help, (w-bw*5)/2+bw*3, h-buth-bw*2,
	     w-(w-bw*5)/2-bw*5, buth);
  return(0);
}


static ButOut  ok_pressed(But *but)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));
  ButOut  retVal = 0;

  clp_setBool(pe_clp, "x.setup", FALSE);
  /*
   * If listening is enabled AND the user has typed in a new socket,
   *   then try opening it.
   */
  if (strcmp(clp_getStr(pe_clp, "x.remote.lsock"),
	     butTextin_get(xio->swin->rmsockin)))  {
    strcpy(portName, butTextin_get(xio->swin->rmsockin));
    clp_setStr(pe_clp, "x.remote.lsock", portName);
    clp_setBool(pe_clp, "x.remote.wanted", TRUE);
    clp_setBool(pe_clp, "x.remote.enabled", TRUE);
    if (!xio_r_init(xio))  {
      retVal = BUTOUT_ERR;
    }
  }
  butWin_destroy(but_win(but));
  return(retVal);
}


static ButOut  help_pressed(But *but)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  xio->hwin = xioHelp_create(xio, 2);
  return(0);
}


static ButOut  color_pressed(But *but, int value)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  clp_setBool(pe_clp, "wasColor", xio_color = value);
  clp_setBool(pe_clp, "color", value);
  xio->restart_x = TRUE;
  return(BUTOUT_STOPWAIT);
}


static ButOut  lang_pressed(But  *but, int value)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  if (xio->lang != value)  {
    xio->lang = value;
    clp_setStr(pe_clp, "language", xioStr_langlist[0][value]);
    xio->restart_x = TRUE;
  }
  return(BUTOUT_STOPWAIT);
}


static ButOut  destroy(ButWin *win)  {
  Xio  *xio = butEnv_packet(butWin_env(win));
  static char  geom[30];

  --xio->openWindows;
  xio->swin = NULL;
  xioSound_check(xio);
  sprintf(geom, "+%d+%d", butWin_x(win), butWin_y(win));
  clp_setStr(pe_clp, "x.setup.geom", geom);
  xioSound_check(xio);
  return(0);
}
  

static ButOut  volset(But *but, int newvol, bool newPress)  {
  int  percent;
  XioSetup  *swin = butWin_packet(but_win(but));
  char  text[5];

  if (newPress)
    xioSound_showErr(swin->xio);
  /* Round percent up so that it only says 0% if it's REALLY at zero. */
  percent = (newvol*100+SND_MAXVOL-1)/SND_MAXVOL;
  sprintf(text, "%d%%", (newvol*100+SND_MAXVOL/2)/SND_MAXVOL);
  butText_set(swin->svout, text);
  xioSound_setVol(swin->xio, newvol, newPress);
  return(0);
}


static ButOut  remote_toggle(But *but, int value)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  clp_setBool(pe_clp, "x.remote.wanted", value);
  clp_setBool(pe_clp, "x.remote.enabled", value);
  but_setFlags(xio->swin->rmsockin, BUT_NOKEY);
  if (value)  {
    if (xio_r_init(xio))
      return(0);
    butCb_set(but, FALSE, FALSE);
    return(BUTOUT_ERR);
  } else
    xio_r_disable(xio);
  return(0);
}


static ButOut  newSocket(But *but, const char *value)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  but_setFlags(but, BUT_NOKEY);
  clp_setBool(pe_clp, "x.remote.enabled", TRUE);
  clp_setBool(pe_clp, "x.remote.wanted", TRUE);
  if (xio_r_init(xio))  {
    butCb_set(xio->swin->rmchk, TRUE, FALSE);
    return(0);
  } else  {
    butCb_set(xio->swin->rmchk, FALSE, FALSE);
    return(BUTOUT_ERR);
  }
}


ButOut  connPressed(But *but)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  if (xio->rnet || xio->remAcc || xio->waitWindow)  {
    /* We already have a connection open, so we'll press disconnect! */
    if (xio->rnet)  {
      butRnet_destroy(xio->rnet, xioStr_netClosed[xio->lang]);
      xio->rnet = NULL;
    }
    if (xio->remAcc)  {
      xioRemAcc_destroy(xio->remAcc);
      xio->remAcc = NULL;
    }
    if (xio->waitWindow)  {
      abutMsg_destroy(xio->waitWindow, TRUE);
      xio->waitWindow = NULL;
    }
  } else
    xio_r_create(butEnv_packet(butWin_env(but_win(but))));
  xioSetup_remoteRelabel(xio->swin);
  return(0);
}


void  xioSetup_freshPortName(XioSetup *swin)  {
  if (swin && !wait_for_open)  {
    strcpy(portName, butTextin_get(swin->rmsockin));
    clp_setStr(pe_clp, "x.remote.lsock", portName);
  }
}
  

void  xioSetup_newVol(struct XioSetup_struct *swin, int vol)  {
  char  text[5];

  if (swin)  {
    butSlide_set(swin->svbar, BUT_NOCHANGE, vol, BUT_NOCHANGE);
    sprintf(text, "%d%%", (vol*100+SND_MAXVOL/2)/SND_MAXVOL);
    butText_set(swin->svout, text);
  }
}


static ButOut  showThinkPressed(But *but, bool val)  {
  clp_setBool(pe_clp, "showthink", val);
  xio_think(PE_REDO, -1);
  return(0);
}


void  xioSetup_remoteRelabel(XioSetup *swin)  {
  Xio  *xio;

  if (swin)  {
    xio = swin->xio;
    if (xio->rnet || xio->remAcc || xio->waitWindow)
      butCt_setText(swin->rmConn, xioStr_disconnect[xio->lang]);
    else
      butCt_setText(swin->rmConn, xioStr_connect[xio->lang]);
  }
}


#endif  /* X11_DISP */
