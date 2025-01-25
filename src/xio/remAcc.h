/*
 * src/xio/remAcc.h, part of Pente (game program)
 * Copyright (C) 1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Header file for routines to manage the remote accept/reject window.
 */

#ifndef  _REMACC_H_
#define  _REMACC_H_  1

struct XioMsgwin_struct;

/**********************************************************************
 * Types
 **********************************************************************/
typedef struct XioRemAcc_struct  {
  Xio  *xio;
  AbutMsg  *mwin;
  ButRnet  *net;
  MAGIC_STRUCT
} XioRemAcc;

/**********************************************************************
 * Functions available externally.
 **********************************************************************/
XioRemAcc  *xioRemAcc_create(ButRnet *net);
void  xioRemAcc_destroy(XioRemAcc *ra);

#endif  /* _REMACC_H_ */
