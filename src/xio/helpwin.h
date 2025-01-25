/*
 * src/xio/helpwin.h, part of Pente (game program)
 * Copyright (C) 1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Header file for routines to manage the setup window.
 */


/**********************************************************************
 * Functions available externally.
 **********************************************************************/
/* Set pagenum to -1 if you don't care which page pops up. */
extern struct XioHelp_struct  *xioHelp_create(Xio *xio, int pageNum);
extern void  xioHelp_resize(struct XioHelp_struct *help);
extern void  xioHelp_newPlayers(struct XioHelp_struct *help);
extern void  xioHelp_storeInfo(struct XioHelp_struct *help);
