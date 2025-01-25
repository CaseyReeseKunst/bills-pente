/*
 * src/xio/sound.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Routines to manage the sounds
 */

#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "../pente.h"
#include "xio.h"
#include "setup.h"
#include "sound.h"


static void  show_snd_error(Xio *xio);


void  xioSound_create(Xio *xio)  {
  static int  newSound = TRUE;

  xio->sound.ceVol = clp_lookup(pe_clp, "volume");
  xio->sound.ceOldVol = clp_lookup(pe_clp, "volumeLast");
  xio->sound.ceSilent = clp_lookup(pe_clp, "silent");
  xio->sound.ceErr = clp_lookup(pe_clp, "soundErr");

  xio->sound.volume = clpEntry_getInt(xio->sound.ceVol);
  if (newSound)  {
    newSound = FALSE;
    if (clp_where(pe_clp, "volume") == clpWhere_cmdline)  {
      xio->sound.volume = xio->sound.volume * SND_MAXVOL / 100;
    } else if ((xio->rcMajor == 2) && (xio->rcMinor == 0) &&
	       (xio->rcBugFix < 3))  {
      xio->sound.volume = xio->sound.volume * SND_MAXVOL / 261;
    }
  }
  if (xio->sound.volume > SND_MAXVOL)
    xio->sound.volume = SND_MAXVOL;
  if (xio->sound.volume < 0)
    xio->sound.volume = 0;
  xio->sound.silent = clpEntry_getBool(xio->sound.ceSilent);
  xio->sound.oldVolume = clpEntry_getInt(xio->sound.ceOldVol);
  if (!xio->sound.volume && xio->sound.oldVolume)
    xio->sound.volume = xio->sound.oldVolume;
  if (xio->sound.silent)
    xio->sound.volume = 0;
  xio->sound.err = clpEntry_getBool(xio->sound.ceErr);
  snd_init(sndState_off, xio->sound.volume);
  xioSound_check(xio);
}


void  xioSound_check(Xio *xio)  {
  static SndState  oldState = sndState_off;
  SndState  newState;
  SndInit  err;
  
  if (xio->openWindows)  {
    newState = sndState_fullOpen;
  } else
    newState = sndState_partOpen;
  if (newState != oldState)  {
    if (!xio->sound.volume && !xio->sound.silent && xio->sound.oldVolume)  {
      clpEntry_setInt(xio->sound.ceVol,
		      xio->sound.volume = xio->sound.oldVolume);
      xio->sound.err = TRUE;
    }
    oldState = newState;
    err = snd_init(newState, -1);
    if (err)  {
      show_snd_error(xio);
    } else  {
      if (xio->sound.err)
	clpEntry_setBool(xio->sound.ceErr, xio->sound.err = FALSE);
      if (xio->sound.volume)
	clpEntry_setInt(xio->sound.ceOldVol,
			xio->sound.oldVolume = xio->sound.volume);
    }
  }
}


ButOut  xioSound_toggle(But *but, bool press)  {
  Xio  *xio = but_packet(but);

  if (!press)
    return(0);
  xioSound_showErr(xio);
  clpEntry_setBool(xio->sound.ceSilent, xio->sound.silent = xio->sound.volume);
  if (xio->sound.silent)  {
    if (xio->sound.volume > 0)
      clpEntry_setInt(xio->sound.ceOldVol,
		      xio->sound.oldVolume = xio->sound.volume);
    clpEntry_setInt(xio->sound.ceVol, xio->sound.volume = 0);
    snd_init(sndState_oldState, 0);
    xioSetup_newVol(xio->swin, 0);
  } else  {
    xio->sound.volume = xio->sound.oldVolume;
    if (snd_init(sndState_oldState, xio->sound.volume) != sndInit_ok)  {
      show_snd_error(xio);
    } else  {
      if (xio->sound.err)
	clpEntry_setBool(xio->sound.ceErr, xio->sound.err = FALSE);
      clpEntry_setInt(xio->sound.ceOldVol,
		      xio->sound.oldVolume = xio->sound.volume);
      xioSetup_newVol(xio->swin, xio->sound.volume);
      clpEntry_setInt(xio->sound.ceVol, xio->sound.volume);
    }
  }
  return(0);
}


void  xioSound_setVol(Xio *xio, int newVol, bool setLast)  {
  SndInit  err;

  err = snd_init(sndState_oldState, newVol);
  if (err)
    show_snd_error(xio);
  else  {
    if (xio->sound.err)
      clpEntry_setBool(xio->sound.ceErr, xio->sound.err = FALSE);
    if (setLast && (xio->sound.volume > 0))
      clpEntry_setInt(xio->sound.ceOldVol,
		      xio->sound.oldVolume = xio->sound.volume);
    clpEntry_setInt(xio->sound.ceVol, xio->sound.volume = newVol);
    if (xio->sound.silent && (newVol > 0))
      clpEntry_setBool(xio->sound.ceSilent, xio->sound.silent = FALSE);
  }
}
    


static void  show_snd_error(Xio *xio)  {
  clpEntry_setInt(xio->sound.ceVol, xio->sound.volume = 0);
  xioSetup_newVol(xio->swin, 0);
  /*
   * If we had an error LAST time, too, then just skip it and don't
   *   show it again.
   */
  if (!xio->sound.err)  {
    clpEntry_setBool(xio->sound.ceErr, xio->sound.err = TRUE);
    abutMsg_winCreate(xio->abut, "Pente Error", snd_error);
    XBell(butEnv_dpy(xio->env), 0);
  }
}

#endif  /* X11_DISP */
