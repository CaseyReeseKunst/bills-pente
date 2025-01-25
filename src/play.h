/*
 * src/play.h, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

/* Exported features:
 *   pl_t
 *     Data type.  No user accessible components.
 *   void pl_init(pl_t *game)
 *     Initializes game to be a new game with an empty board and no moves
 *     made yet.
 *   uint pl_turn(pl_t *game)
 *     Returns the turn number, 0 or more.
 *   uint pl_player(pl_t *game)
 *     Returns the player number (0 or 1) who is to move next.
 *   uint pl_nmoves(pl_t *game)
 *     Returns the total number of moves that have been made so far.
 *   uint pl_maxmoves(pl_t *game)
 *     Returns the maximum number of moves that have been made so far.
 *     This is only diffeurent from pl_nmoves(game) if there have been undos.
 *   bd_loc_t pl_getmove(pl_t *game, uint movenum)
 *     Returns the move made at move movenum.
 *   bd_t *pl_board(pl_t *game)
 *     Returns the board at the current point in time.
 *   bool pl_movep(pl_t *game, bd_loc_t loc)
 *     Returns TRUE if loc is a valid move, or FALSE if it is not.
 *   void pl_move(pl_t *game, bd_loc_t loc)
 *     Makes the move specified, updating the board, the turn count, etc.
 *   void pl_undo(pl_t *game)
 *     Move the board back one move.
 *   void pl_redo(pl_t *game)
 *     Move the board forward one move.
 */

#ifndef _PL_H_
#define _PL_H_

#include "pente.h"
#include "board.h"

/*
 * DYNIX/ptx defines pl_t to be something else, so I have to undefine it
 *   here.  Ugh!
 */
#ifdef  pl_t
#undef  pl_t
#endif  /* pl_t */

#define  PL_MVNUM_MAX  (19*19 + 5*2*2)
typedef struct  {
	bd_t  board;
	bd_loc_t  movelist[PL_MVNUM_MAX];
	uint      move, maxmove;
} pl_t;
extern void  pl_init(pl_t *game);
extern uint  pl_turn(pl_t *game), pl_player(pl_t *game);
extern uint  pl_nmoves(pl_t *game);
extern uint  pl_maxmoves(pl_t *game);
extern bd_loc_t  pl_getmove(pl_t *game, uint movenum);
extern bd_t  *pl_board(pl_t *game);
extern bool  pl_movep(pl_t *game, bd_loc_t loc);
extern bool  pl_move(pl_t *game, bd_loc_t loc);
extern void  pl_undo(pl_t *game);
extern bool  pl_redo(pl_t *game);
extern void  pl_game_str(pl_t *game, char *str);
extern void  pl_str_game(const char *str, pl_t *game);

#endif
