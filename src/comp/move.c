/*
 * src/comp/move.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */


#include "../pente.h"
#include "comp.h"

#ifdef COMP_DEBUG
int  l1_repnum=0, l2_repnum=0, l3_repnum=0;
#endif

extern score_t  comp_move(D_PAR bd_t *board, bd_loc_t loc, uint player,
			  bool check_simple)  {
  D_VAR
    uint  bdsave[2*8], orig_caps, caps;
  uint  movlistlen;
  uint  dir;
  score_t  best_score, cur_score;
  bd_loc_t  cur_loc, movlist[19*19];
  score_t  sscore, sscores[19*19];
  int  dvec;
  uint  fa,fb, ba,bb;
  bool  iscore, iscores[19*19];
  
#ifdef  COMP_DEBUG
  l2_repnum = 0;
  ++l1_repnum;
  if ((l1_repnum != 2) && (l1_repnum != 4))
    l2_repnum = -1;
  else
    printf("%c%2d:\n",bd_loc_x(loc)+'A',bd_loc_y(loc)+1);
#endif
  
  D_INIT(loc, 2);
  
  /* Save old board layout and update the board to reflect this move. */
  orig_caps = board->captures[player];
  caps = orig_caps;
  board->grid[loc] = BD_PLAYER(player);
  for (dir = 0;  dir < 4;  ++dir)  {
    dvec = bd_dvec[dir];
    bdsave[dir+ 0] = fa = board->grid[loc+dvec];
    bdsave[dir+ 4] = fb = board->grid[loc+dvec+dvec];
    bdsave[dir+ 8] = ba = board->grid[loc-dvec];
    bdsave[dir+12] = bb = board->grid[loc-(dvec+dvec)];
    if (fb == BD_EMPTYB)
      board->grid[loc+dvec+dvec] = BD_EMPTYA;
    if (fa == BD_EMPTYB)
      board->grid[loc+dvec] = BD_EMPTYA;
    else if ((fa == BD_OPP(player)) &&
	     (fb == BD_OPP(player)) &&
	     (board->grid[loc+dvec+dvec+dvec] == BD_PLAYER(player)))  {
      ++caps;
      board->grid[loc+dvec] = BD_EMPTYA;
      board->grid[loc+dvec+dvec] = BD_EMPTYA;
    }
    if (bb == BD_EMPTYB)
      board->grid[loc-(dvec+dvec)] = BD_EMPTYA;
    if (ba == BD_EMPTYB)
      board->grid[loc-dvec] = BD_EMPTYA;
    else if ((ba == BD_OPP(player)) &&
	     (bb == BD_OPP(player)) &&
	     (board->grid[loc-(dvec+dvec+dvec)] == BD_PLAYER(player)))  {
      ++caps;
      board->grid[loc-dvec] = BD_EMPTYA;
      board->grid[loc-(dvec+dvec)] = BD_EMPTYA;
    }
  }
  board->captures[player] = caps;
  player ^= 1;
  
  /* Generate the list of moves. */
  movlistlen = 0;
  best_score = -WINVAL*3-1;
  for (cur_loc = BD_LOC_MIN;  cur_loc < BD_LOC_MAX;  ++cur_loc)  {
    if (board->grid[cur_loc] == BD_EMPTYA)  {
      
#define  SSCORE   sscore
#define  BOARD    board
#define  LOC      cur_loc
#define  PLAYER   player
#define  DEGEN    FALSE
#define  ISCORE   iscore
#include "eval.h"
      if (sscore >= WINVAL)  {
	D_WIN(cur_loc);
	best_score = WINVAL*2;
	movlistlen = 0;
	break;
      } else if (check_simple || iscore) {
	if (sscore > V_FORCE(0))  {
	  cur_score = sscore - comp_lmove(D_ARG board, cur_loc, player);
#ifdef  COMP_DEBUG
	  if (l2_repnum >= 0)  {
	    printf("   %c%2d: %8x %8x\n",bd_loc_x(cur_loc)+'A',
		   bd_loc_y(cur_loc)+1,sscore, cur_score);
	  }
#endif
	  if (((cur_score & 1) == 0) && (cur_score > best_score))  {
	    D_CPY();
	    if ((best_score = cur_score) >= WINVAL)  {
	      movlistlen = 0;
	      break;
	    }
	  }
	} else  {
	  movlist[movlistlen] = cur_loc;
	  sscores[movlistlen] = sscore;
	  iscores[movlistlen++] = iscore;
	}
      }
    }
  }
  
  while (movlistlen--)  {
    cur_score = sscores[movlistlen] - comp_lmove(D_ARG board,
						 movlist[movlistlen], player);
#ifdef  COMP_DEBUG
    if (l2_repnum >= 0)  {
      printf("   %c%2d: %8x %8x\n",bd_loc_x(movlist[movlistlen])+'A',
	     bd_loc_y(movlist[movlistlen])+1,sscores[movlistlen], cur_score);
    }
#endif
    if (((cur_score & 1) == 0) && (cur_score > best_score))  {
      D_CPY();
      if ((best_score = cur_score) >= WINVAL)
	break;
    }
  }
  
#ifdef  COMP_DEBUG_LOTS
  if (1)  {
    int  i;
    printf("\n%x %d:\n",best_score,2);
    for (i=0;  i < movlistlen;  ++i)  {
      printf("%8x %8x %c%2d\n", movlist[i].blscore, movlist[i].sscore,
	     bd_loc_x(movlist[i].loc)+'A', bd_loc_y(movlist[i].loc)+1);
    }
  }
#endif
  
  /* Restore the board to its original state. */
  board->captures[player^1] = orig_caps;
  for (dir = 0;  dir < 4;  ++dir)  {
    dvec = bd_dvec[dir];
    board->grid[loc+dvec       ] = bdsave[dir+ 0];
    board->grid[loc+dvec+dvec  ] = bdsave[dir+ 4];
    board->grid[loc-dvec       ] = bdsave[dir+ 8];
    board->grid[loc-(dvec+dvec)] = bdsave[dir+12];
  }
  board->grid[loc] = BD_EMPTYA;
  
  /* Return the best move found. */
  return(best_score);
}
