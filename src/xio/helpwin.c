/*
 * src/xio/helpwin.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <but/menu.h>
#include <but/tblock.h>
#include <but/plain.h>
#include <but/box.h>
#include <but/ctext.h>
#include <abut/abut.h>
#include <abut/swin.h>
#include <abut/msg.h>
#include <but/canvas.h>
#include "../pente.h"
#include "xio.h"
#include "sound.h"
#include "helpwin.h"

typedef struct XioHelp_struct  {
  Xio  *xio;
  But  *box, *dbg, *ok, *soundbut;
  But  *menu;
  But  **textbuts;
  int  n_textbuts, max_textbuts;
  AbutSwin  *swin;
  ButWin  *win, *dwin;
  int  litcolor, shadcolor, bgcolor;
  int  canw, canh, lastViewW;
  MAGIC_STRUCT
} XioHelp;


static bool  wait_for_open = FALSE;

static ButOut  unmap(ButWin *win);
static ButOut  resize(ButWin *win);
static ButOut  destroy(ButWin *win);
static ButOut  ok_pressed(But  *but);
static ButOut  dwin_resize(ButWin *win);
static ButOut  change_menu(But *but, int value);
static void  setup_text(Xio *xio, XioHelp *hwin, bool destroy);


XioHelp  *xioHelp_create(Xio *xio, int pagenum)  {
  static ButKey  ka_ok[] = {{XK_Return, 0,0}, {XK_KP_Enter, 0,0}, {0,0,0}};
  static ButKey  soundkeys[] = {{XK_S, 0,0}, {XK_s, 0,0}, {0,0,0}};
  XioHelp  *hwin;
  ButWin  *win;
  char  geometry[100];
  const char  *optlist[8];
  int  w, h, minh;

  clp_setBool(pe_clp, "x.help", TRUE);
  if (xio->hwin && !wait_for_open)  {
    hwin = xio->hwin;
    assert(MAGIC(hwin));
    XRaiseWindow(butEnv_dpy(xio->env), butWin_xwin(hwin->win));
    if ((pagenum != -1) &&
	(pagenum != butMenu_get(hwin->menu)))  {
      butMenu_set(hwin->menu, pagenum);
      change_menu(hwin->menu, pagenum);
    }
    return(hwin);
  }
  if (pagenum != -1)  {
    clp_setInt(pe_clp, "x.help.page", pagenum);
    clp_setDouble(pe_clp, "x.help.bookmark", 0.0);
  }
  w = butWin_w(xio->mainwin);
  h = butWin_h(xio->mainwin);
  if (w == 0)  {  /* Main window hasn't been opened! */
    wait_for_open = TRUE;
    return(NULL);
  } else
    wait_for_open = FALSE;
  hwin = wms_malloc(sizeof(XioHelp));
  MAGIC_SET(hwin);
  hwin->xio = xio;
  hwin->box = NULL;
  hwin->dbg = NULL;
  hwin->ok = NULL;
  hwin->soundbut = NULL;
  hwin->menu = NULL;
  hwin->textbuts = NULL;
  hwin->n_textbuts = hwin->max_textbuts = 0;
  hwin->swin = NULL;
  hwin->win = hwin->dwin = NULL;
  hwin->lastViewW = 0;

  minh = (but_h(xio->helpbut)*4+butEnv_stdBw(xio->env)*9) * 2;
  if (h < minh)
    h = minh;
  if (w < minh)
    w = minh;
  if (clp_getStr(pe_clp, "x.help.geom")[0] == '\0')  {
    sprintf(geometry, "%dx%d", w, h);
  } else  {
    sprintf(geometry, "%s", clp_getStr(pe_clp, "x.help.geom"));
  }
  if (xio->color == PseudoColor)  {
    hwin->bgcolor = XIO_PIC_BOARD(4);
    hwin->litcolor = XIO_PIC_BOARD(8+7);
    hwin->shadcolor = XIO_PIC_BOARD(16);
  } else if (xio->color == TrueColor)  {
    hwin->bgcolor = XIO_PIC_BOARD(128);
    hwin->litcolor = XIO_PIC_BOARD(511);
    hwin->shadcolor = XIO_PIC_BOARD(512);
  } else  {  /* xio->color == GrayScale */
    hwin->bgcolor = BUT_BG;
    hwin->litcolor = BUT_LIT;
    hwin->shadcolor = BUT_SHAD;
  }
  ++xio->openWindows;
  hwin->win = win = butWin_create(hwin, xio->env, "Pente Help", w,h,
				  unmap, NULL, resize, destroy);
  butWin_setMinW(win, 100);
  butWin_setMinH(win, 100);
  butWin_setMaxW(win, 0);
  butWin_setMaxH(win, 0);
  butWin_setGeom(win, geometry);
  butWin_activate(win);
  hwin->box = butBoxFilled_create(win, 0, BUT_DRAWABLE);
  butBoxFilled_setColors(hwin->box,
			 hwin->litcolor, hwin->shadcolor, hwin->bgcolor);
  optlist[0] = xioStr_phelp[xio->lang];
  optlist[1] = xioStr_ghelp[xio->lang];
  optlist[2] = xioStr_shelp[xio->lang];
  optlist[3] = xioStr_nhelp[xio->lang];
  optlist[4] = xioStr_cphelp[xio->lang];
  optlist[5] = xioStr_ahelp[xio->lang];
  optlist[6] = xioStr_chelp[xio->lang];
  optlist[7] = BUTMENU_OLEND;
  hwin->xio = xio;
  hwin->menu = butMenu_downCreate(change_menu, NULL, win, 1, 3,
				  BUT_DRAWABLE|BUT_PRESSABLE,
				  xioStr_hmenu[xio->lang],
				  optlist, clp_getInt(pe_clp, "x.help.page"));
  hwin->ok = butCt_create(ok_pressed, NULL, win, 1,
			  BUT_DRAWABLE|BUT_PRESSABLE, "OK");
  but_setKeys(hwin->ok, ka_ok);
  hwin->soundbut = butKeytrap_create(xioSound_toggle, xio, win,
				     BUT_DRAWABLE|BUT_PRESSABLE);
  but_setKeys(hwin->soundbut, soundkeys);
  hwin->swin = abutSwin_create(hwin, win, 1, ABUTSWIN_LSLIDE,
				dwin_resize);
  hwin->swin->yCenter = clp_getDouble(pe_clp, "x.help.bookmark");
  hwin->dwin = hwin->swin->win;
  setup_text(xio, hwin, FALSE);
  hwin->dbg = butPlain_create(hwin->dwin, 0, BUT_DRAWABLE, BUT_BG);
  return(hwin);
}


void  xioHelp_resize(XioHelp *hwin)  {
  if (hwin)  {
    assert(MAGIC(hwin));
    resize(hwin->win);
  }
}


static void  setup_text(Xio *xio, XioHelp *hwin, bool destroy)  {
  xio_tb_t  *tblist;
  static int  last_menu = -1;
  int  this_menu;
  int  i;

  assert(MAGIC(hwin));
  if (destroy)  {
    last_menu = -1;
    hwin->n_textbuts = 0;
    return;
  }
  clp_setInt(pe_clp, "x.help.page",
	     this_menu = butMenu_get(hwin->menu));
  if (this_menu == last_menu)
    return;
  last_menu = this_menu;
  for (i = 0;  i < hwin->n_textbuts;  ++i)
    but_destroy(hwin->textbuts[i]);
  hwin->n_textbuts = 0;
  switch(this_menu)  {
  case 0:  /* Program help. */
    tblist = xioStr_proghelp[xio->lang];
    break;
  case 1:  /* Game help. */
    tblist = xioStr_gamehelp[xio->lang];
    break;
  case 2:  /* Setup help. */
    tblist = xioStr_setuphelp[xio->lang];
    break;
  case 3:  /* Network play. */
    tblist = xioStr_networkhelp[xio->lang];
    break;
  case 4:  /* Abouth the computer players. */
    tblist = xioStr_comphelp[xio->lang];
    break;
  case 5:  /* About the author. */
    tblist = xioStr_abouthelp[xio->lang];
    break;
  case 6:  /* Copying. */
    tblist = xioStr_copyinghelp[xio->lang];
    break;
  default:  /* -1; closing down. */
    return;
  }
  for (i = 0;  tblist[i].text != NULL;  ++i);
  hwin->n_textbuts = i;
  if (hwin->max_textbuts < hwin->n_textbuts)  {
    if (hwin->textbuts != NULL)
      wms_free(hwin->textbuts);
    hwin->textbuts = wms_malloc(hwin->n_textbuts * sizeof(But *));
    hwin->max_textbuts = hwin->n_textbuts;
  }
  for (i = 0;  i < hwin->n_textbuts;  ++i)  {
    hwin->textbuts[i] =
      butTblock_create(hwin->dwin, 1, BUT_DRAWABLE,
		       tblist[i].text, tblist[i].align);
    butTblock_setFont(hwin->textbuts[i], tblist[i].font);
  }
}


static ButOut  unmap(ButWin *win)  {
  XioHelp  *hwin = butWin_packet(win);

  assert(MAGIC(hwin));
  clp_setBool(pe_clp, "x.help", FALSE);
  xioHelp_storeInfo(hwin);
  butWin_destroy(win);
  return(0);
}


static ButOut  resize(ButWin *win)  {
  int  w, h, buth;
  ButEnv  *env = butWin_env(win);
  int  bw = butEnv_stdBw(env);
  XioHelp  *hwin = butWin_packet(win);
  Xio  *xio = hwin->xio;
  int  fontsize;

  assert(MAGIC(hwin));
  w = butWin_w(win);
  h = butWin_h(win);
  buth = but_h(xio->helpbut);
  but_resize(hwin->box, 0,0, w,h);
  but_resize(hwin->menu, bw*2,bw*2, w-bw*4,buth*2);
  but_resize(hwin->ok, bw*2, h-bw*2-buth, w - bw*4, buth);
  fontsize = butEnv_fontStr(env, 0)->ascent + butEnv_fontStr(env, 0)->descent;
  abutSwin_resize(hwin->swin, bw*2, bw*3+buth*2, w-bw*4, h-bw*6-buth*3,
		   buth, fontsize);
  return(0);
}


static ButOut  dwin_resize(ButWin *win)  {
  AbutSwin  *swin;
  XioHelp  *hwin;
  int  w, h, i, cur_h, butbw;

  assert(MAGIC(win));
  swin = butWin_packet(win);
  assert(MAGIC(swin));
  hwin = swin->packet;
  assert(MAGIC(hwin));
  w = butWin_viewW(win);
  h = butWin_viewH(win);
  if (w != hwin->lastViewW)  {
    hwin->lastViewW = w;
    butbw = butEnv_stdBw(butWin_env(win));
    cur_h = 0;
    for (i = 0;  i < hwin->n_textbuts;  ++i)
      cur_h += butTblock_resize(hwin->textbuts[i], butbw, cur_h, w-butbw*2);
    if (cur_h < h)
      cur_h = h;
    but_resize(hwin->dbg, 0,0, w,cur_h);
    butCan_resizeWin(hwin->swin->win, w, cur_h, TRUE);
  }
  return(0);
}


static ButOut  ok_pressed(But *but)  {
  XioHelp  *hwin = butWin_packet(but_win(but));

  assert(MAGIC(hwin));
  clp_setBool(pe_clp, "x.help", FALSE);
  xioHelp_storeInfo(hwin);
  butWin_destroy(but_win(but));
  return(BUTOUT_STOPWAIT);
}


static ButOut  destroy(ButWin *win)  {
  XioHelp  *hwin = butWin_packet(win);
  Xio  *xio;

  assert(MAGIC(hwin));
  xio = hwin->xio;
  setup_text(butEnv_packet(butWin_env(win)), hwin, TRUE);
  abutSwin_destroy(hwin->swin);
  --xio->openWindows;
  xioSound_check(hwin->xio);
  MAGIC_UNSET(hwin);
  wms_free(hwin);
  xio->hwin = NULL;
  return(0);
}


void  xioHelp_storeInfo(XioHelp *hwin)  {
  static char  geom[60];
  ButWin  *win;

  if (hwin)  {
    assert(MAGIC(hwin));
    win = hwin->win;
    sprintf(geom, "%dx%d+%d+%d", butWin_w(win), butWin_h(win),
	    butWin_x(win), butWin_y(win));
    clp_setStr(pe_clp, "x.help.geom", geom);
    clp_setDouble(pe_clp, "x.help.bookmark", hwin->swin->yCenter);
  }
}
    

static ButOut  change_menu(But *but, int value)  {
  XioHelp  *hwin = butWin_packet(but_win(but));

  assert(MAGIC(hwin));
  if (value != clp_getInt(pe_clp, "x.help.page"))  {
    clp_setDouble(pe_clp, "x.help.bookmark", 0.0);
    setup_text(butEnv_packet(butWin_env(but_win(but))), hwin, FALSE);
    abutSwin_resize(hwin->swin, BUT_NOCHANGE,BUT_NOCHANGE,
		    BUT_NOCHANGE,BUT_NOCHANGE, BUT_NOCHANGE, BUT_NOCHANGE);
    /* Set lastViewW to 0 to force it to resize the canvas. */
    hwin->lastViewW = 0;
    dwin_resize(hwin->dwin);
    abutSwin_vMove(hwin->swin, 0);
  }
  return(0);
}


#endif  /* X11_DISP */
