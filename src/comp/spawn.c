/*
 * src/comp/spawn.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Spawn off the computer.
 */

#include <wms.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../pente.h"
#include "comp.h"


static void  comp_dispatch(void);


static pid_t  comp_pid;
static int  recv_from_p, send_to_p;
static bool  comp_active = FALSE;
static bool  think_enabled = TRUE;


void  comp_spawn(int *recv_from_c, int *send_to_c)  {
  pid_t  forkout;
  int  pipe_res;
  int  pipes[2];

  pipe_res = pipe(pipes);
  if (pipe_res == -1)  {
    fprintf(stderr,
	    "pente: Error: Could not start computer opponent.\n");
    exit(1);
  }
  *recv_from_c = pipes[0];
  send_to_p = pipes[1];
  
  pipe_res = pipe(pipes);
  if (pipe_res == -1)  {
    fprintf(stderr,
	    "pente: Error: Could not start computer opponent.\n");
    exit(1);
  }
  recv_from_p = pipes[0];
  *send_to_c = pipes[1];
  
  forkout = fork();
  if (forkout == -1)  {
    fprintf(stderr,
	    "pente: Error: Could not start computer opponent.\n");
    exit(1);
  }
  if (forkout == 0)  {
    int  i, maxFiles;

    maxFiles = getdtablesize();
    /*
     * Close off all file descriptors that the audio child doesn't need.
     */
    for (i = 3;  i < maxFiles;  ++i)  {
      if ((i != recv_from_p) && (i != send_to_p))
	close(i);
    }
    comp_dispatch();
    exit(0);
  } else  {
    close(recv_from_p);
    close(send_to_p);
    comp_pid = forkout;
  }
}


/* Kill the computer player, and restart! */
void  comp_reset(int *recv_from_c, int *send_to_c)  {
  int  status;

  if (comp_active)  {
    close(*recv_from_c);
    close(*send_to_c);
    kill(comp_pid, SIGKILL);
    wait(&status);
    comp_spawn(recv_from_c, send_to_c);
    comp_active = FALSE;
  }
}


static void  comp_dispatch(void)  {
  const int  msgin_size = (19*19+10)*4+4;
  char  msgin[(19*19+10)*4+4], *mp;
  int  amt_read, level;
  pl_t  game;
  bd_loc_t  movout;

  /* Make sure that we don't save OUR state! */
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGPIPE, SIG_DFL);
  signal(SIGHUP, SIG_DFL);

  rnd_destroy(pe_rnd);
  pe_rnd = rnd_create(time(NULL) ^ getpid());
  for (;;)  {
    amt_read = read(recv_from_p, msgin, msgin_size - 1);
    msgin[amt_read] = '\0';
    if (amt_read == 0)
      exit(0);
    if (amt_read == -1)  {  /* Dead pipe or something. */
      perror("Pente computer logic: Error ");
      exit(1);
    }
    level = atoi(msgin);
    for (mp = msgin;  *mp != ' ';  ++mp);
    ++mp;
    pl_init(&game);
    pl_str_game(mp, &game);
    movout = comp_selectmove(&game, level);
    msgin[0] = 'M';
    *bd_loc_str(movout, msgin+1) = ' ';
    write(send_to_p, msgin, 4);
  }
}


void  comp_think(bd_loc_t move)  {
  char  msgout[5];

  if (think_enabled)  {
    msgout[0] = 'T';
    *bd_loc_str(move, msgout+1) = ' ';
    write(send_to_p, msgout, 4);
  }
}


void  comp_start(pl_t *game, uint level, int to_c)  {
  char  msgout[(19*19+10)*4+4];
  char  *c;

  comp_active = TRUE;
  sprintf(msgout, "%d ", level);
  for (c = msgout;  *c;  ++c);
  pl_game_str(game, c);
  write(to_c, msgout, strlen(msgout));
}


bd_loc_t  comp_getmove(int from_c, void think(bd_loc_t pos, uint player),
		       uint player)  {
  char  msgin[5];
  bd_loc_t  loc;

  read(from_c, msgin, 4);
  msgin[4] = '\0';
  loc = bd_str_loc(msgin+1, NULL);
  if (msgin[0] == 'T')  {
    if (think)
      think(loc, player);
    loc = PE_COMP;
  } else  {
    /* It's a move command.  The computer is no longer active. */
    comp_active = FALSE;
    snd_play(&pe_move_snd);
  }
  return(loc);
}


bd_loc_t  comp_noSpawnGetMove(pl_t *game, int level)  {
  think_enabled = FALSE;
  return(comp_selectmove(game, level));
}
