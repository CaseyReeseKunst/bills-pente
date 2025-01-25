#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <but/net.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "../pente.h"
#include "xio.h"
#include "remAcc.h"
#include "sound.h"
#include "setup.h"


static ButOut  winClosed(void *packet);
static ButOut  okPressed(But *but);
static ButOut  rejectPressed(But *but);


XioRemAcc  *xioRemAcc_create(ButRnet *net)  {
  XioRemAcc  *ra;
  Xio  *xio;
  static ButKey  ka_ok[] = {{XK_Return, 0,0}, {XK_KP_Enter, 0,0}, {0,0,0}};
  char  msg[1000];
  AbutMsgOpt  buts[2];

  xio = butRnet_packet(net);
  ra = wms_malloc(sizeof(XioRemAcc));
  MAGIC_SET(ra);
  xio->remAcc = ra;
  xioSetup_remoteRelabel(xio->swin);
  sprintf(msg, xioStr_netOffer[xio->lang], butRnet_who(net));
  ra->xio = xio;

  buts[0].name = xioStr_ok[xio->lang];
  buts[0].callback = okPressed;
  buts[0].packet = xio;
  buts[0].keyEq = ka_ok;

  buts[1].name = xioStr_reject[xio->lang];
  buts[1].callback = rejectPressed;
  buts[1].packet = xio;
  buts[1].keyEq = NULL;

  ra->mwin = abutMsg_winOptCreate(xio->abut, "Pente: Incoming Connection", msg,
				  winClosed, xio, 2, buts);
  ra->net = net;
  return(ra);
}


static ButOut  winClosed(void *packet)  {
  Xio  *xio = packet;
  XioRemAcc  *ra = xio->remAcc;

  if (ra != NULL)  {
    assert(MAGIC(ra));
    butRnet_destroy(xio->rnet, "Connection rejected by remote player");
    xio->remAcc = NULL;
    xio->rnet = NULL;
    MAGIC_UNSET(ra);
    wms_free(ra);
    xioSetup_remoteRelabel(xio->swin);
  }
  return(0);
}


static ButOut  okPressed(But *but)  {
  Xio  *xio = but_packet(but);
  XioRemAcc  *ra = xio->remAcc;

  assert(MAGIC(ra));
  xio->remAcc = NULL;
  abutMsg_destroy(ra->mwin, TRUE);
  butRnet_accept(ra->net);
  MAGIC_UNSET(ra);
  wms_free(ra);
  return(0);
}


static ButOut  rejectPressed(But *but)  {
  Xio  *xio = but_packet(but);

  abutMsg_destroy(xio->remAcc->mwin, TRUE);
  return(BUTOUT_STOPWAIT);
}


void  xioRemAcc_destroy(XioRemAcc *ra)  {
  assert(MAGIC(ra));
  abutMsg_destroy(ra->mwin, TRUE);
  MAGIC_UNSET(ra);
  wms_free(ra);
}


#endif  /* XIO_DISP */
