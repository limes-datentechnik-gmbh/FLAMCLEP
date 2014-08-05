/**
 * @file   CLE.c
 * @brief  LIMES Command Line Execution in ANSI-C
 *
 * LIMES Command Line Executor (CLE) in ANSI-C
 * @author FALK REICHBOTT
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
 **********************************************************************/
/*
 * TASK:0000086 Fehlermeldungen ueberarbeiten (Zentralisierung)
 * TASK:0000086 Wide character support fuer command files einbauen
 * TASK:0000086 Mehrsprachigkeit einbauen
 */
/* Standard-Includes **************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#ifdef __WIN__
#define putenv(s) _putenv((s))
#endif

/* Include eigener Bibliotheken  **************************************/

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
 */
#define CLE_VSN_STR       "1.1.19"
#define CLE_VSN_MAJOR      1
#define CLE_VSN_MINOR        1
#define CLE_VSN_REVISION       19

/* Definition der Konstanten ******************************************/
#define CLEMAX_CNFLEN            1023
#define CLEMAX_CNFSIZ            1024
#define CLEMAX_FILLEN            1023
#define CLEMAX_FILSIZ            1024
#define CLEMAX_PGMLEN            63
#define CLEMAX_PGMSIZ            64
#define CLEMAX_CMDLEN            65535
#define CLEMAX_CMDSIZ            65536
#define CLEMAX_PROLEN            262143
#define CLEMAX_PROSIZ            262144
#define CLEMAX_NUMLEN            1023
#define CLEMAX_NUMSIZ            1024
#define CLEMAX_PATLEN            1023
#define CLEMAX_PATSIZ            1024

/* HOME and USER ******************************************************/

#ifdef __WIN__
#define _WIN32_IE 0x5000
#include <shlobj.h>
#include <windows.h>
static char* HOMEDIR(int flg) {
   static char dir[MAX_PATH+1]="";
   SHGetFolderPath(NULL,CSIDL_PERSONAL,NULL,0,dir);
   if (flg) strcat(dir,"\\");
   return(dir);
}
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
static char* HOMEDIR(int flg) {
   static char dir[CLEMAX_FILSIZ]="";
   struct passwd* uP = getpwuid(geteuid());
   if (NULL != uP) {
      strncpy(dir,uP->pw_dir,sizeof(dir)-2);
      dir[sizeof(dir)-2]=0x00;
      if (flg) strcat(dir,"/");
   }
   return(dir);
}
#endif

/* Definition der Strukturen ******************************************/

typedef struct CnfEnt {
   struct CnfEnt*                psNxt;
   struct CnfEnt*                psBak;
   char                          acKyw[CLEMAX_CNFSIZ];
   char                          acVal[CLEMAX_CNFSIZ];
   char                          acEnv[2*CLEMAX_CNFSIZ];
}TsCnfEnt;

typedef struct CnfHdl {
   int                           isChg;
   int                           isClr;
   int                           isCas;
   char                          acFil[CLEMAX_CNFSIZ];
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
   const int                     isFlg,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   char*                         pcFil,
   int*                          piFil);

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
   const int                     isFlg,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf);

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
   const int                     isFlg,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl);

static int siCleSimpleInit(
   FILE*                         pfOut,
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
   const int                     isMan);

static void vdPrnCommandManpage(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcCmd,
   const int                     siInd,
   const int                     isMan,
   const int                     isNbr);

static void vdPrnCommandError(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcCmd,
   const char*                   pcPos,
   const char*                   pcLst,
   const char*                   pcDep);

static void vdPrnPropertyError(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcFil,
   const char*                   pcPro,
   const char*                   pcPos,
   const char*                   pcLst,
   const char*                   pcDep);

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
   char*                         pcPro,
   int*                          piFlg);

static int siCleGetCommand(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcDep,
   const char*                   pcFct,
   int                           argc,
   char*                         argv[],
   char*                         pcCmd);

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
#define VSNLENGTHMAX   256
#define VSNLENGTHMIN   0
#define ABOLENGTHMAX   1024
#define ABOLENGTHMIN   0

extern const char* pcCleVersion(const int l)
{
   static char                   acVsn[VSNLENGTHMAX];
   sprintf(acVsn,"%2.2d FLAM-CLE VERSION: %s.%u BUILD: %s %s %s\n%s",l,CLE_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__,pcClpVersion(l+1));
   if (strlen(acVsn)>=VSNLENGTHMAX || strlen(acVsn)<VSNLENGTHMIN) {
      fprintf(stderr,"\n*** Static area (%d) for version string (%d) too small or too big ***\n\n%s\n\n",(int)sizeof(acVsn),(int)strlen(acVsn),acVsn);
      exit(-1);
   }
   return(acVsn);
}

extern const char* pcCleAbout(const int l)
{
   static char                acAbo[ABOLENGTHMAX];
   sprintf(acAbo,
   "%2.2d Frankenstein Limes Command Line Execution (FLAM-CLE)\n"
   "   Version: %s.%u Build: %s %s %s\n"
   "   Copyright (C) limes datentechnik (R) gmbh\n"
   "   This library is open source from the FLAM(R) project: http://www.flam.de\n"
   "   for license see: https://github.com/limes-datentechnik-gmbh/flamclep\n"
   "This library uses the internal library below:\n%s"
   ,l,CLE_VSN_STR,__BUILDNR__,__BUILD__,__DATE__,__TIME__,pcClpAbout(l+1));
   if (strlen(acAbo)>=ABOLENGTHMAX || strlen(acAbo)<ABOLENGTHMIN) {
      fprintf(stderr,"\n*** Static area (%d) for about message (%d) too small or too big ***\n\n%s\n\n",(int)sizeof(acAbo),(int)strlen(acAbo),acAbo);
      exit(-1);
   }
   return(acAbo);
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
   const int                     isFlg,
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
   const char*                   pcDef)
{
   int                           i,j,l,s,siErr,siDep,siCnt,isSet=0;
   char*                         pcPos=NULL;
   char*                         pcLst=NULL;
   static char                   acCmd[CLEMAX_CMDSIZ];
   TsCnfHdl*                     psCnf;
   char*                         pcCnf;
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
   int                           siFil=0;
   char*                         pcHom=HOMEDIR(1);
   char*                         pcTmp=NULL;

   if (psTab==NULL || argc==0 || argv==NULL || pcPgm==NULL || pcHlp==NULL || pfOut==NULL || pcDep==NULL || pcOpt==NULL || pcEnt==NULL ||
       strlen(pcPgm)==0 || strlen(pcHlp)==0 || strlen(pcPgm)>CLEMAX_PGMLEN) return(24);

   for (i=0;pcPgm[i];i++) acPgm[i]=toupper(pcPgm[i]);

   pfTmp=fopen("DD:STDENV","r");
   if (pfTmp!=NULL) {
      memset(acCnf,0,sizeof(acCnf));
      while (fgets(acCnf,sizeof(acCnf)-1,pfTmp)!=NULL) {
         pcCnf=acCnf+strlen(acCnf);
         while (isspace(*(pcCnf-1))) {
            pcCnf--; *pcCnf=EOS;
         }
         if (putenv(acCnf)) {
            fprintf(pfOut,"Put variable (%s) to environment failed\n",acCnf);
            fclose(pfTmp);
            return(16);
         } else {
            fprintf(pfOut,"Put variable (%s) to environment successful\n",acCnf);
         }
      }
      fclose(pfTmp);
   }

   errno=0;
   sprintf(acCnf,"%s_DEFAULT_OWNER_ID",acPgm);
   pcCnf=getenv(acCnf);
   if (pcCnf!=NULL && strlen(pcCnf) && strlen(pcCnf)<sizeof(acOwn)) strcpy(acOwn,pcCnf); else strcpy(acOwn,pcOwn);
   sprintf(acCnf,"%s_CONFIG_FILE",acPgm);
   pcCnf=getenv(acCnf);
   if (pcCnf==NULL) {

#ifdef __HOST__
      {
         for (j=i=0;pcPgm[i] && j<8;i++) {
            if (isalnum(pcPgm[i])) {
               acCnf[j]=toupper(pcPgm[i]);
               j++;
            }
         }
         strcpy(&acCnf[j],".CONFIG");
      }
#else
      if (pcHom!=NULL && strlen(pcHom)) {
         sprintf(acCnf,".%s.config",pcPgm);
         pfTmp=fopen(acCnf,"r");
         if (pfTmp==NULL) {
            sprintf(acCnf,"%s.%s.config",pcHom,pcPgm);
         } else {
            fclose(pfTmp);
         }
      } else {
         sprintf(acCnf,".%s.config",pcPgm);
      }
      for (i=0;acCnf[i];i++) acCnf[i]=tolower(acCnf[i]);
#endif
      pcCnf=acCnf;
   }
   psCnf=psCnfOpn(pfOut,isCas,pcPgm,pcCnf);
   if (psCnf==NULL) return(11);

   sprintf(acCnf,"%s.owner.id",pcPgm);
   pcCnf=pcCnfGet(psCnf,acCnf);
   if (pcCnf!=NULL && strlen(pcCnf) && strlen(pcCnf)<sizeof(acOwn)) strcpy(acOwn,pcCnf);

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

   sprintf(acCnf,"%s.%s.trace",acOwn,pcPgm);
   pcCnf=pcCnfGet(psCnf,acCnf);
   if (pcCnf!=NULL && strxcmp(isCas,pcCnf,"ON",0,0,FALSE)==0) {
      sprintf(acCnf,"%s.%s.trace.file",acOwn,pcPgm);
      pcCnf=pcCnfGet(psCnf,acCnf);
      if (pcCnf!=NULL && strlen(pcCnf)) {
         pfTrh=fopen(pcCnf,"w");
         if (pfTrh==NULL) {
            fprintf(pfOut,"Open of trace file (%s) failed\n",pcCnf);
         } else pfTrc=pfTrh;
      }
   } else pfTrc=NULL;

   for (i=0; psTab[i].pcKyw!=NULL; i++) {
      if (psTab[i].psTab==NULL || psTab[i].pvClp==NULL || psTab[i].pvPar==NULL ||
          psTab[i].pfIni==NULL || psTab[i].pfMap==NULL || psTab[i].pfRun==NULL || psTab[i].pfFin==NULL ||
          psTab[i].pcMan==NULL || psTab[i].pcHlp==NULL || strlen(psTab[i].pcKyw)==0 || strlen(psTab[i].pcMan)==0 || strlen(psTab[i].pcHlp)==0) {
         fprintf(pfOut,"Row %d of command table not initialized properly!\n",i);
         ERROR(12);
      }
   }
   if (i==0) {
      fprintf(pfOut,"Command table is empty!\n");
      ERROR(12);
   }

   if (argc<2) {
      if (pcDef!=NULL && strlen(pcDef)) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            fprintf(pfOut,"Memory allocation for argument list to run the default command '%s' failed!\n",pcDef);
            ERROR(16);
         }
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc=2; argv=ppArg;
      } else {
         fprintf(pfOut,"Command or built-in function required!\n");
         vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
         ERROR(8);
      }
   }

   if (argv[1][0]=='-') argv[1]++;
   if (argv[1][0]=='-') argv[1]++;

EVALUATE:
   if (strxcmp(isCas,argv[1],"LICENSE",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"License of program \'%s\':\n",pcPgm);
         if (pcLic==NULL) {
            fprintf(pfOut,"No license information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcLic);
         }
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'LICENSE\' not valid\n");
      fprintf(pfOut,"%s %s LICENSE\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"VERSION",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Version for program \'%s\':\n",pcPgm);
         if (pcVsn==NULL) {
            fprintf(pfOut,"No version information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcVsn);
         }
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'VERSION\' not valid\n");
      fprintf(pfOut,"%s %s VERSION\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"ABOUT",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"About program \'%s\':\n",pcPgm);
         if (pcAbo==NULL) {
            fprintf(pfOut,"No about information available\n");
         } else {
            fprintf(pfOut,"%s\n",pcAbo);
         }
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'ABOUT\' not valid\n");
      fprintf(pfOut,"%s %s ABOUT\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"LEXEM",0,0,FALSE)==0) {
      if (argc==2) {
         siErr=siCleSimpleInit(pfOut,pcDep,pcOpt,pcEnt,&pvHdl);
         if (siErr) ERROR(siErr);
         fprintf(pfOut,"Lexemes (regular expressions) for argument list or parameter file:\n");
         siErr=siClpLexem(pvHdl,pfOut);
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'LEXEM\' not valid\n");
      fprintf(pfOut,"%s %s LEXEM\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"GRAMMAR",0,0,FALSE)==0) {
      if (argc==2) {
         siErr=siCleSimpleInit(pfOut,pcDep,pcOpt,pcEnt,&pvHdl);
         if (siErr) ERROR(siErr);
         fprintf(pfOut,"Grammar for argument list, parameter file or property file\n");
         siErr=siClpGrammar(pvHdl,pfOut);
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'GRAMMAR\' not valid\n");
      fprintf(pfOut,"%s %s GRAMMAR\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"SYNTAX",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Syntax for program \'%s\':\n",pcPgm);
         vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
         ERROR(0);
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
               fprintf(pfOut,"Syntax for built-in function \'SYNTAX\' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s SYNTAX %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               ERROR(8);
            }
         } else {
            fprintf(pfOut,"Syntax for built-in function \'SYNTAX\' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s SYNTAX %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            ERROR(8);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,0,'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
               if (siErr) ERROR(siErr);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Syntax for command \'%s\':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Syntax for argument \'%s\':\n",argv[2]);
               }
               vdPrnCommandSyntax(pvHdl,pfOut,argv[0],argv[2],pcDep,siDep);
               ERROR(0);
            }
         }
         if (pcDef!=NULL && strlen(pcDef)) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(pcDef)+strlen(argv[2]+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed!\n",pcDef,argv[2]);
                     ERROR(16);
                  }
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Syntax for argument \'%s\':\n",pcPat);
                  vdPrnCommandSyntax(pvHdl,pfOut,argv[0],pcPat,pcDep,siDep);
                  free(pcPat);
                  ERROR(0);
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'SYNTAX\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s SYNTAX %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"HELP",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Help for program \'%s\':\n",pcPgm);
         vdPrnStaticHelp(pfOut,psTab,argv[0],isCas,pcDep);
         ERROR(0);
      } else if (argc>=3) {
         if (argc==3) {
            if (strxcmp(isCas,argv[2],"MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-MAN",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--MAN",0,0,FALSE)==0) {
               fprintf(pfOut,"Help for program \'%s\':\n",pcPgm);
               fprintf(pfOut,"%s\n",pcMan);
               ERROR(0);
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
               fprintf(pfOut,"Syntax for built-in function \'HELP\' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s HELP %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               ERROR(8);
            }
         } else {
            fprintf(pfOut,"Syntax for built-in function \'HELP\' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s HELP %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            ERROR(8);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,0,'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
               if (siErr) ERROR(siErr);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Help for command \'%s\':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Help for argument \'%s\':\n",argv[2]);
               }
               vdPrnCommandHelp(pvHdl,argv[2],siDep,TRUE);
               if (siDep==0) {
                  fprintf(pfOut,"ARGUMENTS\n");
                  vdPrnCommandHelp(pvHdl,argv[2],1,FALSE);
               }
               ERROR(0);
            }
         }
         if (pcDef!=NULL && strlen(pcDef)) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(psTab[i].pcKyw)+strlen(argv[2]+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed!\n",psTab[i].pcKyw,argv[2]);
                     ERROR(16);
                  }
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",psTab[i].pcKyw,argv[2]);
                  fprintf(pfOut,"Help for argument \'%s\':\n",pcPat);
                  vdPrnCommandHelp(pvHdl,pcPat,siDep,TRUE);
                  if (siDep==0) {
                     fprintf(pfOut,"ARGUMENTS\n");
                     vdPrnCommandHelp(pvHdl,pcPat,1,FALSE);
                  }
                  free(pcPat);
                  ERROR(0);
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'HELP\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s HELP %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL] [MAN]\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"MANPAGE",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Manual page for program \'%s\':\n\n",pcPgm);
         vdCleManProgram(pfOut,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,FALSE,TRUE);
         ERROR(0);
      } else if (argc==3) {
         const char*                pcCmd=NULL;
         const char*                pcFil=NULL;
         int                        isMan=FALSE;
         int                        isAll=FALSE;
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
            isMan=TRUE; errno=0; pfDoc=fopen(pcFil,"w");
            if (pfDoc==NULL) {
               fprintf(pfOut,"Open of manual page file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(8);
            }
         } else {
            pcCmd=argv[2];
            isMan=FALSE;
            pfDoc=pfOut;
         }
         if (strxcmp(isCas,pcCmd,"ALL",0,0,FALSE)==0 || strxcmp(isCas,pcCmd,"-ALL",0,0,FALSE)==0 || strxcmp(isCas,pcCmd,"--ALL",0,0,FALSE)==0) {
            isAll=TRUE;
            if (isMan==FALSE) fprintf(pfOut,"Manual page for program \'%s\':\n\n",pcPgm);
            vdCleManProgram(pfDoc,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for program \'%s\' successfully written to \'%s\'\n",pcPgm,pcFil);
         }
         if (strxcmp(isCas,pcCmd,"SYNTAX",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'SYNTAX\':\n\n");
            vdCleManFunction(pfDoc,"~","4.1" ,"SYNTAX"  ,HLP_CLE_SYNTAX  ,acOwn,pcPgm,SYN_CLE_SYNTAX,MAN_CLE_SYNTAX,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'SYNTAX\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"HELP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'HELP\':\n\n");
            vdCleManFunction(pfDoc,"~","4.2" ,"HELP"    ,HLP_CLE_HELP    ,acOwn,pcPgm,SYN_CLE_HELP,MAN_CLE_HELP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'HELP\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"MANPAGE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'MANPAGE\':\n\n");
            vdCleManFunction(pfDoc,"~","4.3" ,"MANPAGE" ,HLP_CLE_MANPAGE ,acOwn,pcPgm,SYN_CLE_MANPAGE,MAN_CLE_MANPAGE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'MANPAGE\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"GENDOCU",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'GENDOCU\':\n\n");
            vdCleManFunction(pfDoc,"~","4.4" ,"GENDOCU" ,HLP_CLE_GENDOCU ,acOwn,pcPgm,SYN_CLE_GENDOCU,MAN_CLE_GENDOCU,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'GENDOCU\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"GENPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'GENPROP\':\n\n");
            vdCleManFunction(pfDoc,"~","4.5" ,"GENPROP" ,HLP_CLE_GENPROP ,acOwn,pcPgm,SYN_CLE_GENPROP,MAN_CLE_GENPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'GENPROP\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"SETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'SETPROP\':\n\n");
            vdCleManFunction(pfDoc,"~","4.6" ,"SETPROP" ,HLP_CLE_SETPROP ,acOwn,pcPgm,SYN_CLE_SETPROP,MAN_CLE_SETPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'SETPROP\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"CHGPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'CHGPROP\':\n\n");
            vdCleManFunction(pfDoc,"~","4.7" ,"CHGPROP" ,HLP_CLE_CHGPROP ,acOwn,pcPgm,SYN_CLE_CHGPROP,MAN_CLE_CHGPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'CHGPROP\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"DELPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'DELPROP\':\n\n");
            vdCleManFunction(pfDoc,"~","4.8" ,"DELPROP" ,HLP_CLE_DELPROP ,acOwn,pcPgm,SYN_CLE_DELPROP,MAN_CLE_DELPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'DELPROP\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"GETPROP",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'GETPROP\':\n\n");
            vdCleManFunction(pfDoc,"~","4.9" ,"GETPROP" ,HLP_CLE_GETPROP ,acOwn,pcPgm,SYN_CLE_GETPROP,MAN_CLE_GETPROP,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'GETPROP\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"SETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'SETOWNER\':\n\n");
            vdCleManFunction(pfDoc,"~","4.10" ,"SETOWNER",HLP_CLE_SETOWNER,acOwn,pcPgm,SYN_CLE_SETOWNER,MAN_CLE_SETOWNER,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'SETOWNER\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"GETOWNER",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'GETOWNER\':\n\n");
            vdCleManFunction(pfDoc,"~","4.11","GETOWNER",HLP_CLE_GETOWNER,acOwn,pcPgm,SYN_CLE_GETOWNER,MAN_CLE_GETOWNER,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'GETOWNER\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"SETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'SETENV\':\n\n");
            vdCleManFunction(pfDoc,"~","4.12" ,"SETENV",HLP_CLE_SETENV,acOwn,pcPgm,SYN_CLE_SETENV,MAN_CLE_SETENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'SETENV\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"GETENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'GETENV\':\n\n");
            vdCleManFunction(pfDoc,"~","4.13" ,"GETENV",HLP_CLE_GETENV,acOwn,pcPgm,SYN_CLE_GETENV,MAN_CLE_GETENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'GETENV\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"DELENV",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'DELENV\':\n\n");
            vdCleManFunction(pfDoc,"~","4.14" ,"DELENV",HLP_CLE_DELENV,acOwn,pcPgm,SYN_CLE_DELENV,MAN_CLE_DELENV,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'DELENV\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"TRACE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'TRACE\':\n\n");
            vdCleManFunction(pfDoc,"~","4.15","TRACE"   ,HLP_CLE_TRACE   ,acOwn,pcPgm,SYN_CLE_TRACE,MAN_CLE_TRACE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'TRACE\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"CONFIG",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'CONFIG\':\n\n");
            vdCleManFunction(pfDoc,"~","4.16","CONFIG"  ,HLP_CLE_CONFIG  ,acOwn,pcPgm,SYN_CLE_CONFIG,MAN_CLE_CONFIG,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'CONFIG\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"GRAMMAR",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'GRAMMAR\':\n\n");
            vdCleManFunction(pfDoc,"~","4.17","GRAMMAR" ,HLP_CLE_GRAMMAR ,acOwn,pcPgm,SYN_CLE_GRAMMAR,MAN_CLE_GRAMMAR,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'GRAMMAR\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"LEXEM",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'LEXEM\':\n\n");
            vdCleManFunction(pfDoc,"~","4.18","LEXEM"   ,HLP_CLE_LEXEM   ,acOwn,pcPgm,SYN_CLE_LEXEM,MAN_CLE_LEXEM,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'LEXEM\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"LICENSE",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'LICENSE\':\n\n");
            vdCleManFunction(pfDoc,"~","4.19","LICENSE" ,HLP_CLE_LICENSE ,acOwn,pcPgm,SYN_CLE_LICENSE,MAN_CLE_LICENSE,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'LICENSE\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"VERSION",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'VERSION\':\n\n");
            vdCleManFunction(pfDoc,"~","4.20","VERSION" ,HLP_CLE_VERSION ,acOwn,pcPgm,SYN_CLE_VERSION,MAN_CLE_VERSION,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'VERSION\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         if (strxcmp(isCas,pcCmd,"ABOUT",0,0,FALSE)==0 || isAll) {
            if (isMan==FALSE) fprintf(pfOut,"Manual page for built-in function \'ABOUT\':\n\n");
            vdCleManFunction(pfDoc,"~","4.21","ABOUT"   ,HLP_CLE_ABOUT   ,acOwn,pcPgm,SYN_CLE_ABOUT,MAN_CLE_ABOUT,isMan,TRUE);
            if (isMan==TRUE) fprintf(pfOut,"Manual page for built-in function \'ABOUT\' successfully written to \'%s\'\n",pcFil);
            if (isAll==FALSE) ERROR(0);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (isAll) {
               if (psTab[i].siFlg) {
                  pcCmd=psTab[i].pcKyw;
               } else {
                  pcCmd="";
               }
            }
            if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,'.',TRUE)==0) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
               if (siErr) ERROR(siErr);
               if (isMan==FALSE) {
                  if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                     fprintf(pfOut,"Manual page for command \'%s\':\n\n",pcCmd);
                  } else {
                     fprintf(pfOut,"Manual page for argument \'%s\':\n\n",pcCmd);
                  }
               }
               vdPrnCommandManpage(pvHdl,pfDoc,pcCmd,i,isMan,TRUE);
               vdClpClose(pvHdl); pvHdl=NULL;
               if (isMan==TRUE) {
                  if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                     fprintf(pfOut,"Manual page for command \'%s\' successfully written to \'%s\'\n",pcCmd,pcFil);
                  } else {
                     fprintf(pfOut,"Manual page for argument \'%s\' successfully written to \'%s\'\n",pcCmd,pcFil);
                  }
               }
               if (isAll==FALSE) ERROR(0);
            }
         }
         if (isAll==TRUE) ERROR(0);

         if (pcDef!=NULL && strlen(pcDef)) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(pcDef)+strlen(pcCmd+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed!\n",pcDef,argv[2]);
                     ERROR(16);
                  }
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",pcDef,pcCmd);
                  fprintf(pfOut,"Manual page for argument \'%s\':\n\n",pcPat);
                  vdPrnCommandManpage(pvHdl,pfDoc,pcPat,i,isMan,TRUE);
                  if (isMan==TRUE) {
                     fprintf(pfOut,"Manual page for argument \'%s\' successfully written to \'%s\'\n",pcPat,pcFil);
                  }
                  free(pcPat);
                  ERROR(0);
               }
            }
         }

         pcFil=argv[2];
         isMan=TRUE; errno=0; pfDoc=fopen(pcFil,"w");
         if (pfDoc==NULL) {
            fprintf(pfOut,"Open of manual page file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
            ERROR(8);
         }
         vdCleManProgram(pfDoc,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,TRUE,TRUE);
         fprintf(pfOut,"Manual page for program \'%s\' successfully written to \'%s\'\n",pcPgm,pcFil);
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'MANPAGE\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s MANPAGE %s[.path]\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s MANPAGE function\n",pcDep,argv[0]);
      fprintf(pfOut,"%s %s MANPAGE\n",pcDep,argv[0]);
      ERROR(8);
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
               fprintf(pfOut,"Syntax for built-in function \'GENDOCU\' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s GENDOCU %s[.path]=filename [NONBR]\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               fprintf(pfOut,"%s %s GENDOCU filename [NONBR]\n",pcDep,argv[0]);
               ERROR(8);
            }
         }
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
         } else {
            pcFil=argv[2]; pcCmd=NULL;
         }
         errno=0;
         pfDoc=fopen(pcFil,"w");
         if (pfDoc==NULL) {
            fprintf(pfOut,"Open of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
            ERROR(8);
         }
         if (pcCmd!=NULL) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,'.',TRUE)==0) {
                  errno=0;
                  siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
                  if (siErr) ERROR(siErr);
                  sprintf(acNum,"2.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,pcCmd,acNum,TRUE,FALSE,isNbr);
                  if (siErr) {
                     fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                     ERROR(2);
                  } else {
                     if (strlen(pcCmd)==strlen(psTab[i].pcKyw)) {
                        fprintf(pfOut,"Documentation for command \'%s\' successfully created\n",pcCmd);
                     } else {
                        fprintf(pfOut,"Documentation for argument \'%s\' successfully created\n",pcCmd);
                     }
                     ERROR(0);
                  }
               }
            }
            if (pcDef!=NULL && strlen(pcDef)) {
               for (i=0;psTab[i].pcKyw!=NULL;i++) {
                  if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                     char* pcPat=(char*)malloc(strlen(pcDef)+strlen(pcCmd+2));
                     if (pcPat==NULL) {
                        fprintf(pfOut,"Memory allocation for path '%s.%s' failed!\n",pcDef,argv[2]);
                        ERROR(16);
                     }
                     errno=0;
                     siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
                     if (siErr) {
                        free(pcPat);
                        ERROR(siErr);
                     }
                     sprintf(acNum,"2.%d.",i+1);
                     sprintf(pcPat,"%s.%s",pcDef,pcCmd);
                     siErr=siClpDocu(pvHdl,pfDoc,pcPat,acNum,TRUE,FALSE,isNbr);
                     if (siErr) {
                        fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                        free(pcPat);
                        ERROR(2);
                     } else {
                        fprintf(pfOut,"Documentation for argument \'%s\' successfully created\n",pcPat);
                        free(pcPat);
                        ERROR(0);
                     }
                  }
               }
            }
         } else {
            errno=0;
            if (pcCov!=NULL && strlen(pcCov)) {
               fprintf(pfDoc,"%s\n\n",pcCov);
            } else {
               sprintf(acNum,"\'%s\' - User Manual",acPgm); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"="); fprintf(pfDoc,"\n");
               fprintf(pfDoc,":doctype: book\n\n");
            }
            if (isNbr) {
               fprintf(pfDoc,"1. COMMAND LINE PROCESSOR\n");
               fprintf(pfDoc,"-------------------------\n\n");
            } else {
               fprintf(pfDoc,"COMMAND LINE PROCESSOR\n");
               fprintf(pfDoc,"----------------------\n\n");
            }
            fprintf(pfDoc,"%s\n\n",MAN_CLE_MAIN);
            fprintf(pfDoc,"indexterm:[Command line processor]\n\n\n");

            vdCleManProgram(pfDoc,psTab,acOwn,pcPgm,pcHlp,pcMan,pcDep,pcOpt,FALSE,isNbr);

            if (isNbr) {
               sprintf(acNum,"3. Available commands"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            } else {
               sprintf(acNum,"Available commands"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            }
            fprintf(pfDoc,"%s\n\n",MAN_CLE_COMMANDS);
            fprintf(pfDoc,"indexterm:[Available commands]\n\n\n");

            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
               if (siErr) ERROR(siErr);
               if (psTab[i].siFlg) {
                  sprintf(acNum,"3.%d.",i+1);
                  siErr=siClpDocu(pvHdl,pfDoc,psTab[i].pcKyw,acNum,TRUE,FALSE,isNbr);
                  vdClpClose(pvHdl); pvHdl=NULL;
                  if (siErr<0) {
                     fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
                     ERROR(2);
                  }
               }
            }

            if (isNbr) {
               sprintf(acNum,"4. Available built-in functions"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            } else {
               sprintf(acNum,"Available built-in functions"); l=strlen(acNum); fprintf(pfDoc,"%s\n",acNum);
               for (i=0;i<l;i++) fprintf(pfDoc,"-"); fprintf(pfDoc,"\n\n");
            }
            fprintf(pfDoc,"%s\n\n",MAN_CLE_FUNCTIONS);
            fprintf(pfDoc,"indexterm:[Available built-in functions]\n\n\n");

            vdCleManFunction(pfDoc,"~","4.1" ,"SYNTAX"  ,HLP_CLE_SYNTAX  ,acOwn,pcPgm,SYN_CLE_SYNTAX  ,MAN_CLE_SYNTAX  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.2" ,"HELP"    ,HLP_CLE_HELP    ,acOwn,pcPgm,SYN_CLE_HELP    ,MAN_CLE_HELP    ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.3" ,"MANPAGE" ,HLP_CLE_MANPAGE ,acOwn,pcPgm,SYN_CLE_MANPAGE ,MAN_CLE_MANPAGE ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.4" ,"GENDOCU" ,HLP_CLE_GENDOCU ,acOwn,pcPgm,SYN_CLE_GENDOCU ,MAN_CLE_GENDOCU ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.5" ,"GENPROP" ,HLP_CLE_GENPROP ,acOwn,pcPgm,SYN_CLE_GENPROP ,MAN_CLE_GENPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.6" ,"SETPROP" ,HLP_CLE_SETPROP ,acOwn,pcPgm,SYN_CLE_SETPROP ,MAN_CLE_SETPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.7" ,"CHGPROP" ,HLP_CLE_CHGPROP ,acOwn,pcPgm,SYN_CLE_CHGPROP ,MAN_CLE_CHGPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.8" ,"DELPROP" ,HLP_CLE_DELPROP ,acOwn,pcPgm,SYN_CLE_DELPROP ,MAN_CLE_DELPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.9" ,"GETPROP" ,HLP_CLE_GETPROP ,acOwn,pcPgm,SYN_CLE_GETPROP ,MAN_CLE_GETPROP ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.10","SETOWNER",HLP_CLE_SETOWNER,acOwn,pcPgm,SYN_CLE_SETOWNER,MAN_CLE_SETOWNER,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.11","GETOWNER",HLP_CLE_GETOWNER,acOwn,pcPgm,SYN_CLE_GETOWNER,MAN_CLE_GETOWNER,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.12","SETENV"  ,HLP_CLE_SETENV  ,acOwn,pcPgm,SYN_CLE_SETENV  ,MAN_CLE_SETENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.13","GETENV"  ,HLP_CLE_GETENV  ,acOwn,pcPgm,SYN_CLE_GETENV  ,MAN_CLE_GETENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.14","DELENV"  ,HLP_CLE_DELENV  ,acOwn,pcPgm,SYN_CLE_DELENV  ,MAN_CLE_DELENV  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.15","TRACE"   ,HLP_CLE_TRACE   ,acOwn,pcPgm,SYN_CLE_TRACE   ,MAN_CLE_TRACE   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.16","CONFIG"  ,HLP_CLE_CONFIG  ,acOwn,pcPgm,SYN_CLE_CONFIG  ,MAN_CLE_CONFIG  ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.17","GRAMMAR" ,HLP_CLE_GRAMMAR ,acOwn,pcPgm,SYN_CLE_GRAMMAR ,MAN_CLE_GRAMMAR ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.18","LEXEM"   ,HLP_CLE_LEXEM   ,acOwn,pcPgm,SYN_CLE_LEXEM   ,MAN_CLE_LEXEM   ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.19","LICENSE" ,HLP_CLE_LICENSE ,acOwn,pcPgm,SYN_CLE_LICENSE ,MAN_CLE_LICENSE ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.20","VERSION" ,HLP_CLE_VERSION ,acOwn,pcPgm,SYN_CLE_VERSION ,MAN_CLE_VERSION ,FALSE,isNbr);
            vdCleManFunction(pfDoc,"~","4.21","ABOUT"   ,HLP_CLE_ABOUT   ,acOwn,pcPgm,SYN_CLE_ABOUT   ,MAN_CLE_ABOUT   ,FALSE,isNbr);

            s=1;
            siErr=siCleSimpleInit(pfOut,pcDep,pcOpt,pcEnt,&pvHdl);
            if (siErr) ERROR(siErr);
            fprintf(pfDoc,"[[appendix-lexem]]\n");
            fprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"LEXEM\n");
            fprintf(pfDoc,"-----\n\n");
            fprintf(pfDoc,"%s\n",MAN_CLE_APPENDIX_LEXEM);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"Lexemes (regular expressions) for argument list or parameter file\n");
            siErr=siClpLexem(pvHdl,pfDoc); s++;
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            fprintf(pfDoc,"indexterm:[Appendix Lexem]\n\n\n");
            if (siErr<0) {
               fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(2);
            }

            fprintf(pfDoc,"[[appendix-grammar]]\n");
            fprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"GRAMMAR\n");
            fprintf(pfDoc,"-------\n\n");
            fprintf(pfDoc,"%s\n",MAN_CLE_APPENDIX_GRAMMAR);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"Grammar for argument list, parameter file or property file\n");
            siErr=siClpGrammar(pvHdl,pfDoc); s++;
            vdClpClose(pvHdl); pvHdl=NULL;
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            fprintf(pfDoc,"indexterm:[Appendix Grammar]\n\n\n");
            if (siErr<0) {
               fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(2);
            }

            vdClpClose(pvHdl); pvHdl=NULL;

            fprintf(pfDoc,"[[appendix-properties]]\n");
            fprintf(pfDoc,"[appendix]\n");
            fprintf(pfDoc,"PROPERTIES\n");
            fprintf(pfDoc,"----------\n\n");
            fprintf(pfDoc,"%s\n",MAN_CLE_APPENDIX_PROPERTIES);
            fprintf(pfDoc,"------------------------------------------------------------------------\n");
            fprintf(pfDoc,"\n# Property file for: %s.%s #\n\n",acOwn,pcPgm);
            fprintf(pfDoc,HLP_CLE_PROPFIL);
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                       &pvHdl,acFil,&siFil);
               if (siErr) ERROR(siErr);
               siErr=siClpProperties(pvHdl,FALSE,10,psTab[i].pcKyw,pfDoc);
               vdClpClose(pvHdl); pvHdl=NULL;
            }
            fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
            fprintf(pfDoc,"indexterm:[Appendix Properties]\n\n\n");
            if (siErr<0) {
               fprintf(pfOut,"Creation of documentation file (%s) failed (%d - %s)\n",pcFil,errno,strerror(errno));
               ERROR(2);
            }

            if (pcVsn!=NULL && strlen(pcVsn)) {
               fprintf(pfDoc,"[[appendix-version]]\n");
               fprintf(pfDoc,"[appendix]\n");
               fprintf(pfDoc,"VERSION\n");
               fprintf(pfDoc,"-------\n\n");
               fprintf(pfDoc,"%s\n",MAN_CLE_APPENDIX_VERSION);
               fprintf(pfDoc,"------------------------------------------------------------------------\n");
               fprintf(pfDoc,"%s",pcVsn); s++;
               fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
               fprintf(pfDoc,"indexterm:[Appendix Version]\n\n\n");
            }
            if (pcAbo!=NULL && strlen(pcAbo)) {
               fprintf(pfDoc,"[[appendix-about]]\n");
               fprintf(pfDoc,"[appendix]\n");
               fprintf(pfDoc,"ABOUT\n");
               fprintf(pfDoc,"-----\n\n");
               fprintf(pfDoc,"%s\n",MAN_CLE_APPENDIX_ABOUT);
               fprintf(pfDoc,"------------------------------------------------------------------------\n");
               fprintf(pfDoc,"%s",pcAbo); s++;
               fprintf(pfDoc,"------------------------------------------------------------------------\n\n");
               fprintf(pfDoc,"indexterm:[Appendix About]\n\n\n");
            }

            if (pcGls!=NULL) {
               fprintf(pfDoc,"[glossary]\n");
               fprintf(pfDoc,"GLOSSARY\n");
               fprintf(pfDoc,"--------\n\n");
               fprintf(pfDoc,"%s\n",MAN_CLE_GLOSSARY);
               fprintf(pfDoc,"%s\n\n",pcGls);
               fprintf(pfDoc,"indexterm:[Glossary]\n\n\n");
            }

            fprintf(pfDoc,"[index]\n");
            fprintf(pfDoc,"INDEX\n");
            fprintf(pfDoc,"-----\n\n");

            if (pcFin!=NULL && strlen(pcFin)) {
               fprintf(pfDoc,"\n\n%s\n\n",pcFin);
            } else {
               fprintf(pfDoc,"[colophon]\n");
               fprintf(pfDoc,"COLOPHON\n");
               fprintf(pfDoc,"--------\n\n");
               fprintf(pfDoc,"Owner:   %s\n",pcOwn);
               fprintf(pfDoc,"Program: %s\n",pcPgm);
               fprintf(pfDoc,"\n\n");
            }
            fprintf(pfOut,"Documentation for program \'%s\' successfully created\n",pcPgm);
            ERROR(0);
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'GENDOCU\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s GENDOCU %s[.path]=filename [NONBR]\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s GENDOCU filename [NONBR]\n",pcDep,argv[0]);
      ERROR(8);
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
         errno=0;
         pfPro=fopen(pcFil,"w");
         if (pfPro==NULL) {
            fprintf(pfOut,"Open of property file (%s) failed (%d-%s)\n",pcFil,errno,strerror(errno));
            ERROR(8);
         }
         if (pcCmd==NULL) fprintf(pfPro,"\n# Property file for: %s.%s #\n\n",acOwn,pcPgm);
                    else  fprintf(pfPro,"\n# Property file for: %s.%s.%s #\n\n",acOwn,pcPgm,pcCmd);
         fprintf(pfPro,HLP_CLE_PROPFIL);

         if (pcCmd==NULL) {
            errno=0;
            for (siErr=CLP_OK, i=0;psTab[i].pcKyw!=NULL && siErr==CLP_OK;i++) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                       &pvHdl,acFil,&siFil);
               if (siErr) ERROR(siErr);
               siErr=siClpProperties(pvHdl,FALSE,10,psTab[i].pcKyw,pfPro);
               vdClpClose(pvHdl); pvHdl=NULL;
            }
            if (siErr<0) {
               fprintf(pfOut,"Write property file (%s) for program \'%s\' failed (%d-%s)\n",pcFil,pcPgm,errno,strerror(errno));
               ERROR(2);
            } else {
               fprintf(pfOut,"Property file (%s) for program \'%s\' successfully written\n",pcFil,pcPgm);
               ERROR(0);
            }
         } else {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,0,FALSE)==0) {
                  siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                          &pvHdl,acFil,&siFil);
                  if (siErr) ERROR(siErr);
                  errno=0;
                  siErr=siClpProperties(pvHdl,FALSE,10,psTab[i].pcKyw,pfPro);
                  vdClpClose(pvHdl); pvHdl=NULL;
                  if (siErr<0) {
                     fprintf(pfOut,"Write property file (%s) for command \'%s\' failed (%d-%s)\n",pcFil,pcCmd,errno,strerror(errno));
                     ERROR(2);
                  } else {
                     fprintf(pfOut,"Property file (%s) for command \'%s\' successfully written\n",pcFil,pcCmd);
                     ERROR(0);
                  }
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'GENPROP\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s GENPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s GENPROP filename\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"SETPROP",0,0,FALSE)==0) {
      const char*                pcCmd=NULL;
      const char*                pcFil=NULL;
      if (argc==3) {
         pcFil=strchr(argv[2],'=');
         if (pcFil!=NULL) {
            *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
            for (i=0;psTab[i].pcKyw!=NULL && strxcmp(isCas,pcCmd,psTab[i].pcKyw,0,0,FALSE);i++);
            if (psTab[i].pcKyw==NULL) {
               fprintf(pfOut,"Syntax for built-in function \'SETPROP\' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               ERROR(8);
            }
            if (strlen(acOwn)+strlen(pcPgm)+strlen(pcCmd)+20>=sizeof(acCnf)) {
               fprintf(pfOut,"Configuration keyword '%s.%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,pcCmd,(unsigned)sizeof(acCnf));
               ERROR(8);
            }
            sprintf(acCnf,"%s.%s.%s.property.file",acOwn,pcPgm,pcCmd);
         } else {
            pcFil=argv[2]; pcCmd=NULL;
            if (strlen(acOwn)+strlen(pcPgm)+20>=sizeof(acCnf)) {
               fprintf(pfOut,"Configuration keyword '%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,(unsigned)sizeof(acCnf));
               ERROR(8);
            }
            sprintf(acCnf,"%s.%s.property.file",acOwn,pcPgm);
         }
         if (strlen(pcFil)==0) {
            fprintf(pfOut,"Syntax for built-in function \'SETPROP\' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            fprintf(pfOut,"File name was not specified. To delete a property file from the list, please use the function DELPROP [command]\n");
            ERROR(8);
         }
         siErr=siCnfSet(psCnf,pfOut,acCnf,pcFil,TRUE);
         if (siErr) ERROR(2); else {
            fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",acCnf,pcFil);
            ERROR(0);
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'SETPROP\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s SETPROP %s=filename\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s SETPROP filename\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"CHGPROP",0,0,FALSE)==0 || (pcDef!=NULL && strxcmp(isCas,pcDef,"flam",0,0,FALSE)==0 && strxcmp(isCas,argv[1],"DEFAULT",0,0,FALSE)==0)) {
      if (argc>=3) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,0,0,FALSE)==0) {
               char acPro[CLEMAX_CMDSIZ]="";
               for (j=3;j<argc;j++) {
                  if (strlen(acPro)+strlen(argv[j])+strlen(acOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw)+8>CLEMAX_CMDSIZ) {
                     fprintf(pfOut,"Argument list is too long!\n");
                     return(8);
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
                     pcTmp=0x00; pcTmp++;
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
               ERROR(siCleChangeProperties(psTab[i].pfIni,psTab[i].pvClp,pcHom,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                           acPro,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf));
            }
         }
      }
      if (pcDef!=NULL && strlen(pcDef)) {
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
               char acPro[CLEMAX_CMDSIZ]="";
               for (j=2;j<argc;j++) {
                  if (strlen(acPro)+strlen(argv[j])+strlen(acOwn)+strlen(pcPgm)+strlen(psTab[i].pcKyw)+5>CLEMAX_CMDSIZ) {
                     fprintf(pfOut,"Argument list is too long!\n");
                     return(8);
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
                     pcTmp=0x00; pcTmp++;
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
               ERROR(siCleChangeProperties(psTab[i].pfIni,psTab[i].pvClp,pcHom,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                           acPro,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf));
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'CHGPROP\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s CHGPROP %s property_list\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"DELPROP",0,0,FALSE)==0) {
      if (argc==2) {
         if (strlen(acOwn)+strlen(pcPgm)+20>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,(unsigned)sizeof(acCnf));
            ERROR(8);
         }
         sprintf(acCnf,"%s.%s.property.file",acOwn,pcPgm);
      } else if (argc==3) {
         for (i=0;psTab[i].pcKyw!=NULL && strxcmp(isCas,argv[2],psTab[i].pcKyw,0,0,FALSE);i++);
         if (psTab[i].pcKyw==NULL) {
            fprintf(pfOut,"Syntax for built-in function \'DELPROP\' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s DELPROP %s\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            ERROR(8);
         }
         if (strlen(acOwn)+strlen(pcPgm)+strlen(argv[2])+20>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.%s.property.file' too long (>=%u)\n",acOwn,pcPgm,argv[2],(unsigned)sizeof(acCnf));
            ERROR(8);
         }
         sprintf(acCnf,"%s.%s.%s.property.file",acOwn,pcPgm,argv[2]);
      } else {
         fprintf(pfOut,"Syntax for built-in function \'DELPROP\' not valid\n");
         for (i=0;psTab[i].pcKyw!=NULL ;i++) {
            if (psTab[i].siFlg) {
               fprintf(pfOut,"%s %s DELPROP %s\n",pcDep,argv[0],psTab[i].pcKyw);
            }
         }
         fprintf(pfOut,"%s %s DELPROP\n",pcDep,argv[0]);
         ERROR(8);
      }
      siErr=siCnfSet(psCnf,pfOut,acCnf,"",TRUE);
      if (siErr) ERROR(2); else {
         fprintf(pfOut,"Delete configuration keyword '%s' was successful\n",acCnf);
         ERROR(0);
      }
   } else if (strxcmp(isCas,argv[1],"GETPROP",0,0,FALSE)==0 || (pcDef!=NULL && strxcmp(isCas,pcDef,"flam",0,0,FALSE)==0 && strxcmp(isCas,argv[1],"LIST",0,0,FALSE)==0)) {
      if (argc==2) {
         fprintf(pfOut,"Properties for program \'%s\':\n",pcPgm);
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                    psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                    &pvHdl,acFil,&siFil);
            if (siErr) ERROR(siErr);
            vdPrnProperties(pvHdl,psTab[i].pcKyw,TRUE,10);
            vdClpClose(pvHdl); pvHdl=NULL;
         }
         ERROR(0);
      } else if (argc>=3) {
         if (argc==3) {
            siDep=1;  isSet=FALSE;
         } else if (argc==4) {
            if (argv[3][0]=='-') argv[3]++;
            if (argv[3][0]=='-') argv[3]++;
            if (strxcmp(isCas,argv[3],"ALL",0,0,FALSE)==0) {
               siDep=10; isSet=TRUE;
            } else if (strxcmp(isCas,argv[3],"DEPALL",0,0,FALSE)==0) {
                  siDep=1; isSet=FALSE;
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
               fprintf(pfOut,"Syntax for built-in function \'GETPROP\' not valid\n");
               for (i=0;psTab[i].pcKyw!=NULL ;i++) {
                  if (psTab[i].siFlg) {
                     fprintf(pfOut,"%s %s GETPROP %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n",pcDep,argv[0],psTab[i].pcKyw);
                  }
               }
               ERROR(8);
            }
         } else {
            fprintf(pfOut,"Syntax for built-in function \'GETPROP\' not valid\n");
            for (i=0;psTab[i].pcKyw!=NULL ;i++) {
               if (psTab[i].siFlg) {
                  fprintf(pfOut,"%s %s GETPROP %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n",pcDep,argv[0],psTab[i].pcKyw);
               }
            }
            ERROR(8);
         }
         for (i=0;psTab[i].pcKyw!=NULL;i++) {
            if (strxcmp(isCas,argv[2],psTab[i].pcKyw,0,'.',TRUE)==0) {
               siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                       psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                       &pvHdl,acFil,&siFil);
               if (siErr) ERROR(siErr);
               if (strlen(argv[2])==strlen(psTab[i].pcKyw)) {
                  fprintf(pfOut,"Properties for command \'%s\':\n",argv[2]);
               } else {
                  fprintf(pfOut,"Properties for argument \'%s\':\n",argv[2]);
               }
               vdPrnProperties(pvHdl,argv[2],isSet,siDep);
               ERROR(0);
            }
         }
         if (pcDef!=NULL && strlen(pcDef)) {
            for (i=0;psTab[i].pcKyw!=NULL;i++) {
               if (strxcmp(isCas,pcDef,psTab[i].pcKyw,0,0,FALSE)==0) {
                  char* pcPat=(char*)malloc(strlen(pcDef)+strlen(argv[2]+2));
                  if (pcPat==NULL) {
                     fprintf(pfOut,"Memory allocation for path '%s.%s' failed!\n",pcDef,argv[2]);
                     ERROR(16);
                  }
                  siErr=siClePropertyInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,
                                          psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,
                                          &pvHdl,acFil,&siFil);
                  if (siErr) {
                     free(pcPat);
                     ERROR(siErr);
                  }
                  sprintf(pcPat,"%s.%s",pcDef,argv[2]);
                  fprintf(pfOut,"Properties for argument \'%s\':\n",pcPat);
                  vdPrnProperties(pvHdl,pcPat,isSet,siDep);
                  free(pcPat);
                  ERROR(0);
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'GETPROP\' not valid\n");
      for (i=0;psTab[i].pcKyw!=NULL ;i++) {
         if (psTab[i].siFlg) {
            fprintf(pfOut,"%s %s GETPROP %s[.path] [DEPTH1 | DEPTH2 | ... | DEPTH9 | ALL]\n",pcDep,argv[0],psTab[i].pcKyw);
         }
      }
      fprintf(pfOut,"%s %s GETPROP\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"SETOWNER",0,0,FALSE)==0) {
      if (argc==3) {
         sprintf(acCnf,"%s.owner.id",pcPgm);
         siErr=siCnfSet(psCnf,pfOut,acCnf,argv[2],TRUE);
         if (siErr) ERROR(2); else {
            fprintf(pfOut,"Setting configuration key word '%s' to value '%s' was successful\n",acCnf,argv[2]);
            ERROR(0);
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'SETOWNER\' not valid\n");
      fprintf(pfOut,"%s %s SETOWNER name\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"GETOWNER",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Current owner id for \'%s\' is: %s\n",argv[0],acOwn);
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'GETOWNER\' not valid\n");
      fprintf(pfOut,"%s %s GETOWNER\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"SETENV",0,0,FALSE)==0) {
      if (argc==3) {
         const char* pcVal=NULL;
         const char* pcEnv=NULL;
         pcVal=strchr(argv[2],'=');
         if (pcVal!=NULL) {
            *((char*)pcVal)=EOS; pcVal++; pcEnv=argv[2];
         } else {
            fprintf(pfOut,"Syntax for built-in function \'SETENV\' not valid\n");
            fprintf(pfOut,"%s %s SETENV variable=value\n",pcDep,argv[0]);
            ERROR(8);
         }
         if (strlen(acOwn)+strlen(pcPgm)+strlen(pcEnv)+8>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.envar.%s' too long (>=%u)\n",acOwn,pcPgm,pcEnv,(unsigned)sizeof(acCnf));
            ERROR(8);
         }
         sprintf(acCnf,"%s.%s.envar.%s",acOwn,pcPgm,pcEnv);
         siErr=siCnfSet(psCnf,pfOut,acCnf,pcVal,TRUE);
         if (siErr) ERROR(2); else {
            fprintf(pfOut,"Setting environment variable '%s' to value '%s' was successful\n",acCnf,pcVal);
            ERROR(0);
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'SETENV\' not valid\n");
      fprintf(pfOut,"%s %s SETENV variable=value\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"GETENV",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Current environment variables for owner \'%s\':\n",acOwn);
         siCnt=siCnfPrnEnv(psCnf,pfOut,pcDep,acOwn,pcPgm);
         if (siCnt) {
            fprintf(pfOut,"Defined in file \'%s\'\n",psCnf->acFil);
         } else {
            fprintf(pfOut,"No environment variables defined in file \'%s\' for owner \'%s\'\n",psCnf->acFil,acOwn);
         }
         ERROR(0);
      }
      fprintf(pfOut,"Syntax for built-in function \'GETENV\' not valid\n");
      fprintf(pfOut,"%s %s GETENV\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"DELENV",0,0,FALSE)==0) {
      if (argc==3) {
         if (strlen(acOwn)+strlen(pcPgm)+strlen(argv[2])+10>=sizeof(acCnf)) {
            fprintf(pfOut,"Configuration keyword '%s.%s.envar.%s' too long (>=%u)\n",acOwn,pcPgm,argv[2],(unsigned)sizeof(acCnf));
            ERROR(8);
         }
         sprintf(acCnf,"%s.%s.envar.%s",acOwn,pcPgm,argv[2]);
         siErr=siCnfSet(psCnf,pfOut,acCnf,"",TRUE);
         if (siErr) ERROR(2); else {
            fprintf(pfOut,"Deleting the environment variable '%s' was successful\n",acCnf);
            ERROR(0);
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'DELENV\' not valid\n");
      fprintf(pfOut,"%s %s DELENV variable\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"TRACE",0,0,FALSE)==0) {
      if (argc==3) {
         if (strxcmp(isCas,argv[2],"ON",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-ON",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--ON",0,0,FALSE)==0) {
            sprintf(acCnf,"%s.%s.trace",acOwn,pcPgm);
            siErr=siCnfSet(psCnf,pfOut,acCnf,"ON",TRUE);
            if (siErr) ERROR(2); else {
               fprintf(pfOut,"Setting configuration keyword '%s' to value 'ON' was successful\n",acCnf);
               ERROR(0);
            }
         } else if (strxcmp(isCas,argv[2],"OFF",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"-OFF",0,0,FALSE)==0 || strxcmp(isCas,argv[2],"--OFF",0,0,FALSE)==0) {
            sprintf(acCnf,"%s.%s.trace",acOwn,pcPgm);
            siErr=siCnfSet(psCnf,pfOut,acCnf,"OFF",TRUE);
            if (siErr) ERROR(2); else {
               fprintf(pfOut,"Setting configuration keyword '%s' to value 'OFF' was successful\n",acCnf);
               ERROR(0);
            }
         } else {
            const char* pcFil;
            const char* pcCmd;
            pcFil=strchr(argv[2],'=');
            if (pcFil!=NULL) {
               *((char*)pcFil)=EOS; pcFil++; pcCmd=argv[2];
               if (strxcmp(isCas,pcCmd,"FILE",0,0,FALSE)==0) {
                  sprintf(acCnf,"%s.%s.trace.file",acOwn,pcPgm);
                  siErr=siCnfSet(psCnf,pfOut,acCnf,pcFil,TRUE);
                  if (siErr) ERROR(2); else {
                     if (strlen(pcFil)) {
                        fprintf(pfOut,"Setting configuration keyword '%s' to value '%s' was successful\n",acCnf,pcFil);
                     } else {
                        fprintf(pfOut,"Deleting value from configuration keyword '%s' was successful\n",acCnf);
                     }
                     ERROR(0);
                  }
               }
            }
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'TRACE\' not valid\n");
      fprintf(pfOut,"%s %s TRACE ON/OFF/FILE=filenam\n",pcDep,argv[0]);
      ERROR(8);
   } else if (strxcmp(isCas,argv[1],"CONFIG",0,0,FALSE)==0) {
      if (argc==2) {
         fprintf(pfOut,"Current configuration data:\n");
         siCnt=siCnfPrn(psCnf,pfOut,pcDep);
         if (siCnt) {
            fprintf(pfOut,"Assigned to file \'%s\'\n",psCnf->acFil);
         } else {
            fprintf(pfOut,"No configuration data defined for file \'%s\'\n",psCnf->acFil);
         }
         ERROR(0);
      } else if (argc==3) {
         if (argv[2][0]=='-') argv[2]++;
         if (argv[2][0]=='-') argv[2]++;
         if (strxcmp(isCas,argv[2],"CLEAR",0,0,FALSE)==0) {
            siCnt=siCnfClr(psCnf,pfOut,pcDep);
            if (siCnt) {
               fprintf(pfOut,"Delete %d elements from file \'%s\'\n",siCnt,psCnf->acFil);
            } else {
               fprintf(pfOut,"No configuration data defined for file \'%s\'\n",psCnf->acFil);
            }
            ERROR(0);
         }
      }
      fprintf(pfOut,"Syntax for built-in function \'CONFIG\' not valid\n");
      fprintf(pfOut,"%s %s CONFIG\n",pcDep,argv[0]);
      fprintf(pfOut,"%s %s CONFIG CLEAR\n",pcDep,argv[0]);
      ERROR(8);
   } else {
      for (i=0;psTab[i].pcKyw!=NULL;i++) {
         if (strxcmp(isCas,argv[1],psTab[i].pcKyw,0,-1,FALSE)==0) {
            siErr=siCleCommandInit(psTab[i].pfIni,psTab[i].pvClp,acOwn,pcPgm,psTab[i].pcKyw,psTab[i].pcMan,psTab[i].pcHlp,psTab[i].piOid,psTab[i].psTab,isCas,isFlg,siMkl,pfOut,pfTrc,pcDep,pcOpt,pcEnt,psCnf,&pvHdl);
            if (siErr) ERROR(siErr);
            siErr=siCleGetCommand(pvHdl,pfOut,pcDep,psTab[i].pcKyw,argc,argv,acCmd);
            if (siErr) ERROR(siErr);
            siErr=siClpParseCmd(pvHdl,acCmd,TRUE,psTab[i].piOid,&pcPos,&pcLst);
            if (siErr<0) {
               fprintf(pfOut,"Command line parser for command '%s' failed!\n",psTab[i].pcKyw);
               vdPrnCommandError(pvHdl,pfOut,acCmd,pcPos,pcLst,pcDep);
               ERROR(6);
            }
            siErr=psTab[i].pfMap(pfOut,pfTrc,psTab[i].piOid,psTab[i].pvClp,psTab[i].pvPar);
            if (siErr) {
               fprintf(pfOut,"Mapping of CLP structure for command '%s' failed!\n",psTab[i].pcKyw);
               ERROR(4);
            }
            siErr=psTab[i].pfRun(pfOut,pfTrc,acOwn,pcPgm,pcVsn,pcAbo,pcLic,psTab[i].pcKyw,acCmd,pcLst,psTab[i].pvPar);
            if (siErr)  {
               fprintf(pfOut,"Run of command '%s' failed!\n",psTab[i].pcKyw);
               siErr=psTab[i].pfFin(pfOut,pfTrc,psTab[i].pvPar);
               ERROR(2);
            }
            siErr=psTab[i].pfFin(pfOut,pfTrc,psTab[i].pvPar);
            if (siErr) {
               fprintf(pfOut,"Finish/cleanup for command '%s' failed!\n",psTab[i].pcKyw);
               ERROR(1);
            }
            ERROR(0);
         }
      }
      if (pcDef!=NULL && strlen(pcDef) && ppArg==NULL) {
         ppArg=malloc((argc+1)*sizeof(*ppArg));
         if (ppArg == NULL) {
            fprintf(pfOut,"Memory allocation for argument list to run the default command '%s' failed!\n",pcDef);
            ERROR(16);
         }
         for (i=argc;i>1;i--) ppArg[i]=argv[i-1];
         ppArg[0]=argv[0]; ppArg[1]=(char*)pcDef; argc++; argv=ppArg;
         goto EVALUATE;
      }
      fprintf(pfOut,"Command or built-in function \'%s\' not supported\n",argv[1]);
      vdPrnStaticSyntax(pfOut,psTab,argv[0],pcDep,pcOpt);
      ERROR(8);
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
   const int                     isFlg,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl,
   char*                         pcFil,
   int*                          piFil)
{
   int                           siErr;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   unsigned int                  uiFlg=0;
   static char                   acPro[CLEMAX_PROSIZ];
   char*                         pcPos=NULL;
   char*                         pcLst=NULL;

   *ppHdl=NULL;
   siErr=pfIni(pfOut,pfTrc,pcOwn,pcPgm,pvClp);
   if (siErr) {
      fprintf(pfOut,"Initialization of CLP structure for command \'%s\' failed!\n",pcCmd);
      return(10);
   }
   uiFlg=(isFlg)?CLPFLG_PRO:0;
   *ppHdl=pvClpOpen(isCas,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,uiFlg,psTab,pvClp,pfOut,pfOut,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of property parser for command \'%s\' failed!\n",pcCmd);
      return(12);
   }
   siErr=siCleGetProperties(*ppHdl,pfOut,psCnf,pcOwn,pcPgm,pcCmd,pcFil,acPro,piFil);
   if (siErr) {
      vdClpClose(*ppHdl);*ppHdl=NULL;
      return(siErr);
   }
   siErr=siClpParsePro(*ppHdl,acPro,FALSE,&pcPos,&pcLst);
   if (siErr<0) {
      fprintf(pfOut,"Parsing property file \'%s\' for command \'%s\' failed!\n",pcFil,pcCmd);
      vdPrnPropertyError(*ppHdl,pfOut,pcFil,acPro,pcPos,pcLst,pcDep);
      vdClpClose(*ppHdl);*ppHdl=NULL;
      return(6);
   }
   return(0);
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
   if (siFil!=3) {
      sprintf(acEnv,"%s_%s_%s_PROPERTY_FILENAME",pcOwn,pcPgm,pcCmd);
      for (i=0;acEnv[i];i++) acEnv[i]=toupper(acEnv[i]);
      pcFil=getenv(acEnv);
      if (pcFil==NULL) {
#ifdef __HOST__
         {
            int j,k;
            for (j=k=i=0;pcOwn[i] && k<8;i++) {
               if (isalnum(pcOwn[i])) {
                  acEnv[j]=toupper(pcOwn[i]);
                  j++; k++;
               }
            }
            acEnv[j]='.'; j++;
            for (k=i=0;pcPgm[i] && k<8;i++) {
               if (isalnum(pcPgm[i])) {
                  acEnv[j]=toupper(pcPgm[i]);
                  j++; k++;
               }
            }
            acEnv[j]='.'; j++;
            for (k=i=0;pcCmd[i] && k<8;i++) {
               if (isalnum(pcCmd[i])) {
                  acEnv[j]=toupper(pcCmd[i]);
                  j++; k++;
               }
            }
            acEnv[j]=0x00;
            strcat(acEnv,"%s.PROPER");
         }
#else
         if (pcHom!=NULL && strlen(pcHom)) {
            sprintf(acEnv,"%s.%s.%s.%s.properties",pcHom,pcOwn,pcPgm,pcCmd);
         } else {
            sprintf(acEnv,".%s.%s.%s.properties",pcOwn,pcPgm,pcCmd);
         }
         for (i=0;acEnv[i];i++) acEnv[i]=tolower(acEnv[i]);
#endif
         pcFil=acEnv;
      }
   }
   errno=0;
   pfPro=fopen(pcFil,"w");
   if (pfPro==NULL) {
      fprintf(pfOut,"Cannot open the property file \'%s\' for write operation (%d-%s)\n",pcFil,errno,strerror(errno));
      vdClpClose(pvHdl);
      return(16);
   }

   siErr=siClpProperties(pvHdl,FALSE,10,pcCmd,pfPro);
   if (siErr<0) {
      fprintf(pfOut,"Write property file (%s) for command \'%s\' failed (%d-%s)\n",pcFil,pcCmd,errno,strerror(errno));
      vdClpClose(pvHdl); fclose(pfPro); return(2);
   }
   vdClpClose(pvHdl); fclose(pfPro);
   fprintf(pfOut,"Property file (%s) for command \'%s\' successfully written\n",pcFil,pcCmd);

   if (siFil!=3) {
      sprintf(acCnf,"%s.%s.%s.property.file",pcOwn,pcPgm,pcCmd);
      siErr=siCnfSet(psCnf,pfOut,acCnf,pcFil,TRUE);
      if (siErr) {
         fprintf(pfOut,"Activation of property file (%s) for command \'%s\' failed\n",pcFil,pcCmd);
         return(2);
      }
      fprintf(pfOut,"Setting configuration keyword \'%s\' to value \'%s\' was successful\n",acCnf,pcFil);
      fprintf(pfOut,"Activation of property file (%s) for command \'%s\' was successful\n",pcFil,pcCmd);
   }
   return(0);
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
   const int                     isFlg,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf,
   void**                        ppHdl)
{
   int                           siErr,siFil=0;
   int                           isOvl=(piOid==NULL)?FALSE:TRUE;
   unsigned int                  uiFlg;
   static char                   acPro[CLEMAX_PROSIZ];
   char                          acFil[CLEMAX_FILSIZ];
   char*                         pcPos=NULL;
   char*                         pcLst=NULL;


   *ppHdl=NULL;

   siErr=pfIni(pfOut,pfTrc,pcOwn,pcPgm,pvClp);
   if (siErr) {
      fprintf(pfOut,"Initialization of CLP structure for command \'%s\' failed!\n",pcCmd);
      return(10);
   }
   uiFlg=(isFlg)?CLPFLG_PRO:0;
   *ppHdl=pvClpOpen(isCas,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,uiFlg,psTab,pvClp,pfOut,pfOut,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of property parser for command \'%s\' failed!\n",pcCmd);
      return(12);
   }
   siErr=siCleGetProperties(*ppHdl,pfOut,psCnf,pcOwn,pcPgm,pcCmd,acFil,acPro,&siFil);
   if (siErr) {
      vdClpClose(*ppHdl);*ppHdl=NULL;
      return(siErr);
   }
   siErr=siClpParsePro(*ppHdl,acPro,FALSE,&pcPos,&pcLst);
   if (siErr<0) {
      fprintf(pfOut,"Property parser for command \'%s\' failed!\n",pcCmd);
      vdPrnPropertyError(*ppHdl,pfOut,acFil,acPro,pcPos,pcLst,pcDep);
      vdClpClose(*ppHdl);*ppHdl=NULL;
      return(6);
   }

   vdClpClose(*ppHdl);*ppHdl=NULL;

   uiFlg=(isFlg)?CLPFLG_CMD:0;
   *ppHdl=pvClpOpen(isCas,siMkl,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,isOvl,uiFlg,psTab,pvClp,pfOut,pfOut,pfTrc,pfTrc,pfTrc,pfTrc,pcDep,pcOpt,pcEnt);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of command line parser for command \'%s\' failed!\n",pcCmd);
      return(12);
   }
   return(0);
}

static int siCleSimpleInit(
   FILE*                         pfOut,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   void**                        ppHdl)
{
   const TsClpArgument asTab[]={
         {CLPTYP_NUMBER,"XX",NULL,0,1,1,0,0,CLPFLG_NON,NULL,NULL,NULL,"XX",0,0.0,NULL},
         {CLPTYP_NON   ,NULL,NULL,0,0,0,0,0,CLPFLG_NON,NULL,NULL,NULL,NULL,0,0.0,NULL}
   };
   *ppHdl=pvClpOpen(FALSE,0,"","","","","",FALSE,0,asTab,"",pfOut,pfOut,NULL,NULL,NULL,NULL,pcDep,pcOpt,pcEnt);
   if (*ppHdl==NULL) {
      fprintf(pfOut,"Open of command line parser for grammar and lexeme print out failed!\n");
      return(12);
   }
   return(0);
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
   const int                     isFlg,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsCnfHdl*                     psCnf)
{
   int                           siErr;
   void*                         pvHdl=NULL;
   char*                         pcPos=NULL;
   char*                         pcLst=NULL;
   char                          acFil[CLEMAX_FILSIZ];
   int                           siFil=0;

   siErr=siClePropertyInit(pfIni,pvClp,pcOwn,pcPgm,pcCmd,pcMan,pcHlp,
                           piOid,psTab,isCas,isFlg,siMkl,pfOut,pfTrc,
                           pcDep,pcOpt,pcEnt,psCnf,&pvHdl,acFil,&siFil);
   if (siErr) return(siErr);

   siErr=siClpParsePro(pvHdl,pcPro,TRUE,&pcPos,&pcLst);
   if (siErr<0) {
      fprintf(pfOut,"Property parser for command \'%s\' failed!\n",pcCmd);
      vdPrnPropertyError(pvHdl,pfOut,NULL,pcPro,pcPos,pcLst,pcDep);
      vdClpClose(pvHdl);
      return(6);
   }

   siErr=siClePropertyFinish(pcHom,pcOwn,pcPgm,pcCmd,pfOut,pfTrc,psCnf,pvHdl,acFil,siFil);
   if (siErr) return(siErr);

   return(0);
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
      fprintf(pfOut,"(1)\n");
      l=strlen(pcPgm)+3;
      for (i=0;i<l;i++) fprintf(pfOut,"=");
      fprintf(pfOut,"\n");
      fprintf(pfOut,":doctype: manpage\n\n");
      fprintf(pfOut,"NAME\n");
      fprintf(pfOut,"----\n\n");
      for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut," - `%s`\n\n",pcHlp);
      fprintf(pfOut,"SYNOPSIS\n");
      fprintf(pfOut,"--------\n\n");
      fprintf(pfOut,"-----------------------------------------------------------------------\n");
      fprintf(pfOut,"PATH:   %s\n",pcOwn);
      fprintf(pfOut,"TYPE:   PROGRAM\n");
      fprintf(pfOut,"SYNTAX: $ %s COMMAND|FUNCTION ...\n",pcPgm);
      fprintf(pfOut,"-----------------------------------------------------------------------\n\n");
      fprintf(pfOut,"DESCRIPTION\n");
      fprintf(pfOut,"-----------\n\n");
      if (pcMan!=NULL && strlen(pcMan)) {
         fprintf(pfOut,"%s\n\n",pcMan);
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
         fprintf(pfOut,"2. PROGRAM \'%s\'\n",pcPgm);
         l=strlen(pcPgm)+13;
         for (i=0;i<l;i++) fprintf(pfOut,"-"); fprintf(pfOut,"\n\n");
         fprintf(pfOut,"2.1. SYNOPSIS\n");
         fprintf(pfOut,"~~~~~~~~~~~~~\n\n");
      } else {
         fprintf(pfOut,"PROGRAM \'%s\'\n",pcPgm);
         l=strlen(pcPgm)+10;
         for (i=0;i<l;i++) fprintf(pfOut,"-"); fprintf(pfOut,"\n\n");
         fprintf(pfOut,"SYNOPSIS\n");
         fprintf(pfOut,"~~~~~~~~\n\n");
      }
      fprintf(pfOut,"-----------------------------------------------------------------------\n");
      fprintf(pfOut,"HELP:   %s\n",pcHlp);
      fprintf(pfOut,"PATH:   %s\n",pcOwn);
      fprintf(pfOut,"TYPE:   PROGRAM\n");
      fprintf(pfOut,"SYNTAX: $ %s COMMAND|FUNCTION ...\n",pcPgm);
      fprintf(pfOut,"-----------------------------------------------------------------------\n\n");
      fprintf(pfOut,"indexterm:[Synopsis for program %s]\n\n\n",pcPgm);

      if (isNbr) {
         fprintf(pfOut,"2.2. DESCRIPTION\n");
         fprintf(pfOut,"~~~~~~~~~~~~~~~~\n\n");

      } else {
         fprintf(pfOut,"DESCRIPTION\n");
         fprintf(pfOut,"~~~~~~~~~~~\n\n");
      }
      if (pcMan!=NULL && strlen(pcMan)) {
         fprintf(pfOut,"%s\n\n",pcMan);
      } else {
         fprintf(pfOut,"No detailed description available for this program.\n\n");
      }
      fprintf(pfOut,"indexterm:[Description for program %s]\n\n\n",pcPgm);

      if (isNbr) {
         fprintf(pfOut,"2.3. SYNTAX\n");
         fprintf(pfOut,"~~~~~~~~~~~\n\n");
      } else {
         fprintf(pfOut,"SYNTAX\n");
         fprintf(pfOut,"~~~~~~\n\n");
      }
      fprintf(pfOut,"%s\n",MAN_CLE_MAIN_SYNTAX);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Syntax for program \'%s\':\n",pcPgm);
      vdPrnStaticSyntax(pfOut,psTab,pcPgm,pcDep,pcSep);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");
      fprintf(pfOut,"indexterm:[Syntax for program %s]\n\n\n",pcPgm);

      if (isNbr) {
         fprintf(pfOut,"2.4. HELP\n");
         fprintf(pfOut,"~~~~~~~~~\n\n");
      } else {
         fprintf(pfOut,"HELP\n");
         fprintf(pfOut,"~~~~\n\n");
      }
      fprintf(pfOut,"%s\n",MAN_CLE_MAIN_HELP);
      fprintf(pfOut,"------------------------------------------------------------------------\n");
      fprintf(pfOut,"Help for program \'%s\':\n",pcPgm);
      vdPrnStaticHelp(pfOut,psTab,pcPgm,FALSE,pcDep);
      fprintf(pfOut,"------------------------------------------------------------------------\n\n");
      fprintf(pfOut,"indexterm:[Help for program %s]\n\n\n",pcPgm);
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
      fprintf(pfOut,".");
      for(p=pcFct;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut,"(1)\n");
      l=strlen(pcPgm)+strlen(pcFct)+4;
      for (i=0;i<l;i++) fprintf(pfOut,"=");
      fprintf(pfOut,"\n");
      fprintf(pfOut,":doctype: manpage\n\n");
      fprintf(pfOut,"NAME\n");
      fprintf(pfOut,"----\n\n");
      for(p=pcPgm;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut,".");
      for(p=pcFct;*p;p++) fprintf(pfOut,"%c",tolower(*p));
      fprintf(pfOut," - `%s`\n\n",pcHlp);
      fprintf(pfOut,"SYNOPSIS\n");
      fprintf(pfOut,"--------\n\n");
      fprintf(pfOut,"-----------------------------------------------------------------------\n");
      fprintf(pfOut,"PATH:   %s.%s\n",pcOwn,pcPgm);
      fprintf(pfOut,"TYPE:   BUILT-IN FUNCTION\n");
      fprintf(pfOut,"SYNTAX: $ %s %s\n",pcPgm,pcSyn);
      fprintf(pfOut,"-----------------------------------------------------------------------\n\n");
      fprintf(pfOut,"DESCRIPTION\n");
      fprintf(pfOut,"-----------\n\n");
      fprintf(pfOut,"%s\n\n",pcMan);
      fprintf(pfOut,"AUTHOR\n------\n\n");
      fprintf(pfOut,"limes datentechnik(r) gmbh (www.flam.de)\n\n");
   } else {
      if (isNbr) {
         fprintf(pfOut,"%s FUNCTION \'%s\'\n",pcNum,pcFct);
         l=strlen(pcNum)+strlen(pcFct)+12;
         for (i=0;i<l;i++) fprintf(pfOut,pcLev); fprintf(pfOut,"\n\n");
      } else {
         fprintf(pfOut,"FUNCTION \'%s\'\n",pcFct);
         l=strlen(pcFct)+11;
         for (i=0;i<l;i++) fprintf(pfOut,pcLev); fprintf(pfOut,"\n\n");
      }
      fprintf(pfOut,".SYNOPSIS\n\n");
      fprintf(pfOut,"-----------------------------------------------------------------------\n");
      fprintf(pfOut,"HELP:   %s\n",pcHlp);
      fprintf(pfOut,"PATH:   %s.%s\n",pcOwn,pcPgm);
      fprintf(pfOut,"TYPE:   BUILT-IN FUNCTION\n");
      fprintf(pfOut,"SYNTAX: $ %s %s\n",pcPgm,pcSyn);
      fprintf(pfOut,"-----------------------------------------------------------------------\n\n");
      fprintf(pfOut,".DESCRIPTION\n\n");
      fprintf(pfOut,"%s\n\n",pcMan);
      fprintf(pfOut,"indexterm:[Built-in function %s]\n\n\n",pcFct);
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
   fprintf(pfOut,"%s%s %s command \"... argument list ...\"\n",pcDep,pcDep,pcPgm                 );
   fprintf(pfOut,"%s%s %s command=\" parameter file name \"\n",pcDep,pcDep,pcPgm                 );
   fprintf(pfOut,"%s Built-in functions:\n"                   ,pcDep                             );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_SYNTAX  );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_HELP    );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_MANPAGE );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_GENDOCU );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_GENPROP );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_SETPROP );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_CHGPROP );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_DELPROP );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_GETPROP );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_SETOWNER);
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_GETOWNER);
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_SETENV  );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_GETENV  );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_DELENV  );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_TRACE   );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_CONFIG  );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_GRAMMAR );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_LEXEM   );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_LICENSE );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_VERSION );
   fprintf(pfOut,"%s%s %s %s\n"                               ,pcDep,pcDep,pcPgm,SYN_CLE_ABOUT   );
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
         fprintf(pfOut,"%s%s %s %-8.8s - %s \n",pcDep,pcDep,pcPgm,psTab[i].pcKyw,psTab[i].pcHlp);
      }
   }
   fprintf(pfOut,"%s Built-in functions - to give interactive support for the commands above\n",pcDep);
   fprintf(pfOut,"%s%s %s SYNTAX   - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_SYNTAX  );
   fprintf(pfOut,"%s%s %s HELP     - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_HELP    );
   fprintf(pfOut,"%s%s %s MANPAGE  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_MANPAGE );
   fprintf(pfOut,"%s%s %s GENDOCU  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_GENDOCU );
   fprintf(pfOut,"%s%s %s GENPROP  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_GENPROP );
   fprintf(pfOut,"%s%s %s SETPROP  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_SETPROP );
   fprintf(pfOut,"%s%s %s CHGPROP  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_CHGPROP );
   fprintf(pfOut,"%s%s %s DELPROP  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_DELPROP );
   fprintf(pfOut,"%s%s %s GETPROP  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_GETPROP );
   fprintf(pfOut,"%s%s %s SETOWNER - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_SETOWNER);
   fprintf(pfOut,"%s%s %s GETOWNER - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_GETOWNER);
   fprintf(pfOut,"%s%s %s SETENV   - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_SETENV  );
   fprintf(pfOut,"%s%s %s GETENV   - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_GETENV  );
   fprintf(pfOut,"%s%s %s DELENV   - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_DELENV  );
   fprintf(pfOut,"%s%s %s TRACE    - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_TRACE   );
   fprintf(pfOut,"%s%s %s CONFIG   - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_CONFIG  );
   fprintf(pfOut,"%s%s %s GRAMMAR  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_GRAMMAR );
   fprintf(pfOut,"%s%s %s LEXEM    - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_LEXEM   );
   fprintf(pfOut,"%s%s %s LICENSE  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_LICENSE );
   fprintf(pfOut,"%s%s %s VERSION  - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_VERSION );
   fprintf(pfOut,"%s%s %s ABOUT    - %s\n",pcDep,pcDep,pcPgm,HLP_CLE_ABOUT   );
   fprintf(pfOut,"For more information please use the built-in function \'MANPAGE\'\n");
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
   } else siErr=siClpSyntax(pvHdl,TRUE,TRUE,siDep,pcCmd);
}

static void vdPrnCommandHelp(
   void*                   pvHdl,
   const char*             pcCmd,
   const int               siDep,
   const int               isMan)
{
   siClpHelp(pvHdl,siDep,pcCmd,isMan);
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
   sprintf(acNum,"3.%d.",siInd+1);
   siClpDocu(pvHdl,pfOut,pcCmd,acNum,FALSE,isMan,isNbr);
}

static void vdPrnCommandError(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcCmd,
   const char*                   pcPos,
   const char*                   pcLst,
   const char*                   pcDep)
{
   int                           i,l;
   if (pcPos==pcCmd) fprintf(pfOut,"%s Command line error at byte %d ("   ,pcDep,((int)(pcPos-pcCmd))+1);
               else  fprintf(pfOut,"%s Command line error at byte %d (...",pcDep,((int)(pcPos-pcCmd))+1);
   for (i=0;i<32 && !iscntrl(pcPos[i]);i++) fprintf(pfOut,"%c",pcPos[i]);
   if (pcPos[i]==EOS) fprintf(pfOut,")\n");
                 else fprintf(pfOut,"...)\n");
   if (pcLst!=NULL) {
      l=strlen(pcLst);
      if (l>1) {
         l--;
         fprintf(pfOut,"%s after successful parsing of arguments below:\n",pcDep);
         fprintf(pfOut,"%s%s ",pcDep,pcDep);
         for (i=0;i<l;i++) {
            if (pcLst[i]=='\n') {
               fprintf(pfOut,"\n%s%s ",pcDep,pcDep);
            } else fprintf(pfOut,"%c",pcLst[i]);
         }
         fprintf(pfOut,"\n");
      } else fprintf(pfOut,"%s Something with the first argument is wrong\n",pcDep);
   }
}

static void vdPrnPropertyError(
   void*                         pvHdl,
   FILE*                         pfOut,
   const char*                   pcFil,
   const char*                   pcPro,
   const char*                   pcPos,
   const char*                   pcLst,
   const char*                   pcDep)
{
   int                           i,l;
   if (pcFil!=NULL && strlen(pcFil)) {
      if (pcPos==pcPro) fprintf(pfOut,"%s Property error in file \'%s\' at byte %d ("   ,pcDep,pcFil,((int)(pcPos-pcPro))+1);
                  else  fprintf(pfOut,"%s Property error in file \'%s\' at byte %d (...",pcDep,pcFil,((int)(pcPos-pcPro))+1);
   } else {
      if (pcPos==pcPro) fprintf(pfOut,"%s Property error in property list at byte %d ("   ,pcDep,((int)(pcPos-pcPro))+1);
                  else  fprintf(pfOut,"%s Property error in property list at byte %d (...",pcDep,((int)(pcPos-pcPro))+1);
   }
   for (i=0;i<32 && !iscntrl(pcPos[i]);i++) fprintf(pfOut,"%c",pcPos[i]);
   if (pcPos[i]==EOS) fprintf(pfOut,")\n");
                 else fprintf(pfOut,"...)\n");
   if (pcLst!=NULL) {
      l=strlen(pcLst);
      if (l>1) {
         l--;
         fprintf(pfOut,"%s after successful parsing of properties below:\n",pcDep);
         fprintf(pfOut,"%s%s ",pcDep,pcDep);
         for (i=0;i<l;i++) {
            if (pcLst[i]=='\n') {
               fprintf(pfOut,"\n%s%s ",pcDep,pcDep);
            } else fprintf(pfOut,"%c",pcLst[i]);
         }
      } else fprintf(pfOut,"%s Something is wrong with the first property\n",pcDep);
   }
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
   char*                   pcPro,
   int*                    piFlg)
{
   int                     siPro,siRst;
   char*                   pcHlp=NULL;
   FILE*                   pfPro=NULL;
   char                    acRot[CLEMAX_PATSIZ];

   pcFil[0]=EOS;
   pcPro[0]=EOS;
   if (strlen(pcOwn)+strlen(pcPgm)+strlen(pcFct)+2>=CLEMAX_PATLEN) {
      fprintf(pfOut,"Rot (%s.%s.%s) is too long (>=%d)\n",pcOwn,pcPgm,pcFct,CLEMAX_PATLEN);
      return(0);
   }
   sprintf(acRot,"%s.%s.%s.property.file",pcOwn,pcPgm,pcFct);
   pcHlp=pcCnfGet(psCnf,acRot);
   if (pcHlp==NULL) {
      sprintf(acRot,"%s.%s.property.file",pcOwn,pcPgm);
      pcHlp=pcCnfGet(psCnf,acRot);
      if (pcHlp==NULL) {
         sprintf(acRot,"%s.property.file",pcOwn);
         pcHlp=pcCnfGet(psCnf,acRot);
         if (pcHlp==NULL) {
            pcPro[0]=0x00;
            *piFlg=0;
            return(0);
         } else *piFlg=1;
      } else *piFlg=2;
   } else *piFlg=3;

   errno=0;
   strcpy(pcFil,pcHlp);
   pfPro=fopen(pcFil,"r");
   if (pfPro==NULL) {
      fprintf(pfOut,"Cannot open the property file \'%s\' for read operation (%d-%s)\n",pcFil,errno,strerror(errno));
      return(0);
   }
   errno=0;
   pcPro[0]=EOS; pcHlp=pcPro; siRst=CLEMAX_PROLEN;
   while (!errno && !feof(pfPro) && siRst) {
      siPro=fread(pcHlp,1,siRst,pfPro);
      pcHlp+=siPro;
      siRst-=siPro;
   }
   if (errno && !feof(pfPro)) {
      fprintf(pfOut,"Error reading property file (%d-%s)\n",errno,strerror(errno));
      fclose(pfPro);
      pcPro[0]=EOS;
      return(16);
   }
   fclose(pfPro);
   *pcHlp=EOS;
   if (siRst==0) {
      fprintf(pfOut,"Property file is too big (more than %d bytes)\n",CLEMAX_PROLEN);
      return(8);
   }

   return(0);
}

static int siCleGetCommand(
   void*                   pvHdl,
   FILE*                   pfOut,
   const char*             pcDep,
   const char*             pcFct,
   int                     argc,
   char*                   argv[],
   char*                   pcCmd)
{
   int                     i,siCmd,siRst,l=strlen(pcFct);
   char*                   pcHlp=NULL;
   FILE*                   pfCmd=NULL;
   pcCmd[0]=EOS;
   if (argc==2 && argv[1][l]!='=' && argv[1][l]!='.' && argv[1][l]!='(') {
      fprintf(pfOut,"Argument list (\"...\") for command \'%s\' missing\n",pcFct);
      fprintf(pfOut,"Syntax for command \'%s\' not valid\n",pcFct);
      fprintf(pfOut,"%s %s \"",pcDep,argv[0]);
      siClpSyntax(pvHdl,FALSE,FALSE,1,NULL);
      fprintf(pfOut,"\"\n");
      fprintf(pfOut,"Please use \'%s SYNTAX %s[.path]\' for more information\n",argv[0],pcFct);
      return(8);
   }
   if (argv[1][l]==EOS) {
      for (i=2;i<argc;i++) {
         if (strlen(pcCmd)+strlen(argv[i])+2>CLEMAX_CMDLEN) {
            fprintf(pfOut,"Argument list is too long! (more than %d bytes)!\n",CLEMAX_CMDLEN);
            return(8);
         }
         if (i>2) strcat(pcCmd," ");
         strcat(pcCmd,argv[i]);
      }
   } else if (argv[1][l]=='.' || argv[1][l]=='(') {
      strcat(pcCmd,&argv[1][l]);
      for (i=2;i<argc;i++) {
         if (strlen(pcCmd)+strlen(argv[i])+2>CLEMAX_CMDLEN) {
            fprintf(pfOut,"Argument list is too long! (more than %d bytes)!\n",CLEMAX_CMDLEN);
            return(8);
         }
         strcat(pcCmd," "); strcat(pcCmd,argv[i]);
      }
   } else if (argv[1][l]=='=') {
      if (argc!=2) {
         fprintf(pfOut,"The expected parameter file name for \'%s\' is split into more than one parameter\n",pcFct);
         fprintf(pfOut,"The parameter file name must start with \" and end with \" to join anything into one parameter\n");
         fprintf(pfOut,"Syntax for command \'%s\' not valid\n",pcFct);
         fprintf(pfOut,"%s %s %s=\" parameter file name \"\n",pcDep,argv[0],pcFct);
         fprintf(pfOut,"Please use \'%s SYNTAX %s[.path]\' for more information\n",argv[0],pcFct);
         return(8);
      }
      if (strlen(argv[1])>=CLEMAX_CMDLEN) {
         fprintf(pfOut,"Parameter file name is too long (more than %d bytes)!\n",CLEMAX_CMDLEN);
         return(8);
      }
      strcpy(pcCmd,argv[1]+l+1);
      errno=0;
      pfCmd=fopen(pcCmd,"r");
      if (pfCmd==NULL) {
          fprintf(pfOut,"Cannot open the parameter file \'%s\' (%d-%s)\n",pcCmd,errno,strerror(errno));
         return(8);
      }
      errno=0;
      pcCmd[0]=0x00; pcHlp=pcCmd; siRst=CLEMAX_CMDLEN;
      while (!errno && !feof(pfCmd) && siRst) {
         siCmd=fread(pcHlp,1,siRst,pfCmd);
         pcHlp+=siCmd;
         siRst-=siCmd;
      }
      if (errno && !feof(pfCmd)) {
         fprintf(pfOut,"Error reading parameter file (%d-%s)\n",errno,strerror(errno));
         fclose(pfCmd);
         pcCmd[0]=EOS;
         return(16);
      }
      fclose(pfCmd);
      *pcHlp=EOS;
      if (siRst==0) {
         fprintf(pfOut,"Parameter file is too big (more than %d bytes)\n",CLEMAX_CMDLEN);
         return(8);
      }
   } else {
      fprintf(pfOut,"No blank space ' ', equal sign '=', dot '.' or bracket '(' behind \'%s\'\n",pcFct);
      fprintf(pfOut,"Please use a blank space to define an argument list or an equal sign for a parameter file\n");
      fprintf(pfOut,"Syntax for command \'%s\' not valid\n",pcFct);
      fprintf(pfOut,"%s %s %s \"... argument list ...\"\n",pcDep,argv[0],pcFct);
      fprintf(pfOut,"%s %s %s=\" parameter file name \"\n",pcDep,argv[0],pcFct);
      fprintf(pfOut,"Please use \'%s SYNTAX %s[.path]\' for more information\n",argv[0],pcFct);
      return(8);
   }
   return(0);
}

/*****************************************************************************/

static TsCnfHdl* psCnfOpn(
   FILE*                         pfOut,
   const int                     isCas,
   const char*                   pcPgm,
   const char*                   pcFil)
{
   FILE*                         pfFil;
   static char                   acBuf[32768];
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
   psHdl->isChg=FALSE;
   psHdl->isClr=FALSE;
   psHdl->isCas=isCas;
   psHdl->psFst=NULL;
   psHdl->psLst=NULL;
   if (pcPgm!=NULL) {
      if (strlen(pcFil)>=sizeof(psHdl->acFil)-1) {
         if (pfOut!=NULL) fprintf(pfOut,"Program name (%s) too long (>=%u)\n",pcPgm,(unsigned)sizeof(psHdl->acPgm)-1);
         free(psHdl);
         return(NULL);
      }
      strcpy(psHdl->acPgm,pcPgm);
   } else psHdl->acPgm[0]=EOS;
   if (pcFil==NULL || strlen(pcFil)==0) return(psHdl);
   if (strlen(pcFil)>=sizeof(psHdl->acFil)-1) {
      if (pfOut!=NULL) fprintf(pfOut,"Configuration file name (%s) too long (>=%u)\n",pcFil,(unsigned)sizeof(psHdl->acFil)-1);
      free(psHdl);
      return(NULL);
   }
   strcpy(psHdl->acFil,pcFil);

   errno=0;
   pfFil=fopen(psHdl->acFil,"r");
   if (pfFil==NULL && (errno==2 || errno==49 || errno==129)) return(psHdl);
   if (pfFil==NULL) {
      if (pfOut!=NULL) fprintf(pfOut,"Cannot open the configuration file \'%s\'  (%d - %s)\n",psHdl->acFil,errno,strerror(errno));
      free(psHdl);
      return(NULL);
   }

   while (fgets(acBuf,sizeof(acBuf)-1,pfFil)!=NULL) {
      pcHlp=strchr(acBuf,'#');
      if (pcHlp!=NULL) *pcHlp=EOS;
      pcHlp=acBuf+strlen(acBuf);
      while (isspace(*(pcHlp-1))) {
         pcHlp--; *pcHlp=EOS;
      }
      pcHlp=strchr(acBuf,'=');

      if (pcHlp!=NULL) {
         pcKyw=acBuf; pcVal=pcHlp+1; *pcHlp=EOS;
         psEnt=(TsCnfEnt*)calloc(1,sizeof(TsCnfEnt));
         if (psEnt==NULL) {
            if (pfOut!=NULL) fprintf(pfOut,"Memory allocation for configuration data element failed\n");
            fclose(pfFil);
            free(psHdl);
            return(NULL);
         }
         siKyw=strlen(pcKyw); siVal=strlen(pcVal);
         if (siKyw && siVal) {
            if (siKyw>=sizeof(psEnt->acKyw)-1) {
               if (pfOut!=NULL)
                  fprintf(pfOut,"Keyword (%s) in configuration file (%s) too long (>=%u)\n",pcKyw,pcFil,(unsigned)sizeof(psEnt->acKyw)-1);
               free(psEnt);
               fclose(pfFil);
               free(psHdl);
               return(NULL);
            }
            if (siVal>=sizeof(psEnt->acVal)-1) {
               if (pfOut!=NULL)
                  fprintf(pfOut,"Value (%s) in configuration file (%s) too long (>=%u)\n",pcVal,pcFil,(unsigned)sizeof(psEnt->acVal)-1);
               free(psEnt);
               fclose(pfFil);
               free(psHdl);
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
            if (pfOut!=NULL)
               fprintf(pfOut,"Configuration value (%s) too long (>=%u)\n",pcVal,(unsigned)sizeof(psEnt->acVal)-1);
            return(-1);
         } else if (siVal==0) {
            if (psEnt->psNxt!=NULL)
               psEnt->psNxt->psBak=psEnt->psBak;
            else
               psHdl->psLst=psEnt->psBak;
            if (psEnt->psBak!=NULL)
               psEnt->psBak->psNxt=psEnt->psNxt;
            else
               psHdl->psFst=psEnt->psNxt;
            free(psEnt);
         } else {
            if (isOvr || strlen(psEnt->acVal)==0) {
               strcpy(psEnt->acVal,pcVal);
            }else {
               if (pfOut!=NULL)
                  fprintf(pfOut,"Configuration value (%s) for keyword \'%s\' already exists\n",psEnt->acVal,psEnt->acKyw);
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
         if (pfOut!=NULL)
            fprintf(pfOut,"Configuration keyword (%s) too long (>=%u)\n",pcKyw,(unsigned)sizeof(psEnt->acKyw)-1);
         free(psEnt); return(-1);
      }
      if (siVal>=sizeof(psEnt->acVal)-1) {
         if (pfOut!=NULL)
            fprintf(pfOut,"Configuration value (%s) too long (>=%u)\n",pcVal,(unsigned)sizeof(psEnt->acVal)-1);
         free(psEnt); return(-1);
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
   int                           i,j,r;
   TsCnfEnt*                     psEnt;
   for (i=j=0,psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psEnt->psNxt,i++) {
      if (strstr(psEnt->acKyw,pcOwn)!=NULL &&
          strstr(psEnt->acKyw,pcPgm)!=NULL &&
          strstr(psEnt->acKyw,".envar.")!=NULL) {
         const char* pcKyw=strstr(psEnt->acKyw,".envar.")+7;
         if (strlen(pcKyw)+strlen(psEnt->acVal)+2<sizeof(psEnt->acEnv)) {
            sprintf(psEnt->acEnv,"%s=%s",pcKyw,psEnt->acVal);
            r=putenv(psEnt->acEnv);
            if (r==0) {
               const char* pcHlp=getenv(pcKyw);
               if (pcHlp!=NULL) {
                  if (strcmp(pcHlp,psEnt->acVal)==0) {
                     j++;
                  }
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
         const char* pcHlp=getenv(pcKyw);
         if (pcHlp!=NULL) {
            if (strcmp(pcHlp,psEnt->acVal)==0) {
               pcAdd="was verified";
            }
         }
         if (pcPre!=NULL && strlen(pcPre)) {
            fprintf(pfOut,"%s %s=%s # %s\n",pcPre,pcKyw,psEnt->acVal,pcAdd);
         } else {
            fprintf(pfOut,"%s=%s # %s\n",pcKyw,psEnt->acVal,pcAdd);
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
      if (pcPre!=NULL && strlen(pcPre)) {
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
   int                           i;
   TsCnfEnt*                     psEnt;
   TsCnfEnt*                     psHlp;
   for (i=0,psEnt=psHdl->psFst;psEnt!=NULL;psEnt=psHlp,i++) {
      psHlp=psEnt->psNxt;
      memset(psEnt,0,sizeof(TsCnfEnt));
      free(psEnt);
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
         pfFil=fopen(psHdl->acFil,"w");
         if (pfFil!=NULL) fclose(pfFil);
         remove(psHdl->acFil);
      } else {
         psEnt=psHdl->psFst;
         if (psHdl->isChg && strlen(psHdl->acFil)) pfFil=fopen(psHdl->acFil,"w");
         if (pfFil!=NULL) {
            fprintf(pfFil,"# Config file for program \'%s\'\n",psHdl->acPgm);
         }
         while(psEnt!=NULL) {
            if (pfFil!=NULL) fprintf(pfFil,"%s=%s\n",psEnt->acKyw,psEnt->acVal);
            psHlp=psEnt->psNxt; free(psEnt); psEnt=psHlp;
         }
         if (pfFil!=NULL) fclose(pfFil);
         free(psHdl);
      }
   }
}

/**********************************************************************/

