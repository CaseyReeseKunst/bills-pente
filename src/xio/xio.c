/*
 * src/xio/xio.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <but/menu.h>
#include <but/ctext.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include <wms/clp-x.h>

#include "../pente.h"
#include "xio.h"
#include "setup.h"
#include "sound.h"
#include "helpwin.h"

#ifndef  DEBUG
#define  DEBUG  0
#endif


bool   xio_gameover = TRUE, xio_paused = TRUE, xio_stopped = TRUE;
int    xio_turn = 2, xio_turn_num = 1, xio_mbutnum;
bd_loc_t  xio_lastmove = -1;
char  *xio_helpname;

static Xio  pxio;
static int  *level;
bool  xio_safe_shutdown;

static void  init_xio_struct(Xio *xio);
static void  enable_moving(pl_t *game);
static void  disable_moving(void);
static void  update_undo(pl_t *game);
static bool  setup_colors(Xio *xio, ButEnv *env);
static ButOut  xioCompRequest(void *packet, int fd);
#if  !DEBUG
static RETSIGTYPE  term_handler(int signum);
#endif

bd_loc_t  xio_moveSelected;
bool  xio_color = TRUE;

bd_loc_t  xio_init(int *comp_level, uint winner, pl_t *game)  {
  static bool  first_time = TRUE;
  bool  grid_on = FALSE;
  
  level = comp_level;
  xio_lastmove = -1;
  if (first_time)  {
    if (!xio_opendisp(&pxio))
      return(PE_CANTPLAY);
    pl_str_game(clp_getStr(pe_clp, "board"), game);
    bd_init(&pxio.iboard);
    XFlush(pxio.env->dpy);
#if  !DEBUG
    signal(SIGTERM, term_handler);  /* Catch these signals gracefully. */
    signal(SIGINT, term_handler);
    signal(SIGHUP, term_handler);
    signal(SIGPIPE, SIG_IGN);  /* Seems like you can't catch a SIGPIPE. */
#endif
  }
  xio_draw(game, TRUE);
  if (first_time)  {
    xio_stopped = TRUE;
    first_time = FALSE;
  }
  if (!xio_gameover &&
      (( pxio.remoteEnd && (comp_level[xio_turn] == -1)) ||
       (!pxio.remoteEnd && (comp_level[xio_turn] == -2))))  {
    grid_on = TRUE;
    enable_moving(game);
  }
  xio_moveSelected = PE_CANTPLAY;
  do  {
    butEnv_events(pxio.env);
    if (pxio.restart_x)  {
      xio_opendisp(&pxio);
      pxio.restart_x = FALSE;
    }
  } while (xio_moveSelected == PE_CANTPLAY);
  if (grid_on)
    disable_moving();
  return(xio_moveSelected);
}


bool  xio_opendisp(Xio *xio)  {
  static bool  first_time = TRUE;
  bool  enough_colors = TRUE;
  int  i, envinit;
  bool  showColorError;
  ClpEntry  *ce;

  ce = clp_lookup(pe_clp, "wasColor");
  if (!first_time)  {
    xio_safe_shutdown = TRUE;
    butEnv_destroy(xio->env);
    xio_safe_shutdown = FALSE;
  }
  /* Make sure the windows don't think that they're already open! */
  init_xio_struct(&pxio);
  if ((xio->env = butEnv_create("pente " VERSION,
				clp_getStr(pe_clp, "display"),
				xio_shutdown)) == NULL)
    return(FALSE);
  if (first_time)  {
    const char  *lang;
    int  parses;

    first_time = FALSE;
    clp_rXDefaults(pe_clp, butEnv_dpy(xio->env), clp_getStr(pe_clp, "name"));
    clp_rFile(pe_clp, clp_getStr(pe_clp, "adfile"));
    parses = sscanf(clp_getStr(pe_clp, "VersionNumber"),
		    "Pente %d.%d.%d", &xio->rcMajor, &xio->rcMinor,
		    &xio->rcBugFix);
    if (parses != 3)
      xio->rcMajor = xio->rcMinor = xio->rcBugFix = 0;
    clp_setStr(pe_clp, "VersionNumber", "Pente " VERSION);
    level[0] = clp_getInt(pe_clp, "player1");
    level[1] = clp_getInt(pe_clp, "player2");
    lang = clp_getStr(pe_clp, "language");
    for (i = 0;  (i < XIO_LANG_COUNT) &&
	 strcmp(lang, xioStr_langlist[0][i]);  ++i);
    if (i == XIO_LANG_COUNT)  {
      i = 0;
      fprintf(stderr, "Pente: Error; unknown language \"%s\".  Switching "
	      "to \"%s\".\n", lang, xioStr_langlist[0][i]);
    }
    xio->lang = i;
    if (level[0] == 0)
      level[0] = -2;
    if (level[1] == 0)
      level[1] = -2;
  }
  showColorError = (clpEntry_getBool(ce) && clp_getBool(pe_clp, "color"));
  envinit = butEnv_init(xio->env, xio, "Pente", xio_color &&
			clp_getBool(pe_clp, "color"));
  if (envinit == 3)
    xio->color = TrueColor;
  else if (envinit == 2)
    xio->color = PseudoColor;
  else
    xio->color = GrayScale;
  if (envinit == 1)
    /* Couldn't get the colors it wanted! */
    enough_colors = FALSE;
  if (!setup_colors(xio, xio->env))
    enough_colors = FALSE;
  if (!enough_colors)  {
    xio_color = FALSE;
    xio->color = GrayScale;
    if (xio_opendisp(xio))  {
      if (showColorError)  {
	abutMsg_winCreate(xio->abut, "Pente: Error",
			  xioStr_notEnoughColors[xio->lang]);
      }
      return(TRUE);
    } else
      return(FALSE);
  }
  clpEntry_setBool(ce, xio_color);
  butEnv_setChar(xio->env, 1.0, XIO_COMPCHAR, xio_draw_comp, NULL);
  butEnv_setChar(xio->env, 1.0, XIO_PL1CHAR, xio_draw_pl1char, NULL);
  butEnv_setChar(xio->env, 1.0, XIO_PL2CHAR, xio_draw_pl2char, NULL);
  butEnv_setChar(xio->env, 1.0, XIO_PL1MARKCHAR, xio_draw_pl1markchar,
		    NULL);
  butEnv_setChar(xio->env, 1.0, XIO_PL2MARKCHAR, xio_draw_pl2markchar,
		    NULL);
  xio->abut = abut_create(xio->env, xioStr_ok[xio->lang],
			  xioStr_cancel[xio->lang]);
  abut_setColors(xio->abut, xio->litColor, xio->shadColor, xio->bgColor);
  xio_m_create(xio, "Pente", clp_getStr(pe_clp, "geometry"),
	       clp_getBool(pe_clp, "iconic"), xio->color);
  xioSound_create(xio);
  butMenu_set(xio->menu1, level[0] + 2);
  butMenu_set(xio->menu2, level[1] + 2);
  if (clp_getBool(pe_clp, "x.setup"))  {
    xio->swin = xioSetup_create(xio);
  }
  xio->hwin = NULL;
  if (clp_getBool(pe_clp, "x.help"))  {
    xio->hwin = xioHelp_create(xio, -1);
  }
  if (xio->lnet == NULL)
    xio_r_init(xio);
  return(TRUE);
}


void  xio_draw(pl_t *game, bool forced)  {
  bd_t  *newboard = pl_board(game);
  int  i, oldturn;
  uint  piece;
  bd_loc_t  loc;
  bool  old_gameover = xio_gameover, gameover_new = FALSE;
  
  update_undo(game);
  xio_think(BD_LOC_MAX, 0);
  xio_gameover = newboard->gameover;
  if (xio_gameover != old_gameover)
    gameover_new = TRUE;
  if (!old_gameover && xio_gameover)
    butCt_setText(pxio.startbut, xioStr_start[pxio.lang]);
  oldturn = xio_turn_num;
  xio_turn_num = pl_turn(game)+1;
  for (loc = BD_LOC_MIN;  loc < BD_LOC_MAX;  ++loc)  {
    piece = newboard->grid[loc];
    if ((xio_gameover || xio_stopped) && BD_PLAYERP(piece))  {
      if (BD_WINNERP(piece))
	piece = BD_PLAYER(BD_OWNER(piece));
      else
	piece = BD_WINNER(BD_OWNER(piece));
    }
    if (pxio.iboard.grid[loc] != piece)  {
      pxio.iboard.grid[loc] = piece;
      but_grid_change(pxio.gridbuts[loc], pxio.iboard.grid[loc], FALSE);
      but_grid_change(pxio.iconbuts[loc], pxio.iboard.grid[loc], FALSE);
    }
  }
  if (pl_nmoves(game) >= 1)  {
    if (!xio_gameover)  {
      loc = pl_getmove(game, pl_nmoves(game) - 1);
      but_grid_change(pxio.gridbuts[loc], pxio.iboard.grid[loc], TRUE);
      pxio.markpos = loc;
      if (pxio.rnet && (level[xio_turn] > 0))  {
	xioRemote_comp(&pxio, FALSE, loc, xio_turn);
      }
    }
    if (pl_nmoves(game) >= 2)  {
      loc = pl_getmove(game, pl_nmoves(game) - 2);
      but_grid_change(pxio.gridbuts[loc], pxio.iboard.grid[loc], FALSE);
    }
  }
  if (xio_turn_num != oldturn)  {
    xio_m_set_turn(&pxio, xio_turn_num, oldturn);
  }
  
  if (gameover_new || (newboard->captures[0] != pxio.iboard.captures[0]))  {
    pxio.iboard.captures[0] = newboard->captures[0];
    for (i = 0;  i < 5;  ++i)  {
      but_cap_change(pxio.caps1[i], pxio.iboard.captures[0]);
    }
  }
  if (gameover_new || (newboard->captures[1] != pxio.iboard.captures[1]))  {
    pxio.iboard.captures[1] = newboard->captures[1];
    for (i = 0;  i < 5;  ++i)  {
      but_cap_change(pxio.caps2[i], pxio.iboard.captures[1]);
    }
  }
  if (xio_gameover)
    xio_turn = 2;
  else
    xio_turn = pl_player(game);
  XFlush(pxio.env->dpy);
}


bd_loc_t  xio_selectmove(pl_t *game, int compfd)  {
  int  movenum;
  bool  comp;
  
#if  DEBUG
  wms_alloc_stat();
#endif
  comp = (level[pl_player(game)] > 0);
  xio_gameover = FALSE;
  if ((pl_turn(game) == 0) && (pl_player(game) == 0))  {
    if (comp)
      return(PE_COMP);
    else  {
      snd_play(&pe_move_snd);
      return(bd_xy_loc(9,9));
    }
  }
  movenum = pl_nmoves(game);
  xio_turn = pl_player(game);
  xio_moveSelected = PE_CANTPLAY;
  do  {
    if (comp)  {
      if (!pxio.remoteEnd)  {
	butEnv_addFile(pxio.env, BUT_READFILE, compfd, &xio_moveSelected,
		       xioCompRequest);
      }
    } else  {
      if (( pxio.remoteEnd && (level[pl_player(game)] == -1)) ||
	  (!pxio.remoteEnd && (level[pl_player(game)] == -2)))
      enable_moving(game);
    }
    butEnv_events(pxio.env);
    if (comp)
      butEnv_rmFile(pxio.env, BUT_READFILE, compfd);
    if (pxio.restart_x)  {
      xio_opendisp(&pxio);
      pxio.restart_x = FALSE;
    }
  } while (xio_moveSelected == PE_CANTPLAY);
  disable_moving();
  if (pl_movep(game, xio_moveSelected))
    xio_turn ^= 1;
  return(xio_moveSelected);
}


static ButOut  xioCompRequest(void *packet, int fd)  {
  int  *msel = packet;

  *msel = PE_COMP;
  return(BUTOUT_STOPWAIT);
}


void  xio_think(bd_loc_t pos, uint player)  {
  static bd_loc_t  last_think = BD_LOC_MAX;
  static uint  last_player = 0;
  bool  drawn = FALSE;

  if (pos == PE_REDO)
    pos = last_think;
  if (player == -1)
    player = last_player;
  last_player = player;
  if (pxio.rnet && (level[player] > 0))
    xioRemote_comp(&pxio, TRUE, pos, player);
  if (last_think != BD_LOC_MAX)  {
    but_grid_change(pxio.gridbuts[last_think], BD_EMPTYA, FALSE);
    drawn = TRUE;
  }
  last_think = pos;
  if ((pos != BD_LOC_MAX) && clp_getBool(pe_clp, "showthink"))  {
    but_grid_change(pxio.gridbuts[pos], BD_WINNER(player), FALSE);
    drawn = TRUE;
  }
  if (drawn)
    XFlush(pxio.env->dpy);
}


static void  enable_moving(pl_t *game)  {
  int  x, y;
  bd_loc_t  loc;
  Xio  *xio = &pxio;
  
  if (!xio->env)
    return;
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)  {
      loc = bd_xy_loc(x, y);
      if (pl_movep(game, loc))  {
	but_setFlags(xio->gridbuts[loc], BUT_PRESSABLE|BUT_TWITCHABLE);
      }
    }
  }
}


static void  disable_moving(void)  {
  int  x, y;
  
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)  {
      but_setFlags(pxio.gridbuts[bd_xy_loc(x, y)], BUT_NOPRESS|BUT_NOTWITCH);
    }
  }
}


static void  update_undo(pl_t *game)  {
  static bool  undo_was_legal = FALSE, redo_was_legal = FALSE;
  
  if (pl_turn(game) >= 1)  {
    if (!undo_was_legal)  {
      but_setFlags(pxio.undobut, BUT_PRESSABLE|BUT_TWITCHABLE);
      undo_was_legal = TRUE;
    }
  } else if (undo_was_legal)  {
    but_setFlags(pxio.undobut, BUT_NOPRESS|BUT_NOTWITCH);
    undo_was_legal = FALSE;
  }
  
  if (pl_maxmoves(game) > pl_nmoves(game))  {
    if (!redo_was_legal)  {
      but_setFlags(pxio.redobut, BUT_PRESSABLE|BUT_TWITCHABLE);
      redo_was_legal = TRUE;
    }
  } else if (redo_was_legal)  {
    but_setFlags(pxio.redobut, BUT_NOPRESS|BUT_NOTWITCH);
    redo_was_legal = FALSE;
  }
}


#if  !DEBUG
static RETSIGTYPE  term_handler(int signum)  {
  xio_shutdown(NULL);
  exit(0);
}
#endif


ButOut  xio_press_undo(But *info)  {
  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  xio_moveSelected = PE_UNDO;
  xio_paused = TRUE;
  xio_stopped = FALSE;
  butCt_setText(pxio.startbut, xioStr_continue[pxio.lang]);
  return(BUTOUT_STOPWAIT);
}


ButOut  xio_press_move(But *info)  {
  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  if (xio_paused || xio_stopped)
    butCt_setText(pxio.startbut, xioStr_stop[pxio.lang]);
  xio_paused = FALSE;
  xio_stopped = FALSE;
  xio_moveSelected = but_grid_loc(info);
  return(BUTOUT_STOPWAIT);
}


ButOut  xio_press_redo(But *info)  {
  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  xio_moveSelected = PE_REDO;
  xio_paused = TRUE;
  xio_stopped = FALSE;
  butCt_setText(pxio.startbut, xioStr_continue[pxio.lang]);
  return(BUTOUT_STOPWAIT);
}


ButOut  xio_press_start(But *info)  {
  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  if (xio_gameover || xio_stopped)  {
    xio_stopped = FALSE;
    xio_paused = FALSE;
    xio_moveSelected = PE_RESTART;
    butCt_setText(info, xioStr_stop[pxio.lang]);
  } else if (xio_paused)  {
    xio_paused = FALSE;
    xio_moveSelected = PE_CONTINUE;
    butCt_setText(info, xioStr_stop[pxio.lang]);
  } else  {
    xio_moveSelected = PE_PAUSE;
    xio_stopped = TRUE;
    butCt_setText(info, xioStr_start[pxio.lang]);
  }
  return(BUTOUT_STOPWAIT);
}


ButOut  xio_press_help(But *info)  {
  Xio  *xio = butEnv_packet(butWin_env(but_win(info)));

  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  xio->hwin = xioHelp_create(butEnv_packet(butWin_env(but_win(info))), -1);
  return(BUTOUT_STOPWAIT);
}


ButOut  xio_press_setup(But *info)  {
  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  pxio.swin = xioSetup_create(&pxio);
  return(0);
}


ButOut  xio_press_quit(But *info)  {
  if (pxio.wait_nonicon)  {
    pxio.wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  xio_shutdown(NULL);
  return(0);
}


int  xio_shutdown(Display *dpy)  {
  char  geom[100], setupgeom[30];

  snd_deinit();
  sprintf(geom, "%dx%d+%d+%d",
	  butWin_w(pxio.mainwin), butWin_h(pxio.mainwin),
	  butWin_x(pxio.mainwin), butWin_y(pxio.mainwin));
  clp_setStr(pe_clp, "geometry", geom);
  if (pxio.swin)  {
    sprintf(setupgeom, "+%d+%d", butWin_x(xioSetup_win(pxio.swin)),
	    butWin_y(xioSetup_win(pxio.swin)));
    clp_setStr(pe_clp, "x.setup.geom", setupgeom);
  }
  xioHelp_storeInfo(pxio.hwin);
  pe_output_vars();
  if (dpy != NULL)  {
    fprintf(stderr, "pente: Exiting; IO error on X connection.\n");
    exit(1);
  }
  exit(0);
}


ButOut  xio_change_menu(But *info, int value)  {
  Xio  *xio = &pxio;
  int  l1, l2;

  l1 = butMenu_get(xio->menu1) - 2;
  l2 = butMenu_get(xio->menu2) - 2;
  if (xio->remoteEnd)  {
    /*
     * If we're the remote end, then we don't want our computer to swing
     *   into action (we use the other guy's computer), so we set the
     *   level to -3-level.  We don't want to just use -3, because then
     *   the tests (l1 == level[0]) and (l2 == level[1]) would give
     *   different results on the two ends of a network connection.
     */
    if (l1 > 0)
      l1 = -3 - l1;
    if (l2 > 0)
      l2 = -3 - l2;
  }
  /*
   * If the user set one to "remote" and there isn't a remote connection,
   *   then make one!
   */
  if ((((l1 == -1) && (info == xio->menu1)) ||
       ((l2 == -1) && (info == xio->menu2))) &&
      (xio->rnet == NULL) && (xio->waitWindow == NULL) &&
      (xio->remAcc == NULL) && (xio->rwin == NULL))
    xio_r_create(xio);
  if ((l1 == level[0]) && (l2 == level[1]))
    return(0);
  xio_moveSelected = PE_PAUSE;
  if (!xio_paused && !xio_gameover && !xio_stopped)  {
    xio_paused = TRUE;
    butCt_setText(pxio.startbut, xioStr_continue[pxio.lang]);
  }
  level[0] = l1;
  level[1] = l2;
  clp_setInt(pe_clp, "player1", l1);
  clp_setInt(pe_clp, "player2", l2);
  return(BUTOUT_STOPWAIT);
}


static bool  setup_colors(Xio *xio, ButEnv *env)  {
  ButColor  colorset[XIO_NPICS];
  ButColor  bd0, bd7, bdavg, black, white;
  int  i, num_shades;
  
  for (i = 0;  i < XIO_NPICS;  ++i)
    colorset[i].greyLevel = -1;
  black = butColor_create(0,0,0,0);
  white = butColor_create(255,255,255,16);
  bd0 = butColor_create(208,168,121, 8);
  bd7 = butColor_create(205,145, 82, 8);
  bdavg = butColor_mix(bd0,1, bd7,1);
    
  colorset[XIO_PIC_OUTLINE] = white;
  colorset[XIO_PIC_BLACK] = black;
  colorset[XIO_PIC_DIMOUTLINE] = butColor_mix(white,3, black,1);
  colorset[XIO_PIC_STAMP] = butColor_create(184,120,57, 4);
  if (xio->color == TrueColor)
    num_shades = 256;
  else
    num_shades = 8;
  for (i = 0;  i < num_shades;  ++i)  {
    colorset[XIO_PIC_BOARD(i)] = butColor_mix(bd0,i, bd7,num_shades-1-i);
    colorset[XIO_PIC_BOARD(i+num_shades)] =
      butColor_mix(colorset[XIO_PIC_BOARD(i)],4, white,1);
    colorset[XIO_PIC_BOARD(i+2*num_shades)] =
      butColor_mix(colorset[XIO_PIC_BOARD(i)],4, black,1);
  }
  colorset[XIO_PIC_PL1] = butColor_create(0,200,0, 0);
  colorset[XIO_PIC_PL2] = butColor_create(0,0,255, 16);
  colorset[XIO_PIC_DIMPL1] = butColor_mix(colorset[XIO_PIC_PL1],2, black,1);
  colorset[XIO_PIC_DIMPL1].greyLevel = 4;
  colorset[XIO_PIC_DIMPL2] = butColor_mix(colorset[XIO_PIC_PL2],2, black,1);
  colorset[XIO_PIC_DIMPL2].greyLevel = 12;
  colorset[XIO_PIC_BUT1FG] = butColor_create(0,0,0, 16);
  colorset[XIO_PIC_MARK1] = butColor_create(255,255,255, 16);
  colorset[XIO_PIC_MARK2] = butColor_create(255,255,255, 0);
  for (i = 0;  i < XIO_NPICS;  ++i)
    if (colorset[i].greyLevel != -1)  {
      /* The color was changed. */
      if (!butEnv_setColor(env, i, colorset[i]))  {
	return(FALSE);
      }
    }
  if (xio->color == PseudoColor)  {
    xio->bgColor = XIO_PIC_BOARD(4);
    xio->litColor = XIO_PIC_BOARD(8+7);
    xio->shadColor = XIO_PIC_BOARD(16);
  } else if (xio->color == TrueColor)  {
    xio->bgColor = XIO_PIC_BOARD(128);
    xio->litColor = XIO_PIC_BOARD(511);
    xio->shadColor = XIO_PIC_BOARD(512);
  } else  {  /* xio->color == GrayScale */
    xio->bgColor = BUT_BG;
    xio->litColor = BUT_LIT;
    xio->shadColor = BUT_SHAD;
  }
  return(TRUE);
}
     

static void  init_xio_struct(Xio *xio)  {
  xio->env = NULL;
  xio->mainwin = NULL;
  xio->remoteEnd = FALSE;
  xio->hwin = NULL;
  xio->swin = NULL;
  xio->tiwin = NULL;
  xio->wait_nonicon = FALSE;
  xio->restart_x = FALSE;
  xio->copbox = NULL;
  xio->openWindows = 0;
  xio->remAcc = NULL;
  xio->waitWindow = NULL;
}


#endif
