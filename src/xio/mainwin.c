/*
 * src/xio/mainwin.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 *
 * Routines to manage the main window for pente.
 */

#include <wms.h>

#if  X11_DISP

#include <but/but.h>
#include <but/ctext.h>
#include <but/menu.h>
#include <but/text.h>
#include <but/plain.h>
#include <but/box.h>
#include <but/tblock.h>
#include <but/timer.h>
#include <abut/abut.h>
#include <abut/msg.h>

#include "../pente.h"
#include "xio.h"
#include "setup.h"
#include "sound.h"
#include "helpwin.h"

static ButOut  unmap(ButWin *win);
static ButOut  map(ButWin *win);
static ButOut  resize(ButWin *win);
static ButOut  iresize(ButWin *win);
static ButOut  destroy(ButWin *win);
static Pixmap  decorate_bg(Xio *xio, Pixmap bp, int winw, int winh,
			   int gridw, int grid0x, int grid0y, bool icon);
static void  pente_stamp(ButEnv *env, Pixmap pm, int x,int y, int w,int h);
static ButOut  kill_copyright(ButTimer *timer);


static bool  color_enabled;


void  xio_m_create(Xio *xio, const char *name, const char *geometry,
		   bool iconic, bool color)  {
  int  x, y;
  int  i;
  bd_loc_t  pos;
  const char *optlist[10];
  static bool  first_time = TRUE;
  static ButKey  quitkeys[] = {{XK_Q, 0,0}, {XK_q, 0,0}, {0,0,0}};
  static ButKey  helpkeys[] = {{XK_H, 0,0}, {XK_h, 0,0}, {XK_Help, 0,0},
			       {XK_question, 0,0}, {0,0,0}};
  static ButKey  soundkeys[] = {{XK_S, 0,0}, {XK_s, 0,0}, {0,0,0}};
  static ButKey  undokeys[] = {{XK_BackSpace, 0,0}, {XK_Left, 0,0}, {0,0,0}};
  static ButKey  redokeys[] = {{XK_Right, 0,0}, {0,0,0}};

  color_enabled = (color != GrayScale);
  xio->mainwin = butWin_iCreate(NULL, xio->env,
				name, 430, 430,
				&xio->iconwin, iconic, 63, 63,
				unmap, map, resize, iresize, destroy);
  butWin_setMinW(xio->mainwin, 100);
  butWin_setMinH(xio->mainwin, 100);
  butWin_setMaxW(xio->mainwin, 0);
  butWin_setMaxH(xio->mainwin, 0);
  butWin_setWHRatio(xio->mainwin, 1, 1);
  butWin_setGeom(xio->mainwin, geometry);
  butWin_activate(xio->mainwin);
  butWin_setId(xio->mainwin, 0);
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)  {
      pos = bd_xy_loc(x, y);
      xio->gridbuts[pos] = but_grid_create(xio_press_move, xio->mainwin,
					   2, BUT_DRAWABLE, pos);
      xio->iconbuts[pos] = but_grid_create(NULL, xio->iconwin,
					   2, BUT_DRAWABLE, pos);
      but_setId(xio->gridbuts[pos],x+y*19);
      if (!BD_EMPTYP(xio->iboard.grid[pos]))  {
	but_grid_change(xio->gridbuts[pos], xio->iboard.grid[pos],
			pos == xio->markpos);
	but_grid_change(xio->iconbuts[pos], xio->iboard.grid[pos],
			pos == xio->markpos);
      }
    }
  }			      
  for (i = 0;  i < 5;  ++i)  {
    xio->caps1[i] = but_cap_create(xio->mainwin, 1, BUT_DRAWABLE, i+1, 1);
    xio->caps2[i] = but_cap_create(xio->mainwin, 1, BUT_DRAWABLE, i+1, 0);
  }
  for (i = 0;  i < 3;  ++i)  {
    xio->digits[i] = butText_create(xio->mainwin, 1, BUT_DRAWABLE, "",
				    butText_center);
    butText_setFont(xio->digits[i], 1);
  }
  xio_m_set_turn(xio, xio_turn_num, -1);
  for (i = 0;  xioStr_turn[xio->lang][i];  ++i)  {
    char  temp[2];

    temp[1] = '\0';
    temp[0] = xioStr_turn[xio->lang][i];
    xio->turnlabel[i] = butText_create(xio->mainwin, 1, BUT_DRAWABLE, temp,
				       butText_center);
    butText_setFont(xio->turnlabel[i], 1);
  }
  xio->turnlabel[i] = NULL;
  xio->undobut = butAct_create(xio_press_undo, NULL, xio->mainwin, 1,
			       BUT_DRAWABLE, xioStr_undo[xio->lang],
			       BUT_ALEFT|BUT_RRIGHT);
  but_setKeys(xio->undobut, undokeys);
  but_setId(xio->undobut, 19*19+0);
  xio->redobut = butAct_create(xio_press_redo, NULL, xio->mainwin, 1,
			       BUT_DRAWABLE, xioStr_redo[xio->lang],
			       BUT_RLEFT|BUT_ARIGHT);
  but_setKeys(xio->redobut, redokeys);
  but_setId(xio->redobut, 19*19+1);
  xio->helpbut = butCt_create(xio_press_help, NULL, xio->mainwin, 1,
			      BUT_DRAWABLE|BUT_PRESSABLE,
			      xioStr_help[xio->lang]);
  but_setKeys(xio->helpbut, helpkeys);
  but_setId(xio->helpbut, 19*19+2);
  butCt_setNetAction(xio->helpbut, FALSE);
  xio->setupbut = butCt_create(xio_press_setup, xio->swin, xio->mainwin, 1,
			       BUT_DRAWABLE|BUT_PRESSABLE,
			       xioStr_setup[xio->lang]);
  but_setId(xio->setupbut, 19*19+3);
  butCt_setNetAction(xio->setupbut, FALSE);
  xio->startbut = butCt_create(xio_press_start, NULL, xio->mainwin, 1,
			       BUT_DRAWABLE|BUT_PRESSABLE,
			       xioStr_start[xio->lang]);
  but_setId(xio->startbut, 19*19+4);
  xio->quitbut = butCt_create(xio_press_quit, NULL, xio->mainwin, 1,
			      BUT_DRAWABLE|BUT_PRESSABLE,
			      xioStr_quit[xio->lang]);
  but_setKeys(xio->quitbut, quitkeys);
  but_setId(xio->quitbut, 19*19+5);
  butCt_setNetAction(xio->quitbut, FALSE);
  xio->soundbut = butKeytrap_create(xioSound_toggle, xio, xio->mainwin,
				    BUT_DRAWABLE|BUT_PRESSABLE);
  but_setKeys(xio->soundbut, soundkeys);
  if (xio->remoteEnd)  {
    optlist[0] = xioStr_remote[xio->lang];
    optlist[1] = xioStr_human[xio->lang];
  } else  {
    optlist[0] = xioStr_human[xio->lang];
    optlist[1] = xioStr_remote[xio->lang];
  }
  optlist[2] = BUTMENU_OLBREAK;
  optlist[3] = xioStr_comp1[xio->lang];
  optlist[4] = xioStr_comp2[xio->lang];
  optlist[5] = xioStr_comp3[xio->lang];
  optlist[6] = xioStr_comp4[xio->lang];
  optlist[7] = xioStr_comp5[xio->lang];
  optlist[8] = xioStr_comp6[xio->lang];
  optlist[9] = BUTMENU_OLEND;
  xio->menu1 = butMenu_upCreate(xio_change_menu, NULL, xio->mainwin, 1, 3,
				BUT_DRAWABLE|BUT_PRESSABLE,
				xioStr_player1[xio->lang],
				optlist, 0);
  butMenu_setColor(xio->menu1, XIO_PIC_BUT1FG, XIO_PIC_PL1);
#if  !HAVE_SOCKETS
  butMenu_setFlags(xio->menu1, 1, BUTMENU_DISABLED);
#endif
  but_setId(xio->menu1, 19*19+6);
  xio->menu2 = butMenu_upCreate(xio_change_menu, NULL, xio->mainwin, 1, 3,
				 BUT_DRAWABLE|BUT_PRESSABLE,
				 xioStr_player2[xio->lang],
				 optlist, 0);
  butMenu_setColor(xio->menu2, BUT_FG, XIO_PIC_PL2);
#if  !HAVE_SOCKETS
  butMenu_setFlags(xio->menu2, 1, BUTMENU_DISABLED);
#endif
  but_setId(xio->menu2, 19*19+7);
  xio->bgpic = None;
  xio->bgbut = butPixmap_create(xio->mainwin, 0, BUT_DRAWABLE, None);
  xio->ibgpic = None;
  xio->ibgbut = butPixmap_create(xio->iconwin, 0, BUT_DRAWABLE, None);
  if (first_time)  {
    /* Put up the copyright. */
    first_time = FALSE;
    xio->copbox = butBox_create(xio->mainwin, 5, BUT_DRAWABLE);
    xio->copback = butPlain_create(xio->mainwin, 4,
				   BUT_DRAWABLE|BUT_PRESSABLE, BUT_BG);
    xio->copwords1 = butTblock_create(xio->mainwin, 5, BUT_DRAWABLE,
				      "Pente " VERSION, butText_center);
    butTblock_setFont(xio->copwords1, 1);
    xio->copwords2 = butTblock_create(xio->mainwin, 5, BUT_DRAWABLE,
				      xioStr_copyright[xio->lang],
				      butText_center);
    xio->copwords3 = butTblock_create(xio->mainwin, 5, BUT_DRAWABLE,
				      xioStr_nowarr[xio->lang],
				      butText_center);
    xio->copwords4 = butTblock_create(xio->mainwin, 5, BUT_DRAWABLE,
				      xioStr_seehelp[xio->lang],
				      butText_center);
  }
}


static ButOut  unmap(ButWin *win)  {
  Xio  *xio = butEnv_packet(butWin_env(win));
  
  xio->wait_nonicon = FALSE;
  clp_setBool(pe_clp, "iconic", TRUE);
  --xio->openWindows;
  xioSound_check(xio);
  return(0);
}


static ButOut  map(ButWin *win)  {
  Xio  *xio = butEnv_packet(butWin_env(win));

  xio->wait_nonicon = TRUE;
  ++xio->openWindows;
  xioSound_check(xio);
  return(0);
}


static ButOut  resize(ButWin *win)  {
  Xio  *xio;
  int  i, x, y, w, h, winw, winh, buth, bw;
  int  gridw;
  int  grid0x, grid0y, capSpc, capHt;
  bd_loc_t  loc;
  uchar  *bgcmap;
  struct timeval  five_secs;

  xio = butEnv_packet(butWin_env(win));
  if (xio->wait_nonicon)  {
    xio->wait_nonicon = FALSE;
    clp_setBool(pe_clp, "iconic", FALSE);
  }
  winw = butWin_w(win);
  winh = butWin_h(win);
  gridw = (winw * 19 + 215) / 430;
  buth = xio->abut->butH = xio->buth = (winh*5+42)/86;
  while (gridw * 22 > winw)
    --gridw;
  while (gridw * 20 + buth*2 > winh)
    --gridw;
  butEnv_setFont(xio->env, 0, xioStr_mfonts[xio->lang], (buth + 1) / 2);
  butEnv_setFont(xio->env, 1, xioStr_bfonts[xio->lang], (gridw * 16 + 9)/19);
  butEnv_drawAll(butWin_env(win));

  grid0x = (winw - gridw * 19) / 2;
  grid0y = (winh - buth * 2 - gridw * 19) / 2;
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)  {
      loc = bd_xy_loc(x, y);
      but_resize(xio->gridbuts[loc], grid0x+x*gridw, grid0y+y*gridw,
		 gridw, gridw);
    }
  }
  capSpc = (buth - gridw) / 2;
  capHt = winh - buth*2;
  for (i = 0;  i < 5;  ++i)  {
    but_resize(xio->caps1[i], capSpc,
	       (capHt*i)/5 + capHt/10 - gridw, gridw, gridw*2);
    but_resize(xio->caps2[i], winw-gridw-capSpc,
	       (capHt*i)/5 + capHt/10 - gridw, gridw, gridw*2);
  }
  x = 8 + ((strlen(xioStr_turn[xio->lang]) + 4) / 2);
  x = grid0x + (gridw * x) + (gridw+1)/2;
  y = grid0y + (gridw+1)/2;
  for (i = 0;  i < 3;  ++i)  {
    but_resize(xio->digits[i], x-i*gridw,y, gridw,gridw);
  }
  x = 9 - ((strlen(xioStr_turn[xio->lang]) + 4) / 2);
  x = grid0x + (gridw * x) + (gridw+1)/2;
  for (i = 0;  xio->turnlabel[i] != NULL;  ++i)  {
    but_resize(xio->turnlabel[i], x+i*gridw,y, gridw,gridw);
  }
  but_resize(xio->menu1, 0,winh-2*buth,             winw/5,2*buth);
  but_resize(xio->menu2, winw-(winw/5),winh-2*buth, winw/5,2*buth);
  but_resize(xio->undobut,  winw/5,winh-2*buth, winw/5,buth);
  but_resize(xio->setupbut, winw/5,winh-buth,   winw/5,buth);
  but_resize(xio->redobut, winw-2*(winw/5),winh-2*buth, winw/5,buth);
  but_resize(xio->quitbut, winw-2*(winw/5),winh-buth,   winw/5,buth);
  but_resize(xio->startbut, 2*(winw/5),winh-2*buth, winw-4*(winw/5),buth);
  but_resize(xio->helpbut,  2*(winw/5),winh-buth,   winw-4*(winw/5),buth);
  if (xio->bgpic != None)
    XFreePixmap(butEnv_dpy(xio->env), xio->bgpic);
  xio->bgpic = None;
  bgcmap = xio_plasma();
  if (color_enabled)  {
    if (xio->color == TrueColor)  {
      xio->bgpic = cm2pm_chop(xio, bgcmap, 512,512, winw,winh,
			      XIO_PIC_BOARD(0), 256, gridw, buth);
    } else  {
      xio->bgpic = cm2pm_chop(xio, bgcmap, 512,512, winw,winh,
			      XIO_PIC_BOARD(0), 8, gridw, buth);
    }
  }
  xio->bgpic = decorate_bg(xio, xio->bgpic, winw, winh, gridw,
			   grid0x, grid0y, FALSE);
  butPixmap_setPic(xio->bgbut, xio->bgpic, 0,0);
  but_resize(xio->bgbut, 0,0, winw,winh);
  if (xio->copbox != NULL)  {
    w = (winw*2+1)/3;
    h = (winh+1)/3;
    x = (winw - w) / 2;
    y = (winh - h + 1) / 3;
    bw = butEnv_stdBw(xio->env);
    but_resize(xio->copbox, x,y, w,h);
    but_resize(xio->copback, x,y, w,h);
    but_resize(xio->copwords1, x+bw,y+bw*2, w-bw*2,buth);
    y += bw*3+buth;
    y += butTblock_resize(xio->copwords2, x+bw*2,y, w-bw*4);
    y += butTblock_resize(xio->copwords3, x+bw*2,y, w-bw*4);
    butTblock_resize(xio->copwords4, x+bw*2,y, w-bw*4);
    if (xio->coptimer == NULL)  {
      /*
       * Wait until the resize to set the "kill copyright" timer.
       * If set the timer at window creation, it may be more than five
       *   seconds until the window is drawn, in which case the user will
       *   never see the copyright!
       */
      five_secs.tv_sec = 5;
      five_secs.tv_usec = 0;
      xio->coptimer = butTimer_create(xio, xio->copbox, five_secs, five_secs,
				      FALSE, kill_copyright);
    }
  }
  xioSetup_resize(xio->swin);
  xioHelp_resize(xio->hwin);
  return(0);
}


static Pixmap  decorate_bg(Xio *xio, Pixmap bp, int winw, int winh,
			   int gridw, int grid0x, int grid0y, bool icon)  {
  uint  i, j;
  int  halfgw;
  int  buth = xio->buth;
  ButEnv  *env = xio->env;
  Display  *dpy = butEnv_dpy(env);
  GC  gc = butEnv_gc(env);
  int  capHt;

  if (bp == None)  {
    bp = XCreatePixmap(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       winw, winw, DefaultDepth(dpy, DefaultScreen(dpy)));
    butEnv_setXFg(env, XIO_PIC_BOARD(0));
    XFillRectangle(dpy, bp, gc, 0,0,winw,winw);
  }
  if (icon)
    pente_stamp(env, bp, 0,0, winw,winw);
  else
    pente_stamp(env, bp, (winw-gridw*20)/2, 0, gridw*20, gridw*20);
  /* Draw the actual grid itself. */
  butEnv_setXFg(env, XIO_PIC_BLACK);
  if (gridw > 5)  {
    /* Grid lines. */
    i = (gridw+6)/12;
    if (i < 1)
      i = 1;
    XSetLineAttributes(dpy, gc, i, LineSolid, CapProjecting, JoinMiter);
    halfgw = (gridw+1)/2;
    for (i = 0;  i < 19;  ++i)  {
      XDrawLine(dpy, bp, gc, grid0x+halfgw, grid0y+halfgw+gridw*i,
		grid0x+halfgw+gridw*18, grid0y+halfgw+gridw*i);
      XDrawLine(dpy, bp, gc, grid0x+halfgw+gridw*i, grid0y+halfgw,
		grid0x+halfgw+gridw*i, grid0y+halfgw+gridw*18);
    }
    if (!icon)  {
      XFontStruct  *fs = butEnv_fontStr(env, 1);
      XCharStruct  *xcs;

      /* Put the word "Captures" down the sides. */
      capHt = winh - buth*2;
      XSetFont(dpy, gc, fs->fid);
      j = strlen(xioStr_captures[xio->lang]);
      for (i = 0;  i < j;  ++i)  {
	if (fs->per_char == NULL)
	  xcs = &(fs->min_bounds);
	else
	  xcs = &(fs->per_char[xioStr_captures[xio->lang][i] - 
			       fs->min_char_or_byte2]);
	XDrawString(dpy, bp, gc,
		    (buth-xcs->width)/2 - xcs->lbearing,
		    (capHt*i/j)+(capHt/(2*j))+(fs->ascent - fs->descent)/2,
		    &xioStr_captures[xio->lang][i], 1);
	XDrawString(dpy, bp, gc,
		    winw-(buth+xcs->width)/2 - xcs->lbearing,
		    (capHt*i/j)+(capHt/(2*j))+(fs->ascent - fs->descent)/2,
		    &xioStr_captures[xio->lang][i], 1);
      }
    }
  }
  return(bp);
}


/* Stamp a giant "Pente" logo on the board. */
static void  pente_stamp(ButEnv *env, Pixmap pm, int x,int y, int w,int h)  {
  static XPoint  polyf0[] = /* Left part of "P" */
    {{48,793},{225,793},{225,1196},{236,1208},{244,1209},{274,1210},{282,1211},
       {286,1211},{286,1250},{48,1250},{48,1211},{81,1209},{88,1209},{90,1208},
       {93,1208},{105,1195},{105,844},{95,834},{57,833},{55,832},{48,832},
       {48,793}};
  static XPoint  polyf2[] = /* Right part of "P" */
    {{225,793},{290,793},{292,794},{305,794},{307,795},{324,796},{326,797},
       {337,798},{339,799},{359,803},{386,812},{403,821},{424,836},{441,856},
       {452,879},{457,897},{459,911},{460,927},{459,952},{458,954},{457,966},
       {456,968},{452,985},{450,989},{443,1006},{429,1027},{409,1047},
       {404,1049},{396,1056},{373,1067},{346,1076},{327,1080},{306,1083},
       {276,1085},{241,1085},{239,1084},{225,1084},{225,1039},{245,1039},
       {261,1037},{275,1034},{291,1028},{307,1016},{318,1003},{324,990},
       {326,986},{328,978},{329,976},{331,963},{332,961},{332,952},
       {333,950},{333,931},{332,917},{330,905},{327,893},{320,876},{313,867},
       {312,864},{296,850},{285,845},{279,842},{266,840},{264,839},{225,839},
       {225,793}};
  static XPoint  polyf1[] =
    {{1454,826},{1514,826},{1514,923},{1618,923},{1618,973},{1514,973},
       {1514,1146},{1515,1148},{1516,1159},{1517,1161},{1520,1171},
       {1528,1181},{1533,1183},{1537,1186},{1550,1190},{1570,1192},
       {1589,1192},{1591,1191},{1601,1191},{1603,1190},{1615,1189},
       {1617,1188},{1618,1190},{1618,1226},{1590,1236},{1558,1245},
       {1534,1250},{1513,1253},{1497,1254},{1482,1254},{1480,1253},
       {1467,1252},{1465,1251},{1452,1248},{1448,1246},{1432,1239},
       {1418,1227},{1409,1215},{1408,1210},{1407,1208},{1404,1194},
       {1402,1170},{1402,973},{1353,973},{1353,942},{1412,905},{1418,892},
       {1422,887},{1432,866},{1436,861},{1445,842},{1449,837},{1454,826}};
  static XPoint  polyf3[] = /* Top of "e" */
    {{678,903},{704,903},{706,904},{716,904},{718,905},{731,906},{733,907},
       {751,911},{755,913},{761,914},{771,919},{786,926},{807,942},{824,960},
       {826,965},{833,974},{844,998},{851,1022},{736,1039},{736,1033},
       {735,1011},{731,987},{724,969},{715,957},{703,949},{694,947},
       {692,946},{675,946},{675,947},{671,947},{669,948},{662,949},
       {646,959},{634,976},{629,991},{628,993},{627,1000},{626,1002},
       {625,1015},{624,1017},{624,1039},{513,1017},{515,1013},{519,1002},
       {531,980},{549,957},{575,935},
       {580,933},{585,929},{608,918},{636,909},{649,907},{651,906},{665,905},
       {667,904},{678,904},{678,903}};
  static XPoint  polyf6[] = /* Bottom of "e" */
    {{624,1039},{736,1039},{851,1022},{854,1042},{855,1052},
       {856,1068},{856,1081},{621,1081},{622,1100},{623,1102},{624,1112},
       {625,1114},{627,1123},{628,1125},{635,1142},{647,1160},{665,1176},
       {684,1185},{704,1190},{718,1191},{736,1191},{738,1190},{757,1189},
       {759,1188},{778,1185},{780,1184},{797,1180},{801,1178},{824,1170},
       {854,1155},{854,1217},{849,1218},{831,1227},{801,1238},{773,1246},
       {760,1248},{752,1250},{735,1252},{729,1253},{697,1255},{675,1255},
       {673,1254},{662,1254},{660,1253},{646,1252},{644,1251},{625,1247},
       {604,1240},{598,1237},{579,1228},{552,1207},{531,1182},{528,1175},
       {522,1167},{521,1162},{515,1150},{508,1127},{507,1119},{506,1117},
       {504,1099},{503,1081},{504,1057},{505,1055},{506,1042},{507,1040},
       {513,1017},{624,1039}};
  static XPoint  polyf4[] =  /* Top of 2nd "e" */
    {{1822,903},{1848,903},{1850,904},{1860,904},{1862,905},{1875,906},
       {1877,907},{1895,911},{1899,913},{1905,914},{1915,919},{1930,926},
       {1951,942},{1968,960},{1970,965},{1977,974},{1988,998},{1995,1022},
       {1880,1039},{1880,1033},{1879,1011},{1875,987},{1868,969},{1859,957},
       {1847,949},{1838,947},{1836,946},{1819,946},{1819,947},{1815,947},
       {1813,948},{1806,949},{1790,959},{1778,976},{1773,991},{1772,993},
       {1771,1000},{1770,1002},{1769,1015},{1768,1017},{1768,1039},
       {1657,1017},{1659,1013},{1663,1002},{1675,980},
       {1693,957},{1719,935},{1724,933},{1729,929},{1752,918},{1780,909},
       {1793,907},{1795,906},{1809,905},{1811,904},{1822,904},{1822,903}};
  static XPoint  polyf7[] =  /* Bottom of 2nd "e" */
    {{1768,1039},{1880,1039},{1995,1022},
       {1998,1042},{1999,1052},{2000,1068},{2000,1081},{1765,1081},{1766,1100},
       {1767,1102},{1768,1112},{1769,1114},{1771,1123},{1772,1125},{1779,1142},
       {1791,1160},{1809,1176},{1828,1185},{1848,1190},{1862,1191},{1880,1191},
       {1882,1190},{1901,1189},{1903,1188},{1922,1185},{1924,1184},{1941,1180},
       {1945,1178},{1968,1170},{1998,1155},{1998,1217},{1993,1218},{1975,1227},
       {1945,1238},{1917,1246},{1904,1248},{1896,1250},{1879,1252},{1873,1253},
       {1841,1255},{1819,1255},{1817,1254},{1806,1254},{1804,1253},{1790,1252},
       {1788,1251},{1769,1247},{1748,1240},{1742,1237},{1723,1228},{1696,1207},
       {1675,1182},{1672,1175},{1666,1167},{1665,1162},{1659,1150},{1652,1127},
       {1651,1119},{1650,1117},{1648,1099},{1647,1081},{1648,1057},{1649,1055},
       {1650,1042},{1651,1040},{1657,1017},{1768,1039}};
  static XPoint  polyf5[] =
    {{1018,905},{1058,905},{1058,950},{1101,929},{1133,916},{1159,908},
       {1170,907},{1172,906},{1188,905},{1199,905},{1201,906},{1213,907},
       {1215,908},{1228,911},{1247,921},{1264,938},{1266,943},{1270,948},
       {1277,966},{1281,982},{1284,1002},{1285,1024},{1285,1203},{1293,1212},
       {1313,1213},{1315,1214},{1324,1214},{1326,1215},{1329,1215},{1329,1250},
       {1139,1250},{1139,1215},{1164,1212},{1167,1211},{1174,1202},{1174,1041},
       {1173,1039},{1172,1024},{1171,1022},{1167,1008},{1159,995},{1148,986},
       {1136,981},{1122,978},{1106,977},{1083,977},{1081,978},{1060,979},
       {1058,981},{1058,1203},{1068,1212},{1083,1213},{1085,1214},{1092,1214},
       {1094,1215},{1096,1215},{1096,1250},{902,1250},{902,1215},{934,1212},
       {938,1212},{939,1209},{945,1203},{945,977},{897,977},{897,945},
       {1017,906},{1018,905}};
  static XPoint  *(polyf[]) = {polyf0, polyf1, polyf2, polyf3, polyf4,
				 polyf5, polyf6, polyf7};
  int  pflen[] = {sizeof(polyf0)/sizeof(polyf0[0]),
		    sizeof(polyf1)/sizeof(polyf1[0]),
		    sizeof(polyf2)/sizeof(polyf2[0]),
		    sizeof(polyf3)/sizeof(polyf3[0]),
		    sizeof(polyf4)/sizeof(polyf4[0]),
		    sizeof(polyf5)/sizeof(polyf5[0]),
		    sizeof(polyf6)/sizeof(polyf6[0]),
		    sizeof(polyf7)/sizeof(polyf7[0])};
  XPoint  tx[200];
  Display  *dpy = butEnv_dpy(env);
  GC  gc = butEnv_gc(env);
  int  i, j;

  butEnv_setXFg(env, XIO_PIC_STAMP);
  for (i = 0;  i < 8;  ++i)  {
    for (j = 0;  j < pflen[i];  ++j)  {
      tx[j].x = x + (polyf[i][j].x * w + 1024) / 2048;
      tx[j].y = y + (polyf[i][j].y * h + 1024) / 2048;
    }
    XFillPolygon(dpy, pm, gc, tx, pflen[i], Complex, CoordModeOrigin);
  }
}



static ButOut  iresize(ButWin *win)  {
  int  x, y, winw, winh;
  int  gw, gh, g0x, g0y;
  bd_loc_t  loc;
  Xio  *xio;
  uchar  *bgcmap;

  xio = butEnv_packet(butWin_env(win));
  winw = butWin_w(win);
  winh = butWin_h(win);
  gw = winw / 20;
  gh = winh / 20;
  g0x = (winw - 19*gw) / 2;
  g0y = (winh - 19*gh) / 2;
  for (x = 0;  x < 19;  ++x)  {
    for (y = 0;  y < 19;  ++y)  {
      loc = bd_xy_loc(x, y);
      but_resize(xio->iconbuts[loc], g0x+x*gw, g0y+y*gh, gw, gh);
    }
  }
  if (xio->ibgpic != None)
    XFreePixmap(butEnv_dpy(xio->env), xio->ibgpic);
  xio->ibgpic = None;
  bgcmap = xio_plasma();
  if (color_enabled)  {
    if (xio->color == TrueColor)  {
      xio->ibgpic = cm2pm_chop(xio, bgcmap, 512,512, winw,winh,
			       XIO_PIC_BOARD(0), 256, 0, 0);
    } else  {
      xio->ibgpic = cm2pm_chop(xio, bgcmap, 512,512, winw,winh,
			       XIO_PIC_BOARD(0), 8, 0, 0);
    }
  }
  xio->ibgpic = decorate_bg(xio, xio->ibgpic, winw, winh, gw, g0x, g0y, TRUE);
  butPixmap_setPic(xio->ibgbut, xio->ibgpic, 0,0);
  but_resize(xio->ibgbut, 0,0, winw,winh);
  return(0);
}


static ButOut  destroy(ButWin *win)  {
  static char  geom[100];

  sprintf(geom, "%dx%d%+d%+d",
	  butWin_w(win), butWin_h(win), butWin_x(win), butWin_y(win));
  clp_setStr(pe_clp, "geometry", geom);
  if (!xio_safe_shutdown)
    xio_shutdown(NULL);
  return(0);
}


static ButOut  kill_copyright(ButTimer *timer)  {
  Xio  *xio = butTimer_packet(timer);

  xio->coptimer = NULL;
  butTimer_destroy(timer);
  but_destroy(xio->copwords1);
  but_destroy(xio->copwords2);
  but_destroy(xio->copwords3);
  but_destroy(xio->copwords4);
  but_destroy(xio->copbox);
  but_destroy(xio->copback);
  xio->copbox = NULL;
  return(0);
}


void  xio_m_set_turn(Xio *xio, int turn, int oldturn)  {
  char  textout[2];

  if (oldturn == -1)
    oldturn = turn + 111;
  textout[1] = '\0';
  textout[0] = (turn % 10) + '0';
  butText_set(xio->digits[0], textout);
  if (turn/10 != oldturn/10)  {
    if (turn <= 9)
      textout[0] = '\0';
    else
      textout[0] = ((turn / 10) % 10) + '0';
    butText_set(xio->digits[1], textout);
  }
  if (turn/100 != oldturn/100)  {
    if (turn <= 99)
      textout[0] = '\0';
    else
      textout[0] = ((turn / 100) % 10) + '0';
    butText_set(xio->digits[1], textout);
  }
}


#endif  /* X11_DISP */
