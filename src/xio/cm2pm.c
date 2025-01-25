/*
 * src/xio/cm2pm.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include <wms.h>

#ifdef  X11_DISP

#include <but/but.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "../pente.h"
#include "xio.h"


Pixmap  cm2pm_chop(Xio *xio, uchar *cmap, uint cmapw,uint cmaph,
		   uint w,uint h, int pic0, int npic, int gridw, int buth)  {
  ButEnv  *env = xio->env;
  int  depth;
  Visual  *v;
  Display *dpy;
  Window  root;
  Pixmap  p;
  int  i, x, y, div, cmx, cmy;
  unsigned long  clist[256*3];
  XImage  *im;
  int  xaddmin, addstd, subextra, current, cadd;
  const int  bevel_start = 55, avg = 0, hi = 256, lo = 512;
  int  bevw = w / bevel_start;
  int  caph, capw;

  if (gridw > 0)  {
    caph = h - 2*buth;
    capw = (w - caph) / 2;
  } else  {
    caph = h;
    capw = 0;
  }

  div = (256*1024) / npic;
  for (i = 0;  i < 256;  ++i)  {
    clist[i] = butEnv_color(xio->env, pic0 + (i*1024)/div);
    clist[i+hi] = butEnv_color(xio->env, pic0 + (i*1024)/div + npic);
    clist[i+lo] = butEnv_color(xio->env, pic0 + (i*1024)/div + npic*2);
  }
  dpy = butEnv_dpy(env);
  v = DefaultVisual(dpy, DefaultScreen(dpy));
  depth = DefaultDepth(dpy, DefaultScreen(dpy));
  root = RootWindow(dpy, DefaultScreen(dpy));
  im = butEnv_imageCreate(env, w, h);
  xaddmin = cmapw / w;
  addstd = cmapw - (xaddmin * w);
  subextra = w;
  current = 0;
  for (y = 0;  y < h;  ++y)  {
    cmy = ((y * cmaph) / h) * cmapw;
    cmx = 0;
    for (x = 0;  x < w;  ++x)  {
      if (y < bevw)  {
	if (x < capw)
	  cadd = avg;
	else if (x < capw-y)
	  cadd = lo;
	else if (x < w-capw-y)
	  cadd = hi;
	else if (x < w-capw)
	  cadd = lo;
	else
	  cadd = avg;
      } else if (y < caph-bevw) {
	if (x < capw)
	  cadd = avg;
	else if (x < capw+bevw)
	  cadd = hi;
	else if (x < w-capw-bevw)
	  cadd = avg;
	else if (x < w-capw)
	  cadd = lo;
	else
	  cadd = avg;
      } else if (y < caph)  {
	if (x < capw)
	  cadd = avg;
	else if (x < caph-y)
	  cadd = lo;
	else if (x < capw+caph-y)
	  cadd = hi;
	else if (x < w-capw)
	  cadd = lo;
	else
	  cadd = avg;
      } else
	cadd = avg;
      XPutPixel(im, x, y, clist[cmap[cmx+cmy]+cadd]);
      cmx += xaddmin;
      current += addstd;
      if (current > 0)  {
	++cmx;
	current -= subextra;
      }
    }
  }
  p = XCreatePixmap(dpy, root, w,h, depth);
  XPutImage(dpy, p, env->gc, im, 0,0,0,0,w,h);
  butEnv_imageDestroy(im);
  return(p);
}


#endif
