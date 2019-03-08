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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#ifndef __WIN__
#include <unistd.h>
#endif

/* Include eigener Bibliotheken  **************************************/

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
 *         Add new built in function to print out error code messages
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
 * 1.2.74: Support no run reason code (siNrn)
 */
#define CLE_VSN_STR       "1.2.74"
#define CLE_VSN_MAJOR      1
#define CLE_VSN_MINOR        2
#define CLE_VSN_REVISION       74

/* Definition der Konstanten ******************************************/

#define CLEINI_PROSIZ            1024

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

/* Deklaration der internen Funktionen ********************************/

static int siClePropertyInit(
   tpfIni                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   tpfMsg                        pfMsg,
   void*                         pvF2S,
   tpfF2S                        pfF2S);

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
   tpfIni                        pfIni,
   void*                         pvClp,
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   tpfMsg                        pfMsg,
   void*                         pvF2S,
   tpfF2S                        pfF2S);

static int siCleCommandInit(
   tpfIni                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   tpfMsg                        pfMsg,
   void*                         pvF2S,
   tpfF2S                        pfF2S);

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
   void*                         pvHdl,
   char*                         pcBuf);

static void vdCleManProgram(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcHlp,
   const char*                   pcMan,
   const char*                   pcDep,
   const char*                   pcSep,
   const int                     isMan,
   const int                     isNbr);

static void vdCleManFunction(
   FILE*                         pfOut,
   const char*                   pcLev,
   const char*                   pcNum,
   const char*                   pcFct,
   const char*                   pcHlp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcSyn,
   const char*                   pcMan,
   const int                     isMan,
   const int                     isNbr);

static void vdPrnStaticSyntax(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
   const char*                   pcDep,
   const char*                   pcSep);

static void vdPrnStaticHelp(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
   const int                     isCas,
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
   const char*                   pcCmd,
   const int                     siInd,
   const int                     isMan,
   const int                     isNbr);

static void vdPrnProperties(
   void*                         pvHdl,
   const char*                   pcPat,
   const int                     isSet,
   const int                     siDep);

static int siCleGetProperties(
   void*                         pvHdl,
   FILE*                         pfErr,
   TsCnfHdl*                     psCnf,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   char**                        ppFil,
   char**                        ppPro,
   int*                          piFlg,
   void*                         pvF2S,
   tpfF2S                        pfF2S);

static int siCleGetCommand(
   void*                         pvHdl,
   FILE*                         pfErr,
   const char*                   pcDep,
   const char*                   pcFct,
   int                           argc,
   char*                         argv[],
   char**                        ppFil,
   char**                        ppCmd,
   void*                         pvF2S,
   tpfF2S                        pfF2S,
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
   TsCnfHdl*                     psHdl,
   FILE*                         pfErr,
   const char*                   pcPre);

static void vdCnfCls(
   TsCnfHdl*                     psHdl);

/* Implementierung der externen Funktionen ****************************/

extern const char* pcCleVersion(const int l, const int s, char* b)
{
   snprintc(b,s,"%2.2d FLAM-CLE VERSION: %s-%u BUILD: %s %s %s\n",l,CLE_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__);
   return(pcClpVersion(l+1,s,b));
}

extern const char* pcCleAbout(const int l, const int s, char* b)
{
   snprintc(b,s,
   "%2.2d Frankenstein Limes Command Line Execution (FLAM-CLE)\n"
   "   Version: %s-%u Build: %s %s %s\n"
   "   Copyright (C) limes datentechnik (R) gmbh\n"
   "   This library is open source from the FLAM(R) project: http://www.flam.de\n"
   "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
   "This library uses the internal library below:\n"
   ,l,CLE_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__);
   return(pcClpAbout(l+1,s,b));
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

#undef  ERROR
#define ERROR(x,b) do { \
   int r = siCleEndExecution((x),psCnf,pfTrh,pfDoc,pfPro,ppArg,pvHdl,(b));\
   if (r) {\
      if (pcBld!=NULL && *pcBld) {\
         if (pfErr!=NULL) efprintf(pfErr,"Program '%s' (Build: %s (%s %s)) ends with completion code %d (%s)\n",pcProgram,pcBld,__DATE__,__TIME__,r,pcMapCleRtc(r));\
      } else {\
         if (pfErr!=NULL) efprintf(pfErr,"Program '%s' (Build: %s %s) ends with completion code %d (%s)\n",pcProgram,__DATE__,__TIME__,r,pcMapCleRtc(r));\
      }\
   } else {\
      if (pcBld!=NULL && *pcBld) {\
         if (pfOut!=NULL) efprintf(pfOut,"Program '%s' (Build: %s (%s %s)) run successfully\n",pcProgram,pcBld,__DATE__,__TIME__);\
      } else {\
         if (pfOut!=NULL) efprintf(pfOut,"Program '%s' (Build: %s %s) run successfully\n",pcProgram,__DATE__,__TIME__);\
      }\
   }\
   SAFE_FREE(pcHom); \
   SAFE_FREE(pcPgm); \
   SAFE_FREE(pcPgu); \
   SAFE_FREE(pcPgl); \
   SAFE_FREE(pcCnf); \
   SAFE_FREE(pcOwn); \
   SAFE_FREE(pcFil); \
   return(r); \
} while(FALSE)

extern int siCleExecute(
   const TsCleCommand*           psTab,
   int                           argc,
   char*                         argv[],
   const char*                   pcOwner,
   const char*                   pcProgram,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     isEnv,
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
   const char*                   pcMan,
   const char*                   pcCov,
   const char*                   pcGls,
   const char*                   pcFin,
   const char*                   pcScc,
   const char*                   pcDef,
   tpfMsg                        pfMsg,
   const char*                   pcApx,
   const TsCleAppendix*          psApx,
   void*                         pvF2S,
   tpfF2S                        pfF2S,
   const char*                   pcDpa,
   const int                     siNrn)
{
   int                           i,j,l,s,siErr,siDep,siCnt,isSet=0;
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
   FILE*                         pfTmp=NULL;
   void*                         pvHdl=NULL;
   FILE*                         pfDoc=NULL;
   FILE*                         pfPro=NULL;
   FILE*                         pfStd=(pfOut!=NULL)?pfOut:stderr;
   FILE*                         pfErr=(pfOut!=NULL)?pfOut:stderr;
   char**                        ppArg=NULL;
   size_t                        szFil=0;
   char*                         pcFil=NULL;
   char*                         pcHom=NULL;
   int                           isWrn=FALSE;
   int                           siScc=0;
   int                           isEnvOwn;
   const char*                   m;

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

   if (psTab==NULL || argc==0     || argv==NULL  ||  pcOwner==NULL ||  pcProgram==NULL ||  pcHlp==NULL ||
       pcDep==NULL || pcOpt==NULL || pcEnt==NULL || *pcOwner==0x00 || *pcProgram==0x00 || *pcHlp==0   ) {
      if (pfErr!=NULL) fprintf(pfErr,"CLE call parameter incorrect (NULL pointer or empty strings)\n");
      ERROR(CLERTC_FAT,NULL);
   }

   if (isEnv) {
      siErr = readEnvars(NULL,pfOut,pfErr,NULL);
      if (siErr<0) return(-1*siErr);
   }

   pcHom=dhomedir(TRUE);
   if (pcHom==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Determination of home directory failed\n");
      ERROR(CLERTC_MEM,NULL);
   }

   pcPgm=dmapstr(pcProgram,0);
   if (pcPgm==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Conversion of program name failed (not enough memory)\n");
      ERROR(CLERTC_MEM,NULL);
   }

   pcPgu=dmapstr(pcProgram,1);
   if (pcPgu==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Conversion of program name to upper case failed (not enough memory)\n");
      ERROR(CLERTC_MEM,NULL);
   }

   pcPgl=dmapstr(pcProgram,2);
   if (pcPgl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Conversion of program name to lower case failed (not enough memory)\n");
      ERROR(CLERTC_MEM,NULL);
   }

   srprintf(&pcCnf,&szCnf,strlen(pcPgm),"%s_DEFAULT_OWNER_ID",pcPgu);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for %s_DEFAULT_OWNER_ID failed\n",pcPgu);
      ERROR(CLERTC_MEM,NULL);
   }
   m=GETENV(pcCnf);
   if (m!=NULL && *m) srprintf(&pcOwn,&szOwn,strlen(m),"%s",m); else srprintf(&pcOwn,&szOwn,strlen(pcOwner),"%s",pcOwner);
   if (pcOwn==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Determination of owner id failed\n");
      ERROR(CLERTC_MEM,NULL);
   }

   srprintf(&pcCnf,&szCnf,strlen(pcPgm),"%s_CONFIG_FILE",pcPgu);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for %s_CONFIG_FILE  failed\n",pcPgu);
      ERROR(CLERTC_MEM,NULL);
   }
   m=GETENV(pcCnf);
   if (m==NULL) {
#ifdef __ZOS__
      {
         char acUsr[16];
         srprintf(&pcFil,&szFil,strlen(pcPgu)+8,"'%s.",userid(sizeof(acUsr),acUsr));
         if (pcFil==NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            ERROR(CLERTC_MEM,NULL);
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
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            ERROR(CLERTC_MEM,NULL);
         }
         pfTmp=fopen_nowarn(pcFil,"r");
         if (pfTmp==NULL) {
            srprintf(&pcFil,&szFil,strlen(pcHom)+strlen(pcPgl),"%s.%s.config",pcHom,pcPgl);
            if (pcFil==NULL) {
               if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
               ERROR(CLERTC_MEM,NULL);
            }
            if (pfOut!=NULL) fprintf(pfOut,"Use default configuration file (%s) in home directory\n",pcFil);
         } else {
            fclose(pfTmp);
            if (pfOut!=NULL) fprintf(pfOut,"Use existing configuration file (%s) in working directory\n",pcFil);
         }
      } else {
         srprintf(&pcFil,&szFil,strlen(pcPgl),".%s.config",pcPgl);
         if (pcFil==NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            ERROR(CLERTC_MEM,NULL);
         }
         if (pfOut!=NULL) fprintf(pfOut,"Use default configuration file (%s) in working directory\n",pcFil);
      }
#endif
   } else {
      srprintf(&pcFil,&szFil,strlen(m),"%s",m);
      if (pcFil==NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration file name (%s) from environment variable failed\n",pcCnf);
         ERROR(CLERTC_MEM,NULL);
      }
      if (pfOut!=NULL) fprintf(pfOut,"Using configuration file (%s) defined by environment variable (%s)\n",pcFil,pcCnf);
   }

   psCnf=psCnfOpn(pfErr,isCas,pcPgm,pcFil);
   if (psCnf==NULL) ERROR(CLERTC_CFG,NULL);
   SAFE_FREE(pcFil);
   szFil=0;

   srprintf(&pcCnf,&szCnf,strlen(pcPgl),"%s.owner.id",pcPgl);
   if (pcCnf==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration element owner id failed\n");
      ERROR(CLERTC_MEM,NULL);
   }
   m=pcCnfGet(psCnf,pcCnf);
   if (m!=NULL && *m) {
      srprintf(&pcOwn,&szOwn,strlen(m),"%s",m);
      if (pcOwn==NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for owner name (%s) failed\n",m);
         ERROR(CLERTC_MEM,NULL);
      }
   }

   i=siCnfPutEnv(psCnf,pcOwn,pcPgm);
   if (pfOut!=NULL) {
      if (i) {
         if (i==1) {
            fprintf(pfOut,"Load successfully %d environment variable using the configuration file.\n",i);
         } else {
            fprintf(pfOut,"Load successfully %d environment variables using the configuration file.\n",i);
         }
      }
   }

   m=GETENV("OWNERID");
   if (m==NULL || *m==0x00) {
      if (SETENV("OWNERID",pcOwn)) {
         if (pfErr!=NULL) fprintf(pfErr,"Put variable (%s=%s) to environment failed (%d - %s)\n","OWNERID",pcOwn,errno,strerror(errno));
      } else {
         if (strcmp(pcOwn,GETENV("OWNERID"))) {
            if (pfErr!=NULL) fprintf(pfErr,"Put variable (%s=%s) to environment failed (strcmp(%s,GETENV(%s)))\n","OWNERID",pcOwn,pcOwn,"OWNERID");
         } else {
#ifdef __DEBUG__
            if (pfOut!=NULL) fprintf(pfOut,"Put variable (%s=%s) to environment was successful\n","OWNERID",pcOwn);
#endif
         }
      }
      isEnvOwn=TRUE;
   } else {
#ifdef __DEBUG__
      if (pfOut!=NULL) fprintf(pfOut,"Environment variable OWNERID already defined (%s)\n",pcOwn);
#endif
      isEnvOwn=FALSE;
   }

   if (pfOut!=NULL) {
      fprintf( pfOut,"Complete load of environment ...\n");
#ifdef __EBCDIC__
      init_char(gsDiaChr.exc);
#endif
      efprintf(pfOut,"Initialize dia-critical character (!$#@[\\]^`{|}~) conversion (%s)\n",mapccsid(localccsid()));
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
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration element trace failed\n");
      ERROR(CLERTC_MEM,NULL);
   }
   m=pcCnfGet(psCnf,pcCnf);
   if (m!=NULL && strxcmp(isCas,m,"ON",0,0,FALSE)==0) {
      srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgl),"%s.%s.trace.file",pcOwn,pcPgl);
      if (pcCnf==NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration element trace file failed\n");
         ERROR(CLERTC_MEM,NULL);
      }
      m=pcCnfGet(psCnf,pcCnf);
      if (m!=NULL && *m) {
         pcFil=dcpmapfil(m);
         if (pcFil!=NULL) {
            pfTrh=fopen(pcFil,filemode("w"));
            if (pfTrh==NULL) {
               if (pfErr!=NULL) fprintf(pfErr,"Open of trace file (\"%s\",\"%s\") failed\n",pcFil,filemode("w"));
            } else pfTrc=pfTrh;
            SAFE_FREE(pcFil);
         }
      }
   } else pfTrc=NULL;

   for (i=0; psTab[i].pcKyw!=NULL; i++) {
      if (psTab[i].psTab==NULL || psTab[i].pvClp==NULL || psTab[i].pvPar==NULL ||
          psTab[i].pfIni==NULL || psTab[i].pfMap==NULL || psTab[i].pfRun==NULL || psTab[i].pfFin==NULL ||
          psTab[i].pcMan==NULL || psTab[i].pcHlp==NULL || *psTab[i].pcKyw==0 || *psTab[i].pcMan==0 || *psTab[i].pcHlp==0) {
         if (pfErr!=NULL) fprintf(pfErr,"Row %d of command table not initialized properly\n",i);
         ERROR(CLERTC_TAB,NULL);
      }
   }
   if (i==0) {
      if (pfErr!=NULL) fprintf(pfErr,"Command table is empty\n");
      ERROR(CLERTC_TAB,NULL);
   }

   if (argc<2) {
      if (pcDef!=NULL && *pcDef) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef);
            ERROR(CLERTC_MEM,NULL);
         }
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc=2; argv=ppArg;
      } else {
         if (pfErr!=NULL) {
            fprintf(pfErr,"Command or built-in function required\n");
            vdPrnStaticSyntax(pfErr,psTab,argv[0],pcDep,pcOpt);
         }
         ERROR(CLERTC_CMD,NULL);
      }
   }

   if (argv[1][0]=='-') argv[1]++;
   if (argv[1][0]=='-') argv[1]++;

EVALUATE:
   if (strxcmp(isCas,argv[1],"LICENSE",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"License of program '%s':\n",pcPgm);
         if (pcLic==NULL) {
            fprintf(pfOut,"No license information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcLic);
         }
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'LICENSE' not valid\n");
      fprintf(pfErr,"%s %s LICENSE\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"VERSION",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Version for program '%s':\n",pcPgm);
         if (pcVsn==NULL) {
            fprintf(pfOut,"No version information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcVsn);
         }
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'VERSION' not valid\n");
      fprintf(pfErr,"%s %s VERSION\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"ABOUT",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"About program '%s':\n",pcPgm);
         if (pcAbo==NULL) {
            fprintf(pfOut,"No about information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcAbo);
         }
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'ABOUT' not valid\n");
      fprintf(pfErr,"%s %s ABOUT\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"LEXEM",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         siErr=siCleSimpleInit(pfOut,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
         if (siErr) ERROR(siErr,NULL);
         fprintf(pfOut,"Lexemes (regular expressions) for argument list or parameter file:\n");
         siErr=siClpLexem(pvHdl,pfOut);
         if (siErr<0) ERROR(CLERTC_SYN,NULL); else ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'LEXEM' not valid\n");
      fprintf(pfErr,"%s %s LEXEM\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"GRAMMAR",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         siErr=siCleSimpleInit(pfOut,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
         if (siErr) ERROR(siErr,NULL);
         fprintf(pfOut,"Grammar for argument list, parameter file or property file\n");
         siErr=siClpGrammar(pvHdl,pfOut);
         if (siErr<0) ERROR(CLERTC_SYN,NULL); else ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'GRAMMAR' not valid\n");
      fprintf(pfErr,"%s %s GRAMMAR\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"ERRORS",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"\n");
         fprintf(pfOut,"Return/condition/exit codes of the executable\n");
         fprintf(pfOut,"---------------------------------------------\n\n");
         fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_APPENDIX_RETURNCODES,1);
         if (pcScc!=NULL && *pcScc) fprintm(pfOut,pcOwn,pcPgm,pcScc,1);
         if (pfMsg!=NULL) {
            fprintf(pfOut,"Reason codes of the different commands\n");
            fprintf(pfOut,"--------------------------------------\n\n");
            fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_APPENDIX_REASONCODES,1);
            for (i=1,m=pfMsg(i);m!=NULL;i++,m=pfMsg(i)) {
               if (*m) fprintf(pfOut," * %d - %s\n",i,m);
            }
         }
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'ERRORS' not valid\n");
      fprintf(pfErr,"%s %s ERRORS\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"SYNTAX",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Syntax for program '%s':\n",pcPgm);
         vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
         ERROR(CLERTC_OK,NULL);
      } else if (argc>=3) {
         if (argc==3) {
            siDep=1;
         } else if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"ALL",0,0,FALSE)==0) {
               siDep=10;
            } else if (strxcmp(isCas,argv[3],"DEPTH1",0,0,FALSE)==0) {
               siDep=1;
            } else if (strxcmp(isCas,argv[3],"DEPTH2",0,0,FALSE)==0) {
               siDep=2;
            } else if (strxcmp(isCas,argv[3],"DEPTH3",0,0,FALSE)==0) {
               siDep=3;
            } else if (strxcmp(isCas,argv[3],"DEPTH4",0,0,FALSE)==0) {
               siDep=4;
            } else if (strxcmp(isCas,argv[3],"DEPTH5",0,0,FALSE)==0) {
               siDep=5;
            } else if (strxcmp(isCas,argv[3],"DEPTH6",0,0,FALSE)==0) {
               siDep=6;
            } else if (strxcmp(isCas,argv[3],"DEPTH7",0,0,FALSE)==0) {
               siDep=7;
            } else if (strxcmp(isCas,argv[3],"DEPTH8",0,0,FALSE)==0) {
               siDep=8;
            } else if (strxcmp(isCas,argv[3],"DEPTH9",0,0,FALSE)==0) {
               siDep=9;
            } else {
               fprintf(pfErr,"Syntax for built-in function 'SYNTAX' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfErr,"%s %s SYNTAX %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
                  }
               }
               ERROR(CLERTC_CMD,NULL);
            }
         } else {
            fprintf(pfErr,"Syntax for built-in function 'SYNTAX' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfErr,"%s %s SYNTAX %s",pcDep,argv[0],psTab[i].pcKyw);
                  efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
               }
            }
            ERROR(CLERTC_CMD,NULL);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Syntax for command '%s':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Syntax for argument '%s':\n",argv[2]);
               }
               vdPrnCommandSyntax(pvHdl,pfOut,argv[0],argv[2],pcDep,siDep);
               ERROR(CLERTC_OK,NULL);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char acPat[strlen(pcDef)+strlen(argv[2]+2)];
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  sprintf(acPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Syntax for argument '%s':\n",acPat);
                  vdPrnCommandSyntax(pvHdl,pfOut,argv[0],acPat,pcDep,siDep);
                  ERROR(CLERTC_OK,NULL);
               }
            }
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'SYNTAX' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s SYNTAX %s\n",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
         }
      }
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"HELP",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Help for program '%s':\n",pcPgm);
         vdPrnStaticHelp(pfOut,psTab,argv[0],isCas,pcDep);
         ERROR(CLERTC_OK,NULL);
      } else if (argc>=3) {
         if (argc==3) {
            if (strxcmp(isCas,argv[2],"MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--MAN",0,0,FALSE)==0) {
               fprintf(pfOut,"Help for program '%s':\n",pcPgm);
               fprintm(pfOut,pcOwn,pcPgm,pcMan,1);
               ERROR(CLERTC_OK,NULL);
            } else siDep=1;
         } else if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"MAN",0,0,FALSE)==0) {
               siDep=0;
            } else if (strxcmp(isCas,argv[3],"DEPTH1",0,0,FALSE)==0) {
               siDep=1;
            } else if (strxcmp(isCas,argv[3],"DEPTH2",0,0,FALSE)==0) {
               siDep=2;
            } else if (strxcmp(isCas,argv[3],"DEPTH3",0,0,FALSE)==0) {
               siDep=3;
            } else if (strxcmp(isCas,argv[3],"DEPTH4",0,0,FALSE)==0) {
               siDep=4;
            } else if (strxcmp(isCas,argv[3],"DEPTH5",0,0,FALSE)==0) {
               siDep=5;
            } else if (strxcmp(isCas,argv[3],"DEPTH6",0,0,FALSE)==0) {
               siDep=6;
            } else if (strxcmp(isCas,argv[3],"DEPTH7",0,0,FALSE)==0) {
               siDep=7;
            } else if (strxcmp(isCas,argv[3],"DEPTH8",0,0,FALSE)==0) {
               siDep=8;
            } else if (strxcmp(isCas,argv[3],"DEPTH9",0,0,FALSE)==0) {
               siDep=9;
            } else if (strxcmp(isCas,argv[3],"ALL",0,0,FALSE)==0) {
               siDep=10;
            } else {
               fprintf(pfErr,"Syntax for built-in function 'HELP' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfErr,"%s %s HELP %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
                  }
               }
               ERROR(CLERTC_CMD,NULL);
            }
         } else {
            fprintf(pfErr,"Syntax for built-in function 'HELP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfErr,"%s %s HELP %s",pcDep,argv[0],psTab[i].pcKyw);
                  efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
               }
            }
            ERROR(CLERTC_CMD,NULL);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Help for command '%s': %s\n",argv[2],psTab[i].pcHlp);
               } else {
                  fprintf(pfOut,"Help for argument '%s': %s\n",argv[2],pcClpInfo(pvHdl,argv[2]));
               }
               vdPrnCommandHelp(pvHdl,argv[2],siDep,siDep>9,TRUE);
               if (siDep==0) {
                  fprintf(pfOut,"ARGUMENTS\n");
                  fprintf(pfOut,"---------\n");
                  vdPrnCommandHelp(pvHdl,argv[2],1,TRUE,FALSE);
               }
               ERROR(CLERTC_OK,NULL);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char acPat[strlen(psTab[i].pcKyw)+strlen(argv[2]+2)];
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  sprintf(acPat,"%s.%s",psTab[i].pcKyw,argv[2]);
                  fprintf(pfOut,"Help for argument '%s': %s\n",acPat,pcClpInfo(pvHdl,acPat));
                  vdPrnCommandHelp(pvHdl,acPat,siDep,siDep>9,TRUE);
                  if (siDep==0) {
                     fprintf(pfOut,"ARGUMENTS\n");
                     fprintf(pfOut,"---------\n");
                     vdPrnCommandHelp(pvHdl,acPat,1,TRUE,FALSE);
                  }
                  ERROR(CLERTC_OK,NULL);
               }
            }
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'HELP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s HELP %s\n",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
         }
      }
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"MANPAGE",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         if (pfOut!=NULL) {
            fprintf(pfOut,"Manual page for program '%s':\n\n",pcPgm);
            vdCleManProgram(pfOut,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,FALSE,TRUE);
         }
         ERROR(CLERTC_OK,NULL);
      } else if (argc==3) {
         const char*                pcSgn;
         const char*                pcCmd;
         int                        isMan=FALSE;
         int                        isAll=FALSE;
         pcSgn=strchr(argv[2],'=');
         if (pcSgn!=NULL) {
            *((char*)pcSgn)=EOS; pcSgn++; pcCmd=argv[2]; isMan=TRUE;
            pcFil=dcpmapfil(pcSgn);
            if (pcFil==NULL) {
               fprintf(pfErr,"Allocation of memory for file name (%s) failed\n",pcSgn);
               ERROR(CLERTC_MEM,NULL);
            }
            szFil=strlen(pcFil)+1;
            pfDoc=fopen(pcFil,filemode("w"));
            if (pfDoc==NULL) {
               fprintf(pfErr,"Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",pcFil,filemode("w"),errno,strerror(errno));
               ERROR(CLERTC_SYS,NULL);
            }
         } else {
            pcCmd=argv[2];
            isMan=FALSE;
            pfDoc=pfOut;
            srprintf(&pcFil,&szFil,0,":STDOUT:");
            if (pcFil==NULL) {
               fprintf(pfErr,"Allocation of memory for file name (:STDOUT:) failed\n");
               ERROR(CLERTC_MEM,NULL);
            }
         }
         if (strxcmp(isCas,pcCmd,"ALL",0,0,FALSE)==0 || strxcmp(isCas,pcCmd,"-ALL",0,0,FALSE)==0 || strxcmp(isCas,pcCmd,"--ALL",0,0,FALSE)==0) {
            isAll=TRUE;
            if (isMan==FALSE) fprintf(pfOut,"Manual page for program '%s':\n\n",pcPgm);
            vdCleManProgram(pfDoc,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for program '%s' successfully written to file (%s)\n",pcPgm,pcFil);
         }
         if (strxcmp(isCas,pcCmd,"SYNTAX",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SYNTAX':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.1" ,"SYNTAX"  ,HLP_CLE_SYNTAX  ,pcOwn,pcPgm,SYN_CLE_SYNTAX,MAN_CLE_SYNTAX,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SYNTAX' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"HELP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'HELP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.2" ,"HELP"    ,HLP_CLE_HELP    ,pcOwn,pcPgm,SYN_CLE_HELP,MAN_CLE_HELP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'HELP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"MANPAGE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'MANPAGE':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.3" ,"MANPAGE" ,HLP_CLE_MANPAGE ,pcOwn,pcPgm,SYN_CLE_MANPAGE,MAN_CLE_MANPAGE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'MANPAGE' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GENDOCU",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GENDOCU':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.4" ,"GENDOCU" ,HLP_CLE_GENDOCU ,pcOwn,pcPgm,SYN_CLE_GENDOCU,MAN_CLE_GENDOCU,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GENDOCU' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GENPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GENPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.5" ,"GENPROP" ,HLP_CLE_GENPROP ,pcOwn,pcPgm,SYN_CLE_GENPROP,MAN_CLE_GENPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GENPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"SETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.6" ,"SETPROP" ,HLP_CLE_SETPROP ,pcOwn,pcPgm,SYN_CLE_SETPROP,MAN_CLE_SETPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"CHGPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'CHGPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.7" ,"CHGPROP" ,HLP_CLE_CHGPROP ,pcOwn,pcPgm,SYN_CLE_CHGPROP,MAN_CLE_CHGPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'CHGPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"DELPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'DELPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.8" ,"DELPROP" ,HLP_CLE_DELPROP ,pcOwn,pcPgm,SYN_CLE_DELPROP,MAN_CLE_DELPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'DELPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.9" ,"GETPROP" ,HLP_CLE_GETPROP ,pcOwn,pcPgm,SYN_CLE_GETPROP,MAN_CLE_GETPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"SETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETOWNER':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.10" ,"SETOWNER",HLP_CLE_SETOWNER,pcOwn,pcPgm,SYN_CLE_SETOWNER,MAN_CLE_SETOWNER,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETOWNER' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETOWNER':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.11","GETOWNER",HLP_CLE_GETOWNER,pcOwn,pcPgm,SYN_CLE_GETOWNER,MAN_CLE_GETOWNER,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETOWNER' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"SETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETENV':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.12" ,"SETENV",HLP_CLE_SETENV,pcOwn,pcPgm,SYN_CLE_SETENV,MAN_CLE_SETENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETENV' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETENV':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.13" ,"GETENV",HLP_CLE_GETENV,pcOwn,pcPgm,SYN_CLE_GETENV,MAN_CLE_GETENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETENV' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"DELENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'DELENV':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.14" ,"DELENV",HLP_CLE_DELENV,pcOwn,pcPgm,SYN_CLE_DELENV,MAN_CLE_DELENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'DELENV' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"TRACE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'TRACE':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.15","TRACE"   ,HLP_CLE_TRACE   ,pcOwn,pcPgm,SYN_CLE_TRACE,MAN_CLE_TRACE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'TRACE' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"CONFIG",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'CONFIG':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.16","CONFIG"  ,HLP_CLE_CONFIG  ,pcOwn,pcPgm,SYN_CLE_CONFIG,MAN_CLE_CONFIG,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'CONFIG' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GRAMMAR",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GRAMMAR':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.17","GRAMMAR" ,HLP_CLE_GRAMMAR ,pcOwn,pcPgm,SYN_CLE_GRAMMAR,MAN_CLE_GRAMMAR,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GRAMMAR' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"LEXEM",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'LEXEM':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.18","LEXEM"   ,HLP_CLE_LEXEM   ,pcOwn,pcPgm,SYN_CLE_LEXEM,MAN_CLE_LEXEM,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'LEXEM' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"LICENSE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'LICENSE':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.19","LICENSE" ,HLP_CLE_LICENSE ,pcOwn,pcPgm,SYN_CLE_LICENSE,MAN_CLE_LICENSE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'LICENSE' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"VERSION",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'VERSION':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.20","VERSION" ,HLP_CLE_VERSION ,pcOwn,pcPgm,SYN_CLE_VERSION,MAN_CLE_VERSION,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'VERSION' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"ABOUT",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'ABOUT':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.21","ABOUT"   ,HLP_CLE_ABOUT   ,pcOwn,pcPgm,SYN_CLE_ABOUT,MAN_CLE_ABOUT,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'ABOUT' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"ERRORS",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'ERRORS':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.22","ERRORS"  ,HLP_CLE_ERRORS  ,pcOwn,pcPgm,SYN_CLE_ERRORS,MAN_CLE_ERRORS,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'ERRORS' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (isAll) {
               if (psTab[i].siFlg) {
                  pcCmd=psTab[i].pcKyw;
               } else {
                  pcCmd="";
               }
            }
            if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               if (isMan==FALSE) {
                  if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                     fprintf(pfOut,"Manual page for command '%s':\n\n",pcCmd);
                  } else {
                     fprintf(pfOut,"Manual page for argument '%s':\n\n",pcCmd);
                  }
               }
               vdPrnCommandManpage(pvHdl,pfDoc,pcCmd,i,isMan,TRUE);
               vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
               if (isMan==TRUE) {
                  if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                     fprintf(pfOut,"Manual page for command '%s' successfully written to file (%s)\n",pcCmd,pcFil);
                  } else {
                     fprintf(pfOut,"Manual page for argument '%s' successfully written to file (%s)\n",pcCmd,pcFil);
                  }
               }
               if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
            }
         }
         if (isAll==TRUE) ERROR(CLERTC_OK,NULL);

         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char acPat[strlen(pcDef)+strlen(pcCmd+2)];
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  sprintf(acPat,"%s.%s",pcDef,pcCmd);
                  if (pfOut!=NULL)fprintf(pfOut,"Manual page fo'argument '%s':\n\n",acPat);
                  vdPrnCommandManpage(pvHdl,pfDoc,acPat,i,isMan,TRUE);
                  if (isMan==TRUE) {
                     fprintf(pfOut,"Manual page for argument '%s' successfully written to file (%s)\n",acPat,pcFil);
                  }
                  ERROR(CLERTC_OK,NULL);
               }
            }
         }
         SAFE_FREE(pcFil);

         pcFil=dcpmapfil(argv[2]);
         if (pcFil==NULL) {
            fprintf(pfErr,"Allocation of memory for file name (%s) failed\n",argv[2]);
            ERROR(CLERTC_MEM,NULL);
         }
//       szFil=strlen(pcFil)+1; not used
         isMan=TRUE;
         pfDoc=fopen(pcFil,filemode("w"));
         if (pfDoc==NULL) {
            fprintf(pfErr,"Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",pcFil,filemode("w"),errno,strerror(errno));
            ERROR(CLERTC_SYS,NULL);
         }
         vdCleManProgram(pfDoc,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,isMan,TRUE);
         if (pfOut!=NULL) fprintf(pfOut,"Manual page for program '%s' successfully written to file (%s)\n",pcPgm,pcFil);
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'MANPAGE' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s MANPAGE %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr,"[.path]\n");
         }
      }
      fprintf(pfErr,"%s %s MANPAGE function\n",pcDep,argv[0]);
      fprintf(pfErr,"%s %s MANPAGE\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"GENDOCU",0,0,FALSE)==0) {
      const char*                pcCmd=NULL;
      const char*                pcSgn=NULL;
      char                       acHdl[1024];
      char                       acNum[16];
      int                        isNbr=TRUE;
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3 || argc==4) {
         if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"NONBR",0,0,FALSE)==0) {
               isNbr=FALSE;
            } else {
               fprintf(pfErr,"Syntax for built-in function 'GENDOCU' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfErr,"%s %s GENDOCU %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfErr,"[.path]=filename [NONBR]\n");
                  }
               }
               fprintf(pfErr,"%s %s GENDOCU filename %cNONBR%c\n",pcDep,argv[0],C_SBO,C_SBC);
               ERROR(CLERTC_CMD,NULL);
            }
         }
         pcSgn=strchr(argv[2],'=');
         if (pcSgn!=NULL) {
            *((char*)pcSgn)=EOS; pcSgn++; pcCmd=argv[2];
         } else {
            pcSgn=argv[2]; pcCmd=NULL;
         }
         pcFil=dcpmapfil(pcSgn);
         if (pcFil==NULL) {
            fprintf(pfErr,"Allocation of memory for file name (%s) failed\n",pcSgn);
            ERROR(CLERTC_MEM,NULL);
         }
         szFil=strlen(pcFil)+1;
         pfDoc=fopen(pcFil,filemode("w"));
         if (pfDoc==NULL) {
            fprintf(pfErr,"Open of documentation file (\"%s\",\"%s\") failed (%d - %s)\n",pcFil,filemode("w"),errno,strerror(errno));
            ERROR(CLERTC_SYS,NULL);
         }
         if (pcCmd!=NULL) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  snprintf(acNum,sizeof(acNum),"2.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,pcCmd,NULL,acNum,"COMMAND",TRUE,FALSE,isNbr);
                  if (siErr<0) {
                     fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                     ERROR(CLERTC_SYN,NULL);
                  } else {
                     if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                        fprintf(pfOut,"Documentation for command '%s' successfully created\n",pcCmd);
                     } else {
                        fprintf(pfOut,"Documentation for argument '%s' successfully created\n",pcCmd);
                     }
                     ERROR(CLERTC_OK,NULL);
                  }
               }
            }
            if (pcDef!=NULL && *pcDef) {
               for (i=0;psTab[i].pcKyw!=NULL;i++) {
                  if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                     char acPat[strlen(pcDef)+strlen(pcCmd+2)];
                     siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                            isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
                     if (siErr) ERROR(siErr,NULL);
                     snprintf(acNum,sizeof(acNum),"2.%d.",i+1);
                     sprintf(acPat,"%s.%s",pcDef,pcCmd);
                     siErr=siClpDocu(pvHdl,pfDoc,acPat,NULL,acNum,"COMMAND",TRUE,FALSE,isNbr);
                     if (siErr<0) {
                        fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                        ERROR(CLERTC_SYN,NULL);
                     } else {
                        fprintf(pfOut,"Documentation for argument '%s' successfully created\n",acPat);
                        ERROR(CLERTC_OK,NULL);
                     }
                  }
               }
            }
         } else {
            if (pcCov!=NULL && *pcCov) {
               fprintm(pfDoc,pcOwn,pcPgm,pcCov,2);
            } else {
               snprintf(acHdl,sizeof(acHdl),"'%s' - User Manual",pcPgm); l=strlen(acHdl); fprintf(pfDoc,"%s\n",acHdl);
               for (i=0;i<l;i++) fprintf(pfDoc,"=");
               fprintf(pfDoc,"\n");
               fprintf(pfDoc,":doctype: book\n\n");
            }
            efprintf(pfDoc,"[[command-line-processor]]\n");
            if (isNbr) {
               fprintf(pfDoc,"1. COMMAND LINE PROCESSOR\n");
               fprintf(pfDoc,"-------------------------\n\n");
            } else {
               fprintf(pfDoc,"COMMAND LINE PROCESSOR\n");
               fprintf(pfDoc,"----------------------\n\n");
            }
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_MAIN,2);
            efprintf(pfDoc,"indexterm:[Command line processor]\n\n\n");

            vdCleManProgram(pfDoc,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,FALSE,isNbr);

            if (isNbr) {
               snprintf(acHdl,sizeof(acHdl),"3. Available commands"); l=strlen(acHdl); fprintf(pfDoc,"%s\n",acHdl);
               for (i=0;i<l;i++) fprintf(pfDoc,"-");
               fprintf(pfDoc,"\n\n");
            } else {
               snprintf(acHdl,sizeof(acHdl),"Available commands"); l=strlen(acHdl); fprintf(pfDoc,"%s\n",acHdl);
               for (i=0;i<l;i++) fprintf(pfDoc,"-");
               fprintf(pfDoc,"\n\n");
            }
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_COMMANDS,2);
            efprintf(pfDoc,"indexterm:[Available commands]\n\n\n");

            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  snprintf(acNum,sizeof(acNum),"3.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,psTab[i].pcKyw,NULL,acNum,"COMMAND",TRUE,FALSE,isNbr);
                  if (siErr<0) {
                     fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                     ERROR(CLERTC_SYN,NULL);
                  }
                  vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
               }
            }

            if (isNbr) {
               snprintf(acHdl,sizeof(acHdl),"4. Available built-in functions"); l=strlen(acHdl); fprintf(pfDoc,"%s\n",acHdl);
               for (i=0;i<l;i++) fprintf(pfDoc,"-");
               fprintf(pfDoc,"\n\n");
            } else {
               snprintf(acHdl,sizeof(acHdl),"Available built-in functions"); l=strlen(acHdl); fprintf(pfDoc,"%s\n",acHdl);
               for (i=0;i<l;i++) fprintf(pfDoc,"-");
               fprintf(pfDoc,"\n\n");
            }
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_FUNCTIONS,2);
            efprintf(pfDoc,"indexterm:[Available built-in functions]\n\n\n");

            vdCleManFunction(pfDoc,S_TLD,"4.1" ,"SYNTAX"  ,HLP_CLE_SYNTAX  ,pcOwn,pcPgm,SYN_CLE_SYNTAX  ,MAN_CLE_SYNTAX  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.2" ,"HELP"    ,HLP_CLE_HELP    ,pcOwn,pcPgm,SYN_CLE_HELP    ,MAN_CLE_HELP    ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.3" ,"MANPAGE" ,HLP_CLE_MANPAGE ,pcOwn,pcPgm,SYN_CLE_MANPAGE ,MAN_CLE_MANPAGE ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.4" ,"GENDOCU" ,HLP_CLE_GENDOCU ,pcOwn,pcPgm,SYN_CLE_GENDOCU ,MAN_CLE_GENDOCU ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.5" ,"GENPROP" ,HLP_CLE_GENPROP ,pcOwn,pcPgm,SYN_CLE_GENPROP ,MAN_CLE_GENPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.6" ,"SETPROP" ,HLP_CLE_SETPROP ,pcOwn,pcPgm,SYN_CLE_SETPROP ,MAN_CLE_SETPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.7" ,"CHGPROP" ,HLP_CLE_CHGPROP ,pcOwn,pcPgm,SYN_CLE_CHGPROP ,MAN_CLE_CHGPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.8" ,"DELPROP" ,HLP_CLE_DELPROP ,pcOwn,pcPgm,SYN_CLE_DELPROP ,MAN_CLE_DELPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.9" ,"GETPROP" ,HLP_CLE_GETPROP ,pcOwn,pcPgm,SYN_CLE_GETPROP ,MAN_CLE_GETPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.10","SETOWNER",HLP_CLE_SETOWNER,pcOwn,pcPgm,SYN_CLE_SETOWNER,MAN_CLE_SETOWNER,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.11","GETOWNER",HLP_CLE_GETOWNER,pcOwn,pcPgm,SYN_CLE_GETOWNER,MAN_CLE_GETOWNER,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.12","SETENV"  ,HLP_CLE_SETENV  ,pcOwn,pcPgm,SYN_CLE_SETENV  ,MAN_CLE_SETENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.13","GETENV"  ,HLP_CLE_GETENV  ,pcOwn,pcPgm,SYN_CLE_GETENV  ,MAN_CLE_GETENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.14","DELENV"  ,HLP_CLE_DELENV  ,pcOwn,pcPgm,SYN_CLE_DELENV  ,MAN_CLE_DELENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.15","TRACE"   ,HLP_CLE_TRACE   ,pcOwn,pcPgm,SYN_CLE_TRACE   ,MAN_CLE_TRACE   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.16","CONFIG"  ,HLP_CLE_CONFIG  ,pcOwn,pcPgm,SYN_CLE_CONFIG  ,MAN_CLE_CONFIG  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.17","GRAMMAR" ,HLP_CLE_GRAMMAR ,pcOwn,pcPgm,SYN_CLE_GRAMMAR ,MAN_CLE_GRAMMAR ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.18","LEXEM"   ,HLP_CLE_LEXEM   ,pcOwn,pcPgm,SYN_CLE_LEXEM   ,MAN_CLE_LEXEM   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.19","LICENSE" ,HLP_CLE_LICENSE ,pcOwn,pcPgm,SYN_CLE_LICENSE ,MAN_CLE_LICENSE ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.20","VERSION" ,HLP_CLE_VERSION ,pcOwn,pcPgm,SYN_CLE_VERSION ,MAN_CLE_VERSION ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.21","ABOUT"   ,HLP_CLE_ABOUT   ,pcOwn,pcPgm,SYN_CLE_ABOUT   ,MAN_CLE_ABOUT   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.22","ERRORS"  ,HLP_CLE_ERRORS  ,pcOwn,pcPgm,SYN_CLE_ERRORS  ,MAN_CLE_ERRORS  ,FALSE,isNbr);
            s=1;

            if (psApx!=NULL) {
               efprintf(pfDoc,"[[appendix-clp-strings]]\n");
               efprintf(pfDoc,"[appendix]\n");
               fprintf(pfDoc,"OTHER CLP STRINGS\n");
               fprintf(pfDoc,"-----------------\n\n");
               if (pcApx!=NULL && *pcApx) fprintm(pfDoc,pcOwn,pcPgm,pcApx,1);
               for (i=0;psApx[i].pcHdl!=NULL;i++) {
                  pvHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,psApx[i].pcRot,psApx[i].pcKyw,psApx[i].pcMan,psApx[i].pcHlp,psApx[i].isOvl,psApx[i].psTab,NULL,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvF2S,pfF2S);
                  if (pvHdl==NULL) {
                     fprintf(pfErr,"Open of parser for CLP string of appendix '%s' failed\n",psApx[i].pcRot);
                     return(CLERTC_TAB);
                  }
                  snprintf(acNum,sizeof(acNum),"A.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,psApx[i].pcKyw,psApx[i].pcHdl,acNum,"Appendix",TRUE,FALSE,isNbr);
                  if (siErr<0) {
                     fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                     ERROR(CLERTC_SYN,NULL);
                  }
                  vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
               }
               efprintf(pfDoc,"indexterm:[Appendix CLP Strings]\n\n\n");
               s++;
            }

            siErr=siCleSimpleInit(pfOut,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
            if (siErr) ERROR(siErr,NULL);
            efprintf(pfDoc,"[[appendix-lexem]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"LEXEM\n");
            fprintf(pfDoc,"-----\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_LEXEM,1);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"Lexemes (regular expressions) for argument list or parameter file\n");
            siErr=siClpLexem(pvHdl,pfDoc); s++;
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            efprintf(pfDoc,"indexterm:[Appendix Lexem]\n\n\n");
            if (siErr<0) {
               fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(CLERTC_SYN,NULL);
            }

            efprintf(pfDoc,"[[appendix-grammar]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"GRAMMAR\n");
            fprintf(pfDoc,"-------\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_GRAMMAR,1);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"Grammar for argument list, parameter file or property file\n");
            siErr=siClpGrammar(pvHdl,pfDoc); s++;
            vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            efprintf(pfDoc,"indexterm:[Appendix Grammar]\n\n\n");
            if (siErr<0) {
               fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(CLERTC_SYN,NULL);
            }
            efprintf(pfDoc,"[[appendix-properties]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"PROPERTIES\n");
            fprintf(pfDoc,"----------\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_PROPERTIES,1);

            fprintf(pfDoc ,"REMAINING DOCUMENTATION\n");
            efprintf(pfDoc,"~~~~~~~~~~~~~~~~~~~~~~~\n\n");
            fprintf(pfDoc,"Below you can find the documentation for all parameter which are only\n"
                          "available over the property definitions. These parameters are not a part\n"
                          "of the description for each command above. This list is generated and\n"
                          "can be empty. In this case all parameter are available over the\n"
                          "command line. If a manual page available a description in other case\n"
                          "the help message is printed.\n\n");
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               siErr=siClpProperties(pvHdl,CLPPRO_MTD_DOC,10,psTab[i].pcKyw,pfDoc);
               vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
            }
            if (siErr<0) {
               fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(CLERTC_SYN,NULL);
            }

            fprintf(pfDoc ,"PREDEFINED DEFAULTS\n");
            efprintf(pfDoc,"~~~~~~~~~~~~~~~~~~~\n\n");
            fprintf(pfDoc,"Below you can find a sample property file which contains all predefined\n"
                          "default values for all supported commands when this documentation was\n"
                          "created.\n\n");
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"\n%c Property file for: %s.%s %c\n",C_HSH,pcOwn,pcPgm,C_HSH);
            efprintf(pfDoc,"%s",HLP_CLE_PROPFIL);
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               siErr=siClpProperties(pvHdl,CLPPRO_MTD_SET,10,psTab[i].pcKyw,pfDoc);
               vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
            }
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            if (siErr<0) {
               fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(CLERTC_SYN,NULL);
            }
            efprintf(pfDoc,"indexterm:[Appendix Properties]\n\n\n");

            efprintf(pfDoc,"[[appendix-returncodes]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"RETURN CODES\n");
            fprintf(pfDoc,"------------\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_RETURNCODES,1);
            if (pcScc!=NULL && *pcScc) fprintm(pfDoc,pcOwn,pcPgm,pcScc,1);
            efprintf(pfDoc,"indexterm:[Appendix Returncodes]\n\n\n");

            if (pfMsg!=NULL) {
               efprintf(pfDoc,"[[appendix-reasoncodes]]\n");
               efprintf(pfDoc,"[appendix]\n");
               fprintf(pfDoc,"REASON CODES\n");
               fprintf(pfDoc,"------------\n\n");
               fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_REASONCODES,1);
               for (i=1,m=pfMsg(i);m!=NULL;i++,m=pfMsg(i)) {
                  if (*m) fprintf(pfDoc," * %d - %s\n",i,m);
               }
               efprintf(pfDoc,"indexterm:[Appendix Reasoncodes]\n\n\n");
            }

            if (pcVsn!=NULL && *pcVsn) {
               efprintf(pfDoc,"[[appendix-version]]\n");
               efprintf(pfDoc,"[appendix]\n");
               fprintf(pfDoc,"VERSION\n");
               fprintf(pfDoc,"-------\n\n");
               fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_VERSION,1);
               fprintf(pfDoc,"------------------------------------------------------------------------\n");
               fprintf(pfDoc,"%s",pcVsn); s++;
               fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
               efprintf(pfDoc,"indexterm:[Appendix Version]\n\n\n");
            }

            if (pcAbo!=NULL && *pcAbo) {
               efprintf(pfDoc,"[[appendix-about]]\n");
               efprintf(pfDoc,"[appendix]\n");
               fprintf(pfDoc,"ABOUT\n");
               fprintf(pfDoc,"-----\n\n");
               fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_ABOUT,1);
               fprintf(pfDoc,"------------------------------------------------------------------------\n");
               fprintf(pfDoc,"%s",pcAbo); s++;
               fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
               efprintf(pfDoc,"indexterm:[Appendix About]\n\n\n");
            }

            if (pcGls!=NULL && *pcGls) {
               efprintf(pfDoc,"[glossary]\n");
               fprintf(pfDoc,"GLOSSARY\n");
               fprintf(pfDoc,"--------\n\n");
               fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_GLOSSARY,1);
               fprintm(pfDoc,pcOwn,pcPgm,pcGls,2);
               efprintf(pfDoc,"indexterm:[Glossary]\n\n\n");
            }

            efprintf(pfDoc,"[index]\n");
            fprintf(pfDoc,"INDEX\n");
            fprintf(pfDoc,"-----\n\n");

            if (pcFin!=NULL && *pcFin) {
               efprintf(pfDoc,"\n\n");
               fprintm(pfDoc,pcOwn,pcPgm,pcFin,2);
            } else {
               efprintf(pfDoc,"\n\n");
               efprintf(pfDoc,"[colophon]\n");
               fprintf(pfDoc,"COLOPHON\n");
               fprintf(pfDoc,"--------\n\n");
               fprintf(pfDoc,"Owner:   %s\n",pcOwn);
               fprintf(pfDoc,"Program: %s\n",pcPgm);
               fprintf(pfDoc,"\n\n");
            }
            fprintf(pfOut,"Documentation for program '%s' successfully created\n",pcPgm);
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'GENDOCU' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s GENDOCU %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr,"[.path]=filename [NONBR]\n");
         }
      }
      fprintf(pfErr,"%s %s GENDOCU filename %cNONBR%c\n",pcDep,argv[0],C_SBO,C_SBC);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"GENPROP",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3) {
         const char* pcSgn;
         const char* pcCmd;
         pcSgn=strchr(argv[2],'=');
         if (pcSgn!=NULL) {
            *((char*)pcSgn)=EOS; pcSgn++; pcCmd=argv[2];
         } else {
            pcSgn=argv[2]; pcCmd=NULL;
         }
         pcFil=dcpmapfil(pcSgn);
         if (pcFil==NULL) {
            fprintf(pfErr,"Allocation of memory for file name (%s) failed\n",pcSgn);
            ERROR(CLERTC_MEM,NULL);
         }
         szFil=strlen(pcFil)+1;
         pfPro=fopen(pcFil,filemode("w"));
         if (pfPro==NULL) {
            fprintf(pfErr,"Open of property file (\"%s\",\"%s\") failed (%d-%s)\n",pcFil,filemode("w"),errno,strerror(errno));
            ERROR(CLERTC_SYS,NULL);
         }
         if (pcCmd==NULL) fprintf(pfPro,"\n%c Property file for: %s.%s %c\n\n",C_HSH,pcOwn,pcPgm,C_HSH);
                    else  fprintf(pfPro,"\n%c Property file for: %s.%s.%s %c\n\n",C_HSH,pcOwn,pcPgm,pcCmd,C_HSH);
         efprintf(pfPro,"%s",HLP_CLE_PROPFIL);

         if (pcCmd==NULL) {
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               siErr=siClpProperties(pvHdl,CLPPRO_MTD_CMT,10,psTab[i].pcKyw,pfPro);
               vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
            }
            if (siErr<0) {
               fprintf(pfErr,"Write property file (%s) for program '%s' failed (%d-%s)\n",pcFil,pcPgm,errno,strerror(errno));
               ERROR(CLERTC_SYN,NULL);
            } else {
               fprintf(pfOut,"Property file (%s) for program '%s' successfully written\n",pcFil,pcPgm);
               ERROR(CLERTC_OK,NULL);
            }
         } else {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,0,FALSE)==0) {
                  siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  siErr=siClpProperties(pvHdl,CLPPRO_MTD_CMT,10,psTab[i].pcKyw,pfPro);
                  vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
                  if (siErr<0) {
                     fprintf(pfErr,"Write property file (%s) for command '%s' failed (%d-%s)\n",pcFil,pcCmd,errno,strerror(errno));
                     ERROR(CLERTC_SYN,NULL);
                  } else {
                     fprintf(pfOut,"Property file (%s) for command '%s' successfully written\n",pcFil,pcCmd);
                     ERROR(CLERTC_OK,NULL);
                  }
               }
            }
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'GENPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s GENPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfErr,"%s %s GENPROP filename\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"SETPROP",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3) {
         const char* pcSgn;
         const char* pcCmd;
         pcSgn=strchr(argv[2],'=');
         if (pcSgn!=NULL) {
            *((char*)pcSgn)=EOS; pcSgn++; pcCmd=argv[2];
            for (i=0;psTab[i].pcKyw!=NULL && strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,0,FALSE);i++);
            if (psTab[i].pcKyw==NULL) {
               fprintf(pfErr,"Syntax for built-in function 'SETPROP' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfErr,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               ERROR(CLERTC_CMD,NULL);
            }
            srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm)+strlen(pcCmd),"%s.%s.%s.property.file",pcOwn,pcPgm,pcCmd);
            if (pcCnf==NULL) {
               fprintf(pfErr,"Allocation of memory for property string failed\n");
               ERROR(CLERTC_MEM,NULL);
            }
         } else {
            pcSgn=argv[2]; pcCmd=NULL;
            srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm),"%s.%s.property.file",pcOwn,pcPgm);
            if (pcCnf==NULL) {
               fprintf(pfErr,"Allocation of memory for property string failed\n");
               ERROR(CLERTC_MEM,NULL);
            }
         }
         if (*pcSgn==0) {
            fprintf(pfErr,"Syntax for built-in function 'SETPROP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfErr,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            fprintf(pfErr,"File name was not specified.\n"
                          "To delete a property file from the list, please use the function DELPROP %ccommand%c\n",C_SBO,C_SBC);
            ERROR(CLERTC_CMD,NULL);
         }
         siErr=siCnfSet(psCnf,pfOut,pcCnf,pcSgn,TRUE);
         if (siErr) ERROR(CLERTC_CFG,NULL); else {
            fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",pcCnf,pcSgn);
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'SETPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfErr,"%s %s SETPROP filename\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"CHGPROP",0,0,FALSE)==0 || (pcDef!=NULL && strxcmp(isCas,pcDef,"flam",0,0,FALSE)==0 && strxcmp(isCas,argv[1],"DEFAULTS",0,0,FALSE)==0)) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc>=3) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,0,0,FALSE)==0) {
               size_t szPro=CLEINI_PROSIZ;
               char*  pcPro=(char*)calloc(1,szPro);
               if (pcPro==NULL) {
                  fprintf(pfErr,"Memory allocation for property list failed\n");
                  ERROR(CLERTC_MEM,pcPro);
               }
               for (j=3;j<argc;j++) {
                  if (j>3) {
                     srprintc(&pcPro,&szPro,strlen(pcOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw)," %s.%s.%s.",pcOwn,pcPgm,psTab[i].pcKyw);
                  } else {
                     srprintc(&pcPro,&szPro,strlen(pcOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw),"%s.%s.%s.",pcOwn,pcPgm,psTab[i].pcKyw);
                  }
                  char* pcSgn=strchr(argv[j],'=');
                  if (pcSgn!=NULL) {
                     *pcSgn=0x00; pcSgn++;
                     srprintc(&pcPro,&szPro,strlen(argv[j])+strlen(pcSgn),"%s=\"%s\"",argv[j],pcSgn);
                  } else {
                     srprintc(&pcPro,&szPro,strlen(argv[j]),"%s=\"\"",argv[j]);
                  }
               }
               siErr=siCleChangeProperties(psTab[i].pfIni,psTab[i].pvClp,pcHom,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,pcPro,
                     psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,pfMsg,pvF2S,pfF2S);
               ERROR(siErr,pcPro);
            }
         }
      }
      if (pcDef!=NULL && *pcDef) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
               size_t szPro=CLEINI_PROSIZ;
               char*  pcPro=calloc(1,szPro);
               if (pcPro==NULL) {
                  fprintf(pfErr,"Memory allocation for property list failed\n");
                  ERROR(CLERTC_MEM,pcPro);
               }
               for (j=2;j<argc;j++) {
                  if (j>2) {
                     srprintc(&pcPro,&szPro,strlen(pcOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw)," %s.%s.%s.",pcOwn,pcPgm,psTab[i].pcKyw);
                  } else {
                     srprintc(&pcPro,&szPro,strlen(pcOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw),"%s.%s.%s.",pcOwn,pcPgm,psTab[i].pcKyw);
                  }
                  char* pcSgn=strchr(argv[j],'=');
                  if (pcSgn!=NULL) {
                     *pcSgn=0x00; pcSgn++;
                     srprintc(&pcPro,&szPro,strlen(argv[j])+strlen(pcSgn),"%s=\"%s\"",argv[j],pcSgn);
                  } else {
                     srprintc(&pcPro,&szPro,strlen(argv[j]),"%s=\"\"",argv[j]);
                  }
               }
               siErr=siCleChangeProperties(psTab[i].pfIni,psTab[i].pvClp,pcHom,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,pcPro,
                     psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,pfMsg,pvF2S,pfF2S);
               ERROR(siErr,pcPro);
            }
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'CHGPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s CHGPROP %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr," [path[=value]]*\n");
         }
      }
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"DELPROP",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm),"%s.%s.property.file",pcOwn,pcPgm);
          if (pcCnf==NULL) {
             fprintf(pfErr,"Allocation of memory for property string failed\n");
             ERROR(CLERTC_MEM,NULL);
          }
      } else if (argc==3) {
         for (i=0;psTab[i].pcKyw!=NULL && strxcmp(isCas,argv[2],psTab[i].pcKyw,0,0,FALSE);i++);
         if (psTab[i].pcKyw==NULL) {
            fprintf(pfErr,"Syntax for built-in function 'DELPROP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfErr,"%s %s DELPROP %s\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            ERROR(CLERTC_CMD,NULL);
         }
         srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm)+strlen(argv[2]),"%s.%s.%s.property.file",pcOwn,pcPgm,argv[2]);
         if (pcCnf==NULL) {
            fprintf(pfErr,"Allocation of memory for property string failed\n");
            ERROR(CLERTC_MEM,NULL);
         }
      } else {
         fprintf(pfErr,"Syntax for built-in function 'DELPROP' not valid\n");
         for (i=0;psTab[i].pcKyw!=NULL ;i++) {
            if (psTab[i].siFlg) {
               fprintf(pfErr,"%s %s DELPROP %s\n",pcDep,argv[0],psTab[i].pcKyw);
            }
         }
         fprintf(pfErr,"%s %s DELPROP\n",pcDep,argv[0]);
         ERROR(CLERTC_CMD,NULL);
      }
      siErr=siCnfSet(psCnf,pfOut,pcCnf,"",TRUE);
      if (siErr) ERROR(CLERTC_CFG,NULL); else {
         fprintf(pfOut,"Delete configuration keyword '%s' was successful\n",pcCnf);
         ERROR(CLERTC_OK,NULL);
      }
   } else if (strxcmp(isCas,argv[1],"GETPROP",0,0,FALSE)==0 || (pcDef!=NULL && strxcmp(isCas,pcDef,"flam",0,0,FALSE)==0 && strxcmp(isCas,argv[1],"LIST",0,0,FALSE)==0)) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Properties for program '%s':\n",pcPgm);
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                    psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                    &pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
            if (siErr) ERROR(siErr,NULL);
            vdPrnProperties(pvHdl,psTab[i].pcKyw,TRUE,10);
            vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
         }
         ERROR(CLERTC_OK,NULL);
      } else if (argc>=3) {
         if (argc==3) {
            siDep=1;  isSet=FALSE;
         } else if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"DEFALL",0,0,FALSE)==0) {
               siDep=10; isSet=TRUE;
            } else if (strxcmp(isCas,argv[3],"DEPALL",0,0,FALSE)==0) {
                  siDep=10; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH1",0,0,FALSE)==0) {
                  siDep=1; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH2",0,0,FALSE)==0) {
                  siDep=2; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH3",0,0,FALSE)==0) {
                  siDep=3; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH4",0,0,FALSE)==0) {
                  siDep=4; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH5",0,0,FALSE)==0) {
                  siDep=5; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH6",0,0,FALSE)==0) {
                  siDep=6; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH7",0,0,FALSE)==0) {
                  siDep=7; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH8",0,0,FALSE)==0) {
                  siDep=8; isSet=FALSE;
            } else if (strxcmp(isCas,argv[3],"DEPTH9",0,0,FALSE)==0) {
                  siDep=9; isSet=FALSE;
            } else {
               fprintf(pfErr,"Syntax for built-in function 'GETPROP' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfErr,"%s %s GETPROP %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
                  }
               }
               ERROR(CLERTC_CMD,NULL);
            }
         } else {
            fprintf(pfErr,"Syntax for built-in function 'GETPROP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfErr,"%s %s GETPROP %s",pcDep,argv[0],psTab[i].pcKyw);
                  efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
               }
            }
            ERROR(CLERTC_CMD,NULL);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,
                                       psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(siErr,NULL);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Properties for command '%s':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Properties for argument '%s':\n",argv[2]);
               }
               vdPrnProperties(pvHdl,argv[2],isSet,siDep);
               ERROR(CLERTC_OK,NULL);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char acPat[strlen(pcDef)+strlen(argv[2]+2)];
                  siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,
                                          psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S);
                  if (siErr) ERROR(siErr,NULL);
                  sprintf(acPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Properties for argument '%s':\n",acPat);
                  vdPrnProperties(pvHdl,acPat,isSet,siDep);
                  ERROR(CLERTC_OK,NULL);
               }
            }
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'GETPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s GETPROP %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
         }
      }
      fprintf(pfErr,"%s %s GETPROP\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"SETOWNER",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3) {
         srprintf(&pcCnf,&szCnf,strlen(pcPgm),"%s.owner.id",pcPgm);
         if (pcCnf==NULL) {
            fprintf(pfErr,"Allocation of memory for owner string failed\n");
            ERROR(CLERTC_MEM,NULL);
         }
         siErr=siCnfSet(psCnf,pfOut,pcCnf,argv[2],TRUE);
         if (siErr) ERROR(CLERTC_CFG,NULL); else {
            fprintf(pfOut,"Setting configuration key word '%s' to value '%s' was successful\n",pcCnf,argv[2]);
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'SETOWNER' not valid\n");
      fprintf(pfErr,"%s %s SETOWNER name\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"GETOWNER",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Current owner id for '%s' is: %s\n",argv[0],pcOwn);
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'GETOWNER' not valid\n");
      fprintf(pfErr,"%s %s GETOWNER\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"SETENV",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3) {
         const char* pcVal;
         const char* pcEnv;
         pcVal=strchr(argv[2],'=');
         if (pcVal!=NULL) {
            *((char*)pcVal)=EOS; pcVal++; pcEnv=argv[2];
         } else {
            fprintf(pfErr,"Syntax for built-in function 'SETENV' not valid\n");
            fprintf(pfErr,"%s %s SETENV variable=value\n",pcDep,argv[0]);
            ERROR(CLERTC_CMD,NULL);
         }
         srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm)+strlen(pcEnv),"%s.%s.envar.%s",pcOwn,pcPgm,pcEnv);
         if (pcCnf==NULL) {
            fprintf(pfErr,"Allocation of memory for envar string failed\n");
            ERROR(CLERTC_MEM,NULL);
         }
         siErr=siCnfSet(psCnf,pfOut,pcCnf,pcVal,TRUE);
         if (siErr) ERROR(CLERTC_CFG,NULL); else {
            fprintf(pfOut,"Setting environment variable '%s' to value '%s' was successful\n",pcCnf,pcVal);
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'SETENV' not valid\n");
      fprintf(pfErr,"%s %s SETENV variable=value\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"GETENV",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Current environment variables for owner '%s':\n",pcOwn);
         siCnt=siCnfPrnEnv(psCnf,pfOut,pcDep,pcOwn,pcPgm);
         if (siCnt) {
            fprintf(pfOut,"Defined in file \"%s\"\n",psCnf->pcFil);
         } else {
            fprintf(pfOut,"No environment variables defined in file \"%s\" for owner '%s'\n",psCnf->pcFil,pcOwn);
         }
         ERROR(CLERTC_OK,NULL);
      }
      fprintf(pfErr,"Syntax for built-in function 'GETENV' not valid\n");
      fprintf(pfErr,"%s %s GETENV\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"DELENV",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3) {
         srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm)+strlen(argv[2]),"%s.%s.envar.%s",pcOwn,pcPgm,argv[2]);
         if (pcCnf==NULL) {
            fprintf(pfErr,"Allocation of memory for envar string failed\n");
            ERROR(CLERTC_MEM,NULL);
         }
         siErr=siCnfSet(psCnf,pfOut,pcCnf,"",TRUE);
         if (siErr) ERROR(CLERTC_CFG,NULL); else {
            fprintf(pfOut,"Deleting the environment variable '%s' was successful\n",pcCnf);
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'DELENV' not valid\n");
      fprintf(pfErr,"%s %s DELENV variable\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"TRACE",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3) {
         if (strxcmp(isCas,argv[2],"ON",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-ON",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--ON",0,0,FALSE)==0) {
            srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm),"%s.%s.trace",pcOwn,pcPgm);
            if (pcCnf==NULL) {
               fprintf(pfErr,"Allocation of memory for envar string failed\n");
               ERROR(CLERTC_MEM,NULL);
            }
            siErr=siCnfSet(psCnf,pfOut,pcCnf,"ON",TRUE);
            if (siErr) ERROR(CLERTC_CFG,NULL); else {
               fprintf(pfOut,"Setting configuration keyword '%s' to value 'ON' was successful\n",pcCnf);
               ERROR(CLERTC_OK,NULL);
            }
         } else if (strxcmp(isCas,argv[2],"OFF",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-OFF",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--OFF",0,0,FALSE)==0) {
            srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm),"%s.%s.trace",pcOwn,pcPgm);
            if (pcCnf==NULL) {
               fprintf(pfErr,"Allocation of memory for envar string failed\n");
               ERROR(CLERTC_MEM,NULL);
            }
            siErr=siCnfSet(psCnf,pfOut,pcCnf,"OFF",TRUE);
            if (siErr) ERROR(CLERTC_CFG,NULL); else {
               fprintf(pfOut,"Setting configuration keyword '%s' to value 'OFF' was successful\n",pcCnf);
               ERROR(CLERTC_OK,NULL);
            }
         } else {
            const char* pcSgn;
            pcSgn=strchr(argv[2],'=');
            if (pcSgn!=NULL) {
               const char* pcCmd=argv[2];
               *((char*)pcSgn)=EOS; pcSgn++;
               if (strxcmp(isCas,pcCmd,"FILE",0,0,FALSE)==0) {
                  srprintf(&pcCnf,&szCnf,strlen(pcOwn)+strlen(pcPgm),"%s.%s.trace.file",pcOwn,pcPgm);
                  if (pcCnf==NULL) {
                     fprintf(pfErr,"Allocation of memory for envar string failed\n");
                     ERROR(CLERTC_MEM,NULL);
                  }
                  siErr=siCnfSet(psCnf,pfOut,pcCnf,pcSgn,TRUE);
                  if (siErr) ERROR(CLERTC_CFG,NULL); else {
                     if (*pcSgn) {
                        fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",pcCnf,pcSgn);
                     } else {
                        fprintf(pfOut,"Deleting value from configuration keyword '%s' was successful\n",pcCnf);
                     }
                     ERROR(CLERTC_OK,NULL);
                  }
               }
            }
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'TRACE' not valid\n");
      fprintf(pfErr,"%s %s TRACE ON/OFF\n",pcDep,argv[0]);
      fprintf(pfErr,"%s %s TRACE FILE=filenam\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"CONFIG",0,0,FALSE)==0) {
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==2) {
         fprintf(pfOut,"Current configuration data:\n");
         siCnt=siCnfPrn(psCnf,pfOut,pcDep);
         if (siCnt) {
            fprintf(pfOut,"Assigned to file \"%s\"\n",psCnf->pcFil);
         } else {
            fprintf(pfOut,"No configuration data defined for file \"%s\"\n",psCnf->pcFil);
         }
         ERROR(CLERTC_OK,NULL);
      } else if (argc==3) {
         if (argv[2][0]=='-') argv[2]++;
         if (argv[2][0]=='-') argv[2]++;
         if (strxcmp(isCas,argv[2],"CLEAR",0,0,FALSE)==0) {
            siCnt=siCnfClr(psCnf,pfOut,pcDep);
            if (siCnt) {
               fprintf(pfOut,"Delete %d elements from file \"%s\"\n",siCnt,psCnf->pcFil);
            } else {
               fprintf(pfOut,"No configuration data defined for file \"%s\"\n",psCnf->pcFil);
            }
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'CONFIG' not valid\n");
      fprintf(pfErr,"%s %s CONFIG\n",pcDep,argv[0]);
      fprintf(pfErr,"%s %s CONFIG CLEAR\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else {
      int siMaxCC=0x0FFFFFFF;
      int siMinCC=0x00000000;
      if (strxcmp(isCas,argv[argc-1],"MAXCC=",6,0,FALSE)==0) {
          char* h=strchr(&(argv[argc-1][6]),'-');
          if (h!=NULL && isdigit(h[1])) siMinCC=atoi(h+1);
          if (isdigit(argv[argc-1][6])) siMaxCC=atoi(&(argv[argc-1][6]));
          argc--;
      }
      if (strxcmp(isCas,argv[1],"OWNER=",6,0,FALSE)==0) {
         srprintf(&pcOwn,&szOwn,strlen(&argv[1][6]),"%s",&argv[1][6]);
         if (pcOwn==NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for owner string failed\n");
            siErr=CLERTC_MEM;
            ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
         }
         if (isEnvOwn) {
            if (SETENV("OWNERID",pcOwn)) {
               if (pfOut!=NULL) fprintf(pfOut,"Use owner: '%s' (set as environment variable failed (%d - %s))\n",pcOwn,errno,strerror(errno));
            } else {
               if (pfOut!=NULL) fprintf(pfOut,"Use owner: '%s' (set as environment variable was successful)\n",pcOwn);
            }
         } else {
            if (pfOut!=NULL) fprintf(pfOut,"Use owner: '%s' (environment variable was already defined)\n",pcOwn);
         }
         for (i=2;i<argc;i++) argv[i-1]=argv[i];
         argc--;
      }
      if (argc>1) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            l=strlen(psTab[i].pcKyw);
            if (strxcmp(isCas,argv[1],psTab[i].pcKyw,0,-1,FALSE)==0 ||
                strxcmp(isCas,argv[1],psTab[i].pcKyw,l,'=',TRUE)==0 ||
                strxcmp(isCas,argv[1],psTab[i].pcKyw,l,'(',TRUE)==0 ||
                strxcmp(isCas,argv[1],psTab[i].pcKyw,l,'.',TRUE)==0) {
               char*                         pcCmd=NULL;
               char*                         pcTls=NULL;
               char*                         pcLst=NULL;
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S);
               if (siErr) ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               siErr=siCleGetCommand(pvHdl,pfOut,pcDep,psTab[i].pcKyw,argc,argv,&pcFil,&pcCmd,pvF2S,pfF2S,pcDpa);
               if (siErr) ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               siErr=siClpParseCmd(pvHdl,pcFil,pcCmd,TRUE,TRUE,psTab[i].piOid,&pcTls);
               if (siErr<0) {
                  if (pfErr!=NULL) fprintf(pfErr,"Command line parser for command '%s' failed\n",psTab[i].pcKyw);
                  SAFE_FREE(pcCmd);
                  siErr=CLERTC_SYN;
                  ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               }
               if (pcTls!=NULL) {
                  pcLst=(char*)malloc(strlen(pcTls)+1);
                  if (pcLst!=NULL) {
                     strcpy(pcLst,pcTls);
                  }
               }
               vdClpClose(pvHdl,CLPCLS_MTD_KEP);
               siErr=psTab[i].pfMap(pvHdl,pfErr,pfTrc,psTab[i].piOid,psTab[i].pvClp,psTab[i].pvPar);
               if (siErr) {
                  if (siErr!=siNrn) {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        if (pfErr!=NULL) fprintf(pfErr,"Mapping of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_MAP,siErr,pcMsg);
                     } else {
                        if (pfErr!=NULL) fprintf(pfErr,"Mapping of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_MAP,siErr);
                     }
                     siErr=CLERTC_MAP;
                  } else {
                     siErr=CLERTC_OK;
                  }
                  SAFE_FREE(pcCmd); SAFE_FREE(pcLst);
                  psTab[i].pfFin(pfErr,pfTrc,psTab[i].pvPar);
                  ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               }
               siErr=psTab[i].pfRun(pvHdl,pfErr,pfTrc,pcOwn,pcPgm,pcVsn,pcAbo,pcLic,psTab[i].pcKyw,pcCmd,pcLst,psTab[i].pvPar,&isWrn,&siScc);
               SAFE_FREE(pcCmd); SAFE_FREE(pcLst);
               if (siErr) {
                  if (isWrn&0x00010000) {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' ends with warning (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_WRN,siErr,pcMsg);
                     } else {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' ends with warning (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_WRN,siErr);
                     }
                     psTab[i].pfFin(pfErr,pfTrc,psTab[i].pvPar);
                     siErr=CLERTC_WRN;
                     ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
                  } else {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_RUN,siErr,pcMsg);
                     } else {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_RUN,siErr);
                     }
                     psTab[i].pfFin(pfErr,pfTrc,psTab[i].pvPar);
                     if (siScc>CLERTC_MAX) {
                        siErr=siScc;
                        ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
                     } else {
                        siErr=CLERTC_RUN;
                        ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
                     }
                  }
               }
               siErr=psTab[i].pfFin(pfErr,pfTrc,psTab[i].pvPar);
               vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
               if (siErr) {
                  if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                     if (pfErr!=NULL) fprintf(pfErr,"Finish/cleanup for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_FIN,siErr,pcMsg);
                  } else {
                     if (pfErr!=NULL) fprintf(pfErr,"Finish/cleanup for command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_FIN,siErr);
                  }
                  siErr=CLERTC_FIN;
                  ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               }
               siErr=isWrn&0x00000001;
               ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
            }
         }
      }
      if (pcDef!=NULL && *pcDef && ppArg==NULL) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef);
            siErr=CLERTC_MEM;
            ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
         }
         for (i=argc;i>1;i--) ppArg[i]=argv[i-1];
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc++; argv=ppArg;
         goto EVALUATE;
      }
      if (pfErr!=NULL) {
         fprintf(pfErr,"Command or built-in function '%s' not supported\n",argv[1]);
         vdPrnStaticSyntax(pfErr,psTab,argv[0],pcDep,pcOpt);
      }
      siErr=CLERTC_CMD;
      ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
   }
}
#undef ERROR

/* Interne Funktionen *************************************************/

static int siClePropertyInit(
   tpfIni                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   tpfMsg                        pfMsg,
   void*                         pvF2S,
   tpfF2S                        pfF2S)
{
   int                           siErr;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   char*                         pcPro=NULL;
   char*                         pcFil=NULL;
   int                           siFil=0;
   const char*                   pcMsg;

   if (piFil!=NULL) *piFil=0;
   if (ppFil!=NULL) *ppFil=NULL;
   *ppHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,psTab,pvClp,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvF2S,pfF2S);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Open of property parser for command '%s' failed\n",pcCmd);
      return(CLERTC_TAB);
   }
   siErr=pfIni(*ppHdl,pfErr,pfTrc,pcOwn,pcPgm,pvClp);
   if (siErr) {
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg);
      } else {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr);
      }
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(CLERTC_INI);
   }
   siErr=siCleGetProperties(*ppHdl,pfErr,psCnf,pcOwn,pcPgm,pcCmd,&pcFil,&pcPro,&siFil,pvF2S,pfF2S);
   if (siErr) {
      if (pcPro!=NULL) free(pcPro); SAFE_FREE(pcFil);
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(siErr);
   }
   if (pcPro!=NULL) {
      siErr=siClpParsePro(*ppHdl,pcFil,pcPro,FALSE,NULL);
      if (siErr<0) {
         if (pfErr!=NULL) fprintf(pfErr,"Parsing property file \"%s\" for command '%s' failed\n",pcFil,pcCmd);
         vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
         free(pcPro); SAFE_FREE(pcFil);
         return(CLERTC_SYN);
      }
      free(pcPro);
   }
   if (ppFil!=NULL) *ppFil=pcFil; else {
      SAFE_FREE(pcFil);
   }
   if (piFil!=NULL) *piFil=siFil;
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
   int                           siErr,i;
   FILE*                         pfPro;
   char                          acEnv[((pcHom!=NULL)?strlen(pcHom):0)+strlen(pcOwn)+strlen(pcPgm)+strlen(pcCmd)+64];
   size_t                        szEnv=0;
   char*                         pcEnv=NULL;
   char*                         pcHlp=NULL;

   if (siFil!=3) {
      snprintf(acEnv,sizeof(acEnv),"%s_%s_%s_PROPERTY_FILENAME",pcOwn,pcPgm,pcCmd);
      for (i=0;acEnv[i];i++) acEnv[i]=toupper(acEnv[i]);
      pcFil=GETENV(acEnv);
      if (pcFil!=NULL && *pcFil) {
         srprintf(&pcEnv,&szEnv,strlen(pcFil),"%s",pcFil);
         if (pcEnv==NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for property file name (%s) failed\n",pcFil);
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
         for (i=0;acEnv[i];i++) acEnv[i]=tolower(acEnv[i]);
#endif
         pcFil=acEnv;
      }
   }
   pcHlp=dcpmapfil(pcFil);
   if (pcHlp==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for property file name (%s) failed\n",pcFil);
      SAFE_FREE(pcEnv);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      return(CLERTC_MEM);
   }
   pfPro=fopen(pcHlp,filemode("w"));
   if (pfPro==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Cannot open the property file (\"%s\",\"%s\") for write operation (%d-%s)\n",pcHlp,filemode("w"),errno,strerror(errno));
      SAFE_FREE(pcHlp); SAFE_FREE(pcEnv);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      return(CLERTC_SYS);
   }

   siErr=siClpProperties(pvHdl,CLPPRO_MTD_CMT,10,pcCmd,pfPro);
   vdClpClose(pvHdl,CLPCLS_MTD_ALL); fclose(pfPro);
   if (siErr<0) {
      if (pfErr!=NULL) fprintf(pfErr,"Write property file (%s) for command '%s' failed (%d-%s)\n",pcHlp,pcCmd,errno,strerror(errno));
      SAFE_FREE(pcHlp); SAFE_FREE(pcEnv);
      return(CLERTC_SYN);
   }
   if (pfOut!=NULL) fprintf(pfOut,"Property file (%s) for command '%s' successfully written\n",pcHlp,pcCmd);

   if (siFil!=3) {
      snprintf(acEnv,sizeof(acEnv),"%s.%s.%s.property.file",pcOwn,pcPgm,pcCmd);
      siErr=siCnfSet(psCnf,pfErr,acEnv,pcHlp,TRUE);
      if (siErr) {
         if (pfErr!=NULL) fprintf(pfErr,"Activation of property file (%s) for command '%s' failed\n",pcFil,pcCmd);
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
   tpfIni                        pfIni,
   void*                         pvClp,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   tpfMsg                        pfMsg,
   void*                         pvF2S,
   tpfF2S                        pfF2S)
{
   int                           siErr,siFil=0;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   char*                         pcFil=NULL;
   char*                         pcPro=NULL;
   const char*                   pcMsg;

   *ppHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,psTab,pvClp,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvF2S,pfF2S);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Open of parser for command '%s' failed\n",pcCmd);
      return(CLERTC_TAB);
   }
   siErr=pfIni(*ppHdl,pfErr,pfTrc,pcOwn,pcPgm,pvClp);
   if (siErr) {
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg);
      } else {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr);
      }
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(CLERTC_INI);
   }
   siErr=siCleGetProperties(*ppHdl,pfErr,psCnf,pcOwn,pcPgm,pcCmd,&pcFil,&pcPro,&siFil,pvF2S,pfF2S);
   if (siErr) {
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      SAFE_FREE(pcPro); SAFE_FREE(pcFil);
      return(siErr);
   }
   if (pcPro!=NULL) {
      siErr=siClpParsePro(*ppHdl,pcFil,pcPro,FALSE,NULL);
      if (siErr<0) {
         if (pfErr!=NULL) fprintf(pfErr,"Property parser for command '%s' failed\n",pcCmd);
         vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
         free(pcPro); SAFE_FREE(pcFil);
         return(CLERTC_SYN);
      }
      free(pcPro);
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
         {CLPTYP_NUMBER,"XX",NULL,0,1,1,0,0,CLPFLG_NON,NULL,NULL,NULL,"XX",0,0.0,NULL},
         {CLPTYP_NON   ,NULL,NULL,0,0,0,0,0,CLPFLG_NON,NULL,NULL,NULL,NULL,0,0.0,NULL}
   };
   *ppHdl=pvClpOpen(FALSE,isPfl,isRpl,0,"","","","","",FALSE,asTab,"",pfOut,pfErr,NULL,NULL,NULL,NULL,pcDep,pcOpt,pcEnt,NULL,NULL,NULL);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Open of command line parser for grammar and lexem print out failed\n");
      return(CLERTC_TAB);
   }
   return(CLERTC_OK);
}

static int siCleChangeProperties(
   tpfIni                        pfIni,
   void*                         pvClp,
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   tpfMsg                        pfMsg,
   void*                         pvF2S,
   tpfF2S                        pfF2S)
{
   int                           siErr;
   void*                         pvHdl=NULL;
   char*                         pcFil=NULL;
   int                           siFil=0;

   siErr=siClePropertyInit(pfIni,pvClp,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,
                           piOid,psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,
                           pcDep,pcOpt,pcEnt,psCnf,&pvHdl,&pcFil,&siFil,pfMsg,pvF2S,pfF2S);
   if (siErr) {
      SAFE_FREE(pcFil);
      return(siErr);
   }

   siErr=siClpParsePro(pvHdl,pcFil,pcPro,TRUE,NULL);
   if (siErr<0) {
      if (pfErr!=NULL) fprintf(pfErr,"Property parser for command '%s' failed\n",pcCmd);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      SAFE_FREE(pcFil);
      return(CLERTC_SYN);
   }

   if (siErr==0) {
      if (pfOut!=NULL) fprintf(pfOut, "Don't update any property in property file (%s)\n",(NULL==pcFil)?"null":pcFil);
   } else if (siErr==1) {
      if (pfOut!=NULL) fprintf(pfOut, "Updated 1 property in property file (%s)\n",(NULL==pcFil)?"null":pcFil);
   } else {
      if (pfOut!=NULL) fprintf(pfOut, "Updated %d properties in property file (%s)\n",siErr,(NULL==pcFil)?"null":pcFil);
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
   void*                         pvHdl,
   char*                         pcBuf)
{
   if (psCnf!=NULL) vdCnfCls(psCnf);
   if (pfTrc!=NULL) fclose(pfTrc);
   if (pfDoc!=NULL) fclose(pfDoc);
   if (pfPro!=NULL) fclose(pfPro);
   if (ppArg!=NULL) free(ppArg);
   if (pvHdl!=NULL) vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   if (pcBuf!=NULL) free(pcBuf);
   return(siErr);
}

static void vdCleManProgram(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcHlp,
   const char*                   pcMan,
   const char*                   pcDep,
   const char*                   pcSep,
   const int                     isMan,
   const int                     isNbr)
{
   int                           i,l;
   const char*                   p;
   if (isMan) {
      for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, "(1)\n");
      l=strlen(pcPgm)+3;
      for (i=0;i<l;i++) fprintf(pfOut,"=");
      fprintf(pfOut, "\n");
      fprintf(pfOut, ":doctype: manpage\n\n");
      fprintf(pfOut, "NAME\n");
      fprintf(pfOut, "----\n\n");
      for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      efprintf(pfOut," - `%s`\n\n",pcHlp);
      fprintf(pfOut, "SYNOPSIS\n");
      fprintf(pfOut, "--------\n\n");
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      fprintf(pfOut, "PATH:   %s\n",pcOwn);
      fprintf(pfOut, "TYPE:   PROGRAM\n");
      fprintf(pfOut, "SYNTAX: :> %s COMMAND/FUNCTION ...\n",pcPgm);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
      fprintf(pfOut, "DESCRIPTION\n");
      fprintf(pfOut, "-----------\n\n");
      if (pcMan!=NULL && *pcMan) {
         fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      } else {
         fprintf(pfOut,"No detailed description available for this program.\n\n");
      }
      fprintf(pfOut,"AUTHOR\n");
      fprintf(pfOut,"------\n\n");
      fprintf(pfOut,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
      fprintf(pfOut,"SEE ALSO\n");
      fprintf(pfOut,"--------\n\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
            fprintf(pfOut,".");
            for(p=psTab[i].pcKyw;*p;p++) fprintf(pfOut,"%c",tolower(*p));
            fprintf(pfOut,"(1)\n");
         }
      }
      fprintf(pfOut,"\n");
   } else {
      if (isNbr) {
         fprintf(pfOut,"2. PROGRAM '%s'\n",pcPgm);
         l=strlen(pcPgm)+13;
         for (i=0;i<l;i++) fprintf(pfOut,"%c",'-');
         fprintf(pfOut,"\n\n");
         fprintf(pfOut,"2.1. SYNOPSIS\n");
         for (i=0;i<13;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"PROGRAM '%s'\n",pcPgm);
         l=strlen(pcPgm)+10;
         for (i=0;i<l;i++) fprintf(pfOut,"%c",'-');
         fprintf(pfOut,"\n\n");
         fprintf(pfOut,"SYNOPSIS\n");
         for (i=0;i<8;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      }
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      efprintf(pfOut,"HELP:   %s\n",pcHlp);
      fprintf(pfOut, "PATH:   %s\n",pcOwn);
      fprintf(pfOut, "TYPE:   PROGRAM\n");
      fprintf(pfOut, "SYNTAX: :> %s COMMAND/FUNCTION ...\n",pcPgm);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
      fprintf(pfOut, "indexterm:%cSynopsis for program %s%c\n\n\n",C_SBO,pcPgm,C_SBC);

      if (isNbr) {
         fprintf(pfOut,"2.2. DESCRIPTION\n");
         for (i=0;i<16;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");

      } else {
         fprintf(pfOut,"DESCRIPTION\n");
         for (i=0;i<11;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      }
      if (pcMan!=NULL && *pcMan) {
         fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      } else {
         fprintf(pfOut,"No detailed description available for this program.\n\n");
      }
      fprintf(pfOut,"indexterm:%cDescription for program %s%c\n\n\n",C_SBO,pcPgm,C_SBC);
      if (isNbr) {
         fprintf(pfOut,"2.3. SYNTAX\n");
         for (i=0;i<12;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"SYNTAX\n");
         for (i=0;i<6;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_MAIN_SYNTAX,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Syntax for program '%s':\n",pcPgm);
      vdPrnStaticSyntax(pfOut,psTab,pcPgm,pcDep,pcSep);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");
      fprintf(pfOut,"indexterm:%cSyntax for program %s%c\n\n\n",C_SBO,pcPgm,C_SBC);

      if (isNbr) {
         fprintf(pfOut,"2.4. HELP\n");
         for (i=0;i<9;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"HELP\n");
         for (i=0;i<4;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_MAIN_HELP,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Help for program '%s':\n",pcPgm);
      vdPrnStaticHelp(pfOut,psTab,pcPgm,FALSE,pcDep);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");
      fprintf(pfOut,"indexterm:%cHelp for program %s%c\n\n\n",C_SBO,pcPgm,C_SBC);
   }
}

static void vdCleManFunction(
   FILE*                         pfOut,
   const char*                   pcLev,
   const char*                   pcNum,
   const char*                   pcFct,
   const char*                   pcHlp,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcSyn,
   const char*                   pcMan,
   const int                     isMan,
   const int                     isNbr)
{
   int                           i,l;
   const char*                   p;
   if (isMan) {
      for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, ".");
      for(p=pcFct;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, "(1)\n");
      l=strlen(pcPgm)+strlen(pcFct)+4;
      for (i=0;i<l;i++) fprintf(pfOut,"=");
      fprintf(pfOut, "\n");
      fprintf(pfOut, ":doctype: manpage\n\n");
      fprintf(pfOut, "NAME\n");
      fprintf(pfOut, "----\n\n");
      for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, ".");
      for(p=pcFct;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      efprintf(pfOut," - `%s`\n\n",pcHlp);
      fprintf(pfOut, "SYNOPSIS\n");
      fprintf(pfOut, "--------\n\n");
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      fprintf(pfOut, "PATH:   %s.%s\n",pcOwn,pcPgm);
      fprintf(pfOut, "TYPE:   BUILT-IN FUNCTION\n");
      fprintf(pfOut, "SYNTAX: :> %s %s\n",pcPgm,pcSyn);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
      fprintf(pfOut, "DESCRIPTION\n");
      fprintf(pfOut, "-----------\n\n");
      fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      fprintf(pfOut, "AUTHOR\n------\n\n");
      fprintf(pfOut, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
   } else {
      if (isNbr) {
         fprintf(pfOut,"%s FUNCTION '%s'\n",pcNum,pcFct);
         l=strlen(pcNum)+strlen(pcFct)+12;
         for (i=0;i<l;i++) fprintf(pfOut,"%s",pcLev);
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"FUNCTION '%s'\n",pcFct);
         l=strlen(pcFct)+11;
         for (i=0;i<l;i++) fprintf(pfOut,"%s",pcLev);
         fprintf(pfOut,"\n\n");
      }
      fprintf(pfOut, ".SYNOPSIS\n\n");
      fprintf(pfOut, "-----------------------------------------------------------------------\n");
      efprintf(pfOut,"HELP:   %s\n",pcHlp);
      fprintf(pfOut, "PATH:   %s.%s\n",pcOwn,pcPgm);
      fprintf(pfOut, "TYPE:   BUILT-IN FUNCTION\n");
      fprintf(pfOut, "SYNTAX: :> %s %s\n",pcPgm,pcSyn);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
      fprintf(pfOut, ".DESCRIPTION\n\n");
      fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      fprintf(pfOut,"indexterm:%cBuilt-in function %s%c\n\n\n",C_SBO,pcFct,C_SBC);
   }
}

static void vdPrnStaticSyntax(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
   const char*                   pcDep,
   const char*                   pcSep)
{
   int                           i,f=FALSE;
   fprintf(pfOut,"%s Commands: ",pcDep);
   for (i=0;psTab[i].pcKyw!=NULL;i++) {
      if (psTab[i].siFlg) {
         if (f) fprintf(pfOut,"%s",pcSep);
         fprintf(pfOut,"%s",psTab[i].pcKyw);
         f=TRUE;
      }
   }
   fprintf(pfOut,"\n");
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command \"... argument list ...\" %cMAXCC=%cmax%c%c-min%c%c %cQUIET/SILENT%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command=\" parameter file name \" %cMAXCC=%cmax%c%c-min%c%c %cQUIET/SILENT%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
   fprintf(pfOut,"%s%s You can optionally specify:\n",pcDep,pcDep);
   fprintf(pfOut,"%s%s%s the owner id for this command (to use custom configuration files)\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s the maximum condition code (max) to suppress warnings\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s the minimum condition code (min), zero is returned if the condition code would be smaller\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s QUIET disables the normal log output of the command line executer\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s%s%s SILENT disables log and errors messages of the command line executer\n",pcDep,pcDep,pcDep);
   fprintf(pfOut,"%s Built-in functions:\n",pcDep);
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_SYNTAX  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_HELP    );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_MANPAGE );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_GENDOCU );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_GENPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_SETPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_CHGPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_DELPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_GETPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_SETOWNER);
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_GETOWNER);
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_SETENV  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_GETENV  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_DELENV  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_TRACE   );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_CONFIG  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_GRAMMAR );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_LEXEM   );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_LICENSE );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_VERSION );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_ABOUT   );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_ERRORS  );
}

static void vdPrnStaticHelp(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
   const int                     isCas,
   const char*                   pcDep)
{
   int                           i;
   fprintf(pfOut,"%s Commands - to execute powerful subprograms\n",pcDep);
   for (i=0;psTab[i].pcKyw!=NULL;i++) {
      if (psTab[i].siFlg) {
         fprintf(pfOut,"%s%s %s %-8.8s - ",pcDep,pcDep,pcPgm,psTab[i].pcKyw); efprintf(pfOut,"%s \n",psTab[i].pcHlp);
      }
   }
   fprintf(pfOut,"%s Built-in functions - to give interactive support for the commands above\n",pcDep);
   fprintf(pfOut,"%s%s %s SYNTAX   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_SYNTAX);
   fprintf(pfOut,"%s%s %s HELP     - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_HELP);
   fprintf(pfOut,"%s%s %s MANPAGE  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_MANPAGE);
   fprintf(pfOut,"%s%s %s GENDOCU  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_GENDOCU);
   fprintf(pfOut,"%s%s %s GENPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_GENPROP);
   fprintf(pfOut,"%s%s %s SETPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_SETPROP);
   fprintf(pfOut,"%s%s %s CHGPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_CHGPROP);
   fprintf(pfOut,"%s%s %s DELPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_DELPROP);
   fprintf(pfOut,"%s%s %s GETPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_GETPROP);
   fprintf(pfOut,"%s%s %s SETOWNER - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_SETOWNER);
   fprintf(pfOut,"%s%s %s GETOWNER - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_GETOWNER);
   fprintf(pfOut,"%s%s %s SETENV   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_SETENV);
   fprintf(pfOut,"%s%s %s GETENV   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_GETENV);
   fprintf(pfOut,"%s%s %s DELENV   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_DELENV);
   fprintf(pfOut,"%s%s %s TRACE    - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_TRACE);
   fprintf(pfOut,"%s%s %s CONFIG   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_CONFIG);
   fprintf(pfOut,"%s%s %s GRAMMAR  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_GRAMMAR);
   fprintf(pfOut,"%s%s %s LEXEM    - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_LEXEM);
   fprintf(pfOut,"%s%s %s LICENSE  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_LICENSE);
   fprintf(pfOut,"%s%s %s VERSION  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_VERSION);
   fprintf(pfOut,"%s%s %s ABOUT    - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_ABOUT);
   fprintf(pfOut,"%s%s %s ERRORS   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_ERRORS);
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
   int                     siErr;
   if (siDep==1) {
      fprintf(pfOut,"%s %s \"",pcDep,pcPgm);
      siErr=siClpSyntax(pvHdl,FALSE,FALSE,siDep,pcCmd);
      if (siErr==CLP_OK) fprintf(pfOut,"\"\n");
   } else
      siClpSyntax(pvHdl,TRUE,TRUE,siDep,pcCmd);
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
   const char*             pcCmd,
   const int               siInd,
   const int               isMan,
   const int               isNbr)
{
   char                    acNum[16];
   snprintf(acNum,sizeof(acNum),"3.%d.",siInd+1);
   siClpDocu(pvHdl,pfOut,pcCmd,NULL,acNum,"COMMAND",FALSE,isMan,isNbr);
}

static void vdPrnProperties(
   void*                   pvHdl,
   const char*             pcPat,
   const int               isSet,
   const int               siDep)
{
   siClpProperties(pvHdl,CLPPRO_MTD_SET,siDep,pcPat,NULL);
}

static int siCleGetProperties(
   void*                   pvHdl,
   FILE*                   pfErr,
   TsCnfHdl*               psCnf,
   const char*             pcOwn,
   const char*             pcPgm,
   const char*             pcCmd,
   char**                  ppFil,
   char**                  ppPro,
   int*                    piFlg,
   void*                   pvF2S,
   tpfF2S                  pfF2S)
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
         } else *piFlg=1;
      } else *piFlg=2;
   } else *piFlg=3;
   *ppFil=dcpmapfil(pcHlp);
   if (*ppFil==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for property file name (%s) failed)\n",pcHlp);
      return(CLERTC_MEM);
   }
   siErr=pfF2S(pvF2S,*ppFil,ppPro,&siSiz,acMsg,sizeof(acMsg));
   if (siErr<0) {
      if (pfErr!=NULL) fprintf(pfErr,"Property file: %s\n",acMsg);
      SAFE_FREE(*ppFil);
      SAFE_FREE(*ppPro);
      return(CLERTC_SYS);
   }
   return(CLERTC_OK);
}

static int siCleGetCommand(
   void*                   pvHdl,
   FILE*                   pfErr,
   const char*             pcDep,
   const char*             pcFct,
   int                     argc,
   char*                   argv[],
   char**                  ppFil,
   char**                  ppCmd,
   void*                   pvF2S,
   tpfF2S                  pfF2S,
   const char*             pcDpa)
{
   int                     siErr,siSiz=0;
   int                     l=strlen(pcFct);
   SAFE_FREE(*ppFil);
   if (argv[1][l]==EOS) {
      if (argc>2) {
         siErr=arry2str(argv+2,argc-2," ",1,ppCmd,&siSiz);
         if (siErr<0) {
            SAFE_FREE(*ppCmd);
            switch(siErr) {
            case -1: if (pfErr!=NULL) fprintf(pfErr,"Illegal parameters passed to arry2str() (Bug)\n");                                 return(CLERTC_FAT);
            case -2: if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,strerror(errno)); return(CLERTC_MEM);
            default: if (pfErr!=NULL) fprintf(pfErr,"An unknown error occurred while reading command line.\n");                         return(CLERTC_FAT);
            }
         }
      } else {
         if (pcDpa!=NULL && *pcDpa) {
            siErr=pfF2S(pvF2S,pcDpa,ppCmd,&siSiz,NULL,0);
            if(siErr>0) {
               if (pfErr!=NULL) fprintf(pfErr,"Read parameter in length %d from file '%s'\n",siErr,pcDpa);
            } else {
               char* pcHlp=(char*)realloc(*ppCmd,1);
               if (pcHlp==NULL) {
                  if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,strerror(errno));
                  return(CLERTC_MEM);
               }
               pcHlp[0]=0x00;
               *ppCmd=pcHlp;
            }
         } else {
            char* pcHlp=(char*)realloc(*ppCmd,1);
            if (pcHlp==NULL) {
               if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,strerror(errno));
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
         case -1: if (pfErr!=NULL) fprintf(pfErr,"Illegal parameters passed to arry2str() (Bug)\n");                                 return(CLERTC_FAT);
         case -2: if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for command line failed (%d - %s).\n",errno,strerror(errno)); return(CLERTC_MEM);
         default: if (pfErr!=NULL) fprintf(pfErr,"An unknown error occurred while reading command line.\n");                         return(CLERTC_FAT);
         }
      }
   } else if (argv[1][l]=='=') {
      if (argc!=2) {
         if (pfErr!=NULL) fprintf(pfErr,"The expected parameter file name for '%s' is split into more than one parameter\n",pcFct);
         if (pfErr!=NULL) fprintf(pfErr,"The parameter file name must start with \" and end with \" to join anything into one parameter\n");
         if (pfErr!=NULL) fprintf(pfErr,"Syntax for command '%s' not valid\n",pcFct);
         if (pfErr!=NULL) fprintf(pfErr,"%s %s %s=\" parameter file name \"\n",pcDep,argv[0],pcFct);
         if (pfErr!=NULL) fprintf(pfErr,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
         return(CLERTC_CMD);
      }
      *ppFil=dcpmapfil(argv[1]+l+1);
      if (*ppFil==NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for command file (%s) failed\n",argv[1]+l+1);
         return(CLERTC_MEM);
      }

      char acMsg[1024]="";
      siErr=pfF2S(pvF2S,*ppFil,ppCmd,&siSiz,acMsg,sizeof(acMsg));
      if (siErr<0) {
         if (pfErr!=NULL) fprintf(pfErr,"Command file: %s\n",acMsg);
         SAFE_FREE(*ppFil);
         SAFE_FREE(*ppCmd);
         return(CLERTC_SYS);
      }
   } else {
      if (pfErr!=NULL) fprintf(pfErr,"No blank space ' ', equal sign '=', dot '.' or bracket '(' behind '%s'\n",pcFct);
      if (pfErr!=NULL) fprintf(pfErr,"Please use a blank space to define an argument list or an equal sign for a parameter file\n");
      if (pfErr!=NULL) fprintf(pfErr,"Syntax for command '%s' not valid\n",pcFct);
      if (pfErr!=NULL) fprintf(pfErr,"%s %s %cOWNER=oid%c %s \"... argument list ...\" %cMAXCC=%cmax%c%c-min%c%c %cQUIET%c\n",pcDep,argv[0],C_SBO,C_SBC,pcFct,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
      if (pfErr!=NULL) fprintf(pfErr,"%s %s %cOWNER=oid%c %s=\" parameter file name \" %cMAXCC=%cmax%c%c-min%c%c %cQUIET%c\n",pcDep,argv[0],C_SBO,C_SBC,pcFct,C_SBO,C_SBO,C_SBC,C_SBO,C_SBC,C_SBC,C_SBO,C_SBC);
      if (pfErr!=NULL) fprintf(pfErr,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
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
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration data handle failed\n");
      return(NULL);
   }
   psHdl->isCas=isCas;
   psHdl->psFst=NULL;
   psHdl->psLst=NULL;
   psHdl->pcPgm=pcPgm;
   if (pcFil==NULL || *pcFil==0) return(psHdl);
   psHdl->pcFil=malloc(strlen(pcFil)+1);
   if (psHdl->pcFil==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration file name (%s) failed\n",pcFil);
      vdCnfCls(psHdl);
      return(NULL);
   }
   strcpy(psHdl->pcFil,pcFil);

   pfFil=fopen(psHdl->pcFil,filemode("r"));
   if (pfFil==NULL && (errno==2 || errno==49 || errno==129)) return(psHdl);
   if (pfFil==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Cannot open the configuration file (\"%s\",\"r\") for read operation (%d - %s)\n",psHdl->pcFil,errno,strerror(errno));
      vdCnfCls(psHdl);
      return(NULL);
   }

   while (fgets(acBuf,sizeof(acBuf)-1,pfFil)!=NULL) {
      pcHlp=strpbrk(acBuf,HSH_PBRK);//all variants of '#'
      if (pcHlp!=NULL) *pcHlp=EOS;
      size_t l=strlen(acBuf);
      if (l>0) {
         pcHlp=acBuf+l;
         while (isspace(*(pcHlp-1))) {
            pcHlp--; *pcHlp=EOS;
         }
         pcHlp=strchr(acBuf,'=');
         if (pcHlp!=NULL) {
            pcKyw=acBuf; pcVal=pcHlp+1; *pcHlp=EOS;
            while (isspace(*pcKyw)) pcKyw++;
            while (isspace(*pcVal)) pcVal++;
            for (char* p=pcKyw+strlen(pcKyw);p>pcKyw && isspace(*(p-1));p--) *p=0x00;
            for (char* p=pcVal+strlen(pcVal);p>pcVal && isspace(*(p-1));p--) *p=0x00;
            psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
            if (psEnt==NULL) {
               if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration data element failed\n");
               fclose(pfFil);
               vdCnfCls(psHdl);
               return(NULL);
            }
            siKyw=strlen(pcKyw); siVal=strlen(pcVal);
            if (siKyw && siVal) {
               srprintf(&psEnt->pcKyw,&psEnt->szKyw,siKyw,"%s",pcKyw);
               srprintf(&psEnt->pcVal,&psEnt->szVal,siVal,"%s",pcVal);
               if (psEnt->pcKyw==NULL || psEnt->pcVal==NULL) {
                  if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration data (%s=%s) failed\n",pcKyw,pcVal);
                  free(psEnt);
                  fclose(pfFil);
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

   fclose(pfFil);
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
                  if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for configuration value (%s=%s) failed\n",pcKyw,pcVal);
                  return(-1);
               }
            }else {
               if (pfErr!=NULL) fprintf(pfErr,"Configuration value (%s) for keyword '%s' already exists\n",psEnt->pcVal,psEnt->pcKyw);
               return(1);
            }
         }
         psHdl->isChg=TRUE;
         return(0);
      }
   }

   psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
   if (psEnt==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration data element failed\n");
      return(-1);
   }
   siKyw=strlen(pcKyw); siVal=strlen(pcVal);
   if (siKyw && siVal) {
      srprintf(&psEnt->pcKyw,&psEnt->szKyw,siKyw,"%s",pcKyw);
      srprintf(&psEnt->pcVal,&psEnt->szVal,siVal,"%s",pcVal);
      if (psEnt->pcKyw==NULL || psEnt->pcVal==NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration data (%s=%s) failed\n",pcKyw,pcVal);
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
      if (pfErr!=NULL) fprintf(pfErr,"Configuration keyword(%s) and/or value(%s) is empty\n", pcKyw, pcVal);
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
      if (strxcmp(psHdl->isCas,psEnt->pcKyw,pcKyw,0,0,FALSE)==0) return(psEnt->pcVal);
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
            } else pcAdd="not verified";
         } else pcAdd="not verified";
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
   TsCnfHdl*                     psHdl,
   FILE*                         pfErr,
   const char*                   pcPre)
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
            pfFil=fopen(psHdl->pcFil,filemode("w"));
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
      if (pfFil!=NULL) fclose(pfFil);
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
   void*                         pvF2S,
   tpfF2S                        pfF2S,
   void**                        ppClp)
{
   int                           siErr=0;
   void*                         pvHdl;
   FILE*                         pfTmp;
   TsClpError                    stErr;

   pfTmp=fopen_tmp(FALSE);

   pvHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,
                   psTab,pvDat,pfTmp,pfTmp,NULL,NULL,NULL,NULL,
                   pcDep,pcOpt,pcEnt,&stErr,pvF2S,pfF2S);
   if (pvHdl==NULL) {
      snprintf(pcErr,uiErr,"CTX-MESSAGE : Open of string parser for command '%s' failed\n",pcCmd);
      if (pfTmp!=NULL) {rewind(pfTmp); size_t l=strlen(pcErr); size_t r=fread(pcErr+l,1,uiErr-(l+1),pfTmp); fclose_tmp(pfTmp); pcErr[l+r]=0x00;}
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
      if (pfTmp!=NULL) {rewind(pfTmp); size_t l=strlen(pcErr); size_t r=fread(pcErr+l,1,uiErr-(l+1),pfTmp); fclose_tmp(pfTmp); pcErr[l+r]=0x00;}
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
      return siErr;
   }
   if (pfTmp!=NULL) fclose_tmp(pfTmp);
   if(ppClp==NULL){
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   }else{
      vdClpClose(pvHdl,CLPCLS_MTD_KEP);
      *ppClp=pvHdl;
   }
   return 0;
}

/**********************************************************************/

