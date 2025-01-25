/*
 * src/board.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Defines the board data type and related data types.
 * Types:
 *   bd_t     - The board itself.
 *   bd_loc_t - The location; used as an index into the board.  Useful values
 *              range from BD_LOC_MIN to BD_LOC_MAX.  Valid values are from
 *              0 to BD_GRIDSIZE.
 *   x,y      - The x and y coordinates of a space on the board.  Range from
 *              0 through 18.  If you want to access board locations through
 *              integers, here's how to do it.
 */

#include "pente.h"

/*
 * Direction vectors for the 8 directions.
 * That is, to move in direction 3, add bd_dvec[3] to loc.
 * Opposite directions are space 4 directions apart.
 * That is, bd_dvec[n] = -bd_dvec[n+4].
 *
 * lvec[] in xio/xdraw.c is based on this array.  Be sure to change both or
 *   neither.
 */
const int  bd_dvec[8] = {-22-1, -22, -22+1, -1,
			  22+1,  22,  22-1,  1};

int  bd_loc_x(bd_loc_t loc)   {
  return((loc-BD_LOC_MIN)%22);
}


int  bd_loc_y(bd_loc_t loc)  {
  return((loc-BD_LOC_MIN)/22);
}


bd_loc_t  bd_xy_loc(int x, int y)  {
  return(x + y*22 + BD_LOC_MIN);
}


void  bd_init(bd_t *board)  {
  uint  player;
  int   x, y;
  bd_loc_t  loc;
  
  for (player = 0;  player < BD_MAXPLAYERS;  ++player)
    board->captures[player] = 0;
  for (loc = 0;  loc < BD_GRIDSIZE;  ++loc)
    board->grid[loc] = BD_EDGE;
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)
      board->grid[bd_xy_loc(x, y)] = BD_EMPTYB;
  }
  board->gameover = FALSE;
}


bd_loc_t  bd_str_loc(const char *str, const char **strout)  {
  int  x, y;

  if (isdigit(str[0]))  {
    y = str[0] - '0';
    ++str;
    if (isdigit(str[0]))  {
      y = (y * 10) + str[0] - '0';
      ++str;
    }
    x = tolower(*(str++)) - 'a';
  } else  {
    x = tolower(*(str++)) - 'a';
    y = str[0] - '0';
    ++str;
    if (isdigit(str[0]))  {
      y = (y * 10) + str[0] - '0';
      ++str;
    }
  }
  --y;
  if ((x == 'i' - 'a') || ((uint)x > 19) || ((uint) y > 18))  {
    return(BD_LOC_MAX);
  }
  if (x > 'i' - 'a')
    --x;
  if (strout != NULL)
    *strout = str;
  return(bd_xy_loc(x, y));
}


char  *bd_loc_str(bd_loc_t loc, char *str)  {
  char  cout[] = "abcdefghjklmnopqrst";

  sprintf(str, "%c%d", cout[bd_loc_x(loc)], bd_loc_y(loc) + 1);
  while (*(++str));
  return(str);
}
