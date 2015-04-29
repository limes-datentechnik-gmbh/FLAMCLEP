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

/* Include eigener Bibliotheken  **************************************/

#include "CLEPUTL.h"
#include "FLAMCLP.h"

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
 * 1.1.33: Use setenv() instead of putenv() for DD:STDENV on __HOST__
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
 */
#define CLE_VSN_STR       "1.1.46"
#define CLE_VSN_MAJOR      1
#define CLE_VSN_MINOR        1
#define CLE_VSN_REVISION       46

/* Definition der Konstanten ******************************************/
#define CLEMAX_CNFLEN            1023
#define CLEMAX_CNFSIZ            1024
#define CLEMAX_FILLEN            1023
#define CLEMAX_FILSIZ            1024
#define CLEMAX_MODLEN            31
#define CLEMAX_MODSIZ            32
#define CLEMAX_PGMLEN            63
#define CLEMAX_PGMSIZ            64
#define CLEMAX_NUMLEN            1023
#define CLEMAX_NUMSIZ            1024
#define CLEMAX_PATLEN            1023
#define CLEMAX_PATSIZ            1024

/* Definition der Strukturen ******************************************/

typedef struct CnfEnt {
   struct CnfEnt*                psNxt;
   struct CnfEnt*                psBak;
   char                          acKyw[CLEMAX_CNFSIZ];
   char                          acVal[CLEMAX_CNFSIZ];
}TsCnfEnt;

typedef struct CnfHdl {
   int                           isChg;
   int                           isClr;
   int                           isCas;
   char                          acFil[CLEMAX_CNFSIZ];
   char                          acMod[CLEMAX_MODSIZ];
   char                          acPgm[CLEMAX_CNFSIZ];
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
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   char*                         pcFil,
   int*                          piFil,
   tpfMsg                        pfMsg);

static int siClePropertyFinish(
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   FILE*                         pfOut,
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
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   tpfMsg                        pfMsg);

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
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   tpfMsg                        pfMsg);

static int siCleSimpleInit(
   FILE*                         pfOut,
   const int                     isPfl,
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
   FILE*                         pfOut,
   TsCnfHdl*                     psCnf,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcFct,
   char*                         pcFil,
   char**                        ppPro,
   int*                          piFlg);

static int siCleGetCommand(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcDep,
   const char*                   pcFct,
   int                           argc,
   char*                         argv[],
   char*                         pcFil,
   char**                        ppCmd);

static TsCnfHdl* psCnfOpn(
   FILE*                         pfOut,
   const int                     isCas,
   const char*                   pcPgm,
   const char*                   pcFil);

static int siCnfSet(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcKyw,
   const char*                   pcVal,
   const int                     isOvr);

static char* pcCnfGet(
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
   FILE*                         pfOut,
   const char*                   pcPre);

static void vdCnfCls(
   TsCnfHdl*                     psHdl);

/* Implementierung der externen Funktionen ****************************/

extern const char* pcCleVersion(const int l, const int s, char* b)
{
   snprintc(b,s,"%2.2d FLAM-CLE VERSION: %s.%u BUILD: %s %s %s\n",l,CLE_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__);
   return(pcClpVersion(l+1,s,b));
}

extern const char* pcCleAbout(const int l, const int s, char* b)
{
   snprintc(b,s,
   "%2.2d Frankenstein Limes Command Line Execution (FLAM-CLE)\n"
   "   Version: %s.%u Build: %s %s %s\n"
   "   Copyright (C) limes datentechnik (R) gmbh\n"
   "   This library is open source from the FLAM(R) project: http://www.flam.de\n"
   "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
   "This library uses the internal library below:\n"
   ,l,CLE_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__);
   return(pcClpAbout(l+1,s,b));
}

#undef  ERROR
#define ERROR(x) return(siCleEndExecution((x),psCnf,pfTrh,pfDoc,pfPro,ppArg,pvHdl))
extern int siCleExecute(
   const TsCleCommand*           psTab,
   int                           argc,
   char*                         argv[],
   const char*                   pcOwn,
   const char*                   pcPgm,
   const int                     isCas,
   const int                     isPfl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   const char*                   pcLic,
   const char*                   pcVsn,
   const char*                   pcAbo,
   const char*                   pcHlp,
   const char*                   pcMan,
   const char*                   pcCov,
   const char*                   pcGls,
   const char*                   pcFin,
   const char*                   pcDef,
   tpfMsg                        pfMsg)
{
   int                           i,j,l,s,siErr,siDep,siCnt,isSet=0;
   TsCnfHdl*                     psCnf;
   char*                         pcCnf;
   const char*                   pcMsg;
   char                          acCnf[CLEMAX_CNFSIZ];
   char                          acOwn[CLEMAX_CNFSIZ];
   char                          acPgm[CLEMAX_PGMSIZ];
   FILE*                         pfTrh=NULL;
   FILE*                         pfTmp=NULL;
   void*                         pvHdl=NULL;
   char                          acNum[CLEMAX_NUMSIZ];
   FILE*                         pfDoc=NULL;
   FILE*                         pfPro=NULL;
   char**                        ppArg=NULL;
   char                          acFil[CLEMAX_FILSIZ];
   char                          acHlp[CLEMAX_FILSIZ];
   char                          acMod[CLEMAX_MODSIZ];
   int                           siFil=0;
   char                          acHom[CLEMAX_FILSIZ]="";
   char*                         pcTmp=NULL;
   int                           isWrn=FALSE;
   const char*                   m;

   homedir(TRUE,sizeof(acHom),acHom);

   if (psTab==NULL || argc==0 || argv==NULL || pcPgm==NULL || pcHlp==NULL || pfOut==NULL || pcDep==NULL || pcOpt==NULL || pcEnt==NULL ||
       *pcPgm==0 || *pcHlp==0 || strlen(pcPgm)>CLEMAX_PGMLEN) return(CLERTC_FAT);

   for (i=0;i<(sizeof(acPgm)-1) && pcPgm[i];i++) acPgm[i]=toupper(pcPgm[i]);
   acPgm[i]=0;
#ifdef __HOST__
   pfTmp=fopen("DD:STDENV","r");
   if (pfTmp!=NULL) {
      memset(acCnf,0,sizeof(acCnf));
      while (fgets(acCnf,sizeof(acCnf)-1,pfTmp)!=NULL) {
         pcCnf=acCnf+strlen(acCnf);
         while (isspace(*(pcCnf-1))) {
            pcCnf--; *pcCnf=EOS;
         }
         pcCnf=strchr(acCnf,'=');
         if (pcCnf!=NULL) {
            *pcCnf=0x00;
            for (pcTmp=acCnf;isspace(*pcTmp);pcTmp++);
            if (*pcTmp) {
               if (SETENV(pcTmp,pcCnf+1)) {
                  fprintf(pfOut,"Put variable (%s=%s) to environment failed (%d - %s)\n",pcTmp,pcCnf+1,errno,strerror(errno));
                  fclose(pfTmp);
                  return(CLERTC_SYS);
               } else {
                  if (strcmp(pcCnf+1,GETENV(pcTmp))) {
                     fprintf(pfOut,"Put variable (%s=%s) to environment failed (strcmp(%s,GETENV(%s)))\n",pcTmp,pcCnf+1,pcCnf+1,pcTmp);
                     fclose(pfTmp);
                     return(CLERTC_SYS);
                  } else {
                     fprintf(pfOut,"Put variable (%s=%s) to environment was successful\n",pcTmp,pcCnf+1);
                  }
               }
            }
         }
      }
      fclose(pfTmp);
   }
#endif
   snprintf(acCnf,sizeof(acCnf),"%s_DEFAULT_OWNER_ID",acPgm);
   pcCnf=GETENV(acCnf);
   if (pcCnf!=NULL && *pcCnf) snprintf(acOwn,sizeof(acOwn),"%s",pcCnf); else snprintf(acOwn,sizeof(acOwn),"%s",pcOwn);
   snprintf(acCnf,sizeof(acCnf),"%s_CONFIG_FILE",acPgm);
   pcCnf=GETENV(acCnf);
   if (pcCnf==NULL) {
#ifdef __HOST__
      {
         strcpy(acCnf,"<SYSUID>.");
         for (j=strlen(acCnf),i=0;i<8 && pcPgm[i];i++) {
            if (isalnum(pcPgm[i])) {
               acCnf[j]=toupper(pcPgm[i]);
               j++;
            }
         }
         strcpy(&acCnf[j],".CONFIG");
      }
#else
      if (acHom[0]) {
         snprintf(acCnf,sizeof(acCnf),".%s.config",pcPgm);
         pfTmp=fopen(acCnf,"r");
         if (pfTmp==NULL) {
            snprintf(acCnf,sizeof(acCnf),"%s.%s.config",acHom,pcPgm);
            for (i=0;acCnf[i];i++) acCnf[i]=tolower(acCnf[i]);
            fprintf(pfOut,"Use default configuration file (%s) in home directory\n",acCnf);
         } else {
            fclose(pfTmp);
            for (i=0;acCnf[i];i++) acCnf[i]=tolower(acCnf[i]);
            fprintf(pfOut,"Use existing configuration file (%s) in working directory\n",acCnf);
         }
      } else {
         snprintf(acCnf,sizeof(acCnf),".%s.config",pcPgm);
         for (i=0;acCnf[i];i++) acCnf[i]=tolower(acCnf[i]);
         fprintf(pfOut,"Use default configuration file (%s) in working directory\n",acCnf);
      }
#endif
      pcCnf=acCnf;
   } else {
      fprintf(pfOut,"Use configuration file (%s) defined by environment variable (%s)\n",pcCnf,acCnf);
   }
   psCnf=psCnfOpn(pfOut,isCas,pcPgm,pcCnf);
   if (psCnf==NULL) return(CLERTC_CFG);

   snprintf(acCnf,sizeof(acCnf),"%s.owner.id",pcPgm);
   pcCnf=pcCnfGet(psCnf,acCnf);
   if (pcCnf!=NULL && *pcCnf) snprintf(acOwn,sizeof(acOwn),"%s",pcCnf);

#ifdef __DEBUG__
   i=siCnfPutEnv(psCnf,acOwn,pcPgm);
   if (i) {
      if (i==1) {
         fprintf(pfOut,"%d environment variable set\n",i);
      } else {
         fprintf(pfOut,"%d environment variables set\n",i);
      }
   }
#else
   siCnfPutEnv(psCnf,acOwn,pcPgm);
#endif

   snprintf(acCnf,sizeof(acCnf),"%s.%s.trace",acOwn,pcPgm);
   pcCnf=pcCnfGet(psCnf,acCnf);
   if (pcCnf!=NULL && strxcmp(isCas,pcCnf,"ON",0,0,FALSE)==0) {
      snprintf(acCnf,sizeof(acCnf),"%s.%s.trace.file",acOwn,pcPgm);
      pcCnf=pcCnfGet(psCnf,acCnf);
      if (pcCnf!=NULL && *pcCnf) {
         snprintf(acMod,sizeof(acMod),"w%s",cpmapfil(acFil,sizeof(acFil),pcCnf,TRUE));
         pfTrh=fopen(acFil,acMod);
         if (pfTrh==NULL) {
            fprintf(pfOut,"Open of trace file (\"%s\",\"%s\") failed\n",acFil,acMod);
         } else pfTrc=pfTrh;
      }
   } else pfTrc=NULL;

   for (i=0; psTab[i].pcKyw!=NULL; i++) {
      if (psTab[i].psTab==NULL || psTab[i].pvClp==NULL || psTab[i].pvPar==NULL ||
          psTab[i].pfIni==NULL || psTab[i].pfMap==NULL || psTab[i].pfRun==NULL || psTab[i].pfFin==NULL ||
          psTab[i].pcMan==NULL || psTab[i].pcHlp==NULL || *psTab[i].pcKyw==0 || *psTab[i].pcMan==0 || *psTab[i].pcHlp==0) {
         fprintf(pfOut,"Row %d of command table not initialized properly\n",i);
         ERROR(CLERTC_TAB);
      }
   }
   if (i==0) {
      fprintf(pfOut,"Command table is empty\n");
      ERROR(CLERTC_TAB);
   }

   if (argc<2) {
      if (pcDef!=NULL && *pcDef) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            fprintf(pfOut,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef);
            ERROR(CLERTC_SYS);
         }
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc=2; argv=ppArg;
      } else {
         fprintf(pfOut,"Command or built-in function required\n");
         vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
         ERROR(CLERTC_CMD);
      }
   }

   if (argv[1][0]=='-') argv[1]++;
   if (argv[1][0]=='-') argv[1]++;

EVALUATE:
   if (strxcmp(isCas,argv[1],"LICENSE",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"License of program '%s':\n",pcPgm);
         if (pcLic==NULL) {
            fprintf(pfOut,"No license information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcLic);
         }
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'LICENSE' not valid\n");
      fprintf(pfOut,"%s %s LICENSE\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"VERSION",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Version for program '%s':\n",pcPgm);
         if (pcVsn==NULL) {
            fprintf(pfOut,"No version information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcVsn);
         }
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'VERSION' not valid\n");
      fprintf(pfOut,"%s %s VERSION\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"ABOUT",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"About program '%s':\n",pcPgm);
         if (pcAbo==NULL) {
            fprintf(pfOut,"No about information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcAbo);
         }
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'ABOUT' not valid\n");
      fprintf(pfOut,"%s %s ABOUT\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"LEXEM",0,0,FALSE)==0) {
      if (argc==2) {
         siErr=siCleSimpleInit(pfOut,isPfl,pcDep,pcOpt,pcEnt,&pvHdl);
         if (siErr) ERROR(siErr);
         fprintf(pfOut,"Lexemes (regular expressions) for argument list or parameter file:\n");
         siErr=siClpLexem(pvHdl,pfOut);
         if (siErr<0) ERROR(CLERTC_SYN); else ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'LEXEM' not valid\n");
      fprintf(pfOut,"%s %s LEXEM\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"GRAMMAR",0,0,FALSE)==0) {
      if (argc==2) {
         siErr=siCleSimpleInit(pfOut,isPfl,pcDep,pcOpt,pcEnt,&pvHdl);
         if (siErr) ERROR(siErr);
         fprintf(pfOut,"Grammar for argument list, parameter file or property file\n");
         siErr=siClpGrammar(pvHdl,pfOut);
         if (siErr<0) ERROR(CLERTC_SYN); else ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'GRAMMAR' not valid\n");
      fprintf(pfOut,"%s %s GRAMMAR\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"ERRORS",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"\n");
         fprintf(pfOut,"Return/condition/exit codes of the executable\n");
         fprintf(pfOut,"---------------------------------------------\n\n");
         fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_APPENDIX_RETURNCODES,1);
         if (pfMsg!=NULL) {
            fprintf(pfOut,"Reason codes of the different commands\n");
            fprintf(pfOut,"--------------------------------------\n\n");
            fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_APPENDIX_REASONCODES,1);
            for (i=1,m=pfMsg(i);m!=NULL;i++,m=pfMsg(i)) {
               if (*m) fprintf(pfOut," * %d - %s\n",i,m);
            }
         }
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'ERRORS' not valid\n");
      fprintf(pfOut,"%s %s ERRORS\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"SYNTAX",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Syntax for program '%s':\n",pcPgm);
         vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
         ERROR(CLERTC_OK);
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
               fprintf(pfOut,"Syntax for built-in function 'SYNTAX' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s SYNTAX %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
                  }
               }
               ERROR(CLERTC_CMD);
            }
         } else {
            fprintf(pfOut,"Syntax for built-in function 'SYNTAX' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s SYNTAX %s",pcDep,argv[0],psTab[i].pcKyw);
                  efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
               }
            }
            ERROR(CLERTC_CMD);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
               if (siErr) ERROR(siErr);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Syntax for command '%s':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Syntax for argument '%s':\n",argv[2]);
               }
               vdPrnCommandSyntax(pvHdl,pfOut,argv[0],argv[2],pcDep,siDep);
               ERROR(CLERTC_OK);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(pcDef)+strlen(argv[2]+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed\n",pcDef,argv[2]);
                     ERROR(CLERTC_SYS);
                  }
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Syntax for argument '%s':\n",pcPat);
                  vdPrnCommandSyntax(pvHdl,pfOut,argv[0],pcPat,pcDep,siDep);
                  free(pcPat);
                  ERROR(CLERTC_OK);
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'SYNTAX' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s SYNTAX %s\n",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n");
         }
      }
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"HELP",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Help for program '%s':\n",pcPgm);
         vdPrnStaticHelp(pfOut,psTab,argv[0],isCas,pcDep);
         ERROR(CLERTC_OK);
      } else if (argc>=3) {
         if (argc==3) {
            if (strxcmp(isCas,argv[2],"MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--MAN",0,0,FALSE)==0) {
               fprintf(pfOut,"Help for program '%s':\n",pcPgm);
               fprintm(pfOut,pcOwn,pcPgm,pcMan,1);
               ERROR(CLERTC_OK);
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
               fprintf(pfOut,"Syntax for built-in function 'HELP' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s HELP %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
                  }
               }
               ERROR(CLERTC_CMD);
            }
         } else {
            fprintf(pfOut,"Syntax for built-in function 'HELP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s HELP %s",pcDep,argv[0],psTab[i].pcKyw);
                  efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
               }
            }
            ERROR(CLERTC_CMD);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
               if (siErr) ERROR(siErr);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Help for command '%s':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Help for argument '%s':\n",argv[2]);
               }
               vdPrnCommandHelp(pvHdl,argv[2],siDep,siDep>9,TRUE);
               if (siDep==0) {
                  fprintf(pfOut,"ARGUMENTS\n");
                  fprintf(pfOut,"---------\n");
                  vdPrnCommandHelp(pvHdl,argv[2],1,TRUE,FALSE);
               }
               ERROR(CLERTC_OK);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(psTab[i].pcKyw)+strlen(argv[2]+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed\n",psTab[i].pcKyw,argv[2]);
                     ERROR(CLERTC_SYS);
                  }
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",psTab[i].pcKyw,argv[2]);
                  fprintf(pfOut,"Help for argument '%s':\n",pcPat);
                  vdPrnCommandHelp(pvHdl,pcPat,siDep,siDep>9,TRUE);
                  if (siDep==0) {
                     fprintf(pfOut,"ARGUMENTS\n");
                     fprintf(pfOut,"---------\n");
                     vdPrnCommandHelp(pvHdl,pcPat,1,TRUE,FALSE);
                  }
                  free(pcPat);
                  ERROR(CLERTC_OK);
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'HELP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s HELP %s\n",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n");
         }
      }
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"MANPAGE",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Manual page for program '%s':\n\n",pcPgm);
         vdCleManProgram(pfOut,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,FALSE,TRUE);
         ERROR(CLERTC_OK);
      } else if (argc==3) {
         const char*                pcCmd=NULL;
         const char*                pcFil=NULL;
         int                        isMan=FALSE;
         int                        isAll=FALSE;
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
            isMan=TRUE;
            snprintf(acMod,sizeof(acMod),"w%s",cpmapfil(acFil,sizeof(acFil),pcFil,TRUE));
            pfDoc=fopen(acFil,acMod);
            if (pfDoc==NULL) {
               fprintf(pfOut,"Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",acFil,acMod,errno,strerror(errno));
               ERROR(CLERTC_SYS);
            }
         } else {
            pcCmd=argv[2];
            isMan=FALSE;
            pfDoc=pfOut;
            strcpy(acFil,":STDOUT:");
         }
         if (strxcmp(isCas,pcCmd,"ALL",0,0,FALSE)==0 || strxcmp(isCas,pcCmd,"-ALL",0,0,FALSE)==0 || strxcmp(isCas,pcCmd,"--ALL",0,0,FALSE)==0) {
            isAll=TRUE;
            if (isMan==FALSE) fprintf(pfOut,"Manual page for program '%s':\n\n",pcPgm);
            vdCleManProgram(pfDoc,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for program '%s' successfully written to file (%s)\n",pcPgm,acFil);
         }
         if (strxcmp(isCas,pcCmd,"SYNTAX",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SYNTAX':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.1" ,"SYNTAX"  ,HLP_CLE_SYNTAX  ,acOwn,pcPgm,SYN_CLE_SYNTAX,MAN_CLE_SYNTAX,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SYNTAX' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"HELP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'HELP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.2" ,"HELP"    ,HLP_CLE_HELP    ,acOwn,pcPgm,SYN_CLE_HELP,MAN_CLE_HELP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'HELP' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"MANPAGE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'MANPAGE':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.3" ,"MANPAGE" ,HLP_CLE_MANPAGE ,acOwn,pcPgm,SYN_CLE_MANPAGE,MAN_CLE_MANPAGE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'MANPAGE' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"GENDOCU",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GENDOCU':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.4" ,"GENDOCU" ,HLP_CLE_GENDOCU ,acOwn,pcPgm,SYN_CLE_GENDOCU,MAN_CLE_GENDOCU,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GENDOCU' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"GENPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GENPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.5" ,"GENPROP" ,HLP_CLE_GENPROP ,acOwn,pcPgm,SYN_CLE_GENPROP,MAN_CLE_GENPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GENPROP' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"SETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.6" ,"SETPROP" ,HLP_CLE_SETPROP ,acOwn,pcPgm,SYN_CLE_SETPROP,MAN_CLE_SETPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETPROP' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"CHGPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'CHGPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.7" ,"CHGPROP" ,HLP_CLE_CHGPROP ,acOwn,pcPgm,SYN_CLE_CHGPROP,MAN_CLE_CHGPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'CHGPROP' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"DELPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'DELPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.8" ,"DELPROP" ,HLP_CLE_DELPROP ,acOwn,pcPgm,SYN_CLE_DELPROP,MAN_CLE_DELPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'DELPROP' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"GETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETPROP':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.9" ,"GETPROP" ,HLP_CLE_GETPROP ,acOwn,pcPgm,SYN_CLE_GETPROP,MAN_CLE_GETPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETPROP' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"SETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETOWNER':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.10" ,"SETOWNER",HLP_CLE_SETOWNER,acOwn,pcPgm,SYN_CLE_SETOWNER,MAN_CLE_SETOWNER,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETOWNER' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"GETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETOWNER':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.11","GETOWNER",HLP_CLE_GETOWNER,acOwn,pcPgm,SYN_CLE_GETOWNER,MAN_CLE_GETOWNER,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETOWNER' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"SETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'SETENV':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.12" ,"SETENV",HLP_CLE_SETENV,acOwn,pcPgm,SYN_CLE_SETENV,MAN_CLE_SETENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'SETENV' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"GETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GETENV':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.13" ,"GETENV",HLP_CLE_GETENV,acOwn,pcPgm,SYN_CLE_GETENV,MAN_CLE_GETENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GETENV' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"DELENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'DELENV':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.14" ,"DELENV",HLP_CLE_DELENV,acOwn,pcPgm,SYN_CLE_DELENV,MAN_CLE_DELENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'DELENV' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"TRACE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'TRACE':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.15","TRACE"   ,HLP_CLE_TRACE   ,acOwn,pcPgm,SYN_CLE_TRACE,MAN_CLE_TRACE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'TRACE' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"CONFIG",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'CONFIG':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.16","CONFIG"  ,HLP_CLE_CONFIG  ,acOwn,pcPgm,SYN_CLE_CONFIG,MAN_CLE_CONFIG,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'CONFIG' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"GRAMMAR",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'GRAMMAR':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.17","GRAMMAR" ,HLP_CLE_GRAMMAR ,acOwn,pcPgm,SYN_CLE_GRAMMAR,MAN_CLE_GRAMMAR,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'GRAMMAR' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"LEXEM",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'LEXEM':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.18","LEXEM"   ,HLP_CLE_LEXEM   ,acOwn,pcPgm,SYN_CLE_LEXEM,MAN_CLE_LEXEM,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'LEXEM' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"LICENSE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'LICENSE':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.19","LICENSE" ,HLP_CLE_LICENSE ,acOwn,pcPgm,SYN_CLE_LICENSE,MAN_CLE_LICENSE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'LICENSE' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"VERSION",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'VERSION':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.20","VERSION" ,HLP_CLE_VERSION ,acOwn,pcPgm,SYN_CLE_VERSION,MAN_CLE_VERSION,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'VERSION' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"ABOUT",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'ABOUT':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.21","ABOUT"   ,HLP_CLE_ABOUT   ,acOwn,pcPgm,SYN_CLE_ABOUT,MAN_CLE_ABOUT,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'ABOUT' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
         }
         if (strxcmp(isCas,pcCmd,"ERRORS",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function 'ERRORS':\n\n");
            vdCleManFunction(pfDoc,S_TLD,"4.22","ERRORS"  ,HLP_CLE_ERRORS  ,acOwn,pcPgm,SYN_CLE_ERRORS,MAN_CLE_ERRORS,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function 'ERRORS' successfully written to file (%s)\n",acFil);
            if (isAll==FALSE) ERROR(CLERTC_OK);
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
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
               if (siErr) ERROR(siErr);
               if (isMan==FALSE) {
                  if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                     fprintf(pfOut,"Manual page for command '%s':\n\n",pcCmd);
                  } else {
                     fprintf(pfOut,"Manual page for argument '%s':\n\n",pcCmd);
                  }
               }
               vdPrnCommandManpage(pvHdl,pfDoc,pcCmd,i,isMan,TRUE);
               vdClpClose(pvHdl); pvHdl=NULL;
               if (isMan==TRUE) {
                  if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                     fprintf(pfOut,"Manual page for command '%s' successfully written to file (%s)\n",pcCmd,acFil);
                  } else {
                     fprintf(pfOut,"Manual page for argument '%s' successfully written to file (%s)\n",pcCmd,acFil);
                  }
               }
               if (isAll==FALSE) ERROR(CLERTC_OK);
            }
         }
         if (isAll==TRUE) ERROR(CLERTC_OK);

         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(pcDef)+strlen(pcCmd+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed\n",pcDef,argv[2]);
                     ERROR(CLERTC_SYS);
                  }
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",pcDef,pcCmd);
                  fprintf(pfOut,"Manual page fo'argument '%s':\n\n",pcPat);
                  vdPrnCommandManpage(pvHdl,pfDoc,pcPat,i,isMan,TRUE);
                  if (isMan==TRUE) {
                     fprintf(pfOut,"Manual page for argument '%s' successfully written to file (%s)\n",pcPat,acFil);
                  }
                  free(pcPat);
                  ERROR(CLERTC_OK);
               }
            }
         }

         pcFil=argv[2];
         isMan=TRUE;
         snprintf(acMod,sizeof(acMod),"w%s",cpmapfil(acFil,sizeof(acFil),pcFil,TRUE));
         pfDoc=fopen(acFil,acMod);
         if (pfDoc==NULL) {
            fprintf(pfOut,"Open of manual page file (\"%s\",\"%s\") failed (%d - %s)\n",acFil,acMod,errno,strerror(errno));
            ERROR(CLERTC_SYS);
         }
         vdCleManProgram(pfDoc,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,isMan,TRUE);
         fprintf(pfOut,"Manual page for program '%s' successfully written to file (%s)\n",pcPgm,acFil);
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'MANPAGE' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s MANPAGE %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfOut,"[.path]\n");
         }
      }
      fprintf(pfOut,"%s %s MANPAGE function\n",pcDep,argv[0]);
      fprintf(pfOut,"%s %s MANPAGE\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"GENDOCU",0,0,FALSE)==0) {
      const char*                pcCmd=NULL;
      const char*                pcFil=NULL;
      int                        isNbr=TRUE;
      if (argc==3 || argc==4) {
         if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"NONBR",0,0,FALSE)==0) {
               isNbr=FALSE;
            } else {
               fprintf(pfOut,"Syntax for built-in function 'GENDOCU' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s GENDOCU %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfOut,"[.path]=filename [NONBR]\n");
                  }
               }
               fprintf(pfOut,"%s %s GENDOCU filename %cNONBR%c\n",pcDep,argv[0],C_SBO,C_SBC);
               ERROR(CLERTC_CMD);
            }
         }
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
         } else {
            pcFil=argv[2]; pcCmd=NULL;
         }
         snprintf(acMod,sizeof(acMod),"w%s",cpmapfil(acFil,sizeof(acFil),pcFil,TRUE));
         pfDoc=fopen(acFil,acMod);
         if (pfDoc==NULL) {
            fprintf(pfOut,"Open of documentation file (\"%s\",\"%s\") failed (%d - %s)\n",acFil,acMod,errno,strerror(errno));
            ERROR(CLERTC_SYS);
         }
         if (pcCmd!=NULL) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
                  if (siErr) ERROR(siErr);
                  snprintf(acNum,sizeof(acNum),"2.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,pcCmd,acNum,"COMMAND",TRUE,FALSE,isNbr);
                  if (siErr<0) {
                     fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",acFil,errno,strerror(errno));
                     ERROR(CLERTC_SYN);
                  } else {
                     if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                        fprintf(pfOut,"Documentation for command '%s' successfully created\n",pcCmd);
                     } else {
                        fprintf(pfOut,"Documentation for argument '%s' successfully created\n",pcCmd);
                     }
                     ERROR(CLERTC_OK);
                  }
               }
            }
            if (pcDef!=NULL && *pcDef) {
               for (i=0;psTab[i].pcKyw!=NULL;i++) {
                  if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                     char* pcPat=(char*)malloc(strlen(pcDef)+strlen(pcCmd+2));
                     if (pcPat==NULL) {
                        fprintf(pfOut,"Memory allocation for path '%s.%s' failed\n",pcDef,argv[2]);
                        ERROR(CLERTC_SYS);
                     }
                     siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
                     if (siErr) {
                        free(pcPat);
                        ERROR(siErr);
                     }
                     snprintf(acNum,sizeof(acNum),"2.%d.",i+1);
                     sprintf(pcPat,"%s.%s",pcDef,pcCmd);
                     siErr=siClpDocu(pvHdl,pfDoc,pcPat,acNum,"COMMAND",TRUE,FALSE,isNbr);
                     if (siErr<0) {
                        fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",acFil,errno,strerror(errno));
                        free(pcPat);
                        ERROR(CLERTC_SYN);
                     } else {
                        fprintf(pfOut,"Documentation for argument '%s' successfully created\n",pcPat);
                        free(pcPat);
                        ERROR(CLERTC_OK);
                     }
                  }
               }
            }
         } else {
            if (pcCov!=NULL && *pcCov) {
               fprintm(pfDoc,pcOwn,pcPgm,pcCov,2);
            } else {
               snprintf(acNum,sizeof(acNum),"'%s' - User Manual",acPgm); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
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

            vdCleManProgram(pfDoc,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,FALSE,isNbr);

            if (isNbr) {
               snprintf(acNum,sizeof(acNum),"3. Available commands"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            } else {
               snprintf(acNum,sizeof(acNum),"Available commands"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            }
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_COMMANDS,2);
            efprintf(pfDoc,"indexterm:[Available commands]\n\n\n");

            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
                  if (siErr) ERROR(siErr);
                  snprintf(acNum,sizeof(acNum),"3.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,psTab[i].pcKyw,acNum,"COMMAND",TRUE,FALSE,isNbr);
                  if (siErr<0) {
                     fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",acFil,errno,strerror(errno));
                     vdClpClose(pvHdl); pvHdl=NULL;
                     ERROR(CLERTC_SYN);
                  }
                  vdClpClose(pvHdl); pvHdl=NULL;
               }
            }

            if (isNbr) {
               snprintf(acNum,sizeof(acNum),"4. Available built-in functions"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            } else {
               snprintf(acNum,sizeof(acNum),"Available built-in functions"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            }
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_FUNCTIONS,2);
            efprintf(pfDoc,"indexterm:[Available built-in functions]\n\n\n");

            vdCleManFunction(pfDoc,S_TLD,"4.1" ,"SYNTAX"  ,HLP_CLE_SYNTAX  ,acOwn,pcPgm,SYN_CLE_SYNTAX  ,MAN_CLE_SYNTAX  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.2" ,"HELP"    ,HLP_CLE_HELP    ,acOwn,pcPgm,SYN_CLE_HELP    ,MAN_CLE_HELP    ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.3" ,"MANPAGE" ,HLP_CLE_MANPAGE ,acOwn,pcPgm,SYN_CLE_MANPAGE ,MAN_CLE_MANPAGE ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.4" ,"GENDOCU" ,HLP_CLE_GENDOCU ,acOwn,pcPgm,SYN_CLE_GENDOCU ,MAN_CLE_GENDOCU ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.5" ,"GENPROP" ,HLP_CLE_GENPROP ,acOwn,pcPgm,SYN_CLE_GENPROP ,MAN_CLE_GENPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.6" ,"SETPROP" ,HLP_CLE_SETPROP ,acOwn,pcPgm,SYN_CLE_SETPROP ,MAN_CLE_SETPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.7" ,"CHGPROP" ,HLP_CLE_CHGPROP ,acOwn,pcPgm,SYN_CLE_CHGPROP ,MAN_CLE_CHGPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.8" ,"DELPROP" ,HLP_CLE_DELPROP ,acOwn,pcPgm,SYN_CLE_DELPROP ,MAN_CLE_DELPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.9" ,"GETPROP" ,HLP_CLE_GETPROP ,acOwn,pcPgm,SYN_CLE_GETPROP ,MAN_CLE_GETPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.10","SETOWNER",HLP_CLE_SETOWNER,acOwn,pcPgm,SYN_CLE_SETOWNER,MAN_CLE_SETOWNER,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.11","GETOWNER",HLP_CLE_GETOWNER,acOwn,pcPgm,SYN_CLE_GETOWNER,MAN_CLE_GETOWNER,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.12","SETENV"  ,HLP_CLE_SETENV  ,acOwn,pcPgm,SYN_CLE_SETENV  ,MAN_CLE_SETENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.13","GETENV"  ,HLP_CLE_GETENV  ,acOwn,pcPgm,SYN_CLE_GETENV  ,MAN_CLE_GETENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.14","DELENV"  ,HLP_CLE_DELENV  ,acOwn,pcPgm,SYN_CLE_DELENV  ,MAN_CLE_DELENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.15","TRACE"   ,HLP_CLE_TRACE   ,acOwn,pcPgm,SYN_CLE_TRACE   ,MAN_CLE_TRACE   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.16","CONFIG"  ,HLP_CLE_CONFIG  ,acOwn,pcPgm,SYN_CLE_CONFIG  ,MAN_CLE_CONFIG  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.17","GRAMMAR" ,HLP_CLE_GRAMMAR ,acOwn,pcPgm,SYN_CLE_GRAMMAR ,MAN_CLE_GRAMMAR ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.18","LEXEM"   ,HLP_CLE_LEXEM   ,acOwn,pcPgm,SYN_CLE_LEXEM   ,MAN_CLE_LEXEM   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.19","LICENSE" ,HLP_CLE_LICENSE ,acOwn,pcPgm,SYN_CLE_LICENSE ,MAN_CLE_LICENSE ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.20","VERSION" ,HLP_CLE_VERSION ,acOwn,pcPgm,SYN_CLE_VERSION ,MAN_CLE_VERSION ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.21","ABOUT"   ,HLP_CLE_ABOUT   ,acOwn,pcPgm,SYN_CLE_ABOUT   ,MAN_CLE_ABOUT   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,S_TLD,"4.22","ERRORS"  ,HLP_CLE_ERRORS  ,acOwn,pcPgm,SYN_CLE_ERRORS  ,MAN_CLE_ERRORS  ,FALSE,isNbr);

            s=1;
            siErr=siCleSimpleInit(pfOut,isPfl,pcDep,pcOpt,pcEnt,&pvHdl);
            if (siErr) ERROR(siErr);
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
               fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",acFil,errno,strerror(errno));
               ERROR(CLERTC_SYN);
            }

            efprintf(pfDoc,"[[appendix-grammar]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"GRAMMAR\n");
            fprintf(pfDoc,"-------\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_GRAMMAR,1);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"Grammar for argument list, parameter file or property file\n");
            siErr=siClpGrammar(pvHdl,pfDoc); s++;
            vdClpClose(pvHdl); pvHdl=NULL;
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            efprintf(pfDoc,"indexterm:[Appendix Grammar]\n\n\n");
            if (siErr<0) {
               fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",acFil,errno,strerror(errno));
               ERROR(CLERTC_SYN);
            }

            vdClpClose(pvHdl); pvHdl=NULL;

            efprintf(pfDoc,"[[appendix-properties]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"PROPERTIES\n");
            fprintf(pfDoc,"----------\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_PROPERTIES,1);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"\n%c Property file for: %s.%s %c\n\n",C_HSH,acOwn,pcPgm,C_HSH);
            efprintf(pfDoc,"%s",HLP_CLE_PROPFIL);
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acHlp,&siFil,pfMsg);
               if (siErr) ERROR(siErr);
               siErr=siClpProperties(pvHdl,FALSE,10,psTab[i].pcKyw,pfDoc);
               vdClpClose(pvHdl); pvHdl=NULL;
            }
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            efprintf(pfDoc,"indexterm:[Appendix Properties]\n\n\n");
            if (siErr<0) {
               fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",acFil,errno,strerror(errno));
               ERROR(CLERTC_SYN);
            }

            efprintf(pfDoc,"[[appendix-returncodes]]\n");
            efprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"RETURN CODES\n");
            fprintf(pfDoc,"------------\n\n");
            fprintm(pfDoc,pcOwn,pcPgm,MAN_CLE_APPENDIX_RETURNCODES,1);
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
            ERROR(CLERTC_OK);
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'GENDOCU' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s GENDOCU %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfOut,"[.path]=filename [NONBR]\n");
         }
      }
      fprintf(pfOut,"%s %s GENDOCU filename %cNONBR%c\n",pcDep,argv[0],C_SBO,C_SBC);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"GENPROP",0,0,FALSE)==0) {
      const char*                pcCmd=NULL;
      const char*                pcFil=NULL;
      if (argc==3) {
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
         } else {
            pcFil=argv[2]; pcCmd=NULL;
         }
         snprintf(acMod,sizeof(acMod),"w%s",cpmapfil(acFil,sizeof(acFil),pcFil,TRUE));
         pfPro=fopen(acFil,acMod);
         if (pfPro==NULL) {
            fprintf(pfOut,"Open of property file (\"%s\",\"%s\") failed (%d-%s)\n",acFil,acMod,errno,strerror(errno));
            ERROR(CLERTC_SYS);
         }
         if (pcCmd==NULL) fprintf(pfPro,"\n%c Property file for: %s.%s %c\n\n",C_HSH,acOwn,pcPgm,C_HSH);
                    else  fprintf(pfPro,"\n%c Property file for: %s.%s.%s %c\n\n",C_HSH,acOwn,pcPgm,pcCmd,C_HSH);
         efprintf(pfPro,"%s",HLP_CLE_PROPFIL);

         if (pcCmd==NULL) {
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acHlp,&siFil,pfMsg);
               if (siErr) ERROR(siErr);
               siErr=siClpProperties(pvHdl,FALSE,10,psTab[i].pcKyw,pfPro);
               vdClpClose(pvHdl); pvHdl=NULL;
            }
            if (siErr<0) {
               fprintf(pfOut,"Write property file (%s) for program '%s' failed (%d-%s)\n",acFil,pcPgm,errno,strerror(errno));
               ERROR(CLERTC_SYN);
            } else {
               fprintf(pfOut,"Property file (%s) for program '%s' successfully written\n",acFil,pcPgm);
               ERROR(CLERTC_OK);
            }
         } else {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,0,FALSE)==0) {
                  siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acHlp,&siFil,pfMsg);
                  if (siErr) ERROR(siErr);
                  siErr=siClpProperties(pvHdl,FALSE,10,psTab[i].pcKyw,pfPro);
                  vdClpClose(pvHdl); pvHdl=NULL;
                  if (siErr<0) {
                     fprintf(pfOut,"Write property file (%s) for command '%s' failed (%d-%s)\n",acFil,pcCmd,errno,strerror(errno));
                     ERROR(CLERTC_SYN);
                  } else {
                     fprintf(pfOut,"Property file (%s) for command '%s' successfully written\n",acFil,pcCmd);
                     ERROR(CLERTC_OK);
                  }
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'GENPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s GENPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s GENPROP filename\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"SETPROP",0,0,FALSE)==0) {
      const char*                pcCmd=NULL;
      const char*                pcFil=NULL;
      if (argc==3) {
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
            for (i=0;psTab[i].pcKyw!=NULL && strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,0,FALSE);i++);
            if (psTab[i].pcKyw==NULL) {
               fprintf(pfOut,"Syntax for built-in function 'SETPROP' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               ERROR(CLERTC_CMD);
            }
            if (strlen(acOwn)+strlen(pcPgm)+strlen(pcCmd)+20>=sizeof(acCnf)) {
               fprintf(pfOut,"Configuration keyword '%s.%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,pcCmd,(unsigned)sizeof(acCnf));
               ERROR(CLERTC_CMD);
            }
            snprintf(acCnf,sizeof(acCnf),"%s.%s.%s.property.file",acOwn,pcPgm,pcCmd);
         } else {
            pcFil=argv[2]; pcCmd=NULL;
            if (strlen(acOwn)+strlen(pcPgm)+20>=sizeof(acCnf)) {
               fprintf(pfOut,"Configuration keyword '%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,(unsigned)sizeof(acCnf));
               ERROR(CLERTC_CMD);
            }
            snprintf(acCnf,sizeof(acCnf),"%s.%s.property.file",acOwn,pcPgm);
         }
         if (*pcFil==0) {
            fprintf(pfOut,"Syntax for built-in function 'SETPROP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            fprintf(pfOut,"File name was not specified.\n"
                          "To delete a property file from the list, please use the function DELPROP %ccommand%c\n",C_SBO,C_SBC);
            ERROR(CLERTC_CMD);
         }
         siErr=siCnfSet(psCnf,pfOut,acCnf,pcFil,TRUE);
         if (siErr) ERROR(CLERTC_CFG); else {
            fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",acCnf,pcFil);
            ERROR(CLERTC_OK);
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'SETPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s SETPROP filename\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"CHGPROP",0,0,FALSE)==0 || (pcDef!=NULL && strxcmp(isCas,pcDef,"flam",0,0,FALSE)==0 && strxcmp(isCas,argv[1],"DEFAULTS",0,0,FALSE)==0)) {
      if (argc>=3) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,0,0,FALSE)==0) {
               char acPro[CLEMAX_PATSIZ]="";
               for (j=3;j<argc;j++) {
                  if (strlen(acPro)+strlen(argv[j])+strlen(acOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw)+8>CLEMAX_PATSIZ) {
                     fprintf(pfOut,"Argument list is too long\n");
                     ERROR(CLERTC_CMD);
                  }
                  if (j>3) strcat(acPro," ");
                  strcat(acPro,acOwn);
                  strcat(acPro,".");
                  strcat(acPro,pcPgm);
                  strcat(acPro,".");
                  strcat(acPro,psTab[i].pcKyw);
                  strcat(acPro,".");
                  pcTmp=strchr(argv[j],'=');
                  if (pcTmp!=NULL) {
                     *pcTmp=0x00; pcTmp++;
                     strcat(acPro,argv[j]);
                     strcat(acPro,"=\"");
                     strcat(acPro,pcTmp);
                     strcat(acPro,"\"");
                  } else {
                     strcat(acPro,argv[j]);
                     strcat(acPro,"=\"");
                     strcat(acPro,"\"");
                  }
               }
               ERROR(siCleChangeProperties(psTab[i].pfIni,psTab[i].pvClp,acHom,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                           acPro,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,pfMsg));
            }
         }
      }
      if (pcDef!=NULL && *pcDef) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
               char acPro[CLEMAX_PATSIZ]="";
               for (j=2;j<argc;j++) {
                  if (strlen(acPro)+strlen(argv[j])+strlen(acOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw)+5>CLEMAX_PATSIZ) {
                     fprintf(pfOut,"Argument list is too long\n");
                     ERROR(CLERTC_CMD);
                  }
                  if (j>2) strcat(acPro," ");
                  strcat(acPro,acOwn);
                  strcat(acPro,".");
                  strcat(acPro,pcPgm);
                  strcat(acPro,".");
                  strcat(acPro,psTab[i].pcKyw);
                  strcat(acPro,".");
                  pcTmp=strchr(argv[j],'=');
                  if (pcTmp!=NULL) {
                     *pcTmp=0x00; pcTmp++;
                     strcat(acPro,argv[j]);
                     strcat(acPro,"=\"");
                     strcat(acPro,pcTmp);
                     strcat(acPro,"\"");
                  } else {
                     strcat(acPro,argv[j]);
                     strcat(acPro,"=\"");
                     strcat(acPro,"\"");
                  }
               }
               ERROR(siCleChangeProperties(psTab[i].pfIni,psTab[i].pvClp,acHom,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                           acPro,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,pfMsg));
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'CHGPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s CHGPROP %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfOut," [path[=value]]*\n");
         }
      }
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"DELPROP",0,0,FALSE)==0) {
      if (argc==2) {
         if (strlen(acOwn)+strlen(pcPgm)+20>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,(unsigned)sizeof(acCnf));
            ERROR(CLERTC_CMD);
         }
         snprintf(acCnf,sizeof(acCnf),"%s.%s.property.file",acOwn,pcPgm);
      } else if (argc==3) {
         for (i=0;psTab[i].pcKyw!=NULL && strxcmp(isCas,argv[2],psTab[i].pcKyw,0,0,FALSE);i++);
         if (psTab[i].pcKyw==NULL) {
            fprintf(pfOut,"Syntax for built-in function 'DELPROP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s DELPROP %s\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            ERROR(CLERTC_CMD);
         }
         if (strlen(acOwn)+strlen(pcPgm)+strlen(argv[2])+20>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,argv[2],(unsigned)sizeof(acCnf));
            ERROR(CLERTC_CMD);
         }
         snprintf(acCnf,sizeof(acCnf),"%s.%s.%s.property.file",acOwn,pcPgm,argv[2]);
      } else {
         fprintf(pfOut,"Syntax for built-in function 'DELPROP' not valid\n");
         for (i=0;psTab[i].pcKyw!=NULL ;i++) {
            if (psTab[i].siFlg) {
               fprintf(pfOut,"%s %s DELPROP %s\n",pcDep,argv[0],psTab[i].pcKyw);
            }
         }
         fprintf(pfOut,"%s %s DELPROP\n",pcDep,argv[0]);
         ERROR(CLERTC_CMD);
      }
      siErr=siCnfSet(psCnf,pfOut,acCnf,"",TRUE);
      if (siErr) ERROR(CLERTC_CFG); else {
         fprintf(pfOut,"Delete configuration keyword '%s' was successful\n",acCnf);
         ERROR(CLERTC_OK);
      }
   } else if (strxcmp(isCas,argv[1],"GETPROP",0,0,FALSE)==0 || (pcDef!=NULL && strxcmp(isCas,pcDef,"flam",0,0,FALSE)==0 && strxcmp(isCas,argv[1],"LIST",0,0,FALSE)==0)) {
      if (argc==2) {
         fprintf(pfOut,"Properties for program '%s':\n",pcPgm);
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                    psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                    &pvHdl,acFil,&siFil,pfMsg);
            if (siErr) ERROR(siErr);
            vdPrnProperties(pvHdl,psTab[i].pcKyw,TRUE,10);
            vdClpClose(pvHdl); pvHdl=NULL;
         }
         ERROR(CLERTC_OK);
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
               fprintf(pfOut,"Syntax for built-in function 'GETPROP' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s GETPROP %s",pcDep,argv[0],psTab[i].pcKyw);
                     efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
                  }
               }
               ERROR(CLERTC_CMD);
            }
         } else {
            fprintf(pfOut,"Syntax for built-in function 'GETPROP' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s GETPROP %s",pcDep,argv[0],psTab[i].pcKyw);
                  efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
               }
            }
            ERROR(CLERTC_CMD);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,strlen(psTab[i].pcKyw),'.',TRUE)==0) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acFil,&siFil,pfMsg);
               if (siErr) ERROR(siErr);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Properties for command '%s':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Properties for argument '%s':\n",argv[2]);
               }
               vdPrnProperties(pvHdl,argv[2],isSet,siDep);
               ERROR(CLERTC_OK);
            }
         }
         if (pcDef!=NULL && *pcDef) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(pcDef)+strlen(argv[2]+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed\n",pcDef,argv[2]);
                     ERROR(CLERTC_SYS);
                  }
                  siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acFil,&siFil,pfMsg);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Properties for argument '%s':\n",pcPat);
                  vdPrnProperties(pvHdl,pcPat,isSet,siDep);
                  free(pcPat);
                  ERROR(CLERTC_OK);
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'GETPROP' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s GETPROP %s",pcDep,argv[0],psTab[i].pcKyw);
            efprintf(pfOut,"[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | DEPALL | DEFALL]\n");
         }
      }
      fprintf(pfOut,"%s %s GETPROP\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"SETOWNER",0,0,FALSE)==0) {
      if (argc==3) {
         snprintf(acCnf,sizeof(acCnf),"%s.owner.id",pcPgm);
         siErr=siCnfSet(psCnf,pfOut,acCnf,argv[2],TRUE);
         if (siErr) ERROR(CLERTC_CFG); else {
            fprintf(pfOut,"Setting configuration key word '%s' to value '%s' was successful\n",acCnf,argv[2]);
            ERROR(CLERTC_OK);
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'SETOWNER' not valid\n");
      fprintf(pfOut,"%s %s SETOWNER name\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"GETOWNER",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Current owner id for '%s' is: %s\n",argv[0],acOwn);
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'GETOWNER' not valid\n");
      fprintf(pfOut,"%s %s GETOWNER\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"SETENV",0,0,FALSE)==0) {
      if (argc==3) {
         const char* pcVal=NULL;
         const char* pcEnv=NULL;
         pcVal=strchr(argv[2],'=');
         if (pcVal!=NULL) {
            *((char*)pcVal)=EOS; pcVal++; pcEnv=argv[2];
         } else {
            fprintf(pfOut,"Syntax for built-in function 'SETENV' not valid\n");
            fprintf(pfOut,"%s %s SETENV variable=value\n",pcDep,argv[0]);
            ERROR(CLERTC_CMD);
         }
         if (strlen(acOwn)+strlen(pcPgm)+strlen(pcEnv)+8>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.envar.%s' too long (>=%u)\n",acOwn,pcPgm,pcEnv,(unsigned)sizeof(acCnf));
            ERROR(CLERTC_CMD);
         }
         snprintf(acCnf,sizeof(acCnf),"%s.%s.envar.%s",acOwn,pcPgm,pcEnv);
         siErr=siCnfSet(psCnf,pfOut,acCnf,pcVal,TRUE);
         if (siErr) ERROR(CLERTC_CFG); else {
            fprintf(pfOut,"Setting environment variable '%s' to value '%s' was successful\n",acCnf,pcVal);
            ERROR(CLERTC_OK);
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'SETENV' not valid\n");
      fprintf(pfOut,"%s %s SETENV variable=value\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"GETENV",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Current environment variables for owner '%s':\n",acOwn);
         siCnt=siCnfPrnEnv(psCnf,pfOut,pcDep,acOwn,pcPgm);
         if (siCnt) {
            fprintf(pfOut,"Defined in file \"%s\"\n",psCnf->acFil);
         } else {
            fprintf(pfOut,"No environment variables defined in file \"%s\" for owner '%s'\n",psCnf->acFil,acOwn);
         }
         ERROR(CLERTC_OK);
      }
      fprintf(pfOut,"Syntax for built-in function 'GETENV' not valid\n");
      fprintf(pfOut,"%s %s GETENV\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"DELENV",0,0,FALSE)==0) {
      if (argc==3) {
         if (strlen(acOwn)+strlen(pcPgm)+strlen(argv[2])+10>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.envar.%s' too long (>=%u)\n",acOwn,pcPgm,argv[2],(unsigned)sizeof(acCnf));
            ERROR(CLERTC_CMD);
         }
         snprintf(acCnf,sizeof(acCnf),"%s.%s.envar.%s",acOwn,pcPgm,argv[2]);
         siErr=siCnfSet(psCnf,pfOut,acCnf,"",TRUE);
         if (siErr) ERROR(CLERTC_CFG); else {
            fprintf(pfOut,"Deleting the environment variable '%s' was successful\n",acCnf);
            ERROR(CLERTC_OK);
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'DELENV' not valid\n");
      fprintf(pfOut,"%s %s DELENV variable\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"TRACE",0,0,FALSE)==0) {
      if (argc==3) {
         if (strxcmp(isCas,argv[2],"ON",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-ON",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--ON",0,0,FALSE)==0) {
            snprintf(acCnf,sizeof(acCnf),"%s.%s.trace",acOwn,pcPgm);
            siErr=siCnfSet(psCnf,pfOut,acCnf,"ON",TRUE);
            if (siErr) ERROR(CLERTC_CFG); else {
               fprintf(pfOut,"Setting configuration keyword '%s' to value 'ON' was successful\n",acCnf);
               ERROR(CLERTC_OK);
            }
         } else if (strxcmp(isCas,argv[2],"OFF",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-OFF",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--OFF",0,0,FALSE)==0) {
            snprintf(acCnf,sizeof(acCnf),"%s.%s.trace",acOwn,pcPgm);
            siErr=siCnfSet(psCnf,pfOut,acCnf,"OFF",TRUE);
            if (siErr) ERROR(CLERTC_CFG); else {
               fprintf(pfOut,"Setting configuration keyword '%s' to value 'OFF' was successful\n",acCnf);
               ERROR(CLERTC_OK);
            }
         } else {
            const char* pcFil;
            const char* pcCmd;
            pcFil=strchr(argv[2],'=');
            if (pcFil!=NULL) {
               *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
               if (strxcmp(isCas,pcCmd,"FILE",0,0,FALSE)==0) {
                  snprintf(acCnf,sizeof(acCnf),"%s.%s.trace.file",acOwn,pcPgm);
                  siErr=siCnfSet(psCnf,pfOut,acCnf,pcFil,TRUE);
                  if (siErr) ERROR(CLERTC_CFG); else {
                     if (*pcFil) {
                        fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",acCnf,pcFil);
                     } else {
                        fprintf(pfOut,"Deleting value from configuration keyword '%s' was successful\n",acCnf);
                     }
                     ERROR(CLERTC_OK);
                  }
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'TRACE' not valid\n");
      fprintf(pfOut,"%s %s TRACE ON/OFF\n",pcDep,argv[0]);
      fprintf(pfOut,"%s %s TRACE FILE=filenam\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else if (strxcmp(isCas,argv[1],"CONFIG",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Current configuration data:\n");
         siCnt=siCnfPrn(psCnf,pfOut,pcDep);
         if (siCnt) {
            fprintf(pfOut,"Assigned to file \"%s\"\n",psCnf->acFil);
         } else {
            fprintf(pfOut,"No configuration data defined for file \"%s\"\n",psCnf->acFil);
         }
         ERROR(CLERTC_OK);
      } else if (argc==3) {
         if (argv[2][0]=='-') argv[2]++;
         if (argv[2][0]=='-') argv[2]++;
         if (strxcmp(isCas,argv[2],"CLEAR",0,0,FALSE)==0) {
            siCnt=siCnfClr(psCnf,pfOut,pcDep);
            if (siCnt) {
               fprintf(pfOut,"Delete %d elements from file \"%s\"\n",siCnt,psCnf->acFil);
            } else {
               fprintf(pfOut,"No configuration data defined for file \"%s\"\n",psCnf->acFil);
            }
            ERROR(CLERTC_OK);
         }
      }
      fprintf(pfOut,"Syntax for built-in function 'CONFIG' not valid\n");
      fprintf(pfOut,"%s %s CONFIG\n",pcDep,argv[0]);
      fprintf(pfOut,"%s %s CONFIG CLEAR\n",pcDep,argv[0]);
      ERROR(CLERTC_CMD);
   } else {
      int siMaxCC=0x0FFFFFFF;
      if (strxcmp(isCas,argv[1],"OWNER=",6,0,FALSE)==0) {
         snprintf(acOwn,sizeof(acOwn),"%s",&argv[1][6]);
         fprintf(pfOut,"Use owner: '%s'\n",acOwn);
         for (i=1;i<argc;i++) argv[i]=argv[i+1];
         argc--;
      }
      if (strxcmp(isCas,argv[argc-1],"MAXCC=",6,0,FALSE)==0) {
          siMaxCC=atoi(&argv[argc-1][6]);
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
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isPfl,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl,pfMsg);
               if (siErr) ERROR(((siErr>siMaxCC)?siMaxCC:siErr));
               siErr=siCleGetCommand(pvHdl,pfOut,pcDep,psTab[i].pcKyw,argc,argv,acFil,&pcCmd);
               if (siErr) ERROR(((siErr>siMaxCC)?siMaxCC:siErr));
               siErr=siClpParseCmd(pvHdl,acFil,pcCmd,TRUE,psTab[i].piOid,&pcTls);
               if (siErr<0) {
                  fprintf(pfOut,"Command line parser for command '%s' failed\n",psTab[i].pcKyw);
                  if (pcCmd!=NULL) free(pcCmd);
                  ERROR(((CLERTC_SYN>siMaxCC)?siMaxCC:CLERTC_SYN));
               }
               if (pcTls!=NULL) {
                  pcLst=(char*)malloc(strlen(pcTls)+1);
                  if (pcLst!=NULL) {
                     strcpy(pcLst,pcTls);
                  }
               }
               vdClpClose(pvHdl); pvHdl=NULL;
               siErr=psTab[i].pfMap(pfOut,pfTrc,psTab[i].piOid,psTab[i].pvClp,psTab[i].pvPar);
               if (siErr) {
                  if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                     fprintf(pfOut,"Mapping of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_MAP,siErr,pcMsg);
                  } else {
                     fprintf(pfOut,"Mapping of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_MAP,siErr);
                  }
                  if (pcCmd!=NULL) free(pcCmd);
                  if (pcLst!=NULL) free(pcLst);
                  ERROR(((CLERTC_MAP>siMaxCC)?siMaxCC:CLERTC_MAP));
               }
               siErr=psTab[i].pfRun(pfOut,pfTrc,acOwn,pcPgm,pcVsn,pcAbo,pcLic,psTab[i].pcKyw,pcCmd,pcLst,psTab[i].pvPar,&isWrn);
               if (pcCmd!=NULL) free(pcCmd);
               if (pcLst!=NULL) free(pcLst);
               if (siErr) {
                  if (isWrn) {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        fprintf(pfOut,"Run of command '%s' ends with warning (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_WRN,siErr,pcMsg);
                     } else {
                        fprintf(pfOut,"Run of command '%s' ends with warning (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_WRN,siErr);
                     }
                     psTab[i].pfFin(pfOut,pfTrc,psTab[i].pvPar);
                     ERROR(((CLERTC_WRN>siMaxCC)?siMaxCC:CLERTC_WRN));
                  } else {
                     if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                        fprintf(pfOut,"Run of command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_RUN,siErr,pcMsg);
                     } else {
                        fprintf(pfOut,"Run of command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_RUN,siErr);
                     }
                     psTab[i].pfFin(pfOut,pfTrc,psTab[i].pvPar);
                     ERROR(((CLERTC_RUN>siMaxCC)?siMaxCC:CLERTC_RUN));
                  }
               }
               siErr=psTab[i].pfFin(pfOut,pfTrc,psTab[i].pvPar);
               if (siErr) {
                  if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
                     fprintf(pfOut,"Finish/cleanup for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",psTab[i].pcKyw,CLERTC_FIN,siErr,pcMsg);
                  } else {
                     fprintf(pfOut,"Finish/cleanup for command '%s' failed (Return code: %d / Reason code: %d)\n",psTab[i].pcKyw,CLERTC_FIN,siErr);
                  }
                  ERROR(((CLERTC_FIN>siMaxCC)?siMaxCC:CLERTC_FIN));
               }
               ERROR(CLERTC_OK);
            }
         }
      }
      if (pcDef!=NULL && *pcDef && ppArg==NULL) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            fprintf(pfOut,"Memory allocation for argument list to run the default command '%s' failed\n",pcDef);
            ERROR(((CLERTC_SYS>siMaxCC)?siMaxCC:CLERTC_SYS));
         }
         for (i=argc;i>1;i--) ppArg[i]=argv[i-1];
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc++; argv=ppArg;
         goto EVALUATE;
      }
      fprintf(pfOut,"Command or built-in function '%s' not supported\n",argv[1]);
      vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
      ERROR(((CLERTC_CMD>siMaxCC)?siMaxCC:CLERTC_CMD));
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
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   char*                         pcFil,
   int*                          piFil,
   tpfMsg                        pfMsg)
{
   int                           siErr;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   char*                         pcPro=NULL;
   const char*                   pcMsg;

   *ppHdl=NULL;
   siErr=pfIni(pfOut,pfTrc,pcOwn,pcPgm,pvClp);
   if (siErr) {
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         fprintf(pfOut,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg);
      } else {
         fprintf(pfOut,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr);
      }
      return(CLERTC_INI);
   }
   *ppHdl=pvClpOpen(isCas,isPfl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,psTab,pvClp,pfOut,pfOut,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of property parser for command '%s' failed\n",pcCmd);
      return(CLERTC_TAB);
   }
   siErr=siCleGetProperties(*ppHdl,pfOut,psCnf,pcOwn,pcPgm,pcCmd,pcFil,&pcPro,piFil);
   if (siErr) {
      if (pcPro!=NULL) free(pcPro);
      vdClpClose(*ppHdl);*ppHdl=NULL;
      return(siErr);
   }
   if (pcPro!=NULL) {
      siErr=siClpParsePro(*ppHdl,pcFil,pcPro,FALSE,NULL);
      if (siErr<0) {
         fprintf(pfOut,"Parsing property file \"%s\" for command '%s' failed\n",pcFil,pcCmd);
         vdClpClose(*ppHdl);*ppHdl=NULL;
         free(pcPro);
         return(CLERTC_SYN);
      }
      free(pcPro);
   }
   return(CLERTC_OK);
}

static int siClePropertyFinish(
   const char*                   pcHom,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcCmd,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   TsCnfHdl*                     psCnf,
   void*                         pvHdl,
   const char*                   pcFil,
   const int                     siFil)
{
   int                           siErr,i;
   FILE*                         pfPro;
   char                          acEnv[CLEMAX_CNFSIZ];
   char                          acCnf[CLEMAX_CNFSIZ];
   char                          acFil[CLEMAX_FILSIZ];
   char                          acMod[CLEMAX_MODSIZ];
   if (siFil!=3) {
      snprintf(acEnv,sizeof(acEnv),"%s_%s_%s_PROPERTY_FILENAME",pcOwn,pcPgm,pcCmd);
      for (i=0;acEnv[i];i++) acEnv[i]=toupper(acEnv[i]);
      pcFil=GETENV(acEnv);
      if (pcFil==NULL) {
#ifdef __HOST__
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
   snprintf(acMod,sizeof(acMod),"w%s",cpmapfil(acFil,sizeof(acFil),pcFil,TRUE));
   pfPro=fopen(acFil,acMod);
   if (pfPro==NULL) {
      fprintf(pfOut,"Cannot open the property file (\"%s\",\"%s\") for write operation (%d-%s)\n",acFil,acMod,errno,strerror(errno));
      vdClpClose(pvHdl);
      return(CLERTC_SYS);
   }

   siErr=siClpProperties(pvHdl,FALSE,10,pcCmd,pfPro);
   if (siErr<0) {
      fprintf(pfOut,"Write property file (%s) for command '%s' failed (%d-%s)\n",acFil,pcCmd,errno,strerror(errno));
      vdClpClose(pvHdl); fclose(pfPro); return(CLERTC_SYN);
   }
   vdClpClose(pvHdl); fclose(pfPro);
   fprintf(pfOut,"Property file (%s) for command '%s' successfully written\n",acFil,pcCmd);

   if (siFil!=3) {
      snprintf(acCnf,sizeof(acCnf),"%s.%s.%s.property.file",pcOwn,pcPgm,pcCmd);
      siErr=siCnfSet(psCnf,pfOut,acCnf,pcFil,TRUE);
      if (siErr) {
         fprintf(pfOut,"Activation of property file (%s) for command '%s' failed\n",pcFil,pcCmd);
         return(CLERTC_CFG);
      }
      fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",acCnf,pcFil);
      fprintf(pfOut,"Activation of property file (%s) for command '%s' was successful\n",pcFil,pcCmd);
   }
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
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   tpfMsg                        pfMsg)
{
   int                           siErr,siFil=0;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   char                          acFil[CLEMAX_FILSIZ];
   char*                         pcPro=NULL;
   const char*                   pcMsg;

   *ppHdl=NULL;

   siErr=pfIni(pfOut,pfTrc,pcOwn,pcPgm,pvClp);
   if (siErr) {
      if (pfMsg!=NULL && (pcMsg=pfMsg(siErr))!=NULL) {
         fprintf(pfOut,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d (%s))\n",pcCmd,CLERTC_INI,siErr,pcMsg);
      } else {
         fprintf(pfOut,"Initialization of CLP structure for command '%s' failed (Return code: %d / Reason code: %d)\n",pcCmd,CLERTC_INI,siErr);
      }
      return(CLERTC_INI);
   }
   *ppHdl=pvClpOpen(isCas,isPfl,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,psTab,pvClp,pfOut,pfOut,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt,NULL);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of parser for command '%s' failed\n",pcCmd);
      return(CLERTC_TAB);
   }
   siErr=siCleGetProperties(*ppHdl,pfOut,psCnf,pcOwn,pcPgm,pcCmd,acFil,&pcPro,&siFil);
   if (siErr) {
      vdClpClose(*ppHdl);*ppHdl=NULL;
      if (pcPro!=NULL) free(pcPro);
      return(siErr);
   }
   if (pcPro!=NULL) {
      siErr=siClpParsePro(*ppHdl,acFil,pcPro,FALSE,NULL);
      if (siErr<0) {
         fprintf(pfOut,"Property parser for command '%s' failed\n",pcCmd);
         vdClpClose(*ppHdl);*ppHdl=NULL;
         free(pcPro);
         return(CLERTC_SYN);
      }
      free(pcPro);
   }

   return(CLERTC_OK);
}

static int siCleSimpleInit(
   FILE*                         pfOut,
   const int                     isPfl,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   void**                        ppHdl)
{
   const TsClpArgument asTab[]={
         {CLPTYP_NUMBER,"XX",NULL,0,1,1,0,0,CLPFLG_NON,NULL,NULL,NULL,"XX",0,0.0,NULL},
         {CLPTYP_NON   ,NULL,NULL,0,0,0,0,0,CLPFLG_NON,NULL,NULL,NULL,NULL,0,0.0,NULL}
   };
   *ppHdl=pvClpOpen(FALSE,isPfl,0,"","","","","",FALSE,asTab,"",pfOut,pfOut,NULL,NULL,NULL,NULL,pcDep,pcOpt,pcEnt,NULL);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of command line parser for grammar and lexem print out failed\n");
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
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   tpfMsg                        pfMsg)
{
   int                           siErr;
   void*                         pvHdl=NULL;
   char                          acFil[CLEMAX_FILSIZ];
   int                           siFil=0;

   siErr=siClePropertyInit(pfIni,pvClp,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,
                           piOid,psTab,isCas,isPfl,siMkl,pfOut,pfTrc,
                           pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acFil,&siFil,pfMsg);
   if (siErr) return(siErr);

   siErr=siClpParsePro(pvHdl,acFil,pcPro,TRUE,NULL);
   if (siErr<0) {
      fprintf(pfOut,"Property parser for command '%s' failed\n",pcCmd);
      vdClpClose(pvHdl);
      return(CLERTC_SYN);
   }

   if (siErr==0) {
      fprintf(pfOut, "Don't update any property in property file (%s)\n",acFil);
   } else if (siErr==1) {
      fprintf(pfOut, "Updated 1 property in property file (%s)\n",acFil);
   } else {
      fprintf(pfOut, "Updated %d properties in property file (%s)\n",siErr,acFil);
   }

   siErr=siClePropertyFinish(pcHom,pcOwn,pcPgm,pcCmd,pfOut,pfTrc,psCnf,pvHdl,acFil,siFil);
   if (siErr) return(siErr);

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
   if (psCnf!=NULL) vdCnfCls(psCnf);
   if (pfTrc!=NULL) fclose(pfTrc);
   if (pfDoc!=NULL) fclose(pfDoc);
   if (pfPro!=NULL) fclose(pfPro);
   if (ppArg!=NULL) free(ppArg);
   if (pvHdl!=NULL) vdClpClose(pvHdl);
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
         for (i=0;i<l;i++) fprintf(pfOut,"%c",'-'); fprintf(pfOut,"\n\n");
         fprintf(pfOut,"2.1. SYNOPSIS\n");
         for (i=0;i<13;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"PROGRAM '%s'\n",pcPgm);
         l=strlen(pcPgm)+10;
         for (i=0;i<l;i++) fprintf(pfOut,"%c",'-'); fprintf(pfOut,"\n\n");
         fprintf(pfOut,"SYNOPSIS\n");
         for (i=0;i<8;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
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
         for (i=0;i<16;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");

      } else {
         fprintf(pfOut,"DESCRIPTION\n");
         for (i=0;i<11;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
      }
      if (pcMan!=NULL && *pcMan) {
         fprintm(pfOut,pcOwn,pcPgm,pcMan,2);
      } else {
         fprintf(pfOut,"No detailed description available for this program.\n\n");
      }
      fprintf(pfOut,"indexterm:%cDescription for program %s%c\n\n\n",C_SBO,pcPgm,C_SBC);
      if (isNbr) {
         fprintf(pfOut,"2.3. SYNTAX\n");
         for (i=0;i<12;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"SYNTAX\n");
         for (i=0;i<6;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
      }
      fprintm(pfOut,pcOwn,pcPgm,MAN_CLE_MAIN_SYNTAX,1);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Syntax for program '%s':\n",pcPgm);
      vdPrnStaticSyntax(pfOut,psTab,pcPgm,pcDep,pcSep);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");
      fprintf(pfOut,"indexterm:%cSyntax for program %s%c\n\n\n",C_SBO,pcPgm,C_SBC);

      if (isNbr) {
         fprintf(pfOut,"2.4. HELP\n");
         for (i=0;i<9;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"HELP\n");
         for (i=0;i<4;i++) fprintf(pfOut,"%c",C_TLD); fprintf(pfOut,"\n\n");
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
         for (i=0;i<l;i++) fprintf(pfOut,"%s",pcLev); fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"FUNCTION '%s'\n",pcFct);
         l=strlen(pcFct)+11;
         for (i=0;i<l;i++) fprintf(pfOut,"%s",pcLev); fprintf(pfOut,"\n\n");
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
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command \"... argument list ...\" %cMAXCC=num%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBC);
   fprintf(pfOut,"%s%s %s %cOWNER=oid%c command=\" parameter file name \" %cMAXCC=num%c\n",pcDep,pcDep,pcPgm,C_SBO,C_SBC,C_SBO,C_SBC);
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
   char                    acNum[CLEMAX_NUMLEN];
   snprintf(acNum,sizeof(acNum),"3.%d.",siInd+1);
   siClpDocu(pvHdl,pfOut,pcCmd,acNum,"COMMAND",FALSE,isMan,isNbr);
}

static void vdPrnProperties(
   void*                   pvHdl,
   const char*             pcPat,
   const int               isSet,
   const int               siDep)
{
   siClpProperties(pvHdl,isSet,siDep,pcPat,NULL);
}

static int siCleGetProperties(
   void*                   pvHdl,
   FILE*                   pfOut,
   TsCnfHdl*               psCnf,
   const char*             pcOwn,
   const char*             pcPgm,
   const char*             pcFct,
   char*                   pcFil,
   char**                  ppPro,
   int*                    piFlg)
{
   int                     siErr,siSiz=0;
   char*                   pcHlp=NULL;
   char                    acRot[CLEMAX_PATSIZ];

   pcFil[0]=EOS;
   if (strlen(pcOwn)+strlen(pcPgm)+strlen(pcFct)+2>=CLEMAX_PATLEN) {
      fprintf(pfOut,"Rot (%s.%s.%s) is too long (>=%d)\n",pcOwn,pcPgm,pcFct,CLEMAX_PATLEN);
      return(CLERTC_CMD);
   }
   snprintf(acRot,sizeof(acRot),"%s.%s.%s.property.file",pcOwn,pcPgm,pcFct);
   pcHlp=pcCnfGet(psCnf,acRot);
   if (pcHlp==NULL) {
      snprintf(acRot,sizeof(acRot),"%s.%s.property.file",pcOwn,pcPgm);
      pcHlp=pcCnfGet(psCnf,acRot);
      if (pcHlp==NULL) {
         snprintf(acRot,sizeof(acRot),"%s.property.file",pcOwn);
         pcHlp=pcCnfGet(psCnf,acRot);
         if (pcHlp==NULL) {
            *piFlg=0;
            return(CLERTC_OK);
         } else *piFlg=1;
      } else *piFlg=2;
   } else *piFlg=3;
   cpmapfil(pcFil,CLEMAX_FILSIZ,pcHlp,TRUE);
   siErr=file2str(pcFil,ppPro,&siSiz);
   if (siErr<0) {
      if (*ppPro!=NULL) { free(*ppPro); *ppPro=NULL; }
      switch(siErr) {
      case -1: fprintf(pfOut,"Illegal parameters passed to file2str() (Bug)\n");                            return(CLERTC_FAT);
      case -2: fprintf(pfOut,"Open of property file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));  return(CLERTC_SYS);
      case -3: fprintf(pfOut,"Property file (%s) is too big (integer overflow)\n",pcFil);                   return(CLERTC_CMD);
      case -4: fprintf(pfOut,"Allocation of memory for property file (%s) failed.\n",pcFil);                return(CLERTC_SYS);
      case -5: fprintf(pfOut,"Read of property file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));  return(CLERTC_SYS);
      default: fprintf(pfOut,"An unknown error occurred while reading property file (%s).\n",pcFil);        return(CLERTC_FAT);
      }
   }
   return(CLERTC_OK);
}

static int siCleGetCommand(
   void*                   pvHdl,
   FILE*                   pfOut,
   const char*             pcDep,
   const char*             pcFct,
   int                     argc,
   char*                   argv[],
   char*                   pcFil,
   char**                  ppCmd)
{
   int                     siErr,siSiz=0;
   int                     l=strlen(pcFct);
   pcFil[0]=EOS;
   if (argc==2 && argv[1][l]!='=' && argv[1][l]!='.' && argv[1][l]!='(') {
      fprintf(pfOut,"Argument list (\"...\") for command '%s' missing\n",pcFct);
      fprintf(pfOut,"Syntax for command '%s' not valid\n",pcFct);
      fprintf(pfOut,"%s %s \"",pcDep,argv[0]);
      siClpSyntax(pvHdl,FALSE,FALSE,1,NULL);
      fprintf(pfOut,"\"\n");
      fprintf(pfOut,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
      return(CLERTC_CMD);
   }
   if (argv[1][l]==EOS) {
      siErr=arry2str(argv+2,argc-2," ",1,ppCmd,&siSiz);
      if (siErr<0) {
         if (*ppCmd!=NULL) { free(*ppCmd); *ppCmd=NULL; }
         switch(siErr) {
         case -1: fprintf(pfOut,"Illegal parameters passed to arry2str() (Bug)\n");                                 return(CLERTC_FAT);
         case -2: fprintf(pfOut,"Allocation of memory for command line failed (%d - %s).\n",errno,strerror(errno)); return(CLERTC_SYS);
         default: fprintf(pfOut,"An unknown error occurred while reading command line.\n");                         return(CLERTC_FAT);
         }
      }
   } else if (argv[1][l]=='.' || argv[1][l]=='(') {
      argv[1]=&argv[1][l];
      siErr=arry2str(argv+1,argc-1," ",1,ppCmd,&siSiz);
      if (siErr<0) {
         if (*ppCmd!=NULL) { free(*ppCmd); *ppCmd=NULL; }
         switch(siErr) {
         case -1: fprintf(pfOut,"Illegal parameters passed to arry2str() (Bug)\n");                                 return(CLERTC_FAT);
         case -2: fprintf(pfOut,"Allocation of memory for command line failed (%d - %s).\n",errno,strerror(errno)); return(CLERTC_SYS);
         default: fprintf(pfOut,"An unknown error occurred while reading command line.\n");                         return(CLERTC_FAT);
         }
      }
   } else if (argv[1][l]=='=') {
      if (argc!=2) {
         fprintf(pfOut,"The expected parameter file name for '%s' is split into more than one parameter\n",pcFct);
         fprintf(pfOut,"The parameter file name must start with \" and end with \" to join anything into one parameter\n");
         fprintf(pfOut,"Syntax for command '%s' not valid\n",pcFct);
         fprintf(pfOut,"%s %s %s=\" parameter file name \"\n",pcDep,argv[0],pcFct);
         fprintf(pfOut,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
         return(CLERTC_CMD);
      }
      if (strlen(argv[1])>=CLEMAX_FILLEN) {
         fprintf(pfOut,"Parameter file name is too long (more than %d bytes)\n",CLEMAX_FILLEN);
         return(CLERTC_CMD);
      }
      cpmapfil(pcFil,CLEMAX_FILSIZ,argv[1]+l+1,TRUE);
      siErr=file2str(pcFil,ppCmd,&siSiz);
      if (siErr<0) {
         if (*ppCmd!=NULL) { free(*ppCmd); *ppCmd=NULL; }
         switch(siErr) {
         case -1: fprintf(pfOut,"Illegal parameters passed to file2str() (Bug)\n");                           return(CLERTC_FAT);
         case -2: fprintf(pfOut,"Open of command file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));  return(CLERTC_SYS);
         case -3: fprintf(pfOut,"Command file (%s) is too big (integer overflow)\n",pcFil);                   return(CLERTC_CMD);
         case -4: fprintf(pfOut,"Allocation of memory for command file (%s) failed.\n",pcFil);                return(CLERTC_SYS);
         case -5: fprintf(pfOut,"Read of command file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));  return(CLERTC_SYS);
         default: fprintf(pfOut,"An unknown error occurred while reading command file (%s).\n",pcFil);        return(CLERTC_FAT);
         }
      }
   } else {
      fprintf(pfOut,"No blank space ' ', equal sign '=', dot '.' or bracket '(' behind '%s'\n",pcFct);
      fprintf(pfOut,"Please use a blank space to define an argument list or an equal sign for a parameter file\n");
      fprintf(pfOut,"Syntax for command '%s' not valid\n",pcFct);
      fprintf(pfOut,"%s %s %cOWNER=oid%c %s \"... argument list ...\" %cMAXCC=num%c\n",pcDep,argv[0],C_SBO,C_SBC,pcFct,C_SBO,C_SBC);
      fprintf(pfOut,"%s %s %cOWNER=oid%c %s=\" parameter file name \" %cMAXCC=num%c\n",pcDep,argv[0],C_SBO,C_SBC,pcFct,C_SBO,C_SBC);
      fprintf(pfOut,"Please use '%s SYNTAX %s%c.path%c' for more information\n",argv[0],pcFct,C_SBO,C_SBC);
      return(CLERTC_CMD);
   }
   return(CLERTC_OK);
}

/**********************************************************************/

static TsCnfHdl* psCnfOpn(
   FILE*                         pfOut,
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
      if (pfOut!=NULL) fprintf(pfOut,"Memory allocation for configuration data handle failed\n");
      return(NULL);
   }
   psHdl->isCas=isCas;
   psHdl->psFst=NULL;
   psHdl->psLst=NULL;
   if (pcPgm!=NULL) snprintf(psHdl->acPgm,sizeof(psHdl->acPgm),"%s",pcPgm);
   if (pcFil==NULL || *pcFil==0) return(psHdl);
   snprintf(psHdl->acMod,sizeof(psHdl->acMod),"w%s",cpmapfil(psHdl->acFil,sizeof(psHdl->acFil),pcFil,TRUE));
   pfFil=fopen(psHdl->acFil,"r");
   if (pfFil==NULL && (errno==2 || errno==49 || errno==129)) return(psHdl);
   if (pfFil==NULL) {
      if (pfOut!=NULL) fprintf(pfOut,"Cannot open the configuration file (\"%s\",\"r\") for read operation (%d - %s)\n",psHdl->acFil,errno,strerror(errno));
      free(psHdl);
      return(NULL);
   }

   while (fgets(acBuf,sizeof(acBuf)-1,pfFil)!=NULL) {
      pcHlp=strchr(acBuf,C_HSH);
      if (pcHlp!=NULL) *pcHlp=EOS;
      pcHlp=acBuf+strlen(acBuf);
      while (isspace(*(pcHlp-1))) {
         pcHlp--; *pcHlp=EOS;
      }
      pcHlp=strchr(acBuf,'=');
      if (pcHlp!=NULL) {
         pcKyw=acBuf; pcVal=pcHlp+1; *pcHlp=EOS;
         while (isspace(*pcKyw)) pcKyw++;
         psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
         if (psEnt==NULL) {
            if (pfOut!=NULL) fprintf(pfOut,"Memory allocation for configuration data element failed\n");
            fclose(pfFil); free(psHdl);
            return(NULL);
         }
         siKyw=strlen(pcKyw); siVal=strlen(pcVal);
         if (siKyw && siVal) {
            if (siKyw>=sizeof(psEnt->acKyw)-1) {
               if (pfOut!=NULL) fprintf(pfOut,"Keyword (%s) in configuration file (%s) too long (>=%u)\n",pcKyw,psHdl->acFil,(unsigned)sizeof(psEnt->acKyw)-1);
               free(psEnt); fclose(pfFil); free(psHdl);
               return(NULL);
            }
            if (siVal>=sizeof(psEnt->acVal)-1) {
               if (pfOut!=NULL) fprintf(pfOut,"Value (%s) in configuration file (%s) too long (>=%u)\n",pcVal,psHdl->acFil,(unsigned)sizeof(psEnt->acVal)-1);
               free(psEnt); fclose(pfFil); free(psHdl);
               return(NULL);
            }
            strcpy(psEnt->acKyw,pcKyw); strcpy(psEnt->acVal,pcVal);
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
   fclose(pfFil);
   return(psHdl);
}

static int siCnfSet(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcKyw,
   const char*                   pcVal,
   const int                     isOvr)
{
   int                           siKyw,siVal;
   TsCnfEnt*                     psEnt;
   for (psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt) {
      if (strxcmp(psHdl->isCas,psEnt->acKyw,pcKyw,0,0,FALSE)==0) {
         siVal=strlen(pcVal);
         if (siVal>=sizeof(psEnt->acVal)-1) {
            if (pfOut!=NULL) fprintf(pfOut,"Configuration value (%s) too long (>=%u)\n",pcVal,(unsigned)sizeof(psEnt->acVal)-1);
            return(-1);
         } else if (siVal==0) {
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
            free(psEnt);
         } else {
            if (isOvr || psEnt->acVal[0]==0) {
               strcpy(psEnt->acVal,pcVal);
            }else {
               if (pfOut!=NULL) fprintf(pfOut,"Configuration value (%s) for keyword '%s' already exists\n",psEnt->acVal,psEnt->acKyw);
               return(1);
            }
         }
         psHdl->isChg=TRUE;
         return(0);
      }
   }

   psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
   if (psEnt==NULL) {
      if (pfOut!=NULL) fprintf(pfOut,"Memory allocation for configuration data element failed\n");
      return(-1);
   }
   siKyw=strlen(pcKyw); siVal=strlen(pcVal);
   if (siKyw && siVal) {
      if (siKyw>=sizeof(psEnt->acKyw)-1) {
         if (pfOut!=NULL) fprintf(pfOut,"Configuration keyword (%s) too long (>=%u)\n",pcKyw,(unsigned)sizeof(psEnt->acKyw)-1);
         free(psEnt);
         return(-1);
      }
      if (siVal>=sizeof(psEnt->acVal)-1) {
         if (pfOut!=NULL) fprintf(pfOut,"Configuration value (%s) too long (>=%u)\n",pcVal,(unsigned)sizeof(psEnt->acVal)-1);
         free(psEnt);
         return(-1);
      }
      strcpy(psEnt->acKyw,pcKyw); strcpy(psEnt->acVal,pcVal);
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
      if (pfOut!=NULL) fprintf(pfOut,"Configuration keyword(%s) and/or value(%s) is empty\n", pcKyw, pcVal);
      free(psEnt);
      return (-1);
   }
   psHdl->isChg=TRUE;
   return(0);
}

static char* pcCnfGet(
   TsCnfHdl*                     psHdl,
   const char*                   pcKyw)
{
   TsCnfEnt*                     psEnt;
   for (psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt) {
      if (strxcmp(psHdl->isCas,psEnt->acKyw,pcKyw,0,0,FALSE)==0) return(psEnt->acVal);
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
      if (strstr(psEnt->acKyw,pcOwn)!=NULL &&
          strstr(psEnt->acKyw,pcPgm)!=NULL &&
          strstr(psEnt->acKyw,".envar.")!=NULL) {
         const char* pcKyw=strstr(psEnt->acKyw,".envar.")+7;
         if (SETENV(pcKyw,psEnt->acVal)==0) {
            const char* pcHlp=GETENV(pcKyw);
            if (pcHlp!=NULL) {
               if (strcmp(pcHlp,psEnt->acVal)==0) {
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
   const char*                   pcAdd="not verified";
   for (i=0,psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt,i++) {
      if (strstr(psEnt->acKyw,pcOwn)!=NULL &&
          strstr(psEnt->acKyw,pcPgm)!=NULL &&
          strstr(psEnt->acKyw,".envar.")!=NULL) {
         const char* pcKyw=strstr(psEnt->acKyw,".envar.")+7;
         const char* pcHlp=GETENV(pcKyw);
         if (pcHlp!=NULL) {
            if (strcmp(pcHlp,psEnt->acVal)==0) {
               pcAdd="was verified";
            }
         }
         if (pcPre!=NULL && *pcPre) {
            fprintf(pfOut,"%s %s=%s %c %s\n",pcPre,pcKyw,psEnt->acVal,C_HSH,pcAdd);
         } else {
            fprintf(pfOut,"%s=%s %c %s\n",pcKyw,psEnt->acVal,C_HSH,pcAdd);
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
         fprintf(pfOut,"%s %s=%s\n",pcPre,psEnt->acKyw,psEnt->acVal);
      } else {
         fprintf(pfOut,"%s=%s\n",psEnt->acKyw,psEnt->acVal);
      }
   }
   return(i);
}

static int siCnfClr(
   TsCnfHdl*                     psHdl,
   FILE*                         pfOut,
   const char*                   pcPre)
{
   int                           i=0;
   TsCnfEnt*                     psEnt;
   TsCnfEnt*                     psHlp;
   psEnt=psHdl->psFst;
   while(psEnt!=NULL) {
      psHlp=psEnt->psNxt;
      memset(psEnt,0,sizeof(TsCnfEnt));
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
      if (psHdl->isClr) {
         pfFil=fopen(psHdl->acFil,psHdl->acMod);
         if (pfFil!=NULL) fclose(pfFil);
         if (!ISDDNAME(psHdl->acFil)) remove(psHdl->acFil);
      } else {
         psEnt=psHdl->psFst;
         if (psHdl->isChg && psHdl->acFil[0]) {
            pfFil=fopen(psHdl->acFil,psHdl->acMod);
         }
         if (pfFil!=NULL) {
            fprintf(pfFil,"%c Configuration file for program '%s'\n",C_HSH,psHdl->acPgm);
         }
         while(psEnt!=NULL) {
            if (pfFil!=NULL) fprintf(pfFil,"%s=%s\n",psEnt->acKyw,psEnt->acVal);
            psHlp=psEnt->psNxt; free(psEnt); psEnt=psHlp;
         }
         if (pfFil!=NULL) fclose(pfFil);
      }
      free(psHdl);
   }
}

/**********************************************************************/

