/*
 * src/comp/eval.h, part of Pente (game program)
 * Copyright (C) 1994 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#ifdef  COMPLEX
#undef  COMPLEX
#endif
#ifdef  ISCORE
#define  COMPLEX
#endif
#ifdef  OSCORE
#define  COMPLEX
#endif

{
  uint  ev_fcaps, ev_dir;
  int   ev_dvec;
  uint  ev_fcg, ev_ecg, ev_ffg, ev_efg, ev_fbg, ev_ebg, ev_temp;
  bd_loc_t  ev_ffloc, ev_fbloc, ev_efloc, ev_ebloc;
#ifdef  OSCORE
  uint  ev_ef3, ev_ef4, ev_ecaps;
#endif
#ifdef  CPM
  uint  ev_ff3, ev_ff4;
#endif
  
  ev_fcaps = BOARD->captures[PLAYER];
#ifdef  CPM
  ev_ff4 = 0; ev_ff3 = 0;
#endif
  SSCORE = 0;
#ifdef  ISCORE
  ISCORE = FALSE;
#endif  /* ISCORE */
#ifdef  OSCORE
  OSCORE = 0;
  ev_ecaps = BOARD->captures[PLAYER^1];
  ev_ef3 = 0;
  ev_ef4 = 0;
#endif  /* OSCORE */
  for (ev_dir = 0;  ev_dir < 4;  ++ev_dir)  {
    ev_fcg = 1;  ev_ecg = 1;  ev_ffg = 0; ev_efg = 0;  ev_fbg = 0;  ev_ebg = 0;
    ev_dvec = bd_dvec[ev_dir];
    ev_efloc = ev_ffloc = LOC + ev_dvec;
    if (BOARD->grid[ev_ffloc] == BD_PLAYER(PLAYER))  {
      do  {
	++ev_fcg;
	ev_ffloc += ev_dvec;
      } while (BOARD->grid[ev_ffloc] == BD_PLAYER(PLAYER));
      if (ev_fcg == 3)  {
	if (BOARD->grid[ev_ffloc] == BD_OPP(PLAYER))  {
	  SSCORE += V_FORCE(0);
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
#ifdef  OSCORE
	  OSCORE += comp_capval[ev_ecaps++];
#endif
	}
#ifdef  OSCORE
	else if (BD_EMPTYP(BOARD->grid[ev_ffloc]))
	  OSCORE += V_FORCE(0);
#endif
      }
    } else if (BOARD->grid[ev_ffloc] == BD_OPP(PLAYER))  {
#ifndef  COMPLEX
      if (BOARD->grid[ev_ffloc+ev_dvec] == BD_OPP(PLAYER))  {
	if (BOARD->grid[ev_ffloc+ev_dvec+ev_dvec] == BD_PLAYER(PLAYER))  {
	  SSCORE += comp_capval[ev_fcaps++];
	} else if (BD_EMPTYP(BOARD->grid[ev_ffloc+ev_dvec+ev_dvec]))
	  SSCORE += V_FORCE(DEGEN);
      }
#else  /* COMPLEX */
      do  {
	++ev_ecg;
	ev_efloc += ev_dvec;
      } while (BOARD->grid[ev_efloc] == BD_OPP(PLAYER));
      if (ev_ecg == 3)  {
	if (BOARD->grid[ev_efloc] == BD_PLAYER(PLAYER))  {
	  SSCORE += comp_capval[ev_fcaps++];
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
#ifdef  OSCORE
	  OSCORE += V_FORCE(0);
#endif
	} else if (BD_EMPTYP(BOARD->grid[ev_efloc]))  {
	  SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
	}
      }
#endif  /* COMPLEX */
    }
    if (BD_EMPTYP(BOARD->grid[ev_ffloc]) &&
	(BOARD->grid[ev_ffloc + ev_dvec] == BD_PLAYER(PLAYER)))  {
      ++ev_ffg;
      ev_ffloc = ev_ffloc + ev_dvec + ev_dvec;
      while (BOARD->grid[ev_ffloc] == BD_PLAYER(PLAYER))  {
	++ev_ffg;
	ev_ffloc += ev_dvec;
      }
    }
#ifdef  COMPLEX
    else if (BD_EMPTYP(BOARD->grid[ev_efloc]) &&
	     (BOARD->grid[ev_efloc + ev_dvec] == BD_OPP(PLAYER)))  {
      ++ev_efg;
      ev_efloc = ev_efloc + ev_dvec + ev_dvec;
      while (BOARD->grid[ev_efloc] == BD_OPP(PLAYER))  {
	++ev_efg;
	ev_efloc += ev_dvec;
      }
    }
#endif
    ev_ebloc = ev_fbloc = LOC - ev_dvec;
    if (BOARD->grid[ev_fbloc] == BD_PLAYER(PLAYER))  {
      ev_temp = 1;
      ev_fbloc -= ev_dvec;
      while (BOARD->grid[ev_fbloc] == BD_PLAYER(PLAYER))  {
	++ev_temp;
	ev_fbloc -= ev_dvec;
      }
      ev_fcg += ev_temp;
      if (ev_temp == 2)  {
	if (BOARD->grid[ev_fbloc] == BD_OPP(PLAYER))  {
	  SSCORE += V_FORCE(0);
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  OSCORE
	  OSCORE += comp_capval[ev_ecaps++];
#endif
	}
#ifdef  OSCORE
	else if (BD_EMPTYP(BOARD->grid[ev_fbloc] == BD_OPP(PLAYER)))
	  OSCORE += V_FORCE(0);
#endif
      }
    } else if (BOARD->grid[ev_fbloc] == BD_OPP(PLAYER))  {
#ifndef  COMPLEX
      if (BOARD->grid[ev_fbloc-ev_dvec] == BD_OPP(PLAYER))  {
	if (BOARD->grid[ev_fbloc-ev_dvec-ev_dvec] == BD_PLAYER(PLAYER))  {
	  SSCORE += comp_capval[ev_fcaps++];
	} else if (BD_EMPTYP(BOARD->grid[ev_fbloc-ev_dvec-ev_dvec]))
	  SSCORE += V_FORCE(DEGEN);
      }
#else  /* COMPLEX */
      ev_temp = 1;
      ev_ebloc -= ev_dvec;
      while (BOARD->grid[ev_ebloc] == BD_OPP(PLAYER))  {
	++ev_temp;
	ev_ebloc -= ev_dvec;
      }
      ev_ecg += ev_temp;
      if (ev_temp == 2)  {
	if (BOARD->grid[ev_ebloc] == BD_PLAYER(PLAYER))  {
	  SSCORE += comp_capval[ev_fcaps++];
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
#ifdef  OSCORE
	  OSCORE += V_FORCE(0);
#endif
	} else if (BD_EMPTYP(BOARD->grid[ev_ebloc]))  {
	  SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
	}
      }
#endif  /* COMPLEX */
    }
    if (BD_EMPTYP(BOARD->grid[ev_fbloc]) &&
	(BOARD->grid[ev_fbloc - ev_dvec] == BD_PLAYER(PLAYER)))  {
      ++ev_fbg;
      ev_fbloc = ev_fbloc - ev_dvec - ev_dvec;
      while (BOARD->grid[ev_fbloc] == BD_PLAYER(PLAYER))  {
	++ev_fbg;
	ev_fbloc -= ev_dvec;
      }
    }
#ifdef  COMPLEX
    else if (BD_EMPTYP(BOARD->grid[ev_ebloc]) &&
	     (BOARD->grid[ev_ebloc - ev_dvec] == BD_OPP(PLAYER)))  {
      ++ev_ebg;
      ev_ebloc = ev_ebloc - ev_dvec - ev_dvec;
      while (BOARD->grid[ev_ebloc] == BD_OPP(PLAYER))  {
	++ev_ebg;
	ev_ebloc -= ev_dvec;
      }
    }
#endif  /* COMPLEX */
    switch(ev_fcg)  {
    case 1:
      if ((ev_ffg == 2) && BD_EMPTYP(BOARD->grid[ev_ffloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_fbloc]) || (ev_fbg != 0)))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff3;
#endif  /* CPM */
      } else if (ev_ffg >= 3)  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff4;
#endif  /* CPM */
      }
      if ((ev_fbg == 2) && BD_EMPTYP(BOARD->grid[ev_fbloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_ffloc]) || (ev_ffg != 0)))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff3;
#endif  /* CPM */
      } else if (ev_fbg >= 3)  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff4;
#endif  /* CPM */
      }
      break;
    case 2:
      ev_temp = 0;
      if (ev_ffg != 0)
	ev_temp = 1;
      else  {
	if (BD_EMPTYP(BOARD->grid[ev_ffloc]))
	  ev_temp = 1;
	else if (BOARD->grid[ev_ffloc] == BD_OPP(PLAYER))
	  ev_temp = 2;
      }
      if (ev_fbg != 0)
	ev_temp |= 1;
      else  {
	if (BD_EMPTYP(BOARD->grid[ev_fbloc]))
	  ev_temp |= 1;
	else if (BOARD->grid[ev_fbloc] == BD_OPP(PLAYER))
	  ev_temp |= 2;
      }
      if (ev_temp == 3)
	SSCORE -= 2*V_FORCE(DEGEN);
      if ((ev_ffg == 1) && BD_EMPTYP(BOARD->grid[ev_ffloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_fbloc]) || (ev_fbg != 0)))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff3;
#endif  /* CPM */
      } else if (ev_ffg >= 2)  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff4;
#endif  /* CPM */
      }
      if ((ev_fbg == 1) && BD_EMPTYP(BOARD->grid[ev_fbloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_ffloc]) || (ev_ffg != 0)))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff3;
#endif  /* CPM */
      } else if (ev_fbg >= 2)  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff4;
#endif  /* CPM */
      }
      break;
    case 3:
      if ((ev_ffg == 0) && (ev_fbg == 0) &&
	  BD_EMPTYP(BOARD->grid[ev_fbloc]) &&
	  BD_EMPTYP(BOARD->grid[ev_ffloc]))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff3;
#endif  /* CPM */
      } else  {
	if (ev_ffg)  {
	  SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	  ++ev_ff4;
#endif  /* CPM */
	}
	if (ev_fbg)  {
	  SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	  ++ev_ff4;
#endif  /* CPM */
	}
      }
      break;
    case 4:
      if ((ev_ffg != 0) || (BD_EMPTYP(BOARD->grid[ev_ffloc])))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff4;
#endif  /* CPM */
      }
      if ((ev_fbg != 0) || (BD_EMPTYP(BOARD->grid[ev_fbloc])))  {
	SSCORE += V_FORCE(DEGEN);
#ifdef  ISCORE
	ISCORE = TRUE;
#endif  /* ISCORE */
#ifdef  CPM
	++ev_ff4;
#endif  /* CPM */
      }
      break;
    default:
      SSCORE = V_WIN(DEGEN);
#ifdef  ISCORE
      ISCORE = TRUE;
#endif  /* ISCORE */
    }
#ifdef  COMPLEX
    switch(ev_ecg)  {
    case 1:
      if ((ev_efg == 2) && BD_EMPTYP(BOARD->grid[ev_efloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_ebloc]) || (ev_ebg != 0)))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef3;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      } else if (ev_efg >= 3)  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef4;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      }
      if ((ev_ebg == 2) && BD_EMPTYP(BOARD->grid[ev_ebloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_efloc]) || (ev_efg != 0)))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef3;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      } else if (ev_ebg >= 3)  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef4;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      }
      break;
    case 2:
#ifdef  OSCORE
      ev_temp = 0;
      if (ev_efg != 0)
	ev_temp = 1;
      else  {
	if (BD_EMPTYP(BOARD->grid[ev_efloc]))
	  ev_temp = 1;
	else if (BOARD->grid[ev_efloc] == BD_PLAYER(PLAYER))
	  ev_temp = 2;
      }
      if (ev_ebg != 0)
	ev_temp |= 1;
      else  {
	if (BD_EMPTYP(BOARD->grid[ev_ebloc]))
	  ev_temp |= 1;
	else if (BOARD->grid[ev_ebloc] == BD_PLAYER(PLAYER))
	  ev_temp |= 2;
      }
      if (ev_temp == 3)
	OSCORE -= 2*V_FORCE(0);
#endif
      if ((ev_efg == 1) && BD_EMPTYP(BOARD->grid[ev_efloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_ebloc]) || (ev_ebg != 0)))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef3;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      } else if (ev_efg >= 2)  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef4;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef ISCORE
	ISCORE = TRUE;
#endif
      }
      if ((ev_ebg == 1) && BD_EMPTYP(BOARD->grid[ev_ebloc]) &&
	  (BD_EMPTYP(BOARD->grid[ev_efloc]) || (ev_efg != 0)))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef3;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      } else if (ev_ebg >= 2)  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef4;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      }
      break;
    case 3:
      if ((ev_efg == 0) && (ev_ebg == 0) &&
	  BD_EMPTYP(BOARD->grid[ev_ebloc]) &&
	  BD_EMPTYP(BOARD->grid[ev_efloc]))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef3;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      } else  {
	if (ev_efg)  {
#ifdef  OSCORE
#ifdef  CPM
	  ++ev_ef4;
#endif
	  OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
	}
	if (ev_ebg)  {
#ifdef  OSCORE
#ifdef  CPM
	  ++ev_ef4;
#endif
	  OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	  ISCORE = TRUE;
#endif
	}
      }
      break;
    case 4:
      if ((ev_efg != 0) || (BD_EMPTYP(BOARD->grid[ev_efloc])))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef4;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      }
      if ((ev_ebg != 0) || (BD_EMPTYP(BOARD->grid[ev_ebloc])))  {
#ifdef  OSCORE
#ifdef  CPM
	++ev_ef4;
#endif
	OSCORE += V_FORCE(0);
#endif
#ifdef  ISCORE
	ISCORE = TRUE;
#endif
      }
      break;
    default:
#ifdef  OSCORE
      OSCORE = V_WIN(0);
#endif
#ifdef  ISCORE
      ISCORE = TRUE;
#endif
      break;
    }
#endif  /* COMPLEX */
  }
#ifdef  CPM
  if (ev_ff4 > 1)  {
    SSCORE += V_D4FORCE(DEGEN);
  } else if (ev_ff3 + ev_ff4 > 1)  {
    SSCORE += V_D3FORCE(DEGEN);
  }
#ifdef  OSCORE
  if (ev_ef4 > 1)  {
    OSCORE += V_D4FORCE(0);
  } else if (ev_ef3 + ev_ef4 > 1)  {
    OSCORE += V_D4FORCE(0);
  }
#endif
#endif
}
