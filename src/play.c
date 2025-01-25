/*
 * src/play.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Functions used to play a game.  No concern for I/O is provided.
 */

#include "pente.h"


void  pl_init(pl_t *game)  {
  bd_init(&game->board);
  game->move = 0;
  game->maxmove = 0;
}


/* Returns TRUE if it is a valid move, FALSE otherwise. */
bool  pl_movep(pl_t *game, bd_loc_t loc)  {
  uint  turn = pl_turn(game), player = pl_player(game);
  
  if ((turn == 0) && (player == 0))
    return(loc == bd_xy_loc(9, 9));
  if ((loc > BD_GRIDSIZE) || !BD_EMPTYP(game->board.grid[loc]))
    return(FALSE);
  if ((turn == 1) && (player == 0))  {
    int  x, y;
    
    x = bd_loc_x(loc);
    y = bd_loc_y(loc);
    if ((x >= 9-2) && (x <= 9+2) && (y >= 9-2) && (y <= 9+2))
      return(FALSE);
  }
  return(TRUE);
}


/*
 * Returns TRUE if the move caused a win.
 */
bool  pl_move(pl_t *game, bd_loc_t loc)  {
  bd_t  *board = &(game->board);
  uint  player = pl_player(game);
  uint  captures = game->board.captures[player];
  uint  dir;
  int   dvec;
  bool  win = FALSE;
  
  game->movelist[game->move++] = loc;
  game->maxmove = game->move;
  board->grid[loc] = BD_PLAYER(player);
  for (dir = 0;  dir < 8;  ++dir)  {
    dvec = bd_dvec[dir];
    if (BD_PLAYERP(board->grid[loc+dvec]) &&
	(board->grid[loc+dvec] != BD_PLAYER(player)) &&
	(board->grid[loc+dvec] == board->grid[loc+dvec+dvec]) &&
	(board->grid[loc+dvec+dvec+dvec] == BD_PLAYER(player)))  {
      ++captures;
      board->grid[loc+dvec] = BD_EMPTYA;
      board->grid[loc+dvec+dvec] = BD_EMPTYA;
    }
  }
  board->captures[player] = captures;
  if (captures >= 5)
    win = TRUE;
  for (dir = 0;  dir < 4;  ++dir)  {
    int  run_length = 1;
    bd_loc_t  curloc;
    
    dvec = bd_dvec[dir];
    curloc = loc+dvec;
    while (board->grid[curloc] == BD_PLAYER(player))  {
      curloc += dvec;
      ++run_length;
    }
    curloc = loc-dvec;
    while (board->grid[curloc] == BD_PLAYER(player))  {
      curloc -= dvec;
      ++run_length;
    }
    if (run_length >= 5)  {
      win = TRUE;
      board->grid[loc] = BD_WINNER(player);
      curloc = loc+dvec;
      while (board->grid[curloc] == BD_PLAYER(player))  {
	board->grid[curloc] = BD_WINNER(player);
	curloc += dvec;
      }
      curloc = loc-dvec;
      while (board->grid[curloc] == BD_PLAYER(player))  {
	board->grid[curloc] = BD_WINNER(player);
	curloc -= dvec;
      }
    }
  }
  return(board->gameover = win);
}


void  pl_undo(pl_t *game)  {
  uint  turn, newturns, maxturns;
  
  if (game->move == 0)  {
    fprintf(stderr, "pente: Internal error detected.  move=0, undo.\n");
    exit(1);
  }
  maxturns = game->maxmove;
  newturns = game->move - 1;
  pl_init(game);
  for (turn = 0;  turn < newturns;  ++turn)
    pl_move(game, game->movelist[turn]);
  game->maxmove = maxturns;
}


bool  pl_redo(pl_t *game)  {
  uint  turn, newturns, maxturns;
  bool  game_over = FALSE;
  
  if (game->move == game->maxmove)  {
    fprintf(stderr,
	    "pente: Internal error detected.  move=maxmove=%d, redo.\n",
	    game->move);
    exit(1);
  }
  maxturns = game->maxmove;
  newturns = game->move + 1;
  pl_init(game);
  for (turn = 0;  turn < newturns;  ++turn)
    game_over = pl_move(game, game->movelist[turn]);
  game->maxmove = maxturns;
  return(game_over);
}


uint  pl_turn(pl_t *game)  {
  return(game->move >> 1);
}


uint  pl_player(pl_t *game)  {
  return(game->move & 1);
}


bd_t  *pl_board(pl_t *game)  {
  return(&(game->board));
}


uint  pl_nmoves(pl_t *game)  {
  return(game->move);
}


uint  pl_maxmoves(pl_t *game)  {
  return(game->maxmove);
}


bd_loc_t  pl_getmove(pl_t *game, uint movenum)  {
  return(game->movelist[movenum]);
}


void  pl_game_str(pl_t *game, char *str)  {
  int  i;

  for (i = 0;  i < pl_nmoves(game);  ++i)  {
    str = bd_loc_str(pl_getmove(game, i), str);
    *(str++) = ',';
  }
  if (i)
    *(str-1) = '\0';
  else
    *str = '\0';
}
  

void  pl_str_game(const char *str, pl_t *game)  {
  while (*str != '\0')  {
    pl_move(game, bd_str_loc(str, &str));
    if (*str == ',')
      ++str;
  }
}
