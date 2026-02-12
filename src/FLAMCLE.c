/**
 * @file   FLAMCLE.c
 * @brief  LIMES Command Line Execution in ANSI-C
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
 *
 **********************************************************************/
/*
 * TASK:0000086 Fehlermeldungen ueberarbeiten (Zentralisierung)
 * TASK:0000086 Wide character support fuer command files einbauen
 * TASK:0000086 Mehrsprachigkeit einbauen
 */
/* Standard-Includes **************************************************/

#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifdef __WIN__
#  include <windows.h>
#else
#  include <dlfcn.h>
#  include <sys/utsname.h>
#endif

/* Include eigener Bibliotheken  **************************************/

#ifdef __FL5__
#  include "STDIOL.h"
#  include "STRINGL.h"
#  include "CHKMEM.h"
#else
#  undef  fclose_unchecked
#  define fclose_unchecked  fclose
#  undef  pcSysError
#  define pcSysError strerror
#endif
#include "CLEPUTL.h"
#include "FLAMCLP.h"

#ifndef fopen_nowarn
#  define fopen_nowarn     fopen
#endif

/* Include der Schnittstelle ******************************************/

#include "FLAMCLE.h"
#include "CLEMAN.h"
#include "CLEMSG.h"

/* Definition der Version von FL-CLE ***********************************
 *
 * Changelog:
 * 1.1.1: Fix of the envar bug (ISSUE: 0000182)
 * 1.1.2: Adjust version and about
 * 1.1.3: Add clear of config
 * 1.1.4: Call FIN if RUN failed
 * 1.1.5: Property and command line specific parsing
 * 1.1.6: Add support for DD:STDENV on mainframes
 * 1.1.7: Remove message for envar's set on release build
 * 1.1.8: Add arguments if flcl help path man used
 * 1.1.9: Correct generation of manpages
 * 1.1.10: Don't print manpage twice at end of path anymore
 * 1.1.11: Correct PGM handling and support "-KYW" or "--KYW"
 * 1.1.12: Support default command or built-in function
 * 1.1.13: Provide license text at run and oid for the mapping function
 * 1.1.14: Compare of keywords instead of compare up to key word length
 * 1.1.15: Add built-in function CHGPROP
 * 1.1.16: Add alias list for getprop and default for chgprop if pcDef=="flam"
 * 1.1.17: Use HOMEDIR as default dir for config and property files
 * 1.1.18: Support new option at GETPROP to print all or only defined (set) properties
 * 1.1.19: search config file first in working dir and then in home dir for read operation
 * 1.1.20: print aliases at help only if keyword ALL or MAN is used
 * 1.1.21: eliminate isFlg from CleExecute() to accept file properties
 * 1.1.22: Add support for parameter files for each object and overlay (read.text=parfilename.txt)
 * 1.1.23: Invent CLEPUTL.h/c
 * 1.1.24: Use file2str() and get it more thrad save
 * 1.1.25: Support switch to enable parameter files for object and overlays
 *         Improve error handling (using new CLP error structure and printing)
 * 1.1.26: Replace static variables for version and about to make it possible to use the lib as DLL
 * 1.1.27: Fix issue 547: Parameter files working properly
 * 1.1.28: Rework to make CLEP better usable with DLLs (eliminate global variables, adjust about and version, correct includes)
 * 1.1.29: Use arry2str for command line to remove last static vars, fix object and overlay handling if default command (>flam4 "(flamin=...)")
 * 1.1.30: fix memory leaks found with memchecker
 * 1.1.31: Support definition of the owner per run of a command
 * 1.1.32: Support DD names for write operation (log, trace, docs, ...)
 * 1.1.33: Use setenv() instead of putenv() for DD:STDENV on z/OS
 * 1.1.34: Use snprintf() instead of sprintf() for static array strings
 * 1.1.35: Support file name mapping (+/<Cuser>)
 * 1.1.36: Introduce SET/GETENV() macros
 * 1.1.37: Rework and correct file handling, adjust documentation
 * 1.1.38: Adjust printout for support of synopsis at help if keyword man is used
 * 1.1.39: Correct clang warning (use only literal format strings)
 * 1.1.40: fix potential memory leaks
 * 1.1.41: Change to new condition codes (separation of warnings and error in RUN function)
 *         Add support of reason codes including generation of an appendix in GENDOCU for it
 *         Add new built-in function to print out error code messages
 * 1.1.42: Code page specific interpretation of punctuation characters on EBCDIC systems
 * 1.1.43: Replace unnecessary strlen()
 * 1.1.44: Define command qualifier for ClpDocu("COMMAND")
 * 1.1.45: Support replacement of &{OWN} and &{PGM} in man pages
 * 1.1.46: Support MAXCC parameter for command execution
 * 1.1.47: Print undefined/empty properties as comment
 * 1.1.48: Add support for special condition codes (SCC)
 * 1.1.49: Set owner id as environment variable if not already defined
 * 1.1.50: Support appendix for other CLP strings in docu generation
 * 1.1.51: Allow empty parameter lists for commands
 * 1.1.52: Improve help output (print help message for path)
 * 1.1.53: Fix typo by generating of indexterm
 * 1.1.54: Redesign appendix properties
 * 1.2.55: Make error message and source variable in length
 * 1.2.56: Make root and property variable in length
 * 1.2.57: Separate acNum (number) and acHdl (head line)
 * 1.2.58: Support dynamic strings and arrays in CLP structure
 * 1.2.59: Support pvClpAlloc in RUN functions
 * 1.2.60: Support dynamic string for file names
 * 1.2.61: Support condition code 1 if a relevant warning in the log
 * 1.2.62: Support suppression of minimal condition codes as MAXCC extension
 * 1.2.63: Support option QUIET in command syntax to suppress all printouts of CLE
 * 1.2.64: Support option SILENT like QUIET but the error messages will be printed
 * 1.2.65: Support pvClpAlloc in INI functions
 * 1.2.66: Fix valgrind issue at read of configuration file
 * 1.2.67: Read environment also from SYSUID.STDENV on z/OS
 * 1.2.68: Separate version and build number with hyphen instead of dot
 * 1.2.69: Use new file2str interface
 * 1.2.70: Support callback function for file to string
 * 1.2.71: Fix memory leak if GETPROP used without command
 * 1.2.72: Fix several issues concerning dia-critical character support for EBCDIC
 * 1.2.73: Support DD:FLAMPAR for FLAM command on z/OS
 * 1.2.74: Support no run reason code (siNoR)
 * 1.2.75: Add black box (handle) for all call back functions
 * 1.2.76: Support additional authorization for CLP path
 * 1.2.77: Use type of function and not type of pointer to function (usable for pragma's)
 * 1.2.78: Support free defined appendix
 * 1.3.79: Support new more flexible table based documentation generation
 * 1.3.80: Support optional built-in function HTMLDOC
 * 1.3.81: Improve printouts and add time stamps
 * 1.3.82: Support CLE_QUITE/SILENT environment variables
 * 1.3.83: Initialize dia-critical characters support also if quiet defined
 * 1.3.84: Correct handling of pfOut and pfErr for built-in functions
 * 1.3.85: Add new about function with indentation
 * 1.3.86: Add new LSTENV and HLPENV built-in functions and use HLPENV to generate docu about used environment variables
 * 1.3.87: Build complete environment using call back functions
 * 1.4.88: Correct support for command overlays
 * 1.4.89: Support environment variables CLE_MAX/MIN_CC
 * 1.4.90: Fix handling of owner and MAX/MINCC (make it independent of default command)
 * 1.4.91: Use always stdout for pfStd
 * 1.4.92: Add abbreviation support for commands and built-in functions (minimum 3 characters)
 *         Implement central siCleFindCommand() function for unique abbreviation matching
 *         Add error messages for ambiguous abbreviations and too short inputs
 * 1.4.93: Fix siCleGetCommand() to support command abbreviations by using matched length
 *         Add siMln parameter to siCleGetCommand() for proper stop character handling
 * 1.4.94: Refactor all built-in functions into separate functions with TsCleBuiltinContext
 *         Convert from goto label dispatch to function pointer table dispatch (asBif[].pfFunc)
 *         Remove ELIMIT_CTX macro and simplify return code handling (direct return siErr)
 *         Remove unused TsCleBuiltinContext fields (9 fields: isNbr, isShl, ppcFil, pszFil,
 *         acTs, ppfDoc, ppfPro, siMaxCC, siMinCC) - reduced structure by 16%
 *         Simplify ERROR macro signature (remove unused pcBuf parameter)
 *         Remove char* pcBuf parameter from siCleEndExecution() function
 */
#define CLE_VSN_STR       "1.4.94"
#define CLE_VSN_MAJOR      1
#define CLE_VSN_MINOR        4
#define CLE_VSN_REVISION       94

/* Definition der Konstanten ******************************************/

#define CLEINI_PROSIZ            1024

#define CLE_BUILTIN_IDX_SYNTAX      0
#define CLE_BUILTIN_IDX_HELP        1
#define CLE_BUILTIN_IDX_MANPAGE     2
#define CLE_BUILTIN_IDX_GENDOCU     3
#define CLE_BUILTIN_IDX_HTMLDOC     4
#define CLE_BUILTIN_IDX_GENPROP     5
#define CLE_BUILTIN_IDX_SETPROP     6
#define CLE_BUILTIN_IDX_CHGPROP     7
#define CLE_BUILTIN_IDX_DELPROP     8
#define CLE_BUILTIN_IDX_GETPROP     9
#define CLE_BUILTIN_IDX_SETOWNER    10
#define CLE_BUILTIN_IDX_GETOWNER    11
#define CLE_BUILTIN_IDX_SETENV      12
#define CLE_BUILTIN_IDX_GETENV      13
#define CLE_BUILTIN_IDX_DELENV      14
#define CLE_BUILTIN_IDX_LSTENV      15
#define CLE_BUILTIN_IDX_HLPENV      16
#define CLE_BUILTIN_IDX_TRACE       17
#define CLE_BUILTIN_IDX_CONFIG      18
#define CLE_BUILTIN_IDX_GRAMMAR     19
#define CLE_BUILTIN_IDX_LEXEMES     20
#define CLE_BUILTIN_IDX_LICENSE     21
#define CLE_BUILTIN_IDX_VERSION     22
#define CLE_BUILTIN_IDX_ABOUT       23
#define CLE_BUILTIN_IDX_ERRORS      24

/* Definition der Strukturen ******************************************/

typedef struct CnfEnt {
   struct CnfEnt*                psNxt;
   struct CnfEnt*                psBak;
   size_t                        szKyw;
   char*                         pcKyw;
   size_t                        szVal;
   char*                         pcVal;
}TsCnfEnt;

typedef struct CnfHdl {
   int                           isChg;
   int                           isClr;
   int                           isCas;
   char*                         pcFil;
   const char*                   pcPgm;
   TsCnfEnt*                     psFst;
   TsCnfEnt*                     psLst;
}TsCnfHdl;

/* Forward declaration for built-in function pointer type */
struct CleBuiltinContext;
typedef int (*TfCleBuiltin)(struct CleBuiltinContext* psCtx);

typedef struct CleBuiltin {
   int                           siIdx;
   const char*                   pcKyw;
   const char*                   pcHlp;
   const char*                   pcSyn;
   const char*                   pcMan;
   int                           isBif;
   TfCleBuiltin                  pfFunc;
} TsCleBuiltin;

#define CLEBIF_OPN(name)                          TsCleBuiltin (name)[]
#define CLETAB_BIF(idx,kyw,hlp,syn,man,bif,func)  {(idx),(kyw),(hlp),(syn),(man),(bif),(func)},
#define CLEBIF_CLS                                { -1, NULL, NULL, NULL, NULL, FALSE, NULL}

/* Context structure for built-in function implementations */
typedef struct CleBuiltinContext {
   /* Command line arguments */
   int                           argc;
   char**                        argv;
   /* File pointers */
   FILE*                         pfOut;
   FILE*                         pfErr;
   FILE*                         pfStd;
   FILE*                         pfTrc;
   /* Configuration strings */
   const char*                   pcDep;
   const char*                   pcOpt;
   const char*                   pcEnt;
   const char*                   pcOwn;
   const char*                   pcPgm;
   const char*                   pcBld;
   const char*                   pcDef;
   /* Tables and flags */
   const TsCleBuiltin*           psBif;
   const void*                   psCmd;  /* TsCleCommand* - avoid forward declaration issue */
   int                           isCas;
   int                           isPfl;
   int                           isRpl;
   int                           siMkl;
   /* Handles and pointers */
   TsCnfHdl*                     psCnf;
   void*                         pvGbl;
   void**                        ppvHdl;
   /* Callbacks */
   void*                         pvF2S;
   void*                         pfF2S;  /* TfF2S* */
   void*                         pvSaf;
   void*                         pfSaf;  /* TfSaf* */
   void*                         pfMsg;  /* TfMsg* */
   /* Info strings */
   const char*                   pcLic;
   const char*                   pcVsn;
   const char*                   pcAbo;
   const char*                   pcHlp;
   const char*                   pcAut;
   const char*                   pcAdr;
   const char*                   pcDpa;
   const char*                   pcPgmNum;
   const char*                   pcBifNum;
   const char*                   pcCmdNum;
   const char*                   pcPgmMan;
   const char*                   pcSccMan;
   const char*                   pcHom;
   /* Working variables (pointers for modification) */
   char**                        ppcCnf;
   size_t*                       pszCnf;
   int*                          psiDep;
   /* Environment */
   const void*                   psEnv;  /* TsClpArgument* */
   const char*                   pcEnv;
   /* Document/property file pointers */
   const void*                   psDoc;  /* TsCleDoc* */
   const void*                   psOth;  /* TsCleOtherClp* */
} TsCleBuiltinContext;

typedef struct CleDocPar {
   const char*                   pcOwn;
   const char*                   pcPgm;
   const char*                   pcAut;
   const char*                   pcAdr;
   const char*                   pcBld;
   const char*                   pcHlp;
   const char*                   pcDep;
   const char*                   pcOpt;
   const char*                   pcEnt;
   const char*                   pcDpa;
   void*                         pvCnf;
   int                           isPfl;
   int                           isRpl;
   int                           isCas;
   int                           siMkl;
   TfMsg*                        pfMsg;
   const char*                   pcVsn;
   const char*                   pcAbo;
   int                           isDep;
   int                           isHdr;
   int                           isAnc;
   int                           isNbr;
   int                           isShl;
   int                           isPat;
   int                           isIdt;
   const TsCleBuiltin*           psBif;
   const TsCleCommand*           psCmd;
   const TsCleOtherClp*          psOth;
   void*                         pvF2S;
   TfF2S*                        pfF2S;
   void*                         pvSaf;
   TfSaf*                        pfSaf;
   int                           siPs1;
   int                           siPs2;
   int                           siPr3;
   const TsClpArgument*          psEnv;
   const char*                   pcEnv;
}TsCleDocPar;

/* Deklaration der internen Funktionen ********************************/

static int siClePropertyInit(
   void*                         pvGbl,
   TfIni*                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const int*                    piOid,
   const TsClpArgument*          psTab,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   char**                        ppFil,
   int*                          piFil,
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf);

static int siClePropertyFinish(
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   TsCnfHdl*                     psCnf,
   void*                         pvHdl,
   const char*                   pcFil,
   const int                     siFil);

static int siCleChangeProperties(
   void*                         pvGbl,
   TfIni*                        pfIni,
   void*                         pvClp,
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const char*                   pcPro,
   const int*                    piOid,
   const TsClpArgument*          psTab,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf);

static int siCleCommandInit(
   void*                         pvGbl,
   TfIni*                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   int*                          piOid,
   const TsClpArgument*          psTab,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf,
   const char*                   pcStr);

static int siCleSimpleInit(
   FILE*                         pfOut,
   FILE*                         pfErr,
   const int                     isPfl,
   const int                     isRpl,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   void**                        ppHdl);

static int siCleEndExecution(
   const int                     siErr,
   TsCnfHdl*                     psCnf,
   FILE*                         pfTrc,
   FILE*                         pfDoc,
   FILE*                         pfPro,
   char**                        ppArg,
   void*                         pvHdl);

static void vdCleManProgram(
   FILE*                         pfOut,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcHlp,
   const char*                   pcMan,
   const char*                   pcDep,
   const char*                   pcSep,
   const char*                   pcDpa,
   const char*                   pcNum,
   const int                     isMan,
   const int                     isNbr);

static void vdCleManFunction(
   FILE*                         pfOut,
   const unsigned int            uiLev,
   const char*                   pcLev,
   const char*                   pcNum,
   const char*                   pcFct,
   const char*                   pcHlp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcSyn,
   const char*                   pcMan,
   const int                     isMan,
   const int                     isPat,
   const int                     isNbr,
   const int                     isShl,
   const int                     isIdt);

static void vdPrnStaticSyntax(
   FILE*                         pfOut,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const char*                   pcPgm,
   const char*                   pcDep,
   const char*                   pcSep,
   const char*                   pcDpa);

static void vdPrnStaticHelp(
   FILE*                         pfOut,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const char*                   pcPgm,
   const char*                   pcDep);

static void vdPrnCommandSyntax(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcPgm,
   const char*                   pcCmd,
   const char*                   pcDep,
   const int                     siDep);

static void vdPrnCommandHelp(
   void*                         pvHdl,
   const char*                   pcCmd,
   const int                     siDep,
   const int                     isAli,
   const int                     isMan);

static void vdPrnCommandManpage(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcNum,
   const char*                   pcCmd,
   const int                     siInd,
   const int                     isMan,
   const int                     isNbr);

static void vdPrnProperties(
   void*                         pvHdl,
   const char*                   pcPat,
   const int                     siDep);

static int siCleGetProperties(
   FILE*                         pfErr,
   TsCnfHdl*                     psCnf,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   char**                        ppFil,
   char**                        ppPro,
   int*                          piFlg,
   void*                         pvGbl,
   void*                         pvF2S,
   TfF2S*                        pfF2S);

static int siCleGetCommand(
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcFct,
   int                           argc,
   char*                         argv[],
   char**                        ppFil,
   char**                        ppCmd,
   void*                         pvGbl,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   const int                     siMln,
   const char*                   pcDpa);

static TsCnfHdl* psCnfOpn(
   FILE*                         pfErr,
   const int                     isCas,
   const char*                   pcPgm,
   const char*                   pcFil);

static int siCnfSet(
   TsCnfHdl*                     psHdl,
   FILE*                         pfErr,
   const char*                   pcKyw,
   const char*                   pcVal,
   const int                     isOvr);

static const char* pcCnfGet(
   TsCnfHdl*                     psHdl,
   const char*                   pcKyw);

static int siCnfPutEnv(
   TsCnfHdl*                     psHdl,
   const char*                   pcOwn,
   const char*                   pcPgm);

static int siCnfPrnEnv(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcPre,
   const char*                   pcOwn,
   const char*                   pcPgm);

static int siCnfPrn(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcPre);

static int siCnfClr(
   TsCnfHdl*                     psHdl);

static void vdCnfCls(
   TsCnfHdl*                     psHdl);

/* Implementierung der externen Funktionen ****************************/

extern const char* pcCleVersion2(const int l, const int s, char* b, const char* is)
{
   if (is!=NULL) {
      int  i;
      int  k=strlen(is);
      char acIndent[(l)*k+1];
      for (acIndent[0]=0x00,i=0;i<(l);i++) {
         snprintc(acIndent,sizeof(acIndent),"%s",is);
      }
      snprintc(b,s,"%s FLAM-CLE VERSION: %s-%d BUILD: %s %s %s\n",acIndent,CLE_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
   } else {
      snprintc(b,s,"%2.2d FLAM-CLE VERSION: %s-%d BUILD: %s %s %s\n",l,CLE_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
   }
   return(pcClpVersion2(l+1,s,b,is));
}
extern const char* pcCleVersion(const int l, const int s, char* b)
{
   return(pcCleVersion2(l,s,b,NULL));
}

extern const char* pcCleAbout2(const int l, const int s, char* b,const char* is)
{
   if (is!=NULL) {
      int  i;
      int  k=strlen(is);
      char acIndent[(l)*k+1];
      for (acIndent[0]=0x00,i=0;i<(l);i++) {
         snprintc(acIndent,sizeof(acIndent),"%s",is);
      }
      snprintc(b,s,"%s Frankenstein Limes Command Line Execution (FLAM-CLE)\n",acIndent);
      snprintc(b,s,"%s   Version: %s-%d Build: %s %s %s\n",acIndent,CLE_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
      snprintc(b,s,"%s   Copyright (C) limes datentechnik (R) gmbh\n",acIndent);
      snprintc(b,s,"%s   This library is open source from the FLAM(R) project: https://flam.de\n",acIndent);
      snprintc(b,s,"%s   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n",acIndent);
      snprintc(b,s,"%s This library uses the internal library below:\n",acIndent);
   } else {
      snprintc(b,s,
            "%2.2d Frankenstein Limes Command Line Execution (FLAM-CLE)\n"
            "   Version: %s-%u Build: %s %s %s\n"
            "   Copyright (C) limes datentechnik (R) gmbh\n"
            "   This library is open source from the FLAM(R) project: https://flam.de\n"
            "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
            "This library uses the internal library below:\n"
            ,l,CLE_VSN_STR,__BUILDNR__,BUILD_STR,__DATE__,__TIME__);
   }
   return(pcClpAbout2(l+1,s,b,is));
}
extern const char* pcCleAbout(const int l, const int s, char* b)
{
   return(pcCleAbout2(l,s,b,NULL));
}

static inline const char* pcMapDocTyp(const unsigned int uiTyp) {
   switch(uiTyp) {
      case CLE_DOCTYP_COVER:        return("CLE_DOCTYP_COVER");
      case CLE_DOCTYP_CHAPTER:      return("CLE_DOCTYP_CHAPTER");
      case CLE_DOCTYP_USEDENVAR:    return("CLE_DOCTYP_USEDENVAR");
      case CLE_DOCTYP_BUILTIN:      return("CLE_DOCTYP_BUILTIN");
      case CLE_DOCTYP_PROGRAM:      return("CLE_DOCTYP_PROGRAM");
      case CLE_DOCTYP_PGMSYNOPSIS:  return("CLE_DOCTYP_PGMSYNOPSIS");
      case CLE_DOCTYP_PGMSYNTAX:    return("CLE_DOCTYP_PGMSYNTAX");
      case CLE_DOCTYP_PGMHELP:      return("CLE_DOCTYP_PGMHELP");
      case CLE_DOCTYP_COMMANDS:     return("CLE_DOCTYP_COMMANDS");
      case CLE_DOCTYP_OTHERCLP:     return("CLE_DOCTYP_OTHERCLP");
      case CLE_DOCTYP_LEXEMES:      return("CLE_DOCTYP_LEXEMES");
      case CLE_DOCTYP_GRAMMAR:      return("CLE_DOCTYP_GRAMMAR");
      case CLE_DOCTYP_PROPREMAIN:   return("CLE_DOCTYP_PROPREMAIN");
      case CLE_DOCTYP_PROPDEFAULTS: return("CLE_DOCTYP_PROPDEFAULTS");
      case CLE_DOCTYP_SPECIALCODES: return("CLE_DOCTYP_SPECIALCODES");
      case CLE_DOCTYP_REASONCODES:  return("CLE_DOCTYP_REASONCODES");
      case CLE_DOCTYP_VERSION:      return("CLE_DOCTYP_VERSION");
      case CLE_DOCTYP_ABOUT:        return("CLE_DOCTYP_ABOUT");
      default:                      return("UNKOWN");
   }
}

static inline const char* pcMapCleRtc(int siRtc) {
   switch(siRtc) {
   case CLERTC_OK:  return("o.k.");
   case CLERTC_INF: return("info");
   case CLERTC_FIN: return("finish function failed");
   case CLERTC_WRN: return("warning");
   case CLERTC_RUN: return("run failed");
   case CLERTC_MAP: return("mapping failed");
   case CLERTC_SYN: return("syntax error");
   case CLERTC_CMD: return("command failed");
   case CLERTC_INI: return("initialization failed");
   case CLERTC_CFG: return("configuration wrong");
   case CLERTC_TAB: return("table error (CLP) ");
   case CLERTC_SYS: return("system error");
   case CLERTC_ACS: return("license/access error");
   case CLERTC_ITF: return("interface error");
   case CLERTC_MEM: return("memory/allocation failed");
   case CLERTC_FAT: return("fatal error");
   default        : return("special condition code");
   }
}

static int siClpFile2String(void* gbl, const void* hdl, const char* filename, char** buf, int* bufsize, char* errmsg, const int msgsiz) {
   (void)gbl;
   char* pcFil=dcpmapfil(filename);
   if (pcFil==NULL) { return(-1); }
   int siErr=file2str(hdl, pcFil, buf, bufsize, errmsg, msgsiz);
   free(pcFil);
   return(siErr);
}

#undef ELIMIT
#define ELIMIT(e) (((e) > siMaxCC) ? siMaxCC : (((e) < siMinCC) ? 0 : (e)))

#undef  ERROR
#define ERROR(x) do { \
   int r = siCleEndExecution((x),psCnf,pfTrh,pfDoc,pfPro,ppArg,pvHdl);\
   if (r) {\
      if (pcBld!=NULL && *pcBld) {\
         if (pfErr!=NULL) { efprintf(pfErr,"%s Program '%s' (Build: %s (%s %s)) ends with completion code %d (%s)\n",cstime(0,acTs),(pcProgram!=NULL)?pcProgram:"-NULL-",pcBld,__DATE__,__TIME__,r,pcMapCleRtc(r)); }\
      } else {\
         if (pfErr!=NULL) { efprintf(pfErr,"%s Program '%s' (Build: %s %s) ends with completion code %d (%s)\n",cstime(0,acTs),(pcProgram!=NULL)?pcProgram:"-NULL-",__DATE__,__TIME__,r,pcMapCleRtc(r)); }\
      }\
   } else {\
      if (pcBld!=NULL && *pcBld) {\
         if (pfOut!=NULL) { efprintf(pfOut,"%s Program '%s' (Build: %s (%s %s)) run successfully\n",cstime(0,acTs),(pcProgram!=NULL)?pcProgram:"-NULL-",pcBld,__DATE__,__TIME__); }\
      } else {\
         if (pfOut!=NULL) { efprintf(pfOut,"%s Program '%s' (Build: %s %s) run successfully\n",cstime(0,acTs),(pcProgram!=NULL)?pcProgram:"-NULL-",__DATE__,__TIME__); }\
      }\
   }\
   if (pfOut!=NULL) { efprintf(pfOut,"%s Total runtime %us / Total CPU time %7.5fs\n",cstime(0,acTs),(U32)(time(NULL)-uiTime),((double)(clock()-uiClock))/CLOCKS_PER_SEC); }\
   SAFE_FREE(pcHom); \
   SAFE_FREE(pcPgm); \
   SAFE_FREE(pcPgu); \
   SAFE_FREE(pcPgl); \
   SAFE_FREE(pcCnf); \
   SAFE_FREE(pcOwn); \
   SAFE_FREE(pcFil); \
   if (pvGbl!=NULL && pfCls!=NULL) { pfCls(pvGbl); }\
   return(r); \
} while(0)

/**********************************************************************/

static inline TsCnfHdl* psOpenConfig(FILE* pfOut, FILE* pfErr, const char* pcHom, const char* pcPgm, const char* pcPgu, const char* pcPgl, const int isCas) {
   size_t         szFil=0;
   size_t         szCnf=0;
   char*          pcCnf=NULL;
   char*          pcFil=NULL;
   TsCnfHdl*      psCnf;
   const char*    m;

   srprintf(&pcCnf,&szCnf,strlen(pcPgu),"%s_CONFIG_FILE",pcPgu);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) {
         fprintf(pfErr,"Memory allocation for %s_CONFIG_FILE  failed\n",pcPgu);
      }
      return(NULL);
   }
   m=GETENV(pcCnf);

   if (m==NULL || *m==0x00) {
   #ifdef __ZOS__
      {
         int   i,j;
         char  acUsr[16];
         srprintf(&pcFil,&szFil,strlen(pcPgu)+8,"'%s.",userid(sizeof(acUsr),acUsr));
         if (pcFil==NULL) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            }
            free(pcCnf);
            return(NULL);
         }
         for (j=strlen(pcFil),i=0;i<8 && pcPgu[i];i++) {
            if (isalnum(pcPgu[i])) {
               pcFil[j]=pcPgu[i];
               j++;
            }
         }
         strcpy(pcFil+j,".CONFIG'");
      }
   #else
      if (pcHom[0]) {
         srprintf(&pcFil,&szFil,strlen(pcPgl),".%s.config",pcPgl);
         if (pcFil==NULL) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            }
            free(pcCnf);
            return(NULL);
         }
         FILE* pfTmp=fopen_nowarn(pcFil,"r");
         if (pfTmp==NULL) {
            srprintf(&pcFil,&szFil,strlen(pcHom)+strlen(pcPgl),"%s.%s.config",pcHom,pcPgl);
            if (pcFil==NULL) {
               if (pfErr!=NULL) {
                  fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
               }
               free(pcCnf);
               return(NULL);
            }
            if (pfOut!=NULL) {
               char  acTs[24];
               fprintf(pfOut,"%s Use default configuration file (%s) in home directory\n",cstime(0,acTs),pcFil);
            }
         } else {
            fclose_unchecked(pfTmp);
            if (pfOut!=NULL) {
               char  acTs[24];
               fprintf(pfOut,"%s Use existing configuration file (%s) in working directory\n",cstime(0,acTs),pcFil);
            }
         }
      } else {
         srprintf(&pcFil,&szFil,strlen(pcPgl),".%s.config",pcPgl);
         if (pcFil==NULL) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            }
            free(pcCnf);
            return(NULL);
         }
         if (pfOut!=NULL) {
            char  acTs[24];
            fprintf(pfOut,"%s Use default configuration file (%s) in working directory\n",cstime(0,acTs),pcFil);
         }
      }
   #endif
   } else {
      srprintf(&pcFil,&szFil,strlen(m),"%s",m);
      if (pcFil==NULL) {
         if (pfErr!=NULL) {
            fprintf(pfErr,"Memory allocation for configuration file name (%s) from environment variable failed\n",pcCnf);
         }
         free(pcCnf);
         return(NULL);
      }
      if (pfOut!=NULL) {
         char  acTs[24];
         fprintf(pfOut,"%s Using configuration file (%s) defined by environment variable (%s)\n",cstime(0,acTs),pcFil,pcCnf);
      }
   }

   psCnf=psCnfOpn(pfErr,isCas,pcPgm,pcFil);
   free(pcFil);
   free(pcCnf);
   return(psCnf);
}

/**********************************************************************/

static void vdPrintEnvars(FILE* pfDoc,const TsClpArgument* psEnvTab) {
   if (pfDoc!=NULL) {
      for (const TsClpArgument* p=psEnvTab;p->pcKyw!=NULL;p++) {
         if (p->pcMan!=NULL) {
            if (strchr(p->pcMan,'\n')!=NULL) {
               fprintf(pfDoc,"* %s - %s\n%s\n",p->pcKyw,p->pcHlp,p->pcMan);
            } else {
               fprintf(pfDoc,"* %s (%s) - %s\n",p->pcKyw,p->pcMan,p->pcHlp);
            }
         } else {
            fprintf(pfDoc,"* %s - %s\n",p->pcKyw,p->pcHlp);
         }
         for (const TsClpArgument* s=p->psTab; s!=NULL && s->pcKyw!=NULL; s++) {
            fprintf(pfDoc,"** %s - %s\n",s->pcKyw,s->pcHlp);
         }
      }
   }
}

static int siPrintChapter(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcAut, const char* pcAdr, const char* pcBld, const int isHdr, const int isNbr, const int isIdt) {
   if (psDoc->pcHdl!=NULL && *psDoc->pcHdl) {
      if (psDoc->pcKyw!=NULL && *psDoc->pcKyw) {
         char acKyw[strlen(psDoc->pcKyw)+4];
         snprintf(acKyw,sizeof(acKyw),"[%s]",psDoc->pcKyw);/*nodiac*/
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,acKyw,1);
      }
      if (psDoc->pcAnc!=NULL && *psDoc->pcAnc) {
         char acAnc[strlen(psDoc->pcAnc)+8];
         snprintf(acAnc,sizeof(acAnc),"[[%s]]",psDoc->pcAnc);/*nodiac*/
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,acAnc,1);
      }
      for (unsigned int i=0;i<psDoc->uiLev;i++) {
         efprintf(pfDoc,"=");
      }
      if (isNbr && psDoc->pcNum!=NULL && *psDoc->pcNum) { efprintf(pfDoc," %s",psDoc->pcNum); }
      efprintf(pfDoc," ");
      if (isHdr && pcAut!=NULL && *pcAut) {
         char acHdr[1024];
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,psDoc->pcHdl,1);
         if (pcAdr!=NULL && *pcAdr) {
            snprintf(acHdr,sizeof(acHdr),"%s <%s>\nv&{BLD}, &{DATE}: &{state}.",pcAut,pcAdr); /*nodiac*/
         } else {
            snprintf(acHdr,sizeof(acHdr),"%s\nv&{BLD}, &{DATE}: &{state}.",pcAut); /*nodiac*/
         }
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,acHdr,2);
      } else {
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,psDoc->pcHdl,2);
      }
      if (isIdt && psDoc->pcIdt!=NULL && *psDoc->pcIdt) {
         const char* pcTmp;
         const char* pcIdt=psDoc->pcIdt;
         for (pcTmp=strchr(pcIdt,'\n');pcTmp!=NULL;pcTmp=strchr(pcIdt,'\n')) {
            char acIdt[strlen(pcIdt)+16];
            snprintf(acIdt,sizeof(acIdt),"indexterm:[%.*s]",(int)(pcTmp-pcIdt),pcIdt);/*nodiac*/
            fprintm(pfDoc,pcOwn,pcPgm,pcBld,acIdt,1);
            pcIdt=pcTmp+1;
         }
         char acIdt[strlen(pcIdt)+16];
         snprintf(acIdt,sizeof(acIdt),"indexterm:[%s]",pcIdt);/*nodiac*/
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,acIdt,2);
      }
      if (psDoc->pcMan!=NULL && *psDoc->pcMan) {
         fprintm(pfDoc,pcOwn,pcPgm,pcBld,psDoc->pcMan,2);
      } else {
         if (pfErr!=NULL) { fprintf(pfErr,"No manual page for chapter '%s' provided\n",psDoc->pcHdl); }
         return(CLERTC_TAB);

      }
      return(CLERTC_OK);
   } else {
      if (pfErr!=NULL) { fprintf(pfErr,"No head line for chapter defined\n"); }
      return(CLERTC_TAB);
   }
}

static int siClePrintCover(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcAut, const char* pcAdr, const char* pcBld, const int isHdr, const int isNbr, const int isIdt) {
   if (psDoc->uiLev!=1) {
      if (pfErr!=NULL) { fprintf(pfErr,"The level (%u) for the cover page must be 1\n",psDoc->uiLev); }
      return(CLERTC_ITF);
   }
   if (isHdr) {
      efprintf(pfDoc,":doctype: book\n\n");
   }
   return(siPrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcAut,pcAdr,pcBld,isHdr,isNbr,isIdt));
}


static int siClePrintChapter(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld, const int isNbr, const int isIdt) {
   if (psDoc->uiLev<2 || psDoc->uiLev>5) {
      if (pfErr!=NULL) { fprintf(pfErr,"The level (%u) for a chapter must be between 2 and 6\n",psDoc->uiLev); }
      return(CLERTC_ITF);
   }
   return(siPrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,NULL,NULL,pcBld,FALSE,isNbr,isIdt));
}

static int siClePrintUsedEnvar(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld, const int isNbr, const int isIdt,const TsClpArgument* psEnvTab, const C08* pcEnvTxt) {
   U32 i;
   C08 acPGM[strlen(pcPgm)+1];
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   for (i=0;pcPgm[i];i++) {
      acPGM[i]=toupper(pcPgm[i]);
   }
   acPGM[i]=0x00;
   efprintf(pfDoc,"Below you can find the list of static environment variables.\n\n");
   vdPrintEnvars(pfDoc,psEnvTab);
   efprintf(pfDoc,
         "\nBeside the static environment variables there a lot of dynamic formed environment variables supported.\n"
         "\n"
         "* %s_CONFIG_FILE - the configuration filename (default is '%cHOME/.%s.config' on UNIX/WIN or '&SYSUID..%s.CONFIG' on mainframes)\n"
         "* %s_DEFAULT_OWNER_ID - the default owner ID (default is '%s')\n"
         "* owner_%s_command_PROPERTY_FILENAME - To override default property file names\n"
         "* path_argument - to override the hard coded default property value\n\n",acPGM,C_DLR,pcPgm,acPGM,acPGM,pcOwn,acPGM);
   if (pcEnvTxt!=NULL && *pcEnvTxt) {
      fprintm(pfDoc,pcOwn,pcPgm,pcBld,pcEnvTxt,2);
   }
   return(CLERTC_OK);
}

static int siClePrintPgmSynopsis(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld, const char* pcHlp, const int isPat, const int isNbr, const int isIdt) {
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   efprintf(pfDoc,"-----------------------------------------------------------------------\n");
   efprintf(pfDoc,"HELP:   %s\n",pcHlp);
   if (isPat) {
   efprintf(pfDoc,"PATH:   %s\n",pcOwn); }
   efprintf(pfDoc,"TYPE:   PROGRAM\n");
   efprintf(pfDoc,"SYNTAX: > %s COMMAND/FUNCTION ...\n",pcPgm);
   efprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
   return(CLERTC_OK);
}

static int siClePrintPgmSyntax(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psCmd, const TsCleBuiltin* psBif,
      const char* pcOwn, const char* pcPgm, const char* pcBld, const char* pcDep, const char* pcOpt, const char* pcDpa, const int isNbr, const int isIdt) {
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Syntax for program '%s':\n",pcPgm);
   vdPrnStaticSyntax(pfDoc,psCmd,psBif,pcPgm,pcDep,pcOpt,pcDpa);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return(CLERTC_OK);
}

static int siClePrintPgmHelp(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psCmd, const TsCleBuiltin* psBif,
      const char* pcOwn, const char* pcPgm, const char* pcBld, const char* pcDep, const int isNbr, const int isIdt) {
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Help for program '%s':\n",pcPgm);
   vdPrnStaticHelp(pfDoc,psCmd,psBif,pcPgm,pcDep);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return(CLERTC_OK);
}

static int siClePrintBuiltIn(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld, const int isPat, const int isNbr, const int isShl, const int isIdt,const TsCleBuiltin* psBif) {
   int   siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   for (int i=0;psBif[i].pcKyw!=NULL;i++) {
      char  acNum[strlen(psDoc->pcNum)+16];
      if (psDoc->pcNum!=NULL && *psDoc->pcNum) { snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,i+1); } else { snprintf(acNum,sizeof(acNum),"%d.",i+1); }
      vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,psBif[i].pcKyw,psBif[i].pcHlp,pcOwn,pcPgm,pcBld,psBif[i].pcSyn,psBif[i].pcMan,FALSE,isPat,isNbr,isShl,isIdt);
   }
   return(CLERTC_OK);
}

static int siClePrintLexemes(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld,
      const int isPfl, const int isRpl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdt) {
   void* pvHdl=NULL;
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   siErr=siCleSimpleInit(NULL,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
   if (siErr) { return(siErr); }
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Lexemes (regular expressions) for argument list or parameter file\n");
   int siRtc=siClpLexemes(pvHdl,pfDoc);
   vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return((siRtc)?CLERTC_SYN:CLERTC_OK);
}

static int siClePrintGrammar(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld,
      const int isPfl, const int isRpl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdt) {
   void* pvHdl=NULL;
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return(siErr); }
   siErr=siCleSimpleInit(NULL,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
   if (siErr) { return(siErr); }
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Grammar for argument list, parameter file or property file\n");
   int siRtc=siClpGrammar(pvHdl,pfDoc);
   vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return((siRtc)?CLERTC_SYN:CLERTC_OK);
}

static int siClePrintPreformatedText(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld, const char* pcTxt, const int isNbr, const int isIdt) {
   if (pcTxt!=NULL && *pcTxt) {
      int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
      if (siErr) { return siErr; }
      efprintf(pfDoc,"------------------------------------------------------------------------\n");
      efprintf(pfDoc,"%s",pcTxt);
      efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
      return(CLERTC_OK);
   } else {
      if (pfErr!=NULL) { fprintf(pfErr,"No pre-formatted text provided but print requested (version, about)\n"); }
      return(CLERTC_ITF);
   }
}

static int siClePrintPropRemain(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psCmd, void* pvCnf, const char* pcOwn, const char* pcPgm, const char* pcBld,
      const int isCas, const int isPfl, const int isRpl, const int siMkl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdt) {
   int         siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return siErr; }
   for (int j=0;siErr==0 && psCmd[j].pcKyw!=NULL;j++) {
      void* pvHdl=NULL;
      siErr=siClePropertyInit(NULL,psCmd[j].pfIni,psCmd[j].pvClp,pcOwn,pcPgm,pcBld,psCmd[j].pcKyw,psCmd[j].pcMan,psCmd[j].pcHlp,
                              psCmd[j].piOid,psCmd[j].psTab,isCas,isPfl,isRpl,siMkl,NULL,pfErr,NULL,
                              pcDep,pcOpt,pcEnt,(TsCnfHdl*)pvCnf,&pvHdl,NULL,NULL,NULL,NULL,siClpFile2String,NULL,NULL);
      if (siErr) {
         return(siErr);
      }
      siErr=siClpProperties(pvHdl,CLPPRO_MTD_DOC,10,psCmd[j].pcKyw,pfDoc);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   }
   return((siErr)?CLERTC_SYN:CLERTC_OK);
}

static int siClePrintPropDefaults(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psCmd, void* pvCnf, const char* pcOwn, const char* pcPgm, const char* pcBld,
      const int isCas, const int isPfl, const int isRpl, const int siMkl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdt) {
   int         siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
   if (siErr) { return siErr; }
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"\n# Property file for: %s.%s #\n",pcOwn,pcPgm);
   efprintf(pfDoc,"%s",HLP_CLE_PROPFIL);
   for (int j=0;siErr==0 && psCmd[j].pcKyw!=NULL;j++) {
      void* pvHdl=NULL;
      siErr=siClePropertyInit(NULL,psCmd[j].pfIni,psCmd[j].pvClp,pcOwn,pcPgm,pcBld,psCmd[j].pcKyw,psCmd[j].pcMan,psCmd[j].pcHlp,
            psCmd[j].piOid,psCmd[j].psTab,isCas,isPfl,isRpl,siMkl,NULL,pfErr,NULL,
            pcDep,pcOpt,pcEnt,(TsCnfHdl*)pvCnf,&pvHdl,NULL,NULL,NULL,NULL,siClpFile2String,NULL,NULL);
      if (siErr) {
         return(siErr);
      }
      siErr=siClpProperties(pvHdl,CLPPRO_MTD_SET,10,psCmd[j].pcKyw,pfDoc);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   }
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return((siErr)?CLERTC_SYN:CLERTC_OK);
}

static int siClePrintReasonCodes(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcBld, TfMsg* pfMsg, const int isNbr, const int isIdt) {
   if (pfMsg!=NULL) {
      int         r;
      const char* m;
      int         siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,pcBld,isNbr,isIdt);
      if (siErr) { return siErr; }
      for (r=1,m=pfMsg(r);m!=NULL;r++,m=pfMsg(r)) {
         if (*m) { efprintf(pfDoc,"* %d - %s\n",r,m); }
      }
      efprintf(pfDoc,"\n");
      return(CLERTC_OK);
   } else {
      if (pfErr!=NULL) { fprintf(pfErr,"The pointer to the message function to map the reason codes is NULL but print was requested\n"); }
      return(CLERTC_ITF);
   }
}

/***********************************************************************/

static int siCleWritePage(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleDocPar* psPar) {
   switch (psDoc->uiTyp) {
      case CLE_DOCTYP_COVER:        return(siClePrintCover(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcAut,psPar->pcAdr,psPar->pcBld,psPar->isHdr,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_CHAPTER:      return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_USEDENVAR:    return(siClePrintUsedEnvar(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isNbr,psPar->isIdt,psPar->psEnv,psPar->pcEnv));
      case CLE_DOCTYP_BUILTIN:      return(siClePrintBuiltIn(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isPat,psPar->isNbr,psPar->isShl,psPar->isIdt,psPar->psBif));
      case CLE_DOCTYP_PROGRAM:      return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_PGMSYNOPSIS:  return(siClePrintPgmSynopsis(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->pcHlp,psPar->isPat,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_PGMSYNTAX:    return(siClePrintPgmSyntax(pfErr,pfDoc,psDoc,psPar->psCmd,psPar->psBif,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->pcDep,psPar->pcOpt,psPar->pcDpa,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_PGMHELP:      return(siClePrintPgmHelp(pfErr,pfDoc,psDoc,psPar->psCmd,psPar->psBif,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->pcDep,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_COMMANDS:     return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_OTHERCLP:     return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_LEXEMES:      return(siClePrintLexemes(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isPfl,psPar->isRpl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_GRAMMAR:      return(siClePrintGrammar(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isPfl,psPar->isRpl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_PROPREMAIN:   return(siClePrintPropRemain(pfErr,pfDoc,psDoc,psPar->psCmd,psPar->pvCnf,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,
            psPar->isCas,psPar->isPfl,psPar->isRpl,psPar->siMkl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_PROPDEFAULTS: return(siClePrintPropDefaults(pfErr,pfDoc,psDoc,psPar->psCmd,psPar->pvCnf,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,
            psPar->isCas,psPar->isPfl,psPar->isRpl,psPar->siMkl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_SPECIALCODES: return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_REASONCODES:  return(siClePrintReasonCodes(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->pfMsg,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_VERSION:      return(siClePrintPreformatedText(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->pcVsn,psPar->isNbr,psPar->isIdt));
      case CLE_DOCTYP_ABOUT:        return(siClePrintPreformatedText(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->pcAbo,psPar->isNbr,psPar->isIdt));
      default:
         if (pfErr!=NULL) { fprintf(pfErr,"Documentation type (%u) not supported\n",psDoc->uiTyp); }
         return(CLERTC_TAB);
   }
}

static int siClePrintPage(FILE* pfOut, FILE* pfErr, const TsCleDoc* psDoc, const char* pcFil, const TsCleDocPar* psPar, void* pvPrn, TfClpPrintPage* pfPrn) {
   unsigned int i;
   if (psDoc->pcHdl==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Headline is NULL\n"); }
      return(CLERTC_TAB);
   }
   if (pfOut!=NULL) {
      fprintf(pfOut,"... print %24s ",pcMapDocTyp(psDoc->uiTyp));
      for (i=0;i<psDoc->uiLev;i++) { fprintf(pfOut,"="); }
      fprintf(pfOut," %s\n",psDoc->pcHdl);
   }
   FILE* pfDoc=fopen_tmp();
   if (pfDoc==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Open of temporary file to print manual page '%s' failed (%d - %s)\n",psDoc->pcHdl,errno,pcSysError(errno)); }
      return(CLERTC_SYS);
   }
   int siErr=siCleWritePage(pfErr,pfDoc,psDoc,psPar);
   if (siErr) {
      fclose_tmp(pfDoc);
      return(siErr);
   }
   long int s=ftell(pfDoc);
   if (s>0) {
      char* pcPge=malloc(s+1);
      if (pcPge==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for temporary file to print manual '%s' page failed\n",psDoc->pcHdl); }
         fclose_tmp(pfDoc);
         return(CLERTC_MEM);
      }
      errno=0;
      rewind(pfDoc);
      size_t r=(errno)?0:fread(pcPge,1,s,pfDoc);
      fclose_tmp(pfDoc);
      if (r!=s) {
         free(pcPge);
         if (pfErr!=NULL) { fprintf(pfErr,"Read of temporary file to print manual page '%s' failed (%d - %s)\n",psDoc->pcHdl,errno,pcSysError(errno)); }
         return(CLERTC_SYS);
      }
      pcPge[r]=0x00;

      const char* p=strchr(pcPge,'=');
      if (p==NULL) {
         free(pcPge);
         if (pfErr!=NULL) { fprintf(pfErr,"No headline found in manual page '%s' (no sign)\n",psDoc->pcHdl); }
         return(CLERTC_FAT);
      }
      while (*p=='=') { p++; }
      if (*p!=' ') {
         free(pcPge);
         if (pfErr!=NULL) { fprintf(pfErr,"No headline found in manual page '%s' (no blank after sign)\n",psDoc->pcHdl); }
         return(CLERTC_FAT);
      }
      p++;
      const char* e=strchr(p,'\n');
      if (e==NULL) {
         free(pcPge);
         if (pfErr!=NULL) { fprintf(pfErr,"No end of headline found in manual page '%s'\n",psDoc->pcHdl); }
         return(CLERTC_FAT);
      }
      int l=e-p;
      char acHdl[l+1];
      memcpy(acHdl,p,l);
      acHdl[l]=0x00;
      l=strlen(pcFil);
      char acFil[l+16];
      unsigned int uiHsh=fnvHash(l,(const unsigned char*)pcFil);
      for (i=0;i<l;i++) {
         if (pcFil[i]=='\t') {
            acFil[i]=psPar->siPs1;
         } else if (pcFil[i]=='\v') {
            acFil[i]=psPar->siPs2;
         } else if (isalnum(pcFil[i])) {
            acFil[i]=tolower(pcFil[i]);
         } else {
            acFil[i]=psPar->siPr3;
         }
      }
      acFil[i]=0x00;
      snprintc(acFil,sizeof(acFil),"%c%04x",psPar->siPr3,uiHsh&0xFFFF);
      siErr=pfPrn(pvPrn,psDoc->uiLev,acHdl,NULL,acFil,psDoc->pcMan,pcPge);
      free(pcPge);
      if (siErr) {
         if (pfErr!=NULL) { fprintf(pfErr,"Print page over call back function for command '%s' failed with %d\n",psDoc->pcHdl,siErr); }
         return(CLERTC_MEM);
      }
   }
   return(CLERTC_OK);
}

/***********************************************************************/

static void* pfLoadHtmlDoc(TfCleOpenPrint** ppHtmlOpn, TfClpPrintPage** ppHtmlPrn, TfCleClosePrint** ppHtmlCls) {
#ifdef __WIN__
   void* pvHtmlDoc=LoadLibrary(TEXT("htmldoc.dll"));
   if (pvHtmlDoc==NULL) return(NULL);
   *ppHtmlOpn=(TfCleOpenPrint*)GetProcAddress(pvHtmlDoc,  "opnHtmlDoc");
   *ppHtmlPrn=(TfClpPrintPage*)GetProcAddress(pvHtmlDoc,  "prnHtmlDoc");
   *ppHtmlCls=(TfCleClosePrint*)GetProcAddress(pvHtmlDoc, "clsHtmlDoc");
#else
   void* pvHtmlDoc=dlopen("libhtmldoc.so",RTLD_LAZY);
   if (pvHtmlDoc==NULL) {
      pvHtmlDoc=dlopen("libhtmldoc5.so",RTLD_LAZY);
      if (pvHtmlDoc==NULL) { return(NULL); }
   }
   *ppHtmlOpn=(TfCleOpenPrint* )dlsym(pvHtmlDoc, "opnHtmlDoc");
   *ppHtmlPrn=(TfClpPrintPage* )dlsym(pvHtmlDoc, "prnHtmlDoc");
   *ppHtmlCls=(TfCleClosePrint*)dlsym(pvHtmlDoc, "clsHtmlDoc");
#endif
   return(pvHtmlDoc);
}

static void vdFreeHtmlDoc(void** ppLib) {
   if (ppLib!=NULL && *ppLib!=NULL) {
#ifdef __WIN__
      FreeLibrary(*ppLib);
#else
      dlclose(*ppLib);
#endif
      *ppLib=NULL;
   }
}

static int siPrintPage(void* pvHdl, const int siLev, const char* pcHdl, const char* pcPat, const char* pcFil, const char* pcOrg, const char* pcPge) {
   int         l=strlen(pcPge);
   int         r=fwrite(pcPge,1,l,(FILE*)pvHdl);
   return(r-l);
}

static int siPrintDocu(
   void*                         pvGbl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   const TsCleDoc*               psDoc,
   TsCleDocPar*                  psPar,
   void*                         pvPrn,
   TfClpPrintPage*               pfPrn)
{
   if (psDoc!=NULL) {
      const char* apPat[8]={"","","","","","","",""};
      for (int i=0; psDoc[i].uiTyp && psDoc[i].uiLev>0 && psDoc[i].uiLev<7; i++) {
         int   x=0;
         char  acFil[1024]="";
         apPat[psDoc[i].uiLev]=psDoc[i].pcHdl;
         for (int j=1;j<=psDoc[i].uiLev;j++) {
            if (j>1) {
               if (x<sizeof(acFil)-1) {
                  acFil[x]='\t';
                  x++;
               }
            }
            for (int k=0;apPat[j][k];k++) {
               if (x<sizeof(acFil)-1) {
                  acFil[x]=apPat[j][k];
                  x++;
               }
            }
         }
         acFil[x]=0x00;
         int siErr=siClePrintPage(pfOut,pfErr,psDoc+i,acFil,psPar,pvPrn,pfPrn);
         if (siErr) { return siErr; }
         if (psDoc[i].uiTyp==CLE_DOCTYP_COMMANDS) {
            for (int j=0;psPar->psCmd[j].pcKyw!=NULL;j++) {
               if (psPar->psCmd[j].siFlg) {
                  void* pvClp=NULL;
                  char  acNum[64];
                  siErr=siCleCommandInit(pvGbl,psPar->psCmd[j].pfIni,psPar->psCmd[j].pvClp,psPar->pcOwn,psPar->pcPgm,psPar->pcBld,psPar->psCmd[j].pcKyw,psPar->psCmd[j].pcMan,psPar->psCmd[j].pcHlp,psPar->psCmd[j].piOid,psPar->psCmd[j].psTab,
                        psPar->isCas,psPar->isPfl,psPar->isRpl,psPar->siMkl,pfOut,pfErr,NULL,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->pvCnf,&pvClp,psPar->pfMsg,psPar->pvF2S,psPar->pfF2S,psPar->pvSaf,psPar->pfSaf,NULL);
                  if (siErr) { return siErr; }
                  if (psDoc[i].pcNum!=NULL && *psDoc[i].pcNum) {
                     snprintf(acNum,sizeof(acNum),"%s%d.",psDoc[i].pcNum,j+1);
                  } else {
                     snprintf(acNum,sizeof(acNum),"%d.",j+1);
                  }
                  siErr=siClpPrint(pvClp,acFil,acNum,"Command",TRUE,psPar->isDep,psPar->isAnc,psPar->isNbr,psPar->isShl,psPar->isIdt,psPar->isPat,
                        psDoc[i].uiLev+1,psPar->siPs1,psPar->siPs2,psPar->siPr3,pvPrn,pfPrn);
                  vdClpClose(pvClp,CLPCLS_MTD_ALL); pvClp=NULL;
                  if (siErr<0) {
                     if (pfErr!=NULL) { fprintf(pfErr,"Creation of documentation for command '%s' failed\n",psPar->psCmd[j].pcKyw); }
                     return(CLERTC_SYN);
                  }
               }
            }
         } else if (psDoc[i].uiTyp==CLE_DOCTYP_OTHERCLP) {
            if (psPar->psOth==NULL) {
               if (pfErr!=NULL) { fprintf(pfErr,"The pointer to the list of other CLP strings is NULL but DOCTYP OTHERCLP requested\n"); }
               return(CLERTC_ITF);
            }
            for (int j=0;psPar->psOth[j].pcKyw!=NULL;j++) {
               void* pvClp=NULL;
               char acNum[64];
               pvClp=pvClpOpen(psPar->isCas,psPar->isPfl,psPar->isRpl,psPar->siMkl,psPar->pcOwn,psPar->psOth[j].pcRot,psPar->pcBld,psPar->psOth[j].pcKyw,psPar->psOth[j].pcMan,psPar->psOth[j].pcHlp,psPar->psOth[j].isOvl,
                               psPar->psOth[j].psTab,NULL,pfOut,pfErr,NULL,NULL,NULL,NULL,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,NULL,pvGbl,psPar->pvF2S,psPar->pfF2S,psPar->pvSaf,psPar->pfSaf);
               if (pvClp==NULL) {
                  if (pfErr!=NULL) { fprintf(pfErr,"Open of parser for CLP string of appendix '%s' failed\n",psPar->psOth[j].pcKyw); }
                  return(CLERTC_TAB);
               }
               if (psDoc[i].pcNum!=NULL && *psDoc[i].pcNum) {
                  snprintf(acNum,sizeof(acNum),"%s%d.",psDoc[i].pcNum,j+1);
               } else {
                  snprintf(acNum,sizeof(acNum),"%d.",j+1);
               }
               siErr=siClpPrint(pvClp,acFil,acNum,"String",FALSE,psPar->isDep,psPar->isAnc,psPar->isNbr,psPar->isShl,psPar->isIdt,psPar->isPat,
                     psDoc[i].uiLev+1,psPar->siPs1,psPar->siPs2,psPar->siPr3,pvPrn,pfPrn);
               vdClpClose(pvClp,CLPCLS_MTD_ALL); pvClp=NULL;
               if (siErr<0) {
                  if (pfErr!=NULL) { fprintf(pfErr,"Creation of documentation for other CLP string '%s' failed\n",psPar->psOth[j].pcKyw); }
                  return(CLERTC_SYN);
               }
            }
         }
      }
   } else {
      if (pfErr!=NULL) { fprintf(pfErr,"No table for documentation generation given\n"); }
      return(CLERTC_TAB);
   }
   return(CLERTC_OK);
}

/*********************************************************************/

CLPCONTAB_OPN(asCleYesOnNoOff) = {
   CLPCONTAB_ENVAR("YES", "Yes, do it")
   CLPCONTAB_ENVAR("ON" , "Activate it")
   CLPCONTAB_ENVAR("NO" , "No, don't do it")
   CLPCONTAB_ENVAR("OFF", "Deactivate it")
   CLPCONTAB_CLS
};

CLPENVTAB_OPN(asCleEnvVarTab) = {
   CLPENVTAB_ENTRY("LANG"                 ,CLPFLG_NON,NULL           ,NULL,NULL,"To determine the CCSID on EBCDIC systems")
   CLPENVTAB_ENTRY("HOME"                 ,CLPFLG_NON,NULL           ,NULL,NULL,"To determine the home directory on UNIX/WIN")
   CLPENVTAB_ENTRY("USER"                 ,CLPFLG_NON,NULL           ,NULL,NULL,"To determine the current user id on UNIX/WIN")
   CLPENVTAB_ENTRY("ENVID"                ,CLPFLG_NON,NULL           ,NULL,NULL,"For the current environment qualifier (D/T/P) if key label template mapping used (default is 'T')")
   CLPENVTAB_ENTRY("OWNERID"              ,CLPFLG_NON,NULL           ,NULL,NULL,"Used for current owner if not already defined")
   CLPENVTAB_ENTRY("CLE_QUIET"            ,CLPFLG_SEL,asCleYesOnNoOff,NULL,NULL,"Disables the normal log output of the command line executer (must set to YES or ON)")
   CLPENVTAB_ENTRY("CLE_SILENT"           ,CLPFLG_SEL,asCleYesOnNoOff,NULL,NULL,"Disables log and errors messages of the command line executer (must set to YES or ON)")
   CLPENVTAB_ENTRY("CLP_NOW"              ,CLPFLG_NON,NULL           ,NULL,NULL,"The current point in time used for predefined constants (0tYYYY/MM/DD.HH:MM:SS)")
   CLPENVTAB_ENTRY("CLP_STRING_CCSID"     ,CLPFLG_NON,NULL           ,NULL,NULL,"CCSID used for interpretation of critical punctuation character on EBCDIC systems (default is taken from LANG)")
   CLPENVTAB_ENTRY("CLP_DEFAULT_DCB"      ,CLPFLG_NON,NULL           ,NULL,NULL,"The default record format and record length in C file mode format (default is 'recfm=VB, lrecl=516' only for z/OS)")
   CLPENVTAB_ENTRY("CLP_SYMTAB_STATISTICS",CLPFLG_SEL,asCleYesOnNoOff,NULL,NULL,"Print symbol table statistics to STDERR stream at close (must set to YES or ON)")
   CLPENVTAB_ENTRY("CLP_MALLOC_STATISTICS",CLPFLG_SEL,asCleYesOnNoOff,NULL,NULL,"Print memory allocation statistics to STDERR stream at close (must set to YES or ON)")
   CLPENVTAB_CLS
};

/***********************************************************************/

/**
 * @brief Keyword entry for abbreviation matching
 */
typedef struct CleKywEntry {
   const char*                   pcKyw;     /* Keyword string */
   int                           siType;    /* 1=Built-in, 2=Command */
   int                           siIndex;   /* Index in original table */
   int                           siMinLen;  /* Minimum unique abbreviation length */
} TsCleKywEntry;

/**
 * @brief Find a command or built-in function with abbreviation support
 *
 * This function searches for a command or built-in function that matches
 * the provided input string. It supports abbreviations with a minimum
 * length requirement calculated to ensure uniqueness among all keywords.
 * The function also handles stop characters (=, (, .) similar to strxcmp.
 *
 * @param[in]  isCas     Case-sensitive flag (TRUE=case-sensitive)
 * @param[in]  pcInput   Input command string (from user, may contain =, (, .)
 * @param[in]  psCmd     Command table (or NULL)
 * @param[in]  psBif     Built-in function table (or NULL)
 * @param[in]  siMinLen  Global minimum abbreviation length (typically 3)
 * @param[out] piType    Output: 0=not found, 1=Built-in, 2=Command
 * @param[out] piIndex   Output: Index in the corresponding table
 * @param[out] piMatchLen Output: Number of matched characters (0 if not found)
 *
 * @return 0=uniquely found, 1=ambiguous, -1=not found, -2=memory error
 */
static int siCleFindCommand(
   const int                     isCas,
   const char*                   pcInput,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const int                     siMinLen,
   int*                          piType,
   int*                          piIndex,
   int*                          piMatchLen)
{
   int i, j, k, siCnt=0, siMatch=0;
   int l=(int)strlen(pcInput);
   int siInputLen=l;
   const char* pcStop=NULL;
   TsCleKywEntry* psKyw=NULL;
   const TsCleKywEntry* psEntry=NULL;

   /* Initialize output parameters */
   *piType = 0;
   *piIndex = -1;
   *piMatchLen = 0;

   /* Find stop characters in input (=, (, .) */
   pcStop=strpbrk(pcInput,"=(.");
   if (pcStop != NULL) {
      siInputLen=(int)(pcStop - pcInput);
      if (siInputLen < 1) {
         return(-1);  /* Invalid input */
      }
   }

   /* Count total number of keywords */
   if (psBif != NULL) {
      for (i=0; psBif[i].pcKyw != NULL; i++) {
         if (psBif[i].isBif) siCnt++;
      }
   }
   if (psCmd != NULL) {
      for (i=0; psCmd[i].pcKyw != NULL; i++) {
         siCnt++;
      }
   }

   if (siCnt == 0) {
      return(-1);
   }

   /* Allocate keyword table */
   psKyw=(TsCleKywEntry*)malloc(siCnt * sizeof(TsCleKywEntry));
   if (psKyw == NULL) {
      return(-1);
   }

   /* Build combined keyword table */
   siCnt=0;
   if (psBif != NULL) {
      for (i=0; psBif[i].pcKyw != NULL; i++) {
         if (psBif[i].isBif) {
            psKyw[siCnt].pcKyw   = psBif[i].pcKyw;
            psKyw[siCnt].siType  = 1;
            psKyw[siCnt].siIndex = i;
            psKyw[siCnt].siMinLen= siMinLen;
            siCnt++;
         }
      }
   }
   if (psCmd != NULL) {
      for (i=0; psCmd[i].pcKyw != NULL; i++) {
         psKyw[siCnt].pcKyw   = psCmd[i].pcKyw;
         psKyw[siCnt].siType  = 2;
         psKyw[siCnt].siIndex = i;
         psKyw[siCnt].siMinLen= siMinLen;
         siCnt++;
      }
   }

   /* Calculate minimum unique length for each keyword */
   for (i=0; i<siCnt; i++) {
      int siLenKyw1=(int)strlen(psKyw[i].pcKyw);
      int siMinUnique=siMinLen;

      for (j=0; j<siCnt; j++) {
         if (i == j) continue;

         int siLenKyw2=(int)strlen(psKyw[j].pcKyw);
         int siLenMin=(siLenKyw1 < siLenKyw2) ? siLenKyw1 : siLenKyw2;

         /* Find first differing character position */
         for (k=0; k<siLenMin; k++) {
            int c1=isCas ? psKyw[i].pcKyw[k] : toupper((unsigned char)psKyw[i].pcKyw[k]);
            int c2=isCas ? psKyw[j].pcKyw[k] : toupper((unsigned char)psKyw[j].pcKyw[k]);
            if (c1 != c2) break;
         }

         /* Update minimum unique length */
         if (k > 0 && k < siLenKyw1) {
            int siNeed=k+1;
            if (siNeed > siMinUnique) {
               siMinUnique=siNeed;
            }
         }
      }

      psKyw[i].siMinLen=siMinUnique;
   }

   /* Check for exact match first */
   for (i=0; i<siCnt; i++) {
      int siLenKyw=(int)strlen(psKyw[i].pcKyw);

      if (siInputLen == siLenKyw && strxcmp(isCas, pcInput, psKyw[i].pcKyw, siInputLen, 0, FALSE) == 0) {
         *piType = psKyw[i].siType;
         *piIndex = psKyw[i].siIndex;
         *piMatchLen = siInputLen;
         free(psKyw);
         return(0);
      }
   }

   /* Check for abbreviation matches */
   for (i=0; i<siCnt; i++) {
      int siLenKyw=(int)strlen(psKyw[i].pcKyw);

      /* Input must be at least minimum length and shorter than full keyword */
      if (siInputLen >= psKyw[i].siMinLen && siInputLen < siLenKyw) {
         /* Check if input matches keyword prefix with minimum length requirement */
         if (strxcmp(isCas, pcInput, psKyw[i].pcKyw, psKyw[i].siMinLen, 0, FALSE) == 0) {
            /* Verify remaining characters also match */
            int isMatch=1;
            for (k=psKyw[i].siMinLen; k<siInputLen; k++) {
               int c1=isCas ? pcInput[k] : toupper((unsigned char)pcInput[k]);
               int c2=isCas ? psKyw[i].pcKyw[k] : toupper((unsigned char)psKyw[i].pcKyw[k]);
               if (c1 != c2) {
                  isMatch=0;
                  break;
               }
            }
            if (isMatch) {
               siMatch++;
               psEntry = &psKyw[i];
            }
         }
      }
   }

   /* Evaluate results */
   if (siMatch == 1) {
      *piType = psEntry->siType;
      *piIndex = psEntry->siIndex;
      *piMatchLen = siInputLen;
      free(psKyw);
      return(0);
   } else if (siMatch > 1) {
      free(psKyw);
      return(1);  /* Ambiguous */
   }

   free(psKyw);
   return(-1);  /* Not found */
}

/* Built-in function implementations **************************************/

static int siCleBuiltinLicense(TsCleBuiltinContext* psCtx)
{
   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "License of program '%s':\n", psCtx->pcPgm);
      }
      if (psCtx->pcLic == NULL) {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "No license information available\n");
         }
      } else {
         fprintf(psCtx->pfStd, "%s\n", psCtx->pcLic);
      }
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'LICENSE' not valid\n");
      fprintf(psCtx->pfErr, "%s %s LICENSE\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

static int siCleBuiltinVersion(TsCleBuiltinContext* psCtx)
{
   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Version for program '%s':\n", psCtx->pcPgm);
      }
      if (psCtx->pcVsn == NULL) {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "No version information available\n");
         }
      } else {
         fprintf(psCtx->pfStd, "%s\n", psCtx->pcVsn);
      }
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'VERSION' not valid\n");
      fprintf(psCtx->pfErr, "%s %s VERSION\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

static int siCleBuiltinAbout(TsCleBuiltinContext* psCtx)
{
   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "About program '%s':\n", psCtx->pcPgm);
      }
      if (psCtx->pcAbo == NULL) {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "No about information available\n");
         }
      } else {
         fprintf(psCtx->pfStd, "%s\n", psCtx->pcAbo);
      }
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'ABOUT' not valid\n");
      fprintf(psCtx->pfErr, "%s %s ABOUT\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

static int siCleBuiltinLexemes(TsCleBuiltinContext* psCtx)
{
   int siErr;

   if (psCtx->argc == 2) {
      siErr = siCleSimpleInit(psCtx->pfOut, psCtx->pfErr, psCtx->isPfl, psCtx->isRpl, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->ppvHdl);
      if (siErr) {
         return(siErr);
      }
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Lexemes (regular expressions) for argument list or parameter file:\n");
      }
      siErr = siClpLexemes(*(psCtx->ppvHdl), psCtx->pfStd);
      if (siErr < 0) {
         return(CLERTC_SYN);
      } else {
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'LEXEMES' not valid\n");
      fprintf(psCtx->pfErr, "%s %s LEXEMES\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

static int siCleBuiltinGrammar(TsCleBuiltinContext* psCtx)
{
   int siErr;

   if (psCtx->argc == 2) {
      siErr = siCleSimpleInit(psCtx->pfOut, psCtx->pfErr, psCtx->isPfl, psCtx->isRpl, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->ppvHdl);
      if (siErr) {
         return(siErr);
      }
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Grammar for argument list, parameter file or property file\n");
      }
      siErr = siClpGrammar(*(psCtx->ppvHdl), psCtx->pfStd);
      if (siErr < 0) {
         return(CLERTC_SYN);
      } else {
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'GRAMMAR' not valid\n");
      fprintf(psCtx->pfErr, "%s %s GRAMMAR\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

static int siCleBuiltinErrors(TsCleBuiltinContext* psCtx)
{
   int i;
   const char* m;
   TfMsg* pfMsg;

   if (psCtx->argc == 2) {
      efprintf(psCtx->pfStd, "\n=Return/condition/exit codes of the executable\n\n");
      fprintm(psCtx->pfStd, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, MAN_CLEP_APPENDIX_RETURNCODES, 1);
      if (psCtx->pcSccMan != NULL && *psCtx->pcSccMan) {
         efprintf(psCtx->pfStd, "\n==Special condition codes\n\n");
         fprintm(psCtx->pfStd, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCtx->pcSccMan, 1);
      }
      pfMsg = (TfMsg*)psCtx->pfMsg;
      if (pfMsg != NULL) {
         efprintf(psCtx->pfStd, "\n==Reason codes of the different commands\n\n");
         fprintm(psCtx->pfStd, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, MAN_CLEP_APPENDIX_REASONCODES, 1);
         for (i = 1, m = pfMsg(i); m != NULL; i++, m = pfMsg(i)) {
            if (*m) {
               fprintf(psCtx->pfStd, ":%d: %s\n", i, m);
            }
         }
      }
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'ERRORS' not valid\n");
      fprintf(psCtx->pfErr, "%s %s ERRORS\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

static int siCleBuiltinSyntax(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd = (const TsCleCommand*)psCtx->psCmd;

   if (psCtx->argc==2) {
      if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Syntax for program '%s':\n",psCtx->pcPgm); }
      vdPrnStaticSyntax(psCtx->pfStd,psCmd,psCtx->psBif,psCtx->argv[0],psCtx->pcDep,psCtx->pcOpt,psCtx->pcDpa);
      return(CLERTC_OK);
   } else if (psCtx->argc>=3) {
      if (psCtx->argc==3) {
         *(psCtx->psiDep)=1;
      } else if (psCtx->argc==4) {
         if (psCtx->argv[3][0]=='-') { psCtx->argv[3]++; }
         if (psCtx->argv[3][0]=='-') { psCtx->argv[3]++; }
         if (strxcmp(psCtx->isCas,psCtx->argv[3],"ALL",0,0,FALSE)==0) {
            *(psCtx->psiDep)=10;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH1",0,0,FALSE)==0) {
            *(psCtx->psiDep)=1;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH2",0,0,FALSE)==0) {
            *(psCtx->psiDep)=2;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH3",0,0,FALSE)==0) {
            *(psCtx->psiDep)=3;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH4",0,0,FALSE)==0) {
            *(psCtx->psiDep)=4;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH5",0,0,FALSE)==0) {
            *(psCtx->psiDep)=5;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH6",0,0,FALSE)==0) {
            *(psCtx->psiDep)=6;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH7",0,0,FALSE)==0) {
            *(psCtx->psiDep)=7;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH8",0,0,FALSE)==0) {
            *(psCtx->psiDep)=8;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH9",0,0,FALSE)==0) {
            *(psCtx->psiDep)=9;
         } else {
            if (psCtx->pfErr!=NULL) {
               fprintf(psCtx->pfErr,"Syntax for built-in function 'SYNTAX' not valid\n");
               for (i=0;psCmd[i].pcKyw!=NULL ;i++) {
                  if (psCmd[i].siFlg) {
                     fprintf(psCtx->pfErr,"%s %s SYNTAX %s",psCtx->pcDep,psCtx->argv[0],psCmd[i].pcKyw);
                     efprintf(psCtx->pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
                  }
               }
            }
            siErr=CLERTC_CMD;
            return(siErr);
         }
      } else {
         if (psCtx->pfErr!=NULL) {
            fprintf(psCtx->pfErr,"Syntax for built-in function 'SYNTAX' not valid\n");
            for (i=0;psCmd[i].pcKyw!=NULL ;i++) {
               if (psCmd[i].siFlg) {
                  fprintf(psCtx->pfErr,"%s %s SYNTAX %s",psCtx->pcDep,psCtx->argv[0],psCmd[i].pcKyw);
                  efprintf(psCtx->pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
               }
            }
         }
         siErr=CLERTC_CMD;
         return(siErr);
      }
      for (i=0;psCmd[i].pcKyw!=NULL;i++) {
         if (strxcmp(psCtx->isCas,psCtx->argv[2],psCmd[i].pcKyw,strlen(psCmd[i].pcKyw),'.',TRUE)==0) {
            siErr=siCleCommandInit(psCtx->pvGbl,psCmd[i].pfIni,psCmd[i].pvClp,psCtx->pcOwn,psCtx->pcPgm,psCtx->pcBld,psCmd[i].pcKyw,psCmd[i].pcMan,psCmd[i].pcHlp,psCmd[i].piOid,psCmd[i].psTab,
                                   psCtx->isCas,psCtx->isPfl,psCtx->isRpl,psCtx->siMkl,psCtx->pfStd,psCtx->pfErr,psCtx->pfTrc,psCtx->pcDep,psCtx->pcOpt,psCtx->pcEnt,psCtx->psCnf,psCtx->ppvHdl,
                                   (TfMsg*)psCtx->pfMsg,psCtx->pvF2S,(TfF2S*)psCtx->pfF2S,psCtx->pvSaf,(TfSaf*)psCtx->pfSaf,NULL);
            if (siErr) {
               return(siErr);
            }
            if (strlen(psCtx->argv[2])==strlen(psCmd[i].pcKyw)) {
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Syntax for command '%s':\n",psCtx->argv[2]); }
            } else {
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Syntax for argument '%s':\n",psCtx->argv[2]); }
            }
            vdPrnCommandSyntax(*(psCtx->ppvHdl),psCtx->pfStd,psCtx->argv[0],psCtx->argv[2],psCtx->pcDep,*(psCtx->psiDep));
            return(CLERTC_OK);
         }
      }
      if (psCtx->pcDef!=NULL && *(psCtx->pcDef)) {
         for (i=0;psCmd[i].pcKyw!=NULL;i++) {
            if (strxcmp(psCtx->isCas,psCtx->pcDef,psCmd[i].pcKyw,0,0,FALSE)==0) {
               char acPat[strlen(psCtx->pcDef)+strlen(psCtx->argv[2])+2];
               siErr=siCleCommandInit(psCtx->pvGbl,psCmd[i].pfIni,psCmd[i].pvClp,psCtx->pcOwn,psCtx->pcPgm,psCtx->pcBld,psCmd[i].pcKyw,psCmd[i].pcMan,psCmd[i].pcHlp,psCmd[i].piOid,psCmd[i].psTab,
                                      psCtx->isCas,psCtx->isPfl,psCtx->isRpl,psCtx->siMkl,psCtx->pfStd,psCtx->pfErr,psCtx->pfTrc,psCtx->pcDep,psCtx->pcOpt,psCtx->pcEnt,psCtx->psCnf,psCtx->ppvHdl,
                                      (TfMsg*)psCtx->pfMsg,psCtx->pvF2S,(TfF2S*)psCtx->pfF2S,psCtx->pvSaf,(TfSaf*)psCtx->pfSaf,NULL);
               if (siErr) {
                  return(siErr);
               }
               snprintf(acPat,sizeof(acPat),"%s.%s",psCtx->pcDef,psCtx->argv[2]);
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Syntax for argument '%s':\n",acPat); }
               vdPrnCommandSyntax(*(psCtx->ppvHdl),psCtx->pfStd,psCtx->argv[0],acPat,psCtx->pcDep,*(psCtx->psiDep));
               return(CLERTC_OK);
            }
         }
      }
   }
   if (psCtx->pfErr!=NULL) {
      fprintf(psCtx->pfErr,"Syntax for built-in function 'SYNTAX' not valid\n");
      for (i=0;psCmd[i].pcKyw!=NULL ;i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr,"%s %s SYNTAX %s\n",psCtx->pcDep,psCtx->argv[0],psCmd[i].pcKyw);
            efprintf(psCtx->pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
         }
      }
   }
   siErr=CLERTC_CMD;
   return(siErr);
}

static int siCleBuiltinHelp(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd = (const TsCleCommand*)psCtx->psCmd;
   const TsCleDoc* psDoc = (const TsCleDoc*)psCtx->psDoc;
   const char* pcPgmMan=NULL;

   /* Find program manual */
   for (i=0;psDoc[i].pcMan!=NULL;i++) {
      if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
         pcPgmMan=psDoc[i].pcMan;
      }
   }

   if (psCtx->argc==2) {
      if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Help for program '%s':\n",psCtx->pcPgm); }
      vdPrnStaticHelp(psCtx->pfStd,psCmd,psCtx->psBif,psCtx->argv[0],psCtx->pcDep);
      return(CLERTC_OK);
   } else if (psCtx->argc>=3) {
      if (psCtx->argc==3) {
         if (strxcmp(psCtx->isCas,psCtx->argv[2],"MAN",0,0,FALSE)==0 || strxcmp(psCtx->isCas,psCtx->argv[2],"-MAN",0,0,FALSE)==0 || strxcmp(psCtx->isCas,psCtx->argv[2],"--MAN",0,0,FALSE)==0) {
            if (pcPgmMan!=NULL && *pcPgmMan) {
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Help for program '%s':\n",psCtx->pcPgm); }
               fprintm(psCtx->pfStd,psCtx->pcOwn,psCtx->pcPgm,psCtx->pcBld,pcPgmMan,1);
               return(CLERTC_OK);
            } else {
               if (psCtx->pfErr!=NULL) {
                  fprintf(psCtx->pfErr,"No manual page available for program '%s'\n",psCtx->pcPgm);
                  fprintf(psCtx->pfErr,"CLE_DOCTYP_PROGRAM not found in documentation table\n");
               }
               siErr=CLERTC_TAB;
               return(siErr);
            }
         } else *(psCtx->psiDep)=1;
      } else if (psCtx->argc==4) {
         if (psCtx->argv[3][0]=='-') { psCtx->argv[3]++; }
         if (psCtx->argv[3][0]=='-') { psCtx->argv[3]++; }
         if (strxcmp(psCtx->isCas,psCtx->argv[3],"MAN",0,0,FALSE)==0) {
            *(psCtx->psiDep)=0;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH1",0,0,FALSE)==0) {
            *(psCtx->psiDep)=1;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH2",0,0,FALSE)==0) {
            *(psCtx->psiDep)=2;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH3",0,0,FALSE)==0) {
            *(psCtx->psiDep)=3;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH4",0,0,FALSE)==0) {
            *(psCtx->psiDep)=4;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH5",0,0,FALSE)==0) {
            *(psCtx->psiDep)=5;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH6",0,0,FALSE)==0) {
            *(psCtx->psiDep)=6;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH7",0,0,FALSE)==0) {
            *(psCtx->psiDep)=7;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH8",0,0,FALSE)==0) {
            *(psCtx->psiDep)=8;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"DEPTH9",0,0,FALSE)==0) {
            *(psCtx->psiDep)=9;
         } else if (strxcmp(psCtx->isCas,psCtx->argv[3],"ALL",0,0,FALSE)==0) {
            *(psCtx->psiDep)=10;
         } else {
            if (psCtx->pfErr!=NULL) {
               fprintf(psCtx->pfErr,"Syntax for built-in function 'HELP' not valid\n");
               for (i=0;psCmd[i].pcKyw!=NULL ;i++) {
                  if (psCmd[i].siFlg) {
                     fprintf(psCtx->pfErr,"%s %s HELP %s",psCtx->pcDep,psCtx->argv[0],psCmd[i].pcKyw);
                     efprintf(psCtx->pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
                  }
               }
            }
            siErr=CLERTC_CMD;
            return(siErr);
         }
      } else {
         if (psCtx->pfErr!=NULL) {
            fprintf(psCtx->pfErr,"Syntax for built-in function 'HELP' not valid\n");
            for (i=0;psCmd[i].pcKyw!=NULL ;i++) {
               if (psCmd[i].siFlg) {
                  fprintf(psCtx->pfErr,"%s %s HELP %s",psCtx->pcDep,psCtx->argv[0],psCmd[i].pcKyw);
                  efprintf(psCtx->pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
               }
            }
         }
         siErr=CLERTC_CMD;
         return(siErr);
      }
      for (i=0;psCmd[i].pcKyw!=NULL;i++) {
         if (strxcmp(psCtx->isCas,psCtx->argv[2],psCmd[i].pcKyw,strlen(psCmd[i].pcKyw),'.',TRUE)==0) {
            siErr=siCleCommandInit(psCtx->pvGbl,psCmd[i].pfIni,psCmd[i].pvClp,psCtx->pcOwn,psCtx->pcPgm,psCtx->pcBld,psCmd[i].pcKyw,psCmd[i].pcMan,psCmd[i].pcHlp,psCmd[i].piOid,psCmd[i].psTab,
                                   psCtx->isCas,psCtx->isPfl,psCtx->isRpl,psCtx->siMkl,psCtx->pfStd,psCtx->pfErr,psCtx->pfTrc,psCtx->pcDep,psCtx->pcOpt,psCtx->pcEnt,psCtx->psCnf,psCtx->ppvHdl,
                                   (TfMsg*)psCtx->pfMsg,psCtx->pvF2S,(TfF2S*)psCtx->pfF2S,psCtx->pvSaf,(TfSaf*)psCtx->pfSaf,NULL);
            if (siErr) {
               return(siErr);
            }
            if (strlen(psCtx->argv[2])==strlen(psCmd[i].pcKyw)) {
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Help for command '%s': %s\n",psCtx->argv[2],psCmd[i].pcHlp); }
            } else {
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Help for argument '%s': %s\n",psCtx->argv[2],pcClpInfo(*(psCtx->ppvHdl),psCtx->argv[2])); }
            }
            vdPrnCommandHelp(*(psCtx->ppvHdl),psCtx->argv[2],*(psCtx->psiDep),*(psCtx->psiDep)>9,TRUE);
            if (*(psCtx->psiDep)==0) {
               fprintf(psCtx->pfStd,"ARGUMENTS\n");
               fprintf(psCtx->pfStd,"---------\n");
               vdPrnCommandHelp(*(psCtx->ppvHdl),psCtx->argv[2],1,TRUE,FALSE);
            }
            return(CLERTC_OK);
         }
      }
      if (psCtx->pcDef!=NULL && *(psCtx->pcDef)) {
         for (i=0;psCmd[i].pcKyw!=NULL;i++) {
            if (strxcmp(psCtx->isCas,psCtx->pcDef,psCmd[i].pcKyw,0,0,FALSE)==0) {
               char acPat[strlen(psCmd[i].pcKyw)+strlen(psCtx->argv[2])+2];
               siErr=siCleCommandInit(psCtx->pvGbl,psCmd[i].pfIni,psCmd[i].pvClp,psCtx->pcOwn,psCtx->pcPgm,psCtx->pcBld,psCmd[i].pcKyw,psCmd[i].pcMan,psCmd[i].pcHlp,psCmd[i].piOid,psCmd[i].psTab,
                                      psCtx->isCas,psCtx->isPfl,psCtx->isRpl,psCtx->siMkl,psCtx->pfStd,psCtx->pfErr,psCtx->pfTrc,psCtx->pcDep,psCtx->pcOpt,psCtx->pcEnt,psCtx->psCnf,psCtx->ppvHdl,
                                      (TfMsg*)psCtx->pfMsg,psCtx->pvF2S,(TfF2S*)psCtx->pfF2S,psCtx->pvSaf,(TfSaf*)psCtx->pfSaf,NULL);
               if (siErr) {
                  return(siErr);
               }
               snprintf(acPat,sizeof(acPat),"%s.%s",psCmd[i].pcKyw,psCtx->argv[2]);
               if (psCtx->pfOut!=NULL) { fprintf(psCtx->pfOut,"Help for argument '%s': %s\n",acPat,pcClpInfo(*(psCtx->ppvHdl),acPat)); }
               vdPrnCommandHelp(*(psCtx->ppvHdl),acPat,*(psCtx->psiDep),*(psCtx->psiDep)>9,TRUE);
               if (*(psCtx->psiDep)==0) {
                  fprintf(psCtx->pfStd,"ARGUMENTS\n");
                  fprintf(psCtx->pfStd,"---------\n");
                  vdPrnCommandHelp(*(psCtx->ppvHdl),acPat,1,TRUE,FALSE);
               }
               return(CLERTC_OK);
            }
         }
      }
   }
   if (psCtx->pfErr!=NULL) {
      fprintf(psCtx->pfErr,"Syntax for built-in function 'HELP' not valid\n");
      for (i=0;psCmd[i].pcKyw!=NULL ;i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr,"%s %s HELP %s\n",psCtx->pcDep,psCtx->argv[0],psCmd[i].pcKyw);
            efprintf(psCtx->pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
         }
      }
   }
   siErr=CLERTC_CMD;
   return(siErr);
}
static int siCleBuiltinManpage(TsCleBuiltinContext* psCtx)
{
   const TsCleCommand* psCmd = (const TsCleCommand*)psCtx->psCmd;
   char*    pcFil = NULL;
   size_t   szFil = 0;
   FILE*    pfDoc = NULL;
   int      siErr = 0;
   int      i;
   int      isManFile = FALSE;
   void*    pvHdl = NULL;


   if (psCtx->argc == 2) {
      if (psCtx->pcPgmMan != NULL && *psCtx->pcPgmMan) {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Manual page for program '%s':\n\n", psCtx->pcPgm);
         }
         vdCleManProgram(psCtx->pfStd, psCmd, psCtx->psBif, psCtx->pcOwn, psCtx->pcPgm,
                        psCtx->pcBld, psCtx->pcHlp, psCtx->pcPgmMan, psCtx->pcDep,
                        psCtx->pcOpt, psCtx->pcDpa, psCtx->pcPgmNum, FALSE, TRUE);
         return(CLERTC_OK);
      } else {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "No manual page available for program '%s'\n", psCtx->pcPgm);
            fprintf(psCtx->pfErr, "CLE_DOCTYP_PROGRAM not found in documentation table\n");
         }
         siErr = CLERTC_TAB;
         return(siErr);
      }
   } else if (psCtx->argc == 3) {
      const char* pcSgn;
      const char* pcCmd;
      int isMan = FALSE;
      int isAll = FALSE;

      pcSgn = strchr(psCtx->argv[2], '=');
      if (pcSgn != NULL) {
         *((char*)pcSgn) = EOS;
         pcSgn++;
         pcCmd = psCtx->argv[2];
         isMan = TRUE;
         pcFil = dcpmapfil(pcSgn);
         if (pcFil == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Allocation of memory for file name (%s) failed\n", pcSgn);
            }
            siErr = CLERTC_MEM;
            return(siErr);
         }
         pfDoc = fopen_hfq(pcFil, "w");
         if (pfDoc == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",
                      pcFil, "w", errno, pcSysError(errno));
            }
            SAFE_FREE(pcFil);
            siErr = CLERTC_SYS;
            return(siErr);
         }
         isManFile = TRUE;
      } else {
         pcCmd = psCtx->argv[2];
         isMan = FALSE;
         pfDoc = psCtx->pfStd;
         srprintf(&pcFil, &szFil, 0, ":STDOUT:");
         if (pcFil == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Allocation of memory for file name (:STDOUT:) failed\n");
            }
            siErr = CLERTC_MEM;
            return(siErr);
         }
      }

      if (strxcmp(psCtx->isCas, pcCmd, "ALL", 0, 0, FALSE) == 0 ||
          strxcmp(psCtx->isCas, pcCmd, "-ALL", 0, 0, FALSE) == 0 ||
          strxcmp(psCtx->isCas, pcCmd, "--ALL", 0, 0, FALSE) == 0) {
         if (psCtx->pcPgmMan != NULL && *psCtx->pcPgmMan) {
            isAll = TRUE;
            if (isMan == FALSE && psCtx->pfOut != NULL) {
               fprintf(psCtx->pfOut, "Manual page for program '%s':\n\n", psCtx->pcPgm);
            }
            vdCleManProgram(pfDoc, psCmd, psCtx->psBif, psCtx->pcOwn, psCtx->pcPgm,
                           psCtx->pcBld, psCtx->pcHlp, psCtx->pcPgmMan, psCtx->pcDep,
                           psCtx->pcOpt, psCtx->pcDpa, psCtx->pcPgmNum, isMan, TRUE);
            if (isMan == TRUE && psCtx->pfOut != NULL) {
               fprintf(psCtx->pfOut, "Manual page for program '%s' successfully written to file (%s)\n",
                      psCtx->pcPgm, pcFil);
            }
         } else {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "No manual page available for program '%s'\n", psCtx->pcPgm);
               fprintf(psCtx->pfErr, "CLE_DOCTYP_PROGRAM not found in documentation table\n");
            }
            if (isManFile) fclose(pfDoc);
            SAFE_FREE(pcFil);
            siErr = CLERTC_TAB;
            return(siErr);
         }
      }

      for (i = 0; psCtx->psBif[i].pcKyw != NULL; i++) {
         if (psCtx->psBif[i].isBif) {
            if (strxcmp(psCtx->isCas, pcCmd, psCtx->psBif[i].pcKyw, 0, 0, FALSE) == 0 || isAll) {
               char acNum[16];
               if (isMan == FALSE && psCtx->pfOut != NULL) {
                  fprintf(psCtx->pfOut, "Manual page for built-in function '%s':\n\n",
                         psCtx->psBif[i].pcKyw);
               }
               snprintf(acNum, sizeof(acNum), "%s%d.", psCtx->pcBifNum, i + 1);
               vdCleManFunction(pfDoc, 0, S_TLD, acNum, psCtx->psBif[i].pcKyw,
                               psCtx->psBif[i].pcHlp, psCtx->pcOwn, psCtx->pcPgm,
                               psCtx->pcBld, psCtx->psBif[i].pcSyn, psCtx->psBif[i].pcMan,
                               isMan, TRUE, TRUE, FALSE, FALSE);
               if (isMan == TRUE && psCtx->pfOut != NULL) {
                  fprintf(psCtx->pfOut, "Manual page for built-in function '%s' successfully written to file (%s)\n",
                         psCtx->psBif[i].pcKyw, pcFil);
               }
               if (isAll == FALSE) {
                  if (isManFile) fclose(pfDoc);
                  SAFE_FREE(pcFil);
                  return(CLERTC_OK);
               }
            }
         }
      }

      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (isAll) {
            if (psCmd[i].siFlg) {
               pcCmd = psCmd[i].pcKyw;
            } else {
               pcCmd = "";
            }
         }
         if (strxcmp(psCtx->isCas, pcCmd, psCmd[i].pcKyw, strlen(psCmd[i].pcKyw), '.', TRUE) == 0) {
            siErr = siCleCommandInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn,
                                    psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan,
                                    psCmd[i].pcHlp, psCmd[i].piOid, psCmd[i].psTab,
                                    psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl,
                                    psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep,
                                    psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf, &pvHdl,
                                    psCtx->pfMsg, psCtx->pvF2S, psCtx->pfF2S, psCtx->pvSaf,
                                    psCtx->pfSaf, NULL);
            if (siErr) {
               if (isManFile) fclose(pfDoc);
               SAFE_FREE(pcFil);
               return(siErr);
            }
            if (isMan == FALSE) {
               if (strlen(pcCmd) == strlen(psCmd[i].pcKyw)) {
                  if (psCtx->pfOut != NULL) {
                     fprintf(psCtx->pfOut, "Manual page for command '%s':\n\n", pcCmd);
                  }
               } else {
                  if (psCtx->pfOut != NULL) {
                     fprintf(psCtx->pfOut, "Manual page for argument '%s':\n\n", pcCmd);
                  }
               }
            }
            vdPrnCommandManpage(pvHdl, pfDoc, psCtx->pcCmdNum, pcCmd, i, isMan, TRUE);
            vdClpClose(pvHdl, CLPCLS_MTD_ALL);
            pvHdl = NULL;
            if (isMan == TRUE) {
               if (strlen(pcCmd) == strlen(psCmd[i].pcKyw)) {
                  if (psCtx->pfOut != NULL) {
                     fprintf(psCtx->pfOut, "Manual page for command '%s' successfully written to file (%s)\n",
                            pcCmd, pcFil);
                  }
               } else {
                  if (psCtx->pfOut != NULL) {
                     fprintf(psCtx->pfOut, "Manual page for argument '%s' successfully written to file (%s)\n",
                            pcCmd, pcFil);
                  }
               }
            }
            if (isAll == FALSE) {
               if (isManFile) fclose(pfDoc);
               SAFE_FREE(pcFil);
               return(CLERTC_OK);
            }
         }
      }

      if (isAll == TRUE) {
         if (isManFile) fclose(pfDoc);
         SAFE_FREE(pcFil);
         return(CLERTC_OK);
      }

      if (psCtx->pcDef != NULL && *psCtx->pcDef) {
         for (i = 0; psCmd[i].pcKyw != NULL; i++) {
            if (strxcmp(psCtx->isCas, psCtx->pcDef, psCmd[i].pcKyw, 0, 0, FALSE) == 0) {
               char acPat[strlen(psCtx->pcDef) + strlen(pcCmd) + 2];
               siErr = siCleCommandInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn,
                                       psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan,
                                       psCmd[i].pcHlp, psCmd[i].piOid, psCmd[i].psTab,
                                       psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl,
                                       psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep,
                                       psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf, &pvHdl,
                                       psCtx->pfMsg, psCtx->pvF2S, psCtx->pfF2S, psCtx->pvSaf,
                                       psCtx->pfSaf, NULL);
               if (siErr) {
                  if (isManFile) fclose(pfDoc);
                  SAFE_FREE(pcFil);
                  return(siErr);
               }
               snprintf(acPat, sizeof(acPat), "%s.%s", psCtx->pcDef, pcCmd);
               if (psCtx->pfOut != NULL) {
                  fprintf(psCtx->pfOut, "Manual page fo'argument '%s':\n\n", acPat);
               }
               vdPrnCommandManpage(pvHdl, pfDoc, psCtx->pcCmdNum, acPat, i, isMan, TRUE);
               if (isMan == TRUE) {
                  if (psCtx->pfOut != NULL) {
                     fprintf(psCtx->pfOut, "Manual page for argument '%s' successfully written to file (%s)\n",
                            acPat, pcFil);
                  }
               }
               if (isManFile) fclose(pfDoc);
               SAFE_FREE(pcFil);
               return(CLERTC_OK);
            }
         }
      }

      SAFE_FREE(pcFil);

      pcFil = dcpmapfil(psCtx->argv[2]);
      if (pcFil == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for file name (%s) failed\n", psCtx->argv[2]);
         }
         siErr = CLERTC_MEM;
         return(siErr);
      }
      isMan = TRUE;
      pfDoc = fopen_hfq(pcFil, "w");
      if (pfDoc == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",
                   pcFil, "w", errno, pcSysError(errno));
         }
         SAFE_FREE(pcFil);
         siErr = CLERTC_SYS;
         return(siErr);
      }
      isManFile = TRUE;

      if (psCtx->pcPgmMan != NULL && *psCtx->pcPgmMan) {
         vdCleManProgram(pfDoc, psCmd, psCtx->psBif, psCtx->pcOwn, psCtx->pcPgm,
                        psCtx->pcBld, psCtx->pcHlp, psCtx->pcPgmMan, psCtx->pcDep,
                        psCtx->pcOpt, psCtx->pcDpa, psCtx->pcPgmNum, isMan, TRUE);
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Manual page for program '%s' successfully written to file (%s)\n",
                   psCtx->pcPgm, pcFil);
         }
         if (isManFile) fclose(pfDoc);
         SAFE_FREE(pcFil);
         return(CLERTC_OK);
      } else {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "No manual page available for program '%s'\n", psCtx->pcPgm);
            fprintf(psCtx->pfErr, "CLE_DOCTYP_PROGRAM not found in documentation table\n");
         }
         if (isManFile) fclose(pfDoc);
         SAFE_FREE(pcFil);
         siErr = CLERTC_TAB;
         return(siErr);
      }
   }

   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'MANPAGE' not valid\n");
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr, "%s %s MANPAGE %s", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
            efprintf(psCtx->pfErr, "[.path]\n");
         }
      }
      fprintf(psCtx->pfErr, "%s %s MANPAGE function\n", psCtx->pcDep, psCtx->argv[0]);
      fprintf(psCtx->pfErr, "%s %s MANPAGE\n", psCtx->pcDep, psCtx->argv[0]);
   }
   siErr = CLERTC_CMD;
   return(siErr);
}
static int siCleBuiltinGendocu(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd;
   const char* pcCmd = NULL;
   const char* pcSgn = NULL;
   int isNbr = TRUE;
   int isDep = TRUE;
   char* pcFil;
   FILE* pfDoc;
   TfF2S* pfF2S;
   TfSaf* pfSaf;
   const TsCleDoc* psDoc;
   const TsCleOtherClp* psOth;
   const TsClpArgument* psEnvTab;
   TsCleDocPar stDocPar;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   pfF2S = (TfF2S*)psCtx->pfF2S;
   pfSaf = (TfSaf*)psCtx->pfSaf;
   psDoc = (const TsCleDoc*)psCtx->psDoc;
   psOth = (const TsCleOtherClp*)psCtx->psOth;
   psEnvTab = (const TsClpArgument*)psCtx->psEnv;

   if (psCtx->argc == 3 || psCtx->argc == 4 || psCtx->argc == 5) {
      if (psCtx->argc == 4) {
         if (psCtx->argv[3][0] == '-') { psCtx->argv[3]++; }
         if (psCtx->argv[3][0] == '-') { psCtx->argv[3]++; }
         if (strxcmp(psCtx->isCas, psCtx->argv[3], "NONBR", 0, 0, FALSE) == 0) {
            isNbr = FALSE;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "SHORT", 0, 0, FALSE) == 0) {
            isDep = FALSE;
         } else {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Syntax for built-in function 'GENDOCU' not valid\n");
               for (i = 0; psCmd[i].pcKyw != NULL; i++) {
                  if (psCmd[i].siFlg) {
                     efprintf(psCtx->pfErr, "%s %s GENDOCU %s[.path]=filename [NONBR][SHORT]\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
                  }
               }
               efprintf(psCtx->pfErr, "%s %s GENDOCU filename [NONBR][SHORT]\n", psCtx->pcDep, psCtx->argv[0]);
            }
            return(CLERTC_CMD);
         }
      } else if (psCtx->argc == 5) {
         if (psCtx->argv[3][0] == '-') { psCtx->argv[3]++; }
         if (psCtx->argv[3][0] == '-') { psCtx->argv[3]++; }
         if (psCtx->argv[4][0] == '-') { psCtx->argv[4]++; }
         if (psCtx->argv[4][0] == '-') { psCtx->argv[4]++; }
         if (strxcmp(psCtx->isCas, psCtx->argv[3], "NONBR", 0, 0, FALSE) == 0 && strxcmp(psCtx->isCas, psCtx->argv[4], "SHORT", 0, 0, FALSE) == 0) {
            isNbr = FALSE;
            isDep = FALSE;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "SHORT", 0, 0, FALSE) == 0 && strxcmp(psCtx->isCas, psCtx->argv[4], "NONBR", 0, 0, FALSE) == 0) {
            isNbr = FALSE;
            isDep = FALSE;
         } else {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Syntax for built-in function 'GENDOCU' not valid\n");
               for (i = 0; psCmd[i].pcKyw != NULL; i++) {
                  if (psCmd[i].siFlg) {
                     efprintf(psCtx->pfErr, "%s %s GENDOCU %s[.path]=filename [NONBR][SHORT]\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
                  }
               }
               efprintf(psCtx->pfErr, "%s %s GENDOCU filename [NONBR][SHORT]\n", psCtx->pcDep, psCtx->argv[0]);
            }
            return(CLERTC_CMD);
         }
      }
      pcSgn = strchr(psCtx->argv[2], '=');
      if (pcSgn != NULL) {
         *((char*)pcSgn) = EOS;
         pcSgn++;
         pcCmd = psCtx->argv[2];
      } else {
         pcSgn = psCtx->argv[2];
         pcCmd = NULL;
      }
      pcFil = dcpmapfil(pcSgn);
      if (pcFil == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for file name (%s) failed\n", pcSgn);
         }
         return(CLERTC_MEM);
      }
      pfDoc = fopen_hfq(pcFil, "w");
      if (pfDoc == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Open of documentation file (\"%s\",\"%s\") failed (%d - %s)\n", pcFil, "w", errno, pcSysError(errno));
         }
         SAFE_FREE(pcFil);
         return(CLERTC_SYS);
      }
      if (pcCmd != NULL) {
         for (i = 0; psCmd[i].pcKyw != NULL; i++) {
            if (strxcmp(psCtx->isCas, pcCmd, psCmd[i].pcKyw, strlen(psCmd[i].pcKyw), '.', TRUE) == 0) {
               char acNum[64];
               siErr = siCleCommandInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp, psCmd[i].piOid, psCmd[i].psTab,
                                        psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf, psCtx->ppvHdl,
                                        (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf, NULL);
               if (siErr) {
                  fclose_unchecked(pfDoc);
                  SAFE_FREE(pcFil);
                  return(siErr);
               }
               snprintf(acNum, sizeof(acNum), "%s%d.", psCtx->pcCmdNum, i + 1);
               siErr = siClpDocu(*(psCtx->ppvHdl), pfDoc, pcCmd, acNum, "Command", TRUE, TRUE, FALSE, FALSE, isNbr, FALSE, TRUE, 0);
               if (siErr < 0) {
                  if (psCtx->pfErr != NULL) {
                     fprintf(psCtx->pfErr, "Creation of documentation file (%s) failed (%d - %s)\n", pcFil, errno, pcSysError(errno));
                  }
                  fclose_unchecked(pfDoc);
                  SAFE_FREE(pcFil);
                  return(CLERTC_SYN);
               } else {
                  if (strlen(pcCmd) == strlen(psCmd[i].pcKyw)) {
                     if (psCtx->pfOut != NULL) {
                        fprintf(psCtx->pfOut, "Documentation for command '%s' successfully created\n", pcCmd);
                     }
                  } else {
                     if (psCtx->pfOut != NULL) {
                        fprintf(psCtx->pfOut, "Documentation for argument '%s' successfully created\n", pcCmd);
                     }
                  }
                  fclose_unchecked(pfDoc);
                  SAFE_FREE(pcFil);
                  return(CLERTC_OK);
               }
            }
         }
         if (psCtx->pcDef != NULL && *(psCtx->pcDef)) {
            for (i = 0; psCmd[i].pcKyw != NULL; i++) {
               if (strxcmp(psCtx->isCas, psCtx->pcDef, psCmd[i].pcKyw, 0, 0, FALSE) == 0) {
                  char acNum[64];
                  char acPat[strlen(psCtx->pcDef) + strlen(pcCmd) + 2];
                  siErr = siCleCommandInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp, psCmd[i].piOid, psCmd[i].psTab,
                                           psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf, psCtx->ppvHdl,
                                           (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf, NULL);
                  if (siErr) {
                     fclose_unchecked(pfDoc);
                     SAFE_FREE(pcFil);
                     return(siErr);
                  }
                  snprintf(acNum, sizeof(acNum), "%s%d.", psCtx->pcCmdNum, i + 1);
                  snprintf(acPat, sizeof(acPat), "%s.%s", psCtx->pcDef, pcCmd);
                  siErr = siClpDocu(*(psCtx->ppvHdl), pfDoc, acPat, acNum, "Command", TRUE, TRUE, FALSE, FALSE, isNbr, FALSE, TRUE, 0);
                  if (siErr < 0) {
                     if (psCtx->pfErr != NULL) {
                        fprintf(psCtx->pfErr, "Creation of documentation file (%s) failed (%d - %s)\n", pcFil, errno, pcSysError(errno));
                     }
                     fclose_unchecked(pfDoc);
                     SAFE_FREE(pcFil);
                     return(CLERTC_SYN);
                  } else {
                     if (psCtx->pfOut != NULL) {
                        fprintf(psCtx->pfOut, "Documentation for argument '%s' successfully created\n", acPat);
                     }
                     fclose_unchecked(pfDoc);
                     SAFE_FREE(pcFil);
                     return(CLERTC_OK);
                  }
               }
            }
         }
      } else {
         stDocPar.isHdr = TRUE;  stDocPar.pcAut = psCtx->pcAut; stDocPar.pcAdr = psCtx->pcAdr;
         stDocPar.isAnc = TRUE;  stDocPar.isNbr = isNbr; stDocPar.isIdt = FALSE; stDocPar.isPat = TRUE;
         stDocPar.isDep = isDep; stDocPar.isCas = psCtx->isCas; stDocPar.isPfl = psCtx->isPfl; stDocPar.isRpl = psCtx->isRpl;
         stDocPar.pcAbo = psCtx->pcAbo; stDocPar.pcDep = psCtx->pcDep; stDocPar.pcDpa = psCtx->pcDpa; stDocPar.pcEnt = psCtx->pcEnt;
         stDocPar.pcHlp = psCtx->pcHlp; stDocPar.pcOpt = psCtx->pcOpt; stDocPar.pcOwn = psCtx->pcOwn; stDocPar.pcPgm = psCtx->pcPgm;
         stDocPar.pcBld = psCtx->pcBld; stDocPar.pcVsn = psCtx->pcVsn; stDocPar.pfMsg = (TfMsg*)psCtx->pfMsg; stDocPar.pvCnf = psCtx->psCnf;
         stDocPar.siMkl = psCtx->siMkl; stDocPar.psBif = psCtx->psBif; stDocPar.psCmd = psCmd; stDocPar.psOth = psOth;
         stDocPar.pvF2S = psCtx->pvF2S; stDocPar.pfF2S = pfF2S; stDocPar.pvSaf = psCtx->pvSaf; stDocPar.pfSaf = pfSaf;
         stDocPar.siPs1 = '/';   stDocPar.siPs2 = '-';   stDocPar.siPr3 = '_';   stDocPar.isShl = FALSE;
         stDocPar.psEnv = psEnvTab; stDocPar.pcEnv = psCtx->pcEnv;
         siErr = siPrintDocu(psCtx->pvGbl, pfDoc, psCtx->pfErr, psDoc, &stDocPar, pfDoc, siPrintPage);
         if (siErr) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Generation of documentation for program '%s' failed\n", psCtx->pcPgm);
            }
            fclose_unchecked(pfDoc);
            SAFE_FREE(pcFil);
            return(siErr);
         } else {
            if (psCtx->pfOut != NULL) {
               fprintf(psCtx->pfOut, "Documentation for program '%s' successfully created\n", psCtx->pcPgm);
            }
            fclose_unchecked(pfDoc);
            SAFE_FREE(pcFil);
            return(CLERTC_OK);
         }
      }
      fclose_unchecked(pfDoc);
      SAFE_FREE(pcFil);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'GENDOCU' not valid\n");
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr, "%s %s GENDOCU %s", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
            efprintf(psCtx->pfErr, "[.path]=filename [NONBR][SHORT]\n");
         }
      }
      efprintf(psCtx->pfErr, "%s %s GENDOCU filename [NONBR][SHORT]\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinHtmldoc(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   int isPat = FALSE;
   const char* pcPar = ".";
   int isNbr = FALSE;
   int isDep = TRUE;
   int isShl = TRUE;
   TfCleOpenPrint* pfHtmlOpn = NULL;
   TfClpPrintPage* pfHtmlPrn = NULL;
   TfCleClosePrint* pfHtmlCls = NULL;
   char* pcPat = NULL;
   void* pvLib;
   void* pvDocHdl;
   TsCleDocPar stDocPar;
   const TsCleCommand* psCmd;
   const TsClpArgument* psEnvTab;
   TfF2S* pfF2S;
   TfSaf* pfSaf;
   const TsCleDoc* psDoc;
   const TsCleOtherClp* psOth;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   psEnvTab = (const TsClpArgument*)psCtx->psEnv;
   pfF2S = (TfF2S*)psCtx->pfF2S;
   pfSaf = (TfSaf*)psCtx->pfSaf;
   psDoc = (const TsCleDoc*)psCtx->psDoc;
   psOth = (const TsCleOtherClp*)psCtx->psOth;

   if (psCtx->argc > 6) {
      if (psCtx->pfErr != NULL) {
         fprintf(psCtx->pfErr, "Syntax for built-in function 'HTMLDOC' not valid (too many arguments)\n");
         efprintf(psCtx->pfErr, "%s %s %s\n", psCtx->pcDep, psCtx->argv[0], SYN_CLE_BUILTIN_HTMLDOC);
      }
      return(CLERTC_CMD);
   }
   for (i = 2; i < psCtx->argc; i++) {
      if (psCtx->argv[i][0] == '-') { psCtx->argv[i]++; }
      if (psCtx->argv[i][0] == '-') { psCtx->argv[i]++; }
      if (strxcmp(psCtx->isCas, psCtx->argv[i], "NUMBERS", 0, 0, FALSE) == 0) {
         isNbr = TRUE;
      } else if (strxcmp(psCtx->isCas, psCtx->argv[i], "TYPES", 0, 0, FALSE) == 0) {
         isShl = FALSE;
      } else if (strxcmp(psCtx->isCas, psCtx->argv[i], "SHORT", 0, 0, FALSE) == 0) {
         isDep = FALSE;
      } else {
         if (isPat) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Syntax for built-in function 'HTMLDOC' not valid (more than one path)\n");
               efprintf(psCtx->pfErr, "%s %s %s\n", psCtx->pcDep, psCtx->argv[0], SYN_CLE_BUILTIN_HTMLDOC);
            }
            return(CLERTC_CMD);
         }
         pcPar = psCtx->argv[i];
         isPat = TRUE;
      }
   }
   pcPat = dcpmapfil(pcPar);
   if (pcPat == NULL) {
      if (psCtx->pfErr != NULL) {
         fprintf(psCtx->pfErr, "Allocation of memory for path name (%s) failed\n", psCtx->pcHlp);
      }
      return(CLERTC_MEM);
   }
   pvLib = pfLoadHtmlDoc(&pfHtmlOpn, &pfHtmlPrn, &pfHtmlCls);
   if (pvLib == NULL) {
      if (psCtx->pfErr != NULL) {
         fprintf(psCtx->pfErr, "There is no service provider DLL/SO (libhtmldoc) available for HTML generation\n");
      }
      SAFE_FREE(pcPat);
      return(CLERTC_FAT);
   }
   stDocPar.isHdr = TRUE;  stDocPar.pcAut = psCtx->pcAut; stDocPar.pcAdr = psCtx->pcAdr;
   stDocPar.isAnc = TRUE;  stDocPar.isNbr = isNbr; stDocPar.isIdt = TRUE;  stDocPar.isPat = FALSE;
   stDocPar.isDep = isDep; stDocPar.isCas = psCtx->isCas; stDocPar.isPfl = psCtx->isPfl; stDocPar.isRpl = psCtx->isRpl;
   stDocPar.pcAbo = psCtx->pcAbo; stDocPar.pcDep = psCtx->pcDep; stDocPar.pcDpa = psCtx->pcDpa; stDocPar.pcEnt = psCtx->pcEnt;
   stDocPar.pcHlp = psCtx->pcHlp; stDocPar.pcOpt = psCtx->pcOpt; stDocPar.pcOwn = psCtx->pcOwn; stDocPar.pcPgm = psCtx->pcPgm;
   stDocPar.pcBld = psCtx->pcBld; stDocPar.pcVsn = psCtx->pcVsn; stDocPar.pfMsg = (TfMsg*)psCtx->pfMsg; stDocPar.pvCnf = psCtx->psCnf;
   stDocPar.siMkl = psCtx->siMkl; stDocPar.psBif = psCtx->psBif; stDocPar.psCmd = psCmd; stDocPar.psOth = psOth;
   stDocPar.pvF2S = psCtx->pvF2S; stDocPar.pfF2S = pfF2S; stDocPar.pvSaf = psCtx->pvSaf; stDocPar.pfSaf = pfSaf;
   stDocPar.siPs1 = '-';   stDocPar.siPs2 = '-';   stDocPar.siPr3 = '_';   stDocPar.isShl = isShl;
   stDocPar.psEnv = psEnvTab; stDocPar.pcEnv = psCtx->pcEnv;
   if (pfHtmlOpn != NULL && pfHtmlPrn != NULL && pfHtmlCls != NULL) {
      pvDocHdl = pfHtmlOpn(psCtx->pfStd, psCtx->pfErr, pcPat, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, &stDocPar.isHdr, &stDocPar.isAnc, &stDocPar.isIdt, &stDocPar.isPat, &stDocPar.siPs1, &stDocPar.siPs2, &stDocPar.siPr3);
      if (pvDocHdl == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Open service provider for HTML generation failed\n");
         }
         SAFE_FREE(pcPat);
         return(CLERTC_FAT);
      }
      siErr = siPrintDocu(psCtx->pvGbl, psCtx->pfStd, psCtx->pfErr, psDoc, &stDocPar, pvDocHdl, pfHtmlPrn);
      if (siErr) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Generation of %s HTML documentation to folder '%s' failed\n", isDep ? "long" : "short", pcPat);
         }
         pfHtmlCls(pvDocHdl);
         SAFE_FREE(pcPat);
         return(siErr);
      }
      siErr = pfHtmlCls(pvDocHdl);
      if (siErr) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Finalise generation of HTML documentation to folder '%s' failed (%d)\n", pcPat, siErr);
         }
         SAFE_FREE(pcPat);
         return(CLERTC_SYS);
      } else {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Generation of %s HTML documentation to folder '%s' was successful\n", isDep ? "long" : "short", pcPat);
         }
         SAFE_FREE(pcPat);
         return(CLERTC_OK);
      }
   } else {
      vdFreeHtmlDoc(&pvLib);
      if (psCtx->pfErr != NULL) {
         fprintf(psCtx->pfErr, "There is no service provider function (opnHtmlDoc, prnHtmlDoc or clsHtmlDoc) available for HTML generation\n");
      }
      SAFE_FREE(pcPat);
      return(CLERTC_FAT);
   }
}
static int siCleBuiltinGenprop(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd;
   const char* pcSgn;
   const char* pcCmd;
   char* pcFil;
   FILE* pfPro;
   TfF2S* pfF2S;
   TfSaf* pfSaf;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   pfF2S = (TfF2S*)psCtx->pfF2S;
   pfSaf = (TfSaf*)psCtx->pfSaf;
   if (psCtx->argc == 3) {
      pcSgn = strchr(psCtx->argv[2], '=');
      if (pcSgn != NULL) {
         *((char*)pcSgn) = EOS;
         pcSgn++;
         pcCmd = psCtx->argv[2];
      } else {
         pcSgn = psCtx->argv[2];
         pcCmd = NULL;
      }
      pcFil = dcpmapfil(pcSgn);
      if (pcFil == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for file name (%s) failed\n", pcSgn);
         }
         return(CLERTC_MEM);
      }
      pfPro = fopen_hfq(pcFil, "w");
      if (pfPro == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Open of property file (\"%s\",\"%s\") failed (%d-%s)\n", pcFil, "w", errno, pcSysError(errno));
         }
         SAFE_FREE(pcFil);
         return(CLERTC_SYS);
      }
      if (pcCmd == NULL) {
         fprintf(pfPro, "\n%c Property file for: %s.%s %c\n\n", C_HSH, psCtx->pcOwn, psCtx->pcPgm, C_HSH);
      } else {
         fprintf(pfPro, "\n%c Property file for: %s.%s.%s %c\n\n", C_HSH, psCtx->pcOwn, psCtx->pcPgm, pcCmd, C_HSH);
      }
      efprintf(pfPro, "%s", HLP_CLE_PROPFIL);
      if (pcCmd == NULL) {
         for (siErr = CLP_OK, i = 0; psCmd[i].pcKyw != NULL && siErr == CLP_OK; i++) {
            siErr = siClePropertyInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp,
                                      psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep,
                                      psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf, psCtx->ppvHdl, NULL, NULL,
                                      (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
            if (siErr) {
               fclose_unchecked(pfPro);
               SAFE_FREE(pcFil);
               return(siErr);
            }
            siErr = siClpProperties(*(psCtx->ppvHdl), CLPPRO_MTD_CMT, 10, psCmd[i].pcKyw, pfPro);
            vdClpClose(*(psCtx->ppvHdl), CLPCLS_MTD_ALL);
            *(psCtx->ppvHdl) = NULL;
         }
         if (siErr < 0) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Write property file (%s) for program '%s' failed (%d-%s)\n", pcFil, psCtx->pcPgm, errno, pcSysError(errno));
            }
            fclose_unchecked(pfPro);
            SAFE_FREE(pcFil);
            return(CLERTC_SYN);
         } else {
            if (psCtx->pfOut != NULL) {
               fprintf(psCtx->pfOut, "Property file (%s) for program '%s' successfully written\n", pcFil, psCtx->pcPgm);
            }
            fclose_unchecked(pfPro);
            SAFE_FREE(pcFil);
            return(CLERTC_OK);
         }
      } else {
         for (i = 0; psCmd[i].pcKyw != NULL; i++) {
            if (strxcmp(psCtx->isCas, pcCmd, psCmd[i].pcKyw, 0, 0, FALSE) == 0) {
               siErr = siClePropertyInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp,
                                         psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf, psCtx->ppvHdl,
                                         NULL, NULL, (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
               if (siErr) {
                  fclose_unchecked(pfPro);
                  SAFE_FREE(pcFil);
                  return(siErr);
               }
               siErr = siClpProperties(*(psCtx->ppvHdl), CLPPRO_MTD_CMT, 10, psCmd[i].pcKyw, pfPro);
               vdClpClose(*(psCtx->ppvHdl), CLPCLS_MTD_ALL);
               *(psCtx->ppvHdl) = NULL;
               if (siErr < 0) {
                  if (psCtx->pfErr != NULL) {
                     fprintf(psCtx->pfErr, "Write property file (%s) for command '%s' failed (%d-%s)\n", pcFil, pcCmd, errno, pcSysError(errno));
                  }
                  fclose_unchecked(pfPro);
                  SAFE_FREE(pcFil);
                  return(CLERTC_SYN);
               } else {
                  if (psCtx->pfOut != NULL) {
                     fprintf(psCtx->pfOut, "Property file (%s) for command '%s' successfully written\n", pcFil, pcCmd);
                  }
                  fclose_unchecked(pfPro);
                  SAFE_FREE(pcFil);
                  return(CLERTC_OK);
               }
            }
         }
      }
      fclose_unchecked(pfPro);
      SAFE_FREE(pcFil);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'GENPROP' not valid\n");
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr, "%s %s GENPROP %s=filename\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
         }
      }
      fprintf(psCtx->pfErr, "%s %s GENPROP filename\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinSetprop(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd;
   const char* pcSgn;
   const char* pcCmd;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   if (psCtx->argc == 3) {
      pcSgn = strchr(psCtx->argv[2], '=');
      if (pcSgn != NULL) {
         *((char*)pcSgn) = EOS;
         pcSgn++;
         pcCmd = psCtx->argv[2];
         for (i = 0; psCmd[i].pcKyw != NULL && strxcmp(psCtx->isCas, pcCmd, psCmd[i].pcKyw, 0, 0, FALSE); i++) {}
         if (psCmd[i].pcKyw == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Syntax for built-in function 'SETPROP' not valid\n");
               for (i = 0; psCmd[i].pcKyw != NULL; i++) {
                  if (psCmd[i].siFlg) {
                     fprintf(psCtx->pfErr, "%s %s SETPROP %s=filename\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
                  }
               }
            }
            return(CLERTC_CMD);
         }
         srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(pcCmd), "%s.%s.%s.property.file", psCtx->pcOwn, psCtx->pcPgm, pcCmd);
         if (*(psCtx->ppcCnf) == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Allocation of memory for property string failed\n");
            }
            return(CLERTC_MEM);
         }
      } else {
         pcSgn = psCtx->argv[2];
         pcCmd = NULL;
         srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm), "%s.%s.property.file", psCtx->pcOwn, psCtx->pcPgm);
         if (*(psCtx->ppcCnf) == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Allocation of memory for property string failed\n");
            }
            return(CLERTC_MEM);
         }
      }
      if (*pcSgn == 0) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Syntax for built-in function 'SETPROP' not valid\n");
            for (i = 0; psCmd[i].pcKyw != NULL; i++) {
               if (psCmd[i].siFlg) {
                  fprintf(psCtx->pfErr, "%s %s SETPROP %s=filename\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
               }
            }
            fprintf(psCtx->pfErr, "File name was not specified.\n"
                                  "To delete a property file from the list, please use the function DELPROP %ccommand%c\n", C_SBO, C_SBC);
         }
         return(CLERTC_CMD);
      }
      siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), pcSgn, TRUE);
      if (siErr) {
         return(CLERTC_CFG);
      } else {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Setting configuration keyword '%s' to value '%s' was successful\n", *(psCtx->ppcCnf), pcSgn);
         }
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'SETPROP' not valid\n");
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr, "%s %s SETPROP %s=filename\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
         }
      }
      fprintf(psCtx->pfErr, "%s %s SETPROP filename\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinChgprop(TsCleBuiltinContext* psCtx)
{
   int i, j;
   int siErr;
   const TsCleCommand* psCmd;
   char* pcPro = NULL;
   size_t szPro;
   TfF2S* pfF2S;
   TfSaf* pfSaf;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   pfF2S = (TfF2S*)psCtx->pfF2S;
   pfSaf = (TfSaf*)psCtx->pfSaf;
   if (psCtx->argc >= 3) {
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (strxcmp(psCtx->isCas, psCtx->argv[2], psCmd[i].pcKyw, 0, 0, FALSE) == 0) {
            szPro = CLEINI_PROSIZ;
            pcPro = (char*)calloc(1, szPro);
            if (pcPro == NULL) {
               if (psCtx->pfErr != NULL) {
                  fprintf(psCtx->pfErr, "Memory allocation for property list failed\n");
               }
               return(CLERTC_MEM);
            }
            for (j = 3; j < psCtx->argc; j++) {
               if (j > 3) {
                  srprintc(&pcPro, &szPro, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(psCmd[i].pcKyw), " %s.%s.%s.", psCtx->pcOwn, psCtx->pcPgm, psCmd[i].pcKyw);
               } else {
                  srprintc(&pcPro, &szPro, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(psCmd[i].pcKyw), "%s.%s.%s.", psCtx->pcOwn, psCtx->pcPgm, psCmd[i].pcKyw);
               }
               char* pcSgn = strchr(psCtx->argv[j], '=');
               if (pcSgn != NULL) {
                  *pcSgn = 0x00;
                  pcSgn++;
                  srprintc(&pcPro, &szPro, strlen(psCtx->argv[j]) + strlen(pcSgn), "%s=\"%s\"", psCtx->argv[j], pcSgn);
               } else {
                  srprintc(&pcPro, &szPro, strlen(psCtx->argv[j]), "%s=\"\"", psCtx->argv[j]);
               }
            }
            siErr = siCleChangeProperties(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcHom, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp, pcPro,
                                          psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfOut, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf,
                                          (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
            SAFE_FREE(pcPro);
            return(siErr);
         }
      }
   }
   if (psCtx->pcDef != NULL && *(psCtx->pcDef)) {
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (strxcmp(psCtx->isCas, psCtx->pcDef, psCmd[i].pcKyw, 0, 0, FALSE) == 0) {
            szPro = CLEINI_PROSIZ;
            pcPro = calloc(1, szPro);
            if (pcPro == NULL) {
               if (psCtx->pfErr != NULL) {
                  fprintf(psCtx->pfErr, "Memory allocation for property list failed\n");
               }
               return(CLERTC_MEM);
            }
            for (j = 2; j < psCtx->argc; j++) {
               if (j > 2) {
                  srprintc(&pcPro, &szPro, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(psCmd[i].pcKyw), " %s.%s.%s.", psCtx->pcOwn, psCtx->pcPgm, psCmd[i].pcKyw);
               } else {
                  srprintc(&pcPro, &szPro, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(psCmd[i].pcKyw), "%s.%s.%s.", psCtx->pcOwn, psCtx->pcPgm, psCmd[i].pcKyw);
               }
               char* pcSgn = strchr(psCtx->argv[j], '=');
               if (pcSgn != NULL) {
                  *pcSgn = 0x00;
                  pcSgn++;
                  srprintc(&pcPro, &szPro, strlen(psCtx->argv[j]) + strlen(pcSgn), "%s=\"%s\"", psCtx->argv[j], pcSgn);
               } else {
                  srprintc(&pcPro, &szPro, strlen(psCtx->argv[j]), "%s=\"\"", psCtx->argv[j]);
               }
            }
            siErr = siCleChangeProperties(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcHom, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp, pcPro,
                                          psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfOut, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf,
                                          (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
            SAFE_FREE(pcPro);
            return(siErr);
         }
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'CHGPROP' not valid\n");
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr, "%s %s CHGPROP %s", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
            efprintf(psCtx->pfErr, " [path[=value]]*\n");
         }
      }
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinDelprop(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   if (psCtx->argc == 2) {
      srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm), "%s.%s.property.file", psCtx->pcOwn, psCtx->pcPgm);
      if (*(psCtx->ppcCnf) == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for property string failed\n");
         }
         return(CLERTC_MEM);
      }
   } else if (psCtx->argc == 3) {
      for (i = 0; psCmd[i].pcKyw != NULL && strxcmp(psCtx->isCas, psCtx->argv[2], psCmd[i].pcKyw, 0, 0, FALSE); i++) {}
      if (psCmd[i].pcKyw == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Syntax for built-in function 'DELPROP' not valid\n");
            for (i = 0; psCmd[i].pcKyw != NULL; i++) {
               if (psCmd[i].siFlg) {
                  fprintf(psCtx->pfErr, "%s %s DELPROP %s\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
               }
            }
         }
         return(CLERTC_CMD);
      }
      srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(psCtx->argv[2]), "%s.%s.%s.property.file", psCtx->pcOwn, psCtx->pcPgm, psCtx->argv[2]);
      if (*(psCtx->ppcCnf) == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for property string failed\n");
         }
         return(CLERTC_MEM);
      }
   } else {
      if (psCtx->pfErr != NULL) {
         fprintf(psCtx->pfErr, "Syntax for built-in function 'DELPROP' not valid\n");
         for (i = 0; psCmd[i].pcKyw != NULL; i++) {
            if (psCmd[i].siFlg) {
               fprintf(psCtx->pfErr, "%s %s DELPROP %s\n", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
            }
         }
         fprintf(psCtx->pfErr, "%s %s DELPROP\n", psCtx->pcDep, psCtx->argv[0]);
      }
      return(CLERTC_CMD);
   }
   siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), "", TRUE);
   if (siErr) {
      return(CLERTC_CFG);
   } else {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Delete configuration keyword '%s' was successful\n", *(psCtx->ppcCnf));
      }
      return(CLERTC_OK);
   }
}
static int siCleBuiltinGetprop(TsCleBuiltinContext* psCtx)
{
   int i;
   int siErr;
   const TsCleCommand* psCmd;
   TfF2S* pfF2S;
   TfSaf* pfSaf;

   psCmd = (const TsCleCommand*)psCtx->psCmd;
   pfF2S = (TfF2S*)psCtx->pfF2S;
   pfSaf = (TfSaf*)psCtx->pfSaf;

   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Properties for program '%s':\n", psCtx->pcPgm);
      }
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         siErr = siClePropertyInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp,
                                   psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt, psCtx->psCnf,
                                   psCtx->ppvHdl, NULL, NULL, (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
         if (siErr) {
            return(siErr);
         }
         vdPrnProperties(*(psCtx->ppvHdl), psCmd[i].pcKyw, 10);
         vdClpClose(*(psCtx->ppvHdl), CLPCLS_MTD_ALL);
         *(psCtx->ppvHdl) = NULL;
      }
      return(CLERTC_OK);
   } else if (psCtx->argc >= 3) {
      if (psCtx->argc == 3) {
         *(psCtx->psiDep) = 1;
      } else if (psCtx->argc == 4) {
         if (psCtx->argv[3][0] == '-') { psCtx->argv[3]++; }
         if (psCtx->argv[3][0] == '-') { psCtx->argv[3]++; }
         if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEFALL", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 10;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPALL", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 10;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH1", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 1;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH2", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 2;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH3", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 3;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH4", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 4;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH5", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 5;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH6", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 6;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH7", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 7;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH8", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 8;
         } else if (strxcmp(psCtx->isCas, psCtx->argv[3], "DEPTH9", 0, 0, FALSE) == 0) {
            *(psCtx->psiDep) = 9;
         } else {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Syntax for built-in function 'GETPROP' not valid\n");
               for (i = 0; psCmd[i].pcKyw != NULL; i++) {
                  if (psCmd[i].siFlg) {
                     fprintf(psCtx->pfErr, "%s %s GETPROP %s", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
                     efprintf(psCtx->pfErr, "[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
                  }
               }
            }
            return(CLERTC_CMD);
         }
      } else {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Syntax for built-in function 'GETPROP' not valid\n");
            for (i = 0; psCmd[i].pcKyw != NULL; i++) {
               if (psCmd[i].siFlg) {
                  fprintf(psCtx->pfErr, "%s %s GETPROP %s", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
                  efprintf(psCtx->pfErr, "[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
               }
            }
         }
         return(CLERTC_CMD);
      }
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (strxcmp(psCtx->isCas, psCtx->argv[2], psCmd[i].pcKyw, strlen(psCmd[i].pcKyw), '.', TRUE) == 0) {
            siErr = siClePropertyInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp,
                                      psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt,
                                      psCtx->psCnf, psCtx->ppvHdl, NULL, NULL, (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
            if (siErr) {
               return(siErr);
            }
            if (strlen(psCtx->argv[2]) == strlen(psCmd[i].pcKyw)) {
               if (psCtx->pfOut != NULL) {
                  fprintf(psCtx->pfOut, "Properties for command '%s':\n", psCtx->argv[2]);
               }
            } else {
               if (psCtx->pfOut != NULL) {
                  fprintf(psCtx->pfOut, "Properties for argument '%s':\n", psCtx->argv[2]);
               }
            }
            vdPrnProperties(*(psCtx->ppvHdl), psCtx->argv[2], *(psCtx->psiDep));
            return(CLERTC_OK);
         }
      }
      if (psCtx->pcDef != NULL && *(psCtx->pcDef)) {
         for (i = 0; psCmd[i].pcKyw != NULL; i++) {
            if (strxcmp(psCtx->isCas, psCtx->pcDef, psCmd[i].pcKyw, 0, 0, FALSE) == 0) {
               char acPat[strlen(psCtx->pcDef) + strlen(psCtx->argv[2]) + 2];
               siErr = siClePropertyInit(psCtx->pvGbl, psCmd[i].pfIni, psCmd[i].pvClp, psCtx->pcOwn, psCtx->pcPgm, psCtx->pcBld, psCmd[i].pcKyw, psCmd[i].pcMan, psCmd[i].pcHlp,
                                         psCmd[i].piOid, psCmd[i].psTab, psCtx->isCas, psCtx->isPfl, psCtx->isRpl, psCtx->siMkl, psCtx->pfStd, psCtx->pfErr, psCtx->pfTrc, psCtx->pcDep, psCtx->pcOpt, psCtx->pcEnt,
                                         psCtx->psCnf, psCtx->ppvHdl, NULL, NULL, (TfMsg*)psCtx->pfMsg, psCtx->pvF2S, pfF2S, psCtx->pvSaf, pfSaf);
               if (siErr) {
                  return(siErr);
               }
               snprintf(acPat, sizeof(acPat), "%s.%s", psCtx->pcDef, psCtx->argv[2]);
               if (psCtx->pfOut != NULL) {
                  fprintf(psCtx->pfOut, "Properties for argument '%s':\n", acPat);
               }
               vdPrnProperties(*(psCtx->ppvHdl), acPat, *(psCtx->psiDep));
               return(CLERTC_OK);
            }
         }
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'GETPROP' not valid\n");
      for (i = 0; psCmd[i].pcKyw != NULL; i++) {
         if (psCmd[i].siFlg) {
            fprintf(psCtx->pfErr, "%s %s GETPROP %s", psCtx->pcDep, psCtx->argv[0], psCmd[i].pcKyw);
            efprintf(psCtx->pfErr, "[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
         }
      }
      fprintf(psCtx->pfErr, "%s %s GETPROP\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinSetowner(TsCleBuiltinContext* psCtx)
{
   int siErr;

   if (psCtx->argc == 3) {
      srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcPgm), "%s.owner.id", psCtx->pcPgm);
      if (*(psCtx->ppcCnf) == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for owner string failed\n");
         }
         return(CLERTC_MEM);
      }
      siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), psCtx->argv[2], TRUE);
      if (siErr) {
         return(CLERTC_CFG);
      } else {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Setting configuration key word '%s' to value '%s' was successful\n", *(psCtx->ppcCnf), psCtx->argv[2]);
         }
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'SETOWNER' not valid\n");
      fprintf(psCtx->pfErr, "%s %s SETOWNER name\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinGetowner(TsCleBuiltinContext* psCtx)
{
   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Current owner id for '%s' is:\n", psCtx->argv[0]);
      }
      fprintf(psCtx->pfStd, "%s\n", psCtx->pcOwn);
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'GETOWNER' not valid\n");
      fprintf(psCtx->pfErr, "%s %s GETOWNER\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinSetenv(TsCleBuiltinContext* psCtx)
{
   int siErr;
   const char* pcVal;
   const char* pcTmp;

   if (psCtx->argc == 3) {
      pcVal = strchr(psCtx->argv[2], '=');
      if (pcVal != NULL) {
         *((char*)pcVal) = EOS;
         pcVal++;
         pcTmp = psCtx->argv[2];
      } else {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Syntax for built-in function 'SETENV' not valid\n");
            fprintf(psCtx->pfErr, "%s %s SETENV variable=value\n", psCtx->pcDep, psCtx->argv[0]);
         }
         return(CLERTC_CMD);
      }
      // cppcheck-suppress [uninitvar, legacyUninitvar]
      srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(pcTmp), "%s.%s.envar.%s", psCtx->pcOwn, psCtx->pcPgm, pcTmp);
      if (*(psCtx->ppcCnf) == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for envar string failed\n");
         }
         return(CLERTC_MEM);
      }
      siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), pcVal, TRUE);
      if (siErr) {
         return(CLERTC_CFG);
      } else {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Setting environment variable '%s' to value '%s' was successful\n", *(psCtx->ppcCnf), pcVal);
         }
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'SETENV' not valid\n");
      fprintf(psCtx->pfErr, "%s %s SETENV variable=value\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinGetenv(TsCleBuiltinContext* psCtx)
{
   int siCnt;

   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Current environment variables for owner '%s':\n", psCtx->pcOwn);
      }
      siCnt = siCnfPrnEnv(psCtx->psCnf, psCtx->pfStd, psCtx->pcDep, psCtx->pcOwn, psCtx->pcPgm);
      if (siCnt) {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Defined in file \"%s\"\n", psCtx->psCnf->pcFil);
         }
      } else {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "No environment variables defined in file \"%s\" for owner '%s'\n", psCtx->psCnf->pcFil, psCtx->pcOwn);
         }
      }
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'GETENV' not valid\n");
      fprintf(psCtx->pfErr, "%s %s GETENV\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinDelenv(TsCleBuiltinContext* psCtx)
{
   int siErr;

   if (psCtx->argc == 3) {
      srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm) + strlen(psCtx->argv[2]), "%s.%s.envar.%s", psCtx->pcOwn, psCtx->pcPgm, psCtx->argv[2]);
      if (*(psCtx->ppcCnf) == NULL) {
         if (psCtx->pfErr != NULL) {
            fprintf(psCtx->pfErr, "Allocation of memory for envar string failed\n");
         }
         return(CLERTC_MEM);
      }
      siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), "", TRUE);
      if (siErr) {
         return(CLERTC_CFG);
      } else {
         if (psCtx->pfOut != NULL) {
            fprintf(psCtx->pfOut, "Deleting the environment variable '%s' was successful\n", *(psCtx->ppcCnf));
         }
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'DELENV' not valid\n");
      fprintf(psCtx->pfErr, "%s %s DELENV variable\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinLstenv(TsCleBuiltinContext* psCtx)
{
   const TsClpArgument* psEnvTab;

   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Status for all possible usable environment variables:\n");
      }
      psEnvTab = (const TsClpArgument*)psCtx->psEnv;
      for (const TsClpArgument* p = psEnvTab; p != NULL && p->pcKyw != NULL; p++) {
         char acEnv[1024];
         const C08* pcTmp = GETENV(p->pcKyw);
         if (pcTmp != NULL) {
            const TsClpArgument* psArg = psClpFindArgument(psCtx->isCas, psCtx->siMkl, pcTmp, p->psTab);
            U32 isList = (p->psTab != NULL) ? TRUE : FALSE;
            U32 isMatch = (psArg != NULL) ? TRUE : FALSE;
            fprintf(psCtx->pfStd, "%s=%s", p->pcKyw, pcTmp);
            if (isList && isMatch == FALSE) {
               if (CLPISF_SEL(p->uiFlg)) {
                  fprintf(psCtx->pfStd, " (invalid value defined (valid are: ");
               } else {
                  fprintf(psCtx->pfStd, " (free value defined (possible also: ");
               }
               for (const TsClpArgument* s = p->psTab; s != NULL && s->pcKyw != NULL; s++) {
                  if (s == p->psTab) {
                     fprintf(psCtx->pfStd, "%s", s->pcKyw);
                  } else {
                     fprintf(psCtx->pfStd, "/%s", s->pcKyw);
                  }
               }
               fprintf(psCtx->pfStd, "))");
            }
            fprintf(psCtx->pfStd, "\n");
         } else {
            pcTmp = getenvar(p->pcKyw, 0, sizeof(acEnv), acEnv);
            if (pcTmp != NULL) {
               fprintf(psCtx->pfStd, "* %s (%s) -> is undefined but default for replacement\n", p->pcKyw, pcTmp);
            } else {
               fprintf(psCtx->pfStd, "* %s -> is undefined\n", p->pcKyw);
            }
         }
      }
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'LSTENV' not valid\n");
      fprintf(psCtx->pfErr, "%s %s LSTENV\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinHlpenv(TsCleBuiltinContext* psCtx)
{
   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Help for all possible usable environment variables:\n");
      }
      vdPrintEnvars(psCtx->pfStd, (const TsClpArgument*)psCtx->psEnv);
      return(CLERTC_OK);
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'HLPENV' not valid\n");
      fprintf(psCtx->pfErr, "%s %s HLPENV\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinTrace(TsCleBuiltinContext* psCtx)
{
   int siErr;

   if (psCtx->argc == 3) {
      if (strxcmp(psCtx->isCas, psCtx->argv[2], "ON", 0, 0, FALSE) == 0 || strxcmp(psCtx->isCas, psCtx->argv[2], "-ON", 0, 0, FALSE) == 0 || strxcmp(psCtx->isCas, psCtx->argv[2], "--ON", 0, 0, FALSE) == 0) {
         srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm), "%s.%s.trace", psCtx->pcOwn, psCtx->pcPgm);
         if (*(psCtx->ppcCnf) == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Allocation of memory for envar string failed\n");
            }
            return(CLERTC_MEM);
         }
         siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), "ON", TRUE);
         if (siErr) {
            return(CLERTC_CFG);
         } else {
            if (psCtx->pfOut != NULL) {
               fprintf(psCtx->pfOut, "Setting configuration keyword '%s' to value 'ON' was successful\n", *(psCtx->ppcCnf));
            }
            return(CLERTC_OK);
         }
      } else if (strxcmp(psCtx->isCas, psCtx->argv[2], "OFF", 0, 0, FALSE) == 0 || strxcmp(psCtx->isCas, psCtx->argv[2], "-OFF", 0, 0, FALSE) == 0 || strxcmp(psCtx->isCas, psCtx->argv[2], "--OFF", 0, 0, FALSE) == 0) {
         srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm), "%s.%s.trace", psCtx->pcOwn, psCtx->pcPgm);
         if (*(psCtx->ppcCnf) == NULL) {
            if (psCtx->pfErr != NULL) {
               fprintf(psCtx->pfErr, "Allocation of memory for envar string failed\n");
            }
            return(CLERTC_MEM);
         }
         siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), "OFF", TRUE);
         if (siErr) {
            return(CLERTC_CFG);
         } else {
            if (psCtx->pfOut != NULL) {
               fprintf(psCtx->pfOut, "Setting configuration keyword '%s' to value 'OFF' was successful\n", *(psCtx->ppcCnf));
            }
            return(CLERTC_OK);
         }
      } else {
         const char* pcSgn;
         pcSgn = strchr(psCtx->argv[2], '=');
         if (pcSgn != NULL) {
            const char* pcCmd = psCtx->argv[2];
            *((char*)pcSgn) = EOS;
            pcSgn++;
            if (strxcmp(psCtx->isCas, pcCmd, "FILE", 0, 0, FALSE) == 0) {
               srprintf(psCtx->ppcCnf, psCtx->pszCnf, strlen(psCtx->pcOwn) + strlen(psCtx->pcPgm), "%s.%s.trace.file", psCtx->pcOwn, psCtx->pcPgm);
               if (*(psCtx->ppcCnf) == NULL) {
                  if (psCtx->pfErr != NULL) {
                     fprintf(psCtx->pfErr, "Allocation of memory for envar string failed\n");
                  }
                  return(CLERTC_MEM);
               }
               siErr = siCnfSet(psCtx->psCnf, psCtx->pfErr, *(psCtx->ppcCnf), pcSgn, TRUE);
               if (siErr) {
                  return(CLERTC_CFG);
               } else {
                  if (psCtx->pfOut != NULL) {
                     if (*pcSgn) {
                        fprintf(psCtx->pfOut, "Setting configuration keyword '%s' to value '%s' was successful\n", *(psCtx->ppcCnf), pcSgn);
                     } else {
                        fprintf(psCtx->pfOut, "Deleting value from configuration keyword '%s' was successful\n", *(psCtx->ppcCnf));
                     }
                  }
                  return(CLERTC_OK);
               }
            }
         }
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'TRACE' not valid\n");
      fprintf(psCtx->pfErr, "%s %s TRACE ON/OFF\n", psCtx->pcDep, psCtx->argv[0]);
      fprintf(psCtx->pfErr, "%s %s TRACE FILE=filenam\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}
static int siCleBuiltinConfig(TsCleBuiltinContext* psCtx)
{
   int siCnt;

   if (psCtx->argc == 2) {
      if (psCtx->pfOut != NULL) {
         fprintf(psCtx->pfOut, "Current configuration data:\n");
      }
      siCnt = siCnfPrn(psCtx->psCnf, psCtx->pfStd, psCtx->pcDep);
      if (psCtx->pfOut != NULL) {
         if (siCnt) {
            fprintf(psCtx->pfOut, "Assigned to file \"%s\"\n", psCtx->psCnf->pcFil);
         } else {
            fprintf(psCtx->pfOut, "No configuration data defined for file \"%s\"\n", psCtx->psCnf->pcFil);
         }
      }
      return(CLERTC_OK);
   } else if (psCtx->argc == 3) {
      if (psCtx->argv[2][0] == '-') { psCtx->argv[2]++; }
      if (psCtx->argv[2][0] == '-') { psCtx->argv[2]++; }
      if (strxcmp(psCtx->isCas, psCtx->argv[2], "CLEAR", 0, 0, FALSE) == 0) {
         siCnt = siCnfClr(psCtx->psCnf);
         if (psCtx->pfOut != NULL) {
            if (siCnt) {
               fprintf(psCtx->pfOut, "Delete %d elements from file \"%s\"\n", siCnt, psCtx->psCnf->pcFil);
            } else {
               fprintf(psCtx->pfOut, "No configuration data defined for file \"%s\"\n", psCtx->psCnf->pcFil);
            }
         }
         return(CLERTC_OK);
      }
   }
   if (psCtx->pfErr != NULL) {
      fprintf(psCtx->pfErr, "Syntax for built-in function 'CONFIG' not valid\n");
      fprintf(psCtx->pfErr, "%s %s CONFIG\n", psCtx->pcDep, psCtx->argv[0]);
      fprintf(psCtx->pfErr, "%s %s CONFIG CLEAR\n", psCtx->pcDep, psCtx->argv[0]);
   }
   return(CLERTC_CMD);
}

/***********************************************************************/

extern int siCleExecute(
   TfEnv*                        pfEnv,
   TfOpn*                        pfOpn,
   TfCls*                        pfCls,
   const TsCleCommand*           psCmd,
   int                           argc,
   char*                         argv[],
   const char*                   pcOwner,
   const char*                   pcProgram,
   const char*                   pcAut,
   const char*                   pcAdr,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   const char*                   pcLic,
   const char*                   pcBld,
   const char*                   pcVsn,
   const char*                   pcAbo,
   const char*                   pcHlp,
   const char*                   pcDef,
   TfMsg*                        pfMsg,
   const TsCleOtherClp*          psOth,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf,
   const char*                   pcDpa,
   const int                     siNoR,
   const TsCleDoc*               psDoc,
   const TsClpArgument*          psEnv,
   const char*                   pcEnv)
{
   int                           i,siErr,siDep;
   int                           siType,siIdx,siRes,siMln;  /* For command/builtin matching */
   void*                         pvGbl=NULL;
   TsCnfHdl*                     psCnf=NULL;
   size_t                        szCnf=0;
   char*                         pcCnf=NULL;
   size_t                        szOwn=0;
   char*                         pcOwn=NULL;
   char*                         pcPgm=NULL;
   char*                         pcPgu=NULL;
   char*                         pcPgl=NULL;
   const char*                   pcMsg=NULL;
   FILE*                         pfTrh=NULL;
   void*                         pvHdl=NULL;
   FILE*                         pfDoc=NULL;
   FILE*                         pfPro=NULL;
   FILE*                         pfStd=stdout;
   FILE*                         pfErr=(pfOut!=NULL)?pfOut:stderr;
   char**                        ppArg=NULL;
   char*                         pcFil=NULL;
   char*                         pcHom=NULL;
   int                           isWrn=FALSE;
   int                           siScc=0;
   int                           isEnvOwn;
   const char*                   pcPgmNum="2.";
   const char*                   pcCmdNum="3.";
   const char*                   pcBifNum="4.";
   const char*                   pcSccMan=NULL;
   const char*                   pcPgmMan=NULL;
   const char*                   m;
   char                          acTs[24];
   time_t                        uiTime=time(NULL);
   clock_t                       uiClock=clock();
   const TsClpArgument*          psEnvTab;

   CLEBIF_OPN(asBif) = {
      CLETAB_BIF(CLE_BUILTIN_IDX_SYNTAX  ,"SYNTAX"  ,HLP_CLE_BUILTIN_SYNTAX  ,SYN_CLE_BUILTIN_SYNTAX  ,MAN_CLEP_BUILTIN_SYNTAX  ,TRUE         ,siCleBuiltinSyntax)
      CLETAB_BIF(CLE_BUILTIN_IDX_HELP    ,"HELP"    ,HLP_CLE_BUILTIN_HELP    ,SYN_CLE_BUILTIN_HELP    ,MAN_CLEP_BUILTIN_HELP    ,TRUE         ,siCleBuiltinHelp)
      CLETAB_BIF(CLE_BUILTIN_IDX_MANPAGE ,"MANPAGE" ,HLP_CLE_BUILTIN_MANPAGE ,SYN_CLE_BUILTIN_MANPAGE ,MAN_CLEP_BUILTIN_MANPAGE ,TRUE         ,siCleBuiltinManpage)
      CLETAB_BIF(CLE_BUILTIN_IDX_GENDOCU ,"GENDOCU" ,HLP_CLE_BUILTIN_GENDOCU ,SYN_CLE_BUILTIN_GENDOCU ,MAN_CLEP_BUILTIN_GENDOCU ,TRUE         ,siCleBuiltinGendocu)
      CLETAB_BIF(CLE_BUILTIN_IDX_HTMLDOC ,"HTMLDOC" ,HLP_CLE_BUILTIN_HTMLDOC ,SYN_CLE_BUILTIN_HTMLDOC ,MAN_CLEP_BUILTIN_HTMLDOC ,TRUE         ,siCleBuiltinHtmldoc)
      CLETAB_BIF(CLE_BUILTIN_IDX_GENPROP ,"GENPROP" ,HLP_CLE_BUILTIN_GENPROP ,SYN_CLE_BUILTIN_GENPROP ,MAN_CLEP_BUILTIN_GENPROP ,TRUE         ,siCleBuiltinGenprop)
      CLETAB_BIF(CLE_BUILTIN_IDX_SETPROP ,"SETPROP" ,HLP_CLE_BUILTIN_SETPROP ,SYN_CLE_BUILTIN_SETPROP ,MAN_CLEP_BUILTIN_SETPROP ,TRUE         ,siCleBuiltinSetprop)
      CLETAB_BIF(CLE_BUILTIN_IDX_CHGPROP ,"CHGPROP" ,HLP_CLE_BUILTIN_CHGPROP ,SYN_CLE_BUILTIN_CHGPROP ,MAN_CLEP_BUILTIN_CHGPROP ,TRUE         ,siCleBuiltinChgprop)
      CLETAB_BIF(CLE_BUILTIN_IDX_DELPROP ,"DELPROP" ,HLP_CLE_BUILTIN_DELPROP ,SYN_CLE_BUILTIN_DELPROP ,MAN_CLEP_BUILTIN_DELPROP ,TRUE         ,siCleBuiltinDelprop)
      CLETAB_BIF(CLE_BUILTIN_IDX_GETPROP ,"GETPROP" ,HLP_CLE_BUILTIN_GETPROP ,SYN_CLE_BUILTIN_GETPROP ,MAN_CLEP_BUILTIN_GETPROP ,TRUE         ,siCleBuiltinGetprop)
      CLETAB_BIF(CLE_BUILTIN_IDX_SETOWNER,"SETOWNER",HLP_CLE_BUILTIN_SETOWNER,SYN_CLE_BUILTIN_SETOWNER,MAN_CLEP_BUILTIN_SETOWNER,TRUE         ,siCleBuiltinSetowner)
      CLETAB_BIF(CLE_BUILTIN_IDX_GETOWNER,"GETOWNER",HLP_CLE_BUILTIN_GETOWNER,SYN_CLE_BUILTIN_GETOWNER,MAN_CLEP_BUILTIN_GETOWNER,TRUE         ,siCleBuiltinGetowner)
      CLETAB_BIF(CLE_BUILTIN_IDX_SETENV  ,"SETENV"  ,HLP_CLE_BUILTIN_SETENV  ,SYN_CLE_BUILTIN_SETENV  ,MAN_CLEP_BUILTIN_SETENV  ,TRUE         ,siCleBuiltinSetenv)
      CLETAB_BIF(CLE_BUILTIN_IDX_GETENV  ,"GETENV"  ,HLP_CLE_BUILTIN_GETENV  ,SYN_CLE_BUILTIN_GETENV  ,MAN_CLEP_BUILTIN_GETENV  ,TRUE         ,siCleBuiltinGetenv)
      CLETAB_BIF(CLE_BUILTIN_IDX_DELENV  ,"DELENV"  ,HLP_CLE_BUILTIN_DELENV  ,SYN_CLE_BUILTIN_DELENV  ,MAN_CLEP_BUILTIN_DELENV  ,TRUE         ,siCleBuiltinDelenv)
      CLETAB_BIF(CLE_BUILTIN_IDX_LSTENV  ,"LSTENV"  ,HLP_CLE_BUILTIN_LSTENV  ,SYN_CLE_BUILTIN_LSTENV  ,MAN_CLEP_BUILTIN_LSTENV  ,TRUE         ,siCleBuiltinLstenv)
      CLETAB_BIF(CLE_BUILTIN_IDX_HLPENV  ,"HLPENV"  ,HLP_CLE_BUILTIN_HLPENV  ,SYN_CLE_BUILTIN_HLPENV  ,MAN_CLEP_BUILTIN_HLPENV  ,TRUE         ,siCleBuiltinHlpenv)
      CLETAB_BIF(CLE_BUILTIN_IDX_TRACE   ,"TRACE"   ,HLP_CLE_BUILTIN_TRACE   ,SYN_CLE_BUILTIN_TRACE   ,MAN_CLEP_BUILTIN_TRACE   ,TRUE         ,siCleBuiltinTrace)
      CLETAB_BIF(CLE_BUILTIN_IDX_CONFIG  ,"CONFIG"  ,HLP_CLE_BUILTIN_CONFIG  ,SYN_CLE_BUILTIN_CONFIG  ,MAN_CLEP_BUILTIN_CONFIG  ,TRUE         ,siCleBuiltinConfig)
      CLETAB_BIF(CLE_BUILTIN_IDX_GRAMMAR ,"GRAMMAR" ,HLP_CLE_BUILTIN_GRAMMAR ,SYN_CLE_BUILTIN_GRAMMAR ,MAN_CLEP_BUILTIN_GRAMMAR ,TRUE         ,siCleBuiltinGrammar)
      CLETAB_BIF(CLE_BUILTIN_IDX_LEXEMES ,"LEXEMES" ,HLP_CLE_BUILTIN_LEXEMES ,SYN_CLE_BUILTIN_LEXEMES ,MAN_CLEP_BUILTIN_LEXEMES ,TRUE         ,siCleBuiltinLexemes)
      CLETAB_BIF(CLE_BUILTIN_IDX_LICENSE ,"LICENSE" ,HLP_CLE_BUILTIN_LICENSE ,SYN_CLE_BUILTIN_LICENSE ,MAN_CLEP_BUILTIN_LICENSE ,pcLic!=NULL  ,siCleBuiltinLicense)
      CLETAB_BIF(CLE_BUILTIN_IDX_VERSION ,"VERSION" ,HLP_CLE_BUILTIN_VERSION ,SYN_CLE_BUILTIN_VERSION ,MAN_CLEP_BUILTIN_VERSION ,pcVsn!=NULL  ,siCleBuiltinVersion)
      CLETAB_BIF(CLE_BUILTIN_IDX_ABOUT   ,"ABOUT"   ,HLP_CLE_BUILTIN_ABOUT   ,SYN_CLE_BUILTIN_ABOUT   ,MAN_CLEP_BUILTIN_ABOUT   ,pcAbo!=NULL  ,siCleBuiltinAbout)
      CLETAB_BIF(CLE_BUILTIN_IDX_ERRORS  ,"ERRORS"  ,HLP_CLE_BUILTIN_ERRORS  ,SYN_CLE_BUILTIN_ERRORS  ,MAN_CLEP_BUILTIN_ERRORS  ,TRUE         ,siCleBuiltinErrors)
      CLEBIF_CLS
   };

   if (argc>0) {
      if (strxcmp(isCas,argv[argc-1],"SILENT",0,0,FALSE)==0) {
         pfErr=NULL;
         pfOut=NULL;
         argc--;
      } else if (strxcmp(isCas,argv[argc-1],"QUIET",0,0,FALSE)==0) {
         pfErr=pfOut;
         pfOut=NULL;
         argc--;
      }
   }

   if (CHECK_ENVAR_ON("CLE_SILENT")) {
      pfErr=NULL;
      pfOut=NULL;
   }

   if (CHECK_ENVAR_ON("CLE_QUIET")) {
      pfErr=pfOut;
      pfOut=NULL;
   }

   if (psEnv==NULL) {
      psEnvTab=asCleEnvVarTab;
   } else {
      psEnvTab=psEnv;
   }

   for (const TsClpArgument* p=psEnvTab;p!=NULL && p->pcKyw!=NULL;p++) {
      if (p->pcHlp==NULL || *p->pcHlp==0x00) {
         if (pfErr!=NULL) { fprintf(pfErr,"Help message for environment variable '%s' is not defined\n",p->pcKyw); }
         return(CLERTC_TAB);
      }
      if (p->pcDft!=NULL) {
         if (GETENV(p->pcKyw)==NULL) {
            if (SETENV(p->pcKyw,p->pcDft)) {
               if (pfErr!=NULL) { fprintf(pfErr,"Put default value for variable (%s=%s) to environment failed (%d - %s)\n",p->pcKyw,p->pcDft,errno,pcSysError(errno)); }
            }
         }
      }
   }

   if (psCmd==NULL || argc==0     || argv==NULL  ||  pcOwner==NULL ||  pcProgram==NULL ||  pcHlp==NULL ||
       pcDep==NULL || pcOpt==NULL || pcEnt==NULL || *pcOwner==0x00 || *pcProgram==0x00 || *pcHlp==0    || psDoc==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"CLE call parameter incorrect (NULL pointer or empty strings)\n"); }
      ERROR(CLERTC_FAT);
   }

#ifdef __WIN__
   char*    pcWinVer = NULL;
   size_t   szWinVer = 0;
   if (pfOut!=NULL) { efprintf(pfOut,"%s Run on %s\n",cstime(0,acTs),windowsversionstring(&pcWinVer, &szWinVer)); }
   if (pcWinVer != NULL) { free(pcWinVer); }
#else
   struct utsname uts;
   if(uname(&uts)>=0) {
      if (pfOut!=NULL) { efprintf(pfOut,"%s Run on %s(%s)v(%s)r(%s)m(%s)\n",cstime(0,acTs),uts.sysname,uts.nodename,uts.version,uts.release,uts.machine); }
   }
#endif

   if (pfOut!=NULL) { efprintf(pfOut,"%s Start program '%s' (Build: %s (%s %s))\n",cstime(0,acTs),pcProgram,pcBld,__DATE__,__TIME__); }

   for (i=0;asBif[i].pcKyw!=NULL;i++) {
      if (asBif[i].siIdx!=i) {
         if (pfErr!=NULL) { fprintf(pfErr,"Index of built-in function '%s' not valid\n",asBif[i].pcKyw); }
         ERROR(CLERTC_TAB);
      }
   }

   for (i=0;psDoc[i].uiTyp;i++) {
      if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
         pcPgmNum=psDoc[i].pcNum;
      }
      if (psDoc[i].uiTyp==CLE_DOCTYP_COMMANDS) {
         pcCmdNum=psDoc[i].pcNum;
      }
      if (psDoc[i].uiTyp==CLE_DOCTYP_BUILTIN) {
         pcBifNum=psDoc[i].pcNum;
      }
      if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
         pcPgmMan=psDoc[i].pcMan;
      }
      if (psDoc[i].uiTyp==CLE_DOCTYP_SPECIALCODES) {
         pcSccMan=psDoc[i].pcMan;
      }
   }

   if (pfF2S==NULL) {
      pvF2S=NULL;
      pfF2S=siClpFile2String;
   }

   if (pfEnv!=NULL) {
      siErr = pfEnv(pfOut,pfErr);
      if (siErr<0) { return(-1*siErr); }
   } else {
      siErr = readEnvars(NULL,pfOut,pfErr,NULL);
      if (siErr<0) { return(-1*siErr); }
   }

   pcHom=dhomedir(TRUE);
   if (pcHom==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Determination of home directory failed\n"); }
      ERROR(CLERTC_MEM);
   }

   pcPgm=dmapstr(pcProgram,0);
   if (pcPgm==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Conversion of program name failed (not enough memory)\n"); }
      ERROR(CLERTC_MEM);
   }

   pcPgu=dmapstr(pcProgram,1);
   if (pcPgu==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Conversion of program name to upper case failed (not enough memory)\n"); }
      ERROR(CLERTC_MEM);
   }

   pcPgl=dmapstr(pcProgram,2);
   if (pcPgl==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Conversion of program name to lower case failed (not enough memory)\n"); }
      ERROR(CLERTC_MEM);
   }

   srprintf(&pcCnf,&szCnf,strlen(pcPgm),"%s_DEFAULT_OWNER_ID",pcPgu);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for %s_DEFAULT_OWNER_ID failed\n",pcPgu); }
      ERROR(CLERTC_MEM);
   }
   m=GETENV(pcCnf);
   if (m!=NULL && *m) { srprintf(&pcOwn,&szOwn,strlen(m),"%s",m); } else { srprintf(&pcOwn,&szOwn,strlen(pcOwner),"%s",pcOwner); }
   if (pcOwn==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Determination of owner id failed\n"); }
      ERROR(CLERTC_MEM);
   }

   psCnf=psOpenConfig(pfOut,pfErr,pcHom,pcPgm,pcPgu,pcPgl,isCas);
   if (psCnf==NULL) { ERROR(CLERTC_CFG); }

   srprintf(&pcCnf,&szCnf,strlen(pcPgl),"%s.owner.id",pcPgl);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration element owner id failed\n"); }
      ERROR(CLERTC_MEM);
   }
   m=pcCnfGet(psCnf,pcCnf);
   if (m!=NULL && *m) {
      srprintf(&pcOwn,&szOwn,strlen(m),"%s",m);
      if (pcOwn==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for owner name (%s) failed\n",m); }
         ERROR(CLERTC_MEM);
      }
   }

   i=siCnfPutEnv(psCnf,pcOwn,pcPgm);
   if (pfOut!=NULL) {
      if (i) {
         if (i==1) {
            fprintf(pfOut,"%s Load successfully %d environment variable using the configuration file.\n",cstime(0,acTs),i);
         } else {
            fprintf(pfOut,"%s Load successfully %d environment variables using the configuration file.\n",cstime(0,acTs),i);
         }
      }
   }

   m=GETENV("OWNERID");
   if (m==NULL || *m==0x00) {
      if (SETENV("OWNERID",pcOwn)) {
         if (pfErr!=NULL) { fprintf(pfErr,"Put variable (%s=%s) to environment failed (%d - %s)\n","OWNERID",pcOwn,errno,pcSysError(errno)); }
      } else {
         m=GETENV("OWNERID");
         if (m==NULL || strcmp(pcOwn,m)) {
            if (pfErr!=NULL) { fprintf(pfErr,"Put variable (%s=%s) to environment failed (strcmp(%s,GETENV(%s)))\n","OWNERID",pcOwn,pcOwn,"OWNERID"); }
         } else {
#ifdef __DEBUG__
            if (pfOut!=NULL) { fprintf(pfOut,"%s Put variable (%s=%s) to environment was successful\n",cstime(0,acTs),"OWNERID",pcOwn); }
#endif
         }
      }
      isEnvOwn=TRUE;
   } else {
#ifdef __DEBUG__
      if (pfOut!=NULL) { fprintf(pfOut,"%s Environment variable OWNERID already defined (%s)\n",cstime(0,acTs),pcOwn); }
#endif
      isEnvOwn=FALSE;
   }

   if (pfOut!=NULL) {
      fprintf(pfOut,"%s Complete load of environment ...\n",cstime(0,acTs));
   }
#ifdef __EBCDIC__
   init_char(gsDiaChr.exc);
#endif
   // cppcheck-suppress duplicateCondition
   if (pfOut!=NULL) {
      efprintf(pfOut,"%s Initialize dia-critical character (!$#@[\\]^`{|}~) conversion (%s)\n",cstime(0,acTs),mapccsid(localccsid()));
   }

   int siMaxCC=0x0FFFFFFF;
   int siMinCC=0x00000000;
   const C08* pcMaxCC=GETENV("CLE_MAX_CC");
   if (pcMaxCC!=NULL && isdigit(*pcMaxCC)) {
      siMaxCC=atoi(pcMaxCC);
   }
   const C08* pcMinCC=GETENV("CLE_MIN_CC");
   if (pcMinCC!=NULL && isdigit(*pcMinCC)) {
      siMinCC=atoi(pcMinCC);
   }

   if (argc>1) {
      if (strxcmp(isCas,argv[argc-1],"MAXCC=",6,0,FALSE)==0) {
          const char* h=strchr(&(argv[argc-1][6]),'-');
          if (h!=NULL && isdigit(h[1])) { siMinCC=atoi(h+1); }
          if (isdigit(argv[argc-1][6])) { siMaxCC=atoi(&(argv[argc-1][6])); }
          argc--;
      }
      if (strxcmp(isCas,argv[1],"OWNER=",6,0,FALSE)==0) {
         srprintf(&pcOwn,&szOwn,strlen(&argv[1][6]),"%s",&argv[1][6]);
         if (pcOwn==NULL) {
            if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for owner string failed\n"); }
            siErr=CLERTC_MEM;
            ERROR(ELIMIT(siErr));
         }
         if (isEnvOwn) {
            if (SETENV("OWNERID",pcOwn)) {
               if (pfOut!=NULL) { fprintf(pfOut,"Use owner: '%s' (set as environment variable failed (%d - %s))\n",pcOwn,errno,pcSysError(errno)); }
            } else {
               if (pfOut!=NULL) { fprintf(pfOut,"Use owner: '%s' (set as environment variable was successful)\n",pcOwn); }
            }
         } else {
            if (pfOut!=NULL) { fprintf(pfOut,"Use owner: '%s' (environment variable was already defined)\n",pcOwn); }
         }
         for (i=2;i<argc;i++) { argv[i-1]=argv[i]; }
         argc--;
      }
   }

   char  acDep[strlen(pcDep)+1];
   char  acOpt[strlen(pcOpt)+1];
   char  acEnt[strlen(pcEnt)+1];
   esnprintf(acDep,sizeof(acDep),"%s",pcDep);
   esnprintf(acOpt,sizeof(acOpt),"%s",pcOpt);
   esnprintf(acEnt,sizeof(acEnt),"%s",pcEnt);
   pcDep=acDep;
   pcOpt=acOpt;
   pcEnt=acEnt;

   srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgl),"%s.%s.trace",pcOwn,pcPgl);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration element trace failed\n"); }
      siErr=CLERTC_MEM;
      ERROR(ELIMIT(siErr));
   }
   m=pcCnfGet(psCnf,pcCnf);
   if (m!=NULL && strxcmp(isCas,m,"ON",0,0,FALSE)==0) {
      srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgl),"%s.%s.trace.file",pcOwn,pcPgl);
      if (pcCnf==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration element trace file failed\n"); }
         siErr=CLERTC_MEM;
         ERROR(ELIMIT(siErr));
      }
      m=pcCnfGet(psCnf,pcCnf);
      if (m!=NULL && *m) {
         pcFil=dcpmapfil(m);
         if (pcFil!=NULL) {
            pfTrh=fopen_hfq(pcFil,"w");
            if (pfTrh==NULL) {
               if (pfErr!=NULL) { fprintf(pfErr,"Open of trace file (\"%s\",\"%s\") failed\n",pcFil,"w"); }
            } else { pfTrc=pfTrh; }
            SAFE_FREE(pcFil);
         }
      }
   } else pfTrc=NULL;

   if (pfOpn!=NULL) {
      char  acMsg[1028];
      if (pfCls==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"If a global open callback provided the corresponding close callback must be given\n"); }
         siErr=CLERTC_ITF;
         ERROR(ELIMIT(siErr));
      }
      pvGbl=pfOpn(sizeof(acMsg),acMsg);
      if (pvGbl==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Open of global resources failed (%s)\n",acMsg); }
         siErr=CLERTC_SYS;
         ERROR(ELIMIT(siErr));
      }
   }

   for (i=0; psCmd[i].pcKyw!=NULL; i++) {
      if (psCmd[i].psTab==NULL || psCmd[i].pvClp==NULL || psCmd[i].pvPar==NULL ||
          psCmd[i].pfIni==NULL || psCmd[i].pfMap==NULL || psCmd[i].pfRun==NULL || psCmd[i].pfFin==NULL ||
          psCmd[i].pcMan==NULL || psCmd[i].pcHlp==NULL || *psCmd[i].pcKyw==0 || *psCmd[i].pcMan==0 || *psCmd[i].pcHlp==0) {
         if (pfErr!=NULL) { fprintf(pfErr,"Row %d of command table not initialized properly\n",i); }
         siErr=CLERTC_TAB;
         ERROR(ELIMIT(siErr));
      }
   }
   if (i==0) {
      if (pfErr!=NULL) { fprintf(pfErr,"Command table is empty\n"); }
      siErr=CLERTC_TAB;
      ERROR(ELIMIT(siErr));
   }

   if (argc<2) {
      if (pcDef!=NULL && *pcDef) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef); }
            siErr=CLERTC_MEM;
            ERROR(ELIMIT(siErr));
         }
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc=2; argv=ppArg;
      } else {
         if (pfErr!=NULL) {
            fprintf(pfErr,"Command or built-in function required\n");
            vdPrnStaticSyntax(pfErr,psCmd,asBif,argv[0],pcDep,pcOpt,pcDpa);
         }
         siErr=CLERTC_CMD;
         ERROR(ELIMIT(siErr));
      }
   }

EVALUATE:
   if (argv[1][0]=='-') { argv[1]++; }
   if (argv[1][0]=='-') { argv[1]++; }

   if (pfOut!=NULL) { fprintf(pfOut,"%s RUN(Program='%s' Owner='%s' Command='%s' ParameterCount=%d MINCC=%d MAXCC=%d)\n", cstime(0,acTs), argv[0], pcOwn, argv[1], argc-2, siMinCC, siMaxCC); }

   /* Find command or built-in function with abbreviation support */
   siType=0; siIdx=-1; siRes=-1; siMln=0;
   siRes=siCleFindCommand(isCas,argv[1],psCmd,asBif,siMkl,&siType,&siIdx,&siMln);

   if (siRes == 0) {
      if (siType == 1) { /* Found built-in function - dispatch via function pointer */
         TsCleBuiltinContext stCtx;

         /* Initialize context structure */
         stCtx.argc = argc;
         stCtx.argv = argv;
         stCtx.pfOut = pfOut;
         stCtx.pfErr = pfErr;
         stCtx.pfStd = pfStd;
         stCtx.pfTrc = pfTrc;
         stCtx.pcDep = pcDep;
         stCtx.pcOpt = pcOpt;
         stCtx.pcEnt = pcEnt;
         stCtx.pcOwn = pcOwn;
         stCtx.pcPgm = pcPgm;
         stCtx.pcBld = pcBld;
         stCtx.pcDef = pcDef;
         stCtx.psBif = asBif;
         stCtx.psCmd = psCmd;
         stCtx.isCas = isCas;
         stCtx.isPfl = isPfl;
         stCtx.isRpl = isRpl;
         stCtx.siMkl = siMkl;
         stCtx.psCnf = psCnf;
         stCtx.pvGbl = pvGbl;
         stCtx.ppvHdl = &pvHdl;
         stCtx.pvF2S = pvF2S;
         stCtx.pfF2S = pfF2S;
         stCtx.pvSaf = pvSaf;
         stCtx.pfSaf = pfSaf;
         stCtx.pfMsg = pfMsg;
         stCtx.pcLic = pcLic;
         stCtx.pcVsn = pcVsn;
         stCtx.pcAbo = pcAbo;
         stCtx.pcHlp = pcHlp;
         stCtx.pcAut = pcAut;
         stCtx.pcAdr = pcAdr;
         stCtx.pcDpa = pcDpa;
         stCtx.pcPgmNum = pcPgmNum;
         stCtx.pcBifNum = pcBifNum;
         stCtx.pcCmdNum = pcCmdNum;
         stCtx.pcPgmMan = pcPgmMan;
         stCtx.pcSccMan = pcSccMan;
         stCtx.pcHom = pcHom;
         stCtx.ppcCnf = &pcCnf;
         stCtx.pszCnf = &szCnf;
         stCtx.psiDep = &siDep;
         stCtx.psEnv = psEnvTab;
         stCtx.pcEnv = pcEnv;
         stCtx.psDoc = psDoc;
         stCtx.psOth = psOth;

         /* Call built-in function via function pointer */
         siErr = asBif[siIdx].pfFunc(&stCtx);
         ERROR(ELIMIT(siErr));
      } else if (siType == 2) { /* Found command - execute it */
         int                           siOid=0;
         char*                         pcCmd=NULL;
         char*                         pcTls=NULL;
         char*                         pcLst=NULL;
         clock_t                       ckCpu1=clock();
         clock_t                       ckCpu2;

         siErr=siCleGetCommand(pfOut,pfErr,pfTrc,pcDep,psCmd[siIdx].pcKyw,argc,argv,&pcFil,&pcCmd,pvGbl,pvF2S,pfF2S,siMln,pcDpa);
         if (siErr) {
            ERROR(ELIMIT(siErr));
         }
         if (pfOut!=NULL) {
            ckCpu2=clock();
            fprintf(pfOut,"%s Determination of parameter string for command '%s' was successful (CPU time %7.5fs)\n",
                    cstime(0,acTs),psCmd[siIdx].pcKyw,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
            ckCpu1=ckCpu2;
         }

         siErr=siCleCommandInit(pvGbl,psCmd[siIdx].pfIni,psCmd[siIdx].pvClp,pcOwn,pcPgm,pcBld,psCmd[siIdx].pcKyw,psCmd[siIdx].pcMan,
                                psCmd[siIdx].pcHlp,psCmd[siIdx].piOid,psCmd[siIdx].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,
                                pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf,pcCmd);
         if (siErr) {
            SAFE_FREE(pcCmd);
            ERROR(ELIMIT(siErr));
         }
         if (pfOut!=NULL) {
            ckCpu2=clock();
            fprintf(pfOut,"%s Initializing of command '%s' was successful (CPU time %7.5fs)\n",
                    cstime(0,acTs),psCmd[siIdx].pcKyw,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
            ckCpu1=ckCpu2;
         }

         siErr=siClpParseCmd(pvHdl,pcFil,pcCmd,TRUE,TRUE,psCmd[siIdx].piOid,&pcTls);
         if (siErr<0) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"%s Command line parser for command '%s' failed\n",cstime(0,acTs),psCmd[siIdx].pcKyw);
            }
            SAFE_FREE(pcCmd);
            siErr=CLERTC_SYN;
            ERROR(ELIMIT(siErr));
         }
         if (psCmd[siIdx].piOid!=NULL) {
            siOid=*psCmd[siIdx].piOid;
         }
         if (pcTls!=NULL) {
            pcLst=(char*)malloc(strlen(pcTls)+1);
            if (pcLst!=NULL) {
               strcpy(pcLst,pcTls);
            }
         }
         vdClpClose(pvHdl,CLPCLS_MTD_KEP);
         if (pfOut!=NULL) {
            ckCpu2=clock();
            fprintf(pfOut,"%s Parsing of parameter string for command '%s' was successful (CPU time %7.5fs)\n",
                    cstime(0,acTs),psCmd[siIdx].pcKyw,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
            ckCpu1=ckCpu2;
         }

         siErr=psCmd[siIdx].pfMap(pvHdl,pfErr,pfTrc,pvGbl,siOid,psCmd[siIdx].pvClp,psCmd[siIdx].pvPar);
         if (siErr) {
            if (siErr!=siNoR) {
               if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"%s Mapping of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",
                             cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_MAP,siErr,pcMsg);
                  }
               } else {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"%s Mapping of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",
                             cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_MAP,siErr);
                  }
               }
               siErr=CLERTC_MAP;
            } else {
               siErr=CLERTC_OK;
            }
            SAFE_FREE(pcCmd); SAFE_FREE(pcLst);
            psCmd[siIdx].pfFin(pfErr,pfTrc,pvGbl,siOid,psCmd[siIdx].pvPar);
            ERROR(ELIMIT(siErr));
         }
         if (pfOut!=NULL) {
            ckCpu2=clock();
            fprintf(pfOut,"%s Mapping of parameter structures for command '%s' was successful (CPU time %7.5fs)\n",
                    cstime(0,acTs),psCmd[siIdx].pcKyw,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
            ckCpu1=ckCpu2;
         }

         siErr=psCmd[siIdx].pfRun(pvHdl,pfErr,pfTrc,pvGbl,pcOwn,pcPgm,pcVsn,pcAbo,pcLic,psCmd[siIdx].pcKyw,pcCmd,pcLst,siOid,
                              psCmd[siIdx].pvPar,&isWrn,&siScc);
         SAFE_FREE(pcCmd); SAFE_FREE(pcLst);
         if (siErr) {
            if (isWrn&0x00010000) {
               if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"%s Run of command '%s' ends with warning (Return code: %d / Reason code: %d (%s))\n",
                             cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_WRN,siErr,pcMsg);
                  }
               } else {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"%s Run of command '%s' ends with warning (Return code: %d / Reason code: %d)\n",
                             cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_WRN,siErr);
                  }
               }
               siErr=CLERTC_WRN;
            } else {
               if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"%s Run of command '%s' failed (Return code: %d / Reason code: %d (%s))\n",
                             cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_RUN,siErr,pcMsg);
                  }
               } else {
                  if (pfErr!=NULL) {
                     fprintf(pfErr,"%s Run of command '%s' failed (Return code: %d / Reason code: %d)\n",
                             cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_RUN,siErr);
                  }
               }
               siErr=CLERTC_RUN;
            }
            psCmd[siIdx].pfFin(pfErr,pfTrc,pvGbl,siOid,psCmd[siIdx].pvPar);
            ERROR(ELIMIT(siErr));
         }
         if (pfOut!=NULL) {
            ckCpu2=clock();
            fprintf(pfOut,"%s Run of command '%s' was successful (CPU time %7.5fs)\n",
                    cstime(0,acTs),psCmd[siIdx].pcKyw,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
            ckCpu1=ckCpu2;
         }

         siErr=psCmd[siIdx].pfFin(pfErr,pfTrc,pvGbl,siOid,psCmd[siIdx].pvPar);
         if (siErr) {
            if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
               if (pfErr!=NULL) {
                  fprintf(pfErr,"%s Finish/cleanup for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",
                          cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_FIN,siErr,pcMsg);
               }
            } else {
               if (pfErr!=NULL) {
                  fprintf(pfErr,"%s Finish/cleanup for command '%s' failed (Return code: %d / Reason code: %d)\n",
                          cstime(0,acTs),psCmd[siIdx].pcKyw,CLERTC_FIN,siErr);
               }
            }
            siErr=CLERTC_FIN;
            ERROR(ELIMIT(siErr));
         }
         if (pfOut!=NULL) {
            ckCpu2=clock();
            fprintf(pfOut,"%s Finalize for command '%s' was successful (CPU time %7.5fs)\n",
                    cstime(0,acTs),psCmd[siIdx].pcKyw,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
         }
         siErr=isWrn&0x00000001;
         ERROR(ELIMIT(siErr));
      } else { /* Command not found - try default command or error */
         if (pcDef!=NULL && *pcDef && ppArg==NULL) {
            ppArg=malloc((argc+1)*sizeof(*ppArg));
            if (ppArg == NULL) {
               if (pfErr!=NULL) {
                  fprintf(pfErr,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef);
               }
               siErr=CLERTC_MEM;
               ERROR(ELIMIT(siErr));
            }
            for (i=argc;i>1;i--) { ppArg[i]=argv[i-1]; }
            ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc++; argv=ppArg;
            goto EVALUATE;
         }
         if (pfErr!=NULL) {
            fprintf(pfErr,"Command or built-in function '%s' not supported\n",argv[1]);
            if (argc>1 && strlen(argv[1])<siMkl && strpbrk(argv[1],"=(.")==NULL) {
               fprintf(pfErr,"Note: Minimum abbreviation length is %d characters\n",siMkl);
            }
            vdPrnStaticSyntax(pfErr,psCmd,asBif,argv[0],pcDep,pcOpt,pcDpa);
         }
         siErr=CLERTC_CMD;
         ERROR(ELIMIT(siErr));
      }
   } else if (siRes == 1) { /* Ambiguous abbreviation */
      if (pfErr!=NULL) {
         const char* pcStop=strpbrk(argv[1],"=(.");
         int siInputLen=(pcStop!=NULL) ? (int)(pcStop-argv[1]) : (int)strlen(argv[1]);
         char acInput[siInputLen+1];

         memcpy(acInput,argv[1],siInputLen);
         acInput[siInputLen]='\0';

         fprintf(pfErr,"Ambiguous abbreviation '%s'. Matching commands/functions:\n",acInput);
         /* List all matches from built-in functions */
         for (i=0;asBif[i].pcKyw!=NULL;i++) {
            if (asBif[i].isBif && siInputLen>=(int)siMkl &&
                strxcmp(isCas,acInput,asBif[i].pcKyw,siMkl,0,FALSE)==0) {
               fprintf(pfErr,"  - %s (built-in function)\n",asBif[i].pcKyw);
            }
         }
         /* List all matches from commands */
         for (i=0;psCmd[i].pcKyw!=NULL;i++) {
            if (siInputLen>=(int)siMkl &&
                strxcmp(isCas,acInput,psCmd[i].pcKyw,siMkl,0,FALSE)==0) {
               fprintf(pfErr,"  - %s (command)\n",psCmd[i].pcKyw);
            }
         }
      }
      siErr=CLERTC_CMD;
      ERROR(ELIMIT(siErr));
   } else { /* Command not found (siRes != 0 and siRes != 1) - try default command or error */
      if (pcDef!=NULL && *pcDef && ppArg==NULL) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            if (pfErr!=NULL) {
               fprintf(pfErr,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef);
            }
            siErr=CLERTC_MEM;
            ERROR(ELIMIT(siErr));
         }
         for (i=argc;i>1;i--) { ppArg[i]=argv[i-1]; }
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc++; argv=ppArg;
         goto EVALUATE;
      }
      if (pfErr!=NULL) {
         fprintf(pfErr,"Command or built-in function '%s' not supported\n",argv[1]);
         if (argc>1 && strlen(argv[1])<siMkl && strpbrk(argv[1],"=(.")==NULL) {
            fprintf(pfErr,"Note: Minimum abbreviation length is %d characters\n",siMkl);
         }
         vdPrnStaticSyntax(pfErr,psCmd,asBif,argv[0],pcDep,pcOpt,pcDpa);
      }
      siErr=CLERTC_CMD;
      ERROR(ELIMIT(siErr));
   }
}
#undef ERROR
#undef ELIMIT

/* Interne Funktionen *************************************************/

static int siClePropertyInit(
   void*                         pvGbl,
   TfIni*                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const int*                    piOid,
   const TsClpArgument*          psTab,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   char**                        ppFil,
   int*                          piFil,
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf)
{
   int                           siErr;
   char*                         pcPro=NULL;
   char*                         pcFil=NULL;
   int                           siFil=0;

   if (piFil!=NULL) { *piFil=0; }
   if (ppFil!=NULL) { *ppFil=NULL; }

   *ppHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcBld,pcCmd,pcMan,pcHlp,piOid!=NULL,psTab,pvClp,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Open of property parser for command '%s' failed\n",pcCmd); }
      return(CLERTC_TAB);
   }

   siErr=pfIni(*ppHdl,pfErr,pfTrc,pvGbl,pcOwn,pcPgm,0,pvClp);
   if (siErr) {
      const char* pcMsg;
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg); }
      } else {
         if (pfErr!=NULL) { fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr); }
      }
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(CLERTC_INI);
   }

   siErr=siCleGetProperties(pfErr,psCnf,pcOwn,pcPgm,pcCmd,&pcFil,&pcPro,&siFil,pvGbl,pvF2S,pfF2S);
   if (siErr) {
      SAFE_FREE(pcPro);
      SAFE_FREE(pcFil);
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(siErr);
   }

   if (pcPro!=NULL) {
      siErr=siClpParsePro(*ppHdl,pcFil,pcPro,FALSE,NULL);
      if (siErr<0) {
         if (pfErr!=NULL) { fprintf(pfErr,"Parsing property file \"%s\" for command '%s' failed\n",pcFil,pcCmd); }
         vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
         free(pcPro); SAFE_FREE(pcFil);
         return(CLERTC_SYN);
      }
      free(pcPro);
   }
   if (ppFil!=NULL) { *ppFil=pcFil; } else {
      SAFE_FREE(pcFil);
   }
   if (piFil!=NULL) { *piFil=siFil; }
   return(CLERTC_OK);
}

static int siClePropertyFinish(
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   TsCnfHdl*                     psCnf,
   void*                         pvHdl,
   const char*                   pcFil,
   const int                     siFil)
{
   (void)                        pfTrc;
   int                           siErr;
   FILE*                         pfPro;
   char                          acEnv[((pcHom!=NULL)?strlen(pcHom):0)+strlen(pcOwn)+strlen(pcPgm)+strlen(pcCmd)+64];
   size_t                        szEnv=0;
   char*                         pcEnv=NULL;
   char*                         pcHlp=NULL;

   if (siFil!=3) {
      int i;
      snprintf(acEnv,sizeof(acEnv),"%s_%s_%s_PROPERTY_FILENAME",pcOwn,pcPgm,pcCmd);
      for (i=0;acEnv[i];i++) { acEnv[i]=toupper(acEnv[i]); }
      pcFil=GETENV(acEnv);
      if (pcFil!=NULL && *pcFil) {
         srprintf(&pcEnv,&szEnv,strlen(pcFil),"%s",pcFil);
         if (pcEnv==NULL) {
            if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for property file name (%s) failed\n",pcFil); }
            vdClpClose(pvHdl,CLPCLS_MTD_ALL);
            return(CLERTC_MEM);
         }
         pcFil=pcEnv;
      } else {
#ifdef __ZOS__
         {
            int  j=9;
            strcpy(acEnv,"<SYSUID>.");
            for (i=0;i<8 && pcOwn[i];i++) {
               if (isalnum(pcOwn[i])) {
                  acEnv[j]=toupper(pcOwn[i]);
                  j++;
               }
            }
            if (i) acEnv[j]='.'; j++;
            for (i=0;i<8 && pcPgm[i];i++) {
               if (isalnum(pcPgm[i])) {
                  acEnv[j]=toupper(pcPgm[i]);
                  j++;
               }
            }
            if (i) acEnv[j]='.'; j++;
            for (i=0;i<8 && pcCmd[i];i++) {
               if (isalnum(pcCmd[i])) {
                  acEnv[j]=toupper(pcCmd[i]);
                  j++;
               }
            }
            acEnv[j]=0x00;
            strcat(acEnv,".PROPS");
         }
#else
         if (pcHom!=NULL && *pcHom) {
            snprintf(acEnv,sizeof(acEnv),"%s.%s.%s.%s.properties",pcHom,pcOwn,pcPgm,pcCmd);
         } else {
            snprintf(acEnv,sizeof(acEnv),".%s.%s.%s.properties",pcOwn,pcPgm,pcCmd);
         }
         for (i=0;acEnv[i];i++) { acEnv[i]=tolower(acEnv[i]); }
#endif
         pcFil=acEnv;
      }
   }
   pcHlp=dcpmapfil(pcFil);
   if (pcHlp==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for property file name (%s) failed\n",pcFil); }
      SAFE_FREE(pcEnv);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      return(CLERTC_MEM);
   }
   pfPro=fopen_hfq(pcHlp,"w");
   if (pfPro==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Cannot open the property file (\"%s\",\"%s\") for write operation (%d-%s)\n",pcHlp,"w",errno,pcSysError(errno)); }
      SAFE_FREE(pcHlp); SAFE_FREE(pcEnv);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      return(CLERTC_SYS);
   }

   siErr=siClpProperties(pvHdl,CLPPRO_MTD_CMT,10,pcCmd,pfPro);
   vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   if (fclose(pfPro) || siErr<0) {
      if (pfErr!=NULL) { fprintf(pfErr,"Write property file (%s) for command '%s' failed (%d-%s)\n",pcHlp,pcCmd,errno,pcSysError(errno)); }
      SAFE_FREE(pcHlp); SAFE_FREE(pcEnv);
      return(CLERTC_SYN);
   }
   if (pfOut!=NULL) { fprintf(pfOut,"Property file (%s) for command '%s' successfully written\n",pcHlp,pcCmd); }

   if (siFil!=3) {
      snprintf(acEnv,sizeof(acEnv),"%s.%s.%s.property.file",pcOwn,pcPgm,pcCmd);
      siErr=siCnfSet(psCnf,pfErr,acEnv,pcHlp,TRUE);
      if (siErr) {
         if (pfErr!=NULL) { fprintf(pfErr,"Activation of property file (%s) for command '%s' failed\n",pcFil,pcCmd); }
         SAFE_FREE(pcHlp); SAFE_FREE(pcEnv);
         return(CLERTC_CFG);
      }
      if (pfOut!=NULL) {
         fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",acEnv,pcFil);
         fprintf(pfOut,"Activation of property file (%s) for command '%s' was successful\n",pcFil,pcCmd);
      }
   }
   SAFE_FREE(pcHlp); SAFE_FREE(pcEnv);
   return(CLERTC_OK);
}

static int siCleCommandInit(
   void*                         pvGbl,
   TfIni*                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   int*                          piOid,
   const TsClpArgument*          psTab,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf,
   const char*                   pcStr)
{
   int                           siErr,siFil=0;
   int                           siOid=0;
   char*                         pcFil=NULL;
   char*                         pcPro=NULL;
   clock_t                       ckCpu1=clock();
   clock_t                       ckCpu2;
   char                          acTs[24];

   *ppHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcBld,pcCmd,pcMan,pcHlp,piOid!=NULL,psTab,pvClp,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Open of parser for command '%s' failed\n",pcCmd); }
      return(CLERTC_TAB);
   }
   if (pfTrc!=NULL) {
      ckCpu2=clock();
      fprintf(pfTrc,"%s Open of command line parser (CLP) for command '%s' was successful (CPU time %7.5fs)\n",cstime(0,acTs),pcCmd,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
      ckCpu1=ckCpu2;
   }

   if (piOid!=NULL) {
      siOid=siClpParseOvl(*ppHdl,pcStr);
      *piOid=siOid;
   }

   siErr=pfIni(*ppHdl,pfErr,pfTrc,pvGbl,pcOwn,pcPgm,siOid,pvClp);
   if (siErr) {
      const char* pcMsg;
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg); }
      } else {
         if (pfErr!=NULL) { fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr); }
      }
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(CLERTC_INI);
   }
   if (pfTrc!=NULL) {
      ckCpu2=clock();
      fprintf(pfTrc,"%s Initialization of CLP structure for command '%s' was successful (CPU time %7.5fs)\n",cstime(0,acTs),pcCmd,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
      ckCpu1=ckCpu2;
   }
   siErr=siCleGetProperties(pfErr,psCnf,pcOwn,pcPgm,pcCmd,&pcFil,&pcPro,&siFil,pvGbl,pvF2S,pfF2S);
   if (siErr) {
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      SAFE_FREE(pcPro); SAFE_FREE(pcFil);
      return(siErr);
   }
   if (pfTrc!=NULL) {
      ckCpu2=clock();
      fprintf(pfTrc,"%s Determine properties for command '%s' was successful (CPU time %7.5fs)\n",cstime(0,acTs),pcCmd,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
      ckCpu1=ckCpu2;
   }
   if (pcPro!=NULL) {
      siErr=siClpParsePro(*ppHdl,pcFil,pcPro,FALSE,NULL);
      if (siErr<0) {
         if (pfErr!=NULL) { fprintf(pfErr,"Property parser for command '%s' failed\n",pcCmd); }
         vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
         free(pcPro); SAFE_FREE(pcFil);
         return(CLERTC_SYN);
      }
      free(pcPro);
      if (pfTrc!=NULL) {
         ckCpu2=clock();
         fprintf(pfTrc,"%s Parsing of properties for command '%s' was successful (CPU time %7.5fs)\n",cstime(0,acTs),pcCmd,((double)(ckCpu2-ckCpu1))/CLOCKS_PER_SEC);
      }
   }
   SAFE_FREE(pcFil);
   return(CLERTC_OK);
}

static int siCleSimpleInit(
   FILE*                         pfOut,
   FILE*                         pfErr,
   const int                     isPfl,
   const int                     isRpl,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   void**                        ppHdl)
{
   const TsClpArgument asTab[]={
         {CLPTYP_NUMBER,"XX",NULL,0,1,1,0,0,CLPFLG_NON,NULL,NULL,NULL,"XX",0,0.0,NULL,"NUMBER"},
         {CLPTYP_NON   ,NULL,NULL,0,0,0,0,0,CLPFLG_NON,NULL,NULL,NULL,NULL,0,0.0,NULL,NULL}
   };
   *ppHdl=pvClpOpen(FALSE,isPfl,isRpl,0,"","","","","","",FALSE,asTab,"",pfOut,pfErr,NULL,NULL,NULL,NULL,pcDep,pcOpt,pcEnt,NULL,NULL,NULL,NULL,NULL,NULL);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Open of command line parser for grammar and lexeme print out failed\n"); }
      return(CLERTC_TAB);
   }
   return(CLERTC_OK);
}

static int siCleChangeProperties(
   void*                         pvGbl,
   TfIni*                        pfIni,
   void*                         pvClp,
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const char*                   pcPro,
   const int*                    piOid,
   const TsClpArgument*          psTab,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfErr,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf)
{
   int                           siErr;
   void*                         pvHdl=NULL;
   char*                         pcFil=NULL;
   int                           siFil=0;

   siErr=siClePropertyInit(pvGbl,pfIni,pvClp,pcOwn,pcPgm,pcBld,pcCmd,pcMan,pcHlp,
                           piOid,psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,
                           pcDep,pcOpt,pcEnt,psCnf,&pvHdl,&pcFil,&siFil,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
   if (siErr) {
      SAFE_FREE(pcFil);
      return(siErr);
   }

   siErr=siClpParsePro(pvHdl,pcFil,pcPro,TRUE,NULL);
   if (siErr<0) {
      if (pfErr!=NULL) { fprintf(pfErr,"Property parser for command '%s' failed\n",pcCmd); }
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      SAFE_FREE(pcFil);
      return(CLERTC_SYN);
   }

   if (siErr==0) {
      if (pfOut!=NULL) { fprintf(pfOut, "Don't update any property in property file (%s)\n",(NULL==pcFil)?"null":pcFil); }
   } else if (siErr==1) {
      if (pfOut!=NULL) { fprintf(pfOut, "Updated 1 property in property file (%s)\n",(NULL==pcFil)?"null":pcFil); }
   } else {
      if (pfOut!=NULL) { fprintf(pfOut, "Updated %d properties in property file (%s)\n",siErr,(NULL==pcFil)?"null":pcFil); }
   }

   siErr=siClePropertyFinish(pcHom,pcOwn,pcPgm,pcCmd,pfOut,pfErr,pfTrc,psCnf,pvHdl,pcFil,siFil);
   if (siErr) {
      SAFE_FREE(pcFil);
      return(siErr);
   }

   SAFE_FREE(pcFil);
   return(CLERTC_OK);
}

static int siCleEndExecution(
   const int                     siErr,
   TsCnfHdl*                     psCnf,
   FILE*                         pfTrc,
   FILE*                         pfDoc,
   FILE*                         pfPro,
   char**                        ppArg,
   void*                         pvHdl)
{
   if (psCnf!=NULL) { vdCnfCls(psCnf); }
   if (pfTrc!=NULL) { fclose_unchecked(pfTrc); }
   if (pfDoc!=NULL) { fclose_unchecked(pfDoc); }
   if (pfPro!=NULL) { fclose_unchecked(pfPro); }
   if (ppArg!=NULL) { free(ppArg); }
   if (pvHdl!=NULL) { vdClpClose(pvHdl,CLPCLS_MTD_ALL); }
   return(siErr);
}

static void vdCleManProgram(
   FILE*                         pfOut,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcHlp,
   const char*                   pcMan,
   const char*                   pcDep,
   const char*                   pcSep,
   const char*                   pcDpa,
   const char*                   pcNum,
   const int                     isMan,
   const int                     isNbr)
{
   int                           i,l;
   if (isMan) {
      for(const char* p=pcPgm;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
      fprintf(pfOut, "(1)\n");
      l=strlen(pcPgm)+3;
      for (i=0;i<l;i++) { fprintf(pfOut,"="); }
      fprintf(pfOut, "\n");
      fprintf(pfOut, ":doctype: manpage\n\n");
      fprintf(pfOut, "NAME\n");
      fprintf(pfOut, "----\n\n");
      for(const char* p=pcPgm;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
      efprintf(pfOut," - `%s`\n\n",pcHlp);
      fprintf(pfOut, "SYNOPSIS\n");
      fprintf(pfOut, "--------\n\n");
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      fprintf(pfOut, "PATH:   %s\n",pcOwn);
      fprintf(pfOut, "TYPE:   PROGRAM\n");
      fprintf(pfOut, "SYNTAX: > %s COMMAND/FUNCTION ...\n",pcPgm);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
      fprintf(pfOut, "DESCRIPTION\n");
      fprintf(pfOut, "-----------\n\n");
      if (pcMan!=NULL && *pcMan) {
         fprintm(pfOut,pcOwn,pcPgm,pcBld,pcMan,2);
      } else {
         fprintf(pfOut,"No detailed description available for this program.\n\n");
      }
      fprintf(pfOut,"AUTHOR\n");
      fprintf(pfOut,"------\n\n");
      fprintf(pfOut,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
      fprintf(pfOut,"SEE ALSO\n");
      fprintf(pfOut,"--------\n\n");
      for (i=0;psCmd[i].pcKyw!=NULL;i++) {
         if (psCmd[i].siFlg) {
            for(const char* p=pcPgm;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
            fprintf(pfOut,".");
            for(const char* p=psCmd[i].pcKyw;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
            fprintf(pfOut,"(1)\n");
         }
      }
      fprintf(pfOut,"\n");
   } else {
      if (isNbr) {
         fprintf(pfOut,"%s Program '%s'\n",pcNum,pcPgm);
         l=strlen(pcNum)+strlen(pcPgm)+11;
         for (i=0;i<l;i++) { fprintf(pfOut,"%c",'-'); }
         fprintf(pfOut,"\n\n");
         fprintf(pfOut,"%s1. Synopsis\n",pcNum);
         l=strlen(pcNum)+11;
         for (i=0;i<l;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"Program '%s'\n",pcPgm);
         l=strlen(pcPgm)+10;
         for (i=0;i<l;i++) { fprintf(pfOut,"%c",'-'); }
         fprintf(pfOut,"\n\n");
         fprintf(pfOut,"Synopsis\n");
         for (i=0;i<8;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      }
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      efprintf(pfOut,"HELP:   %s\n",pcHlp);
      fprintf(pfOut, "PATH:   %s\n",pcOwn);
      fprintf(pfOut, "TYPE:   PROGRAM\n");
      fprintf(pfOut, "SYNTAX: > %s COMMAND/FUNCTION ...\n",pcPgm);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");

      if (isNbr) {
         fprintf(pfOut,"%s2. Description\n",pcNum);
         l=strlen(pcNum)+14;
         for (i=0;i<l;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"Description\n");
         for (i=0;i<11;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      }
      if (pcMan!=NULL && *pcMan) {
         fprintm(pfOut,pcOwn,pcPgm,pcBld,pcMan,2);
      } else {
         fprintf(pfOut,"No detailed description available for this program.\n\n");
      }
      if (isNbr) {
         fprintf(pfOut,"%s3. Syntax\n",pcNum);
         l=strlen(pcNum)+9;
         for (i=0;i<l;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"Syntax\n");
         for (i=0;i<6;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,pcBld,MAN_CLEP_PROGRAM_SYNTAX,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Syntax for program '%s':\n",pcPgm);
      vdPrnStaticSyntax(pfOut,psCmd,psBif,pcPgm,pcDep,pcSep,pcDpa);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");

      if (isNbr) {
         fprintf(pfOut,"%s4. Help\n",pcNum);
         l=strlen(pcNum)+7;
         for (i=0;i<l;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"Help\n");
         for (i=0;i<4;i++) { fprintf(pfOut,"%c",C_TLD); }
         fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,pcBld,MAN_CLEP_PROGRAM_HELP,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Help for program '%s':\n",pcPgm);
      vdPrnStaticHelp(pfOut,psCmd,psBif,pcPgm,pcDep);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");
   }
}

static void vdCleManFunction(
   FILE*                         pfOut,
   const unsigned int            uiLev,
   const char*                   pcLev,
   const char*                   pcNum,
   const char*                   pcFct,
   const char*                   pcHlp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcSyn,
   const char*                   pcMan,
   const int                     isMan,
   const int                     isPat,
   const int                     isNbr,
   const int                     isShl,
   const int                     isIdt)
{
   int                           i,l;
   if (isMan) {
      for(const char* p=pcPgm;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
      fprintf(pfOut, ".");
      for(const char* p=pcFct;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
      fprintf(pfOut, "(1)\n");
      l=strlen(pcPgm)+strlen(pcFct)+4;
      for (i=0;i<l;i++) { fprintf(pfOut,"="); }
      fprintf(pfOut, "\n");
      fprintf(pfOut, ":doctype: manpage\n\n");
      fprintf(pfOut, "NAME\n");
      fprintf(pfOut, "----\n\n");
      for(const char* p=pcPgm;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
      fprintf(pfOut, ".");
      for(const char* p=pcFct;*p;p++) { fprintf(pfOut,"%c",tolower(*p)); }
      efprintf(pfOut," - `%s`\n\n",pcHlp);
      fprintf(pfOut, "SYNOPSIS\n");
      fprintf(pfOut, "--------\n\n");
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      fprintf(pfOut, "PATH:   %s.%s\n",pcOwn,pcPgm);
      fprintf(pfOut, "TYPE:   BUILT-IN FUNCTION\n");
      fprintf(pfOut, "SYNTAX: > %s %s\n",pcPgm,pcSyn);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
      fprintf(pfOut, "DESCRIPTION\n");
      fprintf(pfOut, "-----------\n\n");
      fprintm(pfOut,pcOwn,pcPgm,pcBld,pcMan,2);
      fprintf(pfOut, "AUTHOR\n------\n\n");
      fprintf(pfOut, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
   } else {
      if (uiLev) {
         efprintf(pfOut,"[[CLEP.BUILTIN.%s]]\n",pcFct);
         for (unsigned int j=0;j<uiLev;j++) { efprintf(pfOut,"="); }
         if (isNbr) { efprintf(pfOut," %s",pcNum); }
         if (isShl) {
            efprintf(pfOut," %s\n\n",pcFct);
         } else {
            efprintf(pfOut," Function '%s'\n\n",pcFct);
         }
         if (isIdt) { efprintf(pfOut,"indexterm:[Built-in Function, %s]\n\n",pcFct); }
         efprintf(pfOut,".Synopsis\n\n");
         efprintf(pfOut,"-----------------------------------------------------------------------\n");
         efprintf(pfOut,"HELP:   %s\n",pcHlp);
         if(isPat) {
         efprintf(pfOut,"PATH:   %s.%s\n",pcOwn,pcPgm); }
         efprintf(pfOut,"TYPE:   BUILT-IN FUNCTION\n");
         efprintf(pfOut,"SYNTAX: > %s %s\n",pcPgm,pcSyn);
         efprintf(pfOut,"-----------------------------------------------------------------------\n\n");
         efprintf(pfOut,".Description\n\n");
         fprintm(pfOut,pcOwn,pcPgm,pcBld,pcMan,2);
      } else {
         efprintf(pfOut,"[[CLEP.BUILTIN.%s]]\n",pcFct);
         if (isNbr) {
            if (isShl) {
               fprintf(pfOut,"%s %s\n",pcNum,pcFct);
               l=strlen(pcNum)+strlen(pcFct)+1;
               for (i=0;i<l;i++) { fprintf(pfOut,"%s",pcLev); }
               fprintf(pfOut,"\n\n");
            } else {
               fprintf(pfOut,"%s Function '%s'\n",pcNum,pcFct);
               l=strlen(pcNum)+strlen(pcFct)+12;
               for (i=0;i<l;i++) { fprintf(pfOut,"%s",pcLev); }
               fprintf(pfOut,"\n\n");
            }
         } else {
            if (isShl) {
               fprintf(pfOut,"%s\n",pcFct);
               l=strlen(pcFct);
               for (i=0;i<l;i++) { fprintf(pfOut,"%s",pcLev); }
               fprintf(pfOut,"\n\n");
            } else {
               fprintf(pfOut,"Function '%s'\n",pcFct);
               l=strlen(pcFct)+11;
               for (i=0;i<l;i++) { fprintf(pfOut,"%s",pcLev); }
               fprintf(pfOut,"\n\n");
            }
         }
         if (isIdt) { efprintf(pfOut,"indexterm:[Built-in Function, %s]\n\n",pcFct); }
         fprintf(pfOut, ".Synopsis\n\n");
         fprintf(pfOut, "-----------------------------------------------------------------------\n");
         efprintf(pfOut,"HELP:   %s\n",pcHlp);
         if(isPat) {
         fprintf(pfOut, "PATH:   %s.%s\n",pcOwn,pcPgm); }
         fprintf(pfOut, "TYPE:   BUILT-IN FUNCTION\n");
         fprintf(pfOut, "SYNTAX: > %s %s\n",pcPgm,pcSyn);
         fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
         fprintf(pfOut, ".Description\n\n");
         fprintm(pfOut,pcOwn,pcPgm,pcBld,pcMan,2);
      }
   }
}

static void vdPrnStaticSyntax(
   FILE*                         pfOut,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const char*                   pcPgm,
   const char*                   pcDep,
   const char*                   pcSep,
   const char*                   pcDpa)
{
   int                           f=FALSE;
   fprintf(pfOut,"%s Commands: ",pcDep);
   for (int i=0;psCmd[i].pcKyw!=NULL;i++) {
      if (psCmd[i].siFlg) {
         if (f) { fprintf(pfOut,"%s",pcSep); }
         fprintf(pfOut,"%s",psCmd[i].pcKyw);
         f=TRUE;
      }
   }
   fprintf(pfOut,"\n");
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command \"... argument list ...\" %cMAXCC=%cmax%c%c-min%c%c %cQUIET/SILENT%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command=\" parameter file name \" %cMAXCC=%cmax%c%c-min%c%c %cQUIET/SILENT%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command=>\"parameter file name \" %cMAXCC=%cmax%c%c-min%c%c %cQUIET/SILENT%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
   fprintf(pfOut,"%s%s You can optionally specify:\n",pcDep,pcDep);
   fprintf(pfOut,"%s%s%s the owner id for this command (to use custom configuration files)\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s the maximum condition code (max) to suppress warnings\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s the minimum condition code (min), zero is returned if the condition code would be smaller\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s QUIET disables the normal log output of the command line executer\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s SILENT disables log and errors messages of the command line executer\n",pcDep,pcDep,pcDep);
   if (pcDpa!=NULL) {
   fprintf(pfOut,"%s%s Additional the default parameter file '%s' is supported if only the command provided\n",pcDep,pcDep,pcDpa);
   }
   fprintf(pfOut,"%s Built-in functions:\n",pcDep);
   for (int i=0;psBif[i].pcKyw!=NULL;i++) {
      if (psBif[i].isBif) {
         efprintf(pfOut,"%s%s %s %s\n",pcDep,pcDep,pcPgm,psBif[i].pcSyn);
      }
   }
}

static void vdPrnStaticHelp(
   FILE*                         pfOut,
   const TsCleCommand*           psCmd,
   const TsCleBuiltin*           psBif,
   const char*                   pcPgm,
   const char*                   pcDep)
{
   fprintf(pfOut,"%s Commands - to execute powerful subprograms\n",pcDep);
   for (int i=0;psCmd[i].pcKyw!=NULL;i++) {
      if (psCmd[i].siFlg) {
         fprintf(pfOut,"%s%s %s %-8.8s - %s\n",pcDep,pcDep,pcPgm,psCmd[i].pcKyw,psCmd[i].pcHlp);
      }
   }
   fprintf(pfOut,"%s Built-in functions - to give interactive support for the commands above\n",pcDep);
   for (int i=0;psBif[i].pcKyw!=NULL;i++) {
      if (psBif[i].isBif) {
         efprintf(pfOut,"%s%s %s %-8.8s - %s\n",pcDep,pcDep,pcPgm,psBif[i].pcKyw,psBif[i].pcHlp);
      }
   }
   fprintf(pfOut,"For more information please use the built-in function 'MANPAGE'\n");
}

static void vdPrnCommandSyntax(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcPgm,
   const char*                   pcCmd,
   const char*                   pcDep,
   const int                     siDep)
{
   if (siDep==1) {
      int siErr;
      fprintf(pfOut,"%s %s \"",pcDep,pcPgm);
      siErr=siClpSyntax(pvHdl,FALSE,FALSE,siDep,pcCmd);
      if (siErr==CLP_OK) { fprintf(pfOut,"\"\n"); }
   } else {
      siClpSyntax(pvHdl,TRUE,TRUE,siDep,pcCmd);
   }
}

static void vdPrnCommandHelp(
   void*                   pvHdl,
   const char*             pcCmd,
   const int               siDep,
   const int               isAli,
   const int               isMan)
{
   siClpHelp(pvHdl,siDep,pcCmd,isAli,isMan);
}

static void vdPrnCommandManpage(
   void*                   pvHdl,
   FILE*                   pfOut,
   const char*             pcNum,
   const char*             pcCmd,
   const int               siInd,
   const int               isMan,
   const int               isNbr)
{
   char                    acNum[16];
   snprintf(acNum,sizeof(acNum),"%s%d.",pcNum,siInd+1);
   siClpDocu(pvHdl,pfOut,pcCmd,acNum,"Command",TRUE,FALSE,isMan,FALSE,isNbr,FALSE,TRUE,0);
}

static void vdPrnProperties(
   void*                   pvHdl,
   const char*             pcPat,
   const int               siDep)
{
   siClpProperties(pvHdl,CLPPRO_MTD_SET,siDep,pcPat,NULL);
}

static int siCleGetProperties(
   FILE*                   pfErr,
   TsCnfHdl*               psCnf,
   const char*             pcOwn,
   const char*             pcPgm,
   const char*             pcCmd,
   char**                  ppFil,
   char**                  ppPro,
   int*                    piFlg,
   void*                   pvGbl,
   void*                   pvF2S,
   TfF2S*                  pfF2S)
{
   int                     siErr,siSiz=0;
   const char*             pcHlp=NULL;
   char                    acRoot[strlen(pcOwn)+strlen(pcPgm)+strlen(pcCmd)+17];
   char                    acMsg[1024]="";

   SAFE_FREE(*ppFil);
   snprintf(acRoot,sizeof(acRoot),"%s.%s.%s.property.file",pcOwn,pcPgm,pcCmd);
   pcHlp=pcCnfGet(psCnf,acRoot);
   if (pcHlp==NULL) {
      snprintf(acRoot,sizeof(acRoot),"%s.%s.property.file",pcOwn,pcPgm);
      pcHlp=pcCnfGet(psCnf,acRoot);
      if (pcHlp==NULL) {
         snprintf(acRoot,sizeof(acRoot),"%s.property.file",pcOwn);
         pcHlp=pcCnfGet(psCnf,acRoot);
         if (pcHlp==NULL) {
            *piFlg=0;
            return(CLERTC_OK);
         } else { *piFlg=1; }
      } else { *piFlg=2; }
   } else { *piFlg=3; }
   *ppFil=dcpmapfil(pcHlp);
   if (*ppFil==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for property file name (%s) failed)\n",pcHlp); }
      return(CLERTC_MEM);
   }
   siErr=pfF2S(pvGbl,pvF2S,*ppFil,ppPro,&siSiz,acMsg,sizeof(acMsg));
   if (siErr<0) {
      if (pfErr!=NULL) { fprintf(pfErr,"Property file: %s\n",acMsg); }
      SAFE_FREE(*ppFil);
      SAFE_FREE(*ppPro);
      return(CLERTC_SYS);
   }
   return(CLERTC_OK);
}

static int siCleGetCommand(
   FILE*                   pfOut,
   FILE*                   pfErr,
   FILE*                   pfTrc,
   const char*             pcDep,
   const char*             pcFct,
   int                     argc,
   char*                   argv[],
   char**                  ppFil,
   char**                  ppCmd,
   void*                   pvGbl,
   void*                   pvF2S,
   TfF2S*                  pfF2S,
   const int               siMln,
   const char*             pcDpa)
{
   int                     siErr,siSiz=0;
   int                     l=siMln;  /* Use matched length instead of full command name length */
   SAFE_FREE(*ppFil);
   if (argv[1][l]==EOS) {
      if (argc>2) {
         siErr=arry2str(argv+2,argc-2," ",1,ppCmd,&siSiz);
         if (siErr<0) {
            SAFE_FREE(*ppCmd);
            switch(siErr) {
            case -1: if (pfErr!=NULL) { fprintf(pfErr,"Illegal parameters passed to arry2str() (Bug)\n");                                 } return(CLERTC_FAT);
            case -2: if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,pcSysError(errno)); } return(CLERTC_MEM);
            default: if (pfErr!=NULL) { fprintf(pfErr,"An unknown error occurred while reading command line.\n");                         } return(CLERTC_FAT);
            }
         }
         if (pfTrc!=NULL) {
            char acTs[24];
            fprintf(pfTrc,"%s Read parameter string in length %d from command line\n",cstime(0,acTs),(int)strlen(*ppCmd));
         }
      } else {
         if (pcDpa!=NULL && *pcDpa) {
            siErr=pfF2S(pvGbl,pvF2S,pcDpa,ppCmd,&siSiz,NULL,0);
            if(siErr>0) {
               if (pfTrc!=NULL) {
                  char acTs[24];
                  fprintf(pfTrc,"%s Read parameter string in length %d from default parameter file '%s'\n",cstime(0,acTs),siErr,pcDpa);
               }
            } else {
               char* pcHlp=(char*)realloc(*ppCmd,1);
               if (pcHlp==NULL) {
                  if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,pcSysError(errno)); }
                  return(CLERTC_MEM);
               }
               pcHlp[0]=0x00;
               *ppCmd=pcHlp;
            }
         } else {
            char* pcHlp=(char*)realloc(*ppCmd,1);
            if (pcHlp==NULL) {
               if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,pcSysError(errno)); }
               return(CLERTC_MEM);
            }
            pcHlp[0]=0x00;
            *ppCmd=pcHlp;
         }
      }
   } else if (argv[1][l]=='.' || argv[1][l]=='(') {
      argv[1]=&argv[1][l];
      siErr=arry2str(argv+1,argc-1," ",1,ppCmd,&siSiz);
      if (siErr<0) {
         SAFE_FREE(*ppCmd);
         switch(siErr) {
         case -1: if (pfErr!=NULL) { fprintf(pfErr,"Illegal parameters passed to arry2str() (Bug)\n");                                 } return(CLERTC_FAT);
         case -2: if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,pcSysError(errno)); } return(CLERTC_MEM);
         default: if (pfErr!=NULL) { fprintf(pfErr,"An unknown error occurred while reading command line.\n");                         } return(CLERTC_FAT);
         }
      }
      if (pfTrc!=NULL) {
         char acTs[24];
         fprintf(pfTrc,"%s Read parameter string in length %d from command line\n",cstime(0,acTs),(int)strlen(*ppCmd));
      }
   } else if (argv[1][l]=='=') {
      int o=(argv[1][l+1]=='>')?l+2:l+1;
      if (argc!=2) {
         if (pfErr!=NULL) {
            fprintf(pfErr,"The expected parameter file name for '%s' is split into more than one parameter\n",pcFct);
            fprintf(pfErr,"The parameter file name must start with \" and end with \" to join anything into one parameter\n");
            fprintf(pfErr,"Syntax for command '%s' not valid\n",pcFct);
            fprintf(pfErr,"%s %s %s=\" parameter file name \"\n",pcDep,argv[0],pcFct);
            fprintf(pfErr,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
         }
         return(CLERTC_CMD);
      }
      *ppFil=dcpmapfil(argv[1]+o);
      if (*ppFil==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for command file (%s) failed\n",argv[1]+l+1); }
         return(CLERTC_MEM);
      }

      char acMsg[1024]="";
      siErr=pfF2S(pvGbl,pvF2S,*ppFil,ppCmd,&siSiz,acMsg,sizeof(acMsg));
      if(siErr>0) {
         if (pfTrc!=NULL) {
            char acTs[24];
            fprintf(pfTrc,"%s Read parameter string in length %d from file '%s'\n",cstime(0,acTs),siErr,*ppFil);
         }
      } else {
         if (pfErr!=NULL) { fprintf(pfErr,"Command file: %s\n",acMsg); }
         SAFE_FREE(*ppFil);
         SAFE_FREE(*ppCmd);
         return(CLERTC_SYS);
      }
   } else {
      if (pfErr!=NULL) {
         fprintf(pfErr,"No blank space ' ', equal sign '=', dot '.' or bracket '(' behind '%s'\n",pcFct);
         fprintf(pfErr,"Please use a blank space to define an argument list or an equal sign for a parameter file\n");
         fprintf(pfErr,"Syntax for command '%s' not valid\n",pcFct);
         fprintf(pfErr,"%s %s %cOWNER=oid%c %s \"... argument list ...\" %cMAXCC=%cmax%c%c-min%c%c %cQUIET%c\n",pcDep,argv[0],C_SBO,C_SBC,pcFct,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
         fprintf(pfErr,"%s %s %cOWNER=oid%c %s=\" parameter file name \" %cMAXCC=%cmax%c%c-min%c%c %cQUIET%c\n",pcDep,argv[0],C_SBO,C_SBC,pcFct,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
         fprintf(pfErr,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
      }
      SAFE_FREE(*ppFil);
      return(CLERTC_CMD);
   }
   return(CLERTC_OK);
}

/**********************************************************************/

static TsCnfHdl* psCnfOpn(
   FILE*                         pfErr,
   const int                     isCas,
   const char*                   pcPgm,
   const char*                   pcFil)
{
   FILE*                         pfFil;
   char                          acBuf[4046];
   char*                         pcHlp=NULL;
   char*                         pcKyw=NULL;
   char*                         pcVal=NULL;
   int                           siKyw,siVal;
   TsCnfEnt*                     psEnt=NULL;
   TsCnfHdl*                     psHdl=(TsCnfHdl*)calloc(1,sizeof(TsCnfHdl));
   if (psHdl==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration data handle failed\n"); }
      return(NULL);
   }
   psHdl->isCas=isCas;
   psHdl->psFst=NULL;
   psHdl->psLst=NULL;
   psHdl->pcPgm=pcPgm;
   if (pcFil==NULL || *pcFil==0) { return(psHdl); }
   psHdl->pcFil=malloc(strlen(pcFil)+1);
   if (psHdl->pcFil==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration file name (%s) failed\n",pcFil); }
      vdCnfCls(psHdl);
      return(NULL);
   }
   strcpy(psHdl->pcFil,pcFil);

   pfFil=fopen_hfq(psHdl->pcFil,"r");
   if (pfFil==NULL && (errno==2 || errno==49 || errno==129)) { return(psHdl); }
   if (pfFil==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Cannot open the configuration file (\"%s\",\"r\") for read operation (%d - %s)\n",psHdl->pcFil,errno,pcSysError(errno)); }
      vdCnfCls(psHdl);
      return(NULL);
   }

   while (fgets(acBuf,sizeof(acBuf)-1,pfFil)!=NULL) {
      pcHlp=strpbrk(acBuf,HSH_PBRK);//all variants of '#'
      if (pcHlp!=NULL) { *pcHlp=EOS; }
      size_t l=strlen(acBuf);
      if (l>0) {
         pcHlp=acBuf+l;
         while (isspace(*(pcHlp-1))) {
            pcHlp--; *pcHlp=EOS;
         }
         pcHlp=strchr(acBuf,'=');
         if (pcHlp!=NULL) {
            pcKyw=acBuf; pcVal=pcHlp+1; *pcHlp=EOS;
            while (isspace(*pcKyw)) { pcKyw++; }
            while (isspace(*pcVal)) { pcVal++; }
            for (char* p=pcKyw+strlen(pcKyw);p>pcKyw && isspace(*(p-1));p--) { *p=0x00; }
            for (char* p=pcVal+strlen(pcVal);p>pcVal && isspace(*(p-1));p--) { *p=0x00; }
            psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
            if (psEnt==NULL) {
               if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration data element failed\n"); }
               fclose_unchecked(pfFil);
               vdCnfCls(psHdl);
               return(NULL);
            }
            siKyw=strlen(pcKyw); siVal=strlen(pcVal);
            if (siKyw && siVal) {
               srprintf(&psEnt->pcKyw,&psEnt->szKyw,siKyw,"%s",pcKyw);
               srprintf(&psEnt->pcVal,&psEnt->szVal,siVal,"%s",pcVal);
               if (psEnt->pcKyw==NULL || psEnt->pcVal==NULL) {
                  if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration data (%s=%s) failed\n",pcKyw,pcVal); }
                  SAFE_FREE(psEnt->pcKyw);
                  SAFE_FREE(psEnt->pcVal);
                  free(psEnt);
                  fclose_unchecked(pfFil);
                  vdCnfCls(psHdl);
                  return(NULL);
               }
               if (psHdl->psLst!=NULL) {
                  psEnt->psBak=psHdl->psLst;
                  psEnt->psNxt=psHdl->psLst->psNxt;
                  if (psHdl->psLst->psNxt!=NULL) {
                     psHdl->psLst->psNxt->psBak=psEnt;
                  }
                  psHdl->psLst->psNxt=psEnt;
               } else {
                  psEnt->psNxt=NULL;
                  psEnt->psBak=NULL;
                  psHdl->psFst=psEnt;
               }
               psHdl->psLst=psEnt;
            } else {
               free(psEnt);
            }
         }
      }
   }

   fclose_unchecked(pfFil);
   return(psHdl);
}

static int siCnfSet(
   TsCnfHdl*                     psHdl,
   FILE*                         pfErr,
   const char*                   pcKyw,
   const char*                   pcVal,
   const int                     isOvr)
{
   TsCnfEnt*                     psEnt;
   int                           siKyw,siVal;
   for (psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt) {
      if (strxcmp(psHdl->isCas,psEnt->pcKyw,pcKyw,0,0,FALSE)==0) {
         if (pcVal==NULL || *pcVal==0x00) {
            if (psEnt->psNxt!=NULL) {
               psEnt->psNxt->psBak=psEnt->psBak;
            } else {
               psHdl->psLst=psEnt->psBak;
            }
            if (psEnt->psBak!=NULL) {
               psEnt->psBak->psNxt=psEnt->psNxt;
            } else {
               psHdl->psFst=psEnt->psNxt;
            }
            free(psEnt->pcKyw);
            free(psEnt->pcVal);
            free(psEnt);
         } else {
            if (isOvr || psEnt->pcVal[0]==0) {
               srprintf(&psEnt->pcVal,&psEnt->szVal,strlen(pcVal),"%s",pcVal);
               if (psEnt->pcVal==NULL) {
                  if (pfErr!=NULL) { fprintf(pfErr,"Allocation of memory for configuration value (%s=%s) failed\n",pcKyw,pcVal); }
                  return(-1);
               }
            }else {
               if (pfErr!=NULL) { fprintf(pfErr,"Configuration value (%s) for keyword '%s' already exists\n",psEnt->pcVal,psEnt->pcKyw); }
               return(1);
            }
         }
         psHdl->isChg=TRUE;
         return(0);
      }
   }

   psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
   if (psEnt==NULL) {
      if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration data element failed\n"); }
      return(-1);
   }
   siKyw=strlen(pcKyw); siVal=strlen(pcVal);
   if (siKyw && siVal) {
      srprintf(&psEnt->pcKyw,&psEnt->szKyw,siKyw,"%s",pcKyw);
      srprintf(&psEnt->pcVal,&psEnt->szVal,siVal,"%s",pcVal);
      if (psEnt->pcKyw==NULL || psEnt->pcVal==NULL) {
         if (pfErr!=NULL) { fprintf(pfErr,"Memory allocation for configuration data (%s=%s) failed\n",pcKyw,pcVal); }
         free(psEnt);
         return(-1);
      }
      if (psHdl->psLst!=NULL) {
         psEnt->psBak=psHdl->psLst;
         psEnt->psNxt=psHdl->psLst->psNxt;
         if (psHdl->psLst->psNxt!=NULL) {
            psHdl->psLst->psNxt->psBak=psEnt;
         }
         psHdl->psLst->psNxt=psEnt;
      } else {
         psEnt->psNxt=NULL;
         psEnt->psBak=NULL;
         psHdl->psFst=psEnt;
      }
      psHdl->psLst=psEnt;
   } else {
      if (pfErr!=NULL) { fprintf(pfErr,"Configuration keyword(%s) and/or value(%s) is empty\n", pcKyw, pcVal); }
      free(psEnt);
      return (-1);
   }
   psHdl->isChg=TRUE;
   return(0);
}

static const char* pcCnfGet(
   TsCnfHdl*                     psHdl,
   const char*                   pcKyw)
{
   TsCnfEnt*                     psEnt;
   for (psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt) {
      if (strxcmp(psHdl->isCas,psEnt->pcKyw,pcKyw,0,0,FALSE)==0) { return(psEnt->pcVal); }
   }
   return(NULL);
}

static int siCnfPutEnv(
   TsCnfHdl*                     psHdl,
   const char*                   pcOwn,
   const char*                   pcPgm)
{
   int                           i,j;
   TsCnfEnt*                     psEnt;
   for (i=j=0,psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt,i++) {
      if (strstr(psEnt->pcKyw,pcOwn)!=NULL &&
          strstr(psEnt->pcKyw,pcPgm)!=NULL &&
          strstr(psEnt->pcKyw,".envar.")!=NULL) {
         const char* pcKyw=strstr(psEnt->pcKyw,".envar.")+7;
         if (SETENV(pcKyw,psEnt->pcVal)==0) {
            const char* pcEnv=GETENV(pcKyw);
            if (pcEnv!=NULL) {
               if (strcmp(pcEnv,psEnt->pcVal)==0) {
                  j++;
               }
            }
         }
      }
   }
   return(j);
}

static int siCnfPrnEnv(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcPre,
   const char*                   pcOwn,
   const char*                   pcPgm)
{
   int                           i;
   TsCnfEnt*                     psEnt;
   const char*                   pcAdd;
   for (i=0,psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt,i++) {
      if (strstr(psEnt->pcKyw,pcOwn)!=NULL &&
          strstr(psEnt->pcKyw,pcPgm)!=NULL &&
          strstr(psEnt->pcKyw,".envar.")!=NULL) {
         const char* pcKyw=strstr(psEnt->pcKyw,".envar.")+7;
         const char* pcEnv=GETENV(pcKyw);
         if (pcEnv!=NULL) {
            if (strcmp(pcEnv,psEnt->pcVal)==0) {
               pcAdd="was verified";
            } else { pcAdd="not verified"; }
         } else { pcAdd="not verified"; }
         if (pcPre!=NULL && *pcPre) {
            fprintf(pfOut,"%s %s=%s %c %s\n",pcPre,pcKyw,psEnt->pcVal,C_HSH,pcAdd);
         } else {
            fprintf(pfOut,"%s=%s %c %s\n",pcKyw,psEnt->pcVal,C_HSH,pcAdd);
         }
      }
   }
   return(i);
}

static int siCnfPrn(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcPre)
{
   int                           i;
   TsCnfEnt*                     psEnt;
   for (i=0,psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt,i++) {
      if (pcPre!=NULL && *pcPre) {
         fprintf(pfOut,"%s %s=%s\n",pcPre,psEnt->pcKyw,psEnt->pcVal);
      } else {
         fprintf(pfOut,"%s=%s\n",psEnt->pcKyw,psEnt->pcVal);
      }
   }
   return(i);
}

static int siCnfClr(
   TsCnfHdl*                     psHdl)
{
   int                           i=0;
   TsCnfEnt*                     psEnt;
   TsCnfEnt*                     psHlp;
   psEnt=psHdl->psFst;
   while(psEnt!=NULL) {
      psHlp=psEnt->psNxt;
      free(psEnt->pcKyw);
      free(psEnt->pcVal);
      free(psEnt);
      psEnt=psHlp;
      i++;
   }
   psHdl->psFst=NULL;
   psHdl->psLst=NULL;
   psHdl->isClr=TRUE;
   return(i);
}

static void vdCnfCls(
   TsCnfHdl*                     psHdl)
{
   TsCnfEnt*                     psEnt;
   TsCnfEnt*                     psHlp;
   FILE*                         pfFil=NULL;
   if (psHdl!=NULL) {
      if (psHdl->pcFil!=NULL) {
         if ((psHdl->isChg || psHdl->isClr) && psHdl->pcFil[0]) {
            pfFil=fopen_hfq(psHdl->pcFil,"w");
         }
         free(psHdl->pcFil);
      }
      if (pfFil!=NULL && psHdl->pcPgm!=NULL) {
         fprintf(pfFil,"%c Configuration file for program '%s'\n",C_HSH,psHdl->pcPgm);
      }
      for (psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psHlp) {
         if (pfFil!=NULL && psHdl->isClr==FALSE){
            fprintf(pfFil,"%s=%s\n",psEnt->pcKyw,psEnt->pcVal);
         }
         psHlp=psEnt->psNxt;
         free(psEnt->pcKyw);
         free(psEnt->pcVal);
         free(psEnt);
      }
      if (pfFil!=NULL) {
         fclose_unchecked(pfFil);
      }
      free(psHdl);
   }
}

extern int siCleParseString(
   const int                     uiErr,
   char*                         pcErr,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     siMkl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const int                     isOvl,
   const char*                   pcStr,
   const TsClpArgument*          psTab,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   int*                          piMod,
   void*                         pvDat,
   void*                         pvGbl,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf,
   void**                        ppClp)
{
   int                           siErr=0;
   void*                         pvHdl;
   FILE*                         pfTmp;
   TsClpError                    stErr;

   pfTmp=fopen_tmp();

   pvHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcBld,pcCmd,pcMan,pcHlp,isOvl,
                   psTab,pvDat,pfTmp,pfTmp,NULL,NULL,NULL,NULL,
                   pcDep,pcOpt,pcEnt,&stErr,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
   if (pvHdl==NULL) {
      snprintf(pcErr,uiErr,"CTX-MESSAGE : Open of string parser for command '%s' failed\n",pcCmd);
      if (pfTmp!=NULL) {size_t l=strlen(pcErr); errno=0; rewind(pfTmp); size_t r=(errno)?0:fread(pcErr+l,1,uiErr-(l+1),pfTmp); fclose_tmp(pfTmp); pcErr[l+r]=0x00;}
      return -1;
   }
   siErr=siClpParseCmd(pvHdl,NULL,pcStr,TRUE,TRUE,(int*)piMod,NULL);
   if (siErr<0) {
      snprintf(pcErr,uiErr,
            "CTX-MESSAGE : Parsing of string for command '%s' failed\n"
            "CLP-STRING  : %s\n"
            "CLP-ERROR   : %d - %s\n"
            "CLP-MESSAGE : %s\n"
            "CLP-SOURCE  : %s (ROW: %d COL: %d)\n",
            pcCmd,pcStr,siErr,pcClpError(siErr),
            *stErr.ppMsg,*stErr.ppSrc,*stErr.piRow,*stErr.piCol);
      if (pfTmp!=NULL) {size_t l=strlen(pcErr); errno=0; rewind(pfTmp); size_t r=(errno)?0:fread(pcErr+l,1,uiErr-(l+1),pfTmp); fclose_tmp(pfTmp); pcErr[l+r]=0x00;}
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      return siErr;
   }
   if (pfTmp!=NULL) { fclose_tmp(pfTmp); }
   if(ppClp==NULL){
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   }else{
      vdClpClose(pvHdl,CLPCLS_MTD_KEP);
      *ppClp=pvHdl;
   }
   return 0;
}

/**********************************************************************/
