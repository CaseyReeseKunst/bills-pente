/*
 * src/comp/lmove.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */


#include "../pente.h"
#include "comp.h"


typedef struct  {
  bd_loc_t  loc;
  score_t  score;
} loc_info;


static score_t  comp_end_tiny(D_PAR bd_t *board, bd_loc_t loc, uint player,
			      loc_info *nlcheck);


extern score_t  comp_lmove(D_PAR bd_t *board, bd_loc_t loc, uint player)  {
  D_VAR
    uint  bdsave[2*8], orig_caps, caps;
  bd_loc_t  cur_loc, sscheck[19*19], capcheck[(19*19+2)/3],
  nscheck = BD_LOC_MAX;
  loc_info  nlcheck[10];
  int       dvec, nlnum, capnum = 0, capscore[(19*19+2)/3];
  uint  dir;
  score_t  best_score = -WINVAL*2, best_sscore = -WINVAL*2,
  best_nsscore = -WINVAL*2, cur_score;
  uint  fa,fb, ba,bb;
  int  num_sscores = 0;
  score_t  ss_nsc = -WINVAL*2, sscore, nsscore;
  
  nlcheck[0].score = SCORE_MAX;
  nlcheck[1].score = SCORE_MIN;
  nlcheck[2].score = SCORE_MIN;
  nlcheck[3].score = SCORE_MIN;
  nlcheck[4].score = SCORE_MIN;
  nlcheck[5].score = SCORE_MIN;
  nlcheck[6].score = SCORE_MIN;
  nlcheck[7].score = SCORE_MIN;
  nlcheck[8].score = SCORE_MIN;
  nlcheck[9].score = SCORE_MIN;
  D_INIT(loc, 1);
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
  for (cur_loc = BD_LOC_MIN;  cur_loc < BD_LOC_MAX;  ++cur_loc)  {
    if (board->grid[cur_loc] == BD_EMPTYA)  {
      
#define  SSCORE   sscore
#define  OSCORE   nsscore
#define  BOARD    board
#define  LOC      cur_loc
#define  PLAYER   player
#define  DEGEN    FALSE
#define  CPM
#include "eval.h"
#undef  SSCORE
#undef  OSCORE
#undef  BOARD
#undef  LOC
#undef  PLAYER
#undef  DEGEN
#undef  CPM
      
      if (nlcheck[9].score < nsscore)  {
	for (nlnum = 8;  nlcheck[nlnum].score < nsscore;  --nlnum)
	  nlcheck[nlnum+1] = nlcheck[nlnum];
	nlcheck[nlnum+1].loc = cur_loc;
	nlcheck[nlnum+1].score = nsscore;
      }
      if (sscore >= WINVAL)  {
	best_score = WINVAL*2;
	break;
      } else if (sscore & 0x1e)  {
	capcheck[capnum] = cur_loc;
	capscore[capnum++] = sscore;
      } else if ((sscore > best_sscore) ||
		 ((sscore == best_sscore) && (sscore > V_FORCE(1))))  {
	if (sscore > best_sscore)  {
	  num_sscores = 0;
	  best_sscore = sscore;
	}
	sscheck[num_sscores++] = cur_loc;
#ifdef  COMP_DEBUG_LOTS_N_LOTS
	printf(" >! %2d%c: %11d", bd_loc_y(cur_loc)+1,
	       bd_loc_x(cur_loc)+'A',sscore);
#endif
      }
      if (!(sscore & 0x1e))  {
	if ((nsscore > best_nsscore) ||
	    ((nsscore == best_nsscore) && (sscore > ss_nsc)))  {
	  best_nsscore = nsscore;
	  nscheck = cur_loc;
	  ss_nsc = sscore;
#ifdef  COMP_DEBUG_LOTS_N_LOTS
	  printf(" >! %2d%c: %11d", bd_loc_y(cur_loc)+1,
		 bd_loc_x(cur_loc)+'A',sscore);
#endif
	}
      }
    }
  }
  
  while (capnum--)  {
    cur_score = capscore[capnum] - comp_end_tiny(D_ARG board, capcheck[capnum],
						 player, nlcheck + 1);
    if (cur_score > best_score)  {
      D_CPY();
      best_score = cur_score;
    }
  }
  while (num_sscores--)  {
    cur_score = best_sscore - comp_end_tiny(D_ARG board, sscheck[num_sscores],
					    player, nlcheck + 1);
    if (cur_score > best_score)  {
      D_CPY();
      best_score = cur_score;
    }
  }
  if (nscheck != BD_LOC_MAX)  {
    cur_score = ss_nsc - comp_end_tiny(D_ARG board, nscheck, player,
				       nlcheck + 1);
    if (cur_score > best_score)  {
      D_CPY();
      best_score = cur_score;
    }
  }
  
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


static score_t  comp_end_tiny(D_PAR bd_t *board, bd_loc_t loc, uint player,
			      loc_info *nlcheck)  {
  uint  bdsave[2*8];
  bd_loc_t  cur_loc, cappos[4], mycap[16];
  uint  dir;
  score_t  best_score = -WINVAL*2, cur_score;
  int  dvec, i, ncappos = 0, nmycap = 0;
  uint  fa,fb, ba,bb;
  
  D_INIT(loc, 0);
#ifdef  COMP_DEBUG
  info += 4;
#endif
  /* Save old board layout and update the board to reflect this move. */
  board->grid[loc] = BD_PLAYER(player);
  for (dir = 0;  dir < 4;  ++dir)  {
    dvec = bd_dvec[dir];
    bdsave[dir+ 0] = fa = board->grid[loc+dvec];
    bdsave[dir+ 4] = fb = board->grid[loc+dvec+dvec];
    bdsave[dir+ 8] = ba = board->grid[loc-dvec];
    bdsave[dir+12] = bb = board->grid[loc-(dvec+dvec)];
    if (fa == BD_OPP(player))  {
      if ((fb == BD_OPP(player)) &&
	  (board->grid[loc+dvec+dvec+dvec] == BD_PLAYER(player)))  {
	board->grid[mycap[nmycap++] = loc+dvec] = BD_EMPTYA;
	board->grid[mycap[nmycap++] = loc+dvec+dvec] = BD_EMPTYA;
      } else if ((ba == BD_PLAYER(player)) && BD_EMPTYP(bb))  {
	cappos[ncappos++] = loc-(dvec+dvec);
      }
    } else if (BD_EMPTYP(fa) && (ba == BD_PLAYER(player)) &&
	       (bb == BD_OPP(player)))  {
      cappos[ncappos++] = loc+dvec;
    }
    if (ba == BD_OPP(player))  {
      if ((bb == BD_OPP(player)) &&
	  (board->grid[loc-(dvec+dvec+dvec)] == BD_PLAYER(player)))  {
	board->grid[mycap[nmycap++] = loc-dvec] = BD_EMPTYA;
	board->grid[mycap[nmycap++] = loc-(dvec+dvec)] = BD_EMPTYA;
      } else if ((fa == BD_PLAYER(player)) && BD_EMPTYP(fb))  {
	cappos[ncappos++] = loc+dvec+dvec;
      }
    } else if (BD_EMPTYP(ba) && (fa == BD_PLAYER(player)) &&
	       (fb == BD_OPP(player)))  {
      cappos[ncappos++] = loc-dvec;
    }
  }
  player ^= 1;
  
  for (i = 0;  i < ncappos;  ++i)  {
    cur_loc = cappos[i];
#define  SSCORE  cur_score
#define  BOARD   board
#define  LOC     cur_loc
#define  PLAYER  player
#define  DEGEN   TRUE
#define  CPM     TRUE
#include "eval.h"
    if (cur_score > best_score)  {
      D_INIT(cur_loc, -1);
      best_score = cur_score;
    }
  }
  
  for (i = 0;  i < nmycap;  ++i)  {
    cur_loc = mycap[i];
#define  SSCORE  cur_score
#define  BOARD   board
#define  LOC     cur_loc
#define  PLAYER  player
#define  DEGEN   TRUE
#define  CPM     TRUE
#include "eval.h"
    if (cur_score > best_score)  {
      D_INIT(cur_loc, -1);
      best_score = cur_score;
    }
  }
  
  for (i = 0;  i < 9;  ++i)  {
    cur_loc = nlcheck[i].loc;
    if (!BD_EMPTYP(board->grid[cur_loc]))
      continue;
#define  SSCORE  cur_score
#define  BOARD   board
#define  LOC     cur_loc
#define  PLAYER  player
#define  DEGEN   TRUE
#define  CPM     TRUE
#include "eval.h"
    if (cur_score > best_score)  {
      D_INIT(cur_loc, -1);
      best_score = cur_score;
    }
    if (cur_score >= nlcheck[i].score)
      break;
  }
  
  /* Restore the board to its original state. */
  for (dir = 0;  dir < 4;  ++dir)  {
    dvec = bd_dvec[dir];
    board->grid[loc+dvec       ] = bdsave[dir+ 0];
    board->grid[loc+dvec+dvec  ] = bdsave[dir+ 4];
    board->grid[loc-dvec       ] = bdsave[dir+ 8];
    board->grid[loc-(dvec+dvec)] = bdsave[dir+12];
  }
  board->grid[loc] = BD_EMPTYA;
  
  return(best_score);
}


score_t  comp_end_eval(D_PAR bd_t *board, bd_loc_t loc, uint player)  {
  uint  bdsave[2*8];
  bd_loc_t  cur_loc;
  uint  dir;
  score_t  best_score = -WINVAL*2, cur_score;
  int  dvec;
  uint  fa,fb, ba,bb;
  
  D_INIT(loc, 0);
#ifdef  COMP_DEBUG
  info += 4;
#endif
  /* Save old board layout and update the board to reflect this move. */
  board->grid[loc] = BD_PLAYER(player);
  for (dir = 0;  dir < 4;  ++dir)  {
    dvec = bd_dvec[dir];
    bdsave[dir+ 0] = fa = board->grid[loc+dvec];
    bdsave[dir+ 4] = fb = board->grid[loc+dvec+dvec];
    bdsave[dir+ 8] = ba = board->grid[loc-dvec];
    bdsave[dir+12] = bb = board->grid[loc-(dvec+dvec)];
    if (fa == BD_EMPTYB)
      board->grid[loc+dvec] = BD_EMPTYA;
    else if ((fa == BD_OPP(player)) &&
	     (fb == BD_OPP(player)) &&
	     (board->grid[loc+dvec+dvec+dvec] == BD_PLAYER(player)))  {
      board->grid[loc+dvec] = BD_EMPTYA;
      board->grid[loc+dvec+dvec] = BD_EMPTYA;
    }
    if (ba == BD_EMPTYB)
      board->grid[loc-dvec] = BD_EMPTYA;
    else if ((ba == BD_OPP(player)) &&
	     (bb == BD_OPP(player)) &&
	     (board->grid[loc-(dvec+dvec+dvec)] == BD_PLAYER(player)))  {
      board->grid[loc-dvec] = BD_EMPTYA;
      board->grid[loc-(dvec+dvec)] = BD_EMPTYA;
    }
  }
  player ^= 1;
  
  for (cur_loc = BD_LOC_MIN;  cur_loc < BD_LOC_MAX;  ++cur_loc)  {
    if (board->grid[cur_loc] == BD_EMPTYA)  {
#define  SSCORE  cur_score
#define  BOARD   board
#define  LOC     cur_loc
#define  PLAYER  player
#define  DEGEN   TRUE
#define  CPM     TRUE
#include "eval.h"
      if (cur_score > best_score)  {
	D_INIT(cur_loc, -1);
	best_score = cur_score;
      }
    }
  }
  
  /* Restore the board to its original state. */
  for (dir = 0;  dir < 4;  ++dir)  {
    dvec = bd_dvec[dir];
    board->grid[loc+dvec       ] = bdsave[dir+ 0];
    board->grid[loc+dvec+dvec  ] = bdsave[dir+ 4];
    board->grid[loc-dvec       ] = bdsave[dir+ 8];
    board->grid[loc-(dvec+dvec)] = bdsave[dir+12];
  }
  board->grid[loc] = BD_EMPTYA;
  
  return(best_score);
}
