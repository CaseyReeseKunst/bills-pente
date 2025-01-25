/*
 * src/comp/comp.h, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#ifndef  _COMP_H_
#define  _COMP_H_  1

#ifdef COMP_DEBUG
extern int  l1_repnum, l2_repnum, l3_repnum;
#endif

/* score_t must be at least 32 bits. */
typedef int32  score_t;

#ifdef  COMP_DEBUG
#define  D_PAR     char *info,
#define  D_VAR     char tmpinfo[100];
#define  D_ARG     tmpinfo,
#define  D_CPY()   strcpy(info+4,tmpinfo)
#define  D_WIN(l)\
do  {\
  sprintf(info+4,"%c%2d",bd_loc_x(l)+'A',bd_loc_y(l)+1);\
  info[3] = ' ';\
  info[4] = '\0';\
} while (0)

#define  D_INIT(l, a)\
if (!BD_EMPTYP(board->grid[l]))  {\
  fprintf(stderr, "!!!MOVE ONTO NONEMPTY!!! %d\n", a);\
  for(;;);\
}\
do  {\
  sprintf(info,"%c%2d",bd_loc_x(l)+'A',bd_loc_y(l)+1);\
  info[3] = ' ';\
  info[4] = '\0';\
} while (0)
     
#else  /* !COMP_DEBUG */
#define  D_PAR
#define  D_VAR
#define  D_ARG
#define  D_CPY()
#define  D_WIN(l)
#define  D_INIT(l, a)
#endif
     
/* From "comp_move.c" */
extern score_t  comp_move(D_PAR bd_t *board, bd_loc_t loc, uint player,
			  bool check_simple);
extern score_t  comp_lmove(D_PAR bd_t *board, bd_loc_t loc, uint player);
extern score_t  comp_end_eval(D_PAR bd_t *board, bd_loc_t loc, uint player);
extern score_t  comp_deeptree(D_PAR bd_t *board, bd_loc_t loc, int level,
			      int player);
extern bd_loc_t  comp_selectmove(pl_t *game, uint level);

/* From "comp_spawn.c" */
extern void  comp_think(bd_loc_t move);

/* From "comp_eval.c" */
#define  SCORE_MAX     ((score_t)0x7fffffff)
#define  SCORE_MIN     (-(SCORE_MAX))
#define  WINVAL        ((score_t)0x20000000)
#define  V_WIN(b)      ((score_t)((WINVAL / 2) * 3))
#define  V_D4FORCE(b)  ((score_t)(b?WINVAL/ 8:WINVAL/ 4))
#define  V_D3FORCE(b)  ((score_t)(b?WINVAL/64:WINVAL/32))
#define  V_FORCE(b)    ((score_t)(WINVAL/32768))
#define  CAP0VAL       ((score_t)((V_FORCE(0)*3)+2))
#define  V_CAP0        ((score_t)(CAP0VAL - 2))

extern const score_t  comp_capval[];
extern score_t  comp_eval(bd_t *board, bd_loc_t loc, uint player,
			  score_t *cpmscore, bool *iscore);
     
#endif
