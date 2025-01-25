/*
 * src/xio/pentebuts.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * This code extends the wmslib/but library to add special buttons needed
 *   for pente.
 */

#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <but/net.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "../pente.h"
#include "xio.h"


static ButOut  grid_mmove(But *but, int x, int y);
static ButOut  grid_mleave(But *but);
static ButOut  grid_mpress(But *but, int butnum, int x, int y);
static ButOut  grid_mrelease(But *but, int butnum, int x, int y);
static void  grid_draw(But *but, int x,int y,int w,int h);
static ButOut  grid_destroy(But *but);
static void  grid_newflags(But *but, uint fl);
static void  cap_draw(But *but, int x,int y,int w,int h);
static void  drawp(ButWin *win, uint piece, int x, int y, int size,
		   bool black_border);
static void  mark_piece(ButWin *win, uint piece, int x, int y, uint size);
static ButOut  cap_destroy(But *but);
static ButOut  gridNetPress(But *but, void *buf, int bufLen);

typedef struct grid_t  {
  bd_loc_t  pos;
  uint  ptype;  /* The piece at this point on the board. */
  bool marked;
  ButOut (*callback)(But *but);
} grid_t;

static ButAction  grid_action = {
  grid_mmove, grid_mleave, grid_mpress, grid_mrelease,
  NULL, NULL, grid_draw, grid_destroy, grid_newflags, gridNetPress};

typedef struct cap_t  {
  int  threshold;
  uint  caps;
  int  player;
} cap_t;

static ButAction  cap_action = {
  NULL, NULL, NULL, NULL,
  NULL, NULL, cap_draw, cap_destroy, but_flags, NULL};


But  *but_grid_create(ButOut (*func)(But *but), ButWin *win, int layer,
		      int flags, bd_loc_t pos)  {
  grid_t  *grid;
  But  *but;

  grid = (grid_t *)wms_malloc(sizeof(grid_t));
  but = but_create(win, grid, &grid_action);
  but->uPacket = &(grid->pos);
  but->layer = layer;
  but->flags = flags;

  grid->callback = func;
  grid->pos = pos;
  grid->ptype = BD_EMPTYA;
  but_init(but);
  return(but);
}


void  but_grid_change(But *but, uint piece, bool marked)  {
  grid_t  *grid = but->iPacket;

  assert(but->action == &grid_action);
  grid->ptype = piece;
  grid->marked = marked;
  but_draw(but);
}


bd_loc_t  but_grid_loc(But *but)  {
  grid_t  *grid = but->iPacket;

  assert(but->action == &grid_action);
  return(grid->pos);
}


static ButOut  grid_destroy(But *but)  {
  grid_t  *grid = but->iPacket;

  wms_free(grid);
  return(0);
}


static void  grid_newflags(But *but, uint fl)  {
  uint  ofl = but->flags;

  but->flags = fl;
  if ((ofl & (BUT_TWITCHED|BUT_PRESSED|BUT_NETTWITCH|BUT_NETPRESS)) !=
      (fl & (BUT_TWITCHED|BUT_PRESSED|BUT_NETTWITCH|BUT_NETPRESS)))
    but_draw(but);
}


static ButOut  grid_mmove(But *but, int x, int y)  {
  uint  newflags = but->flags, retval = BUTOUT_CAUGHT;

  if (!(but->flags & BUT_PRESSABLE))
    return(BUTOUT_CAUGHT);
  if ((x < but->x) || (y < but->y) ||
      (x >= but->x + but->w) || (y >= but->y + but->h))  {
    newflags &= ~BUT_TWITCHED;
    if (newflags != but->flags)  {
      butEnv_setCursor(but->win->env, but, butCur_idle);
      but_newFlags(but, newflags);
    }
    if (!(newflags & BUT_LOCKED))
      retval = 0;
  } else  {
    newflags |= BUT_TWITCHED;
    if (newflags != but->flags)  {
      butEnv_setCursor(but->win->env, but, butCur_twitch);
      but_newFlags(but, newflags);
    }
  }
  return(retval);
}
      

static ButOut  grid_mleave(But *but)  {
  int  newflags = but->flags;

  newflags &= ~BUT_TWITCHED;
  if (newflags != but->flags)  {
    butEnv_setCursor(but->win->env, but, butCur_idle);
    but_newFlags(but, newflags);
  }
  return(BUTOUT_CAUGHT);
}
      

static ButOut  grid_mpress(But *but, int butnum, int x, int y)  {
  int  newflags = but->flags, retval = BUTOUT_CAUGHT;

  if (butnum == 1)
    newflags |= BUT_PRESSED|BUT_LOCKED;
  else
    retval |= BUTOUT_ERR;
  if (!(but->flags & BUT_PRESSED) && (newflags & BUT_PRESSED))
    snd_play(&pe_move_snd);
  if (newflags != but->flags)
    but_newFlags(but, newflags);
  return(retval);
}
      

static ButOut  grid_mrelease(But *but, int butnum, int x, int y)  {
  int  newflags = but->flags, retval = BUTOUT_CAUGHT;
  grid_t  *grid = but->iPacket;

  if (butnum != 1)
    return(retval);
  if (but->flags & BUT_TWITCHED)  {
    if (but->flags & BUT_PRESSED)  {
      if (but->id)
	butRnet_butSpecSend(but, NULL, 0);
      retval |= grid->callback(but);
    }
  } else
    retval |= BUTOUT_ERR;
  newflags &= ~(BUT_PRESSED|BUT_LOCKED);
  if (newflags != but->flags)
    but_newFlags(but, newflags);
  return(retval);
}
      

static void  grid_draw(But *but, int x,int y,int w,int h)  {
  grid_t  *grid = but->iPacket;
  uint  piece = grid->ptype;
  uint  inside, flags = but->flags;
  Display  *dpy;

  if (BD_EMPTYP(piece) && !(flags & (BUT_TWITCHED|BUT_NETTWITCH)))
    return;
  dpy = but->win->env->dpy;
  inside = (but->w + 10) / 20;
  if (inside == 0)
    inside = 1;
  if (BD_PLAYERP(piece))  {
    drawp(but->win, piece, but->x, but->y, but->w, FALSE);
    if (grid->marked)  {
      mark_piece(but->win, piece, but->x, but->y, but->w);
    }
  } else if ((flags & (BUT_PRESSED |BUT_NETPRESS)) &&
	     (flags & (BUT_TWITCHED|BUT_NETTWITCH)))  {
    drawp(but->win, BD_PLAYER(xio_turn), but->x, but->y, but->w, FALSE);
  } else if (flags & (BUT_TWITCHED|BUT_NETTWITCH))  {
    drawp(but->win, BD_WINNER(xio_turn), but->x, but->y, but->w, FALSE);
  }
}


static ButOut  gridNetPress(But *but, void *buf, int bufLen)  {
  grid_t  *grid = but->iPacket;

  assert(bufLen == 0);
  snd_play(&pe_move_snd);
  return(grid->callback(but));
}


But  *but_cap_create(ButWin *win, int layer, int flags, int capnum,
		     int player)  {
  But  *but;
  cap_t  *cnstore;
  
  cnstore = (cap_t *)wms_malloc(sizeof(cap_t));
  but = but_create(win, cnstore, &cap_action);
  but->layer = layer;
  but->flags = flags;

  cnstore->threshold = capnum;
  cnstore->caps = 0;
  cnstore->player = player;
  but_init(but);
  return(but);
}


void  but_cap_change(But *but, uint newcap)  {
  cap_t  *cap = but->iPacket;

  assert(but->action == &cap_action);
  cap->caps = newcap;
  but_draw(but);
}


static ButOut  cap_destroy(But *but)  {
  cap_t  *cap = but->iPacket;

  wms_free(cap);
  return(0);
}


static void  cap_draw(But *but, int x,int y,int w,int h)  {
  cap_t  *cap = but->iPacket;
  uint  piece;

  if (cap->threshold > cap->caps)
    return;
  if (xio_gameover && (cap->caps < 5))
    piece = BD_WINNER(cap->player);
  else
    piece = BD_PLAYER(cap->player);
  drawp(but->win, piece, but->x, but->y,        but->w, FALSE);
  drawp(but->win, piece, but->x, but->y+but->w, but->w, FALSE);
}


void  xio_draw_pl1char(void *packet, ButWin *win,
		       int x, int y, int w, int h)  {
  ButEnv  *env = butWin_env(win);

  drawp(win, BD_PLAYER(0), x,y,w, TRUE);
  butEnv_setXFg(env, BUT_FG);
}


void  xio_draw_pl2char(void *packet, ButWin *win,
		       int x, int y, int w, int h)  {
  ButEnv  *env = butWin_env(win);

  drawp(win, BD_PLAYER(1), x,y,w, TRUE);
  butEnv_setXFg(env, BUT_FG);
}


void  xio_draw_pl1markchar(void *packet, ButWin *win,
			   int x, int y, int w, int h)  {
  ButEnv  *env = butWin_env(win);

  drawp(win, BD_PLAYER(0), x,y,w, TRUE);
  mark_piece(win, BD_PLAYER(0), x,y,w);
  butEnv_setXFg(env, BUT_FG);
}


void  xio_draw_pl2markchar(void *packet, ButWin *win,
			   int x, int y, int w, int h)  {
  ButEnv  *env = butWin_env(win);

  drawp(win, BD_PLAYER(1), x,y,w, TRUE);
  mark_piece(win, BD_PLAYER(1), x,y,w);
  butEnv_setXFg(env, BUT_FG);
}


static void  drawp(ButWin *win, uint piece, int x, int y, int size,
		   bool black_border)  {
  int  lwidth;
  ButEnv  *env = win->env;
  Display  *dpy = env->dpy;
  GC  gc = env->gc;
  Xio  *xio;

  lwidth = (size + 12) / 25;
  if (lwidth > 0)  {
    xio = butEnv_packet(env);
    if (black_border && (xio->color == GrayScale))
      butEnv_setXFg(env, XIO_PIC_BLACK);
    else if ((piece == BD_WINNER(0)) || (piece == BD_WINNER(1)))
      butEnv_setXFg(env, XIO_PIC_DIMOUTLINE);
    else
      butEnv_setXFg(env, XIO_PIC_OUTLINE);
    XFillArc(dpy, win->win, gc, x,y, size,size, 0, 360*64);
  }
  switch(piece)  {
  case BD_PLAYER(0):
    butEnv_setXFg(env, XIO_PIC_PL1);
    break;
  case BD_PLAYER(1):
    butEnv_setXFg(env, XIO_PIC_PL2);
    break;
  case BD_WINNER(0):
    butEnv_setXFg(env, XIO_PIC_DIMPL1);
    break;
  case BD_WINNER(1):
    butEnv_setXFg(env, XIO_PIC_DIMPL2);
    break;
  }
  XFillArc(dpy, win->win, gc, x+lwidth,y+lwidth,
	   size-2*lwidth,size-2*lwidth, 0, 360*64);
}


static void  mark_piece(ButWin *win, uint piece, int x, int y, uint size)  {
  int  lwidth;
  uint  point, dent, center;
  XPoint  pset[9];
  
  if (piece == BD_PLAYER(0))
    butEnv_setXFg(win->env, XIO_PIC_MARK1);
  else if (piece == BD_PLAYER(1))
    butEnv_setXFg(win->env, XIO_PIC_MARK2);
  else
    return;

  lwidth = (size + 12) / 25;
  if (lwidth < 1)
    return;
  XSetLineAttributes(win->env->dpy, win->env->gc, lwidth, LineSolid,
		     CapProjecting, JoinMiter);
  
  /* point = (1*size+1)/3;
   * dent = (4*size+4)/9;
   */
  point = (size+4)/9;
  dent = (size+13)/27;
  center = size / 2;
  x += center;
  y += center;
  
  pset[0].x = x - point;
  pset[0].y = y - point;
  
  pset[1].x = x;
  pset[1].y = y - dent;
  
  pset[2].x = x + point;
  pset[2].y = pset[0].y;
  
  pset[3].x = x + dent;
  pset[3].y = y;
  
  pset[4].x = pset[2].x;
  pset[4].y = y + point;
  
  pset[5].x = x;
  pset[5].y = y + dent;
  
  pset[6].x = pset[0].x;
  pset[6].y = pset[4].y;
  
  pset[7].x = x - dent;
  pset[7].y = y;
  
  pset[8] = pset[0];
  
  XDrawLines(win->env->dpy, win->win, win->env->gc, pset, 9, CoordModeOrigin);
}


void  xio_draw_comp(void *packet, ButWin *win, int x, int y, int w, int h)  {
  XPoint  points[4];
  Display  *dpy = butEnv_dpy(butWin_env(win));
  Window  xwin = butWin_xwin(win);
  GC  gc = butEnv_gc(butWin_env(win));
  int  h2 = h/2, h3 = h/3, h12 = h/12;
  int  w6 = w/6, w9 = w/9;

  y += (h - (h12+h2+h12+h3)) / 2;
  points[0].x = x;
  points[0].y = y+h12+h2+h3;
  points[1].x = x+w6;
  points[1].y = y+h12+h2+h12;
  points[2].x = x+w-w6;
  points[2].y = y+h12+h2+h12;
  points[3].x = x+w;
  points[3].y = y+h12+h2+h3;
  XFillPolygon(dpy, xwin, gc, points, 4, Nonconvex, CoordModeOrigin);
  XFillRectangle(dpy, xwin, gc, x+w6,y+h12, w-2*w6,h2);
  butEnv_setXFg(butWin_env(win), BUT_PBG);
  XFillRectangle(dpy, xwin, gc, x+w6+w9,y+h12*2, w-2*w6-2*w9,h2-h12*2);
  butEnv_setXFg(butWin_env(win), BUT_FG);
}

#endif
