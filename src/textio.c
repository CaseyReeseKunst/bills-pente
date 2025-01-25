/*
 * src/textio.c, part of Pente (game program)
 * Copyright (C) 1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include <wms.h>
#include "pente.h"


static void  quit(void);
static char  prefix[] = "                  ", plchar[2] = {'#','O'};
static const char  xname[] = "ABCDEFGHJKLMNOPQRST";
static bool  thinking_needed = TRUE;
static int  *levels;

static char  plname[2][9];


bd_loc_t  text_init(int *comp_level, uint winner, pl_t *game)  {
  int  i, scans;
  static bool  first_time = TRUE;
  
  if (first_time)  {
    printf("Pente version " VERSION ", Copyright (C) 1994-1995 "
	   "William Shubert\n"
	   "   Pente comes with ABSOLUTELY NO WARRANTY and is distrubuted\n"
	   "   under the Gnu General Public Licence, Version 2\n"
	   "   See the file \"COPYING\" for details.\n");
    first_time = FALSE;
  }
  levels = comp_level;
  if (winner != BD_MAXPLAYERS)
    printf("\n%s [%c] has won!\n\n", plname[winner], plchar[winner]);
  for (i = 0;  i < 2;  ++i)  {
    do  {
      printf("Level of player %d [0=Player, 1..9=Computer]: ", i+1);
      if ((scans = scanf("%d%*[^\n]%*c", comp_level + i)) == EOF)  {
	quit();
	exit(0);
      } else if (scans == 0)
	scanf("%*[^\n]%*c");
      if ((comp_level[i] < 0) || (comp_level[i] > 9))
	scans = 0;
    } while (scans != 1);
    if (comp_level[i] == 0)
      sprintf(plname[i], "Player %d", i+1);
    else
      sprintf(plname[i], "Comp Lv%d", comp_level[i]);
  }
  return(PE_RESTART);
}


void  text_draw(pl_t *game, bool forced)  {
  bd_t  *board = pl_board(game);
  int  x, y;
  bd_loc_t  loc;
  char  c1, c2, mvout[4];
  
  printf("\n");
  if (!thinking_needed)  {
    thinking_needed = TRUE;
    printf("\n");
  }
  c1 = c2 = ' ';
  if (pl_player(game) == 0)
    c1 = '<';
  else
    c2 = '>';
  printf("%s%s [%c]: %d%c%c%cTurn%3d%c%c%c%s [%c]: %d\n",
	 prefix, plname[0], plchar[0], board->captures[0], c1,c1,c1,
	 pl_turn(game)+1, c2,c2,c2, plname[1], plchar[1], board->captures[1]);
  printf("%s   ", prefix);
  for (x = 0;  x < 19;  ++x)
    printf("%c ", xname[x]);
  printf("\n");
  for (y = 18;  y >= 0;  --y)  {
    printf("%s%2d ", prefix, y+1);
    for (x = 0;  x < 19;  ++x)  {
      loc = bd_xy_loc(x, y);
      if (BD_EMPTYP(board->grid[loc]))
	printf(". ");
      else
	printf("%c ", plchar[BD_OWNER(board->grid[loc])]);
    }
    printf("%2d\n", y+1);
  }
  printf("%s   ", prefix);
  for (x = 0;  x < 19;  ++x)
    printf("%c ", xname[x]);
  if (pl_nmoves(game) > 0)  {
    loc = pl_getmove(game, pl_nmoves(game) - 1);
    bd_loc_str(loc, mvout);
    printf("\n%s%s [%c] moved at %s.", prefix,
	   plname[pl_player(game)^1], plchar[1-pl_player(game)], mvout);
  }
  printf("\n");
}


bd_loc_t  text_selectmove(pl_t *game, int compfd)  {
  uint  player = pl_player(game), turn_num = pl_turn(game);
  static int  last_turn = -1, last_player = -1;
  int  scancount;
  char  temp[100];
  bd_loc_t  move;
  
  if (levels[player] > 0)
    return(PE_COMP);
  if ((last_turn == turn_num) && (last_player == player))  {
    /* Last move was invalid, so I'm removing. */
    putchar('\a');
  }
  last_turn = turn_num;
  last_player = player;
  if ((turn_num == 0) && (player == 0))
    return(bd_xy_loc(9,9));
  printf("%sPlayer %d [%c]: ", prefix, player+1, plchar[player]);
  if ((scancount = scanf("%*c%99[^\n]", temp)) == EOF)
    quit();
  if (scancount != 1)
    return(PE_CONTINUE);
  move = bd_str_loc(temp, NULL);
  if ((move == BD_LOC_MAX) || !pl_movep(game, move))  {
    putchar('\a');
    return(PE_CONTINUE);
  }
  return(move);
}


void  text_think(bd_loc_t pos, uint player)  {
  char  mvout[4];

  if (thinking_needed)  {
    printf("I'm thinking...");
    thinking_needed = FALSE;
  }
  bd_loc_str(pos, mvout);
  printf("%s...", mvout);
  fflush(stdout);
}


static void  quit(void)  {
  printf("\n\nBye!\n");
  pe_output_vars();
  exit(0);
}
