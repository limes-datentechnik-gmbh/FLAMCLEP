/**
 * @file CLPTST.c
 *
 * LIMES Command Line Parser (FLAMCLP) in ANSI-C
 * @author Falk Reichbott
 * @date 27.02.2013\n
 * @copyright (c) 2013 limes datentechnik gmbh
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

#include<time.h>
#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>

#include "CLEPUTL.h"
#include "FLAMCLP.h"


typedef C08  string5[5];


#define DEFINE_STRUCT
#include "CLPMAC.h"

#define NUMTEST_TABLE \
   CLPARGTAB_SKALAR("NUM08", uiNum08, U08, 1, 1, CLPTYP_NUMBER, CLPFLG_NON, 1, NULL    , "23"  , NULL, "Number with 8 bit" ) \
   CLPARGTAB_SKALAR("NUM16", siNum16, I16, 0, 1, CLPTYP_NUMBER, CLPFLG_SEL, 2, asSelect, "NUM3", NULL, "Number with 16 bit") \
   CLPARGTAB_SKALAR("NUM32", uiNum32, U32, 1, 1, CLPTYP_NUMBER, CLPFLG_NON, 3, NULL    , ""    , NULL, "Number with 32 bit") \
   CLPARGTAB_ALIAS( "NUGO6", "NUM32") \
   CLPARGTAB_ALIAS( "HUGO2", "NUM32") \
   CLPARGTAB_SKALAR("NUM64", siNum64, I64, 0, 1, CLPTYP_NUMBER, CLPFLG_NON, 4, asSelect, "NUM2", NULL, "Number with 64 bit") \
   CLPARGTAB_CLS
typedef struct NumTypes {
   NUMTEST_TABLE
} TsNumTypes;


#define FLTTEST_TABLE \
   CLPARGTAB_SKALAR("FLT32", flFlt32, F32, 0, 1, CLPTYP_FLOATN, CLPFLG_NON, 1, NULL    , NULL/*"123.456"*/, NULL, "Float with 32 bit") \
   CLPARGTAB_SKALAR("FLT64", flFlt64, F64, 0, 1, CLPTYP_FLOATN, CLPFLG_SEL, 2, asSelect, NULL/*"PI"*/     , NULL, "Float with 64 bit") \
   CLPARGTAB_CLS
typedef struct FltTypes {
   FLTTEST_TABLE
} TsFltTypes;

#define ALLTYPES_TABLE \
   CLPARGTAB_SKALAR ("SWT"     ,   siSwitch,        I08, 1,  1, CLPTYP_SWITCH, CLPFLG_NON,0xF, NULL         , "0x00", NULL, "Switch to set 0xf")           \
   CLPARGTAB_SKALAR ("NUMTYPES", stNumTypes, TsNumTypes, 0,  1, CLPTYP_OBJECT, CLPFLG_NON,  1, asClpNumTypes, NULL, NULL, "All kind of numbers")         \
   CLPARGTAB_SKALAR ("FLTTYPES", stFltTypes, TsFltTypes, 0,  1, CLPTYP_OBJECT, CLPFLG_NON,  2, asClpFltTypes, NULL, NULL, "All kind of floats")          \
   CLPARGTAB_SKALAR ("STR09"   ,   siStrLen,        I16, 0,  1, CLPTYP_NUMBER, CLPFLG_SLN,  3, NULL         , NULL, NULL, "16 bit integer for strlen")   \
   CLPARGTAB_STRING ("STR09"   ,      acStr,          9, 0,  1, CLPTYP_STRING, CLPFLG_NON,  4, asSelect     , NULL, NULL, "String with 8 chars")         \
   CLPARGTAB_SKALAR ("BIN32"   ,   uiBinLen,        U32, 0,  1, CLPTYP_NUMBER, CLPFLG_TLN,  5, NULL         , NULL, NULL, "32 bit integer for binlen")   \
   CLPARGTAB_STRING ("BIN32"   ,      acBin,         32, 0,  1, CLPTYP_STRING, CLPFLG_BIN,  6, NULL         , NULL, NULL, "Binary with 32 byte")         \
   CLPARGTAB_SKALAR ("NUM4L"   ,   siNumCnt,        I32, 0,  1, CLPTYP_NUMBER, CLPFLG_CNT,  7, NULL         , NULL, NULL, "32 bit integer for numcnt")   \
   CLPARGTAB_ARRAY  ("NUM4L"   ,   aiNumLst,        I32, 0,  4, CLPTYP_NUMBER, CLPFLG_NON,  8, NULL         , NULL, NULL, "Number array with 4 integer") \
   CLPARGTAB_SKALAR ("STR45"   ,   siStrCnt,        U16, 0,  1, CLPTYP_NUMBER, CLPFLG_CNT,  9, NULL         , NULL, NULL, "16 bit integer for strcnt")   \
   CLPARGTAB_ARRAY  ("STR45"   ,   acStrLst,    string5, 0,  4, CLPTYP_STRING, CLPFLG_FIX, 10, NULL         , NULL, NULL, "String with 8 chars")         \
   CLPARGTAB_SKALAR ("BIN8L"   ,   siBinCnt,        I08, 0,  1, CLPTYP_NUMBER, CLPFLG_CNT, 11, NULL         , NULL, NULL, "8 bit integer for bincnt")    \
   CLPARGTAB_ARRAY  ("BIN8L"   ,   aiBinLen,        I08, 0,  8, CLPTYP_NUMBER, CLPFLG_ELN, 12, NULL         , NULL, NULL, "Length array for 8 binaries") \
   CLPARGTAB_STRING ("BIN8L"   ,   acBinLst,         64, 0,  8, CLPTYP_STRING, CLPFLG_BIN, 13, NULL         , NULL, NULL, "Binary with 64 bytes")        \
   CLPARGTAB_CLS
typedef struct AllTypes {
   ALLTYPES_TABLE
} TsAllTypes;

#define OVERLAY_TABLE \
   CLPARGTAB_SKALAR ("SWT", siSwt,        I16, 0, 1, CLPTYP_SWITCH, CLPFLG_NON, 1,          NULL, NULL, NULL, "Switch to set 0xf")             \
   CLPARGTAB_STRING ("STR", acStr,         33, 0, 1, CLPTYP_STRING, CLPFLG_NON, 2,          NULL, NULL/*"'FALK'"*/, NULL, "String with 33 chars")          \
   CLPARGTAB_SKALAR ("ALL", stAll, TsAllTypes, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 3, asClpAllTypes, NULL, NULL, "All kind of types (overlay)")   \
   CLPARGTAB_SKALAR ("NUM", stNum, TsNumTypes, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 4, asClpNumTypes, NULL, NULL, "All kind of numbers (overlay)") \
   CLPARGTAB_SKALAR ("FLT", stFlt, TsFltTypes, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 5, asClpFltTypes, NULL/*"FLT"*/, NULL, "All kind of floats (overlay)")  \
   CLPARGTAB_CLS
typedef union Overlay {
   OVERLAY_TABLE
} TuOverlay;

#define TEST_TABLE \
   CLPARGTAB_SKALAR ("NUM"  ,    stNum, TsNumTypes, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 1, asClpNumTypes, NULL, NULL, "All kind of numbers (tst)") \
   CLPARGTAB_SKALAR ("NUM"  , siNumOid,        I32, 0, 1, CLPTYP_NUMBER, CLPFLG_OID, 2, NULL         , NULL, NULL, "32 bit integer for oidnum") \
   CLPARGTAB_SKALAR ("ALL3L", siAllCnt,        I32, 0, 1, CLPTYP_NUMBER, CLPFLG_CNT, 3, NULL         , NULL, NULL, "32 bit integer for allcnt") \
   CLPARGTAB_ARRAY  ("ALL3L",    asAll, TsAllTypes, 0, 3, CLPTYP_OBJECT, CLPFLG_NON, 4, asClpAllTypes, NULL, NULL, "All kind of types (tst)")   \
   CLPARGTAB_SKALAR ("OVL4L", siOvlCnt,        I32, 0, 1, CLPTYP_NUMBER, CLPFLG_CNT, 5, NULL         , NULL, NULL, "32 bit integer for ovlcnt") \
   CLPARGTAB_ARRAY  ("OVL4L", aiOvlMod,        I32, 0, 4, CLPTYP_NUMBER, CLPFLG_OID, 6, NULL         , NULL, NULL, "List for Oid's")            \
   CLPARGTAB_ARRAY  ("OVL4L",    auOvl,  TuOverlay, 0, 4, CLPTYP_OVRLAY, CLPFLG_NON, 7, asClpOverlay , NULL/*"STR FLT ALL STR"*/, NULL, "Overlay for test")          \
   CLPARGTAB_CLS
typedef struct Tst {
   TEST_TABLE
} TsTst;

#define LOG_TABLE \
   CLPARGTAB_SKALAR ("DUMMY", uiDummy, I32, 0, 1, CLPTYP_NUMBER, CLPFLG_NON, 1, NULL, NULL, NULL, "Just a dummy") \
   CLPARGTAB_CLS
typedef struct Log {
   LOG_TABLE
}TsLog;

#define MAIN_TABLE \
   CLPARGTAB_SKALAR("INPUT" , stInp, TsTst, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 1, asClpTst, NULL, NULL, "Struture to define inbound parameter")  \
   CLPARGTAB_SKALAR("OUTPUT", stOut, TsTst, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 2, asClpTst, NULL, NULL, "Struture to define outbound parameter") \
   CLPARGTAB_SKALAR("LOG"   , stLog, TsLog, 0, 1, CLPTYP_OBJECT, CLPFLG_NON, 3, asClpLog, NULL, NULL, "Struture to define log parameter")      \
   CLPARGTAB_CLS
typedef struct Main {
   MAIN_TABLE
} TsMain;

/*---------------------------------------------------------*/

#undef DEFINE_STRUCT
#include "CLPMAC.h"

CLPCONTAB_OPN(asSelect) = {
   CLPCONTAB_NUMBER("NUM0",0        ,NULL, "Defines zero")
   CLPCONTAB_NUMBER("NUM1",1        ,NULL, "Defines one")
   CLPCONTAB_NUMBER("NUM2",2        ,NULL, "Defines two")
   CLPCONTAB_NUMBER("NUM3",3        ,NULL, "Defines tree")
   CLPCONTAB_FLOATN("FLT1",123.45   ,NULL, "Test fuer Float")
   CLPCONTAB_FLOATN("PI"  ,3.41     ,NULL, "Float PI 3.41")
   CLPCONTAB_STRING("STR1","String1",NULL, "Hilfe fuer den Teststring 1 ")
   CLPCONTAB_STRING("STR2","String2",NULL, "Hilfe fuer den Teststring 2")
   CLPCONTAB_STRING("STR3","String3",NULL, "Hilfe fuer den Teststring 3")
   CLPCONTAB_BINARY("BIN0","\x00",1 ,NULL, "Binaere Null 1 Byte")
   CLPCONTAB_BINARY("BIN1","\x00\x88\xFF",3,NULL, "Binaere 0088FF 3 Byte")
   CLPCONTAB_CLS
};

#undef  STRUCT_NAME
#define STRUCT_NAME TsNumTypes
TsClpArgument asClpNumTypes[] = {
   NUMTEST_TABLE
};

#undef  STRUCT_NAME
#define STRUCT_NAME TsFltTypes
TsClpArgument asClpFltTypes[] = {
   FLTTEST_TABLE
};

#undef  STRUCT_NAME
#define STRUCT_NAME TsAllTypes
TsClpArgument asClpAllTypes[] = {
   ALLTYPES_TABLE
};

#undef  STRUCT_NAME
#define STRUCT_NAME TuOverlay
TsClpArgument asClpOverlay[] = {
   OVERLAY_TABLE
};

#undef  STRUCT_NAME
#define STRUCT_NAME TsTst
TsClpArgument asClpTst[] = {
   TEST_TABLE
};

#undef  STRUCT_NAME
#define STRUCT_NAME TsLog
TsClpArgument asClpLog[] = {
   LOG_TABLE
};

#undef  STRUCT_NAME
#define STRUCT_NAME TsMain
TsClpArgument asMainArgTab[] = {
   MAIN_TABLE
};

/******************************************************************************/

static void printNumTypes(
   const char*    pcPfx,
   TsNumTypes*    psDat)
{
   printf("%s.uiNum08=%d\n",pcPfx,(int)psDat->uiNum08);
   printf("%s.siNum16=%d\n",pcPfx,(int)psDat->siNum16);
   printf("%s.uiNum32=%d\n",pcPfx,(int)psDat->uiNum32);
   printf("%s.siNum64=%d\n",pcPfx,(int)psDat->siNum64);
}

static void printFltTypes(
   const char*    pcPfx,
   TsFltTypes*    psDat)
{
   printf("%s.flFlt32=%g\n",pcPfx,(float)psDat->flFlt32);
   printf("%s.flFlt64=%f\n",pcPfx,(float)psDat->flFlt64);
}

static void printAllTypes(
   const char*    pcPfx,
   TsAllTypes*    psDat)
{
   char           acPre[1025]="";
   int            i,j,k;
   printf("%s.siSwitch=%d\n",pcPfx,(int)psDat->siSwitch);
   sprintf(acPre,"%s.stNumTypes",pcPfx);
   printNumTypes(acPre,&psDat->stNumTypes);
   sprintf(acPre,"%s.stFltTypes",pcPfx);
   printFltTypes(acPre,&psDat->stFltTypes);
   printf("%s.siStrLen=%d\n",pcPfx,(int)psDat->siStrLen);
   printf("%s.acStr=\'%s\'\n",pcPfx,psDat->acStr);
   printf("%s.uiBinLen=%d\n",pcPfx,(int)psDat->uiBinLen);
   printf("%s.acBin=\'",pcPfx);
   for (i=0;i<psDat->uiBinLen;i++) printf("%2.2X",psDat->acBin[i]);
   printf("\'x\n");
   printf("%s.siNumCnt=%d\n",pcPfx,(int)psDat->siNumCnt);
   for (i=0;i<psDat->siNumCnt;i++) {
      printf("%s.aiNumLst[%d]=%d\n",pcPfx,i,(int)psDat->aiNumLst[i]);
   }
   printf(      "%s.siStrCnt=%d\n",pcPfx,(int)psDat->siStrCnt);
   for (i=0;i<psDat->siStrCnt;i++) {
      printf("%s.acStrLst[%d]=\'%s\'\n",pcPfx,i,psDat->acStrLst[i]);
   }
   printf(      "%s.siBinCnt=%d\n",pcPfx,(int)psDat->siBinCnt);
   for (k=0,i=0;i<psDat->siBinCnt;i++) {
      printf("%s.aiBinLen[%d]=%d\n",pcPfx,i,(int)psDat->aiBinLen[i]);
      printf("%s.acBinLst[%d]=\'",pcPfx,i);
      for (j=0;j<psDat->aiBinLen[i];j++,k++) printf("%2.2X",psDat->acBinLst[k]);
      printf("\'x\n");
   }
}

static void printOverlay(
   const char*    pcPfx,
   TuOverlay*     psDat,
   const int      siOid)
{
   char           acPre[1025]="";
   switch (siOid) {
   case 1: printf("%s.siSwt=%d\n",pcPfx,(int)psDat->siSwt);break;
   case 2: printf("%s.acStr=\'%s\'\n",pcPfx,     psDat->acStr);break;
   case 3:
      sprintf(acPre,"%s.stAll",pcPfx);
      printAllTypes(acPre,&psDat->stAll);
      break;
   case 4:
      sprintf(acPre,"%s.stNum",pcPfx);
      printNumTypes(acPre,&psDat->stNum);
      break;
   case 5:
      sprintf(acPre,"%s.stFlt",pcPfx);
      printFltTypes(acPre,&psDat->stFlt);
      break;
   }
}

static void printTst(
   const char*    pcPfx,
   TsTst*         psDat)
{
   char           acPre[1025]="";
   int            i;
   sprintf(acPre,"%s.stNumTypes"   ,pcPfx);
   printNumTypes(acPre,&psDat->stNum);
   printf("%s.siNumOid=%d\n",pcPfx,(int)psDat->siNumOid);
   printf("%s.siAllCnt=%d\n",pcPfx,(int)psDat->siAllCnt);
   for (i=0;i<psDat->siAllCnt;i++) {
      sprintf(acPre,"%s.asAll[%d]",pcPfx,i);
      printAllTypes(acPre,&psDat->asAll[i]);
   }
   printf("%s.siOvlCnt=%d\n",pcPfx,(int)psDat->siOvlCnt);
   for (i=0;i<psDat->siOvlCnt;i++) {
      sprintf(acPre,"%s.auOvl[%d]",pcPfx,i);
      printOverlay(acPre,&psDat->auOvl[i],psDat->aiOvlMod[i]);
   }
}

static void printMain(
   TsMain*         psDat)
{
   char           acPre[1025]="";
   sprintf(acPre,"Main.stInp");
   printTst(acPre,&psDat->stInp);
   sprintf(acPre,"Main.stOut");
   printTst(acPre,&psDat->stOut);
}

/******************************************************************************/

int main(int argc, char * argv[])
{
   int               siCnt,l,i;
   void*             pvHdl;
   char              acBuf[65536];
   char*             pcLst;
   FILE*             pfPar=fopen("clptst.txt","r");


   TsMain            stMain;

   memset(&stMain,0,sizeof(stMain));

   if (pfPar!=NULL) {
      l=fread(acBuf,1,sizeof(acBuf),pfPar);
      fclose(pfPar);
      acBuf[l]=EOS;
      pvHdl=pvClpOpen(FALSE,TRUE,1,"de.limes","CLPTST","MAIN","man-page","help-msg",FALSE,asMainArgTab,&stMain,stderr,stderr,NULL,NULL,NULL,NULL,"-->","/",",",NULL);
      if (pvHdl!=NULL) {
         fprintf(stderr,"SYNTAX required:\n"); siClpSyntax(pvHdl,FALSE,FALSE,10,NULL); fprintf(stderr,"\n");
         fprintf(stderr,"SYNTAX optional:\n"); siClpSyntax(pvHdl,TRUE,TRUE,10,NULL); fprintf(stderr,"\n");
         siClpHelp(pvHdl,10,NULL,TRUE,TRUE);
         fprintf(stderr,"*** PROPERTY FILE PARSER ***\n");
         siCnt=siClpParseCmd(pvHdl,NULL,acBuf,TRUE,NULL,&pcLst);
         if (siCnt<0) {
            switch (siCnt) {
               case CLPERR_LEX:fprintf(stderr,"LEXICAL-ERROR\n");break;
               case CLPERR_SYN:fprintf(stderr,"SYNTAX-ERROR\n");break;
               case CLPERR_SEM:fprintf(stderr,"SEMANTIC-ERROR\n");break;
               case CLPERR_TYP:fprintf(stderr,"TYPE-ERROR\n");break;
               case CLPERR_TAB:fprintf(stderr,"TABLE-ERROR\n");break;
               case CLPERR_SIZ:fprintf(stderr,"SIZE-ERROR\n");break;
               case CLPERR_SYS:fprintf(stderr,"SYSTEM-ERROR\n");break;
               default        :fprintf(stderr,"UNKOWN-ERROR(%d)\n",siCnt);break;
            }
            fprintf(stderr,"*** PARSING FAILED ***\n");
         } else {
            fprintf(stderr,"*** PARSING SUCCESSFULL ***\n");
         }
         acBuf[0] = 0;
         for (i=1 ; i < argc ; i++) {
            if (strlen(acBuf)+strlen(argv[i])<sizeof(acBuf)-2) {
               strcat(acBuf, argv[i]);
               strcat(acBuf, " ");
            }
         }
         fprintf(stderr,"*** COMMAND LINE PARSER ***\n");
         fprintf(stderr,"line: %s\n", acBuf);
         siCnt=siClpParseCmd(pvHdl,NULL,acBuf,TRUE,NULL,&pcLst);
         if (siCnt<0) {
            switch (siCnt) {
               case CLPERR_LEX:fprintf(stderr,"LEXICAL-ERROR\n");break;
               case CLPERR_SYN:fprintf(stderr,"SYNTAX-ERROR\n");break;
               case CLPERR_SEM:fprintf(stderr,"SEMANTIC-ERROR\n");break;
               case CLPERR_TYP:fprintf(stderr,"TYPE-ERROR\n");break;
               case CLPERR_TAB:fprintf(stderr,"TABLE-ERROR\n");break;
               case CLPERR_SIZ:fprintf(stderr,"SIZE-ERROR\n");break;
               case CLPERR_SYS:fprintf(stderr,"SYSTEM-ERROR\n");break;
               default        :fprintf(stderr,"UNKOWN-ERROR(%d)\n",siCnt);break;
            }
            fprintf(stderr,"*** LIST OF PARSED ARGUMENTS ***\n");
            fprintf(stderr,"%s",pcLst);
            fprintf(stderr,"*** PARSING FAILED ***\n");
         } else {
            fprintf(stderr,"*** PARSING SUCCESSFULL ***\n");
         }
         fflush(stdout);
         fflush(stderr);
         vdClpClose(pvHdl);
#ifdef __GEN__
         {
            FILE     *f=fopen("clptst.bin","w");
            fwrite(&stMain,sizeof(stMain),1,f);
            fclose(f);
         }
#else
         {
            TsMain   stHelp;
            FILE     *f=fopen("clptst.bin","r");
            int      i;
            memset(&stHelp,0,sizeof(stHelp));
            fread(&stHelp,sizeof(stMain),1,f);
            fclose(f);
            if (memcmp(&stMain,&stHelp,sizeof(stMain))) {
               for (i=0;i<sizeof(stMain) && ((char*)&stMain)[i]==((char*)&stHelp)[i];i++);
               printf("\n!!! Verification not successfull !!! Position: %d(%u)\n",i,(U32)sizeof(stMain));
               printf(  "                 ^^^                \n");
            } else {
               printf("\n*** Verification successfull ***\n");
            }
         }

#endif

         printf("\n*** Output ***\n");
         printMain(&stMain);
         printf("*** Output ***\n");
      } else fprintf(stderr,"*** Open CLP failed ***\n");
   } else fprintf(stderr,"*** Open clptst.txt failed ***\n");
   return(0);
}
