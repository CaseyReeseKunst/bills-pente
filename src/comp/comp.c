/*
 * src/comp/comp.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include "../pente.h"
#include "comp.h"


volatile bool  comp_active = FALSE, comp_abort = FALSE;
volatile bd_loc_t  comp_abortval;

static void  init_comp(bd_t *board);
static bd_loc_t  turn_2(bd_t *board);


#ifdef  COMP_DEBUG
static char  debug_info[100], *dip;
#endif


typedef struct movinfo  {
  score_t  score, cpmscore;
  bd_loc_t  loc;
  bool  iscore;
} movinfo;

static int movinfo_cmp(const void *ta, const void *tb)  {
  movinfo  *a = (movinfo *)ta;
  movinfo  *b = (movinfo *)tb;

  if (b->cpmscore == a->cpmscore)
    return(b->loc - a->loc);
  else
    return(b->cpmscore - a->cpmscore);
}


bd_loc_t  comp_selectmove(pl_t *game, uint level)  {
  D_VAR
    bd_t  *board = pl_board(game);
  uint player = pl_player(game), turn_num = pl_turn(game);
  score_t  best_score, cur_score;
  int  mn, movnum;
  uint scores;
  bd_loc_t  cur_loc;
  movinfo  movlist[19*19];
  bool  iscore;
  score_t  cpmscore;
  
  if ((turn_num == 0) && (player == 0))
    return(bd_xy_loc(9, 9));
  if ((turn_num == 1) && (player == 0))
    return(turn_2(board));
  
  do  {
    movnum = 0;
    init_comp(board);
    for (cur_loc = BD_LOC_MIN;  cur_loc < BD_LOC_MAX;  ++cur_loc)  {
      if (board->grid[cur_loc] == BD_EMPTYA)  {
	cur_score = comp_eval(board, cur_loc, player, &cpmscore, &iscore);
	if (cur_score >= WINVAL)
	  return(cur_loc);
	movlist[movnum].score = cur_score;
	movlist[movnum].cpmscore = cpmscore;
	movlist[movnum].loc = cur_loc;
	movlist[movnum].iscore = iscore;
	++movnum;
      }
    }
    qsort(movlist, movnum, sizeof(movinfo), movinfo_cmp);
    scores = 0;
    best_score = -WINVAL + 1;
    for (mn = 0;  mn < movnum;  ++mn)  {
      cur_score = movlist[mn].score;
      cur_loc = movlist[mn].loc;
      if (cur_score < WINVAL)  {
	if (level >= 4)  {
	  cur_score -= comp_move(D_ARG board, cur_loc, player,
				 movlist[mn].iscore);
	  if ((level > 4) && ((cur_score & 1) == 0) &&
	      (cur_score > -WINVAL))  {
	    if (cur_score > best_score)  {
	      if (cur_score >= WINVAL)
		return(cur_loc);
	      best_score = cur_score;
	    }
	    movlist[scores].loc = cur_loc;
	    movlist[scores].score = movlist[mn].score;
	    movlist[scores].cpmscore = cur_score + (rnd_int32(pe_rnd) &
						    (V_FORCE(0) - 1));
	    movlist[scores].iscore = movlist[mn].iscore;
	    ++scores;
	  }
	} else if (level == 3)
	  cur_score -= comp_lmove(D_ARG board, cur_loc, player);
	else if (level == 2)
	  cur_score -= comp_end_eval(D_ARG board, cur_loc, player);
      }
#ifdef COMP_DEBUG
      if (level <= 4)
	printf("%s: %c%8x %d\n", tmpinfo,
	       (cur_score<0?'-':' '), (cur_score>0?cur_score:-cur_score),
	       movlist[mn].cpmscore);
#endif
      if ((level <= 4) && ((cur_score & 1) == 0) &&
	  (cur_score >= best_score))  {
	if (cur_score >= WINVAL)
	  return(cur_loc);
	if (cur_score > best_score)  {
	  best_score = cur_score;
	  scores = 0;
	}
	movlist[scores++].loc = cur_loc;
      }
    }
    if (!scores)  {
#ifdef  COMP_DEBUG
      printf("\tLoss.  L%d ->L%d\n", level, (level-2)|1);
#endif
      level = (level - 2) | 1;
    } else if (level > 4)  {
      int  limit = level, i;
      int  snum = scores, simples;
      
      simples = level - (level / 3);
      best_score = -WINVAL + 1;
      qsort(movlist, scores, sizeof(movinfo), movinfo_cmp);
      if ((scores == 1) || (movlist[1].cpmscore <= -WINVAL))
	return(movlist[0].loc);
      scores = 0;
      for (i = 0;  (i < limit) && (i < snum);  ++i)  {
	if (!simples && !movlist[i].iscore)  {
	  ++limit;
	  continue;
	}
	if (movlist[i].cpmscore <= -WINVAL)
	  break;
	comp_think(movlist[i].loc);
	simples += movlist[i].iscore - 1;
	cur_score = movlist[i].score - comp_deeptree(D_ARG board,
						     movlist[i].loc,
						     level-1, player);
	if (((cur_score & 1) == 0) && (cur_score >= best_score))  {
	  if (cur_score > best_score)  {
	    if (cur_score >= WINVAL)
	      return(movlist[i].loc);
	    scores = 0;
	    best_score = cur_score;
	  }
	  movlist[scores++].loc = movlist[i].loc;
	}
#ifdef COMP_DEBUG
	printf("%s: %c%8x %d\n", tmpinfo,
	       (cur_score<0?'-':' '), (cur_score>0?cur_score:-cur_score),
	       (movlist[i].cpmscore&~(V_FORCE(0)-1))/V_FORCE(0));
#endif
      }
      if (!scores)  {
#ifdef  COMP_DEBUG
	printf("Loss.\n");
#endif
	level = (level - 2)|1;
      }
    }
  } while (!scores);
#ifdef  COMP_DEBUG
  printf("\n\n");
#endif
  return(movlist[rnd_int32(pe_rnd) % scores].loc);
}


/* The first player's second move. */
static bd_loc_t  turn_2(bd_t *board)  {
  static int  turn2x[] = { 6, 7, 8, 9,10,11,12,
			     6,12,    6,12,    6,12,    6,12,    6,12,
			     6, 7, 8, 9,10,11,12};
  static int  turn2y[] = { 6, 6, 6, 6, 6, 6, 6,
			     7, 7,    8, 8,    9, 9,   10,10,   11,11,
			     12,12,12,12,12,12,12};
  uint  move2;
  bd_loc_t  loc;
  
  do  {
    move2 = rnd_int32(pe_rnd) % (sizeof(turn2x) / sizeof(*turn2x));
    loc = bd_xy_loc(turn2x[move2], turn2y[move2]);
  } while (!BD_EMPTYP(board->grid[loc]));
  return(loc);
}


static void  init_comp(bd_t *board)  {
  bd_loc_t  loc;
  uint  dir;
  int   dvec;
  
  for (loc = BD_LOC_MIN;  loc < BD_LOC_MAX;  ++loc)  {
    if (BD_EMPTYP(board->grid[loc]))  {
      board->grid[loc] = BD_EMPTYB;
      for (dir = 0;  dir < 8;  ++dir)  {
	if (BD_PLAYERP(board->grid[loc+bd_dvec[dir]]))  {
	  board->grid[loc] = BD_EMPTYA;
	}
      }
    }
  }
  for (loc = BD_LOC_MIN;  loc < BD_LOC_MAX;  ++loc)  {
    if (BD_PLAYERP(board->grid[loc]))  {
      for (dir = 0;  dir < 8;  ++dir)  {
	dvec = bd_dvec[dir];
	if ((board->grid[loc+dvec] == board->grid[loc]) &&
	    (board->grid[loc+dvec*3] == BD_EMPTYB))
	  board->grid[loc+dvec*3] = BD_EMPTYA;
      }
    }
  }
}


#ifdef  COMP_DEBUG
void  comp_addinfo(int l, bd_loc_t m)  {
  static int  ml = 0;
  
  if (l > ml)  {
    ml = l;
    dip = debug_info + 98 - 4*ml;
  }
  sprintf(debug_info+98-l*4, "%c%2d", bd_loc_x(m)+'A', bd_loc_y(m)+1);
  debug_info[98-l*4+3] = ' ';
}
#endif
