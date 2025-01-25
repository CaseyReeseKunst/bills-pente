/*
 * src/xio/plasma.c, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include <wms.h>

#ifdef  X11_DISP

#include <wms/rnd.h>
#include "../pente.h"

#define  FACTOR  0.8
#define  SIZE    512

static void  diag(int *, int, int), perp(int *, int, int);
static int  my_random(int limit);


uchar  *xio_plasma(void)  {
  static bool  done = FALSE;
  int  *cloud, x;
  static unsigned char  *cout;
  unsigned  step, i;
  float  fskew;
  
  if (done)
    return(cout);
  done = TRUE;
  cout = (uchar *)wms_malloc(SIZE * SIZE * sizeof(char));
  fskew = (float)(750 << 8);
  cloud = (int *)wms_malloc(SIZE * SIZE * sizeof(int));
  if (cloud == NULL)  {
    fprintf(stderr, "Could not allocate enough memory.\n");
    exit(1);
  }
  *cloud = my_random(256 << 16);
  for (step = SIZE/2;  step;  step /= 2)  {
    diag(cloud, step, (int)fskew);
    fskew *= FACTOR;
    perp(cloud, step, (int)fskew);
    fskew *= FACTOR;
  }
  for (i = 0;  i < SIZE*SIZE;  ++i)  {
    x = (cloud[i] >> 8) & 511;
    if (x > 255)
      x = 511 - x;
    cout[i] = x;
  }
  wms_free(cloud);
  return(cout);
}


#define  CLACC(x, y)  cloud[(x) + ((y)*SIZE)]
static void  diag(int *cloud, int step, int skew)  {
  int  i, j;
  int  sum;
  
  for (i = 0;  i < SIZE;  i += step)
    for (j = 0;  j < SIZE;  j += step)  {
      if (((i & step) != 0) && ((j & step) != 0))  {
	sum = (CLACC(i-step, j-step) +
	       CLACC(i-step, (j+step)&(SIZE-1)) +
	       CLACC((i+step)&(SIZE-1), j-step) +
	       CLACC((i+step)&(SIZE-1), (j+step)&(SIZE-1))) / 4;
	CLACC(i, j) = sum + my_random(skew);
      }
    }
}


static void  perp(int *cloud, int step, int skew)  {
  int  i, j;
  int  sum;
  
  for (i = 0;  i < SIZE;  i += step)
    for (j = 0;  j < SIZE;  j += step)  {
      if ((i & step) != (j & step))  {
	sum = (CLACC((i-step)&(SIZE-1), j) +
	       CLACC(i, (j+step)&(SIZE-1)) +
	       CLACC((i+step)&(SIZE-1), j) +
	       CLACC(i, (j-step)&(SIZE-1))) / 4.0;
	CLACC(i, j) = sum + my_random(skew);
      }
    }
}


static int  my_random(int limit)  {
  int  i;
  
  i = rnd_int32(pe_rnd);
  if (i & (1 << 30))
    return(i % limit);
  else
    return(-(i % limit));
}

#endif
