/*
 * $Source: /home/wms/cvs/pente/src/cio.c,v $
 * $Revision: 1.2 $
 * $Date: 2001/08/01 00:01:45 $
 *
 * src/cio.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

/*
 * Typically a file will include wms.h, which includes configure.h.
 * But many curses.h files are SO BADLY put together that I have conflicts
 *   all over the place, so I can't include wms.h.
 */
#include <configure.h>

#if  CURSES_DISP

#include <curses.h>
#include "pente.h"

#define  CTRLKEY(ch)  ((ch)-'A'+1)
static WINDOW  *win;
static const char  plchar[2] = {'#', 'O'};
static const char  xname[] = "ABCDEFGHJKLMNOPQRST";
static char  plname[2][9] = {"Player 1", "Player 2"};
static int  *levels;

static void  quit(void);
static bool  open_window(void);
static bool  readstr(uint x, uint y, char *prompt, char *buf, int buflen);
static void  redraw(void);

bd_loc_t  cio_init(int *comp_level, uint winner, pl_t *game)  {
  static bool  open_needed = TRUE;
  int  i, scans;
  
  levels = comp_level;
  if (open_needed)  {
    pl_t  game;
    
    if (!open_window())
      return(PE_CANTPLAY);
    open_needed = FALSE;
    pl_init(&game);
    cio_draw(&game, TRUE);
  }
  if (winner != BD_MAXPLAYERS)  {
    move(22,0);
    clrtobot();
    mvprintw(22,29, "%s [%c] has won!", plname[winner], plchar[winner]);
  }
  for (i = 0;  i < 2;  ++i)  {
    char  level_q[100], tmpbuf[10];
    
    sprintf(level_q, "Level of player %d [0=Player, 1..6=Computer]: ", i+1);
    do  {
      tmpbuf[0] = '\0';
      if (!readstr(23,17, level_q, tmpbuf, 2))  {
	i = -1;
	break;
      }
      if (tmpbuf[0] == 'U')  {
	if ((pl_turn(game) >= 1) &&
	    ((comp_level[0] < 1) || (comp_level[1] < 1)))
	  return(PE_UNDO);
	else  {
	  putchar('\a');
	  i = -1;
	  break;
	}
      }
      if ((scans = sscanf(tmpbuf, "%d", comp_level + i)) == 1)
	if (comp_level[i] > 6)
	  scans = 0;
    } while (scans != 1);
    if (comp_level[i] == 0)
      sprintf(plname[i], "Player %d", i+1);
    else
      sprintf(plname[i], "Comp Lv%d", comp_level[i]);
  }
  move(23,0);
  clrtoeol();
  return(PE_RESTART);
}


static bool  open_window(void)  {
  if (getenv("TERM") == NULL)
    return(FALSE);
  printf("Pente version " VERSION ", Copyright (C) 1994-1995 William Shubert\n"
	 "   Pente comes with ABSOLUTELY NO WARRANTY and is distrubuted\n"
	 "   under the Gnu General Public Licence, Version 2\n"
	 "   See the file \"COPYING\" for details.\n");
  sleep(5);
  win = initscr();
  if (win == NULL)
    return(FALSE);
  cbreak();
  noecho();
  nonl();
  erase();
  return(TRUE);
}


void  cio_draw(pl_t *game, bool forced) {
  bd_t  *board = pl_board(game);
  int  x, y;
  bd_loc_t  loc;
  char  c1, c2;
  
  c1 = c2 = ' ';
  if (pl_player(game) == 0)
    c1 = '<';
  else
    c2 = '>';
  mvprintw( 4,1, "Selecting moves:");
  mvprintw( 5,4,    "I        P");
  mvprintw( 6,3,   "J K      B F");
  mvprintw( 7,4,    "M        N");
  mvprintw( 8,2,  "Or arrow keys.");
  mvprintw( 9,2,  "<SPACE>, <CR>:");
  mvprintw(10,4,    "Make Move");
  mvprintw(12,1, "U: Undo");
  mvprintw(13,1, "Q: Quit");
  mvprintw(14,1, "R: Restart");
  mvprintw(15,1, "L: Redraw");
  mvprintw(4,62, "Pente " VERSION);
  mvprintw(5,62, DATE);
  mvprintw(6,62, "Bill Shubert");
  mvprintw(7,62, "wms@igoweb.org");
  mvprintw(0,18, "%s [%c]: ", plname[0], plchar[0]);
  if (board->captures[0] > 4)
    standout();
  printw("%d", board->captures[0]);
  standend();
  printw("%c%c%cTurn%3d%c%c%c%s [%c]: ", c1,c1,c1, pl_turn(game)+1,
	 c2,c2,c2, plname[1], plchar[1]);
  if (board->captures[1] > 4)
    standout();
  printw("%d", board->captures[1]);
  standend();
  for (x = 0;  x < 19;  ++x)  {
    mvaddch(1, x*2+21, xname[x]);
    mvaddch(21, x*2+21, xname[x]);
  }
  for (y = 0;  y < 19;  ++y)  {
    mvprintw(2+y, 18, "%2d", 19-y);
    mvprintw(2+y, 18+3+19*2, "%d", 19-y);
  }
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)  {
      move((18-y)+2, x*2+21);
      loc = bd_xy_loc(x, y);
      if (BD_EMPTYP(board->grid[loc]))
	addch('.');
      else  {
	if (BD_WINNERP(board->grid[loc]))
	  standout();
	addch(plchar[BD_OWNER(board->grid[loc])]);
	if (BD_WINNERP(board->grid[loc]))
	  standend();
      }
    }
  }
  if (pl_nmoves(game) > 0)  {
    loc = pl_getmove(game, pl_nmoves(game) - 1);
    mvprintw(22, 18, "%s [%c] moved at %c%d.",
	     plname[1 - pl_player(game)], plchar[1-pl_player(game)],
	     xname[bd_loc_x(loc)], bd_loc_y(loc) + 1);
  } else
    move(22,0);
  clrtoeol();
  if (!forced)  {
    move(23,0);
    clrtoeol();
  }
  refresh();
}



bd_loc_t  cio_selectmove(pl_t *game, int compfd)  {
  static int  last_turn = -1, last_player = -1;
  uint  player = pl_player(game), turn_num = pl_turn(game);
  int  x, y, ch;
  bd_loc_t  loc;
  uint  esc_chars = 0;
  
  if (levels[player] > 0)
    return(PE_COMP);
  last_turn = turn_num;
  last_player = player;
  if ((turn_num == 0) && (player == 0))
    return(bd_xy_loc(9,9));
  loc = pl_getmove(game, pl_nmoves(game) - 1);
  x = bd_loc_x(loc);
  y = bd_loc_y(loc);
  mvprintw(23,18, "Select a move, player %d [%c].", player+1, plchar[player]);
  clrtoeol();
  for (;;)  {
    move((18-y)+2, x*2+21);
    refresh();
    ch = getch();
    switch(ch)  {
    case '\33':
      esc_chars = 1;
      break;
    case '[':
      if (esc_chars == 1)
	esc_chars = 2;
      else
	esc_chars = 0;
      break;
    case 'B':
      if (esc_chars != 2)  {
	esc_chars = 0;
	--x;
	if (x < 0)
	  x = 0;
	break;
      }
    case CTRLKEY('N'):
    case 'n':case 'N':
    case 'm':case 'M':
      esc_chars = 0;
      --y;
      if (y < 0)
	y = 0;
      break;
    case 'A':
      if (esc_chars != 2)
	break;
    case CTRLKEY('P'):
    case 'p':case 'P':
    case 'i':case 'I':
      esc_chars = 0;
      ++y;
      if (y > 18)
	y = 18;
      break;
    case 'C':
      if (esc_chars != 2)
	break;
    case CTRLKEY('F'):
    case 'f':case 'F':
    case 'k':case 'K':
      esc_chars = 0;
      ++x;
      if (x > 18)
	x = 18;
      break;
    case 'D':
      if (esc_chars != 2)
	break;
    case CTRLKEY('B'):
    case 'b':
    case 'j':case 'J':
      esc_chars = 0;
      --x;
      if (x < 0)
	x = 0;
      break;
    case '\n':case'\r':case ' ':
      esc_chars = 0;
      if (pl_movep(game, bd_xy_loc(x, y)))  {
	return(bd_xy_loc(x,y));
      } else
	putchar('\a');
      break;
    case CTRLKEY('Q'):
    case 'q':case 'Q':
      quit();
      break;
    case CTRLKEY('U'):
    case 'u':case 'U':
      esc_chars = 0;
      if (turn_num > 1)
	return(PE_UNDO);
      else
	putchar('\a');
      break;
    case CTRLKEY('R'):
    case 'r':case 'R':
      esc_chars = 0;
      return(PE_RESTART);
      break;
    case CTRLKEY('L'):
    case 'l':case 'L':
      esc_chars = 0;
      redraw();
      break;
    default:
      esc_chars = 0;
      putchar('\a');
      break;
    }
  }
}


void  cio_think(bd_loc_t pos, uint player)  {
  move((18-bd_loc_y(pos))+2, bd_loc_x(pos)*2+21);
  refresh();
}


static void  quit(void)  {
  nl();  /* On some systems endwin() doesn't properly reset this. */
  endwin();
  pe_output_vars();
  exit(0);
}


/* Returns FALSE if you need a total redo. */
static bool  readstr(uint y, uint x, char *prompt, char *buf, int buflen)  {
  char  ch;
  int  ch_no;
  
  move(y, 0);
  clrtobot();
  mvaddstr(y, x, prompt);
  x += strlen(prompt);
  
  ch_no = 0;
  move(y, x);
  for (;;)  {
    refresh();
    ch = getch();
    switch(ch)  {
    case CTRLKEY('Q'):
    case 'q':case 'Q':
      quit();
      break;
    case CTRLKEY('L'):
    case 'l':case 'L':
      redraw();
      break;
    case CTRLKEY('R'):
    case 'r':case 'R':
      return(FALSE);
      break;
    case CTRLKEY('U'):
    case 'u':case 'U':
      buf[0] = 'U';
      buf[1] = '\0';
      return(TRUE);
      break;
    case '\177':
    case '\010':
      if (ch_no)  {
	--ch_no;
	--x;
	mvaddstr(y, x, " ");
	move(y, x);
      }
      break;
    case '\n':
    case '\r':
      if (ch_no != 0)  {
	buf[ch_no] = '\0';
	return(TRUE);
      }
      break;
    default:
      if (isprint(ch) && (ch_no < buflen))  {
	mvaddch(y, x, ch);
	++x;
	buf[ch_no++] = ch;
	break;
      }
    }
  }
}


static void  redraw(void)  {
#ifdef  clearok
  clearok(win, TRUE);
#endif
  refresh();
#ifdef  clearok
  clearok(win, FALSE);
#endif
}

#endif
