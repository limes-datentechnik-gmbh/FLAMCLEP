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
#include <errno.h>
#ifdef __UNIX__
#  include <dlfcn.h>
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
 * 1.2.74: Support no run reason code (siNoR)
 * 1.2.75: Add black box (handle) for all call back functions
 * 1.2.76: Support additional authorization for CLP path
 * 1.2.77: Use type of function and not type of pointer to function (usable for pragma's)
 * 1.2.78: Support free defined appendix
 * 1.2.79: Support new more flexible table based documentation generation
 * 1.2.80: Support optional built-in function HTMLDOC
 */
#define CLE_VSN_STR       "1.2.80"
#define CLE_VSN_MAJOR      1
#define CLE_VSN_MINOR        2
#define CLE_VSN_REVISION       80

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
   void*                         pvGbl,
   TfIni*                        pfIni,
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
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf);

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
   const char*                   pcDpa,
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
   const char*                   pcSyn,
   const char*                   pcMan,
   const int                     isMan,
   const int                     isPat,
   const int                     isNbr,
   const int                     isIdx);

static void vdPrnStaticSyntax(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
   const char*                   pcDep,
   const char*                   pcSep,
   const char*                   pcDpa);

static void vdPrnStaticHelp(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
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
   FILE*                         pfErr,
   const char*                   pcDep,
   const char*                   pcFct,
   int                           argc,
   char*                         argv[],
   char**                        ppFil,
   char**                        ppCmd,
   void*                         pvGbl,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
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

static int siClpFile2String(void* gbl, void* hdl, const char* filename, char** buf, int* bufsize, char* errmsg, const int msgsiz) {
   (void)gbl;
   char* pcFil=dcpmapfil(filename);
   if (pcFil==NULL) return(-1);
   int siErr=file2str(hdl, pcFil, buf, bufsize, errmsg, msgsiz);
   free(pcFil);
   return(siErr);
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
      if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for %s_CONFIG_FILE  failed\n",pcPgu);
      return(NULL);
   }
   m=GETENV(pcCnf);

   if (m==NULL) {
   #ifdef __ZOS__
      {
         int   i,j;
         char  acUsr[16];
         srprintf(&pcFil,&szFil,strlen(pcPgu)+8,"'%s.",userid(sizeof(acUsr),acUsr));
         if (pcFil==NULL) {
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
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
            if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
            free(pcCnf);
            return(NULL);
         }
         FILE* pfTmp=fopen_nowarn(pcFil,"r");
         if (pfTmp==NULL) {
            srprintf(&pcFil,&szFil,strlen(pcHom)+strlen(pcPgl),"%s.%s.config",pcHom,pcPgl);
            if (pcFil==NULL) {
               if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for default configuration file name failed\n");
               free(pcCnf);
               return(NULL);
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
            free(pcCnf);
            return(NULL);
         }
         if (pfOut!=NULL) fprintf(pfOut,"Use default configuration file (%s) in working directory\n",pcFil);
      }
   #endif
   } else {
      srprintf(&pcFil,&szFil,strlen(m),"%s",m);
      if (pcFil==NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Memory allocation for configuration file name (%s) from environment variable failed\n",pcCnf);
         free(pcCnf);
         return(NULL);
      }
      if (pfOut!=NULL) fprintf(pfOut,"Using configuration file (%s) defined by environment variable (%s)\n",pcFil,pcCnf);
   }

   psCnf=psCnfOpn(pfErr,isCas,pcPgm,pcFil);
   free(pcFil);
   free(pcCnf);
   return(psCnf);
}

/**********************************************************************/

static int siPrintChapter(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isNbr, const int isIdx) {
   if (psDoc->pcHdl!=NULL && *psDoc->pcHdl) {
      if (psDoc->pcKyw!=NULL && *psDoc->pcKyw) {
         efprintf(pfDoc,"[%s]\n",psDoc->pcKyw);
      }
      if (psDoc->pcAnc!=NULL && *psDoc->pcAnc) {
         efprintf(pfDoc,"[[%s]]\n",psDoc->pcAnc);
      }
      for (unsigned int i=0;i<psDoc->uiLev;i++) {
         efprintf(pfDoc,"=");
      }
      if (isNbr && psDoc->pcNum!=NULL && *psDoc->pcNum) efprintf(pfDoc," %s",psDoc->pcNum);
      efprintf(pfDoc," %s\n\n",psDoc->pcHdl);
      if (isIdx && psDoc->pcInd!=NULL && *psDoc->pcInd) {
         const char* pcHlp;
         const char* pcInd=psDoc->pcInd;
         for (pcHlp=strchr(pcInd,'\n');pcHlp!=NULL;pcHlp=strchr(pcInd,'\n')) {
            efprintf(pfDoc,"indexterm:[%.*s]\n",(int)(pcHlp-pcInd),pcInd);
            pcInd=pcHlp+1;
         }
         efprintf(pfDoc,"indexterm:[%s]\n\n",pcInd);
      }
      if (psDoc->pcMan!=NULL && *psDoc->pcMan) {
         fprintm(pfDoc,pcOwn,pcPgm,psDoc->pcMan,2);
      }
      return(CLERTC_OK);
   } else {
      if (pfErr!=NULL) fprintf(pfErr,"No head line for chapter defined\n");
      return(CLERTC_ITF);
   }
}

extern int siClePrintCover(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isNbr, const int isIdx) {
   if (psDoc->uiLev!=1) {
      if (pfErr!=NULL) fprintf(pfErr,"The level (%u) for the cover page must be 1\n",psDoc->uiLev);
      return(CLERTC_ITF);
   }
   efprintf(pfDoc,":doctype: book\n\n");
   return(siPrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx));
}

extern int siClePrintChapter(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isNbr, const int isIdx) {
   if (psDoc->uiLev<2 || psDoc->uiLev>5) {
      if (pfErr!=NULL) fprintf(pfErr,"The level (%u) for a chapter must be between 2 and 6\n",psDoc->uiLev);
      return(CLERTC_ITF);
   }
   return(siPrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx));
}

extern int siClePrintPgmSynopsis(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcHlp, const int isPat, const int isNbr, const int isIdx) {
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   efprintf(pfDoc,"-----------------------------------------------------------------------\n");
   efprintf(pfDoc,"HELP:   %s\n",pcHlp);
   if (isPat)
   efprintf(pfDoc,"PATH:   %s\n",pcOwn);
   efprintf(pfDoc,"TYPE:   PROGRAM\n");
   efprintf(pfDoc,"SYNTAX: > %s COMMAND/FUNCTION ...\n",pcPgm);
   efprintf(pfDoc,"-----------------------------------------------------------------------\n\n");
   return(CLERTC_OK);
}

extern int siClePrintPgmSyntax(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, const char* pcOwn, const char* pcPgm, const char* pcDep, const char* pcOpt, const char* pcDpa, const int isNbr, const int isIdx) {
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Syntax for program '%s':\n",pcPgm);
   vdPrnStaticSyntax(pfDoc,psTab,pcPgm,pcDep,pcOpt,pcDpa);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return(CLERTC_OK);
}
extern int siClePrintPgmHelp(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, const char* pcOwn, const char* pcPgm, const char* pcDep, const int isNbr, const int isIdx) {
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Help for program '%s':\n",pcPgm);
   vdPrnStaticHelp(pfDoc,psTab,pcPgm,pcDep);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return(CLERTC_OK);
}

extern int siClePrintBuiltIn(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isPat, const int isNbr, const int isIdx) {
   int   j=1;
   char  acNum[64];
   int   siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"SYNTAX"  ,HLP_CLE_BUILTIN_SYNTAX  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_SYNTAX  ,MAN_CLE_BUILTIN_SYNTAX  ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"HELP"    ,HLP_CLE_BUILTIN_HELP    ,pcOwn,pcPgm,SYN_CLE_BUILTIN_HELP    ,MAN_CLE_BUILTIN_HELP    ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"MANPAGE" ,HLP_CLE_BUILTIN_MANPAGE ,pcOwn,pcPgm,SYN_CLE_BUILTIN_MANPAGE ,MAN_CLE_BUILTIN_MANPAGE ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"GENDOCU" ,HLP_CLE_BUILTIN_GENDOCU ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GENDOCU ,MAN_CLE_BUILTIN_GENDOCU ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"GENPROP" ,HLP_CLE_BUILTIN_GENPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GENPROP ,MAN_CLE_BUILTIN_GENPROP ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"SETPROP" ,HLP_CLE_BUILTIN_SETPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_SETPROP ,MAN_CLE_BUILTIN_SETPROP ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"CHGPROP" ,HLP_CLE_BUILTIN_CHGPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_CHGPROP ,MAN_CLE_BUILTIN_CHGPROP ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"DELPROP" ,HLP_CLE_BUILTIN_DELPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_DELPROP ,MAN_CLE_BUILTIN_DELPROP ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"GETPROP" ,HLP_CLE_BUILTIN_GETPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GETPROP ,MAN_CLE_BUILTIN_GETPROP ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"SETOWNER",HLP_CLE_BUILTIN_SETOWNER,pcOwn,pcPgm,SYN_CLE_BUILTIN_SETOWNER,MAN_CLE_BUILTIN_SETOWNER,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"GETOWNER",HLP_CLE_BUILTIN_GETOWNER,pcOwn,pcPgm,SYN_CLE_BUILTIN_GETOWNER,MAN_CLE_BUILTIN_GETOWNER,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"SETENV"  ,HLP_CLE_BUILTIN_SETENV  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_SETENV  ,MAN_CLE_BUILTIN_SETENV  ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"GETENV"  ,HLP_CLE_BUILTIN_GETENV  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GETENV  ,MAN_CLE_BUILTIN_GETENV  ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"DELENV"  ,HLP_CLE_BUILTIN_DELENV  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_DELENV  ,MAN_CLE_BUILTIN_DELENV  ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"TRACE"   ,HLP_CLE_BUILTIN_TRACE   ,pcOwn,pcPgm,SYN_CLE_BUILTIN_TRACE   ,MAN_CLE_BUILTIN_TRACE   ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"CONFIG"  ,HLP_CLE_BUILTIN_CONFIG  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_CONFIG  ,MAN_CLE_BUILTIN_CONFIG  ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"GRAMMAR" ,HLP_CLE_BUILTIN_GRAMMAR ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GRAMMAR ,MAN_CLE_BUILTIN_GRAMMAR ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"LEXEM"   ,HLP_CLE_BUILTIN_LEXEM   ,pcOwn,pcPgm,SYN_CLE_BUILTIN_LEXEM   ,MAN_CLE_BUILTIN_LEXEM   ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"LICENSE" ,HLP_CLE_BUILTIN_LICENSE ,pcOwn,pcPgm,SYN_CLE_BUILTIN_LICENSE ,MAN_CLE_BUILTIN_LICENSE ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"VERSION" ,HLP_CLE_BUILTIN_VERSION ,pcOwn,pcPgm,SYN_CLE_BUILTIN_VERSION ,MAN_CLE_BUILTIN_VERSION ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"ABOUT"   ,HLP_CLE_BUILTIN_ABOUT   ,pcOwn,pcPgm,SYN_CLE_BUILTIN_ABOUT   ,MAN_CLE_BUILTIN_ABOUT   ,FALSE,isPat,isNbr,isIdx);
   if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
   vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"ERRORS"  ,HLP_CLE_BUILTIN_ERRORS  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_ERRORS  ,MAN_CLE_BUILTIN_ERRORS  ,FALSE,isPat,isNbr,isIdx);
// if (psDoc->pcNum!=NULL && *psDoc->pcNum) snprintf(acNum,sizeof(acNum),"%s%d.",psDoc->pcNum,j); else snprintf(acNum,sizeof(acNum),"%d.",j); j++;
// vdCleManFunction(pfDoc,psDoc->uiLev+1,S_TLD,acNum,"HTMLDOC" ,HLP_CLE_BUILTIN_HTMLDOC ,pcOwn,pcPgm,SYN_CLE_BUILTIN_HTMLDOC ,MAN_CLE_BUILTIN_HTMLDOC ,FALSE,isPat,isNbr,isIdx);
   return(CLERTC_OK);
}

extern int siClePrintLexem(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isPfl, const int isRpl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdx) {
   void* pvHdl=NULL;
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   siErr=siCleSimpleInit(NULL,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
   if (siErr) return(siErr);
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Lexemes (regular expressions) for argument list or parameter file\n");
   int siRtc=siClpLexem(pvHdl,pfDoc);
   vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return((siRtc)?CLERTC_SYN:CLERTC_OK);
}

extern int siClePrintGrammar(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isPfl, const int isRpl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdx) {
   void* pvHdl=NULL;
   int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   siErr=siCleSimpleInit(NULL,pfErr,isPfl,isRpl,pcDep,pcOpt,pcEnt,&pvHdl);
   if (siErr) return(siErr);
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"Grammar for argument list, parameter file or property file\n");
   int siRtc=siClpGrammar(pvHdl,pfDoc);
   vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   return((siRtc)?CLERTC_SYN:CLERTC_OK);
}

extern int siClePrintPreformatedText(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcTxt, const int isNbr, const int isIdx) {
   if (pcTxt!=NULL && *pcTxt) {
      int siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
      if (siErr) return(siErr);
      efprintf(pfDoc,"------------------------------------------------------------------------\n");
      efprintf(pfDoc,"%s",pcTxt);
      efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
      return(CLERTC_OK);
   } else {
      if (pfErr!=NULL) fprintf(pfErr,"No pre-formatted text provided but print requested (version, about)\n");
      return(CLERTC_ITF);
   }
}

extern int siClePrintPropRemain(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, void* pvCnf, const char* pcOwn, const char* pcPgm,
      const int isCas, const int isPfl, const int isRpl, const int siMkl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdx) {
   int         isCnf=FALSE;
   TsCnfHdl*   psCnf=(TsCnfHdl*)pvCnf;
   int         siRtc=0;
   int         siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   if (psCnf==NULL) { //TODO: muss weg
      int         i,l=strlen(pcPgm);
      char        acPgu[l+1];
      char        acPgl[l+1];
      const char* pcHom=dhomedir(TRUE);
      for (i=0;i<l;i++) {
         acPgu[i]=toupper(pcPgm[i]);
      }
      acPgu[i]=0x0;
      for (i=0;i<l;i++) {
         acPgl[i]=tolower(pcPgm[i]);
      }
      acPgl[i]=0x0;
      psCnf=psOpenConfig(NULL,pfErr,pcHom,pcPgm,acPgu,acPgl,isCas);
      SAFE_FREE(pcHom);
      if (psCnf==NULL) {
         return(CLERTC_CFG);
      }
      isCnf=TRUE;
   }
   for (int j=0;siRtc==0 && psTab[j].pcKyw!=NULL;j++) {
      void* pvHdl=NULL;
      siErr=siClePropertyInit(NULL,psTab[j].pfIni,psTab[j].pvClp,pcOwn,pcPgm,psTab[j].pcKyw,psTab[j].pcMan,psTab[j].pcHlp,
                              psTab[j].piOid,psTab[j].psTab,isCas,isPfl,isRpl,siMkl,NULL,pfErr,NULL,
                              pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,NULL,NULL,siClpFile2String,NULL,NULL);
      if (siErr) {
         if (isCnf) vdCnfCls(psCnf);
         return(siErr);
      }
      siRtc=siClpProperties(pvHdl,CLPPRO_MTD_DOC,10,psTab[j].pcKyw,pfDoc);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   }
   if (isCnf) vdCnfCls(psCnf);
   return((siRtc)?CLERTC_SYN:CLERTC_OK);
}

extern int siClePrintPropDefaults(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, void* pvCnf, const char* pcOwn, const char* pcPgm,
      const int isCas, const int isPfl, const int isRpl, const int siMkl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr, const int isIdx) {
   int         isCnf=FALSE;
   TsCnfHdl*   psCnf=(TsCnfHdl*)pvCnf;
   int         siRtc=0;
   int         siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
   if (siErr) return(siErr);
   if (psCnf==NULL) {
      int         i,l=strlen(pcPgm);
      char        acPgu[l+1];
      char        acPgl[l+1];
      const char* pcHom=dhomedir(TRUE);
      for (i=0;i<l;i++) {
         acPgu[i]=toupper(pcPgm[i]);
      }
      acPgu[i]=0x0;
      for (i=0;i<l;i++) {
         acPgl[i]=tolower(pcPgm[i]);
      }
      acPgl[i]=0x0;
      psCnf=psOpenConfig(NULL,NULL,pcHom,pcPgm,acPgu,acPgl,isCas);
      SAFE_FREE(pcHom);
      if (psCnf==NULL) {
         return(CLERTC_CFG);
      }
      isCnf=TRUE;
   }
   efprintf(pfDoc,"------------------------------------------------------------------------\n");
   efprintf(pfDoc,"\n# Property file for: %s.%s #\n",pcOwn,pcPgm);
   efprintf(pfDoc,"%s",HLP_CLE_PROPFIL);
   for (int j=0;siRtc==0 && psTab[j].pcKyw!=NULL;j++) {
      void* pvHdl=NULL;
      siErr=siClePropertyInit(NULL,psTab[j].pfIni,psTab[j].pvClp,pcOwn,pcPgm,psTab[j].pcKyw,psTab[j].pcMan,psTab[j].pcHlp,
            psTab[j].piOid,psTab[j].psTab,isCas,isPfl,isRpl,siMkl,NULL,pfErr,NULL,
            pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,NULL,NULL,siClpFile2String,NULL,NULL);
      if (siErr) {
         if (isCnf) vdCnfCls(psCnf);
         return(siErr);
      }
      siRtc=siClpProperties(pvHdl,CLPPRO_MTD_SET,10,psTab[j].pcKyw,pfDoc);
      vdClpClose(pvHdl,CLPCLS_MTD_ALL);
   }
   efprintf(pfDoc,"------------------------------------------------------------------------\n\n");
   if (isCnf) vdCnfCls(psCnf);
   return((siRtc)?CLERTC_SYN:CLERTC_OK);
}

extern int siClePrintReasonCodes(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, TfMsg* pfMsg, const int isNbr, const int isIdx) {
   if (pfMsg!=NULL) {
      int         r;
      const char* m;
      int         siErr=siClePrintChapter(pfErr,pfDoc,psDoc,pcOwn,pcPgm,isNbr,isIdx);
      if (siErr) return(siErr);
      for (r=1,m=pfMsg(r);m!=NULL;r++,m=pfMsg(r)) {
         if (*m) efprintf(pfDoc,"* %d - %s\n",r,m);
      }
      efprintf(pfDoc,"\n");
      return(CLERTC_OK);
   } else {
      if (pfErr!=NULL) fprintf(pfErr,"The pointer to the message function to map the reason codes is NULL but print was requested\n");
      return(CLERTC_ITF);
   }
}

/***********************************************************************/

static int siClePrintPage(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleDocPar* psPar, const TsCleCommand* psCmd) {
   switch (psDoc->uiTyp) {
      case CLE_DOCTYP_COVER:        return(siClePrintCover(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_CHAPTER:      return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_BUILTIN:      return(siClePrintBuiltIn(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isPat,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_PROGRAM:      return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_PGMSYNOPSIS:  return(siClePrintPgmSynopsis(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcHlp,psPar->isPat,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_PGMSYNTAX:    return(siClePrintPgmSyntax(pfErr,pfDoc,psDoc,psCmd,psPar->pcOwn,psPar->pcPgm,psPar->pcDep,psPar->pcOpt,psPar->pcDpa,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_PGMHELP:      return(siClePrintPgmHelp(pfErr,pfDoc,psDoc,psCmd,psPar->pcOwn,psPar->pcPgm,psPar->pcDep,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_COMMANDS:     return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_OTHERCLP:     return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_LEXEM:        return(siClePrintLexem(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isPfl,psPar->isRpl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_GRAMMAR:      return(siClePrintGrammar(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isPfl,psPar->isRpl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_PROPREMAIN:   return(siClePrintPropRemain(pfErr,pfDoc,psDoc,psCmd,psPar->pvCnf,psPar->pcOwn,psPar->pcPgm,
            psPar->isCas,psPar->isPfl,psPar->isRpl,psPar->siMkl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_PROPDEFAULTS: return(siClePrintPropDefaults(pfErr,pfDoc,psDoc,psCmd,NULL,psPar->pcOwn,psPar->pcPgm,
            psPar->isCas,psPar->isPfl,psPar->isRpl,psPar->siMkl,psPar->pcDep,psPar->pcOpt,psPar->pcEnt,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_SPECIALCODES: return(siClePrintChapter(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_REASONCODES:  return(siClePrintReasonCodes(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pfMsg,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_VERSION:      return(siClePrintPreformatedText(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcVsn,psPar->isNbr,psPar->isIdx));
      case CLE_DOCTYP_ABOUT:        return(siClePrintPreformatedText(pfErr,pfDoc,psDoc,psPar->pcOwn,psPar->pcPgm,psPar->pcAbo,psPar->isNbr,psPar->isIdx));
      default:
         if (pfErr!=NULL) fprintf(pfErr,"Documentation type (%u) not supported\n",psDoc->uiTyp);
         return(CLERTC_TAB);
   }
}

/***********************************************************************/

static void* pfLoadHtmlDoc(TfCleHtmlDoc** ppHtmlDoc) {
#ifdef __UNIX__
      void* pvHtmlDoc=dlopen("libhtmldoc.so.1",RTLD_LAZY);
      if (pvHtmlDoc==NULL) {
         *ppHtmlDoc=NULL;
         return(NULL);
      }
      *ppHtmlDoc=dlsym(pvHtmlDoc, "genHtmlDoc");
      if (*ppHtmlDoc==NULL) {
         dlclose(pvHtmlDoc);
         return(NULL);
      }
      // cppcheck-suppress resourceLeak
      return(pvHtmlDoc);
#else
      *ppHtmlDoc=NULL;
      return(NULL);
#endif
}

static void vdFreeHtmlDoc(void** ppLib) {
#ifdef __UNIX__
   if (ppLib!=NULL && *ppLib!=NULL) {
      dlclose(*ppLib);
      *ppLib=NULL;
   }
#else
   if (ppLib!=NULL && *ppLib!=NULL) {
      *ppLib=NULL;
   }
#endif
}

/***********************************************************************/

extern int siCleExecute(
   void*                         pvGbl,
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
   const char*                   pcDef,
   TfMsg*                        pfMsg,
   const TsCleOtherClp*          psOth,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf,
   const char*                   pcDpa,
   const int                     siNoR,
   const TsCleDoc*               psDoc)
{
   int                           i,j,l,siErr,siDep,siCnt;
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

   if (pfF2S==NULL) {
      pvF2S=NULL;
      pfF2S=siClpFile2String;
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

   psCnf=psOpenConfig(pfOut,pfErr,pcHom,pcPgm,pcPgu,pcPgl,isCas);
   if (psCnf==NULL) ERROR(CLERTC_CFG,NULL);

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
            pfTrh=fopen_hfq(pcFil,"w");
            if (pfTrh==NULL) {
               if (pfErr!=NULL) fprintf(pfErr,"Open of trace file (\"%s\",\"%s\") failed\n",pcFil,"w");
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
            vdPrnStaticSyntax(pfErr,psTab,argv[0],pcDep,pcOpt,pcDpa);
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
         const char* pcScc=NULL;
         efprintf(pfOut,"\n");
         efprintf(pfOut,"Return/condition/exit codes of the executable\n");
         efprintf(pfOut,"---------------------------------------------\n\n");
         fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_APPENDIX_RETURNCODES,1);
         if (psDoc!=NULL) {
            for (i=0;psDoc[i].uiTyp;i++) {
               if (psDoc[i].uiTyp==CLE_DOCTYP_SPECIALCODES) {
                  pcScc=psDoc[i].pcMan;
               }
            }
            if (pcScc!=NULL && *pcScc) {
               efprintf(pfOut,"Special condition codes\n");
               efprintf(pfOut,"~~~~~~~~~~~~~~~~~~~~~~~\n\n");
               fprintm(pfOut,pcOwn,pcPgm,pcScc,1);
            }
         }
         if (pfMsg!=NULL) {
            efprintf(pfOut,"Reason codes of the different commands\n");
            efprintf(pfOut,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
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
         vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt,pcDpa);
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
               siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
                  // TODO: don't use stack allocation because strings can be larger than stack size (user input)
                  char acPat[strlen(pcDef)+strlen(argv[2]+2)];
                  siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
         vdPrnStaticHelp(pfOut,psTab,argv[0],pcDep);
         ERROR(CLERTC_OK,NULL);
      } else if (argc>=3) {
         if (argc==3) {
            if (strxcmp(isCas,argv[2],"MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--MAN",0,0,FALSE)==0) {
               if (psDoc!=NULL) {
                  const char* pcMan=NULL;
                  for (i=0;psDoc[i].uiTyp;i++) {
                     if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
                        pcMan=psDoc[i].pcMan;
                     }
                  }
                  if (pcMan!=NULL && *pcMan) {
                     fprintf(pfOut,"Help for program '%s':\n",pcPgm);
                     fprintm(pfOut,pcOwn,pcPgm,pcMan,1);
                     ERROR(CLERTC_OK,NULL);
                  } else {
                     fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
                     fprintf(pfErr,"CLE_DOCTYP_PROGRAM not found in documentation table\n");
                     ERROR(CLERTC_TAB,NULL);
                  }
               } else {
                  fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
                  fprintf(pfErr,"No table for documentation generation given\n");
                  ERROR(CLERTC_TAB,NULL);
               }
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
               siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
                  // TODO: don't use stack allocation because strings can be larger than stack size (user input)
                  char acPat[strlen(psTab[i].pcKyw)+strlen(argv[2]+2)];
                  siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
            if (psDoc!=NULL) {
               const char* pcMan=NULL;
               for (i=0;psDoc[i].uiTyp;i++) {
                  if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
                     pcMan=psDoc[i].pcMan;
                  }
               }
               if (pcMan!=NULL && *pcMan) {
                  fprintf(pfOut,"Manual page for program '%s':\n\n",pcPgm);
                  vdCleManProgram(pfOut,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,pcDpa,FALSE,TRUE);
                  ERROR(CLERTC_OK,NULL);
               } else {
                  fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
                  fprintf(pfErr,"CLE_DOCTYP_PROGRAM not found in documentation table\n");
                  ERROR(CLERTC_TAB,NULL);
               }
            } else {
               fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
               fprintf(pfErr,"No table for documentation generation given\n");
               ERROR(CLERTC_TAB,NULL);
            }
         }
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
//          szFil=strlen(pcFil)+1; not used
            pfDoc=fopen_hfq(pcFil,"w");
            if (pfDoc==NULL) {
               fprintf(pfErr,"Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",pcFil,"w",errno,strerror(errno));
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
            if (psDoc!=NULL) {
               const char* pcMan=NULL;
               for (i=0;psDoc[i].uiTyp;i++) {
                  if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
                     pcMan=psDoc[i].pcMan;
                  }
               }
               if (pcMan!=NULL && *pcMan) {
                  isAll=TRUE;
                  if (isMan==FALSE) fprintf(pfOut,"Manual page for program '%s':\n\n",pcPgm);
                  vdCleManProgram(pfDoc,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,pcDpa,isMan,TRUE);
                  if (isMan==TRUE) fprintf(pfOut,"Manual page for program '%s' successfully written to file (%s)\n",pcPgm,pcFil);
               } else {
                  fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
                  fprintf(pfErr,"CLE_DOCTYP_PROGRAM not found in documentation table\n");
                  ERROR(CLERTC_TAB,NULL);
               }
            } else {
               fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
               fprintf(pfErr,"No table for documentation generation given\n");
               ERROR(CLERTC_TAB,NULL);
            }
         }
         if (strxcmp(isCas,pcCmd,"SYNTAX",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SYNTAX':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.1" ,"SYNTAX"  ,HLP_CLE_BUILTIN_SYNTAX  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_SYNTAX,MAN_CLE_BUILTIN_SYNTAX,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SYNTAX' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"HELP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'HELP':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.2" ,"HELP"    ,HLP_CLE_BUILTIN_HELP    ,pcOwn,pcPgm,SYN_CLE_BUILTIN_HELP,MAN_CLE_BUILTIN_HELP,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'HELP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"MANPAGE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'MANPAGE':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.3" ,"MANPAGE" ,HLP_CLE_BUILTIN_MANPAGE ,pcOwn,pcPgm,SYN_CLE_BUILTIN_MANPAGE,MAN_CLE_BUILTIN_MANPAGE,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'MANPAGE' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GENDOCU",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GENDOCU':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.4" ,"GENDOCU" ,HLP_CLE_BUILTIN_GENDOCU ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GENDOCU,MAN_CLE_BUILTIN_GENDOCU,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GENDOCU' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GENPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GENPROP':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.5" ,"GENPROP" ,HLP_CLE_BUILTIN_GENPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GENPROP,MAN_CLE_BUILTIN_GENPROP,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GENPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"SETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETPROP':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.6" ,"SETPROP" ,HLP_CLE_BUILTIN_SETPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_SETPROP,MAN_CLE_BUILTIN_SETPROP,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"CHGPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'CHGPROP':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.7" ,"CHGPROP" ,HLP_CLE_BUILTIN_CHGPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_CHGPROP,MAN_CLE_BUILTIN_CHGPROP,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'CHGPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"DELPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'DELPROP':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.8" ,"DELPROP" ,HLP_CLE_BUILTIN_DELPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_DELPROP,MAN_CLE_BUILTIN_DELPROP,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'DELPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETPROP':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.9" ,"GETPROP" ,HLP_CLE_BUILTIN_GETPROP ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GETPROP,MAN_CLE_BUILTIN_GETPROP,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETPROP' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"SETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETOWNER':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.10" ,"SETOWNER",HLP_CLE_BUILTIN_SETOWNER,pcOwn,pcPgm,SYN_CLE_BUILTIN_SETOWNER,MAN_CLE_BUILTIN_SETOWNER,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETOWNER' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETOWNER':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.11","GETOWNER",HLP_CLE_BUILTIN_GETOWNER,pcOwn,pcPgm,SYN_CLE_BUILTIN_GETOWNER,MAN_CLE_BUILTIN_GETOWNER,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETOWNER' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"SETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETENV':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.12" ,"SETENV",HLP_CLE_BUILTIN_SETENV,pcOwn,pcPgm,SYN_CLE_BUILTIN_SETENV,MAN_CLE_BUILTIN_SETENV,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETENV' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETENV':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.13" ,"GETENV",HLP_CLE_BUILTIN_GETENV,pcOwn,pcPgm,SYN_CLE_BUILTIN_GETENV,MAN_CLE_BUILTIN_GETENV,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETENV' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"DELENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'DELENV':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.14" ,"DELENV",HLP_CLE_BUILTIN_DELENV,pcOwn,pcPgm,SYN_CLE_BUILTIN_DELENV,MAN_CLE_BUILTIN_DELENV,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'DELENV' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"TRACE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'TRACE':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.15","TRACE"   ,HLP_CLE_BUILTIN_TRACE   ,pcOwn,pcPgm,SYN_CLE_BUILTIN_TRACE,MAN_CLE_BUILTIN_TRACE,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'TRACE' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"CONFIG",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'CONFIG':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.16","CONFIG"  ,HLP_CLE_BUILTIN_CONFIG  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_CONFIG,MAN_CLE_BUILTIN_CONFIG,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'CONFIG' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"GRAMMAR",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GRAMMAR':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.17","GRAMMAR" ,HLP_CLE_BUILTIN_GRAMMAR ,pcOwn,pcPgm,SYN_CLE_BUILTIN_GRAMMAR,MAN_CLE_BUILTIN_GRAMMAR,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GRAMMAR' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"LEXEM",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'LEXEM':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.18","LEXEM"   ,HLP_CLE_BUILTIN_LEXEM   ,pcOwn,pcPgm,SYN_CLE_BUILTIN_LEXEM,MAN_CLE_BUILTIN_LEXEM,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'LEXEM' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"LICENSE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'LICENSE':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.19","LICENSE" ,HLP_CLE_BUILTIN_LICENSE ,pcOwn,pcPgm,SYN_CLE_BUILTIN_LICENSE,MAN_CLE_BUILTIN_LICENSE,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'LICENSE' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"VERSION",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'VERSION':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.20","VERSION" ,HLP_CLE_BUILTIN_VERSION ,pcOwn,pcPgm,SYN_CLE_BUILTIN_VERSION,MAN_CLE_BUILTIN_VERSION,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'VERSION' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"ABOUT",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'ABOUT':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.21","ABOUT"   ,HLP_CLE_BUILTIN_ABOUT   ,pcOwn,pcPgm,SYN_CLE_BUILTIN_ABOUT,MAN_CLE_BUILTIN_ABOUT,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'ABOUT' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"ERRORS",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'ERRORS':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.22","ERRORS"  ,HLP_CLE_BUILTIN_ERRORS  ,pcOwn,pcPgm,SYN_CLE_BUILTIN_ERRORS,MAN_CLE_BUILTIN_ERRORS,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'ERRORS' successfully written to file (%s)\n",pcFil);
            if (isAll==FALSE) ERROR(CLERTC_OK,NULL);
         }
         if (strxcmp(isCas,pcCmd,"HTMLDOC",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'HTMLDOC':\n\n");
            vdCleManFunction(pfDoc,0,S_TLD,"4.23" ,"HTMLDOC" ,HLP_CLE_BUILTIN_HTMLDOC ,pcOwn,pcPgm,SYN_CLE_BUILTIN_HTMLDOC,MAN_CLE_BUILTIN_HTMLDOC,isMan,TRUE,TRUE,FALSE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'HTMLDOC' successfully written to file (%s)\n",pcFil);
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
               siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
                  // TODO: don't use stack allocation because strings can be larger than stack size (user input)
                  char acPat[strlen(pcDef)+strlen(pcCmd+2)];
                  siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
         pfDoc=fopen_hfq(pcFil,"w");
         if (pfDoc==NULL) {
            fprintf(pfErr,"Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",pcFil,"w",errno,strerror(errno));
            ERROR(CLERTC_SYS,NULL);
         }
         if (psDoc!=NULL) {
            const char* pcMan=NULL;
            for (i=0;psDoc[i].uiTyp;i++) {
               if (psDoc[i].uiTyp==CLE_DOCTYP_PROGRAM) {
                  pcMan=psDoc[i].pcMan;
               }
            }
            if (pcMan!=NULL && *pcMan) {
               vdCleManProgram(pfDoc,psTab,pcOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,pcDpa,isMan,TRUE);
               if (pfOut!=NULL) fprintf(pfOut,"Manual page for program '%s' successfully written to file (%s)\n",pcPgm,pcFil);
               ERROR(CLERTC_OK,NULL);
            } else {
               fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
               fprintf(pfErr,"CLE_DOCTYP_PROGRAM not found in documentation table\n");
               ERROR(CLERTC_TAB,NULL);
            }
         } else {
            fprintf(pfErr,"No manual page available for program '%s'\n",pcPgm);
            fprintf(pfErr,"No table for documentation generation given\n");
            ERROR(CLERTC_TAB,NULL);
         }
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
      int                        isNbr=TRUE;
      int                        isLong=TRUE;
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==3 || argc==4 || argc==5) {
         if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"NONBR",0,0,FALSE)==0) {
               isNbr=FALSE;
            } else if(strxcmp(isCas,argv[3],"SHORT",0,0,FALSE)==0) {
               isLong=FALSE;
            } else {
               fprintf(pfErr,"Syntax for built-in function 'GENDOCU' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     efprintf(pfErr,"%s %s GENDOCU %s[.path]=filename [NONBR][SHORT]\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               efprintf(pfErr,"%s %s GENDOCU filename [NONBR][SHORT]\n",pcDep,argv[0]);
               ERROR(CLERTC_CMD,NULL);
            }
         } else if (argc==5) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (argv[4][0]=='-') argv[4]++;
            if (argv[4][0]=='-') argv[4]++;
            if (strxcmp(isCas,argv[3],"NONBR",0,0,FALSE)==0 && strxcmp(isCas,argv[4],"SHORT",0,0,FALSE)==0) {
               isNbr=FALSE;
               isLong=FALSE;
            } else if(strxcmp(isCas,argv[3],"SHORT",0,0,FALSE)==0 && strxcmp(isCas,argv[4],"NONBR",0,0,FALSE)==0) {
               isNbr=FALSE;
               isLong=FALSE;
            } else {
               fprintf(pfErr,"Syntax for built-in function 'GENDOCU' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     efprintf(pfErr,"%s %s GENDOCU %s[.path]=filename [NONBR][SHORT]\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               efprintf(pfErr,"%s %s GENDOCU filename [NONBR][SHORT]\n",pcDep,argv[0]);
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
         pfDoc=fopen_hfq(pcFil,"w");
         if (pfDoc==NULL) {
            fprintf(pfErr,"Open of documentation file (\"%s\",\"%s\") failed (%d - %s)\n",pcFil,"w",errno,strerror(errno));
            ERROR(CLERTC_SYS,NULL);
         }
         if (pcCmd!=NULL) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
                  char acNum[64];
                  siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                         isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
                  if (siErr) ERROR(siErr,NULL);
                  snprintf(acNum,sizeof(acNum),"2.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,pcCmd,NULL,acNum,"COMMAND",TRUE,FALSE,isNbr,FALSE,TRUE,0);
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
                     char acNum[64];
                     // TODO: don't use stack allocation because strings can be larger than stack size (user input)
                     char acPat[strlen(pcDef)+strlen(pcCmd+2)];
                     siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                            isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
                     if (siErr) ERROR(siErr,NULL);
                     snprintf(acNum,sizeof(acNum),"2.%d.",i+1);
                     sprintf(acPat,"%s.%s",pcDef,pcCmd);
                     siErr=siClpDocu(pvHdl,pfDoc,acPat,NULL,acNum,"COMMAND",TRUE,FALSE,isNbr,FALSE,TRUE,0);
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
            if (psDoc!=NULL) {
               for (i=0; psDoc[i].uiTyp; i++) {
                  if (pfOut!=NULL) {
                     fprintf(pfOut,"... print %24s ",pcMapDocTyp(psDoc[i].uiTyp));
                     for (j=0;j<(int)psDoc[i].uiLev;j++) fprintf(pfOut,"=");
                     fprintf(pfOut," %s\n",(psDoc[i].pcHdl!=NULL)?psDoc[i].pcHdl:"(NULL)");
                  }
                  switch (psDoc[i].uiTyp) {
                     case CLE_DOCTYP_COVER:
                        siErr=siClePrintCover(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_CHAPTER:
                        siErr=siClePrintChapter(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_BUILTIN:
                        siErr=siClePrintBuiltIn(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,TRUE,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_PROGRAM:
                        siErr=siClePrintChapter(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_PGMSYNOPSIS:
                        siErr=siClePrintPgmSynopsis(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,pcHlp,TRUE,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_PGMSYNTAX:
                        siErr=siClePrintPgmSyntax(pfErr,pfDoc,psDoc+i,psTab,pcOwn,pcPgm,pcDep,pcOpt,pcDpa,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_PGMHELP:
                        siErr=siClePrintPgmHelp(pfErr,pfDoc,psDoc+i,psTab,pcOwn,pcPgm,pcDep,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_COMMANDS:
                        siErr=siClePrintChapter(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        for (j=0;psTab[j].pcKyw!=NULL;j++) {
                           if (psTab[j].siFlg) {
                              char acNum[64];
                              siErr=siCleCommandInit(pvGbl,psTab[j].pfIni,psTab[j].pvClp,pcOwn,pcPgm,psTab[j].pcKyw,psTab[j].pcMan,psTab[j].pcHlp,psTab[j].piOid,psTab[j].psTab,
                                                     isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
                              if (siErr) ERROR(siErr,NULL);
                              if (psDoc[i].pcNum!=NULL && *psDoc[i].pcNum) {
                                 snprintf(acNum,sizeof(acNum),"%s%d.",psDoc[i].pcNum,j+1);
                              } else {
                                 snprintf(acNum,sizeof(acNum),"%d.",j+1);
                              }
                              siErr=siClpDocu(pvHdl,pfDoc,psTab[j].pcKyw,NULL,acNum,"COMMAND",isLong,FALSE,isNbr,FALSE,TRUE,psDoc[i].uiLev+1);
                              if (siErr<0) {
                                 fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                                 ERROR(CLERTC_SYN,NULL);
                              }
                              vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
                           }
                        }
                        break;
                     case CLE_DOCTYP_OTHERCLP:
                        if (psOth==NULL) {
                           fprintf(pfErr,"The pointer to the list of other CLP strings is NULL but DOCTYP OTHERCLP requested\n");
                           ERROR(CLERTC_ITF,NULL);
                        }
                        siErr=siClePrintChapter(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        for (j=0;psOth[j].pcHdl!=NULL;j++) {
                           char acNum[64];
                           pvHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,psOth[j].pcRot,psOth[j].pcKyw,psOth[j].pcMan,psOth[j].pcHlp,psOth[j].isOvl,
                                           psOth[j].psTab,NULL,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
                           if (pvHdl==NULL) {
                              fprintf(pfErr,"Open of parser for CLP string of appendix '%s' failed\n",psOth[j].pcRot);
                              return(CLERTC_TAB);
                           }
                           if (psDoc[i].pcNum!=NULL && *psDoc[i].pcNum) {
                              snprintf(acNum,sizeof(acNum),"%s%d.",psDoc[i].pcNum,j+1);
                           } else {
                              snprintf(acNum,sizeof(acNum),"%d.",j+1);
                           }
                           siErr=siClpDocu(pvHdl,pfDoc,psOth[j].pcKyw,psOth[j].pcHdl,acNum,"OTHERCLP",isLong,FALSE,isNbr,FALSE,TRUE,psDoc[i].uiLev+1);
                           if (siErr<0) {
                              fprintf(pfErr,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                              ERROR(CLERTC_SYN,NULL);
                           }
                           vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
                        }
                        break;
                     case CLE_DOCTYP_LEXEM:
                        siErr=siClePrintLexem(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isPfl,isRpl,pcDep,pcOpt,pcEnt,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_GRAMMAR:
                        siErr=siClePrintGrammar(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isPfl,isRpl,pcDep,pcOpt,pcEnt,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_PROPREMAIN:
                        siErr=siClePrintPropRemain(pfErr,pfDoc,psDoc+i,psTab,psCnf,pcOwn,pcPgm,isCas,isPfl,isRpl,siMkl,pcDep,pcOpt,pcEnt,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_PROPDEFAULTS:
                        siErr=siClePrintPropDefaults(pfErr,pfDoc,psDoc+i,psTab,psCnf,pcOwn,pcPgm,isCas,isPfl,isRpl,siMkl,pcDep,pcOpt,pcEnt,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_SPECIALCODES:
                        siErr=siClePrintChapter(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_REASONCODES:
                        siErr=siClePrintReasonCodes(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,pfMsg,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_VERSION:
                        siErr=siClePrintPreformatedText(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,pcVsn,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     case CLE_DOCTYP_ABOUT:
                        siErr=siClePrintPreformatedText(pfErr,pfDoc,psDoc+i,pcOwn,pcPgm,pcAbo,isNbr,FALSE);
                        if (siErr) ERROR(siErr,NULL);
                        break;
                     default:
                        fprintf(pfErr,"Documentation type (%u) not supported\n",psDoc[i].uiTyp);
                        ERROR(CLERTC_TAB,NULL);
                  }
               }
            } else {
               fprintf(pfErr,"No table for documentation generation given\n");
               ERROR(CLERTC_TAB,NULL);
            }
            fprintf(pfOut,"Documentation for program '%s' successfully created\n",pcPgm);
            ERROR(CLERTC_OK,NULL);
         }
      }
      fprintf(pfErr,"Syntax for built-in function 'GENDOCU' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfErr,"%s %s GENDOCU %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfErr,"[.path]=filename [NONBR][SHORT]\n");
         }
      }
      efprintf(pfErr,"%s %s GENDOCU filename [NONBR][SHORT]\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD,NULL);
   } else if (strxcmp(isCas,argv[1],"HTMLDOC",0,0,FALSE)==0) {
      const char*       pcSgn=NULL;
      int               isNbr=FALSE;
      TfCleHtmlDoc*     pfHtmlDoc=NULL;
      if (pfOut==NULL) pfOut=pfStd;
      if (pfErr==NULL) pfErr=pfStd;
      if (argc==4) {
         if (argv[2][0]=='-') argv[2]++;
         if (argv[2][0]=='-') argv[2]++;
         if (argv[3][0]=='-') argv[3]++;
         if (argv[3][0]=='-') argv[3]++;
         if (strxcmp(isCas,argv[3],"NUMBERS",0,0,FALSE)==0) {
            isNbr=TRUE;
            pcSgn=strchr(argv[2],'=');
         } else if (strxcmp(isCas,argv[2],"NUMBERS",0,0,FALSE)==0) {
            isNbr=TRUE;
            pcSgn=strchr(argv[3],'=');
         } else {
            fprintf(pfErr,"Syntax for built-in function 'HTMLDOC' not valid\n");
            efprintf(pfErr,"%s %s %s\n",pcDep,argv[0],SYN_CLE_BUILTIN_HTMLDOC);
            ERROR(CLERTC_CMD,NULL);
         }
      } else if (argc==3) {
         if (argv[2][0]=='-') argv[2]++;
         if (argv[2][0]=='-') argv[2]++;
         if (strxcmp(isCas,argv[2],"NUMBERS",0,0,FALSE)==0) {
            isNbr=TRUE;
            pcSgn="=.";
         } else {
            pcSgn=strchr(argv[2],'=');
         }
      } else if (argc==2) {
         pcSgn="=.";
      } else {
         fprintf(pfErr,"Syntax for built-in function 'HTMLDOC' not valid\n");
         efprintf(pfErr,"%s %s %s\n",pcDep,argv[0],SYN_CLE_BUILTIN_HTMLDOC);
         ERROR(CLERTC_CMD,NULL);
      }
      if (pcSgn==NULL) {
         fprintf(pfErr,"Syntax for built-in function 'HTMLDOC' not valid\n");
         efprintf(pfErr,"%s %s %s\n",pcDep,argv[0],SYN_CLE_BUILTIN_HTMLDOC);
         ERROR(CLERTC_CMD,NULL);
      } else {
         char* pcPat=dcpmapfil(pcSgn+1);
         if (pcPat==NULL) {
            fprintf(pfErr,"Allocation of memory for file name (%s) failed\n",pcSgn);
            ERROR(CLERTC_MEM,NULL);
         }

         void* pvLib=pfLoadHtmlDoc(&pfHtmlDoc);
         if (pvLib==NULL || pfHtmlDoc==NULL) {
            fprintf(pfErr,"There is no service provider DLL/SO (libhtmldoc/genHtmlDoc) available for HTML generation\n");
            ERROR(CLERTC_FAT,pcPat);
         }

         TsCleDocPar stDocPar;
         stDocPar.isNbr=isNbr; stDocPar.isPat=FALSE; stDocPar.isIdx=TRUE;
         stDocPar.isCas=isCas; stDocPar.isPfl=isPfl; stDocPar.isRpl=isRpl;
         stDocPar.pcAbo=pcAbo; stDocPar.pcDep=pcDep; stDocPar.pcDpa=pcDpa;
         stDocPar.pcEnt=pcEnt; stDocPar.pcHlp=pcHlp; stDocPar.pcOpt=pcOpt;
         stDocPar.pcOwn=pcOwn; stDocPar.pcPgm=pcPgm; stDocPar.pcVsn=pcVsn;
         stDocPar.pfMsg=pfMsg; stDocPar.pvCnf=psCnf; stDocPar.siMkl=siMkl;
         siErr=pfHtmlDoc(pfOut,pfErr,pcPat,psDoc,psTab,psOth,&stDocPar,siClePrintPage);
         vdFreeHtmlDoc(&pvLib);
         if (siErr) {
            fprintf(pfErr,"Generation of HTML documentation to folder '%s' failed\n",pcPat);
            ERROR(CLERTC_SYS,pcPat);
         } else {
            fprintf(pfErr,"Generation of HTML documentation to folder '%s' was successful\n",pcPat);
            ERROR(CLERTC_OK,pcPat);
         }
      }
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
         pfPro=fopen_hfq(pcFil,"w");
         if (pfPro==NULL) {
            fprintf(pfErr,"Open of property file (\"%s\",\"%s\") failed (%d-%s)\n",pcFil,"w",errno,strerror(errno));
            ERROR(CLERTC_SYS,NULL);
         }
         if (pcCmd==NULL) fprintf(pfPro,"\n%c Property file for: %s.%s %c\n\n",C_HSH,pcOwn,pcPgm,C_HSH);
                    else  fprintf(pfPro,"\n%c Property file for: %s.%s.%s %c\n\n",C_HSH,pcOwn,pcPgm,pcCmd,C_HSH);
         efprintf(pfPro,"%s",HLP_CLE_PROPFIL);

         if (pcCmd==NULL) {
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
                  siErr=siClePropertyInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
               siErr=siCleChangeProperties(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcHom,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,pcPro,
                     psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
               siErr=siCleChangeProperties(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcHom,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,pcPro,
                     psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
            siErr=siClePropertyInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                    psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                    &pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
            if (siErr) ERROR(siErr,NULL);
            vdPrnProperties(pvHdl,psTab[i].pcKyw,10);
            vdClpClose(pvHdl,CLPCLS_MTD_ALL); pvHdl=NULL;
         }
         ERROR(CLERTC_OK,NULL);
      } else if (argc>=3) {
         if (argc==3) {
            siDep=1;
         } else if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"DEFALL",0,0,FALSE)==0) {
               siDep=10;
            } else if (strxcmp(isCas,argv[3],"DEPALL",0,0,FALSE)==0) {
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
               siErr=siClePropertyInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,
                                       psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
               if (siErr) ERROR(siErr,NULL);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Properties for command '%s':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Properties for argument '%s':\n",argv[2]);
               }
               vdPrnProperties(pvHdl,argv[2],siDep);
               ERROR(CLERTC_OK,NULL);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  // TODO: don't use stack allocation because strings can be larger than stack size (user input)
                  char acPat[strlen(pcDef)+strlen(argv[2]+2)];
                  siErr=siClePropertyInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,
                                          psCnf,&pvHdl,NULL,NULL,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
                  if (siErr) ERROR(siErr,NULL);
                  sprintf(acPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Properties for argument '%s':\n",acPat);
                  vdPrnProperties(pvHdl,acPat,siDep);
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
         const char* pcEnv="";
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
            siCnt=siCnfClr(psCnf);
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
               siErr=siCleCommandInit(pvGbl,psTab[i].pfIni,psTab[i].pvClp,pcOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,
                                      isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
               if (siErr) ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               siErr=siCleGetCommand(pfOut,pcDep,psTab[i].pcKyw,argc,argv,&pcFil,&pcCmd,pvGbl,pvF2S,pfF2S,pcDpa);
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
               siErr=psTab[i].pfMap(pvHdl,pfErr,pfTrc,pvGbl,psTab[i].piOid,psTab[i].pvClp,psTab[i].pvPar);
               if (siErr) {
                  if (siErr!=siNoR) {
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
                  psTab[i].pfFin(pfErr,pfTrc,pvGbl,psTab[i].pvPar);
                  ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
               }
               siErr=psTab[i].pfRun(pvHdl,pfErr,pfTrc,pvGbl,pcOwn,pcPgm,pcVsn,pcAbo,pcLic,psTab[i].pcKyw,pcCmd,pcLst,psTab[i].pvPar,&isWrn,&siScc);
               SAFE_FREE(pcCmd); SAFE_FREE(pcLst);
               if (siErr) {
                  if (isWrn&0x00010000) {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' ends with warning (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_WRN,siErr,pcMsg);
                     } else {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' ends with warning (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_WRN,siErr);
                     }
                     psTab[i].pfFin(pfErr,pfTrc,pvGbl,psTab[i].pvPar);
                     siErr=CLERTC_WRN;
                     ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
                  } else {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_RUN,siErr,pcMsg);
                     } else {
                        if (pfErr!=NULL) fprintf(pfErr,"Run of command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_RUN,siErr);
                     }
                     psTab[i].pfFin(pfErr,pfTrc,pvGbl,psTab[i].pvPar);
                     if (siScc>CLERTC_MAX) {
                        siErr=siScc;
                        ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
                     } else {
                        siErr=CLERTC_RUN;
                        ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
                     }
                  }
               }
               siErr=psTab[i].pfFin(pfErr,pfTrc,pvGbl,psTab[i].pvPar);
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
         vdPrnStaticSyntax(pfErr,psTab,argv[0],pcDep,pcOpt,pcDpa);
      }
      siErr=CLERTC_CMD;
      ERROR(((siErr>siMaxCC)?siMaxCC:(siErr<siMinCC)?0:siErr),NULL);
   }
}
#undef ERROR

/* Interne Funktionen *************************************************/

static int siClePropertyInit(
   void*                         pvGbl,
   TfIni*                        pfIni,
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
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf)
{
   int                           siErr;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   char*                         pcPro=NULL;
   char*                         pcFil=NULL;
   int                           siFil=0;

   if (piFil!=NULL) *piFil=0;
   if (ppFil!=NULL) *ppFil=NULL;
   *ppHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,psTab,pvClp,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Open of property parser for command '%s' failed\n",pcCmd);
      return(CLERTC_TAB);
   }
   siErr=pfIni(*ppHdl,pfErr,pfTrc,pvGbl,pcOwn,pcPgm,pvClp);
   if (siErr) {
      const char* pcMsg;
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg);
      } else {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr);
      }
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(CLERTC_INI);
   }
   siErr=siCleGetProperties(pfErr,psCnf,pcOwn,pcPgm,pcCmd,&pcFil,&pcPro,&siFil,pvGbl,pvF2S,pfF2S);
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
   pfPro=fopen_hfq(pcHlp,"w");
   if (pfPro==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Cannot open the property file (\"%s\",\"%s\") for write operation (%d-%s)\n",pcHlp,"w",errno,strerror(errno));
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
   void*                         pvGbl,
   TfIni*                        pfIni,
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
   TfMsg*                        pfMsg,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf)
{
   int                           siErr,siFil=0;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   char*                         pcFil=NULL;
   char*                         pcPro=NULL;

   *ppHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,psTab,pvClp,pfOut,pfErr,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Open of parser for command '%s' failed\n",pcCmd);
      return(CLERTC_TAB);
   }
   siErr=pfIni(*ppHdl,pfErr,pfTrc,pvGbl,pcOwn,pcPgm,pvClp);
   if (siErr) {
      const char* pcMsg;
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg);
      } else {
         if (pfErr!=NULL) fprintf(pfErr,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr);
      }
      vdClpClose(*ppHdl,CLPCLS_MTD_ALL);*ppHdl=NULL;
      return(CLERTC_INI);
   }
   siErr=siCleGetProperties(pfErr,psCnf,pcOwn,pcPgm,pcCmd,&pcFil,&pcPro,&siFil,pvGbl,pvF2S,pfF2S);
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
         {CLPTYP_NUMBER,"XX",NULL,0,1,1,0,0,CLPFLG_NON,NULL,NULL,NULL,"XX",0,0.0,NULL,"NUMBER"},
         {CLPTYP_NON   ,NULL,NULL,0,0,0,0,0,CLPFLG_NON,NULL,NULL,NULL,NULL,0,0.0,NULL,NULL}
   };
   *ppHdl=pvClpOpen(FALSE,isPfl,isRpl,0,"","","","","",FALSE,asTab,"",pfOut,pfErr,NULL,NULL,NULL,NULL,pcDep,pcOpt,pcEnt,NULL,NULL,NULL,NULL,NULL,NULL);
   if (*ppHdl==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Open of command line parser for grammar and lexem print out failed\n");
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

   siErr=siClePropertyInit(pvGbl,pfIni,pvClp,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,
                           piOid,psTab,isCas,isPfl,isRpl,siMkl,pfOut,pfErr,pfTrc,
                           pcDep,pcOpt,pcEnt,psCnf,&pvHdl,&pcFil,&siFil,pfMsg,pvF2S,pfF2S,pvSaf,pfSaf);
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
   const char*                   pcDpa,
   const int                     isMan,
   const int                     isNbr)
{
   int                           i,l;
   if (isMan) {
      for(const char* p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, "(1)\n");
      l=strlen(pcPgm)+3;
      for (i=0;i<l;i++) fprintf(pfOut,"=");
      fprintf(pfOut, "\n");
      fprintf(pfOut, ":doctype: manpage\n\n");
      fprintf(pfOut, "NAME\n");
      fprintf(pfOut, "----\n\n");
      for(const char* p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
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
            for(const char* p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
            fprintf(pfOut,".");
            for(const char* p=psTab[i].pcKyw;*p;p++) fprintf(pfOut,"%c",tolower(*p));
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
      fprintf(pfOut, "SYNTAX: > %s COMMAND/FUNCTION ...\n",pcPgm);
      fprintf(pfOut, "-----------------------------------------------------------------------\n\n");

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
      if (isNbr) {
         fprintf(pfOut,"2.3. SYNTAX\n");
         for (i=0;i<12;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"SYNTAX\n");
         for (i=0;i<6;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_PROGRAM_SYNTAX,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Syntax for program '%s':\n",pcPgm);
      vdPrnStaticSyntax(pfOut,psTab,pcPgm,pcDep,pcSep,pcDpa);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");

      if (isNbr) {
         fprintf(pfOut,"2.4. HELP\n");
         for (i=0;i<9;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"HELP\n");
         for (i=0;i<4;i++) fprintf(pfOut,"%c",C_TLD);
         fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_PROGRAM_HELP,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Help for program '%s':\n",pcPgm);
      vdPrnStaticHelp(pfOut,psTab,pcPgm,pcDep);
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
   const char*                   pcSyn,
   const char*                   pcMan,
   const int                     isMan,
   const int                     isPat,
   const int                     isNbr,
   const int                     isIdx)
{
   int                           i,l;
   if (isMan) {
      for(const char* p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, ".");
      for(const char* p=pcFct;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, "(1)\n");
      l=strlen(pcPgm)+strlen(pcFct)+4;
      for (i=0;i<l;i++) fprintf(pfOut,"=");
      fprintf(pfOut, "\n");
      fprintf(pfOut, ":doctype: manpage\n\n");
      fprintf(pfOut, "NAME\n");
      fprintf(pfOut, "----\n\n");
      for(const char* p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut, ".");
      for(const char* p=pcFct;*p;p++) fprintf(pfOut,"%c",tolower(*p));
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
      fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      fprintf(pfOut, "AUTHOR\n------\n\n");
      fprintf(pfOut, "limes datentechnik(r) gmbh (www.flam.de)\n\n");
   } else {
      if (uiLev) {
         efprintf(pfOut,"[[CLEP.BUILTIN.%s]]\n",pcFct);
         for (unsigned int j=0;j<uiLev;j++) efprintf(pfOut,"=");
         if (isNbr) efprintf(pfOut," %s",pcNum);
         efprintf(pfOut," FUNCTION '%s'\n\n",pcFct);
         if (isIdx) efprintf(pfOut,"indexterm:[BUILTIN, %s]\n\n",pcFct);
         efprintf(pfOut,".SYNOPSIS\n\n");
         efprintf(pfOut,"-----------------------------------------------------------------------\n");
         efprintf(pfOut,"HELP:   %s\n",pcHlp);
         if(isPat)
         efprintf(pfOut,"PATH:   %s.%s\n",pcOwn,pcPgm);
         efprintf(pfOut,"TYPE:   BUILT-IN FUNCTION\n");
         efprintf(pfOut,"SYNTAX: > %s %s\n",pcPgm,pcSyn);
         efprintf(pfOut,"-----------------------------------------------------------------------\n\n");
         efprintf(pfOut,".DESCRIPTION\n\n");
         fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      } else {
         efprintf(pfOut,"[[CLEP.BUILTIN.%s]]\n",pcFct);
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
         if (isIdx) efprintf(pfOut,"indexterm:[BUILTIN, %s]\n\n",pcFct);
         fprintf(pfOut, ".SYNOPSIS\n\n");
         fprintf(pfOut, "-----------------------------------------------------------------------\n");
         efprintf(pfOut,"HELP:   %s\n",pcHlp);
         if(isPat)
         fprintf(pfOut, "PATH:   %s.%s\n",pcOwn,pcPgm);
         fprintf(pfOut, "TYPE:   BUILT-IN FUNCTION\n");
         fprintf(pfOut, "SYNTAX: > %s %s\n",pcPgm,pcSyn);
         fprintf(pfOut, "-----------------------------------------------------------------------\n\n");
         fprintf(pfOut, ".DESCRIPTION\n\n");
         fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      }
   }
}

static void vdPrnStaticSyntax(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
   const char*                   pcDep,
   const char*                   pcSep,
   const char*                   pcDpa)
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
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_SYNTAX  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_HELP    );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_MANPAGE );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_GENDOCU );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_GENPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_SETPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_CHGPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_DELPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_GETPROP );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_SETOWNER);
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_GETOWNER);
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_SETENV  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_GETENV  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_DELENV  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_TRACE   );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_CONFIG  );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_GRAMMAR );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_LEXEM   );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_LICENSE );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_VERSION );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_ABOUT   );
   fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_ERRORS  );
// fprintf(pfOut,"%s%s %s ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",SYN_CLE_BUILTIN_HTMLDOC );
}

static void vdPrnStaticHelp(
   FILE*                         pfOut,
   const TsCleCommand*           psTab,
   const char*                   pcPgm,
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
   fprintf(pfOut,"%s%s %s SYNTAX   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_SYNTAX);
   fprintf(pfOut,"%s%s %s HELP     - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_HELP);
   fprintf(pfOut,"%s%s %s MANPAGE  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_MANPAGE);
   fprintf(pfOut,"%s%s %s GENDOCU  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_GENDOCU);
   fprintf(pfOut,"%s%s %s GENPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_GENPROP);
   fprintf(pfOut,"%s%s %s SETPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_SETPROP);
   fprintf(pfOut,"%s%s %s CHGPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_CHGPROP);
   fprintf(pfOut,"%s%s %s DELPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_DELPROP);
   fprintf(pfOut,"%s%s %s GETPROP  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_GETPROP);
   fprintf(pfOut,"%s%s %s SETOWNER - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_SETOWNER);
   fprintf(pfOut,"%s%s %s GETOWNER - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_GETOWNER);
   fprintf(pfOut,"%s%s %s SETENV   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_SETENV);
   fprintf(pfOut,"%s%s %s GETENV   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_GETENV);
   fprintf(pfOut,"%s%s %s DELENV   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_DELENV);
   fprintf(pfOut,"%s%s %s TRACE    - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_TRACE);
   fprintf(pfOut,"%s%s %s CONFIG   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_CONFIG);
   fprintf(pfOut,"%s%s %s GRAMMAR  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_GRAMMAR);
   fprintf(pfOut,"%s%s %s LEXEM    - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_LEXEM);
   fprintf(pfOut,"%s%s %s LICENSE  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_LICENSE);
   fprintf(pfOut,"%s%s %s VERSION  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_VERSION);
   fprintf(pfOut,"%s%s %s ABOUT    - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_ABOUT);
   fprintf(pfOut,"%s%s %s ERRORS   - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_ERRORS);
// fprintf(pfOut,"%s%s %s HTMLDOC  - ",pcDep,pcDep,pcPgm);efprintf(pfOut,"%s\n",HLP_CLE_BUILTIN_HTMLDOC);
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
   siClpDocu(pvHdl,pfOut,pcCmd,NULL,acNum,"COMMAND",FALSE,isMan,isNbr,FALSE,TRUE,0);
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
         } else *piFlg=1;
      } else *piFlg=2;
   } else *piFlg=3;
   *ppFil=dcpmapfil(pcHlp);
   if (*ppFil==NULL) {
      if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for property file name (%s) failed)\n",pcHlp);
      return(CLERTC_MEM);
   }
   siErr=pfF2S(pvGbl,pvF2S,*ppFil,ppPro,&siSiz,acMsg,sizeof(acMsg));
   if (siErr<0) {
      if (pfErr!=NULL) fprintf(pfErr,"Property file: %s\n",acMsg);
      SAFE_FREE(*ppFil);
      SAFE_FREE(*ppPro);
      return(CLERTC_SYS);
   }
   return(CLERTC_OK);
}

static int siCleGetCommand(
   FILE*                   pfErr,
   const char*             pcDep,
   const char*             pcFct,
   int                     argc,
   char*                   argv[],
   char**                  ppFil,
   char**                  ppCmd,
   void*                   pvGbl,
   void*                   pvF2S,
   TfF2S*                  pfF2S,
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
            siErr=pfF2S(pvGbl,pvF2S,pcDpa,ppCmd,&siSiz,NULL,0);
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
         if (pfErr!=NULL) fprintf(pfErr,"Allocation of memory for command file (%s) failed\n",argv[1]+l+1);
         return(CLERTC_MEM);
      }

      char acMsg[1024]="";
      siErr=pfF2S(pvGbl,pvF2S,*ppFil,ppCmd,&siSiz,acMsg,sizeof(acMsg));
      if (siErr<0) {
         if (pfErr!=NULL) fprintf(pfErr,"Command file: %s\n",acMsg);
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

   pfFil=fopen_hfq(psHdl->pcFil,"r");
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

   pvHdl=pvClpOpen(isCas,isPfl,isRpl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,
                   psTab,pvDat,pfTmp,pfTmp,NULL,NULL,NULL,NULL,
                   pcDep,pcOpt,pcEnt,&stErr,pvGbl,pvF2S,pfF2S,pvSaf,pfSaf);
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
