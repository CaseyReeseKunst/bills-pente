/*
 * src/xio/remote.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Routines to manage remote connections.
 */

#include <wms.h>

#if  X11_DISP

#include <netinet/in.h>
#include <netdb.h>
#include "../pente.h"
#include <but/but.h>
#include <but/net.h>
#include <but/checkbox.h>
#include <but/text.h>
#include <but/plain.h>
#include <but/box.h>
#include <but/textin.h>
#include <but/ctext.h>
#include <but/menu.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "xio.h"
#include "setup.h"
#include "remAcc.h"
#include "helpwin.h"
#include "sound.h"


typedef struct xioRemote_struct  {
  int  name;
  Xio  *xio;
  ButWin  *win;
  But  *box, *title;
  But  *mlab, *mtxtin;
  But  *plab, *ptxtin;
  But  *ok, *help, *cancel;
  MAGIC_STRUCT
} xioRemote;

static int  xio_r_connect(Xio  *xio);
static ButOut  newRemote(ButRnet *net);
static ButOut  remoteAccepted(ButRnet *net);
static ButOut  unmap(ButWin *win);
static ButOut  resize(ButWin *win);
static ButOut  destroy(ButWin *win);
static int  parsePort(Xio *xio, const char *portStr, bool reportErrors);
static ButOut  okPressed(But *but);
static ButOut  helpPressed(But *but);
static ButOut  cancelPressed(But *but);
static ButOut  newm(But *but, const char *value);
static ButOut  newp(But *but, const char *value);
static ButOut  newMenu(ButRnet *net, void *msg, int msgLen);
static ButOut  forceClose(ButRnet *net, const char *reason);
static ButOut  waitClosed(void *packet);
static ButOut  stopWaiting(But *but);
static void  createWaitingWindow(Xio *xio);
static ButOut  netInfo(ButRnet *net, void *msg, int msgLen);
static ButOut  remoteComp(ButRnet *net, void *msg, int msgLen);
static ButKey  soundkeys[] = {{XK_S, 0,0}, {XK_s, 0,0}, {0,0,0}};


#define  MAX_MACHNAME_SIZE  100
#define  MAX_PORTNAME_SIZE  50
static char  mName[MAX_MACHNAME_SIZE+1];
static char  pName[MAX_PORTNAME_SIZE+1];


bool  xio_r_init(Xio  *xio)  {
  char  errmsg[500];
  const char  *sock;
  int  portNum;
  ClpEntry  *eWant, *eHas;

#if  !HAVE_SOCKETS
  return(FALSE);
  /* NOTREACHED */
#endif
  xio_r_disable(xio);
  xioSetup_freshPortName(xio->swin);
  eWant = clp_lookup(pe_clp, "x.remote.wanted");
  eHas = clp_lookup(pe_clp, "x.remote.enabled");
  if (!clpEntry_getBool(eWant))
    return(FALSE);
  portNum = parsePort(xio, sock = clp_getStr(pe_clp, "x.remote.lsock"),
		      clpEntry_getBool(eHas));
  if (portNum < 0)
    return(FALSE);
  xio->lnet = butLnet_create(xio->env, portNum, xio, newRemote, netInfo,
			     forceClose);
  if (butLnet_error(xio->lnet))  {
    if ((butLnet_errNum(xio->lnet) == EACCES) &&
	(portNum < IPPORT_RESERVED))  {
      sprintf(errmsg, xioStr_errProtSocket[xio->lang], sock, IPPORT_RESERVED);
    } else if (butLnet_errNum(xio->lnet) == EADDRINUSE)  {
      sprintf(errmsg, xioStr_errSockInUse[xio->lang], sock);
    } else  {
      sprintf(errmsg, xioStr_errLSockGeneric[xio->lang], sock,
	      strerror(butLnet_errNum(xio->lnet)));
    }
    if (clpEntry_getBool(eHas))  {
      clpEntry_setBool(eHas, FALSE);
      abutMsg_winCreate(xio->abut, "Pente Error", errmsg);
      XBell(butEnv_dpy(xio->env), 0);
    }
    butLnet_destroy(xio->lnet);
    xio->lnet = NULL;
    xioSetup_newRemoteEnable(xio->swin, FALSE);
    return(FALSE);
  } else  {
    clpEntry_setBool(eHas, TRUE);
    xioSetup_newRemoteEnable(xio->swin, TRUE);
    return(TRUE);
  }
}


void  xio_r_disable(Xio *xio)  {
  if (xio->lnet)  {
    butLnet_destroy(xio->lnet);
    xio->lnet = NULL;
  }
}


static bool  xio_r_connect(Xio *xio)  {
  char  errmsg[500];
  const char  *sock, *mach;
  int  port;

  port = parsePort(xio, sock = clp_getStr(pe_clp, "x.remote.rsock"),
		   TRUE);
  if (port < 0)  {
    return(FALSE);
  }
  xio->rnet = butRnet_create(xio->env,
			     mach = clp_getStr(pe_clp, "x.remote.rname"),
			     port, xio, remoteAccepted, netInfo, forceClose);
  if (butRnet_error(xio->rnet))  {
    if (butRnet_errNum(xio->rnet) == ECONNREFUSED)  {
      sprintf(errmsg, xioStr_errRSockRefused[xio->lang], mach, sock);
    } else  {
      sprintf(errmsg, xioStr_errRSockGeneric[xio->lang],
	      strerror(butRnet_errNum(xio->rnet)), sock, mach);
    }
    abutMsg_winCreate(xio->abut, "Pente Error", errmsg);
    butRnet_destroy(xio->rnet, NULL);
    xio->rnet = NULL;
    return(FALSE);
  } else if (butRnet_lookupError(xio->rnet))  {
    if (butRnet_errNum(xio->rnet) == TRY_AGAIN)  {
      sprintf(errmsg, xioStr_errHostTemp[xio->lang], mach);
    } else  {
      sprintf(errmsg, xioStr_errHostPerm[xio->lang], mach);
    }
    abutMsg_winCreate(xio->abut, "Pente Error", errmsg);
    butRnet_destroy(xio->rnet, NULL);
    xio->rnet = NULL;
    return(FALSE);
  } else  {
    createWaitingWindow(xio);
    return(TRUE);
  }
}


static void  createWaitingWindow(Xio *xio)  {
  AbutMsgOpt  buts[1];

  buts[0].name = xioStr_cancel[xio->lang];
  buts[0].callback = stopWaiting;
  buts[0].packet = xio;
  buts[0].keyEq = NULL;
  xio->waitWindow = abutMsg_winOptCreate(xio->abut,
					 "Pente: Waiting for Accept",
					 xioStr_netWait[xio->lang],
					 waitClosed, xio, 1, buts);
}


static ButOut waitClosed(void *packet)  {
  Xio  *xio = packet;

  if (xio->rnet != NULL)  {
    if (!butRnet_valid(xio->rnet))  {
      butRnet_destroy(xio->rnet, xioStr_netClosed[xio->lang]);
      xio->rnet = NULL;
    }
  }
  xio->waitWindow = NULL;
  xioSetup_remoteRelabel(xio->swin);
  return(0);
}


static ButOut  stopWaiting(But *but)  {
  Xio  *xio = but_packet(but);

  abutMsg_destroy(xio->waitWindow, TRUE);
  return(0);
}
  

void  xio_r_create(Xio *xio)  {
  int  w, h;
  static ButKey  ka_ok[] = {{XK_Return, 0,0}, {XK_KP_Enter, 0,0}, {0,0,0}};
  static ButKey  helpkeys[] = {{XK_H, 0,0}, {XK_h, 0,0}, {XK_Help, 0,0},
                               {XK_question, 0,0}, {0,0,0}};
  static ButKey  cancelkeys[] = {{XK_Escape, 0,0}, {XK_Break, 0,0}, {0,0,0}};
  xioRemote  *rwin;
  ButWin  *win;
  char  geometry[100];
  But  *svTog;

#if  !HAVE_SOCKETS
  abutMsg_winCreate(xio->abut, "Pente Error", xioStr_noSockets[xio->lang]);
  return;
  /* NOTREACHED */
#endif
  if (xio->rwin != NULL)  {
    assert(MAGIC(xio->rwin));
    XRaiseWindow(butEnv_dpy(xio->env), butWin_xwin(xio->rwin->win));
    return;
  }
  xio->rwin = rwin = (xioRemote *)wms_malloc(sizeof(xioRemote));
  MAGIC_SET(rwin);
  rwin->xio = xio;
  xioSetup_remoteRelabel(xio->swin);
  w = (butWin_w(xio->mainwin)*2)/3;
  h = w/2;
  if (w == 0)
    return;  /* How did I get here? */
  sprintf(geometry, "%dx%d", w, h);
  rwin->win = win = butWin_create(rwin, xio->env, "Pente Networking",
				  w, h, unmap, NULL, resize, destroy);
  butWin_activate(win);
  rwin->box = butBoxFilled_create(rwin->win, 0, BUT_DRAWABLE);
  rwin->title = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_netConn[xio->lang], butText_center);
  butText_setFont(rwin->title, 1);
  rwin->mlab = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_machine[xio->lang], butText_left);
  rwin->mtxtin = butTextin_create(newm, xio, win, 1,
				  BUT_DRAWABLE|BUT_PRESSABLE,
				  clp_getStr(pe_clp, "x.remote.rname"),
				  MAX_MACHNAME_SIZE);
  rwin->plab = butText_create(win, 1, BUT_DRAWABLE,
			       xioStr_socket[xio->lang], butText_left);
  rwin->ptxtin = butTextin_create(newp, xio, win, 1,
				  BUT_DRAWABLE|BUT_PRESSABLE,
				  clp_getStr(pe_clp, "x.remote.rsock"),
				  MAX_PORTNAME_SIZE);
  rwin->help = butCt_create(helpPressed, xio, win, 1, BUT_DRAWABLE |
			    BUT_PRESSABLE, xioStr_help[xio->lang]);
  but_setKeys(rwin->help, helpkeys);
  rwin->ok = butCt_create(okPressed, xio, win, 1, BUT_DRAWABLE|BUT_PRESSABLE,
			  xioStr_ok[xio->lang]);
  but_setKeys(rwin->ok, ka_ok);
  rwin->cancel = butCt_create(cancelPressed, xio, win, 1, BUT_DRAWABLE |
			      BUT_PRESSABLE, xioStr_cancel[xio->lang]);
  but_setKeys(rwin->cancel, cancelkeys);
  butBoxFilled_setColors(rwin->box,
			 xio->litColor, xio->shadColor, xio->bgColor);
  svTog = butKeytrap_create(xioSound_toggle, xio, win,
			    BUT_DRAWABLE|BUT_PRESSABLE);
  but_setKeys(svTog, soundkeys);
}


static ButOut  unmap(ButWin *win)  {
  butWin_destroy(win);
  return(0);
}


static ButOut  newm(But *but, const char *value)  {
  Xio  *xio = but_packet(but);

  but_setFlags(xio->rwin->ptxtin, BUT_KEYED);
  return(0);
}


static ButOut  newp(But *but, const char *value)  {
  Xio  *xio = but_packet(but);

  but_setFlags(but, BUT_NOKEY);
  return(okPressed(xio->rwin->ok));
}


static ButOut  resize(ButWin *win)  {
  int  w, h;
  xioRemote  *rwin = butWin_packet(win);
  Xio  *xio = rwin->xio;
  int  butbw = butEnv_stdBw(xio->env);
  int  buth = but_h(xio->helpbut);
  int  rowh, border, mY, pY;
  int  mW, pW, maxW, butw;

  assert(MAGIC(rwin));
  w = butWin_w(win);
  h = butWin_h(win);
  rowh = h - 4*butbw - 2*buth;
  border = (rowh - 2*buth) / 3;
  mY = 2*butbw + buth + border;
  pY = mY + border + buth;

  but_resize(rwin->box, 0,0, w,h);

  butText_resize(rwin->title, w/2,butbw*2, buth);

  mW = butText_resize(rwin->mlab, butbw*2,mY, buth);
  pW = butText_resize(rwin->plab, butbw*2,pY, buth);
  if (mW > pW)
    maxW = mW;
  else
    maxW = pW;
  but_resize(rwin->mtxtin, butbw*3+maxW,mY, w-butbw*5-maxW,buth);
  but_resize(rwin->ptxtin, butbw*3+maxW,pY, w-butbw*5-maxW,buth);

  butw = (w - butbw*6)/3;
  but_resize(rwin->ok, butbw*2,h-buth-2*butbw, butw,buth);
  but_resize(rwin->help, butbw*3+butw,h-buth-2*butbw,
	     (w-butbw*6-butw*2),buth);
  but_resize(rwin->cancel, butbw*4+butw*2,h-buth-2*butbw,
	     butw,buth);
  return(0);
}


static ButOut  destroy(ButWin *win)  {
  Xio  *xio = butEnv_packet(butWin_env(win));
  xioRemote  *rwin = butWin_packet(win);

  assert(MAGIC(rwin));
  MAGIC_UNSET(rwin);
  wms_free(rwin);
  xio->rwin = NULL;
  xioSetup_remoteRelabel(xio->swin);
  return(0);
}
  

static ButOut  newRemote(ButRnet *net)  {
  Xio  *xio = butRnet_packet(net);

  if (strncmp(butRnet_protocol(net), "pente", 5))
    butRnet_destroy(net, "Wrong protocol; expecting pente");
  else if (xio->rnet)
    butRnet_destroy(net, "Remote user is already playing network pente "
		     "against another player");
  else  {
    xio->rnet = net;
    xioRemAcc_create(net);
  }
  return(0);
}


static ButOut  remoteAccepted(ButRnet *net)  {
  Xio  *xio = butRnet_packet(net);
  char  msg[2];

  if (strncmp(butRnet_protocol(net), "pente", 5))  {
    butRnet_destroy(net, "Wrong protocol; expecting pente");
    xio->rnet = NULL;
  }
  abutMsg_destroy(xio->waitWindow, TRUE);
  xio->waitWindow = NULL;
  msg[0] = '0' + butMenu_get(xio->menu1);
  msg[1] = '0' + butMenu_get(xio->menu2);
  butRnet_send(net, msg, 2);
  xio_stopped = FALSE;
  xio_paused = FALSE;
  xio_moveSelected = PE_RESTART;
  xioRemote_newState(xio);
  return(BUTOUT_STOPWAIT);
}


/*
 * Returns -1 if it's a bogus socket.
 */
static int  parsePort(Xio *xio, const char *portStr, bool reportErrors)  {
  int  port;
  bool  err;

  port = wms_atoi(portStr, &err);
  if (err || (port > 65535) || (port < 1))  {
    char  msg[300];

    if (reportErrors)  {
      sprintf(msg, xioStr_errSockNumInvalid[xio->lang], portStr, 65535);
      abutMsg_winCreate(xio->abut, "Pente Error", msg);
    }
    return(-1);
  } else  {
    return(port);
  }
}


static ButOut  okPressed(But *but)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));
  xioRemote  *rwin = xio->rwin;

  but_setFlags(rwin->mtxtin, BUT_NOKEY);
  but_setFlags(rwin->ptxtin, BUT_NOKEY);
  strcpy(mName, butTextin_get(rwin->mtxtin));
  clp_setStr(pe_clp, "x.remote.rname", mName);
  strcpy(pName, butTextin_get(rwin->ptxtin));
  clp_setStr(pe_clp, "x.remote.rsock", pName);;
  butWin_destroy(but_win(but));
  if (xio_r_connect(xio))
    return(0);
  else
    return(BUTOUT_ERR);
}
  

static ButOut  helpPressed(But *but)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(but)));

  xio->hwin = xioHelp_create(xio, 3);
  return(0);
}
  

static ButOut  cancelPressed(But *but)  {
  butWin_destroy(but_win(but));
  return(0);
}
  

static ButOut  forceClose(ButRnet *net, const char *reason)  {
  Xio  *xio;
  char  msgstr[1000];

  xio = butRnet_packet(net);
  sprintf(msgstr, xioStr_netDisconnect[xio->lang], butRnet_who(net), reason);
  abutMsg_winCreate(xio->abut, "Pente: Network closed", msgstr);
  if (xio->waitWindow != NULL)  {
    abutMsg_destroy(xio->waitWindow, TRUE);
    xio->waitWindow = NULL;
  }
  if (xio->remAcc != NULL)  {
    xioRemAcc_destroy(xio->remAcc);
    xio->remAcc = NULL;
  }
  xio->rnet = NULL;
  if (xio->waitWindow != NULL)
    abutMsg_destroy(xio->waitWindow, TRUE);
  xio->remoteEnd = FALSE;
  xioRemote_newState(xio);
  xioSetup_remoteRelabel(xio->swin);
  return(0);
}


void  xioRemote_newState(Xio *xio)  {
  const char  *optlist[10];

  if (xio->remoteEnd)  {
    optlist[0] = xioStr_remote[xio->lang];
    optlist[1] = xioStr_human[xio->lang];
  } else  {
    optlist[0] = xioStr_human[xio->lang];
    optlist[1] = xioStr_remote[xio->lang];
  }
  optlist[2] = BUTMENU_OLBREAK;
  optlist[3] = xioStr_comp1[xio->lang];
  optlist[4] = xioStr_comp2[xio->lang];
  optlist[5] = xioStr_comp3[xio->lang];
  optlist[6] = xioStr_comp4[xio->lang];
  optlist[7] = xioStr_comp5[xio->lang];
  optlist[8] = xioStr_comp6[xio->lang];
  optlist[9] = BUTMENU_OLEND;
  butMenu_setText(xio->menu1, NULL, optlist, BUT_NOCHANGE);
  butMenu_setText(xio->menu2, NULL, optlist, BUT_NOCHANGE);
}


void  xioRemote_destroy(Xio *xio)  {
  butWin_destroy(xio->rwin->win);
}


static ButOut  netInfo(ButRnet *net, void *msg, int msgLen)  {
  if (msgLen == 2)
    return(newMenu(net, msg, msgLen));
  else  {
    assert(msgLen == sizeof(XioCompBroadcast));
    return(remoteComp(net, msg, msgLen));
  }
}


static ButOut  newMenu(ButRnet *net, void *msg, int msgLen)  {
  Xio  *xio = butRnet_packet(net);
  char  *cMsg = msg;

  xio->remoteEnd = TRUE;
  butMenu_set(xio->menu1, cMsg[0] - '0');
  butMenu_set(xio->menu2, cMsg[1] - '0');
  xio_change_menu(xio->menu1, cMsg[0] - '0');
  xio_change_menu(xio->menu2, cMsg[1] - '0');
  xio_stopped = FALSE;
  xio_paused = FALSE;
  xio_moveSelected = PE_RESTART;
  xioRemote_newState(xio);
  return(BUTOUT_STOPWAIT);
}


static ButOut  remoteComp(ButRnet *net, void *msg, int msgLen)  {
  XioCompBroadcast  *cm = msg;
  bd_loc_t  pm;
  Xio  *xio = butRnet_packet(net);

  pm = (bd_loc_t)stdInt32_int(cm->pos);
  if (stdInt32_int(cm->think))  {
    xio_think(pm, stdInt32_int(cm->player));
    return(0);
  } else  {
    if (xio->markpos != pm)  {
      xio_moveSelected = pm;
      return(BUTOUT_STOPWAIT);
    } else
      return(0);
  }
}


void  xioRemote_comp(Xio *xio, bool think, bd_loc_t loc, int player)  {
  XioCompBroadcast  cb;

  cb.think = int_stdInt32((int)think);
  cb.pos = int_stdInt32((int)loc);
  cb.player = int_stdInt32((int)player);
  butRnet_send(xio->rnet, &cb, sizeof(cb));
}


#endif
