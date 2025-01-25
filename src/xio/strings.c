/*
 * $Source: /home/wms/cvs/pente/src/xio/strings.c,v $
 * $Revision: 1.3 $
 * $Date: 2001/07/29 17:08:21 $
 * 
 * src/xio/strings.c, part of Pente (game program)
 * Copyright (C) 1994-1995 William Shubert.
 * See "configure.h.in" for more copyright information.
 */

#include <wms.h>

#ifdef  X11_DISP

#include <but/but.h>
#include <but/text.h>
#include <but/menu.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <abut/abut.h>
#include <abut/msg.h>
#include "../pente.h"
#include "xio.h"

/* Names of the languages...in each language! */
const char  *xioStr_langlist[XIO_LANG_COUNT][XIO_LANG_COUNT+1] = {
  {"English", "French", "Italian", BUTMENU_OLEND},
  {"Anglais", "Français", "Italian", BUTMENU_OLEND},
  {"Inglese", "Francese", "Italiano", BUTMENU_OLEND}};

/*
 * Fonts to use in each language.
 *   If more than one will do, put the first choice with a "/" then the next,
 *   then the next, etc.
 */
char  *xioStr_mfonts[] = {
  "-bitstream-charter-medium-r-normal--%d-*-*-*-*-*-*-*",
  "-bitstream-charter-medium-r-normal--%d-*-*-*-*-*-*-*",
  "-bitstream-charter-medium-r-normal--%d-*-*-*-*-*-*-*"};
char *xioStr_bfonts[] = {
  "-bitstream-charter-black-r-normal--%d-*-*-*-*-*-*-*/"
  "-bitstream-charter-bold-r-normal--%d-*-*-*-*-*-*-*",
  "-bitstream-charter-black-r-normal--%d-*-*-*-*-*-*-*/"
  "-bitstream-charter-bold-r-normal--%d-*-*-*-*-*-*-*",
  "-bitstream-charter-black-r-normal--%d-*-*-*-*-*-*-*/"
  "-bitstream-charter-bold-r-normal--%d-*-*-*-*-*-*-*"};

/* Buttons in the setup window. */
char  *xioStr_pentesetup[] = {"Pente Setup", "Configuration de Pente",
			    "Configurazione di Pente"};
char  *xioStr_language[] = {"Language", "Langue", "Lingua"};
char  *xioStr_color[] = {"Color", "Couleur", "Colore"};
char  *xioStr_showThink[] = {"Show thinking", XIO_COMPCHAR " visible",
			   XIO_COMPCHAR " visibile"};
char  *xioStr_soundvol[] = {"Sound Volume", "Volume Sonore",
			      "Volume del suono"};
char  *xioStr_off[] = {"Off", "Aucun", "Spento"};
char  *xioStr_max[] = {"Max", "Max", "Massimo"};
char  *xioStr_transtab[] = {"Transposition Table", "Table de Transposition",
			  "Tabella di trasposizione"};
char  *xioStr_size[] = {"Size:", "Taille:", "Dimensione:"};
char  *xioStr_autosize[] = {"Autosize", "Taille Automatique",
			  "Automatica"};
char  *xioStr_netplay[] = {"Network Play", "Jeu en Réseau",
			 "Gioco in rete"};
char  *xioStr_enabled[] = {"Enabled", "Activé", "Abilitato"};
char  *xioStr_socket[] = {"Port:", "Port:", "Porta:"};
char  *xioStr_disabled[] = {"Disabled", "Désactivé", "Disattivato"};
char  *xioStr_connect[] = {"Connect", "Connexion", "Connetti"};
char  *xioStr_disconnect[] = {"Disconnect", "Déconnexion", "Disconnetti"};
char  *xioStr_noSockets[] = {
  "Sorry, this program was compiled on a system that doesn't support "
    "sockets.  You cannot play networked.",
  "Désolé, ce programme a été compilé sur un système ne "
    "gérant pas les sockets.  Vous ne pouvez pas jouer en réseau.",
  "Spiacente, il programma è stato compilato su un sistema che non "
    "supporta i socket.  Non puoi giocare in rete."};

/* Labels on the board. */
char  *xioStr_turn[] = {"Turn", "Tour", "Turno"};
char  *xioStr_captures[] = {"CAPTURES", "CAPTURES", "CATTURATE"};
char  *xioStr_copyright[] = {"Copyright \251 1994-1995 William Shubert.",
			    "Copyright \251 1994-1995 William Shubert.",
			   "Copyright \251 1994-1995 William Shubert."};
char  *xioStr_nowarr[] = {"Pente comes with ABSOLUTELY NO WARRANTY and "
			     "is free software.",
			     "Pente est fourni sans AUCUNE GARANTIE et "
			     "est un logiciel libre",
			"Pente è fornito senza ALCUNA GARANZIA ed "
			"è un programma libero."};
char  *xioStr_seehelp[] = {"Please see the \"Copying and Non-Warranty\" help "
			      "page for details.",
			    "Veuillez consulter la page d'aide \""
			    "Copie et Non-Garantie\" pour plus de détails.",
			 "Si prega di consultare la pagina di aiuto \""
			    "Copia e Non-Garanzia\" per maggiori dettagli."};

/* Button labels. */
char  *xioStr_undo[] = {"Undo", "Défaire", "Indietro"};
char  *xioStr_start[] = {"Start", "Débuter", "Inizia"};
char  *xioStr_stop[] = {"Stop", "Arrêter", "Ferma"};
char  *xioStr_continue[] = {"Continue", "Continuer", "Continua"};
char  *xioStr_redo[] = {"Redo", "Refaire", "Ripeti"};
char  *xioStr_setup[] = {"Setup", "Config.", "Configura"};
char  *xioStr_help[] = {"Help", "Aide", "Aiuto"};
char  *xioStr_quit[] = {"Quit", "Quitter", "Esci"};
char  *xioStr_ok[] = {"OK", "OK", "OK"};
char  *xioStr_cancel[] = {"Cancel", "Annuler", "Annulla"};
char  *xioStr_netConn[] = {"Network Connect", "Connexion Réseau", "Connessione in rete"};
char  *xioStr_machine[] = {"Machine:", "Machine:", "Macchina:"};
char  *xioStr_reject[] = {"Reject", "Rejet", "Rifiuta"};
char  *xioStr_noRemote[] = {"Sorry, Pente " VERSION " doesn't have "
			       "networking yet.  Please wait for a later "
			       "version to arrive!",
			     "Désolé, Pente " VERSION " n'a pas encore "
                               "la fonctionnalité réseau.  Veuillez "
			       "attendre une prochaine version!", 
			     "Spiacente, Pente " VERSION " non ha ancora "
			       "la funzionalità di rete. Si prega di "
			       "attendere una versione successiva!"};

/* Menu entries. */
char  *xioStr_player1[] = {"Player 1", "Joueur 1", "Giocatore 1"};
char  *xioStr_player2[] = {"Player 2", "Joueur 2", "Giocatore 2"};
char  *xioStr_human[] = {"Human", "Humain", "Umano"};
char  *xioStr_remote[] = {"Remote", "Distant", "Remoto"};
char  *xioStr_comp1[] = {XIO_COMPCHAR " Simple",
			    XIO_COMPCHAR " Simple", XIO_COMPCHAR " Semplice"};
char  *xioStr_comp2[] = {XIO_COMPCHAR " Easy",
			    XIO_COMPCHAR " Facile", XIO_COMPCHAR " Facile"};
char  *xioStr_comp3[] = {XIO_COMPCHAR " Med",
			    XIO_COMPCHAR " Moyen", XIO_COMPCHAR " Medio"};
char  *xioStr_comp4[] = {XIO_COMPCHAR " Hard",
			    XIO_COMPCHAR " Difficile", XIO_COMPCHAR " Difficile"};
char  *xioStr_comp5[] = {XIO_COMPCHAR " Expert",
			    XIO_COMPCHAR " Expert", XIO_COMPCHAR " Esperto"};
char  *xioStr_comp6[] = {XIO_COMPCHAR " Master",
			    XIO_COMPCHAR " Maître", XIO_COMPCHAR " Maestro"};

/* Messages for message windows. */
char  *xioStr_notEnoughColors[] = {"Pente couldn't allocate enough colors.  "
				     "It will still run, but in black and "
				     "white.",
				   "Pas assez de couleurs.", 
				   "Pente non è riuscito ad allocare "
				     "abbastanza colori. Funzionerà "
				     "ugualmente, ma in bianco e nero."};

/* Messages to be used for network play. */
char  *xioStr_netOffer[] = {"Player \"%s\" wants to play pente against "
			       "you.  Accept?", 
			       "Le joueur \"%s\" souhaite "
			       "jouer à pente contre vous. Acceptez-vous ?", 
			       "Il giocatore \"%s\" vuole giocare a pente "
			       "contro di te. Accetti?"};
char  *xioStr_netReject[] = {"Player \"%s\" rejected your offer to play "
				"pente.", 
				"Le joueur \"%s\" a rejeté votre "
			        "offre de jouer à pente.", 
			        "Il giocatore \"%s\" ha rifiutato il vostro "
			        "invito di giocare a pente."};
char  *xioStr_netWait[] = {"Waiting for connection to be accepted.  Press "
			      "\"Cancel\" to give up.",
			      "En attente de la validation de la connexion."
			      "  Appuyer sur \"Annuler\" pour abandonner.", 
			      "In attesa che la connessione sia accettata."
			      "  Premere \"Annulla\" per abbandonare."};
char  *xioStr_netDisconnect[] = {"Your connection with \"%s\" has been "
				    "disconnected because of \"%s\".",
				    "La connexion avec \"%s\" a été "
				    "interrompue à cause de \"%s\".", 
				    "La connessione con \"%s\" è stata "
				    "interrotta da \"%s\"."};
char  *xioStr_netBusy[] = {"Sorry, player \"%s\" is already playing a "
			      "networked game of pente.",
			      "Désolé, le joueur \"%s\" est déjà en train de "
			      "jouer une partie de pente en réseau.", 
			      "Spiacente, il giocatore \"%s\" sta già "
			      "giocando a pente in rete."};
char  *xioStr_netClosed[] = {"Your opponent has closed your network "
			       "connection.",
			     "Votre adversaire a fermé la connexion.", 
			     "Il vostro avversario ha terminato "
			     "la connessione."};

/* Error messages! */
char  *xioStr_errProtSocket[] = {"You can't use local port %s. Ports up "
				    "to %d are reserved for superuser access "
				    "only.",
				    "Vous ne pouvez pas utiliser le port %s. "
				    "Les ports jusqu'à %d sont réservés à "
				    "l'accès pour le super utilisateur "
				    "uniquement.", 
				    "Non potete utilizzare la porta %s. "
				    "Le porte fino a %d sono riservate "
				    "al super utente."};
char  *xioStr_errSockInUse[] = {"Port %s is already in use on this "
				   "machine.  If you want to play networked, "
				   "you will have to choose a different "
				   "port number.", "Le port %s est déjà "
				   "utilisé sur cette machine. Si vous "
				   "souhaitez jouer en réseau, vous devez "
				   "choisir un numéro de port différent.", 
				   "La porta %s è già in uso su questa "
				   "macchina. Se volete giocare in rete, "
				   "dovete scegliere un numero di porta "
				   "diverso."};
char  *xioStr_errLSockGeneric[] = {"Error opening local port %s: \"%s\".",
				      "Erreur d'ouverture du port local %s: "
				      " \"%s\".", 
				      "Errore durante l'apertura della "
				      "porta locale %s: \"%s\"."};
char  *xioStr_errSockNumInvalid[] = {
  "\"%s\" is not a valid port.  Ports are integers in the "
    "range from 1 through %d.  The default Pente port is 15023; "
    "you should use that unless port 15023 is already in use.",
    "\"%s\" n'est pas un port valide. Les ports valides sont les "
    "entiers qui vont " "de 1 à %d.  Le port de Pente par défaut "
    "est 15023; c'est celui que vous devriez utiliser à moins que "
    "le port 15023 soit déjà utilisé.", 
    "\"%s\" non è una porta valida. Le porte valide sono gli interi "
    "nell'intervallo che va da 1 a %d. La porta standard di Pente "
    "è 15023; dovreste usare quella a meno che non sia già in uso."};
char  *xioStr_errRSockGeneric[] = {"Error \"%s\" occurred while opening "
				      "port %s on machine \"%s\".",
				      "L'erreur \"%s\" est survenue lors de "
				      "l'ouverture du port %s sur la "
				      "machine \"%s\".", 
				      "L'errore \"%s\" si è verificato "
				      "durante l'apertura della porta %s "
				      "sulla macchina \"%s\"."};
char  *xioStr_errRSockRefused[] = {"Your connection to machine \"%s\" port "
				      "%s was refused.  Probably there is no "
				      "Pente running that is set up to "
				      "listen on that port.",
				      "La connexion à la machine \"%s\" sur "
				      "le port %s a été refusée. Il n'y a "
				      "probablement aucun Pente de "
				      "configuré pour écouter sur ce port.", 
				      "La vostra connessione alla macchina "
				      "\"%s\" è stata rifiutata. Probabilmente "
				      "non c'è nessun Pente configurato per "
				      "ascoltare su quella porta."};
char  *xioStr_errHostTemp[] = {"Machine \"%s\" couldn't be found.  This may "
				  "be a temporary situation; you may want to "
				  "try again later.",
				  "La machine \"%s\" n'a pas pu être "
				  "localisée.  Il se peut que ce soit un "
				  "problème temporaire; vous "
				  "pouvez réessayer plus tard.", 
				  "La macchina \"%s\" non è stata trovata. "
				  "Potrebbe trattarsi di un problema temporaneo; "
				  "si prega di riprovare più tardi."};
char  *xioStr_errHostPerm[] = {"Machine \"%s\" couldn't be found.",
			        "Impossible de localiser la machine \"%s\".",
			        "La macchina \"%s\" non è stata trovata."};


/* Help */
char  *xioStr_hmenu[] = {"Help topics", "Sujets D'aide", "Argomenti Della Guida"};
char  *xioStr_phelp[] = {"Program Help", "Aide Du Programme", "Aiuto Del Programma"};
char  *xioStr_ghelp[] = {"How To Play Pente", "Comment Jouer A Pente", "Come Giocare A Pente"};
char  *xioStr_shelp[] = {"The Setup Window", "La Fenêtre De Configuration", "La Finestra Di Configurazione"};
char  *xioStr_nhelp[] = {"Network Play", "Jeu En Réseau", "Gioco In Rete"};
char  *xioStr_cphelp[] = {"About The Computer Players", "A Propos Des "
			     "Joueurs Gérés Par L'ordinateur", "I Giocatori Controllati Dalla Macchina"};
char  *xioStr_ahelp[] = {"About the Author", "A Propos De L'auteur", "Informazioni Sull'Autore"};
char  *xioStr_chelp[] = {"Copying and Non-Warranty", "Copie Et Non-Garantie", "Copia E Non-Garanzia"};

/* Help on using the program. */
static xio_tb_t  proghelp_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "Help on the program Pente"},
  {butText_just, 0, "   This help page explains how to use the features of "
     "Pente version " VERSION ".  At the top of this help window is a menu; "
     "click on it and drag with the mouse to pop up a list of other help "
     "pages available."},
  {butText_just, 0, ""},
  {butText_just, 0, "   To scroll the text in this help window, you may "
     "either use the slider bar to the left or right-click and drag on the "
     "text itself."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The main window of Pente shows the pente board with "
     "some controls at the bottom.  The board itself looks like a regular "
     "pente board and should be mostly self-explanatory.  Simply click on an "
     "intersection to place a piece there.  To either side of "
     "the pente board are pits where captured pieces are kept.  Below the "
     "board are eight controls.  These are the player 1 selector, the player "
     "2 selector, the undo button, the redo button, the start/stop button, "
     "the setup button, the help button, and the quit button."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The player 1 and player 2 selectors are menus "
     "found on the far left and far right of the controls.  "
     "When the menu is opened by pressing the left mouse button, there are "
     "several choices.  The first choice, "
     "\"Human\", lets you play that player.  For example, if both selectors "
     "are set to \"Human\", then two people can use the same computer to "
     "play pente against each other."},
  {butText_just, 0, "   The next selection in a player selector is "
     "\"Remote\".  If your Pente program is configured correctly, then this "
     "option can be used to play pente against somebody on another computer.  "
     "See the networking help page for more information on this."},
  {butText_just, 0, "   The bottom six options in the player selectors are "
     "the computer opponents, represented by a " XIO_COMPCHAR " icon and the "
     "difficulty of that opponent.  Selecting one of these players makes the "
     "computer play for that player.  For example, setting the player 1 "
     "selector to \"Human\" and the player 2 selector to \"" XIO_COMPCHAR
     " Simple\" will let you play as player 1, and make the computer play "
     "for player 2.  This particular computer opponent is very easy to beat.  "
     "The computer players are listed from easiest to beat to hardest to "
     "beat in the player selector."},
  {butText_just, 0, ""},
  {butText_just, 0, "   In the upper center of the controls is the start/"
     "stop button.  This button will start a new game when no game is "
     "being played, or stop a game in progress.  The label on the button "
     "will change to reflect it's action."},
  {butText_just, 0, "   Sometimes the game will be paused (for example, if "
     "you change players in the middle of a game).  When this happens the "
     "label on the start/stop button will change to \"Continue\".  Pressing "
     "the button then will resume the game."},
  {butText_just, 0, ""},
  {butText_just, 0, "   To either side of the start/stop button are the "
     "undo and redo buttons.  Pressing undo will take back moves, one at a "
     "time.  Pressing redo will replay moves, one at a time.  Using these "
     "buttons you can review a game to see what moves led to the current "
     "position."},
  {butText_just, 0, "   Undo can also be used to take back a move.  If you "
     "press undo until the move you want to take back is gone, then move "
     "somewhere else, the game will continue from there."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The help button is in the bottom center of the "
     "controls.  Since you're reading this, you must already know how to "
     "use this button."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The setup button, to the left of the help button, "
     "lets you change the configuration of Pente.  See the setup window "
     "help page for more information."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The quit button, which is to the right of the help "
     "button, exits Pente immediately."},
  {butText_just, 0, ""},
  {butText_just, 0, "   There are some keyboard accelerators available as "
     "well.  Pressing \"Enter\" or \"Return\" is the same as pressing the "
     "\"OK\" button in a window.  Pressing \"H\" or \"Help\" is the same as "
     "pressing the \"Help\" button.  Pressing \"Q\" in the main window will "
     "quit the program.  Pressing \"S\" will toggle sound on and off.  "
     "The left and right arrow keys will undo and redo moves."},
  {butText_just, 0, "   If you press \"Q\" by accident and lose your game, "
     "don't worry!  Just restart Pente and your game will still be there, "
     "exactly where you left off."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  proghelp_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "Aide du programme Pente"},
  {butText_just, 0, "   Cette page d'aide explique comment utiliser les "
     "caractéristiques de Pente version " VERSION ".  En haut de cette fenêtre "
     "d'aide se trouve un menu; cliquer dessus et tirer avec la "
     "souris pour accéder à une liste des pages d'aide disponibles."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Pour faire défiler le texte dans cette fenêtre "
     "d'aide, vous pouvez soit utiliser la barre de défilement à gauche, "
     "soit cliquer avec le bouton droit et tirer le texte lui-même."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La fenêtre principale de Pente montre le plateau "
     "de jeu de pente comportant quelques boutons de contrôle en bas. "
     "Le plateau de jeu lui-même ressemble à un plateau de jeu de "
     "pente habituel et devrait être suffisamment explicite. "
     "Il suffit de cliquer sur une "
     "intersection pour mettre un jeton à cet endroit. On trouve de chaque "
     "côté du plateau du jeu de pente des renfoncements où les jetons "
     "capturés sont conservés. On trouve sous le plateau de jeu huit "
     "boutons de contrôle. Il s'agit du sélectionneur du joueur 1, du "
     "sélectionneur du joueur 2, du bouton pour défaire un coup, du "
     "bouton pour refaire un coup, du bouton de début/arrêt de "
     "partie, du bouton de configuration, "
     "du bouton d'aide et du bouton pour quitter."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Les sélectionneurs du joueur 1 et du joueur 2 sont "
     "des menus qui se trouvent à l'extrême gauche et à l'extrême droite "
     "des boutons de contrôle.  Une fois le menu ouvert en appuyant sur "
     "le bouton gauche de la souris, on a accès à plusieurs choix. "
     "Le premier choix \"Humain\", vous "
     "permet d'utiliser ce joueur. Par exemple, lorsque "
     "les deux sélectionneurs "
     "sont positionnés sur \"Humain\", deux personnes peuvent utiliser le "
     "même ordinateur pour jouer l'un contre l'autre."},
  {butText_just, 0, "   Le choix suivant du sélectionneur de joueur est "
     "\"Distant\". Si le programme Pente est configuré correctement cette "
     "option peut être utilisée pour jouer à pente contre quelqu'un sur "
     "un autre ordinateur.  Consultez la page d'aide concernant le réseau pour "
     "obtenir de plus amples information."},
  {butText_just, 0, "   Les six dernières options du sélectionneur de joueur "
     "sont les adversaires gérés par l'ordinateur, elles sont représentées "
     "par une icône " XIO_COMPCHAR " suivi de la difficulté de cet "
     "adversaire. La sélection d'un de ces joueurs permet de jouer contre "
     "ce joueur géré par l'ordinateur. Par exemple, si on positionne le "
     "sélectionneur du joueur 1 à \"Humain\" et le sélectionneur du joueur 2 à "
     "\"" XIO_COMPCHAR " Simple\", on jouera alors en tant que joueur 1, et "
     "l'ordinateur gérera le joueur 2. Cet adversaire là est très facile à "
     "battre. Les joueurs gérés par l'ordinateur sont affichés dans l'ordre "
     "croissant de difficulté dans le sélectionneur de joueur."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Le bouton de début/arrêt se trouve en haut et au "
     "milieu des boutons de contrôle. Ce bouton permet débuter une nouvelle "
     "partie lorsqu'aucune partie n'est en cours, ou d'arrêter un jeu en "
     "cours. Le nom de ce bouton change suivant le contexte."},
  {butText_just, 0, "   Parfois le jeu est en pause (par exemple, si vous "
     "changez les joueurs en plein milieu d'une partie). Dans ce cas, le nom "
     "du bouton de début/arrêt se change en \"Continuer\".  L'appui sur ce "
     "bouton permet alors de reprendre la partie."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Les boutons défaire et refaire se trouvent des deux "
     "côtés du bouton de début/arrêt. L'appui sur défaire permet de "
     "revenir en arrière, d'un coup à la fois. L'appui sur refaire permet "
     "de rejouer les coups, un à la fois. En utilisant ces boutons "
     "vous pouvez compulser une partie afin de voir quels coups ont "
     "conduit à la position actuelle."},
  {butText_just, 0, "   Défaire peut également être utilisé pour revenir "
     "en arrière. Si vous appuyez sur Défaire jusqu'à ce que le coup "
     "que vous souhaitez enlever ne soit plus là, puis si vous bougez "
     "le curseur autre part, alors la partie continuera depuis cette "
     "position."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Le bouton d'aide se trouve au milieu et en bas "
     "des boutons de contrôle. Puisque vous êtes en train de lire ceci, vous "
     "devez déjà savoir comment utiliser ce bouton."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Le bouton de configuration, à gauche du bouton "
     "d'aide, permet de modifier la configuration de Pente. Consultez "
     "la page d'aide de la fenêtre de configuration pour obtenir de "
     "plus amples renseignements."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Le bouton pour quitter, qui se trouve à droite "
     "du bouton d'aide, permet de sortir de Pente immédiatement."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Il existe aussi quelques raccourcis clavier. "
     "L'appui sur \"Entrée\" ou \"Retour Chariot\" revient au même qu'un "
     "appui sur le bouton \"OK\" d'une fenêtre.  L'appui sur \"H\" ou "
     "\"Aide\" revient au même qu'un appui sur le bouton \"Aide\". "
     "L'appui sur \"Q\" dans la fenêtre principale permet de sortir du "
     "programme. L'appui sur \"S\" permet de mettre ou d'enlever le son. "
     "Les touches de flèches gauche et droite permettent de défaire ou "
     "refaire les coups."},
  {butText_just, 0, "   Si vous appuyez sur \"Q\" par mégarde et perdez "
     "votre partie, pas de panique! Il suffit de relancer Pente et votre "
     "partie sera toujours là, exactement où vous en étiez."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  proghelp_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "di Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "Aiuto del programma Pente"},
  {butText_just, 0, "   Questa pagina di aiuto spiega come usare le opzioni di "
     "Pente versione " VERSION ".  In cima a questa finestra di aiuto c'è un menu; "
     "cliccatelo e trascinate il mouse per ottenere la lista delle altre pagine "
     "di aiuto disponibili."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Per scorrere il testo in questa finestra di aiuto, "
     "potete usare la barra di scorrimento sul lato sinistro o cliccare il "
     "testo stesso con il tasto destro del mouse per trascinarlo."},
  {butText_just, 0, ""},
  
  {butText_just, 0, "   La finestra principale di Pente mostra una griglia di "
      "pente con alcuni comandi in fondo.  La griglia ha l'aspetto di una comune "
      "griglia di pente e dovrebbe essere autoesplicativa.  Basta cliccare su "
      "un'intersezione per metterci una pedina.  Su ciascun lato della griglia di "
      "pente ci sono le buche nelle quali vengono messe le pedine catturate.  "
      "Sotto la griglia ci sono otto comandi.  Questi sono il selettore per il "
      "giocatore 1, il selettore per il giocatore 2, il pulsante Indietro, "
      "il pulsante Ripeti, il pulsante Inizia/Ferma, il pulsante Configura, "
      "il pulsante Aiuto e il pulsante Esci."},
  {butText_just, 0, ""},
  {butText_just, 0, "   I selettori per il giocatore 1 e il giocatore 2 sono i "
     "menu che si trovano all'estrema sinistra e all'estrema destra dei comandi.  "
     "Quando il menu viene aperto premendo il tasto sinistro del mouse, si hanno "
     "varie scelte.  La prima scelta, "
     "\"Umano\", vi consente di usare quel giocatore.  Per esempio, se entrambe "
     "i selettori sono impostati a \"Umano\", due persone possono usare lo "
     "stesso computer per giocare a pente una contro l'altra."},
  {butText_just, 0, "   La scelta successiva in un selettore di giocatore è "
     "\"Remoto\".  Se il vostro programma di Pente è configurato correttamente, "
     "questa opzione può essere usata per giocare a pente contro qualcuno su "
     "un altro computer.  "
     "Fate riferimento alla pagina di aiuto sul gioco in rete per maggiori "
     "informazioni su questo."},
  {butText_just, 0, "   Le ultime sei opzioni nei selettori dei giocatori "
     "sono gli avversari gestiti dal computer, rappresentati da una icona "
     XIO_COMPCHAR " e la difficoltà per quel giocatore.  Selezionando uno "
     "di quei giocatori il computer muove per esso.  Per esempio, impostando "
     "il selettore del giocatore 1 a \"Umano\" e il selettore del giocatore 2 "
     "a \"" XIO_COMPCHAR " Semplice\" giocherete come giocatore 1 e il "
     "computer muoverà per il giocatore 2.  Questo specifico giocatore del "
     "computer è molto facile da battere.  "
     "I giocatori controllati dal computer sono elencati dal più facile al "
     "più difficile da battere nel selettore del giocatore."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Nella parte superiore centrale dei comandi si trova il "
     "pulsante inizia/ferma.  Questo pulsante fa iniziare una nuova partita "
     "se nessuna è in corso o ferma quella corrente.  L'etichetta del "
     "pulsante cambia per riflettere la sua funzione."},
  {butText_just, 0, "   Qualche volta il gioco verrà temporaneamente sospeso "
     "(per esempio, se cambiate i giocatori nel corso di una partita). Quando "
     "questo accade, l'etichetta del pulsante inizia/ferma diventerà \"Continua\". "
     "  Premendo il pulsante il gioco riprenderà."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Su ciascun lato del pulsante inizia/ferma si trovano "
     "i pulsanti indietro e ripeti.  Premendo indietro si potranno "
     "annullare le proprie mosse, una alla volta.  Premendo ripeti le mosse "
     "verranno effettuate nuovamente, una alla volta.  Usando questi pulsanti "
     "potete rivedere una partita per capire quali mosse hanno portato alla "
     "situazione corrente."},
  {butText_just, 0, "   indietro può anche essere usato per correggere "
     "una mossa.  Se premete indietro fino a cancellare la mossa che vi "
     "interessa poi ne fate una diversa, il gioco continuerà da li."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Il pulsante aiuto è nella parte inferiore centrale "
     "dei comandi.  Dal momento che state leggendo questo, dovete già sapere "
     "come usare questo pulsante."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Il pulsante Configura, a sinistra del pulsante "
     "aiuto, vi consente di cambiare la configurazione di Pente.  Fate riferimento "
     "alla pagina di aiuto sulla configurazione per maggiori informazione."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Il pulsante esci, che si trova a destra del pulsante "
     "aiuto, esce da Pente immediatamente."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Ci sono diversi tasti di scelta rapida disponibili.  "
     "Premendo \"Invio\" o \"Return\" equivale a premere il pulsante \"OK\" "
     "in una finestra.  Premere \"H\" è come premere il pulsante \"aiuto\".  "
     "Premendo \"Q\" nella finestra principale si esce dal programma.  "
     "Premendo \"S\" si attiva/disattiva il sonoro.  "
     "I tasti freccia sinistra e destra si comportano come i pulsanti "
     "indietro e ripeti."},
  {butText_just, 0, "   Se premete \"Q\" per sbaglio e uscite dal gioco, "
     "non preoccupatevi!  Basta rilanciare Pente e la vostra partita sarà "
     "ancora li, esattamente dove l'avevate lasciata."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  gamehelp_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "How to play Pente"},
  {butText_just, 0, "   Pente is the English name of the Asian game of "
     "ni-nuki, which is itself a version of the game of go-moku.  Pente "
     "is played on a 19x19 grid with stones in two different colors.  "
     "Each player chooses one set of stones; then the players take turns "
     "placing their stones on any unoccupied intersection until one player "
     "wins."},
  {butText_just, 0, "   There are two ways to win.  If a player makes five "
     "or more stones in a straight line (across, down, or diagonally), then "
     "that player wins.  Or, if a player captures five pairs of his or her "
     "opponent's stones, that player also wins."},
  {butText_just, 0, "   Stones may be captured in pairs only.  To capture "
     "a pair of stones, a player must place one stone on either side of the "
     "pair.  For example:"},
  {butText_center, 0, XIO_PL1CHAR XIO_PL2CHAR XIO_PL2CHAR},
  {butText_just, 0, "   If another " XIO_PL1CHAR " stone is placed on the "
     "right side of this group, the two center stones will be captured.  "
     "This will count as one capture."},
  {butText_just, 0, "   That's it!"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  gamehelp_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "Comment jouer à Pente"},
  {butText_just, 0, "   Pente est le nom Anglais du jeu asiatique "
     "ni-nuki, qui est lui-même une version du jeu go-moku. "
     " Pente se joue sur une grille de 19x19 cases en utilisant des "
     "pierres de deux couleurs différentes.  Chaque joueur choisit un "
     "ensemble de pierres; puis les joueurs placent à tour de rôle leurs "
     "pierres sur les intersections innocupées jusqu'à "
     "ce qu'un des joueurs gagne."},
  {butText_just, 0, "   Il y a deux façons de gagner.  Un joueur peut "
     "gagner s'il aligne (verticalement, horizontalement ou en diagonale) "
     "consécutivement cinq pierres ou plus. Un joueur peut aussi gagner en "
     "capturant cinq paires de pierres de son adversaire."},
  {butText_just, 0, "   Les pierres peuvent être uniquement capturées par "
     "paires. Pour capturer une paire de pierres, un joueur doit placer une "
     "pierre de part et d'autre de la paire. Par exemple:"},
  {butText_center, 0, XIO_PL1CHAR XIO_PL2CHAR XIO_PL2CHAR},
  {butText_just, 0, "   Si une autre " XIO_PL1CHAR " pierre est placée "
     "sur le côté droit de ce groupe, les deux pierres centrales seront "
     "capturées. Cela comptera alors pour une capture."},
  {butText_just, 0, "   Et c'est tout!"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  gamehelp_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "di Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "Come giocare a Pente"},
  {butText_just, 0, "   Pente è il nome Inglese del gioco Asiatico "
     "ni-nuki, che è esso stesso una versione del gioco go-moku.  Pente "
     "è giocato su una griglia 19x19 con pedine di due colori diversi.  "
     "Ogni giocatore sceglie un set di pedine; i giocatori a turno "
     "mettono le loro pedine sugli incroci liberi fino a quando uno "
     "dei due vince."},
  {butText_just, 0, "   Ci sono due modi in cui si vince. Se un giocatore "
     "mette cinque o più pedine in linea retta (orizzontale, verticale o "
     "diagonale), vince. Oppure si può vincere anche catturando cinque "
     "coppie di pedine del proprio avversario."},
  {butText_just, 0, "   Le pedine possono essere catturate solo a coppie.  "
     "Per catturare una coppia di pedine, un giocatore deve mettere una pedina "
     "su ciascuna lato della coppia.  Per esempio:"},
  {butText_center, 0, XIO_PL1CHAR XIO_PL2CHAR XIO_PL2CHAR},
  {butText_just, 0, "   Se un'altra pedina " XIO_PL1CHAR " viene messa a "
     "destra di questo gruppo, le due pedine centrali verranno catturate.  "
     "Questo conta come una presa."},
  {butText_just, 0, "   Tutto qua!"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  setuphelp_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "Help on the setup window"},
  {butText_just, 0, "   This help page explains the features of the setup "
     "window in Pente.  To see the setup window, press the \"setup\" button "
     "on the main window."},
  {butText_just, 0, "   There are four subsections of the setup window.  "
     "These are the options section, the sound section, the "
     "transposition table section, and the networking section."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The options section, in the upper left portion of "
     "the setup window, has three controls.  There is a \"Language\" menu "
     "that selects which language text messages should appear in.  Currently "
     "the only languages supported are English, French and Italian.  If you "
     "know another language and are willing to do the translations, by all "
     "means get in touch with me!  I'd like to translate this program into "
     "as many languages as possible."},
  {butText_just, 0, "   In the options section there is also a checkbox "
     "for color.  If you have a color screen, pressing this checkbox will "
     "switch between using color and black and white."},
  {butText_just, 0, "   The last control in the options section is the "
     "\"Show thinking\" check box.  If this is checked, you can see the "
     "moves that the computer is thinking about when it is playing at "
     "the \"Expert\" and \"Master\" levels.  If this is not checked, you "
     "cannot see the moves that the computer is considering."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The sound section is in the lower left of the "
     "setup window.  It contains one control: a slider which sets the sound "
     "volume.  If you have sound, sliding this control will set how loud the "
     "sound effects should be.  Sliding all the way to zero will turn off "
     "all sound."},
  {butText_just, 0, ""},
  {butText_just, 0, "   The transposition table section is in the upper "
     "right of the setup window.  This is currently disabled."},
  {butText_just, 0, ""},
  {butText_just, 0, ""},
  {butText_just, 0, "   At the bottom of the setup window are two buttons, "
     "labeled \"OK\" and \"Help\".  Pressing \"OK\" closes the setup window "
     "and pressing \"Help\" opens this help page."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  setuphelp_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "Aide sur la fenêtre de configuration"},
  {butText_just, 0, "   Cette page d'aide explique les différentes options "
     "de la fenêtre de configuration de Pente. Pour accèder à la fenêtre "
     "de configuration, appuyez sur le bouton \"Config.\" de la fenêtre "
     "principale."},
  {butText_just, 0, "   Il y a quatre parties dans la fenêtre de "
     "configuration.  On y trouve la partie des options, la partie "
     "son, la partie de la table de transposition et la partie "
     "réseau."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La partie des options, en haut et à gauche de la "
     "fenêtre de configuration, contient trois options de contrôle. On trouve "
     "le menu \"Langue\" qui permet de choisir dans quelle langue "
     "les messages s'affichent. Pour l'instant, les seules langues "
     "disponibles sont l'Anglais et le Français.  Si vous connaissez une "
     "autre langue et si vous avez envie de réaliser la traduction, alors "
     "surtout contactez moi!  J'aimerais traduire mon programme dans "
     "autant de langues que possible."},
  {butText_just, 0, "   Dans la partie des options, il y a également une "
     "boîte à cocher pour la couleur. Si vous avez un moniteur couleur, lors "
     "de l'appui sur la boîte à cocher vous pouvez basculer entre la couleur "
     "et le noir et blanc."},
  {butText_just, 0, "   Le dernier bouton do contrôle de la partie des "
     "options est la boîte á cocher \"" XIO_COMPCHAR " visible\".  "
     "Si elle est cochée, vous pouvez visualiser les déplacements "
     "auxquels l'ordinateur réfléchit lorsqu'il joue aux niveaux "
     "\"Expert\" et \"Maître\".  Si elle n'est pas cochée, vous ne "
     "pouvez pas voir les déplacements que l'ordinateur est en train de "
     "considérer."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La partie son se trouve en bas et à gauche "
     "de la fenêtre de configuration. On y trouve une seule option de "
     "contrôle: une barre de défilement qui permet d'ajuster le volume "
     "sonore. Si vous avez du son, en glissant le curseur vous pouvez "
     "spécifiez le volume du son. En glissant le curseur vers zéro "
     "vous pouvez arrêter tout effet sonore."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La partie de la table de transposition se trouve en "
     "haut et à droite de la fenêtre de configuration. Pour l'instant, elle "
     "n'est pas en fonction."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La partie réseau se trouve en bas et à droite de "
     "la fenêtre de configuration. Pour l'instant, elle "
     "n'est pas en fonction."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Tout en bas de la fenêtre de configuration, on "
     "trouve deux boutons dont le nom est \"OK\" et \"Aide\". L'appui sur "
     "\"OK\" ferme la fenêtre de configuration et l'appui sur \"Aide\" "
     "permet d'activer cette page d'aide."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  setuphelp_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "di Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "Aiuto sulla finestra di configurazione"},
  {butText_just, 0, "   Questa pagina di aiuto spiega le opzioni della "
     "finestra di configurazione in Pente.  Per accedere alla finestra di "
     "configurazione, premere il pulsante \"Configura\" nella "
     "finestra principale."},
  {butText_just, 0, "   Vi sono quattro sottosezioni nella finestra di "
     "configurazione.  Si tratta della sezione delle opzioni, di quella "
     "del sonoro, di quella della tabella di trasposizione e di quella "
     "per il gioco in rete."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La sezione delle opzioni, nella parte superiore "
     "sinistra della finestra di configurazione, ha tre comandi.  C'è il "
     "menu della \"Lingua\" che seleziona la lingua da usare per i messaggi "
     "all'utente.  Attualmente, le uniche lingue supportate sono l'Inglese, "
     "il Francese e l'Italiano.  Se conoscete un'altra lingua e siete "
     "disposti a fare la traduzione, contattatemi senza ulteriori indugi!  "
     "Vorrei tradurre questo programma nel maggior numero possibile di lingue."},
  {butText_just, 0, "   Sempre nella sezione delle opzioni, c'è una casella "
     "per il colore.  Se avete uno schermo a colori, cliccando su questa casella "
     "passerete da colore a bianco e nero e viceversa."},
  {butText_just, 0, "   L'ultimo comando nella sezione delle opzioni è la "
     "casella \"Visibile\".  Se questa è marcata, potete vedere le mosse che "
     "il computer sta pensando quando gioca ai livelli \"Esperto\" e "
     "\"Maestro\".  Se non è abilitata, non potete vedere le mosse che il "
     "computer sta esaminando."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La sezione del sonoro è nella parte inferiore "
     "sinistra della finestra di configurazione.  Contiene un solo comando: "
     "un cursore per regolare il volume del sonoro.  Se il vostro sistema "
     "è dotato di sonoro, spostando questo controllo imposterete il volume degli "
     "effetti sonori.  Spostandolo completamente a zero disabiliterete "
     "il sonoro."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La sezione della tabella di trasposizione è nella "
     "parte superiore destra della finestra di configurazione ed è "
     "attualmente disabilitata."},
  {butText_just, 0, ""},
  {butText_just, 0, "   La sezione della rete è nella parte inferiore destra "
     "della finestra di setup.  Contiene una casella, un pulsante ed un campo "
     "di immissione.  La casella vi consente di decidere se Pente deve accettare "
     "o meno le richieste remote di giocare in rete.  Il pulsante \"Connetti\" "
     "può essere usato per iniziare subito una partita in rete e il campo "
     "\"Porta\" imposta il numero di porta di rete usata da Pente.  Si prega di "
     "consultare la pagina di aiuto sul \"Gioco in rete\" per maggiori "
     "informazioni."},
  {butText_just, 0, ""},
  {butText_just, 0, "   In fondo alla finestra di configurazione, si trovano "
     "due pulsanti, etichettati \"OK\" e \"aiuto\".  Premendo \"OK\" si chiude "
     "la finestra di configurazione e premendo \"aiuto\" si apre questa pagina."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  network_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "Setting Up Network Play"},
  {butText_just, 0, "   To play pente against somebody on a different "
     "machine, both players have to start by running Pente (version 2.1.0 or "
     "later).  In the setup window, there's a control panel that lets you set "
     "your port number.  Exactly which port number you choose is pretty "
     "unimportant; they're all the same.  This is the port where your Pente "
     "program will be listening for network games.  You should probably leave "
     "it at 15023, which is the standard Pente port."},
  {butText_just, 0, "   Once both players have run Pente, decide who will "
     "connect and who will listen.  The player who listens must make sure "
     "that remote operation is turned on with the checkbox in the setup "
     "window.  Then the player who will connect can either press the "
     "\"Connect\" button in his setup window or select \"Remote\" on one of "
     "his player menus.  The connecting player then gets a window where he is "
     "asked for the name of the machine to connect to and the port.  He "
     "should type in the name of the listening player's machine and the same "
     "port than the listening player has set in his setup window (this will "
     "be 15023 unless the listening player decided to be different).  After "
     "pressing OK, the two players should be connected and ready to go!"},
  {butText_just, 0, ""},
  {butText_just, 1, "Using Network Play"},
  {butText_just, 0, "   Once the two players are connected, you should see "
     "each other's mice.  Your opponent's mouse will be upside down.  Now "
     "just set one player to \"Remote\" and the other to \"human\" and you'll "
     "be playing against each other."},
  {butText_just, 0, ""},
  {butText_just, 0, "   If a player wishes to stop using network play, they "
     "should press the \"Disconnect\" button in the setup window."},
  {butText_just, 0, ""},

  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  network_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "Configurer le Jeu en Réseau"},
  {butText_just, 0, "   Pour jouer à pente contre quelqu'un sur une machine "
     "différente, les deux joueurs doivent démarrer Pente (version 2.1.0 ou "
     "plus). Dans la fenêtre de configuration, on trouve un panneau de "
     "contrôle permettant de sélectionner le numéro de port. Peu importe le "
     "numéro de port que vous choisissez; ce sont tous les mêmes. Il "
     "s'agit du port sur lequel le programme Pente va se mettre à l'écoute "
     "pour les jeux en réseau. Vous devriez laisser le numéro de port à "
     "15023, c'est le port standard de Pente."},
  {butText_just, 0, "   Une fois que les deux joueurs ont lancé Pente, il "
     "faut décider qui va se connecter et qui va écouter. Le joueur "
     "qui écoute doit s'assurer que l'option de connexion "
     "est validée par une coche dans la fenêtre de configuration. "
     "Ensuite, le joueur qui se connecte "
     "peut soit appuyer sur le bouton \"Connexion\" de sa fenêtre de "
     "configuration, soit sélectionner \"Distant\" sur l'un des menus "
     "des sélectionneurs de joueur. Le joueur qui se connecte "
     "obtient alors une fenêtre où il "
     "doit saisir le nom de la machine où se connecter ainsi que le port. "
     "Le nom à saisir est celui de la machine du joueur distant et le "
     "port est le même que celui choisi par le joueur distant (ce sera "
     "15023 à moins que le joueur distant en ait décidé autrement). Après "
     "avoir appuyé sur OK, les deux joueurs devraient être connectés et "
     "prêts à jouer!"},
  {butText_just, 0, ""},
  {butText_just, 1, "Utilisation du jeu en réseau"},
  {butText_just, 0, "   Une fois que les deux joueurs sont connectés, "
     "vous devriez voir la souris de chacun des deux. La souris de votre "
     "adversaire est retournée. A présent il suffit de sélectionner l'un des "
     "joueurs comme \"Distant\" et l'autre comme \"Humain\" afin de "
     "jouer l'un contre l'autre."},
  {butText_just, 0, ""},
  {butText_just, 0, "   FRENCH If a player wishes to stop using network play, "
     "they should press the \"Disconnect\" button in the setup window."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  network_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "di Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "Impostare il gioco in rete"},
  {butText_just, 0, "   Per giocare a pente contro qualcuno su una macchina "
     "diversa, entrambe i giocatori devono iniziare con l'eseguire Pente "
     "(versione 2.1.0 o successiva).  Nella finestra di configurazione, c'è "
     "un pannello nel quale potete impostare il numero della porta di rete.  "
     "Non è molto importante il numero che scegliete: sono tutti equivalenti.  "
     "Questa è la porta sulla quale il vostro programma di Pente aspetterà "
     "eventuali richieste per il gioco in rete.  Tutto sommato, è meglio che "
     "la lasciate a 15023, che è la porta standard di Pente."},
  {butText_just, 0, "   Una volta che entrambe i giocatori stanno eseguendo "
     "Pente,  decidete chi chiamerà e chi ascolterà.  Il giocatore in ascolto "
     "deve assicurarsi di avere abilitato il gioco in rete nella casella della "
     "finestra di configurazione.  A questo punto, il giocatore chiamante "
     "può o premere il pulsante \"Connetti\" nella sua finestra di configurazione "
     "o selezionare \"Remoto\" in uno dei suoi selettori di giocatore.  Al "
     "giocatore chiamante comparirà una finestra in cui dovrà specificare "
     "il nome della macchina a cui connettersi e il numero della porta.  "
     "Dovrà scrivere il nome della macchina del giocatore in ascolto e lo "
     "stesso numero di porta da lui impostato nella propria finestra di "
     "configurazione (che sarà 15023 a meno che il ricevente non abbia deciso "
     "altrimenti).  Dopo aver premuto OK, i due giocatori dovrebbero essere "
     "connessi e pronti per giocare!"},
  {butText_just, 0, ""},
  {butText_just, 1, "Giocare in rete"},
  {butText_just, 0, "   Una volta che i due giocatori sono collegati,  "
     "ognuno dovrebbe vedere il mouse dell'altro.  Il mouse del vostro "
     "avversario sarà capovolto.  Adesso basta impostare un giocatore "
     "a \"Remoto\" e l'altro a \"Umano\" e potrete giocare uno contro "
     "l'altro."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Se un giocatore desidera interrompere una partita in "
     "rete,  gli basterà premere il pulsante \"Disconnetti\" nella finestra "
     "di configurazione."},
  {butText_just, 0, ""},

  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
  
static xio_tb_t  computer_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "About the Computer Players"},
  {butText_just, 0, "   The computer players use a tree search to determine "
     "their moves.  Making a threat (such as three in a row, open on both "
     "ends) counts as good thing; captures count even better.  So pretty "
     "much they analyze all possible moves, looking for sequences that let "
     "them make lots more threats and captures than their opponents."},
  {butText_just, 0, "   Each computer player searches one move farther "
     "ahead than the previous one.  So \"" XIO_COMPCHAR " Simple\" only looks "
     "one move ahead, but \"" XIO_COMPCHAR " Master\" looks six moves ahead."},
  {butText_just, 0, "   At deeper searches, pruning is done.  I wanted "
     "to experiment with different pruning algorithms besides regular "
     "alpha-beta, but my results haven't been that great.  I prune off all "
     "sequences that last 3 or more moves with no threats or captures; and "
     "I never even consider moves that are more than 2 spaces from all other "
     "pieces on the board.  Lastly, if the computer is searching more than "
     "4 moves ahead, I do a 4 ply search then search deeper on the \"n\" most "
     "promising moves, where \"n\" is the overall search depth.  This last "
     "pruning algorithm is a lot like alpha-beta, but probably not as good."},
  {butText_just, 0, "   I plan on doing a major rewrite, replacing all "
     "this weird stuff with normal alpha-beta and installing a transposition "
     "table, then comparing the result against my current algorithms.  I "
     "suspect that this new version will be able to search deeper in less "
     "time, and thus play a better game."},
  {butText_just, 0, "   As an aside, originally I used a pattern matching "
     "algorithm instead of deep searches.  The pattern matcher trained "
     "itself using simulated annealing.  After letting the annealing "
     "algorithm run for a week or two, it played a surprisingly strong "
     "game for only one ply lookahead.  I tried training it to recognize "
     "patterns useful for a two ply search, but unfortunately the annealing "
     "algorithm was far too slow for this.  After giving up on tuning the "
     "algorithm enough for reasonable performance, I switched to the search "
     "that I use today."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  computer_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "A Propos des Joueurs Gérés par l'Ordinateur"},
  {butText_just, 0, "   Les joueurs gérés par l'ordinateur utilisent une "
     "recherche d'arbre pour déterminer leurs coups. Porter une menace "
     "(telle que trois pierres alignées avec une ouverture de chaque côté) "
     "compte comme une bonne chose; les captures compte pour encore plus.  "
     "Ils analysent donc tous les coups possibles, en recherchant des "
     "séquences leur permettant de faire beaucoup plus de menaces et de "
     "captures que leurs adversaires."},
  {butText_just, 0, "   Chaque joueur géré par l'ordinateur cherche un coup "
     "en avance de plus que le précédent joueur. "
     "Donc \"" XIO_COMPCHAR " Simple\" "
     "regarde juste un coup en avance, mais \"" XIO_COMPCHAR " Maître\" "
     "regarde six coups en avance."},
  {butText_just, 0, "   Pour les recherches plus profondes, on a recours à "
     "un élagage.  Je voulais expérimenter différents algorithmes "
     "d'élagage en plus de l'alpha-beta habituel, mais mes résultats n'ont "
     "pas été si bons que ça. "
     "J'élague toutes les séquences de 3 coups ou plus sans menaces "
     "ni captures; et je ne prends jamais en considération les coups qui "
     "se trouvent à plus de 2 cases des autres jetons du plateau de jeu. "
     "Finalement, si l'ordinateur cherche plus de 4 coups en avance, je "
     "commence la recherche de 4 coups en avance  puis je cherche plus "
     "profondément dans "
     "les \"n\" plus prometteurs coups, où \"n\" est la profondeur de "
     "recherche globale. Ce dernier algorithme d'élagage ressemble "
     "beaucoup à de l'alpha-beta, mais n'est peut être pas aussi bon."},
  {butText_just, 0, "   Je projette de réaliser une grande réécriture, de "
     "remplacer toutes ces choses étranges par de l'alpha-beta normal et "
     "d'installer une table de transposition, puis de comparer le résultat "
     "avec l'algorithme que j'utilise pour le moment.  Je suppose que "
     "cette nouvelle version sera capable de chercher encore plus "
     "profondément et en moins de temps, et de jouer encore mieux."},
  {butText_just, 0, "   Un petit mot en en apparté: au départ "
     "j'utilisais un algorithme de "
     "comparaison de motif à la place de recherches en profondeur.  Le "
     "comparateur de motif s'entraînait tout seul en utilisant une "
     "heuristique. Après avoir laissé tourner l'heuristique pendant "
     "une semaine ou deux, le programme était étonnement fort pour "
     "seulement une recherche d'un coup "
     "d'avance. J'ai essayé de l'entraîner à reconnaître des motifs utiles "
     "pour une recherche de deux coups en avance, mais malheureusement "
     "l'heuristique était beaucoup trop lente. Après avoir abandonné la "
     "mise au point de l'algorithme, suffisamment pour obtenir des "
     "performances raisonnables, je suis passé à la recherche que j'utilise "
     "à présent."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  computer_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "A proposito dei giocatori controllati dalla macchina"},
  {butText_just, 0, "   I giocatori del computer usano un albero di ricerca "
     "per scegliere le loro mosse.  Portare un attacco (come tre pedine allineate, "
     "libere da entrambe gli estremi) è una buona cosa; le prese sono anche meglio.  "
     "Quindi, in pratica, analizzano tutto le mosse possibili, alla ricerca di "
     "sequenze che consentano loro di portare più attacchi ed effettuare più "
     "prese dei loro avversari."},
  {butText_just, 0, "   Ogni giocatore del computer esamina una mossa in più "
     "del precedente.  Quindi, \"" XIO_COMPCHAR " Semplice\" guarda solo una "
     "mossa avanti, mentre \"" XIO_COMPCHAR " Maestro\" ne guarda sei."},
  {butText_just, 0, "   Man mano che la profondità della ricerca cresce, "
     "l'albero di ricerca viene sfoltito.  Volevo fare delle prove con "
     "algoritmi di sfoltimento diversi dai classici alfa-beta, ma i "
     "miei risultati non sono stati particolarmente incoraggianti.  Escludo "
     "dalla ricerca tutte le sequenze di 3 o più mosse senza attacchi o prese "
     "e non considero nemmeno le mosse che distano più di due posizioni da "
     "tutte le altre pedine sulla griglia.  Infine, se il computer sta "
     "esaminando oltre la quarta mossa successiva, eseguo prima una ricerca "
     "fino al quarto livello, poi continuo la ricerca sulle \"n\" mosse più "
     "promettenti, dove \"n\" è la profondità complessiva della ricerca.  "
     "Quest'ultimo algoritmo di sfoltimento assomiglia molto all'alfa-beta, "
     "ma probabilmente non è altrettanto buono."},
  {butText_just, 0, "   Sto meditando di riscriverne buona parte, sostituendo "
     "tutta questa strana roba con un normale alfa-beta e installando una "
     "tabella di trasposizione, per poi confrontare i risultati con quelli dei "
     "miei algoritmi attuali.  Ho idea che questa nuova versione sarà in "
     "grado di cercare più in profondità in meno tempo e quindi giocherà meglio."},
  {butText_just, 0, "   Per inciso, originariamente utilizzavo un'algoritmo di "
     "riconoscimento delle configurazioni invece della ricerca in profondità.  "
     "Tale algoritmo si allenava con un procedimento chiamato \"raffreddamento "
     "simulato\".  Dopo averlo lasciato girare per una settimana o due, giocava "
     "veramente bene considerato che guardava solo fino alla mossa successiva.  "
     "Ho provato ad allenarlo per riconoscere le configurazioni utili per una "
     "ricerca al secondo livello, ma l'algoritmo di raffreddamento era "
     "decisamente troppo lento per fare ciò.  Dopo aver abbandonato i tentativi "
     "di migliorare l'algoritmo per ottenere delle prestazioni accettabili, "
     "sono passato all'algoritmo di ricerca che uso ancora oggi."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  author_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "About the author"},
  {butText_just, 0, "   Hello!  My name is Bill Shubert.  I've always loved "
     "playing pente, and computer algorithms to play human-oriented games "
     "fascinate me, so writing a computer program to play pente seemed "
     "like a natural thing to try.  Then I wanted to learn X, so I wrote "
     "the GUI to go with it.  It seems like I can always think of a way to "
     "improve this program so I'm never really done."},
  {butText_just, 0, "   Feel free to get in touch with me if you have any "
     "comments about this program!  As of the date at the top of this "
     "help page, you can contact me at:"},
  {butText_left, 0, "   Email: wms@igoweb.org"},
  {butText_left, 0, "   WWW: http://www.igoweb.org/~wms/"},
  {butText_just, 0, ""},
  {butText_just, 0, "   I'd like to thank Eric Dupas for translating all "
     "text into French and Andrea Borgia (email: borgia@cs.unibo.it; "
     "homepage: http://www.cs.unibo.it/~borgia) for translating the text into "
     "Italian."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Thanks for trying out my program.  I hope you like "
     "it!"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  author_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "A Propos de l'Auteur"},
  {butText_just, 0, "   Bonjour!  Mon nom est Bill Shubert.  J'ai toujours "
     "aimé jouer à pente, et les algorithmes pour jouer "
     "à des jeux pour humains me fascinent, il m'a donc semblé naturel "
     "d'écrire un programme pour jouer à pente. "
     "Ensuite j'ai voulu apprendre X, donc j'ai écrit l'Interface Graphique "
     "Utilisateur qui fonctionne avec. J'ai toujours l'impression d'avoir "
     "une façon d'améliorer ce programme, je n'en ai donc jamais "
     "vraiment terminé."},
  {butText_just, 0, "   Vous pouvez me joindre si vous avez des commentaires "
     "concernant ce programme! A la date qui se trouve en haut de cette page "
     "d'aide, vous pouvez me contacter de plusieurs manières:"},
  {butText_left, 0, "   Email: wms@igoweb.org"},
  {butText_left, 0, "   WWW: http://www.igoweb.org/~wms/"},
  {butText_just, 0, ""},
  {butText_just, 0, "   J'aimerais remercier Eric Dupas pour la traduction "
     "de tout le texte en Français."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Merci d'essayer mon programme.  J'espère que "
     "vous l'appréciez!"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};
static xio_tb_t  author_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "di Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "A proposito dell'autore"},
  {butText_just, 0, "   Salve!  Il mio nome è Bill Shubert.  Mi è sempre "
     "piaciuto giocare a pente e gli algoritmi per giocare dei giochi per "
     "umani mi affascinano, per cui mi è sembrato naturale scrivere un "
     "programma per giocare a pente col computer.  Poi volevo imparare X, "
     "quindi ho scritto l'interfaccia che lo accompagna.  Sembra che io trovi "
     "sempre nuovi modi per migliorare questo programma, per cui non l'ho "
     "mai davvero finito."}, 
  {butText_just, 0, "   Siete liberi di contattarmi se avete dei commenti su "
     "questo programma!  Alla data riportata in cima a questa pagina di aiuto, "
     "potete contattarmi in questi modi:"},
  {butText_left, 0, "   Email: wms@igoweb.org"},
  {butText_left, 0, "   WWW: http://www.igoweb.org/~wms/"},
  {butText_just, 0, ""},
  {butText_just, 0, "   Desidero ringraziare Eric Dupas per la traduzione del "
     "testo in Francese e Andrea Borgia (email: borgia@cs.unibo.it; homepage: "
     "http://www.cs.unibo.it/~borgia) per quella in Italiano."},
  {butText_just, 0, ""},
  {butText_just, 0, "   Grazie di aver provato il mio programma: spero che vi piaccia."},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  copying_eng[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "By Bill Shubert"},
  {butText_center, 0, DATE},
  {butText_just, 0, ""},
  {butText_just, 1, "Distribution"},
  {butText_just, 0, "   This program is distributed under the Gnu General "
     "Public License, version 2.  This should have been included with your "
     "copy of the source code in a file called \"COPYING\".  In any case, the "
     "license is also included below."},
  {butText_just, 0, "   Basically, there are three things that most of you "
     "will care about in this license.  One is that you can give away free "
     "copies to all your friends.  Another is that you have to be told where "
     "you can get the source code from.  You can get it from me; see the "
     "\"About the Author\" help page for how to get in touch.  The last is "
     "that there is no warranty, so if this software ruins your life don't "
     "come crying to me."},
  {butText_just, 0, "   If you want to do anything else with this software, "
     "you should read the license below for details."},
  {butText_just, 0, ""},
  {butText_center, 0, "GNU GENERAL PUBLIC LICENSE"},
  {butText_center, 0, "TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND "
     "MODIFICATION"},
  {butText_just, 0, ""},
  {butText_just, 0, "  0. This License applies to any program or other work "
     "which contains a notice placed by the copyright holder saying it may be "
     "distributed under the terms of this General Public License.  The "
     "\"Program\", below, refers to any such program or work, and a \"work "
     "based on the Program\" means either the Program or any derivative work "
     "under copyright law: that is to say, a work containing the Program or a "
     "portion of it, either verbatim or with modifications and/or translated "
     "into another language.  (Hereinafter, translation is included without "
     "limitation in the term \"modification\".)  Each licensee is addressed "
     "as \"you\"."},
  {butText_just, 0, ""},
  {butText_just, 0, "Activities other than copying, distribution and "
     "modification are not covered by this License; they are outside its "
     "scope.  The act of running the Program is not restricted, and the "
     "output from the Program is covered only if its contents constitute a "
     "work based on the Program (independent of having been made by running "
     "the Program). Whether that is true depends on what the Program does."},
  {butText_just, 0, ""},
  {butText_just, 0, "  1. You may copy and distribute verbatim copies of the "
     "Program's source code as you receive it, in any medium, provided that "
     "you conspicuously and appropriately publish on each copy an appropriate "
     "copyright notice and disclaimer of warranty; keep intact all the "
     "notices that refer to this License and to the absence of any warranty; "
     "and give any other recipients of the Program a copy of this License "
     "along with the Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "You may charge a fee for the physical act of "
     "transferring a copy, and you may at your option offer warranty "
     "protection in exchange for a fee."},
  {butText_just, 0, ""},
  {butText_just, 0, "  2. You may modify your copy or copies of the Program "
     "or any portion of it, thus forming a work based on the Program, and "
     "copy and distribute such modifications or work under the terms of "
     "Section 1 above, provided that you also meet all of these "
     "conditions:"},
  {butText_just, 0, ""},
  {butText_just, 0, "    a) You must cause the modified files to carry "
     "prominent notices stating that you changed the files and the date of "
     "any change."},
  {butText_just, 0, ""},
  {butText_just, 0, "    b) You must cause any work that you distribute or "
     "publish, that in whole or in part contains or is derived from the "
     "Program or any part thereof, to be licensed as a whole at no charge to "
     "all third parties under the terms of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "    c) If the modified program normally reads commands "
     "interactively when run, you must cause it, when started running for "
     "such interactive use in the most ordinary way, to print or display an "
     "announcement including an appropriate copyright notice and a notice "
     "that there is no warranty (or else, saying that you provide a warranty) "
     "and that users may redistribute the program under these conditions, and "
     "telling the user how to view a copy of this License.  (Exception: if "
     "the Program itself is interactive but does not normally print such an "
     "announcement, your work based on the Program is not required to print "
     "an announcement.)"},
  {butText_just, 0, ""},
  {butText_just, 0, "These requirements apply to the modified work as a "
     "whole.  If identifiable sections of that work are not derived from the "
     "Program, and can be reasonably considered independent and separate "
     "works in themselves, then this License, and its terms, do not apply to "
     "those sections when you distribute them as separate works.  But when "
     "you distribute the same sections as part of a whole which is a work "
     "based on the Program, the distribution of the whole must be on the "
     "terms of this License, whose permissions for other licensees extend to "
     "the entire whole, and thus to each and every part regardless of who "
     "wrote it."},
  {butText_just, 0, ""},
  {butText_just, 0, "Thus, it is not the intent of this section to claim "
     "rights or contest your rights to work written entirely by you; rather, "
     "the intent is to exercise the right to control the distribution of "
     "derivative or collective works based on the Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "In addition, mere aggregation of another work not based "
     "on the Program with the Program (or with a work based on the Program) "
     "on a volume of a storage or distribution medium does not bring the "
     "other work under the scope of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  3. You may copy and distribute the Program (or a work "
     "based on it, under Section 2) in object code or executable form under "
     "the terms of Sections 1 and 2 above provided that you also do one of "
     "the following:"},
  {butText_just, 0, ""},
  {butText_just, 0, "    a) Accompany it with the complete corresponding "
     "machine-readable source code, which must be distributed under the terms "
     "of Sections 1 and 2 above on a medium customarily used for software "
     "interchange; or,"},
  {butText_just, 0, ""},
  {butText_just, 0, "    b) Accompany it with a written offer, valid for at "
     "least three years, to give any third party, for a charge no more than "
     "your cost of physically performing source distribution, a complete "
     "machine-readable copy of the corresponding source code, to be "
     "distributed under the terms of Sections 1 and 2 above on a medium "
     "customarily used for software interchange; or,"},
  {butText_just, 0, ""},
  {butText_just, 0, "    c) Accompany it with the information you received "
     "as to the offer to distribute corresponding source code.  (This "
     "alternative is allowed only for noncommercial distribution and only if "
     "you received the program in object code or executable form with such an "
     "offer, in accord with Subsection b above.)"},
  {butText_just, 0, ""},
  {butText_just, 0, "The source code for a work means the preferred form of "
     "the work for making modifications to it.  For an executable work, "
     "complete source code means all the source code for all modules it "
     "contains, plus any associated interface definition files, plus the "
     "scripts used to control compilation and installation of the executable. "
     " However, as a special exception, the source code distributed need not "
     "include anything that is normally distributed (in either source or "
     "binary form) with the major components (compiler, kernel, and so on) of "
     "the operating system on which the executable runs, unless that "
     "component itself accompanies the executable."},
  {butText_just, 0, ""},
  {butText_just, 0, "If distribution of executable or object code is made by "
     "offering access to copy from a designated place, then offering "
     "equivalent access to copy the source code from the same place counts as "
     "distribution of the source code, even though third parties are not "
     "compelled to copy the source along with the object code."},
  {butText_just, 0, ""},
  {butText_just, 0, "  4. You may not copy, modify, sublicense, or "
     "distribute the Program except as expressly provided under this License. "
     " Any attempt otherwise to copy, modify, sublicense or distribute the "
     "Program is void, and will automatically terminate your rights under "
     "this License.  However, parties who have received copies, or rights, "
     "from you under this License will not have their licenses terminated so "
     "long as such parties remain in full compliance."},
  {butText_just, 0, ""},
  {butText_just, 0, "  5. You are not required to accept this License, since "
     "you have not signed it.  However, nothing else grants you permission to "
     "modify or distribute the Program or its derivative works.  These "
     "actions are prohibited by law if you do not accept this License.  "
     "Therefore, by modifying or distributing the Program (or any work based "
     "on the Program), you indicate your acceptance of this License to do so, "
     "and all its terms and conditions for copying, distributing or modifying "
     "the Program or works based on it."},
  {butText_just, 0, ""},
  {butText_just, 0, "  6. Each time you redistribute the Program (or any "
     "work based on the Program), the recipient automatically receives a "
     "license from the original licensor to copy, distribute or modify the "
     "Program subject to these terms and conditions.  You may not impose any "
     "further restrictions on the recipients' exercise of the rights granted "
     "herein.  You are not responsible for enforcing compliance by third "
     "parties to this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  7. If, as a consequence of a court judgment or "
     "allegation of patent infringement or for any other reason (not limited "
     "to patent issues), conditions are imposed on you (whether by court "
     "order, agreement or otherwise) that contradict the conditions of this "
     "License, they do not excuse you from the conditions of this License.  "
     "If you cannot distribute so as to satisfy simultaneously your "
     "obligations under this License and any other pertinent obligations, "
     "then as a consequence you may not distribute the Program at all.  For "
     "example, if a patent license would not permit royalty-free "
     "redistribution of the Program by all those who receive copies directly "
     "or indirectly through you, then the only way you could satisfy both it "
     "and this License would be to refrain entirely from distribution of the "
     "Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "If any portion of this section is held invalid or "
     "unenforceable under any particular circumstance, the balance of the "
     "section is intended to apply and the section as a whole is intended to "
     "apply in other circumstances."},
  {butText_just, 0, ""},
  {butText_just, 0, "It is not the purpose of this section to induce you to "
     "infringe any patents or other property right claims or to contest "
     "validity of any such claims; this section has the sole purpose of "
     "protecting the integrity of the free software distribution system, "
     "which is implemented by public license practices.  Many people have "
     "made generous contributions to the wide range of software distributed "
     "through that system in reliance on consistent application of that "
     "system; it is up to the author/donor to decide if he or she is willing "
     "to distribute software through any other system and a licensee cannot "
     "impose that choice."},
  {butText_just, 0, ""},
  {butText_just, 0, "This section is intended to make thoroughly clear what "
     "is believed to be a consequence of the rest of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  8. If the distribution and/or use of the Program is "
     "restricted in certain countries either by patents or by copyrighted "
     "interfaces, the original copyright holder who places the Program under "
     "this License may add an explicit geographical distribution limitation "
     "excluding those countries, so that distribution is permitted only in or "
     "among countries not thus excluded.  In such case, this License "
     "incorporates the limitation as if written in the body of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  9. The Free Software Foundation may publish revised "
     "and/or new versions of the General Public License from time to time.  "
     "Such new versions will be similar in spirit to the present version, but "
     "may differ in detail to address new problems or concerns."},
  {butText_just, 0, ""},
  {butText_just, 0, "Each version is given a distinguishing version number.  "
     "If the Program specifies a version number of this License which applies "
     "to it and \"any later version\", you have the option of following the "
     "terms and conditions either of that version or of any later version "
     "published by the Free Software Foundation.  If the Program does not "
     "specify a version number of this License, you may choose any version "
     "ever published by the Free Software Foundation."},
  {butText_just, 0, ""},
  {butText_just, 0, "  10. If you wish to incorporate parts of the Program "
     "into other free programs whose distribution conditions are different, "
     "write to the author to ask for permission.  For software which is "
     "copyrighted by the Free Software Foundation, write to the Free Software "
     "Foundation; we sometimes make exceptions for this.  Our decision will "
     "be guided by the two goals of preserving the free status of all "
     "derivatives of our free software and of promoting the sharing and reuse "
     "of software generally."},
  {butText_just, 0, ""},
  {butText_center, 0, "NO WARRANTY"},
  {butText_just, 0, ""},
  {butText_just, 0, "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, "
     "THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY "
     "APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT "
     "HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT "
     "WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT "
     "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
     "PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE "
     "OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU "
     "ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION."},
  {butText_just, 0, ""},
  {butText_just, 0, "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR "
     "AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO "
     "MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE "
     "LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL "
     "OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE "
     "PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING "
     "RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A "
     "FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF "
     "SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH "
     "DAMAGES."},
  {butText_just, 0, ""},
  {butText_center, 0, "END OF TERMS AND CONDITIONS"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  copying_french[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "Par Bill Shubert"},
  {butText_center, 0, DATE_FRENCH},
  {butText_just, 0, ""},
  {butText_just, 1, "Distribution"},
  {butText_just, 0, "   This program is distributed under the Gnu General "
     "Public License, version 2.  This should have been included with your "
     "copy of the source code in a file called \"COPYING\".  In any case, the "
     "license is also included below."},
  {butText_just, 0, "   Basically, there are three things that most of you "
     "will care about in this license.  One is that you can give away free "
     "copies to all your friends.  Another is that you have to be told where "
     "you can get the source code from.  You can get it from me; see the "
     "\"About the Author\" help page for how to get in touch.  The last is "
     "that there is no warranty, so if this software ruins your life don't "
     "come crying to me."},
  {butText_just, 0, "   If you want to do anything else with this software, "
     "you should read the license below for details."},
  {butText_just, 0, ""},
  {butText_center, 0, "GNU GENERAL PUBLIC LICENSE"},
  {butText_center, 0, "TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND "
     "MODIFICATION"},
  {butText_just, 0, ""},
  {butText_just, 0, "  0. This License applies to any program or other work "
     "which contains a notice placed by the copyright holder saying it may be "
     "distributed under the terms of this General Public License.  The "
     "\"Program\", below, refers to any such program or work, and a \"work "
     "based on the Program\" means either the Program or any derivative work "
     "under copyright law: that is to say, a work containing the Program or a "
     "portion of it, either verbatim or with modifications and/or translated "
     "into another language.  (Hereinafter, translation is included without "
     "limitation in the term \"modification\".)  Each licensee is addressed "
     "as \"you\"."},
  {butText_just, 0, ""},
  {butText_just, 0, "Activities other than copying, distribution and "
     "modification are not covered by this License; they are outside its "
     "scope.  The act of running the Program is not restricted, and the "
     "output from the Program is covered only if its contents constitute a "
     "work based on the Program (independent of having been made by running "
     "the Program). Whether that is true depends on what the Program does."},
  {butText_just, 0, ""},
  {butText_just, 0, "  1. You may copy and distribute verbatim copies of the "
     "Program's source code as you receive it, in any medium, provided that "
     "you conspicuously and appropriately publish on each copy an appropriate "
     "copyright notice and disclaimer of warranty; keep intact all the "
     "notices that refer to this License and to the absence of any warranty; "
     "and give any other recipients of the Program a copy of this License "
     "along with the Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "You may charge a fee for the physical act of "
     "transferring a copy, and you may at your option offer warranty "
     "protection in exchange for a fee."},
  {butText_just, 0, ""},
  {butText_just, 0, "  2. You may modify your copy or copies of the Program "
     "or any portion of it, thus forming a work based on the Program, and "
     "copy and distribute such modifications or work under the terms of "
     "Section 1 above, provided that you also meet all of these "
     "conditions:"},
  {butText_just, 0, ""},
  {butText_just, 0, "    a) You must cause the modified files to carry "
     "prominent notices stating that you changed the files and the date of "
     "any change."},
  {butText_just, 0, ""},
  {butText_just, 0, "    b) You must cause any work that you distribute or "
     "publish, that in whole or in part contains or is derived from the "
     "Program or any part thereof, to be licensed as a whole at no charge to "
     "all third parties under the terms of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "    c) If the modified program normally reads commands "
     "interactively when run, you must cause it, when started running for "
     "such interactive use in the most ordinary way, to print or display an "
     "announcement including an appropriate copyright notice and a notice "
     "that there is no warranty (or else, saying that you provide a warranty) "
     "and that users may redistribute the program under these conditions, and "
     "telling the user how to view a copy of this License.  (Exception: if "
     "the Program itself is interactive but does not normally print such an "
     "announcement, your work based on the Program is not required to print "
     "an announcement.)"},
  {butText_just, 0, ""},
  {butText_just, 0, "These requirements apply to the modified work as a "
     "whole.  If identifiable sections of that work are not derived from the "
     "Program, and can be reasonably considered independent and separate "
     "works in themselves, then this License, and its terms, do not apply to "
     "those sections when you distribute them as separate works.  But when "
     "you distribute the same sections as part of a whole which is a work "
     "based on the Program, the distribution of the whole must be on the "
     "terms of this License, whose permissions for other licensees extend to "
     "the entire whole, and thus to each and every part regardless of who "
     "wrote it."},
  {butText_just, 0, ""},
  {butText_just, 0, "Thus, it is not the intent of this section to claim "
     "rights or contest your rights to work written entirely by you; rather, "
     "the intent is to exercise the right to control the distribution of "
     "derivative or collective works based on the Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "In addition, mere aggregation of another work not based "
     "on the Program with the Program (or with a work based on the Program) "
     "on a volume of a storage or distribution medium does not bring the "
     "other work under the scope of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  3. You may copy and distribute the Program (or a work "
     "based on it, under Section 2) in object code or executable form under "
     "the terms of Sections 1 and 2 above provided that you also do one of "
     "the following:"},
  {butText_just, 0, ""},
  {butText_just, 0, "    a) Accompany it with the complete corresponding "
     "machine-readable source code, which must be distributed under the terms "
     "of Sections 1 and 2 above on a medium customarily used for software "
     "interchange; or,"},
  {butText_just, 0, ""},
  {butText_just, 0, "    b) Accompany it with a written offer, valid for at "
     "least three years, to give any third party, for a charge no more than "
     "your cost of physically performing source distribution, a complete "
     "machine-readable copy of the corresponding source code, to be "
     "distributed under the terms of Sections 1 and 2 above on a medium "
     "customarily used for software interchange; or,"},
  {butText_just, 0, ""},
  {butText_just, 0, "    c) Accompany it with the information you received "
     "as to the offer to distribute corresponding source code.  (This "
     "alternative is allowed only for noncommercial distribution and only if "
     "you received the program in object code or executable form with such an "
     "offer, in accord with Subsection b above.)"},
  {butText_just, 0, ""},
  {butText_just, 0, "The source code for a work means the preferred form of "
     "the work for making modifications to it.  For an executable work, "
     "complete source code means all the source code for all modules it "
     "contains, plus any associated interface definition files, plus the "
     "scripts used to control compilation and installation of the executable. "
     " However, as a special exception, the source code distributed need not "
     "include anything that is normally distributed (in either source or "
     "binary form) with the major components (compiler, kernel, and so on) of "
     "the operating system on which the executable runs, unless that "
     "component itself accompanies the executable."},
  {butText_just, 0, ""},
  {butText_just, 0, "If distribution of executable or object code is made by "
     "offering access to copy from a designated place, then offering "
     "equivalent access to copy the source code from the same place counts as "
     "distribution of the source code, even though third parties are not "
     "compelled to copy the source along with the object code."},
  {butText_just, 0, ""},
  {butText_just, 0, "  4. You may not copy, modify, sublicense, or "
     "distribute the Program except as expressly provided under this License. "
     " Any attempt otherwise to copy, modify, sublicense or distribute the "
     "Program is void, and will automatically terminate your rights under "
     "this License.  However, parties who have received copies, or rights, "
     "from you under this License will not have their licenses terminated so "
     "long as such parties remain in full compliance."},
  {butText_just, 0, ""},
  {butText_just, 0, "  5. You are not required to accept this License, since "
     "you have not signed it.  However, nothing else grants you permission to "
     "modify or distribute the Program or its derivative works.  These "
     "actions are prohibited by law if you do not accept this License.  "
     "Therefore, by modifying or distributing the Program (or any work based "
     "on the Program), you indicate your acceptance of this License to do so, "
     "and all its terms and conditions for copying, distributing or modifying "
     "the Program or works based on it."},
  {butText_just, 0, ""},
  {butText_just, 0, "  6. Each time you redistribute the Program (or any "
     "work based on the Program), the recipient automatically receives a "
     "license from the original licensor to copy, distribute or modify the "
     "Program subject to these terms and conditions.  You may not impose any "
     "further restrictions on the recipients' exercise of the rights granted "
     "herein.  You are not responsible for enforcing compliance by third "
     "parties to this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  7. If, as a consequence of a court judgment or "
     "allegation of patent infringement or for any other reason (not limited "
     "to patent issues), conditions are imposed on you (whether by court "
     "order, agreement or otherwise) that contradict the conditions of this "
     "License, they do not excuse you from the conditions of this License.  "
     "If you cannot distribute so as to satisfy simultaneously your "
     "obligations under this License and any other pertinent obligations, "
     "then as a consequence you may not distribute the Program at all.  For "
     "example, if a patent license would not permit royalty-free "
     "redistribution of the Program by all those who receive copies directly "
     "or indirectly through you, then the only way you could satisfy both it "
     "and this License would be to refrain entirely from distribution of the "
     "Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "If any portion of this section is held invalid or "
     "unenforceable under any particular circumstance, the balance of the "
     "section is intended to apply and the section as a whole is intended to "
     "apply in other circumstances."},
  {butText_just, 0, ""},
  {butText_just, 0, "It is not the purpose of this section to induce you to "
     "infringe any patents or other property right claims or to contest "
     "validity of any such claims; this section has the sole purpose of "
     "protecting the integrity of the free software distribution system, "
     "which is implemented by public license practices.  Many people have "
     "made generous contributions to the wide range of software distributed "
     "through that system in reliance on consistent application of that "
     "system; it is up to the author/donor to decide if he or she is willing "
     "to distribute software through any other system and a licensee cannot "
     "impose that choice."},
  {butText_just, 0, ""},
  {butText_just, 0, "This section is intended to make thoroughly clear what "
     "is believed to be a consequence of the rest of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  8. If the distribution and/or use of the Program is "
     "restricted in certain countries either by patents or by copyrighted "
     "interfaces, the original copyright holder who places the Program under "
     "this License may add an explicit geographical distribution limitation "
     "excluding those countries, so that distribution is permitted only in or "
     "among countries not thus excluded.  In such case, this License "
     "incorporates the limitation as if written in the body of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  9. The Free Software Foundation may publish revised "
     "and/or new versions of the General Public License from time to time.  "
     "Such new versions will be similar in spirit to the present version, but "
     "may differ in detail to address new problems or concerns."},
  {butText_just, 0, ""},
  {butText_just, 0, "Each version is given a distinguishing version number.  "
     "If the Program specifies a version number of this License which applies "
     "to it and \"any later version\", you have the option of following the "
     "terms and conditions either of that version or of any later version "
     "published by the Free Software Foundation.  If the Program does not "
     "specify a version number of this License, you may choose any version "
     "ever published by the Free Software Foundation."},
  {butText_just, 0, ""},
  {butText_just, 0, "  10. If you wish to incorporate parts of the Program "
     "into other free programs whose distribution conditions are different, "
     "write to the author to ask for permission.  For software which is "
     "copyrighted by the Free Software Foundation, write to the Free Software "
     "Foundation; we sometimes make exceptions for this.  Our decision will "
     "be guided by the two goals of preserving the free status of all "
     "derivatives of our free software and of promoting the sharing and reuse "
     "of software generally."},
  {butText_just, 0, ""},
  {butText_center, 0, "NO WARRANTY"},
  {butText_just, 0, ""},
  {butText_just, 0, "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, "
     "THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY "
     "APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT "
     "HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT "
     "WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT "
     "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
     "PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE "
     "OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU "
     "ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION."},
  {butText_just, 0, ""},
  {butText_just, 0, "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR "
     "AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO "
     "MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE "
     "LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL "
     "OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE "
     "PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING "
     "RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A "
     "FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF "
     "SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH "
     "DAMAGES."},
  {butText_just, 0, ""},
  {butText_center, 0, "END OF TERMS AND CONDITIONS"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

static xio_tb_t  copying_italian[] = {
  {butText_center, 1, "Pente " VERSION},
  {butText_center, 0, "di Bill Shubert"},
  {butText_center, 0, DATE_ITALIAN},
  {butText_just, 0, ""},
  {butText_just, 1, "Distribuzione"},
  {butText_just, 0, "   Questo programma è distribuito con la Gnu General "
     "Public License, versione 2.  Questa dovrebbe essere inclusa nella vostra "
     "copia del codice sorgente in un file chiamato \"COPYING\".  Ad ogni modo, "
     "la licenza è riportata anche qui di seguito."},
  {butText_just, 0, "   In sostanza, sono tre i punti di questa licenza che "
   "interesseranno la maggior parte di voi.  Una è che potete distribuire "
   "delle copie ai vostri amici.  Un'altra è che vi deve essere detto dove "
     "potete trovare il codice sorgente.  Lo potete avere da me; leggete la "
     "pagina di aiuto \"A proposito dell'autore\" per sapere come contattarmi.  "
     "L'ultima è che non c'è garanzia, quindi se questo programma vi rovina la "
     "vita, non venite a lamentarvi da me."},
  {butText_just, 0, "   Se volete fare qualcos'altro di questo programma, "
     "dovreste leggere la licenza che segue per i dettagli."},
  {butText_just, 0, ""},
  {butText_center, 0, "GNU GENERAL PUBLIC LICENSE"},
  {butText_center, 0, "TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND "
     "MODIFICATION"},
  {butText_just, 0, ""},
  {butText_just, 0, "  0. This License applies to any program or other work "
     "which contains a notice placed by the copyright holder saying it may be "
     "distributed under the terms of this General Public License.  The "
     "\"Program\", below, refers to any such program or work, and a \"work "
     "based on the Program\" means either the Program or any derivative work "
     "under copyright law: that is to say, a work containing the Program or a "
     "portion of it, either verbatim or with modifications and/or translated "
     "into another language.  (Hereinafter, translation is included without "
     "limitation in the term \"modification\".)  Each licensee is addressed "
     "as \"you\"."},
  {butText_just, 0, ""},
  {butText_just, 0, "Activities other than copying, distribution and "
     "modification are not covered by this License; they are outside its "
     "scope.  The act of running the Program is not restricted, and the "
     "output from the Program is covered only if its contents constitute a "
     "work based on the Program (independent of having been made by running "
     "the Program). Whether that is true depends on what the Program does."},
  {butText_just, 0, ""},
  {butText_just, 0, "  1. You may copy and distribute verbatim copies of the "
     "Program's source code as you receive it, in any medium, provided that "
     "you conspicuously and appropriately publish on each copy an appropriate "
     "copyright notice and disclaimer of warranty; keep intact all the "
     "notices that refer to this License and to the absence of any warranty; "
     "and give any other recipients of the Program a copy of this License "
     "along with the Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "You may charge a fee for the physical act of "
     "transferring a copy, and you may at your option offer warranty "
     "protection in exchange for a fee."},
  {butText_just, 0, ""},
  {butText_just, 0, "  2. You may modify your copy or copies of the Program "
     "or any portion of it, thus forming a work based on the Program, and "
     "copy and distribute such modifications or work under the terms of "
     "Section 1 above, provided that you also meet all of these "
     "conditions:"},
  {butText_just, 0, ""},
  {butText_just, 0, "    a) You must cause the modified files to carry "
     "prominent notices stating that you changed the files and the date of "
     "any change."},
  {butText_just, 0, ""},
  {butText_just, 0, "    b) You must cause any work that you distribute or "
     "publish, that in whole or in part contains or is derived from the "
     "Program or any part thereof, to be licensed as a whole at no charge to "
     "all third parties under the terms of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "    c) If the modified program normally reads commands "
     "interactively when run, you must cause it, when started running for "
     "such interactive use in the most ordinary way, to print or display an "
     "announcement including an appropriate copyright notice and a notice "
     "that there is no warranty (or else, saying that you provide a warranty) "
     "and that users may redistribute the program under these conditions, and "
     "telling the user how to view a copy of this License.  (Exception: if "
     "the Program itself is interactive but does not normally print such an "
     "announcement, your work based on the Program is not required to print "
     "an announcement.)"},
  {butText_just, 0, ""},
  {butText_just, 0, "These requirements apply to the modified work as a "
     "whole.  If identifiable sections of that work are not derived from the "
     "Program, and can be reasonably considered independent and separate "
     "works in themselves, then this License, and its terms, do not apply to "
     "those sections when you distribute them as separate works.  But when "
     "you distribute the same sections as part of a whole which is a work "
     "based on the Program, the distribution of the whole must be on the "
     "terms of this License, whose permissions for other licensees extend to "
     "the entire whole, and thus to each and every part regardless of who "
     "wrote it."},
  {butText_just, 0, ""},
  {butText_just, 0, "Thus, it is not the intent of this section to claim "
     "rights or contest your rights to work written entirely by you; rather, "
     "the intent is to exercise the right to control the distribution of "
     "derivative or collective works based on the Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "In addition, mere aggregation of another work not based "
     "on the Program with the Program (or with a work based on the Program) "
     "on a volume of a storage or distribution medium does not bring the "
     "other work under the scope of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  3. You may copy and distribute the Program (or a work "
     "based on it, under Section 2) in object code or executable form under "
     "the terms of Sections 1 and 2 above provided that you also do one of "
     "the following:"},
  {butText_just, 0, ""},
  {butText_just, 0, "    a) Accompany it with the complete corresponding "
     "machine-readable source code, which must be distributed under the terms "
     "of Sections 1 and 2 above on a medium customarily used for software "
     "interchange; or,"},
  {butText_just, 0, ""},
  {butText_just, 0, "    b) Accompany it with a written offer, valid for at "
     "least three years, to give any third party, for a charge no more than "
     "your cost of physically performing source distribution, a complete "
     "machine-readable copy of the corresponding source code, to be "
     "distributed under the terms of Sections 1 and 2 above on a medium "
     "customarily used for software interchange; or,"},
  {butText_just, 0, ""},
  {butText_just, 0, "    c) Accompany it with the information you received "
     "as to the offer to distribute corresponding source code.  (This "
     "alternative is allowed only for noncommercial distribution and only if "
     "you received the program in object code or executable form with such an "
     "offer, in accord with Subsection b above.)"},
  {butText_just, 0, ""},
  {butText_just, 0, "The source code for a work means the preferred form of "
     "the work for making modifications to it.  For an executable work, "
     "complete source code means all the source code for all modules it "
     "contains, plus any associated interface definition files, plus the "
     "scripts used to control compilation and installation of the executable. "
     " However, as a special exception, the source code distributed need not "
     "include anything that is normally distributed (in either source or "
     "binary form) with the major components (compiler, kernel, and so on) of "
     "the operating system on which the executable runs, unless that "
     "component itself accompanies the executable."},
  {butText_just, 0, ""},
  {butText_just, 0, "If distribution of executable or object code is made by "
     "offering access to copy from a designated place, then offering "
     "equivalent access to copy the source code from the same place counts as "
     "distribution of the source code, even though third parties are not "
     "compelled to copy the source along with the object code."},
  {butText_just, 0, ""},
  {butText_just, 0, "  4. You may not copy, modify, sublicense, or "
     "distribute the Program except as expressly provided under this License. "
     " Any attempt otherwise to copy, modify, sublicense or distribute the "
     "Program is void, and will automatically terminate your rights under "
     "this License.  However, parties who have received copies, or rights, "
     "from you under this License will not have their licenses terminated so "
     "long as such parties remain in full compliance."},
  {butText_just, 0, ""},
  {butText_just, 0, "  5. You are not required to accept this License, since "
     "you have not signed it.  However, nothing else grants you permission to "
     "modify or distribute the Program or its derivative works.  These "
     "actions are prohibited by law if you do not accept this License.  "
     "Therefore, by modifying or distributing the Program (or any work based "
     "on the Program), you indicate your acceptance of this License to do so, "
     "and all its terms and conditions for copying, distributing or modifying "
     "the Program or works based on it."},
  {butText_just, 0, ""},
  {butText_just, 0, "  6. Each time you redistribute the Program (or any "
     "work based on the Program), the recipient automatically receives a "
     "license from the original licensor to copy, distribute or modify the "
     "Program subject to these terms and conditions.  You may not impose any "
     "further restrictions on the recipients' exercise of the rights granted "
     "herein.  You are not responsible for enforcing compliance by third "
     "parties to this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  7. If, as a consequence of a court judgment or "
     "allegation of patent infringement or for any other reason (not limited "
     "to patent issues), conditions are imposed on you (whether by court "
     "order, agreement or otherwise) that contradict the conditions of this "
     "License, they do not excuse you from the conditions of this License.  "
     "If you cannot distribute so as to satisfy simultaneously your "
     "obligations under this License and any other pertinent obligations, "
     "then as a consequence you may not distribute the Program at all.  For "
     "example, if a patent license would not permit royalty-free "
     "redistribution of the Program by all those who receive copies directly "
     "or indirectly through you, then the only way you could satisfy both it "
     "and this License would be to refrain entirely from distribution of the "
     "Program."},
  {butText_just, 0, ""},
  {butText_just, 0, "If any portion of this section is held invalid or "
     "unenforceable under any particular circumstance, the balance of the "
     "section is intended to apply and the section as a whole is intended to "
     "apply in other circumstances."},
  {butText_just, 0, ""},
  {butText_just, 0, "It is not the purpose of this section to induce you to "
     "infringe any patents or other property right claims or to contest "
     "validity of any such claims; this section has the sole purpose of "
     "protecting the integrity of the free software distribution system, "
     "which is implemented by public license practices.  Many people have "
     "made generous contributions to the wide range of software distributed "
     "through that system in reliance on consistent application of that "
     "system; it is up to the author/donor to decide if he or she is willing "
     "to distribute software through any other system and a licensee cannot "
     "impose that choice."},
  {butText_just, 0, ""},
  {butText_just, 0, "This section is intended to make thoroughly clear what "
     "is believed to be a consequence of the rest of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  8. If the distribution and/or use of the Program is "
     "restricted in certain countries either by patents or by copyrighted "
     "interfaces, the original copyright holder who places the Program under "
     "this License may add an explicit geographical distribution limitation "
     "excluding those countries, so that distribution is permitted only in or "
     "among countries not thus excluded.  In such case, this License "
     "incorporates the limitation as if written in the body of this License."},
  {butText_just, 0, ""},
  {butText_just, 0, "  9. The Free Software Foundation may publish revised "
     "and/or new versions of the General Public License from time to time.  "
     "Such new versions will be similar in spirit to the present version, but "
     "may differ in detail to address new problems or concerns."},
  {butText_just, 0, ""},
  {butText_just, 0, "Each version is given a distinguishing version number.  "
     "If the Program specifies a version number of this License which applies "
     "to it and \"any later version\", you have the option of following the "
     "terms and conditions either of that version or of any later version "
     "published by the Free Software Foundation.  If the Program does not "
     "specify a version number of this License, you may choose any version "
     "ever published by the Free Software Foundation."},
  {butText_just, 0, ""},
  {butText_just, 0, "  10. If you wish to incorporate parts of the Program "
     "into other free programs whose distribution conditions are different, "
     "write to the author to ask for permission.  For software which is "
     "copyrighted by the Free Software Foundation, write to the Free Software "
     "Foundation; we sometimes make exceptions for this.  Our decision will "
     "be guided by the two goals of preserving the free status of all "
     "derivatives of our free software and of promoting the sharing and reuse "
     "of software generally."},
  {butText_just, 0, ""},
  {butText_center, 0, "NO WARRANTY"},
  {butText_just, 0, ""},
  {butText_just, 0, "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, "
     "THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY "
     "APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT "
     "HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT "
     "WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT "
     "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
     "PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE "
     "OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU "
     "ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION."},
  {butText_just, 0, ""},
  {butText_just, 0, "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR "
     "AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO "
     "MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE "
     "LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL "
     "OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE "
     "PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING "
     "RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A "
     "FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF "
     "SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH "
     "DAMAGES."},
  {butText_just, 0, ""},
  {butText_center, 0, "END OF TERMS AND CONDITIONS"},
  {butText_just, 0, ""},
  {butText_center, 0,
     XIO_PL1CHAR XIO_PL2CHAR XIO_PL1MARKCHAR XIO_PL2CHAR XIO_PL1CHAR},
  {butText_just, 0, ""},
  {0,0,NULL}};

xio_tb_t  *xioStr_proghelp[] = {proghelp_eng, proghelp_french,
			      proghelp_italian};
xio_tb_t  *xioStr_gamehelp[] = {gamehelp_eng, gamehelp_french,
			      gamehelp_italian};
xio_tb_t  *xioStr_setuphelp[] = {setuphelp_eng, setuphelp_french,
			       setuphelp_italian};
xio_tb_t  *xioStr_networkhelp[] = {network_eng, network_french,
				 network_italian};
xio_tb_t  *xioStr_comphelp[] = {computer_eng, computer_french,
			      computer_italian};
xio_tb_t  *xioStr_abouthelp[] = {author_eng, author_french,
			       author_italian};
xio_tb_t  *xioStr_copyinghelp[] = {copying_eng, copying_french,
				 copying_italian};

#endif  /* X11_DISP */
