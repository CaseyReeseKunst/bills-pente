/*
 * $Source: /home/wms/cvs/pente/src/pente.c,v $
 * $Revision: 1.2 $
 * $Date: 2001/08/01 00:01:45 $
 *
 * src/pente.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include "pente.h"

static void  stealth(void);
static void  play_pente(int level[BD_MAXPLAYERS],
			uint init(int level[BD_MAXPLAYERS], uint winner,
				  pl_t *game),
			void draw(pl_t *game, bool forced),
			bd_loc_t selectmove(pl_t *game, int compfd),
			void  show_think(bd_loc_t pos, uint player),
			pl_t *game, bd_loc_t firstmove,
			int  from_c, int to_c);

static pl_t  game;

Clp  *pe_clp;
Rnd  *pe_rnd;

static struct  {
  char  *name;
  uint  (*init)(int level[BD_MAXPLAYERS], uint winner, pl_t *game);
  void  (*draw)(pl_t *game, bool forced);
  bd_loc_t  (*selectmove)(pl_t *game, int compfd);
  void  (*show_think)(bd_loc_t pos, uint player);
} disp[] = {
#if  X11_DISP
  {"X11", xio_init,  xio_draw,  xio_selectmove, xio_think},
#endif
#if  CURSES_DISP
  {"curses", cio_init,  cio_draw,  cio_selectmove, cio_think},
#endif
  {"text", text_init, text_draw, text_selectmove, text_think},
  {NULL, NULL, NULL}};

static ClpSetup  clpVars[] = {
  CLPSETUP_MSG("Pente " VERSION " by William Shubert - " DATE),
  CLPSETUP_MSG(""),
  {"VersionNumber", VERSION, NULL, 0, NULL},
  {"version,-version", "f", "Print version information",
     CLPSETUP_BOOL|CLPSETUP_NOSAVE, NULL},
  {"showthink", "t", "Show computer thinking", CLPSETUP_BOOL, NULL},
  CLPSETUP_MSG(""),
  CLPSETUP_MSG("Display type options:"),
#if  X11_DISP
  {"X11,x11", "f", "X11", CLPSETUP_BOOL|CLPSETUP_SHOWBOOL|CLPSETUP_NOSAVE,
     NULL},
#endif
#if  CURSES_DISP
  {"curses", "f", "Curses", CLPSETUP_BOOL|CLPSETUP_SHOWBOOL|CLPSETUP_NOSAVE,
     NULL},
#endif
  {"text", "f", "Plain text", CLPSETUP_BOOL|CLPSETUP_SHOWBOOL|CLPSETUP_NOSAVE,
     NULL},
#if  X11_DISP
  CLPSETUP_MSG(""),
  CLPSETUP_MSG("X11 Switches:"),
  {"name", "pente", "Name to use for X resource database", CLPSETUP_NOSAVE,
     NULL},
  {"adfile", "~/.pente.ad", "Set name of ad file", CLPSETUP_NOSAVE, NULL},
  {"iconic", "f", "Start up as an icon", CLPSETUP_SHOWBOOL|CLPSETUP_BOOL,
     NULL},
  {"geometry", "430x430", "Starting geometry", 0, NULL},
  {"icongeom", "63x63", "Icon geometry", 0, NULL},
  {"display", NULL, "Display to use", 0, NULL},
  {"color", "t", "Use color if possible", CLPSETUP_SHOWBOOL|CLPSETUP_BOOL,
     NULL},
  {"wasColor", "t", NULL, CLPSETUP_BOOL, NULL},
  {"stealth", "t", "Stealth mode", CLPSETUP_SHOWBOOL|CLPSETUP_BOOL, NULL},
  {"language", "English", "Language", 0, NULL},
#if  SND_AVAILABLE
  {"volume", "75", "Sound volume, 0..100", 0, NULL},
  {"volumeLast", "75", NULL, 0, NULL},
  {"silent", "f", "Same as \"-volume 0\"", CLPSETUP_BOOL, NULL},
  {"soundErr", "f", NULL, CLPSETUP_BOOL, NULL},
#else  /* !SND_AVAILABLE */
  {"volume", "0", NULL, 0, NULL},
  {"volumeLast", "0", NULL, 0, NULL},
  {"silent", "f", NULL, CLPSETUP_BOOL, NULL},
  {"soundErr", "t", NULL, CLPSETUP_BOOL, NULL},
#endif  /* SND_AVAILABLE */
  {"x.setup", "f", NULL, 0, NULL},
  {"x.setup.geom", "", NULL, 0, NULL},
  {"x.help", "f", NULL, 0, NULL},
  {"x.help.geom", "", NULL, 0, NULL},
  {"x.help.page", "0", NULL, 0, NULL},
  {"x.help.bookmark", "0.0", NULL, 0, NULL},
  {"x.remote.wanted", "t", NULL, 0, NULL},
  {"x.remote.enabled", "t", NULL, 0, NULL},
  {"x.remote.lsock", "15023", NULL, 0, NULL},
  {"x.remote.rsock", "15023", NULL, 0, NULL},
  {"x.remote.rname", "", NULL, 0, NULL},
#endif
  {"perf", "f", NULL, CLPSETUP_BOOL, NULL},

  {"player1", "0", NULL, 0, NULL},
  {"player2", "3", NULL, 0, NULL},
  {"board", NULL, NULL, 0, NULL},

  CLPSETUP_END};

static int  play_level[BD_MAXPLAYERS] = {3, 0};


int  main(int argc, char *argv[])  {
  int  tnum, defnum;
  bd_loc_t  resp;
  int  from_c, to_c;
    
  pe_clp = clp_create(clpVars);

  if ((argc = clp_rCmdline(pe_clp, argv)) == CLP_ARGS_NOGOOD)  {
    exit(1);
  }
  play_level[0] = clp_getInt(pe_clp, "player1");
  play_level[1] = clp_getInt(pe_clp, "player2");
  
  if (clp_getBool(pe_clp, "version"))  {
    printf("Pente " VERSION " by William Shubert (wms@igoweb.org) "
	   DATE "\n");
    exit(0);
  }

  if (clp_getBool(pe_clp, "perf"))  {
    /* Analyze the performance of pente...have the computer play itself
     *   at level 5, with a known random seed, to get the same game
     *   every time.  Time this on different machines to determine
     *   how fast that machine is.
     */
    bd_loc_t  loc;
    bool  game_over;

    pe_rnd = rnd_create(0);
    pl_init(&game);
    do  {
      loc = comp_noSpawnGetMove(&game, 5);
      game_over = pl_move(&game, loc);
    } while (!game_over);
    if (pl_nmoves(&game) != 34)  {
      fprintf(stderr,
	      "pente: Timing invalid.  Game play was not deterministic.  "
	      "%d moves.\n", pl_nmoves(&game));
      exit(1);
    }
    printf("pente: Timed game was %d moves long.\n", pl_nmoves(&game));
    exit(0);
  }
  
  for (tnum = 0, defnum = 0;  disp[tnum].init;  ++tnum)  {
    if (clp_getBool(pe_clp, disp[tnum].name))
      ++defnum;
  }
  
  pe_rnd = rnd_create(time(NULL) ^ getpid());

#ifdef  DEBUG
#define  STEALTH_VALID  (!(DEBUG))
#else
#define  STEALTH_VALID  1
#endif
  /*
   * It's really more convient, IMHO, to do the stealth in the X11 code.
   * But, alas, doing stealth after spawning comp causes errors on some
   *   operating systems.  :-(
   */
#if  X11_DISP
  if (clp_getBool(pe_clp, "stealth") && STEALTH_VALID &&
      ((defnum == 0) || clp_getBool(pe_clp, "x11")))
    stealth();
#endif
  comp_spawn(&from_c, &to_c);
  pl_init(&game);
  
  switch(defnum)  {
  case 0:
    for (tnum = 0;  disp[tnum].init;  ++tnum)  {
      resp = disp[tnum].init(play_level, BD_MAXPLAYERS, &game);
      if (resp != PE_CANTPLAY)  {
	play_pente(play_level, disp[tnum].init, disp[tnum].draw,
		   disp[tnum].selectmove, disp[tnum].show_think, &game, resp,
		   from_c, to_c);
	break;
      } else  {
	fprintf(stderr, "Pente: Could not start up %s display.\n"
		"       Other displays are available with these commands:\n",
		disp[tnum].name);
	for (++tnum;  disp[tnum].name != NULL;  ++tnum)  {
	  fprintf(stderr, "         pente -%s\n",
		  disp[tnum].name);
	}
	exit(1);
      }
    }
    if (!disp[tnum].init)  {
      fprintf(stderr, "pente: Error - Init failed.\n");
      exit(1);
    }
    break;
  case 1:
    for (tnum = 0;  !clp_getBool(pe_clp, disp[tnum].name);
	 ++tnum);
    resp = disp[tnum].init(play_level, BD_MAXPLAYERS, &game);
    if (resp != PE_CANTPLAY)  {
      play_pente(play_level, disp[tnum].init, disp[tnum].draw,
		 disp[tnum].selectmove, disp[tnum].show_think, &game, resp,
		 from_c, to_c);
    } else  {
      fprintf(stderr, "pente: Display type \"%s\" failed.\n",
	      disp[tnum].name);
      exit(1);
    }
    break;
  default:
    fprintf(stderr, "pente: At most one display type can be specified.\n");
    exit(1);
  }
  
  exit(0);
}


static void  play_pente(int level[BD_MAXPLAYERS],
			uint init(int level[BD_MAXPLAYERS], uint winner,
				  pl_t *game),
			void draw(pl_t *game, bool forced),
			bd_loc_t selectmove(pl_t *game, int compfd),
			void show_think(bd_loc_t pos, uint player),
			pl_t *game, bd_loc_t firstmove,
			int  from_c, int to_c)  {
  bd_loc_t  loc = firstmove;
  bool  game_over = FALSE, paused = FALSE;
  uint  turn_num, old_cap, player = pl_player(game);
  char  gameStr[4*(19*19+20)];
  
  for (;;)  {
    switch(loc)  {
    case PE_CONTINUE:
      paused = FALSE;
      break;
    case PE_UNDO:
      player ^= 1;
      paused = TRUE;
      game_over = FALSE;
      pl_undo(game);
      draw(game, TRUE);
      break;
    case PE_REDO:
      paused = TRUE;
      old_cap = pl_board(game)->captures[player];
      game_over = pl_redo(game);
      draw(game, TRUE);
      while (old_cap < pl_board(game)->captures[player])  {
	snd_play(&pe_capture_snd);
	++old_cap;
      }
      player ^= 1;
      if (game_over)
	snd_play(&pe_gameover_snd);
      break;
    case PE_PAUSE:
      paused = TRUE;
      break;
    case PE_RESTART:
      pl_init(game);
      player = pl_player(game);
      turn_num = pl_turn(game);
      paused = FALSE;
      game_over = FALSE;
      break;
    default:
      if (paused)
	paused = FALSE;
      assert(pl_movep(game, loc));
      if (!pl_movep(game, loc))  {
	fprintf(stderr, "pente: Illegal move reported.\n");
	exit(1);
      }
      old_cap = pl_board(game)->captures[player];
      game_over = pl_move(game, loc);
      pl_game_str(game, gameStr);
      clp_setStr(pe_clp, "board", gameStr);
      while (old_cap < pl_board(game)->captures[player])  {
	snd_play(&pe_capture_snd);
	++old_cap;
      }
      if (game_over)
	snd_play(&pe_gameover_snd);
      player ^= 1;
      break;
    }
    draw(game, (game_over || paused ||
		((level[player^1] == 0) || (level[player] != 0))));
    if (game_over || paused)
      loc = init(level, player^1, game);
    else  {
      if (level[player] < 1)  {
	loc = selectmove(game, from_c);
      } else  {
	comp_start(game, level[player], to_c);
	do  {
	  loc = selectmove(game, from_c);
	  if (loc == PE_COMP)  {
	    loc = comp_getmove(from_c, show_think, player);
	  } else  {
	    comp_reset(&from_c, &to_c);
	  }
	} while (loc == PE_COMP);
      }
    }
  }
}


void  pe_output_vars(void)  {
#if  X11_DISP
  clp_wFile(pe_clp, clp_getStr(pe_clp, "adfile"),
	    clp_getStr(pe_clp, "name"));
#else
  clp_wFile(pe_clp, "~/.pente.ad", "pente");
#endif  
}


static void  stealth(void)  {
  int pid;
#ifdef  TIOCNOTTY
  int  tty;
#endif
  
  pid = fork();
  if (pid < 0)  {
    perror("pente: fork() failed");
    return;
  } else if (pid > 0)
    /* Parent just exits. */
    exit(0);
  /* Go stealth (ditch our controlling tty). */
#ifdef  TIOCNOTTY
  tty = open("/dev/tty", 0);
  if (tty < 0)
    return;
  ioctl(tty, TIOCNOTTY, 0);
  close(tty);
#endif
}
