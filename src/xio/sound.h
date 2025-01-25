/*
 * src/xio/sound.h, part of Pente (game program)
 * Copyright (C) 1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Header file for routines to manage the setup window.
 */

/**********************************************************************
 * Types
 **********************************************************************/

/* The nameless struct for sound is defined in xio.h. */

/**********************************************************************
 * Functions available externally.
 **********************************************************************/
extern void  xioSound_create(Xio *xio);
extern void  xioSound_check(Xio *xio);
extern ButOut  xioSound_toggle(But *but, bool press);
extern void  xioSound_setVol(Xio *xio, int newVol, bool setLast);
#define  xioSound_showErr(xio)  ((xio)->sound.err = FALSE)
