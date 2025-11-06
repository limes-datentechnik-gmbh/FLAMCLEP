/**
 * @file   FLAMCLP.c
 * @brief  LIMES Command Line Parser in ANSI-C
 * @author limes datentechnik gmbh
 * @date  06.03.2015
 * @copyright (c) 2015 limes datentechnik gmbh
 * www.flam.de
 *
 * LIMES Command Line Executor (CLE) in ANSI-C
 *
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
#include <limits.h>
#include <math.h>

#ifdef __FL5__
//#  define __HEAP_STATISTIC__
#  include "STDIOL.h"
#  include "STDLIBL.h"
#  include "STRINGL.h"
#  include "CHKMEM.h"
#else
#  undef  pcSysError
#  define pcSysError strerror
#  undef  fclose_unchecked
#  define fclose_unchecked  fclose
#  undef  fflush_unchecked
#  define fflush_unchecked  fflush
#  undef  strtoI32
#  define strtoI32(s,b) ((signed int)strtol((s),NULL,(b)))
#  undef  strtoU32
#  define strtoU32(s,b) ((unsigned int)strtol((s),NULL,(b)))
#  undef  strtoI64
#  define strtoI64(s,b) ((signed long long int)strtol((s),NULL,(b)))
#  undef  strtoU64
#  define strtoU64(s,b) ((unsigned long long int)strtol((s),NULL,(b)))
#  undef  flseed
#  define flseed     ((U32)time(NULL)^(U32)clock())
#  undef  flsrand
#  define flsrand    srand
#  undef  flrand
#  define flrand     rand
#endif
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
 * 1.1.7: Add possibility to use getenv to override hard coded default values
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
 * 1.1.48: Support dia-critical characters in string lexemes (add new macro isStr())
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
 * 1.1.64: Make acPro and acSrc dynamic to reduce memory utilization of symbol table
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
 * 1.2.76: Make lexeme dynamic in length
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
 * 1.2.97: Support NULL pointer for owner, program, command, help and manpage at ClpOpen (path don't start with '.' in such case)
 * 1.2.98: Use threat safe time functions
 * 1.2.99: avoid using set locale for strtod
 * 1.2.100: Support XML path string mapping with '(' and ')' instead of '<' and '>'
 * 1.2.101: Add xxYEAR2 string literal for year without century (YY)
 * 1.2.102: Add missing frees if ClpOpen failed
 * 1.2.103: Add flag hidden (CLPFLG_HID) to support hidden parameter
 * 1.2.104: Separate version and build number with hyphen instead of dot
 * 1.2.105: Support parameter list without '***SECRET***' replacement
 * 1.2.106: Allow help, info, syntax, docu and proterty generation without command as start of the path
 * 1.2.107: Correct '***SECRET***' replacement
 * 1.2.108: Don't support disablement of '***SECRET***' replacement in trace messages
 * 1.2.109: Use new file2str interface
 * 1.2.110: Support callback function for file to string
 * 1.2.111: Support escaping of CLP string (&xxx; or &nnnn<...>)
 * 1.2.112: Fix replacement of environment variables and increase amount from 32 to 256
 * 1.2.113: Support critical character escape sequences for strings, file names and key labels
 * 1.2.114: Fix reallocation with pointer change for lexeme if key word at scanning detected
 * 1.2.115: Support empty strings behind assignments (comment= ...)
 * 1.2.116: Don't parse but accept parameter files if isPfl==2
 * 1.2.117: Required strings are only terminated with separation characters (space or comma), comment or close bracket on level 0
 * 1.2.118: Use main keyword instead of alias in parsed parameter list
 * 1.2.119: Support parameter files for arguments (keyword=>filename)
 * 1.2.120: Support arrays of simple values after assignment (keyword=hugo,berta detlef)
 * 1.2.121: Index for variables in expressions must be enclosed in curly brackets and only a number is useless
 * 1.2.122: Support additional access control check possibility for each write in CLP structure
 * 1.2.123: Increase maximal amount of parameter per object from 256 to 512 (CLPMAX_TABCNT)
 * 1.2.124: Use type of function and not type of pointer to function (usable for pragma's)
 * 1.2.125: Add vdClpReset function to reset after an application handled error
 * 1.3.126: Support better docu generation and headings as single line variants (= Hdl1, ==Hdl2, ...)
 * 1.3.127: Support documentation generation by callback function (for built-in HTMLDOC)
 * 1.3.128: Use trace macro with fflush and time stamp, add symbols find to parse trace
 * 1.3.129: Add symbol able and memory allocation statistics
 * 1.3.130: Support ASCII, EBCDIC and HEX entry also for non binary flagged strings
 * 1.3.131: Envar 'CLP_MALLOC_STATISTICS' and 'CLP_SYMTAB_STATISTICS' requires now "YES" or "ON"
 * 1.3.132: Secure erase memory for dynamic entries in CLP structure if CLPFLG_PWD used
 * 1.3.133: Add new about function with indentation
 * 1.3.134: Add new function psClpFindArgument to find arguments in a table (for envar processing)
 * 1.3.135: Support GMOFFSET/ABS and LCOFFSET/ABS as keyword for difference between local and GM time
 * 1.3.136: Make pvDat and psErr optional available at reset of CLP (and improve reset)
 * 1.4.137: Correct support for command overlays
 * 1.4.138: Hide parameter with hide flag in each case
 * 1.4.139: Fix memory leak in re-allocation of lexem buffer in CLP (runs re-alloc in a loop with doubling the space until a pointer change was happen)
 * 1.4.140: Improve error message if and of list determined based on string type which could be a wrong written keyword
 * 1.4.141: Support hidden flag also for aliases
 * 1.4.142: Rework internal function to typed handle parameter (psHdl instead of pvHdl)
 * 1.5.143: Rework symbol table to allocate only used portions
**/

#define CLP_VSN_STR       "1.5.143"
#define CLP_VSN_MAJOR      1
#define CLP_VSN_MINOR        5
#define CLP_VSN_REVISION       143

/* Definition der Konstanten ******************************************/

#define CLPMAX_TABCNT            512
#define CLPMAX_HDEPTH            128
#define CLPMAX_KYWLEN            63
#define CLPMAX_KYWSIZ            64
#define CLPMAX_BUFCNT            256

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
#define CLPTOK_SAB               16
#define CLPTOK_CBO               17
#define CLPTOK_CBC               18

#define CLP_ASSIGNMENT           "="

#define isPrnInt(p,v) (CLPISF_PWD((p)->psStd->uiFlg) ? ((I64)0) : (v))
#define isPrnFlt(p,v) (CLPISF_PWD((p)->psStd->uiFlg) ? ((F64)0.0) : (v))
#define isPrnStr(p,v) ((CLPISF_PWD((p)->psStd->uiFlg) && psHdl->isPwd) ? ("***SECRET***") : (v))
#define isPrnLen(p,v) (CLPISF_PWD((p)->psStd->uiFlg) ? ((int)0) : (v))

#ifndef realloc_nowarn
#  define realloc_nowarn      realloc
#endif

#define TRACE(f,...) do {\
   if ((f) != NULL) {\
      char acTs[24];\
      fprintf((f), "%s ", cstime(0, acTs));\
      efprintf((f), __VA_ARGS__);\
      fflush_unchecked((f));\
   }\
} while (0)

static inline const char* pcMapClpTok(int siTok) {
   switch (siTok) {
      case CLPTOK_INI: return("INI");
      case CLPTOK_END: return("END");
      case CLPTOK_KYW: return("KEYWORD");
      case CLPTOK_RBO: return("ROUND-BRACKET-OPEN");
      case CLPTOK_RBC: return("ROUND-BRACKET-CLOSE");
      case CLPTOK_SBO: return("SQUARED-BRACKET-OPEN");
      case CLPTOK_SBC: return("SQUARED-BRACKET-CLOSE");
      case CLPTOK_SGN: return("SIGN");
      case CLPTOK_DOT: return("DOT");
      case CLPTOK_ADD: return("ADD");
      case CLPTOK_SUB: return("SUB");
      case CLPTOK_MUL: return("MUL");
      case CLPTOK_DIV: return("DIV");
      case CLPTOK_STR: return("STRING");
      case CLPTOK_NUM: return("NUMBER");
      case CLPTOK_FLT: return("FLOAT");
      case CLPTOK_SAB: return("SIGN-ANGEL-BRACKET");
      case CLPTOK_CBO: return("CURLY-BRACKET-OPEN");
      case CLPTOK_CBC: return("CURLY-BRACKET-CLOSE");
      default:         return("UNKNOWN");
   }
}

static inline const char* pcMapClpTyp(int siTyp) {
   switch (siTyp) {
      case CLPTYP_NON:    return("NO-TYP");
      case CLPTYP_SWITCH: return("SWITCH");
      case CLPTYP_NUMBER: return("NUMBER");
      case CLPTYP_FLOATN: return("FLOAT");
      case CLPTYP_STRING: return("STRING");
      case CLPTYP_OBJECT: return("OBJECT");
      case CLPTYP_OVRLAY: return("OVERLAY");
      case CLPTYP_XALIAS: return("XALIAS");
      default:            return("UNKNOWN");
   }
}

static inline const char* pcClpErr(int siErr) {
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
   case CLPERR_AUT: return("AUTHORIZATION-ERROR");
   default        : return("UNKNOWN-ERROR");
   }
}

/* Definition der Strukturen ******************************************/

typedef struct Std {
   const char*                   pcKyw;
   struct Sym*                   psAli;
   unsigned int                  uiFlg;
   int                           siKwl;
#ifdef __DEBUG__
   int                           siLev;
#endif
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
   const TsClpArgument*          psArg;
   const TsClpArgument*          psTab;
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
   unsigned int                  uiFlg;
} TsPtr;

typedef struct ParamDescriptor {
   const char*                   pcAnchorPrefix;
   const char*                   pcCommand;
   const char*                   pcPath;
   const char*                   pcNum;
} TsParamDescription;
static const TsParamDescription stDefaultParamDesc = { "", "" ,"", "" };

typedef struct Hdl {
   const char*                   pcOwn;
   const char*                   pcPgm;
   const char*                   pcBld;
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
   int                           isFul;
   int                           siMkl;
   int                           isOvl;
   int                           isChk;
   int                           isPwd;
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
   unsigned int                  uiLev;
   I64                           siNow;
   I64                           siRnd;
   int                           siPtr;
   int                           szPtr;
   TsPtr*                        psPtr;
   const TsSym*                  psVal;
   void*                         pvGbl;
   void*                         pvF2s;
   TfF2S*                        pfF2s;
   void*                         pvSaf;
   TfSaf*                        pfSaf;
   long                          siSym;
   void*                         pvPrn;
   TfClpPrintPage*               pfPrn;
   int                           isMan;
   int                           isDep;
   int                           isAnc;
   int                           isNbr;
   int                           isShl;
   int                           isIdt;
   int                           isPat;
   int                           siPs1;
   int                           siPs2;
   int                           siPr3;
} TsHdl;

static inline const char* GETALI(const TsSym* sym) { return (sym->psStd->psAli != NULL) ? sym->psStd->psAli->psStd->pcKyw : NULL; }
static inline const char* GETKYW(const TsSym* sym) { return (sym->psStd->psAli != NULL) ? sym->psStd->psAli->psStd->pcKyw : sym->psStd->pcKyw; }

/* Deklaration der internen Funktionen ********************************/

static TsSym* psClpSymIns(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsClpArgument*          psArg,
   const TsClpArgument*          psTab,
   TsSym*                        psHih,
   TsSym*                        psCur);

static int siClpSymIni(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsClpArgument*          psArg,
   const TsClpArgument*          psTab,
   TsSym*                        psHih,
   TsSym**                       ppFst);

static int siClpSymCal(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsSym*                  psArg,
   TsSym*                        psTab);

static int siClpSymFnd(
   TsHdl*                        psHdl,
   const int                     siLev,
   const char*                   pcKyw,
   const TsSym*                  psTab,
   TsSym**                       ppArg,
   int*                          piElm);

static int siClpSymPrn(
   TsHdl*                        psHdl,
   int                           siLev,
   TsSym*                        psSym);

static int siClpSymTrc(
   TsHdl*                        psHdl);

static void vdClpSymDel(
   TsSym*                        psSym,
   uint64_t*                     piCnt,
   uint64_t*                     piSiz);

static char* pcClpUnEscape(
   TsHdl*                        psHdl,
   const char*                   pcInp);

static int siClpScnNat(
   TsHdl*                        psHdl,
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
   TsHdl*                        psHdl,
   int                           siTyp,
   const TsSym*                  psArg);

static int siClpConNat(
   TsHdl*                        psHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   size_t*                       pzLex,
   char**                        ppLex,
   const int                     siTyp,
   const TsSym*                  psArg);

static int siClpConSrc(
   TsHdl*                        psHdl,
   const int                     isChk,
   const TsSym*                  psArg);

static int siClpPrsMain(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   const int                     isOvl,
   int*                          piOid);

static int siClpPrsParLst(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsSym*                  psTab);

static int siClpPrsPar(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const TsSym*                  psTab,
   const int                     isOvl,
   int*                          piOid);

static int siClpPrsNum(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsSwt(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsSgn(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsFil(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg);

static int siClpAcpFil(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg);

static int siClpPrsObjWob(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsObj(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsOvl(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsAry(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpPrsValLstFlexible(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siTok,
   TsSym*                        psArg);

static int siClpPrsValLstOnlyArg(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     isEnd,
   const int                     siTok,
   TsSym*                        psArg);

static int siClpPrsObjLst(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg);

static int siClpPrsOvlLst(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg);

static int siClpPrsVal(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg);

static int siClpPrsProLst(
   TsHdl*                        psHdl,
   const TsSym*                  psTab);

static int siClpPrsPro(
   TsHdl*                        psHdl,
   const TsSym*                  psTab);

static int siClpPrsKywLst(
   TsHdl*                        psHdl,
   size_t*                       pzPat,
   char**                        ppPat);

static int siClpBldPro(
   TsHdl*                        psHdl,
   const char*                   pcPat,
   const char*                   pcPro,
   const int                     siRow);

static int siClpBldLnk(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const I64                     siVal,
   TsSym*                        psArg,
   const int                     isApp);

static int siClpBldSwt(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpBldNum(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static int siClpBldLit(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal);

static int siClpIniMainObj(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   TsVar                         asSav[]);

static int siClpFinMainObj(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   const TsVar                   asSav[]);

static int siClpIniMainOvl(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   TsVar                         asSav[]);

static int siClpFinMainOvl(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   const TsVar                   asSav[],
   const int                     siOid);

static int siClpIniObj(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar                         asSav[]);

static int siClpFinObj(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar                   asSav[]);

static int siClpIniOvl(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar                         asSav[]);

static int siClpFinOvl(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar                   asSav[],
   const int                     siOid);

static int siClpSetDefault(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg);

static void vdClpPrnArg(
   TsHdl*                        psHdl,
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
   TsHdl*                        psHdl,
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
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     isPat,
   const int                     siLev,
   const TsSym*                  psArg);

static int siClpPrnCmd(
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     siCnt,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psArg,
   const TsSym*                  psTab,
   const int                     isSkr,
   const int                     isMin);

static int siClpPrnHlp(
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     isAli,
   const int                     siLev,
   const int                     siDep,
   const int                     siTyp,
   const TsSym*                  psTab,
   const int                     isFlg);

static int siClpPrnDoc(
   TsHdl*                        psHdl,
   FILE*                         pfDoc,
   const int                     siLev,
   const TsParamDescription*     psParamDesc,
   const TsSym*                  psArg,
   const TsSym*                  psTab);

static int siClpPrnPro(
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   int                           isMan,
   const int                     siMtd,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psTab,
   const char*                   pcArg);

static int siFromNumberLexeme(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg,
   const char*                   pcVal,
   I64*                          piVal);

static int siFromFloatLexeme(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg,
   const char*                   pcVal,
   double*                       pfVal);

static const char* fpcPre(
   TsHdl*                        psHdl,
   const int                     siLev);

static const char* fpcPat(
   TsHdl*                        psHdl,
   const int                     siLev);

static inline int CLPERR(TsHdl* psHdl,int siErr, char* pcMsg, ...) {
   va_list              argv;
   int                  r;
   const char*          pcErr=pcClpErr(siErr);
   char                 acMsg[1024]="";
   va_start(argv,pcMsg);
   r=vsnprintf(acMsg,sizeof(acMsg),pcMsg,argv);
   if (r>=sizeof(acMsg)) {
      acMsg[sizeof(acMsg)-1]=0x00;
   }
   va_end(argv);
   srprintf(&psHdl->pcMsg,&psHdl->szMsg,strlen(pcErr)+strlen(acMsg),"%s: %s",pcErr,acMsg);
   psHdl->siErr=siErr;
   if (psHdl->pcRow!=NULL && psHdl->pcOld>=psHdl->pcRow) {
      psHdl->siCol=(int)((psHdl->pcOld-psHdl->pcRow)+1);
   } else psHdl->siCol=0;
   if (psHdl->pfErr!=NULL) {
      fprintf(psHdl->pfErr,"%s:\n%s %s\n",pcErr,fpcPre(psHdl,0),acMsg);
      if (psHdl->pcSrc!=NULL && psHdl->pcInp!=NULL && psHdl->pcOld!=NULL && psHdl->pcCur!=NULL && (psHdl->pcCur>psHdl->pcInp || psHdl->pcLst!=NULL || psHdl->siRow)) {
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
         } else if (strncmp(psHdl->pcSrc,CLPSRC_ISR,strlen(CLPSRC_ISR))==0) {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d from internal string\n",               fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol);
         } else {
            fprintf(psHdl->pfErr,"%s Cause: Row=%d Column=%d in file '%s'\n",                       fpcPre(psHdl,1),psHdl->siRow,psHdl->siCol,psHdl->pcSrc);
         }
         if (psHdl->pcRow!=NULL) {
            int f=FALSE;
            fprintf(psHdl->pfErr,"%s \"",fpcPre(psHdl,1));
            for (const char* p=psHdl->pcRow;!iscntrl(*p);p++) { fprintf(psHdl->pfErr,"%c",*p); }
            fprintf(psHdl->pfErr,"\"\n");
            if (psHdl->pcCur==psHdl->pcRow) {
               fprintf(psHdl->pfErr,"%s %c",fpcPre(psHdl,1),C_CRT);
            } else {
               fprintf(psHdl->pfErr,"%s  ",fpcPre(psHdl,1));
            }
            for (const char* p=psHdl->pcRow;!iscntrl(*p);p++) {
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
         int l=(psHdl->pcLst!=NULL)?strlen(psHdl->pcLst):0;
         if (l>1) {
            l--;
            fprintf(psHdl->pfErr,"%s After successful parsing of arguments below:\n",fpcPre(psHdl,0));
            fprintf(psHdl->pfErr,"%s ",fpcPre(psHdl ,1));
            for (int i=0;i<l;i++) {
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
   const unsigned char* p=(const unsigned char*)&siRnd;
   U64            h=0xcbf29ce48422232LLU;
   h^=p[0]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[1]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[2]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[3]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[4]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[5]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[6]; h*=0x100000001b3LLU;
   // cppcheck-suppress objectIndex
   h^=p[7]; h*=0x100000001b3LLU;
   return h;
}

static inline void* pvClpAllocNew(
   TsHdl*                        psHdl,
   const int                     siSiz,
   int*                          piInd,
   const unsigned int            uiFlg)
{
   if (siSiz>0) {
      if (psHdl->siPtr>=psHdl->szPtr) {
         void* pvHlp=realloc_nowarn(psHdl->psPtr,sizeof(TsPtr)*(psHdl->szPtr+CLPINI_PTRCNT));
         if (pvHlp==NULL) { return(NULL); }
         psHdl->psPtr=pvHlp;
         psHdl->szPtr+=CLPINI_PTRCNT;
      }
      void* pvPtr=calloc(1,siSiz);
      if (pvPtr==NULL) { return(NULL); }
      psHdl->psPtr[psHdl->siPtr].pvPtr=pvPtr;
      psHdl->psPtr[psHdl->siPtr].siSiz=siSiz;
      psHdl->psPtr[psHdl->siPtr].uiFlg=uiFlg;
      if (piInd!=NULL) { *piInd=psHdl->siPtr; }
      psHdl->siPtr++;
      return(pvPtr);
   }
   return(NULL);
}

static inline void* pvClpAllocAgain(
   TsPtr*                        psPtr,
   const int                     siSiz,
   const unsigned int            uiFlg)
{
   if (siSiz<=0) { // free
      if (psPtr->pvPtr!=NULL) {
         if (psPtr->siSiz) {
            if (CLPISF_PWD(psPtr->uiFlg)) {
               secure_memset(psPtr->pvPtr,psPtr->siSiz);
            }
         }
         free(psPtr->pvPtr);
         psPtr->pvPtr =NULL;
      }
      psPtr->siSiz =0;
      psPtr->uiFlg|=uiFlg;
      return(NULL);
   } else {
      if (psPtr->siSiz>siSiz) { // smaller
         if (CLPISF_PWD(psPtr->uiFlg)) {
            secure_memset(((char*)psPtr->pvPtr)+siSiz,psPtr->siSiz-siSiz);
         }
      }
      void* pvPtr=realloc_nowarn(psPtr->pvPtr,siSiz);
      if (pvPtr==NULL) { return(NULL); }
      if (psPtr->siSiz<siSiz) { // larger
         memset(((char*)pvPtr)+psPtr->siSiz,0,siSiz-psPtr->siSiz);
      }
      psPtr->pvPtr =pvPtr;
      psPtr->siSiz =siSiz;
      psPtr->uiFlg|=uiFlg;
      return(pvPtr);
   }
}

extern void* pvClpAllocFlg(
   void*                         pvHdl,
   void*                         pvPtr,
   const int                     siSiz,
   int*                          piInd,
   const unsigned int            uiFlg)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   if (pvPtr==NULL) {
      return(pvClpAllocNew(psHdl,siSiz,piInd,uiFlg));
   } else {
      if (piInd!=NULL && *piInd>=0 && *piInd<psHdl->siPtr && psHdl->psPtr[*piInd].pvPtr==pvPtr) {
         return(pvClpAllocAgain(psHdl->psPtr+(*piInd),siSiz,uiFlg));
      } else {
         for (int i=0;i<psHdl->siPtr;i++) {
            if (psHdl->psPtr[i].pvPtr==pvPtr) {
               pvPtr=pvClpAllocAgain(psHdl->psPtr+i,siSiz,uiFlg);
               if (piInd!=NULL) { *piInd=i; }
               return(pvPtr);
            }
         }
         return(pvClpAllocNew(psHdl,siSiz,piInd,uiFlg));
      }
   }
}

static void vdClpFree(
   TsHdl*                        psHdl)
{
   if (psHdl->psPtr!=NULL) {
      uint64_t uiCnt=0;
      uint64_t uiSiz=0;
      for (int i=0;i<psHdl->siPtr;i++) {
         if (psHdl->psPtr[i].pvPtr!=NULL) {
            uiCnt++;
            if (psHdl->psPtr[i].siSiz) {
               uiSiz+=psHdl->psPtr[i].siSiz;
               if (CLPISF_PWD(psHdl->psPtr[i].uiFlg)) {
                  secure_memset(psHdl->psPtr[i].pvPtr,psHdl->psPtr[i].siSiz);
               }
            }
            free(psHdl->psPtr[i].pvPtr);
            psHdl->psPtr[i].pvPtr=NULL;
            psHdl->psPtr[i].siSiz=0;
            psHdl->psPtr[i].uiFlg=0;
         }
      }
      if (CHECK_ENVAR_ON("CLP_MALLOC_STATISTICS")) {
         char acTs[24];
         fprintf(stderr,"%s CLP_MALLOC_STATISTICS(Amount(%"PRIu64"),Table(%d(%"PRIu64")),Size(%"PRIu64"))\n",cstime(0,acTs),uiCnt,psHdl->szPtr,((uint64_t)sizeof(TsPtr))*((uint64_t)psHdl->szPtr),uiSiz);
      }
   }
}

/* Implementierung der externen Funktionen ****************************/

extern const char* pcClpVersion2(const int l, const int s, char* b, const char* is)
{
   if (is!=NULL) {
      int  i;
      int  k=strlen(is);
      char acIndent[(l)*k+1];
      for (acIndent[0]=0x00,i=0;i<(l);i++) {
         snprintc(acIndent,sizeof(acIndent),"%s",is);
      }
      snprintc(b,s,"%s FLAM-CLP VERSION: %s-%d BUILD: %s %s %s\n",acIndent,CLP_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
   } else {
      snprintc(b,s,"%2.2d FLAM-CLP VERSION: %s-%d BUILD: %s %s %s\n",l,CLP_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
   }
   return(b);
}

extern const char* pcClpVersion(const int l, const int s, char* b)
{
   return(pcClpVersion2(l,s,b,NULL));
}

extern const char* pcClpAbout2(const int l, const int s, char* b, const char* is)
{
   if (is!=NULL) {
      int  i;
      int  k=strlen(is);
      char acIndent[(l)*k+1];
      for (acIndent[0]=0x00,i=0;i<(l);i++) {
         snprintc(acIndent,sizeof(acIndent),"%s",is);
      }
      snprintc(b,s,"%s Frankenstein Limes Command Line Parser (FLAM-CLP)\n",acIndent);
      snprintc(b,s,"%s   Version: %s-%d Build: %s %s %s\n",acIndent,CLP_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
      snprintc(b,s,"%s   Copyright (C) limes datentechnik (R) gmbh\n",acIndent);
      snprintc(b,s,"%s   This library is open source from the FLAM(R) project: https://flam.de\n",acIndent);
      snprintc(b,s,"%s   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n",acIndent);
   } else {
      snprintc(b,s,
            "%2.2d Frankenstein Limes Command Line Parser (FLAM-CLP)\n"
            "   Version: %s-%u Build: %s %s %s\n"
            "   Copyright (C) limes datentechnik (R) gmbh\n"
            "   This library is open source from the FLAM(R) project: https://flam.de\n"
            "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
            ,l,CLP_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
   }
   return(b);
}
extern const char* pcClpAbout(const int l, const int s, char* b)
{
   return(pcClpAbout2(l,s,b,NULL));
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

static int siOwnFile2String(void* gbl, const void* hdl, const char* filename, char** buf, int* bufsize, char* errmsg, const int msgsiz) {
   (void)gbl;
   char* pcFil=dcpmapfil(filename);
   if (pcFil==NULL) { return(-1); }
   int siErr=file2str(hdl, pcFil, buf, bufsize, errmsg, msgsiz);
   free(pcFil);
   return(siErr);
}

static int siExtentSymTab(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsSym*                  psArg)
{
   psHdl->apPat[siLev]=psArg;
   if (psArg->psDep==NULL && psArg->psTab!=NULL) {
      int siErr;
      siErr=siClpSymIni(psHdl,siLev+1,psArg->psArg,psArg->psTab,(TsSym*)psArg,NULL);
      if (siErr<0) { return(siErr); }
      siErr=siClpSymCal(psHdl,siLev+1,psArg,psArg->psDep);
      if (siErr<0) { return(siErr); }
   }
   return(CLP_OK);
}

extern void* pvClpOpen(
   const int                     isCas,
   const int                     isPfl,
   const int                     isEnv,
   const int                     siMkl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
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
   TsClpError*                   psErr,
   void*                         pvGbl,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf)
{
   TsHdl*                        psHdl=NULL;
   const char*                   pcNow=NULL;
   I64                           siNow=0;
   int                           siErr,i;
   if (psTab!=NULL) {
      flsrand(flseed);
      psHdl=(TsHdl*)calloc(1,sizeof(TsHdl));
      if (psHdl!=NULL) {
         if (CHECK_ENVAR_ON("CLP_FULL_SYMTAB")) {
            psHdl->isFul=TRUE;
         }
         psHdl->isCas=isCas;
         psHdl->isPfl=isPfl;
         psHdl->isEnv=isEnv;
         psHdl->siMkl=siMkl;
         psHdl->pcOwn=(pcOwn!=NULL)?pcOwn:"";
         psHdl->pcPgm=(pcPgm!=NULL)?pcPgm:"";
         psHdl->pcBld=(pcBld!=NULL)?pcBld:"";
         psHdl->pcCmd=(pcCmd!=NULL)?pcCmd:"";
         psHdl->pcMan=(pcMan!=NULL)?pcMan:"";
         psHdl->pcHlp=(pcHlp!=NULL)?pcHlp:"";
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
         if (psHdl->pcLex==NULL || psHdl->pcSrc==NULL || psHdl->pcPre==NULL || psHdl->pcPat==NULL || psHdl->pcLst==NULL || psHdl->pcMsg==NULL)  {
            SAFE_FREE(psHdl->pcLex);
            SAFE_FREE(psHdl->pcSrc);
            SAFE_FREE(psHdl->pcPre);
            SAFE_FREE(psHdl->pcPat);
            SAFE_FREE(psHdl->pcLst);
            SAFE_FREE(psHdl->pcMsg);
            free(psHdl);
            return(NULL);
         }
         for (i=0;i<CLPMAX_BUFCNT;i++) {
            psHdl->pzBuf[i]=0;
            psHdl->apBuf[i]=NULL;
         }
         psHdl->pvDat=pvDat;
         psHdl->psTab=NULL;
         psHdl->psSym=NULL;
         psHdl->psOld=NULL;
         psHdl->isChk=FALSE;
         if (pfHlp==NULL) { psHdl->pfHlp=stderr; } else { psHdl->pfHlp=pfHlp; }
         psHdl->pfErr=pfErr;
         psHdl->pfSym=pfSym;
         psHdl->pfScn=pfScn;
         psHdl->pfPrs=pfPrs;
         psHdl->pfBld=pfBld;
         psHdl->pcDep=pcDep;
         psHdl->pcOpt=pcOpt;
         psHdl->pcEnt=pcEnt;
         psHdl->pvGbl=pvGbl;
         if (pfF2S!=NULL) {
            psHdl->pfF2s=pfF2S;
            psHdl->pvF2s=pvF2S;
         } else {
            psHdl->pfF2s=siOwnFile2String;
            psHdl->pvF2s=NULL;
         }
         psHdl->pvSaf=pvSaf;
         psHdl->pfSaf=pfSaf;

#if defined(__DEBUG__) && defined(__HEAP_STATISTIC__)
         long siBeginCurHeapSize=CUR_HEAP_SIZE();
#endif
         siErr=siClpSymIni(psHdl,0,NULL,psTab,NULL,&psHdl->psTab);
         if (siErr<0) {
            uint64_t uiCnt=0;
            uint64_t uiSiz=0;
            vdClpSymDel(psHdl->psTab,&uiCnt,&uiSiz);
            if (CHECK_ENVAR_ON("CLP_SYMTAB_STATISTICS")) {
               char acTs[24];
               fprintf(stderr,"%s CLP_SYMTAB_STATISTICS(Amount(%"PRIu64"),Size(%"PRIu64")) after fail of in siClpSymIni()\n",cstime(0,acTs),uiCnt,uiSiz);
            }
            SAFE_FREE(psHdl->pcLex);
            SAFE_FREE(psHdl->pcSrc);
            SAFE_FREE(psHdl->pcPre);
            SAFE_FREE(psHdl->pcPat);
            SAFE_FREE(psHdl->pcLst);
            SAFE_FREE(psHdl->pcMsg);
            free(psHdl);
            return(NULL);
         }
#if defined(__DEBUG__) && defined(__HEAP_STATISTIC__)
         long siEndCurHeapSize=CUR_HEAP_SIZE();
         printd("---------- CLP-SYMTAB-CUR_HEAP_SIZE(%ld)=>%ld(%ld) Count==%ld(%ld)\n",siBeginCurHeapSize,siEndCurHeapSize,siEndCurHeapSize-siBeginCurHeapSize,psHdl->siSym,(siEndCurHeapSize-siBeginCurHeapSize)/psHdl->siSym);
#endif
         siErr=siClpSymCal(psHdl,0,NULL,psHdl->psTab);
         if (siErr<0) {
            uint64_t uiCnt=0;
            uint64_t uiSiz=0;
            vdClpSymDel(psHdl->psTab,&uiCnt,&uiSiz);
            if (CHECK_ENVAR_ON("CLP_SYMTAB_STATISTICS")) {
               char acTs[24];
               fprintf(stderr,"%s CLP_SYMTAB_STATISTICS(Amount(%"PRIu64"),Size(%"PRIu64")) after fail of in siClpSymCal()\n",cstime(0,acTs),uiCnt,uiSiz);
            }
            SAFE_FREE(psHdl->pcLex);
            SAFE_FREE(psHdl->pcSrc);
            SAFE_FREE(psHdl->pcPre);
            SAFE_FREE(psHdl->pcPat);
            SAFE_FREE(psHdl->pcLst);
            SAFE_FREE(psHdl->pcMsg);
            free(psHdl);
            return(NULL);
         }
         siErr=siClpSymTrc(psHdl);
         if (siErr<0) {
            uint64_t uiCnt=0;
            uint64_t uiSiz=0;
            vdClpSymDel(psHdl->psTab,&uiCnt,&uiSiz);
            if (CHECK_ENVAR_ON("CLP_SYMTAB_STATISTICS")) {
               char acTs[24];
               fprintf(stderr,"%s CLP_SYMTAB_STATISTICS(Amount(%"PRIu64"),Size(%"PRIu64")) after fail of in siClpSymCal()\n",cstime(0,acTs),uiCnt,uiSiz);
            }
            SAFE_FREE(psHdl->pcLex);
            SAFE_FREE(psHdl->pcSrc);
            SAFE_FREE(psHdl->pcPre);
            SAFE_FREE(psHdl->pcPat);
            SAFE_FREE(psHdl->pcLst);
            SAFE_FREE(psHdl->pcMsg);
            free(psHdl);
            return(NULL);
         }
         if (psErr!=NULL) {
            psErr->ppMsg=(const char**)&psHdl->pcMsg;
            psErr->ppSrc=(const char**)&psHdl->pcSrc;
            psErr->piRow=&psHdl->siRow;
            psErr->piCol=&psHdl->siCol;
         }
         psHdl->siNow=time(NULL);
         srand(psHdl->siNow+clock());
         psHdl->siRnd=ClpRndFnv(flrand()^clock());
         pcNow=GETENV("CLP_NOW");
         if (pcNow!=NULL && *pcNow) {
            siErr=siClpScnNat(psHdl,psHdl->pfErr,psHdl->pfScn,&pcNow,&psHdl->szLex,&psHdl->pcLex,CLPTYP_NUMBER,NULL,NULL,NULL);
            if (siErr==CLPTOK_NUM) {
               siErr=siFromNumberLexeme(psHdl,0,NULL,psHdl->pcLex,&siNow);
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
         if (pfErr!=NULL) { fprintf(pfErr,"Allocation of CLP structure failed\n"); }
      }
   } else {
      if (pfErr!=NULL) { fprintf(pfErr,"Parameter psTab is NULL\n"); }
   }
   return((void*)psHdl);
}

extern void vdClpReset(
   void*                         pvHdl,
   void*                         pvDat,
   TsClpError*                   psErr)
{
   if (pvHdl!=NULL) {
      TsHdl* psHdl=(TsHdl*)pvHdl;
      psHdl->siTok=CLPTOK_INI;
      psHdl->siRow=1;
      psHdl->siCol=0;
      psHdl->siErr=0;
      psHdl->pcInp=NULL;
      psHdl->pcCur=NULL;
      psHdl->pcOld=NULL;
      psHdl->pcLex[0]=EOS;
      psHdl->isChk=FALSE;
      if (pvDat!=NULL) {
         psHdl->pvDat=pvDat;
      }
      if (psErr!=NULL) {
         psErr->ppMsg=(const char**)&psHdl->pcMsg;
         psErr->ppSrc=(const char**)&psHdl->pcSrc;
         psErr->piRow=&psHdl->siRow;
         psErr->piCol=&psHdl->siCol;
      }
   }
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

   if (pcPro==NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Property string is NULL");
   }

   if (psHdl->pcLst!=NULL) {
      psHdl->pcLst[0]=0x00;
   }

   if (pcSrc!=NULL && *pcSrc) {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_PRF)+strlen(pcSrc),"%s%s",CLPSRC_PRF,pcSrc);
   } else {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_PRO),"%s",CLPSRC_PRO);
   }
   psHdl->pcInp=pcClpUnEscape(psHdl,pcPro);
   if (psHdl->pcInp==NULL) {
      return CLPERR(psHdl,CLPERR_MEM,"Un-escaping of property string failed");
   }
   psHdl->pcCur=psHdl->pcInp;
   psHdl->pcOld=psHdl->pcInp;
   psHdl->pcRow=psHdl->pcInp;
   psHdl->isChk=isChk;
   psHdl->siRow=1;
   psHdl->siCol=0;
   psHdl->pcLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
#if defined(__DEBUG__) && defined(__HEAP_STATISTIC__)
      long siBeginCurHeapSize=CUR_HEAP_SIZE();
#endif
      TRACE(psHdl->pfPrs,"PROPERTY-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      siCnt=siClpPrsProLst(psHdl,psHdl->psTab);
      if (siCnt<0) { return(siCnt); }
#if defined(__DEBUG__) && defined(__HEAP_STATISTIC__)
      long siEndCurHeapSize=CUR_HEAP_SIZE();
      printd("---------- CLP-PRSPRO-CUR_HEAP_SIZE(%ld)=>%ld(%ld)\n",siBeginCurHeapSize,siEndCurHeapSize,siEndCurHeapSize-siBeginCurHeapSize);
#endif
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcInp=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->pcLex[0]=EOS;
         psHdl->isChk=FALSE;
         TRACE(psHdl->pfPrs,"PROPERTY-PARSER-END(CNT=%d)\n",siCnt);
         if (ppLst!=NULL) { *ppLst=psHdl->pcLst; }
         return(siCnt);
      } else {
         if (ppLst!=NULL) { *ppLst=psHdl->pcLst; }
         return CLPERR(psHdl,CLPERR_SYN,"Last token (%s(%s)) of property list is not EOS",pcMapClpTok(psHdl->siTok),psHdl->pcLex);
      }
   } else {
      if (ppLst!=NULL) { *ppLst=psHdl->pcLst; }
      return CLPERR(psHdl,CLPERR_SYN,"Initial token (%s(%s)) in handle is not valid",pcMapClpTok(psHdl->siTok),psHdl->pcLex);
   }
}

static int siClpParseCmd2(
   TsHdl*                        psHdl,
   const char*                   pcSrc,
   const char*                   pcCmd,
   const int                     isChk,
   const int                     isPwd,
   const int                     isOvl,
   int*                          piOid,
   char**                        ppLst)
{
   int                           siCnt;

   if (pcCmd==NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Command string is NULL");
   }

   if (psHdl->pcLst!=NULL) {
      psHdl->pcLst[0]=0x00;
   }

   if (pcSrc!=NULL && *pcSrc) {
      if (*pcSrc!=':') { // file
         srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_CMF)+strlen(pcSrc),"%s%s",CLPSRC_CMF,pcSrc);
      } else { // other source
         srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(pcSrc),"%s",pcSrc);
      }
   } else {
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_CMD),"%s",CLPSRC_CMD);
   }
   psHdl->pcInp=pcClpUnEscape(psHdl,pcCmd);
   if (psHdl->pcInp==NULL) {
      return CLPERR(psHdl,CLPERR_MEM,"Un-escaping of command string failed");
   }
   psHdl->pcCur=psHdl->pcInp;
   psHdl->pcOld=psHdl->pcInp;
   psHdl->pcRow=psHdl->pcInp;
   psHdl->isChk=isChk;
   psHdl->isPwd=isPwd;
   psHdl->siRow=1;
   psHdl->siCol=0;
   psHdl->pcLex[0]=EOS;
   if (psHdl->siTok==CLPTOK_INI) {
#if defined(__DEBUG__) && defined(__HEAP_STATISTIC__)
      long siBeginCurHeapSize=CUR_HEAP_SIZE();
#endif
      TRACE(psHdl->pfPrs,"COMMAND-PARSER-BEGIN\n");
      psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      siCnt=siClpPrsMain(psHdl,psHdl->psTab,isOvl,piOid);
      if (siCnt<0) { return (siCnt); }
#if defined(__DEBUG__) && defined(__HEAP_STATISTIC__)
      long siEndCurHeapSize=CUR_HEAP_SIZE();
      printd("---------- CLP-PRSCMD-CUR_HEAP_SIZE(%ld)=>%ld(%ld)\n",siBeginCurHeapSize,siEndCurHeapSize,siEndCurHeapSize-siBeginCurHeapSize);
#endif
      if (psHdl->siTok==CLPTOK_END) {
         psHdl->siTok=CLPTOK_INI;
         psHdl->pcInp=NULL;
         psHdl->pcCur=NULL;
         psHdl->pcOld=NULL;
         psHdl->pcLex[0]=EOS;
         psHdl->isChk=FALSE;
         TRACE(psHdl->pfPrs,"COMMAND-PARSER-END(CNT=%d)\n",siCnt);
         if (ppLst!=NULL) { *ppLst=psHdl->pcLst; }
         return(siCnt);
      } else {
         if (ppLst!=NULL) { *ppLst=psHdl->pcLst; }
         return CLPERR(psHdl,CLPERR_SYN,"Last token (%s(%s)) of parameter list is not EOS",pcMapClpTok(psHdl->siTok),psHdl->pcLex);
      }
   } else {
      if (ppLst!=NULL) { *ppLst=psHdl->pcLst; }
      return CLPERR(psHdl,CLPERR_SYN,"Initial token (%s(%s)) in handle is not valid",pcMapClpTok(psHdl->siTok),psHdl->pcLex);
   }
}


extern int siClpParseCmd(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcCmd,
   const int                     isChk,
   const int                     isPwd,
   int*                          piOid,
   char**                        ppLst)
{
   return(siClpParseCmd2((TsHdl*)pvHdl,pcSrc,pcCmd,isChk,isPwd,FALSE,piOid,ppLst));
}

extern int siClpParseOvl(
   void*                         pvHdl,
   const char*                   pcCmd)
{
   int siOid=0;
   if (pvHdl!=NULL && pcCmd!=NULL) {
      TsHdl* psHdl=(TsHdl*)pvHdl;
      if (psHdl->isOvl) {
         siClpParseCmd2(psHdl,"",pcCmd,FALSE,FALSE,TRUE,&siOid,NULL);
         vdClpReset(psHdl,NULL,NULL);
      }
   }
   return(siOid);
}

extern int siClpSyntax(
   void*                         pvHdl,
   const int                     isSkr,
   const int                     isMin,
   const int                     siDep,
   const char*                   pcPat)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   int                           siErr;
   unsigned int                  l=strlen(psHdl->pcCmd);

   if (psHdl->pcLst!=NULL) { psHdl->pcLst[0]=0x00; }

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
      if (l==0 || strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         int siLev;
         if (l && strlen(pcPat)>l && pcPat[l]!='.') {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
         }
         for (siLev=0,pcPtr=l?strchr(pcPat,'.'):pcPat-1;pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
            int i;
            char acKyw[CLPMAX_KYWSIZ];
            for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) { acKyw[i]=pcKyw[i]; }
            acKyw[i]=EOS;
            siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,NULL);
            if (siErr<0) { return(siErr); }
            siErr=siExtentSymTab(psHdl,siLev,psArg);
            if (siErr<0) { return(siErr); }
            psTab=psArg->psDep;
         }
         if (psTab!=NULL && !CLPISF_CON(psTab->psStd->uiFlg)) {
            siErr=siClpPrnCmd(psHdl,psHdl->pfHlp,0,siLev,siLev+siDep,psArg,psTab,isSkr,isMin);
            if (siErr<0) { return (siErr); }
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains too many or invalid qualifiers",pcPat);
         }
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
      }
   } else {
      siErr=siClpPrnCmd(psHdl,psHdl->pfHlp,0,0,siDep,NULL,psHdl->psTab,isSkr,isMin);
      if (siErr<0) { return (siErr); }
   }
   if (isSkr && psHdl->pfHlp!=NULL) { fprintf(psHdl->pfHlp,"\n"); }
   return(CLP_OK);
}


extern const char* pcClpInfo(
   void*                         pvHdl,
   const char*                   pcPat)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;
   const TsSym*                  psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   unsigned int                  l=strlen(psHdl->pcCmd);

   if (psHdl->pcLst!=NULL) { psHdl->pcLst[0]=0x00; }

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
      if (l==0 || strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
         if (strlen(pcPat)<=l || pcPat[l]=='.') {
            int siLev;
            for (siLev=0,pcPtr=l?strchr(pcPat,'.'):pcPat-1;pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
               int i,siErr;
               char acKyw[CLPMAX_KYWSIZ];
               for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) { acKyw[i]=pcKyw[i]; }
               acKyw[i]=EOS;
               siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,NULL);
               if (siErr<0) { return(""); }
               siErr=siExtentSymTab(psHdl,siLev,psArg);
               if (siErr<0) { return(""); }
               psTab=psArg->psDep;
            }
            if (psArg!=NULL) { return(psArg->psFix->pcHlp); }
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
   const TsSym*                  psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   unsigned int                  l=strlen(psHdl->pcCmd);

   if (psHdl->pcLst!=NULL) { psHdl->pcLst[0]=0x00; }

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
         if (l==0 || strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
            int siLev;
            if (l && strlen(pcPat)>l && pcPat[l]!='.') {
               return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
            }
            for (siLev=0,pcPtr=l?strchr(pcPat,'.'):pcPat-1;pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
               int i,siErr;
               char acKyw[CLPMAX_KYWSIZ];
               for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) { acKyw[i]=pcKyw[i]; }
               acKyw[i]=EOS;
               siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,NULL);
               if (siErr<0) { return(siErr); }
               siErr=siExtentSymTab(psHdl,siLev,psArg);
               if (siErr<0) { return(siErr); }
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
                     fprintf(psHdl->pfHlp,   "SYNTAX: > %s ",psHdl->pcPgm);
                     int siErr=siClpPrnCmd(psHdl,psHdl->pfHlp,0,0,1,NULL,psHdl->psTab,FALSE,FALSE);
                     if (siErr<0) { return(siErr); }
                     fprintf(psHdl->pfHlp,"\n\n");
                     fprintf(psHdl->pfHlp,"DESCRIPTION\n");
                     fprintf(psHdl->pfHlp,"-----------\n");
                     if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
                        fprintm(psHdl->pfHlp,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHdl->pcMan,1);
                     } else {
                        fprintf(psHdl->pfHlp,"No detailed description available for this command.\n\n");
                     }
                  } else {
                     fprintf(psHdl->pfHlp, "SYNOPSIS\n");
                     fprintf(psHdl->pfHlp, "--------\n");
                     efprintf(psHdl->pfHlp,"HELP:   %s\n",psArg->psFix->pcHlp);
                     fprintf(psHdl->pfHlp, "PATH:   %s\n",fpcPat(psHdl,siLev-1));
                     fprintf(psHdl->pfHlp, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
                     fprintf(psHdl->pfHlp, "SYNTAX: ");
                     int siErr=siClpPrnSyn(psHdl,psHdl->pfHlp,FALSE,siLev-1,psArg);
                     fprintf(psHdl->pfHlp,"\n\n");
                     if (siErr<0) { return(siErr); }
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
                        int siErr=siClpPrnHlp(psHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,-1,psTab,FALSE);
                        if (siErr<0) { return(siErr); }
                     } else {
                        if (CLPISF_SEL(psArg->psStd->uiFlg)) {
                           int siErr=siClpPrnHlp(psHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,psArg->psFix->siTyp,psTab,FALSE);
                           if (siErr<0) { return(siErr); }
                        } else {
                           int siErr=siClpPrnHlp(psHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,psArg->psFix->siTyp,psTab,TRUE);
                           if (siErr<0) { return(siErr); }
                        }
                     }
                  } else {
                     int siErr=siClpPrnHlp(psHdl,psHdl->pfHlp,isAli,siLev,siLev+siDep,-1,psTab,FALSE);
                     if (siErr<0) { return(siErr); }
                  }
               }
            } else {
               if (isMan) {
                  if (psArg!=NULL) {
                     fprintf(psHdl->pfHlp, "SYNOPSIS\n");
                     fprintf(psHdl->pfHlp, "--------\n");
                     efprintf(psHdl->pfHlp,"HELP:   %s\n",psArg->psFix->pcHlp);
                     fprintf(psHdl->pfHlp, "PATH:   %s\n",fpcPat(psHdl,siLev-1));
                     fprintf(psHdl->pfHlp, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
                     fprintf(psHdl->pfHlp, "SYNTAX: ");
                     int siErr=siClpPrnSyn(psHdl,psHdl->pfHlp,FALSE,siLev-1,psArg);
                     fprintf(psHdl->pfHlp, "\n\n");
                     if (siErr<0) { return(siErr); }
                     fprintf(psHdl->pfHlp, "DESCRIPTION\n");
                     fprintf(psHdl->pfHlp, "-----------\n");
                     if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                        fprintm(psHdl->pfHlp,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,1);
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
            fprintf(psHdl->pfHlp,   "SYNTAX: > %s ",psHdl->pcPgm);
            int siErr=siClpPrnCmd(psHdl,psHdl->pfHlp,0,0,1,NULL,psHdl->psTab,FALSE,FALSE);
            fprintf(psHdl->pfHlp,"\n\n");
            if (siErr<0) { return(siErr); }
            fprintf(psHdl->pfHlp,"DESCRIPTION\n");
            fprintf(psHdl->pfHlp,"-----------\n");
            if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
               fprintm(psHdl->pfHlp,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHdl->pcMan,1);
            } else {
               fprintf(psHdl->pfHlp,"No detailed description available for this command.\n\n");
            }
         } else {
            int siErr=siClpPrnHlp(psHdl,psHdl->pfHlp,isAli,0,siDep,-1,psTab,FALSE);
            if (siErr<0) { return(siErr); }
         }
      }
   }
   return(CLP_OK);
}

static inline void vdPrintHdl(FILE* pfDoc, const TsHdl* psHdl, const TsParamDescription* psParamDesc, const char* pcKnd, const char* pcKyw, const char chHdl) {
   unsigned int   l,i,isLev=psHdl->uiLev>1;
   unsigned int   uiLev=psHdl->uiLev;

   if (chHdl==C_CRT) {
      uiLev+=1;
   } else if (chHdl=='+') {
      uiLev+=2;
   }
   if (psHdl->isAnc && psParamDesc != NULL) {
      efprintf(pfDoc, "[[%s%s%s]]\n", psParamDesc->pcAnchorPrefix, psParamDesc->pcCommand, psParamDesc->pcPath);
   }
   if (psHdl->isNbr) {
      if (isLev) {
         for (l=0;l<uiLev;l++) {
            fprintf(pfDoc,"=");
         }
         if (psHdl->isShl) {
            fprintf(pfDoc,   " %s %s\n\n",psParamDesc->pcNum,pcKyw);
         } else {
            fprintf(pfDoc,   " %s %s '%s'\n\n",psParamDesc->pcNum,pcKnd,pcKyw);
         }
      } else {
         if (psHdl->isShl) {
            fprintf(pfDoc,   "%s %s\n",psParamDesc->pcNum,pcKyw);
            l=strlen(psParamDesc->pcNum)+strlen(pcKyw)+1;
            for (i=0;i<l;i++) {
               fprintf(pfDoc,"%c",chHdl);
            }
            fprintf(pfDoc,"\n\n");
         } else {
            fprintf(pfDoc,   "%s %s '%s'\n",psParamDesc->pcNum,pcKnd,pcKyw);
            l=strlen(psParamDesc->pcNum)+strlen(pcKnd)+strlen(pcKyw)+4;
            for (i=0;i<l;i++) {
               fprintf(pfDoc,"%c",chHdl);
            }
            fprintf(pfDoc,"\n\n");
         }
      }
   } else {
      if (isLev) {
         for (l=0;l<uiLev;l++) { fprintf(pfDoc,"="); }
         if (psHdl->isShl) {
            fprintf(pfDoc,   " %s\n",pcKyw);
         } else {
            fprintf(pfDoc,   " %s '%s'\n",pcKnd,pcKyw);
         }
      } else {
         if (psHdl->isShl) {
            fprintf(pfDoc,   "%s\n",pcKyw);
            l=strlen(pcKyw);
            for (i=0;i<l;i++) { fprintf(pfDoc,"%c",chHdl); }
            fprintf(pfDoc,"\n\n");
         } else {
            fprintf(pfDoc,   "%s '%s'\n",pcKnd,pcKyw);
            l=strlen(pcKnd)+strlen(pcKyw)+3;
            for (i=0;i<l;i++) { fprintf(pfDoc,"%c",chHdl); }
            fprintf(pfDoc,"\n\n");
         }
      }
   }
   if (psHdl->isIdt && psParamDesc != NULL) {
      efprintf(pfDoc, "indexterm:[%s, %s, %s%s]\n\n", pcKnd, pcKyw, psParamDesc->pcCommand, psParamDesc->pcPath);
   }
}

extern int siClpDocu(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const char*                   pcPat,
   const char*                   pcNum,
   const char*                   pcKnd,
   const int                     isCmd,
   const int                     isDep,
   const int                     isMan,
   const int                     isAnc,
   const int                     isNbr,
   const int                     isIdt,
   const int                     isPat,
   const unsigned int            uiLev)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;

   if (psHdl->pcLst!=NULL) { psHdl->pcLst[0]=0x00; }

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->uiLev=uiLev;
   psHdl->isMan=isMan;
   psHdl->isDep=isDep;
   psHdl->isAnc=isAnc;
   psHdl->isNbr=isNbr;
   psHdl->isShl=FALSE;
   psHdl->isIdt=isIdt;
   psHdl->isPat=isPat;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":DOCU:");
   if (psHdl->uiLev) {
      if (psHdl->uiLev<3 || psHdl->uiLev>4) {
         return CLPERR(psHdl,CLPERR_INT,"Initial level for docu generation (%u) is not valid (<3 or >4)",psHdl->uiLev);
      }
   }

   if (pcNum!=NULL && strlen(pcNum)<100 && pcKnd!=NULL) {
      if (pfDoc!=NULL) {
         const TsSym*                  psTab=psHdl->psTab;
         TsSym*                        psArg=NULL;
         const char*                   pcPtr=NULL;
         const char*                   pcKyw=NULL;
         int                           siErr=0,siPos;
         unsigned int                  i,l=strlen(psHdl->pcCmd);
         char                          acNum[64];
         const char*                   p;
         if (pcPat!=NULL && *pcPat) {
            if (l==0 || strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
               if (l && strlen(pcPat)>l && pcPat[l]!='.') {
                  return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
               }
               if (strlen(pcPat)>l) {
                  int siLev;
                  strlcpy(acNum,pcNum,sizeof(acNum));
                  for (siLev=0,pcPtr=l?strchr(pcPat,'.'):pcPat-1;pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
                     char acKyw[CLPMAX_KYWSIZ];
                     for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) { acKyw[i]=pcKyw[i]; }
                     acKyw[i]=EOS;
                     siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,&siPos);
                     if (siErr<0) { return(siErr); }
                     siErr=siExtentSymTab(psHdl,siLev,psArg);
                     if (siErr<0) { return(siErr); }
                     psTab=psArg->psDep;
                     snprintc(acNum,sizeof(acNum),"%d.",siPos+1);
                  }
                  if (psArg!=NULL) {
                     TsParamDescription stParamDesc = stDefaultParamDesc;
                     stParamDesc.pcNum = acNum;
                     psHdl->isAnc=FALSE;
                     psHdl->isIdt=FALSE;
                     if (CLPISF_ARG(psArg->psStd->uiFlg)) {
                        if (!CLPISF_HID(psArg->psStd->uiFlg)) {
                           if (isMan) {
                              if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                                 for (p=psHdl->pcPgm;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                                 fprintf(pfDoc,".");
                                 l=strlen(psHdl->pcPgm)+strlen(fpcPat(psHdl,siLev))+4;
                              } else {
                                 l=strlen(fpcPat(psHdl,siLev))+3;
                              }
                              for (p=fpcPat(psHdl,siLev);*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                              fprintf(pfDoc,"(3)\n");
                              for (i=0;i<l;i++) { fprintf(pfDoc,"="); }
                              fprintf(pfDoc,"\n");
                              fprintf(pfDoc, ":doctype: manpage\n\n");
                              fprintf(pfDoc, "NAME\n");
                              fprintf(pfDoc, "----\n\n");
                              if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                                 for (p=psHdl->pcPgm;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                                 fprintf(pfDoc,".");
                                 for (p=fpcPat(psHdl,siLev);*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                              } else {
                                  vdClpPrnAli(pfDoc,", ",psArg);
                              }
                              efprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
                              fprintf(pfDoc, "SYNOPSIS\n");
                              fprintf(pfDoc, "--------\n\n");
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                              fprintf(pfDoc, "PATH:   %s\n",fpcPat(psHdl,siLev-1));
                              fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
                              fprintf(pfDoc, "SYNTAX: "); siErr=siClpPrnSyn(psHdl,pfDoc,FALSE,siLev-1,psArg); fprintf(pfDoc,"\n");
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                              if (siErr<0) { return(siErr); }
                              fprintf(pfDoc, "DESCRIPTION\n");
                              fprintf(pfDoc, "-----------\n\n");
                              if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                                 fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,2);
                              } else {
                                 fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                              }
                              fprintf(pfDoc, "AUTHOR\n");
                              fprintf(pfDoc, "------\n\n");
                              fprintf(pfDoc, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
                           } else {
                              char acArg[20];
                              switch (psArg->psFix->siTyp){
                              case CLPTYP_OBJECT:strcpy(acArg,"Object");break;
                              case CLPTYP_OVRLAY:strcpy(acArg,"Overlay");break;
                              default           :strcpy(acArg,"Parameter");break;
                              }
                              vdPrintHdl(pfDoc,psHdl,&stParamDesc,acArg,psArg->psStd->pcKyw,C_CRT);
                              fprintf(pfDoc, ".Synopsis\n\n");
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                              efprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
                              if(isPat) {
                              fprintf(pfDoc, "PATH:   %s\n",fpcPat(psHdl,siLev-1)); }
                              fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
                              fprintf(pfDoc, "SYNTAX: "); siErr=siClpPrnSyn(psHdl,pfDoc,FALSE,siLev-1,psArg); fprintf(pfDoc,"\n");
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                              if (siErr<0) { return(siErr); }
                              fprintf(pfDoc,".Description\n\n");
                              if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                                 fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,2);
                              } else {
                                 fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                              }
                              siErr=siClpPrnDoc(psHdl,pfDoc,siLev,&stParamDesc,psArg,psTab);
                              if (siErr<0) { return(siErr); }
                           }
                        }
                     } else if (CLPISF_CON(psArg->psStd->uiFlg)) {
                        if (!CLPISF_HID(psArg->psStd->uiFlg)) {
                           if (isMan) {
                              if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                                 for (p=psHdl->pcPgm;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                                 fprintf(pfDoc,".");
                                 l=strlen(psHdl->pcPgm)+strlen(fpcPat(psHdl,siLev))+4;
                              } else {
                                 l=strlen(fpcPat(psHdl,siLev))+3;
                              }
                              for(p=fpcPat(psHdl,siLev);*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                              fprintf(pfDoc,"(3)\n");
                              for (i=0;i<l;i++) { fprintf(pfDoc,"="); }
                              fprintf(pfDoc,"\n");
                              fprintf(pfDoc, ":doctype: manpage\n\n");
                              fprintf(pfDoc, "NAME\n");
                              fprintf(pfDoc, "----\n\n");
                              if (psHdl->pcPgm!=NULL && *psHdl->pcPgm) {
                                 for (p=psHdl->pcPgm;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                                 fprintf(pfDoc,".");
                                 for (p=fpcPat(psHdl,siLev);*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                              } else {
                                 for (p=psArg->psStd->pcKyw;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                              }
                              efprintf(pfDoc," - `%s`\n\n",psArg->psFix->pcHlp);
                              fprintf(pfDoc, "SYNOPSIS\n");
                              fprintf(pfDoc, "--------\n\n");
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                              fprintf(pfDoc, "PATH:   %s\n",fpcPat(psHdl,siLev-1));
                              fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
                              fprintf(pfDoc, "SYNTAX: %s\n",psArg->psStd->pcKyw);
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                              if (siErr<0) { return(siErr); }
                              fprintf(pfDoc, "DESCRIPTION\n");
                              fprintf(pfDoc, "-----------\n\n");
                              if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                                 fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,2);
                              } else {
                                 fprintf(pfDoc,"No detailed description available for this constant.\n\n");
                              }
                              fprintf(pfDoc, "AUTHOR\n");
                              fprintf(pfDoc, "------\n\n");
                              fprintf(pfDoc, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
                           } else if (!CLPISF_HID(psArg->psStd->uiFlg)) {
                              vdPrintHdl(pfDoc,psHdl,&stParamDesc,"Constant",psArg->psStd->pcKyw,'+');
                              fprintf(pfDoc, ".Synopsis\n\n");
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                              efprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
                              if(isPat) {
                              fprintf(pfDoc, "PATH:   %s\n",fpcPat(psHdl,siLev-1)); }
                              fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
                              fprintf(pfDoc, "SYNTAX: %s\n",psArg->psStd->pcKyw);
                              fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                              if (siErr<0) { return(siErr); }
                              fprintf(pfDoc,".Description\n\n");
                              if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
                                 fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,2);
                              } else {
                                 fprintf(pfDoc,"No detailed description available for this constant.\n\n");
                              }
                           }
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
                for (p=psHdl->pcPgm;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                fprintf(pfDoc,".");
                l=strlen(psHdl->pcPgm)+strlen(psHdl->pcCmd)+4;
            } else {
                l=strlen(psHdl->pcCmd)+3;
            }
            for (p=psHdl->pcCmd;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
            fprintf(pfDoc,   "(1)\n");
            for (i=0;i<l;i++) { fprintf(pfDoc,"="); }
            fprintf(pfDoc,"\n");
            fprintf(pfDoc,   ":doctype: manpage\n\n");
            fprintf(pfDoc,   "NAME\n");
            fprintf(pfDoc,   "----\n\n");
            if (psHdl->pcPgm!=NULL && strlen(psHdl->pcPgm)) {
                for (p=psHdl->pcPgm;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
                fprintf(pfDoc,".");
            }
            for (p=psHdl->pcCmd;*p;p++) { fprintf(pfDoc,"%c",tolower(*p)); }
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
            fprintf(pfDoc,   "SYNTAX: > %s ",psHdl->pcPgm); siErr=siClpPrnCmd(psHdl,pfDoc,0,0,1,NULL,psHdl->psTab,FALSE,FALSE); fprintf(pfDoc,"\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
            if (siErr<0) { return(siErr); }
            fprintf(pfDoc,   "DESCRIPTION\n");
            fprintf(pfDoc,   "-----------\n\n");
            if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
               fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHdl->pcMan,2);
            } else {
               fprintf(pfDoc,"No detailed description available for this command.\n\n");
            }
            fprintf(pfDoc,"AUTHOR\n");
            fprintf(pfDoc,"------\n\n");
            fprintf(pfDoc,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
         } else {
            TsParamDescription stParamDesc = {
                  .pcCommand = psHdl->pcCmd,
                  .pcPath = "",
                  .pcAnchorPrefix = isCmd?"CLEP.COMMAND.":"CLEP.OTHERCLP.",
                  .pcNum = pcNum
            };
            vdPrintHdl(pfDoc,psHdl,&stParamDesc,pcKnd,psHdl->pcCmd,C_TLD);
            fprintf(pfDoc,   ".Synopsis\n\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n");
            efprintf(pfDoc,  "HELP:   %s\n",psHdl->pcHlp);
            if (isPat) {
               fprintf(pfDoc,"PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
            }
            if (psHdl->isOvl) {
               fprintf(pfDoc,"TYPE:   OVERLAY\n");
            } else {
               fprintf(pfDoc,"TYPE:   OBJECT\n");
            }
            fprintf(pfDoc,   "SYNTAX: > %s ",psHdl->pcPgm); siErr=siClpPrnCmd(psHdl,pfDoc,0,0,1,NULL,psHdl->psTab,FALSE,FALSE); fprintf(pfDoc,"\n");
            fprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
            if (siErr<0) { return(siErr); }
            fprintf(pfDoc,   ".Description\n\n");
            if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
               fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHdl->pcMan,2);
            } else {
               fprintf(pfDoc,"No detailed description available for this command.\n\n");
            }
            siPos=siClpPrnDoc(psHdl,pfDoc,0,&stParamDesc,NULL,psTab);
            if (siPos<0) { return(siPos); }
         }
      }
   } else {
      return CLPERR(psHdl,CLPERR_INT,"No valid initial number or command string for head lines (%s)",psHdl->pcCmd);
   }
   return(CLP_OK);
}

static int siClpPrintTable(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsParamDescription*     psParamDesc,
   const char*                   pcPat,
   const char*                   pcFil,
   const TsSym*                  psTab);

static int siClpWriteRemaining(
   TsHdl*                        psHdl,
   FILE*                         pfDoc,
   const int                     siLev,
   const char*                   pcPat,
   const TsSym*                  psTab)
{
   int                           i=0;
   for (const TsSym* psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
      if (psHlp->psFix->pcMan==NULL) {
         if (CLPISF_ARG(psHlp->psStd->uiFlg) && CLPISF_CMD(psHlp->psStd->uiFlg) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
            if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
               return CLPERR(psHdl,CLPERR_TAB,"Manual page for %s '%s.%s' missing",pcMapClpTyp(psHlp->psFix->siTyp),pcPat,psHlp->psStd->pcKyw);
            }
            if (i==0) { fprintf(pfDoc,".Arguments\n\n"); }
            efprintf(pfDoc,"* `%s: ",pcMapClpTyp(psHlp->psFix->siTyp)); siClpPrnSyn(psHdl,pfDoc,FALSE,siLev,psHlp); efprintf(pfDoc," - %s`\n",psHlp->psFix->pcHlp);
            int siErr=siExtentSymTab(psHdl,siLev,psHlp);
            if (siErr<0) { return(siErr); }
            for (const TsSym* psSel=psHlp->psDep;psSel!=NULL;psSel=psSel->psNxt) {
               if (psSel->psFix->siTyp==psHlp->psFix->siTyp && !CLPISF_HID(psSel->psStd->uiFlg)) {
                  if (psSel->psFix->pcMan!=NULL) {
                     return CLPERR(psHdl,CLPERR_TAB,"Manual page for SELECTION '%s.%s.%s' useless",pcPat,psHlp->psStd->pcKyw,psSel->psStd->pcKyw);
                  }
                  efprintf(pfDoc,"** `%s - %s`\n",psSel->psStd->pcKyw,psSel->psFix->pcHlp);
               }
            }
            i++;
         } else if (CLPISF_CON(psHlp->psStd->uiFlg) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
            if (i==0) { fprintf(pfDoc,".Selections\n\n"); }
            efprintf(pfDoc,"* `%s - %s`\n",psHlp->psStd->pcKyw,psHlp->psFix->pcHlp);
            i++;
         }
      }
   }
   if (i) { fprintf(pfDoc,"\n"); }
   return(CLP_OK);
}

static int siClpPrintWritten(
   TsHdl*                        psHdl,
   FILE*                         pfDoc,
   const int                     siLev,
   const char*                   pcPat,
   const char*                   pcFil,
   const char*                   pcMan)
{
   long int s=ftell(pfDoc);
   if (s>0) {
      char* pcPge=malloc(s+1);
      if (pcPge==NULL) {
         return CLPERR(psHdl,CLPERR_SYS,"Allocation of memory for temporary file to print page for argument '%s' failed",psHdl->pcCmd);
      }
      errno=0;
      rewind(pfDoc);
      if (errno) {
         free(pcPge);
         return CLPERR(psHdl,CLPERR_SYS,"Rewind of temporary file to print page for command '%s' failed",psHdl->pcCmd);
      }
      size_t r=fread(pcPge,1,s,pfDoc);
      if (r!=s) {
         free(pcPge);
         return CLPERR(psHdl,CLPERR_SYS,"Read of temporary file to print page for command '%s' failed",psHdl->pcCmd);
      }
      pcPge[r]=0x00;

      const char* p=strchr(pcPge,'=');
      if (p==NULL) {
         free(pcPge);
         return CLPERR(psHdl,CLPERR_INT,"No headline found in manual page for command '%s' (no sign)",psHdl->pcCmd);
      }
      while (*p=='=') { p++; }
      if (*p!=' ') {
         free(pcPge);
         return CLPERR(psHdl,CLPERR_INT,"No headline found in manual page for command '%s' (no blank after sign)",psHdl->pcCmd);
      }
      p++;
      const char* e=strchr(p,'\n');
      if (e==NULL) {
         free(pcPge);
         return CLPERR(psHdl,CLPERR_INT,"No end of headline found in manual page for command '%s'",psHdl->pcCmd);
      }
      int l=e-p;
      char acHdl[l+1];
      memcpy(acHdl,p,l);
      acHdl[l]=0x00;
      l=strlen(pcFil);
      char acFil[l+16];
      unsigned int uiHsh=fnvHash(l,(const unsigned char*)pcFil);
      int i;
      for (i=0;i<l;i++) {
         if (pcFil[i]=='\t') {
            acFil[i]=psHdl->siPs1;
         } else if (pcFil[i]=='\v') {
            acFil[i]=psHdl->siPs2;
         } else if (isalnum(pcFil[i])) {
            acFil[i]=tolower(pcFil[i]);
         } else {
            acFil[i]=psHdl->siPr3;
         }
      }
      acFil[i]=0x00;
      snprintc(acFil,sizeof(acFil),"%c%04x",psHdl->siPr3,uiHsh&0xFFFF);
      int siErr=psHdl->pfPrn(psHdl->pvPrn,psHdl->uiLev+siLev,acHdl,pcPat,acFil,pcMan,pcPge);
      free(pcPge);
      if (siErr<0) {
         return CLPERR(psHdl,CLPERR_SYS,"Print page over call back function for command '%s' failed with %d",psHdl->pcCmd,siErr);
      }
   }
   return(CLP_OK);
}

extern int siClpPrint(
   void*                         pvHdl,
   const char*                   pcFil,
   const char*                   pcNum,
   const char*                   pcKnd,
   const int                     isCmd,
   const int                     isDep,
   const int                     isAnc,
   const int                     isNbr,
   const int                     isShl,
   const int                     isIdt,
   const int                     isPat,
   const unsigned int            uiLev,
   const int                     siPs1,
   const int                     siPs2,
   const int                     siPr3,
   void*                         pvPrn,
   TfClpPrintPage*               pfPrn)
{
   TsHdl*                        psHdl=(TsHdl*)pvHdl;

   if (psHdl->pcLst!=NULL) { psHdl->pcLst[0]=0x00; }

   psHdl->pcInp=NULL;
   psHdl->pcCur=NULL;
   psHdl->pcOld=NULL;
   psHdl->pcRow=NULL;
   psHdl->siCol=0;
   psHdl->siRow=0;
   psHdl->uiLev=uiLev;
   psHdl->isMan=FALSE;
   psHdl->isDep=TRUE;
   psHdl->isAnc=isAnc;
   psHdl->isNbr=isNbr;
   psHdl->isShl=isShl;
   psHdl->isIdt=isIdt;
   psHdl->isPat=isPat;
   psHdl->siPs1=siPs1;
   psHdl->siPs2=siPs2;
   psHdl->siPr3=siPr3;
   psHdl->pvPrn=pvPrn;
   psHdl->pfPrn=pfPrn;
   psHdl->pcLex[0]=EOS;
   psHdl->pcMsg[0]=EOS;
   psHdl->pcPat[0]=EOS;
   psHdl->pcPre[0]=EOS;
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,0,":DOCU:");
   if (psHdl->uiLev) {
      if (psHdl->uiLev<3 || psHdl->uiLev>4) {
         return CLPERR(psHdl,CLPERR_INT,"Initial level for docu generation (%u) is not valid (<3 or >4)",psHdl->uiLev);
      }
   }
   if (psHdl->pfPrn==NULL) {
      return CLPERR(psHdl,CLPERR_INT,"No print callback function provided");
   }

   if (pcNum!=NULL && pcKnd!=NULL) {
      int   siErr;
      FILE* pfDoc;
      char  acFil[strlen(pcFil)+strlen(psHdl->pcCmd)+2];
      snprintf(acFil,sizeof(acFil),"%s\t%s",pcFil,psHdl->pcCmd);
      pfDoc=fopen_tmp();
      if (pfDoc==NULL) {
         return CLPERR(psHdl,CLPERR_SYS,"Open of temporary file to print main page for command '%s' failed",psHdl->pcCmd);
      }
      TsParamDescription stParamDesc = {
            .pcCommand = psHdl->pcCmd,
            .pcPath = "",
            .pcAnchorPrefix = isCmd?"CLEP.COMMAND.":"CLEP.OTHERCLP.",
            .pcNum = pcNum
      };
      vdPrintHdl(pfDoc,psHdl,&stParamDesc,pcKnd,psHdl->pcCmd,C_TLD);
      efprintf(pfDoc,   ".Synopsis\n\n");
      efprintf(pfDoc,   "-----------------------------------------------------------------------\n");
      efprintf(pfDoc,   "HELP:   %s\n",psHdl->pcHlp);
      if(psHdl->isPat) {
         efprintf(pfDoc,"PATH:   %s.%s\n",psHdl->pcOwn,psHdl->pcPgm);
      }
      if (psHdl->isOvl) {
         efprintf(pfDoc,"TYPE:   OVERLAY\n");
      } else {
         efprintf(pfDoc,"TYPE:   OBJECT\n");
      }
      efprintf(pfDoc,   "SYNTAX: > %s ",psHdl->pcPgm);
      siErr=siClpPrnCmd(psHdl,pfDoc,0,0,1,NULL,psHdl->psTab,FALSE,FALSE);
      fprintf(pfDoc,    "\n");
      efprintf(pfDoc,   "-----------------------------------------------------------------------\n\n");
      if (siErr<0) {
         fclose_tmp(pfDoc);
         return(siErr);
      }
      efprintf(pfDoc,   ".Description\n\n");
      if (psHdl->pcMan!=NULL && *psHdl->pcMan) {
         fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHdl->pcMan,2);
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Manual page for COMMAND/OTHERCLP '%s' missing",psHdl->pcCmd);
      }

      siErr=siClpWriteRemaining(psHdl,pfDoc,0,psHdl->pcCmd,psHdl->psTab);
      if (siErr<0) {
         fclose_tmp(pfDoc);
         return(siErr);
      }

      siErr=siClpPrintWritten(psHdl,pfDoc,0,psHdl->pcCmd,acFil,psHdl->pcMan);
      fclose_tmp(pfDoc);
      if (siErr<0) { return(siErr); }
      if (isDep) {
         siErr=siClpPrintTable(psHdl,0,&stParamDesc,psHdl->pcCmd,acFil,psHdl->psTab);
         if (siErr<0) { return(siErr); }
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
   const TsSym*                  psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   unsigned int                  l=strlen(psHdl->pcCmd);
   const char*                   pcArg=NULL;

   if (psHdl->pcLst!=NULL) { psHdl->pcLst[0]=0x00; }

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

   if (pfOut==NULL) { pfOut=psHdl->pfHlp; }
   if (pfOut!=NULL) {
      if (pcPat!=NULL && *pcPat) {
         if (l==0 || strxcmp(psHdl->isCas,psHdl->pcCmd,pcPat,l,0,FALSE)==0) {
            int siLev;
            if (l && strlen(pcPat)>l && pcPat[l]!='.') {
               return CLPERR(psHdl,CLPERR_SEM,"Path (%s) is not valid",pcPat);
            }
            for (siLev=0,pcPtr=l?strchr(pcPat,'.'):pcPat-1;pcPtr!=NULL && siLev<CLPMAX_HDEPTH;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
               int i,siErr;
               char acKyw[CLPMAX_KYWSIZ];
               for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) { acKyw[i]=pcKyw[i]; }
               acKyw[i]=EOS;
               if (pcArg!=NULL) {
                  return CLPERR(psHdl,CLPERR_SEM,"Path (%s) contains too many or invalid qualifiers",pcPat);
               }
               siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,NULL);
               if (siErr<0) { return(siErr); }
               siErr=siExtentSymTab(psHdl,siLev,psArg);
               if (siErr<0) { return(siErr); }
               if (psArg->psDep!=NULL) {
                  psTab=psArg->psDep;
               } else {
                  pcArg=psArg->psStd->pcKyw;
               }
            }
            if (pcArg!=NULL) { siLev--; }
            int siErr=siClpPrnPro(psHdl,pfOut,FALSE,siMtd,siLev,siLev+siDep,psTab,pcArg);
            if (siErr<0) { return(siErr); }
         } else {
            return CLPERR(psHdl,CLPERR_SEM,"Root of path (%s) does not match root of handle (%s)",pcPat,psHdl->pcCmd);
         }
      } else {
         int siErr=siClpPrnPro(psHdl,pfOut,FALSE,siMtd,0,siDep,psTab,NULL);
         if (siErr<0) { return(siErr); }
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
   case CLPSYM_HIH:   psHdl->psSym = psHdl->psSym->psHih;        break;
   case CLPSYM_ALIAS: psHdl->psSym = psHdl->psSym->psStd->psAli; break;
   case CLPSYM_COUNT: psHdl->psSym = psHdl->psSym->psFix->psCnt; break;
   case CLPSYM_ELN:   psHdl->psSym = psHdl->psSym->psFix->psEln; break;
   case CLPSYM_LINK:  psHdl->psSym = psHdl->psSym->psFix->psLnk; break;
   case CLPSYM_OID:   psHdl->psSym = psHdl->psSym->psFix->psOid; break;
   case CLPSYM_SLN:   psHdl->psSym = psHdl->psSym->psFix->psSln; break;
   case CLPSYM_TLN:   psHdl->psSym = psHdl->psSym->psFix->psTln; break;
   case CLPSYM_DEP: {
      int siErr=siExtentSymTab(psHdl,0,psHdl->psSym);
      if (siErr<0) { return(siErr); }
      psHdl->psSym = psHdl->psSym->psDep;
   } break;
   default: return CLPERR(psHdl,CLPERR_PAR,"Operation (%u) for symbol table walk not supported",uiOpr);
   }
   if (psHdl->psSym!=NULL) {
      TsSym*               apTmp[CLPMAX_HDEPTH];
      TsSym*               psTmp;
      int                  i,siErr;

      if (psHdl->pcCmd!=NULL && *psHdl->pcCmd) {
         srprintf(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->pcCmd),"%s",psHdl->pcCmd);
      } else {
         psHdl->pcPat[0]=EOS;
      }
      for (i=0,psTmp=psHdl->psSym->psHih;i<CLPMAX_HDEPTH && psTmp!=NULL;psTmp=psTmp->psHih,i++) { apTmp[i]=psTmp; }
      while (i>0) {
         srprintc(&psHdl->pcPat,&psHdl->szPat,strlen(apTmp[i-1]->psStd->pcKyw),".%s",apTmp[i-1]->psStd->pcKyw);
         i--;
      }
      siErr=siExtentSymTab(psHdl,0,psHdl->psSym);
      if (siErr<0) { return(siErr); }

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
      if (psHdl->psTab!=NULL) {
         uint64_t uiCnt=0;
         uint64_t uiSiz=0;
         vdClpSymDel(psHdl->psTab,&uiCnt,&uiSiz);
         if (CHECK_ENVAR_ON("CLP_SYMTAB_STATISTICS")) {
            char acTs[24];
            fprintf(stderr,"%s CLP_SYMTAB_STATISTICS(Amount(%"PRIu64"),Size(%"PRIu64")) in vdClpClose()\n",cstime(0,acTs),uiCnt,uiSiz);
         }
         psHdl->psTab=NULL;
      }

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
         free(psHdl);
         break;
      default:
         if (psHdl->psPtr!=NULL) {
            vdClpFree(psHdl);
            free(psHdl->psPtr);
            psHdl->psPtr=NULL;
            psHdl->szPtr=0;
            psHdl->siPtr=0;
         }
         free(psHdl);
         break;
      }
   }
}

/* Interne Funktionen *************************************************/

static const char* get_env(char* var,const size_t size,const char* fmtstr, ...)
{
   int                  i,r;
   va_list              argv;
   va_start(argv,fmtstr);
   r=vsnprintf(var,size,fmtstr,argv);
   va_end(argv);
   if (r>=size) {
      var[size-1]=0x00;
   }
   for (i=0;var[i];i++) {
      var[i]=toupper(var[i]);
      if (var[i]=='.') { var[i]='_'; }
   }
   return(GETENV(var));
}

#undef  ERROR
#define ERROR(s) do {\
   if ((s)!=NULL) {                                                                       \
      if ((s)->psStd!=NULL) { free((s)->psStd); (s)->psStd=NULL; }                        \
      if ((s)->psFix!=NULL) {                                                             \
         if ((s)->psFix->pcPro!=NULL) { free((s)->psFix->pcPro); (s)->psFix->pcPro=NULL; }\
         if ((s)->psFix->pcSrc!=NULL) { free((s)->psFix->pcSrc); (s)->psFix->pcSrc=NULL; }\
         free((s)->psFix); (s)->psFix=NULL;                                               \
      }                                                                                   \
      if ((s)->psVar!=NULL) { free((s)->psVar); (s)->psVar=NULL; }                        \
      free(s);\
   } return NULL;\
} while (0)

static TsSym* psClpSymIns(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsClpArgument*          psArg,
   const TsClpArgument*          psTab,
   TsSym*                        psHih,
   TsSym*                        psCur)
{
   TsSym*                        psSym;
   TsSym*                        psHlp;
   const char*                   pcEnv=NULL;
   const char*                   pcPat=fpcPat(psHdl,siLev);
   char                          acVar[strlen(psHdl->pcOwn)+strlen(psHdl->pcPgm)+strlen(pcPat)+strlen(psArg->pcKyw)+4];
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
         CLPERR(psHdl,CLPERR_TAB,"Invalid first letter (%c(0x%02X)) in keyword '%s.%s'",*psArg->pcKyw,*psArg->pcKyw,pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      for (const char* p=psArg->pcKyw+1; *p; p++) {
         if (CLPISF_CON(psArg->uiFlg)?!isCst(*p):!isKyw(*p)) {
            CLPERR(psHdl,CLPERR_TAB,"Invalid letter (%c(0x%02X)) in keyword '%s.%s'",*p,*p,pcPat,psArg->pcKyw);
            ERROR(psSym);
         }
      }
   }

   if (psArg->pcAli!=NULL) {
      if (!isalpha(*psArg->pcAli)) {
         CLPERR(psHdl,CLPERR_TAB,"Invalid first letter (%c(0x%02X)) in alias '%s.%s'",*psArg->pcAli,*psArg->pcAli,pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      for (const char* p=psArg->pcAli+1; *p; p++) {
         if (CLPISF_CON(psArg->uiFlg)?!isCst(*p):!isKyw(*p)) {
            CLPERR(psHdl,CLPERR_TAB,"Invalid letter (%c(0x%02X)) in alias '%s.%s'",*p,*p,pcPat,psArg->pcAli);
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
   if (CHECK_ENVAR_OFF("CLEP_NO_SECRETS")) {
      psSym->psStd->uiFlg&=~CLPFLG_PWD;
   }
   psSym->psArg=psArg;
   psSym->psTab=psTab;
   psSym->psStd->psAli=NULL;
   psSym->psStd->siKwl=strlen(psSym->psStd->pcKyw);
#ifdef __DEBUG__
   psSym->psStd->siLev=siLev;
#endif
   psSym->psFix->pcMan=psArg->pcMan;
   psSym->psFix->pcHlp=psArg->pcHlp;
   pcEnv=NULL;
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
      const size_t szSrc=strlen(CLPSRC_ENV)+strlen(acVar)+1;
      psSym->psFix->pcSrc=malloc(szSrc);
      if (psSym->psFix->pcSrc==NULL) {
         CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element source '%s.%s' failed",pcPat,psArg->pcKyw);
         ERROR(psSym);
      }
      snprintf(psSym->psFix->pcSrc,szSrc,"%s%s",CLPSRC_ENV,acVar);
      psSym->psFix->siRow=1;
   } else {
      psSym->psFix->pcDft=psArg->pcDft;
      if (psArg->pcDft!=NULL) {
         const size_t szSrc=strlen(CLPSRC_DEF)+strlen(pcPat)+strlen(psArg->pcKyw)+2;
         psSym->psFix->pcSrc=malloc(szSrc);
         if (psSym->psFix->pcSrc==NULL) {
            CLPERR(psHdl,CLPERR_MEM,"Allocation of memory for symbol element source '%s.%s' failed",pcPat,psArg->pcKyw);
            ERROR(psSym);
         }
         snprintf(psSym->psFix->pcSrc,szSrc,"%s%s.%s",CLPSRC_DEF,pcPat,psArg->pcKyw);
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
      CLPERR(psHdl,CLPERR_TAB,"Default flag for type '%s' of argument '%s.%s' not supported",pcMapClpTyp(psSym->psFix->siTyp),pcPat);
      ERROR(psSym);
   }
   if (psSym->psFix->siTyp==CLPTYP_NUMBER && CLPISF_DEF(psSym->psStd->uiFlg) && psSym->psFix->siMax>1) {
      CLPERR(psHdl,CLPERR_TAB,"Default flag for arrays of type number of argument '%s.%s' not supported",pcPat);
      ERROR(psSym);
   }

   if (psArg->pcAli!=NULL) {
      int k;
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
               psSym->psStd->uiFlg=psHlp->psStd->uiFlg|psArg->uiFlg|CLPFLG_ALI;
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
      // cppcheck-suppress knownConditionTrueFalse
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
      // cppcheck-suppress knownConditionTrueFalse
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
            CLPERR(psHdl,CLPERR_TAB,"Type '%s' for constant '%s.%s' requires a value (pcVal==NULL)",pcMapClpTyp(psSym->psFix->siTyp),pcPat,psSym->psStd->pcKyw);
            ERROR(psSym);
         }
         if (!CLPISF_BIN(psSym->psStd->uiFlg) && psSym->psFix->siSiz==0) {
            psSym->psFix->siSiz=strlen((const char*)psArg->pcVal)+1;
         }
         psSym->psVar->pvDat=(void*)psArg->pcVal;
         break;
      default:
         CLPERR(psHdl,CLPERR_TAB,"Type (%s) for argument '%s.%s' not supported for constant definitions",pcMapClpTyp(psSym->psFix->siTyp),pcPat,psSym->psStd->pcKyw);
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

   if (!CLPISF_CMD(psSym->psStd->uiFlg) && !CLPISF_PRO(psSym->psStd->uiFlg) && !CLPISF_DMY(psSym->psStd->uiFlg) && !CLPISF_HID(psSym->psStd->uiFlg)) {
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
      if (psHih!=NULL) {
         psHih->psDep=(psSym->psStd->psAli!=NULL)?psSym->psStd->psAli:psSym;
         if (psHih->psStd->psAli!=NULL) {
            if (psHih->psStd->psAli->psDep==NULL) {
               psHih->psStd->psAli->psDep=(psSym->psStd->psAli!=NULL)?psSym->psStd->psAli:psSym;
            } else {
               if (psHih->psStd->psAli->psDep->psStd->pcKyw!=psSym->psStd->pcKyw) {
                  CLPERR(psHdl,CLPERR_TAB,"Keyword to alias (%p/%p(%s/%s)) of argument '%s.%s' mismatch",
                        psHih->psStd->psAli->psDep->psStd->pcKyw,psSym->psStd->pcKyw,
                        psHih->psStd->psAli->psDep->psStd->pcKyw,psSym->psStd->pcKyw,
                        pcPat,psSym->psStd->pcKyw);
                  ERROR(psSym);
               }
            }
         } else {
            psHlp=psHih->psNxt;
            while(psHlp!=NULL && psHlp->psStd->psAli==psHih) { // Propagate psDep for each alias
               psHlp->psDep=(psSym->psStd->psAli!=NULL)?psSym->psStd->psAli:psSym;
               psHlp=psHlp->psNxt;
            }
         }
      }
   }
   psSym->psDep=NULL;
   psSym->psHih=psHih;
   psHdl->siSym++;
   return(psSym);
}
#undef ERROR

static int siClpSymIni(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsClpArgument*          psArg,
   const TsClpArgument*          psTab,
   TsSym*                        psHih,
   TsSym**                       ppFst)
{
   int                           siErr;
   TsSym*                        psCur=NULL;
   int                           i,j;

   if (psTab==NULL) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table not defined%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s:%s' not defined",fpcPat(psHdl,siLev),psArg->pcKyw);
      }
   }

   for (j=i=0;psTab[i].siTyp;i++) {
      if (i>=CLPMAX_TABCNT) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Argument table bigger than maximal supported entry count (%d)",CLPMAX_TABCNT);
         } else {
            return CLPERR(psHdl,CLPERR_TAB,"Table for key word (%s:%s) bigger than maximal supported entry count (%d)",fpcPat(psHdl,siLev),psArg->pcKyw,CLPMAX_TABCNT);
         }
      }

      if (psTab[i].pcKyw==NULL || psTab[i].pcKyw[0]==0x00) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"There is no keyword defined in argument table at index %d",i);
         } else {
            return CLPERR(psHdl,CLPERR_TAB,"The table for keyword (%s:%s) has no keyword defined at index %d",fpcPat(psHdl,siLev),psArg->pcKyw,i);
         }
      } else {
         if (strlen(psTab[i].pcKyw)>CLPMAX_KYWLEN) {
            if (psArg==NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"The keyword defined in argument table at index %d is too long (>%d)",i,CLPMAX_KYWLEN);
            } else {
               return CLPERR(psHdl,CLPERR_TAB,"The keyword defined in table (%s:%s) at index %d is too long (>%d)",fpcPat(psHdl,siLev),psArg->pcKyw,i,CLPMAX_KYWLEN);
            }
         }
      }

      if (!CLPISF_DMY(psTab[i].uiFlg)) {
         psCur=psClpSymIns(psHdl,siLev,psTab+i,psTab[i].psTab,psHih,psCur);
         if (psCur==NULL) {
            return CLPERR(psHdl,CLPERR_SYS,"Insert of symbol (%s.%s) in symbol table failed",fpcPat(psHdl,siLev),psTab[i].pcKyw);
         }
         if (j==0 && ppFst!=NULL) { (*ppFst)=psCur; }

         switch (psTab[i].siTyp) {
         case CLPTYP_SWITCH:
            if (psTab[i].psTab!=NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s.%s' of type switch is defined (NULL for psTab required)",fpcPat(psHdl,siLev),psTab[i].pcKyw);
            }
            if (psTab[i].siMax!=1) {
               return CLPERR(psHdl,CLPERR_TAB,"Array definition not possible for a switch (%s.%s)",fpcPat(psHdl,siLev),psTab[i].pcKyw);
            }
            break;
         case CLPTYP_NUMBER:
         case CLPTYP_FLOATN:
         case CLPTYP_STRING:
            if (CLPISF_SEL(psTab[i].uiFlg)) {
               psHdl->apPat[siLev]=psCur;
               siErr=siClpSymIni(psHdl,siLev+1,psTab+i,psTab[i].psTab,psCur,NULL);
               if (siErr<0) { return(siErr); }
            } else {
               if (psTab[i].psTab!=NULL) {
                  psHdl->apPat[siLev]=psCur;
                  siErr=siClpSymIni(psHdl,siLev+1,psTab+i,psTab[i].psTab,psCur,NULL);
                  if (siErr<0) { return(siErr); }
               }
            }
            break;
         case CLPTYP_OBJECT:
         case CLPTYP_OVRLAY:
            if (psHdl->isFul) {
               psHdl->apPat[siLev]=psCur;
               siErr=siClpSymIni(psHdl,siLev+1,psTab+i,psTab[i].psTab,psCur,NULL);
               if (siErr<0) { return(siErr); }
            }
            break;
         case CLPTYP_XALIAS:
            break;
         default:
            return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter '%s.%s' not supported",psTab[i].siTyp,fpcPat(psHdl,siLev),psTab[i].pcKyw);
         }
         j++;
      }
   }
   return(CLP_OK);
}

static int siClpSymLnkCnt(
   TsHdl*            psHdl,
   int               siLev,
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   int siErr=siExtentSymTab(psHdl,siLev,psSym);
   if (siErr<0) { return(siErr); }
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psCnt=psLnk;
         siErr=siClpSymLnkCnt(psHdl,siLev+1,psSon,psLnk);
         if (siErr<0) { return(siErr); }
      }
   }
   return(CLP_OK);
}

static int siClpSymLnkOid(
   TsHdl*            psHdl,
   int               siLev,
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   int siErr=siExtentSymTab(psHdl,siLev,psSym);
   if (siErr<0) { return(siErr); }
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psOid=psLnk;
         siErr=siClpSymLnkOid(psHdl,siLev+1,psSon,psLnk);
         if (siErr<0) { return(siErr); }
      }
   }
   return(CLP_OK);
}

static int siClpSymLnkInd(
   TsHdl*            psHdl,
   int               siLev,
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   int siErr=siExtentSymTab(psHdl,siLev,psSym);
   if (siErr<0) { return(siErr); }
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psInd=psLnk;
         siErr=siClpSymLnkInd(psHdl,siLev+1,psSon,psLnk);
         if (siErr<0) { return(siErr); }
      }
   }
   return(CLP_OK);
}

static int siClpSymLnkEln(
   TsHdl*            psHdl,
   int               siLev,
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   int siErr=siExtentSymTab(psHdl,siLev,psSym);
   if (siErr<0) { return(siErr); }
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psEln=psLnk;
         siErr=siClpSymLnkEln(psHdl,siLev+1,psSon,psLnk);
         if (siErr<0) { return(siErr); }
      }
   }
   return(CLP_OK);
}

static int siClpSymLnkSln(
   TsHdl*            psHdl,
   int               siLev,
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   int siErr=siExtentSymTab(psHdl,siLev,psSym);
   if (siErr<0) { return(siErr); }
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psSln=psLnk;
         siErr=siClpSymLnkSln(psHdl,siLev+1,psSon,psLnk);
         if (siErr<0) { return(siErr); }
      }
   }
   return(CLP_OK);
}

static int siClpSymLnkTln(
   TsHdl*            psHdl,
   int               siLev,
   TsSym*            psSym,
   TsSym*            psLnk)
{
   TsSym*            psSon;
   int siErr=siExtentSymTab(psHdl,siLev,psSym);
   if (siErr<0) { return(siErr); }
   for (psSon=psSym->psDep; psSon!=NULL; psSon=psSon->psNxt) {
      if (psSon->psFix->siTyp==CLPTYP_OVRLAY) {
         psSon->psFix->psTln=psLnk;
         siErr=siClpSymLnkTln(psHdl,siLev+1,psSon,psLnk);
         if (siErr<0) { return(siErr); }
      }
   }
   return(CLP_OK);
}

static int siClpSymCal(
   TsHdl*                        psHdl,
   int                           siLev,
   const TsSym*                  psArg,
   TsSym*                        psTab)
{
   TsSym*                        psSym;
   TsSym*                        psHlp=NULL;
   int                           isPar=FALSE;
   int                           isCon=FALSE;
   int                           siCon=0;
   int                           siErr,siPos,k,h;

   for (siPos=0,psSym=psTab;psSym!=NULL;psSym=psSym->psNxt,siPos++) {
#ifdef __DEBUG__
      if (psSym->psStd->siLev!=siLev) {
         if (psArg==NULL) {
            return CLPERR(psHdl,CLPERR_INT,"Argument table not in sync with symbol table%s","");
         } else {
            return CLPERR(psHdl,CLPERR_INT,"Parameter table of argument '%s.%s' not in sync with symbol table",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
      }
#endif
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
                     siErr=siClpSymLnkCnt(psHdl,siLev,psHlp,psSym);
                     if (siErr<0) { return(siErr); }
                  }
               }
               if (CLPISF_OID(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psOid=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     siErr=siClpSymLnkOid(psHdl,siLev,psHlp,psSym);
                     if (siErr<0) { return(siErr); }
                  }
               }
               if (CLPISF_IND(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psInd=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     siErr=siClpSymLnkInd(psHdl,siLev,psHlp,psSym);
                     if (siErr<0) { return(siErr); }
                  }
               }
               if (CLPISF_ELN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psEln=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     siErr=siClpSymLnkEln(psHdl,siLev,psHlp,psSym);
                     if (siErr<0) { return(siErr); }
                  }
               }
               if (CLPISF_SLN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psSln=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     siErr=siClpSymLnkSln(psHdl,siLev,psHlp,psSym);
                     if (siErr<0) { return(siErr); }
                  }
               }
               if (CLPISF_TLN(psSym->psStd->uiFlg)) {
                  psHlp->psFix->psTln=psSym; k++;
                  if (psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     siErr=siClpSymLnkTln(psHdl,siLev,psHlp,psSym);
                     if (siErr<0) { return(siErr); }
                  }
               }
            }
         }
         if (psSym->psFix->psLnk==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword '%s.%s' cannot be resolved",fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
         }
         if (h>1) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword '%s.%s' is not unique",fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
         }
         if (k>1) {
            return CLPERR(psHdl,CLPERR_TAB,"More than one link defined for keyword '%s.%s'",fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
         }
         if (k==0) {
            return CLPERR(psHdl,CLPERR_TAB,"Link for keyword '%s.%s' was not assigned",fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
         }
      } else if (CLPISF_CON(psSym->psStd->uiFlg)) {
         isCon=TRUE; siCon++;
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Kind (ALI/ARG/LNK/CON) of argument '%s.%s' not determinable",fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
      }

      psSym->psStd->siKwl=strlen(psSym->psStd->pcKyw);
      if (psHdl->siMkl>0) {
         if (psSym->psStd->siKwl>psHdl->siMkl) { psSym->psStd->siKwl=psHdl->siMkl; }
         if (!CLPISF_LNK(psSym->psStd->uiFlg)) {
            for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
               if (psHlp!=psSym && !CLPISF_LNK(psHlp->psStd->uiFlg)) {
                  if (psHdl->isCas) {
                     for (k=0;psSym->psStd->pcKyw[k] && psHlp->psStd->pcKyw[k] &&         psSym->psStd->pcKyw[k] ==        psHlp->psStd->pcKyw[k] ;k++) {}
                  } else {
                     for (k=0;psSym->psStd->pcKyw[k] && psHlp->psStd->pcKyw[k] && toupper(psSym->psStd->pcKyw[k])==toupper(psHlp->psStd->pcKyw[k]);k++) {}
                  }
                  if (psSym->psStd->pcKyw[k]==0 && psHlp->psStd->pcKyw[k]==0) {
                     return CLPERR(psHdl,CLPERR_TAB,"Key word '%s.%s' is not unique",fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
                  }
                  if (psSym->psStd->pcKyw[k]==0) {
                     psSym->psStd->siKwl=k;
                     if (psHlp->psStd->siKwl<=k) { psHlp->psStd->siKwl=k+1; }
                  } else if (psHlp->psStd->pcKyw[k]==0) {
                     psHlp->psStd->siKwl=k;
                     if (psSym->psStd->siKwl<=k) { psSym->psStd->siKwl=k+1; }
                  } else {
                     if (psSym->psStd->siKwl<=k) { psSym->psStd->siKwl=k+1; }
                  }
               }
            }
         }
      }
      if (psSym->psDep!=NULL) {
         psHdl->apPat[siLev]=psSym;
         siErr=siClpSymCal(psHdl,siLev+1,psSym,psSym->psDep);
         if (siErr<0) { return(siErr); }
      }
#ifdef __DEBUG__
      char  acKyw[strlen(psSym->psStd->pcKyw)+1];
      strcpy(acKyw,psSym->psStd->pcKyw);
      for (int i=strlen(acKyw);i>=psSym->psStd->siKwl;i--) {
         acKyw[i]=0x00;
         if (CLPTOK_KYW!=siClpConNat(psHdl,psHdl->pfErr,NULL,acKyw,NULL,NULL,-1,NULL)) {
            fprintf(stderr,"%s:%d:1: warning: Constant keyword (%s) re-used in table definitions (%s.%s)\n",__FILE__,__LINE__,acKyw,fpcPat(psHdl,siLev),psSym->psStd->pcKyw);
         }
      }
#endif
   }

   if (isCon && (isPar || siCon!=siPos)) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table is not consistent (mix of constants and parameter)%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s.%s' is not consistent (mix of constants and parameter)",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
   }
   if (isCon==FALSE && isPar==FALSE) {
      if (psArg==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Argument table neither contains constants nor arguments%s","");
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Parameter table of argument '%s.%s' neither contains constants nor arguments",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
   }
   for (psSym=psTab;psSym!=NULL;psSym=psSym->psNxt) {
      if (!CLPISF_LNK(psSym->psStd->uiFlg)) {
         if (psSym->psStd->siKwl<=0) {
            return CLPERR(psHdl,CLPERR_TAB,"Required keyword length (%d) of argument '%s.%s' is smaller or equal to zero",psSym->psStd->siKwl,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         if (psSym->psStd->siKwl>(int)strlen(psSym->psStd->pcKyw)) {
            return CLPERR(psHdl,CLPERR_TAB,"Required keyword length (%d) of argument '%s.%s' is greater than keyword length",psSym->psStd->siKwl,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
      }
      if (CLPISF_ALI(psSym->psStd->uiFlg)) {
         psSym->psDep=psSym->psStd->psAli->psDep;
         psSym->psHih=psSym->psStd->psAli->psHih;
         psSym->psArg=psSym->psStd->psAli->psArg;
         psSym->psTab=psSym->psStd->psAli->psTab;
      }
   }
   return(CLP_OK);
}

extern const TsClpArgument* psClpFindArgument(
   const int                     isCas,
   const int                     siKwl,
   const char*                   pcKyw,
   const TsClpArgument*          psTab)
{
   const TsClpArgument*          psHlp=NULL;
   int                           i,j,k,l=strlen(pcKyw);
   if (l>siKwl) { l=siKwl; }
   for (i=0,psHlp=psTab;psHlp!=NULL && psHlp->pcKyw!=NULL;psHlp++,i++) {
      if (!CLPISF_LNK(psHlp->uiFlg)) {
         if (isCas) {
            for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->pcKyw!=NULL;j++) {
               if (pcKyw[j]!=psHlp->pcKyw[j]) { k++; }
            }
         } else {
            for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->pcKyw!=NULL;j++) {
               if (toupper(pcKyw[j])!=toupper(psHlp->pcKyw[j])) { k++; }
            }
         }
         if (k==0 && j>=l) {
            return(psHlp);
         }
      }
   }
   return(NULL);
}

static const TsSym* psClpFndSym(
   TsHdl*                        psHdl,
   const char*                   pcKyw,
   const TsSym*                  psTab)
{
   const TsSym*                  psHlp=NULL;
   int                           i,j,k;
   for (i=0,psHlp=psTab;psHlp!=NULL && psHlp->psStd!=NULL;psHlp=psHlp->psNxt,i++) {
      if (!CLPISF_LNK(psHlp->psStd->uiFlg)) {
         if (psHdl->isCas) {
            for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
               if (pcKyw[j]!=psHlp->psStd->pcKyw[j]) { k++; }
            }
         } else {
            for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
               if (toupper(pcKyw[j])!=toupper(psHlp->psStd->pcKyw[j])) { k++; }
            }
         }
         if (k==0 && j>=psHlp->psStd->siKwl) {
            TRACE(psHdl->pfPrs,"FIND-SYMBOL1(KYW=%s(%s))\n",pcKyw,psHlp->psStd->pcKyw);
            return(psHlp);
         }
      }
   }
   return(NULL);
}

static const TsSym* psClpFndSym2(
   TsHdl*                        psHdl,
   const char*                   pcKyw,
   const TsSym*                  psTab)
{
   const TsSym*                  psHlp=NULL;
   if (psTab) {
      int i;
      for (i=0,psHlp=psTab;psHlp!=NULL && psHlp->psStd!=NULL;psHlp=psHlp->psBak,i++) {
         if (!CLPISF_LNK(psHlp->psStd->uiFlg)) {
            int j,k;
            if (psHdl->isCas) {
               for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
                  if (pcKyw[j]!=psHlp->psStd->pcKyw[j]) { k++; }
               }
            } else {
               for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
                  if (toupper(pcKyw[j])!=toupper(psHlp->psStd->pcKyw[j])) { k++; }
               }
            }
            if (k==0 && j>=psHlp->psStd->siKwl) {
               TRACE(psHdl->pfPrs,"FIND-SYMBOL2a(KYW=%s(%s))\n",pcKyw,psHlp->psStd->pcKyw);
               return(psHlp);
            }
         }
      }
      for (i=0,psHlp=psTab->psNxt;psHlp!=NULL && psHlp->psStd!=NULL;psHlp=psHlp->psNxt,i++) {
         if (!CLPISF_LNK(psHlp->psStd->uiFlg)) {
            int j,k;
            if (psHdl->isCas) {
               for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
                  if (pcKyw[j]!=psHlp->psStd->pcKyw[j]) { k++; }
               }
            } else {
               for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
                  if (toupper(pcKyw[j])!=toupper(psHlp->psStd->pcKyw[j])) { k++; }
               }
            }
            if (k==0 && j>=psHlp->psStd->siKwl) {
               TRACE(psHdl->pfPrs,"FIND-SYMBOL2b(KYW=%s(%s))\n",pcKyw,psHlp->psStd->pcKyw);
               return(psHlp);
            }
         }
      }
   }
   return(NULL);
}

static int siClpSymFnd(
   TsHdl*                        psHdl,
   const int                     siLev,
   const char*                   pcKyw,
   const TsSym*                  psTab,
   TsSym**                       ppArg,
   int*                          piElm)
{
   const TsSym*                  psHlp=NULL;
   int                           i,j,k,e;
   *ppArg=NULL;
   if (psTab==NULL) {
      CLPERR(psHdl,CLPERR_SYN,"Keyword '%s.%s' not valid",fpcPat(psHdl,siLev),pcKyw);
      CLPERRADD(psHdl,      1,"Unexpected end of path reached%s","");
      return(CLPERR_SYN);
   }
   if (psTab->psBak!=NULL) {
      CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (1)",fpcPat(psHdl,siLev),psTab->psStd->pcKyw);
      CLPERRADD(psHdl,      1,"Try to find keyword '%s' in this table",pcKyw);
      return(CLPERR_INT);
   }
   for (e=i=0,psHlp=psTab;psHlp!=NULL && psHlp->psStd!=NULL;psHlp=psHlp->psNxt,i++) {
      if (!CLPISF_LNK(psHlp->psStd->uiFlg)) {
         if (psHdl->isCas) {
            for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
               if (pcKyw[j]!=psHlp->psStd->pcKyw[j]) { k++; }
            }
         } else {
            for (k=j=0;k==0 && pcKyw[j]!=EOS && psHlp->psStd->pcKyw!=NULL;j++) {
               if (toupper(pcKyw[j])!=toupper(psHlp->psStd->pcKyw[j])) { k++; }
            }
         }
         if (k==0 && j>=psHlp->psStd->siKwl) {
            if (piElm!=NULL) { (*piElm)=e; }
            *ppArg=(TsSym*)psHlp;
            TRACE(psHdl->pfPrs,"%s FIND-SYMBOL3(LEV=%d ELM=%d IND=%d KYW=%s(%s))\n",fpcPre(psHdl,siLev),siLev,e,i,pcKyw,psHlp->psStd->pcKyw);
            return(i);
         }
         if (piElm!=NULL) {
            if (CLPISF_ARG(psHlp->psStd->uiFlg) || CLPISF_CON(psHlp->psStd->uiFlg)) {
               e++;
            }
         }
      }
   }
   CLPERR(psHdl,CLPERR_SYN,"Parameter '%s.%s' not valid",fpcPat(psHdl,siLev),pcKyw);
   CLPERRADD(psHdl,0,"Please use one of the following parameters:%s","");
   vdClpPrnArgTab(psHdl,psHdl->pfErr,1,-1,psTab);
   return(CLPERR_SYN);
}

static int siClpSymPrn(
   TsHdl*                        psHdl,
   int                           siLev,
   TsSym*                        psSym)
{
   TsSym*                        psHlp=psSym;
   while (psHlp!=NULL) {
      int siErr=siExtentSymTab(psHdl,siLev,psHlp);
      if (siErr<0) { return(siErr); }
      if (psHdl->pfSym!=NULL) {
         char acTs[24];
         efprintf(psHdl->pfSym,"%s %s - %s (KWL=%d TYP=%s MIN=%d MAX=%d SIZ=%d OFS=%d OID=%d FLG=%8.8X (NXT=%p BAK=%p DEP=%p HIH=%p ALI=%p CNT=%p OID=%p IND=%p ELN=%p SLN=%p TLN=%p LNK=%p)) - %s\n",
            cstime(0,acTs),fpcPre(psHdl,siLev),psHlp->psStd->pcKyw,psHlp->psStd->siKwl,pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->siMin,psHlp->psFix->siMax,psHlp->psFix->siSiz,
            psHlp->psFix->siOfs,psHlp->psFix->siOid,psHlp->psStd->uiFlg,psHlp->psNxt,psHlp->psBak,psHlp->psDep,psHlp->psHih,psHlp->psStd->psAli,psHlp->psFix->psCnt,psHlp->psFix->psOid,
            psHlp->psFix->psInd,psHlp->psFix->psEln,psHlp->psFix->psSln,psHlp->psFix->psTln,psHlp->psFix->psLnk,psHlp->psFix->pcHlp);
         fflush_unchecked(psHdl->pfSym);
      }
      if (psHlp->psDep!=NULL) {
         siErr=siClpSymPrn(psHdl,siLev+1,psHlp->psDep);
         if (siErr<0) { return(siErr); }
      }
      psHlp=psHlp->psNxt;
   }
   return(CLP_OK);
}

static int siClpSymTrc(
   TsHdl*                        psHdl)
{
   if (psHdl->pfSym!=NULL) {
      char acTs[24];
      fprintf(psHdl->pfSym,"%s BEGIN-SYMBOL-TABLE-TRACE\n",cstime(0,acTs));
      fflush_unchecked(psHdl->pfSym);
      int siErr=siClpSymPrn(psHdl,0,psHdl->psTab);
      if (siErr<0) { return(siErr); }
      fprintf(psHdl->pfSym,"%s END-SYMBOL-TABLE-TRACE\n",cstime(0,acTs));
      fflush_unchecked(psHdl->pfSym);
   }
   return(CLP_OK);
}

static void vdClpSymDel(
   TsSym*                        psSym,
   uint64_t*                     piCnt,
   uint64_t*                     piSiz)
{
   TsSym*                        psHlp=psSym;
   while (psHlp!=NULL) {
      if (psHlp->psStd!=NULL) {
         if (!CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psDep!=NULL) {
            vdClpSymDel(psHlp->psDep,piCnt,piSiz);
         }
         if (psHlp->psHih!=NULL) { // not really required
            psHlp->psHih->psDep=NULL;
         }
         if (psHlp->psNxt!=NULL) { // not really required
            psHlp->psNxt->psBak=NULL;
         }
         if (!CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psVar!=NULL) {
            (*piSiz)+=sizeof(TsVar);
            memset(psHlp->psVar,0,sizeof(TsVar));
            free(psHlp->psVar);
            psHlp->psVar=NULL;
         }
         if (!CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psFix!=NULL) {
            if (psHlp->psFix->pcPro!=NULL) {
               (*piSiz)+=strlen(psHlp->psFix->pcPro)+1;
               free(psHlp->psFix->pcPro);
            }
            if (psHlp->psFix->pcSrc!=NULL) {
               (*piSiz)+=strlen(psHlp->psFix->pcSrc)+1;
               free(psHlp->psFix->pcSrc);
            }
            (*piSiz)+=sizeof(TsFix);
            memset(psHlp->psFix,0,sizeof(TsFix));
            free(psHlp->psFix);
            psHlp->psFix=NULL;
         }
         (*piSiz)+=sizeof(TsStd);
         memset(psHlp->psStd,0,sizeof(TsStd));
         free(psHlp->psStd);
         psHlp->psStd=NULL;
      }
      TsSym* psOld=psHlp;
      psHlp=psHlp->psNxt;
      (*piSiz)+=sizeof(TsSym);
      memset(psOld,0,sizeof(TsSym));
      free(psOld);
      (*piCnt)++;
   }
}

/* Scanner ************************************************************/

#define STRCHR '\''
#define SPMCHR '\"'
#define ALTCHR C_GRV

extern int siClpLexemes(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   if (pfOut!=NULL) {
      TsHdl* psHdl=(TsHdl*)pvHdl;
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," COMMENT   '#' [:print:]* '#'                              (will be ignored)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCOMMENT  ';' [:print:]* 'nl'                             (will be ignored)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," OPERATOR '=' | '.' | '(' | ')' | '[' | ']' | (SGN, DOT, RBO, RBC, SBO, SBC)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"  '=>'| '+' | '-' | '*' | '/' | '{' | '}' (SAB, ADD, SUB, MUL, DIV, CBO,CBC)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," KEYWORD   ['-'['-']][:alpha:]+[:alnum: | '_']*          (always predefined)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," SELECTION ['-'['-']][:alpha:]+[:alnum: | '_' | '-' | '/']*     (predefined)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," NUMBER    ([+|-]  [ :digit:]+)  |                       (decimal (default))\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," num       ([+|-]0b[ :digit:]+)  |                                  (binary)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," num       ([+|-]0o[ :digit:]+)  |                                   (octal)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," num       ([+|-]0d[ :digit:]+)  |                                 (decimal)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," num       ([+|-]0x[ :xdigit:]+) |                             (hexadecimal)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," num       ([+|-]0t(yyyy/mm/tt.hh:mm:ss)) |  (relativ (+|-) or absolut time)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," FLOAT     ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) | (decimal(default))\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," flt       ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)            (decimal)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," STRING         ''' [:print:]* ''' |          (default (if binary c else s))\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," str       [s|S]''' [:print:]* ''' |                (null-terminated string)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," str       [c|C]''' [:print:]* ''' |  (binary string in local character set)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," str       [a|A]''' [:print:]* ''' |                (binary string in ASCII)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," str       [e|E]''' [:print:]* ''' |               (binary string in EBCDIC)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," str       [x|X]''' [:print:]* ''' |         (binary string in hex notation)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," str       [f|F]''' [:print:]* ''' | (read string from file (for passwords))\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"           Strings can contain two '' to represent one '                    \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"           Strings can also be enclosed in \" or %c instead of '              \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"           Strings can directly start behind a '=' without enclosing ('%c\")  \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"              In this case the string ends at the next separator or operator\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"              and keywords are preferred. To use keywords, separators or    \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"              operators in strings, enclosing quotes are required.          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                                                                            \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," The predefined constant keyword below can be used in a value expressions   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," NOW       NUMBER - current time in seconds since 1970 (+0t0000)            \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," MINUTE    NUMBER - minute in seconds (60)                                  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," HOUR      NUMBER - hour in seconds   (60*60)                               \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," DAY       NUMBER - day in seconds    (24*60*60)                            \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," YEAR      NUMBER - year in seconds   (365*24*60*60)                        \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," KiB       NUMBER - kilobyte          (1024)                                \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," MiB       NUMBER - megabyte          (1024*1024)                           \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GiB       NUMBER - gigabyte          (1024*1024*1024)                      \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," TiB       NUMBER - terrabyte         (1024*1024*1024*1024)                 \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," RNDn      NUMBER - simple random number with n * 8 bit in length (1,2,4,8) \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," PI        FLOAT  - PI (3.14159265359)                                      \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCSTAMP   STRING - current local stamp in format:           YYYYMMDD.HHMMSS\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCDATE    STRING - current local date in format:            YYYYMMDD       \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCYEAR    STRING - current local year in format:            YYYY           \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCYEAR2   STRING - current local year in format:            YY             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCMONTH   STRING - current local month in format:           MM             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCDAY     STRING - current local day in format:             DD             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCTIME    STRING - current local time in format:            HHMMSS         \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCHOUR    STRING - current local hour in format:            HH             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCMINUTE  STRING - current local minute in format:          MM             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCSECOND  STRING - current local second in format:          SS             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCOFFSET  STRING - difference to Greenwich mean time:    +/-HH             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," LCOFFABS  STRING - difference to GMT (absolute):             HH             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMSTAMP   STRING - current Greenwich mean stamp in format:  YYYYMMDD.HHMMSS\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMDATE    STRING - current Greenwich mean date in format:   YYYYMMDD       \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMYEAR    STRING - current Greenwich mean year in format:   YYYY           \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMYEAR2   STRING - current Greenwich mean year in format:   YY             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMMONTH   STRING - current Greenwich mean month in format:  MM             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMDAY     STRING - current Greenwich mean day in format:    DD             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMTIME    STRING - current Greenwich mean time in format:   HHMMSS         \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMHOUR    STRING - current Greenwich mean hour in format:   HH             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMMINUTE  STRING - current Greenwich mean minute in format: MM             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMSECOND  STRING - current Greenwich mean second in format: SS             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMOFFSET  STRING - difference to local time:             +/-HH             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," GMOFFABS  STRING - difference to local time (absolute):     HH             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," SnRND10   STRING - decimal random number of length n (1 to 8)              \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," SnRND16   STRING - hexadecimal random number of length n (1 to 8)          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                                                                            \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," SUPPLEMENT     '\"' [:print:]* '\"' |   (null-terminated string (properties))\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"           Supplements can contain two \"\" to represent one \"                \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"           Supplements can also be enclosed in ' or %c instead of \"          \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"           Supplements can also be enclosed in ' or %c instead of \"          \n",ALTCHR);
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," ENVIRONMENT VARIABLES '<'varnam'>' will replaced by the corresponding value  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," Escape sequences for critical punctuation characters on EBCDIC systems     \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '!' = '&EXC;'   - Exclamation mark                                      \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '$' = '&DLR;'   - Dollar sign                                           \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '#' = '&HSH;'   - Hashtag (number sign)                                 \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '@' = '&ATS;'   - At sign                                               \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '[' = '&SBO;'   - Square bracket open                                   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '\\' = '&BSL;'   - Backslash                                             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    ']' = '&SBC;'   - Square bracket close                                  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '^' = '&CRT;'   - Caret (circumflex)                                    \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '`' = '&GRV;'   - Grave accent                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '{' = '&CBO;'   - Curly bracket open                                    \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '|' = '&VBR;'   - Vertical bar                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '}' = '&CBC;'   - Curly bracket close                                   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '~' = '&TLD;'   - Tilde                                                 \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," Define CCSIDs for certain areas in CLP strings on EBCDIC systems (0-reset) \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '&' [:digit:]+ ';  (...\"&1047;get.file='&0;%%s&1047;'\",f)                \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," Escape sequences for hexadecimal byte values                               \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"    '&' ['X''x'] :xdigit: :xdigit: ';' (\"&xF5;\")                            \n");
   }
   return(CLP_OK);
}

static inline int isPrintF(const TsSym* p) { return (p != NULL) ? (CLPISF_PWD(p->psStd->uiFlg) == FALSE) : TRUE; }
static inline char* isPrnLex(const TsSym* p, char* l) { return (isPrintF(p)) ? l : "***SECRET***"; }
static inline int isPrintF2(const TsSym* p) { return (CLPISF_PWD(p->psStd->uiFlg)==FALSE); }
static inline const char* isPrnLex2(const TsSym* p, const char* l) { return (isPrintF2(p)) ? l : "***SECRET***"; }
static inline int isStringChr(int c) { return (c == STRCHR || c == SPMCHR || c == ALTCHR); }
static inline int isSeparation(int c) { return (isspace(c) || iscntrl(c) || c == ','); }
static inline int isReqStrOpr1(int c) { return (c == '=' || c == '+' || c == ')' || c == C_SBC || c == C_CBC); }// required string cannot start with this characters
static inline int isReqStrOpr2(int c) { return (c == ';' || c == C_HSH); }                                      // required string must end at this
static inline int isReqStrOpr3(int c) { return (isReqStrOpr1(c) || c == '(' || c == '.' || c == C_SBO || c == C_CBO || isStringChr(c)); }// required string must end at key word if one of this follows

#define LEX_REALLOC do {\
   if (pcLex>=(pcEnd-4)) {\
      intptr_t l=pcLex-(*ppLex);\
      intptr_t h=pcHlp-(*ppLex);\
      intptr_t z=(pcZro!=NULL)?pcZro-(*ppLex):0;\
      size_t s=(*pzLex)+CLPINI_LEXSIZ;\
      char*  b=(char*)realloc_nowarn(*ppLex,s);\
      if (b==NULL) { return CLPERR(psHdl,CLPERR_MEM,"Re-allocation of buffer in size %d to store the lexeme failed",(int)s); }\
      (*pzLex)=s;\
      if (b!=(*ppLex)) {\
         (*ppLex)=b;\
         pcLex=(*ppLex)+l;\
         pcHlp=(*ppLex)+h;\
         pcZro=(pcZro!=NULL)?(*ppLex)+z:NULL;\
      }\
      pcEnd=(*ppLex)+s;\
   }\
} while (0)

static int siClpConNat(
   TsHdl*                        psHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   size_t*                       pzLex,
   char**                        ppLex,
   const int                     siTyp,
   const TsSym*                  psArg)
{
   (void)pfErr;
   if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"NOW",0,0,FALSE)==0) {
      if (pzLex!=NULL && ppLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)psHdl->siNow));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
         if (psArg!=NULL) { psArg->psStd->uiFlg|=CLPFLG_TIM; }
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"MINUTE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)60));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"HOUR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)60)*((U64)60));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"DAY",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)24)*((U64)60)*((U64)60));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"YEAR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)365)*((U64)24)*((U64)60)*((U64)60));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"KiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"MiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024)*((U64)1024));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024)*((U64)1024)*((U64)1024));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"TiB",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%"PRIu64"",((U64)1024)*((U64)1024)*((U64)1024)*((U64)1024));
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND8",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         if (psHdl->siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%"PRIi64"",psHdl->siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%"PRIi64"",psHdl->siRnd);
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND4",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         I32   siRnd=(I32)psHdl->siRnd;
         if (siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%d",siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%d",siRnd);
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND2",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         I16   siRnd=(I16)psHdl->siRnd;
         if (siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%d",(I32)siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%d",(I32)siRnd);
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_NUMBER || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"RND1",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         I08   siRnd=(I08)psHdl->siRnd;
         if (siRnd>=0) {
            srprintf(ppLex,pzLex,24,"d+%d",(I32)siRnd);
         } else {
            srprintf(ppLex,pzLex,24,"d%d",(I32)siRnd);
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(NUM)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_NUM);
   } else if ((siTyp==CLPTYP_FLOATN || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"PI",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         srprintf(ppLex,pzLex,24,"d+%f",3.14159265359);
         for (char* p=*ppLex;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(FLT)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_FLT);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCSTAMP",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d.%H%M%S",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCDATE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCYEAR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCYEAR2",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%y",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCMONTH",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%m",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCDAY",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%d",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCTIME",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H%M%S",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCHOUR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCMINUTE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%M",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCSECOND",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%S",localtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCOFFSET",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm st;
         const time_t t=psHdl->siNow;
         const I64 lt=localtime_r(&t,&st)->tm_hour;
         const I64 gt=gmtime_r(&t,&st)->tm_hour;
         if (gt>=lt) {
            snprintf(*ppLex,*pzLex,"d'+%02d",(I32)(gt-lt));
         } else {
            snprintf(*ppLex,*pzLex,"d'-%02d",(I32)(lt-gt));
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"LCOFFABS",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm st;
         const time_t t=psHdl->siNow;
         const I64 lt=localtime_r(&t,&st)->tm_hour;
         const I64 gt=gmtime_r(&t,&st)->tm_hour;
         if (gt>lt) {
            snprintf(*ppLex,*pzLex,"d'%02d",(I32)(gt-lt));
         } else {
            snprintf(*ppLex,*pzLex,"d'%02d",(I32)(lt-gt));
         }
   TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMSTAMP",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d.%H%M%S",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMDATE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y%m%d",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMYEAR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%Y",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMYEAR2",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%y",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMMONTH",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%m",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMDAY",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%d",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMTIME",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H%M%S",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMHOUR",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%H",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMMINUTE",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%M",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMSECOND",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm   st;
         time_t      t=psHdl->siNow;
         strftime(*ppLex,*pzLex,"d'%S",gmtime_r(&t,&st));
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMOFFSET",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm st;
         const time_t t=psHdl->siNow;
         const I64 lt=localtime_r(&t,&st)->tm_hour;
         const I64 gt=gmtime_r(&t,&st)->tm_hour;
         if (gt>lt) {
            snprintf(*ppLex,*pzLex,"d'-%02d",(I32)(gt-lt));
         } else {
            snprintf(*ppLex,*pzLex,"d'+%02d",(I32)(lt-gt));
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"GMOFFABS",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         struct tm st;
         const time_t t=psHdl->siNow;
         const I64 lt=localtime_r(&t,&st)->tm_hour;
         const I64 gt=gmtime_r(&t,&st)->tm_hour;
         if (gt>lt) {
            snprintf(*ppLex,*pzLex,"d'%02d",(I32)(gt-lt));
         } else {
            snprintf(*ppLex,*pzLex,"d'%02d",(I32)(lt-gt));
         }
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S1RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%01u",((U32)psHdl->siRnd)%10);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S2RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%02u",((U32)psHdl->siRnd)%100);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S3RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%03u",((U32)psHdl->siRnd)%1000);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S4RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%04u",((U32)psHdl->siRnd)%10000);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S5RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%05u",((U32)psHdl->siRnd)%100000);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S6RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%06u",((U32)psHdl->siRnd)%1000000);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S7RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%07u",((U32)psHdl->siRnd)%10000000);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S8RND10",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%08u",((U32)psHdl->siRnd)%100000000);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S1RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%01x",((U32)psHdl->siRnd)&0x0000000F);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S2RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%02x",((U32)psHdl->siRnd)&0x000000FF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S3RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%03x",((U32)psHdl->siRnd)&0x00000FFF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S4RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%04x",((U32)psHdl->siRnd)&0x0000FFFF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S5RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%05x",((U32)psHdl->siRnd)&0x000FFFFF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S6RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%06x",((U32)psHdl->siRnd)&0x00FFFFFF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S7RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%07x",((U32)psHdl->siRnd)&0x0FFFFFFF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else if ((siTyp==CLPTYP_STRING || siTyp==-1) && strxcmp(psHdl->isCas,pcKyw,"S8RND16",0,0,FALSE)==0) {
      if (pzLex!=NULL) {
         psHdl->siRnd=ClpRndFnv((psHdl->siRnd+1)^flrand());
         srprintf(ppLex,pzLex,10,"d'%08x",((U32)psHdl->siRnd)&0xFFFFFFFF);
         TRACE(pfTrc,"CONSTANT-TOKEN(STR)-LEXEME(%s)\n",*ppLex);
      }
      return(CLPTOK_STR);
   } else {
      return(CLPTOK_KYW);
   }
}

static inline int siClpNxtOpr(
   const char*                   pcCur)
{
   while (1) {
      if (*pcCur==EOS) { /*end*/
         return(0x00);
      } else if (isSeparation(*pcCur)) { /*separation*/
         pcCur++;
      } else if (*pcCur==C_HSH) { /*comment*/
         pcCur++;
         while (*pcCur!=C_HSH && *pcCur!=EOS) {
            pcCur++;
         }
         if (*pcCur!=C_HSH) {
            return(0x00);
         }
         pcCur++;
      } else if (*pcCur==';') { /*line comment*/
         pcCur++;
         while (*pcCur!='\n' && *pcCur!=EOS) {
            pcCur++;
         }
      } else {
         return(*pcCur);
      }
   }
}

static inline int isClpKywTyp(
   TsHdl*                        psHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   const TsSym*                  psArg)
{
   const TsSym*                  psVal=NULL;
   if (psArg->psDep!=NULL) { // selection
      psVal=psClpFndSym(psHdl,pcKyw,psArg->psDep);
   }
   if (psVal==NULL && !CLPISF_SEL(psArg->psStd->uiFlg)) {
      psVal=psClpFndSym2(psHdl,pcKyw,psArg);
   }
   if (psVal!=NULL && psVal->psFix->siTyp==psArg->psFix->siTyp) {
      return(TRUE);
   }
   return(CLPTOK_KYW!=siClpConNat(psHdl,pfErr,pfTrc,pcKyw,NULL,NULL,psArg->psFix->siTyp,psArg));
}

static inline int isClpKywVal(
   TsHdl*                        psHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   const TsSym*                  psArg,
   const TsSym**                 ppVal)
{
   const TsSym*                  psVal=NULL;
   if (psArg!=NULL) {
      if (psArg->psDep!=NULL) { // selection
         psVal=psClpFndSym(psHdl,pcKyw,psArg->psDep);
      }
      if (psVal==NULL && !CLPISF_SEL(psArg->psStd->uiFlg)) {
         psVal=psClpFndSym2(psHdl,pcKyw,psArg);
      }
   }
   if (ppVal!=NULL) { *ppVal=psVal; }
   if (psVal!=NULL) { return(TRUE); }
   return(CLPTOK_KYW!=siClpConNat(psHdl,pfErr,pfTrc,pcKyw,NULL,NULL,-1,psArg));
}

static inline int isClpKywStr(
   TsHdl*                        psHdl,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcKyw,
   const TsSym*                  psArg,
   const TsSym**                 ppVal,
   const char                    scOpr)
{
   const TsSym*                  psVal=NULL;
   if (ppVal!=NULL) { *ppVal=NULL; }
   if (psArg!=NULL) {
      if (psArg->psDep!=NULL) { // selection
         psVal=psClpFndSym(psHdl,pcKyw,psArg->psDep);
      }
      if (psVal==NULL && !CLPISF_SEL(psArg->psStd->uiFlg)) {
         psVal=psClpFndSym2(psHdl,pcKyw,psArg);
      }
   }
   if (psVal!=NULL) {
      switch (scOpr) {
      case '(': // required string ends only if type is an object
         if (psVal->psFix->siTyp==CLPTYP_OBJECT) {
            if (ppVal!=NULL) { *ppVal=psVal; }
            return(TRUE);
         }
         break;
      case '.': // required string ends only if type is an overlay
         if (psVal->psFix->siTyp==CLPTYP_OVRLAY) {
            if (ppVal!=NULL) { *ppVal=psVal; }
            return(TRUE);
         }
         break;
      default: // at each other operator or separator the required string must end
         if (ppVal!=NULL) { *ppVal=psVal; }
         return(TRUE);
      }
   }
   return(CLPTOK_KYW!=siClpConNat(psHdl,pfErr,pfTrc,pcKyw,NULL,NULL,CLPTYP_STRING,psArg));
}

static inline int isClpKywAry(
   TsHdl*                        psHdl,
   const int                     siTok,
   const char*                   pcKyw,
   const TsSym*                  psArg,
   const TsSym**                 ppVal)
{
   const TsSym*                  psVal=NULL;
   if (ppVal!=NULL) {
      if (*ppVal!=NULL) {
         int siOpr=siClpNxtOpr(psHdl->pcCur);
         if ((*ppVal)->psFix->siTyp==psArg->psFix->siTyp && siOpr!='=' && siOpr!='(' && siOpr!='.' && siOpr!=C_SBO) {
            return(TRUE);
         } else {
            return(FALSE);
         }
      }
   }
   if (psArg!=NULL) {
      if (psArg->psDep!=NULL) { // selection
         psVal=psClpFndSym(psHdl,pcKyw,psArg->psDep);
      }
      if (psVal==NULL && !CLPISF_SEL(psArg->psStd->uiFlg)) {
         psVal=psClpFndSym2(psHdl,pcKyw,psArg);
      }
      if (psVal!=NULL) {
         if (ppVal!=NULL) { *ppVal=psVal; }
         int siOpr=siClpNxtOpr(psHdl->pcCur);
         if (psVal->psFix->siTyp==psArg->psFix->siTyp && siOpr!='=' && siOpr!='(' && siOpr!='.' && siOpr!=C_SBO) {
            return(TRUE);
         } else {
            return(FALSE);
         }
      }
      return(siTok==siClpConNat(psHdl,psHdl->pfErr,NULL,pcKyw,NULL,NULL,psArg->psFix->siTyp,psArg));
   }
   return(FALSE);
}

static char* pcClpUnEscape(
   TsHdl*                        psHdl,
   const char*                   pcInp)
{
   char* pcOut=(char*)pvClpAllocFlg(psHdl,NULL,strlen(pcInp)+1,NULL,CLPFLG_PWD);
   if (pcOut==NULL) { return(pcOut); }
   return(unEscape(pcInp,pcOut));
}

static int siClpScnNat(
   TsHdl*                        psHdl,
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
   char*                         pcLex=(*ppLex);
   char*                         pcHlp=(*ppLex);
   const char*                   pcEnd=(*ppLex)+(*pzLex);
   const char*                   pcCur=(*ppCur);
   int                           isEnv=psHdl->isEnv;
   const char*                   pcEnv=NULL;
   const char*                   pcOld=NULL;
   char*                         pcZro=NULL;
   time_t                        t;
   struct tm                     tm;
   struct tm                     stAkt;
   struct tm                     *tmAkt;
   char                          acHlp[1024];
   int                           siRbc=0;
   int                           siSbc=0;
   int                           siCbc=0;

   if (siTyp!=CLPTYP_NUMBER && siTyp!=CLPTYP_FLOATN && siTyp!=CLPTYP_STRING) { siTyp=0; }
   if (piSep!=NULL) { *piSep=FALSE; }
   if (ppVal!=NULL) { *ppVal=NULL; }

   while (1) {
      if (*(*ppCur)==EOS) { /*end*/
         pcLex[0]=EOS;
         TRACE(pfTrc,"SCANNER-TOKEN(END)-LEXEME(%s)\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_END);
      } else if (isSeparation(*(*ppCur))) { /*separation*/
         if (piSep!=NULL) { *piSep=(unsigned char)(*(*ppCur)); }
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
         while (*(*ppCur)!='\n' && *(*ppCur)!=EOS) { (*ppCur)++; }
         if (*(*ppCur)=='\n') {
            (*ppCur)++;
            psHdl->siRow++;
            psHdl->pcRow=(*ppCur);
         }
         psHdl->pcOld=(*ppCur);
      } else if (isEnv && *(*ppCur)=='<') { /*environment variable replacement*/
         (*ppCur)++;
         while ((*ppCur)[0]!=EOS && (*ppCur)[0]!='>') {
            LEX_REALLOC;
            *pcLex=*(*ppCur); pcLex++;
            (*ppCur)++;
         }
         *pcLex=EOS;
         if (*(*ppCur)!='>') {
            return CLPERR(psHdl,CLPERR_LEX,"Environment variable not terminated with '>'");
         }
         (*ppCur)++;
         pcLex=(*ppLex);
         pcEnv=getenvar(pcLex,0,sizeof(acHlp),acHlp);
         if (pcEnv!=NULL) {
            size_t l=pcCur-psHdl->pcInp;
            if (psHdl->siBuf>=CLPMAX_BUFCNT) {
               return CLPERR(psHdl,CLPERR_LEX,"Environment variable replacement (%s=%s) not possible (more than %d replacements)",pcLex,pcEnv,CLPMAX_BUFCNT);
            }
            srprintf(&psHdl->apBuf[psHdl->siBuf],&psHdl->pzBuf[psHdl->siBuf],l+strlen(pcEnv)+strlen((*ppCur)),"%.*s%s%s",(int)l,psHdl->pcInp,pcEnv,(*ppCur));
            TRACE(pfTrc,"SCANNER-ENVARREP\n%s %s\n%s %s\n",fpcPre(psHdl,0),psHdl->pcInp,fpcPre(psHdl,0),psHdl->apBuf[psHdl->siBuf]);
            (*ppCur)=psHdl->apBuf[psHdl->siBuf]+l;
            psHdl->pcInp=psHdl->apBuf[psHdl->siBuf];
            psHdl->pcOld=psHdl->apBuf[psHdl->siBuf]+(psHdl->pcOld-psHdl->pcInp);
            psHdl->pcRow=psHdl->apBuf[psHdl->siBuf]+(psHdl->pcRow-psHdl->pcInp);
            psHdl->siBuf++;
            isEnv=TRUE;
         } else {
            isEnv=FALSE;
            (*ppCur)=pcCur;
         }
      } else if (isStringChr((*ppCur)[0])) {/*simple string*/
         char USECHR=(*ppCur)[0];
         *pcLex= 'd'; pcLex++;
         *pcLex='\''; pcLex++;
         (*ppCur)++;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=USECHR || (*ppCur)[1]==USECHR)) {
            LEX_REALLOC;
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
         TRACE(pfTrc,"SCANNER-TOKEN(STR)-LEXEME(%s)-SIMPLE\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if ((tolower((*ppCur)[0])=='x' || tolower((*ppCur)[0])=='a' || tolower((*ppCur)[0])=='e'  ||
                  tolower((*ppCur)[0])=='c' || tolower((*ppCur)[0])=='s' || tolower((*ppCur)[0])=='f') &&
                  isStringChr((*ppCur)[1])) {/*defined string '...'*/
         char USECHR=(*ppCur)[1];
         *pcLex=tolower(*(*ppCur)); pcLex++;
         *pcLex='\''; pcLex++;
         (*ppCur)+=2;
         while ((*ppCur)[0]!=EOS && ((*ppCur)[0]!=USECHR || (*ppCur)[1]==USECHR)) {
            LEX_REALLOC;
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
         TRACE(pfTrc,"SCANNER-TOKEN(STR)-LEXEME(%s)-DEFINED\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if (((*ppCur)[0]=='-' && isalpha((*ppCur)[1])) || ((*ppCur)[0]=='-' && (*ppCur)[1]=='-' && isalpha((*ppCur)[2]))) { /*defined keyword*/
         while ((*ppCur)[0]=='-') {
            (*ppCur)++;
         }
         *pcLex=*(*ppCur);
         (*ppCur)++; pcLex++;
         while (isCst(*(*ppCur))) {
            LEX_REALLOC;
            if (!isKyw(*(*ppCur)) && pcOld==NULL) {
               pcOld=(*ppCur);
               pcZro=pcLex;
            }
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
         }
         *pcLex=EOS;
         // cppcheck-suppress objectIndex
         if(pcCur[0]=='-' && pcCur[1]!='-' && psArg!=NULL && psArg->psFix->siTyp==siTyp && isClpKywTyp(psHdl,pfErr,pfTrc,pcHlp,psArg)) {
            (*ppCur)=pcCur+1;
            pcLex[0]='-'; pcLex[1]=EOS;
            TRACE(pfTrc,"SCANNER-TOKEN(SUB)-LEXEME(%s)-NOKYW\n",pcHlp);
            return(CLPTOK_SUB);
         } else {
            if (pcOld!=NULL && !isClpKywVal(psHdl,pfErr,pfTrc,pcHlp,psArg,ppVal)) {
               if (pcZro!=NULL) *pcZro=0x00;
               *ppCur=pcOld;
            }
            TRACE(pfTrc,"SCANNER-TOKEN(KYW)-LEXEME(%s)-DEFINED\n",pcHlp);
            return(CLPTOK_KYW);
         }
      } else if (siTyp==CLPTYP_STRING && isStr((*ppCur)[0]) && !isReqStrOpr1((*ppCur)[0])) {/*required string*/
         if ((*ppCur)[0]=='(') {
            siRbc++;
         } else if ((*ppCur)[0]==C_SBO) {
            siSbc++;
         } else if ((*ppCur)[0]==C_CBO) {
            siCbc++;
         } else if ((*ppCur)[0]==')') {
            siRbc--;
         } else if ((*ppCur)[0]==C_SBC) {
            siSbc--;
         } else if ((*ppCur)[0]==C_CBC) {
            siCbc--;
         }
         *pcLex='d'; pcLex++;
         *pcLex='\''; pcLex++;
         if (psArg!=NULL && isalpha(*(*ppCur))) {
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            while (isCst(*(*ppCur))) {
               LEX_REALLOC;
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            if (isReqStrOpr3(*(*ppCur)) || isSeparation(*(*ppCur))) {
               if (isClpKywStr(psHdl,pfErr,pfTrc,(*ppLex)+2,psArg,ppVal,*(*ppCur))) {
                  char* p1=pcHlp;
                  const char* p2=(*ppLex)+2;
                  while (*p2) {
                     *p1++=*p2++;
                  }
                  *p1=EOS;
                  TRACE(pfTrc,"SCANNER-TOKEN(KYW)-LEXEME(%s)-REQSTR\n",pcHlp);
                  return(CLPTOK_KYW);
               }
            }
         }
         while ((*ppCur)[0]!=EOS && isStr((*ppCur)[0]) && !isSeparation((*ppCur)[0]) && !isReqStrOpr2((*ppCur)[0]) &&
                (siRbc>0 || (*ppCur)[0]!=')') && (siSbc>0 || (*ppCur)[0]!=C_SBC) && (siCbc>0 || (*ppCur)[0]!=C_CBC)) {
            if ((*ppCur)[0]=='(') {
               siRbc++;
            } else if ((*ppCur)[0]==C_SBO) {
               siSbc++;
            } else if ((*ppCur)[0]==C_CBO) {
               siCbc++;
            } else if ((*ppCur)[0]==')') {
               siRbc--;
            } else if ((*ppCur)[0]==C_SBC) {
               siSbc--;
            } else if ((*ppCur)[0]==C_CBC) {
               siCbc--;
            }
            LEX_REALLOC;
            *pcLex=*(*ppCur);
            pcLex++; (*ppCur)++;
         }
         *pcLex=EOS;
         TRACE(pfTrc,"SCANNER-TOKEN(STR)-LEXEME(%s)-REQUIRED\n",isPrnLex(psArg,pcHlp));
         return(CLPTOK_STR);
      } else if (isalpha((*ppCur)[0])) { /*simple keyword*/
         *pcLex=*(*ppCur);
         (*ppCur)++; pcLex++;
         while (isCst(*(*ppCur))) {
            LEX_REALLOC;
            if (!isKyw(*(*ppCur)) && pcOld==NULL) {
               pcOld=(*ppCur);
               pcZro=pcLex;
            }
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
         }
         *pcLex=EOS;
         if (pcOld!=NULL && !isClpKywVal(psHdl,pfErr,pfTrc,pcHlp,psArg,ppVal)) {
            if (pcZro!=NULL) *pcZro=0x00;
            *ppCur=pcOld;
         }
         TRACE(pfTrc,"SCANNER-TOKEN(KYW)-LEXEME(%s)-SIMPLE\n",pcHlp);
         return(CLPTOK_KYW);
      } else if ((((*ppCur)[0]=='+' || (*ppCur)[0]=='-') && isdigit((*ppCur)[1])) || isdigit((*ppCur)[0])) { /*number*/
         if ((*ppCur)[0]=='+' || (*ppCur)[0]=='-') {
            // cppcheck-suppress objectIndex
            pcLex[1]=(*ppCur)[0];
            (*ppCur)++;
            // cppcheck-suppress objectIndex
         } else { pcLex[1]=' '; }
         if (((*ppCur)[0]=='0') &&
             (tolower((*ppCur)[1])=='b' || tolower((*ppCur)[1])=='o' || tolower((*ppCur)[1])=='d' || tolower((*ppCur)[1])=='x' || tolower((*ppCur)[1])=='t') &&
             (isdigit((*ppCur)[2]) || (isxdigit((*ppCur)[2]) && tolower((*ppCur)[1])=='x'))) {
            pcLex[0]=tolower((*ppCur)[1]);
            (*ppCur)+=2;
         } else { pcLex[0]='d'; }
         pcLex+=2;
         if (pcHlp[0]=='x') {
            while (isxdigit(*(*ppCur))) {
               LEX_REALLOC;
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
         } else if (pcHlp[0]=='t') {
            memset(&tm,0,sizeof(tm));
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC;
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            tm.tm_mon=0;
            tm.tm_mday=0;
            tm.tm_hour=0;
            tm.tm_min=0;
            tm.tm_sec=0;
            tm.tm_year=strtoI32(pcHlp+2,10);
            if ((*ppCur)[0]=='/' && isdigit((*ppCur)[1])) {
               pcLex=pcHlp+2; (*ppCur)++;
               while (isdigit(*(*ppCur))) {
                  LEX_REALLOC;
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               *pcLex=EOS;
               tm.tm_mon=strtoI32(pcHlp+2,10);
               if ((*ppCur)[0]=='/' && isdigit((*ppCur)[1])) {
                  pcLex=pcHlp+2; (*ppCur)++;
                  while (isdigit(*(*ppCur))) {
                     LEX_REALLOC;
                     *pcLex=*(*ppCur);
                     (*ppCur)++; pcLex++;
                  }
                  *pcLex=EOS;
                  tm.tm_mday=strtoI32(pcHlp+2,10);
                  if ((*ppCur)[0]=='.' && isdigit((*ppCur)[1])) {
                     pcLex=pcHlp+2; (*ppCur)++;
                     while (isdigit(*(*ppCur))) {
                        LEX_REALLOC;
                        *pcLex=*(*ppCur);
                        (*ppCur)++; pcLex++;
                     }
                     *pcLex=EOS;
                     tm.tm_hour=strtoI32(pcHlp+2,10);
                     if ((*ppCur)[0]==':' && isdigit((*ppCur)[1])) {
                        pcLex=pcHlp+2; (*ppCur)++;
                        while (isdigit(*(*ppCur))) {
                           LEX_REALLOC;
                           *pcLex=*(*ppCur);
                           (*ppCur)++; pcLex++;
                        }
                        *pcLex=EOS;
                        tm.tm_min=strtoI32(pcHlp+2,10);
                        if ((*ppCur)[0]==':' && isdigit((*ppCur)[1])) {
                           pcLex=pcHlp+2; (*ppCur)++;
                           while (isdigit(*(*ppCur))) {
                              LEX_REALLOC;
                              *pcLex=*(*ppCur);
                              (*ppCur)++; pcLex++;
                           }
                           *pcLex=EOS;
                           tm.tm_sec=strtoI32(pcHlp+2,10);
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
               tmAkt=localtime_r(&t,&stAkt);
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
               tmAkt=localtime_r(&t,&stAkt);
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
               if (tm.tm_year>=1900) { tm.tm_year-=1900; }
               if (tm.tm_mon >=   1) { tm.tm_mon -=1; }
               if (tm.tm_mday==   0) { tm.tm_mday++; }
               t=mktime(&tm);
               if (t==-1) {
                  return CLPERR(psHdl,CLPERR_LEX,"The given time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d) cannot be converted to a number",
                                                 tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
               }
               if (tm.tm_isdst>0) { t-=60*60; }//correct daylight saving time
            }
            pcHlp[1]='+';
            const ssize_t szRst=pcEnd-(pcHlp+2);
            if (szRst>0) {
               int e=(snprintf)(pcHlp+2,szRst,"%"PRIu64"",(U64)t);
               if (e<0 || e>=szRst) {
                  return CLPERR(psHdl,CLPERR_LEX,"The given time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d (%"PRIu64")) cannot be stored (%d)",(U64)t,(I32)szRst,
                                                 tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
               }
            } else {
               return CLPERR(psHdl,CLPERR_LEX,"The given time value (0t%4.4d/%2.2d/%2.2d.%2.2d:%2.2d:%2.2d (%"PRIu64")) cannot be stored (%d)",(U64)t,(I32)szRst,
                                              tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
            }
            TRACE(pfTrc,"SCANNER-TOKEN(NUM)-LEXEME(%s)-TIME\n",isPrnLex(psArg,pcHlp));
            if (psArg!=NULL) { psArg->psStd->uiFlg|=CLPFLG_TIM; }
            return(CLPTOK_NUM);
         } else {
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC;
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
         }
         if (pcHlp[0]=='d' && (*ppCur)[0]=='.' && (isdigit((*ppCur)[1]))) { /*float*/
            *pcLex=*(*ppCur);
            (*ppCur)++; pcLex++;
            while (isdigit(*(*ppCur))) {
               LEX_REALLOC;
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
                  LEX_REALLOC;
                  *pcLex=*(*ppCur);
                  (*ppCur)++; pcLex++;
               }
               *pcLex=EOS;
               if (pcHlp[1]==' ') { pcHlp[1]='+'; }
               TRACE(pfTrc,"SCANNER-TOKEN(FLT)-LEXEME(%s)\n",isPrnLex(psArg,pcHlp));
               return(CLPTOK_FLT);
            }
            *pcLex=EOS;
            if (pcHlp[1]==' ') { pcHlp[1]='+'; }
            TRACE(pfTrc,"SCANNER-TOKEN(FLT)-LEXEME(%s)\n",isPrnLex(psArg,pcHlp));
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
               LEX_REALLOC;
               *pcLex=*(*ppCur);
               (*ppCur)++; pcLex++;
            }
            *pcLex=EOS;
            if (pcHlp[1]==' ') { pcHlp[1]='+'; }
            TRACE(pfTrc,"SCANNER-TOKEN(FLT)-LEXEME(%s)\n",isPrnLex(psArg,pcHlp));
            return(CLPTOK_FLT);
         } else {
            *pcLex=EOS;
            if (pcHlp[1]==' ') { pcHlp[1]='+'; }
            TRACE(pfTrc,"SCANNER-TOKEN(NUM)-LEXEME(%s)\n",isPrnLex(psArg,pcHlp));
            return((siTyp==CLPTOK_FLT)?CLPTOK_FLT:CLPTOK_NUM);
         }
      } else if (*(*ppCur)=='=') { /*sign or sign with agle breckets*/
         if ((*ppCur)[1]=='>') {
            // cppcheck-suppress objectIndex
            pcLex[0]='='; pcLex[1]='>'; pcLex[2]=EOS; (*ppCur)+=2;
            TRACE(pfTrc,"SCANNER-TOKEN(SAB)-LEXEME(%s)\n",pcHlp);
            return(CLPTOK_SAB);
         } else {
            // cppcheck-suppress objectIndex
            pcLex[0]='='; pcLex[1]=EOS; (*ppCur)++;
            TRACE(pfTrc,"SCANNER-TOKEN(SGN)-LEXEME(%s)\n",pcHlp);
            return(CLPTOK_SGN);
         }
      } else if (*(*ppCur)=='.') { /*dot*/
         // cppcheck-suppress objectIndex
         pcLex[0]='.'; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(DOT)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_DOT);
      } else if (*(*ppCur)=='+') { /*add*/
         // cppcheck-suppress objectIndex
         pcLex[0]='+'; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(ADD)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_ADD);
      } else if (*(*ppCur)=='-') { /*sub*/
         // cppcheck-suppress objectIndex
         pcLex[0]='-'; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(SUB)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_SUB);
      } else if (*(*ppCur)=='*') { /*mul*/
         // cppcheck-suppress objectIndex
         pcLex[0]='*'; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(MUL)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_MUL);
      } else if (*(*ppCur)=='/') { /*div*/
         // cppcheck-suppress objectIndex
         pcLex[0]='/'; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(DIV)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_DIV);
      } else if (*(*ppCur)=='(') { /*round bracket open*/
         // cppcheck-suppress objectIndex
         pcLex[0]='('; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(RBO)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_RBO);
      } else if (*(*ppCur)==')') { /*round bracket close*/
         // cppcheck-suppress objectIndex
         pcLex[0]=')'; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(RBC)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_RBC);
      } else if (*(*ppCur)==C_SBO) { /*squared bracket open*/
         // cppcheck-suppress objectIndex
         pcLex[0]=C_SBO; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(SBO)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_SBO);
      } else if (*(*ppCur)==C_SBC) { /*squared bracket close*/
         // cppcheck-suppress objectIndex
         pcLex[0]=C_SBC; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(SBC)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_SBC);
      } else if (*(*ppCur)==C_CBO) { /*curly bracket open*/
         // cppcheck-suppress objectIndex
         pcLex[0]=C_CBO; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(CBO)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_CBO);
      } else if (*(*ppCur)==C_CBC) { /*curly bracket close*/
         // cppcheck-suppress objectIndex
         pcLex[0]=C_CBC; pcLex[1]=EOS; (*ppCur)++;
         TRACE(pfTrc,"SCANNER-TOKEN(CBC)-LEXEME(%s)\n",pcHlp);
         return(CLPTOK_CBC);
      } else { /*lexical error*/
         pcLex[0]=EOS; (*ppCur)++;
         return CLPERR(psHdl,CLPERR_LEX,"Character ('%c') not valid",*((*ppCur)-1));
      }
   }
}


static int siClpConSrc(
   TsHdl*                        psHdl,
   const int                     isChk,
   const TsSym*                  psArg)
{
   return(siClpConNat(psHdl,psHdl->pfErr,psHdl->pfScn,psHdl->pcLex,(isChk)?NULL:&psHdl->szLex,(isChk)?NULL:&psHdl->pcLex,(psArg!=NULL)?psArg->psFix->siTyp:0,psArg));
}

static int siClpScnSrc(
   TsHdl*                        psHdl,
   int                           siTyp,
   const TsSym*                  psArg)
{
   psHdl->pcOld=psHdl->pcCur;
   return(siClpScnNat(psHdl,psHdl->pfErr,psHdl->pfScn,&psHdl->pcCur,&psHdl->szLex,&psHdl->pcLex,siTyp,psArg,&psHdl->isSep,&psHdl->psVal));
}

/**********************************************************************/

extern int siClpGrammar(
   void*                         pvHdl,
   FILE*                         pfOut)
{
   if (pfOut!=NULL) {
      TsHdl* psHdl=(TsHdl*)pvHdl;
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," Command Line Parser                                              \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," command        -> ['('] parameter_list [')']       (main=object) \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  ['.'] parameter                  (main=overlay)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," parameter_list -> parameter SEP parameter_list                   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," parameter      -> switch | assignment | object | overlay | array \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," switch         -> KEYWORD                                        \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," assignment     -> KEYWORD '=' value                              \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' SELECTION                          \n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=>' STRING # parameter file #         \n");
   }
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," object         -> KEYWORD ['('] parameter_list [')']             \n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' STRING # parameter file #          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=>' STRING # parameter file #         \n");
   }
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," overlay        -> KEYWORD ['.'] parameter                        \n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' STRING # parameter file #          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=>' STRING # parameter file #         \n");
   }
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," array          -> KEYWORD '[' value_list   ']'                   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '[' object_list  ']'                   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '[' overlay_list ']'                   \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '=' value_list # with certain limitations #\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," It is recommended to use only enclosed array lists to know the end\n");
   if (psHdl->isPfl) {
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '[=' STRING ']' # parameter file #     \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '[=>' STRING ']' # parameter file #    \n");
   }
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," value_list     -> value SEP value_list                           \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," object_list    -> object SEP object_list                         \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," overlay_list   -> overlay SEP overlay_list                       \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," A list of objects requires parenthesis to enclose the arguments  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                                                                  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," value          -> term '+' value                                 \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  term '-' value                                 \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  term                                           \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," term           -> factor '*' term                                \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  factor '/' term                                \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  factor                                         \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," factor         -> NUMBER | FLOAT | STRING                        \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  selection | variable | constant                \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  '(' value ')'                                  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," selection      -> KEYWORD # value from a selection table        #\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," variable       -> KEYWORD # value from a previous assignment    #\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD '{' NUMBER '}' # with index for arrays#\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," constant       -> KEYWORD # see predefined constants at lexeme  #\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," For strings only the operator '+' is implemented as concatenation\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," Strings without an operator in between are also concatenated     \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," A number followed by a constant is a multiplication (4KiB=4*1024)\n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                                                                  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," Property File Parser                                             \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," properties     -> property_list                                  \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," property_list  -> property SEP property_list                     \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  EMPTY                                          \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," property       -> keyword_list '=' SUPPLEMENT                    \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," keyword_list   -> KEYWORD '.' keyword_list                       \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut,"                |  KEYWORD                                        \n");
      fprintf(pfOut,"%s",fpcPre(psHdl,0)); efprintf(pfOut," SUPPLEMENT is a string in double quotation marks (\"property\")    \n");
   }
   return(CLP_OK);
}

static int siClpPrsParLst(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsSym*                  psTab)
{
   int                           siPos=0;
   while (psHdl->siTok==CLPTOK_KYW) {
      int siErr=siClpPrsPar(psHdl,siLev,siPos,psTab,FALSE,NULL);
      if (siErr<0) { return(siErr); }
      siPos++;
   }
   return(siPos);
}

static int siClpPrsPar(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const TsSym*                  psTab,
   const int                     isOvl,
   int*                          piOid)
{
   TsSym*                        psArg=NULL;

   if (psHdl->siTok==CLPTOK_KYW) {
      int                        siErr;
      char                       acKyw[CLPMAX_KYWSIZ];
      strlcpy(acKyw,psHdl->pcLex,sizeof(acKyw));
      siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,NULL);
      if (siErr<0) { return(siErr); }
      siErr=siClpBldLnk(psHdl,siLev,siPos,(psHdl->pcOld-psHdl->pcInp),psArg->psFix->psInd,TRUE);
      if (siErr<0) { return(siErr); }
      if (piOid!=NULL) { *piOid=psArg->psFix->siOid; }
      if (isOvl) { // stop parsing if object id for main overlay known
         psHdl->siTok=CLPTOK_END;
         return(CLP_OK);
      }
      psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      if (psHdl->siTok==CLPTOK_SGN) {
         if (psArg->psFix->siTyp==CLPTYP_OBJECT || psArg->psFix->siTyp==CLPTYP_OVRLAY) {
            if (psHdl->isPfl) {
               if (psHdl->isPfl==2) {
                  return(siClpAcpFil(psHdl,siLev,siPos,FALSE,psArg));
               } else {
                  return(siClpPrsFil(psHdl,siLev,siPos,FALSE,psArg));
               }
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Parameter files not allowed (%s.?)",fpcPat(psHdl,siLev));
               return(CLPERR_SEM);
            }
         } else {
            return(siClpPrsSgn(psHdl,siLev,siPos,psArg));
         }
      } else if (psHdl->siTok==CLPTOK_SAB) {
         if (psHdl->isPfl) {
            if (psHdl->isPfl==2) {
               return(siClpAcpFil(psHdl,siLev,siPos,FALSE,psArg));
            } else {
               return(siClpPrsFil(psHdl,siLev,siPos,FALSE,psArg));
            }
         } else {
            CLPERR(psHdl,CLPERR_SEM,"Parameter files not allowed (%s.?)",fpcPat(psHdl,siLev));
            return(CLPERR_SEM);
         }
      } else if (psHdl->siTok==CLPTOK_RBO) {
         return(siClpPrsObj(psHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
         if (psHdl->siTok<0) { return(psHdl->siTok); }
         return(siClpPrsOvl(psHdl,siLev,siPos,psArg));
      } else if (psHdl->siTok==CLPTOK_SBO) {
         return(siClpPrsAry(psHdl,siLev,siPos,psArg));
      } else {
         if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
            return(siClpPrsObjWob(psHdl,siLev,siPos,psArg));
         } else if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
            return(siClpPrsOvl(psHdl,siLev,siPos,psArg));
         } else if (psArg->psFix->siTyp==CLPTYP_NUMBER && CLPISF_DEF(psArg->psStd->uiFlg)) {
            return(siClpPrsNum(psHdl,siLev,siPos,psArg));
         } else {
            return(siClpPrsSwt(psHdl,siLev,siPos,psArg));
         }
      }
   } else {
      CLPERR(psHdl,CLPERR_SYN,"Keyword instead token (%s(%s)) expected (%s.?)",pcMapClpTok(psHdl->siTok),psHdl->pcLex,fpcPat(psHdl,siLev));
      CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
      vdClpPrnArgTab(psHdl,psHdl->pfErr,1,-1,psTab);
      return(CLPERR_SYN);
   }
}

static int siClpPrsNum(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d NUM(%s) USING OID AS DEFAULT VALUE)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   return(siClpBldNum(psHdl,siLev,siPos,psArg));
}

static int siClpPrsSwt(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SWT(%s))\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   return(siClpBldSwt(psHdl,siLev,siPos,psArg));
}

static int siClpPrsSgn(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SGN(%s=val)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   if (psArg->psFix->siMax>1) { // ARRAY
      switch (psArg->psFix->siTyp) {
      case CLPTYP_NUMBER: return(siClpPrsValLstOnlyArg(psHdl,siLev,FALSE,CLPTOK_NUM,psArg));
      case CLPTYP_FLOATN: return(siClpPrsValLstOnlyArg(psHdl,siLev,FALSE,CLPTOK_FLT,psArg));
      case CLPTYP_STRING: return(siClpPrsValLstOnlyArg(psHdl,siLev,FALSE,CLPTOK_STR,psArg));
      default:
         return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter '%s.%s' is not supported with arrays",psArg->psFix->siTyp,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
   } else {
      return(siClpPrsVal(psHdl,siLev,siPos,FALSE,psArg));
   }
}

static int siClpAcpFil(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg)
{
   const char*                   pcPat=fpcPat(psHdl,siLev);
   (void)isAry;
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d PARFIL(%s=val)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(psHdl,CLPTYP_STRING,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   if (psHdl->siTok!=CLPTOK_STR) {
      return CLPERR(psHdl,CLPERR_SYN,"After object/overlay/array assignment '%s.%s=' parameter file ('filename') expected",pcPat,psArg->psStd->pcKyw);
   }
   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg))+strlen(isPrnLex2(psArg,psHdl->pcLex)),"%s.%s=%s\n",pcPat,GETKYW(psArg),isPrnLex2(psArg,psHdl->pcLex));
   psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   return(CLP_OK);
}

static int siClpPrsFil(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg)
{
   char                          acSrc[strlen(psHdl->pcSrc)+1];
   char*                         pcPar=NULL;
   int                           siRow,siCnt,siErr,siSiz=0;
   const char*                   pcCur;
   const char*                   pcInp;
   const char*                   pcOld;
   const char*                   pcRow;
   char                          acMsg[1024]="";

   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d PARFIL(%s=val)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(psHdl,CLPTYP_STRING,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   if (psHdl->siTok!=CLPTOK_STR) {
      return CLPERR(psHdl,CLPERR_SYN,"After object/overlay/array assignment '%s.%s=' parameter file ('filename') expected",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
   }

   char acFil[strlen(psHdl->pcLex)];
   strcpy(acFil,psHdl->pcLex+2);
   siErr=psHdl->pfF2s(psHdl->pvGbl,psHdl->pvF2s,acFil,&pcPar,&siSiz,acMsg,sizeof(acMsg));
   if (siErr<0) {
      siErr=CLPERR(psHdl,CLPERR_SYS,"Parameter file: %s",acMsg);
      SAFE_FREE(pcPar);
      return(siErr);
   }

   TRACE(psHdl->pfPrs,"PARAMETER-FILE-PARSER-BEGIN(FILE=%s)\n",acFil);
   strcpy(acSrc,psHdl->pcSrc);
   srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_PAF)+strlen(acFil),"%s%s",CLPSRC_PAF,acFil);

   pcInp=psHdl->pcInp; psHdl->pcInp=pcClpUnEscape(psHdl,pcPar);
   SAFE_FREE(pcPar);
   if (psHdl->pcInp==NULL) {
      siErr=CLPERR(psHdl,CLPERR_MEM,"Un-escaping of parameter file (%s) failed",acFil);
      return(siErr);
   }
   pcCur=psHdl->pcCur; psHdl->pcCur=psHdl->pcInp;
   pcOld=psHdl->pcOld; psHdl->pcOld=psHdl->pcInp;
   pcRow=psHdl->pcRow; psHdl->pcRow=psHdl->pcInp;
   siRow=psHdl->siRow; psHdl->siRow=1;
   psHdl->siBuf++;
   psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   if (isAry) {
      switch (psArg->psFix->siTyp) {
      case CLPTYP_NUMBER: siCnt=siClpPrsValLstFlexible(psHdl,siLev,CLPTOK_NUM,psArg); break;
      case CLPTYP_FLOATN: siCnt=siClpPrsValLstFlexible(psHdl,siLev,CLPTOK_FLT,psArg); break;
      case CLPTYP_STRING: siCnt=siClpPrsValLstFlexible(psHdl,siLev,CLPTOK_STR,psArg); break;
      case CLPTYP_OBJECT: siCnt=siClpPrsObjLst(psHdl,siLev,psArg); break;
      case CLPTYP_OVRLAY: siCnt=siClpPrsOvlLst(psHdl,siLev,psArg); break;
      default:
         return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter '%s.%s' is not supported with arrays",psArg->psFix->siTyp,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      if (siCnt<0) { return(siCnt); }
      psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
   } else {
      if (psHdl->siTok==CLPTOK_RBO) {
         siCnt=siClpPrsObj(psHdl,siLev,siPos,psArg);
         if (siCnt<0) { return(siCnt); }
      } else if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
         if (psHdl->siTok<0) {
            return(psHdl->siTok);
         }
         siCnt=siClpPrsOvl(psHdl,siLev,siPos,psArg);
         if (siCnt<0) { return(siCnt); }
      } else {
         if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
            siCnt=siClpPrsObjWob(psHdl,siLev,siPos,psArg);
            if (siCnt<0) { return(siCnt); }
         } else if(psArg->psFix->siTyp==CLPTYP_OVRLAY) {
            siCnt=siClpPrsOvl(psHdl,siLev,siPos,psArg);
            if (siCnt<0) { return(siCnt); }
         } else {
            if (psArg->psFix->siMax>1) { // ARRAY
               switch (psArg->psFix->siTyp) {
               case CLPTYP_NUMBER: siCnt=siClpPrsValLstOnlyArg(psHdl,siLev,TRUE,CLPTOK_NUM,psArg); break;
               case CLPTYP_FLOATN: siCnt=siClpPrsValLstOnlyArg(psHdl,siLev,TRUE,CLPTOK_FLT,psArg); break;
               case CLPTYP_STRING: siCnt=siClpPrsValLstOnlyArg(psHdl,siLev,TRUE,CLPTOK_STR,psArg); break;
               default:
                  return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter '%s.%s' is not supported with arrays",psArg->psFix->siTyp,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
               }
            } else {
               siCnt=siClpPrsVal(psHdl,siLev,siPos,isAry,psArg);
               if (siCnt<0) { return(siCnt); }
            }
         }
      }
   }
   if (psHdl->siTok==CLPTOK_END) {
      psHdl->siBuf--;
      psHdl->pcLex[0]=EOS;
      strcpy(psHdl->pcSrc,acSrc);
      psHdl->pcInp=pcInp; psHdl->pcCur=pcCur; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
      TRACE(psHdl->pfPrs,"PARAMETER-FILE-PARSER-END(FILE=%s CNT=%d)\n",acFil,siCnt);
      psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      return(siCnt);
   } else {
      return(CLPERR(psHdl,CLPERR_SYN,"Last token (%s(%s)) of parameter file '%s' is not EOF",pcMapClpTok(psHdl->siTok),psHdl->pcLex,acFil));
   }
}

static int siClpPrsObjWob(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   int                           siErr,siCnt;
   TsSym*                        psDep=NULL;
   TsVar                         asSav[CLPMAX_TABCNT];

   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d OBJ(%s(parlst))-OPN)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   siErr=siClpIniObj(psHdl,siLev,siPos,psArg,&psDep,asSav);
   if (siErr<0) { return(siErr); }
   siCnt=siClpPrsParLst(psHdl,siLev+1,psDep);
   if (siCnt<0) { return(siCnt); }
   siErr=siClpFinObj(psHdl,siLev,siPos,psArg,psDep,asSav);
   if (siErr<0) { return(siErr); }
   return(siCnt);
}

static int siClpPrsObj(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   int                           siCnt;

   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d OBJ(%s(parlst))-OPN)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   siCnt=siClpPrsObjWob(psHdl,siLev,siPos,psArg);
   if (siCnt<0) { return(siCnt); }
   if (psHdl->siTok==CLPTOK_RBC) {
      psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d OBJ(%s(parlst))-CLS)\n",fpcPre(psHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw);
   } else {
      if (psHdl->siTok==CLPTOK_STR) {
         return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing to enclose object (%s) - String (%s) given -> possible wrong keyword",fpcPat(psHdl,siLev),psHdl->pcLex);
      } else {
         return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing to enclose object (%s) - Token (%s(%s)) don't match expectations",fpcPat(psHdl,siLev),pcMapClpTok(psHdl->siTok),psHdl->pcLex);
      }
   }
   return(CLP_OK);
}

static int siClpPrsOvl(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   int                           siErr,siCnt,siOid=0;
   TsSym*                        psDep=NULL;
   TsVar                         asSav[CLPMAX_TABCNT];
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d OVL(%s.par)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw);
   siErr=siClpIniOvl(psHdl,siLev,siPos,psArg,&psDep,asSav);
   if (siErr<0) { return(siErr); }
   siCnt=siClpPrsPar(psHdl,siLev+1,siPos,psDep,FALSE,&siOid);
   if (siCnt<0) { return(siCnt); }
   siErr=siClpFinOvl(psHdl,siLev,siPos,psArg,psDep,asSav,siOid);
   if (siErr<0) { return(siErr); }
   return(CLP_OK);
}

static int siClpPrsMain(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   const int                     isOvl,
   int*                          piOid)
{
   int                           siErr,siOid;
   TsVar                         asSav[CLPMAX_TABCNT];
   if (psHdl->isOvl) {
      TRACE(psHdl->pfPrs,"%s PARSER(OVL(MAIN.par)\n",fpcPre(psHdl,0));
      if (psHdl->siTok==CLPTOK_DOT) {
         psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
         if (psHdl->siTok<0) { return(psHdl->siTok); }
      }
      siErr=siClpIniMainOvl(psHdl,psTab,asSav);
      if (siErr<0) { return(siErr); }
      siErr=siClpPrsPar(psHdl,0,0,psTab,isOvl,&siOid);
      if (siErr<0) { return(siErr); }
      siErr=siClpFinMainOvl(psHdl,psTab,asSav,siOid);
      if (siErr<0) { return(siErr); }
      if (piOid!=NULL) { (*piOid)=siOid; }
      return(1);
   } else {
      int siCnt;
      TRACE(psHdl->pfPrs,"%s PARSER(OBJ(MAIN(parlst)-OPN)\n",fpcPre(psHdl,0));
      siErr=siClpIniMainObj(psHdl,psTab,asSav);
      if (siErr<0) { return(siErr); }
      if (psHdl->siTok==CLPTOK_RBO) {
         psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
         if (psHdl->siTok<0) { return(psHdl->siTok); }
         siCnt=siClpPrsParLst(psHdl,0,psTab);
         if (siCnt<0) { return(siCnt); }
         if (psHdl->siTok==CLPTOK_RBC) {
            psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
            if (psHdl->siTok<0) { return(psHdl->siTok); }
         } else {
            if (psHdl->siTok==CLPTOK_STR) {
               return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing to enclose object (%s) - String (%s) given -> possible wrong keyword","***MAIN***",psHdl->pcLex);
            } else {
               return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing to enclose object (%s) - Token (%s(%s)) don't match expectations","***MAIN***",pcMapClpTok(psHdl->siTok),psHdl->pcLex);
            }
         }
      } else {
         siCnt=siClpPrsParLst(psHdl,0,psTab);
         if (siCnt<0) { return(siCnt); }
      }
      siErr=siClpFinMainObj(psHdl,psTab,asSav);
      if (siErr<0) { return(siErr); }
      TRACE(psHdl->pfPrs,"%s PARSER(OBJ(MAIN(parlst))-CLS)\n",fpcPre(psHdl,0));
      return(siCnt);
   }
}

static int siClpPrsAry(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   int                           siCnt;
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ARY(%s%ctyplst%c)-OPN)\n",fpcPre(psHdl,siLev),siLev,siPos,psArg->psStd->pcKyw,C_SBO,C_SBC);
   psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
   if (psHdl->siTok<0) { return(psHdl->siTok); }
   if (psHdl->siTok==CLPTOK_SGN || psHdl->siTok==CLPTOK_SAB) {
      if (psHdl->isPfl) {
         if (psHdl->isPfl==2) {
            siCnt=siClpAcpFil(psHdl,siLev,siPos,TRUE,psArg);
         } else {
            siCnt=siClpPrsFil(psHdl,siLev,siPos,TRUE,psArg);
         }
         if (siCnt<0) { return(siCnt); }
      } else {
         CLPERR(psHdl,CLPERR_SEM,"Parameter files not allowed (%s.?)",fpcPat(psHdl,siLev));
         return(CLPERR_SEM);
      }
   } else {
      switch (psArg->psFix->siTyp) {
      case CLPTYP_NUMBER: siCnt=siClpPrsValLstFlexible(psHdl,siLev,CLPTOK_NUM,psArg); break;
      case CLPTYP_FLOATN: siCnt=siClpPrsValLstFlexible(psHdl,siLev,CLPTOK_FLT,psArg); break;
      case CLPTYP_STRING: siCnt=siClpPrsValLstFlexible(psHdl,siLev,CLPTOK_STR,psArg); break;
      case CLPTYP_OBJECT: siCnt=siClpPrsObjLst(psHdl,siLev,psArg); break;
      case CLPTYP_OVRLAY: siCnt=siClpPrsOvlLst(psHdl,siLev,psArg); break;
      default:
         return CLPERR(psHdl,CLPERR_SEM,"Type (%d) of parameter '%s.%s' is not supported with arrays",psArg->psFix->siTyp,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      if (siCnt<0) { return(siCnt); }
   }
   if (psHdl->siTok==CLPTOK_SBC) {
      psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d CNT=%d ARY(%s%ctyplst%c)-CLS)\n",fpcPre(psHdl,siLev),siLev,siPos,siCnt,psArg->psStd->pcKyw,C_SBO,C_SBC);
      return(CLP_OK);
   } else {
      if (psHdl->siTok==CLPTOK_STR) {
         return CLPERR(psHdl,CLPERR_SYN,"Character '%c' missing to enclose the array (%s) - String (%s) given -> possible wrong keyword",C_SBC,fpcPat(psHdl,siLev),psHdl->pcLex);
      } else {
         return CLPERR(psHdl,CLPERR_SYN,"Character '%c' missing to enclose the array (%s) - Token (%s(%s)) don't match expectations",C_SBC,fpcPat(psHdl,siLev),pcMapClpTok(psHdl->siTok),psHdl->pcLex);
      }
   }
}

static int siClpPrsValLstFlexible(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siTok,
   TsSym*                        psArg)
{
   int                           siPos=0;
   while (psHdl->siTok==siTok || psHdl->siTok==CLPTOK_KYW || psHdl->siTok==CLPTOK_RBO) {
      int siErr=siClpPrsVal(psHdl,siLev,siPos,TRUE,psArg);
      if (siErr<0) { return(siErr); }
      siPos++;
   }
   return(siPos);
}

static int siClpPrsValLstOnlyArg(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     isEnd,
   const int                     siTok,
   TsSym*                        psArg)
{
   int                           siPos=0;
   while ((isEnd || siPos==0 || psHdl->isSep==',' || psHdl->isSep=='\n') &&
          (psHdl->siTok==siTok || (psHdl->siTok==CLPTOK_KYW && isClpKywAry(psHdl,siTok,psHdl->pcLex,psArg,&psHdl->psVal)))) {
      int siErr=siClpPrsVal(psHdl,siLev,siPos,TRUE,psArg);
      if (siErr<0) { return(siErr); }
      siPos++;
   }
   return(siPos);
}

static int siClpPrsObjLst(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg)
{
   int                           siPos=0;
   while (psHdl->siTok==CLPTOK_RBO) {
      int siErr=siClpPrsObj(psHdl,siLev,siPos,psArg);
      if (siErr<0) { return(siErr); }
      siPos++;
   }
   return(siPos);
}

static int siClpPrsOvlLst(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg)
{
   int                           siPos=0;
   while (psHdl->siTok==CLPTOK_KYW) {
      int siErr=siClpPrsOvl(psHdl,siLev,siPos,psArg);
      if (siErr<0) { return(siErr); }
      siPos++;
   }
   return(siPos);
}
/**********************************************************************/

static int siFromNumberLexeme(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg,
   const char*                   pcVal,
   I64*                          piVal)
{
   char*                         pcHlp=NULL;

   switch (pcVal[0]) {
   case 'b': {
      errno=0;
      unsigned long long int l=strtoull(pcVal+1,&pcHlp, 2);
      if ((l==ULLONG_MAX && errno==ERANGE)) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      (*piVal)=l;
   } break;
   case 'o': {
      errno=0;
      unsigned long long int l=strtoull(pcVal+1,&pcHlp, 8);
      if ((l==ULLONG_MAX && errno==ERANGE)) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      (*piVal)=l;
   } break;
   case 'd': {
      errno=0;
      long long int l=strtoll(pcVal+1,&pcHlp,10);
      if ((l==LLONG_MIN || l==LLONG_MAX) && errno==ERANGE) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      (*piVal)=l;
   } break;
   case 'x': {
      errno=0;
      unsigned long long int l=strtoull(pcVal+1,&pcHlp,16);
      if ((l==ULLONG_MAX && errno==ERANGE)) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      (*piVal)=l;
   } break;
   case 't': {
      errno=0;
      long long int l=strtoll(pcVal+1,&pcHlp,10);
      if ((l==LLONG_MIN || l==LLONG_MAX) && errno==ERANGE) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      (*piVal)=l;
   } break;
   default:
      return CLPERR(psHdl,CLPERR_SEM,"Base (%c(0x%02X)) of number literal (%s.%s=%s) not supported",pcVal[0],pcVal[0],fpcPat(psHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcVal+1));
   }
   if (pcHlp==NULL || pcHlp[0]) {
      if (pcHlp==NULL) {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (end pointer is NULL)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (rest: %s)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcHlp));
      }
   }
   return(CLP_OK);
}

static int siFromFloatLexeme(
   TsHdl*                        psHdl,
   const int                     siLev,
   TsSym*                        psArg,
   const char*                   pcVal,
   double*                       pfVal)
{
   char*                         pcHlp=NULL;

   switch (pcVal[0]) {
   case 'd': {
      errno=0;
      double d=strtod(pcVal+1,&pcHlp);
      if (d==HUGE_VAL && errno==ERANGE) {
         return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }
      if (pcHlp!=NULL && *pcHlp=='.') {
         char* p=pcHlp;
         *p=',';
         errno=0;
         d=strtod(pcVal+1,&pcHlp);
         if (d==HUGE_VAL && errno==ERANGE) {
            return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (limits achieved)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         *p='.';
      }
      *pfVal=d;
   } break;
   default: return CLPERR(psHdl,CLPERR_SEM,"Base (%c(0x%02X)) of floating point literal (%s.%s=%s) not supported",pcVal[0],pcVal[0],fpcPat(psHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcVal+1));
   }
   if (pcHlp==NULL || pcHlp[0]) {
      if (pcHlp==NULL) {
         return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (end pointer is NULL)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      } else {
         return CLPERR(psHdl,CLPERR_SEM,"Floating number (%s) of '%s.%s' cannot be converted to a valid 64 bit value (rest: %s)",isPrnStr(psArg,pcVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw,isPrnStr(psArg,pcHlp));
      }
   }
   return(CLP_OK);
}

static int siClpPrsExp(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal);

static int siClpPrsFac(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal)
{
   const TsSym*                  psVal=psHdl->psVal;
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
      psHdl->siTok=siClpScnSrc(psHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      srprintf(ppVal,pzVal,strlen(acLex),"%s",acLex);
      TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d NUM/FLT/STR(%s))\n",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
      return(CLP_OK);
   case CLPTOK_KYW:
      siInd=0;
      if (siClpNxtOpr(psHdl->pcCur)==C_CBO) {
         psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
         if (psHdl->siTok<0) { return(psHdl->siTok); }
         if (psHdl->siTok==CLPTOK_CBO) {
            psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
            if (psHdl->siTok<0) { return(psHdl->siTok); }
            if (psHdl->siTok!=CLPTOK_NUM) {
               return CLPERR(psHdl,CLPERR_SYN,"Index number expected (%s)",fpcPat(psHdl,siLev));
            }
            siErr=siFromNumberLexeme(psHdl,siLev,psArg,psHdl->pcLex,&siInd);
            if (siErr<0) { return(siErr); }
            psHdl->siTok=siClpScnSrc(psHdl,0,psArg);
            if (psHdl->siTok<0) { return(psHdl->siTok); }
            if (psHdl->siTok!=CLPTOK_CBC) {
               return CLPERR(psHdl,CLPERR_SYN,"Character '%c' missing to define the index (%s)",C_CBC,fpcPat(psHdl,siLev));
            }
         }
      }
      psHdl->siTok=siClpScnSrc(psHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      if (psVal==NULL) { // not already find in scanner
         psVal=psClpFndSym(psHdl,acLex,psArg->psDep);
         if (psVal==NULL) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
               vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
               return(CLPERR_SEM);
            }
            psVal=psClpFndSym2(psHdl,acLex,psArg);
         }
      }
      if (psVal!=NULL) {
         if (psArg->psFix->siTyp!=psVal->psFix->siTyp) {
            return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of the symbol (%s) for argument '%s.%s' don't match the expected type (%s)",
                  pcMapClpTyp(psVal->psFix->siTyp),acLex,fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
         }
         if (psVal->psVar->pvDat==NULL || psVal->psVar->siCnt==0) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) and type (%s) of variable value for argument (%s.%s) defined but data pointer not set (variable not yet defined)",
                  psVal->psStd->pcKyw,pcMapClpTyp(psVal->psFix->siTyp),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         if (siInd<0 || siInd>=psVal->psVar->siCnt) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s) and type (%s) of variable value for argument (%s.%s) defined but data element counter (%d) too small (index (%d) not valid)",
                  psVal->psStd->pcKyw,pcMapClpTyp(psVal->psFix->siTyp),fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psVal->psVar->siCnt,(int)siInd);
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
                  psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
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
                  psVal->psFix->siSiz,psVal->psStd->pcKyw,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
            }
            if (flVal>=0.0) {
               srprintf(ppVal,pzVal,24,"d+%f",flVal);
            } else {
               srprintf(ppVal,pzVal,24,"d%f",flVal);
            }
            for (char* p=*ppVal;*p;p++) {
               if (*p==',') { *p='.'; }
            }
            break;
         case CLPTYP_STRING:
            if (siInd>0) {
               char* pcDat=pvDat;
               const char* pcEnd=pcDat+psVal->psVar->siLen;
               if (CLPISF_FIX(psVal->psStd->uiFlg)) {
                  pcDat+=siInd*psVal->psFix->siSiz;
               } else {
                  for (int j=0;pcDat<pcEnd && j<siInd;pcDat++) {
                     if (*pcDat==0x00) { j++; }
                  }
               }
               if (pcDat>=pcEnd) {
                  return CLPERR(psHdl,CLPERR_TAB,"Array access with index %d is not possible for this string argument (%s.%s)",(int)siInd,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
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
                  psArg->psFix->siTyp,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d KYW-CON(%s) TAB)\n",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
      } else {
         siErr=siClpConNat(psHdl,psHdl->pfErr,psHdl->pfScn,acLex,pzVal,ppVal,psArg->psFix->siTyp,psArg);
         if (siErr==CLPTOK_KYW) {
            return CLPERR(psHdl,CLPERR_SYN,"Keyword (%s) not valid in expression of type %s for argument %s.%s",
                  acLex,pcMapClpTyp(psArg->psFix->siTyp),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d KYW-CON(%s) FIX)\n",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
      }
      return(CLP_OK);
   case CLPTOK_RBO:
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      siErr=siClpPrsExp(psHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
      if (siErr<0) { return(siErr); }
      if (psHdl->siTok==CLPTOK_RBC) {
         psHdl->siTok=siClpScnSrc(psHdl,(isAry)?psArg->psFix->siTyp:0,psArg);
         if (psHdl->siTok<0) { return(psHdl->siTok); }
         return(CLP_OK);
      } else {
         if (psHdl->siTok==CLPTOK_STR) {
            return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing to enclose expression (%s) - String (%s) given -> possible wrong keyword",fpcPat(psHdl,siLev),psHdl->pcLex);
         } else {
            return CLPERR(psHdl,CLPERR_SYN,"Character ')' missing to enclose expression (%s) - Token (%s(%s)) don't match expectations",fpcPat(psHdl,siLev),pcMapClpTok(psHdl->siTok),psHdl->pcLex);
         }
      }
   default://Empty string behind = and in front of the next not matching token
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      srprintf(ppVal,pzVal,0,"d'");
      TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d NUM/FLT/STR(%s))\n",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
      return(CLP_OK);
   }
}

static int siClpPrsTrm(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal)
{
   int                           siErr;
   I64                           siVal1=0;
   I64                           siVal2=0;
   I64                           siVal=0;
   F64                           flVal1=0;
   F64                           flVal2=0;
   F64                           flVal=0;
   siErr=siClpPrsFac(psHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
   if (siErr<0) { return(siErr); }
   if (psHdl->siTok==CLPTOK_MUL) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
      siErr=siClpPrsTrm(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr<0) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,*ppVal,&siVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         siVal=siVal1*siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d MUL-NUM(%"PRIi64"*%"PRIi64"=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,siVal1,siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,*ppVal,&flVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         flVal=flVal1*flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         for (char* p=*ppVal;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d MUL-FLT(%f*%f=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Multiplication not supported for type %s",pcMapClpTyp(psArg->psFix->siTyp));
      }
      free(pcVal);
   } else if (psHdl->siTok==CLPTOK_DIV) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
      siErr=siClpPrsTrm(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr<0) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,*ppVal,&siVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         if (siVal2==0) {
            free(pcVal);
            return CLPERR(psHdl,CLPERR_SEM,"Devision by zero",pcMapClpTyp(psArg->psFix->siTyp));
         }
         siVal=siVal1/siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d DIV-NUM(%"PRIi64"/%"PRIi64"=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,siVal1,siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,*ppVal,&flVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         if (flVal2==0) {
            free(pcVal);
            return CLPERR(psHdl,CLPERR_SEM,"Devision by zero",pcMapClpTyp(psArg->psFix->siTyp));
         }
         flVal=flVal1/flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         for (char* p=*ppVal;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d DIV-FLT(%f/%f=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Division not supported for type %s",pcMapClpTyp(psArg->psFix->siTyp));
      }
      free(pcVal);
   } else if (psHdl->siTok==CLPTOK_KYW && psHdl->isSep==FALSE && CLPTOK_KYW!=siClpConSrc(psHdl,TRUE,psArg)) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
      siErr=siClpPrsTrm(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr<0) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,*ppVal,&siVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         siVal=siVal1*siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-MUL-NUM(%"PRIi64"*%"PRIi64"=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,siVal1,siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,*ppVal,&flVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         flVal=flVal1*flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         for (char* p=*ppVal;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-MUL-FLT(%f*%f=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
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
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-STR(%s+",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
         srprintc(ppVal,pzVal,strlen(pcVal),"%s",pcVal+2);
         TRACE(psHdl->pfPrs,"%s=%s))\n",pcVal,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Multiplication not supported for type %s",pcMapClpTyp(psArg->psFix->siTyp));
      }
      free(pcVal);
   }
   return(CLP_OK);
}

static int siClpPrsExp(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg,
   size_t*                       pzVal,
   char**                        ppVal)
{
   int                           siErr;
   I64                           siVal1=0;
   I64                           siVal2=0;
   I64                           siVal=0;
   F64                           flVal1=0;
   F64                           flVal2=0;
   F64                           flVal=0;
   siErr=siClpPrsTrm(psHdl,siLev,siPos,isAry,psArg,pzVal,ppVal);
   if (siErr<0) { return(siErr); }
   if (psHdl->siTok==CLPTOK_ADD) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
      siErr=siClpPrsExp(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr<0) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,*ppVal,&siVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         siVal=siVal1+siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-NUM(%"PRIi64"+%"PRIi64"=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,siVal1,siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,*ppVal,&flVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         flVal=flVal1+flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         for (char* p=*ppVal;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-FLT(%f+%f=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      case CLPTYP_STRING:
         if (pcVal[1]!=STRCHR) {
            siErr=CLPERR(psHdl,CLPERR_SEM,"The provided value (%s) is not a string literal",pcVal);
            free(pcVal);
            return(siErr);
         }
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
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-STR(%s+",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
         srprintc(ppVal,pzVal,strlen(pcVal),"%s",pcVal+2);
         TRACE(psHdl->pfPrs,"%s=%s))\n",pcVal,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Addition not supported for type %s",pcMapClpTyp(psArg->psFix->siTyp));
      }
      free(pcVal);
   } else if (psHdl->siTok==CLPTOK_SUB) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
      siErr=siClpPrsExp(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr<0) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,*ppVal,&siVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         siVal=siVal1-siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SUB-NUM(%"PRIi64"-%"PRIi64"=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,siVal1,siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,*ppVal,&flVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         flVal=flVal1-flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         for (char* p=*ppVal;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d SUB-FLT(%f-%f=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Subtracation not supported for type %s",pcMapClpTyp(psArg->psFix->siTyp));
      }
      free(pcVal);
   } else if ((psHdl->siTok==CLPTOK_NUM || psHdl->siTok==CLPTOK_FLT || psHdl->siTok==CLPTOK_STR) && psHdl->isSep==FALSE) {
      if (CLPISF_SEL(psArg->psStd->uiFlg)) {
         CLPERR(psHdl,CLPERR_SEM,"The argument '%s.%s' requires one of the defined keywords as value",fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         CLPERRADD(psHdl,0,"Please use one of the following arguments:%s","");
         vdClpPrnArgTab(psHdl,psHdl->pfErr,1,psArg->psFix->siTyp,psArg->psDep);
         return(CLPERR_SEM);
      }
      size_t szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
      char*  pcVal=(char*)calloc(1,szVal);
      if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
      siErr=siClpPrsExp(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
      if (siErr<0) { free(pcVal); return(siErr); }
      switch(psArg->psFix->siTyp) {
      case CLPTYP_NUMBER:
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,*ppVal,&siVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         siVal=siVal1+siVal2;
         if (siVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%"PRIi64"",siVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%"PRIi64"",siVal);
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-ADD-NUM(%"PRIi64"+%"PRIi64"=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,siVal1,siVal2,*ppVal);
         break;
      case CLPTYP_FLOATN:
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,*ppVal,&flVal1);
         if (siErr<0) { free(pcVal); return(siErr); }
         siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal2);
         if (siErr<0) { free(pcVal); return(siErr); }
         flVal=flVal1+flVal2;
         if (flVal>=0) {
            srprintf(ppVal,pzVal,24,"d+%f",flVal);
         } else {
            srprintf(ppVal,pzVal,24,"d%f",flVal);
         }
         for (char* p=*ppVal;*p;p++) {
            if (*p==',') { *p='.'; }
         }
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d AUTO-ADD-FLT(%f+%f=%s))\n",fpcPre(psHdl,siLev),siLev,siPos,flVal1,flVal2,*ppVal);
         break;
      case CLPTYP_STRING:
         if (pcVal[1]!=STRCHR) {
            siErr=CLPERR(psHdl,CLPERR_SEM,"The provided value (%s) is not a string literal",pcVal);
            free(pcVal);
            return(siErr);
         }
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
         TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d ADD-STR(%s+",fpcPre(psHdl,siLev),siLev,siPos,*ppVal);
         srprintc(ppVal,pzVal,strlen(pcVal),"%s",pcVal+2);
         TRACE(psHdl->pfPrs,"%s=%s))\n",pcVal,*ppVal);
         break;
      default:
         free(pcVal);
         return CLPERR(psHdl,CLPERR_SEM,"Addition not supported for type %s",pcMapClpTyp(psArg->psFix->siTyp));
      }
      free(pcVal);
   }
   return(CLP_OK);
}

static int siClpPrsVal(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const int                     isAry,
   TsSym*                        psArg)
{
   int                           siInd;
   size_t                        szVal=strlen(psHdl->pcLex)+CLPINI_VALSIZ;
   char*                         pcVal=(char*)malloc(szVal);
   if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store expression values failed")); }
   psHdl->apPat[siLev]=psArg;
   siInd=siClpPrsExp(psHdl,siLev,siPos,isAry,psArg,&szVal,&pcVal);
   if (siInd<0) { free(pcVal); return(siInd); }
   TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(psHdl,siLev),siLev,siPos,isPrnLex(psArg,pcVal));
   siInd=siClpBldLit(psHdl,siLev,siPos,psArg,pcVal);
   free(pcVal);
   return(siInd);
}

/**********************************************************************/

static int siClpPrsProLst(
   TsHdl*                        psHdl,
   const TsSym*                  psTab)
{
   int                           siPos=0;
   while (psHdl->siTok==CLPTOK_KYW) {
      int siErr=siClpPrsPro(psHdl,psTab);
      if (siErr<0) { return(siErr); }
      siPos++;
   }
   return(siPos);
}

static int siClpPrsPro(
   TsHdl*                        psHdl,
   const TsSym*                  psTab)
{
   size_t                        szPat=CLPINI_PATSIZ;
   char*                         pcPat=(char*)calloc(1,szPat);
   int                           siErr,siLev;
   if (pcPat==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store the path failed")); }
   siLev=siClpPrsKywLst(psHdl,&szPat,&pcPat);
   if (siLev<0) {
      free(pcPat);
      return(siLev);
   }
   if (psHdl->siTok==CLPTOK_SGN) {
      int siRow=psHdl->siRow;
      psHdl->siTok=siClpScnSrc(psHdl,CLPTYP_STRING,psTab);
      if (psHdl->siTok<0) {
         free(pcPat);
         return(psHdl->siTok);
      }
      if (psHdl->siTok==CLPTOK_STR) {
         char acSup[strlen(psHdl->pcLex)];
         strcpy(acSup,psHdl->pcLex+2);
         psHdl->siTok=siClpScnSrc(psHdl,0,psTab);
         if (psHdl->siTok<0) {
            free(pcPat);
            return(psHdl->siTok);
         }
         siErr=siClpBldPro(psHdl,pcPat,acSup,siRow);
         free(pcPat);
         return(siErr);
      } else {
         siErr=CLPERR(psHdl,CLPERR_SYN,"Property string (\"...\") missing (%s) - Token (%s(%s)) don't match expectations",pcPat,pcMapClpTok(psHdl->siTok),psHdl->pcLex);
         free(pcPat);
         return(siErr);
      }
   } else {
      siErr=CLPERR(psHdl,CLPERR_SYN,"Assignment character ('=') missing (%s) - Token (%s(%s)) don't match expectations",pcPat,pcMapClpTok(psHdl->siTok),psHdl->pcLex);
      free(pcPat);
      return(siErr);
   }
}

static int siClpPrsKywLst(
   TsHdl*                        psHdl,
   size_t*                       pzPat,
   char**                        ppPat)
{
   int                           siLev=0;

   (*ppPat)[0]=EOS;
   while (psHdl->siTok==CLPTOK_KYW) {
      srprintc(ppPat,pzPat,strlen(psHdl->pcLex),"%s",psHdl->pcLex);
      psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
      if (psHdl->siTok<0) { return(psHdl->siTok); }
      if (psHdl->siTok==CLPTOK_DOT) {
         srprintc(ppPat,pzPat,0,".");
         psHdl->siTok=siClpScnSrc(psHdl,0,NULL);
         if (psHdl->siTok<0) { return(psHdl->siTok); }
      }
      siLev++;
   }
   return(siLev);
}

/**********************************************************************/

static int siClpBldPro(
   TsHdl*                        psHdl,
   const char*                   pcPat,
   const char*                   pcPro,
   const int                     siRow)
{
   const TsSym*                  psTab=psHdl->psTab;
   TsSym*                        psArg=NULL;
   const char*                   pcPtr=NULL;
   const char*                   pcKyw=NULL;
   int                           l=strlen(psHdl->pcOwn)+strlen(psHdl->pcPgm)+strlen(psHdl->pcCmd)+2;
   char                          acRot[l+1];
   snprintf(acRot,sizeof(acRot),"%s.%s.%s",psHdl->pcOwn,psHdl->pcPgm,psHdl->pcCmd);

   if (strxcmp(psHdl->isCas,acRot,pcPat,l,0,FALSE)==0) {
      int siLev;
      if (pcPat[l]!='.') {
         return CLPERR(psHdl,CLPERR_SEM,"Property path (%s) is not valid",pcPat);
      }
      for (siLev=0,pcPtr=pcPat+l;pcPtr!=NULL && siLev<CLPMAX_HDEPTH && psTab!=NULL;pcPtr=strchr(pcPtr+1,'.'),siLev++) {
         int i,siErr;
         char acKyw[CLPMAX_KYWSIZ];
         for (pcKyw=pcPtr+1,i=0;i<CLPMAX_KYWLEN && pcKyw[i]!=EOS && pcKyw[i]!='.';i++) { acKyw[i]=pcKyw[i]; }
         acKyw[i]=EOS;
         siErr=siClpSymFnd(psHdl,siLev,acKyw,psTab,&psArg,NULL);
         if (siErr<0) { return(siErr); }
         if (psArg!=NULL) {
            siErr=siExtentSymTab(psHdl,siLev,psArg);
            if (siErr<0) { return(siErr); }
            psTab=psArg->psDep;
         } else {
            psTab=NULL;
         }
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
            TRACE(psHdl->pfBld,"BUILD-PROPERTY %s=\"%s\"\n",pcPat,isPrnStr(psArg,pcPro));
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
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   const I64                     siVal,
   TsSym*                        psArg,
   const int                     isApp)
{
   const int                     siTyp=CLPTYP_NUMBER;
   (void)siPos;

   if (psArg!=NULL) {
      if (isApp==FALSE) {
         psArg->psVar->pvPtr=psArg->psVar->pvDat;
         psArg->psVar->siCnt=0;
         psArg->psVar->siLen=0;
         psArg->psVar->siRst=(CLPISF_DYN(psArg->psStd->uiFlg))?0:psArg->psFix->siSiz;
         if (CLPISF_FIX(psArg->psStd->uiFlg)) { psArg->psVar->siRst*=psArg->psFix->siMax; }
      }

      if (psArg->psFix->siTyp!=siTyp) {
         return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of link '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
      }
      if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
         return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of link '%s.%s' with type '%s'",psArg->psFix->siMax,fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
      }
      if (psArg->psVar->pvDat==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of link are defined but data pointer not set",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
      }

      if (CLPISF_DYN(psArg->psStd->uiFlg)) {
         void** ppDat=(void**)psArg->psVar->pvDat;
         (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
         if ((*ppDat)==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d*%d)) for link '%s.%s' failed",psArg->psVar->siLen,(CLPISF_DLM(psArg->psStd->uiFlg))?2:1,psArg->psFix->siSiz,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
      } else {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for link '%s.%s' with type '%s'",psArg->psVar->siRst,fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
         }
         if (psArg->psVar->pvPtr==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of link are defined but write pointer not set",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
         }
      }

      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<(-128) || siVal>255) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRIi64") for link '%s.%s' need more than 8 Bit",isPrnInt(psArg,siVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LINK-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 2:
         if (siVal<(-32768) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRIi64") for link '%s.%s' need more than 16 Bit",isPrnInt(psArg,siVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LINK-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 4:
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            return CLPERR(psHdl,CLPERR_SEM,"Internal number (%"PRIi64") for link '%s.%s' need more than 32 Bit",isPrnInt(psArg,siVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
         }
         *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LINK-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 8:
         *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LINK-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRIi64") of link '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,siVal),fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
      }

      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
      psArg->psVar->siCnt++;

      return(psArg->psFix->siTyp);
   }
   return(CLP_OK);
}

static int siClpBldSwt(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   const int                     siTyp=CLPTYP_SWITCH;
   const char*                   pcPat=fpcPat(psHdl,siLev);
   int                           siErr;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of switch '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of switch '%s.%s' with type '%s'",psArg->psFix->siMax,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
   }
   if (psArg->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of switch defined but data pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }

   if (psHdl->pfSaf!=NULL) {
      char acEnt[strlen(pcPat)+strlen(GETKYW(psArg))+2];
      snprintf(acEnt,sizeof(acEnt),"%s.%s",pcPat,GETKYW(psArg));
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,acEnt)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",acEnt);
      }
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d*%d)) for switch '%s.%s' failed",psArg->psVar->siLen,(CLPISF_DLM(psArg->psStd->uiFlg))?2:1,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for switch '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
      }
      if (psArg->psVar->pvPtr==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of switch are defined but write pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
      }
  }

   switch (psArg->psFix->siSiz) {
   case 1:
      if (psArg->psFix->siOid<(-128) || psArg->psFix->siOid>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRIi64") of '%s.%s' need more than 8 Bit",isPrnInt(psArg,psArg->psFix->siOid),pcPat,psArg->psStd->pcKyw);
      }
      *((I08*)psArg->psVar->pvPtr)=(I08)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-SWITCH-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   case 2:
      if (psArg->psFix->siOid<(-32768) || psArg->psFix->siOid>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Object identifier (%"PRIi64") of '%s.%s' need more than 16 Bit",isPrnInt(psArg,psArg->psFix->siOid),pcPat,psArg->psStd->pcKyw);
      }
      *((I16*)psArg->psVar->pvPtr)=(I16)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-SWITCH-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   case 4:
      *((I32*)psArg->psVar->pvPtr)=(I32)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-SWITCH-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   case 8:
      *((I64*)psArg->psVar->pvPtr)=(I64)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-SWITCH-64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   default:
      return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRIi64") of '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,psArg->psFix->siOid),pcPat,psArg->psStd->pcKyw);
   }

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg)),"%s.%s=ON\n",pcPat,GETKYW(psArg));

   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) { return(siErr); }
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) { return(siErr); }
   }
   return(psArg->psFix->siTyp);
}

static int siClpBldNum(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   const int                     siTyp=CLPTYP_NUMBER;
   const char*                   pcPat=fpcPat(psHdl,siLev);
   int                           siErr;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of argument '%s.%s' with type '%s'",psArg->psFix->siMax,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
   }
   if (psArg->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but data pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }

   if (psHdl->pfSaf!=NULL) {
      char acEnt[strlen(pcPat)+strlen(GETKYW(psArg))+2];
      snprintf(acEnt,sizeof(acEnt),"%s.%s",pcPat,GETKYW(psArg));
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,acEnt)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",acEnt);
      }
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d*%d)) for argument '%s.%s' failed (1)",psArg->psVar->siLen,(CLPISF_DLM(psArg->psStd->uiFlg))?2:1,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
      }
      if (psArg->psVar->pvPtr==NULL) {
         return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but write pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
      }
  }

   switch (psArg->psFix->siSiz) {
   case 1:
      if (psArg->psFix->siOid<(-128) || psArg->psFix->siOid>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Default value (%"PRIi64") of '%s.%s' need more than 8 Bit",isPrnInt(psArg,psArg->psFix->siOid),pcPat,psArg->psStd->pcKyw);
      }
      *((I08*)psArg->psVar->pvPtr)=(I08)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-NUMBER-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   case 2:
      if (psArg->psFix->siOid<(-32768) || psArg->psFix->siOid>65535) {
         return CLPERR(psHdl,CLPERR_SEM,"Default value (%"PRIi64") of '%s.%s' need more than 16 Bit",isPrnInt(psArg,psArg->psFix->siOid),pcPat,psArg->psStd->pcKyw);
      }
      *((I16*)psArg->psVar->pvPtr)=(I16)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-NUMBER-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   case 4:
      *((I32*)psArg->psVar->pvPtr)=(I32)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-NUMBER-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   case 8:
      *((I64*)psArg->psVar->pvPtr)=(I64)psArg->psFix->siOid;
      TRACE(psHdl->pfBld,"%s BUILD-NUMBER-64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                              fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,psArg->psFix->siOid));
      break;
   default:
      return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%"PRIi64") of '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnInt(psArg,psArg->psFix->siOid),pcPat,psArg->psStd->pcKyw);
   }

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg))+16,"%s.%s=DEFAULT(%d)\n",pcPat,GETKYW(psArg),psArg->psFix->siOid);

   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) { return(siErr); }
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) { return(siErr); }
   }
   return(psArg->psFix->siTyp);
}

static int siClpBldLit(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   const char*                   pcVal)
{
   int                           siErr;
   int                           l0=0;
   int                           l1=0;
   int                           l2=0;
   int                           siSln=0;
   I64                           siVal=0;
   F64                           flVal=0;
   char*                         pcHlp=NULL;
   const char*                   pcKyw=NULL;
   const char*                   pcPat=fpcPat(psHdl,siLev);
   TsSym*                        psCon;
   C08                           acTim[CSTIME_BUFSIZ];

   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of argument '%s.%s' with type '%s'",psArg->psFix->siMax,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psVar->pvDat==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but data pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }

   if (psHdl->pfSaf!=NULL) {
      char acEnt[strlen(pcPat)+strlen(GETKYW(psArg))+2];
      snprintf(acEnt,sizeof(acEnt),"%s.%s",pcPat,GETKYW(psArg));
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,acEnt)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",acEnt);
      }
   }

   switch (psArg->psFix->siTyp) {
   case CLPTYP_SWITCH:
   case CLPTYP_NUMBER:
      if (CLPISF_DYN(psArg->psStd->uiFlg)) {
         void** ppDat=(void**)psArg->psVar->pvDat;
         (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
         if ((*ppDat)==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d*%d)) for argument '%s.%s' failed (2)",psArg->psVar->siLen,(CLPISF_DLM(psArg->psStd->uiFlg))?2:1,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
      } else {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
         }
         if (psArg->psVar->pvPtr==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
         }
      }
      siErr=siFromNumberLexeme(psHdl,siLev,psArg,pcVal,&siVal);
      if (siErr<0) { return(siErr); }
      switch (psArg->psFix->siSiz) {
      case 1:
         if (siVal<0 && CLPISF_UNS(psArg->psStd->uiFlg)) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' is negative (%"PRIi64") but marked as unsigned",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw,siVal);
         }
         if (siVal<(-128) || siVal>255) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' need more than 8 Bit",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw);
         }
         *((I08*)psArg->psVar->pvPtr)=(I08)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LITERAL-I08(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 2:
         if (siVal<0 && CLPISF_UNS(psArg->psStd->uiFlg)) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' is negative (%"PRIi64") but marked as unsigned",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw,siVal);
         }
         if (siVal<(-32768) || siVal>65535) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' need more than 16 Bit",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw);
         }
         *((I16*)psArg->psVar->pvPtr)=(I16)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LITERAL-I16(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 4:
         if (siVal<0 && CLPISF_UNS(psArg->psStd->uiFlg)) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' is negative (%"PRIi64") but marked as unsigned",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw,siVal);
         }
         if (siVal<(-2147483648LL) || siVal>4294967295LL) {
            return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' need more than 32 Bit",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw);
         }
         *((I32*)psArg->psVar->pvPtr)=(I32)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LITERAL-I32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      case 8:
         *((I64*)psArg->psVar->pvPtr)=(I64)siVal;
         TRACE(psHdl->pfBld,"%s BUILD-LITERAL-I64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%"PRIi64"\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnInt(psArg,siVal));
         break;
      default:
         return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%s) of '%s.%s' is not 1, 2, 4 or 8)",psArg->psFix->siSiz,isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
      siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) { return(siErr); }
      break;
   case CLPTYP_FLOATN:
      if (CLPISF_DYN(psArg->psStd->uiFlg)) {
         void** ppDat=(void**)psArg->psVar->pvDat;
         (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
         if ((*ppDat)==NULL) {
            return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d*%d)) for argument '%s.%s' failed (3)",psArg->psVar->siLen,(CLPISF_DLM(psArg->psStd->uiFlg))?2:1,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
         }
         psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
      } else {
         if (psArg->psVar->siRst<psArg->psFix->siSiz) {
            return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
         }
         if (psArg->psVar->pvPtr==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
         }
      }
      siErr=siFromFloatLexeme(psHdl,siLev,psArg,pcVal,&flVal);
      if (siErr<0) { return(siErr); }
      if (flVal<0 && CLPISF_UNS(psArg->psStd->uiFlg)) {
         return CLPERR(psHdl,CLPERR_SEM,"Literal number (%s) of '%s.%s' is negative (%f) but marked as unsigned",isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw,flVal);
      }
      switch (psArg->psFix->siSiz) {
      case 4:
         *((F32*)psArg->psVar->pvPtr)=flVal;
         TRACE(psHdl->pfBld,"%s BUILD-LITERAL-F32(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnFlt(psArg,flVal));
         break;
      case 8:
         *((F64*)psArg->psVar->pvPtr)=flVal;
         TRACE(psHdl->pfBld,"%s BUILD-LITERAL-F64(PTR=%p CNT=%d LEN=%d RST=%d)%s=%f\n",
                                 fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnFlt(psArg,flVal));
         break;
      default: return CLPERR(psHdl,CLPERR_SIZ,"Size (%d) for the value (%s) of '%s.%s' is not 4 (float) or 8 (double))",psArg->psFix->siSiz,isPrnStr(psArg,pcVal),pcPat,psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
      psArg->psVar->siLen+=psArg->psFix->siSiz;
      psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
      siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psEln,FALSE);
      if (siErr<0) { return(siErr); }
      break;
   case CLPTYP_STRING:
      if (CLPISF_FIX(psArg->psStd->uiFlg)) {
         l0=psArg->psFix->siSiz;
         if (CLPISF_DYN(psArg->psStd->uiFlg)) {
            void** ppDat=(void**)psArg->psVar->pvDat;
            (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
            if ((*ppDat)==NULL) {
               return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d*%d)) for argument '%s.%s' failed (4)",psArg->psVar->siLen,(CLPISF_DLM(psArg->psStd->uiFlg))?2:1,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
            }
            psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
         } else {
            if (psArg->psVar->siRst<psArg->psFix->siSiz) {
               return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
            }
            if (psArg->psVar->pvPtr==NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
            }
         }
      } else {
         l0=psArg->psVar->siRst;
         if (!CLPISF_DYN(psArg->psStd->uiFlg)) {
            if (psArg->psVar->pvPtr==NULL) {
               return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument are defined but write pointer not set",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
            }
         }
      }
      char chMode = pcVal[0];
      U32  isNotDone;
      do {
         isNotDone = FALSE;
         switch (chMode) {
         case 'x':
            l1=strlen(pcVal+2);
            if (l1%2) {
               return CLPERR(psHdl,CLPERR_LEX,"Length of hexadecimal string (%c(%s)) for '%s.%s' is not a multiple of 2",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw);
            }
            if ((l1/2)>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg) && !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+(l1/2)>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+(l1/2)+4,&psArg->psVar->siInd,psArg->psStd->uiFlg);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+(%d/+2)+4) for argument '%s.%s' failed (5)",psArg->psVar->siLen,l1,pcPat,psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"Hexadecimal string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,2*l0);
               }
            }
            l2=hex2bin(pcVal+2,(U08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1/2) {
               return CLPERR(psHdl,CLPERR_SEM,"Hexadecimal string (%c(%s)) of '%s.%s' cannot be converted from hex to bin (error after %d bytes)",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,l2*2);
            }
            siSln=l2;
            ((char*)psArg->psVar->pvPtr)[l2]=EOS;
            if (!CLPISF_BIN(psArg->psStd->uiFlg)) {
               // parameter is not flagged as binary => check that data does not contain \0
               U32 uiStrLen = strlen((char*)psArg->psVar->pvPtr);
               if (uiStrLen != l2) {
                  return CLPERR(psHdl,CLPERR_SEM,"HEX string contains NUL-bytes at offset %d which is not allowed for argument '%s.%s' (not flagged as binary)",uiStrLen,pcPat,psArg->psStd->pcKyw);
               }
            }
            TRACE(psHdl->pfBld,"%s BUILD-LITERAL-HEX(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            break;
         case 'a':
            l1=strlen(pcVal+2);
            if (l1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd,psArg->psStd->uiFlg);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+%d+4) for argument '%s.%s' failed (6)",psArg->psVar->siLen,l1,pcPat,psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"ASCII string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,l0);
               }
            }
            l2=chr2asc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               return CLPERR(psHdl,CLPERR_SEM,"ASCII string (%c(%s)) of '%s.%s' cannot be converted to ASCII (error after %d bytes)",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,l2);
            }
            siSln=l2;
            ((char*)psArg->psVar->pvPtr)[l2]=EOS;
            TRACE(psHdl->pfBld,"%s BUILD-LITERAL-ASC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            break;
         case 'e':
            l1=strlen(pcVal+2);
            if (l1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg) && !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd,psArg->psStd->uiFlg);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+%d+4) for argument '%s.%s' failed (7)",psArg->psVar->siLen,l1,pcPat,psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"EBCDIC string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,l0);
               }
            }
            l2=chr2ebc(pcVal+2,(C08*)psArg->psVar->pvPtr,l1);
            if (l2!=l1) {
               return CLPERR(psHdl,CLPERR_SEM,"EBCDIC string (%c(%s)) of '%s.%s' cannot be converted to EBCDIC (error after %d bytes)",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,l2);
            }
            siSln=l2;
            ((char*)psArg->psVar->pvPtr)[l2]=EOS;
            TRACE(psHdl->pfBld,"%s BUILD-LITERAL-EBC(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            break;
         case 'c':
            l1=strlen(pcVal+2);
            if (l1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg)&& !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1>psArg->psFix->siSiz) {
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd,psArg->psStd->uiFlg);
                  if ((*ppDat)==NULL) {
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+%d+4) for argument '%s.%s' failed (8)",psArg->psVar->siLen,l1,pcPat,psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  return CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcVal+2),pcPat,psArg->psStd->pcKyw,l0);
               }
            }
            memcpy(psArg->psVar->pvPtr,pcVal+2,l1);
            siSln=l2=l1;
            ((char*)psArg->psVar->pvPtr)[l2]=EOS;
            if (!CLPISF_BIN(psArg->psStd->uiFlg)) {
               // parameter is not flagged as binary => check that data does not contain \0
               U32 uiStrLen = strlen((char*)psArg->psVar->pvPtr);
               if (uiStrLen != l2) {
                  return CLPERR(psHdl,CLPERR_SEM,"String contains NUL-bytes at offset %d which is not allowed for argument '%s.%s' (not flagged as binary)",uiStrLen,pcPat,psArg->psStd->pcKyw);
               }
            }
            TRACE(psHdl->pfBld,"%s BUILD-LITERAL-CHR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
            break;
         case 's':
            if (CLPISF_XML(psArg->psStd->uiFlg)) {
               pcHlp=dmapxml(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
            } else if (CLPISF_FIL(psArg->psStd->uiFlg)) {
               pcHlp=dmapfil(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
            } else if (CLPISF_LAB(psArg->psStd->uiFlg)) {
               pcHlp=dmaplab(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
            } else {
               pcHlp=dmapstr(pcVal+2,CLPISF_UPP(psArg->psStd->uiFlg)?1:CLPISF_LOW(psArg->psStd->uiFlg)?2:0);
            }
            if (pcHlp==NULL) {
               return CLPERR(psHdl,CLPERR_MEM,"String mapping (memory allocation) for argument '%s.%s' failed",pcPat,psArg->psStd->pcKyw);
            }
            l1=strlen(pcHlp);
            if (l1+1>l0) {
               if (CLPISF_DYN(psArg->psStd->uiFlg) && !CLPISF_FIX(psArg->psStd->uiFlg)) {
                  void** ppDat=(void**)psArg->psVar->pvDat;
                  if (psArg->psVar->siLen+l1+1>psArg->psFix->siSiz) {
                     free(pcHlp);
                     return CLPERR(psHdl,CLPERR_MEM,"Size limit (%d) reached for argument '%s.%s'",psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
                  }
                  (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+l1+4,&psArg->psVar->siInd,psArg->psStd->uiFlg);
                  if ((*ppDat)==NULL) {
                     free(pcHlp);
                     return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+%d+4) for argument '%s.%s' failed (9)",psArg->psVar->siLen,l1,pcPat,psArg->psStd->pcKyw);
                  }
                  psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
               } else {
                  siErr=CLPERR(psHdl,CLPERR_LEX,"Character string (%c(%s)) of '%s.%s' is longer than %d",pcVal[0],isPrnStr(psArg,pcHlp),pcPat,psArg->psStd->pcKyw,l0-1);
                  free(pcHlp);
                  return(siErr);
               }
            }
            memcpy(psArg->psVar->pvPtr,pcHlp,l1);
            ((char*)psArg->psVar->pvPtr)[l1]=EOS;
            free(pcHlp);
            siSln=l1; l2=l1+1;
            TRACE(psHdl->pfBld,"%s BUILD-LITERAL-STR(PTR=%p CNT=%d LEN=%d RST=%d)%s=%s(%d)\n",
                                    fpcPre(psHdl,siLev),psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst,psArg->psStd->pcKyw,isPrnStr(psArg,pcVal),isPrnLen(psArg,l2));
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
            char                          acSrc[strlen(psHdl->pcSrc)+1];
            size_t                        szLex=CLPINI_LEXSIZ;
            char*                         pcLex=(char*)calloc(1,szLex);
            char                          acMsg[1024]="";
            if (pcLex==NULL) {
               return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store the lexeme or file name failed"));
            }
            siErr=psHdl->pfF2s(psHdl->pvGbl,psHdl->pvF2s,pcVal+2,&pcDat,&siSiz,acMsg,sizeof(acMsg));
            if (siErr<0) {
               siErr=CLPERR(psHdl,CLPERR_SYS,"String file: %s",acMsg);
               SAFE_FREE(pcDat); free(pcLex);
               return(siErr);
            }
            TRACE(psHdl->pfPrs,"STRING-FILE-BEGIN(%s)\n",pcVal+2);
            strcpy(acSrc,psHdl->pcSrc);
            srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(CLPSRC_SRF)+strlen(pcVal+2),"%s%s",CLPSRC_SRF,pcVal+2);

            pcInp=psHdl->pcInp; psHdl->pcInp=pcClpUnEscape(psHdl,pcDat);
            SAFE_FREE(pcDat);
            if (psHdl->pcInp==NULL) {
               siErr=CLPERR(psHdl,CLPERR_MEM,"Un-escaping of string file (%s) failed",pcVal+2);
               free(pcLex);
               return(siErr);
            }
            pcCur=psHdl->pcCur; psHdl->pcCur=psHdl->pcInp;
            pcOld=psHdl->pcOld; psHdl->pcOld=psHdl->pcInp;
            pcRow=psHdl->pcRow; psHdl->pcRow=psHdl->pcInp;
            siRow=psHdl->siRow; psHdl->siRow=1;
            psHdl->siBuf++;
            siTok=siClpScnNat(psHdl,psHdl->pfErr,psHdl->pfScn,&psHdl->pcCur,&szLex,&pcLex,CLPTYP_STRING,psArg,NULL,NULL);
            if (siTok<0) {
               free(pcLex);
               return(siTok);
            }
            if (siTok!=CLPTOK_STR) {
               siErr=CLPERR(psHdl,CLPERR_SYN,"The token (%s(%s)) is not allowed in a string file (%c(%s)) of '%s.%s'",pcMapClpTok(siTok),pcLex,pcVal[0],pcVal+2,pcPat,psArg->psStd->pcKyw);
               free(pcLex);
               return(siErr);
            }
            if (pcLex[0]=='f') {
               siErr=CLPERR(psHdl,CLPERR_SYN,"Define a string file (%c(%s)) in a string file (%c(%s)) is not allowed (%s.%s)",pcLex[0],pcLex+2,pcVal[0],pcVal+2,pcPat,psArg->psStd->pcKyw);
               free(pcLex);
               return(siErr);
            }
            siErr=siClpBldLit(psHdl,siLev,siPos,psArg,pcLex);
            psHdl->siBuf--;
            strcpy(psHdl->pcSrc,acSrc);
            psHdl->pcInp=pcInp; psHdl->pcCur=pcCur; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
            TRACE(psHdl->pfPrs,"STRING-FILE-END(%s)\n",pcVal+2);
            free(pcLex);
            return(siErr);
         }
         case 'd':
            if (CLPISF_BIN(psArg->psStd->uiFlg)) {
               if (CLPISF_HEX(psArg->psStd->uiFlg)) {
                  chMode = 'x';
               } else if (CLPISF_ASC(psArg->psStd->uiFlg)) {
                  chMode = 'a';
               } else if (CLPISF_EBC(psArg->psStd->uiFlg)) {
                  chMode = 'e';
               } else {
                  chMode = 'c';
               }
            } else {
               chMode = 's';
            }
            isNotDone = TRUE;
            break;
         default:
            CLPERR(psHdl,CLPERR_LEX,"String prefix (%c(0x%02X)) of '%s.%s' is not supported",pcVal[0],pcVal[0],pcPat,psArg->psStd->pcKyw);
            CLPERRADD(psHdl,0,"Please use one of the following values:%s","");
            CLPERRADD(psHdl,1,"x - for conversion from hex to bin%s","");
            CLPERRADD(psHdl,1,"a - for conversion in ASCII%s","");
            CLPERRADD(psHdl,1,"e - for conversion in EBCDIC%s","");
            CLPERRADD(psHdl,1,"c - for no conversion (normal character string without null termination)%s","");
            CLPERRADD(psHdl,1,"s - normal character string with null termination%s","");
            CLPERRADD(psHdl,1,"f - use file content as string%s","");
            // 'd' is the default depending on the flags and defined if no character given to qualify the string
            return(CLPERR_LEX);
         }
      } while (isNotDone);

      if (CLPISF_FIX(psArg->psStd->uiFlg)) {
         memset(((U08*)psArg->psVar->pvPtr)+l2,0,psArg->psFix->siSiz-l2);
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
         psArg->psVar->siLen+=psArg->psFix->siSiz;
         psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
         siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
         if (siErr<0) { return(siErr); }
      } else {
         if (CLPISF_DLM(psArg->psStd->uiFlg)) {
            ((char*)psArg->psVar->pvPtr)[l2]=0xFF; // end of string list
         }
         psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+l2;
         psArg->psVar->siLen+=l2;
         psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:l2;
         siErr=siClpBldLnk(psHdl,siLev,siPos,l2,psArg->psFix->psEln,TRUE);
         if (siErr<0) { return(siErr); }
      }
      siErr=siClpBldLnk(psHdl,siLev,siPos,siSln,psArg->psFix->psSln,TRUE);
      if (siErr<0) { return(siErr); }
      break;
   default: return CLPERR(psHdl,CLPERR_TYP,"Type (%d) of parameter '%s.%s' not supported in this case (literal)",psArg->psFix->siTyp,pcPat,psArg->psStd->pcKyw);
   }
   psArg->psVar->siCnt++;

   for (psCon=psArg->psDep;psCon!=NULL;psCon=psCon->psNxt) {
      if (pcKyw==NULL && psCon->psFix->siTyp==psArg->psFix->siTyp) {
         switch (psCon->psFix->siTyp) {
         case CLPTYP_NUMBER:
            switch (psCon->psFix->siSiz) {
            case 1: if (siVal==((I08*)psCon->psVar->pvDat)[0]) { pcKyw=psCon->psStd->pcKyw; } break;
            case 2: if (siVal==((I16*)psCon->psVar->pvDat)[0]) { pcKyw=psCon->psStd->pcKyw; } break;
            case 4: if (siVal==((I32*)psCon->psVar->pvDat)[0]) { pcKyw=psCon->psStd->pcKyw; } break;
            case 8: if (siVal==((I64*)psCon->psVar->pvDat)[0]) { pcKyw=psCon->psStd->pcKyw; } break;
            }
            break;
         case CLPTYP_FLOATN:
            switch (psCon->psFix->siSiz) {
            case 4: if (flVal==((F32*)psCon->psVar->pvDat)[0]) { pcKyw=psCon->psStd->pcKyw; } break;
            case 8: if (flVal==((F64*)psCon->psVar->pvDat)[0]) { pcKyw=psCon->psStd->pcKyw; } break;
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

   if (pcKyw!=NULL) {
      if (psArg->psFix->siTyp==CLPTYP_NUMBER && (CLPISF_TIM(psArg->psStd->uiFlg) || pcVal[0]=='t')) {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg))+strlen(isPrnStr(psArg,pcVal))+strlen(cstime(siVal,acTim)),"%s.%s=%s(%s(%s))\n",pcPat,GETKYW(psArg),pcKyw,isPrnStr(psArg,pcVal),acTim);
      } else {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg))+strlen(isPrnStr(psArg,pcVal)),"%s.%s=%s(%s)\n",pcPat,GETKYW(psArg),pcKyw,isPrnStr(psArg,pcVal));
      }
   } else {
      if (psArg->psFix->siTyp==CLPTYP_NUMBER && (CLPISF_TIM(psArg->psStd->uiFlg) || pcVal[0]=='t')) {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg))+strlen(isPrnStr(psArg,pcVal))+strlen(cstime(siVal,acTim)),"%s.%s=%s(%s)\n",pcPat,GETKYW(psArg),isPrnStr(psArg,pcVal),acTim);
      } else {
         srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg))+strlen(isPrnStr(psArg,pcVal)),"%s.%s=%s\n",pcPat,GETKYW(psArg),isPrnStr(psArg,pcVal));
      }
   }

   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) { return(siErr); }
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) { return(siErr); }
   }
   return(psArg->psFix->siTyp);
}

static int siClpIniMainObj(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   TsVar                         asSav[])
{
   const char*                   pcPat=fpcPat(psHdl,0);
   TsSym*                        psHlp;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (2)",pcPat,psTab->psStd->pcKyw);
   }

   if (psHdl->pfSaf!=NULL) {
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,pcPat)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",pcPat);
      }
   }

   if (psHdl->pvDat!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) {
         asSav[0]=psHlp->psVar[0];
         psHlp->psVar->pvDat=((char*)psHdl->pvDat)+psHlp->psFix->siOfs;
         psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
         psHlp->psVar->siCnt=0;
         psHlp->psVar->siLen=0;
         psHlp->psVar->siInd=0;
         psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
         if (CLPISF_FIX(psHlp->psStd->uiFlg)) { psHlp->psVar->siRst*=psHlp->psFix->siMax; }
      }
   } else {
      return CLPERR(psHdl,CLPERR_PAR,"Pointer to CLP data structure is NULL (%s.%s)",pcPat,psTab->psStd->pcKyw);
   }

   TRACE(psHdl->pfBld,"BUILD-BEGIN-MAIN-ARGUMENT-LIST\n");
   return(CLP_OK);
}

static int siClpFinMainObj(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   const TsVar                   asSav[])
{
   TsSym*                        psHlp=NULL;
   int                           i;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (3)",fpcPat(psHdl,0),psTab->psStd->pcKyw);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      int siErr=siClpSetDefault(psHdl,0,i,psHlp);
      if (siErr<0) {
         CLPERRADD(psHdl,0,"Set default value for argument '%s.%s' failed",fpcPat(psHdl,0),psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s' not specified",fpcPat(psHdl,0),psHlp->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please specify parameter:%s","");
               vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s' is smaller than required minimum amount (%d)",psHlp->psVar->siCnt,fpcPat(psHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
               CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
               vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            }
            return(CLPERR_SEM);
         }
      }
   }

   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) { psHlp->psVar[0]=asSav[0]; }

   TRACE(psHdl->pfBld,"BUILD-END-MAIN-ARGUMENT-LIST\n");
   return(CLP_OK);
}

static int siClpIniMainOvl(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   TsVar                         asSav[])
{
   const char*                   pcPat=fpcPat(psHdl,0);
   TsSym*                        psHlp;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (4)",pcPat,psTab->psStd->pcKyw);
   }

   if (psHdl->pfSaf!=NULL) {
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,pcPat)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",pcPat);
      }
   }

   if (psHdl->pvDat!=NULL) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) {
         asSav[0]=psHlp->psVar[0];
         psHlp->psVar->pvDat=((char*)psHdl->pvDat);
         psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
         psHlp->psVar->siCnt=0;
         psHlp->psVar->siLen=0;
         psHlp->psVar->siInd=0;
         psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
         if (CLPISF_FIX(psHlp->psStd->uiFlg)) { psHlp->psVar->siRst*=psHlp->psFix->siMax; }
      }
   } else {
      return CLPERR(psHdl,CLPERR_PAR,"Pointer to CLP data structure is NULL (%s.%s)",pcPat,psTab->psStd->pcKyw);
   }

   TRACE(psHdl->pfBld,"BUILD-BEGIN-MAIN-ARGUMENT\n");
   return(CLP_OK);
}

static int siClpFinMainOvl(
   TsHdl*                        psHdl,
   TsSym*                        psTab,
   const TsVar                   asSav[],
   const int                     siOid)
{
   TsSym*                        psHlp=NULL;
   int                           siErr,i;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (5)",fpcPat(psHdl,0),psTab->psStd->pcKyw);
   }

   for (i=0,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(psHdl,0,i,psHlp);
         if (siErr<0) {
            CLPERRADD(psHdl,0,"Set default value for argument '%s.%s' failed",fpcPat(psHdl,0),psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s' not specified",fpcPat(psHdl,0),psHlp->psStd->pcKyw);
                  CLPERRADD(psHdl,0,"Please specify parameter:%s","");
                  vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_SEL(psHlp->psStd->uiFlg));
               } else {
                  CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s' is smaller than required minimum amount (%d)",psHlp->psVar->siCnt,fpcPat(psHdl,0),psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
                  vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
               }
               return(CLPERR_SEM);
            }
         }
      }
   }

   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) { psHlp->psVar[0]=asSav[0]; }

   TRACE(psHdl->pfBld,"BUILD-END-MAIN-ARGUMENT\n");
   return(CLP_OK);
}

static int siClpIniObj(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar                         asSav[])
{
   int                           siErr;
   const int                     siTyp=CLPTYP_OBJECT;
   const char*                   pcPat=fpcPat(psHdl,siLev);
   TsSym*                        psHlp;
   (void)siPos;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of '%s.%s' with type '%s'",psArg->psFix->siMax,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }

   if (psHdl->pfSaf!=NULL) {
      char acEnt[strlen(pcPat)+strlen(GETKYW(psArg))+2];
      snprintf(acEnt,sizeof(acEnt),"%s.%s",pcPat,GETKYW(psArg));
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,acEnt)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",acEnt);
      }
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+%d) for object '%s.%s' failed",psArg->psVar->siLen,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
      }
   }

   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg)),"%s.%s(\n",pcPat,GETKYW(psArg));

   siErr=siExtentSymTab(psHdl,siLev,psArg);
   if (siErr<0) { return(siErr); }
   if (psArg->psDep==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set (1)",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
   }

   for (psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) {
      asSav[0]=psHlp->psVar[0];
      psHlp->psVar->pvDat=((char*)psArg->psVar->pvPtr)+psHlp->psFix->siOfs;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siInd=0;
      psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
      if (CLPISF_FIX(psHlp->psStd->uiFlg)) { psHlp->psVar->siRst*=psHlp->psFix->siMax; }
   }

   TRACE(psHdl->pfBld,"%s BUILD-BEGIN-OBJECT-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(psHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   *ppDep=psArg->psDep;
   return(psArg->psFix->siTyp);
}

static int siClpFinObj(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar                   asSav[])
{
   const int                     siTyp=CLPTYP_OBJECT;
   TsSym*                        psHlp;
   const char*                   pcPat;
   int                           siErr,i;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set (2)",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      siErr=siClpSetDefault(psHdl,siLev+1,i,psHlp);
      if (siErr<0) {
         CLPERRADD(psHdl,0,"Set default value for argument '%s.%s.%s' failed",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
         return(siErr);
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
            if (psHlp->psFix->siMin<=1) {
               CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s.%s' not specified",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
               CLPERRADD(psHdl,0,"Please specify parameter:%s","");
               vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            } else {
               CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s.%s' is smaller than required minimum amount (%d)",psDep->psVar->siCnt,fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
               CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
               vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                           CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
            }
            return(CLPERR_SEM);
         }
      }
   }

   for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) { psHlp->psVar[0]=asSav[0]; }

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   TRACE(psHdl->pfBld,"%s BUILD-END-OBJECT-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(psHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   pcPat=fpcPat(psHdl,siLev);
   srprintc(&psHdl->pcLst,&psHdl->szLst,strlen(pcPat)+strlen(GETKYW(psArg)),"%s.%s)\n",pcPat,GETKYW(psArg));

   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) { return(siErr); }
   if(psArg->psFix->siOid){
      siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) { return(siErr); }
   }
   return(psArg->psFix->siTyp);
}

static int siClpIniOvl(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym**                       ppDep,
   TsVar                         asSav[])
{
   int                           siErr;
   const int                     siTyp=CLPTYP_OVRLAY;
   const char*                   pcPat=fpcPat(psHdl,siLev);
   TsSym*                        psHlp;
   (void)siPos;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psVar->siCnt>=psArg->psFix->siMax) {
      return CLPERR(psHdl,CLPERR_SEM,"Too many (>%d) occurrences of '%s.%s' with type '%s'",psArg->psFix->siMax,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }

   if (psHdl->pfSaf!=NULL) {
      char acEnt[strlen(pcPat)+strlen(GETKYW(psArg))+2];
      snprintf(acEnt,sizeof(acEnt),"%s.%s",pcPat,GETKYW(psArg));
      if (psHdl->pfSaf(psHdl->pvGbl,psHdl->pvSaf,acEnt)) {
         return CLPERR(psHdl,CLPERR_AUT,"Authorization request for entity '%s' failed",acEnt);
      }
   }

   if (CLPISF_DYN(psArg->psStd->uiFlg)) {
      void** ppDat=(void**)psArg->psVar->pvDat;
      (*ppDat)=pvClpAllocFlg(psHdl,(*ppDat),psArg->psVar->siLen+((((CLPISF_DLM(psArg->psStd->uiFlg))?2:1)*psArg->psFix->siSiz)),&psArg->psVar->siInd,psArg->psStd->uiFlg);
      if ((*ppDat)==NULL) {
         return CLPERR(psHdl,CLPERR_MEM,"Dynamic memory allocation (%d+%d) for overlay '%s.%s' failed",psArg->psVar->siLen,psArg->psFix->siSiz,pcPat,psArg->psStd->pcKyw);
      }
      psArg->psVar->pvPtr=((char*)(*ppDat))+psArg->psVar->siLen;
   } else {
      if (psArg->psVar->siRst<psArg->psFix->siSiz) {
         return CLPERR(psHdl,CLPERR_SIZ,"Rest of space (%d) is not big enough for argument '%s.%s' with type '%s'",psArg->psVar->siRst,pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
      }
   }

   siErr=siExtentSymTab(psHdl,siLev,psArg);
   if (siErr<0) { return(siErr); }
   if (psArg->psDep==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set (3)",pcPat,psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
   }

   for (psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) {
      asSav[0]=psHlp->psVar[0];
      psHlp->psVar->pvDat=(char*)psArg->psVar->pvPtr;
      psHlp->psVar->pvPtr=psHlp->psVar->pvDat;
      psHlp->psVar->siCnt=0;
      psHlp->psVar->siLen=0;
      psHlp->psVar->siRst=CLPISF_DYN(psHlp->psStd->uiFlg)?0:psHlp->psFix->siSiz;
      if (CLPISF_FIX(psHlp->psStd->uiFlg)) { psHlp->psVar->siRst*=psHlp->psFix->siMax; }
   }

   TRACE(psHdl->pfBld,"%s BUILD-BEGIN-OVERLAY-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(psHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   *ppDep=psArg->psDep;
   return(psArg->psFix->siTyp);
}

static int siClpFinOvl(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg,
   TsSym*                        psDep,
   const TsVar                   asSav[],
   const int                     siOid)
{
   const int                     siTyp=CLPTYP_OVRLAY;
   TsSym*                        psHlp;
   int                           siErr,i;

   if (psArg->psFix->siTyp!=siTyp) {
      return CLPERR(psHdl,CLPERR_SEM,"The type (%s) of argument '%s.%s' don't match the expected type (%s)",pcMapClpTyp(siTyp),fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(psArg->psFix->siTyp));
   }
   if (psArg->psDep==NULL || psArg->psDep!=psDep) {
      return CLPERR(psHdl,CLPERR_TAB,"Keyword (%s.%s) and type (%s) of argument defined but pointer to parameter table not set (4)",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,pcMapClpTyp(siTyp));
   }

   for (i=0,psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,i++) {
      if (psHlp->psFix->siOid==siOid) {
         siErr=siClpSetDefault(psHdl,siLev+1,i,psHlp);
         if (siErr<0) {
            CLPERRADD(psHdl,0,"Set default value for argument '%s.%s.%s' failed",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
            return(siErr);
         }
      }
   }

   if (psHdl->isChk) {
      for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psHlp->psFix->siOid==siOid) {
            if (psHlp->psVar->siCnt<psHlp->psFix->siMin) {
               if (psHlp->psFix->siMin<=1) {
                  CLPERR(psHdl,CLPERR_SEM,"Parameter '%s.%s.%s' not specified",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw);
                  CLPERRADD(psHdl,0,"Please specify parameter:%s","");
                  vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
               } else {
                  CLPERR(psHdl,CLPERR_SEM,"Amount of occurrences (%d) of parameter '%s.%s.%s' is smaller than required minimum amount (%d)",psDep->psVar->siCnt,fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psHlp->psStd->pcKyw,psHlp->psFix->siMin);
                  CLPERRADD(psHdl,0,"Please specify parameter additionally %d times:%s","");
                  vdClpPrnArg(psHdl,psHdl->pfErr,1,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
               }
               return(CLPERR_SEM);
            }
         }
      }
   }

   for (psHlp=psDep;psHlp!=NULL;psHlp=psHlp->psNxt,asSav++) { psHlp->psVar[0]=asSav[0]; }

   psArg->psVar->pvPtr=((char*)psArg->psVar->pvPtr)+psArg->psFix->siSiz;
   psArg->psVar->siLen+=psArg->psFix->siSiz;
   psArg->psVar->siRst-=CLPISF_DYN(psArg->psStd->uiFlg)?0:psArg->psFix->siSiz;
   psArg->psVar->siCnt++;

   TRACE(psHdl->pfBld,"%s BUILD-END-OVERLAY-%s(PTR=%p CNT=%d LEN=%d RST=%d)\n",
                           fpcPre(psHdl,siLev),psArg->psStd->pcKyw,psArg->psVar->pvPtr,psArg->psVar->siCnt,psArg->psVar->siLen,psArg->psVar->siRst);

   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siCnt,psArg->psFix->psCnt,FALSE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psFix->siSiz,psArg->psFix->psEln,TRUE);
   if (siErr<0) { return(siErr); }
   siErr=siClpBldLnk(psHdl,siLev,siPos,psArg->psVar->siLen,psArg->psFix->psTln,FALSE);
   if (siErr<0) { return(siErr); }
   if(siOid){
      siErr=siClpBldLnk(psHdl,siLev,siPos,siOid,psArg->psFix->psOid,TRUE);
      if (siErr<0) { return(siErr); }
   }
   return(psArg->psFix->siTyp);
}

/**********************************************************************/

static int siClpSetDefault(
   TsHdl*                        psHdl,
   const int                     siLev,
   const int                     siPos,
   TsSym*                        psArg)
{
   TsSym*                        psDep=NULL;
   TsSym*                        psVal=NULL;
   size_t                        szVal=CLPINI_VALSIZ;
   char*                         pcVal=(char*)calloc(1,szVal);
   const char*                   pcCur;
   const char*                   pcInp;
   const char*                   pcOld;
   const char*                   pcRow;
   if (pcVal==NULL) { return(CLPERR(psHdl,CLPERR_MEM,"Allocation of memory to store the value expression failed")); }
   if (CLPISF_ARG(psArg->psStd->uiFlg) && psArg->psVar->siCnt==0 && psArg->psFix->pcDft!=NULL && strlen(psArg->psFix->pcDft)) {
      int   siRow,siTok,siErr;
      char  acSrc[strlen(psHdl->pcSrc)+1];
      char  acLex[strlen(psHdl->pcLex)+1];
      TRACE(psHdl->pfPrs,"SUPPLEMENT-LIST-PARSER-BEGIN(%s.%s=%s)\n",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psArg->psFix->pcDft);
      strcpy(acSrc,psHdl->pcSrc);
      strcpy(acLex,psHdl->pcLex);
      srprintf(&psHdl->pcSrc,&psHdl->szSrc,strlen(psArg->psFix->pcSrc),"%s",psArg->psFix->pcSrc);
      pcInp=psHdl->pcInp; psHdl->pcInp=psArg->psFix->pcDft;
      pcCur=psHdl->pcCur; psHdl->pcCur=psHdl->pcInp;
      pcOld=psHdl->pcOld; psHdl->pcOld=psHdl->pcInp;
      pcRow=psHdl->pcRow; psHdl->pcRow=psHdl->pcInp;
      siRow=psHdl->siRow; psHdl->siRow=psArg->psFix->siRow;
      siTok=psHdl->siTok;
      psHdl->siBuf++;

      for (psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg);psHdl->siTok>=0 && psHdl->siTok!=CLPTOK_END;
           psHdl->siTok=siClpScnSrc(psHdl,psArg->psFix->siTyp,psArg)) {
         if (psHdl->siTok==CLPTOK_KYW) {
            if (psArg->psFix->siTyp==CLPTYP_OBJECT) {
               TsVar asSav[CLPMAX_TABCNT];
               if (strxcmp(psHdl->isCas,psHdl->pcLex,"INIT",0,0,FALSE)!=0) {
                  siErr=CLPERR(psHdl,CLPERR_SYN,"Keyword (%s) in default / property definition for object '%s.%s' is not 'INIT'",psHdl->pcLex,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
                  free(pcVal);
                  return(siErr);
               }
               siErr=siClpIniObj(psHdl,siLev,siPos,psArg,&psDep,asSav);
               if (siErr<0) { free(pcVal); return(siErr); }
               siErr=siClpFinObj(psHdl,siLev,siPos,psArg,psDep,asSav);
               if (siErr<0) { free(pcVal); return(siErr); }
            } else  if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
               TsVar asSav[CLPMAX_TABCNT];
               siErr=siClpIniOvl(psHdl,siLev,siPos,psArg,&psDep,asSav);
               if (siErr<0) { free(pcVal); return(siErr); }
               siErr=siClpSymFnd(psHdl,siLev+1,psHdl->pcLex,psDep,&psVal,NULL);
               if (siErr<0) { free(pcVal); return(siErr); }
               siErr=siClpFinOvl(psHdl,siLev,siPos,psArg,psDep,asSav,psVal->psFix->siOid);
               if (siErr<0) { free(pcVal); return(siErr); }
            } else if (psArg->psFix->siTyp==CLPTYP_SWITCH) {
                  if (strxcmp(psHdl->isCas,psHdl->pcLex,"ON",0,0,FALSE)!=0 && strxcmp(psHdl->isCas,psHdl->pcLex,"OFF",0,0,FALSE)!=0) {
                     siErr=CLPERR(psHdl,CLPERR_SYN,"Keyword (%s) in default / property definition for switch '%s.%s' is not 'ON' or 'OFF'",psHdl->pcLex,fpcPat(psHdl,siLev),psArg->psStd->pcKyw);
                     free(pcVal);
                     return(siErr);
                  }
                  if (strxcmp(psHdl->isCas,psHdl->pcLex,"ON",0,0,FALSE)==0) {
                     siErr=siClpBldSwt(psHdl,siLev,siPos,psArg);
                     if (siErr<0) { free(pcVal); return(siErr); }
                  }
            } else {
               siErr=siClpPrsExp(psHdl,siLev,siPos,FALSE,psArg,&szVal,&pcVal);
               if (siErr<0) { free(pcVal); return(siErr); }
               TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(psHdl,siLev),siLev,siPos,isPrnLex(psArg,pcVal));
               siErr=siClpBldLit(psHdl,siLev,siPos,psArg,pcVal);
               if (siErr<0) { free(pcVal); return(siErr); }
            }
         } else {
            siErr=siClpPrsExp(psHdl,siLev,siPos,FALSE,psArg,&szVal,&pcVal);
            if (siErr<0) { free(pcVal); return(siErr); }
            TRACE(psHdl->pfPrs,"%s PARSER(LEV=%d POS=%d LIT(%s))\n",fpcPre(psHdl,siLev),siLev,siPos,isPrnLex(psArg,pcVal));
            siErr=siClpBldLit(psHdl,siLev,siPos,psArg,pcVal);
            if (siErr<0) { free(pcVal); return(siErr); }
         }
      }
      if (psHdl->siTok<0) { free(pcVal); return(psHdl->siTok); }
      psHdl->siBuf--;
      psHdl->siTok=siTok;
      strcpy(psHdl->pcLex,acLex);
      strcpy(psHdl->pcSrc,acSrc);
      psHdl->pcInp=pcInp; psHdl->pcCur=pcCur; psHdl->pcOld=pcOld; psHdl->pcRow=pcRow; psHdl->siRow=siRow;
      TRACE(psHdl->pfPrs,"SUPPLEMENT-LIST-PARSER-END(%s.%s=%s)\n",fpcPat(psHdl,siLev),psArg->psStd->pcKyw,psArg->psFix->pcDft);
   }
   free(pcVal);
   return(CLP_OK);
}

/**********************************************************************/

static void vdClpPrnArg(
   TsHdl*                        psHdl,
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
   const char*                   p=fpcPre(psHdl,siLev);
   const char*                   a="TYPE";
   const char*                   b=(isSel)?"SELECTION":((isCon)?"KEYWORD":pcMapClpTyp(siTyp));

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
         for (int i=0;i<siKwl;i++) { fprintf(pfOut,"%c",C_CRT); }
         fprintf(pfOut,"\n");
      } else {
         int i,siLen=strlen(pcKyw);
         fprintf(pfOut,"%s ",p);
         for (i=0;i<siKwl;i++) { fprintf(pfOut,"%c",toupper(pcKyw[i])); }
         for (/*i=i*/;i<siLen;i++) { fprintf(pfOut,"%c",tolower(pcKyw[i])); }
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
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     siLev,
   int                           siTyp,
   const TsSym*                  psTab)
{
   const TsSym*                  psHlp;
   if (psTab!=NULL && psTab->psHih!=NULL && psTab->psStd!=NULL) {
      if (CLPISF_CON(psTab->psStd->uiFlg)) {
         if (siTyp<0) { siTyp=psTab->psHih->psFix->siTyp; }
         if (CLPISF_SEL(psTab->psHih->psStd->uiFlg)==FALSE) {
            if (pfOut!=NULL) {
               if (siTyp>=0) {
                  fprintf(pfOut,"%s Enter a value (TYPE: %s) or use one of the keywords below:\n",fpcPre(psHdl,siLev),pcMapClpTyp(siTyp));
               } else {
                  fprintf(pfOut,"%s Enter a value or use one of the keywords below:\n",fpcPre(psHdl,siLev));
               }
            }
         }
      }
   }
   for (psHlp=psTab;psHlp!=NULL && psHlp->psStd!=NULL;psHlp=psHlp->psNxt) {
      if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISF_LNK(psHlp->psStd->uiFlg) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
         vdClpPrnArg(psHdl,pfOut,siLev,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
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
   const TsSym*                  psHlp;
   if (pfOut!=NULL && psTab!=NULL && !CLPISF_HID(psTab->psStd->uiFlg)) {
      int k=0;
      for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && !CLPISF_LNK(psHlp->psStd->uiFlg) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
            if (k) { fprintf(pfOut,"%s",pcSep); }
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
   if (pfOut!=NULL && psTab!=NULL && !CLPISF_HID(psTab->psStd->uiFlg)) {
      fprintf(pfOut,"%s",psTab->psStd->pcKyw);
      for (psHlp=psTab->psNxt;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISF_ALI(psHlp->psStd->uiFlg) && psHlp->psStd->psAli==psTab && !CLPISF_HID(psHlp->psStd->uiFlg)) {
            fprintf(pfOut,"%s",pcSep);
            fprintf(pfOut,"%s",psHlp->psStd->pcKyw);
         }
      }
   }
}

static int siClpPrnCmd(
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     siCnt,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psArg,
   const TsSym*                  psTab,
   const int                     isSkr,
   const int                     isMin)
{
   const char*                   pcHlp=NULL;
   const TsSym*                  psHlp;

   if (psTab==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Argument table not defined%s","");
   }

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (6)",fpcPat(psHdl,siLev),psTab->psStd->pcKyw);
   }

   if (pfOut!=NULL) {
      if (siLev<siDep || siDep>9) {
         int         k=0;
         const char* pcSep;
         if (psArg==NULL) {
            if (psHdl->isOvl) {
               pcSep=psHdl->pcOpt;
               if (siCnt==0) {
                  fprintf(pfOut,"%s.%c",psHdl->pcCmd,C_CBO);
                  if (isSkr) { k++; }
               }
            } else {
               pcSep=psHdl->pcEnt;
               if (siCnt==0) {
                  fprintf(pfOut,"%s(",psHdl->pcCmd);
                  if (isSkr) { k++; }
               }
            }
            if (isSkr) { k++; }
         } else {
            if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
               pcSep=psHdl->pcOpt;
               if (siCnt==0) {
                  fprintf(pfOut,"%s.%c",fpcPat(psHdl,siLev),C_CBO);
                  if (isSkr) { k++; }
               }
            }  else {
               pcSep=psHdl->pcEnt;
               if (siCnt==0) {
                  fprintf(pfOut,"%s(",fpcPat(psHdl,siLev));
                  if (isSkr) { k++; }
               }
            }
         }
         for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if (CLPISF_CMD(psHlp->psStd->uiFlg) && !CLPISF_LNK(psHlp->psStd->uiFlg) && !CLPISF_ALI(psHlp->psStd->uiFlg) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
               int siErr;
               if (isSkr) {
                  if (k) { fprintf(pfOut,"\n%s ",fpcPre(psHdl,siLev)); }
                    else { fprintf(pfOut,"%s "  ,fpcPre(psHdl,siLev)); }
                  if (isMin) {
                     if (psHlp->psFix->siMin) { fprintf(pfOut,"%c ",C_EXC); } else { fprintf(pfOut,"%c ",'?'); }
                  }
               } else {
                  if (k) { fprintf(pfOut,"%s",pcSep); }
                  if (isMin) {
                     if (psHlp->psFix->siMin) { fprintf(pfOut,"%c",C_EXC); } else { fprintf(pfOut,"%c",'?'); }
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
                  } else { pcHlp="str"; }
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
                  siErr=siExtentSymTab(psHdl,siLev,psHlp);
                  if (siErr<0) { return(siErr); }
                  if (psHlp->psDep==NULL) {
                     return CLPERR(psHdl,CLPERR_TAB,"Argument table for object '%s' not defined",psHlp->psStd->pcKyw);
                  }
                  if (psHlp->psFix->siMax==1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp); fprintf(pfOut,"(");
                     if (isSkr && (siLev+1<siDep || siDep>9)) { fprintf(pfOut,"\n"); }
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(psHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) { return(siErr); }
                     fprintf(pfOut,")");
                  } else if (psHlp->psFix->siMax>1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp); fprintf(pfOut,"%c(",C_SBO);
                     if (isSkr && (siLev+1<siDep || siDep>9)) { fprintf(pfOut,"\n"); }
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(psHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) { return(siErr); }
                     fprintf(pfOut,")...%c",C_SBC);
                  } else {
                     return CLPERR(psHdl,CLPERR_TAB,"Maximum amount of entries (%d) for parameter '%s' not valid",psHlp->psFix->siMax,psHlp->psStd->pcKyw);
                  }
                  break;
               case CLPTYP_OVRLAY:
                  siErr=siExtentSymTab(psHdl,siLev,psHlp);
                  if (siErr<0) { return(siErr); }
                  if (psHlp->psDep==NULL) {
                     return CLPERR(psHdl,CLPERR_TAB,"Argument table for overlay '%s' not defined",psHlp->psStd->pcKyw);
                  }
                  if (psHlp->psFix->siMax==1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,".%c",C_CBO);
                     if (isSkr  && (siLev+1<siDep || siDep>9)) { fprintf(pfOut,"\n"); }
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(psHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) { return(siErr); }
                     fprintf(pfOut,"%c",C_CBC);
                  } else if (psHlp->psFix->siMax>1) {
                     vdClpPrnAli(pfOut,psHdl->pcOpt,psHlp);fprintf(pfOut,"%c%c",C_SBO,C_CBO);
                     if (isSkr && (siLev+1<siDep || siDep>9)) { fprintf(pfOut,"\n"); }
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnCmd(psHdl,pfOut,siCnt+1,siLev+1,siDep,psHlp,psHlp->psDep,isSkr,isMin);
                     if (siErr<0) { return(siErr); }
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
               if (siCnt==0) { fprintf(pfOut,"%c",C_CBC); }
            } else {
               if (siCnt==0) { fprintf(pfOut,"%c",')'); }
            }
         } else {
            if (psArg->psFix->siTyp==CLPTYP_OVRLAY) {
               if (siCnt==0) { fprintf(pfOut,"%c",C_CBC); }
            } else {
               if (siCnt==0) { fprintf(pfOut,"%c",')'); }
            }
         }
      }
   }
   return (CLP_OK);
}

static int siClpPrnHlp(
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     isAli,
   const int                     siLev,
   const int                     siDep,
   const int                     siTyp,
   const TsSym*                  psTab,
   const int                     isFlg)
{
   const TsSym*                  psHlp;

   if (psTab==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Argument table not defined%s","");
   }

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (7)",fpcPat(psHdl,siLev),psTab->psStd->pcKyw);
   }

   if (pfOut!=NULL) {
      if (siLev<siDep || siDep>9) {
         if (isFlg) {
            if (siTyp>=0) {
               fprintf(pfOut,"%s Enter a value (TYPE: %s) or use one of the keywords below:\n",fpcPre(psHdl,siLev),pcMapClpTyp(siTyp));
            } else {
               fprintf(pfOut,"%s Enter a value or use one of the keywords below:\n",fpcPre(psHdl,siLev));
            }
         }
         for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if ((psHlp->psFix->siTyp==siTyp || siTyp<0) && CLPISF_CMD(psHlp->psStd->uiFlg) && !CLPISF_LNK(psHlp->psStd->uiFlg) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
               if (!CLPISF_ALI(psHlp->psStd->uiFlg) || (CLPISF_ALI(psHlp->psStd->uiFlg) && isAli)) {
                  vdClpPrnArg(psHdl,pfOut,siLev,psHlp->psStd->pcKyw,GETALI(psHlp),psHlp->psStd->siKwl,psHlp->psFix->siTyp,psHlp->psFix->pcHlp,psHlp->psFix->pcDft,
                              CLPISF_SEL(psHlp->psStd->uiFlg),CLPISF_CON(psHlp->psStd->uiFlg));
                  if (psHlp->psFix->siTyp==CLPTYP_NUMBER && CLPISF_DEF(psHlp->psStd->uiFlg)) {
                     fprintf(pfOut,"%s If you type the keyword without an assignment of a value, the default (%d) is used\n",fpcPre(psHdl,siLev+1),psHlp->psFix->siOid);
                  }
                  int siErr=siExtentSymTab(psHdl,siLev,psHlp);
                  if (siErr<0) { return(siErr); }
                  if (psHlp->psDep!=NULL) {
                     if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                        psHdl->apPat[siLev]=psHlp;
                        siErr=siClpPrnHlp(psHdl,pfOut,isAli,siLev+1,siDep,-1,psHlp->psDep,FALSE);
                        if (siErr<0) { return(siErr); }
                     } else {
                        psHdl->apPat[siLev]=psHlp;
                        if (CLPISF_SEL(psHlp->psStd->uiFlg)) {
                           siErr=siClpPrnHlp(psHdl,pfOut,isAli,siLev+1,siDep,psHlp->psFix->siTyp,psHlp->psDep,FALSE);
                           if (siErr<0) { return(siErr); }
                        } else {
                           siErr=siClpPrnHlp(psHdl,pfOut,isAli,siLev+1,siDep,psHlp->psFix->siTyp,psHlp->psDep,TRUE);
                           if (siErr<0) { return(siErr); }
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
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   const int                     isPat,
   const int                     siLev,
   const TsSym*                  psArg)
{
   const char*                   pcHlp=NULL;

   if (psArg==NULL) {
      return CLPERR(psHdl,CLPERR_TAB,"Argument not defined%s","");
   }

   if (pfOut!=NULL) {
      int siErr;
      if (isPat) {
         fprintf(pfOut,"%s.",fpcPat(psHdl,siLev));
      }
      switch (psArg->psFix->siTyp) {
      case CLPTYP_SWITCH:
         vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
         break;
      case CLPTYP_NUMBER:
         if (psArg->psFix->siMax==1) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
               fprintf(pfOut,CLP_ASSIGNMENT);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%snum%s",CLP_ASSIGNMENT,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%snum",CLP_ASSIGNMENT);
               }
            }
         } else {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
               fprintf(pfOut,"%c",C_SBO);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...%c",C_SBC);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%cnum%s",C_SBO,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
                  fprintf(pfOut,"...%c",C_SBC);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%cnum...%c",C_SBO,C_SBC);
               }
            }
         }
         break;
      case CLPTYP_FLOATN:
         if (psArg->psFix->siMax==1) {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
               fprintf(pfOut,CLP_ASSIGNMENT);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%sflt%s",CLP_ASSIGNMENT,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%sflt",CLP_ASSIGNMENT);
               }
            }
         } else {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
               fprintf(pfOut,"%c",C_SBO);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...%c",C_SBC);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%cflt%s",C_SBO,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
                  fprintf(pfOut,"...%c",C_SBC);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%cflt...%c",C_SBO,C_SBC);
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
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
               fprintf(pfOut,CLP_ASSIGNMENT);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%s'%s'%s",CLP_ASSIGNMENT,pcHlp,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%s'%s'",CLP_ASSIGNMENT,pcHlp);
               }
            }
         } else {
            if (CLPISF_SEL(psArg->psStd->uiFlg)) {
               vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
               fprintf(pfOut,"%c",C_SBO);
               vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
               fprintf(pfOut,"...%c",C_SBC);
            } else {
               if (psArg->psDep!=NULL) {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%c'%s'%s",C_SBO,pcHlp,psHdl->pcOpt);
                  vdClpPrnOpt(pfOut,psHdl->pcOpt,psArg->psFix->siTyp,psArg->psDep);
                  fprintf(pfOut,"...%c",C_SBC);
               } else {
                  vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
                  fprintf(pfOut,"%c'%s'...%c",C_SBO,pcHlp,C_SBC);
               }
            }
         }
         break;
      case CLPTYP_OBJECT:
         siErr=siExtentSymTab(psHdl,siLev,psArg);
         if (siErr<0) { return(siErr); }
         if (psArg->psDep==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Argument table for object '%s' not defined",psArg->psStd->pcKyw);
         }
         if (psArg->psFix->siMax==1) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
            fprintf(pfOut,"(");
            siErr=siClpPrnCmd(psHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,")");
            if (siErr<0) { return(siErr); }
         } else {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
            fprintf(pfOut,"%c(",C_SBO);
            siErr=siClpPrnCmd(psHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,")...%c",C_SBC);
            if (siErr<0) { return(siErr); }
         }
         break;
      case CLPTYP_OVRLAY:
         siErr=siExtentSymTab(psHdl,siLev,psArg);
         if (siErr<0) { return(siErr); }
         if (psArg->psDep==NULL) {
            return CLPERR(psHdl,CLPERR_TAB,"Argument table for overlay '%s' not defined",psArg->psStd->pcKyw);
         }
         if (psArg->psFix->siMax==1) {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
            fprintf(pfOut,".%c",C_CBO);
            siErr=siClpPrnCmd(psHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,"%c",C_CBC);
            if (siErr<0) { return(siErr); }
         } else {
            vdClpPrnAli(pfOut,psHdl->pcOpt,psArg);
            fprintf(pfOut,"%c%c",C_SBO,C_CBO);
            siErr=siClpPrnCmd(psHdl,pfOut,1,siLev+1,siLev+2,psArg,psArg->psDep,FALSE,FALSE);
            fprintf(pfOut,"%c...%c",C_CBC,C_SBC);
            if (siErr<0) { return(siErr); }
         }
         break;
      }
   }
   return(CLP_OK);
}

static int siClpPrnDoc(
   TsHdl*                        psHdl,
   FILE*                         pfDoc,
   const int                     siLev,
   const TsParamDescription*     psParamDesc,
   const TsSym*                  psArg,
   const TsSym*                  psTab)
{
   if (pfDoc!=NULL && psTab!=NULL && psParamDesc!=NULL) {
      const TsSym*                  psHlp;
      const TsSym*                  psSel;
      int                           isCon;
      int                           k,m;
      int                           siMan=0;
      int                           siLst=0;
      const TsSym*                  apMan[CLPMAX_TABCNT];
      const TsSym*                  apLst[CLPMAX_TABCNT];
      if (psTab->psBak!=NULL) {
         return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (8)",fpcPat(psHdl,siLev),psTab->psStd->pcKyw);
      }
      for (isCon=FALSE,psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (CLPISF_ARG(psHlp->psStd->uiFlg) && CLPISF_CMD(psHlp->psStd->uiFlg)) {
            if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY || (psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan)) {
               apMan[siMan]=psHlp;
               siMan++;
            } else {
               for (psSel=psHlp->psDep;psSel!=NULL;psSel=psSel->psNxt) {
                  if (psSel->psFix->pcMan!=NULL && *psSel->psFix->pcMan) {
                     apMan[siMan]=psHlp;
                     siMan++;
                     break;
                  }
               }
               if (psSel==NULL) {
                  apLst[siLst]=psHlp;
                  siLst++;
               }
            }
         } else if (CLPISF_CON(psHlp->psStd->uiFlg) && psArg!=NULL && psArg->psFix->siTyp == psHlp->psFix->siTyp) {
            if (psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan) {
               apMan[siMan]=psHlp;
               siMan++;
            } else {
               apLst[siLst]=psHlp;
               siLst++;
            }
            isCon=TRUE;
         }
      }

      if (isCon) {
         if (psArg!=NULL) {
            if (siLst) {
               fprintf(pfDoc,".Selections\n\n");
               for (m=0;m<siLst;m++) {
                  if (!CLPISF_HID(apLst[m]->psStd->uiFlg)) {
                     efprintf(pfDoc,"* `%s - %s`\n",apLst[m]->psStd->pcKyw,apLst[m]->psFix->pcHlp);
                  }
               }
               fprintf(pfDoc,"\n");
            }
            if (psHdl->isDep && siMan) {
               for (k=m=0;m<siMan;m++) {
                  if (!CLPISF_HID(apMan[m]->psStd->uiFlg)) {
                     TsParamDescription stNewParamDesc = *psParamDesc;
                     char acNewPath[strlen(psParamDesc->pcPath)+strlen(apMan[m]->psStd->pcKyw)+2];
                     char acNewNum[strlen(psParamDesc->pcNum)+10];

                     snprintf(acNewPath,sizeof(acNewPath),"%s.%s",psParamDesc->pcPath,apMan[m]->psStd->pcKyw);
                     snprintf(acNewNum,sizeof(acNewNum),"%s%d.",psParamDesc->pcNum,k+1);
                     stNewParamDesc.pcPath = acNewPath;
                     stNewParamDesc.pcNum = acNewNum;

                     vdPrintHdl(pfDoc,psHdl,&stNewParamDesc,"Constant",apMan[m]->psStd->pcKyw,'+');
                     fprintf(pfDoc, ".Synopsis\n\n");
                     fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                     efprintf(pfDoc,"HELP:   %s\n",apMan[m]->psFix->pcHlp);
                     if (psHdl->isPat) {
                        fprintf(pfDoc, "PATH:   %s\n",fpcPat(psHdl,siLev));
                     }
                     fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(apMan[m]->psFix->siTyp));
                     fprintf(pfDoc, "SYNTAX: %s\n",apMan[m]->psStd->pcKyw);
                     fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                     fprintf(pfDoc, ".Description\n\n");
                     fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,apMan[m]->psFix->pcMan,2);
                     k++;
                  }
               }
            }
         }
      } else {
         if (siLst) {
            fprintf(pfDoc,".Arguments\n\n");
            for (m=0;m<siLst;m++) {
               if (!CLPISF_HID(apLst[m]->psStd->uiFlg)) {
                  fprintf(pfDoc,"* %c%s: ",C_GRV,pcMapClpTyp(apLst[m]->psFix->siTyp));
                  siClpPrnSyn(psHdl,pfDoc,FALSE,siLev,apLst[m]);
                  efprintf(pfDoc," - %s`\n",apLst[m]->psFix->pcHlp);
                  for (psSel=apLst[m]->psDep;psSel!=NULL;psSel=psSel->psNxt) {
                     if (!CLPISF_HID(psSel->psStd->uiFlg)) {
                        efprintf(pfDoc,"** `%s - %s`\n",psSel->psStd->pcKyw,psSel->psFix->pcHlp);
                     }
                  }
               }
            }
            fprintf(pfDoc,"\n");
         }
         if (psHdl->isDep && siMan) {
            char  acArg[20];
            for (k=m=0;m<siMan;m++) {
               if (!CLPISF_HID(apMan[m]->psStd->uiFlg)) {
                  psHdl->apPat[siLev]=apMan[m];
                  switch (apMan[m]->psFix->siTyp){
                  case CLPTYP_OBJECT:strcpy(acArg,"Object");break;
                  case CLPTYP_OVRLAY:strcpy(acArg,"Overlay");break;
                  default           :strcpy(acArg,"Parameter");break;
                  }

                  TsParamDescription stParamDesc = *psParamDesc;
                  char acNewPath[strlen(psParamDesc->pcPath)+strlen(apMan[m]->psStd->pcKyw)+2];
                  char acNewNum[strlen(psParamDesc->pcNum)+10];

                  snprintf(acNewPath,sizeof(acNewPath),"%s.%s",psParamDesc->pcPath,apMan[m]->psStd->pcKyw);
                  snprintf(acNewNum,sizeof(acNewNum),"%s%d.",psParamDesc->pcNum,k+1);
                  stParamDesc.pcPath = acNewPath;
                  stParamDesc.pcNum = acNewNum;

                  vdPrintHdl(pfDoc,psHdl,&stParamDesc,acArg,apMan[m]->psStd->pcKyw,C_CRT);
                  fprintf(pfDoc, ".Synopsis\n\n");
                  fprintf(pfDoc, "-----------------------------------------------------------------------\n");
                  efprintf(pfDoc,"HELP:   %s\n",apMan[m]->psFix->pcHlp);
                  if (psHdl->isPat) {
                  fprintf(pfDoc, "PATH:   %s\n",fpcPat(psHdl,siLev)); }
                  fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(apMan[m]->psFix->siTyp));
                  fprintf(pfDoc, "SYNTAX: ");
                  int siErr=siClpPrnSyn(psHdl,pfDoc,FALSE,siLev,apMan[m]);
                  fprintf(pfDoc,"\n");
                  if (siErr<0) { return(siErr); }
                  fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
                  fprintf(pfDoc, ".Description\n\n");
                  if (apMan[m]->psFix->pcMan!=NULL && *apMan[m]->psFix->pcMan) {
                     fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,apMan[m]->psFix->pcMan,2);
                  } else {
                     fprintf(pfDoc,"No detailed description available for this argument.\n\n");
                  }
                  siErr=siExtentSymTab(psHdl,siLev,apMan[m]);
                  if (siErr<0) { return(siErr); }
                  if (apMan[m]->psDep!=NULL) {
                     siErr=siClpPrnDoc(psHdl,pfDoc,siLev+1,&stParamDesc,apMan[m],apMan[m]->psDep);
                     if (siErr<0) { return(siErr); }
                  }
                  k++;
               }
            }
         }
      }
   }
   return (CLP_OK);
}

static int siClpWriteArgument(
   TsHdl*                        psHdl,
   FILE*                         pfDoc,
   const int                     siLev,
   const TsParamDescription*     psParamDesc,
   const char*                   pcPat,
   const TsSym*                  psArg)
{
   if (CLPISF_CON(psArg->psStd->uiFlg) && !CLPISF_HID(psArg->psStd->uiFlg)) {
      vdPrintHdl(pfDoc,psHdl,psParamDesc,"Constant",psArg->psStd->pcKyw,'+');
      fprintf(pfDoc, ".Synopsis\n\n");
      fprintf(pfDoc, "-----------------------------------------------------------------------\n");
      efprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
      if (psHdl->isPat) {
      fprintf(pfDoc, "PATH:   %s\n",pcPat); }
      fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
      fprintf(pfDoc, "SYNTAX: %s\n",psArg->psStd->pcKyw);
      fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
      fprintf(pfDoc, ".Description\n\n");
      if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
         fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,2);
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Manual page for constant '%s.%s' missing",pcPat,psArg->psStd->pcKyw);
      }
      int i=0;
      for (const TsSym* psHlp=psArg->psDep;psHlp!=NULL;psHlp=psHlp->psNxt) {
         if (psArg->psFix->siTyp==psHlp->psFix->siTyp && psHlp->psFix->pcMan==NULL && !CLPISF_HID(psHlp->psStd->uiFlg)) {
            if (i==0) { fprintf(pfDoc,".Selections\n\n"); }
            efprintf(pfDoc,"* `%s - %s`\n",psHlp->psStd->pcKyw,psHlp->psFix->pcHlp);
            i++;
         }
      }
      if (i) { fprintf(pfDoc,"\n"); }
   } else if (CLPISF_ARG(psArg->psStd->uiFlg) && !CLPISF_HID(psArg->psStd->uiFlg)){
      int   siErr;
      char  acArg[16];
      switch (psArg->psFix->siTyp){
      case CLPTYP_OBJECT:strcpy(acArg,"Object");break;
      case CLPTYP_OVRLAY:strcpy(acArg,"Overlay");break;
      default           :strcpy(acArg,"Parameter");break;
      }
      vdPrintHdl(pfDoc,psHdl,psParamDesc,acArg,psArg->psStd->pcKyw,C_CRT);
      fprintf(pfDoc, ".Synopsis\n\n");
      fprintf(pfDoc, "-----------------------------------------------------------------------\n");
      efprintf(pfDoc,"HELP:   %s\n",psArg->psFix->pcHlp);
      if (psHdl->isPat) {
      fprintf(pfDoc, "PATH:   %s\n",pcPat); }
      fprintf(pfDoc, "TYPE:   %s\n",pcMapClpTyp(psArg->psFix->siTyp));
      fprintf(pfDoc, "SYNTAX: ");
      siErr=siClpPrnSyn(psHdl,pfDoc,FALSE,siLev,psArg);
      fprintf(pfDoc,"\n");
      if (siErr<0) { return(siErr); }
      fprintf(pfDoc, "-----------------------------------------------------------------------\n\n");
      fprintf(pfDoc, ".Description\n\n");
      if (psArg->psFix->pcMan!=NULL && *psArg->psFix->pcMan) {
         fprintm(pfDoc,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psArg->psFix->pcMan,2);
      } else {
         return CLPERR(psHdl,CLPERR_TAB,"Manual page for %s '%s.%s' missing",acArg,pcPat,psArg->psStd->pcKyw);
      }
      siErr=siExtentSymTab(psHdl,siLev,psArg);
      if (siErr<0) { return(siErr); }
      siErr=siClpWriteRemaining(psHdl,pfDoc,siLev,pcPat,psArg->psDep);
      if (siErr<0) { return(siErr); }
   }
   return (CLP_OK);
}

static int siClpPrintArgument(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsParamDescription*     psParamDesc,
   const char*                   pcPat,
   const char*                   pcFil,
   const TsSym*                  psArg)
{
   int                           siErr;
   FILE*                         pfTmp;

   pfTmp=fopen_tmp();
   if (pfTmp==NULL) {
      return CLPERR(psHdl,CLPERR_SYS,"Open of temporary file to print page for argument '%s.%s' failed",pcPat,psArg->psStd->pcKyw);
   }

   siErr=siClpWriteArgument(psHdl,pfTmp,siLev,psParamDesc,pcPat,psArg);
   if (siErr<0) {
      fclose_tmp(pfTmp);
      return(siErr);
   }

   char  acPat[strlen(pcPat)+strlen(psArg->psStd->pcKyw)+2];
   char  acFil[strlen(pcFil)+strlen(psArg->psStd->pcKyw)+2];
   snprintf(acPat,sizeof(acPat),"%s.%s",pcPat,psArg->psStd->pcKyw);
   snprintf(acFil,sizeof(acFil),"%s\v%s",pcFil,psArg->psStd->pcKyw);
   siErr=siClpPrintWritten(psHdl,pfTmp,siLev+1,acPat,acFil,psArg->psFix->pcMan);
   fclose_tmp(pfTmp);
   if (siErr<0) {
      return(siErr);
   }

   return(CLP_OK);
}

static int siClpPrintTable(
   TsHdl*                        psHdl,
   const int                     siLev,
   const TsParamDescription*     psParamDesc,
   const char*                   pcPat,
   const char*                   pcFil,
   const TsSym*                  psTab)
{
   int                           siErr,i=1;
   const TsSym*                  psHlp;

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (9)",pcPat,psTab->psStd->pcKyw);
   }
   if (psParamDesc==NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Parameter psParamDesc must not be NULL.");
   }
   for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
      if (CLPISF_CMD(psHlp->psStd->uiFlg) && (CLPISF_ARG(psHlp->psStd->uiFlg) || CLPISF_CON(psHlp->psStd->uiFlg)) &&
         (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY || (psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan))) {
         TsParamDescription stParamDesc = *psParamDesc;
         char  acNewNum[strlen(psParamDesc->pcNum)+16];
         char  acNewPath[strlen(psParamDesc->pcPath)+strlen(psHlp->psStd->pcKyw)+2];

         snprintf(acNewPath,sizeof(acNewPath),"%s.%s",psParamDesc->pcPath,psHlp->psStd->pcKyw);
         snprintf(acNewNum,sizeof(acNewNum),"%s%d.",psParamDesc->pcNum,i); i++;
         stParamDesc.pcPath=acNewPath;
         stParamDesc.pcNum=acNewNum;

         siErr=siExtentSymTab(psHdl,siLev,psHlp);
         if (siErr<0) { return(siErr); }

         siErr=siClpPrintArgument(psHdl,siLev,&stParamDesc,pcPat,pcFil,psHlp);
         if (siErr<0) { return(siErr); }
         if (psHlp->psDep!=NULL) {
            char  acPat[strlen(pcPat)+strlen(psHlp->psStd->pcKyw)+2];
            char  acFil[strlen(pcFil)+strlen(psHlp->psStd->pcKyw)+2];
            snprintf(acPat,sizeof(acPat),"%s.%s",pcPat,psHlp->psStd->pcKyw);
            snprintf(acFil,sizeof(acFil),"%s\v%s",pcFil,psHlp->psStd->pcKyw);
            siErr=siClpPrintTable(psHdl,siLev+1,&stParamDesc,acPat,acFil,psHlp->psDep);
            if (siErr<0) { return(siErr); }
         }
      }
   }
   return(CLP_OK);
}

static int siClpPrnPro(
   TsHdl*                        psHdl,
   FILE*                         pfOut,
   int                           isMan,
   const int                     siMtd,
   const int                     siLev,
   const int                     siDep,
   const TsSym*                  psTab,
   const char*                   pcArg)
{
   const TsSym*                  psHlp;

   if (psTab==NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Argument table not defined%s","");
   }

   if (psTab->psBak!=NULL) {
      return CLPERR(psHdl,CLPERR_INT,"Entry '%s.%s' not at beginning of a table (10)",fpcPat(psHdl,siLev),psTab->psStd->pcKyw);
   }

   if (pfOut!=NULL) {
      if (siLev<siDep || siDep>9) {
         for (psHlp=psTab;psHlp!=NULL;psHlp=psHlp->psNxt) {
            if (CLPISF_ARG(psHlp->psStd->uiFlg) && CLPISF_PRO(psHlp->psStd->uiFlg) && (pcArg==NULL || strxcmp(psHdl->isCas,psHlp->psStd->pcKyw,pcArg,0,0,FALSE)==0) && !CLPISF_HID(psHlp->psStd->uiFlg)) {
               if (psHlp->psFix->pcDft!=NULL && *psHlp->psFix->pcDft) {
                  if ((isMan || (!CLPISF_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan) {
                     if (siMtd==CLPPRO_MTD_DOC) {
                        fprintf(pfOut,".DESCRIPTION FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw,pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->pcHlp);
                        fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHlp->psFix->pcMan,0);
                        fprintf(pfOut," \n");
                     } else {
                        fprintf(pfOut,"\n%c DESCRIPTION for %s.%s.%s.%s:\n",C_HSH,psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw);
                        fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHlp->psFix->pcMan,0);
                        fprintf(pfOut," %c\n",C_HSH);
                     }
                     isMan=TRUE;
                  } else {
                     if (isMan) { fprintf(pfOut,"\n"); }
                     isMan=FALSE;
                  }
                  if (siMtd==CLPPRO_MTD_DOC) {
                     if (!CLPISF_CMD(psHlp->psStd->uiFlg)) {
                        fprintf(pfOut,".HELP FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw,pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->pcHlp);
                     }
                  } else {
                     fprintf(pfOut," %s.%s.%s.%s=\"%s\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw,psHlp->psFix->pcDft);
                     efprintf(pfOut,"# TYPE: %s HELP: %s #\n",pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->pcHlp);
                  }
               } else {
                  if (siMtd==CLPPRO_MTD_ALL || siMtd==CLPPRO_MTD_CMT || siMtd==CLPPRO_MTD_DOC) {
                     if ((isMan || (!CLPISF_CMD(psHlp->psStd->uiFlg))) && psHlp->psFix->pcMan!=NULL && *psHlp->psFix->pcMan) {
                        if (siMtd==CLPPRO_MTD_DOC) {
                           fprintf(pfOut,".DESCRIPTION FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw,pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->pcHlp);
                           fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHlp->psFix->pcMan,0);
                           fprintf(pfOut," \n");
                        } else {
                           fprintf(pfOut,"\n%c DESCRIPTION for %s.%s.%s.%s:\n",C_HSH,psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw);
                           fprintm(pfOut,psHdl->pcOwn,psHdl->pcPgm,psHdl->pcBld,psHlp->psFix->pcMan,0);
                           fprintf(pfOut," %c\n",C_HSH);
                        }
                        isMan=TRUE;
                     } else {
                        if (isMan) { fprintf(pfOut,"\n"); }
                        isMan=FALSE;
                     }
                     if (siMtd==CLPPRO_MTD_DOC) {
                        if (!CLPISF_CMD(psHlp->psStd->uiFlg)) {
                           fprintf(pfOut,".HELP FOR %s.%s.%s.%s: (TYPE: %s) %s\n\n",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw,pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->pcHlp);
                        }
                     } else {
                        if (siMtd==CLPPRO_MTD_CMT) {
                           fprintf(pfOut, ";%s.%s.%s.%s=\"\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw);
                        } else {
                           fprintf(pfOut, " %s.%s.%s.%s=\"\" ",psHdl->pcOwn,psHdl->pcPgm,fpcPat(psHdl,siLev),psHlp->psStd->pcKyw);
                        }
                        efprintf(pfOut,"# TYPE: %s HELP: %s #\n",pcMapClpTyp(psHlp->psFix->siTyp),psHlp->psFix->pcHlp);
                     }
                  }
               }

               int siErr=siExtentSymTab(psHdl,siLev,psHlp);
               if (siErr<0) { return(siErr); }

               if (psHlp->psDep!=NULL) {
                  if (psHlp->psFix->siTyp==CLPTYP_OBJECT || psHlp->psFix->siTyp==CLPTYP_OVRLAY) {
                     psHdl->apPat[siLev]=psHlp;
                     siErr=siClpPrnPro(psHdl,pfOut,isMan,siMtd,siLev+1,siDep,psHlp->psDep,NULL);
                     if (siErr<0) { return(siErr); }
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
   TsHdl*                        psHdl,
   const int                     siLev)
{
   int                           i;
   psHdl->pcPre[0]=EOS;
   for (i=0;i<siLev+1;i++) {
      srprintc(&psHdl->pcPre,&psHdl->szPre,strlen(psHdl->pcDep),"%s",psHdl->pcDep);
   }
   return(psHdl->pcPre);
}

static const char* fpcPat(
   TsHdl*                        psHdl,
   const int                     siLev)
{
   int                           i;
   if (psHdl->pcCmd!=NULL && *psHdl->pcCmd) {
      if (siLev) {
         srprintf(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->pcCmd),"%s.",psHdl->pcCmd);
      } else {
         srprintf(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->pcCmd),"%s",psHdl->pcCmd);
      }
   } else {
      psHdl->pcPat[0]=EOS;
   }
   for (i=0;i<(siLev);i++) {
      if (i) {
         srprintc(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->apPat[i]->psStd->pcKyw),".%s",psHdl->apPat[i]->psStd->pcKyw);
      } else {
         srprintc(&psHdl->pcPat,&psHdl->szPat,strlen(psHdl->apPat[i]->psStd->pcKyw),"%s",psHdl->apPat[i]->psStd->pcKyw);
      }
   }
   return(psHdl->pcPat);
}

/**********************************************************************/
