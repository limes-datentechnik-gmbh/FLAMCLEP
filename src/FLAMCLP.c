/**
 * @file   FLAMCLP.c
 * @brief  LIMES Command Line Parser in ANSI-C
 *
 * LIMES Command Line Executor (CLE) in ANSI-C
 * @author limes datentechnik gmbh
 * @date  06.03.2015
 * @copyright (c) 2015 limes datentechnik gmbh
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

/* Standard-Includes **************************************************/

#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>

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
 * 1.1.10: Add new flag to prevent print outs in clear form for passwords or other critical information
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
 * 1.1.34: Get properties from environment variables and property files working
 * 1.1.35: Correct error position (source, row, column) if property parsing used
 * 1.1.36: Use snprintf() instead of sprintf() for static array strings
 * 1.1.37: Support file name mapping (+/<Cuser>)
 * 1.1.38: Use GETENV() makro
 * 1.1.39: Rework all format strings (replace "\'" with "'" for better readability)
 * 1.1.40: Print synopsis at help if keyword man is used
 * 1.1.41: Support OID as default for numbers if CLPFLG_DEF defined (if only the keyword used (DECODE))
 * 1.1.42: Correct order (scan new token as last step) to fix issue 614
 * 1.1.43: fix null pointer references
 * 1.1.44: Code page specific interpretation of punctuation characters on EBCDIC systems
 * 1.1.45: Replace unnecessary strlen()
 * 1.1.46: Change "###SECRET###" in "***SECRET***" to eliminate dia-critical characters
 * 1.1.47: Make qualifier for commands variable in ClpDocu
 * 1.1.48: Support dia-critical characters in string lexems (add new macro isStr())
 * 1.1.49: Fix issue 684: correct length in error messages (longer then n)
 * 1.1.50: Support replacement of &{OWN} and &{PGM} in man pages
 * 1.1.51: Fix cut & paste error at syntax  error print out
 * 1.1.52: Add symbol table walk and update functions
 * 1.1.53: Change flag (isSet) to method (siMtd) to better define property printing
 * 1.1.54: Support links in overlay of overlays
 * 1.1.55: Fix CLEP lexical error message "Character ('%c') not valid"
 * 1.1.56: Fix build scan issue: Access to field 'psDep' results in a dereference of a null pointer (loaded from variable 'psArg')
 * 1.1.57: Support filename type for strings to read passwords from files (f'pwdfile.txt') - string file support
 * 1.1.58: Print time string for time entry in parsed parameter list
 * 1.1.59: Support shorted time entries (0t2015, 0t2015/04/01, 0t2015/04/02.23:13)
 * 1.1.60: Fix wrong hour at time entry if daylight saving time used
 * 1.1.62: Support optional headline in the first level of docu generation
 * 1.1.63: Accept decimal number as a float if it is a integer and the expected type is float
 * 1.1.64: Make acPro and acSrc dynamic to reduce memory consumption of symbol table
 * 1.1.65: Support also grave (` - 0x60) to enclose strings
 * 1.1.66: Add new flag bit to separate default from defined properties
 * 1.1.67: Correct type of variable t to time_t to get correct time on z/OS
 * 1.1.68: Correct relative time entry (use localtime() instead of gmtime() for mktime())
 * 1.1.69: Don't add OID to array of OIDs if overlay of overlay used if OID==0
 * 1.1.70: Add info function to print help message for a path
 * 1.1.71: Add CLPPRO_MTD_DOC for property print out
 * 1.1.72: Don't print if print file NULL
 * 1.2.73: Make acLst dynamic (pcLst, introduce srprintc)
 * 1.2.74: Separate CLPMAX_KYW/SRCSIZ from CLPMAX_LEXSIZ (introduce srprintf, limit key word length to a maximum of 63)
 * 1.2.75: Rename pcSrc in pcInp and make source qualifier dynamic
 * 1.2.76: Make lexem dynamic in length
 * 1.2.77: Make prefix and path dynamic in length
 * 1.2.78: Make message dynamic in length
 * 1.2.79: Add parameter file support for arrays
 * 1.2.80: Support command string replacements by environment variables (<HOME>)
 * 1.2.81: Support constant expression (blksiz=64*KiB)
 * 1.2.82: Read siNow from environment variable
 * 1.2.83: Support CLPFLG_TIM to mark numbers as time values
 * 1.2.84: Support expression including symbol keywords which are defined (FROM=NOW-10Day TO=FROM[0]+5DAY)
 * 1.2.85: Add new CLPFLG_DLM to ensure an additional element as delimiter in arrays (required if CLPFLG_CNT cannot used (overlay with arrays))
 * 1.2.86: Support dynamic strings and arrays as new flag CLPFLG_DYN
 * 1.2.87: Support string mapping functions for build of CLP structure
 * 1.2.88: Make remaining parameter file names dynamic
 * 1.2.89: Check if keyword and alias contain only valid letters
 * 1.2.90: Use realloc_nowarn macro for realloc() to give the possibility to use own defines for it
 * 1.2.91: Support separation of signed and unsigned numbers over a new flag
 * 1.2.92: Support type string to determine unsigned flag
 * 1.2.93: Support literal or static variable assignments for dynamic values in CLP structure
 * 1.2.94: Reduce memory of symbol table (don't store pcAli use psAli instead)
 * 1.2.95: Add new link to get the index (byte offset) of the current key word in the CLP string
 * 1.2.96: Set locale to "C" close to strtod (remove from open and close functions)
**/

#define CLP_VSN_STR       "1.2.96"
#define CLP_VSN_MAJOR      1
#define CLP_VSN_MINOR        2
#define CLP_VSN_REVISION       96

/* Definition der Konstanten ******************************************/

#define CLPMAX_TABCNT            256
#define CLPMAX_HDEPTH            128
#define CLPMAX_KYWLEN            63
#define CLPMAX_KYWSIZ            64
#define CLPMAX_BUFCNT            32

#define CLPINI_LEXSIZ            1024
#define CLPINI_LSTSIZ            1024
#define CLPINI_SRCSIZ            1024
#define CLPINI_MSGSIZ            1024
#define CLPINI_PRESIZ            1024
#define CLPINI_PATSIZ            1024
#define CLPINI_VALSIZ            128
#define CLPINI_PTRCNT            128

#define CLPTOK_INI               0
#define CLPTOK_END               1
#define CLPTOK_KYW               2
#define CLPTOK_RBO               3
#define CLPTOK_RBC               4
#define CLPTOK_SBO               5
#define CLPTOK_SBC               6
#define CLPTOK_SGN               7
#define CLPTOK_DOT               8
#define CLPTOK_ADD               9
#define CLPTOK_SUB               10
#define CLPTOK_MUL               11
#define CLPTOK_DIV               12
#define CLPTOK_STR               13
#define CLPTOK_NUM               14
#define CLPTOK_FLT               15

#define isPrnInt(p,v) (CLPISF_PWD(p->psStd->uiFlg)?((I64)0):(v))
#define isPrnFlt(p,v) (CLPISF_PWD(p->psStd->uiFlg)?((F64)0.0):(v))
#define isPrnStr(p,v) (CLPISF_PWD(p->psStd->uiFlg)?("***SECRET***"):(v))
#define isPrnLen(p,v) (CLPISF_PWD(p->psStd->uiFlg)?((int)0):(v))

#define GETALI(sym) (((sym)->psStd->psAli!=NULL)?(sym)->psStd->psAli->psStd->pcKyw:NULL)

#ifndef realloc_nowarn
#  define realloc_nowarn      realloc
#endif

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
      "ADD",
      "SUB",
      "MUL",
      "DIV",
      "STRING",
      "NUMBER",
      "FLOAT"};

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
   struct Sym*                   psAli;
   unsigned int                  uiFlg;
   int                           siKwl;
   int                           siLev;
   int                           siPos;
}TsStd;

typedef struct Fix {
   const char*                   pcDft;
   const char*                   pcMan;
   const char*                   pcHlp;
   char*                         pcPro;
   int                           siTyp;
   int                           siMin;
   int                           siMax;
   int                           siSiz;
   int                           siOfs;
   int                           siOid;
   struct Sym*                   psLnk;
   struct Sym*                   psCnt;
   struct Sym*                   psOid;
   struct Sym*                   psInd;
   struct Sym*                   psEln;
   struct Sym*                   psSln;
   struct Sym*                   psTln;
   char*                         pcSrc;
   int                           siRow;
}TsFix;

typedef struct Var {
   void*                         pvDat;
   void*                         pvPtr;
   int                           siCnt;
   int                           siLen;
   int                           siRst;
   int                           siInd;
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

typedef struct Ptr {
   void*                         pvPtr;
   int                           siSiz;
} TsPtr;

typedef struct Hdl {
   const char*                   pcOwn;
   const char*                   pcPgm;
   const char*                   pcCmd;
   const char*                   pcMan;
   const char*                   pcHlp;
   const char*                   pcInp;
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
   int                           isEnv;
   int                           siTok;
   int                           isSep;
   size_t                        szLex;
   char*                         pcLex;
   size_t                        szSrc;
   char*                         pcSrc;
   TsSym*                        psTab;
   TsSym*                        psSym;
   TsSym*                        psOld;
   void*                         pvDat;
   FILE*                         pfHlp;
   FILE*                         pfErr;
   FILE*                         pfSym;
   FILE*                         pfScn;
   FILE*                         pfPrs;
   FILE*                         pfBld;
   const TsSym*                  apPat[CLPMAX_HDEPTH];
   size_t                        szMsg;
   char*                         pcMsg;
   size_t                        szPre;
   char*                         pcPre;
   size_t                        szPat;
   char*                         pcPat;
   size_t                        szLst;
   char*                         pcLst;
   int                           siBuf;
   size_t                        pzBuf[CLPMAX_BUFCNT];
   char*                         apBuf[CLPMAX_BUFCNT];
   int                           siRow;
   int                           siCol;
   int                           siErr;
   I64                           siNow;
   I64                           siRnd;
   int                           siPtr;
   int                           szPtr;
   TsPtr*                        psPtr;
   const TsSym*                  psVal;
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
   size_t*                       pzLex,
   char**                        ppLex,
   int                           siTyp,
   const TsSym*                  psArg,
   int*                          piSep,
   const TsSym**                 ppVal);

static int siClpScnSrc(
   void*                         pvHdl,
   int                           siTyp,
   const TsSym*                  psArg);

static int siClpConNat(
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   size_t*                       pzLex,
   char**                        ppLex,
   const int                     siTyp,
   const TsSym*                  psArg);

static int siClpConSrc(
   void*                         pvHdl,
   const int                     isChk,
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

static int siClpPrsNum(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

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
   const int                     isAry,
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
   const int                     isAry,
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
   size_t*                       pzPat,
   char**                        ppPat);

static int siClpBldPro(
   void*                         pvHdl,
   const char*                   pcPat,
   const char*                   pcPro,
   const int                     siRow);

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

static int siClpBldNum(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpBldLit(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal);

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
   const unsigned int            isSel,
   const unsigned int            isCon);

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

static int siFromNumberLexem(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal,
   I64*                          piVal);

static int siFromFloatLexem(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal,
   double*                       pfVal);

static const char* fpcPre(
   void*                         pvHdl,
   const int                     siLev);

static const char* fpcPat(
   void*                         pvHdl,
   const int                     siLev);

static inline int CLPERR(TsHdl* psHdl,int siErr, char* pcMsg, ...) {
   const char*          p;
   va_list              argv;
   int                  i,l,f=FALSE;
   const char*          pcErr=pcClpErr(siErr);
   char                 acMsg[1024];
   va_start(argv,pcMsg); vsnprintf(acMsg,sizeof(acMsg),pcMsg,argv); va_end(argv);
   srprintf(&psHdl->pcMsg,&psHdl->szMsg,strlen(pcErr)+strlen(acMsg),"%s: %s",pcErr,acMsg);
   psHdl->siErr=siErr;
   if (psHdl->pcRow!=NULL && psHdl->pcOld>=psHdl->pcRow) {
      psHdl->siCol=(int)((psHdl->pcOld-psHdl->pcRow)+1);
   } else psHdl->siCol=0;
   if (psHdl->pfErr!=NULL) {
      fprintf(psHdl->pfErr,"%s:\n%s %s\n",pcErr,fpcPre(psHdl,0),acMsg);
      if (psHdl->pcSrc!=NULL && psHdl->pcInp!=NULL && psHdl->pcRow!=NULL && psHdl->pcOld!=NULL && psHdl->pcCur!=NULL && (psHdl->pcCur>psHdl->pcInp || psHdl->pcLst!=NULL || psHdl->siRow)) {
         if (strcmp(psHdl->pcSrc,CLPSRC_CMD)==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from command line\n",                  fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol);
         } else if (strcmp(psHdl->pcSrc,CLPSRC_PRO)==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from property list\n",                 fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol);
         } else if (strncmp(psHdl->pcSrc,CLPSRC_ENV,strlen(CLPSRC_ENV))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from environment variable '%s'\n",     fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc+strlen(CLPSRC_ENV));
         } else if (strncmp(psHdl->pcSrc,CLPSRC_DEF,strlen(CLPSRC_DEF))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from default value of argument '%s'\n",fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc+strlen(CLPSRC_DEF));
         } else if (strncmp(psHdl->pcSrc,CLPSRC_PRF,strlen(CLPSRC_PRF))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d supplement from property file '%s'\n", fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc+strlen(CLPSRC_PRF));
         } else if (strncmp(psHdl->pcSrc,CLPSRC_PAF,strlen(CLPSRC_PAF))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from parameter file '%s'\n",           fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc+strlen(CLPSRC_PAF));
         } else if (strncmp(psHdl->pcSrc,CLPSRC_SRF,strlen(CLPSRC_SRF))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from string file '%s'\n",              fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc+strlen(CLPSRC_SRF));
         } else if (strncmp(psHdl->pcSrc,CLPSRC_CMF,strlen(CLPSRC_CMF))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from command file '%s'\n",             fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc+strlen(CLPSRC_CMF));
         } else {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d in file '%s'\n",                       fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc);
         }
         if (psHdl->pcRow!=NULL) {
            fprintf(psHdl->pfErr,"%s \"",fpcPre(psHdl,1));
            for (p=psHdl->pcRow;!iscntrl(*p);p++) fprintf(psHdl->pfErr,"%c",*p);
            fprintf(psHdl->pfErr,"\"\n");
            if (psHdl->pcCur==psHdl->pcRow) {
               fprintf(psHdl->pfErr,"%s %c",fpcPre(psHdl,1),C_CRT);
            } else {
               fprintf(psHdl->pfErr,"%s  ",fpcPre(psHdl,1));
            }
            for (p=psHdl->pcRow;!iscntrl(*p);p++) {
               if (p>=psHdl->pcOld && p<psHdl->pcCur) {
                  f=TRUE;
                  fprintf(psHdl->pfErr,"%c",C_CRT);
               } else {
                  fprintf(psHdl->pfErr," ");
               }
            }
            if (f) {
               fprintf(psHdl->pfErr," \n");
            } else {
               fprintf(psHdl->pfErr,"%c\n",C_CRT);
            }
         }
         l=(psHdl->pcLst!=NULL)?strlen(psHdl->pcLst):0;
         if (l>1) {
            l--;
            fprintf(psHdl->pfErr,"%s After successful parsing of arguments below:\n",fpcPre(psHdl,0));
            fprintf(psHdl->pfErr,"%s ",fpcPre(psHdl ,1));
            for (i=0;i<l;i++) {
               if (psHdl->pcLst[i]=='\n') {
                  fprintf(psHdl->pfErr,"\n%s ",fpcPre(psHdl,1));
               } else fprintf(psHdl->pfErr,"%c",psHdl->pcLst[i]);
            }
            fprintf(psHdl->pfErr,"\n");
         } else fprintf(psHdl->pfErr,"%s Something is wrong with the first argument\n",fpcPre(psHdl,0));
      }
   }
   return(siErr);
}

static inline void CLPERRADD(TsHdl* psHdl,int siLev, char* pcMsg, ...) {
   if (psHdl->pfErr!=NULL) {
      va_list              argv;
      fprintf(psHdl->pfErr,"%s ",fpcPre(psHdl,siLev));
      va_start(argv,pcMsg); vfprintf(psHdl->pfErr,pcMsg,argv); va_end(argv);
      fprintf(psHdl->pfErr,"\n");
   }
}

static inline I64 ClpRndFnv(const I64 siRnd)
{
   unsigned char* p=(unsigned char*)&siRnd;
   U64            h=0xcbf29ce48422232LLU;
   h^=p[0]; h*=0x100000001b3LLU;
   h^=p[1]; h*=0x100000001b3LLU;
   h^=p[2]; h*=0x100000001b3LLU;
   h^=p[3]; h*=0x100000001b3LLU;
   h^=p[4]; h*=0x100000001b3LLU;
   h^=p[5]; h*=0x100000001b3LLU;
   h^=p[6]; h*=0x100000001b3LLU;
   h^=p[7]; h*=0x100000001b3LLU;
   return h;
}

extern void* pvClpAlloc(
   void*                         pvHdl,
   void*                         pvPtr,
   int                           siSiz,
   int*                          piInd)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (pvPtr==NULL) {
      if (siSiz>0) {
         if (psHdl->siPtr>=psHdl->szPtr) {
            void* pvHlp=realloc_nowarn(psHdl->psPtr,sizeof(TsPtr)*(psHdl->szPtr+CLPINI_PTRCNT));
            if (pvHlp==NULL) return(NULL);
            psHdl->psPtr=pvHlp;
            psHdl->szPtr+=CLPINI_PTRCNT;
         }
         pvPtr=calloc(1,siSiz);
         if (pvPtr!=NULL) {
            psHdl->psPtr[psHdl->siPtr].pvPtr=pvPtr;
            psHdl->psPtr[psHdl->siPtr].siSiz=siSiz;
            if (piInd!=NULL) *piInd=psHdl->siPtr;
            psHdl->siPtr++;
         }
      } else return(NULL);
   } else {
      if (piInd!=NULL && *piInd>=0 && *piInd<psHdl->siPtr && psHdl->psPtr[*piInd].pvPtr==pvPtr) {
         pvPtr=realloc_nowarn(pvPtr,siSiz);
         if (pvPtr!=NULL) {
            if (psHdl->psPtr[*piInd].siSiz<siSiz) {
               memset(((char*)pvPtr)+psHdl->psPtr[*piInd].siSiz,0,siSiz-psHdl->psPtr[*piInd].siSiz);
            }
            psHdl->psPtr[*piInd].pvPtr=pvPtr;
            psHdl->psPtr[*piInd].siSiz=siSiz;
         }
      } else {
         for (int i=0;i<psHdl->siPtr;i++) {
            if (psHdl->psPtr[i].pvPtr==pvPtr) {
               pvPtr=realloc_nowarn(pvPtr,siSiz);
               if (pvPtr!=NULL) {
                  if (psHdl->psPtr[i].siSiz<siSiz) {
                     memset(((char*)pvPtr)+psHdl->psPtr[i].siSiz,0,siSiz-psHdl->psPtr[i].siSiz);
                  }
                  psHdl->psPtr[i].pvPtr=pvPtr;
                  psHdl->psPtr[i].siSiz=siSiz;
                  if (piInd!=NULL) *piInd=i;
               }
               return(pvPtr);
            }
         }
         return(pvClpAlloc(pvHdl,NULL,siSiz,piInd));
      }
   }
   return(pvPtr);
}

static void vdClpFree(
   void*                         pvHdl)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (psHdl->psPtr!=NULL) {
      for (int i=0;i<psHdl->siPtr;i++) {
         if (psHdl->psPtr[i].pvPtr!=NULL) {
            free(psHdl->psPtr[i].pvPtr);
            psHdl->psPtr[i].pvPtr=NULL;
            psHdl->psPtr[i].siSiz=0;
         }
      }
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
   const int                     isEnv,
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
   const char*                   pcNow=NULL;
   I64                           siNow=0;
   int                           siErr,i;
   if (pcOwn!=NULL && pcPgm!=NULL && pcCmd!=NULL && psTab!=NULL) {
      psHdl=(TsHdl*)calloc(1,sizeof(TsHdl));
      if (psHdl!=NULL) {
         psHdl->isCas=isCas;
         psHdl->isPfl=isPfl;
         psHdl->isEnv=isEnv;
         psHdl->siMkl=siMkl;
         psHdl->pcOwn=pcOwn;
         psHdl->pcPgm=pcPgm;
         psHdl->pcCmd=pcCmd;
         psHdl->pcMan=pcMan;
         psHdl->pcHlp=pcHlp;
         psHdl->isOvl=isOvl;
         psHdl->pcInp=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->psPtr=NULL;
         psHdl->szLex=CLPINI_LEXSIZ;
         psHdl->pcLex=(C08*)calloc(1,psHdl->szLex);
         psHdl->szSrc=CLPINI_SRCSIZ;
         psHdl->pcSrc=(C08*)calloc(1,psHdl->szSrc);
         psHdl->szPre=CLPINI_PRESIZ;
         psHdl->pcPre=(C08*)calloc(1,psHdl->szPre);
         psHdl->szPat=CLPINI_PATSIZ;
         psHdl->pcPat=(C08*)calloc(1,psHdl->szPat);
         psHdl->szLst=CLPINI_LSTSIZ;
         psHdl->pcLst=(C08*)calloc(1,psHdl->szLst);
         psHdl->szMsg=CLPINI_MSGSIZ;
         psHdl->pcMsg=(C08*)calloc(1,psHdl->szMsg);
         for (i=0;i<CLPMAX_BUFCNT;i++) {
            psHdl->pzBuf[i]=0;
            psHdl->apBuf[i]=NULL;
         }
         psHdl->pvDat=pvDat;
         psHdl->psTab=NULL;
         psHdl->psSym=NULL;
         psHdl->psOld=NULL;
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
         siErr=siClpSymIni(psHdl,0,NULL,psTab,NULL,&psHdl->psTab);
         if (siErr<0) { vdClpSymDel(psHdl->psTab); free(psHdl); return(NULL); }
         siErr=siClpSymCal(psHdl,0,NULL,psHdl->psTab);
         if (siErr<0) { vdClpSymDel(psHdl->psTab); free(psHdl); return(NULL); }
         vdClpSymTrc(psHdl);
         if (psErr!=NULL) {
            psErr->ppMsg=(const char**)&psHdl->pcMsg;
            psErr->ppSrc=(const char**)&psHdl->pcSrc;
            psErr->piRow=&psHdl->siRow;
            psErr->piCol=&psHdl->siCol;
         }
         psHdl->siNow=time(NULL);
         srand(psHdl->siNow+clock());
         psHdl->siRnd=ClpRndFnv(rand()+clock());
         pcNow=GETENV("CLP_NOW");
         if (pcNow!=NULL && *pcNow) {
            siErr=siClpScnNat(psHdl,psHdl->pfErr,psHdl->pfScn,&pcNow,&psHdl->szLex,&psHdl->pcLex,CLPTYP_NUMBER,NULL,NULL,NULL);
            if (siErr==CLPTOK_NUM) {
               siErr=siFromNumberLexem(psHdl,0,0,NULL,psHdl->pcLex,&siNow);
               if (siErr==0 && siNow>0) {
                  siErr=siClpScnNat(psHdl,psHdl->pfErr,psHdl->pfScn,&pcNow,&psHdl->szLex,&psHdl->pcLex,0,NULL,NULL,NULL);
                  if (siErr==CLPTOK_END) {
                     psHdl->siNow=siNow;
                  }
               }
            }
         }
         psHdl->siTok=CLPTOK_INI;
      } else {
         if (pfErr!=NULL) fprintf(pfErr,"Allocation of CLP structure failed\n");
      }
   } else {
      if (pfErr!=NULL) fprintf(pfErr,"One or more parameter pcOwn(%p), pcPgm(%p), pcCmd(%p) or psTab(%p) are NULL\n",pcOwn,pcPgm,pcCmd,psTab);
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

   if (pcPro==NULL)
      return CLPERR(psHdl,CLPERR_INT,"Property string is NULL");

   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   if (pcSrc!=NULL && *pcSrc) {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_PRF)+strlen(pcSrc),"%s%s",CLPSRC_PRF,pcSrc);
   } else {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_PRO),"%s",CLPSRC_PRO);
   }
   psHdl->pcInp=pcPro;
   psHdl->pcCur=pcPro;
   psHdl->pcOld=pcPro;
   psHdl->pcRow=pcPro;
   psHdl->isChk=isChk;
   psHdl->siRow=1;
   psHdl->siCol=0;
   psHdl->pcLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PROPERTY-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
      if (psHdl->siTok<0) return(psHdl->siTok);
      siCnt=siClpPrsProLst(pvHdl,psHdl->psTab);
      if (siCnt<0) return(siCnt);
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcInp=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->pcLex[0]=EOS;
         psHdl->isChk=FALSE;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PROPERTY-PARSER-END(CNT=%d)\n",siCnt);
         if (ppLst!=NULL) *ppLst=psHdl->pcLst;
         return(siCnt);
      } else {
         if (ppLst!=NULL) *ppLst=psHdl->pcLst;
         return CLPERR(psHdl,CLPERR_SYN,"Last token (%s) of property list is not EOS",apClpTok[psHdl->siTok]);
      }
   } else {
      if (ppLst!=NULL) *ppLst=psHdl->pcLst;
      return CLPERR(psHdl,CLPERR_SYN,"Initial token (%s) in handle is not valid",apClpTok[psHdl->siTok]);
   }
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

   if (pcCmd==NULL)
      return CLPERR(psHdl,CLPERR_INT,"Command string is NULL");

   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   if (pcSrc!=NULL && *pcSrc) {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_CMF)+strlen(pcSrc),"%s%s",CLPSRC_CMF,pcSrc);
   } else {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_CMD),"%s",CLPSRC_CMD);
   }
   psHdl->pcInp=pcCmd;
   psHdl->pcCur=pcCmd;
   psHdl->pcOld=pcCmd;
   psHdl->pcRow=pcCmd;
   psHdl->isChk=isChk;
   psHdl->siRow=1;
   psHdl->siCol=0;
   psHdl->pcLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"COMMAND-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
      if (psHdl->siTok<0) return(psHdl->siTok);
      siCnt=siClpPrsMain(pvHdl,psHdl->psTab,piOid);
      if (siCnt<0) return (siCnt);
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcInp=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->pcLex[0]=EOS;
         psHdl->isChk=FALSE;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"COMMAND-PARSER-END(CNT=%d)\n",siCnt);
         if (ppLst!=NULL) *ppLst=psHdl->pcLst;
         return(siCnt);
      } else {
         if (ppLst!=NULL) *ppLst=psHdl->pcLst;
         return CLPERR(psHdl,CLPERR_SYN,"Last token (%s) of parameter list is not EOS",apClpTok[psHdl->siTok]);
      }
   } else {
      if (ppLst!=NULL) *ppLst=psHdl->pcLst;
      return CLPERR(psHdl,CLPERR_SYN,"Initial token (%s) in handle is not valid",apClpTok[psHdl->siTok]);
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
   TsSym*                        psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_KYWSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);

   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":SYNTAX:");

   if (pcPat!=NULL && *pcPat) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         if (strlen(pcPat)>l && pcPat[l]!='.') {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
         }
         for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
            for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) acKyw[i]=pcKyw[i];
            acKyw[i]=EOS;
            siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
            if (siErr<0) return(siErr);
            psHdl->apPat[siLev]=psArg;
            psTab=psArg->psDep;
         }
         if (psTab!=NULL && !CLPISF_CON(psTab->psStd->uiFlg)) {
            siErr=siClpPrnCmd(pvHdl,psHdl->pfHlp,0,siLev,siLev+siDep,psArg,psTab,isSkr,isMin);
            if (siErr<0) return (siErr);
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains too many or invalid qualifiers",pcPat);
         }
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
      }
   } else {
      siErr=siClpPrnCmd(pvHdl,psHdl->pfHlp,0,0,siDep,NULL,psHdl->psTab,isSkr,isMin);
      if (siErr<0) return (siErr);
   }
   if (isSkr && psHdl->pfHlp!=NULL) fprintf(psHdl->pfHlp,"\n");
   return(CLP_OK);
}


extern const char* pcClpInfo(
   void*                         pvHdl,
   const char*                   pcPat)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_KYWSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);

   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":INFO:");

   if (pcPat!=NULL && *pcPat) {
      if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         if (strlen(pcPat)<=l || pcPat[l]=='.') {
            for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
               for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) acKyw[i]=pcKyw[i];
               acKyw[i]=EOS;
               siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
               if (siErr<0) return("");
               psHdl->apPat[siLev]=psArg;
               psTab=psArg->psDep;
            }
            if (psArg!=NULL) return(psArg->psFix->pcHlp);
         }
      }
   }
   return("");
}

extern int siClpHelp(
   void*                         pvHdl,
   const int                     siDep,
   const char*                   pcPat,
   const int                     isAli,
   const int                     isMan)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_KYWSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);

   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":HELP:");

   if (psHdl->pfHlp!=NULL) {
      if (pcPat!=NULL && *pcPat) {
         if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
            if (strlen(pcPat)>l && pcPat[l]!='.') {
               return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
            }
            for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
               for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) acKyw[i]=pcKyw[i];
               acKyw[i]=EOS;
               siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
               if (siErr<0) return(siErr);
               psHdl->apPat[siLev]=psArg;
               psTab=psArg->psDep;
            }
            if (psTab!=NULL) {
               if (siDep==0) {
                  if (psArg==NULL) {
                     fprintf(psHdl->pfHlp,   "SYNOPSIS\n");
                     fprintf(psHdl->pfHlp,   "--------\n");
                     efprintf(psHdl->pfHlp,  "HELP:   %s\n",psHdl->pcHlp);
                     fprintf(psHdl->pfHlp,   "PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
                     if (psHdl->isOvl) {
                        fprintf(psHdl->pfHlp,"TYPE:   OVERLAY\n");
                     } else {
                        fprintf(psHdl->pfHlp,"TYPE:   OBJECT\n");
                     }
                     fprintf(psHdl->pfHlp,   "SYNTAX: :> %s ",psHdl->pcPgm);
                     siErr=siClpPrnCmd(pvHdl,psHdl->pfHlp,0,0,1,NULL,psHdl->psTab,FALSE,FALSE);
                     if (siErr<0) return(siErr);
                     fprintf(psHdl->pfHlp,"\n\n");
                     fprintf(psHdl->pfHlp,"DESCRIPTION\n");
                     fprintf(psHdl->pfHlp,"-----------\n");
                     if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
                        fprintm(psHdl->pfHlp,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcMan,1);
                     } else {
                        fprintf(psHdl->pfHlp,"No detailed description available for this command.\n\n");
                     }
                  } else {
                     fprintf(psHdl->pfHlp, "SYNOPSIS\n");
                     fprintf(psHdl->pfHlp, "--------\n");
                     efprintf(psHdl->pfHlp,"HELP:   %s\n",psArg->psFix->pcHlp);
                     fprintf(psHdl->pfHlp, "PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                     fprintf(psHdl->pfHlp, "TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                     fprintf(psHdl->pfHlp, "SYNTAX: ");
                     siErr=siClpPrnSyn(pvHdl,psHdl->pfHlp,FALSE,siLev-1,psArg);
                     fprintf(psHdl->pfHlp,"\n\n");
                     if (siErr<0) return(siErr);
                     fprintf(psHdl->pfHlp, "DESCRIPTION\n");
                     fprintf(psHdl->pfHlp, "-----------\n");
                     if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                        efprintf(psHdl->pfHlp,"%s\n",psArg->psFix->pcMan);
                     } else {
                        fprintf(psHdl->pfHlp,"No detailed description available for this argument.\n\n");
                     }
                  }
               } else {
                  if (psArg!=NULL) {
                     if (psArg->psFix->siTyp==CLPTYP_OBJECT || psArg->psFix->siTyp==CLPTYP_OVRLAY) {
                        siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,-1,psTab,FALSE);
                        if (siErr<0) return(siErr);
                     } else {
                        if (CLPISF_SEL(psArg->psStd->uiFlg)) {
                           siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,psArg->psFix->siTyp,psTab,FALSE);
                           if (siErr<0) return(siErr);
                        } else {
                           siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,psArg->psFix->siTyp,psTab,TRUE);
                           if (siErr<0) return(siErr);
                        }
                     }
                  } else {
                     siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,-1,psTab,FALSE);
                     if (siErr<0) return(siErr);
                  }
               }
            } else {
               if (isMan) {
                  if (psArg!=NULL) {
                     fprintf(psHdl->pfHlp, "SYNOPSIS\n");
                     fprintf(psHdl->pfHlp, "--------\n");
                     efprintf(psHdl->pfHlp,"HELP:   %s\n",psArg->psFix->pcHlp);
                     fprintf(psHdl->pfHlp, "PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                     fprintf(psHdl->pfHlp, "TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                     fprintf(psHdl->pfHlp, "SYNTAX: ");
                     siErr=siClpPrnSyn(pvHdl,psHdl->pfHlp,FALSE,siLev-1,psArg);
                     fprintf(psHdl->pfHlp, "\n\n");
                     if (siErr<0) return(siErr);
                     fprintf(psHdl->pfHlp, "DESCRIPTION\n");
                     fprintf(psHdl->pfHlp, "-----------\n");
                     if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                        fprintm(psHdl->pfHlp,psHdl->pcOwn,psHdl->pcPgm,psArg->psFix->pcMan,1);
                     } else {
                        fprintf(psHdl->pfHlp,"No detailed description available for this argument.\n\n");
                     }
                  } else {
                     fprintf(psHdl->pfHlp,"No detailed description available for this argument.\n\n");
                  }
               } else {
                  fprintf(psHdl->pfHlp,"No further arguments available.\n");
               }
            }
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
         }
      } else {
         if (siDep==0) {
            fprintf(psHdl->pfHlp,   "SYNOPSIS\n");
            fprintf(psHdl->pfHlp,   "--------\n");
            efprintf(psHdl->pfHlp,  "HELP:   %s\n",psHdl->pcHlp);
            fprintf(psHdl->pfHlp,   "PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
            if (psHdl->isOvl) {
               fprintf(psHdl->pfHlp,"TYPE:   OVERLAY\n");
            } else {
               fprintf(psHdl->pfHlp,"TYPE:   OBJECT\n");
            }
            fprintf(psHdl->pfHlp,   "SYNTAX: :> %s ",psHdl->pcPgm);
            siErr=siClpPrnCmd(pvHdl,psHdl->pfHlp,0,0,1,NULL,psHdl->psTab,FALSE,FALSE);
            fprintf(psHdl->pfHlp,"\n\n");
            if (siErr<0) return(siErr);
            fprintf(psHdl->pfHlp,"DESCRIPTION\n");
            fprintf(psHdl->pfHlp,"-----------\n");
            if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
               fprintm(psHdl->pfHlp,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcMan,1);
            } else {
               fprintf(psHdl->pfHlp,"No detailed description available for this command.\n\n");
            }
         } else {
            siErr=siClpPrnHlp(pvHdl,psHdl->pfHlp,isAli,0,siDep,-1,psTab,FALSE);
            if (siErr<0) return(siErr);
         }
      }
   }
   return(CLP_OK);
}

extern int siClpDocu(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const char*                   pcPat,
   const char*                   pcHdl,
   const char*                   pcNum,
   const char*                   pcCmd,
   const int                     isDep,
   const int                     isMan,
   const int                     isNbr)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":DOCU:");

   if (pcNum!=NULL && strlen(pcNum)<100 && pcCmd!=NULL) {
      if (pfDoc!=NULL) {
         TsSym*                        psTab=psHdl->psTab;
         TsSym*                        psArg=NULL;
         char*                         pcPtr=NULL;
         char*                         pcKyw=NULL;
         char                          acKyw[CLPMAX_KYWSIZ];
         int                           siErr=0,siLev,siPos,i;
         const char*                   pcSta=(pcHdl!=NULL&&*pcHdl)?pcHdl:psHdl->pcCmd;
         int                           l=strlen(psHdl->pcCmd);
         char                          acNum[strlen(pcNum)+10];
         char                          acArg[20];
         const char*                   p;
         if (pcPat!=NULL && *pcPat) {
            if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
               if (strlen(pcPat)>l && pcPat[l]!='.') {
                  return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
               }
               if (strlen(pcPat)>l) {
                  strlcpy(acNum,pcNum,sizeof(acNum));
                  for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
                     for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) acKyw[i]=pcKyw[i];
                     acKyw[i]=EOS;
                     siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,&siPos);
                     if (siErr<0) return(siErr);
                     psHdl->apPat[siLev]=psArg;
                     psTab=psArg->psDep;
                     snprintc(acNum,sizeof(acNum),"%d.",siPos+1);
                  }
                  if (psArg!=NULL) {
                     if (CLPISF_ARG(psArg->psStd->uiFlg)) {
                        if (isMan) {
                           if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                              for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                              fprintf(pfDoc,".");
                              l=strlen(psHdl->pcPgm)+strlen(fpcPat(pvHdl,siLev))+4;
                           } else {
                              l=strlen(fpcPat(pvHdl,siLev))+3;
                           }
                           for (p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           fprintf(pfDoc,"(3)\n");
                           for (i=0;i<l;i++) fprintf(pfDoc,"=");
                           fprintf(pfDoc,"\n");
                           fprintf(pfDoc, ":doctype: manpage\n\n");
                           fprintf(pfDoc, "NAME\n");
                           fprintf(pfDoc, "----\n\n");
                           if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                              for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                              fprintf(pfDoc,".");
                              for (p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           } else {
                               vdClpPrnAli(pfDoc,", ",psArg);
                           }
                           efprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
                           fprintf(pfDoc, "SYNOPSIS\n");
                           fprintf(pfDoc, "--------\n\n");
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                           fprintf(pfDoc, "PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                           fprintf(pfDoc, "TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                           fprintf(pfDoc, "SYNTAX: "); siErr=siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev-1,psArg); fprintf(pfDoc,"\n");
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                           if (siErr<0) return(siErr);
                           fprintf(pfDoc, "DESCRIPTION\n");
                           fprintf(pfDoc, "-----------\n\n");
                           if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                              fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psArg->psFix->pcMan,2);
                           } else {
                              fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                           }
                           fprintf(pfDoc, "AUTHOR\n");
                           fprintf(pfDoc, "------\n\n");
                           fprintf(pfDoc, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
                        } else {
                           switch (psArg->psFix->siTyp){
                           case CLPTYP_OBJECT:strcpy(acArg,"OBJECT");break;
                           case CLPTYP_OVRLAY:strcpy(acArg,"OVERLAY");break;
                           default           :strcpy(acArg,"PARAMETER");break;
                           }
                           if (isNbr) {
                              fprintf(pfDoc,"%s %s '%s'\n",acNum,acArg,psArg->psStd->pcKyw);
                              l=strlen(acNum)+strlen(acArg)+strlen(psArg->psStd->pcKyw)+4;
                              for (i=0;i<l;i++) fprintf(pfDoc,"%c",C_CRT);
                              fprintf(pfDoc,"\n\n");
                           } else {
                              fprintf(pfDoc,"%s '%s'\n",acArg,psArg->psStd->pcKyw);
                              l=strlen(acArg)+strlen(psArg->psStd->pcKyw)+3;
                              for (i=0;i<l;i++) fprintf(pfDoc,"%c",C_CRT);
                              fprintf(pfDoc,"\n\n");
                           }
                           fprintf(pfDoc, ".SYNOPSIS\n\n");
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                           efprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
                           fprintf(pfDoc, "PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                           fprintf(pfDoc, "TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                           fprintf(pfDoc, "SYNTAX: "); siErr=siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev-1,psArg); fprintf(pfDoc,"\n");
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                           if (siErr<0) return(siErr);
                           fprintf(pfDoc,".DESCRIPTION\n\n");
                           if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                              fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psArg->psFix->pcMan,2);
                           } else {
                              fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                           }
                           fprintf(pfDoc,"indexterm:%cArgument %s%c\n\n\n",C_SBO,psArg->psStd->pcKyw,C_SBC);
                           if (isDep) {
                              siErr=siClpPrnDoc(pvHdl,pfDoc,siLev,isNbr,acNum,psArg,psTab);
                              if (siErr<0) return(siErr);
                           }
                        }
                     } else if (CLPISF_CON(psArg->psStd->uiFlg)) {
                        if (isMan) {
                           if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                              for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                              fprintf(pfDoc,".");
                              l=strlen(psHdl->pcPgm)+strlen(fpcPat(pvHdl,siLev))+4;
                           } else {
                              l=strlen(fpcPat(pvHdl,siLev))+3;
                           }
                           for(p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           fprintf(pfDoc,"(3)\n");
                           for (i=0;i<l;i++) fprintf(pfDoc,"=");
                           fprintf(pfDoc,"\n");
                           fprintf(pfDoc, ":doctype: manpage\n\n");
                           fprintf(pfDoc, "NAME\n");
                           fprintf(pfDoc, "----\n\n");
                           if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                              for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                              fprintf(pfDoc,".");
                              for (p=fpcPat(pvHdl,siLev);*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           } else {
                              for (p=psArg->psStd->pcKyw;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                           }
                           efprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
                           fprintf(pfDoc, "SYNOPSIS\n");
                           fprintf(pfDoc, "--------\n\n");
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                           fprintf(pfDoc, "PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                           fprintf(pfDoc, "TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                           fprintf(pfDoc, "SYNTAX: %s\n",psArg->psStd->pcKyw);
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                           if (siErr<0) return(siErr);
                           fprintf(pfDoc, "DESCRIPTION\n");
                           fprintf(pfDoc, "-----------\n\n");
                           if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                              fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psArg->psFix->pcMan,2);
                           } else {
                              fprintf(pfDoc,"No detailed description available for this constant.\n\n");
                           }
                           fprintf(pfDoc, "AUTHOR\n");
                           fprintf(pfDoc, "------\n\n");
                           fprintf(pfDoc, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
                        } else {
                           if (isNbr) {
                              fprintf(pfDoc,"%s CONSTANT '%s'\n",acNum,psArg->psStd->pcKyw);
                              l=strlen(acNum)+strlen(psArg->psStd->pcKyw)+12;
                              for (i=0;i<l;i++) fprintf(pfDoc,"+");
                              fprintf(pfDoc,"\n\n");
                           } else {
                              fprintf(pfDoc,"CONSTANT '%s'\n",psArg->psStd->pcKyw);
                              l=strlen(psArg->psStd->pcKyw)+11;
                              for (i=0;i<l;i++) fprintf(pfDoc,"+");
                              fprintf(pfDoc,"\n\n");
                           }
                           fprintf(pfDoc, ".SYNOPSIS\n\n");
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                           efprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
                           fprintf(pfDoc, "PATH:   %s\n",fpcPat(pvHdl,siLev-1));
                           fprintf(pfDoc, "TYPE:   %s\n",apClpTyp[psArg->psFix->siTyp]);
                           fprintf(pfDoc, "SYNTAX: %s\n",psArg->psStd->pcKyw);
                           fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                           if (siErr<0) return(siErr);
                           fprintf(pfDoc,".DESCRIPTION\n\n");
                           if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                              fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psArg->psFix->pcMan,2);
                           } else {
                              fprintf(pfDoc,"No detailed description available for this constant.\n\n");
                           }
                           fprintf(pfDoc,"indexterm:%cConstant %s%c\n\n\n",C_SBO,psArg->psStd->pcKyw,C_SBC);
                        }
                     } else {
                        return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains too many or invalid qualifiers",pcPat);
                     }
                  } else {
                     return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains too many or invalid qualifiers",pcPat);
                  }
                  return(CLP_OK);
               }
            } else {
               return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
            }
         }
         if (isMan) {
            if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                fprintf(pfDoc,".");
                l=strlen(psHdl->pcPgm)+strlen(pcSta)+4;
            } else {
                l=strlen(pcSta)+3;
            }
            for (p=pcSta;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
            fprintf(pfDoc,   "(1)\n");
            for (i=0;i<l;i++) fprintf(pfDoc,"=");
            fprintf(pfDoc,"\n");
            fprintf(pfDoc,   ":doctype: manpage\n\n");
            fprintf(pfDoc,   "NAME\n");
            fprintf(pfDoc,   "----\n\n");
            if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
                for (p=psHdl->pcPgm;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
                fprintf(pfDoc,".");
            }
            for (p=pcSta;*p;p++) fprintf(pfDoc,"%c",tolower(*p));
            efprintf(pfDoc,  " - `%s`\n\n",psHdl->pcHlp);
            fprintf(pfDoc,   "SYNOPSIS\n");
            fprintf(pfDoc,   "--------\n\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n");
            fprintf(pfDoc,   "PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
            if (psHdl->isOvl) {
               fprintf(pfDoc,"TYPE:   OVERLAY\n");
            } else {
               fprintf(pfDoc,"TYPE:   OBJECT\n");
            }
            fprintf(pfDoc,   "SYNTAX: :> %s ",psHdl->pcPgm); siErr=siClpPrnCmd(pvHdl,pfDoc,0,0,1,NULL,psHdl->psTab,FALSE,FALSE); fprintf(pfDoc,"\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
            if (siErr<0) return(siErr);
            fprintf(pfDoc,   "DESCRIPTION\n");
            fprintf(pfDoc,   "-----------\n\n");
            if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
               fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcMan,2);
            } else {
               fprintf(pfDoc,"No detailed description available for this command.\n\n");
            }
            fprintf(pfDoc,"AUTHOR\n");
            fprintf(pfDoc,"------\n\n");
            fprintf(pfDoc,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
         } else {
            if (isNbr) {
               fprintf(pfDoc,   "%s %s '%s'\n",pcNum,pcCmd,pcSta);
               l=strlen(pcNum)+strlen(pcCmd)+strlen(pcSta)+4;
               for (i=0;i<l;i++) fprintf(pfDoc,"%c",C_TLD);
               fprintf(pfDoc,"\n\n");
            } else {
               fprintf(pfDoc,   "%s '%s'\n",pcCmd,pcSta);
               l=strlen(pcCmd)+strlen(pcSta)+3;
               for (i=0;i<l;i++) fprintf(pfDoc,"%c",C_TLD);
               fprintf(pfDoc,"\n\n");
            }
            fprintf(pfDoc,   ".SYNOPSIS\n\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n");
            efprintf(pfDoc,  "HELP:   %s\n",psHdl->pcHlp);
            fprintf(pfDoc,   "PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
            if (psHdl->isOvl) {
               fprintf(pfDoc,"TYPE:   OVERLAY\n");
            } else {
               fprintf(pfDoc,"TYPE:   OBJECT\n");
            }
            fprintf(pfDoc,   "SYNTAX: :> %s ",psHdl->pcPgm); siErr=siClpPrnCmd(pvHdl,pfDoc,0,0,1,NULL,psHdl->psTab,FALSE,FALSE); fprintf(pfDoc,"\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
            if (siErr<0) return(siErr);
            fprintf(pfDoc,   ".DESCRIPTION\n\n");
            if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
               fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcMan,2);
            } else {
               fprintf(pfDoc,"No detailed description available for this command.\n\n");
            }
            fprintf(pfDoc,   "indexterm:%c%s %s%c\n\n\n",C_SBO,pcCmd,pcSta,C_SBC);
            if (isDep) {
               siPos=siClpPrnDoc(pvHdl,pfDoc,0,isNbr,pcNum,NULL,psTab);
               if (siPos<0) return(siPos);
            }
         }
      }
   } else {
      return CLPERR(psHdl,CLPERR_INT,"No valid initial number or command string for head lines (%s)",psHdl->pcCmd);
   }
   return(CLP_OK);
}

extern int siClpProperties(
   void*                         pvHdl,
   const int                     siMtd,
   const int                     siDep,
   const char*                   pcPat,
   FILE*                         pfOut)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   char*                         pcPtr=NULL;
   char*                         pcKyw=NULL;
   char                          acKyw[CLPMAX_KYWSIZ];
   int                           siErr,siLev,i;
   int                           l=strlen(psHdl->pcCmd);
   const char*                   pcArg=NULL;

   if (psHdl->pcLst!=NULL) psHdl->pcLst[0]=0x00;

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":PROPERTIES:");

   if (pfOut==NULL) pfOut=psHdl->pfHlp;
   if (pfOut!=NULL) {
      if (pcPat!=NULL && *pcPat) {
         if (strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
            if (strlen(pcPat)>l && pcPat[l]!='.') {
               return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
            }
            for (siLev=0,pcPtr=strchr(pcPat,'.');pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
               for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) acKyw[i]=pcKyw[i];
               acKyw[i]=EOS;
               if (pcArg!=NULL) {
                  return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains too many or invalid qualifiers",pcPat);
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
            siErr=siClpPrnPro(pvHdl,pfOut,FALSE,siMtd,siLev,siLev+siDep,psTab,pcArg);
            if (siErr<0) return(siErr);
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
         }
      } else {
         siErr=siClpPrnPro(pvHdl,pfOut,FALSE,siMtd,0,siDep,psTab,NULL);
         if (siErr<0) return(siErr);
      }
   }
   return(CLP_OK);
}

extern int siClpSymbolTableWalk(
   void*                         pvHdl,
   const unsigned int            uiOpr,
   TsClpSymWlk*                  psSym)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   psHdl->psOld=psHdl->psSym;
   switch (uiOpr) {
   case CLPSYM_ROOT:  psHdl->psSym = psHdl->psTab;               break;
   case CLPSYM_OLD:   psHdl->psSym = psHdl->psOld;               break;
   case CLPSYM_NEXT:  psHdl->psSym = psHdl->psSym->psNxt;        break;
   case CLPSYM_BACK:  psHdl->psSym = psHdl->psSym->psBak;        break;
   case CLPSYM_DEP:   psHdl->psSym = psHdl->psSym->psDep;        break;
   case CLPSYM_HIH:   psHdl->psSym = psHdl->psSym->psHih;        break;
   case CLPSYM_ALIAS: psHdl->psSym = psHdl->psSym->psStd->psAli; break;
   case CLPSYM_COUNT: psHdl->psSym = psHdl->psSym->psFix->psCnt; break;
   case CLPSYM_ELN:   psHdl->psSym = psHdl->psSym->psFix->psEln; break;
   case CLPSYM_LINK:  psHdl->psSym = psHdl->psSym->psFix->psLnk; break;
   case CLPSYM_OID:   psHdl->psSym = psHdl->psSym->psFix->psOid; break;
   case CLPSYM_SLN:   psHdl->psSym = psHdl->psSym->psFix->psSln; break;
   case CLPSYM_TLN:   psHdl->psSym = psHdl->psSym->psFix->psTln; break;
   default: return CLPERR(psHdl,CLPERR_PAR,"Operation (%u) for symbol table walk not supported",uiOpr);
   }
   if (psHdl->psSym!=NULL) {
      TsSym*               apTmp[CLPMAX_HDEPTH];
      TsSym*               psTmp;
      int                  i;
      if (psHdl->pcCmd!=NULL) {
         srprintf(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->pcCmd),"%s",psHdl->pcCmd);
      } else {
         psHdl->pcPat[0]=EOS;
      }
      for (i=0,psTmp=psHdl->psSym->psHih;i<CLPMAX_HDEPTH && psTmp!=NULL;psTmp=psTmp->psHih,i++) apTmp[i]=psTmp;
      while (i>0) {
         srprintc(&psHdl->pcPat,&psHdl->szPat,strlen(apTmp[i-1]->psStd->pcKyw),".%s",apTmp[i-1]->psStd->pcKyw);
         i--;
      }
      psSym->pcPat=psHdl->pcPat;
      psSym->siKwl=psHdl->psSym->psStd->siKwl;
      psSym->pcKyw=psHdl->psSym->psStd->pcKyw;
      psSym->pcAli=GETALI(psHdl->psSym);
      psSym->uiFlg=psHdl->psSym->psStd->uiFlg;
      psSym->pcDft=psHdl->psSym->psFix->pcDft;
      psSym->pcMan=psHdl->psSym->psFix->pcMan;
      psSym->pcHlp=psHdl->psSym->psFix->pcHlp;
      psSym->siTyp=psHdl->psSym->psFix->siTyp;
      psSym->siMin=psHdl->psSym->psFix->siMin;
      psSym->siMax=psHdl->psSym->psFix->siMax;
      psSym->siSiz=psHdl->psSym->psFix->siSiz;
      psSym->siOid=psHdl->psSym->psFix->siOid;
      psSym->uiOpr=0;
      psSym->uiOpr|=(psHdl->psTab!=NULL)?CLPSYM_ROOT:0;
      psSym->uiOpr|=(psHdl->psOld!=NULL)?CLPSYM_OLD:0;
      psSym->uiOpr|=(psHdl->psSym->psNxt!=NULL)?CLPSYM_NEXT:0;
      psSym->uiOpr|=(psHdl->psSym->psBak!=NULL)?CLPSYM_BACK:0;
      psSym->uiOpr|=(psHdl->psSym->psDep!=NULL)?CLPSYM_DEP:0;
      psSym->uiOpr|=(psHdl->psSym->psHih!=NULL)?CLPSYM_HIH:0;
      psSym->uiOpr|=(psHdl->psSym->psStd->psAli!=NULL)?CLPSYM_ALIAS:0;
      psSym->uiOpr|=(psHdl->psSym->psFix->psCnt!=NULL)?CLPSYM_COUNT:0;
      psSym->uiOpr|=(psHdl->psSym->psFix->psEln!=NULL)?CLPSYM_ELN:0;
      psSym->uiOpr|=(psHdl->psSym->psFix->psLnk!=NULL)?CLPSYM_LINK:0;
      psSym->uiOpr|=(psHdl->psSym->psFix->psOid!=NULL)?CLPSYM_OID:0;
      psSym->uiOpr|=(psHdl->psSym->psFix->psSln!=NULL)?CLPSYM_SLN:0;
      psSym->uiOpr|=(psHdl->psSym->psFix->psTln!=NULL)?CLPSYM_TLN:0;
      return(psSym->siTyp);
   } else {
      psHdl->psSym=psHdl->psOld;
      memset(psSym,0,sizeof(TsClpSymWlk));
      return(0);
   }
}

extern int siClpSymbolTableUpdate(
   void*                         pvHdl,
   TsClpSymUpd*                  psSym)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   C08*                          pcHlp;
   if (psSym->pcPro!=NULL) {
      if (!CLPISF_ARG(psHdl->psSym->psStd->uiFlg)) {
         return CLPERR(psHdl,CLPERR_SIZ,"Update of property field failed (symbol (%s) is not a argument)",psHdl->psSym->psStd->pcKyw);
      }
      pcHlp=realloc_nowarn(psHdl->psSym->psFix->pcPro,strlen(psSym->pcPro)+1);
      if (pcHlp==NULL) {
         return CLPERR(psHdl,CLPERR_SIZ,"Update of property field failed (string (%d(%s)) too long)",(int)strlen(psSym->pcPro),psSym->pcPro);
      }
      psHdl->psSym->psFix->pcPro=pcHlp;
      strcpy(psHdl->psSym->psFix->pcPro,psSym->pcPro);
      psHdl->psSym->psFix->pcDft=psHdl->psSym->psFix->pcPro;
      psHdl->psSym->psStd->uiFlg|=CLPFLG_PDF;
   }
   return(CLP_OK);
}

extern void vdClpClose(
   void*                         pvHdl,
   const int                     siMtd)
{
   if (pvHdl!=NULL) {
      TsHdl*                     psHdl=(TsHdl*)pvHdl;
      int                        i;
      if (psHdl->pcLex!=NULL) {
         free(psHdl->pcLex);
         psHdl->pcLex=NULL;
         psHdl->szLex=0;
      }
      if (psHdl->pcSrc!=NULL) {
         free(psHdl->pcSrc);
         psHdl->pcSrc=NULL;
         psHdl->szSrc=0;
      }
      if (psHdl->pcPre!=NULL) {
         free(psHdl->pcPre);
         psHdl->pcPre=NULL;
         psHdl->szPre=0;
      }
      if (psHdl->pcPat!=NULL) {
         free(psHdl->pcPat);
         psHdl->pcPat=NULL;
         psHdl->szPat=0;
      }
      if (psHdl->pcLst!=NULL) {
         free(psHdl->pcLst);
         psHdl->pcLst=NULL;
         psHdl->szLst=0;
      }
      if (psHdl->pcMsg!=NULL) {
         free(psHdl->pcMsg);
         psHdl->pcMsg=NULL;
         psHdl->szMsg=0;
      }
      for (i=0;i<CLPMAX_BUFCNT;i++) {
         if (psHdl->apBuf[i]!=NULL) {
            free(psHdl->apBuf[i]);
            psHdl->apBuf[i]=NULL;
            psHdl->pzBuf[i]=0;
         }
      }
      vdClpSymDel(psHdl->psTab);
      psHdl->psTab=NULL;

      switch (siMtd) {
      case CLPCLS_MTD_KEP:
         break;
      case CLPCLS_MTD_EXC:
         if (psHdl->psPtr!=NULL) {
            free(psHdl->psPtr);
            psHdl->psPtr=NULL;
            psHdl->szPtr=0;
            psHdl->siPtr=0;
         }
         free(pvHdl);
         break;
      default:
         if (psHdl->psPtr!=NULL) {
            vdClpFree(psHdl);
            free(psHdl->psPtr);
            psHdl->psPtr=NULL;
            psHdl->szPtr=0;
            psHdl->siPtr=0;
         }
         free(pvHdl);
         break;
      }
   }
}

/* Interne Funktionen *************************************************/

static const char* get_env(char* var,const size_t size,const char* fmtstr, ...)
{
   int                  i;
   va_list              argv;
   va_start(argv,fmtstr); vsnprintf(var,size,fmtstr,argv); va_end(argv);
   for (i=0;var[i];i++) {
      var[i]=toupper(var[i]);
      if (var[i]=='.') var[i]='_';
   }
   return(GETENV(var));
}

#undef  ERROR
#define ERROR(s) if (s!=NULL) {                                                     \
      if (s->psStd!=NULL) { free(s->psStd); s->psStd=NULL; }                        \
      if (s->psFix!=NULL) {                                                         \
         if (s->psFix->pcPro!=NULL) { free(s->psFix->pcPro); s->psFix->pcPro=NULL; }\
         if (s->psFix->pcSrc!=NULL) { free(s->psFix->pcSrc); s->psFix->pcSrc=NULL; }\
         free(s->psFix); s->psFix=NULL;                                             \
      }                                                                             \
      if (s->psVar!=NULL) { free(s->psVar); s->psVar=NULL; }                        \
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
   const char*                   pcEnv=NULL;
   const char*                   pcPat=fpcPat(pvHdl,siLev);
   // TODO: Stack Allokation mit unbegrenzter Größe = Potenzielle Sicherheitslücke
   char                          acVar[strlen(psHdl->pcOwn)+strlen(psHdl->pcPgm)+strlen(pcPat)+strlen(psArg->pcKyw)+4];
   int                           k;
   acVar[0]=0x00;
   psSym=(TsSym*)calloc(1,sizeof(TsSym));
   if (psSym==NULL) {
      CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol '%s.%s' failed",pcPat,psArg->pcKyw);
      ERROR(psSym);
   }
   psSym->psStd=(TsStd*)calloc(1,sizeof(TsStd));
   psSym->psFix=(TsFix*)calloc(1,sizeof(TsFix));
   psSym->psVar=(TsVar*)calloc(1,sizeof(TsVar));
   if (psSym->psStd==NULL || psSym->psFix==NULL || psSym->psVar==NULL) {
      CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element '%s.%s' failed",pcPat,psArg->pcKyw);
      ERROR(psSym);
   }

   if (psArg->pcKyw!=NULL) {
      if (!isalpha(*psArg->pcKyw)) {
         CLPERR(psHdl,CLPERR_TAB,"Invalid first letter (%c) in keyword '%s.%s'",*psArg->pcKyw,pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      for (const char* p=psArg->pcKyw+1; *p; p++) {
         if (CLPISF_CON(psArg->uiFlg)?!isCon(*p):!isKyw(*p)) {
            CLPERR(psHdl,CLPERR_TAB,"Invalid letter (%c) in keyword '%s.%s'",*p,pcPat,psArg->pcKyw);
            ERROR(psSym);
         }
      }
   }

   if (psArg->pcAli!=NULL) {
      if (!isalpha(*psArg->pcAli)) {
         CLPERR(psHdl,CLPERR_TAB,"Invalid first letter (%c) in alias '%s.%s'",*psArg->pcAli,pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      for (const char* p=psArg->pcAli+1; *p; p++) {
         if (CLPISF_CON(psArg->uiFlg)?!isCon(*p):!isKyw(*p)) {
            CLPERR(psHdl,CLPERR_TAB,"Invalid letter (%c) in alias '%s.%s'",*p,pcPat,psArg->pcAli);
            ERROR(psSym);
         }
      }
   }

   psSym->psStd->pcKyw=psArg->pcKyw;
   psSym->psStd->uiFlg=psArg->uiFlg;
   if (psArg->pcTyp!=NULL) {
      if (strcmp(psArg->pcTyp,"U08")==0 || strcmp(psArg->pcTyp,"U16")==0 || strcmp(psArg->pcTyp,"U32")==0 || strcmp(psArg->pcTyp,"U64")==0) {
         psSym->psStd->uiFlg|=CLPFLG_UNS;
      }
   }
   psSym->psStd->psAli=NULL;
   psSym->psStd->siKwl=strlen(psSym->psStd->pcKyw);
   psSym->psStd->siLev=siLev;
   psSym->psStd->siPos=siPos;
   psSym->psFix->pcMan=psArg->pcMan;
   psSym->psFix->pcHlp=psArg->pcHlp;
   if (CLPISF_ARG(psArg->uiFlg)) {
      pcEnv=get_env(acVar,sizeof(acVar),"%s.%s.%s.%s",psHdl->pcOwn,psHdl->pcPgm,pcPat,psArg->pcKyw);
      if (pcEnv==NULL) {
         pcEnv=get_env(acVar,sizeof(acVar),"%s.%s.%s",psHdl->pcPgm,pcPat,psArg->pcKyw);
         if (pcEnv==NULL) {
            pcEnv=get_env(acVar,sizeof(acVar),"%s.%s",pcPat,psArg->pcKyw);
         }
      }
   }
   if (pcEnv!=NULL && *pcEnv) {
      psSym->psFix->pcPro=malloc(strlen(pcEnv)+1);
      if (psSym->psFix->pcPro==NULL) {
         CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element property '%s.%s' failed",pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      strcpy(psSym->psFix->pcPro,pcEnv);
      psSym->psFix->pcDft=psSym->psFix->pcPro;
      psSym->psFix->pcSrc=malloc(strlen(CLPSRC_ENV)+strlen(acVar)+1);
      if (psSym->psFix->pcSrc==NULL) {
         CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element source '%s.%s' failed",pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      sprintf(psSym->psFix->pcSrc,"%s%s",CLPSRC_ENV,acVar);
      psSym->psFix->siRow=1;
   } else {
      psSym->psFix->pcDft=psArg->pcDft;
      if (psArg->pcDft!=NULL) {
         psSym->psFix->pcSrc=malloc(strlen(CLPSRC_DEF)+strlen(pcPat)+strlen(psArg->pcKyw)+2);
         if (psSym->psFix->pcSrc==NULL) {
            CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element source '%s.%s' failed",pcPat,psArg->pcKyw);
            ERROR(psSym);
         }
         sprintf(psSym->psFix->pcSrc,"%s%s.%s",CLPSRC_DEF,pcPat,psArg->pcKyw);
         psSym->psFix->siRow=1;
      }
   }
   psSym->psFix->siTyp=psArg->siTyp;
   psSym->psFix->siMin=psArg->siMin;
   psSym->psFix->siMax=(psArg->siMax==0)?((CLPISF_DYN(psSym->psStd->uiFlg))?2147483647:0):psArg->siMax;
   psSym->psFix->siSiz=(psArg->siSiz==0)?((CLPISF_DYN(psSym->psStd->uiFlg))?2147483647:0):psArg->siSiz;
   psSym->psFix->siOfs=psArg->siOfs;
   psSym->psFix->siOid=psArg->siOid;
   psSym->psFix->psLnk=NULL;
   psSym->psFix->psCnt=NULL;
   psSym->psFix->psOid=NULL;
   psSym->psFix->psInd=NULL;
   psSym->psFix->psEln=NULL;
   psSym->psFix->psSln=NULL;
   psSym->psFix->psTln=NULL;

   switch (psSym->psFix->siTyp) {
   case CLPTYP_SWITCH: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_NUMBER: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_FLOATN: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_OBJECT: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_OVRLAY: psSym->psStd->uiFlg|=CLPFLG_FIX; break;
   case CLPTYP_STRING:
      if (CLPISF_DLM(psSym->psStd->uiFlg) && !CLPISF_FIX(psSym->psStd->uiFlg) && psSym->psFix->siSiz>0) {
         psSym->psFix->siSiz--; // CLPFLG_DLM support
      }
      break;
   case CLPTYP_XALIAS: break;
   default:
      CLPERR(psHdl,CLPERR_TAB,"Type (%d) for argument '%s.%s' not supported",psSym->psFix->siTyp,pcPat);
      ERROR(psSym);
   }

   if (CLPISF_DLM(psSym->psStd->uiFlg) && CLPISF_FIX(psSym->psStd->uiFlg) && psSym->psFix->siMax>0) {
      psSym->psFix->siMax--; // CLPFLG_DLM support
   }

   if (psSym->psFix->siTyp!=CLPTYP_NUMBER && CLPISF_DEF(psSym->psStd->uiFlg)) {
      CLPERR(psHdl,CLPERR_TAB,"Default flag for type '%s' of argument '%s.%s' not supported",apClpTyp[psSym->psFix->siTyp],pcPat);
      ERROR(psSym);
   }
   if (psSym->psFix->siTyp==CLPTYP_NUMBER && CLPISF_DEF(psSym->psStd->uiFlg) && psSym->psFix->siMax>1) {
      CLPERR(psHdl,CLPERR_TAB,"Default flag for arrays of type number of argument '%s.%s' not supported",pcPat);
      ERROR(psSym);
   }

   if (psArg->pcAli!=NULL) {
      if (psSym->psStd->pcKyw==NULL || *psSym->psStd->pcKyw==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword of alias (%s.%s) is not defined",pcPat,psArg->pcAli);
         ERROR(psSym);
      }
      if (strxcmp(psHdl->isCas,psSym->psStd->pcKyw,psArg->pcAli,0,0,FALSE)==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword and alias (%s.%s) are equal",pcPat,psArg->pcAli);
         ERROR(psSym);
      }
      for (k=0,psHlp=psCur;psHlp!=NULL;psHlp=psHlp->psBak) {
         if (CLPISF_ARG(psHlp->psStd->uiFlg) && strxcmp(psHdl->isCas,psArg->pcAli,psHlp->psStd->pcKyw,0,0,FALSE)==0) {
            if (k==0) {
               psSym->psStd->psAli=psHlp;
               psSym->psStd->uiFlg=psHlp->psStd->uiFlg|CLPFLG_ALI;
               free(psSym->psFix); psSym->psFix=psHlp->psFix;
               free(psSym->psVar); psSym->psVar=psHlp->psVar;
            } else {
               CLPERR(psHdl,CLPERR_TAB,"Alias '%s' for keyword '%s.%s' is not unique",psArg->pcAli,pcPat,psSym->psStd->pcKyw);
               ERROR(psSym);
            }
            k++;
         }
      }
      if (k==0) {
         CLPERR(psHdl,CLPERR_TAB,"Alias '%s' for keyword '%s.%s' cannot be resolved",psArg->pcAli,pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
   } else if (CLPISF_ARG(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || *psSym->psStd->pcKyw==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword for argument (%s.?) is not defined",pcPat);
         ERROR(psSym);
      }
      if (psSym->psFix->siMax<1 || psSym->psFix->siMax<psSym->psFix->siMin) {
         CLPERR(psHdl,CLPERR_TAB,"Maximal amount for argument '%s.%s' is too small",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siSiz<1) {
         CLPERR(psHdl,CLPERR_TAB,"Size for argument '%s.%s' is smaller than 1",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcHlp==NULL || *psSym->psFix->pcHlp==0) {
         CLPERR(psHdl,CLPERR_TAB,"Help for argument '%s.%s' not defined",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
   } else if (CLPISF_LNK(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || *psSym->psStd->pcKyw==0) {
         CLPERR(psHdl,CLPERR_TAB,"Keyword of a link (%s.?) is not defined",pcPat);
         ERROR(psSym);
      }
      if (psArg->pcAli!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Alias (%s) for link '%s.%s' defined",psArg->pcAli,pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcDft!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Default (%s) for link '%s.%s' defined",psSym->psFix->pcDft,pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siTyp!=CLPTYP_NUMBER) {
         CLPERR(psHdl,CLPERR_TAB,"Type for link '%s.%s' is not a number",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siMax<1 || psSym->psFix->siMax<psSym->psFix->siMin) {
         CLPERR(psHdl,CLPERR_TAB,"Maximal amount for link '%s.%s' is too small",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->siSiz<1) {
         CLPERR(psHdl,CLPERR_TAB,"Size for link '%s.%s' is smaller than 1",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (CLPISF_SEL(psSym->psStd->uiFlg) || CLPISF_CON(psSym->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_TAB,"Flag SEL or CON set for link '%s.%s'",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
   } else if (CLPISF_CON(psSym->psStd->uiFlg)) {
      if (psSym->psStd->pcKyw==NULL || *psSym->psStd->pcKyw==0) {
         CLPERR(psHdl,CLPERR_TAB,"Key word for a constant (%s.?) is not defined",pcPat);
         ERROR(psSym);
      }
      if (psArg->pcAli!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Alias (%s) for constant '%s.%s' defined",psArg->pcAli,pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcDft!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Default (%s) for constant '%s.%s' defined",psSym->psFix->pcDft,pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psArg->psTab!=NULL) {
         CLPERR(psHdl,CLPERR_TAB,"Table for constant '%s.%s' defined",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (CLPISF_SEL(psSym->psStd->uiFlg) || CLPISF_LNK(psSym->psStd->uiFlg)  || CLPISF_ALI(psSym->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_TAB,"Flags SEL, LNK or ALI set for constant '%s.%s'",pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      if (psSym->psFix->pcHlp==NULL || *psSym->psFix->pcHlp==0) {
         CLPERR(psHdl,CLPERR_TAB,"Help for constant '%s.%s' not defined",pcPat,psSym->psStd->pcKyw);
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
            CLPERR(psHdl,CLPERR_TAB,"Type '%s' for constant '%s.%s' requires a value (pcVal==NULL)",apClpTyp[psSym->psFix->siTyp],pcPat,psSym->psStd->pcKyw);
            ERROR(psSym);
         }
         if (!CLPISF_BIN(psSym->psStd->uiFlg) && psSym->psFix->siSiz==0) {
            psSym->psFix->siSiz=strlen((char*)psArg->pcVal)+1;
         }
         psSym->psVar->pvDat=(void*)psArg->pcVal;
         break;
      default:
         CLPERR(psHdl,CLPERR_TAB,"Type (%s) for argument '%s.%s' not supported for constant definitions",apClpTyp[psSym->psFix->siTyp],pcPat,psSym->psStd->pcKyw);
         ERROR(psSym);
      }
      psSym->psVar->pvPtr=NULL;
      psSym->psVar->siLen=psSym->psFix->siSiz;
      psSym->psVar->siCnt=1;
      psSym->psVar->siRst=0;
   } else {
      CLPERR(psHdl,CLPERR_TAB,"Kind (ALI/ARG/LNK/CON) of argument '%s.%s' not determinable",pcPat,psSym->psStd->pcKyw);
      ERROR(psSym);
   }

   if (!CLPISF_CMD(psSym->psStd->uiFlg) && !CLPISF_PRO(psSym->psStd->uiFlg) && !CLPISF_DMY(psSym->psStd->uiFlg)) {
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
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s:%s' not defined",fpcPat(pvHdl,siLev),psArg->pcKyw);
      }
   }

   for (j=i=0;psTab[i].siTyp;i++) {
      if (i>=CLPMAX_TABCNT) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Argument table bigger than maximal supported entry count (%d)",CLPMAX_TABCNT);
         } else {
            return CLPERR(psHdl,CLPERR_TAB,"Table for key word (%s:%s) bigger than maximal supported entry count (%d)",fpcPat(pvHdl,siLev),psArg->pcKyw,CLPMAX_TABCNT);
         }
      }

      if (psTab[i].pcKyw==NULL || psTab[i].pcKyw[0]==0x00) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"There is no keyword defined in argument table at index %d",i);
         } else {
            return CLPERR(psHdl,CLPERR_TAB,"The table for keyword (%s:%s) has no keyword defined at index %d",fpcPat(pvHdl,siLev),psArg->pcKyw,i);
         }
      } else {
         if (strlen(psTab[i].pcKyw)>CLPMAX_KYWLEN) {
            if (psArg==NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"The keyword defined in argument table at index %d is too long (>%d)",i,CLPMAX_KYWLEN);
            } else {
               return CLPERR(psHdl,CLPERR_TAB,"The keyword defined in table (%s:%s) at index %d is too long (>%d)",fpcPat(pvHdl,siLev),psArg->pcKyw,i,CLPMAX_KYWLEN);
            }
         }
      }

      if (!CLPISF_DMY(psTab[i].uiFlg)) {
         psCur=psClpSymIns(pvHdl,siLev,i,&psTab[i],psHih,psCur);
         if (psCur==NULL) {
            return CLPERR(psHdl,CLPERR_SYS,"Insert of symbol (%s.%s) in symbol table failed",fpcPat(pvHdl,siLev),psTab[i].pcKyw);
         }
         if (j==0) *ppFst=psCur;

         switch (psTab[i].siTyp) {
         case CLPTYP_SWITCH:
            if (psTab[i].psTab!=NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s.%s' is defined (NULL for psTab required)",fpcPat(pvHdl,siLev),psTab[i].pcKyw);
            }
            break;
         case CLPTYP_NUMBER:
         case CLPTYP_FLOATN:
         case CLPTYP_STRING:
            if (CLPISF_SEL(psTab[i].uiFlg)) {
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
            return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter '%s.%s' not supported",psTab[i].siTyp,fpcPat(pvHdl,siLev),psTab[i].pcKyw);
         }
         j++;
      }
   }
   return(CLP_OK);
}

static void vdClpSymLnkCnt(
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psCnt=psLnk;
         vdClpSymLnkCnt(psSon,psLnk);
      }
   }
}

static void vdClpSymLnkOid(
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psOid=psLnk;
         vdClpSymLnkOid(psSon,psLnk);
      }
   }
}

static void vdClpSymLnkInd(
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psInd=psLnk;
         vdClpSymLnkInd(psSon,psLnk);
      }
   }
}

static void vdClpSymLnkEln(
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psEln=psLnk;
         vdClpSymLnkEln(psSon,psLnk);
      }
   }
}

static void vdClpSymLnkSln(
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psSln=psLnk;
         vdClpSymLnkSln(psSon,psLnk);
      }
   }
}

static void vdClpSymLnkTln(
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psTln=psLnk;
         vdClpSymLnkTln(psSon,psLnk);
      }
   }
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
            return CLPERR(psHdl,CLPERR_INT,"Parameter table of argument '%s.%s' not in sync with symbol table",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }

      if (CLPISF_ALI(psSym->psStd->uiFlg)) {
         isPar=TRUE;
      } else if (CLPISF_ARG(psSym->psStd->uiFlg)) {
         isPar=TRUE;
      } else if (CLPISF_LNK(psSym->psStd->uiFlg)) {
         isPar=TRUE;
         for (h=k=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if (CLPISF_ARG(psHlp->psStd->uiFlg) && strxcmp(psHdl->isCas,psSym->psStd->pcKyw,psHlp->psStd->pcKyw,0,0,FALSE)==0) {
               psSym->psFix->psLnk=psHlp; h++;
               if (CLPISF_CNT(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psCnt=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     vdClpSymLnkCnt(psHlp,psSym);
                  }
               }
               if (CLPISF_OID(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psOid=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     vdClpSymLnkOid(psHlp,psSym);
                  }
               }
               if (CLPISF_IND(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psInd=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     vdClpSymLnkInd(psHlp,psSym);
                  }
               }
               if (CLPISF_ELN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psEln=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     vdClpSymLnkEln(psHlp,psSym);
                  }
               }
               if (CLPISF_SLN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psSln=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     vdClpSymLnkSln(psHlp,psSym);
                  }
               }
               if (CLPISF_TLN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psTln=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     vdClpSymLnkTln(psHlp,psSym);
                  }
               }
            }
         }
         if (psSym->psFix->psLnk==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword '%s.%s' cannot be resolved",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         if (h>1) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword '%s.%s' is not unique",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         if (k>1) {
            return CLPERR(psHdl,CLPERR_TAB,"More than one link defined for keyword '%s.%s'",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
         if (k==0) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword '%s.%s' was not assigned",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
      } else if (CLPISF_CON(psSym->psStd->uiFlg)) {
         isCon=TRUE; siCon++;
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Kind (ALI/ARG/LNK/CON) of argument '%s.%s' not determinable",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
      }

      psSym->psStd->siKwl=strlen(psSym->psStd->pcKyw);
      if (psHdl->siMkl>0) {
         if (psSym->psStd->siKwl>psHdl->siMkl) psSym->psStd->siKwl=psHdl->siMkl;
         if (!CLPISF_LNK(psSym->psStd->uiFlg)) {
            for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
               if (psHlp!=psSym && !CLPISF_LNK(psHlp->psStd->uiFlg)) {
                  if (psHdl->isCas) {
                     for (k=0;psSym->psStd->pcKyw[k] && psHlp->psStd->pcKyw[k] &&         psSym->psStd->pcKyw[k] ==        psHlp->psStd->pcKyw[k] ;k++);
                  } else {
                     for (k=0;psSym->psStd->pcKyw[k] && psHlp->psStd->pcKyw[k] && toupper(psSym->psStd->pcKyw[k])==toupper(psHlp->psStd->pcKyw[k]);k++);
                  }
                  if (psSym->psStd->pcKyw[k]==0 && psHlp->psStd->pcKyw[k]==0) {
                     return CLPERR(psHdl,CLPERR_TAB,"Key word '%s.%s' is not unique",fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
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
#ifdef __DEBUG__
      char  acKyw[strlen(psSym->psStd->pcKyw)+1];
      strcpy(acKyw,psSym->psStd->pcKyw);
      for (int i=strlen(acKyw);i>=psSym->psStd->siKwl;i--) {
         acKyw[i]=0x00;
         if (CLPTOK_KYW!=siClpConNat(pvHdl,psHdl->pfErr,NULL,acKyw,NULL,NULL,-1,NULL)) {
            fprintf(stderr,"%s:%d:1: warning: Constant keyword (%s) re-used in table definitions (%s.%s)",__FILE__,__LINE__,acKyw,fpcPat(pvHdl,siLev),psSym->psStd->pcKyw);
         }
      }
#endif
   }

   if (isCon && (isPar || siCon!=siPos)) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table is not consistent (mix of constants and parameter)%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s.%s' is not consistent (mix of constants and parameter)",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
   }
   if (isCon==FALSE && isPar==FALSE) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table neither contains constants nor arguments%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s.%s' neither contains constants nor arguments",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
   }
   for (psSym=psTab;psSym!=NULL;psSym=psSym->psNxt) {
      if (!CLPISF_LNK(psSym->psStd->uiFlg)) {
         if (psSym->psStd->siKwl<=0) {
            return CLPERR(psHdl,CLPERR_TAB,"Required keyword length (%d) of argument '%s.%s' is smaller or equal to zero",psSym->psStd->siKwl,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         if (psSym->psStd->siKwl>strlen(psSym->psStd->pcKyw)) {
            return CLPERR(psHdl,CLPERR_TAB,"Required keyword length (%d) of argument '%s.%s' is greater then keyword length",psSym->psStd->siKwl,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
      }
      if (CLPISF_ALI(psSym->psStd->uiFlg)) {
         psSym->psDep=psSym->psStd->psAli->psDep;
         psSym->psHih=psSym->psStd->psAli->psHih;
      }
   }
   return(CLP_OK);
}

static const TsSym* psClpFndSym(
   void*                         pvHdl,
   const char*                   pcKyw,
   const TsSym*                  psTab)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psHlp=NULL;
   int                           i,j,k;
   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (!CLPISF_LNK(psHlp->psStd->uiFlg)) {
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
            return(psHlp);
         }
      }
   }
   return(NULL);
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
      CLPERR(psHdl,CLPERR_SYN,"Keyword '%s.%s' not valid",fpcPat(pvHdl,siLev),pcKyw);
      CLPERRADD(psHdl,      1,"Unexpected end of path reached%s","");
      return(CLPERR_SYN);
   }
   if (psTab->psBak!=NULL) {
      CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      CLPERRADD(psHdl,      1,"Try to find keyword '%s' in this table",pcKyw);
      return(CLPERR_INT);
   }
   for (e=i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (!CLPISF_LNK(psHlp->psStd->uiFlg)) {
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
            if (CLPISF_ARG(psHlp->psStd->uiFlg) || CLPISF_CON(psHlp->psStd->uiFlg)) {
               e++;
            }
         }
      }
   }
   CLPERR(psHdl,CLPERR_SYN,"Parameter '%s.%s' not valid",fpcPat(pvHdl,siLev),pcKyw);
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
      if (psHdl->pfSym!=NULL) efprintf(psHdl->pfSym,"%s %3.3d - %s (KWL=%d TYP=%s MIN=%d MAX=%d SIZ=%d OFS=%d OID=%d FLG=%8.8X (NXT=%p BAK=%p DEP=%p HIH=%p ALI=%p CNT=%p OID=%p IND=%p ELN=%p SLN=%p TLN=%p LNK=%p)) - %s\n",
            fpcPre(pvHdl,siLev),psHlp->psStd->siPos+1,psHlp->psStd->pcKyw,psHlp->psStd->siKwl,apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->siMin,psHlp->psFix->siMax,psHlp->psFix->siSiz,
            psHlp->psFix->siOfs,psHlp->psFix->siOid,psHlp->psStd->uiFlg,psHlp->psNxt,psHlp->psBak,psHlp->psDep,psHlp->psHih,psHlp->psStd->psAli,psHlp->psFix->psCnt,psHlp->psFix->psOid,
            psHlp->psFix->psInd,psHlp->psFix->psEln,psHlp->psFix->psSln,psHlp->psFix->psTln,psHlp->psFix->psLnk,psHlp->psFix->pcHlp);
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
      vdClpSymPrn(pvHdl,0,psHdl->psTab);
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
      if (!CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psDep!=NULL) {
         vdClpSymDel(psHlp->psDep);
      }
      if (!CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psVar!=NULL) {
         memset(psHlp->psVar,0,sizeof(TsVar));
         free(psHlp->psVar);
         psHlp->psVar=NULL;
      }
      if (!CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psFix!=NULL) {
         if (psHlp->psFix->pcPro!=NULL) free(psHlp->psFix->pcPro);
         if (psHlp->psFix->pcSrc!=NULL) free(psHlp->psFix->pcSrc);
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

#define STRCHR '\''
#define SPMCHR '\"'
#define ALTCHR C_GRV

extern int siClpLexem(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   if (pfOut!=NULL) {
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," COMMENT   '#' [:print:]* '#'                              (will be ignored)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCOMMENT  ';' [:print:]* 'nl'                             (will be ignored)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," OPERATOR1 '=' | '.' | '(' | ')' | '[' | ']'  (SGN, DOT, RBO, RBC, SBO, SBC)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," OPERATOR2 '+' | '-' | '*' | '/'                        (ADD, SUB, MUL, DIV)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," KEYWORD   ['-'['-']][:alpha:]+[:alnum: | '_']*          (always predefined)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," NUMBER    ([+|-]  [ :digit:]+)  |                       (decimal (default))\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," num       ([+|-]0b[ :digit:]+)  |                                  (binary)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," num       ([+|-]0o[ :digit:]+)  |                                   (octal)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," num       ([+|-]0d[ :digit:]+)  |                                 (decimal)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," num       ([+|-]0x[ :xdigit:]+) |                             (hexadecimal)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," num       ([+|-]0t(yyyy/mm/tt.hh:mm:ss)) |  (relativ (+|-) or absolut time)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," FLOAT     ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) | (decimal(default))\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," flt       ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)            (decimal)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," STRING         ''' [:print:]* ''' |          (default (if binary c else s))\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," str       [s|S]''' [:print:]* ''' |                (null-terminated string)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," str       [c|C]''' [:print:]* ''' |  (binary string in local character set)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," str       [a|A]''' [:print:]* ''' |                (binary string in ASCII)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," str       [e|E]''' [:print:]* ''' |               (binary string in EBCDIC)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," str       [x|X]''' [:print:]* ''' |         (binary string in hex notation)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," str       [f|F]''' [:print:]* ''' | (read string from file (for passwords))\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"           Strings can contain two '' to represent one '                    \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"           Strings can also be enclosed in \" or %c instead of '              \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"           Strings can directly start behind a '=' without enclosing '/\"    \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"              In this case the string ends at the next separator or operator\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"              and keywords are preferred. To use keywords, separators or    \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"              operators in strings, enclosing quotes are required.          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                                                                            \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," The predefined constant keyword below can be used in a value expressions   \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," NOW       NUMBER - current time in seconds since 1970 (+0t0000)            \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," MINUTE    NUMBER - minute in seconds (60)                                  \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," HOUR      NUMBER - hour in seconds   (60*60)                               \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," DAY       NUMBER - day in seconds    (24*60*60)                            \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," YEAR      NUMBER - year in seconds   (365*24*60*60)                        \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," KiB       NUMBER - kilobyte          (1024)                                \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," MiB       NUMBER - megabyte          (1024*1024)                           \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GiB       NUMBER - gigabyte          (1024*1024*1024)                      \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," TiB       NUMBER - terrabyte         (1024*1024*1024*1024)                 \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," RNDn      NUMBER - simple random number with n * 8 bit in length (1,2,4,8) \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," PI        FLOAT  - PI (3.14159265359)                                      \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCSTAMP   STRING - current local stamp in format:           YYYYMMDD.HHMMSS\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCDATE    STRING - current local date in format:            YYYYMMDD       \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCYEAR    STRING - current local year in format:            YYYY           \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCMONTH   STRING - current local month in format:           MM             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCDAY     STRING - current local day in format:             DD             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCTIME    STRING - current local time in format:            HHMMSS         \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCHOUR    STRING - current local hour in format:            HH             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCMINUTE  STRING - current local minute in format:          MM             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," LCSECOND  STRING - current local second in format:          SS             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMSTAMP   STRING - current Greenwich mean stamp in format:  YYYYMMDD.HHMMSS\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMDATE    STRING - current Greenwich mean date in format:   YYYYMMDD       \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMYEAR    STRING - current Greenwich mean year in format:   YYYY           \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMMONTH   STRING - current Greenwich mean month in format:  MM             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMDAY     STRING - current Greenwich mean day in format:    DD             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMTIME    STRING - current Greenwich mean time in format:   HHMMSS         \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMHOUR    STRING - current Greenwich mean hour in format:   HH             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMMINUTE  STRING - current Greenwich mean minute in format: MM             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMSECOND  STRING - current Greenwich mean second in format: SS             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," GMSECOND  STRING - current Greenwich mean second in format: SS             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," SnRND10   STRING - decimal random number of length n (1 to 8)              \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," SnRND16   STRING - hexadecimal random number of length n (1 to 8)          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                                                                            \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," SUPPLEMENT     '\"' [:print:]* '\"' |   (null-terminated string (properties))\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"           Supplements can contain two \"\" to represent one \"                \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"           Supplements can also be enclosed in ' or %c instead of \"          \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"           Supplements can also be enclosed in ' or %c instead of \"          \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," ENVIRONMENT VARIABLES '<'varnam'>' will replaced by the corresponding value  \n");
   }
   return(CLP_OK);
}

#define isPrintF(p)    (((p)!=NULL)?(CLPISF_PWD((p)->psStd->uiFlg)==FALSE):(TRUE))
#define isPrnLex(p,l)  (isPrintF(p)?(l):("***SECRET***"))
#define isPrintF2(p)   (CLPISF_PWD((p)->psStd->uiFlg)==FALSE)
#define isPrnLex2(p,l) (isPrintF2(p)?(l):("***SECRET***"))
#define isSeparation(c) (isspace((c)) || iscntrl((c)) || ((c))==',')

#define LEX_REALLOC \
   if (pcLex>=(pcEnd-4)) {\
      size_t l=pcLex-(*ppLex);\
      size_t h=pcHlp-(*ppLex);\
      size_t s=(*pzLex)?(*pzLex)*2:CLPINI_LEXSIZ;\
      char*  b=(char*)realloc_nowarn(*ppLex,s);\
      if (b==NULL) return CLPERR(psHdl,CLPERR_MEM,"Re-allocation to store the lexem failed");\
      (*pzLex)=s;\
      if (b!=(*ppLex)) {\
         (*ppLex)=b;\
         pcLex=(*ppLex)+l;\
         pcHlp=(*ppLex)+h;\
         pcEnd=(*ppLex)+(*pzLex);\
      }\
   }

static int siClpConNat(
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   size_t*                       pzLex,
   char**                        ppLex,
   const int                     siTyp,
   const TsSym*                  psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;

   if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"NOW",0,0,FALSE)==0) {
      if (pzLex!=NULL && ppLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)psHdl->siNow));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
         if (psArg!=NULL) psArg->psStd->uiFlg|=CLPFLG_TIM;
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"MINUTE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)60));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"HOUR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)60)*((U64)60));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"DAY",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)24)*((U64)60)*((U64)60));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"YEAR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)365)*((U64)24)*((U64)60)*((U64)60));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"KiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"MiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024)*((U64)1024));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024)*((U64)1024)*((U64)1024));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"TiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024)*((U64)1024)*((U64)1024)*((U64)1024));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND8",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         if (psHdl->siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%"PRIi64"",psHdl->siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%"PRIi64"",psHdl->siRnd);
         }
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND4",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         I32   siRnd=(I32)psHdl->siRnd;
         if (siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%d",siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%d",siRnd);
         }
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND2",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         I16   siRnd=(I16)psHdl->siRnd;
         if (siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%d",(I32)siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%d",(I32)siRnd);
         }
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND1",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         I08   siRnd=(I08)psHdl->siRnd;
         if (siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%d",(I32)siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%d",(I32)siRnd);
         }
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_FLOATN || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"PI",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%f",3.14159265359);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(FLT)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_FLT);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCSTAMP",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d.%H%M%S",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCDATE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCYEAR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCMONTH",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%m",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCDAY",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%d",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCTIME",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H%M%S",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCHOUR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCMINUTE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%M",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCSECOND",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%S",localtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMSTAMP",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d.%H%M%S",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMDATE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMYEAR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMMONTH",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%m",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMDAY",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%d",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMTIME",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H%M%S",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMHOUR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMMINUTE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%M",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMSECOND",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         time_t t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%S",gmtime(&t));
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S1RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%01u",((U32)psHdl->siRnd)%10);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S2RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%02u",((U32)psHdl->siRnd)%100);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S3RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%03u",((U32)psHdl->siRnd)%1000);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S4RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%04u",((U32)psHdl->siRnd)%10000);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S5RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%05u",((U32)psHdl->siRnd)%100000);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S6RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%06u",((U32)psHdl->siRnd)%1000000);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S7RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%07u",((U32)psHdl->siRnd)%10000000);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S8RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%08u",((U32)psHdl->siRnd)%100000000);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S1RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%01x",((U32)psHdl->siRnd)&0x0000000F);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S2RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%02x",((U32)psHdl->siRnd)&0x000000FF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S3RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%03x",((U32)psHdl->siRnd)&0x00000FFF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S4RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%04x",((U32)psHdl->siRnd)&0x0000FFFF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S5RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%05x",((U32)psHdl->siRnd)&0x000FFFFF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S6RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%06x",((U32)psHdl->siRnd)&0x00FFFFFF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S7RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%07x",((U32)psHdl->siRnd)&0x0FFFFFFF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S8RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^rand());
         srprintf(ppLex,pzLex,10,"d'%08x",((U32)psHdl->siRnd)&0xFFFFFFFF);
         if (pfTrc!=NULL) fprintf(pfTrc,"CONSTANT-TOKEN(STR)-LEXEM(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else {
      return(CLPTOK_KYW);
   }
}

static inline int isClpKyw (
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   const int                     siFlg,
   const int                     siTyp,
   const TsSym*                  psArg,
   const TsSym**                 ppVal)
{
   const TsSym*                  psHlp=NULL;
   const TsSym*                  psVal=NULL;
   if (siFlg>0 && psArg!=NULL && psArg->psDep!=NULL) {
      psVal=psClpFndSym(pvHdl,pcKyw,psArg->psDep);
   }
   if (siFlg>1 && psArg!=NULL && psVal==NULL && !CLPISF_SEL(psArg->psStd->uiFlg)) {
      for (psHlp=psArg;psHlp->psBak!=NULL;psHlp=psHlp->psBak);
      psVal=psClpFndSym(pvHdl,pcKyw,psHlp);
   }
   if (psVal!=NULL && ((((siTyp>0 && psVal->psFix->siTyp==siTyp && psVal->psVar->siCnt)) || siTyp<=0))) {
      if (ppVal!=NULL) *ppVal=psVal;
      return(TRUE);
   } else {
      return(CLPTOK_KYW!=siClpConNat(pvHdl,pfErr,pfTrc,pcKyw,NULL,NULL,siTyp,psArg));
   }
}

static int siClpScnNat(
   void*                         pvHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char**                  ppCur,
   size_t*                       pzLex,
   char**                        ppLex,
   int                           siTyp,
   const TsSym*                  psArg,
   int*                          piSep,
   const TsSym**                 ppVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char*                         pcLex=(*ppLex);
   char*                         pcHlp=(*ppLex);
   char*                         pcEnd=(*ppLex)+(*pzLex);
   const char*                   pcCur=(*ppCur);
   int                           isEnv=psHdl->isEnv;
   const char*                   pcEnv=NULL;
   const char*                   pcOld=NULL;
   char*                         pcZro=NULL;
   time_t                        t;
   struct tm                     tm;
   struct tm                     *tmAkt;
   char                          acHlp[1024];

   if (siTyp!=CLPTYP_NUMBER && siTyp!=CLPTYP_FLOATN && siTyp!=CLPTYP_STRING) siTyp=0;
   if (piSep!=NULL) *piSep=FALSE;
   if (ppVal!=NULL) *ppVal=NULL;

   while (1) {
      if (*(*ppCur)==EOS) { /*end*/
         pcLex[0]=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(END)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_END);
      } else if (isSeparation(*(*ppCur))) { /*separation*/
         if (piSep!=NULL) *piSep=TRUE;
         if (*(*ppCur)=='\n') {
            psHdl->siRow++;
            psHdl->pcRow=(*ppCur)+1;
         }
         (*ppCur)++;
         psHdl->pcOld=(*ppCur);
      } else if (*(*ppCur)==C_HSH) { /*comment*/
         (*ppCur)++;
         while (*(*ppCur)!=C_HSH && *(*ppCur)!=EOS) {
            if (*(*ppCur)=='\n') {
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur)+1;
            }
            (*ppCur)++;
         }
         if (*(*ppCur)!=C_HSH) {
            return CLPERR(psHdl,CLPERR_LEX,"Comment not terminated with '%c'",C_HSH);
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
      } else if (isEnv && *(*ppCur)=='<') { /*environment variable replacement*/
         (*ppCur)++;
         while ((*ppCur)[0]!=EOS && (*ppCur)[0]!='>') {
            LEX_REALLOC
            *pcLex=*(*ppCur); pcLex++;
            (*ppCur)++;
         }
         *pcLex=EOS;
         if (*(*ppCur)!='>') {
            return CLPERR(psHdl,CLPERR_LEX,"Environment variable not terminated with '>'");
         }
         (*ppCur)++;
         pcEnv=getenvar(pcHlp,sizeof(acHlp),acHlp);
         pcLex=(*ppLex);
         if (pcEnv!=NULL) {
            size_t l=pcCur-psHdl->pcInp;
            if (psHdl->siBuf>=CLPMAX_BUFCNT) {
               return CLPERR(psHdl,CLPERR_LEX,"Environment variable replacement (%s=%s) not possible (more than %d recursions)",pcLex,pcEnv,CLPMAX_BUFCNT);
            }
            srprintf(&psHdl->apBuf[psHdl->siBuf],&psHdl->pzBuf[psHdl->siBuf],l+strlen(pcEnv)+strlen((*ppCur)),"%.*s%s%s",(int)l,psHdl->pcInp,pcEnv,(*ppCur));
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-ENVARREP\n%s %s\n%s %s\n",fpcPre(psHdl,0),psHdl->pcInp,fpcPre(psHdl,0),psHdl->apBuf[psHdl->siBuf]);
            (*ppCur)=psHdl->apBuf[psHdl->siBuf]+l;
            psHdl->pcInp=psHdl->apBuf[psHdl->siBuf];
            psHdl->pcOld=psHdl->apBuf[psHdl->siBuf]+(psHdl->pcOld-psHdl->pcInp);
            psHdl->pcRow=psHdl->apBuf[psHdl->siBuf]+(psHdl->pcRow-psHdl->pcInp);
            isEnv=TRUE;
         } else {
            isEnv=FALSE;
            (*ppCur)=pcCur;
         }
      } else if ((*ppCur)[0]==SPMCHR || (*ppCur)[0]==STRCHR || (*ppCur)[0]==ALTCHR) {/*simple string*/
         char USECHR=(*ppCur)[0];
         *pcLex= 'd'; pcLex++;
         *pcLex='\''; pcLex++;
         (*ppCur)++;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=USECHR || ((*ppCur)[0]==USECHR && (*ppCur)[1]==USECHR))) {
            LEX_REALLOC
            *pcLex=*(*ppCur); pcLex++;
            if (*(*ppCur)=='\n') {
               (*ppCur)++;
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur);
            } else if ((*ppCur)[0]==USECHR) {
               (*ppCur)+=2;
            } else {
               (*ppCur)++;
            }
         }
         *pcLex=EOS;
         if (*(*ppCur)!=USECHR) {
            return CLPERR(psHdl,CLPERR_LEX,"String literal not terminated with '%c'",USECHR);
         }
         (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if ((tolower((*ppCur)[0])=='x' || tolower((*ppCur)[0])=='a' || tolower((*ppCur)[0])=='e' ||
                  tolower((*ppCur)[0])=='c' || tolower((*ppCur)[0])=='s' || tolower((*ppCur)[0])=='f') &&
                 ((*ppCur)[1]==SPMCHR || (*ppCur)[1]==STRCHR || (*ppCur)[1]==ALTCHR)) {/*defined string '...'*/
         char USECHR=(*ppCur)[1];
         *pcLex=tolower(*(*ppCur)); pcLex++;
         *pcLex='\''; pcLex++;
         (*ppCur)+=2;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=USECHR || ((*ppCur)[0]==USECHR && (*ppCur)[1]==USECHR))) {
            LEX_REALLOC
            *pcLex=*(*ppCur); pcLex++;
            if (*(*ppCur)=='\n') {
               (*ppCur)++;
               psHdl->siRow++;
               psHdl->pcRow=(*ppCur);
            } else if ((*ppCur)[0]==USECHR) {
               (*ppCur)+=2;
            } else {
               (*ppCur)++;
            }
         }
         *pcLex=EOS;
         if (*(*ppCur)!=USECHR) {
            return CLPERR(psHdl,CLPERR_LEX,"String literal not terminated with '%c'",USECHR);
         }
         (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if (siTyp==0 && (((*ppCur)[0]=='-' && isalpha((*ppCur)[1])) || ((*ppCur)[0]=='-' && (*ppCur)[1]=='-' && isalpha((*ppCur)[2])))) { /*defined keyword*/
         while ((*ppCur)[0]=='-') {
            (*ppCur)++;
         }
         *pcLex=*(*ppCur);
         (*ppCur)++; pcLex++;
         while (isCon(*(*ppCur))) {
            LEX_REALLOC
            if (!isKyw(*(*ppCur)) && pcOld==NULL) {
               pcOld=(*ppCur);
               pcZro=pcLex;
            }
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
         }
         *pcLex=EOS;
         if(pcCur[0]=='-' && pcCur[1]!='-' && isClpKyw(pvHdl,pfErr,pfTrc,pcHlp,2,psArg!=NULL?psArg->psFix->siTyp:-1,psArg,NULL)) {
            (*ppCur)=pcCur;
            pcLex[0]='-'; pcLex[1]=EOS; (*ppCur)++;
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SUB)-LEXEM(%s)\n",pcHlp);
            return(CLPTOK_SUB);
         } else {
            if (pcOld!=NULL && !isClpKyw(pvHdl,pfErr,pfTrc,pcHlp,2,-1,psArg,ppVal)) {
               *pcZro=0x00;
               *ppCur=pcOld;
            }
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(KYW)-LEXEM(%s)\n",pcHlp);
            return(CLPTOK_KYW);
         }
      } else if (siTyp==CLPTYP_STRING && isStr((*ppCur)[0])     &&
                        (*ppCur)[0]!='('    && (*ppCur)[0]!=')' && (*ppCur)[0]!='+' &&
                        (*ppCur)[0]!=C_SBO  && (*ppCur)[0]!=C_SBC) {/*required string*/
         char*             pcKyw;
         *pcLex='d'; pcLex++;
         *pcLex='\''; pcLex++;
         pcKyw=pcLex;
         if (psArg!=NULL) {
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            while (isCon(*(*ppCur))) {
               LEX_REALLOC
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            if (isClpKyw(pvHdl,pfErr,pfTrc,pcKyw,2,CLPTYP_STRING,psArg,ppVal)) {
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
         while ((*ppCur)[0]!=EOS && isStr((*ppCur)[0]) && !isSeparation((*ppCur)[0]) &&
                           (*ppCur)[0]!='('    && (*ppCur)[0]!=')'   &&
                           (*ppCur)[0]!=C_SBO  && (*ppCur)[0]!=C_SBC) {
            LEX_REALLOC
            *pcLex=*(*ppCur);
            pcLex++; (*ppCur)++;
         }
         *pcLex=EOS;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(STR)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if (isalpha((*ppCur)[0])) { /*simple keyword*/
         *pcLex=*(*ppCur);
         (*ppCur)++; pcLex++;
         while (isCon(*(*ppCur))) {
            LEX_REALLOC
            if (!isKyw(*(*ppCur)) && pcOld==NULL) {
               pcOld=(*ppCur);
               pcZro=pcLex;
            }
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
         }
         *pcLex=EOS;
         if (pcOld!=NULL && !isClpKyw(pvHdl,pfErr,pfTrc,pcHlp,2,-1,psArg,ppVal)) {
            *pcZro=0x00;
            *ppCur=pcOld;
         }
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
            while (isxdigit(*(*ppCur))) {
               LEX_REALLOC
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
         } else if (pcHlp[0]=='t') {
            memset(&tm,0,sizeof(tm));
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            tm.tm_year=strtol(pcHlp+2,NULL,10);
            if ((*ppCur)[0]=='/' && isdigit((*ppCur)[1])) {
               pcLex=pcHlp+2; (*ppCur)++;
               while (isdigit(*(*ppCur))) {
                  LEX_REALLOC
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               *pcLex=EOS;
               tm.tm_mon=strtol(pcHlp+2,NULL,10);
               if ((*ppCur)[0]=='/' && isdigit((*ppCur)[1])) {
                  pcLex=pcHlp+2; (*ppCur)++;
                  while (isdigit(*(*ppCur))) {
                     LEX_REALLOC
                     *pcLex=*(*ppCur);
                     (*ppCur)++; pcLex++;
                  }
                  *pcLex=EOS;
                  tm.tm_mday=strtol(pcHlp+2,NULL,10);
                  if ((*ppCur)[0]=='.' && isdigit((*ppCur)[1])) {
                     pcLex=pcHlp+2; (*ppCur)++;
                     while (isdigit(*(*ppCur))) {
                        LEX_REALLOC
                        *pcLex=*(*ppCur);
                        (*ppCur)++; pcLex++;
                     }
                     *pcLex=EOS;
                     tm.tm_hour=strtol(pcHlp+2,NULL,10);
                     if ((*ppCur)[0]==':' && isdigit((*ppCur)[1])) {
                        pcLex=pcHlp+2; (*ppCur)++;
                        while (isdigit(*(*ppCur))) {
                           LEX_REALLOC
                           *pcLex=*(*ppCur);
                           (*ppCur)++; pcLex++;
                        }
                        *pcLex=EOS;
                        tm.tm_min=strtol(pcHlp+2,NULL,10);
                        if ((*ppCur)[0]==':' && isdigit((*ppCur)[1])) {
                           pcLex=pcHlp+2; (*ppCur)++;
                           while (isdigit(*(*ppCur))) {
                              LEX_REALLOC
                              *pcLex=*(*ppCur);
                              (*ppCur)++; pcLex++;
                           }
                           *pcLex=EOS;
                           tm.tm_sec=strtol(pcHlp+2,NULL,10);
                        }
                     }
                  }
               }
            }
            if (pcHlp[1]=='+') {
               t=time(NULL);
               if (t==-1) {
                  return CLPERR(psHdl,CLPERR_SYS,"Determine the current time is not possible%s","");
               }
               tmAkt=localtime(&t);
               tmAkt->tm_year +=tm.tm_year;
               tmAkt->tm_mon  +=tm.tm_mon;
               tmAkt->tm_mday +=tm.tm_mday;
               tmAkt->tm_hour +=tm.tm_hour;
               tmAkt->tm_min  +=tm.tm_min;
               tmAkt->tm_sec  +=tm.tm_sec;
               t=mktime(tmAkt);
               if (t==-1) {
                  return CLPERR(psHdl,CLPERR_LEX,"The calculated time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) cannot be converted to a number",
                                                 tmAkt->tm_year+1900,tmAkt->tm_mon+1,tmAkt->tm_mday,tmAkt->tm_hour,tmAkt->tm_min,tmAkt->tm_sec);
               }
            } else if (pcHlp[1]=='-') {
               t=time(NULL);
               if (t==-1) {
                  return CLPERR(psHdl,CLPERR_SYS,"Determine the current time is not possible%s","");
               }
               tmAkt=localtime(&t);
               tmAkt->tm_year -=tm.tm_year;
               tmAkt->tm_mon  -=tm.tm_mon;
               tmAkt->tm_mday -=tm.tm_mday;
               tmAkt->tm_hour -=tm.tm_hour;
               tmAkt->tm_min  -=tm.tm_min;
               tmAkt->tm_sec  -=tm.tm_sec;
               t=mktime(tmAkt);
               if (t==-1) {
                  return CLPERR(psHdl,CLPERR_LEX,"The calculated time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) cannot be converted to a number",
                                                 tmAkt->tm_year+1900,tmAkt->tm_mon+1,tmAkt->tm_mday,tmAkt->tm_hour,tmAkt->tm_min,tmAkt->tm_sec);
               }
            } else {
               if (tm.tm_year>=1900) tm.tm_year-=1900;
               if (tm.tm_mon >=   1) tm.tm_mon-=1;
               if (tm.tm_mday ==  0) tm.tm_mday++;
               t=mktime(&tm);
               if (t==-1) {
                  return CLPERR(psHdl,CLPERR_LEX,"The given time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) cannot be converted to a number",
                                                 tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
               }
               if (tm.tm_isdst>0) t-=60*60;//correct daylight saving time
            }
            pcHlp[1]='+';
            sprintf(pcHlp+2,"%"PRIu64"",(U64)t);
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(NUM)-LEXEM(%s)-TIME\n",isPrnLex(psArg,pcHlp));
            if (psArg!=NULL) psArg->psStd->uiFlg|=CLPFLG_TIM;
            return(CLPTOK_NUM);
         } else {
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
         }
         if (pcHlp[0]=='d' && (*ppCur)[0]=='.' && (isdigit((*ppCur)[1]))) { /*float*/
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC
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
               while (isdigit(*(*ppCur))) {
                  LEX_REALLOC
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               *pcLex=EOS;
               if (pcHlp[1]==' ') pcHlp[1]='+';
               if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
               return(CLPTOK_FLT);
            }
            *pcLex=EOS;
            if (pcHlp[1]==' ') pcHlp[1]='+';
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
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            if (pcHlp[1]==' ') pcHlp[1]='+';
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(FLT)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_FLT);
         } else {
            *pcLex=EOS;
            if (pcHlp[1]==' ') pcHlp[1]='+';
            if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(NUM)-LEXEM(%s)\n",isPrnLex(psArg,pcHlp));
            return((siTyp==CLPTOK_FLT)?CLPTOK_FLT:CLPTOK_NUM);
         }
      } else if (*(*ppCur)=='=') { /*sign*/
         pcLex[0]='='; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SGN)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SGN);
      } else if (*(*ppCur)=='.') { /*dot*/
         pcLex[0]='.'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(DOT)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_DOT);
      } else if (*(*ppCur)=='+') { /*add*/
         pcLex[0]='+'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(ADD)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_ADD);
      } else if (*(*ppCur)=='-') { /*sub*/
         pcLex[0]='-'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SUB)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SUB);
      } else if (*(*ppCur)=='*') { /*mul*/
         pcLex[0]='*'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(MUL)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_MUL);
      } else if (*(*ppCur)=='/') { /*div*/
         pcLex[0]='/'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(DIV)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_DIV);
      } else if (*(*ppCur)=='(') { /*round bracket open*/
         pcLex[0]='('; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(RBO)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_RBO);
      } else if (*(*ppCur)==')') { /*round bracket close*/
         pcLex[0]=')'; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(RBC)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_RBC);
      } else if (*(*ppCur)==C_SBO) { /*squared bracket open*/
         pcLex[0]=C_SBO; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SBO)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SBO);
      } else if (*(*ppCur)==C_SBC) { /*squared bracket close*/
         pcLex[0]=C_SBC; pcLex[1]=EOS; (*ppCur)++;
         if (pfTrc!=NULL) fprintf(pfTrc,"SCANNER-TOKEN(SBC)-LEXEM(%s)\n",pcHlp);
         return(CLPTOK_SBC);
      } else { /*lexical error*/
         pcLex[0]=EOS; (*ppCur)++;
         return CLPERR(psHdl,CLPERR_LEX,"Character ('%c') not valid",*((*ppCur)-1));
      }
   }
}


static int siClpConSrc(
   void*                         pvHdl,
   const int                     isChk,
   const TsSym*                  psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   return(siClpConNat(pvHdl,psHdl->pfErr,psHdl->pfScn,psHdl->pcLex,(isChk)?NULL:&psHdl->szLex,(isChk)?NULL:&psHdl->pcLex,(psArg!=NULL)?psArg->psFix->siTyp:0,psArg));
}

static int siClpScnSrc(
   void*                         pvHdl,
   int                           siTyp,
   const TsSym*                  psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   psHdl->pcOld=psHdl->pcCur;
   return(siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&psHdl->pcCur,&psHdl->szLex,&psHdl->pcLex,siTyp,psArg,&psHdl->isSep,&psHdl->psVal));
}

/**********************************************************************/

extern int siClpGrammar(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   if (pfOut!=NULL) {
      TsHdl*                        psHdl=(TsHdl*)pvHdl;
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," Command Line Parser                                              \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," command        -> ['('] parameter_list [')']       (main=object) \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  ['.'] parameter                  (main=overlay)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," parameter_list -> parameter SEP parameter_list                   \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," parameter      -> switch | assignment | object | overlay | array \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," switch         -> KEYWORD                                        \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," assignment     -> KEYWORD '=' value                              \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' KEYWORD # SELECTION #              \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," object         -> KEYWORD ['('] parameter_list [')']             \n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' STRING # parameter file #          \n");
   }
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," overlay        -> KEYWORD ['.'] parameter                        \n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' STRING # parameter file #          \n");
   }
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," array          -> KEYWORD '[' value_list   ']'                   \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '[' object_list  ']'                   \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '[' overlay_list ']'                   \n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '[=' STRING ']' # parameter file #     \n");
   }
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," value_list     -> value SEP value_list                           \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," object_list    -> object SEP object_list                         \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," overlay_list   -> overlay SEP overlay_list                       \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," A list of objects requires parenthesis to enclose the arguments  \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                                                                  \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," value          -> term '+' value                                 \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  term '-' value                                 \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  term                                           \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," term           -> factor '*' term                                \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  factor '/' term                                \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  factor                                         \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," factor         -> NUMBER | FLOAT | STRING                        \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  selection | variable | constant                \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  '(' value ')'                                  \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," selection      -> KEYWORD # value from a selection table        #\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," variable       -> KEYWORD # value from a previous assignment    #\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD '[' value ']' # with index for arrays #\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," constant       -> KEYWORD # see predefined constants at lexem   #\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," For strings only the operator '+' is implemented as concatenation\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," Strings without an operator in between are also concatenated     \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," A number followed by a constant is a multiplication (4KiB=4*1024)\n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                                                                  \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," Property File Parser                                             \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," properties     -> property_list                                  \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," property_list  -> property SEP property_list                     \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," property       -> keyword_list '=' SUPPLEMENT                    \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," keyword_list   -> KEYWORD '.' keyword_list                       \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut,"                |  KEYWORD                                        \n");
      fprintf(pfOut,"%s",fpcPre(pvHdl,0)); efprintf(pfOut," SUPPLEMENT is a string in double quotation marks (\"property\")    \n");
   }
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
   char                          acKyw[CLPMAX_KYWSIZ];
   TsSym*                        psArg=NULL;
   int                           siErr;

   if (psHdl->siTok==CLPTOK_KYW) {
      strlcpy(acKyw,psHdl->pcLex,sizeof(acKyw));
      siErr=siClpSymFnd(pvHdl,siLev,siPos,acKyw,psTab,&psArg,NULL);
      if (siErr<0) return(siErr);
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psHdl->pcOld-psHdl->pcInp,psArg->psFix->psInd,TRUE);
      if (siErr<0) return(siErr);
      if (piOid!=NULL) *piOid=psArg->psFix->siOid;
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_SGN) {
         if (psHdl->isPfl && (psArg->psFix->siTyp==CLPTYP_OBJECT || psArg->psFix->siTyp==CLPTYP_OVRLAY)) {
            return(siClpPrsFil(pvHdl,siLev,siPos,FALSE,psArg));
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
         } else if (psArg->psFix->siTyp==CLPTYP_NUMBER && CLPISF_DEF(psArg->psStd->uiFlg)) {
            return(siClpPrsNum(pvHdl,siLev,siPos,psArg));
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

static int siClpPrsNum(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d NUM(%s) USING OID AS DEFAULT VALUE)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   return(siClpBldNum(pvHdl,siLev,siPos,psArg));
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
   psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   return(siClpPrsVal(pvHdl,siLev,siPos,FALSE,psArg));
}

static int siClpPrsFil(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   // TODO: Stack Allokation mit unbegrenzter Größe = Potenzielle Sicherheitslücke
   char                          acSrc[strlen(psHdl->pcSrc)+1];
   char*                         pcFil;
   char*                         pcPar=NULL;
   int                           siRow,siCnt,siErr,siSiz=0;
   const char*                   pcCur;
   const char*                   pcInp;
   const char*                   pcOld;
   const char*                   pcRow;

   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d PARFIL(%s=val)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(pvHdl,CLPTYP_STRING,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   if (psHdl->siTok!=CLPTOK_STR) {
      return CLPERR(psHdl,CLPERR_SYN,"After object/overlay/array assignment '%s.%s=' parameter file ('filename') expected",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   pcFil=dcpmapfil(psHdl->pcLex+2);
   if (pcFil==NULL) {
      return CLPERR(psHdl,CLPERR_MEM,"Dynamic allocation of parameter file string (%s) for argument '%s.%s' failed",psHdl->pcLex+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   siErr=file2str(pcFil,&pcPar,&siSiz,filemode("r"));
   if (siErr<0) {
      switch(siErr) {
      case -1: siErr=CLPERR(psHdl,CLPERR_INT,"Illegal parameters passed to file2str() (Bug)%s","");break;
      case -2: siErr=CLPERR(psHdl,CLPERR_SYS,"Open of parameter file (%s) failed (%d - %s)",pcFil,errno,strerror(errno));break;
      case -3: siErr=CLPERR(psHdl,CLPERR_SEM,"Parameter file (%s) is too big (integer overflow)",pcFil);break;
      case -4: siErr=CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for parameter file (%s) failed",pcFil);break;
      case -5: siErr=CLPERR(psHdl,CLPERR_SYS,"Read of parameter file (%s) failed (%d - %s)",pcFil,errno,strerror(errno));break;
      default: siErr=CLPERR(psHdl,CLPERR_SYS,"An unknown error occurred while reading parameter file (%s)",pcFil);break;
      }
      if (pcPar!=NULL) free(pcPar);
      free(pcFil);
      return(siErr);
   }

   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PARAMETER-FILE-PARSER-BEGIN(FILE=%s)\n",pcFil);
   strcpy(acSrc,psHdl->pcSrc);
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_PAF)+strlen(pcFil),"%s%s",CLPSRC_PAF,pcFil);
   pcCur=psHdl->pcCur; psHdl->pcCur=pcPar;
   pcInp=psHdl->pcInp; psHdl->pcInp=pcPar;
   pcOld=psHdl->pcOld; psHdl->pcOld=pcPar;
   pcRow=psHdl->pcRow; psHdl->pcRow=pcPar;
   siRow=psHdl->siRow; psHdl->siRow=1;
   psHdl->siBuf++;
   psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
   if (psHdl->siTok<0) {
      if (pcPar!=NULL) free(pcPar);
      free(pcFil);
      return(psHdl->siTok);
   }
   if (isAry) {
      switch (psArg->psFix->siTyp) {
      case CLPTYP_NUMBER: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_NUM,psArg); break;
      case CLPTYP_FLOATN: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_FLT,psArg); break;
      case CLPTYP_STRING: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_STR,psArg); break;
      case CLPTYP_OBJECT: siCnt=siClpPrsObjLst(pvHdl,siLev,psArg); break;
      case CLPTYP_OVRLAY: siCnt=siClpPrsOvlLst(pvHdl,siLev,psArg); break;
      default:
         if (pcPar!=NULL) free(pcPar);
         free(pcFil);
         return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter '%s.%s' is not supported with arrays",psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      if (siCnt<0) {
         if (pcPar!=NULL) free(pcPar);
         free(pcFil);
         return(siCnt);
      }
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) {
         if (pcPar!=NULL) free(pcPar);
         free(pcFil);
         return(psHdl->siTok);
      }
   } else {
      if (psHdl->siTok==CLPTOK_RBO) {
         siCnt=siClpPrsObj(pvHdl,siLev,siPos,psArg);
         if (siCnt<0) {
            if (pcPar!=NULL) free(pcPar);
            free(pcFil);
            return(siCnt);
         }
      } else if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
         if (psHdl->siTok<0) {
            if (pcPar!=NULL) free(pcPar);
            free(pcFil);
            return(psHdl->siTok);
         }
         siCnt=siClpPrsOvl(pvHdl,siLev,siPos,psArg);
         if (siCnt<0) {
            if (pcPar!=NULL) free(pcPar);
            free(pcFil);
            return(siCnt);
         }
      } else {
         if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
            siCnt=siClpPrsObjWob(pvHdl,siLev,siPos,psArg);
            if (siCnt<0) {
               if (pcPar!=NULL) free(pcPar);
               free(pcFil);
               return(siCnt);
            }
         } else {
            siCnt=siClpPrsOvl(pvHdl,siLev,siPos,psArg);
            if (siCnt<0) {
               if (pcPar!=NULL) free(pcPar);
               free(pcFil);
               return(siCnt);
            }
         }
      }
   }
   if (psHdl->siTok==CLPTOK_END) {
      psHdl->siBuf--;
      psHdl->pcLex[0]=EOS;
      strcpy(psHdl->pcSrc,acSrc);
      psHdl->pcCur=pcCur; psHdl->pcInp=pcInp; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
      if (pcPar!=NULL) free(pcPar);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"PARAMETER-FILE-PARSER-END(FILE=%s CNT=%d)\n",pcFil,siCnt);
      free(pcFil);
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      return(siCnt);
   } else {
      siErr=CLPERR(psHdl,CLPERR_SYN,"Last token (%s) of parameter file '%s' is not EOF",apClpTok[psHdl->siTok],pcFil);
      if (pcPar!=NULL) free(pcPar);
      free(pcFil);
      return(siErr);
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
      return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing (%s)",fpcPat(pvHdl,siLev));
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
            return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing (MAIN)%s","");
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
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ARY(%s%ctyplst%c)-OPN)\n",fpcPre(pvHdl,siLev),siLev,siPos,psArg->psStd->pcKyw,C_SBO,C_SBC);
   psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
   if (psHdl->siTok<0) return(psHdl->siTok);
   if (psHdl->isPfl && psHdl->siTok==CLPTOK_SGN) {
      siCnt=siClpPrsFil(pvHdl,siLev,siPos,TRUE,psArg);
      if (siCnt<0) return(siCnt);
   } else {
      switch (psArg->psFix->siTyp) {
      case CLPTYP_NUMBER: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_NUM,psArg); break;
      case CLPTYP_FLOATN: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_FLT,psArg); break;
      case CLPTYP_STRING: siCnt=siClpPrsValLst(pvHdl,siLev,CLPTOK_STR,psArg); break;
      case CLPTYP_OBJECT: siCnt=siClpPrsObjLst(pvHdl,siLev,psArg); break;
      case CLPTYP_OVRLAY: siCnt=siClpPrsOvlLst(pvHdl,siLev,psArg); break;
      default:
         return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter '%s.%s' is not supported with arrays",psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      if (siCnt<0) return(siCnt);
   }
   if (psHdl->siTok==CLPTOK_SBC) {
      psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d ARY(%s%ctyplst%c)-CLS)\n",fpcPre(pvHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw,C_SBO,C_SBC);
      return(CLP_OK);
   } else {
      return CLPERR(psHdl,CLPERR_SYN,"Character '%c' missing (%s)",C_SBC,fpcPat(pvHdl,siLev));
   }
}

static int siClpPrsValLst(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siTok,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr,siPos=0;
   while (psHdl->siTok==siTok || psHdl->siTok==CLPTOK_KYW || psHdl->siTok==CLPTOK_RBO) {
      siErr=siClpPrsVal(pvHdl,siLev,siPos,TRUE,psArg);
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
/**********************************************************************/

static int siFromNumberLexem(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal,
   I64*                          piVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char*                         pcHlp=NULL;

   errno=0;
   switch (pcVal[0]) {
   case 'b':*piVal=strtoll(pcVal+1,&pcHlp, 2); break;
   case 'o':*piVal=strtoll(pcVal+1,&pcHlp, 8); break;
   case 'd':*piVal=strtoll(pcVal+1,&pcHlp,10); break;
   case 'x':*piVal=strtoll(pcVal+1,&pcHlp,16); break;
   case 't':*piVal=strtoll(pcVal+1,&pcHlp,10); break;
   default: return CLPERR(psHdl,CLPERR_SEM,"Base (%c) of number literal (%s.%s=%s) not supported",pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcVal+1));
   }
   if (errno || (pcHlp!=NULL && *pcHlp)) {
      if (pcHlp!=NULL && *pcHlp) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (rest: %s)",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcHlp));
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (errno: %d - %s)",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,errno,strerror(errno));
      }
   }
   return(CLP_OK);
}

static int siFromFloatLexem(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal,
   double*                       pfVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   char*                         pcHlp=NULL;
   char*                         pcLoc;
   errno=0;
   switch (pcVal[0]) {
   case 'd':
      // TODO: avoid using setlocale()/localeconv() anywhere in the project (except in main()) as they are not thread-safe
      pcLoc=setlocale(LC_NUMERIC, NULL);
      setlocale(LC_NUMERIC, "C");
      *pfVal=strtod(pcVal+1,&pcHlp);
      setlocale(LC_NUMERIC, pcLoc);
      break;
   default: return CLPERR(psHdl,CLPERR_SEM,"Base (%c) of floating point literal (%s.%s=%s) not supported",pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcVal+1));
   }
   if (errno ||  (pcHlp!=NULL && *pcHlp)) {
      if (pcHlp!=NULL && *pcHlp) {
         return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (rest: %s)",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,pcHlp);
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (errno: %d - %s)",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,errno,strerror(errno));
      }
   }
   return(CLP_OK);
}

static int siClpPrsExp(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal);

static int siClpPrsFac(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psVal=psHdl->psVal;
   TsSym*                        psHlp;
   int                           siErr;
   I64                           siInd;
   I64                           siVal;
   F64                           flVal;
   void*                         pvDat;
   char                          acLex[strlen(psHdl->pcLex)+1];
   strcpy(acLex,psHdl->pcLex);
   switch(psHdl->siTok) {
   case CLPTOK_NUM:
   case CLPTOK_FLT:
   case CLPTOK_STR:
      psHdl->siTok=siClpScnSrc(pvHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      srprintf(ppVal,pzVal,strlen(acLex),"%s",acLex);
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d NUM/FLT/STR(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,*ppVal);
      return(CLP_OK);
   case CLPTOK_KYW:
      siInd=0;
      psHdl->siTok=siClpScnSrc(pvHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_SBO) {
         psHdl->siTok=siClpScnSrc(pvHdl,0,psArg);
         if (psHdl->siTok<0) return(psHdl->siTok);
         siErr=siClpPrsExp(pvHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
         if (siErr) return(siErr);
         if (psHdl->siTok==CLPTOK_SBC) {
            psHdl->siTok=siClpScnSrc(pvHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
            if (psHdl->siTok<0) return(psHdl->siTok);
         } else {
            return CLPERR(psHdl,CLPERR_SYN,"Character '%c' missing (%s)",C_SBC,fpcPat(pvHdl,siLev));
         }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siInd);
         if (siErr) return(siErr);
      }
      if (psVal==NULL) { // not already find in scanner
         psVal=psClpFndSym(pvHdl,acLex,psArg->psDep);
         if (psVal==NULL) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
               vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
               return(CLPERR_SEM);
            }
            for (psHlp=psArg;psHlp->psBak!=NULL;psHlp=psHlp->psBak);
            psVal=psClpFndSym(pvHdl,acLex,psHlp);
         }
      }
      if (psVal!=NULL) {
         if (psArg->psFix->siTyp!=psVal->psFix->siTyp) {
            return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of the symbol (%s) for argument '%s.%s' don't match the expected type (%s)",
                  apClpTyp[psVal->psFix->siTyp],acLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         if (psVal->psVar->pvDat==NULL || psVal->psVar->siCnt==0) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) and type (%s) of variable value for argument (%s.%s) defined but data pointer not set (variable not defined)",
                  psVal->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         if (siInd<0 || siInd>=psVal->psVar->siCnt) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) and type (%s) of variable value for argument (%s.%s) defined but data element counter (%d) too small (index (%d) not valid)",
                  psVal->psStd->pcKyw,apClpTyp[psVal->psFix->siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psVal->psVar->siCnt,(int)siInd);
         }
         if (CLPISF_DYN(psVal->psStd->uiFlg)) {
            pvDat=(*((void**)psVal->psVar->pvDat));
         } else {
            pvDat=psVal->psVar->pvDat;
         }
         switch (psVal->psFix->siTyp) {
         case CLPTYP_NUMBER:
            switch (psVal->psFix->siSiz) {
            case 1: siVal=((I08*)pvDat)[siInd]; break;
            case 2: siVal=((I16*)pvDat)[siInd]; break;
            case 4: siVal=((I32*)pvDat)[siInd]; break;
            case 8: siVal=((I64*)pvDat)[siInd]; break;
            default:return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the constant value '%s' of '%s.%s' is not 1, 2, 4 or 8)",
                  psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            if (siVal>=0) {
               srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
            } else {
               srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
            }
            break;
         case CLPTYP_FLOATN:
            switch (psVal->psFix->siSiz) {
            case 4: flVal=((F32*)pvDat)[siInd]; break;
            case 8: flVal=((F64*)pvDat)[siInd]; break;
            default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the constant value '%s' of '%s.%s' is not 4 or 8)",
                  psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            if (flVal>=0.0) {
               srprintf(ppVal,pzVal,24,"d+%f",flVal);
            } else {
               srprintf(ppVal,pzVal,24,"d%f",flVal);
            }
            break;
         case CLPTYP_STRING:
            if (siInd>0) {
               char* pcDat=pvDat;
               char* pcEnd=pcDat+psVal->psVar->siLen;
               if (CLPISF_FIX(psVal->psStd->uiFlg)) {
                  pcDat+=siInd*psVal->psFix->siSiz;
               } else {
                  for (int j=0;pcDat<pcEnd && j<siInd;pcDat++) {
                     if (*pcDat==0x00) j++;
                  }
               }
               if (pcDat>=pcEnd) {
                  return CLPERR(psHdl,CLPERR_TAB,"Array access with index %d is not possible for this string argument (%s.%s)",(int)siInd,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               pvDat=pcDat;
            }
            if (CLPISF_BIN(psVal->psStd->uiFlg)) {
               char acHlp[(2*psVal->psVar->siLen)+1];
               int l=bin2hex((unsigned char*)pvDat,acHlp,psVal->psVar->siLen);
               acHlp[l]=0x00;
               srprintf(ppVal,pzVal,strlen(acHlp),"x'%s",acHlp);
            } else if (CLPISF_HEX(psVal->psStd->uiFlg)) {
               srprintf(ppVal,pzVal,strlen((char*)pvDat),"x'%s",(char*)pvDat);
            } else if (CLPISF_ASC(psVal->psStd->uiFlg)) {
               srprintf(ppVal,pzVal,strlen((char*)pvDat),"a'%s",(char*)pvDat);
            } else if (CLPISF_EBC(psVal->psStd->uiFlg)) {
               srprintf(ppVal,pzVal,strlen((char*)pvDat),"e'%s",(char*)pvDat);
            } else {
               srprintf(ppVal,pzVal,strlen((char*)pvDat),"d'%s",(char*)pvDat);
            }
            break;
         default:
            return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of constant '%s.%s' not supported in this case",
                  psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d KYW-CON(%s) TAB)\n",fpcPre(pvHdl,siLev),siLev,siPos,*ppVal);
      } else {
         siErr=siClpConNat(pvHdl,psHdl->pfErr,psHdl->pfScn,acLex,pzVal,ppVal,psArg->psFix->siTyp,psArg);
         if (siErr==CLPTOK_KYW) {
            return CLPERR(psHdl,CLPERR_SYN,"Keyword (%s) not valid in expression of type %s for argument %s.%s",
                  acLex,apClpTyp[psArg->psFix->siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d KYW-CON(%s) FIX)\n",fpcPre(pvHdl,siLev),siLev,siPos,*ppVal);
      }
      return(CLP_OK);
   case CLPTOK_RBO:
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      siErr=siClpPrsExp(pvHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
      if (siErr) return(siErr);
      if (psHdl->siTok==CLPTOK_RBC) {
         psHdl->siTok=siClpScnSrc(pvHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
         if (psHdl->siTok<0) return(psHdl->siTok);
         return(CLP_OK);
      } else {
         return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing (%s)",fpcPat(pvHdl,siLev));
      }
   default:
      return(CLPERR(psHdl,CLPERR_SYN,"After assignment '%s.%s=' number(-123), float(+123.45e78), string('abc') expression expected",
            fpcPat(pvHdl,siLev),psArg->psStd->pcKyw));
   }
}

static int siClpPrsTrm(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr;
   I64                           siVal1=0;
   I64                           siVal2=0;
   I64                           siVal=0;
   F64                           flVal1=0;
   F64                           flVal2=0;
   F64                           flVal=0;
   siErr=siClpPrsFac(pvHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
   if (siErr) return(siErr);
   if (psHdl->siTok==CLPTOK_MUL) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
      siErr=siClpPrsTrm(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal2);
         if (siErr) { free(pcVal); return(siErr); }
         siVal=siVal1*siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d MUL-NUM(%"PRIi64"*%"PRIi64"=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,(int64_t)siVal1,(int64_t)siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,*ppVal,&flVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal2);
         if (siErr) { free(pcVal); return(siErr); }
         flVal=flVal1*flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d MUL-FLT(%f*%f=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Multiplication not supported for type %s",apClpTyp[psArg->psFix->siTyp]);
      }
      free(pcVal);
   } else if (psHdl->siTok==CLPTOK_DIV) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
      siErr=siClpPrsTrm(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal2);
         if (siErr) { free(pcVal); return(siErr); }
         if (siVal2==0) {
            free(pcVal);
            return CLPERR(psHdl,CLPERR_SEM,"Devision by zero",apClpTyp[psArg->psFix->siTyp]);
         }
         siVal=siVal1/siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d DIV-NUM(%"PRIi64"/%"PRIi64"=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,(int64_t)siVal1,(int64_t)siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,*ppVal,&flVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal2);
         if (siErr) { free(pcVal); return(siErr); }
         if (flVal2==0) {
            free(pcVal);
            return CLPERR(psHdl,CLPERR_SEM,"Devision by zero",apClpTyp[psArg->psFix->siTyp]);
         }
         flVal=flVal1/flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d DIV-FLT(%f/%f=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Division not supported for type %s",apClpTyp[psArg->psFix->siTyp]);
      }
      free(pcVal);
   } else if (psHdl->siTok==CLPTOK_KYW && psHdl->isSep==FALSE && CLPTOK_KYW!=siClpConSrc(pvHdl,TRUE,psArg)) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
      siErr=siClpPrsTrm(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal2);
         if (siErr) { free(pcVal); return(siErr); }
         siVal=siVal1*siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-MUL-NUM(%"PRIi64"*%"PRIi64"=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,(int64_t)siVal1,(int64_t)siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,*ppVal,&flVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal2);
         if (siErr) { free(pcVal); return(siErr); }
         flVal=flVal1*flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-MUL-FLT(%f*%f=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      case CLPTYP_STRING:
         if ((*ppVal)[0]==pcVal[0]) {
         } else if (((*ppVal)[0]=='d' && pcVal[0]=='s') || ((*ppVal)[0]=='s' && pcVal[0]=='d')){
            (*ppVal)[0]='s';
         } else if (((*ppVal)[0]=='d' && pcVal[0]=='c') || ((*ppVal)[0]=='c' && pcVal[0]=='d')){
            (*ppVal)[0]='c';
         } else {
            siErr=CLPERR(psHdl,CLPERR_SEM,"Cannot concatenate different types (%c <> %c) of strings",(*ppVal)[0],pcVal[0]);
            free(pcVal);
            return(siErr);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-STR(%s+",fpcPre(pvHdl,siLev),siLev,siPos,*ppVal);
         srprintc(ppVal,pzVal,strlen(pcVal),"%s",pcVal+2);
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s=%s))\n",pcVal,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Multiplication not supported for type %s",apClpTyp[psArg->psFix->siTyp]);
      }
      free(pcVal);
   }
   return(CLP_OK);
}

static int siClpPrsExp(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr;
   I64                           siVal1=0;
   I64                           siVal2=0;
   I64                           siVal=0;
   F64                           flVal1=0;
   F64                           flVal2=0;
   F64                           flVal=0;
   siErr=siClpPrsTrm(pvHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
   if (siErr) return(siErr);
   if (psHdl->siTok==CLPTOK_ADD) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
      siErr=siClpPrsExp(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal2);
         if (siErr) { free(pcVal); return(siErr); }
         siVal=siVal1+siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-NUM(%"PRIi64"+%"PRIi64"=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,(int64_t)siVal1,(int64_t)siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,*ppVal,&flVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal2);
         if (siErr) { free(pcVal); return(siErr); }
         flVal=flVal1+flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-FLT(%f+%f=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      case CLPTYP_STRING:
         if ((*ppVal)[0]==pcVal[0]) {
         } else if (((*ppVal)[0]=='d' && pcVal[0]=='s') || ((*ppVal)[0]=='s' && pcVal[0]=='d')){
            (*ppVal)[0]='s';
         } else if (((*ppVal)[0]=='d' && pcVal[0]=='c') || ((*ppVal)[0]=='c' && pcVal[0]=='d')){
            (*ppVal)[0]='c';
         } else {
            siErr=CLPERR(psHdl,CLPERR_SEM,"Cannot concatenate different types (%c <> %c) of strings",(*ppVal)[0],pcVal[0]);
            free(pcVal);
            return(siErr);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-STR(%s+",fpcPre(pvHdl,siLev),siLev,siPos,*ppVal);
         srprintc(ppVal,pzVal,strlen(pcVal),"%s",pcVal+2);
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s=%s))\n",pcVal,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Addition not supported for type %s",apClpTyp[psArg->psFix->siTyp]);
      }
      free(pcVal);
   } else if (psHdl->siTok==CLPTOK_SUB) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) return(psHdl->siTok);
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
      siErr=siClpPrsExp(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal2);
         if (siErr) { free(pcVal); return(siErr); }
         siVal=siVal1-siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SUB-NUM(%"PRIi64"-%"PRIi64"=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,(int64_t)siVal1,(int64_t)siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,*ppVal,&flVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal2);
         if (siErr) { free(pcVal); return(siErr); }
         flVal=flVal1-flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SUB-FLT(%f-%f=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Subtracation not supported for type %s",apClpTyp[psArg->psFix->siTyp]);
      }
      free(pcVal);
   } else if ((psHdl->siTok==CLPTOK_NUM || psHdl->siTok==CLPTOK_FLT || psHdl->siTok==CLPTOK_STR) && psHdl->isSep==FALSE) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(pvHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
      siErr=siClpPrsExp(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,*ppVal,&siVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal2);
         if (siErr) { free(pcVal); return(siErr); }
         siVal=siVal1+siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-ADD-NUM(%"PRIi64"+%"PRIi64"=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,(int64_t)siVal1,(int64_t)siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,*ppVal,&flVal1);
         if (siErr) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal2);
         if (siErr) { free(pcVal); return(siErr); }
         flVal=flVal1+flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-ADD-FLT(%f+%f=%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      case CLPTYP_STRING:
         if ((*ppVal)[0]==pcVal[0]) {
         } else if (((*ppVal)[0]=='d' && pcVal[0]=='s') || ((*ppVal)[0]=='s' && pcVal[0]=='d')){
            (*ppVal)[0]='s';
         } else if (((*ppVal)[0]=='d' && pcVal[0]=='c') || ((*ppVal)[0]=='c' && pcVal[0]=='d')){
            (*ppVal)[0]='c';
         } else {
            siErr=CLPERR(psHdl,CLPERR_SEM,"Cannot concatenate different types (%c <> %c) of strings",(*ppVal)[0],pcVal[0]);
            free(pcVal);
            return(siErr);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-STR(%s+",fpcPre(pvHdl,siLev),siLev,siPos,*ppVal);
         srprintc(ppVal,pzVal,strlen(pcVal),"%s",pcVal+2);
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s=%s))\n",pcVal,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Addition not supported for type %s",apClpTyp[psArg->psFix->siTyp]);
      }
      free(pcVal);
   }
   return(CLP_OK);
}

static int siClpPrsVal(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siInd;
   size_t                        szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
   char*                         pcVal=(char*)malloc(szVal);
   if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed"));
   psHdl->apPat[siLev]=psArg;
   siInd=siClpPrsExp(pvHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
   if (siInd<0) { free(pcVal); return(siInd); }
   if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,isPrnLex(psArg,pcVal));
   siInd=siClpBldLit(pvHdl,siLev,siPos,psArg,pcVal);
   free(pcVal);
   return(siInd);
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
   size_t                        szPat=CLPINI_PATSIZ;
   char*                         pcPat=(char*)calloc(1,szPat);
   int                           siErr,siLev,siRow;
   if (pcPat==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store the path failed"));
   siLev=siClpPrsKywLst(pvHdl,siPos,&szPat,&pcPat);
   if (siLev<0) {
      free(pcPat);
      return(siLev);
   }
   if (psHdl->siTok==CLPTOK_SGN) {
      siRow=psHdl->siRow;
      psHdl->siTok=siClpScnSrc(pvHdl,CLPTYP_STRING,psTab);
      if (psHdl->siTok<0) {
         free(pcPat);
         return(psHdl->siTok);
      }
      if (psHdl->siTok==CLPTOK_STR) {
         char acSup[strlen(psHdl->pcLex)];
         strcpy(acSup,psHdl->pcLex+2);
         psHdl->siTok=siClpScnSrc(pvHdl,0,psTab);
         if (psHdl->siTok<0) {
            free(pcPat);
            return(psHdl->siTok);
         }
         siErr=siClpBldPro(pvHdl,pcPat,acSup,siRow);
         free(pcPat);
         return(siErr);
      } else {
         siErr=CLPERR(psHdl,CLPERR_SYN,"Property string (\"...\") missing (%s)",pcPat);
         free(pcPat);
         return(siErr);
      }
   } else {
      siErr=CLPERR(psHdl,CLPERR_SYN,"Assignment character ('=') missing (%s)",pcPat);
      free(pcPat);
      return(siErr);
   }
}

static int siClpPrsKywLst(
   void*                         pvHdl,
   const int                     siPos,
   size_t*                       pzPat,
   char**                        ppPat)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siLev=0;

   (*ppPat)[0]=EOS;
   while (psHdl->siTok==CLPTOK_KYW) {
      srprintc(ppPat,pzPat,strlen(psHdl->pcLex),"%s",psHdl->pcLex);
      psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
      if (psHdl->siTok<0) return(psHdl->siTok);
      if (psHdl->siTok==CLPTOK_DOT) {
         srprintc(ppPat,pzPat,0,".");
         psHdl->siTok=siClpScnSrc(pvHdl,0,NULL);
         if (psHdl->siTok<0) return(psHdl->siTok);
      }
      siLev++;
   }
   return(siLev);
}

/**********************************************************************/

static int siClpBldPro(
   void*                         pvHdl,
   const char*                   pcPat,
   const char*                   pcPro,
   const int                     siRow)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   TsSym*                        psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   char                          acKyw[CLPMAX_KYWSIZ];
   int                           siErr,siLev,i,l=strlen(psHdl->pcOwn)+strlen(psHdl->pcPgm)+strlen(psHdl->pcCmd)+2;
   char                          acRot[l+1];
   snprintf(acRot,sizeof(acRot),"%s.%s.%s",psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd);

   if (strxcmp(psHdl->isCas,acRot,pcPat,l,0,FALSE)==0) {
      if (pcPat[l]!='.') {
         return CLPERR(psHdl,CLPERR_SEM,"Property path (%s) is not valid",pcPat);
      }
      for (siLev=0,pcPtr=pcPat+l;pcPtr!=NULL && siLev<CLPMAX_HDEPTH && psTab!=NULL;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
         for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) acKyw[i]=pcKyw[i];
         acKyw[i]=EOS;
         siErr=siClpSymFnd(pvHdl,siLev,0,acKyw,psTab,&psArg,NULL);
         if (siErr<0) return(siErr);
         psHdl->apPat[siLev]=psArg;
         if (psArg!=NULL) psTab=psArg->psDep; else psTab=NULL;
      }
      if (psArg!=NULL) {
         if (CLPISF_ARG(psArg->psStd->uiFlg) || CLPISF_ALI(psArg->psStd->uiFlg)) {
            C08* pcHlp=realloc_nowarn(psArg->psFix->pcPro,strlen(pcPro)+1);
            if (pcHlp==NULL) {
               return CLPERR(psHdl,CLPERR_SIZ,"Build of property field failed (string (%d(%s)) too long)",(int)strlen(pcPro),pcPro);
            }
            psArg->psFix->pcPro=pcHlp;
            strcpy(psArg->psFix->pcPro,pcPro);
            psArg->psFix->pcDft=psArg->psFix->pcPro;
            pcHlp=realloc_nowarn(psArg->psFix->pcSrc,strlen(psHdl->pcSrc)+1);
            if (pcHlp==NULL) {
               return CLPERR(psHdl,CLPERR_SIZ,"Build of source field failed (string (%d(%s)) too long)",(int)strlen(psHdl->pcSrc),psHdl->pcSrc);
            }
            psArg->psFix->pcSrc=pcHlp;
            strcpy(psArg->psFix->pcSrc,psHdl->pcSrc);
            psArg->psFix->siRow=siRow;
            srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(isPrnLex2(psArg,pcPro)),"%s=\"%s\"\n",pcPat,isPrnLex2(psArg,pcPro));
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
         psArg->psVar->siRst=(CLPISF_DYN(psArg->psStd->uiFlg))?0:psArg->psFix->siSiz;
         if (CLPISF_FIX(psArg->psStd->uiFlg)) psArg->psVar->siRst*=psArg->psFix->siMax;
      }

      if (psArg->psFix->siTyp!=siTyp) {
         return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of link '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
      if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
         return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of link '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      if (psArg->psVar->pvDat==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of link are defined but data pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }

      if (CLPISF_DYN(psArg->psStd->uiFlg)) {
         void** ppDat=(void**)psArg->psVar->pvDat;
         (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
         if ((*ppDat)==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for link '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
      } else {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for link '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
         }
         if (psArg->psVar->pvPtr==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of link are defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
      }

      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>255) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRId64") for link '%s.%s' need more than 8 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRId64") for link '%s.%s' need more than 16 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LINK-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRId64") for link '%s.%s' need more than 32 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
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
      default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRId64") of link '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }

      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
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
   const char*                   pcPat=NULL;
   int                           siErr;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of switch '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of switch '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (psArg->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of switch defined but data pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for switch '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for switch '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      if (psArg->psVar->pvPtr==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of switch are defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
  }

   switch (psArg->psFix->siSiz) {
   case 1:
      if (siVal<(-128) || siVal>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRId64") of '%s.%s' need more than 8 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 2:
      if (siVal<(-32768) || siVal>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRId64") of '%s.%s' need more than 16 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-SWITCH-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 4:
      if (siVal<(-2147483648LL) || siVal>4294967295LL) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRId64") of '%s.%s' need more than 32 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
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
      return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRId64") of '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   pcPat=fpcPat(pvHdl,siLev);
   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw),"%s.%s=ON\n",pcPat,psArg->psStd->pcKyw);

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) return(siErr);
   }
   return(psArg->psFix->siTyp);
}

static int siClpBldNum(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const int                     siTyp=CLPTYP_NUMBER;
   I64                           siVal=psArg->psFix->siOid;
   const char*                   pcPat=NULL;
   int                           siErr;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of argument '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }
   if (psArg->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but data pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
      if (psArg->psVar->pvPtr==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
      }
  }

   switch (psArg->psFix->siSiz) {
   case 1:
      if (siVal<(-128) || siVal>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Default value (%"PRId64") of '%s.%s' need more than 8 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-NUMBER-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 2:
      if (siVal<(-32768) || siVal>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Default value (%"PRId64") of '%s.%s' need more than 16 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-NUMBER-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 4:
      if (siVal<(-2147483648LL) || siVal>4294967295LL) {
         return CLPERR(psHdl,CLPERR_SEM,"Default value (%"PRId64") of '%s.%s' need more than 32 Bit",isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-NUMBER-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   case 8:
      *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
      if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-NUMBER-64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                              fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
      break;
   default:
      return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRId64") of '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,siVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   pcPat=fpcPat(pvHdl,siLev);
   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw)+16,"%s.%s=DEFAULT(%d)\n",pcPat,psArg->psStd->pcKyw,(int)siVal);

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) return(siErr);
   }
   return(psArg->psFix->siTyp);
}

static int siClpBldLit(
   void*                         pvHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           siErr;
   int                           l0=0;
   int                           l1=0;
   int                           l2=0;
   int                           siSln=0;
   I64                           siVal=0;
   F64                           flVal=0;
   char*                         pcHlp=NULL;
   const char*                   pcKyw=NULL;
   const char*                   pcPat=NULL;
   TsSym*                        psCon;

   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of argument '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but data pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }

   switch (psArg->psFix->siTyp) {
   case CLPTYP_SWITCH:
   case CLPTYP_NUMBER:
      if (CLPISF_DYN(psArg->psStd->uiFlg)) {
         void** ppDat=(void**)psArg->psVar->pvDat;
         (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
         if ((*ppDat)==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
      } else {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         if (psArg->psVar->pvPtr==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
      }
      siErr=siFromNumberLexem(pvHdl,siLev,siPos,psArg,pcVal,&siVal);
      if (siErr) return(siErr);
      if (siVal<0 && CLPISF_UNS(psArg->psStd->uiFlg)) {
         return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' is negative (%"PRIi64") but marked as unsigned",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,siVal);
      }
      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>255) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' need more than 8 Bit",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' need more than 16 Bit",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRId64"\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' need more than 32 Bit",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
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
         return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%s) of '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_FLOATN:
      if (CLPISF_DYN(psArg->psStd->uiFlg)) {
         void** ppDat=(void**)psArg->psVar->pvDat;
         (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
         if ((*ppDat)==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
      } else {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
         if (psArg->psVar->pvPtr==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
         }
      }
      siErr=siFromFloatLexem(pvHdl,siLev,siPos,psArg,pcVal,&flVal);
      if (siErr) return(siErr);
      if (flVal<0 && CLPISF_UNS(psArg->psStd->uiFlg)) {
         return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' is negative (%f) but marked as unsigned",isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,flVal);
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
      default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%s) of '%s.%s' is not 4 (float) or 8 (double))",psArg->psFix->siSiz,isPrnStr(psArg,pcVal),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) return(siErr);
      break;
   case CLPTYP_STRING:
      if (CLPISF_FIX(psArg->psStd->uiFlg)) {
         l0=psArg->psFix->siSiz;
         if (CLPISF_DYN(psArg->psStd->uiFlg)) {
            void** ppDat=(void**)psArg->psVar->pvDat;
            (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
            if ((*ppDat)==NULL) {
               return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
         } else {
            if (psArg->psVar->siRst<psArg->psFix->siSiz) {
               return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
            }
            if (psArg->psVar->pvPtr==NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
            }
         }
      } else {
         l0=psArg->psVar->siRst;
         if (!CLPISF_DYN(psArg->psStd->uiFlg)) {
            if (psArg->psVar->pvPtr==NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
            }
         }
      }
      switch (pcVal[0]) {
      case 'x':
         l1=strlen(pcVal+2);
         if (CLPISF_BIN(psArg->psStd->uiFlg)) {
            if (l1%2) {
               return CLPERR(psHdl,CLPERR_LEX,"Length of hexadecimal string (%c(%s)) for '%s.%s' is not a multiple of 2",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            if ((l1/2)>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg) && !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+(l1/2)>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+(l1/2)+4,&psArg->psVar->siInd);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+(l1/2)+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"Hexadecimal string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,2*l0);
               }
            }
            l2=hex2bin(pcVal+2,(U08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1/2) {
               return CLPERR(psHdl,CLPERR_SEM,"Hexadecimal string (%c(%s)) of '%s.%s' cannot be converted from hex to bin",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            siSln=l2;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-HEX(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for '%s.%s' is binary (only null-terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 'a':
         l1=strlen(pcVal+2);
         if (CLPISF_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"ASCII string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
            }
            l2=chr2asc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               return CLPERR(psHdl,CLPERR_SEM,"ASCII string (%c(%s)) of '%s.%s' cannot be converted to ASCII",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-ASC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for '%s.%s' is binary (only null-terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 'e':
         l1=strlen(pcVal+2);
         if (CLPISF_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg) && !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"EBCDIC string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
            }
            l2=chr2ebc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               return CLPERR(psHdl,CLPERR_SEM,"EBCDIC string (%c(%s)) of '%s.%s' cannot be converted to EBCDIC",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-EBC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for '%s.%s' is binary (only null-terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 'c':
         l1=strlen(pcVal+2);
         if (CLPISF_BIN(psArg->psStd->uiFlg)) {
            if (l1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
               }
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1); l2=l1;
            siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"String literal (%c(%s)) for '%s.%s' is binary (only null-terminated character string permitted)",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         break;
      case 's':
         if (CLPISF_FIL(psArg->psStd->uiFlg)) {
            pcHlp=dmapfil(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
         } else if (CLPISF_LAB(psArg->psStd->uiFlg)) {
            pcHlp=dmaplab(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
         } else {
            pcHlp=dmapstr(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
         }
         if (pcHlp==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"String mapping (memory allocation) for argument '%s.%s' failed",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         }
         l1=strlen(pcHlp);
         if (l1+1>l0) {
            if (CLPISF_DYN(psArg->psStd->uiFlg) && !CLPISF_FIX(psArg->psStd->uiFlg)) {
               void** ppDat=(void**)psArg->psVar->pvDat;
               if (psArg->psVar->siLen+l1+1>psArg->psFix->siSiz) {
                  free(pcHlp);
                  return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
               if ((*ppDat)==NULL) {
                  free(pcHlp);
                  return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
            } else {
               siErr=CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcHlp),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0-1);
               free(pcHlp);
               return(siErr);
            }
         }
         memcpy(psArg->psVar->pvPtr,pcHlp,l1);
         ((char*)psArg->psVar->pvPtr)[l1]=EOS;
         free(pcHlp);
         l2=l1+1; siSln=l1;
         if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                 fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         break;
      case 'f':
      {
         int                           siTok;
         int                           siRow;
         int                           siSiz=0;
         char*                         pcDat=NULL;
         const char*                   pcCur;
         const char*                   pcInp;
         const char*                   pcOld;
         const char*                   pcRow;
         // TODO: Stack Allokation mit unbegrenzter Größe = Potenzielle Sicherheitslücke
         char                          acSrc[strlen(psHdl->pcSrc)+1];
         size_t                        szLex=CLPINI_LEXSIZ;
         char*                         pcLex=(char*)calloc(1,szLex);
         char*                         pcFil=dcpmapfil(pcVal+2);
         if (pcLex==NULL || pcFil==NULL) {
            if (pcLex!=NULL) free(pcLex);
            if (pcFil!=NULL) free(pcFil);
            return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store the lexem or file name failed"));
         }
         siErr=file2str(pcFil,&pcDat,&siSiz,filemode("r"));
         if (siErr<0) {
            switch(siErr) {
            case -1: siErr=CLPERR(psHdl,CLPERR_INT,"Illegal parameters passed to file2str() (Bug)%s","");break;
            case -2: siErr=CLPERR(psHdl,CLPERR_SYS,"Open of string file (%s) failed (%d - %s)",pcFil,errno,strerror(errno));break;
            case -3: siErr=CLPERR(psHdl,CLPERR_SEM,"String file (%s) is too big (integer overflow)",pcFil);break;
            case -4: siErr=CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for string file (%s) failed",pcFil);break;
            case -5: siErr=CLPERR(psHdl,CLPERR_SYS,"Read of string file (%s) failed (%d - %s)",pcFil,errno,strerror(errno));break;
            default: siErr=CLPERR(psHdl,CLPERR_SYS,"An unknown error occurred while reading string file (%s)",pcFil);break;
            }
            if (pcDat!=NULL) free(pcDat);
            free(pcLex); free(pcFil);
            return(siErr);
         }
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"STRING-FILE-BEGIN(%s)\n",pcFil);
         strcpy(acSrc,psHdl->pcSrc);
         srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_SRF)+strlen(pcFil),"%s%s",CLPSRC_SRF,pcFil);
         pcCur=psHdl->pcCur; psHdl->pcCur=pcDat;
         pcInp=psHdl->pcInp; psHdl->pcInp=pcDat;
         pcOld=psHdl->pcOld; psHdl->pcOld=pcDat;
         pcRow=psHdl->pcRow; psHdl->pcRow=pcDat;
         siRow=psHdl->siRow; psHdl->siRow=1;
         psHdl->siBuf++;
         siTok=siClpScnNat(pvHdl,psHdl->pfErr,psHdl->pfScn,&psHdl->pcCur,&szLex,&pcLex,CLPTYP_STRING,psArg,NULL,NULL);
         if (siTok<0) {
            free(pcDat);
            free(pcLex);
            free(pcFil);
            return(siTok);
         }
         if (siTok!=CLPTOK_STR) {
            siErr=CLPERR(psHdl,CLPERR_SYN,"The token (%s(%s)) is not allowed in a string file (%c(%s)) of '%s.%s'",apClpTok[siTok],pcLex,pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            free(pcDat);
            free(pcLex);
            free(pcFil);
            return(siErr);
         }
         if (pcLex[0]=='f') {
            siErr=CLPERR(psHdl,CLPERR_SYN,"Define a string file (%c(%s)) in a string file (%c(%s)) is not allowed (%s.%s)",pcLex[0],pcLex+2,pcVal[0],pcVal+2,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            free(pcDat);
            free(pcLex);
            free(pcFil);
            return(siErr);
         }
         siErr=siClpBldLit(pvHdl,siLev,siPos,psArg,pcLex);
         psHdl->siBuf--;
         strcpy(psHdl->pcSrc,acSrc);
         psHdl->pcCur=pcCur; psHdl->pcInp=pcInp; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
         if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"STRING-FILE-END(%s)\n",pcFil);
         free(pcDat);
         free(pcLex);
         free(pcFil);
         return(siErr);
      }
      case 'd':
         if (CLPISF_BIN(psArg->psStd->uiFlg)) {
            if (CLPISF_HEX(psArg->psStd->uiFlg)) {
               l1=strlen(pcVal+2);
               if (l1%2) {
                  return CLPERR(psHdl,CLPERR_LEX,"Length of hexadecimal string (%c(%s)) for '%s.%s' is not a multiple of 2",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               if ((l1/2)>l0) {
                  if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                     void** ppDat=(void**)psArg->psVar->pvDat;
                     if (psArg->psVar->siLen+(l1/2)>psArg->psFix->siSiz) {
                        return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+(l1/2)+4,&psArg->psVar->siInd);
                     if ((*ppDat)==NULL) {
                        return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+(l1/2)+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
                  } else {
                     return CLPERR(psHdl,CLPERR_LEX,"Hexadecimal string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,2*l0);
                  }
               }
               l2=hex2bin(pcVal+2,(U08*)psArg->psVar->pvPtr,l1);
               if (l2!=l1/2) {
                  return CLPERR(psHdl,CLPERR_SEM,"Hexadecimal string (%c(%s)) of '%s.%s' cannot be converted from hex to bin",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siSln=l2;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-HEX(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            } else if (CLPISF_ASC(psArg->psStd->uiFlg)) {
               l1=strlen(pcVal+2);
               if (l1>l0) {
                  if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                     void** ppDat=(void**)psArg->psVar->pvDat;
                     if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                        return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                     if ((*ppDat)==NULL) {
                        return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
                  } else {
                     return CLPERR(psHdl,CLPERR_LEX,"ASCII string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
                  }
               }
               l2=chr2asc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
               if (l2!=l1) {
                  return CLPERR(psHdl,CLPERR_SEM,"ASCII string (%c(%s)) of '%s.%s' cannot be converted to ASCII",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siSln=l1;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-ASC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            } else if (CLPISF_EBC(psArg->psStd->uiFlg)) {
               l1=strlen(pcVal+2);
               if (l1>l0) {
                  if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                     void** ppDat=(void**)psArg->psVar->pvDat;
                     if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                        return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                     if ((*ppDat)==NULL) {
                        return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
                  } else {
                     return CLPERR(psHdl,CLPERR_LEX,"EBCDIC string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
                  }
               }
               l2=chr2ebc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
               if (l2!=l1) {
                  return CLPERR(psHdl,CLPERR_SEM,"EBCDIC string (%c(%s)) of '%s.%s' cannot be converted to EBCDIC",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
               }
               siSln=l1;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-EBC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            } else {
               l1=strlen(pcVal+2);
               if (l1>l0) {
                  if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                     void** ppDat=(void**)psArg->psVar->pvDat;
                     if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                        return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                     if ((*ppDat)==NULL) {
                        return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     }
                     psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
                  } else {
                     return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0);
                  }
               }
               memcpy(psArg->psVar->pvPtr,pcVal+2,l1); l2=l1;
               siSln=l1;
               if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                       fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            }
         } else {
            if (CLPISF_FIL(psArg->psStd->uiFlg)) {
               pcHlp=dmapfil(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg));
            } else if (CLPISF_LAB(psArg->psStd->uiFlg)) {
               pcHlp=dmaplab(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg));
            } else {
               pcHlp=dmapstr(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg));
            }
            if (pcHlp==NULL) {
               return CLPERR(psHdl,CLPERR_MEM,"String mapping (memory allocation) for argument '%s.%s' failed",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
            }
            l1=strlen(pcHlp);
            if (l1+1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1+1>psArg->psFix->siSiz) {
                     free(pcHlp);
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd);
                  if ((*ppDat)==NULL) {
                     free(pcHlp);
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+l1+4,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  siErr=CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcHlp),fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,l0-1);
                  free(pcHlp);
                  return(siErr);
               }
            }
            memcpy(psArg->psVar->pvPtr,pcHlp,l1);
            ((char*)psArg->psVar->pvPtr)[l1]=EOS;
            free(pcHlp);
            l2=l1+1; siSln=l1;
            if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(pvHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
         }
         break;
      default:
         CLPERR(psHdl,CLPERR_LEX,"String prefix (%c) of '%s.%s' is not supported",pcVal[0],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following values:%s","");
         CLPERRADD(psHdl,1,"x - for conversion from hex to bin%s","");
         CLPERRADD(psHdl,1,"a - for conversion in ASCII%s","");
         CLPERRADD(psHdl,1,"e - for conversion in EBCDIC%s","");
         CLPERRADD(psHdl,1,"c - for no conversion (normal character string without null termination)%s","");
         CLPERRADD(psHdl,1,"s - normal character string with null termination%s","");
         CLPERRADD(psHdl,1,"f - use file content as string%s","");
         return(CLPERR_LEX);
      }

      if (CLPISF_FIX(psArg->psStd->uiFlg)) {
         memset(((U08*)psArg->psVar->pvPtr)+l2,0,psArg->psFix->siSiz-l2);
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
         psArg->psVar->siLen+=psArg->psFix->siSiz;
         psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
         siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
         if (siErr<0) return(siErr);
      } else {
         if (CLPISF_DLM(psArg->psStd->uiFlg)) {
            ((char*)psArg->psVar->pvPtr)[l2]=0xFF; // end of string list
         }
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+l2;
         psArg->psVar->siLen+=l2;
         psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:l2;
         siErr=siClpBldLnk(pvHdl,siLev,siPos,l2,psArg->psFix->psEln,TRUE);
         if (siErr<0) return(siErr);
      }
      siErr=siClpBldLnk(pvHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
      if (siErr<0) return(siErr);
      break;
   default: return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter '%s.%s' not supported in this case (literal)",psArg->psFix->siTyp,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
   }
   psArg->psVar->siCnt++;

   for (psCon=psArg->psDep;psCon!=NULL;psCon=psCon->psNxt) {
      if (pcKyw==NULL && psCon->psFix->siTyp==psArg->psFix->siTyp) {
         switch (psCon->psFix->siTyp) {
         case CLPTYP_NUMBER:
            switch (psCon->psFix->siSiz) {
            case 1: if (siVal==((I08*)psCon->psVar->pvDat)[0]) pcKyw=psCon->psStd->pcKyw; break;
            case 2: if (siVal==((I16*)psCon->psVar->pvDat)[0]) pcKyw=psCon->psStd->pcKyw; break;
            case 4: if (siVal==((I32*)psCon->psVar->pvDat)[0]) pcKyw=psCon->psStd->pcKyw; break;
            case 8: if (siVal==((I64*)psCon->psVar->pvDat)[0]) pcKyw=psCon->psStd->pcKyw; break;
            }
            break;
         case CLPTYP_FLOATN:
            switch (psCon->psFix->siSiz) {
            case 4: if (flVal==((F32*)psCon->psVar->pvDat)[0]) pcKyw=psCon->psStd->pcKyw; break;
            case 8: if (flVal==((F64*)psCon->psVar->pvDat)[0]) pcKyw=psCon->psStd->pcKyw; break;
            }
            break;
         default:
            if (l2>0 && psCon->psVar->siLen==l2 && memcmp(psCon->psVar->pvDat,((char*)psArg->psVar->pvPtr)-l2,l2)==0) {
               pcKyw=psCon->psStd->pcKyw;
            }
            break;
         }
      }
   }
   pcPat=fpcPat(pvHdl,siLev);
   if (pcKyw!=NULL) {
      if (psArg->psFix->siTyp==CLPTYP_NUMBER && (CLPISF_TIM(psArg->psStd->uiFlg) || pcVal[0]=='t')) {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw)+strlen(isPrnStr(psArg,pcVal))+strlen(cstime(siVal,NULL)),"%s.%s=%s(%s(%s))\n",pcPat,psArg->psStd->pcKyw,pcKyw,isPrnStr(psArg,pcVal),cstime(siVal,NULL));
      } else {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw)+strlen(isPrnStr(psArg,pcVal)),"%s.%s=%s(%s)\n",pcPat,psArg->psStd->pcKyw,pcKyw,isPrnStr(psArg,pcVal));
      }
   } else {
      if (psArg->psFix->siTyp==CLPTYP_NUMBER && (CLPISF_TIM(psArg->psStd->uiFlg) || pcVal[0]=='t')) {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw)+strlen(isPrnStr(psArg,pcVal))+strlen(cstime(siVal,NULL)),"%s.%s=%s(%s)\n",pcPat,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),cstime(siVal,NULL));
      } else {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw)+strlen(isPrnStr(psArg,pcVal)),"%s.%s=%s\n",pcPat,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal));
      }
   }

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) return(siErr);
   }
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   if (psHdl->pvDat!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
         *psSav=*psHlp->psVar;
         psHlp->psVar->pvDat=((char*)psHdl->pvDat)+psHlp->psFix->siOfs;
         psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
         psHlp->psVar->siCnt=0;
         psHlp->psVar->siLen=0;
         psHlp->psVar->siInd=0;
         psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
         if (CLPISF_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(pvHdl,0,i,psHlp);
      if (siErr<0) {
         CLPERRADD(psHdl,0,"Set default value for argument '%s.%s' failed",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s' not specified",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please specify parameter:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s' is smaller than required minimum amount (%d)",psHlp->psVar->siCnt,fpcPat(pvHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
               CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   if (psHdl->pvDat!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
         *psSav=*psHlp->psVar;
         psHlp->psVar->pvDat=((char*)psHdl->pvDat);
         psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
         psHlp->psVar->siCnt=0;
         psHlp->psVar->siLen=0;
         psHlp->psVar->siInd=0;
         psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
         if (CLPISF_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,0),psTab->psStd->pcKyw);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(pvHdl,0,i,psHlp);
         if (siErr<0) {
            CLPERRADD(psHdl,0,"Set default value for argument '%s.%s' failed",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s' not specified",fpcPat(pvHdl,0),psHlp->psStd->pcKyw);
                  CLPERRADD(psHdl,0,"Please specify parameter:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_SEL(psHlp->psStd->uiFlg));
               } else {
                  CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s' is smaller than required minimum amount (%d)",psHlp->psVar->siCnt,fpcPat(pvHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
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
   const char*                   pcPat;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psDep==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
   }

   for (psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
      *psSav=*psHlp->psVar;
      psHlp->psVar->pvDat=((char*)psArg->psVar->pvPtr)+psHlp->psFix->siOfs;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siInd=0;
      psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
      if (CLPISF_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
   }

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-BEGIN-OBJECT-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   pcPat=fpcPat(pvHdl,siLev);
   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw),"%s.%s(\n",pcPat,psArg->psStd->pcKyw);

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
   const char*                   pcPat;
   int                           siErr,i;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(pvHdl,siLev+1,i,psHlp);
      if (siErr<0) {
         CLPERRADD(psHdl,0,"Set default value for argument '%s.%s.%s' failed",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s.%s' not specified",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please specify parameter:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s.%s' is smaller than required minimum amount (%d)",psDep->psVar->siCnt,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
               CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
               vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            }
            return(CLPERR_SEM);
         }
      }
   }

   for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) *psHlp->psVar=*psSav;

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-END-OBJECT-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   pcPat=fpcPat(pvHdl,siLev);
   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(psArg->psStd->pcKyw),"%s.%s)\n",pcPat,psArg->psStd->pcKyw);

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) return(siErr);
   }
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
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psDep==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAlloc(pvHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d) for argument '%s.%s' failed",psArg->psVar->siLen+psArg->psFix->siSiz,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
      }
   }

   for (psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) {
      *psSav=*psHlp->psVar;
      psHlp->psVar->pvDat=(char*)psArg->psVar->pvPtr;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
      if (CLPISF_FIX(psHlp->psStd->uiFlg)) psHlp->psVar->siRst*=psHlp->psFix->siMax;
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
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",apClpTyp[siTyp],fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[psArg->psFix->siTyp]);
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,apClpTyp[siTyp]);
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(pvHdl,siLev+1,i,psHlp);
         if (siErr<0) {
            CLPERRADD(psHdl,0,"Set default value for argument '%s.%s.%s' failed",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s.%s' not specified",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
                  CLPERRADD(psHdl,0,"Please specify parameter:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
               } else {
                  CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s.%s' is smaller than required minimum amount (%d)",psDep->psVar->siCnt,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
                  vdClpPrnArg(pvHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
               }
               return(CLPERR_SEM);
            }
         }
      }
   }

   for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,psSav++) *psHlp->psVar=*psSav;

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   if (psHdl->pfBld!=NULL) fprintf(psHdl->pfBld,"%s BUILD-END-OVERLAY-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) return(siErr);
   siErr=siClpBldLnk(pvHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) return(siErr);
   if(siOid){
      siErr=siClpBldLnk(pvHdl,siLev,siPos,siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) return(siErr);
   }
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
   TsSym*                        psDep=NULL;
   TsSym*                        psVal=NULL;
   size_t                        szVal=CLPINI_VALSIZ;
   char*                         pcVal=(char*)calloc(1,szVal);
   // TODO: Stack Allokation mit unbegrenzter Größe = Potenzielle Sicherheitslücke
   char                          acSrc[strlen(psHdl->pcSrc)+1];
   char                          acLex[strlen(psHdl->pcLex)+1];
   TsVar                         asSav[CLPMAX_TABCNT];
   int                           siErr,siRow,siTok;
   const char*                   pcCur;
   const char*                   pcInp;
   const char*                   pcOld;
   const char*                   pcRow;
   if (pcVal==NULL) return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store the value expression failed"));
   if (CLPISF_ARG(psArg->psStd->uiFlg) && psArg->psVar->siCnt==0 && psArg->psFix->pcDft!=NULL && strlen(psArg->psFix->pcDft)) {
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"SUPPLEMENT-LIST-PARSER-BEGIN(%s.%s=%s)\n",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psFix->pcDft);
      strcpy(acSrc,psHdl->pcSrc);
      strcpy(acLex,psHdl->pcLex);
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(psArg->psFix->pcSrc),"%s",psArg->psFix->pcSrc);
      pcCur=psHdl->pcCur; psHdl->pcCur=psArg->psFix->pcDft;
      pcInp=psHdl->pcInp; psHdl->pcInp=psArg->psFix->pcDft;
      pcOld=psHdl->pcOld; psHdl->pcOld=psArg->psFix->pcDft;
      pcRow=psHdl->pcRow; psHdl->pcRow=psArg->psFix->pcDft;
      siRow=psHdl->siRow; psHdl->siRow=psArg->psFix->siRow;
      siTok=psHdl->siTok;
      psHdl->siBuf++;

      for (psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg);psHdl->siTok>=0 && psHdl->siTok!=CLPTOK_END;
           psHdl->siTok=siClpScnSrc(pvHdl,psArg->psFix->siTyp,psArg)) {
         if (psHdl->siTok==CLPTOK_KYW) {
            if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
               if (strxcmp(psHdl->isCas,psHdl->pcLex,"INIT",0,0,FALSE)!=0) {
                  siErr=CLPERR(psHdl,CLPERR_SYN,"Keyword (%s) in default / property definition for object '%s.%s' is not 'INIT'",psHdl->pcLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                  free(pcVal);
                  return(siErr);
               }
               siErr=siClpIniObj(pvHdl,siLev,siPos,psArg,&psDep,asSav);
               if (siErr<0) { free(pcVal); return(siErr); }
               siErr=siClpFinObj(pvHdl,siLev,siPos,psArg,psDep,asSav);
               if (siErr<0) { free(pcVal); return(siErr); }
            } else  if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
               siErr=siClpIniOvl(pvHdl,siLev,siPos,psArg,&psDep,asSav);
               if (siErr<0) { free(pcVal); return(siErr); }
               siErr=siClpSymFnd(pvHdl,siLev+1,siPos,psHdl->pcLex,psDep,&psVal,NULL);
               if (siErr<0) { free(pcVal); return(siErr); }
               siErr=siClpFinOvl(pvHdl,siLev,siPos,psArg,psDep,asSav,psVal->psFix->siOid);
               if (siErr<0) { free(pcVal); return(siErr); }
            } else if (psArg->psFix->siTyp==CLPTYP_SWITCH) {
                  if (strxcmp(psHdl->isCas,psHdl->pcLex,"ON",0,0,FALSE)!=0 && strxcmp(psHdl->isCas,psHdl->pcLex,"OFF",0,0,FALSE)!=0) {
                     siErr=CLPERR(psHdl,CLPERR_SYN,"Keyword (%s) in default / property definition for switch '%s.%s' is not 'ON' or 'OFF'",psHdl->pcLex,fpcPat(pvHdl,siLev),psArg->psStd->pcKyw);
                     free(pcVal);
                     return(siErr);
                  }
                  if (strxcmp(psHdl->isCas,psHdl->pcLex,"ON",0,0,FALSE)==0) {
                     siErr=siClpBldSwt(pvHdl,siLev,siPos,psArg);
                     if (siErr<0) { free(pcVal); return(siErr); }
                  }
            } else {
               siErr=siClpPrsExp(pvHdl,siLev,siPos,FALSE,psArg,&szVal,&pcVal);
               if (siErr<0) { free(pcVal); return(siErr); }
               if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,isPrnLex(psArg,pcVal));
               siErr=siClpBldLit(pvHdl,siLev,siPos,psArg,pcVal);
               if (siErr<0) { free(pcVal); return(siErr); }
            }
         } else {
            siErr=siClpPrsExp(pvHdl,siLev,siPos,FALSE,psArg,&szVal,&pcVal);
            if (siErr<0) { free(pcVal); return(siErr); }
            if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(pvHdl,siLev),siLev,siPos,isPrnLex(psArg,pcVal));
            siErr=siClpBldLit(pvHdl,siLev,siPos,psArg,pcVal);
            if (siErr<0) { free(pcVal); return(siErr); }
         }
      }
      if (psHdl->siTok<0) { free(pcVal); return(psHdl->siTok); }
      psHdl->siBuf--;
      psHdl->siTok=siTok;
      strcpy(psHdl->pcLex,acLex);
      strcpy(psHdl->pcSrc,acSrc);
      psHdl->pcCur=pcCur; psHdl->pcInp=pcInp; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
      if (psHdl->pfPrs!=NULL) fprintf(psHdl->pfPrs,"SUPPLEMENT-LIST-PARSER-END(%s.%s=%s)\n",fpcPat(pvHdl,siLev),psArg->psStd->pcKyw,psArg->psFix->pcDft);
   }
   free(pcVal);
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
   const unsigned int            isSel,
   const unsigned int            isCon)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const char*                   p=fpcPre(pvHdl,siLev);
   int                           i,siLen;
   const char*                   a="TYPE";
   const char*                   b=(isSel)?"SELECTION":((isCon)?"KEYWORD":apClpTyp[siTyp]);

   if (pcAli!=NULL && *pcAli) {
      a="ALIAS";
      b=pcAli;
   }
   if (pfOut!=NULL) {
      if (psHdl->isCas) {
         if (pcDft!=NULL && *pcDft) {
            fprintf(pfOut,"%s %s (%s: %s) - ",p,pcKyw,a,b);
            efprintf(pfOut,"%s",pcHlp);
            fprintf(pfOut," (PROPERTY: %c%s%c)\n",C_SBO,pcDft,C_SBC);
         } else {
            fprintf(pfOut,"%s %s (%s: %s) - ",p,pcKyw,a,b);
            efprintf(pfOut,"%s\n",pcHlp);
         }
         fprintf(pfOut,"%s ",p);
         for (i=0;i<siKwl;i++) fprintf(pfOut,"%c",C_CRT);
         fprintf(pfOut,"\n");
      } else {
         siLen=strlen(pcKyw);
         fprintf(pfOut,"%s ",p);
         for (i=0;i<siKwl;i++) fprintf(pfOut,"%c",toupper(pcKyw[i]));
         for (/*i=i*/;i<siLen;i++) fprintf(pfOut,"%c",tolower(pcKyw[i]));
         if (pcDft!=NULL && *pcDft) {
            fprintf(pfOut," (%s: %s) - ",a,b);
            efprintf(pfOut,"%s",pcHlp);
            fprintf(pfOut," (PROPERTY: %c%s%c)\n",C_SBO,pcDft,C_SBC);
         } else {
            fprintf(pfOut," (%s: %s) - ",a,b);
            efprintf(pfOut,"%s\n",pcHlp);
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
      if (CLPISF_CON(psTab->psStd->uiFlg)) {
         if (siTyp<0) siTyp=psTab->psHih->psFix->siTyp;
         if (CLPISF_SEL(psTab->psHih->psStd->uiFlg)==FALSE) {
            if (pfOut!=NULL) {
               if (siTyp>=0) {
                  fprintf(pfOut,"%s Enter a value (TYPE: %s) or use one of the keywords below:\n",fpcPre(pvHdl,siLev),apClpTyp[siTyp]);
               } else {
                  fprintf(pfOut,"%s Enter a value or use one of the keywords below:\n",fpcPre(pvHdl,siLev));
               }
            }
         }
      }
   }
   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
      if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISF_LNK(psHlp->psStd->uiFlg)) {
         vdClpPrnArg(pvHdl,pfOut,siLev,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                     CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
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
         if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISF_LNK(psHlp->psStd->uiFlg)) {
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
         if (CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psStd->psAli==psTab) {
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
   }

   if (pfOut!=NULL) {
      if (siLev<siDep || siDep>9) {
         if (psArg==NULL) {
            if (psHdl->isOvl) {
               pcSep=psHdl->pcOpt;
               if (siCnt==0) {
                  fprintf(pfOut,"%s.%c",psHdl->pcCmd,C_CBO);
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
                  fprintf(pfOut,"%s.%c",fpcPat(pvHdl,siLev),C_CBO);
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
            if (CLPISF_CMD(psHlp->psStd->uiFlg) && !CLPISF_LNK(psHlp->psStd->uiFlg) && !CLPISF_ALI(psHlp->psStd->uiFlg)) {
               if (isSkr) {
                  if (k) fprintf(pfOut,"\n%s ",fpcPre(pvHdl,siLev));
                    else fprintf(pfOut,"%s "  ,fpcPre(pvHdl,siLev));
                  if (isMin) {
                     if (psHlp->psFix->siMin) fprintf(pfOut,"%c ",C_EXC); else fprintf(pfOut,"%c ",'?');
                  }
               } else {
                  if (k) fprintf(pfOut,"%s",pcSep);
                  if (isMin) {
                     if (psHlp->psFix->siMin) fprintf(pfOut,"%c",C_EXC); else fprintf(pfOut,"%c",'?');
                  }
               }
               k++;
               switch (psHlp->psFix->siTyp) {
               case CLPTYP_SWITCH:
                  if (psHlp->psFix->siMax==1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               case CLPTYP_NUMBER:
                  if (psHlp->psFix->siMax==1) {
                     if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);
                        if (CLPISF_DEF(psHlp->psStd->uiFlg)) {
                           fprintf(pfOut,"%c%s",C_SBO,CLP_ASSIGNMENT);
                           vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                           fprintf(pfOut,"%c",C_SBC);
                        } else {
                           fprintf(pfOut,CLP_ASSIGNMENT);
                           vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        }
                     } else {
                        if (psHlp->psDep!=NULL) {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);
                           if (CLPISF_DEF(psHlp->psStd->uiFlg)) {
                              fprintf(pfOut,"%c%snum%s",C_SBO,CLP_ASSIGNMENT,psHdl->pcOpt);
                              vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                              fprintf(pfOut,"%c",C_SBC);
                           } else {
                              fprintf(pfOut,"%snum%s",CLP_ASSIGNMENT,psHdl->pcOpt);
                              vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                           }
                        } else {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);
                           if (CLPISF_DEF(psHlp->psStd->uiFlg)) {
                              fprintf(pfOut,"%c%snum%c",C_SBO,CLP_ASSIGNMENT,C_SBC);
                           } else {
                              fprintf(pfOut,"%snum",CLP_ASSIGNMENT);
                           }
                        }
                     }
                  } else if (psHlp->psFix->siMax>1) {
                     if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c",C_SBO);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        fprintf(pfOut,"...%c",C_SBC);
                     } else {
                        if (psHlp->psDep!=NULL) {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%cnum%s",C_SBO,psHdl->pcOpt);
                           vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                           fprintf(pfOut,"...%c",C_SBC);
                        } else {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%cnum...%c",C_SBO,C_SBC);
                        }
                     }
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               case CLPTYP_FLOATN:
                  if (psHlp->psFix->siMax==1) {
                     if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
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
                     if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c",C_SBO);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        fprintf(pfOut,"...%c",C_SBC);
                     } else {
                        if (psHlp->psDep!=NULL) {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%cflt%s",C_SBO,psHdl->pcOpt);
                           vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                           fprintf(pfOut,"...%c",C_SBC);
                        } else {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%cflt...%c",C_SBO,C_SBC);
                        }
                     }
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               case CLPTYP_STRING:
                  if (CLPISF_BIN(psHlp->psStd->uiFlg)) {
                     if (CLPISF_HEX(psHlp->psStd->uiFlg)) {
                        pcHlp="bin-hex";
                     } else if (CLPISF_ASC(psHlp->psStd->uiFlg)) {
                        pcHlp="bin-ascii";
                     } else if (CLPISF_EBC(psHlp->psStd->uiFlg)) {
                        pcHlp="bin-ebcdic";
                     } else if (CLPISF_CHR(psHlp->psStd->uiFlg)) {
                        pcHlp="bin-char";
                     } else {
                        pcHlp="bin";
                     }
                  } else pcHlp="str";
                  if (psHlp->psFix->siMax==1) {
                     if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,CLP_ASSIGNMENT);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                     } else {
                        if (psHlp->psDep!=NULL) {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%s'%s'%s",CLP_ASSIGNMENT,pcHlp,psHdl->pcOpt);
                           vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        } else {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%s'%s'",CLP_ASSIGNMENT,pcHlp);
                        }
                     }
                  } else if (psHlp->psFix->siMax>1) {
                     if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
                        vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c",C_SBO);
                        vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                        fprintf(pfOut,"...%c",C_SBC);
                     } else {
                        if (psHlp->psDep!=NULL) {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c'%s'%s",C_SBO,pcHlp,psHdl->pcOpt);
                           vdClpPrnOpt(pfOut,psHdl->pcOpt,psHlp->psFix->siTyp,psHlp->psDep);
                           fprintf(pfOut,"...%c",C_SBC);
                        } else {
                           vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c'%s'...%c",C_SBO,pcHlp,C_SBC);
                        }
                     }
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               case CLPTYP_OBJECT:
                  if (psHlp->psDep==NULL) {
                     return CLPERR(psHdl,CLPERR_TAB,"Argument table for object '%s' not defined",psHlp->psStd->pcKyw);
                  }
                  if (psHlp->psFix->siMax==1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp); fprintf(pfOut,"(");
                     if (isSkr && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) return(siErr);
                     fprintf(pfOut,")");
                  } else if (psHlp->psFix->siMax>1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp); fprintf(pfOut,"%c(",C_SBO);
                     if (isSkr && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) return(siErr);
                     fprintf(pfOut,")...%c",C_SBC);
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               case CLPTYP_OVRLAY:
                  if (psHlp->psDep==NULL) {
                     return CLPERR(psHdl,CLPERR_TAB,"Argument table for object '%s' not defined",psHlp->psStd->pcKyw);
                  }
                  if (psHlp->psFix->siMax==1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,".%c",C_CBO);
                     if (isSkr  && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) return(siErr);
                     fprintf(pfOut,"%c",C_CBC);
                  } else if (psHlp->psFix->siMax>1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c%c",C_SBO,C_CBO);
                     if (isSkr && (siLev+1<siDep || siDep>9)) fprintf(pfOut,"\n");
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(pvHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) return(siErr);
                     fprintf(pfOut,"%c...%c",C_CBC,C_SBC);
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               default:
                  return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter '%s' not supported",psHlp->psFix->siTyp,psHlp->psStd->pcKyw);
               }
            }
         }
         if (psArg==NULL) {
            if (psHdl->isOvl) {
               if (siCnt==0) fprintf(pfOut,"%c",C_CBC);
            } else {
               if (siCnt==0) fprintf(pfOut,"%c",')');
            }
         } else {
            if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
               if (siCnt==0) fprintf(pfOut,"%c",C_CBC);
            } else {
               if (siCnt==0) fprintf(pfOut,"%c",')');
            }
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
   }

   if (pfOut!=NULL) {
      if (siLev<siDep || siDep>9) {
         if (isFlg) {
            if (siTyp>=0) {
               fprintf(pfOut,"%s Enter a value (TYPE: %s) or use one of the keywords below:\n",fpcPre(pvHdl,siLev),apClpTyp[siTyp]);
            } else {
               fprintf(pfOut,"%s Enter a value or use one of the keywords below:\n",fpcPre(pvHdl,siLev));
            }
         }
         for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && CLPISF_CMD(psHlp->psStd->uiFlg) && !CLPISF_LNK(psHlp->psStd->uiFlg)) {
               if (!CLPISF_ALI(psHlp->psStd->uiFlg) || (CLPISF_ALI(psHlp->psStd->uiFlg) && isAli)) {
                  vdClpPrnArg(pvHdl,pfOut,siLev,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
                  if (psHlp->psFix->siTyp==CLPTYP_NUMBER && CLPISF_DEF(psHlp->psStd->uiFlg)) {
                     fprintf(pfOut,"%s If you type the keyword without an assignment of a value, the default (%d) is used\n",fpcPre(pvHdl,siLev+1),psHlp->psFix->siOid);
                  }
                  if (psHlp->psDep!=NULL) {
                     if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                        psHdl->apPat[siLev]=psHlp;
                        siErr=siClpPrnHlp(pvHdl,pfOut,isAli,siLev+1,siDep,-1,psHlp->psDep,FALSE);
                        if (siErr<0) return(siErr);
                     } else {
                        psHdl->apPat[siLev]=psHlp;
                        if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
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

   if (pfOut!=NULL) {
      if (isPat) {
         fprintf(pfOut,"%s.",fpcPat(pvHdl,siLev));
      }
      switch (psArg->psFix->siTyp) {
      case CLPTYP_SWITCH:
         vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
         break;
      case CLPTYP_NUMBER:
         if (psArg->psFix->siMax==1) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
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
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%c",C_SBO);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...%c",C_SBC);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%cnum%s",C_SBO,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
                  fprintf(pfOut,"...%c",C_SBC);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%cnum...%c",C_SBO,C_SBC);
               }
            }
         }
         break;
      case CLPTYP_FLOATN:
         if (psArg->psFix->siMax==1) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
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
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%c",C_SBO);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...%c",C_SBC);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%cflt%s",C_SBO,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
                  fprintf(pfOut,"...%c",C_SBC);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%cflt...%c",C_SBO,C_SBC);
               }
            }
         }
         break;
      case CLPTYP_STRING:
         if (CLPISF_BIN(psArg->psStd->uiFlg)) {
            if (CLPISF_HEX(psArg->psStd->uiFlg)) {
               pcHlp="bin-hex";
            } else if (CLPISF_ASC(psArg->psStd->uiFlg)) {
               pcHlp="bin-ascii";
            } else if (CLPISF_EBC(psArg->psStd->uiFlg)) {
               pcHlp="bin-ebcdic";
            } else if (CLPISF_CHR(psArg->psStd->uiFlg)) {
               pcHlp="bin-char";
            } else {
               pcHlp="bin";
            }
         } else pcHlp="str";
         if (psArg->psFix->siMax==1) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,CLP_ASSIGNMENT);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%s'%s'%s",CLP_ASSIGNMENT,pcHlp,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%s'%s'",CLP_ASSIGNMENT,pcHlp);
               }
            }
         } else {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%c",C_SBO);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...%c",C_SBC);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%c'%s'%s",C_SBO,pcHlp,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
                  fprintf(pfOut,"...%c",C_SBC);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);fprintf(pfOut,"%c'%s'...%c",C_SBO,pcHlp,C_SBC);
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
            fprintf(pfOut,"%c(",C_SBO);
            psHdl->apPat[siLev]=psArg;
            siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,")...%c",C_SBC);
            if (siErr<0) return(siErr);
         }
         break;
      case CLPTYP_OVRLAY:
         if (psArg->psFix->siMax==1) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
            fprintf(pfOut,".%c",C_CBO);
            psHdl->apPat[siLev]=psArg;
            siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,"%c",C_CBC);
            if (siErr<0) return(siErr);
         } else {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
            fprintf(pfOut,"%c%c",C_SBO,C_CBO);
            psHdl->apPat[siLev]=psArg;
            siErr=siClpPrnCmd(pvHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,"%c...%c",C_CBC,C_SBC);
            if (siErr<0) return(siErr);
         }
         break;
      }
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

   if (pfDoc!=NULL && psTab!=NULL && pcNum!=NULL) {
      const TsSym*                  psHlp;
      const TsSym*                  psSel;
      int                           siErr,isCon;
      int                           i,k,l,m;
      char                          acNum[strlen(pcNum)+10];
      char                          acArg[20];
      int                           siMan=0;
      int                           siLst=0;
      const TsSym*                  apMan[CLPMAX_HDEPTH];
      const TsSym*                  apLst[CLPMAX_HDEPTH];
      if (psTab->psBak!=NULL) {
         return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
      }
      for (isCon=FALSE,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISF_ARG(psHlp->psStd->uiFlg) && CLPISF_CMD(psHlp->psStd->uiFlg)) {
            if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY || (psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan)) {
               apMan[siMan]=psHlp; siMan++;
            } else {
               for (psSel=psHlp->psDep;psSel!=NULL;psSel=psSel->psNxt) {
                  if (psSel->psFix->pcMan!=NULL && *psSel->psFix->pcMan) {
                     apMan[siMan]=psHlp; siMan++;
                     break;
                  }
               }
               if (psSel==NULL) {
                  apLst[siLst]=psHlp; siLst++;
               }
            }
         } else if (CLPISF_CON(psHlp->psStd->uiFlg) && psArg->psFix->siTyp == psHlp->psFix->siTyp) {
            if (psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan) {
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
                  efprintf(pfDoc," * `%s - %s`\n",apLst[m]->psStd->pcKyw,apLst[m]->psFix->pcHlp);
               }
               fprintf(pfDoc,"\n");
            }
            if (siMan) {
               for (k=m=0;m<siMan;m++) {
                  snprintf(acNum,sizeof(acNum),"%s%d.",pcNum,k+1);
                  if (isNbr) {
                     fprintf(pfDoc,"%s CONSTANT '%s'\n",acNum,apMan[m]->psStd->pcKyw);
                     l=strlen(acNum)+strlen(apMan[m]->psStd->pcKyw)+12;
                     for (i=0;i<l;i++) fprintf(pfDoc,"+");
                     fprintf(pfDoc,"\n\n");
                  } else {
                     fprintf(pfDoc,"CONSTANT '%s'\n",apMan[m]->psStd->pcKyw);
                     l=strlen(apMan[m]->psStd->pcKyw)+11;
                     for (i=0;i<l;i++) fprintf(pfDoc,"+");
                     fprintf(pfDoc,"\n\n");
                  }
                  fprintf(pfDoc, ".SYNOPSIS\n\n");
                  fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                  efprintf(pfDoc,"HELP:   %s\n",apMan[m]->psFix->pcHlp);
                  fprintf(pfDoc, "PATH:   %s\n",fpcPat(pvHdl,siLev));
                  fprintf(pfDoc, "TYPE:   %s\n",apClpTyp[apMan[m]->psFix->siTyp]);
                  fprintf(pfDoc, "SYNTAX: %s\n",apMan[m]->psStd->pcKyw);
                  fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                  fprintf(pfDoc, ".DESCRIPTION\n\n");
                  fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,apMan[m]->psFix->pcMan,2);
                  fprintf(pfDoc,"indexterm:%cConstant %s%c\n\n\n",C_SBO,apMan[m]->psStd->pcKyw,C_SBC);
                  k++;
               }
            }
         }
      } else {
         if (siLst) {
            fprintf(pfDoc,".ARGUMENTS\n\n");
            for (m=0;m<siLst;m++) {
               fprintf(pfDoc," * %c%s: ",C_GRV,apClpTyp[apLst[m]->psFix->siTyp]); siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev,apLst[m]); efprintf(pfDoc," - %s`\n",apLst[m]->psFix->pcHlp);
               for (psSel=apLst[m]->psDep;psSel!=NULL;psSel=psSel->psNxt) {
                  efprintf(pfDoc," ** `%s - %s`\n",psSel->psStd->pcKyw,psSel->psFix->pcHlp);
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
               snprintf(acNum,sizeof(acNum),"%s%d.",pcNum,k+1);
               if (isNbr) {
                  fprintf(pfDoc,"%s %s '%s'\n",acNum,acArg,apMan[m]->psStd->pcKyw);
                  l=strlen(acNum)+strlen(acArg)+strlen(apMan[m]->psStd->pcKyw)+4;
                  for (i=0;i<l;i++) fprintf(pfDoc,"%c",C_CRT);
                  fprintf(pfDoc,"\n\n");
               } else {
                  fprintf(pfDoc,"%s '%s'\n",acArg,apMan[m]->psStd->pcKyw);
                  l=strlen(acArg)+strlen(apMan[m]->psStd->pcKyw)+3;
                  for (i=0;i<l;i++) fprintf(pfDoc,"%c",C_CRT);
                  fprintf(pfDoc,"\n\n");
               }
               fprintf(pfDoc, ".SYNOPSIS\n\n");
               fprintf(pfDoc, "-----------------------------------------------------------------------\n");
               efprintf(pfDoc,"HELP:   %s\n",apMan[m]->psFix->pcHlp);
               fprintf(pfDoc, "PATH:   %s\n",fpcPat(pvHdl,siLev));
               fprintf(pfDoc, "TYPE:   %s\n",apClpTyp[apMan[m]->psFix->siTyp]);
               fprintf(pfDoc, "SYNTAX: ");
               siErr=siClpPrnSyn(pvHdl,pfDoc,FALSE,siLev,apMan[m]);
               fprintf(pfDoc,"\n");
               if (siErr<0) return(siErr);
               fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
               fprintf(pfDoc, ".DESCRIPTION\n\n");
               if (apMan[m]->psFix->pcMan!=NULL && *apMan[m]->psFix->pcMan) {
                  fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,apMan[m]->psFix->pcMan,2);
               } else {
                  fprintf(pfDoc,"No detailed description available for this argument.\n\n");
               }
               fprintf(pfDoc,"indexterm:%cArgument %s%c\n\n\n",C_SBO,apMan[m]->psStd->pcKyw,C_SBC);
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
   const int                     siMtd,
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
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table",fpcPat(pvHdl,siLev),psTab->psStd->pcKyw);
   }

   if (pfOut!=NULL) {
      if (siLev<siDep || siDep>9) {
         for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if (CLPISF_ARG(psHlp->psStd->uiFlg) && CLPISF_PRO(psHlp->psStd->uiFlg) && (pcArg==NULL || strxcmp(psHdl->isCas,psHlp->psStd->pcKyw,pcArg,0,0,FALSE)==0)) {
               if (psHlp->psFix->pcDft!=NULL && *psHlp->psFix->pcDft) {
                  if ((isMan || (!CLPISF_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan) {
                     if (siMtd==CLPPRO_MTD_DOC) {
                        fprintf(pfOut,".DESCRIPTION FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
                        fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHlp->psFix->pcMan,0);
                        fprintf(pfOut," \n");
                     } else {
                        fprintf(pfOut,"\n%c DESCRIPTION for %s.%s.%s.%s:\n",C_HSH,psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw);
                        fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHlp->psFix->pcMan,0);
                        fprintf(pfOut," %c\n",C_HSH);
                     }
                     isMan=TRUE;
                  } else {
                     if (isMan) fprintf(pfOut,"\n");
                     isMan=FALSE;
                  }
                  if (siMtd==CLPPRO_MTD_DOC) {
                     if (!CLPISF_CMD(psHlp->psStd->uiFlg)) {
                        fprintf(pfOut,".HELP FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
                     }
                  } else {
                     fprintf(pfOut," %s.%s.%s.%s=\"%s\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcDft);
                     efprintf(pfOut,"# TYPE: %s HELP: %s #\n",apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
                  }
               } else {
                  if (siMtd==CLPPRO_MTD_ALL || siMtd==CLPPRO_MTD_CMT || siMtd==CLPPRO_MTD_DOC) {
                     if ((isMan || (!CLPISF_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan) {
                        if (siMtd==CLPPRO_MTD_DOC) {
                           fprintf(pfOut,".DESCRIPTION FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
                           fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHlp->psFix->pcMan,0);
                           fprintf(pfOut," \n");
                        } else {
                           fprintf(pfOut,"\n%c DESCRIPTION for %s.%s.%s.%s:\n",C_HSH,psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw);
                           fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHlp->psFix->pcMan,0);
                           fprintf(pfOut," %c\n",C_HSH);
                        }
                        isMan=TRUE;
                     } else {
                        if (isMan) fprintf(pfOut,"\n");
                        isMan=FALSE;
                     }
                     if (siMtd==CLPPRO_MTD_DOC) {
                        if (!CLPISF_CMD(psHlp->psStd->uiFlg)) {
                           fprintf(pfOut,".HELP FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw,apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
                        }
                     } else {
                        if (siMtd==CLPPRO_MTD_CMT) {
                           fprintf(pfOut, ";%s.%s.%s.%s=\"\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw);
                        } else {
                           fprintf(pfOut, " %s.%s.%s.%s=\"\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(pvHdl,siLev),psHlp->psStd->pcKyw);
                        }
                        efprintf(pfOut,"# TYPE: %s HELP: %s #\n",apClpTyp[psHlp->psFix->siTyp],psHlp->psFix->pcHlp);
                     }
                  }
               }

               if (psHlp->psDep!=NULL) {
                  if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnPro(pvHdl,pfOut,isMan,siMtd,siLev+1,siDep,psHlp->psDep,NULL);
                     if (siErr<0) return(siErr);
                  }
               }
            }
         }
      }
   }
   return (CLP_OK);
}

/**********************************************************************/

static const char* fpcPre(
   void*                         pvHdl,
   const int                     siLev)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           i;
   psHdl->pcPre[0]=EOS;
   for (i=0;i<siLev+1;i++) {
      srprintc(&psHdl->pcPre,&psHdl->szPre,strlen(psHdl->pcDep),"%s",psHdl->pcDep);
   }
   return(psHdl->pcPre);
}

static const char* fpcPat(
   void*                         pvHdl,
   const int                     siLev)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   int                           i;
   if (psHdl->pcCmd!=NULL) {
      srprintf(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->pcCmd),"%s",psHdl->pcCmd);
   } else {
      psHdl->pcPat[0]=EOS;
   }
   for (i=0;i<(siLev);i++) {
      srprintc(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->apPat[i]->psStd->pcKyw),".%s",psHdl->apPat[i]->psStd->pcKyw);
   }
   return(psHdl->pcPat);
}

/**********************************************************************/
