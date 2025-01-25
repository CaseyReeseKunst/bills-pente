/*
 * src/board.h, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

/*
 * Exported features:
 *   bd_t
 *     Board data type.  User accessible features:
 *       uint captures[0..1]
 *         Number of pairs captured by the player in question.
 *       uint grid[BD_LOC_MIN..BD_LOC_MAX]
 *         Array containing the grid values.
 *       bool gameover
 *         TRUE if the game has been won (or tied)
 *   bd_init(bd_t *board)
 *     Clears the board, leaving you with a completely empty playing field
 *     and no captures for either player.
 *   bd_loc_t
 *     Data type.  Points to a location on the board.  Values of BD_LOC_MAX
 *     or larger are guaranteed to be invalid board locations.
 *   BD_EMPTYA, BD_EMPTYB, BD_EMPTYC
 *     Three different types of empty board locations.  Use at will; they all
 *     behave the same.
 *   bool BD_EMPTYP(val)
 *     TRUE iff val is BD_EMPTYA, BD_EMPTYB, or BD_EMPTYC.
 *   BD_EDGE
 *     The value of a board location off the board.  There is guaranteed to be
 *     at least one line of BD_EDGE values off each edge of the board.
 *   uint BD_PLAYER(player)
 *     The value respresenting a piece of player.
 *   uint BD_WINNER(player)
 *     The value respresenting a piece just used to win the game.
 *   uint BD_OWNER(val)
 *     Returns the player number whose piece this is.
 *   uint BD_OPP(player)
 *     The value representing a piece of player's opponent.
 *   bool BD_PLAYERP(val)
 *     Returns TRUE if val is BD_PLAYER(0), BD_PLAYER(1), BD_WINNER(0), or
 *     BD_WINNER(1).
 *   bool BD_WINNERP(val)
 *     Returns TRUE if val is BD_WINNER(0) or BD_WINNER(1).
 *   BD_MAXPLAYERS
 *     The most players supported.  2.
 *   int bd_dvec[8]
 *     Values added to a bd_loc_t to move it in one of the eight cardinal
 *     directions.  bd_dvec[x+4] is the opposite of bd_dvec[x].
 *   int bd_loc_x(bd_loc_t loc)
 *     Gets the x coordinate of a location.  0..18.
 *   int bd_loc_y(bd_loc_t loc)
 *     Gets the y coordinate of a location.  0..18.
 *   bd_loc_t bd_xy_loc(int x, int y)
 *     Gets the location from the x and y coordinates.
 *   bd_loc_t bd_str_loc(char *str, char **strout)
 *     Gets a board loc from a string.  Returns the end of the string in
 *     strout if strout is non-NULL.
 *   char *bd_loc_str(bd_loc_t loc, char *str)
 *     Writes loc to str.  Returns pointer to null-terminated end of string.
 */

#ifndef _BOARD_H_
#define _BOARD_H_  1

#define  BD_LOC_MIN    (22*3+3)
#define  BD_LOC_MAX    (BD_LOC_MIN + 18*22 + 19)
#define  BD_GRIDSIZE   (BD_LOC_MAX + 22*3+3)
#define  BD_MAXPLAYERS  2

typedef struct  {
  uint  grid[BD_GRIDSIZE];
  uint  captures[BD_MAXPLAYERS];
  uint  gameover;
} bd_t;
#define  BD_EMPTYA     0  /* Empty & Amusing */
#define  BD_EMPTYB     1  /* Empty & Boring  */
#define  BD_EMPTYP(x)  ((x) <= 1)
#define  BD_EDGE       3
#define  BD_PLAYER(n)  (4+(n))
#define  BD_OWNER(n)   ((n)&1)
#define  BD_OPP(n)     (5-(n))
#define  BD_WINNER(n)  (6+(n))
#define  BD_PLAYERP(n) ((n) >= BD_PLAYER(0))
#define  BD_WINNERP(n) ((n) >= BD_WINNER(0))

typedef uint  bd_loc_t;

extern const int   bd_dvec[8];

extern int  bd_loc_x(bd_loc_t loc), bd_loc_y(bd_loc_t loc);
extern bd_loc_t  bd_xy_loc(int x, int y);
extern void  bd_init(bd_t  *board);
extern bd_loc_t  bd_str_loc(const char *str, const char **strout);
extern char  *bd_loc_str(bd_loc_t loc, char *str);

#endif  /* _BOARD_H_ */
