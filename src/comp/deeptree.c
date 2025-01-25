/*
 * src/comp/deeptree.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */


#include "../pente.h"
#include "comp.h"

typedef struct  {
  bd_loc_t  loc;
  bool  iscore;
  score_t  score, modscore, evscore;
} dtinfo;


static int  dtcmp(const void *a, const void *b)  {
  const dtinfo  *dta = a, *dtb = b;

  if (dtb->modscore == dta->modscore)
    return(dtb->loc - dta->loc);
  else
    return(dtb->modscore - dta->modscore);
}


score_t  comp_deeptree(D_PAR bd_t *board, bd_loc_t loc, int level,
		       int player)  {
  D_VAR
    dtinfo  dtd[19*19];
  int  dtdnum=0, simples = 0, i, limit;
  bool  iscore;
  score_t  cscore, cpmscore, best_score = -WINVAL*3-1, evs;
  bd_loc_t  cur_loc;
  uint  orig_caps, caps, fa,fb, ba,bb, dir, bdsave[2*8];
  int  dvec;
  
  D_INIT(loc, level-1);
  
  /* Update the board for the move. */
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
  
  best_score = -WINVAL*2;
  for (cur_loc = BD_LOC_MIN;  cur_loc < BD_LOC_MAX;  ++cur_loc)  {
    if (board->grid[cur_loc] == BD_EMPTYA)  {
      cscore = evs = comp_eval(board, cur_loc, player, &cpmscore, &iscore);
      if (evs < WINVAL)
	cscore -= comp_move(D_ARG board, cur_loc, player, iscore);
      else  {
	D_WIN(cur_loc);
	best_score = WINVAL*2;
	dtdnum = 1;
	level = 4;
	break;
      }
      if (((cscore & 1) == 0) && (cscore > -WINVAL))  {
	if (cscore > best_score)  {
	  if (cscore >= WINVAL)  {
	    D_WIN(cur_loc);
	    best_score = WINVAL*2;
	    dtdnum = 1;
	    level = 4;
	    break;
	  }
	  D_CPY();
	  best_score = cscore;
	}
	dtd[dtdnum].loc = cur_loc;
	dtd[dtdnum].iscore = iscore;
	dtd[dtdnum].score = cscore;
	dtd[dtdnum].modscore = cscore + (rnd_int32(pe_rnd) & (V_FORCE(0) - 1));
	dtd[dtdnum].evscore = evs;
	++dtdnum;
      }
    }
  }
  if (dtdnum && (level > 4))  {
    qsort(dtd, dtdnum, sizeof(dtinfo), dtcmp);
    simples = level - (level / 3);
    limit = level;
    best_score = -WINVAL*3-1;
    for (i = 0;  (i < limit) && (i < dtdnum);  ++i)  {
      if (!simples && !dtd[i].iscore)  {
	++limit;
	continue;
      }
      if (dtd[i].score <= -WINVAL)
	break;
      simples += dtd[i].iscore - 1;
      cscore = dtd[i].evscore - comp_deeptree(D_ARG board, dtd[i].loc, level-1,
					      player);
      if (cscore > best_score)
	best_score = cscore;
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
  
  return(best_score);
}
