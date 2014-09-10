/**
 * @file   CLP.c
 * @brief  LIMES Command Line Parser in ANSI-C
 *
 * LIMES Command Line Executor (CLE) in ANSI-C
 * @author limes datentechnik gmbh
 * @date  04.08.2014
 * @copyright (c) 2014 limes datentechnik gmbh
 * www.flam.de
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * If you need professional services or support for this library please
 * contact support@flam.de.
 **********************************************************************/
/*
 * TASK:0000086 Centralized error messages
 * TASK:0000086 Multi-language support
 */

/* Standard-Includes **************************************************/

#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "CLEPUTL.h"
/* Include der Schnittstelle ******************************************/
#include "FLAMCLP.h"

/* Definition der Version von FL-CLP **********************************
 *
 * Changelog:
 * 1.1.1: Adjust version and about
 * 1.1.2: Change escape sequence for strings and supplements to two times the same character (''/"")
 * 1.1.3: Support of command line or property file only parameter
 * 1.1.4: Support of dummy (DMY) flag for parameter which are not visible on command line and property file
 * 1.1.5: Support the use of different symbol tables for property and command line parsing
 * 1.1.6: Add pcClpError to provide an error message for an error code
 * 1.1.7: Add possibility to use getenv to overrule hard coded default values
 * 1.1.8: An empty path "" are handled like NULL pointer path
 * 1.1.9: Allow strings without ' like keyword=...SPACE/SEP and switch between " and '
 * 1.1.10: Add new flag to prevent print outs in clear form for passwords or other critical informations
 * 1.1.11: Add overlays and objects to parsed parameter list
 * 1.1.12: Correct generation of manpages
 * 1.1.13: Keywords can now be preceded by '-' or '--'
 * 1.1.14: Don't print manpage twice at end of path anymore
 * 1.1.15: Support of new flags to define default interpretation of binary strings (CHR/ASC/EBC/HEX)
 * 1.1.16: Get selections, object and overlays for aliases up and running
 * 1.1.17: Support line comment (initiated with ';' up to '\n')
 * 1.1.18: Support object without parenthesis and overlays without dot
 * 1.1.19: Extent strxcmp() to support keyword compare and stop flag
 * 1.1.20: Support flag to print all or only defined (set) properties
 * 1.1.21: Support flag to print aliases at help (if false aliases are now suppressed at help)
 * 1.1.22: Support property generation up to single parameters
 * 1.1.23: eliminate uiFlg to manage file properties and command line with the same symbol table
 * 1.1.24: Add support for parameter files for each object and overlay (read.text=parfilename.txt)
 * 1.1.25: Invent CLEPUTL.h/c
 * 1.1.26: Eliminate static variables to get more thread safe
 * 1.1.27: To save memory and simplify the usage of CLP the pointer to the data structure can be NULL
 * 1.1.28: Improve error handling (count rows and cols, print error msg and build error structure) and support isPfl-Flag
 * 1.1.29: Replace static variables for version and about to make it possible to use the lib as DLL
 * 1.1.30: Rework to make CLEP better usable with DLLs (eliminate global variables, adjust about and version, adjust includes)
 * 1.1.31: fix memory leaks found with memchecker
 * 1.1.32: use SELECTION as Type in argument lists if the selection flag on and KEYWORD for constant definitions
 * 1.1.33: If no SELECTION but keywords possible the help shows a additional statement that you can enter also a value
 **/

#define CLP_VSN_STR       "1.1.33"
#define CLP_VSN_MAJOR      1
#define CLP_VSN_MINOR        1
#define CLP_VSN_REVISION       33

/* Definition der Flag-Makros *****************************************/

#define CLPISS_ALI(flg)          ((flg)&CLPFLG_ALI)
#define CLPISS_CON(flg)          ((flg)&CLPFLG_CON)
#define CLPISS_CMD(flg)          ((flg)&CLPFLG_CMD)
#define CLPISS_PRO(flg)          ((flg)&CLPFLG_PRO)
#define CLPISS_DMY(flg)          ((flg)&CLPFLG_DMY)
#define CLPISS_SEL(flg)          ((flg)&CLPFLG_SEL)
#define CLPISS_FIX(flg)          ((flg)&CLPFLG_FIX)
#define CLPISS_BIN(flg)          ((flg)&CLPFLG_BIN)
#define CLPISS_CNT(flg)          ((flg)&CLPFLG_CNT)
#define CLPISS_OID(flg)          ((flg)&CLPFLG_OID)
#define CLPISS_ELN(flg)          ((flg)&CLPFLG_ELN)
#define CLPISS_SLN(flg)          ((flg)&CLPFLG_SLN)
#define CLPISS_TLN(flg)          ((flg)&CLPFLG_TLN)
#define CLPISS_PWD(flg)          ((flg)&CLPFLG_PWD)
#define CLPISS_CHR(flg)          ((flg)&CLPFLG_CHR)
#define CLPISS_ASC(flg)          ((flg)&CLPFLG_ASC)
#define CLPISS_EBC(flg)          ((flg)&CLPFLG_EBC)
#define CLPISS_HEX(flg)          ((flg)&CLPFLG_HEX)
#define CLPISS_LNK(flg)          (CLPISS_CNT(flg) ||  CLPISS_OID(flg) ||  CLPISS_ELN(flg) || CLPISS_SLN(flg) ||  CLPISS_TLN(flg))
#define CLPISS_ARG(flg)          ((!CLPISS_LNK(flg)) && (!CLPISS_CON(flg)) && (!CLPISS_ALI(flg)))

/* Definition der Konstanten ******************************************/

#define CLPMAX_TABCNT            256
#define CLPMAX_HDEPTH            128
#define CLPMAX_LEXLEN            1023
#define CLPMAX_LEXSIZ            1024
#define CLPMAX_PRELEN            1023
#define CLPMAX_PRESIZ            1024
#define CLPMAX_PATLEN            1023
#define CLPMAX_PATSIZ            1024
#define CLPMAX_LSTLEN            65535
#define CLPMAX_LSTSIZ            65536
#define CLPMAX_MSGLEN            1023
#define CLPMAX_MSGSIZ            1024

#define CLPTOK_INI               0
#define CLPTOK_END               1
#define CLPTOK_KYW               2
#define CLPTOK_RBO               3
#define CLPTOK_RBC               4
#define CLPTOK_SBO               5
#define CLPTOK_SBC               6
#define CLPTOK_SGN               7
#define CLPTOK_DOT               8
#define CLPTOK_STR               9
#define CLPTOK_NUM               10
#define CLPTOK_FLT               11
#define CLPTOK_SUP               12

static const char*               apClpTok[]={
      "INI",
      "END",
      "KEYWORD",
      "ROUND-BRACKET-OPEN",
      "ROUND-BRACKET-CLOSE",
      "SQUARED-BRACKET-OPEN",
      "SQUARED-BRACKET-CLOSE",
      "SIGN",
      "DOT",
      "STRING",
      "NUMBER",
      "FLOAT",
      "SUPPLEMENT"};

static const char*        apClpTyp[]={
      "NO-TYP",
      "SWITCH",
      "NUMBER",
      "FLOAT",
      "STRING",
      "OBJECT",
      "OVERLAY"};

#define CLP_ASSIGNMENT     "="

static const char* pcClpErr(int siErr) {
   switch (siErr) {
   case CLPERR_LEX: return("LEXICAL-ERROR");
   case CLPERR_SYN: return("SYNTAX-ERROR");
   case CLPERR_SEM: return("SEMANTIC-ERROR");
   case CLPERR_TYP: return("TYPE-ERROR");
   case CLPERR_TAB: return("TABLE-ERROR");
   case CLPERR_SIZ: return("SIZE-ERROR");
   case CLPERR_PAR: return("PARAMETER-ERROR");
   case CLPERR_MEM: return("MEMORY-ERROR");
   case CLPERR_INT: return("INTERNAL-ERROR");
   case CLPERR_SYS: return("SYSTEM-ERROR");
   default        : return("UNKNOWN-ERROR");
   }
}

/* Definition der Strukturen ******************************************/

typedef struct Std {
   const char*                   pcKyw;
   const char*                   pcAli;
   unsigned long                 uiFlg;
   struct Sym*                   psAli;
   int                           siKwl;
   int                           siLev;
   int                           siPos;
}TsStd;

typedef struct Fix {
   const char*                   pcDft;
   const char*                   pcMan;
   const char*                   pcHlp;
   char                          acPro[CLPMAX_LEXSIZ];
   int                           siTyp;
   int                           siMin;
   int                           siMax;
   int                           siSiz;
   int                           siOfs;
   int                           siOid;
   struct Sym*                   psLnk;
   struct Sym*                   psCnt;
   struct Sym*                   psOid;
   struct Sym*                   psEln;
   struct Sym*                   psSln;
   struct Sym*                   psTln;
}TsFix;

typedef struct Var {
   void*                         pvDat;
   void*                         pvPtr;
   int                           siCnt;
   int                           siLen;
   int                           siRst;
}TsVar;

typedef struct Sym {
   struct Sym*                   psNxt;
   struct Sym*                   psBak;
   struct Sym*                   psDep;
   struct Sym*                   psHih;
   TsStd*                        psStd;
   TsFix*                        psFix;
   TsVar*                        psVar;
}TsSym;

typedef struct Hdl {
   const char*                   pcOwn;
   const char*                   pcPgm;
   const char*                   pcCmd;
   const char*                   pcMan;
   const char*                   pcHlp;
   const char*                   pcSrc;
   const char*                   pcCur;
   const char*                   pcOld;
   const char*                   pcDep;
   const char*                   pcOpt;
   const char*                   pcEnt;
   const char*                   pcRow;
   int                           siMkl;
   int                           isOvl;
   int                           isChk;
   int                           isCas;
   int                           isPfl;
   int                           siTok;
   char                          acLex[CLPMAX_LEXSIZ];
   char                          acSrc[CLPMAX_LEXSIZ];
   TsSym*                        psSym;
   void*                         pvDat;
   FILE*                         pfHlp;
   FILE*                         pfErr;
   FILE*                         pfSym;
   FILE*                         pfScn;
   FILE*                         pfPrs;
   FILE*                         pfBld;
   const TsSym*                  apPat[CLPMAX_HDEPTH];
   char                          acPat[CLPMAX_PATSIZ];
   char                          acPre[CLPMAX_PRESIZ];
   char                          acLst[CLPMAX_LSTSIZ];
   char                          acMsg[CLPMAX_MSGSIZ];
   int                           siRow;
   int                           siCol;
   int                           siErr;
} TsHdl;


/* Deklaration der internen Funktionen ********************************/

static TsSym* psClpSymIns(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const TsClpArgument*          psArg,
   TsSym*                        psHih,
   TsSym*                        psCur);

static int siClpSymIni(
   void*                         pvHdl,
   const int                     siLev,
   const TsClpArgument*          psArg,
   const TsClpArgument*          psTab,
   TsSym*                        psHih,
   TsSym**                       ppFst);

static int siClpSymCal(
   void*                         pvHdl,
   int                           siLev,
   TsSym*                        psArg,
   TsSym*                        psTab);

static int siClpSymFnd(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const char*                   pcKyw,
   const TsSym*                  psTab,
   TsSym**                       ppArg,
   int*                          piElm);

static void vdClpSymPrn(
   void*                         pvHdl,
   int                           siLev,
   TsSym*                        psSym);

static void vdClpSymTrc(
   void*                         pvHdl);

static void vdClpSymDel(
   TsSym*                        psSym);

static int siClpScnNat(
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char**                  ppCur,
   char*                         pcLex,
   const int                     uiTok,
   const TsSym*                  psArg);

static int siClpScnSrc(
   void*                         pvHdl,
   const int                     uiTok,
   const TsSym*                  psArg);

static int siClpPrsMain(
   void*                         pvHdl,
   TsSym*                        psTab,
   int*                          piOid);

static int siClpPrsParLst(
   void*                         pvHdl,
   const int                     siLev,
   const TsSym*                  psArg,
   const TsSym*                  psTab);

static int siClpPrsPar(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const TsSym*                  psTab,
   int*                          piOid);

static int siClpPrsSwt(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsSgn(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsFil(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsObjWob(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsObj(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsOvl(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsAry(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsValLst(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siTok,
   const int                     siTyp,
   TsSym*                        psArg);

static int siClpPrsObjLst(
   void*                         pvHdl,
   const int                     siLev,
   TsSym*                        psArg);

static int siClpPrsOvlLst(
   void*                         pvHdl,
   const int                     siLev,
   TsSym*                        psArg);

static int siClpPrsVal(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     siTok,
   const int                     siTyp,
   TsSym*                        psArg);

static int siClpPrsProLst(
   void*                         pvHdl,
   const TsSym*                  psTab);

static int siClpPrsPro(
   void*                         pvHdl,
   const int                     siPos,
   const TsSym*                  psTab);

static int siClpPrsKywLst(
   void*                         pvHdl,
   const int                     siPos,
   char*                         pcPat);

static int siClpBldPro(
   void*                         pvHdl,
   const char*                   pcPat,
   const char*                   pcPro);

static int siClpBldLnk(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     siNum,
   TsSym*                        psArg,
   const int                     isApp);

static int siClpBldSwt(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpBldLit(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     siTyp,
   TsSym*                        psArg,
   const char*                   pcVal);

static int siClpBldCon(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psVal);

static int siClpIniMainObj(
   void*                         pvHdl,
   TsSym*                        psTab,
   TsVar*                        psSav);

static int siClpFinMainObj(
   void*                         pvHdl,
   TsSym*                        psTab,
   const TsVar*                  psSav);

static int siClpIniMainOvl(
   void*                         pvHdl,
   TsSym*                        psTab,
   TsVar*                        psSav);

static int siClpFinMainOvl(
   void*                         pvHdl,
   TsSym*                        psTab,
   const TsVar*                  psSav,
   const int                     siOid);

static int siClpIniObj(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar*                        psSav);

static int siClpFinObj(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar*                  psSav);

static int siClpIniOvl(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar*                        psSav);

static int siClpFinOvl(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar*                  psSav,
   const int                     siOid);

static int siClpSetDefault(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static void vdClpPrnArg(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siLev,
   const char*                   pcKyw,
   const char*                   pcAli,
   const int                     siKwl,
   const int                     siTyp,
   const char*                   pcHlp,
   const char*                   pcDft,
   const int                     isSel,
   const int                     isCon);

static void vdClpPrnArgTab(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siLev,
   int                           siTyp,
   const TsSym*                  psTab);

static void vdClpPrnAli(
   FILE*                         pfOut,
   const char*                   pcSep,
   const TsSym*                  psTab);

static void vdClpPrnOpt(
   FILE*                         pfOut,
   const char*                   pcSep,
   const int                     siTyp,
   const TsSym*                  psTab);

static int siClpPrnSyn(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     isPat,
   const int                     siLev,
   const TsSym*                  psArg);

static int siClpPrnCmd(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siCnt,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psArg,
   const TsSym*                  psTab,
   const int                     isSkr,
   const int                     isMin);

static int siClpPrnHlp(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     isAli,
   const int                     siLev,
   const int                     siDep,
   const int                     siTyp,
   const TsSym*                  psArg,
   const int                     isFlg);

static int siClpPrnDoc(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const int                     isNbr,
   const int                     siLev,
   const char*                   pcNum,
   const TsSym*                  psArg,
   const TsSym*                  psTab);

static int siClpPrnPro(
   void*                         pvHdl,
   FILE*                         pfOut,
   int                           isMan,
   const int                     isAll,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psTab,
   const char*                   pcArg);

static char* fpcPre(
   void*                         pvHdl,
   const int                     siLev);

static char* fpcPat(
   void*                         pvHdl,
   const int                     siLev);

static int CLPERR(TsHdl* psHdl,int siErr, char* pcMsg, ...) {
   const char*          p;
   int                  i,l,f=FALSE;
   va_list              argv;
   psHdl->siErr=siErr;
   if (psHdl->pcRow!=NULL && psHdl->pcOld>=psHdl->pcRow) {
      psHdl->siCol=(int)((psHdl->pcOld-psHdl->pcRow)+1);
   } else psHdl->siCol=0;
   if (psHdl->pfErr!=NULL) {
      fprintf(psHdl->pfErr,"%s:\n%s ",pcClpErr(siErr),fpcPre(psHdl,0));
      va_start(argv,pcMsg); vfprintf(psHdl->pfErr,pcMsg,argv); va_end(argv);
      fprintf(psHdl->pfErr,"\n");
      if (psHdl->pcSrc!=NULL && psHdl->pcRow!=NULL && psHdl->pcOld!=NULL && psHdl->pcCur!=NULL && (psHdl->pcCur>psHdl->pcSrc || strlen(psHdl->acLst) || psHdl->siRow)) {
         if (strcmp(psHdl->acSrc,CLPSRC_CMD)==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from command line\n", fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol);
         } else if (strcmp(psHdl->acSrc,CLPSRC_PRO)==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from property list\n",fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol);
         } else {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d in file %s\n",        fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->acSrc);
         }
         fprintf(psHdl->pfErr,"%s \"",fpcPre(psHdl,1));
         for (p=psHdl->pcRow;!iscntrl(*p);p++) fprintf(psHdl->pfErr,"%c",*p);
         fprintf(psHdl->pfErr,"\"\n");
         if (psHdl->pcCur==psHdl->pcRow) {
            fprintf(psHdl->pfErr,"%s ^",fpcPre(psHdl,1));
         } else {
            fprintf(psHdl->pfErr,"%s  ",fpcPre(psHdl,1));
         }
         for (p=psHdl->pcRow;!iscntrl(*p);p++) {
            if (p>=psHdl->pcOld && p<psHdl->pcCur) {
               f=TRUE;
               fprintf(psHdl->pfErr,"^");
            } else {
               fprintf(psHdl->pfErr," ");
            }
         }
         if (f) {
            fprintf(psHdl->pfErr," \n");
         } else {
            fprintf(psHdl->pfErr,"^\n");
         }
         l=strlen(psHdl->acLst);
         if (l>1) {
            l--;
            fprintf(psHdl->pfErr,"%s After successful parsing of arguments below:\n",fpcPre(psHdl,0));
            fprintf(psHdl->pfErr,"%s ",fpcPre(psHdl ,1));
            for (i=0;i<l;i++) {
               if (psHdl->acLst[i]=='\n') {
                  fprintf(psHdl->pfErr,"\n%s ",fpcPre(psHdl,1));
               } else fprintf(psHdl->pfErr,"%c",psHdl->acLst[i]);
            }
            fprintf(psHdl->pfErr,"\n");
         } else fprintf(psHdl->pfErr,"%s Something is wrong with the first argument\n",fpcPre(psHdl,0));
      }
   }
   l=sprintf(psHdl->acMsg,"%s: ",pcClpErr(siErr));
   if (l>0) {
      va_start(argv,pcMsg); vsnprintf(psHdl->acMsg+l,CLPMAX_MSGLEN-(l+1),pcMsg,argv); va_end(argv);
   } else {
      sprintf(psHdl->acMsg,"%s",pcClpError(siErr));
   }
   return(siErr);
}

static void CLPERRADD(TsHdl* psHdl,int siLev, char* pcMsg, ...) {
   if (psHdl->pfErr!=NULL) {
      va_list              argv;
      fprintf(psHdl->pfErr,"%s ",fpcPre(psHdl,siLev));
      va_start(argv,pcMsg); vfprintf(psHdl->pfErr,pcMsg,argv); va_end(argv);
      fprintf(psHdl->pfErr,"\n");
   }
}

/* Implementierung der externen Funktionen ****************************/

extern const char* pcClpVersion(const int l, const int s, char* b)
{
   snprintc(b,s,"%2.2d FLAM-CLP VERSION: %s.%u BUILD: %s %s %s\n",l,CLP_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__);
   return(b);
}

extern const char* pcClpAbout(const int l, const int s, char* b)
{
   snprintc(b,s,
   "%2.2d Frankenstein Limes Command Line Parser (FLAM-CLP)\n"
   "   Version: %s.%u Build: %s %s %s\n"
   "   Copyright (C) limes datentechnik (R) gmbh\n"
   "   This library is open source from the FLAM(R) project: http://www.flam.de\n"
   "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
   ,l,CLP_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__);
   return(b);
}

extern char* pcClpError(
   int               siErr)
{
   switch(siErr) {
   case CLP_OK    :return("No error, everything O.K.");
   case CLPERR_LEX:return("Lexical error (determined by scanner)");
   case CLPERR_SYN:return("Syntax error (determined by parser)");
   case CLPERR_SEM:return("Semantic error (determined by builder)");
   case CLPERR_TYP:return("Type error (internal error with argument types)");
   case CLPERR_TAB:return("Table error (internal error with argument tables)");
   case CLPERR_SIZ:return("Size error (internal error with argument tables and data structures)");
   case CLPERR_PAR:return("Parameter error (internal error with argument tables and data structures)");
   case CLPERR_MEM:return("Memory error (internal error with argument tables and data structures)");
   case CLPERR_INT:return("Internal error (internal error with argument tables and data structures)");
   case CLPERR_SYS:return("System error (internal error with argument tables and data structures)");
   default:        return("Unknown error (not expected)");
   }
}

extern void* pvClpOpen(
   const int                     isCas,
   const int                     isPfl,
   const int                     siMkl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const int                     isOvl,
   const TsClpArgument*          psTab,
   void*                         pvDat,
   FILE*                         pfHlp,
   FILE*                         pfErr,
   FILE*                         pfSym,
   FILE*                         pfScn,
   FILE*                         pfPrs,
   FILE*                         pfBld,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsClpError*                   psErr)
{
   TsHdl*                        psHdl=NULL;
   int                           siErr;
   if (pcOwn!=NULL && pcPgm!=NULL && pcCmd!=NULL && psTab!=NULL) {
      psHdl=(TsHdl*)calloc(1,sizeof(TsHdl));
      if (psHdl!=NULL) {
         psHdl->isCas=isCas;
         psHdl->isPfl=isPfl;
         psHdl->siMkl=siMkl;
         psHdl->pcOwn=pcOwn;
         psHdl->pcPgm=pcPgm;
         psHdl->pcCmd=pcCmd;
         psHdl->pcMan=pcMan;
         psHdl->pcHlp=pcHlp;
         psHdl->isOvl=isOvl;
         psHdl->pcSrc=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->siTok=CLPTOK_INI;
         psHdl->acLex[0]=EOS;
         psHdl->pvDat=pvDat;
         psHdl->psSym=NULL;
         psHdl->isChk=FALSE;
         if (pfHlp==NULL) psHdl->pfHlp=stderr; else psHdl->pfHlp=pfHlp;
         psHdl->pfErr=pfErr;
         psHdl->pfSym=pfSym;
         psHdl->pfScn=pfScn;
         psHdl->pfPrs=pfPrs;
         psHdl->pfBld=pfBld;
         psHdl->pcDep=pcDep;
         psHdl->pcOpt=pcOpt;
         psHdl->pcEnt=pcEnt;
         siErr=siClpSymIni(psHdl,0,NULL,psTab,NULL,&psHdl->psSym);
         if (siErr<0) { vdClpSymDel(psHdl->psSym); free(psHdl); return(NULL); }
         siErr=siClpSymCal(psHdl,0,NULL,psHdl->psSym);
         if (siErr<0) { vdClpSymDel(psHdl->psSym); free(psHdl); return(NULL); }
         vdClpSymTrc(psHdl);
         if (psErr!=NULL) {
            psErr->pcMsg=psHdl->acMsg;
            psErr->pcSrc=psHdl->acSrc;
            psErr->piRow=&psHdl->siRow;
            psErr->piCol=&psHdl->siCol;
         }
      }
   }
   return((void*)psHdl);
}

extern int siClpParsePro(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcPro,
   const int                     isChk,
   char**                        ppLst)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siCnt;

   psHdl->acLst[0]=EOS;
   if (pcSrc!=NULL && strlen(pcSrc)) {
      snprintf(psHdl->acSrc,sizeof(psHdl->acSrc),"%s",pcSrc);
   } else strcpy(psHdl->acSrc,CLPSRC_PRO);
   psHdl->pcSrc=pcPro;
   psHdl->pcCur=pcPro;
   psHdl->pcOld=pcPro;
   psHdl->pcRow=pcPro;
   psHdl->isChk=isChk;
   if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
   psHdl->siRow=1;
   psHdl->siCol=0;
   psHdl->acLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PROPERTY-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
      if (psHdl->siTok<0) return(psHdl->siTok);
      siCnt=siClpPrsProLst(pvHdl,psHdl->psSym);
      if (siCnt<0) return(siCnt);
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcSrc=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->acLex[0]=EOS;
         psHdl->isChk=FALSE;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PROPERTY-PARSER-END(CNT=%d)\n",siCnt);
         return(siCnt);
      } else return CLPERR(psHdl,CLPERR_SYN,"Last token (%s) of property list is not EOS",apClpTok[psHdl->siTok]);
   } else return CLPERR(psHdl,CLPERR_SYN,"Initial token (%s) in handle is not valid",apClpTok[psHdl->siTok]);
}

extern int siClpParseCmd(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcCmd,
   const int                     isChk,
   int*                          piOid,
   char**                        ppLst)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siCnt;

   psHdl->acLst[0]=EOS;
   if (pcSrc!=NULL && strlen(pcSrc)) {
      snprintf(psHdl->acSrc,sizeof(psHdl->acSrc),"%s",pcSrc);
   } else strcpy(psHdl->acSrc,CLPSRC_CMD);
   psHdl->pcSrc=pcCmd;
   psHdl->pcCur=pcCmd;
   psHdl->pcOld=pcCmd;
   psHdl->pcRow=pcCmd;
   psHdl->isChk=isChk;
   if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
   psHdl->siRow=1;
   psHdl->siCol=0;
   psHdl->acLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"COMMAND-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
      if (psHdl->siTok<0) return(psHdl->siTok);
      siCnt=siClpPrsMain(pvHdl,psHdl->psSym,piOid);
      if (siCnt<0) return (siCnt);
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcSrc=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->acLex[0]=EOS;
         psHdl->isChk=FALSE;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"COMMAND-PARSER-END(CNT=%d)\n",siCnt);
         return(siCnt);
      } else return CLPERR(psHdl,CLPERR_SYN,"Last token (%s) of parameter list is not EOS",apClpTok[psHdl->siTok]);
   } else return CLPERR(psHdl,CLPERR_SYN,"Initial token (%s) in handle is not valid",apClpTok[psHdl->siTok]);
}

extern int siClpSyntax(
   void*                         pvHdl,
   const int                     isSkr,
   const int                     isMin,
   const int                     siDep,
   const char*                   pcPat)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psSym;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_LEXSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);

   psHdl->pcSrc=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->acLex[0]=EOS;
   psHdl->acMsg[0]=EOS;
   psHdl->acLst[0]=EOS;
   psHdl->acPat[0]=EOS;
   psHdl->acPre[0]=EOS;
   strcpy(psHdl->acSrc,":SYNTAX:");

   if (pcPat!=NULL && strlen(pcPat)) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
         }
         for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
            for (pcKyw=pcPtr+1,i=0;pcKyw[i]!=EOS && pcKyw[i]!='.' && i<CLPMAX_LEXLEN;i++) acKyw[i]=pcKyw[i];
            acKyw[i]=EOS;
            siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
            if (siErr<0) return(siErr);
            psHdl->apPat[siLev]=psArg;
            psTab=psArg->psDep;
         }
         if (psTab!=NULL && !CLPISS_CON(psTab->psStd->uiFlg)) {
            siErr=siClpPrnCmd(pvHdl,psHdl->pfHlp,0,siLev,siLev+siDep,psArg,psTab,isSkr,isMin);
            if (siErr<0) return (siErr);
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains to many or invalid qualifiers",pcPat);
         }
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
      }
   } else {
      siErr=siClpPrnCmd(pvHdl,psHdl->pfHlp,0,0,siDep,NULL,psHdl->psSym,isSkr,isMin);
      if (siErr<0) return (siErr);
   }
   if (isSkr) fprintf(psHdl->pfHlp,"\n");
   return(CLP_OK);
}

extern int siClpHelp(
   void*                         pvHdl,
   const int                     siDep,
   const char*                   pcPat,
   const int                     isAli,
   const int                     isMan)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psSym;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_LEXSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);

   psHdl->pcSrc=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->acLex[0]=EOS;
   psHdl->acMsg[0]=EOS;
   psHdl->acLst[0]=EOS;
   psHdl->acPat[0]=EOS;
   psHdl->acPre[0]=EOS;
   strcpy(psHdl->acSrc,":HELP:");

   if (pcPat!=NULL && strlen(pcPat)) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
         }
         for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
            for (pcKyw=pcPtr+1,i=0;pcKyw[i]!=EOS && pcKyw[i]!='.' && i<CLPMAX_LEXLEN;i++) acKyw[i]=pcKyw[i];
            acKyw[i]=EOS;
            siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
            if (siErr<0) return(siErr);
            psHdl->apPat[siLev]=psArg;
            psTab=psArg->psDep;
         }
         if (psTab!=NULL) {
            if (siDep==0) {
               if (psArg==NULL) {
                  if (psHdl->pcMan!=NULL && strlen(psHdl->pcMan)) {
                     fprintf(psHdl->pfHlp,"%s\n",psHdl->pcMan);
                  } else {
                     fprintf(psHdl->pfHlp,"No detailed description available for this command.\n");
                  }
               } else {
                  if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
                     fprintf(psHdl->pfHlp,"%s\n",psArg->psFix->pcMan);
                  } else {
                     fprintf(psHdl->pfHlp,"No detailed description available for this argument.\n");
                  }
               }
            } else {
               if (psArg->psFix->siTyp==CLPTYP_OBJECT || psArg->psFix->siTyp==CLPTYP_OVRLAY) {
                  siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,-1,psTab,FALSE);
                  if (siErr<0) return(siErr);
               } else {
                  if (CLPISS_SEL(psArg->psStd->uiFlg)) {
                     siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,psArg->psFix->siTyp,psTab,FALSE);
                     if (siErr<0) return(siErr);
                  } else {
                     siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,psArg->psFix->siTyp,psTab,TRUE);
                     if (siErr<0) return(siErr);
                  }
               }
            }
         } else {
            if (isMan) {
               if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
                  fprintf(psHdl->pfHlp,"%s\n",psArg->psFix->pcMan);
               } else {
                  fprintf(psHdl->pfHlp,"No detailed description available for this argument.\n");
               }
            } else {
               fprintf(psHdl->pfHlp,"No farther arguments available.\n");
            }
         }
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
      }
   } else {
      if (siDep==0) {
         if (psHdl->pcMan!=NULL && strlen(psHdl->pcMan)) {
            fprintf(psHdl->pfHlp,"%s\n",psHdl->pcMan);
         } else {
            fprintf(psHdl->pfHlp,"No detailed description available for this command.\n");
         }
      } else {
         siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,0,siDep,-1,psTab,FALSE);
         if (siErr<0) return(siErr);
      }
   }
   return(CLP_OK);
}

extern int siClpDocu(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const char*                   pcPat,
   const char*                   pcNum,
   const int                     isDep,
   const int                     isMan,
   const int                     isNbr)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psSym;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_LEXSIZ];
   int                           siErr=0,siLev,siPos,i;
   int                           l=strlen(psHdl->pcCmd);
   char                          acNum[CLPMAX_PRESIZ];
   char                          acHlp[CLPMAX_PRESIZ];
   char                          acArg[20];
   const char*                   p;

   psHdl->pcSrc=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->acLex[0]=EOS;
   psHdl->acMsg[0]=EOS;
   psHdl->acLst[0]=EOS;
   psHdl->acPat[0]=EOS;
   psHdl->acPre[0]=EOS;
   strcpy(psHdl->acSrc,":DOCU:");

   if (pcNum!=NULL && strlen(pcNum)<100) {
      if (pcPat!=NULL && strlen(pcPat)) {
         if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
            if (strlen(pcPat)>l && pcPat[l]!='.') {
               return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
            }
            if (strlen(pcPat)>l) {
               strcpy(acNum,pcNum);
               for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
                  for (pcKyw=pcPtr+1,i=0;pcKyw[i]!=EOS && pcKyw[i]!='.' && i<CLPMAX_LEXLEN;i++) acKyw[i]=pcKyw[i];
                  acKyw[i]=EOS;
                  siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,&siPos);
                  if (siErr<0) return(siErr);
                  psHdl->apPat[siLev]=psArg;
                  psTab=psArg->psDep;
                  sprintf(acHlp,"%s%d.",acNum,siPos+1);
                  strcpy(acNum,acHlp);
               }
               if (psArg!=NULL) {
                  if (CLPISS_ARG(psArg->psStd->uiFlg)) {
                     if (isMan) {
                        if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
                           for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           fprintf(pfDoc,".");
                           l=strlen(psHdl->pcPgm)+strlen(fpcPat(pvHdl,siLev))+4;
                        } else {
                           l=strlen(fpcPat(pvHdl,siLev))+3;
                        }
                        for (p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                        fprintf(pfDoc,"(3)\n");
                        for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
                        fprintf(pfDoc,":doctype: manpage\n\n");
                        fprintf(pfDoc,"NAME\n");
                        fprintf(pfDoc,"----\n\n");
                        if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
                           for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           fprintf(pfDoc,".");
                           for (p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                        } else {
                            vdClpPrnAli(pfDoc,", ",psArg);
                        }
                        fprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
                        fprintf(pfDoc,"SYNOPSIS\n");
                        fprintf(pfDoc,"--------\n\n");
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n");
                        fprintf(pfDoc,"PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                        fprintf(pfDoc,"TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                        fprintf(pfDoc,"SYNTAX: "); siErr=siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev-1,psArg); fprintf(pfDoc,"\n");
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
                        if (siErr<0) return(siErr);
                        fprintf(pfDoc,"DESCRIPTION\n");
                        fprintf(pfDoc,"-----------\n\n");
                        if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
                           fprintf(pfDoc,"%s\n\n",psArg->psFix->pcMan);
                        } else {
                           fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                        }
                        fprintf(pfDoc,"AUTHOR\n");
                        fprintf(pfDoc,"------\n\n");
                        fprintf(pfDoc,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
                     } else {
                        switch (psArg->psFix->siTyp){
                        case CLPTYP_OBJECT:strcpy(acArg,"OBJECT");break;
                        case CLPTYP_OVRLAY:strcpy(acArg,"OVERLAY");break;
                        default           :strcpy(acArg,"PARAMETER");break;
                        }
                        if (isNbr) {
                           fprintf(pfDoc,"%s %s \'%s\'\n",acNum,acArg,psArg->psStd->pcKyw);
                           l=strlen(acNum)+strlen(acArg)+strlen(psArg->psStd->pcKyw)+4;
                           for (i=0;i<l;i++) fprintf(pfDoc,"^"); fprintf(pfDoc,"\n\n");
                        } else {
                           fprintf(pfDoc,"%s \'%s\'\n",acArg,psArg->psStd->pcKyw);
                           l=strlen(acArg)+strlen(psArg->psStd->pcKyw)+3;
                           for (i=0;i<l;i++) fprintf(pfDoc,"^"); fprintf(pfDoc,"\n\n");
                        }
                        fprintf(pfDoc,".SYNOPSIS\n\n");
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n");
                        fprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
                        fprintf(pfDoc,"PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                        fprintf(pfDoc,"TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                        fprintf(pfDoc,"SYNTAX: "); siErr=siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev-1,psArg); fprintf(pfDoc,"\n");
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
                        if (siErr<0) return(siErr);
                        fprintf(pfDoc,".DESCRIPTION\n\n");
                        if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
                           fprintf(pfDoc,"%s\n\n",psArg->psFix->pcMan);
                        } else {
                           fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                        }
                        fprintf(pfDoc,"indexterm:[Argument %s]\n\n\n",psArg->psStd->pcKyw);
                        if (isDep) {
                           siErr=siClpPrnDoc(pvHdl,pfDoc,siLev,isNbr,acNum,psArg,psTab);
                           if (siErr<0) return(siErr);
                        }
                     }
                  } else if (CLPISS_CON(psArg->psStd->uiFlg)) {
                     if (isMan) {
                        if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
                           for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           fprintf(pfDoc,".");
                           l=strlen(psHdl->pcPgm)+strlen(fpcPat(pvHdl,siLev))+4;
                        } else {
                           l=strlen(fpcPat(pvHdl,siLev))+3;
                        }
                        for(p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                        fprintf(pfDoc,"(3)\n");
                        for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
                        fprintf(pfDoc,":doctype: manpage\n\n");
                        fprintf(pfDoc,"NAME\n");
                        fprintf(pfDoc,"----\n\n");
                        if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
                           for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           fprintf(pfDoc,".");
                           for (p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                        } else {
                           for (p=psArg->psStd->pcKyw;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                        }
                        fprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
                        fprintf(pfDoc,"SYNOPSIS\n");
                        fprintf(pfDoc,"--------\n\n");
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n");
                        fprintf(pfDoc,"PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                        fprintf(pfDoc,"TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                        fprintf(pfDoc,"SYNTAX: %s\n",psArg->psStd->pcKyw);
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
                        if (siErr<0) return(siErr);
                        fprintf(pfDoc,"DESCRIPTION\n");
                        fprintf(pfDoc,"-----------\n\n");
                        if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
                           fprintf(pfDoc,"%s\n\n",psArg->psFix->pcMan);
                        } else {
                           fprintf(pfDoc,"No detailed description available for this constant.\n\n");
                        }
                        fprintf(pfDoc,"AUTHOR\n");
                        fprintf(pfDoc,"------\n\n");
                        fprintf(pfDoc,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
                     } else {
                        if (isNbr) {
                           fprintf(pfDoc,"%s CONSTANT \'%s\'\n",acNum,psArg->psStd->pcKyw);
                           l=strlen(acNum)+strlen(psArg->psStd->pcKyw)+12;
                           for (i=0;i<l;i++) fprintf(pfDoc,"+"); fprintf(pfDoc,"\n\n");
                        } else {
                           fprintf(pfDoc,"CONSTANT \'%s\'\n",psArg->psStd->pcKyw);
                           l=strlen(psArg->psStd->pcKyw)+11;
                           for (i=0;i<l;i++) fprintf(pfDoc,"+"); fprintf(pfDoc,"\n\n");
                        }
                        fprintf(pfDoc,".SYNOPSIS\n\n");
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n");
                        fprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
                        fprintf(pfDoc,"PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                        fprintf(pfDoc,"TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                        fprintf(pfDoc,"SYNTAX: %s\n",psArg->psStd->pcKyw);
                        fprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
                        if (siErr<0) return(siErr);
                        fprintf(pfDoc,".DESCRIPTION\n\n");
                        if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
                           fprintf(pfDoc,"%s\n\n",psArg->psFix->pcMan);
                        } else {
                           fprintf(pfDoc,"No detailed description available for this constant.\n\n");
                        }
                        fprintf(pfDoc,"indexterm:[Constant %s]\n\n\n",psArg->psStd->pcKyw);
                     }
                  } else {
                     return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains to many or invalid qualifiers",pcPat);
                  }
               } else {
                  return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains to many or invalid qualifiers",pcPat);
               }
               return(CLP_OK);
            }
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
         }
      }
      if (isMan) {
         if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
             for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
             fprintf(pfDoc,".");
             l=strlen(psHdl->pcPgm)+strlen(psHdl->pcCmd)+4;
         } else {
             l=strlen(psHdl->pcCmd)+3;
         }
         for (p=psHdl->pcCmd;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
         fprintf(pfDoc,   "(1)\n");
         for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
         fprintf(pfDoc,   ":doctype: manpage\n\n");
         fprintf(pfDoc,   "NAME\n");
         fprintf(pfDoc,   "----\n\n");
         if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
             for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
             fprintf(pfDoc,".");
         }
         for (p=psHdl->pcCmd;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
         fprintf(pfDoc,   " - `%s`\n\n",psHdl->pcHlp);
         fprintf(pfDoc,   "SYNOPSIS\n");
         fprintf(pfDoc,   "--------\n\n");
         fprintf(pfDoc,   "-----------------------------------------------------------------------\n");
         fprintf(pfDoc,   "PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
         if (psHdl->isOvl) {
            fprintf(pfDoc,"TYPE:   OVERLAY\n");
         } else {
            fprintf(pfDoc,"TYPE:   OBJECT\n");
         }
         fprintf(pfDoc,   "SYNTAX: $ %s ",psHdl->pcPgm); siErr=siClpPrnCmd(pvHdl,pfDoc,0,0,1,NULL,psHdl->psSym,FALSE,FALSE); fprintf(pfDoc,"\n");
         fprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
         if (siErr<0) return(siErr);
         fprintf(pfDoc,   "DESCRIPTION\n");
         fprintf(pfDoc,   "-----------\n\n");
         if (psHdl->pcMan!=NULL && strlen(psHdl->pcMan)) {
            fprintf(pfDoc,"%s\n\n",psHdl->pcMan);
         } else {
            fprintf(pfDoc,"No detailed description available for this command.\n\n");
         }
         fprintf(pfDoc,"AUTHOR\n");
         fprintf(pfDoc,"------\n\n");
         fprintf(pfDoc,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
      } else {
         if (isNbr) {
            fprintf(pfDoc,   "%s COMMAND \'%s\'\n",pcNum,psHdl->pcCmd);
            l=strlen(pcNum)+strlen(psHdl->pcCmd)+11;
            for (i=0;i<l;i++) fprintf(pfDoc,"~"); fprintf(pfDoc,"\n\n");
         } else {
            fprintf(pfDoc,   "COMMAND \'%s\'\n",psHdl->pcCmd);
            l=strlen(psHdl->pcCmd)+10;
            for (i=0;i<l;i++) fprintf(pfDoc,"~"); fprintf(pfDoc,"\n\n");
         }
         fprintf(pfDoc,   ".SYNOPSIS\n\n");
         fprintf(pfDoc,   "-----------------------------------------------------------------------\n");
         fprintf(pfDoc,   "HELP:   %s\n",psHdl->pcHlp);
         fprintf(pfDoc,   "PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
         if (psHdl->isOvl) {
            fprintf(pfDoc,"TYPE:   OVERLAY\n");
         } else {
            fprintf(pfDoc,"TYPE:   OBJECT\n");
         }
         fprintf(pfDoc,   "SYNTAX: $ %s ",psHdl->pcPgm); siErr=siClpPrnCmd(pvHdl,pfDoc,0,0,1,NULL,psHdl->psSym,FALSE,FALSE); fprintf(pfDoc,"\n");
         fprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
         if (siErr<0) return(siErr);
         fprintf(pfDoc,   ".DESCRIPTION\n\n");
         if (psHdl->pcMan!=NULL && strlen(psHdl->pcMan)) {
            fprintf(pfDoc,"%s\n\n",psHdl->pcMan);
         } else {
            fprintf(pfDoc,"No detailed description available for this command.\n\n");
         }
         fprintf(pfDoc,   "indexterm:[Command %s]\n\n\n",psHdl->pcCmd);
         if (isDep) {
            siPos=siClpPrnDoc(pvHdl,pfDoc,0,isNbr,pcNum,NULL,psTab);
            if (siPos<0) return(siPos);
         }
      }
   } else {
      return CLPERR(psHdl,CLPERR_INT,"No valid initial number string for head lines (%s)",psHdl->pcCmd);
   }
   return(CLP_OK);
}

extern int siClpProperties(
   void*                         pvHdl,
   const int                     isSet,
   const int                     siDep,
   const char*                   pcPat,
   FILE*                         pfOut)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psSym;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_LEXSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);
   const char*                   pcArg=NULL;

   psHdl->pcSrc=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->acLex[0]=EOS;
   psHdl->acMsg[0]=EOS;
   psHdl->acLst[0]=EOS;
   psHdl->acPat[0]=EOS;
   psHdl->acPre[0]=EOS;
   strcpy(psHdl->acSrc,":PROPERTIES:");

   if (pfOut==NULL) pfOut=psHdl->pfHlp;
   if (pcPat!=NULL && strlen(pcPat)) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
         }
         for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
            for (pcKyw=pcPtr+1,i=0;pcKyw[i]!=EOS && pcKyw[i]!='.' && i<CLPMAX_LEXLEN;i++) acKyw[i]=pcKyw[i];
            acKyw[i]=EOS;
            if (pcArg!=NULL) {
               return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains to many or invalid qualifiers",pcPat);
            }
            siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
            if (siErr<0) return(siErr);
            psHdl->apPat[siLev]=psArg;
            if (psArg->psDep!=NULL) {
               psTab=psArg->psDep;
            } else {
               pcArg=psArg->psStd->pcKyw;
            }
         }
         if (pcArg!=NULL) siLev--;
         siErr=siClpPrnPro(pvHdl,pfOut,FALSE,isSet,siLev,siLev+siDep,psTab,pcArg);
         if (siErr<0) return(siErr);
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
      }
   } else {
      siErr=siClpPrnPro(pvHdl,pfOut,FALSE,isSet,0,siDep,psTab,NULL);
      if (siErr<0) return(siErr);
   }
   return(CLP_OK);
}

extern void vdClpClose(
   void*                         pvHdl)
{
   if (pvHdl!=NULL) {
      TsHdl*                     psHdl=(TsHdl*)pvHdl;
      vdClpSymDel(psHdl->psSym);
      free(pvHdl);
   }
}

/* Interne Funktionen *************************************************/

static char* get_env(const char* fmtstr, ...)
{
   int                  i;
   char                 var[CLPMAX_PATSIZ]={0};
   va_list              argv;
   va_start(argv,fmtstr); vsnprintf(var,CLPMAX_PATLEN,fmtstr,argv); va_end(argv);
   for (i=0;var[i];i++) {
      var[i]=toupper(var[i]);
      if (var[i]=='.') var[i]='_';
   }
   return(getenv(var));
}

#undef  ERROR
#define ERROR(s) if (s!=NULL) {\
      if (s->psStd!=NULL) { free(s->psStd); s->psStd=NULL; }\
      if (s->psFix!=NULL) { free(s->psFix); s->psFix=NULL; }\
      if (s->psVar!=NULL) { free(s->psVar); s->psVar=NULL; }\
      free(s); } return(NULL);

static TsSym* psClpSymIns(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const TsClpArgument*          psArg,
   TsSym*                        psHih,
   TsSym*                        psCur)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psSym=NULL;
   TsSym*                        psHlp=NULL;
   char*                         pcEnv=NULL;
   int                           k;

   psSym=(TsSym*)calloc(1,sizeof(TsSym));
   if (psSym==NULL) {
      CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol \'%s.%s\' failed",fpcPat(pvHdl,siLev),psArg->pcKyw);
      ERROR(psSym);
   }
   psSym->psStd=(TsStd*)calloc(1,sizeof(TsStd));
   psSym->psFix=(TsFix*)calloc(1,sizeof(TsFix));
   psSym->psVar=(TsVar*)calloc(1,sizeof(TsVar));
   if (psSym->psStd==NULL || psSym->psFix==NULL || psSym->psVar==NULL) {
      CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element \'%s.%s\' failed",fpcPat(pvHdl,siLev),psArg->pcKyw);
      ERROR(psSym);
   }

   psSym->psStd->pcKyw=psArg->pcKyw;
   psSym->psStd->pcAli=psArg->pcAli;
   psSym->psStd->uiFlg=psArg->uiFlg;
   psSym->psStd->psAli=NULL;
   psSym->psStd->siKwl=strlen(psSym->psStd->pcKyw);
   psSym->psStd->siLev=siLev;
   psSym->psStd->siPos=siPos;
   psSym->psFix->pcMan=psArg->pcMan;
   psSym->psFix->pcHlp=psArg->pcHlp;
   pcEnv=get_env("%s.%s.%s",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev));
   if (pcEnv==NULL) {
      pcEnv=get_env("%s.%s",psHdl->pcPgm,fpcPat(pvHdl,siLev));
      if (pcEnv==NULL) {
         pcEnv=get_env("%s",fpcPat(pvHdl,siLev));
      }
   }
   if (pcEnv!=NULL && strlen(pcEnv)) {
      strcpy(psSym->psFix->acPro,pcEnv);
      psSym->psFix->pcDft=psSym->psFix->acPro;
   } else {
      psSym->psFix->pcDft=psArg->pcDft;
   }
   psSym->psFix->siTyp=psArg->siTyp;
   psSym->psFix->siMin=psArg->siMin;
   psSym->psFix->siMax=psArg->siMax;
   psSym->psFix->siSiz=psArg->siSiz;
   psSym->psFix->siOfs=psArg->siOfs;
   psSym->psFix->siOid=psArg->siOid;
   psSym->psFix->psLnk=NULL;
   psSym->psFix->psCnt=NULL;
   psSym->psFix->psOid=NULL;
   psSym->psFix->psEln=NULL;
   psSym->psFix->psSln=NULL;
   psSym->psFix->psTln=NULL;

   switch (psSym->psFix->siTyp) {
   case CLPTYP_SWITCH: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_NUMBER: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_FLOATN: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_OBJECT: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_OVRLAY: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_STRING: break;
   case CLPTYP_XALIAS: break;
   default:
      CLPERR(psHdl,CLPERR_TAB,"Type (%d) for argument \'%s.%s\' not supported",psSym->psFix->siTyp,fpcPat(pvHdl,siLev));
      ERROR(psSym);
   }

   if (psSym->psStd->pcAli!=NULL) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword of alias (%s.%s) is not defined",fpcPat(pvHdl,siLev),psSym->psStd->pcAli);
         ERROR(psSym);
      }
      if (strxcmp(psHdl->isCas,psSym->psStd->pcKyw,psSym->psStd->pcAli,0,0,FALSE)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword and alias (%s.%s) are equal",fpcPat(pvHdl,siLev),psSym->psStd->pcAli);
         ERROR(psSym);
      }
      for (k=0,psHlp=psCur;psHlp!=NULL;psHlp=psHlp->psBak) {
         if (CLPISS_ARG(psHlp->psStd->uiFlg) && strxcmp(psHdl->isCas,psSym->psStd->pcAli,psHlp->psStd->pcKyw,0,0,FALSE)==0) {
            if (k==0) {
               psSym->psStd->psAli=psHlp;
               psSym->psStd->uiFlg=psHlp->psStd->uiFlg|CLPFLG_ALI;
               free(psSym->psFix); psSym->psFix=psHlp->psFix;
               free(psSym->psVar); psSym->psVar=psHlp->psVar;
            } else {
               CLPERR(psHdl,CLPERR_TAB,"Alias for keyword \'%s.%s\' is not unique",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
               ERROR(psSym);
            }
            k++;
         }
      }
      if (k==0) {
         CLPERR(psHdl,CLPERR_TAB,"Alias \'%s\' for keyword \'%s.%s\' can not be resolved",psSym->psStd->pcAli,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
   } else if (CLPISS_ARG(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword for argument (%s.?) is not defined",fpcPat(pvHdl,siLev));
         ERROR(psSym);
      }
      if (psSym->psFix->siMax<1 || psSym->psFix->siMax<psSym->psFix->siMin) {
         CLPERR(psHdl,CLPERR_TAB,"Maximal amount for argument \'%s.%s\' is too small",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siSiz<1) {
         CLPERR(psHdl,CLPERR_TAB,"Size for argument \'%s.%s\' is smaller than 1",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcHlp==NULL || strlen(psSym->psFix->pcHlp)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Help for argument \'%s.%s\' not defined",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
   } else if (CLPISS_LNK(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword of a link (%s.?) is not defined",fpcPat(pvHdl,siLev));
         ERROR(psSym);
      }
      if (psSym->psStd->pcAli!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Alias (%s) for link \'%s.%s\' defined",psSym->psStd->pcAli,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcDft!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Default (%s) for link \'%s.%s\' defined",psSym->psFix->pcDft,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siTyp!=CLPTYP_NUMBER) {
         CLPERR(psHdl,CLPERR_TAB,"Type for link \'%s.%s\' is not a number",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siMax<1 || psSym->psFix->siMax<psSym->psFix->siMin) {
         CLPERR(psHdl,CLPERR_TAB,"Maximal amount for link \'%s.%s\' is too small",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siSiz<1) {
         CLPERR(psHdl,CLPERR_TAB,"Size for link \'%s.%s\' is smaller than 1",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (CLPISS_SEL(psSym->psStd->uiFlg) || CLPISS_CON(psSym->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_TAB,"Flag SEL or CON set for link \'%s.%s\'",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
   } else if (CLPISS_CON(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Key word for a constant (%s.?) is not defined",fpcPat(pvHdl,siLev));
         ERROR(psSym);
      }
      if (psSym->psStd->pcAli!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Alias (%s) for constant \'%s.%s\' defined",psSym->psStd->pcAli,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcDft!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Default (%s) for constant \'%s.%s\' defined",psSym->psFix->pcDft,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psArg->psTab!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Table for constant \'%s.%s\' defined",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (CLPISS_SEL(psSym->psStd->uiFlg) || CLPISS_LNK(psSym->psStd->uiFlg)  || CLPISS_ALI(psSym->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_TAB,"Flags SEL, LNK or ALI set for constant \'%s.%s\'",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcHlp==NULL || strlen(psSym->psFix->pcHlp)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Help for constant \'%s.%s\' not defined",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      psSym->psFix->siMin=1;
      psSym->psFix->siMax=1;
      psSym->psFix->siOfs=0;
      psSym->psFix->siOid=0;
      switch (psSym->psFix->siTyp) {
      case CLPTYP_NUMBER:
         psSym->psFix->siSiz=sizeof(psArg->siVal);
         psSym->psVar->pvDat=(void*)&psArg->siVal;
         break;
      case CLPTYP_FLOATN:
         psSym->psFix->siSiz= sizeof(psArg->flVal);
         psSym->psVar->pvDat=(void*)&psArg->flVal;
         break;
      case CLPTYP_STRING:
         if (psArg->pcVal==NULL) {
            CLPERR(psHdl,CLPERR_TAB,"Type \'%s\' for constant \'%s.%s\' requires a value (pcVal==NULL)",apClpTyp[psSym->psFix->siTyp],fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            ERROR(psSym);
         }
         if (!CLPISS_BIN(psSym->psStd->uiFlg) && psSym->psFix->siSiz==0) {
            psSym->psFix->siSiz=strlen((char*)psArg->pcVal)+1;
         }
         psSym->psVar->pvDat=(void*)psArg->pcVal;
         break;
      default:
         CLPERR(psHdl,CLPERR_TAB,"Type (%s) for argument \'%s.%s\' not supported for constant definitions",apClpTyp[psSym->psFix->siTyp],fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      psSym->psVar->pvPtr=NULL;
      psSym->psVar->siLen=psSym->psFix->siSiz;
      psSym->psVar->siCnt=1;
      psSym->psVar->siRst=0;
   } else {
      CLPERR(psHdl,CLPERR_TAB,"Kind (ALI/ARG/LNK/CON) of argument \'%s.%s\' not determinable",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
      ERROR(psSym);
   }

   if (!CLPISS_CMD(psSym->psStd->uiFlg) && !CLPISS_PRO(psSym->psStd->uiFlg) && !CLPISS_DMY(psSym->psStd->uiFlg)) {
      psSym->psStd->uiFlg|=CLPFLG_CMD;
      psSym->psStd->uiFlg|=CLPFLG_PRO;
   }

   if (psCur!=NULL) {
      psSym->psNxt=psCur->psNxt;
      psSym->psBak=psCur;
      if (psCur->psNxt!=NULL) {
         psCur->psNxt->psBak=psSym;
      }
      psCur->psNxt=psSym;
   } else {
      psSym->psNxt=NULL;
      psSym->psBak=NULL;
      if (psHih!=NULL) psHih->psDep=psSym;
   }
   psSym->psDep=NULL;
   psSym->psHih=psHih;

   return(psSym);
}
#undef ERROR

static int siClpSymIni(
   void*                         pvHdl,
   const int                     siLev,
   const TsClpArgument*          psArg,
   const TsClpArgument*          psTab,
   TsSym*                        psHih,
   TsSym**                       ppFst)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr;
   TsSym*                        psCur=NULL;
   TsSym*                        psFst=NULL;
   int                           i,j;

   if (psTab==NULL) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table not defined%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument \'%s.%s\' not defined",fpcPat(pvHdl,siLev),psArg->pcKyw);
      }
   }

   for (j=i=0;psTab[i].siTyp;i++) {
      if (i>=CLPMAX_TABCNT) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Argument table bigger than maximal supported entry count (%d)",CLPMAX_TABCNT);
         } else {
            return CLPERR(psHdl,CLPERR_TAB,"Table for key word (%s.%s) bigger than maximal supported entry count (%d)",fpcPat(pvHdl,siLev),psArg->pcKyw,CLPMAX_TABCNT);
         }
      }

      if (psTab[i].pcKyw==NULL) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"There is no keyword defined in argument table at index %d",i);
         } else {
            return CLPERR(psHdl,CLPERR_TAB,"Table for key word (%s.%s) has no keyword defined at index %d",fpcPat(pvHdl,siLev),psArg->pcKyw,i);
         }
      }

      if (!CLPISS_DMY(psTab[i].uiFlg)) {
         psCur=psClpSymIns(pvHdl,siLev,i,&psTab[i],psHih,psCur);
         if (psCur==NULL) {
            return CLPERR(psHdl,CLPERR_SYS,"Insert of symbol (%s.%s.%s) in symbol table failed",fpcPat(pvHdl,siLev),psArg->pcKyw,psTab[i].pcKyw);
         }
         if (j==0) *ppFst=psCur;

         switch (psTab[i].siTyp) {
         case CLPTYP_SWITCH:
            if (psTab[i].psTab!=NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument \'%s.%s\' is defined (NULL for psTab required)",fpcPat(pvHdl,siLev),psTab[i].pcKyw);
            }
            break;
         case CLPTYP_NUMBER:
         case CLPTYP_FLOATN:
         case CLPTYP_STRING:
            if (CLPISS_SEL(psTab[i].uiFlg)) {
               psHdl->apPat[siLev]=psCur;
               siErr=siClpSymIni(pvHdl,siLev+1,psTab+i,psTab[i].psTab,psCur,&psFst);
               if (siErr<0) return(siErr);
            } else {
               if (psTab[i].psTab!=NULL) {
                  psHdl->apPat[siLev]=psCur;
                  siErr=siClpSymIni(pvHdl,siLev+1,psTab+i,psTab[i].psTab,psCur,&psFst);
                  if (siErr<0) return(siErr);
               }
            }
            break;
         case CLPTYP_OBJECT:
         case CLPTYP_OVRLAY:
            psHdl->apPat[siLev]=psCur;
            siErr=siClpSymIni(pvHdl,siLev+1,psTab+i,psTab[i].psTab,psCur,&psFst);
            if (siErr<0) return(siErr);
            break;
         case CLPTYP_XALIAS: break;
         default:
            return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter \'%s.%s\' not supported",psTab[i].siTyp,fpcPat(pvHdl,siLev),psTab[i].pcKyw);
         }
         j++;
      }
   }
   return(CLP_OK);
}

static int siClpSymCal(
   void*                         pvHdl,
   int                           siLev,
   TsSym*                        psArg,
   TsSym*                        psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psSym=psTab;
   TsSym*                        psHlp=NULL;
   int                           isPar=FALSE;
   int                           isCon=FALSE;
   int                           siCon=0;
   int                           siErr,siPos,k,h;

   for (siPos=0,psSym=psTab;psSym!=NULL;psSym=psSym->psNxt,siPos++) {
      if (psSym->psStd->siLev!=siLev) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_INT,"Argument table not in sync with symbol table%s","");
         } else {
            return CLPERR(psHdl,CLPERR_INT,"Parameter table of argument \'%s.%s\' not in sync with symbol table",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }

      if (CLPISS_ALI(psSym->psStd->uiFlg)) {
         isPar=TRUE;
      } else if (CLPISS_ARG(psSym->psStd->uiFlg)) {
         isPar=TRUE;
      } else if (CLPISS_LNK(psSym->psStd->uiFlg)) {
         isPar=TRUE;
         for (h=k=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if (CLPISS_ARG(psHlp->psStd->uiFlg) && strxcmp(psHdl->isCas,psSym->psStd->pcKyw,psHlp->psStd->pcKyw,0,0,FALSE)==0) {
               psSym->psFix->psLnk=psHlp; h++;
               if (CLPISS_CNT(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psCnt=psSym; k++;
               }
               if (CLPISS_OID(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psOid=psSym; k++;
               }
               if (CLPISS_ELN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psEln=psSym; k++;
               }
               if (CLPISS_SLN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psSln=psSym; k++;
               }
               if (CLPISS_TLN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psTln=psSym; k++;
               }
            }
         }
         if (psSym->psFix->psLnk==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword \'%s.%s\' can not be resolved",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         if (h>1) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword \'%s.%s\' is not unique",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         if (k>1) {
            return CLPERR(psHdl,CLPERR_TAB,"More then one link defined for keyword \'%s.%s\'",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         if (k==0) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword \'%s.%s\' was not assigned",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
      } else if (CLPISS_CON(psSym->psStd->uiFlg)) {
         isCon=TRUE; siCon++;
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Kind (ALI/ARG/LNK/CON) of argument \'%s.%s\' not determinable",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
      }

      psSym->psStd->siKwl=strlen(psSym->psStd->pcKyw);
      if (psHdl->siMkl>0) {
         if (psSym->psStd->siKwl>psHdl->siMkl) psSym->psStd->siKwl=psHdl->siMkl;
         if (!CLPISS_LNK(psSym->psStd->uiFlg)) {
            for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
               if (psHlp!=psSym && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
                  if (psHdl->isCas) {
                     for (k=0;psSym->psStd->pcKyw[k] && psHlp->psStd->pcKyw[k] &&         psSym->psStd->pcKyw[k] ==        psHlp->psStd->pcKyw[k] ;k++);
                  } else {
                     for (k=0;psSym->psStd->pcKyw[k] && psHlp->psStd->pcKyw[k] && toupper(psSym->psStd->pcKyw[k])==toupper(psHlp->psStd->pcKyw[k]);k++);
                  }
                  if (psSym->psStd->pcKyw[k]==0 && psHlp->psStd->pcKyw[k]==0) {
                     return CLPERR(psHdl,CLPERR_TAB,"Key word \'%s.%s\' is not unique",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
                  }
                  if (psSym->psStd->pcKyw[k]==0) {
                     psSym->psStd->siKwl=k;
                     if (psHlp->psStd->siKwl<=k) psHlp->psStd->siKwl=k+1;
                  } else if (psHlp->psStd->pcKyw[k]==0) {
                     psHlp->psStd->siKwl=k;
                     if (psSym->psStd->siKwl<=k) psSym->psStd->siKwl=k+1;
                  } else {
                     if (psSym->psStd->siKwl<=k) psSym->psStd->siKwl=k+1;
                  }
               }
            }
         }
      }
      if (psSym->psDep!=NULL) {
         psHdl->apPat[siLev]=psSym;
         siErr=siClpSymCal(pvHdl,siLev+1,psSym,psSym->psDep);
         if (siErr<0) return(siErr);
      }
   }
   if (isCon && (isPar || siCon!=siPos)) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table is not consistent (mix of constants and parameter)%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument \'%s.%s\' is not consistent (mix of constants and parameter)",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
   }
   if (isCon==FALSE && isPar==FALSE) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table neither contains constants nor arguments%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument \'%s.%s\' neither contains constants nor arguments",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
   }
   for (psSym=psTab;psSym!=NULL;psSym=psSym->psNxt) {
      if (!CLPISS_LNK(psSym->psStd->uiFlg)) {
         if (psSym->psStd->siKwl<=0) {
            return CLPERR(psHdl,CLPERR_TAB,"Required keyword length (%d) of argument \'%s.%s\' is smaller or equal to zero",psSym->psStd->siKwl,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         if (psSym->psStd->siKwl>strlen(psSym->psStd->pcKyw)) {
            return CLPERR(psHdl,CLPERR_TAB,"Required keyword length (%d) of argument \'%s.%s\' is greater then keyword length",psSym->psStd->siKwl,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }
      if (CLPISS_ALI(psSym->psStd->uiFlg)) {
         psSym->psDep=psSym->psStd->psAli->psDep;
         psSym->psHih=psSym->psStd->psAli->psHih;
      }
   }
   return(CLP_OK);
}

static int siClpSymFnd(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const char*                   pcKyw,
   const TsSym*                  psTab,
   TsSym**                       ppArg,
   int*                          piElm)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp=NULL;
   int                           i,j,k,e;
   *ppArg=NULL;
   if (psTab==NULL) {
      CLPERR(psHdl,CLPERR_SYN,"Keyword \'%s.%s\' not valid",fpcPat(pvHdl,siLev),pcKyw);
      CLPERRADD(psHdl,      1,"Unexpected end of path reached%s","");
      return(CLPERR_SYN);
   }
   if (psTab->psBak!=NULL) {
      CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      CLPERRADD(psHdl,      1,"Try to find keyword \'%s\' in this table",pcKyw);
      return(CLPERR_INT);
   }
   for (e=i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (!CLPISS_LNK(psHlp->psStd->uiFlg)) {
         if (psHdl->isCas) {
            for (k=j=0;pcKyw[j]!=EOS;j++) {
               if (pcKyw[j]!=psHlp->psStd->pcKyw[j]) k++;
            }
         } else {
            for (k=j=0;pcKyw[j]!=EOS;j++) {
               if (toupper(pcKyw[j])!=toupper(psHlp->psStd->pcKyw[j])) k++;
            }
         }
         if (k==0 && j>=psHlp->psStd->siKwl) {
            if (piElm!=NULL) (*piElm)=e;
            *ppArg=(TsSym*)psHlp;
            return(i);
         }
         if (piElm!=NULL) {
            if (CLPISS_ARG(psHlp->psStd->uiFlg) || CLPISS_CON(psHlp->psStd->uiFlg)) {
               e++;
            }
         }
      }
   }
   CLPERR(psHdl,CLPERR_SEM,"Parameter \'%s.%s\' not valid",fpcPat(pvHdl,siLev),pcKyw);
   CLPERRADD(psHdl,0,"Please use one of the following parameters:%s","");
   vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,-1,psTab);
   return(CLPERR_SEM);
}

static void vdClpSymPrn(
   void*                         pvHdl,
   int                           siLev,
   TsSym*                        psSym)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psHlp=psSym;
   while (psHlp!=NULL) {
      fprintf(psHdl->pfSym,"%s %3.3d - %s (KWL=%d TYP=%s MIN=%d MAX=%d SIZ=%d OFS=%d OID=%d FLG=%8.8lX (NXT=%p BAK=%p DEP=%p HIH=%p ALI=%p CNT=%p OID=%p ELN=%p SLN=%p TLN=%p LNK=%p)) - %s\n",
            fpcPre(pvHdl,siLev),psHlp->psStd->siPos+1,psHlp->psStd->pcKyw,psHlp->psStd->siKwl,apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->siMin,psHlp->psFix->siMax,psHlp->psFix->siSiz,
            psHlp->psFix->siOfs,psHlp->psFix->siOid,psHlp->psStd->uiFlg,psHlp->psNxt,psHlp->psBak,psHlp->psDep,psHlp->psHih,psHlp->psStd->psAli,psHlp->psFix->psCnt,psHlp->psFix->psOid,
            psHlp->psFix->psEln,psHlp->psFix->psSln,psHlp->psFix->psTln,psHlp->psFix->psLnk,psHlp->psFix->pcHlp);
      if (psHlp->psDep!=NULL) {
         vdClpSymPrn(pvHdl,siLev+1,psHlp->psDep);
      }
      psHlp=psHlp->psNxt;
   }
}

static void vdClpSymTrc(
   void*                         pvHdl)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (psHdl->pfSym!=NULL) {
      fprintf(psHdl->pfSym,"BEGIN-SYMBOL-TABLE-TRACE\n");
      vdClpSymPrn(pvHdl,0,psHdl->psSym);
      fprintf(psHdl->pfSym,"END-SYMBOL-TABLE-TRACE\n");
      fflush(psHdl->pfSym);
   }
}

static void vdClpSymDel(
   TsSym*                        psSym)
{
   TsSym*                        psHlp=psSym;
   TsSym*                        psOld;
   while (psHlp!=NULL) {
      if (!CLPISS_ALI(psHlp->psStd->uiFlg) && psHlp->psDep!=NULL) {
         vdClpSymDel(psHlp->psDep);
      }
      if (!CLPISS_ALI(psHlp->psStd->uiFlg) && psHlp->psVar!=NULL) {
         memset(psHlp->psVar,0,sizeof(TsVar));
         free(psHlp->psVar);
         psHlp->psVar=NULL;
      }
      if (!CLPISS_ALI(psHlp->psStd->uiFlg) && psHlp->psFix!=NULL) {
         memset(psHlp->psFix,0,sizeof(TsFix));
         free(psHlp->psFix);
         psHlp->psFix=NULL;
      }
      if (psHlp->psStd!=NULL) {
         memset(psHlp->psStd,0,sizeof(TsStd));
         free(psHlp->psStd);
         psHlp->psStd=NULL;
      }
      psOld=psHlp;
      psHlp=psHlp->psNxt;
      memset(psOld,0,sizeof(TsSym));
      free(psOld);
   }
}

/* Scanner ************************************************************/

/*now difference between host and open world anymore */
#ifdef __HOST__
   #define STRCHR '\''
   #define SPMCHR '\"'
#else
   #define STRCHR '\''
   #define SPMCHR '\"'
#endif

extern int siClpLexem(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   fprintf(pfOut,"%s COMMENT   '#' [:print:]* '#'                              (will be ignored)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s LCOMMENT  ';' [:print:]* 'nl'                             (will be ignored)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s OPERATOR  '=' | '.' | '(' | ')' | '[' | ']'  (SGN, DOT, RBO, RBC, SBO, SBC)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s KEYWORD   ['-'['-']][:alpha:]+[:alnum: | '_' | '-']*    (always predefined)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s NUMBER    ([+|-]  [ :digit:]+)  |                       (decimal (default))\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s num       ([+|-]0b[ :digit:]+)  |                                  (binary)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s num       ([+|-]0o[ :digit:]+)  |                                   (octal)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s num       ([+|-]0d[ :digit:]+)  |                                 (decimal)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s num       ([+|-]0x[ :xdigit:]+) |                             (hexadecimal)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s num       ([+|-]0t(yyyy/mm/tt.hh:mm:ss)) |  (relativ (+|-) or absolut time)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s FLOAT     ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) | (decimal(default))\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s flt       ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)            (decimal)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s STRING         ''' [:print:]* ''' |          (default (if binary c else s))\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s str       [s|S]''' [:print:]* ''' |                (zero terminated string)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s str       [c|C]''' [:print:]* ''' |  (binary string in local character set)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s str       [a|A]''' [:print:]* ''' |                (binary string in ASCII)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s str       [e|E]''' [:print:]* ''' |               (binary string in EBCDIC)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s str       [x|X]''' [:print:]* ''' |         (binary string in hex notation)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s           Strings can contain two '' to represent one '                    \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s           Strings can also be enclosed in \" instead of '                   \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s           Strings can directly start behind a '=' without enclosing '/\"    \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s              In this case the string ends at the next separator or operator\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s              and keywords are preferred. To use keywords, separators or    \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s              operators in strings, enclosing quotes are required.          \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s SUPPLEMENT     '\"' [:print:]* '\"' |   (zero terminated string (properties))\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s           Supplements can contain two \"\" to represent one \"                \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s           Supplements can also be enclosed in ' instead of \"               \n",fpcPre(pvHdl,0));
   return(CLP_OK);
}

#define isPrintF(p)   (((p)!=NULL)?(CLPISS_PWD((p)->psStd->uiFlg)==FALSE):(TRUE))
#define isPrnLex(p,l) (isPrintF(p)?(l):("###SECRET###"))
static int siClpScnNat(
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char**                  ppCur,
   char*                         pcLex,
   const int                     uiTok,
   const TsSym*                  psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char*                         pcEnd=pcLex+CLPMAX_LEXLEN;
   char*                         pcHlp=pcLex;
   U64                           t;
   struct tm                     tm;
   struct tm                     *tmAkt;

   while (1) {
      if (*(*ppCur)==EOS) { /*end*/
         pcLex[0]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(END)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_END);
      } else if (isspace(*(*ppCur)) || iscntrl(*(*ppCur)) || (*(*ppCur))==',') { /*separation*/
         if (*(*ppCur)=='\n') {
            psHdl->siRow++;
            psHdl->pcRow=(*ppCur)+1;
         }
         (*ppCur)++;
         psHdl->pcOld=(*ppCur);
      } else if (*(*ppCur)=='#') { /*comment*/
         (*ppCur)++;
         while (*(*ppCur)!='#' && *(*ppCur)!=EOS) {
            if (*(*ppCur)=='\n') {
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur)+1;
            }
            (*ppCur)++;
         }
         if (*(*ppCur)!='#') {
            return CLPERR(psHdl,CLPERR_LEX,"Comment not terminated with \'#\'%s","");
         }
         (*ppCur)++;
         psHdl->pcOld=(*ppCur);
      } else if (*(*ppCur)==';') { /*line comment*/
         (*ppCur)++;
         while (*(*ppCur)!='\n' && *(*ppCur)!=EOS) {
            if (*(*ppCur)=='\n') {
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur)+1;
            }
            (*ppCur)++;
         }
         if (*(*ppCur)=='\n') {
            (*ppCur)++;
            psHdl->siRow++;
            psHdl->pcRow=(*ppCur);
         }
         psHdl->pcOld=(*ppCur);
      } else if ((*ppCur)[0]==SPMCHR) {/*supplement || simple string*/
         char* pcSup;
         *pcLex='d'; pcLex++;
         *pcLex='\''; pcLex++;
         pcSup=pcLex;
         (*ppCur)++;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=SPMCHR || ((*ppCur)[0]==SPMCHR && (*ppCur)[1]==SPMCHR))  && pcLex<pcEnd) {
            *pcLex=*(*ppCur); pcLex++;
            if (*(*ppCur)=='\n') {
               (*ppCur)++;
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur);
            } else if ((*ppCur)[0]==SPMCHR) {
               (*ppCur)+=2;
            } else {
               (*ppCur)++;
            }
         }
         *pcLex=EOS;
         if (*(*ppCur)!=SPMCHR) {
            return CLPERR(psHdl,CLPERR_LEX,"Supplement string / string literal not terminated with \'%c\'",SPMCHR);
         }
         (*ppCur)++;
         if (uiTok==CLPTOK_SUP) {
            char* p1=pcHlp;
            char* p2=pcSup;
            while (*p2) {
               *p1++=*p2++;
            }
            *p1=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SUP)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_SUP);
         } else {
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_STR);
         }
      } else if ((*ppCur)[0]==STRCHR) {/*simple string || supplement*/
         char* pcSup;
         *pcLex='d'; pcLex++;
         *pcLex='\''; pcLex++;
         pcSup=pcLex;
         (*ppCur)++;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=STRCHR || ((*ppCur)[0]==STRCHR && (*ppCur)[1]==STRCHR)) &&  pcLex<pcEnd) {
            *pcLex=*(*ppCur); pcLex++;
            if (*(*ppCur)=='\n') {
               (*ppCur)++;
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur);
            } else if ((*ppCur)[0]==STRCHR) {
               (*ppCur)+=2;
            } else {
               (*ppCur)++;
            }
         }
         *pcLex=EOS;
         if (*(*ppCur)!=STRCHR) {
            return CLPERR(psHdl,CLPERR_LEX,"String literal / supplement string not terminated with \'%c\'",STRCHR);
         }
         (*ppCur)++;
         if (uiTok==CLPTOK_SUP) {
            char* p1=pcHlp;
            char* p2=pcSup;
            while (*p2) {
               *p1++=*p2++;
            }
            *p1=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SUP)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_SUP);
         } else {
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_STR);
         }
      } else if ((tolower((*ppCur)[0])=='x' || tolower((*ppCur)[0])=='a' ||tolower((*ppCur)[0])=='e' ||
                  tolower((*ppCur)[0])=='c' || tolower((*ppCur)[0])=='s') && (*ppCur)[1]==STRCHR) {/*defined string '...'*/
         *pcLex=tolower(*(*ppCur)); pcLex++;
         *pcLex='\''; pcLex++;
         (*ppCur)+=2;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=STRCHR || ((*ppCur)[0]==STRCHR && (*ppCur)[1]==STRCHR)) &&  pcLex<pcEnd) {
            *pcLex=*(*ppCur); pcLex++;
            if (*(*ppCur)=='\n') {
               (*ppCur)++;
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur);
            } else if ((*ppCur)[0]==STRCHR) {
               (*ppCur)+=2;
            } else {
               (*ppCur)++;
            }
         }
         *pcLex=EOS;
         if (*(*ppCur)!=STRCHR) {
            return CLPERR(psHdl,CLPERR_LEX,"String literal not terminated with \'%c\'",STRCHR);
         }
         (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if ((tolower((*ppCur)[0])=='x' || tolower((*ppCur)[0])=='a' ||tolower((*ppCur)[0])=='e' ||
                  tolower((*ppCur)[0])=='c' || tolower((*ppCur)[0])=='s') && (*ppCur)[1]==SPMCHR) {/*defined string "..."*/
         *pcLex=tolower(*(*ppCur)); pcLex++;
         *pcLex='\''; pcLex++;
         (*ppCur)+=2;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=SPMCHR || ((*ppCur)[0]==SPMCHR && (*ppCur)[1]==SPMCHR)) &&  pcLex<pcEnd) {
            *pcLex=*(*ppCur); pcLex++;
            if (*(*ppCur)=='\n') {
               (*ppCur)++;
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur);
            } else if ((*ppCur)[0]==SPMCHR) {
               (*ppCur)+=2;
            } else {
               (*ppCur)++;
            }
         }
         *pcLex=EOS;
         if (*(*ppCur)!=SPMCHR) {
            return CLPERR(psHdl,CLPERR_LEX,"String literal not terminated with \'%c\'",SPMCHR);
         }
         (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if (((*ppCur)[0]=='-' && isalpha((*ppCur)[1])) || ((*ppCur)[0]=='-' && (*ppCur)[1]=='-' && isalpha((*ppCur)[2]))) { /*defined keyword*/
         while ((*ppCur)[0]=='-') {
            (*ppCur)++;
         }
         *pcLex=*(*ppCur);
         (*ppCur)++; pcLex++;
         while (isKyw(*(*ppCur)) && pcLex<pcEnd) {
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
         }
         *pcLex=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(KYW)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_KYW);
      } else if (uiTok==CLPTOK_STR && isprint((*ppCur)[0]) && (*ppCur)[0]!='(' && (*ppCur)[0]!=')' && (*ppCur)[0]!='[' && (*ppCur)[0]!=']') {/*required string*/
         char*             pcKyw;
         *pcLex='d'; pcLex++;
         *pcLex='\''; pcLex++;
         pcKyw=pcLex;
         if (psArg!=NULL && psArg->psDep!=NULL) {
            int               k,j,f;
            TsSym*            psHlp;
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            while (isKyw(*(*ppCur)) && pcLex<pcEnd) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;

            for (f=0,psHlp=psArg->psDep;psHlp!=NULL && f==0;psHlp=psHlp->psNxt) {
               if (psHlp->psFix->siTyp==psArg->psFix->siTyp && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
                  if (psHdl->isCas) {
                     for (k=j=0;pcKyw[j]!=EOS;j++) {
                        if (pcKyw[j]!=psHlp->psStd->pcKyw[j]) k++;
                     }
                  } else {
                     for (k=j=0;pcKyw[j]!=EOS;j++) {
                        if (toupper(pcKyw[j])!=toupper(psHlp->psStd->pcKyw[j])) k++;
                     }
                  }
                  if (k==0 && j>=psHlp->psStd->siKwl) {
                     f=1;
                  }
               }
            }

            if (f) {
               char* p1=pcHlp;
               char* p2=pcKyw;
               while (*p2) {
                  *p1++=*p2++;
               }
               *p1=EOS;
               if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(KYW)-LEXEM(%s)\n",pcHlp);
               return(CLPTOK_KYW);
            }
         }
         while ((*ppCur)[0]!=EOS && isprint((*ppCur)[0]) && (*ppCur)[0]!=' ' && (*ppCur)[0]!=',' && (*ppCur)[0]!='(' && (*ppCur)[0]!=')' && (*ppCur)[0]!='[' && (*ppCur)[0]!=']' &&  pcLex<pcEnd) {
            *pcLex=*(*ppCur);
            pcLex++; (*ppCur)++;
         }
         *pcLex=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if (isalpha((*ppCur)[0])) { /*simple keyword*/
         *pcLex=*(*ppCur);
         (*ppCur)++; pcLex++;
         while ((isKyw(*(*ppCur))) && pcLex<pcEnd) {
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
         }
         *pcLex=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(KYW)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_KYW);
      } else if ((((*ppCur)[0]=='+' || (*ppCur)[0]=='-') && isdigit((*ppCur)[1])) || isdigit((*ppCur)[0])) { /*number*/
         if ((*ppCur)[0]=='+' || (*ppCur)[0]=='-') {
            pcLex[1]=(*ppCur)[0];
            (*ppCur)++;
         } else pcLex[1]=' ';
         if (((*ppCur)[0]=='0') &&
             (tolower((*ppCur)[1])=='b' || tolower((*ppCur)[1])=='o' || tolower((*ppCur)[1])=='d' || tolower((*ppCur)[1])=='x' || tolower((*ppCur)[1])=='t') &&
             (isdigit((*ppCur)[2]) || (isxdigit((*ppCur)[2]) && tolower((*ppCur)[1])=='x'))) {
            pcLex[0]=tolower((*ppCur)[1]);
            (*ppCur)+=2;
         } else pcLex[0]='d';
         pcLex+=2;
         if (pcHlp[0]=='x') {
            while (isxdigit(*(*ppCur)) && pcLex<pcEnd) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
         } else if (pcHlp[0]=='t') {
            memset(&tm,0,sizeof(tm));
            while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            if ((*ppCur)[0]=='/' && isdigit((*ppCur)[1])) {
               tm.tm_year=strtol(pcHlp+2,NULL,10);
               pcLex=pcHlp+2;
               (*ppCur)++;
               while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               *pcLex=EOS;
               if ((*ppCur)[0]=='/' && isdigit((*ppCur)[1])) {
                  tm.tm_mon=strtol(pcHlp+2,NULL,10);
                  pcLex=pcHlp+2;
                  (*ppCur)++;
                  while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
                     *pcLex=*(*ppCur);
                     (*ppCur)++; pcLex++;
                  }
                  *pcLex=EOS;
                  if ((*ppCur)[0]=='.' && isdigit((*ppCur)[1])) {
                     tm.tm_mday=strtol(pcHlp+2,NULL,10);
                     pcLex=pcHlp+2;
                     (*ppCur)++;
                     while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
                        *pcLex=*(*ppCur);
                        (*ppCur)++; pcLex++;
                     }
                     *pcLex=EOS;
                     if ((*ppCur)[0]==':' && isdigit((*ppCur)[1])) {
                        tm.tm_hour=strtol(pcHlp+2,NULL,10);
                        pcLex=pcHlp+2;
                        (*ppCur)++;
                        while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
                           *pcLex=*(*ppCur);
                           (*ppCur)++; pcLex++;
                        }
                        *pcLex=EOS;
                        if ((*ppCur)[0]==':' && isdigit((*ppCur)[1])) {
                           tm.tm_min=strtol(pcHlp+2,NULL,10);
                           pcLex=pcHlp+2;
                           (*ppCur)++;
                           while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
                              *pcLex=*(*ppCur);
                              (*ppCur)++; pcLex++;
                           }
                           *pcLex=EOS;
                           tm.tm_sec=strtol(pcHlp+2,NULL,10);
                           if (pcHlp[1]=='+') {
                              t=time(NULL);
                              if (t==-1) {
                                 return CLPERR(psHdl,CLPERR_SYS,"Determine the current time is not possible%s","");
                              }
                              /*Fix the hour*/
                              tm.tm_hour++;
                              tmAkt=gmtime((time_t*)&t);
                              tmAkt->tm_year +=tm.tm_year;
                              tmAkt->tm_mon  +=tm.tm_mon;
                              tmAkt->tm_mday +=tm.tm_mday;
                              tmAkt->tm_hour +=tm.tm_hour;
                              tmAkt->tm_min  +=tm.tm_min;
                              tmAkt->tm_sec  +=tm.tm_sec;
                              t=mktime(tmAkt);
                              if (t==-1) {
                                 return CLPERR(psHdl,CLPERR_LEX,"The calculated time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) can not be converted to a number",
                                                                tmAkt->tm_year+1900,tmAkt->tm_mon+1,tmAkt->tm_mday,tmAkt->tm_hour,tmAkt->tm_min,tmAkt->tm_sec);
                              }
                           } else if (pcHlp[1]=='-') {
                              t=time(NULL);
                              if (t==-1) {
                                 return CLPERR(psHdl,CLPERR_SYS,"Determine the current time is not possible%s","");
                              }
                              /*Fix the hour*/
                              tm.tm_hour--;
                              tmAkt=gmtime((time_t*)&t);
                              tmAkt->tm_year -=tm.tm_year;
                              tmAkt->tm_mon  -=tm.tm_mon;
                              tmAkt->tm_mday -=tm.tm_mday;
                              tmAkt->tm_hour -=tm.tm_hour;
                              tmAkt->tm_min  -=tm.tm_min;
                              tmAkt->tm_sec  -=tm.tm_sec;

                              t=mktime(tmAkt);
                              if (t==-1) {
                                 return CLPERR(psHdl,CLPERR_LEX,"The calculated time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) can not be converted to a number",
                                                                tmAkt->tm_year+1900,tmAkt->tm_mon+1,tmAkt->tm_mday,tmAkt->tm_hour,tmAkt->tm_min,tmAkt->tm_sec);
                              }
                           } else {
                              tm.tm_year-=1900;
                              tm.tm_mon-=1;
                              t=mktime(&tm);
                              if (t==-1) {
                                 return CLPERR(psHdl,CLPERR_LEX,"The given time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) can not be converted to a number",
                                                                tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
                              }
                           }
                           pcHlp[1]=' ';
                           sprintf(pcHlp+2,"%"PRIu64"",t);
                           if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(NUM)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
                           return(CLPTOK_NUM);
                        } else return CLPERR(psHdl,CLPERR_LEX,"Character \':\' expected to separate minute from second%s","");
                     } else return CLPERR(psHdl,CLPERR_LEX,"Character \':\' expected to separate hour from minute%s","");
                  } else return CLPERR(psHdl,CLPERR_LEX,"Character \'.\' expected to separate day from hour%s","");
               } else return CLPERR(psHdl,CLPERR_LEX,"Character \'/\' expected to separate month from day%s","");
            } else return CLPERR(psHdl,CLPERR_LEX,"Character \'/\' expected to separate year from month%s","");
         } else {
            while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
         }
         if (pcHlp[0]=='d' && (*ppCur)[0]=='.' && (isdigit((*ppCur)[1]))) { /*float*/
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            if ((tolower((*ppCur)[0])=='e') && (isdigit((*ppCur)[1]) ||
                ((*ppCur)[1]=='+' && isdigit((*ppCur)[2])) ||
                ((*ppCur)[1]=='-' && isdigit((*ppCur)[2])))) { /*float*/
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
               if (!isdigit((*ppCur)[0])) {
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               *pcLex=EOS;
               if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
               return(CLPTOK_FLT);
            }
            *pcLex=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_FLT);
         } else if ((tolower((*ppCur)[0])=='e') && (isdigit((*ppCur)[1]) ||
                    ((*ppCur)[1]=='+' && isdigit((*ppCur)[2])) ||
                    ((*ppCur)[1]=='-' && isdigit((*ppCur)[2])))) { /*float*/
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            if (!isdigit((*ppCur)[0])) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            while (isdigit(*(*ppCur)) && pcLex<pcEnd) {
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_FLT);
         } else {
            *pcLex=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(NUM)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_NUM);
         }
      } else if (*(*ppCur)=='=') { /*sign*/
         pcLex[0]='='; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SGN)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SGN);
      } else if (*(*ppCur)=='.') { /*dot*/
         pcLex[0]='.'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(DOT)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_DOT);
      } else if (*(*ppCur)=='(') { /*round bracket open*/
         pcLex[0]='('; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(RBO)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_RBO);
      } else if (*(*ppCur)==')') { /*round bracket close*/
         pcLex[0]=')'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(RBC)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_RBC);
      } else if (*(*ppCur)=='[') { /*squared bracket open*/
         pcLex[0]='['; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SBO)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SBO);
      } else if (*(*ppCur)==']') { /*squared bracket close*/
         pcLex[0]=']'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SBC)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SBC);
      } else { /*lexical error*/
         pcLex[0]=EOS; (*ppCur)++;
         return CLPERR(psHdl,CLPERR_LEX,"Character (\'%c\') not valid%s",*((*ppCur)-1));
      }
   }
}

static int siClpScnSrc(
   void*                         pvHdl,
   const int                     uiTok,
   const TsSym*                  psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   psHdl->pcOld=psHdl->pcCur;
   return(siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&psHdl->pcCur,psHdl->acLex,uiTok,psArg));
}

/**********************************************************************/

extern int siClpGrammar(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   fprintf(pfOut,"%s Command Line Parser                                              \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s command        -> ['('] parameter_list [')']       (main=object) \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  ['.'] parameter                  (main=overlay)\n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s parameter_list -> parameter SEP parameter_list                   \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s parameter      -> switch | assignment | object | overlay | array \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s switch         -> KEYWORD                                        \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s assignment     -> KEYWORD '=' value                              \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s object         -> KEYWORD ['('] parameter_list [')']             \n",fpcPre(pvHdl,1));
if (psHdl->isPfl) {
   fprintf(pfOut,"%s                |  KEYWORD '=' STRING # parameter file #          \n",fpcPre(pvHdl,1));
}
   fprintf(pfOut,"%s overlay        -> KEYWORD ['.'] parameter                        \n",fpcPre(pvHdl,1));
if (psHdl->isPfl) {
   fprintf(pfOut,"%s                |  KEYWORD '=' STRING # parameter file #          \n",fpcPre(pvHdl,1));
}
   fprintf(pfOut,"%s array          -> KEYWORD '[' value_list   ']'                   \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  KEYWORD '[' object_list  ']'                   \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  KEYWORD '[' overlay_list ']'                   \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s value_list     -> value SEP value_list                           \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s object_list    -> object SEP object_list                         \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s overlay_list   -> overlay SEP overlay_list                       \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s value          -> NUMBER | FLOAT | STRING | KEYWORD              \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s A list of objects requires parenthesis to enclose the arguments  \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                                                                  \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s Property File Parser                                             \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s properties     -> property_list                                  \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s property_list  -> property SEP property_list                     \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s property       -> keyword_list '=' SUPPLEMENT                    \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s keyword_list   -> KEYWORD '.' keyword_list                       \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  KEYWORD                                        \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s SUPPLEMENT is a string in double quotation marks (\"property\")    \n",fpcPre(pvHdl,1));
   return(CLP_OK);
}

static int siClpPrsParLst(
   void*                         pvHdl,
   const int                     siLev,
   const TsSym*                  psArg,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siPos=0;
   while (psHdl->siTok==CLPTOK_KYW) {
      siErr=siClpPrsPar(pvHdl,siLev,siPos,psTab,NULL);
      if (siErr<0) return(siErr);
      siPos++;
   }
   return(siPos);
}

static int siClpPrsPar(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const TsSym*                  psTab,
   int*                          piOid)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char                          acKyw[CLPMAX_LEXSIZ];
   TsSym*                        psArg=NULL;
   int                           siInd;

   if (psHdl->siTok==CLPTOK_KYW) {
      strcpy(acKyw,psHdl->acLex);
      siInd=siClpSymFnd(pvHdl,siLev,siPos,acKyw,psTab,&psArg,NULL);
      if (siInd<0) return(siInd);
      if (piOid!=NULL) *piOid=psArg->psFix->siOid;
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_SGN) {
         if (psHdl->isPfl && (psArg->psFix->siTyp==CLPTYP_OBJECT || psArg->psFix->siTyp==CLPTYP_OVRLAY)) {
            return(siClpPrsFil(pvHdl,siLev,siPos,psArg));
         } else {
            return(siClpPrsSgn(pvHdl,siLev,siPos,psArg));
         }
      } else if (psHdl->siTok==CLPTOK_RBO) {
         return(siClpPrsObj(pvHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
         if (psHdl->siTok<0) return(psHdl->siTok);
         return(siClpPrsOvl(pvHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_SBO) {
         return(siClpPrsAry(pvHdl,siLev,siPos,psArg));
      } else {
         if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
            return(siClpPrsObjWob(pvHdl,siLev,siPos,psArg));
         } else if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
            return(siClpPrsOvl(pvHdl,siLev,siPos,psArg));
         } else {
            return(siClpPrsSwt(pvHdl,siLev,siPos,psArg));
         }
      }
   } else {
      CLPERR(psHdl,CLPERR_SYN,"Keyword expected (%s.?)",fpcPat(pvHdl,siLev));
      CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
      vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,-1,psTab);
      return(CLPERR_SYN);
   }
}

static int siClpPrsSwt(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SWT(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   return(siClpBldSwt(pvHdl,siLev,siPos,psArg));
}

static int siClpPrsSgn(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SGN(%s=val)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=(psArg->psFix->siTyp==CLPTYP_STRING)?siClpScnSrc(pvHdl,CLPTOK_STR,psArg):siClpScnSrc(pvHdl,0,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   switch (psHdl->siTok) {
   case CLPTOK_NUM: return(siClpPrsVal(pvHdl,siLev,siPos,0,CLPTYP_NUMBER,psArg));
   case CLPTOK_FLT: return(siClpPrsVal(pvHdl,siLev,siPos,0,CLPTYP_FLOATN,psArg));
   case CLPTOK_STR: return(siClpPrsVal(pvHdl,siLev,siPos,0,CLPTYP_STRING,psArg));
   case CLPTOK_KYW: return(siClpPrsVal(pvHdl,siLev,siPos,0,psArg->psFix->siTyp,psArg));
   default:         return(CLPERR(psHdl,CLPERR_SYN,"After assignment \'%s.%s=\' number(-123), float(+123.45e78), string(\'abc\') or keyword (MODE) expected",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw));
   }
}

static int siClpPrsFil(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char                          acSrc[CLPMAX_LEXSIZ];
   char                          acFil[CLPMAX_LEXSIZ];
   char*                         pcPar=NULL;
   int                           siRow,siCnt,siErr,siSiz=0;
   const char*                   pcCur;
   const char*                   pcSrc;
   const char*                   pcOld;
   const char*                   pcRow;

   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d PARFIL(%s=val)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(pvHdl,CLPTOK_STR,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   if (psHdl->siTok!=CLPTOK_STR) {
      return CLPERR(psHdl,CLPERR_SYN,"After object/overlay assignment \'%s.%s=\' parameter file (\'filename\') expected",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   strcpy(acFil,psHdl->acLex+2);
   siErr=file2str(acFil,&pcPar,&siSiz);
   if (siErr<0) {
      switch(siErr) {
      case -1: siErr=CLPERR(psHdl,CLPERR_INT,"Illegal parameters passed to file2str() (Bug)%s","");break;
      case -2: siErr=CLPERR(psHdl,CLPERR_SYS,"Open of parameter file (%s) failed (%d - %s)",acFil,errno,strerror(errno));break;
      case -3: siErr=CLPERR(psHdl,CLPERR_SEM,"Parameter file (%s) is too big (integer overflow)",acFil);break;
      case -4: siErr=CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for parameter file (%s) failed",acFil);break;
      case -5: siErr=CLPERR(psHdl,CLPERR_SYS,"Read of parameter file (%s) failed (%d - %s)",acFil,errno,strerror(errno));break;
      default: siErr=CLPERR(psHdl,CLPERR_SYS,"An unknown error occurred while reading parameter file (%s)",acFil);break;
      }
      if (pcPar!=NULL) free(pcPar);
      return(siErr);
   }

   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PARAMETER-FILE-PARSER-BEGIN(FILE=%s)\n",acFil);
   strcpy(acSrc,psHdl->acSrc); strcpy(psHdl->acSrc,acFil);
   pcCur=psHdl->pcCur; psHdl->pcCur=pcPar;
   pcSrc=psHdl->pcSrc; psHdl->pcSrc=pcPar;
   pcOld=psHdl->pcOld; psHdl->pcOld=pcPar;
   pcRow=psHdl->pcRow; psHdl->pcRow=pcPar;
   siRow=psHdl->siRow; psHdl->siRow=1;
   psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
   if (psHdl->siTok<0) {
      if (pcPar!=NULL) free(pcPar);
      return(psHdl->siTok);
   }
   if (psHdl->siTok==CLPTOK_RBO) {
      siCnt=siClpPrsObj(pvHdl,siLev,siPos,psArg);
      if (siCnt<0) {
         if (pcPar!=NULL) free(pcPar);
         return(siCnt);
      }
   } else if (psHdl->siTok==CLPTOK_DOT) {
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) {
         if (pcPar!=NULL) free(pcPar);
         return(psHdl->siTok);
      }
      siCnt=siClpPrsOvl(pvHdl,siLev,siPos,psArg);
      if (siCnt<0) {
         if (pcPar!=NULL) free(pcPar);
         return(siCnt);
      }
   } else {
      if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
         siCnt=siClpPrsObjWob(pvHdl,siLev,siPos,psArg);
         if (siCnt<0) {
            if (pcPar!=NULL) free(pcPar);
            return(siCnt);
         }
      } else {
         siCnt=siClpPrsOvl(pvHdl,siLev,siPos,psArg);
         if (siCnt<0) {
            if (pcPar!=NULL) free(pcPar);
            return(siCnt);
         }
      }
   }
   if (pcPar!=NULL) free(pcPar);
   if (psHdl->siTok==CLPTOK_END) {
      psHdl->acLex[0]=EOS;
      strcpy(psHdl->acSrc,acSrc);
      psHdl->pcCur=pcCur; psHdl->pcSrc=pcSrc; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PARAMETER-FILE-PARSER-END(FILE=%s CNT=%d)\n",acFil,siCnt);
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      return(siCnt);
   } else {
      return CLPERR(psHdl,CLPERR_SYN,"Last token (%s) of parameter file \'%s\' is not EOF",apClpTok[psHdl->siTok],acFil);
   }
}

static int siClpPrsObjWob(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siCnt;
   TsSym*                        psDep=NULL;
   TsVar                         asSav[CLPMAX_TABCNT];

   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d OBJ(%s(parlst))-OPN)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   siErr=siClpIniObj(pvHdl,siLev,siPos,psArg,&psDep,asSav);
   if (siErr<0) return(siErr);
   siCnt=siClpPrsParLst(pvHdl,siLev+1,psArg,psDep);
   if (siCnt<0) return(siCnt);
   siErr=siClpFinObj(pvHdl,siLev,siPos,psArg,psDep,asSav);
   if (siErr<0) return(siErr);
   return(siCnt);
}

static int siClpPrsObj(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siCnt;

   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d OBJ(%s(parlst))-OPN)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   siCnt=siClpPrsObjWob(pvHdl,siLev,siPos,psArg);
   if (siCnt<0) return(siCnt);
   if (psHdl->siTok==CLPTOK_RBC) {
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d OBJ(%s(parlst))-CLS)\n",fpcPre(pvHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw);
   } else {
      return CLPERR(psHdl,CLPERR_SYN,"Character \')\' missing (%s)",fpcPat(pvHdl,siLev));
   }
   return(CLP_OK);
}

static int siClpPrsOvl(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siCnt,siOid=0;
   TsSym*                        psDep=NULL;
   TsVar                         asSav[CLPMAX_TABCNT];
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d OVL(%s.par)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   siErr=siClpIniOvl(pvHdl,siLev,siPos,psArg,&psDep,asSav);
   if (siErr<0) return(siErr);
   siCnt=siClpPrsPar(pvHdl,siLev+1,siPos,psDep,&siOid);
   if (siCnt<0) return(siCnt);
   siErr=siClpFinOvl(pvHdl,siLev,siPos,psArg,psDep,asSav,siOid);
   if (siErr<0) return(siErr);
   return(CLP_OK);
}

static int siClpPrsMain(
   void*                         pvHdl,
   TsSym*                        psTab,
   int*                          piOid)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siCnt,siOid;
   TsVar                         asSav[CLPMAX_TABCNT];
   if (psHdl->isOvl) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(OVL(MAIN.par)\n",fpcPre(pvHdl,0));
      if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
         if (psHdl->siTok<0) return(psHdl->siTok);
      }
      siErr=siClpIniMainOvl(pvHdl,psTab,asSav);
      if (siErr<0) return(siErr);
      siErr=siClpPrsPar(pvHdl,0,0,psTab,&siOid);
      if (siErr<0) return(siErr);
      siErr=siClpFinMainOvl(pvHdl,psTab,asSav,siOid);
      if (siErr<0) return(siErr);
      if (piOid!=NULL) (*piOid)=siOid;
      return(1);
   } else {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(OBJ(MAIN(parlst)-OPN)\n",fpcPre(pvHdl,0));
      siErr=siClpIniMainObj(pvHdl,psTab,asSav);
      if (siErr<0) return(siErr);
      if (psHdl->siTok==CLPTOK_RBO) {
         psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
         if (psHdl->siTok<0) return(psHdl->siTok);
         siCnt=siClpPrsParLst(pvHdl,0,NULL,psTab);
         if (siCnt<0) return(siCnt);
         if (psHdl->siTok==CLPTOK_RBC) {
            psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
            if (psHdl->siTok<0) return(psHdl->siTok);
         } else {
            return CLPERR(psHdl,CLPERR_SYN,"Character \')\' missing (MAIN)%s","");
         }
      } else {
         siCnt=siClpPrsParLst(pvHdl,0,NULL,psTab);
         if (siCnt<0) return(siCnt);
      }
      siErr=siClpFinMainObj(pvHdl,psTab,asSav);
      if (siErr<0) return(siErr);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(OBJ(MAIN(parlst))-CLS)\n",fpcPre(pvHdl,0));
      return(siCnt);
   }
}

static int siClpPrsAry(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siCnt;
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ARY(%s[typlst])-OPN)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=(psArg->psFix->siTyp==CLPTYP_STRING)?siClpScnSrc(pvHdl,CLPTOK_STR,psArg):siClpScnSrc(pvHdl,0,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   switch (psArg->psFix->siTyp) {
   case CLPTYP_NUMBER: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_NUM,psArg->psFix->siTyp,psArg); break;
   case CLPTYP_FLOATN: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_FLT,psArg->psFix->siTyp,psArg); break;
   case CLPTYP_STRING: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_STR,psArg->psFix->siTyp,psArg); break;
   case CLPTYP_OBJECT: siCnt=siClpPrsObjLst(pvHdl,siLev,psArg); break;
   case CLPTYP_OVRLAY: siCnt=siClpPrsOvlLst(pvHdl,siLev,psArg); break;
   default:
      return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter \'%s.%s\' is not supported with arrays",psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   if (siCnt<0) return(siCnt);
   if (psHdl->siTok==CLPTOK_SBC) {
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d ARY(%s[typlst])-CLS)\n",fpcPre(pvHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw);
      return(CLP_OK);
   } else {
      return CLPERR(psHdl,CLPERR_SYN,"Character \']\' missing (%s)",fpcPat(pvHdl,siLev));
   }
}

static int siClpPrsValLst(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siTok,
   const int                     siTyp,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siPos=0;
   while (psHdl->siTok==siTok || psHdl->siTok==CLPTOK_KYW) {
      siErr=siClpPrsVal(pvHdl,siLev,siPos,(siTok==CLPTOK_STR)?siTok:0,siTyp,psArg);
      if (siErr<0) return(siErr);
      siPos++;
   }
   return(siPos);
}

static int siClpPrsObjLst(
   void*                         pvHdl,
   const int                     siLev,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siPos=0;
   while (psHdl->siTok==CLPTOK_RBO) {
      siErr=siClpPrsObj(pvHdl,siLev,siPos,psArg);
      if (siErr<0) return(siErr);
      siPos++;
   }
   return(siPos);
}

static int siClpPrsOvlLst(
   void*                         pvHdl,
   const int                     siLev,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siPos=0;
   while (psHdl->siTok==CLPTOK_KYW) {
      siErr=siClpPrsOvl(pvHdl,siLev,siPos,psArg);
      if (siErr<0) return(siErr);
      siPos++;
   }
   return(siPos);
}

static int siClpPrsVal(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     siTok,
   const int                     siTyp,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siInd;
   TsSym*                        psVal;
   char                          acVal[CLPMAX_LEXSIZ];
   strcpy(acVal,psHdl->acLex);
   if (psHdl->siTok==CLPTOK_KYW) {
      psHdl->siTok=siClpScnSrc(pvHdl,siTok,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      psHdl->apPat[siLev]=psArg;
      siInd=siClpSymFnd(pvHdl,siLev+1,siPos,acVal,psArg->psDep,&psVal,NULL);
      if (siInd<0) return(siInd);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d KYW(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,acVal);
      return(siClpBldCon(pvHdl,siLev,siPos,psArg,psVal));
   } else {
      psHdl->siTok=siClpScnSrc(pvHdl,siTok,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,isPrnLex(psArg,acVal));
      return(siClpBldLit(pvHdl,siLev,siPos,siTyp,psArg,acVal));
   }
}

/**********************************************************************/

static int siClpPrsProLst(
   void*                         pvHdl,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siPos=0;
   while (psHdl->siTok==CLPTOK_KYW) {
      siErr=siClpPrsPro(pvHdl,siPos,psTab);
      if (siErr<0) return(siErr);
      siPos++;
   }
   return(siPos);
}

static int siClpPrsPro(
   void*                         pvHdl,
   const int                     siPos,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char                          acSup[CLPMAX_LEXSIZ];
   char                          acPat[CLPMAX_PATSIZ];
   int                           siLev;

   siLev=siClpPrsKywLst(pvHdl,siPos,acPat);
   if (siLev<0) return(siLev);
   if (psHdl->siTok==CLPTOK_SGN) {
      psHdl->siTok=siClpScnSrc(pvHdl,CLPTOK_SUP,psTab);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_SUP) {
         strcpy(acSup,psHdl->acLex);
         psHdl->siTok=siClpScnSrc(pvHdl,0,psTab);
         if (psHdl->siTok<0) return(psHdl->siTok);
         return(siClpBldPro(pvHdl,acPat,acSup));
      } else {
         return CLPERR(psHdl,CLPERR_SYN,"Property string (\"...\") missing (%s)",acPat);
      }
   } else {
      return CLPERR(psHdl,CLPERR_SYN,"Assignment character (\'=\') missing (%s)",acPat);
   }
}

static int siClpPrsKywLst(
   void*                         pvHdl,
   const int                     siPos,
   char*                         pcPat)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siLev=0;

   pcPat[0]=EOS;
   while (psHdl->siTok==CLPTOK_KYW) {
      if (strlen(pcPat)+strlen(psHdl->acLex)+1>=CLPMAX_PATLEN) {
         return CLPERR(psHdl,CLPERR_INT,"Property path (%s) is too long (more than %d byte)",pcPat,CLPMAX_PATLEN);
      }
      strcat(pcPat,psHdl->acLex);
      psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_DOT) {
         strcat(pcPat,".");
         psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
         if (psHdl->siTok<0) return(psHdl->siTok);
      }
      siLev++;
   }
   return(siLev);
}

/**********************************************************************/

#define isPrnInt(p,v) (CLPISS_PWD(p->psStd->uiFlg)?((I64)0):(v))
#define isPrnFlt(p,v) (CLPISS_PWD(p->psStd->uiFlg)?((F64)0.0):(v))
#define isPrnStr(p,v) (CLPISS_PWD(p->psStd->uiFlg)?("###SECRET###"):(v))
#define isPrnLen(p,v) (CLPISS_PWD(p->psStd->uiFlg)?((int)0):(v))

static int siClpBldPro(
   void*                         pvHdl,
   const char*                   pcPat,
   const char*                   pcPro)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psSym;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   char                          acKyw[CLPMAX_LEXSIZ];
   char                          acRot[CLPMAX_PATSIZ];
   int                           siErr,siLev,i,l;

   if (strlen(psHdl->pcOwn)+strlen(psHdl->pcPgm)+strlen(psHdl->pcCmd)+2>=CLPMAX_PATLEN) {
      return CLPERR(psHdl,CLPERR_PAR,"Root (%s.%s.%s) is too long (>=%d)",psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd,CLPMAX_PATLEN);
   }
   sprintf(acRot,"%s.%s.%s",psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd);
   l=strlen(acRot);

   if (strxcmp(psHdl->isCas,acRot,pcPat,l,0,FALSE)==0) {
      if (pcPat[l]!='.') {
         return CLPERR(psHdl,CLPERR_SEM,"Property path (%s) is not valid",pcPat);
      }
      for (siLev=0,pcPtr=pcPat+l;pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
         for (pcKyw=pcPtr+1,i=0;pcKyw[i]!=EOS && pcKyw[i]!='.' && i<CLPMAX_LEXLEN;i++) acKyw[i]=pcKyw[i];
         acKyw[i]=EOS;
         siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
         if (siErr<0) return(siErr);
         psHdl->apPat[siLev]=psArg;
         psTab=psArg->psDep;
      }
      if (psArg!=NULL) {
         if (CLPISS_ARG(psArg->psStd->uiFlg) || CLPISS_ALI(psArg->psStd->uiFlg)) {
            strcpy(psArg->psFix->acPro,pcPro);
            psArg->psFix->pcDft=psArg->psFix->acPro;
            if (strlen(psHdl->acLst) + strlen(pcPat) + strlen(pcPro) + 8 < CLPMAX_LSTLEN) {
               sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s=\"%s\"\n",pcPat,isPrnLex(psArg,pcPro));
            }
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"BUILD-PROPERTY %s=\"%s\"\n",pcPat,isPrnStr(psArg,pcPro));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Path '%s' for property \"%s\" is not an argument or alias",pcPat,isPrnStr(psArg,pcPro));
         }
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Path '%s' not valid",pcPat);
      }
   } else {
      if (psHdl->isChk) {
         return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s.%s.%s)",pcPat,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd);
      }
   }
   return(CLP_OK);
}

static int siClpBldLnk(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     siNum,
   TsSym*                        psArg,
   const int                     isApp)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_NUMBER;
   I64                           siVal=siNum;

   if (psArg!=NULL) {
      if (isApp==FALSE) {
         psArg->psVar->pvPtr=psArg->psVar->pvDat;
         psArg->psVar->siCnt=0;
         psArg->psVar->siLen=0;
         psArg->psVar->siRst=psArg->psFix->siSiz;
         if (CLPISS_FIX(psArg->psStd->uiFlg)) psArg->psVar->siRst*=psArg->psFix->siMax;
      }
      if (psArg->psFix->siTyp!=siTyp) {
         return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of link \'%s.%s\' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
         return CLPERR(psHdl,CLPERR_SEM,"To many (>%d) occurrences of link \'%s.%s\' with type \'%s\'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for link \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of link are defined but data pointer or write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }

      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRId64") for link \'%s.%s\' need more than 8 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRId64") for link \'%s.%s\' need more than 16 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRId64") for link \'%s.%s\' need more than 32 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 8:
         *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRId64") of link \'%s.%s\' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=psArg->psFix->siSiz;
      psArg->psVar->siCnt++;
      return(psArg->psFix->siTyp);
   } else return(CLP_OK);
}

static int siClpBldSwt(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_SWITCH;
   I64                           siVal=psArg->psFix->siOid;
   char*                         pcHlp=NULL;
   int                           siErr;
   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' dont match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"To many (>%d) occurrences of \'%s.%s\' with type \'%s\'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (psArg->psVar->siRst<psArg->psFix->siSiz) {
      return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but data pointer or write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }

   switch (psArg->psFix->siSiz) {
   case 1:
      if (siVal<(-128) || siVal>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRId64") of \'%s.%s\' need more than 8 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 2:
      if (siVal<(-32768) || siVal>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRId64") of \'%s.%s\' need more than 16 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 4:
      if (siVal<(-2147483648LL) || siVal>4294967295LL) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRId64") of \'%s.%s\' need more than 32 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 8:
      *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   default:
      return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRId64") of \'%s.%s\' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   pcHlp=fpcPat(pvHdl,siLev);
   if (strlen(psHdl->acLst) + strlen(pcHlp) + strlen(psArg->psStd->pcKyw) + 8 < CLPMAX_LSTLEN) {
      sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s.%s=TRUE\n",pcHlp,psArg->psStd->pcKyw);
   }

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
   if (siErr<0) return(siErr);
   return(psArg->psFix->siTyp);
}

static int siClpBldLit(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     siTyp,
   TsSym*                        psArg,
   const char*                   pcVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,l0,l1,l2,siSln;
   I64                           siVal=0;
   F64                           flVal=0;
   char*                         pcHlp=NULL;
   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"To many (>%d) occurrences of \'%s.%s\' with type \'%s\'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (CLPISS_SEL(psArg->psStd->uiFlg)) {
      CLPERR(psHdl,CLPERR_SEM,"The argument \'%s.%s\' accept only a keyword representing a constant definition for type \'%s\'",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
      vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
      return(CLPERR_SEM);
   }
   if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but data pointer or write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }

   switch (siTyp) {
   case CLPTYP_SWITCH:
   case CLPTYP_NUMBER:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",fpcPat(pvHdl,siLev),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      errno=0;
      switch (pcVal[0]) {
      case 'b':siVal=strtoll(pcVal+1,&pcHlp, 2); break;
      case 'o':siVal=strtoll(pcVal+1,&pcHlp, 8); break;
      case 'd':siVal=strtoll(pcVal+1,&pcHlp,10); break;
      case 'x':siVal=strtoll(pcVal+1,&pcHlp,16); break;
      case 't':siVal=strtoll(pcVal+1,&pcHlp,10); break;
      default: return CLPERR(psHdl,CLPERR_SEM,"Base (%c) of number literal (%s.%s=%s) not supported",pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcVal+1));
      }
      if (errno || strlen(pcHlp)) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of \'%s.%s\' can\'t be converted to a 64 bit value (rest: %s)",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcHlp));
      }
      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>255) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of \'%s.%s\' need more than 8 Bit",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of \'%s.%s\' need more than 16 Bit",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of \'%s.%s\' need more than 32 Bit",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 8:
         *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      default:
         return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%s) of \'%s.%s\' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=psArg->psFix->siSiz;
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_FLOATN:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SEM,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      errno=0;
      switch (pcVal[0]) {
      case 'd':flVal=strtod(pcVal+1,&pcHlp); break;
      default: return CLPERR(psHdl,CLPERR_SEM,"Base (%c) of floating point literal (%s.%s=%s) not supported",pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcVal+1));
      }
      if (errno || strlen(pcHlp)) {
         return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of \'%s.%s\' can\'t be converted to a valid 64 bit value (rest: %s)",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,pcHlp);
      }
      switch (psArg->psFix->siSiz) {
      case 4:
         *((F32*)psArg->psVar->pvPtr)=flVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-F32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnFlt(psArg,flVal));
         break;
      case 8:
         *((F64*)psArg->psVar->pvPtr)=flVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-F64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnFlt(psArg,flVal));
         break;
      default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%s) of \'%s.%s\' is not 4 (float) or 8 (double))",psArg->psFix->siSiz,isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=psArg->psFix->siSiz;
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_STRING:
      if (CLPISS_FIX(psArg->psStd->uiFlg)) l0=psArg->psFix->siSiz; else l0=psArg->psVar->siRst;
      l1=strlen(pcVal+2);
      switch (pcVal[0]) {
      case 'x':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1%2) {
               return CLPERR(psHdl,CLPERR_LEX,"Length of hexadecimal string (%c(%s)) for \'%s.%s\' is not a multiple of 2",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            if ((l1/2)>l0) {
               return CLPERR(psHdl,CLPERR_LEX,"Hexadecimal string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
            }
            l2=hex2bin(pcVal+2,(U08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1/2) {
               return CLPERR(psHdl,CLPERR_SEM,"Hexadecimal string (%c(%s)) of \'%s.%s\' can\'t be converted from hex to bin",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            siSln=l2;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-HEX(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for \'%s.%s\' is binary (only zero terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 'a':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               return CLPERR(psHdl,CLPERR_LEX,"ASCII string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
            }
            l2=chr2asc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               return CLPERR(psHdl,CLPERR_SEM,"ASCII string (%c(%s)) of \'%s.%s\' can\'t be converted to ASCII",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-ASC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for \'%s.%s\' is binary (only zero terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 'e':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               return CLPERR(psHdl,CLPERR_LEX,"EBCDIC string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
            }
            l2=chr2ebc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               return CLPERR(psHdl,CLPERR_SEM,"EBCDIC string (%c(%s)) of \'%s.%s\' can\'t be converted to EBCDIC",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-EBC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for \'%s.%s\' is binary (only zero terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 'c':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1); l2=l1;
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for \'%s.%s\' is binary (only zero terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 's':
         if (l1+1>l0) {
            return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
         }
         memcpy(psArg->psVar->pvPtr,pcVal+2,l1);
         ((char*)psArg->psVar->pvPtr)[l1]=EOS;
         l2=l1+1; siSln=l1;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         break;
      case 'd':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (CLPISS_HEX(psArg->psStd->uiFlg)) {
               if (l1%2) {
                  return CLPERR(psHdl,CLPERR_LEX,"Length of hexadecimal string (%c(%s)) for \'%s.%s\' is not a multiple of 2",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               if ((l1/2)>l0) {
                  return CLPERR(psHdl,CLPERR_LEX,"Hexadecimal string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               l2=hex2bin(pcVal+2,(U08*)psArg->psVar->pvPtr,l1);
               if (l2!=l1/2) {
                  return CLPERR(psHdl,CLPERR_SEM,"Hexadecimal string (%c(%s)) of \'%s.%s\' can\'t be converted from hex to bin",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siSln=l2;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-HEX(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            } else if (CLPISS_ASC(psArg->psStd->uiFlg)) {
               if (l1>l0) {
                  return CLPERR(psHdl,CLPERR_LEX,"ASCII string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               l2=chr2asc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
               if (l2!=l1) {
                  return CLPERR(psHdl,CLPERR_SEM,"ASCII string (%c(%s)) of \'%s.%s\' can\'t be converted to ASCII",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siSln=l1;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-ASC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            } else if (CLPISS_EBC(psArg->psStd->uiFlg)) {
               if (l1>l0) {
                  return CLPERR(psHdl,CLPERR_LEX,"EBCDIC string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               l2=chr2ebc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
               if (l2!=l1) {
                  return CLPERR(psHdl,CLPERR_SEM,"EBCDIC string (%c(%s)) of \'%s.%s\' can\'t be converted to EBCDIC",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siSln=l1;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-EBC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            } else {
               if (l1>l0) {
                  return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               memcpy(psArg->psVar->pvPtr,pcVal+2,l1); l2=l1;
               siSln=l1;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            }
         } else {
            if (l1+1>l0) {
               return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of \'%s.%s\' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1);
            ((char*)psArg->psVar->pvPtr)[l1]=EOS;
            l2=l1+1; siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         }
         break;
      default:
         CLPERR(psHdl,CLPERR_LEX,"String prefix (%c) of \'%s.%s\' is not supported",pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following values:%s","");
         CLPERRADD(psHdl,1,"x - for conversion from hex to bin%s","");
         CLPERRADD(psHdl,1,"a - for conversion in ASCII%s","");
         CLPERRADD(psHdl,1,"e - for conversion in EBCDIC%s","");
         CLPERRADD(psHdl,1,"c - for no conversion (normal character string without zero termination)%s","");
         CLPERRADD(psHdl,1,"x - s - normal character string with zero termination%s","");
         return(CLPERR_LEX);
      }

      if (CLPISS_FIX(psArg->psStd->uiFlg)) {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
         }
         memset(((U08*)psArg->psVar->pvPtr)+l2,0,psArg->psFix->siSiz-l2);
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
         psArg->psVar->siLen+=psArg->psFix->siSiz;
         psArg->psVar->siRst-=psArg->psFix->siSiz;
         siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
         if (siErr<0) return(siErr);
      } else {
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+l2;
         psArg->psVar->siLen+=l2;
         psArg->psVar->siRst-=l2;
         siErr=siClpBldLnk(pvHdl,siLev,siPos,l2,psArg->psFix->psEln,TRUE);
         if (siErr<0) return(siErr);
      }
      siErr=siClpBldLnk(pvHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
      if (siErr<0) return(siErr);
      break;
   default: return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter \'%s.%s\' not supported in this case (literal)",psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   psArg->psVar->siCnt++;

   pcHlp=fpcPat(pvHdl,siLev);
   if (strlen(psHdl->acLst) + strlen(pcHlp) + strlen(psArg->psStd->pcKyw) + strlen(pcVal) + 4 < CLPMAX_LSTLEN) {
      sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s.%s=%s\n",pcHlp,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal));
   }

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
   if (siErr<0) return(siErr);
   return(psArg->psFix->siTyp);
}

static int siClpBldCon(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,i,k,l;
   I64                           siVal,siEln,siSln;
   F64                           flVal;
   char*                         pcArg;
   char*                         pcVal;
   char*                         pcHlp;
   if (psArg->psFix->siTyp!=psVal->psFix->siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' don\'t match the type (%s) of value \'%s\'",apClpTyp[psArg->psFix->siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp],psVal->psStd->pcKyw);
   }
   if (CLPISS_SEL(psArg->psStd->uiFlg) && psVal->psVar->pvPtr!=NULL) {
      CLPERR(psHdl,CLPERR_SEM,"The argument \'%s.%s\' accept only a key word representing a constant definition for type \'%s\'",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      CLPERRADD(psHdl,0,"Please use one of the following values:%s","");
      vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
      return(CLPERR_SEM);
   }
   if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Key word (%s.%s) and type (%s) of argument defined but data pointer or write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt+psVal->psVar->siCnt>psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"To many (>%d) occurrences of \'%s.%s\' with type \'%s\'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psVal->psVar->siCnt==0) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) and type (%s) of constant value defined but data element counter is 0",psVal->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp]);
   }
   if (psVal->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) and type (%s) of constant value defined but data pointer not set",psVal->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp]);
   }
   switch (psArg->psFix->siTyp) {
   case CLPTYP_NUMBER:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      for (i=0;i<psVal->psVar->siCnt;i++) {
         switch (psVal->psFix->siSiz) {
         case 1: siVal=((I08*)psVal->psVar->pvDat)[i]; break;
         case 2: siVal=((I16*)psVal->psVar->pvDat)[i]; break;
         case 4: siVal=((I32*)psVal->psVar->pvDat)[i]; break;
         case 8: siVal=((I64*)psVal->psVar->pvDat)[i]; break;
         default:return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 1, 2, 4 or 8)",psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         switch (psArg->psFix->siSiz) {
         case 1:
            if (siVal<(-128) || siVal>255) {
               return CLPERR(psHdl,CLPERR_SEM,"Constant number (%s=%"PRId64") of \'%s.%s\' needs more than 8 Bit",psVal->psStd->pcKyw,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
            break;
         case 2:
            if (siVal<(-32768) || siVal>65535) {
               return CLPERR(psHdl,CLPERR_SEM,"Constant number (%s=%"PRId64") of \'%s.%s\' needs more than 16 Bit",psVal->psStd->pcKyw,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
            break;
         case 4:
            if (siVal<(-2147483648LL) || siVal>4294967295LL) {
               return CLPERR(psHdl,CLPERR_SEM,"Constant number (%s=%"PRId64") of \'%s.%s\' needs more than 32 Bit",psVal->psStd->pcKyw,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
            break;
         case 8:
            *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
            break;
         default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value of \'%s.%s\' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
         psArg->psVar->siLen+=psArg->psFix->siSiz;
         psArg->psVar->siRst-=psArg->psFix->siSiz;
      }
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_FLOATN:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      for (i=0;i<psVal->psVar->siCnt;i++) {
         switch (psVal->psFix->siSiz) {
         case 4: flVal=((F32*)psVal->psVar->pvDat)[i]; break;
         case 8: flVal=((F64*)psVal->psVar->pvDat)[i]; break;
         default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 4 or 8)",psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         switch (psArg->psFix->siSiz) {
         case 4:
            *((F32*)psArg->psVar->pvPtr)=(F32)flVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-F32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnFlt(psArg,flVal));
            break;
         case 8:
            *((F64*)psArg->psVar->pvPtr)=(F64)flVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-F64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnFlt(psArg,flVal));
            break;
         default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 4 or 8)",psArg->psFix->siSiz,psVal->psStd->pcKyw,psArg->psStd->pcKyw,fpcPat(pvHdl,siLev));
         }
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
         psArg->psVar->siLen+=psArg->psFix->siSiz;
         psArg->psVar->siRst-=psArg->psFix->siSiz;
      }
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_STRING:
      if (!CLPISS_BIN(psArg->psStd->uiFlg)) {
         if (CLPISS_BIN(psVal->psStd->uiFlg)) {
            return CLPERR(psHdl,CLPERR_SEM,"String constant \'%s\' for \'%s.%s\' is binary (only zero terminated character string permitted)",psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }
      if (CLPISS_FIX(psArg->psStd->uiFlg)) {
         if (psVal->psFix->siSiz>psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Size of constant value \'%s\' (%d) is bigger than size of argument \'%s.%s\' (%d) with type \'%s\'",
                                            psVal->psStd->pcKyw, psVal->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psFix->siSiz,apClpTyp[psArg->psFix->siTyp]);
         }
         pcArg=(char*)psArg->psVar->pvPtr;
         pcVal=(char*)psVal->psVar->pvDat;
         for (l=i=0;i<psVal->psVar->siCnt;i++) {
            if (psArg->psVar->siRst<psArg->psFix->siSiz) {
               return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
            }
            memcpy(pcArg,pcVal,psVal->psFix->siSiz);
            memset(pcArg+psVal->psFix->siSiz,0,psArg->psFix->siSiz-psVal->psFix->siSiz);
            for (siSln=0;pcArg[siSln]!=EOS && siSln<psArg->psFix->siSiz;siSln++);
            siEln=psVal->psFix->siSiz;
            if (psHdl->pfBld!=NULL) {
               fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s='",
                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw);
               if (CLPISS_PWD(psArg->psStd->uiFlg)) {
                  fprintf(psHdl->pfBld,"###SECRET###");
               } else {
                  for (k=0;k<siEln;k++) {
                     if (isprint(pcArg[k])) fprintf(psHdl->pfBld,"%c",pcArg[k]); else fprintf(psHdl->pfBld,"\\x%2.2X",(unsigned int)pcArg[k]);
                  }
               }
               fprintf(psHdl->pfBld,"\'(%"PRId64")\n",isPrnInt(psArg,siEln));
            }
            siErr=siClpBldLnk(pvHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
            if (siErr<0) return(siErr);
            pcArg+=psArg->psFix->siSiz;
            pcVal+=psVal->psFix->siSiz;
            psArg->psVar->siLen+=psArg->psFix->siSiz;
            psArg->psVar->siRst-=psArg->psFix->siSiz;
            siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
            if (siErr<0) return(siErr);
         }
         psArg->psVar->pvPtr=pcArg;
      } else {
         if (psArg->psVar->siRst<psVal->psVar->siLen) {
            return CLPERR(psHdl,CLPERR_SEM,"String data of constant value \'%s\' is longer than space left %d of argument \'%s.%s\'",psVal->psStd->pcKyw,psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         pcArg=(char*)psArg->psVar->pvPtr;
         pcVal=(char*)psVal->psVar->pvDat;
         memcpy(pcArg,pcVal,psVal->psVar->siLen);
         if (psVal->psFix->psEln==NULL) {
            for (siSln=0;pcArg[siSln]!=EOS && siSln<psArg->psFix->siSiz;siSln++);
            if (CLPISS_BIN(psVal->psStd->uiFlg)) {
               siEln=psVal->psVar->siLen;
            } else {
               siEln=(siSln==psArg->psFix->siSiz)?siSln:siSln+1;
            }
            if (psHdl->pfBld!=NULL) {
               fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s='",
                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw);
               if (CLPISS_PWD(psArg->psStd->uiFlg)) {
                  fprintf(psHdl->pfBld,"###SECRET###");
               } else {
                  for (k=0;k<siEln;k++) {
                     if (isprint(pcArg[k])) fprintf(psHdl->pfBld,"%c",pcArg[k]); else fprintf(psHdl->pfBld,"\\x%2.2X",(unsigned int)pcArg[k]);
                  }
               }
               fprintf(psHdl->pfBld,"\'(%"PRId64")\n",isPrnInt(psArg,siEln));
            }
            siErr=siClpBldLnk(pvHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
            if (siErr<0) return(siErr);
            siErr=siClpBldLnk(pvHdl,siLev,siPos,siEln,psArg->psFix->psEln,TRUE);
            if (siErr<0) return(siErr);
         } else {
            for (l=i=0;i<psVal->psVar->siCnt;i++) {
               switch (psVal->psFix->psEln->psFix->siSiz) {
               case 1: siEln=((I08*)(psVal->psFix->psEln->psVar->pvDat))[i]; break;
               case 2: siEln=((I16*)(psVal->psFix->psEln->psVar->pvDat))[i]; break;
               case 4: siEln=((I32*)(psVal->psFix->psEln->psVar->pvDat))[i]; break;
               case 8: siEln=((I64*)(psVal->psFix->psEln->psVar->pvDat))[i]; break;
               default:
                  return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 1, 2, 4 or 8)",psVal->psFix->psEln->psFix->siSiz,psVal->psFix->psEln->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               for (siSln=0;pcArg[siSln]!=EOS && siSln<siEln;siSln++);
               if (psHdl->pfBld!=NULL) {
                  fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s='",
                          fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw);
                  if (CLPISS_PWD(psArg->psStd->uiFlg)) {
                     fprintf(psHdl->pfBld,"###SECRET###");
                  } else {
                     for (k=0;k<siEln;k++) {
                        if (isprint(pcArg[k])) fprintf(psHdl->pfBld,"%c",pcArg[k]); else fprintf(psHdl->pfBld,"\\x%2.2X",(unsigned int)pcArg[k]);
                     }
                  }
                  fprintf(psHdl->pfBld,"\'(%"PRId64")\n",isPrnInt(psArg,siEln));
               }
               siErr=siClpBldLnk(pvHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
               if (siErr<0) return(siErr);
               siErr=siClpBldLnk(pvHdl,siLev,siPos,siEln,psArg->psFix->psEln,TRUE);
               if (siErr<0) return(siErr);
               l+=siEln; pcArg+=siEln;
            }
            if (l!=psVal->psVar->siLen) {
               return CLPERR(psHdl,CLPERR_INT,"Length conflict between argument \'%s.%s.%s\' and link \'%s\'",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psVal->psStd->pcKyw,psVal->psFix->psEln->psStd->pcKyw);
            }
         }
         psArg->psVar->siLen+=psVal->psVar->siLen;
         psArg->psVar->siRst-=psVal->psVar->siLen;
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psVal->psVar->siLen;
      }
      break;
   default: return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter \'%s.%s\' not supported in this case (constant)",psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   psArg->psVar->siCnt+=psVal->psVar->siCnt;

   pcHlp=fpcPat(pvHdl,siLev);
   if (strlen(psHdl->acLst) + strlen(pcHlp) + strlen(psArg->psStd->pcKyw) + strlen(psVal->psStd->pcKyw) + 4 < CLPMAX_LSTLEN) {
      sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s.%s=%s\n",pcHlp,psArg->psStd->pcKyw,psVal->psStd->pcKyw);
   }

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
   if (siErr<0) return(siErr);
   return(psArg->psFix->siTyp);
}

static int siClpIniMainObj(
   void*                         pvHdl,
   TsSym*                        psTab,
   TsVar*                        psSav)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psHlp;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   if (psHdl->pvDat!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
         *psSav=*psHlp->psVar;
         psHlp->psVar->pvDat=((char*)psHdl->pvDat)+psHlp->psFix->siOfs;
         psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
         psHlp->psVar->siCnt=0;
         psHlp->psVar->siLen=0;
         psHlp->psVar->siRst=psHlp->psFix->siSiz;
         if (CLPISS_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
      }
   } else {
      return CLPERR(psHdl,CLPERR_PAR,"Pointer to CLP data structure is NULL (%s.%s)",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"BUILD-BEGIN-MAIN-ARGUMENT-LIST\n");
   return(CLP_OK);
}

static int siClpFinMainObj(
   void*                         pvHdl,
   TsSym*                        psTab,
   const TsVar*                  psSav)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psHlp=NULL;
   int                           siErr,i;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(pvHdl,0,i,psHlp);
      if (siErr<0) {
         CLPERRADD(psHdl,0,"Set default value for argument \'%s.%s\' failed",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               CLPERR(psHdl,CLPERR_SEM,"Parameter \'%s.%s\' not specified",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please specify parameter:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter \'%s.%s\' is smaller than required minimum amount (%d)",psHlp->psVar->siCnt,fpcPat(pvHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
               CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
            }
            return(CLPERR_SEM);
         }
      }
   }

   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) *psHlp->psVar=*psSav;

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"BUILD-END-MAIN-ARGUMENT-LIST\n");
   return(CLP_OK);
}

static int siClpIniMainOvl(
   void*                         pvHdl,
   TsSym*                        psTab,
   TsVar*                        psSav)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psHlp;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   if (psHdl->pvDat!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
         *psSav=*psHlp->psVar;
         psHlp->psVar->pvDat=((char*)psHdl->pvDat);
         psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
         psHlp->psVar->siCnt=0;
         psHlp->psVar->siLen=0;
         psHlp->psVar->siRst=psHlp->psFix->siSiz;
         if (CLPISS_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
      }
   } else {
      return CLPERR(psHdl,CLPERR_PAR,"Pointer to CLP data structure is NULL (%s.%s)",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"BUILD-BEGIN-MAIN-ARGUMENT\n");
   return(CLP_OK);
}

static int siClpFinMainOvl(
   void*                         pvHdl,
   TsSym*                        psTab,
   const TsVar*                  psSav,
   const int                     siOid)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psHlp=NULL;
   int                           siErr,i;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(pvHdl,0,i,psHlp);
         if (siErr<0) {
            CLPERRADD(psHdl,0,"Set default value for argument \'%s.%s\' failed",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  CLPERR(psHdl,CLPERR_SEM,"Parameter \'%s.%s\' not specified",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
                  CLPERRADD(psHdl,0,"Please specify parameter:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_SEL(psHlp->psStd->uiFlg));
               } else {
                  CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter \'%s.%s\' is smaller than required minimum amount (%d)",psHlp->psVar->siCnt,fpcPat(pvHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
               }
               return(CLPERR_SEM);
            }
         }
      }
   }

   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) *psHlp->psVar=*psSav;

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"BUILD-END-MAIN-ARGUMENT\n");
   return(CLP_OK);
}

static int siClpIniObj(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar*                        psSav)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_OBJECT;
   TsSym*                        psHlp;
   char*                         pcHlp;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"To many (>%d) occurrences of \'%s.%s\' with type \'%s\'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siRst<psArg->psFix->siSiz) {
      return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (psArg->psDep==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   for (psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
      *psSav=*psHlp->psVar;
      psHlp->psVar->pvDat=((char*)psArg->psVar->pvPtr)+psHlp->psFix->siOfs;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siRst=psHlp->psFix->siSiz;
      if (CLPISS_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
   }
   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-BEGIN-OBJECT-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   pcHlp=fpcPat(pvHdl,siLev);
   if (strlen(psHdl->acLst) + strlen(pcHlp) + strlen(psArg->psStd->pcKyw) + 8 < CLPMAX_LSTLEN) {
      sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s.%s(\n",pcHlp,psArg->psStd->pcKyw);
   }

   psHdl->apPat[siLev]=psArg;
   *ppDep=psArg->psDep;
   return(psArg->psFix->siTyp);
}

static int siClpFinObj(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar*                  psSav)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_OBJECT;
   TsSym*                        psHlp;
   char*                         pcHlp;
   int                           siErr,i;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(pvHdl,siLev+1,i,psHlp);
      if (siErr<0) {
         CLPERRADD(psHdl,0,"Set default value for argument \'%s.%s.%s\' failed",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               CLPERR(psHdl,CLPERR_SEM,"Parameter \'%s.%s.%s\' not specified",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please specify parameter:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter \'%s.%s.%s\' is smaller than required minimum amount (%d)",psDep->psVar->siCnt,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
               CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
            }
            return(CLPERR_SEM);
         }
      }
   }

   for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) *psHlp->psVar=*psSav;

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-END-OBJECT-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   pcHlp=fpcPat(pvHdl,siLev);
   if (strlen(psHdl->acLst) + strlen(pcHlp) + strlen(psArg->psStd->pcKyw) + 8 < CLPMAX_LSTLEN) {
      sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s.%s)\n",pcHlp,psArg->psStd->pcKyw);
   }

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
   if (siErr<0) return(siErr);
   return(psArg->psFix->siTyp);
}

static int siClpIniOvl(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar*                        psSav)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_OVRLAY;
   TsSym*                        psHlp;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"To many (>%d) occurrences of \'%s.%s\' with type \'%s\'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siRst<psArg->psFix->siSiz) {
      return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (psArg->psDep==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   for (psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
      *psSav=*psHlp->psVar;
      psHlp->psVar->pvDat=(char*)psArg->psVar->pvPtr;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siRst=psHlp->psFix->siSiz;
      if (CLPISS_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
   }

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-BEGIN-OVERLAY-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   psHdl->apPat[siLev]=psArg;
   *ppDep=psArg->psDep;
   return(psArg->psFix->siTyp);
}

static int siClpFinOvl(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar*                  psSav,
   const int                     siOid)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_OVRLAY;
   TsSym*                        psHlp;
   int                           siErr,i;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(pvHdl,siLev+1,i,psHlp);
         if (siErr<0) {
            CLPERRADD(psHdl,0,"Set default value for argument \'%s.%s.%s\' failed",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  CLPERR(psHdl,CLPERR_SEM,"Parameter \'%s.%s.%s\' not specified",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
                  CLPERRADD(psHdl,0,"Please specify parameter:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
               } else {
                  CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter \'%s.%s.%s\' is smaller than required minimum amount (%d)",psDep->psVar->siCnt,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
               }
               return(CLPERR_SEM);
            }
         }
      }
   }

   for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) *psHlp->psVar=*psSav;

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-END-OVERLAY-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,siOid,psArg->psFix->psOid,TRUE);
   if (siErr<0) return(siErr);
   return(psArg->psFix->siTyp);
}

/**********************************************************************/

static int siClpSetDefault(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psVal=NULL;
   TsSym*                        psDep=NULL;
   TsVar                         asSav[CLPMAX_TABCNT];
   const char*                   pcCur=NULL;
   char                          acLex[CLPMAX_LEXLEN];
   int                           siErr,siInd,siTok;

   if (CLPISS_ARG(psArg->psStd->uiFlg) && psArg->psVar->siCnt==0 && psArg->psFix->pcDft!=NULL && strlen(psArg->psFix->pcDft)) {
      pcCur=psArg->psFix->pcDft;
      for (siTok=siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&pcCur,acLex,0,psArg);siTok!=CLPTOK_END;
           siTok=siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&pcCur,acLex,0,psArg)) {
         switch(siTok) {
         case CLPTOK_NUM:
            if (psArg->psFix->siTyp==CLPTYP_SWITCH) {
               siErr=siClpBldLit(pvHdl,siLev,siPos,CLPTYP_SWITCH,psArg,acLex);
               if (siErr<0) return(siErr);
            } else {
               siErr=siClpBldLit(pvHdl,siLev,siPos,CLPTYP_NUMBER,psArg,acLex);
               if (siErr<0) return(siErr);
            }
            break;
         case CLPTOK_STR:
            siErr=siClpBldLit(pvHdl,siLev,siPos,CLPTYP_STRING,psArg,acLex);
            if (siErr<0) return(siErr);
            break;
         case CLPTOK_FLT:
            siErr=siClpBldLit(pvHdl,siLev,siPos,CLPTYP_FLOATN,psArg,acLex);
            if (siErr<0) return(siErr);
            break;
         case CLPTOK_KYW:
            if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
               if (strxcmp(psHdl->isCas,acLex,"INIT",0,0,FALSE)!=0) {
                  return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) in default / property definition for object \'%s.%s\' is not \'INIT\'",acLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siErr=siClpIniObj(pvHdl,siLev,siPos,psArg,&psDep,asSav);
               if (siErr<0) return(siErr);
               siErr=siClpFinObj(pvHdl,siLev,siPos,psArg,psDep,asSav);
               if (siErr<0) return(siErr);
            } else  if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
               siErr=siClpIniOvl(pvHdl,siLev,siPos,psArg,&psDep,asSav);
               if (siErr<0) return(siErr);
               siInd=siClpSymFnd(pvHdl,siLev+1,siPos,acLex,psDep,&psVal,NULL);
               if (siInd<0) return(siInd);
               siErr=siClpFinOvl(pvHdl,siLev,siPos,psArg,psDep,asSav,psVal->psFix->siOid);
               if (siErr<0) return(siErr);
            } else if (psArg->psFix->siTyp==CLPTYP_SWITCH) {
                  if (strxcmp(psHdl->isCas,acLex,"ON",0,0,FALSE)!=0 && strxcmp(psHdl->isCas,acLex,"OFF",0,0,FALSE)!=0) {
                     return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) in default / property definition for switch \'%s.%s\' is not \'ON\' or \'OFF\'",acLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  if (strxcmp(psHdl->isCas,acLex,"ON",0,0,FALSE)==0) {
                     siErr=siClpBldSwt(pvHdl,siLev,siPos,psArg);
                     if (siErr<0) return(siErr);
                  }
            } else {
               psHdl->apPat[siLev]=psArg;
               siInd=siClpSymFnd(pvHdl,siLev+1,siPos,acLex,psArg->psDep,&psVal,NULL);
               if (siInd<0) return(siInd);
               siErr=siClpBldCon(pvHdl,siLev,siPos,psArg,psVal);
               if (siErr<0) return(siErr);
            }
            break;
         default: return CLPERR(psHdl,CLPERR_SYN,"Token (%s) not allowed in default / property definition (%s) for argument \'%s.%s'",apClpTok[siTok],isPrnStr(psArg,psArg->psFix->pcDft),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }
   }
   return(CLP_OK);
}

/**********************************************************************/

static void vdClpPrnArg(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siLev,
   const char*                   pcKyw,
   const char*                   pcAli,
   const int                     siKwl,
   const int                     siTyp,
   const char*                   pcHlp,
   const char*                   pcDft,
   const int                     isSel,
   const int                     isCon)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char*                         p=fpcPre(pvHdl,siLev);
   int                           i,siLen;
   const char*                   a="TYPE";
   const char*                   b=(isSel)?"SELECTION":((isCon)?"KEYWORD":apClpTyp[siTyp]);

   if (pcAli!=NULL && strlen(pcAli)) {
      a="ALIAS";
      b=pcAli;
   }
   if (pfOut!=NULL) {
      if (psHdl->isCas) {
         if (pcDft!=NULL && strlen(pcDft)) {
            fprintf(pfOut,"%s %s (%s: %s) - %s (PROPERTY: [%s])\n",p,pcKyw,a,b,pcHlp,pcDft);
         } else {
            fprintf(pfOut,"%s %s (%s: %s) - %s\n",p,pcKyw,a,b,pcHlp);
         }
         fprintf(pfOut,"%s ",p);
         for (i=0;i<siKwl;i++) fprintf(pfOut,"^");
         fprintf(pfOut,"\n");
      } else {
         siLen=strlen(pcKyw);
         fprintf(pfOut,"%s ",p);
         for (i=0;i<siKwl;i++) fprintf(pfOut,"%c",toupper(pcKyw[i]));
         for (/*i=i*/;i<siLen;i++) fprintf(pfOut,"%c",tolower(pcKyw[i]));
         if (pcDft!=NULL && strlen(pcDft)) {
            fprintf(pfOut," (%s: %s) - %s (PROPERTY: [%s])\n",a,b,pcHlp,pcDft);
         } else {
            fprintf(pfOut," (%s: %s) - %s\n",a,b,pcHlp);
         }
      }
   }
}

static void vdClpPrnArgTab(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siLev,
   int                           siTyp,
   const TsSym*                  psTab)
{
   const TsSym*                  psHlp;
   if (psTab!=NULL && psTab->psHih!=NULL) {
      if (CLPISS_CON(psTab->psStd->uiFlg)) {
         if (siTyp<0) siTyp=psTab->psHih->psFix->siTyp;
         if (CLPISS_SEL(psTab->psHih->psStd->uiFlg)==FALSE) {
            if (siTyp>=0) {
               fprintf(pfOut,"%s Enter a value (TYPE: %s) or use one of the keywords below:\n",fpcPre(pvHdl,siLev),apClpTyp[siTyp]);
            } else {
               fprintf(pfOut,"%s Enter a value or use one of the keywords below:\n",fpcPre(pvHdl,siLev));
            }
         }
      }
   }
   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
      if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
         vdClpPrnArg(pvHdl,pfOut,siLev,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                     CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
      }
   }
}

static void vdClpPrnOpt(
   FILE*                         pfOut,
   const char*                   pcSep,
   const int                     siTyp,
   const TsSym*                  psTab)
{
   int                           k=0;
   const TsSym*                  psHlp;
   if (pfOut!=NULL && psTab!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
            if (k) fprintf(pfOut,"%s",pcSep);
            fprintf(pfOut,"%s",psHlp->psStd->pcKyw);
            k++;
         }
      }
   }
}

static void vdClpPrnAli(
   FILE*                         pfOut,
   const char*                   pcSep,
   const TsSym*                  psTab)
{
   const TsSym*                  psHlp;
   if (pfOut!=NULL && psTab!=NULL) {
      fprintf(pfOut,"%s",psTab->psStd->pcKyw);
      for (psHlp=psTab->psNxt;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISS_ALI(psHlp->psStd->uiFlg) && psHlp->psStd->psAli==psTab) {
            fprintf(pfOut,"%s",pcSep);
            fprintf(pfOut,"%s",psHlp->psStd->pcKyw);
         }
      }
   }
}

static int siClpPrnCmd(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siCnt,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psArg,
   const TsSym*                  psTab,
   const int                     isSkr,
   const int                     isMin)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,k=0;
   const char*                   pcHlp=NULL;
   const TsSym*                  psHlp;
   const char*                   pcSep;

   if (psTab==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Argument table not defined%s","");
   }

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
   }

   if (siLev<siDep || siDep>9) {
      if (psArg==NULL) {
         if (psHdl->isOvl) {
            pcSep=psHdl->pcOpt;
            if (siCnt==0) {
               fprintf(pfOut,"%s.{",psHdl->pcCmd);
               if (isSkr) k++;
            }
         } else {
            pcSep=psHdl->pcEnt;
            if (siCnt==0) {
               fprintf(pfOut,"%s(",psHdl->pcCmd);
               if (isSkr) k++;
            }
         }
         if (isSkr) k++;
      } else {
         if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
            pcSep=psHdl->pcOpt;
            if (siCnt==0) {
               fprintf(pfOut,"%s.{",fpcPat(pvHdl,siLev));
               if (isSkr) k++;
            }
         }  else {
            pcSep=psHdl->pcEnt;
            if (siCnt==0) {
               fprintf(pfOut,"%s(",fpcPat(pvHdl,siLev));
               if (isSkr) k++;
            }
         }
      }
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISS_CMD(psHlp->psStd->uiFlg) && !CLPISS_LNK(psHlp->psStd->uiFlg) && !CLPISS_ALI(psHlp->psStd->uiFlg)) {
            if (isSkr) {
               if (k) fprintf(pfOut,"\n%s ",fpcPre(pvHdl,siLev));
                 else fprintf(pfOut,"%s "  ,fpcPre(pvHdl,siLev));
               if (isMin) {
                  if (psHlp->psFix->siMin) fprintf(pfOut,"! "); else fprintf(pfOut,"? ");
               }
            } else {
               if (k) fprintf(pfOut,"%s",pcSep);
               if (isMin) {
                  if (psHlp->psFix->siMin) fprintf(pfOut,"!"); else fprintf(pfOut,"?");
               }
            }
            k++;
            switch (psHlp->psFix->siTyp) {
            case CLPTYP_SWITCH:
               if (psHlp->psFix->siMax==1) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);
               } else {
                  return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter \'%s\' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
               }
               break;
            case CLPTYP_NUMBER:
               if (psHlp->psFix->siMax==1) {
                  if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,CLP_ASSIGNMENT);
                     vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                  } else {
                     if (psHlp->psDep!=NULL) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%snum%s",CLP_ASSIGNMENT,psHdl->pcOpt);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     } else {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%snum",CLP_ASSIGNMENT);
                     }
                  }
               } else if (psHlp->psFix->siMax>1) {
                  if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[");
                     vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     fprintf(pfOut,"...]");
                  } else {
                     if (psHlp->psDep!=NULL) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[num%s",psHdl->pcOpt);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        fprintf(pfOut,"...]");
                     } else {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[num...]");
                     }
                  }
               } else {
                  return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter \'%s\' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
               }
               break;
            case CLPTYP_FLOATN:
               if (psHlp->psFix->siMax==1) {
                  if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,CLP_ASSIGNMENT);
                     vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                  } else {
                     if (psHlp->psDep!=NULL) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%sflt%s",CLP_ASSIGNMENT,psHdl->pcOpt);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     } else {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%sflt",CLP_ASSIGNMENT);
                     }
                  }
               } else if (psHlp->psFix->siMax>1) {
                  if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[");
                     vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     fprintf(pfOut,"...]");
                  } else {
                     if (psHlp->psDep!=NULL) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[flt%s",psHdl->pcOpt);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        fprintf(pfOut,"...]");
                     } else {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[flt...]");
                     }
                  }
               } else {
                  return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter \'%s\' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
               }
               break;
            case CLPTYP_STRING:
               if (CLPISS_BIN(psHlp->psStd->uiFlg)) {
                  if (CLPISS_HEX(psHlp->psStd->uiFlg)) {
                     pcHlp="bin-hex";
                  } else if (CLPISS_ASC(psHlp->psStd->uiFlg)) {
                     pcHlp="bin-ascii";
                  } else if (CLPISS_EBC(psHlp->psStd->uiFlg)) {
                     pcHlp="bin-ebcdic";
                  } else if (CLPISS_CHR(psHlp->psStd->uiFlg)) {
                     pcHlp="bin-char";
                  } else {
                     pcHlp="bin";
                  }
               } else pcHlp="str";
               if (psHlp->psFix->siMax==1) {
                  if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,CLP_ASSIGNMENT);
                     vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                  } else {
                     if (psHlp->psDep!=NULL) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%s\'%s\'%s",CLP_ASSIGNMENT,pcHlp,psHdl->pcOpt);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     } else {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%s\'%s\'",CLP_ASSIGNMENT,pcHlp);
                     }
                  }
               } else if (psHlp->psFix->siMax>1) {
                  if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[");
                     vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     fprintf(pfOut,"...]");
                  } else {
                     if (psHlp->psDep!=NULL) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[\'%s\'%s",pcHlp,psHdl->pcOpt);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        fprintf(pfOut,"...]");
                     } else {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[\'%s\'...]",pcHlp);
                     }
                  }
               } else {
                  return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter \'%s\' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
               }
               break;
            case CLPTYP_OBJECT:
               if (psHlp->psDep==NULL) {
                  return CLPERR(psHdl,CLPERR_TAB,"Argument table for object \'%s\' not defined",psHlp->psStd->pcKyw);
               }
               if (psHlp->psFix->siMax==1) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp); fprintf(pfOut,"(");
                  if (isSkr && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                  if (siErr<0) return(siErr);
                  fprintf(pfOut,")");
               } else if (psHlp->psFix->siMax>1) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp); fprintf(pfOut,"[(");
                  if (isSkr && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                  if (siErr<0) return(siErr);
                  fprintf(pfOut,")...]");
               } else {
                  return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter \'%s\' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
               }
               break;
            case CLPTYP_OVRLAY:
               if (psHlp->psDep==NULL) {
                  return CLPERR(psHdl,CLPERR_TAB,"Argument table for object \'%s\' not defined",psHlp->psStd->pcKyw);
               }
               if (psHlp->psFix->siMax==1) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,".{");
                  if (isSkr  && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                  if (siErr<0) return(siErr);
                  fprintf(pfOut,"}");
               } else if (psHlp->psFix->siMax>1) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"[{");
                  if (isSkr && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                  if (siErr<0) return(siErr);
                  fprintf(pfOut,"}...]");
               } else {
                  return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter \'%s\' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
               }
               break;
            default:
               return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter \'%s\' not supported",psHlp->psFix->siTyp,psHlp->psStd->pcKyw);
            }
         }
      }
      if (psArg==NULL) {
         if (psHdl->isOvl) {
            if (siCnt==0) fprintf(pfOut,"}");
         } else {
            if (siCnt==0) fprintf(pfOut,")");
         }
      } else {
         if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
            if (siCnt==0) fprintf(pfOut,"}");
         } else {
            if (siCnt==0) fprintf(pfOut,")");
         }
      }
   }
   return (CLP_OK);
}

static int siClpPrnHlp(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     isAli,
   const int                     siLev,
   const int                     siDep,
   const int                     siTyp,
   const TsSym*                  psTab,
   const int                     isFlg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp;
   int                           siErr;

   if (psTab==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Argument table not defined%s","");
   }

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
   }

   if (siLev<siDep || siDep>9) {
      if (isFlg) {
         if (siTyp>=0) {
            fprintf(pfOut,"%s Enter a value (TYPE: %s) or use one of the keywords below:\n",fpcPre(pvHdl,siLev),apClpTyp[siTyp]);
         } else {
            fprintf(pfOut,"%s Enter a value or use one of the keywords below:\n",fpcPre(pvHdl,siLev));
         }
      }
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && CLPISS_CMD(psHlp->psStd->uiFlg) && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
            if (!CLPISS_ALI(psHlp->psStd->uiFlg) || (CLPISS_ALI(psHlp->psStd->uiFlg) && isAli)) {
               vdClpPrnArg(pvHdl,pfOut,siLev,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISS_SEL(psHlp->psStd->uiFlg),CLPISS_CON(psHlp->psStd->uiFlg));
               if (psHlp->psDep!=NULL) {
                  if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnHlp(pvHdl,pfOut,isAli,siLev+1,siDep,-1,psHlp->psDep,FALSE);
                     if (siErr<0) return(siErr);
                  } else {
                     psHdl->apPat[siLev]=psHlp;
                     if (CLPISS_SEL(psHlp->psStd->uiFlg)) {
                        siErr=siClpPrnHlp(pvHdl,pfOut,isAli,siLev+1,siDep,psHlp->psFix->siTyp,psHlp->psDep,FALSE);
                        if (siErr<0) return(siErr);
                     } else {
                        siErr=siClpPrnHlp(pvHdl,pfOut,isAli,siLev+1,siDep,psHlp->psFix->siTyp,psHlp->psDep,TRUE);
                        if (siErr<0) return(siErr);
                     }
                  }
               }
            }
         }
      }
   }

   return (CLP_OK);
}

static int siClpPrnSyn(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     isPat,
   const int                     siLev,
   const TsSym*                  psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const char*                   pcHlp=NULL;
   int                           siErr;

   if (psArg==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Argument not defined%s","");
   }

   if (isPat) {
      fprintf(pfOut,"%s.",fpcPat(pvHdl,siLev));
   }
   switch (psArg->psFix->siTyp) {
   case CLPTYP_SWITCH:
      vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
      break;
   case CLPTYP_NUMBER:
      if (psArg->psFix->siMax==1) {
         if (CLPISS_SEL(psArg->psStd->uiFlg)) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg); fprintf(pfOut,CLP_ASSIGNMENT);
            vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
         } else {
            if (psArg->psDep!=NULL) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%snum%s",CLP_ASSIGNMENT,psHdl->pcOpt);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%snum",CLP_ASSIGNMENT);
            }
         }
      } else {
         if (CLPISS_SEL(psArg->psStd->uiFlg)) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[");
            vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            fprintf(pfOut,"...]");
         } else {
            if (psArg->psDep!=NULL) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[num%s",psHdl->pcOpt);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...]");
            } else {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[num...]");
            }
         }
      }
      break;
   case CLPTYP_FLOATN:
      if (psArg->psFix->siMax==1) {
         if (CLPISS_SEL(psArg->psStd->uiFlg)) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,CLP_ASSIGNMENT);
            vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
         } else {
            if (psArg->psDep!=NULL) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%sflt%s",CLP_ASSIGNMENT,psHdl->pcOpt);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%sflt",CLP_ASSIGNMENT);
            }
         }
      } else {
         if (CLPISS_SEL(psArg->psStd->uiFlg)) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[");
            vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            fprintf(pfOut,"...]");
         } else {
            if (psArg->psDep!=NULL) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[flt%s",psHdl->pcOpt);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...]");
            } else {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[flt...]");
            }
         }
      }
      break;
   case CLPTYP_STRING:
      if (CLPISS_BIN(psArg->psStd->uiFlg)) {
         if (CLPISS_HEX(psArg->psStd->uiFlg)) {
            pcHlp="bin-hex";
         } else if (CLPISS_ASC(psArg->psStd->uiFlg)) {
            pcHlp="bin-ascii";
         } else if (CLPISS_EBC(psArg->psStd->uiFlg)) {
            pcHlp="bin-ebcdic";
         } else if (CLPISS_CHR(psArg->psStd->uiFlg)) {
            pcHlp="bin-char";
         } else {
            pcHlp="bin";
         }
      } else pcHlp="str";
      if (psArg->psFix->siMax==1) {
         if (CLPISS_SEL(psArg->psStd->uiFlg)) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,CLP_ASSIGNMENT);
            vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
         } else {
            if (psArg->psDep!=NULL) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%s\'%s\'%s",CLP_ASSIGNMENT,pcHlp,psHdl->pcOpt);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%s\'%s\'",CLP_ASSIGNMENT,pcHlp);
            }
         }
      } else {
         if (CLPISS_SEL(psArg->psStd->uiFlg)) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[");
            vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            fprintf(pfOut,"...]");
         } else {
            if (psArg->psDep!=NULL) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[\'%s\'%s",pcHlp,psHdl->pcOpt);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...]");
            } else {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"[\'%s\'...]",pcHlp);
            }
         }
      }
      break;
   case CLPTYP_OBJECT:
      if (psArg->psFix->siMax==1) {
         vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
         fprintf(pfOut,"(");
         psHdl->apPat[siLev]=psArg;
         siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
         fprintf(pfOut,")");
         if (siErr<0) return(siErr);
      } else {
         vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
         fprintf(pfOut,"[(");
         psHdl->apPat[siLev]=psArg;
         siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
         fprintf(pfOut,")...]");
         if (siErr<0) return(siErr);
      }
      break;
   case CLPTYP_OVRLAY:
      if (psArg->psFix->siMax==1) {
         vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
         fprintf(pfOut,".{");
         psHdl->apPat[siLev]=psArg;
         siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
         fprintf(pfOut,"}");
         if (siErr<0) return(siErr);
      } else {
         vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
         fprintf(pfOut,"[{");
         psHdl->apPat[siLev]=psArg;
         siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
         fprintf(pfOut,"}...]");
         if (siErr<0) return(siErr);
      }
      break;
   }

   return(CLP_OK);
}

static int siClpPrnDoc(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const int                     siLev,
   const int                     isNbr,
   const char*                   pcNum,
   const TsSym*                  psArg,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp;
   const TsSym*                  psSel;
   int                           siErr,isCon;
   int                           i,k,l,m;
   char                          acNum[CLPMAX_PRESIZ];
   char                          acArg[20];
   int                           siMan=0;
   int                           siLst=0;
   const TsSym*                  apMan[CLPMAX_HDEPTH];
   const TsSym*                  apLst[CLPMAX_HDEPTH];


   if (psTab!=NULL && pcNum!=NULL) {
      if (psTab->psBak!=NULL) {
         return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      }
      for (isCon=FALSE,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISS_ARG(psHlp->psStd->uiFlg) && CLPISS_CMD(psHlp->psStd->uiFlg)) {
            if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY || (psHlp->psFix->pcMan!=NULL && strlen(psHlp->psFix->pcMan))) {
               apMan[siMan]=psHlp; siMan++;
            } else {
               for (psSel=psHlp->psDep;psSel!=NULL;psSel=psSel->psNxt) {
                  if (psSel->psFix->pcMan!=NULL && strlen(psSel->psFix->pcMan)) {
                     apMan[siMan]=psHlp; siMan++;
                     break;
                  }
               }
               if (psSel==NULL) {
                  apLst[siLst]=psHlp; siLst++;
               }
            }
         } else if (CLPISS_CON(psHlp->psStd->uiFlg) && psArg->psFix->siTyp == psHlp->psFix->siTyp) {
            if (psHlp->psFix->pcMan!=NULL && strlen(psHlp->psFix->pcMan)) {
               apMan[siMan]=psHlp; siMan++;
            } else {
               apLst[siLst]=psHlp; siLst++;
            }
            isCon=TRUE;
         }
      }

      if (isCon) {
         if (psArg!=NULL) {
            if (siLst) {
               fprintf(pfDoc,".SELECTIONS\n\n");
               for (m=0;m<siLst;m++) {
                  fprintf(pfDoc," * `%s - %s`\n",apLst[m]->psStd->pcKyw,apLst[m]->psFix->pcHlp);
               }
               fprintf(pfDoc,"\n");
            }
            if (siMan) {
               for (k=m=0;m<siMan;m++) {
                  sprintf(acNum,"%s%d.",pcNum,k+1);
                  if (isNbr) {
                     fprintf(pfDoc,"%s CONSTANT \'%s\'\n",acNum,apMan[m]->psStd->pcKyw);
                     l=strlen(acNum)+strlen(apMan[m]->psStd->pcKyw)+12;
                     for (i=0;i<l;i++) fprintf(pfDoc,"+"); fprintf(pfDoc,"\n\n");
                  } else {
                     fprintf(pfDoc,"CONSTANT \'%s\'\n",apMan[m]->psStd->pcKyw);
                     l=strlen(apMan[m]->psStd->pcKyw)+11;
                     for (i=0;i<l;i++) fprintf(pfDoc,"+"); fprintf(pfDoc,"\n\n");
                  }
                  fprintf(pfDoc,".SYNOPSIS\n\n");
                  fprintf(pfDoc,"-----------------------------------------------------------------------\n");
                  fprintf(pfDoc,"HELP:   %s\n",apMan[m]->psFix->pcHlp);
                  fprintf(pfDoc,"PATH:   %s\n",fpcPat(pvHdl,siLev));
                  fprintf(pfDoc,"TYPE:   %s\n",apClpTyp[apMan[m]->psFix->siTyp]);
                  fprintf(pfDoc,"SYNTAX: %s\n",apMan[m]->psStd->pcKyw);
                  fprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
                  fprintf(pfDoc,".DESCRIPTION\n\n");
                  fprintf(pfDoc,"%s\n\n",apMan[m]->psFix->pcMan);
                  fprintf(pfDoc,"indexterm:[Constant %s]\n\n\n",apMan[m]->psStd->pcKyw);
                  k++;
               }
            }
         }
      } else {
         if (siLst) {
            fprintf(pfDoc,".ARGUMENTS\n\n");
            for (m=0;m<siLst;m++) {
               fprintf(pfDoc," * `%s: ",apClpTyp[apLst[m]->psFix->siTyp]); siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev,apLst[m]); fprintf(pfDoc," - %s`\n",apLst[m]->psFix->pcHlp);
               for (psSel=apLst[m]->psDep;psSel!=NULL;psSel=psSel->psNxt) {
                  fprintf(pfDoc," ** `%s - %s`\n",psSel->psStd->pcKyw,psSel->psFix->pcHlp);
               }
            }
            fprintf(pfDoc,"\n");
         }
         if (siMan) {
            for (k=m=0;m<siMan;m++) {
               psHdl->apPat[siLev]=apMan[m];
               switch (apMan[m]->psFix->siTyp){
               case CLPTYP_OBJECT:strcpy(acArg,"OBJECT");break;
               case CLPTYP_OVRLAY:strcpy(acArg,"OVERLAY");break;
               default           :strcpy(acArg,"PARAMETER");break;
               }
               sprintf(acNum,"%s%d.",pcNum,k+1);
               if (isNbr) {
                  fprintf(pfDoc,"%s %s \'%s\'\n",acNum,acArg,apMan[m]->psStd->pcKyw);
                  l=strlen(acNum)+strlen(acArg)+strlen(apMan[m]->psStd->pcKyw)+4;
                  for (i=0;i<l;i++) fprintf(pfDoc,"^"); fprintf(pfDoc,"\n\n");
               } else {
                  fprintf(pfDoc,"%s \'%s\'\n",acArg,apMan[m]->psStd->pcKyw);
                  l=strlen(acArg)+strlen(apMan[m]->psStd->pcKyw)+3;
                  for (i=0;i<l;i++) fprintf(pfDoc,"^"); fprintf(pfDoc,"\n\n");
               }
               fprintf(pfDoc,".SYNOPSIS\n\n");
               fprintf(pfDoc,"-----------------------------------------------------------------------\n");
               fprintf(pfDoc,"HELP:   %s\n",apMan[m]->psFix->pcHlp);
               fprintf(pfDoc,"PATH:   %s\n",fpcPat(pvHdl,siLev));
               fprintf(pfDoc,"TYPE:   %s\n",apClpTyp[apMan[m]->psFix->siTyp]);
               fprintf(pfDoc,"SYNTAX: "); siErr=siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev,apMan[m]); fprintf(pfDoc,"\n");
               fprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
               fprintf(pfDoc,".DESCRIPTION\n\n");
               if (apMan[m]->psFix->pcMan!=NULL && strlen(apMan[m]->psFix->pcMan)) {
                  fprintf(pfDoc,"%s\n\n",apMan[m]->psFix->pcMan);
               } else {
                  fprintf(pfDoc,"No detailed description available for this argument.\n\n");
               }
               fprintf(pfDoc,"indexterm:[Argument %s]\n\n\n",apMan[m]->psStd->pcKyw);
               if (apMan[m]->psDep!=NULL) {
                  siErr=siClpPrnDoc(pvHdl,pfDoc,siLev+1,isNbr,acNum,apMan[m],apMan[m]->psDep);
                  if (siErr<0) return(siErr);
               }
               k++;
            }
         }
      }
   }
   return (CLP_OK);
}

static int siClpPrnPro(
   void*                         pvHdl,
   FILE*                         pfOut,
   int                           isMan,
   const int                     isSet,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psTab,
   const char*                   pcArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp;
   int                           siErr;

   if (psTab==NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Argument table not defined%s","");
   }

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry \'%s.%s\' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
   }

   if (siLev<siDep || siDep>9) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISS_ARG(psHlp->psStd->uiFlg) && CLPISS_PRO(psHlp->psStd->uiFlg) && (pcArg==NULL || strxcmp(psHdl->isCas,psHlp->psStd->pcKyw,pcArg,0,0,FALSE)==0)) {
            if (psHlp->psFix->pcDft!=NULL && strlen(psHlp->psFix->pcDft)) {
               if ((isMan || (!CLPISS_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && strlen(psHlp->psFix->pcMan)) {
                  fprintf(pfOut,"\n# DESCRIPTION for %s.%s.%s.%s:\n %s #\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcMan);
                  isMan=TRUE;
               } else {
                  if (isMan) fprintf(pfOut,"\n");
                  isMan=FALSE;
               }
               fprintf(pfOut,"%s.%s.%s.%s=\"%s\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcDft);
               fprintf(pfOut,"# TYPE: %s HELP: %s #\n",apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
            } else {
               if (isSet==FALSE) {
                  if ((isMan || (!CLPISS_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && strlen(psHlp->psFix->pcMan)) {
                     fprintf(pfOut,"\n# DESCRIPTION for %s.%s.%s.%s:\n %s #\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcMan);
                     isMan=TRUE;
                  } else {
                     if (isMan) fprintf(pfOut,"\n");
                     isMan=FALSE;
                  }
                  fprintf(pfOut,"%s.%s.%s.%s=\"\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw);
                  fprintf(pfOut,"# TYPE: %s HELP: %s #\n",apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
               }
            }

            if (psHlp->psDep!=NULL) {
               if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnPro(pvHdl,pfOut,isMan,isSet,siLev+1,siDep,psHlp->psDep,NULL);
                  if (siErr<0) return(siErr);
               }
            }
         }
      }
   }

   return (CLP_OK);
}

/**********************************************************************/

static char* fpcPre(
   void*                         pvHdl,
   const int                     siLev)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           i,j;
   psHdl->acPre[0]=EOS;
   for (j=i=0;i<siLev+1 && j<CLPMAX_PRELEN-strlen(psHdl->pcDep);i++) {
      strcat(psHdl->acPre,psHdl->pcDep);
   }
   return(psHdl->acPre);
}

static char* fpcPat(
   void*                         pvHdl,
   const int                     siLev)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           i;
   if (psHdl->pcCmd!=NULL) strcpy(psHdl->acPat,psHdl->pcCmd); else psHdl->acPat[0]=EOS;
   for (i=0;i<(siLev);i++) {
      if (strlen(psHdl->acPat)+1+strlen(psHdl->apPat[i]->psStd->pcKyw)<CLPMAX_PATLEN) {
         strcat(psHdl->acPat,"."); strcat(psHdl->acPat,psHdl->apPat[i]->psStd->pcKyw);
      }
   }
   return(psHdl->acPat);
}

/**********************************************************************/
