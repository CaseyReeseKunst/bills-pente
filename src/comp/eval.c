/*
 * src/comp/eval.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */


#include "../pente.h"
#include "comp.h"

const score_t  comp_capval[13] = {CAP0VAL,
				    CAP0VAL+  V_FORCE(0)+2,
				    CAP0VAL+2*V_FORCE(0)+4,
				    CAP0VAL+3*V_FORCE(0)+6,
				    V_WIN(0), 0,0,0,0,0,0,0,0};


score_t  comp_eval(bd_t *board, bd_loc_t loc, uint player,
		   score_t *cpmscore, bool *iscore)  {
  score_t  score = 0;
  
#define  SSCORE  score
#define  ISCORE  (*iscore)
#define  BOARD   board
#define  LOC     loc
#define  PLAYER  player
#define  DEGEN   FALSE
#include "eval.h"
  
#undef   SSCORE
#define  SSCORE  (*cpmscore)
#undef   ISCORE
#define  CPM     TRUE
#include "eval.h"
  
  return(score);
}
