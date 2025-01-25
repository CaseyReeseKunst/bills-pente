/*
 * src/pente.h, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#ifndef _PENTE_H_
#define _PENTE_H_

#include <wms.h>
#include <wms/snd.h>
#include <wms/clp.h>
#include <wms/rnd.h>
#include "board.h"
#include "play.h"

#define  MAXSTRLEN  80

/* In "pente.c" */
#define  PE_CONTINUE  (BD_LOC_MAX)
#define  PE_CANTPLAY  (BD_LOC_MAX+1)
#define  PE_UNDO      (BD_LOC_MAX+2)
#define  PE_REDO      (BD_LOC_MAX+3)
#define  PE_RESTART   (BD_LOC_MAX+4)
#define  PE_PAUSE     (BD_LOC_MAX+5)
#define  PE_COMP      (BD_LOC_MAX+6)

extern Clp  *pe_clp;
extern Rnd  *pe_rnd;
extern void  pe_output_vars(void);

extern bool  pe_perfmode;

/* In "pe_snd.c" */
extern Snd  pe_gameover_snd, pe_capture_snd, pe_move_snd;

/* In "xio/xio.c" */
#if  X11_DISP
extern bd_loc_t  xio_init(int *comp_level, uint winner, pl_t *game);
extern void  xio_draw(pl_t *game, bool forced);
extern bd_loc_t  xio_selectmove(pl_t *game, int compfd);
extern void  xio_think(bd_loc_t pos, uint player);
#endif  /* X11_DISP */

/* In "cio/cio.c" */
#ifdef  CURSES_DISP
extern bd_loc_t  cio_init(int *comp_level, uint winner, pl_t *game);
extern void  cio_draw(pl_t *game, bool forced);
extern bd_loc_t  cio_selectmove(pl_t *game, int compfd);
extern void  cio_think(bd_loc_t pos, uint player);
#endif

/* In "textio/textio.c" */
extern bd_loc_t  text_init(int *comp_level, uint winner, pl_t *game);
extern void  text_draw(pl_t *game, bool forced);
extern bd_loc_t  text_selectmove(pl_t *game, int compfd);
extern void  text_think(bd_loc_t pos, uint player);

/* In "comp/comp.c" */
extern void  comp_start(pl_t *game, uint level, int to_c);
extern bd_loc_t  comp_getmove(int from_c,
			      void think(bd_loc_t pos, uint player),
			      uint player);
extern bd_loc_t  comp_noSpawnGetMove(pl_t *game, int level);
extern void  comp_spawn(int *recv_from_c, int *send_to_c);
extern void  comp_reset(int *recv_from_c, int *send_to_c);

#endif
