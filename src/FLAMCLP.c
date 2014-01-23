/**
 * @file   CLP.c
 * @brief  LIMES Command Line Parser in ANSI-C
 *
 * LIMES Command Line Executor (CLE) in ANSI-C
 * @author FALK REICHBOTT
 * @date  22.01.2014
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
 ******************************************************************************/
/*
 * TASK:0000086 Fehlermeldungen ueberarbeiten (Zentralisierung)
 * TASK:0000086 Mehrsprachigkeit einbauen
 */

/* Standard-Includes **********************************************************/

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>

/* Include der Schnittstelle **************************************************/

#include "FLAMCLP.h"

/* Definition der Version von FL-CLP ******************************************/

#define CLP_VSN_STR       "1.0.1.6"
#define CLP_VSN_MAJOR      1
#define CLP_VSN_MINOR        0
#define CLP_VSN_REVISION       1
//#define CLP_VSN_SUBREVIS       1 /*Adjust version and about*/
//#define CLP_VSN_SUBREVIS       2 /*Change escape sequence for strings and supplements to two times the same character (''/"")*/
//#define CLP_VSN_SUBREVIS       3 /*Support of command line or property file only parameter*/
//#define CLP_VSN_SUBREVIS       4 /*Support of dummy (DMY) flag for parameter which are not visible on command line and property file*/
//#define CLP_VSN_SUBREVIS       5 /*Support the use of different symbol tables for property and command line parsing*/
#define CLP_VSN_SUBREVIS         6 /*Add pcClpError to provide a error message for an error code*/



/* Definition der Flag-Makros *************************************************/

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
#define CLPISS_LNK(flg)          (CLPISS_CNT(flg) ||  CLPISS_OID(flg) ||  CLPISS_ELN(flg) || CLPISS_SLN(flg) ||  CLPISS_TLN(flg))
#define CLPISS_ARG(flg)          ((!CLPISS_LNK(flg)) && (!CLPISS_CON(flg)) && (!CLPISS_ALI(flg)))

/* Definition der Konstanten **************************************************/

#define CLPMAX_TABCNT            128
#define CLPMAX_HDEPTH            128
#define CLPMAX_LEXLEN            1023
#define CLPMAX_LEXSIZ            1024
#define CLPMAX_PRELEN            1023
#define CLPMAX_PRESIZ            1024
#define CLPMAX_PATLEN            1023
#define CLPMAX_PATSIZ            1024
#define CLPMAX_LSTLEN            65535
#define CLPMAX_LSTSIZ            65536

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

/* Definition der Strukturen *************************************************/

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
   unsigned int                  uiFlg;
   int                           siMkl;
   int                           isOvl;
   int                           isChk;
   int                           isCas;
   int                           siTok;
   char                          acLex[CLPMAX_LEXSIZ];
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
} TsHdl;

/* Deklaration der internen Funktionen ***************************************/

static U32 chr2asc(
   const C08*                    chr,
         C08*                    asc,
   const U32                     len);

static U32 chr2ebc(
   const C08*                    chr,
         C08*                    asc,
   const U32                     len);

static U32 hex2bin(
   const C08*                    hex,
         U08*                    bin,
   const U32                     len);

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
   char*                         pcLex);

static int siClpScnSrc(
   void*                         pvHdl);

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
   const char*                   pcDft);

static void vdClpPrnArgTab(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siLev,
   const int                     siTyp,
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
   const int                     siLev,
   const int                     siDep,
   const int                     siTyp,
   const TsSym*                  psArg);

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
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psTab);

static char* fpcPre(
   void*                         pvHdl,
   const int                     siLev);

static char* fpcPat(
   void*                         pvHdl,
   const int                     siLev);

/* Implementierung der externen Funktionen ***********************************/
#define VSNLENGTHMAX   128
#define VSNLENGTHMIN   0
#define ABOLENGTHMAX   512
#define ABOLENGTHMIN   0

extern const char* pcClpVersion(const int l)
{
   static C08                 acVsn[VSNLENGTHMAX];
   sprintf(acVsn,"%2.2d FLAM-CLP VERSION: %s BUILD: %s %s %s\n",l,CLP_VSN_STR,__BUILD__,__DATE__,__TIME__);
   if (strlen(acVsn)>=VSNLENGTHMAX || strlen(acVsn)<VSNLENGTHMIN) {
      fprintf(stderr,"\n*** Static area (%d) for version string (%d) to small or to big ***\n\n%s\n\n",(int)sizeof(acVsn),(int)strlen(acVsn),acVsn);
      exit(-1);
   }
   return(acVsn);
}

extern const char* pcClpAbout(const int l)
{
   static char                acAbo[ABOLENGTHMAX];
   sprintf(acAbo,
   "%2.2d Frankenstein Limes Command Line Parser (FLAM-CLP)\n"
   "   Version: %s Build: %s %s %s\n"
   "   Copyright (C) limes datentechnik (R) gmbh\n"
   "   This library is open source from the FLAM(R) project: http://www.flam.de\n"
   "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
   ,l,CLP_VSN_STR,__BUILD__,__DATE__,__TIME__);
   if (strlen(acAbo)>=ABOLENGTHMAX || strlen(acAbo)<ABOLENGTHMIN) {
      fprintf(stderr,"\n*** Static area (%d) for about message (%d) to small or to big ***\n\n%s\n\n",(int)sizeof(acAbo),(int)strlen(acAbo),acAbo);
      exit(-1);
   }
   return(acAbo);
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
   const int                     siMkl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const int                     isOvl,
   const unsigned int            uiFlg,
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
   const char*                   pcEnt)
{
   TsHdl*                        psHdl=NULL;
   int                           siErr;
   if (pcOwn!=NULL && pcPgm!=NULL && pcCmd!=NULL && psTab!=NULL && pvDat!=NULL) {
      psHdl=(TsHdl*)calloc(1,sizeof(TsHdl));
      if (psHdl!=NULL) {
         psHdl->isCas=isCas;
         psHdl->siMkl=siMkl;
         psHdl->pcOwn=pcOwn;
         psHdl->pcPgm=pcPgm;
         psHdl->pcCmd=pcCmd;
         psHdl->pcMan=pcMan;
         psHdl->pcHlp=pcHlp;
         psHdl->isOvl=isOvl;
         psHdl->uiFlg=uiFlg;
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
      }
   }
   return((void*)psHdl);
}

extern int siClpParsePro(
   void*                         pvHdl,
   const char*                   pcPro,
   const int                     isChk,
   char**                        ppPos,
   char**                        ppLst)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siCnt;

   psHdl->acLst[0]=EOS;
   psHdl->pcSrc=pcPro;
   psHdl->pcCur=pcPro;
   psHdl->pcOld=pcPro;
   psHdl->isChk=isChk;
   if (ppPos!=NULL) *ppPos=(char*)psHdl->pcSrc;
   if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
   psHdl->acLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PROPERTY-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) {
         if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
         if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
         return(psHdl->siTok);
      }
      siCnt=siClpPrsProLst(pvHdl,psHdl->psSym);
      if (siCnt<0) {
         if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
         if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
         return(siCnt);
      }
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcSrc=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->acLex[0]=EOS;
         psHdl->isChk=FALSE;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PROPERTY-PARSER-END(CNT=%d)\n",siCnt);
         return(siCnt);
      } else {
         if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
         if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
            fprintf(psHdl->pfErr,"%s Last token (%s) of property list is not EOS\n",fpcPre(pvHdl,0),apClpTok[psHdl->siTok]);
         }
         return(CLPERR_SYN);
      }
   } else {
      if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
      if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Initial token (%s) in handle is not valid\n",fpcPre(pvHdl,0),apClpTok[psHdl->siTok]);
      }
      return(CLPERR_SYN);
   }
}

extern int siClpParseCmd(
   void*                         pvHdl,
   const char*                   pcCmd,
   const int                     isChk,
   int*                          piOid,
   char**                        ppPos,
   char**                        ppLst)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siCnt;

   psHdl->acLst[0]=EOS;
   psHdl->pcSrc=pcCmd;
   psHdl->pcCur=pcCmd;
   psHdl->pcOld=pcCmd;
   psHdl->isChk=isChk;
   if (ppPos!=NULL) *ppPos=(char*)psHdl->pcSrc;
   if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
   psHdl->acLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"COMMAND-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) {
         if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
         if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
         return(psHdl->siTok);
      }
      siCnt=siClpPrsMain(pvHdl,psHdl->psSym,piOid);
      if (siCnt<0) {
         if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
         if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
         return (siCnt);
      }
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcSrc=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->acLex[0]=EOS;
         psHdl->isChk=FALSE;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"COMMAND-PARSER-END(CNT=%d)\n",siCnt);
         return(siCnt);
      } else {
         if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
         if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
            fprintf(psHdl->pfErr,"%s Last token (%s) of parameter list is not EOS\n",fpcPre(pvHdl,0),apClpTok[psHdl->siTok]);
         }
         return(CLPERR_SYN);
      }
   } else {
      if (ppPos!=NULL) *ppPos=(char*)psHdl->pcOld;
      if (ppLst!=NULL) *ppLst=(char*)psHdl->acLst;
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Initial token (%s) in handle is not valid\n",fpcPre(pvHdl,0),apClpTok[psHdl->siTok]);
      }
      return(CLPERR_SYN);
   }
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
   if (pcPat!=NULL) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Path (%s) is not valid\n",fpcPre(pvHdl,0),pcPat);
            }
            return(CLPERR_SEM);
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
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s End of path reached, no parameter table anymore\n",fpcPre(pvHdl,0));
            }
            return(CLPERR_SEM);
         }
      } else {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Root of path (%s) does not match root of handle (%s)\n",fpcPre(pvHdl,0),pcPat,psHdl->pcCmd);
         }
         return(CLPERR_SEM);
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

   if (pcPat!=NULL) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Path (%s) is not valid\n",fpcPre(pvHdl,0),pcPat);
            }
            return(CLPERR_SEM);
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
               siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,siLev,siLev+siDep,-1,psTab);
               if (siErr<0) return(siErr);
            }
         } else {
            if (psArg->psFix->pcMan!=NULL && strlen(psArg->psFix->pcMan)) {
               fprintf(psHdl->pfHlp,"%s\n",psArg->psFix->pcMan);
            } else {
               fprintf(psHdl->pfHlp,"No detailed description available for this argument.\n");
            }
         }
      } else {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Root of path (%s) does not match root of handle (%s)\n",fpcPre(pvHdl,0),pcPat,psHdl->pcCmd);
         }
         return(CLPERR_SEM);
      }
   } else {
      if (siDep==0) {
         if (psHdl->pcMan!=NULL && strlen(psHdl->pcMan)) {
            fprintf(psHdl->pfHlp,"%s\n",psHdl->pcMan);
         } else {
            fprintf(psHdl->pfHlp,"No detailed description available for this command.\n");
         }
      } else {
         siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,0,siDep,-1,psTab);
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

   if (pcNum!=NULL && strlen(pcNum)<100) {
      if (pcPat!=NULL) {
         if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0)==0) {
            if (strlen(pcPat)>l && pcPat[l]!='.') {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Path (%s) is not valid\n",fpcPre(pvHdl,0),pcPat);
               }
               return(CLPERR_SEM);
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
                        fprintf(pfDoc,"%s(3)\n",fpcPat(pvHdl,siLev));
                        l=strlen(fpcPat(pvHdl,siLev))+3;
                        for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
                        fprintf(pfDoc,":doctype: manpage\n\n");
                        fprintf(pfDoc,"NAME\n");
                        fprintf(pfDoc,"----\n\n");
                        vdClpPrnAli(pfDoc,", ",psArg); fprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
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
                        fprintf(pfDoc,"%s(3)\n",fpcPat(pvHdl,siLev));
                        l=strlen(fpcPat(pvHdl,siLev))+3;
                        for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
                        fprintf(pfDoc,":doctype: manpage\n\n");
                        fprintf(pfDoc,"NAME\n");
                        fprintf(pfDoc,"----\n\n");
                        fprintf(pfDoc,"%s - `%s`\n\n",psArg->psStd->pcKyw,psArg->psFix->pcHlp);
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
                     if (psHdl->pfErr!=NULL) {
                        fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                        fprintf(psHdl->pfErr,"%s End of path reached, no valid argument or constant\n",fpcPre(pvHdl,0));
                     }
                     return(CLPERR_SEM);
                  }
               } else {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                     fprintf(psHdl->pfErr,"%s End of path reached, no parameter table anymore\n",fpcPre(pvHdl,0));
                  }
                  return(CLPERR_SEM);
               }
               return(CLP_OK);
            }
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Root of path (%s) does not match root of handle (%s)\n",fpcPre(pvHdl,0),pcPat,psHdl->pcCmd);
            }
            return(CLPERR_SEM);
         }
      }
      if (isMan) {
         fprintf(pfDoc,   "%s.%s(1)\n",psHdl->pcPgm,psHdl->pcCmd);
         l=strlen(psHdl->pcPgm)+strlen(psHdl->pcCmd)+4;
         for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
         fprintf(pfDoc,   ":doctype: manpage\n\n");
         fprintf(pfDoc,   "NAME\n");
         fprintf(pfDoc,   "----\n\n");
         fprintf(pfDoc,   "%s - `%s`\n\n",psHdl->pcCmd,psHdl->pcHlp);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s No valid initial number string for head lines\n",fpcPre(pvHdl,0));
      }
      return(CLPERR_INT);
   }
   return(CLP_OK);
}

extern int siClpProperties(
   void*                         pvHdl,
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

   if (pfOut==NULL) pfOut=psHdl->pfHlp;

   if (pcPat!=NULL) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Path (%s) is not valid\n",fpcPre(pvHdl,0),pcPat);
            }
            return(CLPERR_SEM);
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
            siErr=siClpPrnPro(pvHdl,pfOut,FALSE,siLev,siLev+siDep,psTab);
            if (siErr<0) return(siErr);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s End of path reached, no parameter table anymore\n",fpcPre(pvHdl,0));
            }
            return(CLPERR_SEM);
         }
      } else {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Root of path (%s) does not match root of handle (%s)\n",fpcPre(pvHdl,0),pcPat,psHdl->pcCmd);
         }
         return(CLPERR_SEM);
      }
   } else {
      siErr=siClpPrnPro(pvHdl,pfOut,FALSE,0,siDep,psTab);
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

/* Interne Funktionen *********************************************************/

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
   int                           k;

   psSym=(TsSym*)calloc(1,sizeof(TsSym));
   if (psSym==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"MEMORY-ERROR\n");
         fprintf(psHdl->pfErr,"%s Allocation of memory for symbol \'%s.%s\' failed\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->pcKyw);
      }
      ERROR(psSym);
   }
   psSym->psStd=(TsStd*)calloc(1,sizeof(TsStd));
   psSym->psFix=(TsFix*)calloc(1,sizeof(TsFix));
   psSym->psVar=(TsVar*)calloc(1,sizeof(TsVar));
   if (psSym->psStd==NULL || psSym->psFix==NULL || psSym->psVar==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"MEMORY-ERROR\n");
         fprintf(psHdl->pfErr,"%s Allocation of memory for symbol element \'%s.%s\' failed\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->pcKyw);
      }
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
   psSym->psFix->pcDft=psArg->pcDft;
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Type (%d) for argument \'%s.%s\' not supported\n",fpcPre(pvHdl,0),psSym->psFix->siTyp,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
      }
      ERROR(psSym);
   }

   if (psSym->psStd->pcAli!=NULL) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Keyword of alias (%s.%s) is not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcAli);
         }
         ERROR(psSym);
      }
      if (strxcmp(psHdl->isCas,psSym->psStd->pcKyw,psSym->psStd->pcAli,0,0)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Keyword and alias (%s.%s) are equal\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcAli);
         }
         ERROR(psSym);
      }
      for (k=0,psHlp=psCur;psHlp!=NULL;psHlp=psHlp->psBak) {
         if (CLPISS_ARG(psHlp->psStd->uiFlg) && strxcmp(psHdl->isCas,psSym->psStd->pcAli,psHlp->psStd->pcKyw,0,0)==0) {
            if (k==0) {
               psSym->psStd->psAli=psHlp;
               psSym->psStd->uiFlg=psHlp->psStd->uiFlg|CLPFLG_ALI;
               free(psSym->psFix); psSym->psFix=psHlp->psFix;
               free(psSym->psVar); psSym->psVar=psHlp->psVar;
            } else {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"TABLE-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Alias for keyword \'%s.%s\' is not unique\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
               }
               ERROR(psSym);
            }
            k++;
         }
      }
      if (k==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Alias \'%s\' for keyword \'%s.%s\' can not be resolved\n",fpcPre(pvHdl,0),psSym->psStd->pcAli,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
   } else if (CLPISS_ARG(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Keyword for argument (%s.?) is not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev));
         }
         ERROR(psSym);
      }
      if (psSym->psFix->siMax<1 || psSym->psFix->siMax<psSym->psFix->siMin) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Maximal amount for argument \'%s.%s\' is too small\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->siSiz<1) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Size for argument \'%s.%s\' is smaller than 1\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->pcHlp==NULL || strlen(psSym->psFix->pcHlp)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Help for argument \'%s.%s\' not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
   } else if (CLPISS_LNK(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Keyword of a link (%s.?) is not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev));
         }
         ERROR(psSym);
      }
      if (psSym->psStd->pcAli!=NULL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Alias (%s) for link \'%s.%s\' defined\n",fpcPre(pvHdl,0),psSym->psStd->pcAli,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->pcDft!=NULL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Default (%s) for link \'%s.%s\' defined\n",fpcPre(pvHdl,0),psSym->psFix->pcDft,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->siTyp!=CLPTYP_NUMBER) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Type for link \'%s.%s\' is not a number\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->siMax<1 || psSym->psFix->siMax<psSym->psFix->siMin) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Maximal amount for link \'%s.%s\' is too small\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->siSiz<1) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Size for link \'%s.%s\' is smaller than 1\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (CLPISS_SEL(psSym->psStd->uiFlg) || CLPISS_CON(psSym->psStd->uiFlg)) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Flag SEL or CON set for link \'%s.%s\'\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
   } else if (CLPISS_CON(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || strlen(psSym->psStd->pcKyw)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Key word for a constant (%s.?) is not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev));
         }
         ERROR(psSym);
      }
      if (psSym->psStd->pcAli!=NULL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Alias (%s) for constant \'%s.%s\' defined\n",fpcPre(pvHdl,0),psSym->psStd->pcAli,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->pcDft!=NULL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Default (%s) for link \'%s.%s\' defined\n",fpcPre(pvHdl,0),psSym->psFix->pcDft,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psArg->psTab!=NULL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Table for constant \'%s.%s\' defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->pcKyw);
         }
         ERROR(psSym);
      }
      if (CLPISS_SEL(psSym->psStd->uiFlg) || CLPISS_LNK(psSym->psStd->uiFlg)  || CLPISS_ALI(psSym->psStd->uiFlg)) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Flags SEL, LNK or ALI set for constant \'%s.%s\'\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      if (psSym->psFix->pcHlp==NULL || strlen(psSym->psFix->pcHlp)==0) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Help for constant \'%s.%s\' not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
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
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Type \'%s\' for constant \'%s.%s\' requires a value (pcVal==NULL)\n",fpcPre(pvHdl,0),apClpTyp[psSym->psFix->siTyp],fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            ERROR(psSym);
         }
         if (!CLPISS_BIN(psSym->psStd->uiFlg) && psSym->psFix->siSiz==0) {
            psSym->psFix->siSiz=strlen((char*)psArg->pcVal)+1;
         }
         psSym->psVar->pvDat=(void*)psArg->pcVal;
         break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Type (%s) for argument \'%s.%s\' not supported for constant definitions\n",fpcPre(pvHdl,0),apClpTyp[psSym->psFix->siTyp],fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         ERROR(psSym);
      }
      psSym->psVar->pvPtr=NULL;
      psSym->psVar->siLen=psSym->psFix->siSiz;
      psSym->psVar->siCnt=1;
      psSym->psVar->siRst=0;
   } else {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Kind (ALI/ARG/LNK/CON) of argument \'%s.%s\' not determinably\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
      }
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
   int                           i,j,isIns;

   if (psTab==NULL) {
      fprintf(psHdl->pfErr,"TABLE-ERROR\n");
      if (psArg==NULL) {
         if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Argument table not defined\n",fpcPre(pvHdl,0));
      } else {
         if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Parameter table of argument \'%s.%s\' not defined\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->pcKyw);
      }
      return(CLPERR_TAB);
   }

   for (j=i=0;psTab[i].siTyp;i++) {
      if (i>=CLPMAX_TABCNT) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         if (psArg==NULL) {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Argument table bigger than maximal supported entry count (%d)\n",fpcPre(pvHdl,0),CLPMAX_TABCNT);
         } else {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Table for key word (%s.%s) bigger than maximal supported entry count (%d)\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->pcKyw,CLPMAX_TABCNT);
         }
         return(CLPERR_TAB);
      }

      if (psTab[i].pcKyw==NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         if (psArg==NULL) {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s There is no keyword defined in argument table at index %d\n",fpcPre(pvHdl,0),i);
         } else {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Table for key word (%s.%s) has no keyword defined at index %d\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->pcKyw,i);
         }
         return(CLPERR_TAB);
      }

      isIns=TRUE;
      if (psHdl->uiFlg && (CLPISS_PRO(psTab[i].uiFlg) || CLPISS_CMD(psTab[i].uiFlg) || CLPISS_DMY(psTab[i].uiFlg))) {
         isIns=FALSE;
         if (CLPISS_PRO(psHdl->uiFlg) && CLPISS_PRO(psTab[i].uiFlg)) isIns=TRUE;
         if (CLPISS_CMD(psHdl->uiFlg) && CLPISS_CMD(psTab[i].uiFlg)) isIns=TRUE;
      }

      if (isIns) {
         psCur=psClpSymIns(pvHdl,siLev,i,&psTab[i],psHih,psCur);
         if (psCur==NULL) return(CLPERR_MEM);
         if (j==0) *ppFst=psCur;

         switch (psTab[i].siTyp) {
         case CLPTYP_SWITCH:
            if (psTab[i].psTab!=NULL) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"TABLE-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Parameter table of argument \'%s.%s\' is defined (NULL for psTab required)\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psTab[i].pcKyw);
               }
               return(CLPERR_TAB);
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
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TYPE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Type (%d) of parameter \'%s.%s\' not supported\n",fpcPre(pvHdl,0),psTab[i].siTyp,fpcPat(pvHdl,siLev),psTab[i].pcKyw);
            }
            return(CLPERR_TYP);
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
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
            if (psArg==NULL) {
               if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Argument table not in sync with symbol table\n",fpcPre(pvHdl,0));
            } else {
               if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Parameter table of argument \'%s.%s\' not in sync with symbol table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
         }
         return(CLPERR_INT);
      }

      if (CLPISS_ALI(psSym->psStd->uiFlg)) {
         isPar=TRUE;
      } else if (CLPISS_ARG(psSym->psStd->uiFlg)) {
         isPar=TRUE;
      } else if (CLPISS_LNK(psSym->psStd->uiFlg)) {
         isPar=TRUE;
         for (h=k=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if (CLPISS_ARG(psHlp->psStd->uiFlg) && strxcmp(psHdl->isCas,psSym->psStd->pcKyw,psHlp->psStd->pcKyw,0,0)==0) {
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
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Link for keyword \'%s.%s\' can not be resolved\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            return(CLPERR_TAB);
         }
         if (h>1) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Link for keyword \'%s.%s\' is not unique\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            return(CLPERR_TAB);
         }
         if (k>1) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s More then one link defined for keyword \'%s.%s\'\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            return(CLPERR_TAB);
         }
         if (k==0) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Link for keyword \'%s.%s\' was not assigned\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            return(CLPERR_TAB);
         }
      } else if (CLPISS_CON(psSym->psStd->uiFlg)) {
         isCon=TRUE; siCon++;
      } else {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Kind (ALI/ARG/LNK/CON) of argument \'%s.%s\' not determinable\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         return(CLPERR_TAB);
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
                     if (psHdl->pfErr!=NULL) {
                        fprintf(psHdl->pfErr,"TABLE-ERROR\n");
                        fprintf(psHdl->pfErr,"%s Key word \'%s.%s\' is not unique\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
                     }
                     return(CLPERR_TAB);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         if (psArg==NULL) {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Argument table is not consistent (mix of constants and parameter)\n",fpcPre(pvHdl,0));
         } else {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Parameter table of argument \'%s.%s\' is not consistent (mix of constants and parameter)\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }
      return(CLPERR_TAB);
   }
   if (isCon==FALSE && isPar==FALSE) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         if (psArg==NULL) {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Argument table neither contains constants nor arguments\n",fpcPre(pvHdl,0));
         } else {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Parameter table of argument \'%s.%s\' neither contains constants nor arguments\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }
      return(CLPERR_TAB);
   }
   for (psSym=psTab;psSym!=NULL;psSym=psSym->psNxt) {
      if (!CLPISS_LNK(psSym->psStd->uiFlg)) {
         if (psSym->psStd->siKwl<=0) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Required keyword length (%d) of argument \'%s.%s\' is smaller or equal to zero\n",fpcPre(pvHdl,0),psSym->psStd->siKwl,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            return(CLPERR_TAB);
         }
         if (psSym->psStd->siKwl>strlen(psSym->psStd->pcKyw)) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"TABLE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Required keyword length (%d) of argument \'%s.%s\' is greater then keyword length\n",fpcPre(pvHdl,0),psSym->psStd->siKwl,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
            }
            return(CLPERR_TAB);
         }
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword \'%s.%s\' not valid in this case\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),pcKyw);
         fprintf(psHdl->pfErr,"%s You might want to specify a string (\'%s\') instead of this keyword (%s)\n",fpcPre(pvHdl,0),pcKyw,pcKyw);
         fprintf(psHdl->pfErr,"%s Or end of path achieved\n",fpcPre(pvHdl,0));
      }
      return(CLPERR_SYN);
   }
   if (psTab->psBak!=NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
         fprintf(psHdl->pfErr,"%s Try to find keyword \'%s\' in this table\n",fpcPre(pvHdl,0),pcKyw);
      }
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
   if (psHdl->pfErr!=NULL) {
      fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
      fprintf(psHdl->pfErr,"%s Parameter \'%s.%s\' is not valid\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),pcKyw);
      fprintf(psHdl->pfErr,"%s Valid values:\n",fpcPre(pvHdl,0));
      vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,-1,psTab);
   }
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
      if (psHlp->psDep!=NULL) {
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

/* Scanner ********************************************************************/

/*now differenz between host and open world anymore */
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
   fprintf(pfOut,"%s SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s OPERATOR  '=' | '.' | '(' | ')' | '[' | ']'  (SGN, DOT, RBO, RBC, SBO, SBC)\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s KEYWORD   [:alpha:]+[:alnum: | '_' | '-']*              (always predefined)\n",fpcPre(pvHdl,0));
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
   fprintf(pfOut,"%s SUPPLEMENT     '\"' [:print:]* '\"' |   (zero terminated string (properties))\n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s           Supplements can contain two \"\" to represent one \"                \n",fpcPre(pvHdl,0));
   return(CLP_OK);
}

static int siClpScnNat(
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char**                  ppCur,
   char*                         pcLex)
{
   char*                         pcEnd=pcLex+CLPMAX_LEXLEN;
   char*                         pcHlp=pcLex;
   U64                           t;
   struct tm                     tm;
   struct tm                     *tmAkt;
   const char*                   pcCur=*ppCur;

   while (1) {
      if (*pcCur==EOS) { /*end*/
         pcLex[0]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(END)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_END);
      } else if (isspace(*pcCur) || iscntrl(*pcCur) || (*pcCur)==',') { /*separation*/
         pcCur++;
      } else if (*pcCur=='#') { /*comment*/
         pcCur++;
         while (*pcCur!='#' && *pcCur!=EOS) pcCur++;
         if (*pcCur!='#') {
            if (pfErr!=NULL) {
                fprintf(pfErr,"LEXICAL-ERROR\n");
                fprintf(pfErr,"%s Comment not terminated with \'#\'\n",fpcPre(pvHdl,0));
            }
            return(CLPERR_LEX);
         }
         pcCur++;
      } else if (pcCur[0]==SPMCHR) {/*supplement*/
         pcCur+=1;
         while (pcCur[0]!=EOS && (pcCur[0]!=SPMCHR || (pcCur[0]==SPMCHR && pcCur[1]==SPMCHR))  && pcLex<pcEnd) {
            *pcLex=*pcCur; pcLex++;
            pcCur+=(pcCur[0]==SPMCHR)?2:1;
         }
         *pcLex=EOS;
         if (*pcCur!=SPMCHR) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"LEXICAL-ERROR\n");
               fprintf(pfErr,"%s Supplement string not terminated with \'%c\'\n",fpcPre(pvHdl,0),SPMCHR);
            }
            return(CLPERR_LEX);
         }
         pcCur++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SUP)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_SUP);
      } else if (((tolower(pcCur[0])=='x' || tolower(pcCur[0])=='a' ||tolower(pcCur[0])=='e' ||
                   tolower(pcCur[0])=='c' || tolower(pcCur[0])=='s') && pcCur[1]==STRCHR) || pcCur[0]==STRCHR) {/*string*/
         if (pcCur[0]==STRCHR) {
            *pcLex='d'; pcLex++;
            *pcLex='\''; pcLex++;
            pcCur+=1;
         } else {
            *pcLex=tolower(*pcCur); pcLex++;
            *pcLex='\''; pcLex++;
            pcCur+=2;
         }
         while (pcCur[0]!=EOS && (pcCur[0]!=STRCHR || (pcCur[0]==STRCHR && pcCur[1]==STRCHR)) &&  pcLex<pcEnd) {
            *pcLex=*pcCur; pcLex++;
            pcCur+=(pcCur[0]==STRCHR)?2:1;
         }
         *pcLex=EOS;
         if (*pcCur!=STRCHR) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"LEXICAL-ERROR\n");
               fprintf(pfErr,"%s String literal not terminated with \'%c\'\n",fpcPre(pvHdl,0),STRCHR);
            }
            return(CLPERR_LEX);
         }
         pcCur++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_STR);
      } else if (isalpha(*pcCur)) { /*keyword*/
         *pcLex=*pcCur;
         pcCur++; pcLex++;
         while ((isalnum(*pcCur) || *pcCur=='_' || *pcCur=='-') && pcLex<pcEnd) {
            *pcLex=*pcCur;
            pcCur++; pcLex++;
         }
         *pcLex=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(KYW)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_KYW);
      } else if (((pcCur[0]=='+' || pcCur[0]=='-') && isdigit(pcCur[1])) || isdigit(pcCur[0])) { /*number*/
         if (pcCur[0]=='+' || pcCur[0]=='-') {
            pcLex[1]=pcCur[0]; pcCur++;
         } else pcLex[1]=' ';
         if ((pcCur[0]=='0') &&
             (tolower(pcCur[1])=='b' || tolower(pcCur[1])=='o' || tolower(pcCur[1])=='d' || tolower(pcCur[1])=='x' || tolower(pcCur[1])=='t') &&
             (isdigit(pcCur[2]) || (isxdigit(pcCur[2]) && tolower(pcCur[1])=='x'))) {
            pcLex[0]=tolower(pcCur[1]); pcCur+=2;
         } else pcLex[0]='d';
         pcLex+=2;
         if (pcHlp[0]=='x') {
            while (isxdigit(*pcCur) && pcLex<pcEnd) {
               *pcLex=*pcCur;
               pcCur++; pcLex++;
            }
         } else if (pcHlp[0]=='t') {
            memset(&tm,0,sizeof(tm));
            while (isdigit(*pcCur) && pcLex<pcEnd) {
               *pcLex=*pcCur;
               pcCur++; pcLex++;
            }
            *pcLex=EOS;
            if (pcCur[0]=='/' && isdigit(pcCur[1])) {
               tm.tm_year=strtol(pcHlp+2,NULL,10);
               pcLex=pcHlp+2; pcCur++;
               while (isdigit(*pcCur) && pcLex<pcEnd) {
                  *pcLex=*pcCur;
                  pcCur++; pcLex++;
               }
               *pcLex=EOS;
               if (pcCur[0]=='/' && isdigit(pcCur[1])) {
                  tm.tm_mon=strtol(pcHlp+2,NULL,10);
                  pcLex=pcHlp+2; pcCur++;
                  while (isdigit(*pcCur) && pcLex<pcEnd) {
                     *pcLex=*pcCur;
                     pcCur++; pcLex++;
                  }
                  *pcLex=EOS;
                  if (pcCur[0]=='.' && isdigit(pcCur[1])) {
                     tm.tm_mday=strtol(pcHlp+2,NULL,10);
                     pcLex=pcHlp+2; pcCur++;
                     while (isdigit(*pcCur) && pcLex<pcEnd) {
                        *pcLex=*pcCur;
                        pcCur++; pcLex++;
                     }
                     *pcLex=EOS;
                     if (pcCur[0]==':' && isdigit(pcCur[1])) {
                        tm.tm_hour=strtol(pcHlp+2,NULL,10);
                        pcLex=pcHlp+2; pcCur++;
                        while (isdigit(*pcCur) && pcLex<pcEnd) {
                           *pcLex=*pcCur;
                           pcCur++; pcLex++;
                        }
                        *pcLex=EOS;
                        if (pcCur[0]==':' && isdigit(pcCur[1])) {
                           tm.tm_min=strtol(pcHlp+2,NULL,10);
                           pcLex=pcHlp+2; pcCur++;
                           while (isdigit(*pcCur) && pcLex<pcEnd) {
                              *pcLex=*pcCur;
                              pcCur++; pcLex++;
                           }
                           *pcLex=EOS;
                           tm.tm_sec=strtol(pcHlp+2,NULL,10);
                           if (pcHlp[1]=='+') {
                              t=time(NULL);
                              if (t==-1) {
                                 if (pfErr!=NULL) {
                                    fprintf(pfErr,"SYSTEM-ERROR\n");
                                    fprintf(pfErr,"%s Determine the current time is not possible\n",fpcPre(pvHdl,0));
                                 }
                                 return(CLPERR_SYS);
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
                                 if (pfErr!=NULL) {
                                    fprintf(pfErr,"LEXICAL-ERROR\n");
                                    fprintf(pfErr,"%s The calculated time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) can not be converted to a number\n",fpcPre(pvHdl,0),
                                        tmAkt->tm_year+1900,tmAkt->tm_mon+1,tmAkt->tm_mday,tmAkt->tm_hour,tmAkt->tm_min,tmAkt->tm_sec);
                                 }
                                 return(CLPERR_LEX);
                              }
                           } else if (pcHlp[1]=='-') {
                              t=time(NULL);
                              if (t==-1) {
                                 if (pfErr!=NULL) {
                                    fprintf(pfErr,"SYSTEM-ERROR\n");
                                    fprintf(pfErr,"%s Determine the current time is not possible\n",fpcPre(pvHdl,0));
                                 }
                                 return(CLPERR_SYS);
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
                                 if (pfErr!=NULL) {
                                    fprintf(pfErr,"LEXICAL-ERROR\n");
                                    fprintf(pfErr,"%s The calculated time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) can not be converted to a number\n",fpcPre(pvHdl,0),
                                          tmAkt->tm_year+1900,tmAkt->tm_mon+1,tmAkt->tm_mday,tmAkt->tm_hour,tmAkt->tm_min,tmAkt->tm_sec);
                                 }
                                 return(CLPERR_LEX);
                              }
                           } else {
                              tm.tm_year-=1900;
                              tm.tm_mon-=1;
                              t=mktime(&tm);
                              if (t==-1) {
                                 if (pfErr!=NULL) {
                                    fprintf(pfErr,"LEXICAL-ERROR\n");
                                    fprintf(pfErr,"%s The given time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) can not be converted to a number\n",fpcPre(pvHdl,0),
                                            tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
                                 }
                                 return(CLPERR_LEX);
                              }
                           }
                           pcHlp[1]=' ';
                           sprintf(pcHlp+2,"%"PRIu64"",t);
                           if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(NUM)-LEXEM(%s)\n",pcHlp);
                           *ppCur=pcCur;
                           return(CLPTOK_NUM);
                        } else {
                           if (pfErr!=NULL) {
                              fprintf(pfErr,"LEXICAL-ERROR\n");
                              fprintf(pfErr,"%s Character \':\' expected to separate minute from second\n",fpcPre(pvHdl,0));
                           }
                           return(CLPERR_LEX);
                        }
                     } else {
                        if (pfErr!=NULL) {
                           fprintf(pfErr,"LEXICAL-ERROR\n");
                           fprintf(pfErr,"%s Character \':\' expected to separate hour from minute\n",fpcPre(pvHdl,0));
                        }
                        return(CLPERR_LEX);
                     }
                  } else {
                     if (pfErr!=NULL) {
                        fprintf(pfErr,"LEXICAL-ERROR\n");
                        fprintf(pfErr,"%s Character \'.\' expected to separate day from hour\n",fpcPre(pvHdl,0));
                     }
                     return(CLPERR_LEX);
                  }
               } else {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"LEXICAL-ERROR\n");
                     fprintf(pfErr,"%s Character \'/\' expected to separate month from day\n",fpcPre(pvHdl,0));
                  }
                  return(CLPERR_LEX);
               }
            } else {
               if (pfErr!=NULL) {
                  fprintf(pfErr,"LEXICAL-ERROR\n");
                  fprintf(pfErr,"%s Character \'/\' expected to separate year from month\n",fpcPre(pvHdl,0));
               }
               return(CLPERR_LEX);
            }
         } else {
            while (isdigit(*pcCur) && pcLex<pcEnd) {
               *pcLex=*pcCur;
               pcCur++; pcLex++;
            }
         }
         if (pcHlp[0]=='d' && pcCur[0]=='.' && (isdigit(pcCur[1]))) { /*float*/
            *pcLex=*pcCur;
            pcCur++; pcLex++;
            while (isdigit(*pcCur) && pcLex<pcEnd) {
               *pcLex=*pcCur;
               pcCur++; pcLex++;
            }
            if ((tolower(pcCur[0])=='e') && (isdigit(pcCur[1]) ||
                (pcCur[1]=='+' && isdigit(pcCur[2])) ||
                (pcCur[1]=='-' && isdigit(pcCur[2])))) { /*float*/
               *pcLex=*pcCur;
               pcCur++; pcLex++;
               if (!isdigit(pcCur[0])) {
                  *pcLex=*pcCur;
                  pcCur++; pcLex++;
               }
               while (isdigit(*pcCur) && pcLex<pcEnd) {
                  *pcLex=*pcCur;
                  pcCur++; pcLex++;
               }
               *pcLex=EOS;
               if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",pcHlp);
               *ppCur=pcCur;
               return(CLPTOK_FLT);
            }
            *pcLex=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",pcHlp);
            *ppCur=pcCur;
            return(CLPTOK_FLT);
         } else if ((tolower(pcCur[0])=='e') && (isdigit(pcCur[1]) ||
                    (pcCur[1]=='+' && isdigit(pcCur[2])) ||
                    (pcCur[1]=='-' && isdigit(pcCur[2])))) { /*float*/
            *pcLex=*pcCur;
            pcCur++; pcLex++;
            if (!isdigit(pcCur[0])) {
               *pcLex=*pcCur;
               pcCur++; pcLex++;
            }
            while (isdigit(*pcCur) && pcLex<pcEnd) {
               *pcLex=*pcCur;
               pcCur++; pcLex++;
            }
            *pcLex=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",pcHlp);
            *ppCur=pcCur;
            return(CLPTOK_FLT);
         } else {
            *pcLex=EOS;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(NUM)-LEXEM(%s)\n",pcHlp);
            *ppCur=pcCur;
            return(CLPTOK_NUM);
         }
      } else if (*pcCur=='=') { /*sign*/
         pcCur++; pcLex[0]='='; pcLex[1]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SGN)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_SGN);
      } else if (*pcCur=='.') { /*dot*/
         pcCur++; pcLex[0]='.'; pcLex[1]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(DOT)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_DOT);
      } else if (*pcCur=='(') { /*round bracket open*/
         pcCur++;  pcLex[0]='('; pcLex[1]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(RBO)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_RBO);
      } else if (*pcCur==')') { /*round bracket close*/
         pcCur++;  pcLex[0]=')'; pcLex[1]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(RBC)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_RBC);
      } else if (*pcCur=='[') { /*squared bracket open*/
         pcCur++;  pcLex[0]='['; pcLex[1]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SBO)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_SBO);
      } else if (*pcCur==']') { /*squared bracket close*/
         pcCur++;  pcLex[0]=']'; pcLex[1]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SBC)-LEXEM(%s)\n",pcHlp);
         *ppCur=pcCur;
         return(CLPTOK_SBC);
      } else { /*lexical error*/
         pcLex[0]=EOS;
         if (pfErr!=NULL) {
            fprintf(pfErr,"LEXICAL-ERROR\n");
            fprintf(pfErr,"%s Character (\'%c\') not valid\n",fpcPre(pvHdl,0),*pcCur);
         }
         return(CLPERR_LEX);
      }
   }
}

static int siClpScnSrc(
   void*                         pvHdl)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   psHdl->pcOld=psHdl->pcCur;
   return(siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&psHdl->pcCur,psHdl->acLex));
}

/*****************************************************************************/

extern int siClpGrammar(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   fprintf(pfOut,"%s Command Line Parser                                              \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s command        -> ['('] parameter_list [')']       (main=object) \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  ['.'] parameter                  (main=overlay)\n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s parameter_list -> parameter SEP parameter_list                   \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s parameter      -> switch | assignment | object | overlay | array \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s switch         -> KEYWORD                                        \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s assignment     -> KEYWORD '=' value                              \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s object         -> KEYWORD '(' parameter_list ')'                 \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s overlay        -> KEYWORD '.' parameter                          \n",fpcPre(pvHdl,1));
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
   fprintf(pfOut,"%s Property File Parser                                             \n",fpcPre(pvHdl,0));
   fprintf(pfOut,"%s properties     -> property_list                                  \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s property_list  -> property SEP property_list                     \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  EMPTY                                          \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s property       -> keyword_list '=' SUPPLEMENT                    \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s keyword_list   -> KEYWORD '.' keyword_list                       \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s                |  KEYWORD                                        \n",fpcPre(pvHdl,1));
   fprintf(pfOut,"%s SUPPLEMENT is a string in double quotation marks (\"property\")  \n",fpcPre(pvHdl,1));
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
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_SGN) {
         return(siClpPrsSgn(pvHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_RBO) {
         return(siClpPrsObj(pvHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(pvHdl);
         if (psHdl->siTok<0) return(psHdl->siTok);
         return(siClpPrsOvl(pvHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_SBO) {
         return(siClpPrsAry(pvHdl,siLev,siPos,psArg));
      } else {
         return(siClpPrsSwt(pvHdl,siLev,siPos,psArg));
      }
   } else {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword expected (%s.?)\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev));
         fprintf(psHdl->pfErr,"%s Valid values:\n",fpcPre(pvHdl,0));
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,-1,psTab);
      }
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
   psHdl->siTok=siClpScnSrc(pvHdl);
   if (psHdl->siTok<0) return(psHdl->siTok);
   switch (psHdl->siTok) {
   case CLPTOK_NUM: return(siClpPrsVal(pvHdl,siLev,siPos,CLPTYP_NUMBER,psArg));
   case CLPTOK_FLT: return(siClpPrsVal(pvHdl,siLev,siPos,CLPTYP_FLOATN,psArg));
   case CLPTOK_STR: return(siClpPrsVal(pvHdl,siLev,siPos,CLPTYP_STRING,psArg));
   case CLPTOK_KYW: return(siClpPrsVal(pvHdl,siLev,siPos,psArg->psFix->siTyp,psArg));
   default:
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s After assignment \'%s.%s=\' number(-123), float(+123.45e78), string(\'abc\') or keyword (MODE) expected\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      return(CLPERR_SYN);
   }
}

static int siClpPrsObj(
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
   psHdl->siTok=siClpScnSrc(pvHdl);
   if (psHdl->siTok<0) return(psHdl->siTok);
   siErr=siClpIniObj(pvHdl,siLev,siPos,psArg,&psDep,asSav);
   if (siErr<0) return(siErr);
   siCnt=siClpPrsParLst(pvHdl,siLev+1,psArg,psDep);
   if (siCnt<0) return(siCnt);
   siErr=siClpFinObj(pvHdl,siLev,siPos,psArg,psDep,asSav);
   if (siErr<0) return(siErr);
   if (psHdl->siTok==CLPTOK_RBC) {
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d OBJ(%s(parlst))-CLS)\n",fpcPre(pvHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw);
   } else {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Character \')\' missing (%s)\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev));
      }
      return(CLPERR_SYN);
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
         psHdl->siTok=siClpScnSrc(pvHdl);
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
         psHdl->siTok=siClpScnSrc(pvHdl);
         if (psHdl->siTok<0) return(psHdl->siTok);
         siCnt=siClpPrsParLst(pvHdl,0,NULL,psTab);
         if (siCnt<0) return(siCnt);
         if (psHdl->siTok==CLPTOK_RBC) {
            psHdl->siTok=siClpScnSrc(pvHdl);
            if (psHdl->siTok<0) return(psHdl->siTok);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
               fprintf(psHdl->pfErr,"%s Character \')\' missing (MAIN)\n",fpcPre(pvHdl,0));
            }
            return(CLPERR_SYN);
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
   psHdl->siTok=siClpScnSrc(pvHdl);
   if (psHdl->siTok<0) return(psHdl->siTok);
   switch (psArg->psFix->siTyp) {
   case CLPTYP_NUMBER: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_NUM,psArg->psFix->siTyp,psArg); break;
   case CLPTYP_FLOATN: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_FLT,psArg->psFix->siTyp,psArg); break;
   case CLPTYP_STRING: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_STR,psArg->psFix->siTyp,psArg); break;
   case CLPTYP_OBJECT: siCnt=siClpPrsObjLst(pvHdl,siLev,psArg); break;
   case CLPTYP_OVRLAY: siCnt=siClpPrsOvlLst(pvHdl,siLev,psArg); break;
   default:
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s Type (%d) of parameter \'%s.%s\' is not supported with arrays\n",fpcPre(pvHdl,0),psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      return(CLPERR_SEM);
   }
   if (siCnt<0) return(siCnt);
   if (psHdl->siTok==CLPTOK_SBC) {
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d ARY(%s[typlst])-CLS)\n",fpcPre(pvHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw);
      return(CLP_OK);
   } else {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Character \']\' missing (%s)\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev));
      }
      return(CLPERR_SYN);
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
      siErr=siClpPrsVal(pvHdl,siLev,siPos,siTyp,psArg);
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
   const int                     siTyp,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siInd;
   TsSym*                        psVal;
   char                          acVal[CLPMAX_LEXSIZ];
   strcpy(acVal,psHdl->acLex);
   if (psHdl->siTok==CLPTOK_KYW) {
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      psHdl->apPat[siLev]=psArg;
      siInd=siClpSymFnd(pvHdl,siLev+1,siPos,acVal,psArg->psDep,&psVal,NULL);
      if (siInd<0) return(siInd);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d KYW(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,acVal);
      return(siClpBldCon(pvHdl,siLev,siPos,psArg,psVal));
   } else {
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,acVal);
      return(siClpBldLit(pvHdl,siLev,siPos,siTyp,psArg,acVal));
   }
}

/*****************************************************************************/

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
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_SUP) {
         strcpy(acSup,psHdl->acLex);
         psHdl->siTok=siClpScnSrc(pvHdl);
         if (psHdl->siTok<0) return(psHdl->siTok);
         return(siClpBldPro(pvHdl,acPat,acSup));
      } else {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
            fprintf(psHdl->pfErr,"%s Property string (\"...\") missing (%s)\n",fpcPre(pvHdl,0),acPat);
         }
         return(CLPERR_SYN);
      }
   } else {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
         fprintf(psHdl->pfErr,"%s Assignment character (\'=\') missing (%s)\n",fpcPre(pvHdl,0),acPat);
      }
      return(CLPERR_SYN);
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
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
            fprintf(psHdl->pfErr,"%s Property path (%s) is too long (more then %d byte)\n",fpcPre(pvHdl,0),pcPat,CLPMAX_PATLEN);
         }
         return(CLPERR_INT);
      }
      strcat(pcPat,psHdl->acLex);
      psHdl->siTok=siClpScnSrc(pvHdl);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_DOT) {
         strcat(pcPat,".");
         psHdl->siTok=siClpScnSrc(pvHdl);
         if (psHdl->siTok<0) return(psHdl->siTok);
      }
      siLev++;
   }
   return(siLev);
}

/*****************************************************************************/

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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"PARAMETER-ERROR\n");
         fprintf(psHdl->pfErr,"%s Root (%s.%s.%s) is too long (>=%d)\n",fpcPre(pvHdl,0),psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd,CLPMAX_PATLEN);
      }
      return(CLPERR_PAR);
   }
   sprintf(acRot,"%s.%s.%s",psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd);
   l=strlen(acRot);

   if (strxcmp(psHdl->isCas,acRot,pcPat,l,0)==0) {
      if (pcPat[l]!='.') {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Property path (%s) is not valid\n",fpcPre(pvHdl,0),pcPat);
         }
         return(CLPERR_SEM);
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
               sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s=\"%s\"\n",pcPat,pcPro);
            }
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"BUILD-PROPERTY %s=\"%s\"\n",pcPat,pcPro);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Path '%s' for property \"%s\" is not an argument or alias\n",fpcPre(pvHdl,0),pcPat,pcPro);
            }
            return(CLPERR_SEM);
         }
      } else {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Path '%s' for property \"%s\" not found\n",fpcPre(pvHdl,0),pcPat,pcPro);
         }
         return(CLPERR_SEM);
      }
   } else {
      if (psHdl->isChk) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Root of path (%s) does not match root of handle (%s.%s.%s)\n",
                    fpcPre(pvHdl,0),pcPat,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd);
         }
         return(CLPERR_SEM);
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
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s The type (%s) of link \'%s.%s\' dont match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         return(CLPERR_SEM);
      }
      if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s To many (>%d) occurrences of link \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
         }
         return(CLPERR_SEM);
      }
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for link \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
         }
         return(CLPERR_SIZ);
      }
      if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"TABLE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Keyword (%s.%s) and type (%s) of link are defined but data pointer or write pointer not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         return(CLPERR_TAB);
      }

      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>65535) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Internal number (%"PRId64") for link \'%s.%s\' need more than 8 Bit\n",fpcPre(pvHdl,0),siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Internal number (%"PRId64") for link \'%s.%s\' need more than 16 Bit\n",fpcPre(pvHdl,0),siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Internal number (%"PRId64") for link \'%s.%s\' need more than 32 Bit\n",fpcPre(pvHdl,0),siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      case 8:
         *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Size (%d) for the value (%"PRId64") of link \'%s.%s\' is not 1, 2, 4 or 8)\n",fpcPre(pvHdl,0),psArg->psFix->siSiz,siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         return(CLPERR_SIZ);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' dont match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s To many (>%d) occurrences of \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siRst<psArg->psFix->siSiz) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SIZE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_SIZ);
   }
   if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s.%s) and type (%s) of argument defined but data pointer or write pointer not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_TAB);
   }

   switch (psArg->psFix->siSiz) {
   case 1:
      if (siVal<(-128) || siVal>65535) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Object identifier (%"PRId64") of \'%s.%s\' need more than 8 Bit\n",fpcPre(pvHdl,0),siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         return(CLPERR_SEM);
      }
      *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
      break;
   case 2:
      if (siVal<(-32768) || siVal>65535) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Object identifier (%"PRId64") of \'%s.%s\' need more than 16 Bit\n",fpcPre(pvHdl,0),siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         return(CLPERR_SEM);
      }
      *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
      break;
   case 4:
      if (siVal<(-2147483648LL) || siVal>4294967295LL) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Object identifier (%"PRId64") of \'%s.%s\' need more than 32 Bit\n",fpcPre(pvHdl,0),siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         return(CLPERR_SEM);
      }
      *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
      break;
   case 8:
      *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
      break;
   default:
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SIZE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Size (%d) for the value (%"PRId64") of \'%s.%s\' is not 1, 2, 4 or 8)\n",fpcPre(pvHdl,0),psArg->psFix->siSiz,siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      return(CLPERR_SIZ);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s To many (>%d) occurrences of \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (CLPISS_SEL(psArg->psStd->uiFlg)) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The argument \'%s.%s\' accept only a keyword representing a constant defintion for type \'%s\'\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         fprintf(psHdl->pfErr,"%s Valid values:\n",fpcPre(pvHdl,0));
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s.%s) and type (%s) of argument defined but data pointer or write pointer not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_TAB);
   }

   switch (siTyp) {
   case CLPTYP_SWITCH:
   case CLPTYP_NUMBER:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
         }
         return(CLPERR_SIZ);
      }
      errno=0;
      switch (pcVal[0]) {
      case 'b':siVal=strtoll(pcVal+1,&pcHlp, 2); break;
      case 'o':siVal=strtoll(pcVal+1,&pcHlp, 8); break;
      case 'd':siVal=strtoll(pcVal+1,&pcHlp,10); break;
      case 'x':siVal=strtoll(pcVal+1,&pcHlp,16); break;
      case 't':siVal=strtoll(pcVal+1,&pcHlp,10); break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Base (%c) of number literal (%s.%s=%s) not supported\n",fpcPre(pvHdl,0),pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,pcVal+1);
         }
         return(CLPERR_SEM);
      }
      if (errno || strlen(pcHlp)) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Number (%s) of \'%s.%s\' can\'t be converted to a 64 bit value (rest: %s)\n",fpcPre(pvHdl,0),pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,pcHlp);
         }
         return(CLPERR_SEM);
      }
      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>255) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Literal number (%s) of \'%s.%s\' need more than 8 Bit\n",fpcPre(pvHdl,0),pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Literal number (%s) of \'%s.%s\' need more than 16 Bit\n",fpcPre(pvHdl,0),pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s Literal number (%s) of \'%s.%s\' need more than 32 Bit\n",fpcPre(pvHdl,0),pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      case 8:
         *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
         break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Size (%d) for the value (%s) of \'%s.%s\' is not 1, 2, 4 or 8)\n",fpcPre(pvHdl,0),psArg->psFix->siSiz,pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         return(CLPERR_SIZ);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=psArg->psFix->siSiz;
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_FLOATN:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
         }
         return(CLPERR_SIZ);
      }
      errno=0;
      switch (pcVal[0]) {
      case 'd':flVal=strtod(pcVal+1,&pcHlp); break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Base (%c) of floating point literal (%s.%s=%s) not supported\n",fpcPre(pvHdl,0),pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,pcVal+1);
         }
         return(CLPERR_SEM);
      }
      if (errno || strlen(pcHlp)) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
            fprintf(psHdl->pfErr,"%s Floating number (%s) of \'%s.%s\' can\'t be converted to a valid 64 bit value (rest: %s)\n",fpcPre(pvHdl,0),pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,pcHlp);
         }
         return(CLPERR_SEM);
      }
      switch (psArg->psFix->siSiz) {
      case 4:
         *((F32*)psArg->psVar->pvPtr)=flVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-F32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,flVal);
         break;
      case 8:
         *((F64*)psArg->psVar->pvPtr)=flVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-F64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,flVal);
         break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Size (%d) for the value (%s) of \'%s.%s\' is not 4 (float) or 8 (double))\n",fpcPre(pvHdl,0),psArg->psFix->siSiz,pcVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         return(CLPERR_SIZ);
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
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Length of hexadecimal string (%c(%s)) for \'%s.%s\' is not a multiple of 2\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_LEX);
            }
            if ((l1/2)>l0) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Hexadecimal string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               return(CLPERR_LEX);
            }
            l2=hex2bin(pcVal+2,(U08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1/2) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Hexadecimal string (%c(%s)) of \'%s.%s\' can\'t be converted from hex to bin\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_SEM);
            }
            siSln=l2;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-HEX(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s String literal (%c(%s)) for \'%s.%s\' is a binary (only zero terminated character string permitted)\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         break;
      case 'a':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s ASCII string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               return(CLPERR_LEX);
            }
            l2=chr2asc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s ASCII string (%c(%s)) of \'%s.%s\' can\'t be converted to ASCII\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_SEM);
            }
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-ASC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s String literal (%c(%s)) for \'%s.%s\' is a binary (only zero terminated character string permitted)\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         break;
      case 'e':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s EBCDIC string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               return(CLPERR_LEX);
            }
            l2=chr2ebc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s EBCDIC string (%c(%s)) of \'%s.%s\' can\'t be converted to EBCDIC\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_SEM);
            }
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-EBC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s String literal (%c(%s)) for \'%s.%s\' is a binary (only zero terminated character string permitted)\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         break;
      case 'c':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Character string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               return(CLPERR_LEX);
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1); l2=l1;
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         } else {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s String literal (%c(%s)) for \'%s.%s\' is a binary (only zero terminated character string permitted)\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
         break;
      case 's':
         if (l1+1>l0) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
               fprintf(psHdl->pfErr,"%s Character string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
            }
            return(CLPERR_LEX);
         }
         memcpy(psArg->psVar->pvPtr,pcVal+2,l1);
         ((char*)psArg->psVar->pvPtr)[l1]=EOS;
         l2=l1+1; siSln=l1;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         break;
      case 'd':
         if (CLPISS_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Character string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               return(CLPERR_LEX);
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1); l2=l1;
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         } else {
            if (l1+1>l0) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Character string (%c(%s)) of \'%s.%s\' is longer than %d\n",fpcPre(pvHdl,0),pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
               return(CLPERR_LEX);
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1);
            ((char*)psArg->psVar->pvPtr)[l1]=EOS;
            l2=l1+1; siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,pcVal,l2);
         }
         break;
      default:
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"LEXICAL-ERROR\n");
            fprintf(psHdl->pfErr,"%s String prefix (%c) of \'%s.%s\' is not supported\n",fpcPre(pvHdl,0),pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            fprintf(psHdl->pfErr,"%s Valid values:\n",fpcPre(pvHdl,0));
            fprintf(psHdl->pfErr,"%s x - for conversion from hex to bin \n",fpcPre(pvHdl,1));
            fprintf(psHdl->pfErr,"%s a - for conversion in ASCII\n",fpcPre(pvHdl,1));
            fprintf(psHdl->pfErr,"%s e - for conversion in EBCDIC\n",fpcPre(pvHdl,1));
            fprintf(psHdl->pfErr,"%s c - for no conversion (normal character string without zero termination)\n",fpcPre(pvHdl,1));
            fprintf(psHdl->pfErr,"%s s - normal character string with zero termination\n",fpcPre(pvHdl,1));
         }
         return(CLPERR_LEX);
      }

      if (CLPISS_FIX(psArg->psStd->uiFlg)) {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SIZE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
            }
            return(CLPERR_SIZ);
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
   default:
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TYPE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Type (%d) of parameter \'%s.%s\' not supported in this case (literal)\n",fpcPre(pvHdl,0),psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      return(CLPERR_TYP);
   }
   psArg->psVar->siCnt++;

   pcHlp=fpcPat(pvHdl,siLev);
   if (strlen(psHdl->acLst) + strlen(pcHlp) + strlen(psArg->psStd->pcKyw) + strlen(pcVal) + 4 < CLPMAX_LSTLEN) {
      sprintf(&psHdl->acLst[strlen(psHdl->acLst)],"%s.%s=%s\n",pcHlp,psArg->psStd->pcKyw,pcVal);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' don\'t match the type (%s) of value \'%s\'\n",
        		 fpcPre(pvHdl,0),apClpTyp[psArg->psFix->siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp],psVal->psStd->pcKyw);
      }
      return(CLPERR_SEM);
   }
   if (CLPISS_SEL(psArg->psStd->uiFlg) && psVal->psVar->pvPtr!=NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The argument \'%s.%s\' accept only a key word representing a constant defintion for type \'%s\'\n",
        		 fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         fprintf(psHdl->pfErr,"%s Valid values:\n",fpcPre(pvHdl,0));
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->pvDat==NULL || psArg->psVar->pvPtr==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Key word (%s.%s) and type (%s) of argument defined but data pointer or write pointer not set\n",
        		 fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_TAB);
   }
   if (psArg->psVar->siCnt+psVal->psVar->siCnt>psArg->psFix->siMax) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s To many (>%d) occurrences of \'%s.%s\' with type \'%s\'\n",
        		 fpcPre(pvHdl,0),psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psVal->psVar->siCnt==0) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s) and type (%s) of constant value defined but data element counter is 0\n",
        		 fpcPre(pvHdl,0),psVal->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp]);
      }
      return(CLPERR_TAB);
   }
   if (psVal->psVar->pvDat==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s) and type (%s) of constant value defined but data pointer not set\n",
        		 fpcPre(pvHdl,0),psVal->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp]);
      }
      return(CLPERR_TAB);
   }
   switch (psArg->psFix->siTyp) {
   case CLPTYP_NUMBER:
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",
            		fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         return(CLPERR_SIZ);
      }
      for (i=0;i<psVal->psVar->siCnt;i++) {
         switch (psVal->psFix->siSiz) {
         case 1: siVal=((I08*)psVal->psVar->pvDat)[i]; break;
         case 2: siVal=((I16*)psVal->psVar->pvDat)[i]; break;
         case 4: siVal=((I32*)psVal->psVar->pvDat)[i]; break;
         case 8: siVal=((I64*)psVal->psVar->pvDat)[i]; break;
         default:
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SIZE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 1, 2, 4 or 8)\n",
            		   fpcPre(pvHdl,0),psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SIZ);
         }
         switch (psArg->psFix->siSiz) {
         case 1:
            if (siVal<(-128) || siVal>255) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Constant number (%s=%"PRId64") of \'%s.%s\' needs more than 8 Bit\n",
                		  fpcPre(pvHdl,0),psVal->psStd->pcKyw,siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_SEM);
            }
            *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
            break;
         case 2:
            if (siVal<(-32768) || siVal>65535) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Constant number (%s=%"PRId64") of \'%s.%s\' needs more than 16 Bit\n",
                		  fpcPre(pvHdl,0),psVal->psStd->pcKyw,siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_SEM);
            }
            *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
            break;
         case 4:
            if (siVal<(-2147483648LL) || siVal>4294967295LL) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Constant number (%s=%"PRId64") of \'%s.%s\' needs more than 32 Bit\n",
                		  fpcPre(pvHdl,0),psVal->psStd->pcKyw,siVal,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               return(CLPERR_SEM);
            }
            *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
            break;
         case 8:
            *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,siVal);
            break;
         default:
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SIZE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Size (%d) for the value of \'%s.%s\' is not 1, 2, 4 or 8)\n",
            		   fpcPre(pvHdl,0),psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SIZ);
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
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"SIZE-ERROR\n");
            fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",
            		fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         return(CLPERR_SIZ);
      }
      for (i=0;i<psVal->psVar->siCnt;i++) {
         switch (psVal->psFix->siSiz) {
         case 4: flVal=((F32*)psVal->psVar->pvDat)[i]; break;
         case 8: flVal=((F64*)psVal->psVar->pvDat)[i]; break;
         default:
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SIZE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 4 or 8)\n",
            		   fpcPre(pvHdl,0),psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SIZ);
         }
         switch (psArg->psFix->siSiz) {
         case 4:
            *((F32*)psArg->psVar->pvPtr)=(F32)flVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-F32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,flVal);
            break;
         case 8:
            *((F64*)psArg->psVar->pvPtr)=(F64)flVal;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-F64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,flVal);
            break;
         default:
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SIZE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 4 or 8)\n",
            		   fpcPre(pvHdl,0),psArg->psFix->siSiz,psVal->psStd->pcKyw,psArg->psStd->pcKyw,fpcPat(pvHdl,siLev));
            }
            return(CLPERR_SIZ);
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
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s String constant \'%s\' for \'%s.%s\' is a binary (only zero terminated character string permitted)\n",
            		   fpcPre(pvHdl,0),psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
         }
      }
      if (CLPISS_FIX(psArg->psStd->uiFlg)) {
         if (psVal->psFix->siSiz>psArg->psFix->siSiz) {
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SIZE-ERROR\n");
               fprintf(psHdl->pfErr,"%s Size of constant value \'%s\' (%d) is bigger then sizeof argument \'%s.%s\' (%d) with type \'%s\'\n",
            		   fpcPre(pvHdl,0),psVal->psStd->pcKyw, psVal->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psFix->siSiz,apClpTyp[psArg->psFix->siTyp]);
            }
            return(CLPERR_SIZ);
         }
         pcArg=(char*)psArg->psVar->pvPtr;
         pcVal=(char*)psVal->psVar->pvDat;
         for (l=i=0;i<psVal->psVar->siCnt;i++) {
            if (psArg->psVar->siRst<psArg->psFix->siSiz) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SIZE-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",
                		  fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
               }
               return(CLPERR_SIZ);
            }
            memcpy(pcArg,pcVal,psVal->psFix->siSiz);
            memset(pcArg+psVal->psFix->siSiz,0,psArg->psFix->siSiz-psVal->psFix->siSiz);
            for (siSln=0;pcArg[siSln]!=EOS && siSln<psArg->psFix->siSiz;siSln++);
            siEln=psVal->psFix->siSiz;
            if (psHdl->pfBld!=NULL) {
               fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s='",
                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw);
               for (k=0;k<siEln;k++) {
                  if (isprint(pcArg[k])) fprintf(psHdl->pfBld,"%c",pcArg[k]); else fprintf(psHdl->pfBld,"\\x%2.2X",pcArg[k]);
               }
               fprintf(psHdl->pfBld,"\'(%"PRId64")\n",siEln);
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
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
               fprintf(psHdl->pfErr,"%s String data of constant value \'%s\' is longer than space left %d of argument \'%s.%s\'\n",
            		   fpcPre(pvHdl,0),psVal->psStd->pcKyw,psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SEM);
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
               for (k=0;k<siEln;k++) {
                  if (isprint(pcArg[k])) fprintf(psHdl->pfBld,"%c",pcArg[k]); else fprintf(psHdl->pfBld,"\\x%2.2X",pcArg[k]);
               }
               fprintf(psHdl->pfBld,"\'(%"PRId64")\n",siEln);
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"SIZE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Size (%d) for the constant value \'%s\' of \'%s.%s\' is not 1, 2, 4 or 8)\n",
                    		 fpcPre(pvHdl,0),psVal->psFix->psEln->psFix->siSiz,psVal->psFix->psEln->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  return(CLPERR_SIZ);
               }
               for (siSln=0;pcArg[siSln]!=EOS && siSln<siEln;siSln++);
               if (psHdl->pfBld!=NULL) {
                  fprintf(psHdl->pfBld,"%s BUILD-CONSTANT-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s='",
                          fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw);
                  for (k=0;k<siEln;k++) {
                     if (isprint(pcArg[k])) fprintf(psHdl->pfBld,"%c",pcArg[k]); else fprintf(psHdl->pfBld,"\\x%2.2X",pcArg[k]);
                  }
                  fprintf(psHdl->pfBld,"\'(%"PRId64")\n",siEln);
               }
               siErr=siClpBldLnk(pvHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
               if (siErr<0) return(siErr);
               siErr=siClpBldLnk(pvHdl,siLev,siPos,siEln,psArg->psFix->psEln,TRUE);
               if (siErr<0) return(siErr);
               l+=siEln; pcArg+=siEln;
            }
            if (l!=psVal->psVar->siLen) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SYSTEM-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Length conflict between argument \'%s.%s.%s\' and link \'%s\')\n",
                		  fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psVal->psStd->pcKyw,psVal->psFix->psEln->psStd->pcKyw);
               }
               return(CLPERR_SYS);
            }
         }
         psArg->psVar->siLen+=psVal->psVar->siLen;
         psArg->psVar->siRst-=psVal->psVar->siLen;
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psVal->psVar->siLen;
      }
      break;
   default:
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TYPE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Type (%d) of parameter \'%s.%s\' not supported in this case (constant)\n",
        		 fpcPre(pvHdl,0),psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      return(CLPERR_TYP);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
   }

   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
      *psSav=*psHlp->psVar;
      psHlp->psVar->pvDat=((char*)psHdl->pvDat)+psHlp->psFix->siOfs;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siRst=psHlp->psFix->siSiz;
      if (CLPISS_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(pvHdl,0,i,psHlp);
      if (siErr<0) {
         if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Set default value for argument \'%s.%s' failed\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Parameter \'%s.%s\' not specified\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
                  fprintf(psHdl->pfErr,"%s Please specify parameter:\n",fpcPre(pvHdl,0));
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
               }
            } else {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Amount of occurrences (%d) of parameter \'%s.%s\' is smaller than required minimum amount (%d)\n",
                		  fpcPre(pvHdl,0),psHlp->psVar->siCnt,fpcPat(pvHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  fprintf(psHdl->pfErr,"%s Please specify parameter additionally %d times:\n",fpcPre(pvHdl,0),psHlp->psFix->siMin-psHlp->psVar->siCnt);
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
               }
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
   }

   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
      *psSav=*psHlp->psVar;
      psHlp->psVar->pvDat=((char*)psHdl->pvDat);
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siRst=psHlp->psFix->siSiz;
      if (CLPISS_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(pvHdl,0,i,psHlp);
         if (siErr<0) {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Set default value for argument \'%s.%s' failed\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Parameter \'%s.%s\' not specified\n",fpcPre(pvHdl,0),fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
                     fprintf(psHdl->pfErr,"%s Please specify parameter:\n",fpcPre(pvHdl,0));
                     vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
                  }
               } else {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Amount of occurrences (%d) of parameter \'%s.%s\' is smaller than required minimum amount (%d)\n",
                    		 fpcPre(pvHdl,0),psHlp->psVar->siCnt,fpcPat(pvHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                     fprintf(psHdl->pfErr,"%s Please specify parameter additionally %d times:\n",fpcPre(pvHdl,0),psHlp->psFix->siMin-psHlp->psVar->siCnt);
                     vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
                  }
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

   if (psArg->psFix->siTyp!=siTyp) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' dont match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s To many (>%d) occurrences of \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siRst<psArg->psFix->siSiz) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SIZE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enought for argument \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_SIZ);
   }
   if (psArg->psDep==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_TAB);
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
   int                           siErr,i;

   if (psArg->psFix->siTyp!=siTyp) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_TAB);
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(pvHdl,siLev+1,i,psHlp);
      if (siErr<0) {
         if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Set default value for argument \'%s.%s.%s\' failed\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Parameter \'%s.%s.%s\' not specified\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
                  fprintf(psHdl->pfErr,"%s Please specify parameter:\n",fpcPre(pvHdl,0));
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
               }
            } else {
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Amount of occurrences (%d) of parameter \'%s.%s.%s\' is smaller than required minimum amount (%d)\n",
                		  fpcPre(pvHdl,0),psDep->psVar->siCnt,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  fprintf(psHdl->pfErr,"%s Please specify parameter additionally %d times:\n",fpcPre(pvHdl,0),psHlp->psFix->siMin-psHlp->psVar->siCnt);
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
               }
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s To many (>%d) occurrences of \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psVar->siRst<psArg->psFix->siSiz) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SIZE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Rest of space (%d) is not big enough for argument \'%s.%s\' with type \'%s\'\n",fpcPre(pvHdl,0),psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_SIZ);
   }
   if (psArg->psDep==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_TAB);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
         fprintf(psHdl->pfErr,"%s The type (%s) of argument \'%s.%s\' don\'t match the expected type (%s)\n",fpcPre(pvHdl,0),apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      return(CLPERR_SEM);
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Key word (%s.%s) and type (%s) of argument defined but pointer to parameter table not set\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      return(CLPERR_TAB);
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(pvHdl,siLev+1,i,psHlp);
         if (siErr<0) {
            if (psHdl->pfErr!=NULL) fprintf(psHdl->pfErr,"%s Set default value for argument \'%s.%s.%s\' failed\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Parameter \'%s.%s.%s\' not specified\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
                     fprintf(psHdl->pfErr,"%s Please specify parameter:\n",fpcPre(pvHdl,0));
                     vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
                  }
               } else {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"SEMANTIC-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Amount of occurrences (%d) of parameter \'%s.%s.%s\' is smaller than required minimum amount (%d)\n",
                    		 fpcPre(pvHdl,0),psHlp->psVar->siCnt,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                     fprintf(psHdl->pfErr,"%s Please specify parameter additionally %d times:\n",fpcPre(pvHdl,0),psHlp->psFix->siMin-psHlp->psVar->siCnt);
                     vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
                  }
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

/*****************************************************************************/

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
      for (siTok=siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&pcCur,acLex);siTok!=CLPTOK_END;
           siTok=siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&pcCur,acLex)) {
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
               if (strxcmp(psHdl->isCas,acLex,"INIT",0,0)!=0) {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"TABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Keyword (%s) in default / property definition for object \'%s.%s\' is not \'INIT\'\n",
                                          fpcPre(pvHdl,0),acLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  return(CLPERR_TAB);
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
                  if (strxcmp(psHdl->isCas,acLex,"ON",0,0)!=0 && strxcmp(psHdl->isCas,acLex,"OFF",0,0)!=0) {
                     if (psHdl->pfErr!=NULL) {
                        fprintf(psHdl->pfErr,"TABLE-ERROR\n");
                        fprintf(psHdl->pfErr,"%s Keyword (%s) in default / property definition for switch \'%s.%s\' is not \'ON\' or \'OFF\'\n",
                                             fpcPre(pvHdl,0),acLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     return(CLPERR_TAB);
                  }
                  if (strxcmp(psHdl->isCas,acLex,"ON",0,0)==0) {
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
         default:
            if (psHdl->pfErr!=NULL) {
               fprintf(psHdl->pfErr,"SYNTAX-ERROR\n");
               fprintf(psHdl->pfErr,"%s Token (%s) not allowed in default / property definition (%s) for argument \'%s.%s'\n",
                                    fpcPre(pvHdl,0),apClpTok[siTok],psArg->psFix->pcDft,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            return(CLPERR_SYN);
         }
      }
   }
   return(CLP_OK);
}

/*****************************************************************************/

static void vdClpPrnArg(
   void*                         pvHdl,
   FILE*                         pfOut,
   const int                     siLev,
   const char*                   pcKyw,
   const char*                   pcAli,
   const int                     siKwl,
   const int                     siTyp,
   const char*                   pcHlp,
   const char*                   pcDft)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char*                         p=fpcPre(pvHdl,siLev);
   int                           i,siLen;
   const char*                   a="TYPE";
   const char*                   b=apClpTyp[siTyp];
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
         for (i=i;i<siLen;i++) fprintf(pfOut,"%c",tolower(pcKyw[i]));
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
   const int                     siTyp,
   const TsSym*                  psTab)
{
   const TsSym*                  psHlp;
   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
      if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
         vdClpPrnArg(pvHdl,pfOut,siLev,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Argument table not defined\n",fpcPre(pvHdl,0));
      }
      return (CLPERR_TAB);
   }

   if (psTab->psBak!=NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at beginning of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Maximum amount of entries (%d) for parameter \'%s\' not valid\n",fpcPre(pvHdl,0),psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Maximum amount of entries (%d) for parameter \'%s\' not valid\n",fpcPre(pvHdl,0),psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Maximum amount of entries (%d) for parameter \'%s\' not valid\n",fpcPre(pvHdl,0),psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
               }
               break;
            case CLPTYP_STRING:
               if (CLPISS_BIN(psHlp->psStd->uiFlg)) pcHlp="bin"; else pcHlp="str";
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Maximum amount of entries (%d) for parameter \'%s\' not valid\n",fpcPre(pvHdl,0),psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
               }
               break;
            case CLPTYP_OBJECT:
               if (psHlp->psDep==NULL) {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Argument table for object \'%s\' not defined\n",fpcPre(pvHdl,0),psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Maximum amount of entries (%d) for parameter \'%s\' not valid\n",fpcPre(pvHdl,0),psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
               }
               break;
            case CLPTYP_OVRLAY:
               if (psHlp->psDep==NULL) {
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Argument table for object \'%s\' not defined\n",fpcPre(pvHdl,0),psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
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
                  if (psHdl->pfErr!=NULL) {
                     fprintf(psHdl->pfErr,"\nTABLE-ERROR\n");
                     fprintf(psHdl->pfErr,"%s Maximum amount of entries (%d) for parameter \'%s\' not valid\n",fpcPre(pvHdl,0),psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  return (CLPERR_TAB);
               }
               break;
            default:
               if (psHdl->pfErr!=NULL) {
                  fprintf(psHdl->pfErr,"\nTYPE-ERROR\n");
                  fprintf(psHdl->pfErr,"%s Type (%d) of parameter \'%s\' not supported\n",fpcPre(pvHdl,0),psHlp->psFix->siTyp,psHlp->psStd->pcKyw);
               }
               return (CLPERR_TYP);
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
   const int                     siLev,
   const int                     siDep,
   const int                     siTyp,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp;
   int                           siErr;

   if (psTab==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Argument table not defined\n",fpcPre(pvHdl,0));
      }
      return (CLPERR_TAB);
   }

   if (psTab->psBak!=NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
   }

   if (siLev<siDep || siDep>9) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && CLPISS_CMD(psHlp->psStd->uiFlg) && !CLPISS_LNK(psHlp->psStd->uiFlg)) {
            vdClpPrnArg(pvHdl,pfOut,siLev,psHlp->psStd->pcKyw,psHlp->psStd->pcAli,psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft);
            if (psHlp->psDep!=NULL) {
               if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnHlp(pvHdl,pfOut,siLev+1,siDep,-1,psHlp->psDep);
                  if (siErr<0) return(siErr);
               } else {
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnHlp(pvHdl,pfOut,siLev+1,siDep,psHlp->psFix->siTyp,psHlp->psDep);
                  if (siErr<0) return(siErr);
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
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Argument not defined\n",fpcPre(pvHdl,0));
      }
      return (CLPERR_TAB);
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
      if (CLPISS_BIN(psArg->psStd->uiFlg)) pcHlp="bin"; else pcHlp="str";
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
         if (psHdl->pfErr!=NULL) {
            fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
            fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
         }
         return(CLPERR_INT);
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
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp;
   int                           siErr;

   if (psTab==NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"TABLE-ERROR\n");
         fprintf(psHdl->pfErr,"%s Argument table not defined\n",fpcPre(pvHdl,0));
      }
      return (CLPERR_TAB);
   }

   if (psTab->psBak!=NULL) {
      if (psHdl->pfErr!=NULL) {
         fprintf(psHdl->pfErr,"INTERNAL-ERROR\n");
         fprintf(psHdl->pfErr,"%s Entry \'%s.%s\' not at begining of a table\n",fpcPre(pvHdl,0),fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      }
      return(CLPERR_INT);
   }

   if (siLev<siDep || siDep>9) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISS_ARG(psHlp->psStd->uiFlg) && CLPISS_PRO(psHlp->psStd->uiFlg)) {
            if ((isMan || (!CLPISS_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && strlen(psHlp->psFix->pcMan)) {
               fprintf(pfOut,"\n# DESCRIPTION for %s.%s.%s.%s:\n %s #\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcMan);
               isMan=TRUE;
            } else {
               if (isMan) fprintf(pfOut,"\n");
               isMan=FALSE;
            }
            if (psHlp->psFix->pcDft!=NULL) {
               fprintf(pfOut,"%s.%s.%s.%s=\"%s\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcDft);
            } else {
               fprintf(pfOut,"%s.%s.%s.%s=\"\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw);
            }
            fprintf(pfOut,"# TYPE: %s HELP: %s #\n",apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);

            if (psHlp->psDep!=NULL) {
               if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                  psHdl->apPat[siLev]=psHlp;
                  siErr=siClpPrnPro(pvHdl,pfOut,isMan,siLev+1,siDep,psHlp->psDep);
                  if (siErr<0) return(siErr);
               }
            }
         }
      }
   }

   return (CLP_OK);
}

/******************************************************************************/

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

/******************************************************************************/

static U32 chr2asc(
   const C08*           chr,
         C08*           asc,
   const U32            len)
{
   U32                  i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
         case '\n' : asc[i]=0x0A; break;
         case '\r' : asc[i]=0x0D; break;

         case ' ' : asc[i]=0x20; break;
         case '!' : asc[i]=0x21; break;
         case '\"': asc[i]=0x22; break;
         case '#' : asc[i]=0x23; break;
         case '$' : asc[i]=0x24; break;
         case '%' : asc[i]=0x25; break;
         case '&' : asc[i]=0x26; break;
         case '\'': asc[i]=0x27; break;
         case '(' : asc[i]=0x28; break;
         case ')' : asc[i]=0x29; break;
         case '*' : asc[i]=0x2A; break;
         case '+' : asc[i]=0x2B; break;
         case ',' : asc[i]=0x2C; break;
         case '-' : asc[i]=0x2D; break;
         case '.' : asc[i]=0x2E; break;
         case '/' : asc[i]=0x2F; break;

         case '0' : asc[i]=0x30; break;
         case '1' : asc[i]=0x31; break;
         case '2' : asc[i]=0x32; break;
         case '3' : asc[i]=0x33; break;
         case '4' : asc[i]=0x34; break;
         case '5' : asc[i]=0x35; break;
         case '6' : asc[i]=0x36; break;
         case '7' : asc[i]=0x37; break;
         case '8' : asc[i]=0x38; break;
         case '9' : asc[i]=0x39; break;
         case ':' : asc[i]=0x3A; break;
         case ';' : asc[i]=0x3B; break;
         case '<' : asc[i]=0x3C; break;
         case '=' : asc[i]=0x3D; break;
         case '>' : asc[i]=0x3E; break;
         case '?' : asc[i]=0x3F; break;

         case '@' : asc[i]=0x40; break;
         case 'A' : asc[i]=0x41; break;
         case 'B' : asc[i]=0x42; break;
         case 'C' : asc[i]=0x43; break;
         case 'D' : asc[i]=0x44; break;
         case 'E' : asc[i]=0x45; break;
         case 'F' : asc[i]=0x46; break;
         case 'G' : asc[i]=0x47; break;
         case 'H' : asc[i]=0x48; break;
         case 'I' : asc[i]=0x49; break;
         case 'J' : asc[i]=0x4A; break;
         case 'K' : asc[i]=0x4B; break;
         case 'L' : asc[i]=0x4C; break;
         case 'M' : asc[i]=0x4D; break;
         case 'N' : asc[i]=0x4E; break;
         case 'O' : asc[i]=0x4F; break;

         case 'P' : asc[i]=0x50; break;
         case 'Q' : asc[i]=0x51; break;
         case 'R' : asc[i]=0x52; break;
         case 'S' : asc[i]=0x53; break;
         case 'T' : asc[i]=0x54; break;
         case 'U' : asc[i]=0x55; break;
         case 'V' : asc[i]=0x56; break;
         case 'W' : asc[i]=0x57; break;
         case 'X' : asc[i]=0x58; break;
         case 'Y' : asc[i]=0x59; break;
         case 'Z' : asc[i]=0x5A; break;
         case '[' : asc[i]=0x5B; break;
         case '\\': asc[i]=0x5C; break;
         case ']' : asc[i]=0x5D; break;
         case '^' : asc[i]=0x5E; break;
         case '_' : asc[i]=0x5F; break;

         case '`' : asc[i]=0x60; break;
         case 'a' : asc[i]=0x61; break;
         case 'b' : asc[i]=0x62; break;
         case 'c' : asc[i]=0x63; break;
         case 'd' : asc[i]=0x64; break;
         case 'e' : asc[i]=0x65; break;
         case 'f' : asc[i]=0x66; break;
         case 'g' : asc[i]=0x67; break;
         case 'h' : asc[i]=0x68; break;
         case 'i' : asc[i]=0x69; break;
         case 'j' : asc[i]=0x6A; break;
         case 'k' : asc[i]=0x6B; break;
         case 'l' : asc[i]=0x6C; break;
         case 'm' : asc[i]=0x6D; break;
         case 'n' : asc[i]=0x6E; break;
         case 'o' : asc[i]=0x6F; break;

         case 'p' : asc[i]=0x70; break;
         case 'q' : asc[i]=0x71; break;
         case 'r' : asc[i]=0x72; break;
         case 's' : asc[i]=0x73; break;
         case 't' : asc[i]=0x74; break;
         case 'u' : asc[i]=0x75; break;
         case 'v' : asc[i]=0x76; break;
         case 'w' : asc[i]=0x77; break;
         case 'x' : asc[i]=0x78; break;
         case 'y' : asc[i]=0x79; break;
         case 'z' : asc[i]=0x7A; break;
         case '{' : asc[i]=0x7B; break;
         case '|' : asc[i]=0x7C; break;
         case '}' : asc[i]=0x7D; break;
         case '~' : asc[i]=0x7E; break;
         default  : asc[i]=0x00; return(i);
      }
   }
   return(i);
}

static U32 chr2ebc(
   const C08*           chr,
         C08*           asc,
   const U32            len)
{
   U32                  i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
         case '\n' : asc[i]=0x25; break;
         case '\r' : asc[i]=0x0D; break;

         case ' ' : asc[i]=0x40; break;
         case '!' : asc[i]=0x5A; break;
         case '\"': asc[i]=0x7F; break;
         case '#' : asc[i]=0x7B; break;
         case '$' : asc[i]=0x5B; break;
         case '%' : asc[i]=0x6C; break;
         case '&' : asc[i]=0x50; break;
         case '\'': asc[i]=0x7D; break;
         case '(' : asc[i]=0x4D; break;
         case ')' : asc[i]=0x5D; break;
         case '*' : asc[i]=0x5C; break;
         case '+' : asc[i]=0x4E; break;
         case ',' : asc[i]=0x6B; break;
         case '-' : asc[i]=0x60; break;
         case '.' : asc[i]=0x4B; break;
         case '/' : asc[i]=0x61; break;

         case '0' : asc[i]=0xF0; break;
         case '1' : asc[i]=0xF1; break;
         case '2' : asc[i]=0xF2; break;
         case '3' : asc[i]=0xF3; break;
         case '4' : asc[i]=0xF4; break;
         case '5' : asc[i]=0xF5; break;
         case '6' : asc[i]=0xF6; break;
         case '7' : asc[i]=0xF7; break;
         case '8' : asc[i]=0xF8; break;
         case '9' : asc[i]=0xF9; break;
         case ':' : asc[i]=0x7A; break;
         case ';' : asc[i]=0x5E; break;
         case '<' : asc[i]=0x4C; break;
         case '=' : asc[i]=0x7E; break;
         case '>' : asc[i]=0x6E; break;
         case '?' : asc[i]=0x6F; break;

         case '@' : asc[i]=0x7C; break;
         case 'A' : asc[i]=0xC1; break;
         case 'B' : asc[i]=0xC2; break;
         case 'C' : asc[i]=0xC3; break;
         case 'D' : asc[i]=0xC4; break;
         case 'E' : asc[i]=0xC5; break;
         case 'F' : asc[i]=0xC6; break;
         case 'G' : asc[i]=0xC7; break;
         case 'H' : asc[i]=0xC8; break;
         case 'I' : asc[i]=0xC9; break;
         case 'J' : asc[i]=0xD1; break;
         case 'K' : asc[i]=0xD2; break;
         case 'L' : asc[i]=0xD3; break;
         case 'M' : asc[i]=0xD4; break;
         case 'N' : asc[i]=0xD5; break;
         case 'O' : asc[i]=0xD6; break;

         case 'P' : asc[i]=0xD7; break;
         case 'Q' : asc[i]=0xD8; break;
         case 'R' : asc[i]=0xD9; break;
         case 'S' : asc[i]=0xE2; break;
         case 'T' : asc[i]=0xE3; break;
         case 'U' : asc[i]=0xE4; break;
         case 'V' : asc[i]=0xE5; break;
         case 'W' : asc[i]=0xE6; break;
         case 'X' : asc[i]=0xE7; break;
         case 'Y' : asc[i]=0xE8; break;
         case 'Z' : asc[i]=0xE9; break;
         case '[' : asc[i]=0x00; break;
         case '\\': asc[i]=0xE0; break;
         case ']' : asc[i]=0x00; break;
         case '^' : asc[i]=0x5F; break;
         case '_' : asc[i]=0x6D; break;

         case '`' : asc[i]=0x79; break;
         case 'a' : asc[i]=0x81; break;
         case 'b' : asc[i]=0x82; break;
         case 'c' : asc[i]=0x83; break;
         case 'd' : asc[i]=0x84; break;
         case 'e' : asc[i]=0x85; break;
         case 'f' : asc[i]=0x86; break;
         case 'g' : asc[i]=0x87; break;
         case 'h' : asc[i]=0x88; break;
         case 'i' : asc[i]=0x89; break;
         case 'j' : asc[i]=0x91; break;
         case 'k' : asc[i]=0x92; break;
         case 'l' : asc[i]=0x93; break;
         case 'm' : asc[i]=0x94; break;
         case 'n' : asc[i]=0x95; break;
         case 'o' : asc[i]=0x96; break;

         case 'p' : asc[i]=0x97; break;
         case 'q' : asc[i]=0x98; break;
         case 'r' : asc[i]=0x99; break;
         case 's' : asc[i]=0xA2; break;
         case 't' : asc[i]=0xA3; break;
         case 'u' : asc[i]=0xA4; break;
         case 'v' : asc[i]=0xA5; break;
         case 'w' : asc[i]=0xA6; break;
         case 'x' : asc[i]=0xA7; break;
         case 'y' : asc[i]=0xA8; break;
         case 'z' : asc[i]=0xA9; break;
         case '{' : asc[i]=0xC0; break;
         case '|' : asc[i]=0x4F; break;
         case '}' : asc[i]=0xD0; break;
         case '~' : asc[i]=0xA1; break;

         default  : asc[i]=0x00; return(i);
      }
   }
   return(i);
}

static U32 hex2bin(
   const C08*           hex,
         U08*           bin,
   const U32            len)
{
   U32                  i,j;
   U08                  h1,h2;

   for (j=i=0;i<len-1;i+=2,j++)
   {
      switch (toupper(hex[i+0]))
      {
         case '0': h1=0x00; break;
         case '1': h1=0x01; break;
         case '2': h1=0x02; break;
         case '3': h1=0x03; break;
         case '4': h1=0x04; break;
         case '5': h1=0x05; break;
         case '6': h1=0x06; break;
         case '7': h1=0x07; break;
         case '8': h1=0x08; break;
         case '9': h1=0x09; break;
         case 'A': h1=0x0A; break;
         case 'B': h1=0x0B; break;
         case 'C': h1=0x0C; break;
         case 'D': h1=0x0D; break;
         case 'E': h1=0x0E; break;
         case 'F': h1=0x0F; break;
         default: return(j);
      }
      switch (toupper(hex[i+1]))
      {
         case '0': h2=0x00; break;
         case '1': h2=0x01; break;
         case '2': h2=0x02; break;
         case '3': h2=0x03; break;
         case '4': h2=0x04; break;
         case '5': h2=0x05; break;
         case '6': h2=0x06; break;
         case '7': h2=0x07; break;
         case '8': h2=0x08; break;
         case '9': h2=0x09; break;
         case 'A': h2=0x0A; break;
         case 'B': h2=0x0B; break;
         case 'C': h2=0x0C; break;
         case 'D': h2=0x0D; break;
         case 'E': h2=0x0E; break;
         case 'F': h2=0x0F; break;
         default: return(j);
      }
      bin[j]=((h1<<4)&0xF0)|((h2<<0)&0x0F);
   }
   return(j);
}

/******************************************************************************/

extern int strxcmp(
   const int            f,
   const char*          s1,
   const char*          s2,
   const int            n,
   const int            c)
{
   if (f) {
      if (n) {
         int i=1;
         int d=*s1-*s2;
         while (d==0 && *s1!=0 && *s2!=0 && i<n && *s1!=c && *s2!=c) {
            s1++; s2++; i++;
            d=*s1-*s2;
         }
         return(d);
      } else {
         int d=*s1-*s2;
         while (d==0 && *s1!=0 && *s2!=0 && *s1!=c && *s2!=c) {
            s1++; s2++;
            d=*s1-*s2;
         }
         return(d);
      }
   } else {
      if (n) {
         int i=1;
         int d=tolower(*s1)-tolower(*s2);
         while (d==0 && *s1!=0 && *s2!=0 && i<n && *s1!=c && *s2!=c) {
            s1++; s2++; i++;
            d=tolower(*s1)-tolower(*s2);
         }
         return(d);
      } else {
         int d=tolower(*s1)-tolower(*s2);
         while (d==0 && *s1!=0 && *s2!=0 && *s1!=c && *s2!=c) {
            s1++; s2++;
            d=tolower(*s1)-tolower(*s2);
         }
         return(d);
      }
   }
}

