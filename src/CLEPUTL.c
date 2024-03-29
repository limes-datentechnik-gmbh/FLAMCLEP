/**
 * @file
 * @brief  Implementierung diverser Hilffunktionen in ANSI C
 * @author limes datentechnik gmbh
 * @date  06.03.2015
 * @copyright limes datentechnik gmbh
 * www.flam.de
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
 *
 **********************************************************************/

/*Standard-Includes                                                   */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <locale.h>
#include <time.h>
#include <sys/stat.h>
#ifdef __UNIX__
#  include <langinfo.h>
#endif
#ifdef __WIN__
#  include <windows.h>
#  include <olectl.h>
#endif
#if defined(__ZOS__) && defined(__FL5__)
#  include "FSTATZOS.h"
#  include "FLZASM31.h"
#  define flzsym FLZSYM
#  define flzjsy FLZJSY
#else
static inline int flzsym(const char* pcDat, const int* piSln, char* pcVal, int* piVln) {
   (void)pcDat; (void)piSln;
   memset(pcVal,0,*piVln);
   *piVln=0;
   return(8);
}
static inline int flzjsy(const char* pcDat, const int* piSln, char* pcVal, int* piVln) {
   (void)pcDat; (void)piSln;
   memset(pcVal,0,*piVln);
   *piVln=0;
   return(8);
}
#endif /* __ZOS__ */
#if !defined(__USS__) && !defined(__ZOS__) && defined(__FL5__)
#  include "mfinit.h"
#endif

#if defined(__DEBUG__) && defined(__FL5__)
#  include "CHKMEM.h"
#endif
#include "CLEPUTL.h"

#ifndef realloc_nowarn
#  define realloc_nowarn      realloc
#endif

#ifndef fopen_nowarn
#  define fopen_nowarn        fopen
#endif

#ifdef __ZOS__
   static inline const char* filemode(const char* mode) {
      if(mode!=NULL) {
         if(strcmp(mode,"r")==0 || strcmp(mode,"rt")==0){
            return "r, noseek, samethread, abend=recover";
         }
         if(strcmp(mode,"r+")==0 || strcmp(mode,"rt+")==0 || strcmp(mode,"r+t")==0){
            return "r+, noseek, samethread, abend=recover";
         }
         if(strcmp(mode,"rb")==0){
            return "rb, noseek, samethread, abend=recover";
         }
         if(strcmp(mode,"rb+")==0 || strcmp(mode,"r+b")==0){
            return "rb+, noseek, samethread, abend=recover";
         }
         if(strcmp(mode,"rs")==0 || strcmp(mode,"rts")==0){
            return "r, byteseek, samethread, abend=recover";
         }
         if(strcmp(mode,"rs+")==0 || strcmp(mode,"r+s")==0 || strcmp(mode,"rts+")==0 || strcmp(mode,"r+ts")==0){
            return "r+, byteseek, samethread, abend=recover";
         }
         if(strcmp(mode,"rbs")==0){
            return "rb, byteseek, samethread, abend=recover";
         }
         if(strcmp(mode,"rbs+")==0 || strcmp(mode,"r+bs")==0){
            return "rb+, byteseek, samethread, abend=recover";
         }

         if(strcmp(mode,"w")==0 || strcmp(mode,"wt")==0){
            return "w, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"w+")==0 || strcmp(mode,"wt+")==0 || strcmp(mode,"w+t")==0){
            return "w+, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"wb")==0){
            return "wb, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"wb+")==0 || strcmp(mode,"w+b")==0){
            return "wb+, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"ws")==0 || strcmp(mode,"wts")==0){
            return "w, byteseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"ws+")==0 || strcmp(mode,"w+s")==0 || strcmp(mode,"wts+")==0 || strcmp(mode,"w+ts")==0){
            return "w+, byteseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"wbs")==0){
            return "wb, byteseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"wbs+")==0 || strcmp(mode,"w+bs")==0){
            return "wb+, byteseek, samethread, abend=recover, recfm=*";
         }

         if(strcmp(mode,"a")==0 || strcmp(mode,"at")==0){
            return "a, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"a+")==0 || strcmp(mode,"at+")==0 || strcmp(mode,"a+t")==0){
            return "a+, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"ab")==0){
            return "ab, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"ab+")==0 || strcmp(mode,"a+b")==0){
            return "ab+, noseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"as")==0 || strcmp(mode,"ats")==0){
            return "a, byteseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"as+")==0 || strcmp(mode,"a+s")==0 || strcmp(mode,"ats+")==0  || strcmp(mode,"a+ts")==0){
            return "a+, byteseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"abs")==0){
            return "ab, byteseek, samethread, abend=recover, recfm=*";
         }
         if(strcmp(mode,"abs+")==0 || strcmp(mode,"a+bs")==0){
            return "ab+, byteseek, samethread, abend=recover, recfm=*";
         }
      }
      return mode;
   }
#  define DEFAULT_DCB "recfm=VB, lrecl=516"
   static inline FILE* fopen_hst(const char* name, const char* mode) {
      FILE*       f;
      char        acMode[strlen(mode)+32];
      char*       pcRecfm;
      const char* pcDefDcb=GETENV("CLP_DEFAULT_DCB");
      if (pcDefDcb==NULL || *pcDefDcb) pcDefDcb=DEFAULT_DCB;
      strcpy(acMode,mode);
      if ((acMode[0]=='a' && acMode[1]=='b' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='t' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='+')) {
         acMode[0]='r';
         f=fopen(name, acMode);
         if (f!=NULL) return(f);
         acMode[0]='w';
         pcRecfm=strstr(acMode,"recfm=*");
         if (pcRecfm!=NULL) {
            pcRecfm[0]=0x00;
            strcat(acMode,pcDefDcb);
         }
      } else {
         if (!ISPATHNAME(name) && !ISDDNAME(name)) {
            if (acMode[0]=='w' || acMode[0]=='a') {
               pcRecfm=strstr(acMode,"recfm=*");
               if (pcRecfm!=NULL) {
                  char r=pcRecfm[0];
                  char m=acMode[0];
                  pcRecfm[0]=0x00;
                  acMode[0]='r';
                  f=fopen(name, acMode);
                  if (f!=NULL) {
                     fclose(f);
                     acMode[0]=m;
                     pcRecfm[0]=r;
                  } else {
                     acMode[0]=m;
                     strcat(acMode,pcDefDcb);
                  }
               }
            }
         }
      }
      f=fopen(name, acMode);
      return(f);
   }
   static inline FILE* fopen_hst_nowarn(const char* name, const char* mode) {
      FILE*       f;
      char        acMode[strlen(mode)+32];
      char*       pcRecfm;
      const char* pcDefDcb=GETENV("CLP_DEFAULT_DCB");
      if (pcDefDcb==NULL || *pcDefDcb==0x00) pcDefDcb=DEFAULT_DCB;
      strcpy(acMode,mode);
      if ((acMode[0]=='a' && acMode[1]=='b' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='t' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='+')) {
         acMode[0]='r';
         f=fopen_nowarn(name, acMode);
         if (f!=NULL) return(f);
         acMode[0]='w';
         pcRecfm=strstr(acMode,"recfm=*");
         if (pcRecfm!=NULL) {
            pcRecfm[0]=0x00;
            strcat(acMode,pcDefDcb);
         }
      } else {
         if (!ISPATHNAME(name) && !ISDDNAME(name)) {
            if (acMode[0]=='w' || acMode[0]=='a') {
               pcRecfm=strstr(acMode,"recfm=*");
               if (pcRecfm!=NULL) {
                  char r=pcRecfm[0];
                  char m=acMode[0];
                  pcRecfm[0]=0x00;
                  acMode[0]='r';
                  f=fopen_nowarn(name, acMode);
                  if (f!=NULL) {
                     fclose(f);
                     acMode[0]=m;
                     pcRecfm[0]=r;
                  } else {
                     acMode[0]=m;
                     strcat(acMode,pcDefDcb);
                  }
               }
            }
         }
      }
      f=fopen_nowarn(name, acMode);
      return(f);
   }
   extern FILE* fopen_hfq(const char* name, const char* mode) {
      if (ISPATHNAME(name) || ISDDNAME(name)) {
         return(fopen_hst(name, filemode(mode)));
      } else {
         if (name[0]=='\'') {
            FILE* f;
            int i=1;
            while (i<9 && ISDDN(name[i])) i++;
            if (name[i]=='\'' || name[i]=='(' || name[i]==0x00) { // only one qualifier -> try static allocation first
               int j;
               char path[12]="DD:";
               for(j=1;j<i;j++) path[j+2]=name[j];
               path[j+2]=0x00;
               f=fopen_hst(path, filemode(mode));
               // cppcheck-suppress knownConditionTrueFalse
               if (f==NULL) { // no static allocation available -> try dynamic allocation of only one qualifier
                  f=fopen_hst(name, filemode(mode));
               }
            } else { // more than one qualifier
               f=fopen_hst(name, filemode(mode));
            }
            return(f);
         } else {
            FILE* f;
            int i=0;
            while (i<8 && ISDDN(name[i])) i++;
            if (name[i]=='(' || name[i]==0x00) { // only one qualifier -> try static allocation first
               int j;
               char path[12]="DD:";
               for(j=0;j<i;j++) path[j+3]=name[j];
               path[j+3]=0x00;
               f=fopen_hst(path, filemode(mode));
               // cppcheck-suppress knownConditionTrueFalse
               if (f==NULL) { // no static allocation available -> try dynamic allocation of only one qualifier
                  char help[strlen(name)+3];
                  snprintf(help,sizeof(help),"'%s'",name);
                  f=fopen_hst(help, filemode(mode));
               }
            } else { // more than one qualifier
               char help[strlen(name)+3];
               snprintf(help,sizeof(help),"'%s'",name);
               f=fopen_hst(help, filemode(mode));
            }
            return(f);
         }
      }
   }
   extern FILE* fopen_hfq_nowarn(const char* name, const char* mode) {
      if (ISPATHNAME(name) || ISDDNAME(name)) {
         return(fopen_hst_nowarn(name, filemode(mode)));
      } else {
         if (name[0]=='\'') {
            FILE* f;
            int i=1;
            while (i<9 && ISDDN(name[i])) i++;
            if (name[i]=='\'' || name[i]=='(' || name[i]==0x00) { // only one qualifier -> try static allocation first
               int j;
               char path[12]="DD:";
               for(j=1;j<i;j++) path[j+2]=name[j];
               path[j+2]=0x00;
               f=fopen_hst_nowarn(path, filemode(mode));
               // cppcheck-suppress knownConditionTrueFalse
               if (f==NULL) { // no static allocation available -> try dynamic allocation of only one qualifier
                  f=fopen_hst_nowarn(name, filemode(mode));
               }
            } else { // more than one qualifier
               f=fopen_hst_nowarn(name, filemode(mode));
            }
            return(f);
         } else {
            FILE* f;
            int i=0;
            while (i<8 && ISDDN(name[i])) i++;
            if (name[i]=='(' || name[i]==0x00) { // only one qualifier -> try static allocation first
               int j;
               char path[12]="DD:";
               for(j=0;j<i;j++) path[j+3]=name[j];
               path[j+3]=0x00;
               f=fopen_hst_nowarn(path, filemode(mode));
               // cppcheck-suppress knownConditionTrueFalse
               if (f==NULL) { // no static allocation available -> try dynamic allocation of only one qualifier
                  char help[strlen(name)+3];
                  snprintf(help,sizeof(help),"'%s'",name);
                  f=fopen_hst_nowarn(help, filemode(mode));
               }
            } else { // more than one qualifier
               char help[strlen(name)+3];
               snprintf(help,sizeof(help),"'%s'",name);
               f=fopen_hst_nowarn(help, filemode(mode));
            }
            return(f);
         }
      }
   }
   extern FILE* freopen_hfq(const char* name, const char* mode, FILE* stream) {
      if (name==NULL || *name==':') {
         return(freopen("", filemode(mode), stream));
      } else if (*name=='\'' || *name=='(' || ISPATHNAME(name) || ISDDNAME(name)) {
         return(freopen(name, filemode(mode), stream));
      }else {
         char help[strlen(name)+3];
         snprintf(help,sizeof(help),"'%s'",name);
         return(freopen(help, filemode(mode), stream));
      }
   }
   extern FILE* fopen_tmp(void) {
      FILE* f=fopen("*","wb+,type=memory(hiperspace)");
      if (f==NULL) {
         f=fopen("*","wb+,type=memory");
      }
      return(f);
   }
   extern int fclose_tmp(FILE* fp) {
      int      r;
      char     fn[FILENAME_MAX]="";
      fldata_t fi={0};
      fldata((fp),fn,&fi);
      r=fclose((fp));
      remove(fn);
      return(r);
   }
   extern int remove_hfq(const char* name) {
      if (ISPATHNAME(name) || ISDDNAME(name) || name[0]=='\'' || name[0]=='(') {
         return(remove(name));
      } else {
         char help[strlen(name)+3];
         snprintf(help,sizeof(help),"'%s'",name);
         return(remove(help));
      }
   }
   extern long long getFileSize(const char* name) {
      if (ISPATHNAME(name)) {
         C08* pcName=dmapfil(name,0);
         if (pcName!=NULL) {
            struct stat stSta;
            memset(&stSta,0,sizeof(stSta));
            if (stat(pcName,&stSta)==0) {
               free(pcName);
               return(stSta.st_size);
            }
            free(pcName);
         } else {
            errno=ENOMEM;
         }
      } else {
#ifdef __FL5__
         C08*                 pcName=dmapfil(name,0);
         if (pcName!=NULL) {
            TsFileStatZOS        stZos;
            C08*                 pcHlp=strchr(pcName,'(');
            if (pcHlp!=NULL) *pcHlp=0x00;
            memset(&stZos,0,sizeof(stZos));
            if (fstatZos(pcName,&stZos)==0) {
               free(pcName);
               U64 uiFilSiz=0;
               if (stZos.datasetIsNonVSAM) {
                  for (int i=0;i<stZos._nonVSAMAttr._spaceAllocation.numberOfExtents;i++) {
                     uiFilSiz+=stZos._nonVSAMAttr._spaceAllocation._extents[i].tracks<<16;
                  }
               }
               if (stZos.approxSize && (uiFilSiz && stZos.approxSize<uiFilSiz)) {
                  return(stZos.approxSize);
               }
               return(uiFilSiz);
            } else {
               errno=EINVAL;
            }
            free(pcName);
         } else {
            errno=ENOMEM;
         }
#endif
      }
      return(-1);
   }
#else
   static inline const char* filemode(const char* mode) {
      if(mode!=NULL) {
         if(strcmp(mode,"r")==0 || strcmp(mode,"rt")==0 || strcmp(mode,"rs")==0 || strcmp(mode,"rts")==0){
            return "r";
         }
         if(strcmp(mode,"r+")==0 || strcmp(mode,"rs+")==0 || strcmp(mode,"rt+")==0 || strcmp(mode,"rts+")==0 || strcmp(mode,"r+t")==0 || strcmp(mode,"r+ts")==0){
            return "r+";
         }
         if(strcmp(mode,"rb")==0 || strcmp(mode,"rbs")==0){
            return "rb";
         }
         if(strcmp(mode,"rb+")==0 || strcmp(mode,"rbs+")==0 || strcmp(mode,"r+b")==0 || strcmp(mode,"r+bs")==0){
            return "rb+";
         }
         if(strcmp(mode,"w")==0 || strcmp(mode,"wt")==0 || strcmp(mode,"ws")==0 || strcmp(mode,"wts")==0){
            return "w";
         }
         if(strcmp(mode,"w+")==0 || strcmp(mode,"ws+")==0 || strcmp(mode,"wt+")==0 || strcmp(mode,"wts+")==0 || strcmp(mode,"w+t")==0 || strcmp(mode,"w+ts")==0){
            return "w+";
         }
         if(strcmp(mode,"wb")==0 || strcmp(mode,"wbs")==0){
            return "wb";
         }
         if(strcmp(mode,"wb+")==0 || strcmp(mode,"wbs+")==0 || strcmp(mode,"w+b")==0 || strcmp(mode,"w+bs")==0){
            return "wb+";
         }
         if(strcmp(mode,"a")==0 || strcmp(mode,"at")==0 || strcmp(mode,"as")==0 || strcmp(mode,"ats")==0){
            return "a";
         }
         if(strcmp(mode,"a+")==0 || strcmp(mode,"as+")==0 || strcmp(mode,"at+")==0 || strcmp(mode,"ats+")==0 || strcmp(mode,"a+t")==0 || strcmp(mode,"a+ts")==0){
            return "a+";
         }
         if(strcmp(mode,"ab")==0 || strcmp(mode,"abs")==0){
            return "ab";
         }
         if(strcmp(mode,"ab+")==0 || strcmp(mode,"abs+")==0 || strcmp(mode,"a+b")==0 || strcmp(mode,"a+bs")==0){
            return "ab+";
         }
      }
      return mode;
   }
   static inline FILE* fopen_opn(const char* name, const char* mode) {
      FILE* f;
      char  acMode[strlen(mode)+32];
      strcpy(acMode,mode);
      if ((acMode[0]=='a' && acMode[1]=='b' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='t' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='+')) {
         acMode[0]='r';
         f=fopen(name, acMode);
         if (f!=NULL) return(f);
         acMode[0]='w';
      }
      f=fopen(name, acMode);
      return(f);
   }
   static inline FILE* fopen_opn_nowarn(const char* name, const char* mode) {
      FILE* f;
      char  acMode[strlen(mode)+32];
      strcpy(acMode,mode);
      if ((acMode[0]=='a' && acMode[1]=='b' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='t' && acMode[2]=='+') ||
          (acMode[0]=='a' && acMode[1]=='+')) {
         acMode[0]='r';
         f=fopen_nowarn(name, acMode);
         if (f!=NULL) return(f);
         acMode[0]='w';
      }
      f=fopen_nowarn(name, acMode);
      return(f);
   }
   extern FILE* fopen_hfq(const char* name, const char* mode) {
      return(fopen_opn(name, filemode(mode)));
   }
   extern FILE* fopen_hfq_nowarn(const char* name, const char* mode) {
      return(fopen_opn_nowarn(name, filemode(mode)));
   }
   extern FILE* freopen_hfq(const char* name, const char* mode, FILE* stream) {
      return(freopen(name, filemode(mode), stream));
   }
   extern long long getFileSize(const char* name) {
      char* pcName=dmapfil(name,0);
      if (pcName!=NULL) {
         struct stat stSta;
         memset(&stSta,0,sizeof(stSta));
         if (stat(pcName,&stSta)==0) {
            free(pcName);
            return(stSta.st_size);
         }
         free(pcName);
      } else {
         errno=ENOMEM;
      }
      return(-1);
   }
#endif

extern void init_diachr(TsDiaChr* psDiaChr,const unsigned int uiCcsId) {
   switch (uiCcsId) {
#ifdef __EBCDIC__
   case 37:
   case 424:
   case 1140:
      psDiaChr->exc[0]='\x5A';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\xBA';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\xBB';
      psDiaChr->crt[0]='\xB0';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x4F';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xA1';
      break;
   case 273:
   case 1141:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\xB5';
      psDiaChr->sbo[0]='\x63';
      psDiaChr->bsl[0]='\xEC';
      psDiaChr->sbc[0]='\xFC';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\x43';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\xDC';
      psDiaChr->tld[0]='\x59';
      break;
   case 277:
   case 1142:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x4A';
      psDiaChr->dlr[0]='\x67';
      psDiaChr->ats[0]='\x80';
      psDiaChr->sbo[0]='\x9E';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\x9F';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\x9C';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\x47';
      psDiaChr->tld[0]='\xDC';
      break;
   case 278:
   case 1143:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x63';
      psDiaChr->dlr[0]='\x67';
      psDiaChr->ats[0]='\xEC';
      psDiaChr->sbo[0]='\xB5';
      psDiaChr->bsl[0]='\x71';
      psDiaChr->sbc[0]='\x9F';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x51';
      psDiaChr->cbo[0]='\x43';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\x47';
      psDiaChr->tld[0]='\xDC';
      break;
   case 280:
   case 1144:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\xB1';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\xB5';
      psDiaChr->sbo[0]='\x90';
      psDiaChr->bsl[0]='\x48';
      psDiaChr->sbc[0]='\x51';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\xDD';
      psDiaChr->cbo[0]='\x44';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\x54';
      psDiaChr->tld[0]='\x58';
      break;
   case 284:
   case 1145:
      psDiaChr->exc[0]='\xBB';
      psDiaChr->hsh[0]='\x69';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\x4A';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\x5A';
      psDiaChr->crt[0]='\xBA';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x4F';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xBD';
      break;
   case 285:
   case 1146:
      psDiaChr->exc[0]='\x5A';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x4A';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\xB1';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\xBB';
      psDiaChr->crt[0]='\xBA';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x4F';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xBC';
      break;
   case 297:
   case 1147:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\xB1';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x44';
      psDiaChr->sbo[0]='\x90';
      psDiaChr->bsl[0]='\x48';
      psDiaChr->sbc[0]='\xB5';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\xA0';
      psDiaChr->cbo[0]='\x51';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\x54';
      psDiaChr->tld[0]='\xBD';
      break;
   case 500:
   case 875:
   case 1148:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\x4A';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\x5A';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xA1';
      break;
   case 871:
   case 1149:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\xAC';
      psDiaChr->sbo[0]='\xAE';
      psDiaChr->bsl[0]='\xBE';
      psDiaChr->sbc[0]='\x9E';
      psDiaChr->crt[0]='\xEC';
      psDiaChr->grv[0]='\x8C';
      psDiaChr->cbo[0]='\x8E';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\x9C';
      psDiaChr->tld[0]='\xCC';
      break;
   case 870:
   case 1153:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\x4A';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\x5A';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x6A';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xA1';
      break;
   case 1025:
   case 1154:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\x4A';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\x5A';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x6A';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xA1';
      break;
   case 1112:
   case 1156:
      psDiaChr->exc[0]='\x5A';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\xBA';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\xBB';
      psDiaChr->crt[0]='\xB0';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x4F';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xA1';
      break;
   case 1157:
   case 1122:
      psDiaChr->exc[0]='\x4F';
      psDiaChr->hsh[0]='\x63';
      psDiaChr->dlr[0]='\x67';
      psDiaChr->ats[0]='\xEC';
      psDiaChr->sbo[0]='\xB5';
      psDiaChr->bsl[0]='\x71';
      psDiaChr->sbc[0]='\x9F';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x51';
      psDiaChr->cbo[0]='\x43';
      psDiaChr->vbr[0]='\xBB';
      psDiaChr->cbc[0]='\x47';
      psDiaChr->tld[0]='\xDC';
      break;
   case 924:
   case 1047:
      psDiaChr->exc[0]='\x5A';
      psDiaChr->hsh[0]='\x7B';
      psDiaChr->dlr[0]='\x5B';
      psDiaChr->ats[0]='\x7C';
      psDiaChr->sbo[0]='\xAD';
      psDiaChr->bsl[0]='\xE0';
      psDiaChr->sbc[0]='\xBD';
      psDiaChr->crt[0]='\x5F';
      psDiaChr->grv[0]='\x79';
      psDiaChr->cbo[0]='\xC0';
      psDiaChr->vbr[0]='\x4F';
      psDiaChr->cbc[0]='\xD0';
      psDiaChr->tld[0]='\xA1';
      break;
#endif
   default:
#ifdef __EBCDIC__
      if (psDiaChr==&gsDiaChr) {
#endif
         psDiaChr->exc[0]='!' ;/*nodiac*/
         psDiaChr->hsh[0]='#' ;/*nodiac*/
         psDiaChr->dlr[0]='$' ;/*nodiac*/
         psDiaChr->ats[0]='@' ;/*nodiac*/
         psDiaChr->sbo[0]='[' ;/*nodiac*/
         psDiaChr->bsl[0]='\\';/*nodiac*/
         psDiaChr->sbc[0]=']' ;/*nodiac*/
         psDiaChr->crt[0]='^' ;/*nodiac*/
         psDiaChr->grv[0]='`' ;/*nodiac*/
         psDiaChr->cbo[0]='{' ;/*nodiac*/
         psDiaChr->vbr[0]='|' ;/*nodiac*/
         psDiaChr->cbc[0]='}' ;/*nodiac*/
         psDiaChr->tld[0]='~' ;/*nodiac*/
#ifdef __EBCDIC__
      } else {
         psDiaChr->exc[0]=C_EXC;
         psDiaChr->hsh[0]=C_HSH;
         psDiaChr->dlr[0]=C_DLR;
         psDiaChr->ats[0]=C_ATS;
         psDiaChr->sbo[0]=C_SBO;
         psDiaChr->bsl[0]=C_BSL;
         psDiaChr->sbc[0]=C_SBC;
         psDiaChr->crt[0]=C_CRT;
         psDiaChr->grv[0]=C_GRV;
         psDiaChr->cbo[0]=C_CBO;
         psDiaChr->vbr[0]=C_VBR;
         psDiaChr->cbc[0]=C_CBC;
         psDiaChr->tld[0]=C_TLD;
      }
#endif
      break;
   }
   psDiaChr->svb[0]='=';
   psDiaChr->svb[1]=psDiaChr->vbr[0];
   psDiaChr->sbs[0]='/';
   psDiaChr->sbs[1]=psDiaChr->bsl[0];
   psDiaChr->idt[0]='-';
   psDiaChr->idt[1]='-';
   psDiaChr->idt[2]=psDiaChr->vbr[0];
}

#ifdef __EBCDIC__

TsDiaChr gsDiaChr={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
                   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
                   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
                   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

extern char init_char(const char* p) {
   unsigned int uiCcsId=localccsid();
   printd("Initialize gsDiaChr structure with CCSID %u\n",uiCcsId);
   init_diachr(&gsDiaChr,uiCcsId);
   return(p[0]);
}

extern const char* init_string(const char* p) {
   init_char(p);
   return(p);
}

#define RPLDIAC(str) {                            \
   for (char* p=(str); *p; p++) {                 \
      switch (*p) {                               \
      case '!' : *p=C_EXC; break;/*nodiac*/       \
      case '$' : *p=C_DLR; break;/*nodiac*/       \
      case '#' : *p=C_HSH; break;/*nodiac*/       \
      case '@' : *p=C_ATS; break;/*nodiac*/       \
      case '[' : *p=C_SBO; break;/*nodiac*/       \
      case '\\': *p=C_BSL; break;/*nodiac*/       \
      case ']' : *p=C_SBC; break;/*nodiac*/       \
      case '^' : *p=C_CRT; break;/*nodiac*/       \
      case '`' : *p=C_GRV; break;/*nodiac*/       \
      case '{' : *p=C_CBO; break;/*nodiac*/       \
      case '|' : *p=C_VBR; break;/*nodiac*/       \
      case '}' : *p=C_CBC; break;/*nodiac*/       \
      case '~' : *p=C_TLD; break;/*nodiac*/       \
      }                                           \
   }                                              \
}

extern int ebcdic_srprintc(char** buffer, size_t* size, const size_t expansion, const char* format, ...) {

   va_list  argv;
   int      r;
   size_t   h=(*buffer!=NULL)?strlen(*buffer):0;
   size_t   s=h+strlen(format)+expansion+1;
   if ((*size)<s || *buffer==NULL) {
      s=(*size>s)?*size:2*s;
      char* b=(char*)realloc_nowarn(*buffer,s);
      if (b==NULL) return(0);
      (*buffer)=b;
      (*size)=s;
   }
   va_start(argv, format);
   r = vsnprintf((*buffer)+h, (*size)-h, format, argv);
   va_end(argv);
   (*buffer)[(*size)-1]=0;
   RPLDIAC(*buffer);
   return(h+r);
}

extern int ebcdic_snprintf(char* string, size_t size, const char* format, ...) {
   va_list  argv;
   int      r;
   va_start(argv, format);
   r = vsnprintf(string, size, format, argv);
   va_end(argv);
   RPLDIAC(string);
   return(r);
}

extern int ebcdic_sprintf(char* string, const char* format, ...) {
   va_list  argv;
   int      r;
   va_start(argv, format);
   r = vsprintf(string, format, argv);
   va_end(argv);
   RPLDIAC(string);
   return(r);
}

extern int ebcdic_fprintf(FILE* file, const char* format, ...) {
   char     temp[65536];
   size_t   bufSiz = sizeof(temp);
   va_list  argv;
   int      result, rc = 0;
   char*    buf = NULL;

   va_start(argv, format);
   result = vsnprintf(temp, bufSiz, format, argv);
   va_end(argv);
   if (result < 0) {
      rc = result;
   } else if (result < bufSiz) {
      if (*temp) {
         RPLDIAC(temp);
         rc=fprintf(file, "%s", temp);
      }
   } else {
      while (result >= bufSiz) {
         bufSiz = result+1;
         char* hlp = (char*)realloc(buf, bufSiz);
         if (hlp == NULL) {
            SAFE_FREE(buf);
            errno = ENOMEM;
            rc = -1;
            break;
         }
         buf = hlp;
         va_start(argv, format);
         result = vsnprintf(buf, bufSiz, format, argv);
         va_end(argv);
         if (result < 0) {
            rc = result;
            break;
         }
      }
      if (rc == 0) {
         if (*buf) {
            RPLDIAC(buf);
            rc=fprintf(file, "%s", buf);
         }
      }
      SAFE_FREE(buf);
   }
   return rc;
}

#endif

#ifdef __WIN__
#undef _WIN32_IE
#define _WIN32_IE 0x5000
#include <windows.h>
#include <shlobj.h>
#include <versionhelpers.h>

extern char* userid(const int size, char* buffer) {
   DWORD tmp=size;
   buffer[0]=0x00;
   GetUserName(buffer,&tmp);
   return(buffer);
}

extern char* duserid(void) {
   char*    buffer=NULL;
   size_t   size=0;
   char user[1024]={0};
   DWORD tmp=sizeof(user);
   GetUserName(user,&tmp);
   srprintf(&buffer,&size,strlen(user),"%s",user);
   return(buffer);
}

extern char* homedir(int flag, const int size, char* buffer) {
   char path[MAX_PATH+1]={0};
   buffer[0]=0x00;
   if (SHGetFolderPath(NULL,CSIDL_PROFILE,NULL,0,path)==S_OK) {
      if (flag) {
         snprintf(buffer,size,"%s%c",path,C_BSL);
      } else {
         strlcpy(buffer,path,size);
      }
   }
   return(buffer);
}

extern char* dhomedir(int flag) {
   char*    buffer=NULL;
   size_t   size=0;
   char path[MAX_PATH+1]={0};
   if (SHGetFolderPath(NULL,CSIDL_PROFILE,NULL,0,path)==S_OK) {
      if (flag) {
         srprintf(&buffer,&size,strlen(path)+1,"%s%c",path,C_BSL);
      } else {
         srprintf(&buffer,&size,strlen(path),"%s",path);
      }
   }
   return(buffer);
}

extern int win_setenv(const char* name, const char* value){
   // TODO: don't use stack allocation because strings can be larger than stack size (user input)
   char envstr[strlen(name) + strlen(value) + 2];
   sprintf(envstr,"%s=%s",name,value);
   return  _putenv(envstr);
}

extern int win_unsetenv(const char* name){
   // TODO: don't use stack allocation because string can be larger than stack size (user input)
  char envstr[strlen(name) + 2];
  sprintf(envstr,"%s=",name);
  return _putenv(envstr);
}

extern char* windowsversionstring(char** vstr, size_t* size)
{
    size_t exp = 28;
    if (IsWindows10OrGreater()) {
        srprintf(vstr, size, exp, "Windows10 %s or greater", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindows8Point1OrGreater()) {
        srprintf(vstr, size, exp, "Windows8.1 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindows8OrGreater()) {
        srprintf(vstr, size, exp, "Windows8 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindows7SP1OrGreater()) {
        srprintf(vstr, size, exp, "Windows7SP1 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindows7OrGreater()) {
        srprintf(vstr, size, exp, "Windows7 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsVistaSP2OrGreater()) {
        srprintf(vstr, size, exp, "Windows VistaSP2 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsVistaSP1OrGreater()) {
        srprintf(vstr, size, exp, "Windows VistaSP1 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsVistaOrGreater()) {
        srprintf(vstr, size, exp, "Windows Vista %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsXPSP3OrGreater()) {
        srprintf(vstr, size, exp, "WindowsXPSP3 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsXPSP2OrGreater()) {
        srprintf(vstr, size, exp, "WindowsXPSP2 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsXPSP1OrGreater()) {
        srprintf(vstr, size, exp, "WindowsXPSP1 %s", IsWindowsServer() ? "Server": "Client");
    } else if (IsWindowsXPOrGreater()) {
        srprintf(vstr, size, exp, "WindowsXP %s", IsWindowsServer() ? "Server": "Client");
    } else {
        srprintf(vstr, size, exp, "Windows OLDER THAN XP");
    }
    return *vstr;
}

#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

extern char* userid(const int size, char* buffer) {
#if defined(__ZOS__) && defined(__FL5__)
   int      r,i;
   char     acGid[16];
   char     acUid[16];
   memset(acUid,0,sizeof(acUid));
   r=FLZGUID(acGid,acUid);
   if (r==0) {
      for (i=strlen(acUid);i>0 && acUid[i-1]<=0x40;i--);
      acUid[i]=0x00;
   } else i=0;
   if (i) {
      strlcpy(buffer,acUid,size);
      return(buffer);
   } else {
      if (size>=0) buffer[0]=0x00;
   }
#endif
   char           acBuffer[1024];
   struct passwd* uP=NULL;
   struct passwd  sP={0};
   int e=getpwuid_r(geteuid(),&sP,acBuffer,sizeof(acBuffer),&uP);
   if (0==e && NULL != uP) {
      strlcpy(buffer,uP->pw_name,size);
   } else {
      if (size>=0) buffer[0]=0x00;
   }
   return(buffer);
}
extern char* duserid(void) {
   char*          buffer= NULL;
   size_t         size  = 0;
#if defined(__ZOS__) && defined(__FL5__)
   int      r,i;
   char     acGid[16];
   char     acUid[16];
   memset(acUid,0,sizeof(acUid));
   r=FLZGUID(acGid,acUid);
   if (r==0) {
      for (i=strlen(acUid);i>0 && acUid[i-1]<=0x40;i--);
      acUid[i]=0x00;
   } else i=0;
   if (i) {
      srprintf(&buffer,&size,strlen(acUid),"%s",acUid);
      return(buffer);
   }
#endif
   char           acBuffer[1024];
   struct passwd* uP=NULL;
   struct passwd  sP={0};
   int e=getpwuid_r(geteuid(),&sP,acBuffer,sizeof(acBuffer),&uP);
   if (0==e && NULL != uP) {
      srprintf(&buffer,&size,strlen(uP->pw_name),"%s",uP->pw_name);
   } else {
      srprintf(&buffer,&size,0,"%s","");
   }
   return(buffer);
}
extern char* homedir(int flag, const int size, char* buffer) {
   const char*    home  = GETENV("HOME");
   if (home!=NULL && *home) {
      if (flag) {
         snprintf(buffer,size,"%s/",home);
      } else {
         strlcpy(buffer,home,size);
      }
   } else {
      char           acBuffer[1024];
      struct passwd* uP=NULL;
      struct passwd  sP={0};
      int r=getpwuid_r(geteuid(),&sP,acBuffer,sizeof(acBuffer),&uP);
      if (0==r && uP != NULL && uP->pw_dir != NULL) {
         if (flag) {
            snprintf(buffer,size,"%s/",uP->pw_dir);
         } else {
            strlcpy(buffer,uP->pw_dir,size);
         }
      } else {
         char user[64]={0};
         userid(sizeof(user),user);
         if (user[0]) {
#ifdef __ZOS__
         if (flag) {
            snprintf(buffer,size,"%s.",user);
         } else {
            strlcpy(buffer,user,size);
         }
#elif defined(__USS__)
         if (flag) {
            snprintf(buffer,size,"/u/%s/",user);
         } else {
            snprintf(buffer,size,"/u/%s",user);
         }
#else
         if (flag) {
            snprintf(buffer,size,"/home/%s/",user);
         } else {
            snprintf(buffer,size,"/home/%s",user);
         }
#endif
         } else {
            if (flag) {
               snprintf(buffer,size,"/");
            } else {
               if (size>=0) buffer[0]=0x00;
            }
         }
      }
   }
   return(buffer);
}
extern char* dhomedir(int flag) {
   char*          buffer= NULL;
   size_t         size  = 0;
   const char*    home  = GETENV("HOME");
   if (home!=NULL && *home) {
      if (flag) {
         srprintf(&buffer,&size,strlen(home),"%s/",home);
      } else {
         srprintf(&buffer,&size,strlen(home),"%s",home);
      }
   } else {
      char           acBuffer[1024];
      struct passwd* uP=NULL;
      struct passwd  sP={0};
      int r=getpwuid_r(geteuid(),&sP,acBuffer,sizeof(acBuffer),&uP);
      if (0==r && uP != NULL && uP->pw_dir != NULL) {
         if (flag) {
            srprintf(&buffer,&size,strlen(uP->pw_dir),"%s/",uP->pw_dir);
         } else {
            srprintf(&buffer,&size,strlen(uP->pw_dir),"%s",uP->pw_dir);
         }
      } else {
         char* user=duserid();
         if (user!=NULL) {
            if (user[0]) {
#ifdef __ZOS__
               if (flag) {
                  srprintf(&buffer,&size,strlen(user),"%s.",user);
               } else {
                  srprintf(&buffer,&size,strlen(user),"%s",user);
               }
#elif defined(__USS__)
               if (flag) {
                  srprintf(&buffer,&size,strlen(user),"/u/%s/",user);
               } else {
                  srprintf(&buffer,&size,strlen(user),"/u/%s",user);
               }
#else
               if (flag) {
                  srprintf(&buffer,&size,strlen(user),"/home/%s/",user);
               } else {
                  srprintf(&buffer,&size,strlen(user),"/home/%s",user);
               }
#endif
            } else {
               if (flag) {
                  srprintf(&buffer,&size,0,"/");
               } else {
                  srprintf(&buffer,&size,0,"%s","");
               }
            }
            free(user);
         } else {
            if (flag) {
               srprintf(&buffer,&size,0,"/");
            } else {
               srprintf(&buffer,&size,0,"%s","");
            }
         }
      }
   }
   return(buffer);
}
#endif

extern char* envid(const int size, char* buffer) {
   snprintf(buffer,size,"T");
   return(buffer);
}

extern char* safe_getenv(const char* name, char* buffer, size_t bufsiz) {
   if (buffer!=NULL) {
      const char* env = GETENV(name);
      if(env!=NULL) {
         snprintf(buffer, bufsiz, "%s", env);
      } else {
         return NULL;
      }
   }
   return buffer;
}

/**********************************************************************/

typedef void *(*memset_t)(void *,int,size_t);
static volatile memset_t memset_func=memset;
extern void secure_memset(void *ptr,size_t len)
{
    memset_func(ptr,0,len);
}

extern char* unEscape(const char* input, char* output)
{
   const char*                   i=input;
   char*                         o=output;
   TsDiaChr                      stDiaChr;
   init_diachr(&stDiaChr,mapcdstr(GETENV("CLP_STRING_CCSID")));
   while(i[0]) {
      if (i[0]=='&') {
         if (i[1]=='&') {
            if ((toupper(i[2])=='E' && toupper(i[3])=='X' && toupper(i[4])=='C' && i[5]==';') ||
                (toupper(i[2])=='H' && toupper(i[3])=='S' && toupper(i[4])=='H' && i[5]==';') ||
                (toupper(i[2])=='D' && toupper(i[3])=='L' && toupper(i[4])=='R' && i[5]==';') ||
                (toupper(i[2])=='A' && toupper(i[3])=='T' && toupper(i[4])=='S' && i[5]==';') ||
                (toupper(i[2])=='S' && toupper(i[3])=='B' && toupper(i[4])=='O' && i[5]==';') ||
                (toupper(i[2])=='B' && toupper(i[3])=='S' && toupper(i[4])=='L' && i[5]==';') ||
                (toupper(i[2])=='S' && toupper(i[3])=='B' && toupper(i[4])=='C' && i[5]==';') ||
                (toupper(i[2])=='C' && toupper(i[3])=='R' && toupper(i[4])=='T' && i[5]==';') ||
                (toupper(i[2])=='G' && toupper(i[3])=='R' && toupper(i[4])=='V' && i[5]==';') ||
                (toupper(i[2])=='C' && toupper(i[3])=='B' && toupper(i[4])=='O' && i[5]==';') ||
                (toupper(i[2])=='V' && toupper(i[3])=='B' && toupper(i[4])=='R' && i[5]==';') ||
                (toupper(i[2])=='C' && toupper(i[3])=='B' && toupper(i[4])=='C' && i[5]==';') ||
                (toupper(i[2])=='T' && toupper(i[3])=='L' && toupper(i[4])=='D' && i[5]==';') ||
                (toupper(i[2])=='X' && isxdigit(i[3])     && isxdigit(i[4])     && i[5]==';'))
            {
               o[0]='&';
               i+=2; o++;
            } else if (isdigit(i[2])) {
               const char* x=i;
               strtol(i+2,(char**)&x,10);
               if (x[0]==';') {
                  o[0]='&';
                  i+=2; o++;
               } else {
                  o[0]=i[0];
                  o[1]=i[1];
                  i+=2; o+=2;
               }
            } else {
               o[0]=i[0];
               o[1]=i[1];
               i+=2; o+=2;
            }
         } else if (toupper(i[1])=='E' && toupper(i[2])=='X' && toupper(i[3])=='C' && i[4]==';') {
            o[0]=C_EXC;
            i+=5; o++;
         } else if (toupper(i[1])=='H' && toupper(i[2])=='S' && toupper(i[3])=='H' && i[4]==';') {
            o[0]=C_HSH;
            i+=5; o++;
         } else if (toupper(i[1])=='D' && toupper(i[2])=='L' && toupper(i[3])=='R' && i[4]==';') {
            o[0]=C_DLR;
            i+=5; o++;
         } else if (toupper(i[1])=='A' && toupper(i[2])=='T' && toupper(i[3])=='S' && i[4]==';') {
            o[0]=C_ATS;
            i+=5; o++;
         } else if (toupper(i[1])=='S' && toupper(i[2])=='B' && toupper(i[3])=='O' && i[4]==';') {
            o[0]=C_SBO;
            i+=5; o++;
         } else if (toupper(i[1])=='B' && toupper(i[2])=='S' && toupper(i[3])=='L' && i[4]==';') {
            o[0]=C_BSL;
            i+=5; o++;
         } else if (toupper(i[1])=='S' && toupper(i[2])=='B' && toupper(i[3])=='C' && i[4]==';') {
            o[0]=C_SBC;
            i+=5; o++;
         } else if (toupper(i[1])=='C' && toupper(i[2])=='R' && toupper(i[3])=='T' && i[4]==';') {
            o[0]=C_CRT;
            i+=5; o++;
         } else if (toupper(i[1])=='G' && toupper(i[2])=='R' && toupper(i[3])=='V' && i[4]==';') {
            o[0]=C_GRV;
            i+=5; o++;
         } else if (toupper(i[1])=='C' && toupper(i[2])=='B' && toupper(i[3])=='O' && i[4]==';') {
            o[0]=C_CBO;
            i+=5; o++;
         } else if (toupper(i[1])=='V' && toupper(i[2])=='B' && toupper(i[3])=='R' && i[4]==';') {
            o[0]=C_VBR;
            i+=5; o++;
         } else if (toupper(i[1])=='C' && toupper(i[2])=='B' && toupper(i[3])=='C' && i[4]==';') {
            o[0]=C_CBC;
            i+=5; o++;
         } else if (toupper(i[1])=='T' && toupper(i[2])=='L' && toupper(i[3])=='D' && i[4]==';') {
            o[0]=C_TLD;
            i+=5; o++;
         } else if (toupper(i[1])=='X' && isxdigit(i[2]) && isxdigit(i[3]) && i[4]==';') {
            char h,l;
            if (toupper(i[2])>='A' && toupper(i[2])<='F') {
               h=(toupper(i[2])-'A')+10;
            } else {
               h=i[2]-'0';
            }
            if (toupper(i[3])>='A' && toupper(i[3])<='F') {
               l=(toupper(i[3])-'A')+10;
            } else {
               l=i[3]-'0';
            }
            o[0]=(h<<4)+l;
            i+=5; o++;
         } else if (isdigit(i[1])) {
            const char* x=i;
            init_diachr(&stDiaChr,(unsigned int)strtol(i+1,(char**)&x,10));
            if (x[0]==';') {
               i=x+1;
            } else {
               o[0]=i[0];
               i++; o++;
            }
         } else {
            o[0]=i[0];
            i++; o++;
         }
      } else if (i[0]==stDiaChr.exc[0]) {
         o[0]=C_EXC;
         i++; o++;
      } else if (i[0]==stDiaChr.hsh[0]) {
         o[0]=C_HSH;
         i++; o++;
      } else if (i[0]==stDiaChr.dlr[0]) {
         o[0]=C_DLR;
         i++; o++;
      } else if (i[0]==stDiaChr.ats[0]) {
         o[0]=C_ATS;
         i++; o++;
      } else if (i[0]==stDiaChr.sbo[0]) {
         o[0]=C_SBO;
         i++; o++;
      } else if (i[0]==stDiaChr.bsl[0]) {
         o[0]=C_BSL;
         i++; o++;
      } else if (i[0]==stDiaChr.sbc[0]) {
         o[0]=C_SBC;
         i++; o++;
      } else if (i[0]==stDiaChr.crt[0]) {
         o[0]=C_CRT;
         i++; o++;
      } else if (i[0]==stDiaChr.grv[0]) {
         o[0]=C_GRV;
         i++; o++;
      } else if (i[0]==stDiaChr.cbo[0]) {
         o[0]=C_CBO;
         i++; o++;
      } else if (i[0]==stDiaChr.vbr[0]) {
         o[0]=C_VBR;
         i++; o++;
      } else if (i[0]==stDiaChr.cbc[0]) {
         o[0]=C_CBC;
         i++; o++;
      } else if (i[0]==stDiaChr.tld[0]) {
         o[0]=C_TLD;
         i++; o++;
      } else {
         o[0]=i[0];
         i++; o++;
      }
   }
   o[0]=EOS;
   return(output);
}

extern char* dynUnEscape(const char* input) {
   if (input!=NULL) {
      char* output=malloc(strlen(input)+1);
      if (output==NULL) return(NULL);
      return(unEscape(input,output));
   } else return NULL;
}

extern void fprintm(FILE* file,const char* own, const char* pgm, const char* bld, const char* man, const int cnt) {
   char*       hlp;
   char*       ptr;
   char        tmp[strlen(man)+4];
   switch (cnt) {
   case  0:strlcpy(tmp,man,sizeof(tmp));     break;
   case  1:snprintf(tmp,sizeof(tmp),"%s\n",man);   break;
   default:snprintf(tmp,sizeof(tmp),"%s\n\n",man);
   }
   ptr=tmp;
   hlp=strstr(ptr,"&{"); /*nodiac*/;
   while (hlp!=NULL) {
      *hlp='\0';
      efprintf(file,"%s",ptr);
      if (strncmp(hlp+2,"OWN}",4)==0) { /*nodiac*/
         if (own!=NULL) {
            for (const char* p=own;*p;p++) fprintf(file,"%c",toupper(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"own}",4)==0) { /*nodiac*/
         if (own!=NULL) {
            for (const char* p=own;*p;p++) fprintf(file,"%c",tolower(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"Own}",4)==0) { /*nodiac*/
         if (own!=NULL) {
            fprintf(file,"%c",toupper(*own));
            for (const char* p=own+1;*p;p++) fprintf(file,"%c",tolower(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"oWn}",4)==0) { /*nodiac*/
         if (own!=NULL) {
            fprintf(file,"%s",own);
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"PGM}",4)==0) { /*nodiac*/
         if (pgm!=NULL) {
            for (const char* p=pgm;*p;p++) fprintf(file,"%c",toupper(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"pgm}",4)==0) { /*nodiac*/
         if (pgm!=NULL) {
            for (const char* p=pgm;*p;p++) fprintf(file,"%c",tolower(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"Pgm}",4)==0) { /*nodiac*/
         if (pgm!=NULL) {
            fprintf(file,"%c",toupper(*pgm));
            for (const char* p=pgm+1;*p;p++) fprintf(file,"%c",tolower(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"pGm}",4)==0) { /*nodiac*/
         if (pgm!=NULL) {
            fprintf(file,"%s",pgm);
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"BLD}",4)==0) { /*nodiac*/
         if (bld!=NULL) {
            for (const char* p=bld;*p;p++) fprintf(file,"%c",toupper(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"bld}",4)==0) { /*nodiac*/
         if (bld!=NULL) {
            for (const char* p=bld;*p;p++) fprintf(file,"%c",tolower(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"Bld}",4)==0) { /*nodiac*/
         if (bld!=NULL) {
            fprintf(file,"%c",toupper(*bld));
            for (const char* p=bld+1;*p;p++) fprintf(file,"%c",tolower(*p));
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"bLd}",4)==0) { /*nodiac*/
         if (bld!=NULL) {
            fprintf(file,"%s",bld);
         }
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"DATE}",5)==0) { /*nodiac*/
         char              acBuf[20];
         time_t            h=time(NULL);
         struct tm         st;
         const struct tm*  x=localtime_r(&h,&st);
         strftime(acBuf,sizeof(acBuf),"%Y-%m-%d",x);
         fprintf(file,"%s",acBuf);
         ptr=hlp+7;
      } else if (strncmp(hlp+2,"TIME}",5)==0) { /*nodiac*/
         char              acBuf[20];
         time_t            h=time(NULL);
         struct tm         st;
         const struct tm*  x=localtime_r(&h,&st);
         strftime(acBuf,sizeof(acBuf),"%H:%M:%S",x);
         fprintf(file,"%s",acBuf);
         ptr=hlp+7;
      } else if (strncmp(hlp+2,"STATE}",6)==0) { /*nodiac*/
#ifdef __DEBUG__
         fprintf(file,"DEBUG");
#else
         fprintf(file,"RELEASE");
#endif
         ptr=hlp+8;
      } else if (strncmp(hlp+2,"state}",6)==0) { /*nodiac*/
#ifdef __DEBUG__
         fprintf(file,"debug");
#else
         fprintf(file,"release");
#endif
         ptr=hlp+8;
      } else if (strncmp(hlp+2,"State}",6)==0) { /*nodiac*/
#ifdef __DEBUG__
         fprintf(file,"Debug");
#else
         fprintf(file,"Release");
#endif
         ptr=hlp+8;
      } else {
         char* nxt=strchr(hlp,'}'); /*nodiac*/
         if (nxt!=NULL) {
            ptr=nxt+1;
         } else {
            while (*ptr) ptr++;
         }
      }
      hlp=strstr(ptr,"&{"); /*nodiac*/
   }
   efprintf(file,"%s",ptr);
}

extern int snprintm(char* buffer, size_t size, const char* own, const char* pgm, const char* bld, const char* man, const int cnt) {
   FILE*       f=fopen_tmp();
   if (f!=NULL) {
      fprintm(f,own,pgm,bld,man,cnt);
      size_t s=(size_t)ftell(f);
      rewind(f);
      char* p=malloc(s+1);
      if (p!=NULL) {
         size_t r=fread(p,1,s,f);
         fclose_tmp(f);
         if (r==s) {
            p[r]=0x00;
            int i=snprintf(buffer,size,"%s",p);
            free(p);
            return(i);
         } else {
            free(p);
         }
      } else {
         fclose_tmp(f);
      }
   }
   return(snprintf(buffer,size,"Convert of manual page failed"));
}

extern unsigned int localccsid(void) {
   unsigned int ccsid = 0;
   const char* charset;
#ifdef __UNIX__

   // From man page:
   // On startup of the main program, the portable "C" locale is selected
   // as default.
   // TODO: avoid using setlocale()/localeconv() anywhere in the project (except in main()) as they are not thread-safe
   const char* oldLocale = setlocale(LC_ALL, NULL);
   setlocale(LC_ALL, "");
   charset = nl_langinfo(CODESET);
   setlocale(LC_ALL, oldLocale);
   ccsid = mapcdstr(charset);

#elif defined(__WIN__)
   static CPINFOEX info;

   if (GetCPInfoEx(CP_ACP, 0, &info) && mapccsid(info.CodePage) != NULL) {
      // We got a supported CCSID => return it
      return info.CodePage;
   }
#endif

   // cppcheck-suppress knownConditionTrueFalse
   if (ccsid == 0) {
      // fallback to LANG variable
      // cppcheck-suppress knownConditionTrueFalse
      charset = mapl2c(' ' == 0x40);
      ccsid = mapcdstr(charset);

      if (ccsid == 0) {
         // fallback to platform default (aka. we don't know any better)
         // cppcheck-suppress knownConditionTrueFalse
         if (' ' == 0x40)
            return CLEP_DEFAULT_CCSID_EBCDIC; // IBM-1047 (EBCDIC (Open Systems Latin-1))
         else
            return CLEP_DEFAULT_CCSID_ASCII; // ISO-8859-1 (ASCII (Latin-1))
      }
   }

   return ccsid;
}

extern const char* mapl2c(unsigned isEBCDIC) {
   const char* pcPtr=NULL;
   const char* pcEnv=GETENV("LANG");
   if (pcEnv!=NULL && *pcEnv) {
      // cppcheck-suppress knownConditionTrueFalse
      if ((isEBCDIC && '0'==0xF0) || (!isEBCDIC && '0'==0x30)) {
         pcPtr=strchr(pcEnv,'.');
         if (pcPtr!=NULL) {
            size_t      i;
            static char acHlp[32];
            pcPtr++;
            for (i=0;i<(sizeof(acHlp)-1) && (isalnum(pcPtr[i]) || pcPtr[i]=='-' || pcPtr[i]=='_');i++) {
               acHlp[i]=pcPtr[i];
            }
            acHlp[i]=0x00;
            if (i) {
               return(acHlp);
            }
         }
      }
      return(lng2ccsd(pcEnv, isEBCDIC));
   }
   return(NULL);
}

extern const char* lng2ccsd(const char* pcLang, unsigned isEbcdic) {
   char     pcLngCpy[2];
   char*    pcPtr =NULL;
   int      isEuro=FALSE;
   size_t   uiLen=strlen(pcLang);

   if (uiLen<1) return NULL;
   pcLngCpy[0] = tolower(pcLang[0]);

   if (uiLen==1) {
      if(pcLngCpy[0]=='c') return(isEbcdic?"IBM-1047":"US-ASCII");
      else                 return NULL;
   }
   pcLngCpy[1] = tolower(pcLang[1]);

   pcPtr=strpbrk(pcLang, ATS_PBRK);
   if (pcPtr!=NULL) {
      if (((pcPtr-pcLang)+4 < (int)uiLen) && tolower(pcPtr[1])=='e' && tolower(pcPtr[2])=='u' && tolower(pcPtr[3])=='r' && tolower(pcPtr[4])=='o') {
         isEuro=TRUE;
      }
   }

   if(pcLngCpy[0]=='a') {
      if(pcLngCpy[1]=='a') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='f') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='n') return (isEbcdic?NULL:"ISO8859-15");
      if(pcLngCpy[1]=='r') return (isEbcdic?"IBM-420":"ISO8859-6");
   } else if(pcLngCpy[0]=='b') {
      if(pcLngCpy[1]=='e') return (isEbcdic?NULL:"CP1251");
      if(pcLngCpy[1]=='g') return (isEbcdic?NULL:"CP1251");
      if(pcLngCpy[1]=='r') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='s') return (isEbcdic?"IBM-1153":"ISO8859-2");
   } else if(pcLngCpy[0]=='c') {
      if(pcLngCpy[1]=='a') return (isEbcdic?NULL:"ISO8859-15");
      if(pcLngCpy[1]=='s') return (isEbcdic?"IBM-1153":"ISO8859-2");
      if(pcLngCpy[1]=='y') return (isEbcdic?NULL:"ISO8859-14");
   } else if(pcLngCpy[0]=='d') {
      if(pcLngCpy[1]=='a') return (isEbcdic?"IBM-1142":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='e') return (isEbcdic?"IBM-1141":isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='e') {
      if(pcLngCpy[1]=='l') return (isEbcdic?"IBM-875":"ISO8859-7");
      if(pcLngCpy[1]=='n') return (isEbcdic?"IBM-1140":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='o') return (isEbcdic?NULL:"ISO8859-3");
      if(pcLngCpy[1]=='s') return (isEbcdic?"IBM-1145":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='t') return (isEbcdic?"IBM-1122":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='u') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='f') {
      if(pcLngCpy[1]=='i') return (isEbcdic?"IBM-1143":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='o') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='r') return (isEbcdic?"IBM-1147":isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='g') {
      if(pcLngCpy[1]=='a') return (isEbcdic?"IBM-1146":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='d') return (isEbcdic?"IBM-1146":"ISO8859-15");
      if(pcLngCpy[1]=='l') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='v') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='h') {
      if(pcLngCpy[1]=='e') return (isEbcdic?"IBM-424":"ISO8859-8");
      if(pcLngCpy[1]=='r') return (isEbcdic?"IBM-1153":"ISO8859-2");
      if(pcLngCpy[1]=='u') return (isEbcdic?"IBM-1153":"ISO8859-2");
   } else if(pcLngCpy[0]=='i') {
      if(pcLngCpy[1]=='d') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='s') return (isEbcdic?"IBM-1149":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='t') return (isEbcdic?"IBM-1144":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='w') return (isEbcdic?NULL:"ISO8859-8");
   } else if(pcLngCpy[0]=='k') {
      if(pcLngCpy[1]=='l') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='u') return (isEbcdic?NULL:"ISO8859-9");
      if(pcLngCpy[1]=='w') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='l') {
      if(pcLngCpy[1]=='g') return (isEbcdic?NULL:"ISO8859-10");
      if(pcLngCpy[1]=='t') return (isEbcdic?"IBM-1156":"ISO8859-13");
      if(pcLngCpy[1]=='v') return (isEbcdic?"IBM-1156":"ISO8859-13");
   } else if(pcLngCpy[0]=='m') {
      if(pcLngCpy[1]=='g') return (isEbcdic?NULL:"ISO8859-15");
      if(pcLngCpy[1]=='i') return (isEbcdic?NULL:"ISO8859-13");
      if(pcLngCpy[1]=='k') return (isEbcdic?NULL:"ISO8859-5");
      if(pcLngCpy[1]=='s') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='t') return (isEbcdic?NULL:"ISO8859-3");
   } else if(pcLngCpy[0]=='n') {
      if(pcLngCpy[1]=='b') return (isEbcdic?"IBM-1142":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='l') return (isEbcdic?"IBM-1140":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='n') return (isEbcdic?"IBM-1142":isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='o') {
      if(pcLngCpy[1]=='c') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='m') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='p') {
      if(pcLngCpy[1]=='l') return (isEbcdic?"IBM-1153":"ISO8859-2");
      if(pcLngCpy[1]=='t') return (isEbcdic?"IBM-1140":isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='r') {
      if(pcLngCpy[1]=='o') return (isEbcdic?"IBM-1153":"ISO8859-2");
      if(pcLngCpy[1]=='u') return (isEbcdic?"IBM-1154":"ISO8859-5");
   } else if(pcLngCpy[0]=='s') {
      if(pcLngCpy[1]=='k') return (isEbcdic?"IBM-1153":"ISO8859-2");
      if(pcLngCpy[1]=='l') return (isEbcdic?"IBM-1153":"ISO8859-2");
      if(pcLngCpy[1]=='o') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='q') return (isEbcdic?"IBM-1153":isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='r') return (isEbcdic?"IBM-1153":"UTF-8");
      if(pcLngCpy[1]=='t') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='v') return (isEbcdic?"IBM-1143":isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='t') {
      if(pcLngCpy[1]=='g') return (isEbcdic?NULL:"KOI8-T");
      if(pcLngCpy[1]=='h') return (isEbcdic?NULL:"TIS-620");
      if(pcLngCpy[1]=='l') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
      if(pcLngCpy[1]=='r') return (isEbcdic?NULL:"ISO8859-9");
   } else if(pcLngCpy[0]=='u') {
      if(pcLngCpy[1]=='k') return (isEbcdic?NULL:"KOI8-U");
      if(pcLngCpy[1]=='z') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='w') {
      if(pcLngCpy[1]=='a') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='x') {
      if(pcLngCpy[1]=='h') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   } else if(pcLngCpy[0]=='y') {
      if(pcLngCpy[1]=='i') return (isEbcdic?NULL:"CP1255");
   } else if(pcLngCpy[0]=='z') {
      if(pcLngCpy[1]=='u') return (isEbcdic?NULL:isEuro?"ISO8859-15":"ISO8859-1");
   }
   return NULL;
}

extern unsigned int mapcdstr(const char* p) {
   if (p!=NULL) {
      int o;
      while (1) {
         if (*p==0x00) {
            return(0);
         } else if (isspace(*p)) {
            p++;
         } else if (toupper(p[0])=='U' && toupper(p[1])=='T' && toupper(p[2])=='F') { /*UTF-xxxx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if ((p[0]=='8' || p[0]=='1') && (p[1]==0x00 || isspace(p[1]))) {
               return(1208);
            } else if (p[0]=='0' && (p[1]=='8' || p[1]=='1') && (p[2]==0x00 || isspace(p[2]))) {
               return(1208);
            } else if (p[0]=='1' && p[1]=='6') {
               o=0;  p+=2;
            } else if (p[0]=='3' && p[1]=='2') {
               o=32; p+=2;
            } else if (p[0]=='0' && p[1]=='2') {
               o=0; p+=2;
            } else if (p[0]=='0' && p[1]=='4') {
               o=32; p+=2;
            } else if (p[0]=='2') {
               o=0;  p+=1;
            } else if (p[0]=='4') {
               o=32; p+=1;
            } else return(0);
            if ((p[0]==0x00 || isspace(p[0]))) {
               o+=4;
            } else if (toupper(p[0])=='L' && toupper(p[1])=='E' && (p[2]==0x00 || isspace(p[2]))) {
               o+=2;
            } else if (toupper(p[0])=='B' && toupper(p[1])=='E' && (p[2]==0x00 || isspace(p[2]))) {
               o+=0;
            } else return(0);
            return(1200+o);
         } else if (toupper(p[0])=='U' && toupper(p[1])=='C' && toupper(p[2])=='S') { /*UCS-xxxx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if ((p[0]=='8' || p[0]=='1') && (p[1]==0x00 || isspace(p[1]))) {
               return(13496);
            } else if (p[0]=='0' && (p[1]=='8' || p[1]=='1') && (p[2]==0x00 || isspace(p[2]))) {
               return(13496);
            } else if (p[0]=='1' && p[1]=='6') {
               o=0;  p+=2;
            } else if (p[0]=='3' && p[1]=='2') {
               o=32; p+=2;
            } else if (p[0]=='0' && p[1]=='2') {
               o=0; p+=2;
            } else if (p[0]=='0' && p[1]=='4') {
               o=32; p+=2;
            } else if (p[0]=='2') {
               o=0;  p+=1;
            } else if (p[0]=='4') {
               o=32; p+=1;
            } else return(0);
            if ((p[0]==0x00 || isspace(p[0]))) {
               o+=4;
            } else if (toupper(p[0])=='L' && toupper(p[1])=='E' && (p[2]==0x00 || isspace(p[2]))) {
               o+=2;
            } else if (toupper(p[0])=='B' && toupper(p[1])=='E' && (p[2]==0x00 || isspace(p[2]))) {
               o+=0;
            } else return(0);
            return(13488+o);
         } else if (toupper(p[0])=='I' && toupper(p[1])=='S' && toupper(p[2])=='O') { /*ISO*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='8' && p[1]=='8' && p[2]=='5'  && p[3]=='9') { /*ISO-8859-xx*/
               p+=4; if (p[0]=='-' || p[0]=='_') p++;
               for (o=0;isdigit(p[0]);p++) {
                  o=(o*10)+(p[0]-'0');
               }
               if (p[0]==0x00 || isspace(p[0])) {
                  switch (o) {
                  case  1: return( 819);
                  case  2: return( 912);
                  case  3: return( 913);
                  case  4: return( 914);
                  case  5: return( 915);
                  case  6: return(1089);
                  case  7: return( 813);
                  case  8: return( 916);
                  case  9: return( 920);
                  case 10: return( 919);
                  case 11: return( 874);
                  case 12: return(   0);// not used yet
                  case 13: return( 921);
                  case 14: return(   0);// no ccsid defined (Latin-8)
                  case 15: return( 923);
                  case 16: return(   0);// no ccsid defined (Latin-10)
                  default: return(   0);
                  }
               } else if (o==8) {
                  p+=1; if (p[0]=='-') p++;
                  if (toupper(p[0])=='I' && (p[1]==0x00 || isspace(p[1]))) {
                     return(62238);
                  } else return(0);
               } else return(0);
            } else if (p[0]=='2' && p[1]=='0' && p[2]=='2'  && p[3]=='2') { /*ISO-2022-xx*/
               p+=4; if (p[0]=='-' || p[0]=='_') p++;
               if (toupper(p[0])=='J' && toupper(p[1])=='P' && (p[2]==0x00 || isspace(p[2]))) {
                  return(5052);
               } else return(0);
            } else return(0);
         } else if (toupper(p[0])=='L' && toupper(p[1])=='A' && toupper(p[2])=='T' && toupper(p[3])=='I' && toupper(p[4])=='N') { /*LATIN*/
            p+=5; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='1' && (p[1]==0x00 || isspace(p[1]))) {
               return(819);
            } else if (p[0]=='2' && (p[1]==0x00 || isspace(p[1]))) {
                  return(912);
            } else if (p[0]=='3' && (p[1]==0x00 || isspace(p[1]))) {
                  return(913);
            } else if (p[0]=='4' && (p[1]==0x00 || isspace(p[1]))) {
                  return(914);
            } else if (p[0]=='5' && (p[1]==0x00 || isspace(p[1]))) {
                  return(915);
            } else if (p[0]=='6' && (p[1]==0x00 || isspace(p[1]))) {
                  return(919);
            } else if (p[0]=='7' && (p[1]==0x00 || isspace(p[1]))) {
                  return(921);
            } else if (p[0]=='8' && (p[1]==0x00 || isspace(p[1]))) {
                  return(0);// no CCSID defined
            } else if (p[0]=='9' && (p[1]==0x00 || isspace(p[1]))) {
                  return(923);
            } else if (p[0]=='1' && p[1]=='0' && (p[2]==0x00 || isspace(p[2]))) {
                  return(0);// no CCSID defined
            } else return(0);
         } else if (toupper(p[0])=='C' && toupper(p[1])=='P') { /*CP-125x*/
            p+=2; if (p[0]=='-') p++;
            if (p[0]=='1' && p[1]=='2' && p[2]=='5' && isdigit(p[3]) && (p[4]==0x00 || isspace(p[4]))) {
               return(1250+(p[3]-'0'));
            } else return(0);
         } else if (toupper(p[0])=='I' && toupper(p[1])=='B'  && toupper(p[2])=='M') { /*IBM-xxxx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            for (o=0;isdigit(p[0]);p++) {
               o=(o*10)+(p[0]-'0');
            }
            if ((p[0]==0x00 || isspace(p[0])) && o && o<65536) return (o); else return(0);
         } else if (toupper(p[0])=='U' && toupper(p[1])=='S') { /*US-ASCII*/
            p+=2; if (p[0]=='-' || p[0]=='_') p++;
            if (toupper(p[0])=='A' && toupper(p[1])=='S' && toupper(p[2])=='C'  && toupper(p[3])=='I'  && toupper(p[4])=='I' && (p[5]==0x00 || isspace(p[5]))) {
               return(367);
            } else return(0);
         } else if (toupper(p[0])=='D' && toupper(p[1])=='E') { /*DE-ASCII*/
            p+=2; if (p[0]=='-' || p[0]=='_') p++;
            if (toupper(p[0])=='A' && toupper(p[1])=='S' && toupper(p[2])=='C'  && toupper(p[3])=='I'  && toupper(p[4])=='I' && (p[5]==0x00 || isspace(p[5]))) {
               return(66003);
            } else return(0);
         } else if (toupper(p[0])=='A' && toupper(p[1])=='S' && toupper(p[2])=='C'  && toupper(p[3])=='I'  && toupper(p[4])=='I' && (p[5]==0x00 || isspace(p[5]))) {
            return(367);
         } else if (toupper(p[0])=='K' && toupper(p[1])=='O' && toupper(p[2])=='I'  && p[3]=='8') { /*KOI8-xx*/
            p+=4; if (p[0]=='-' || p[0]=='_') p++;
            if (toupper(p[0])=='R' && (p[1]==0x00 || isspace(p[1]))) {
               return(878);
            } else if (toupper(p[0])=='R' && toupper(p[1])=='U' && (p[2]==0x00 || isspace(p[2]))) {
               return(1167);
            } else if (toupper(p[0])=='U' && (p[1]==0x00 || isspace(p[1]))) {
               return(1168);
            } else return(0);
         } else if (toupper(p[0])=='E' && toupper(p[1])=='U' && toupper(p[2])=='C') { /*EUC-xx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if (toupper(p[0])=='J' && toupper(p[1])=='P' && (p[2]==0x00 || isspace(p[2]))) {
               return(954);
            } else if (toupper(p[0])=='T' && toupper(p[1])=='W' && (p[2]==0x00 || isspace(p[2]))) {
               return(964);
            } else if (toupper(p[0])=='K' && toupper(p[1])=='R' && (p[2]==0x00 || isspace(p[2]))) {
               return(949);
            } else if (toupper(p[0])=='C' && toupper(p[1])=='N' && (p[2]==0x00 || isspace(p[2]))) {
               return(1383);
            } else return(0);
         } else if (toupper(p[0])=='B' && toupper(p[1])=='I' && toupper(p[2])=='G') { /*BIG-5*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='5' && (p[1]==0x00 || isspace(p[1]))) {
               return(950);
            } else return(0);
         } else if (toupper(p[0])=='G' && toupper(p[1])=='B') { /*GB*/
            if (toupper(p[2])=='K' && (p[3]==0x00 || isspace(p[3]))) {
               return(1386);
            }
            p+=2; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='1' && p[1]=='8' && p[2]=='0' && p[3]=='3' && p[4]=='0' && (p[5]==0x00 || isspace(p[5]))) {
               return(5488);
            } else if (p[0]=='2' && p[1]=='3' && p[2]=='1' && p[3]=='2' && (p[4]==0x00 || isspace(p[4]))) {
               return(1381);
            } else return(0);
         } else if (toupper(p[0])=='S' && toupper(p[1])=='H' && toupper(p[2])=='I' && toupper(p[3])=='F' && toupper(p[4])=='T') { /*SHIFT*/
            p+=5; if (p[0]=='-' || p[0]=='_') p++;
            if (toupper(p[0])=='J' && toupper(p[1])=='I' && toupper(p[2])=='S' && (p[3]==0x00 || isspace(p[3]))) {
               return(932);
            } else return(0);
         } else if (toupper(p[0])=='W' && toupper(p[1])=='I' && toupper(p[2])=='N' && toupper(p[3])=='D' && toupper(p[4])=='O' && toupper(p[5])=='W' && toupper(p[6])=='S') { /*SHIFT*/
            p+=7; if (p[0]=='-' || p[0]=='_') p++;
            if (toupper(p[0])=='3' && toupper(p[1])=='1' && toupper(p[2])=='J' && (p[3]==0x00 || isspace(p[3]))) {
               return(943);
            } else return(0);
         } else if (toupper(p[0])=='K' && toupper(p[1])=='S' && toupper(p[2])=='C') { /*KSC-xx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='5' && p[1]=='6' && p[2]=='0' && p[3]=='1' && (p[4]==0x00 || isspace(p[4]))) {
               return(1363);
            } else return(0);
         } else if (toupper(p[0])=='T' && toupper(p[1])=='I' && toupper(p[2])=='S') { /*TIS-xx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='6' && p[1]=='2' && p[2]=='0' && (p[3]==0x00 || isspace(p[3]))) {
               return(1162);
            } else return(0);
         } else if (toupper(p[0])=='V' && toupper(p[1])=='I' && toupper(p[2])=='S' && toupper(p[3])=='C' && toupper(p[4])=='I' && toupper(p[5])=='I') { /*VISCII*/
            p+=6;
            if (p[0]==0x00 || isspace(p[0])) {
               return(1129);
            } else return(0);
         } else if (toupper(p[0])=='M' && toupper(p[1])=='A' && toupper(p[2])=='C' && toupper(p[3])=='I' && toupper(p[4])=='N' && toupper(p[5])=='T' && toupper(p[6])=='O'  && toupper(p[7])=='S' && toupper(p[8])=='H') { /*MACINTOSH*/
            p+=9;
            if (p[0]==0x00 || isspace(p[0])) {
               return(1275);
            } else return(0);
         } else if (toupper(p[0])=='D' && toupper(p[1])=='I'  && toupper(p[2])=='N') { /*DIN-xxxx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            for (o=0;isdigit(p[0]);p++) {
               o=(o*10)+(p[0]-'0');
            }
            if ((p[0]==0x00 || isspace(p[0])) && o == 66003) return (o); else return(0);
         } else if (isdigit(p[0])) { /*CCSID*/
            for (o=(p[0]-'0'),p++;isdigit(p[0]);p++) {
               o=(o*10)+(p[0]-'0');
            }
            if ((p[0]==0x00 || isspace(p[0])) && o<65536) {
               return(o);
            } else return(0);
         } else return(0);
      }
   } else return(0);
}

/* TODO: add to mapping above
   case  1051: return("CSHPROMAN8");
   case  1212: return("SCSU");
   case  1213: return("SCSU");
   case  1214: return("BOCU-1");
   case  1215: return("BOCU-1");
*/

/* Function to map ccsid in encoding string */
extern const char* mapccsid(const unsigned int uiCcsId){
   switch (uiCcsId) {
   case    37: return("IBM-037");
   case   273: return("IBM-273");
   case   274: return("IBM-274");
   case   275: return("IBM-275");
   case   277: return("IBM-277");
   case   278: return("IBM-278");
   case   280: return("IBM-280");
   case   281: return("IBM-281");
   case   282: return("IBM-282");
   case   284: return("IBM-284");
   case   285: return("IBM-285");
   case   290: return("IBM-290");
   case   297: return("IBM-297");
   case   300: return("IBM-300");
   case   301: return("IBM-301");
   case   367: return("US-ASCII");
   case   420: return("IBM-420");
   case   424: return("IBM-424");
   case   425: return("IBM-425");
   case   437: return("IBM-437");
   case   500: return("IBM-500");
   case   646: return("US-ASCII");
   case   808: return("ISO8859-5");
   case   813: return("ISO8859-7");
   case   819: return("ISO8859-1");
   case   833: return("IBM-833");
   case   834: return("IBM-834");
   case   835: return("IBM-835");
   case   836: return("IBM-836");
   case   837: return("IBM-837");
   case   838: return("IBM-838");
   case   848: return("IBM-848");
   case   850: return("IBM-850");
   case   852: return("IBM-852");
   case   855: return("IBM-855");
   case   856: return("IBM-856");
   case   858: return("IBM-858");
   case   859: return("IBM-859");
   case   861: return("IBM-861");
   case   862: return("IBM-862");
   case   864: return("IBM-864");
   case   866: return("IBM-866");
   case   867: return("IBM-867");
   case   869: return("IBM-869");
   case   870: return("IBM-870");
   case   871: return("IBM-871");
   case   872: return("IBM-872");
   case   874: return("ISO8859-11");
   case   875: return("IBM-875");
   case   878: return("KOI8-R");
   case   880: return("IBM-880");
   case   901: return("IBM-901");
   case   902: return("IBM-902");
   case   904: return("IBM-904");
   case   912: return("ISO8859-2");
   case   913: return("ISO8859-3");
   case   914: return("ISO8859-4");
   case   915: return("ISO8859-5");
   case   916: return("ISO8859-8");
   case   919: return("ISO8859-10");
   case   920: return("ISO8859-9");
   case   921: return("ISO8859-13");
   case   922: return("IBM-922");
   case   923: return("ISO8859-15");
   case   924: return("IBM-924");
   case   927: return("IBM-927");
   case   928: return("IBM-928");
   case   930: return("IBM-930");
//   case   932: return("SHIFT-JIS"); //MULTIBYTE
   case   933: return("IBM-933");
   case   935: return("IBM-935");
   case   936: return("IBM-936");
   case   937: return("IBM-937");
   case   938: return("IBM-938");
   case   939: return("IBM-939");
   case   942: return("IBM-942");
//   case   943: return("WINDOWS-31J");//MULTIBYTE
   case   946: return("IBM-946");
   case   947: return("IBM-947");
   case   948: return("IBM-948");
//   case   949: return("EUC-KR");//MULTIBYTE
//   case   950: return("BIG5");//MULTIBYTE
   case   951: return("IBM-951");
//   case   954: return("EUC-JP");//MULTIBYTE
   case   956: return("IBM-956");
   case   957: return("IBM-957");
   case   958: return("IBM-958");
   case   959: return("IBM-959");
//   case   964: return("EUC-TW");//MULTIBYTE
//   case   970: return("EUC-KR");//MULTIBYTE
   case  1025: return("IBM-1025");
   case  1026: return("IBM-1026");
   case  1027: return("IBM-1027");
   case  1043: return("IBM-1043");
   case  1046: return("IBM-1046");
   case  1047: return("IBM-1047");
   case  1088: return("IBM-1088");
   case  1089: return("ISO8859-6");
   case  1112: return("IBM-1112");
   case  1115: return("IBM-1115");
   case  1122: return("IBM-1122");
   case  1123: return("IBM-1123");
   case  1124: return("IBM-1124");
   case  1125: return("IBM-1125");
   case  1126: return("IBM-1126");
   case  1129: return("VISCII");
   case  1140: return("IBM-1140");
   case  1141: return("IBM-1141");
   case  1142: return("IBM-1142");
   case  1143: return("IBM-1143");
   case  1144: return("IBM-1144");
   case  1145: return("IBM-1145");
   case  1146: return("IBM-1146");
   case  1147: return("IBM-1147");
   case  1148: return("IBM-1148");
   case  1149: return("IBM-1149");
   case  1153: return("IBM-1153");
   case  1154: return("IBM-1154");
   case  1155: return("IBM-1155");
   case  1156: return("IBM-1156");
   case  1157: return("IBM-1157");
   case  1158: return("IBM-1158");
   case  1159: return("IBM-1159");
   case  1160: return("IBM-1160");
   case  1161: return("IBM-1161");
   case  1162: return("TIS-620");
   case  1163: return("VISCII");
   case  1165: return("IBM-1165");
   case  1167: return("KOI8-RU");
   case  1168: return("KOI8-U");
   case  1200: return("UTF-16BE");
   case  1201: return("UTF-16BE");
   case  1202: return("UTF-16LE");
   case  1203: return("UTF-16LE");
   case  1204: return("UTF-16");
   case  1205: return("UTF-16");
   case  1208: return("UTF-8");
   case  1209: return("UTF-8");
   case  1232: return("UTF-32BE");
   case  1233: return("UTF-32BE");
   case  1234: return("UTF-32LE");
   case  1235: return("UTF-32LE");
   case  1236: return("UTF-32");
   case  1237: return("UTF-32");
   case  1250: return("CP1250");
   case  1251: return("CP1251");
   case  1252: return("CP1252");
   case  1253: return("CP1253");
   case  1254: return("CP1254");
   case  1255: return("CP1255");
   case  1256: return("CP1256");
   case  1257: return("CP1257");
   case  1258: return("CP1258");
   case  1275: return("MACINTOSH");
   case  1362: return("IBM-1362");
   case  1363: return("KSC-5601");
   case  1364: return("IBM-1364");
   case  1370: return("IBM-1370");
   case  1371: return("IBM-1371");
   case  1380: return("IBM-1380");
//   case  1381: return("GB2312");//MULTIBYTE
//   case  1383: return("EUC-CN");//MULTIBYTE
//   case  1386: return("GBK");//MULTIBYTE
   case  1388: return("IBM-1388");
   case  1390: return("IBM-1390");
//   case  1392: return("GB18030");//MULTIBYTE
   case  1399: return("IBM-1399");
   case  4396: return("IBM-4396");
   case  4909: return("ISO8859-7");
   case  4930: return("IBM-4930");
   case  4933: return("IBM-4933");
   case  4946: return("IBM-4946");
   case  4971: return("IBM-4971");
   case  5026: return("IBM-5026");
   case  5031: return("IBM-5031");
   case  5035: return("IBM-5035");
//   case  5039: return("SHIFT-JIS");//MULTIBYTE
   case  5052: return("ISO-2022-JP");
   case  5053: return("IBM-5053");
   case  5054: return("IBM-5054");
   case  5055: return("IBM-5055");
   case  5123: return("IBM-5123");
   case  5346: return("CP1250");
   case  5347: return("CP1251");
   case  5348: return("CP1252");
   case  5349: return("CP1253");
   case  5350: return("CP1254");
   case  5351: return("CP1255");
   case  5352: return("CP1256");
   case  5353: return("CP1257");
   case  5354: return("CP1258");
   case  5488: return("GB18030");
   case  8482: return("IBM-8482");
   case  8612: return("IBM-420");
   case  8616: return("IBM-424");
   case  9005: return("ISO8859-7");
   case  9027: return("IBM-9027");
   case  9044: return("IBM-9044");
   case  9061: return("IBM-9061");
   case  9238: return("IBM-9238");
   case 12712: return("IBM-12712");
   case 13121: return("IBM-13121");
   case 13124: return("IBM-13124");
   case 13488: return("UCS-2BE");
   case 13489: return("UCS-2BE");
   case 13490: return("UCS-2LE");
   case 13491: return("UCS-2LE");
   case 13492: return("UCS-2");
   case 13496: return("UCS-1");
   case 13497: return("UCS-1");
   case 13520: return("UCS-4BE");
   case 13521: return("UCS-4BE");
   case 13522: return("UCS-4LE");
   case 13523: return("UCS-4LE");
   case 13524: return("UCS-4");
   case 16684: return("IBM-16684");
   case 16804: return("IBM-16804");
   case 17248: return("IBM-17248");
   case 17584: return("UTF-16BE");
   case 17585: return("UTF-16BE");
   case 17586: return("UTF-16LE");
   case 17587: return("UTF-16LE");
   case 17592: return("UTF-8");
   case 17593: return("UTF-8");
   case 17616: return("UTF-32BE");
   case 17617: return("UTF-32BE");
   case 17618: return("UTF-32LE");
   case 17619: return("UTF-32LE");
   case 28709: return("IBM-28709");
//   case 33722: return("EUC-JP");//MULTIBYTE
   case 53668: return("IBM-53668");
   case 54191: return("IBM-54191");
   case 62209: return("IBM-862");
   case 62210: return("ISO8859-8");
   case 62211: return("IBM-424");
   case 62213: return("IBM-862");
   case 62215: return("ISO8859-8");
   case 62218: return("IBM-864");
   case 62221: return("IBM-862");
   case 62222: return("ISO8859-8");
   case 62223: return("CP1255");
   case 62224: return("IBM-420");
   case 62225: return("IBM-864");
   case 62227: return("ISO8859-6");
   case 62228: return("CP1256");
   case 62229: return("IBM-424");
   case 62231: return("IBM-862");
   case 62232: return("ISO8859-8");
   case 62233: return("IBM-420");
   case 62234: return("IBM-420");
   case 62235: return("IBM-424");
   case 62237: return("CP1255");
   case 62238: return("ISO8859-8-I");
   case 62239: return("CP1255");
   case 62240: return("IBM-424");
   case 62242: return("IBM-862");
   case 62243: return("ISO8859-8-I");
   case 62244: return("CP1255");
   case 62245: return("IBM-424");
   case 62250: return("IBM-420");
   case 62383: return("IBM-62383");
   case 66003: return("DIN-66003");
   default   : return(NULL);
   }
}

/**********************************************************************/


static char* drplchar(const char* string, const size_t limit, const char c, const char* value)
{
   char*       buf;
   char*       out;
   const char* end;
   size_t      size;
   size_t      valueLen=strlen(value);

   if (valueLen==0)
      return NULL;

   size=strlen(string)+1;
   buf=malloc(size);
   out=buf;
   // cppcheck-suppress knownConditionTrueFalse
   if (buf==NULL) return(NULL);

   end = limit > 0 ? string + limit : string + size;
   while (string[0]) {
      if (string[0]==c && string < end) {
         if (string[1]==c) { // escape sequence
            out[0]=string[0];
            out++;
            string+=2;
         } else { // replacement
            ptrdiff_t offset=out-buf;
            char* h=realloc_nowarn(buf,size+(valueLen-1));
            if (h==NULL) {
               free(buf);
               return NULL;
            }
            size+=valueLen-1;
            buf=h;
            out=buf+offset;
            memcpy(out,value,valueLen);
            out+=valueLen;
            string++;
         }
      } else { // copy
         out[0]=string[0];
         out++;
         string++;
      }
   }
   out[0]=0x00;
   return buf;
}

static void rplchar(char* string, const size_t size, const size_t limit, const char c, const char* value)
{
   char        h[size];
   char*       in;
   char*       out;
   char*       inEnd;
   char*       match;
   int         inHelper = 0;
   size_t      vlen=strlen(value);

   // check whether we even have a match
   match = strchr(string, c);
   if (match != NULL) {
      in = match;
      out = match;
      inEnd = limit > 0 ? string + limit : string + size;

      while (*in != '\0' && out + 1 < string + size) {
         if (*in == c && in < inEnd) {
            // character match
            if (in[1] == c) {
               // handle doubled character (escaping) => remove one character
               *out = *in;
               out++;
               in += 2;
            } else {
               if (!inHelper) {
                  // output is expanding for the first time => since we do in-place replacement,
                  // save rest of input to temp buffer, keeping the relative offsets,
                  // and change 'in' pointer from 'name' parameter to temp buffer
                  size_t offset = in - string;
                  strlcpy(h + offset, in, size - offset);
                  inEnd = limit > 0 ? h + limit : h + size;
                  in = h + offset;
                  inHelper = 1;
               }
               // write replacement string
               size_t remaining = size - (out - string) - 1;
               if (remaining < vlen) {
                  memcpy(out, value, remaining);
                  out += remaining;
               } else {
                  memcpy(out, value, vlen);
                  out += vlen;
               }
               in++;
            }
         } else {
            // no match => copy character
            *out++ = *in++;
         }
      }
      *out = '\0';
   }
}

static const char* getjclvar(const char* symbol, int size, char* value)
{
   int        err=0;
   int        siSln;
   int        siVln=size-1;
   char       acCpy[strlen(symbol)+2];
   if(symbol[0]!='&'){
      siSln=snprintf(acCpy,sizeof(acCpy),"&%s",symbol);
      symbol=acCpy;
   } else {
      siSln=strlen(symbol);
   }
   err=flzjsy(symbol,&siSln,value,&siVln);
   if (err==0) {
      if (siVln!=siSln || memcmp(symbol,value,siVln)) {                  // check if real replacement
         while (siVln>0 && isspace(value[siVln-1])) siVln--;            // remove trailing whitespace
         if (siVln) {
            value[siVln]=0x00;
            return(value);
         }
      }
   }
   return(NULL);
}

static const char* getsysvar(const char* symbol, int size, char* value)
{
   int        err=0;
   int        siSln;
   int        siVln=size-1;
   char       acCpy[strlen(symbol)+2];
   if(symbol[0]!='&'){
      siSln=snprintf(acCpy,sizeof(acCpy),"&%s",symbol);
      symbol=acCpy;
   } else {
      siSln=strlen(symbol);
   }
   err=flzsym(symbol,&siSln,value,&siVln);
   if (err==0) {
      if (siVln!=siSln || memcmp(symbol,value,siVln)) {                  // check if real replacement
         while (siVln>0 && isspace(value[siVln-1])) siVln--;            // remove trailing whitespace
         if (siVln) {
            value[siVln]=0x00;
            return(value);
         }
      }
   }
   return(NULL);
}

extern char* getenvar(const char* name,const size_t length,const size_t size,char* string)
{
   char  acNam[length+1];
   if (length) {
      memcpy(acNam,name,length);
      acNam[length]=0x00;
      name=acNam;
   }
   char acVal[256];
   memset(acVal,0,sizeof(acVal));
   const char* v=GETENV(name);
   if (v==NULL) v=getjclvar(name,sizeof(acVal)-1,acVal);
   if (v==NULL) v=getsysvar(name,sizeof(acVal)-1,acVal);
   if (v!=NULL && *v) {
      size_t lv=strlen(v);
      while(lv>0 && isspace(v[lv-1])) {
         lv--;
      }
      lv = (lv < size) ? lv : size-1;
      memcpy(string, v, lv);
      string[lv] = 0;
      return(string);
   } else if (strcmp(name,"HOME")==0) {
      return(homedir(FALSE,size,string));
   } else if (strcmp(name,"USER")==0) {
      return(userid(size,string));
   } else if (strcmp(name,"SYSUID")==0) {
      return(userid(size,string));
   } else if (strcmp(name,"CUSER")==0) {
      userid(size,string);
      for(char* p=string;*p;p++) *p = toupper(*p);
      return(string);
   } else if (strcmp(name,"cuser")==0) {
      userid(size,string);
      for(char* p=string;*p;p++) *p = tolower(*p);
      return(string);
   } else if (strcmp(name,"Cuser")==0) {
      userid(size,string);
      if (*string) {
         *string = toupper(*string);
         for(char* p=string+1;*p;p++) *p = tolower(*p);
      }
      return(string);
   } else if (strcmp(name,"OWNERID")==0) {
      return(userid(size,string));
   } else if (strcmp(name,"ENVID")==0) {
      return(envid(size,string));
   } else if (strcmp(name,"HOSTNAME")==0) {
      #ifdef __WIN__
      WORD wVersionRequested;
      WSADATA wsaData;
      wVersionRequested = MAKEWORD( 2, 0 );

      if (WSAStartup(wVersionRequested, &wsaData)==0) {
      #endif
         if (gethostname(string, size)==0)
            string[size-1]='\0';
         else
            string[0]='\0'; //gethostname() failed => empty string
      #ifdef __WIN__
         WSACleanup( );
      }
      #endif
      return(string);
   } else {
      return(NULL);
   }
}

static void rplenvar(char* name,const size_t size,const char opn, const char cls)
{
   char        h[size];
   char        x[size];
   char*       a=name;
   char*       b;
   char*       c;
   char*       v;
   char*       p;
   int         match;
   ssize_t     catlen, hlen, vlen;

   for (b=strchr(a,opn); b!=NULL ;b=strchr(a,opn)) {
      if (b[1]==opn) {
         for (p=b;*p;p++) p[0]=p[1];
         p[0]=0x00;
         a=b+1;
      } else {
         c=strchr(b,cls);
         if (c!=NULL) {
            b[0]=c[0]=0; match=1;
            hlen=strlcpy(h,c+1,size);
            v=getenvar(b+1,0,size,x);
            if (v==NULL) {
               b[0]=opn; c[0]=cls; a=c+1;
               match = 0;
            }
            if (match) {
               vlen = strlen(v);
               catlen = (ssize_t)size - (ssize_t)strlen(name) - 1;
               if (catlen > 0) strncat(a, v, catlen); else catlen = 0;
               if (vlen >= catlen) name[size-1] = 0;
               catlen = (ssize_t)size - (ssize_t)strlen(name) - 1;
               if (catlen > 0) strncat(a, h, catlen); else catlen = 0;
               if (hlen >= catlen) name[size-1] = 0;
               a=b+vlen;
            }
         } else {
            a=b+1;
         }
      }
   }
}

static char* drplenvar(const char* string,const char opn, const char cls)
{
   const char* p=string;
   size_t      s=strlen(string)+1;
   char*       b=malloc(s);
   char*       r=b;
   // cppcheck-suppress knownConditionTrueFalse
   if (b==NULL)
      return(NULL);

   while(p[0]) {
      if (p[0]==opn) {
         if (p[1]==opn) {
            r[0]=p[0];
            r++; p+=2;
         } else {
            char* c=strchr(p+1,cls);
            if (c!=NULL) {
               char  e[1024];
               int   x=c-(p+1);
               const char* v=getenvar(p+1,x,sizeof(e),e);
               if (v!=NULL) {
                  int   l=strlen(v);
                  ptrdiff_t offset=r-b;
                  char* h=realloc_nowarn(b,s+(l-x));
                  if (h==NULL) {
                     free(b);
                     return(NULL);
                  }
                  s+=l-x;
                  b=h;
                  r=b+offset;
                  memcpy(r,v,l);
                  r+=l; p=c+1;
               } else {
                  r[0]=p[0];
                  r++; p++;
               }
            } else {
               r[0]=p[0];
               r++; p++;
            }
         }
      } else {
         r[0]=p[0];
         r++; p++;
      }
   }
   r[0]=0x00;
   return(b);
}

// replace against key template
static char* rpltpl(char* string,int size,const char* templ,const char* values) {
   char*       s;
   char*       e;
   const char* t;
   for (s=string,e=string+(size-1),t=templ;t[0] && s<e;t++) {
      if (t[0]=='%' && t[1]=='%') {
         s[0]='%'; s++; t++;
      } else if ((t[0]=='%' && t[1])) {
         for (const char* v=values;v[0];v++) {
            if (toupper(v[0])==toupper(t[1]) && v[1]==':') {
               for (v+=2;v[0] && v[0]!='\n' && s<e;v++) {
                  s[0]=v[0]; s++;
               }
               break;
            }
         }
         t++;
      } else {
         s[0]=t[0]; s++;
      }
   }
   if (s<e) s[0]=0x00; else e[0]=0x00;
   return(string);
}

// dynamic version of template replacement
static char* drpltpl(const char* templ,const char* values) {
   const char* p=templ;
   size_t      s=strlen(templ)+1;
   char*       b=malloc(s);
   char*       r=b;
   // cppcheck-suppress knownConditionTrueFalse
   if (b==NULL)
      return(NULL);

   while(p[0]) {
      if (p[0]=='%') {
         if (p[1]=='%') {
            r[0]=p[0];
            r++; p+=2;
         } else if (p[1]) {
            for (const char* v=values;v[0];v++) {
               if (toupper(v[0])==toupper(p[1]) && v[1]==':') {
                  const char* x=v+2;
                  while (x[0] && x[0]!='\n')
                     x++;
                  int l=x-(v+2);
                  ptrdiff_t offset=r-b;
                  char* h=realloc_nowarn(b,s+(l-2));
                  if (h==NULL) {
                     free(b);
                     return(NULL);
                  }
                  s+=l-2;
                  b=h;
                  r=b+offset;
                  memcpy(r,v+2,l);
                  r+=l;
                  break;
               }
            }
            p+=2;
         } else {
            r[0]=p[0];
            r++; p++;
         }
      } else {
         r[0]=p[0];
         r++; p++;
      }
   }
   r[0]=0x00;
   return(b);
}

// Adjust prefix
static const char* adjpfx(char* file, int size)
{
    char *p1,*p2;
# ifdef __ZOS__
    if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='S' && file[4]==':') {
       for (p1=file,p2=file+5 ; *p2 ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1 = 0;
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='D' && file[4]==':') {
       for (p1=file,p2=file+2 ; *p2 ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1 = 0;
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') {
       for (p1=file,p2=file+3 ; *p2 && *p2!='\'' ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1 = 0;
    } else if (file[0]=='/' && file[1]=='/') {
       file[0] = C_TLD;
       if (ISPATHNAME(file+2)) {
          file[1] = '/';
          return "<HOME>";
       } else {
          file[1] = '.';
          return "<SYSUID>";
       }
    }
    if (ISPATHNAME(file)) {
       return "<HOME>";
    } else {
       return "<SYSUID>";
    }
# elif defined(__USS__)
    if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='S' && file[4]==':') {
       file[2] = '\'';
       for (p1=file+3,p2=file+5 ; *p2 ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1++ = '\'';
       *p1 = 0;
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='D' && file[4]==':') {
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') {
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/') {
       return "<SYSUID>";
    }
    return "<HOME>";
# else
    char h[size];
    if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='S' && file[4]==':') {
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='D' && file[4]==':') {
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') { // insert DS:
       strcpy(h,file+3);
       file[2] = 'D'; file[3] = 'S'; file[4] = ':';
       for (p1=file+5,p2=h ; *p2 && *p2!='\'' ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1 = 0;
       return "<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/') { // insert DS:~.
       strcpy(h,file+2);
       file[2] = 'D'; file[3] = 'S'; file[4] = ':'; file[5]=C_TLD; file[6]='.';
       for (p1=file+7,p2=h ; *p2 && *p2!='\'' ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1 = 0;
       return "<SYSUID>";
    }
    return "<HOME>";
# endif
}

// dynamic version of adjuct prefix
static char* dadjpfx(const char* file,char** tilde)
{
    char* b=malloc(strlen(file)+8);
    if (b==NULL) return(NULL);
# ifdef __ZOS__
    if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='S' && file[4]==':') {
       strcpy(b,file+5);
       *tilde=ISPATHNAME(b)?"<HOME>":"<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='D' && file[4]==':') {
       strcpy(b,file+2);
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') {
       strcpy(b,file+3);
       *tilde=ISPATHNAME(b)?"<HOME>":"<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/') {
       b[0] = C_TLD;
       if (ISPATHNAME(file+2)) {
          b[1] = '/';
          *tilde="<HOME>";
       } else {
          b[1] = '.';
          *tilde="<SYSUID>";
       }
       strcpy(b+2,file+2);
    } else {
       strcpy(b,file);
       *tilde=ISPATHNAME(b)?"<HOME>":"<SYSUID>";
    }
# elif defined(__USS__)
    if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='S' && file[4]==':') {
       b[0]='/';
       b[1]='/';
       b[2]='\'';
       int l=strlen(file+5);
       memcpy(b+3,file+5,l);
       b[l+3]='\'';
       b[l+4]=0x00;
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='D' && file[4]==':') {
       strcpy(b,file);
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') {
       strcpy(b,file);
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/') {
       strcpy(b,file);
       *tilde="<SYSUID>";
    } else {
       strcpy(b,file);
       *tilde="<HOME>";
    }
# else
    if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='S' && file[4]==':') {
       strcpy(b,file);
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='D' && file[3]=='D' && file[4]==':') {
       strcpy(b,file);
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') { // insert DS:
       b[0]='/';
       b[1]='/';
       b[2]='D';
       b[3]='S';
       b[4]=':';
       strcpy(b+5,file+3);
       int l=strlen(b);
       if (l>0 && b[l-1]=='\'') b[l-1]=0x00;
       *tilde="<SYSUID>";
    } else if (file[0]=='/' && file[1]=='/') { // insert DS:~.
       b[0]='/';
       b[1]='/';
       b[2]='D';
       b[3]='S';
       b[4]=':';
       b[5]=C_TLD;
       b[6]='.';
       strcpy(b+7,file+2);
       *tilde="<SYSUID>";
    } else {
       strcpy(b,file);
       *tilde="<HOME>";
    }
# endif
    return(b);
}

extern char* mapstr(char* string,int size)
{
   unEscape(string,string);
   rplenvar(string,size,'<','>');
   return(string);
}

extern char* dmapstr(const char* string,int method)
{
   char* h0=dynUnEscape(string);
   if (h0!=NULL) {
      char* h1=drplenvar(h0,'<','>');
      free(h0);
      if (h1!=NULL){
         switch (method) {
         case 1: for(char* p=h1;*p;p++) *p=toupper(*p); break;
         case 2: for(char* p=h1;*p;p++) *p=tolower(*p); break;
         }
      }
      return(h1);
   }
   return(NULL);
}

extern char* dmapxml(const char* string,int method)
{
   char* h0=dynUnEscape(string);
   if (h0!=NULL) {
      char* h1=drplenvar(h0,'(',')');
      free(h0);
      if (h1!=NULL){
         switch (method) {
         case 1: for(char* p=h1;*p;p++) *p=toupper(*p); break;
         case 2: for(char* p=h1;*p;p++) *p=tolower(*p); break;
         }
      }
      return(h1);
   }
   return(NULL);
}

extern char* mapfil(char* file,int size)
{
   unEscape(file,file);
   rplchar(file,size,1,C_TLD,adjpfx(file,size));
   rplenvar(file,size,'<','>');
   return(file);
}

extern char* mapfil2(char* file,int size)
{
   unEscape(file,file);
   rplchar(file,size,1,C_TLD,adjpfx(file,size));
   rplenvar(file,size,'<','>');
#ifdef __WIN__
   size_t l=strlen(file);
   for (size_t i=0;i<l;i++) {
      if (file[i]=='/') file[i]=C_BSL;
   }
#endif
   return(file);
}

extern char* dmapfil(const char* file, int method)
{
   char* pfx="";
   char* h0=dynUnEscape(file);
   if (h0!=NULL) {
      char* h1=dadjpfx(h0,&pfx);
      free(h0);
      if (h1!=NULL) {
         char* h2=drplchar(h1,1,C_TLD,pfx);
         free(h1);
         if (h2!=NULL) {
            char* h3=drplenvar(h2,'<','>');
            free(h2);
            if (h3!=NULL){
               switch (method) {
               case 1: for(char* p=h3;*p;p++) *p=toupper(*p); break;
               case 2: for(char* p=h3;*p;p++) *p=tolower(*p); break;
               }
            }
            return(h3);
         }
      }
   }
   return(NULL);
}

#ifdef __ZOS__

extern char* dcpmapfil(const char* file) {
   if (ISPATHNAME(file)) {
      return dmapfil(file,0);
   } else {
      return dmapfil(file,1);
   }
}

#else

extern char* dcpmapfil(const char* file) {
   return dmapfil(file,0);
}

#endif

extern char* cpmapfil(char* dest, int size,const char* source) {
   strlcpy(dest,source,size);
   return mapfil(dest,size);
}

extern char* maplab(char* label,int size, int toUpper) {
   unEscape(label,label);
   rplchar(label,size,0,C_EXC,"<ENVID>");
   rplchar(label,size,0,C_TLD,"<SYSUID>");
   rplchar(label,size,0,C_CRT,"<OWNERID>");
   rplenvar(label,size,'<','>');
   if(toUpper){
      for(int i=0,l=strlen(label);i<l;i++){
         label[i] = toupper(label[i]);
      }
   }
   return(label);
}

extern char* dmaplab(const char* label, int method)
{
   char* h0=dynUnEscape(label);
   if (h0!=NULL) {
      char* h1=drplchar(h0,0,C_EXC,"<ENVID>");
      free(h0);
      if (h1!=NULL) {
         char* h2=drplchar(h1,0,C_TLD,"<SYSUID>");
         free(h1);
         if (h2!=NULL) {
            char* h3=drplchar(h2,0,C_CRT,"<OWNERID>");
            free(h2);
            if (h3!=NULL) {
               char* h4=drplenvar(h3,'<','>');
               free(h3);
               if (h4!=NULL){
                  switch (method) {
                  case 1: for(char* p=h4;*p;p++) *p=toupper(*p); break;
                  case 2: for(char* p=h4;*p;p++) *p=tolower(*p); break;
                  }
               }
               return(h4);
            }
         }
      }
   }
   return(NULL);
}

extern char* cpmaplab(char* label, int size,const char* templ, const char* values, int toUpper) {
   char* t0=dynUnEscape(templ);
   char* v0=dynUnEscape(values);
   if (t0!=NULL) {
      if (v0!=NULL) {
         rpltpl(label,size,t0,v0);
         maplab(label,size,toUpper);
         free(t0); free(v0);
         return(label);
      }
      free(t0);
   }
   return(NULL);
}

extern char* dcpmaplab(const char* templ, const char* values, int method) {
   char* t0=dynUnEscape(templ);
   char* v0=dynUnEscape(values);
   if (t0!=NULL) {
      if (v0!=NULL) {
         char* h1=drpltpl(t0,v0);
         free(t0); free(v0);
         if (h1!=NULL) {
            char* h2=dmaplab(h1,method);
            free(h1);
            return(h2);
         }
      } else {
         free(t0);
      }
   }
   return(NULL);
}

/* implementation of the external functions ***********************************/

extern const char* prsdstr(const char** hdl, const char* str, int len)
{
   const char* end=(len<0)?NULL:str+len;
   const char* hlp;
   if (*hdl==NULL) {
      (*hdl)=hlp=str;
   } else {
      hlp=(*hdl);
   }
   if (hlp==NULL || (end!=NULL && hlp>=end) || ((unsigned char*)hlp)[0]==0xFFU) return(NULL);
   (*hdl)+=strlen(hlp)+1;
   return(hlp);
}

extern size_t strlcpy(char *dest, const char *src, size_t n)
{
   size_t len = strlen(src);
   if (len>n-1)
      len=n-1;
   memmove(dest, src, len);
   dest[len]='\0';
   return len;
}

extern int printd(const char* format,...)
{
#if defined(__DEBUG__) && !defined(__METAL__)
   int r;
   va_list  argv;
   va_start(argv, format);
   r=vfprintf (stderr, format, argv );
   va_end(argv);
   fflush(stderr);
   return r;
#else
   (void)format;
   return 0;
#endif
}

extern int snprintc(char* buffer,size_t size,const char* format,...)
{
   unsigned int h = strlen(buffer);
   if (size >= (h+1)) {
      int      r;
      va_list  argv;
      va_start(argv, format);
      r = vsnprintf(buffer+h, size-h, format, argv);
      va_end(argv);
      return(r<0?r:h+r);
   } else {
      return (0);
   }
}

extern int srprintc(char** buffer,size_t* size,const size_t expansion,const char* format,...)
{
   va_list  argv;
   int      r;
   size_t   h=(*buffer!=NULL)?strlen(*buffer):0;
   size_t   s=h+strlen(format)+expansion+1;
   if ((*size)<s || *buffer==NULL) {
      s=(*size>s)?*size:2*s;
      char* b=(char*)realloc_nowarn(*buffer,s);
      if (b==NULL)
         return(0);
      (*buffer)=b;
      (*size)=s;
   }
   va_start(argv, format);
   r = vsnprintf((*buffer)+h, (*size)-h, format, argv);
   va_end(argv);
   return(r<0?r:h+r);
}

extern int srprintf(char** buffer,size_t* size,const size_t expansion,const char* format,...)
{
   va_list  argv;
   int      r;
   size_t   s=strlen(format)+expansion+1;
   if ((*size)<s || *buffer==NULL) {
      s=(*size>s)?*size:2*s;
      char* b=(char*)realloc_nowarn(*buffer,s);
      if (b==NULL) return(0);
      (*buffer)=b;
      (*size)=s;
   }
   va_start(argv, format);
   r = vsnprintf((*buffer), (*size), format, argv);
   va_end(argv);
   return(r);
}

extern unsigned int bin2hex(
   const unsigned char* bin,
         char*          hex,
   const unsigned int   len)
{
   unsigned int         i;
   const unsigned char  m[16]={'0','1','2','3',
                               '4','5','6','7',
                               '8','9','A','B',
                               'C','D','E','F'};
   for(i=0;i<len;i++)
   {
      hex[i*2+0]=m[((bin[i])>>4)&0x0F];
      hex[i*2+1]=m[((bin[i])>>0)&0x0F];
   }
   return(2*i);
}

extern unsigned int hex2bin(
   const char*          hex,
         unsigned char* bin,
   const unsigned int   len)
{
   unsigned int         i,j;
   unsigned char        h1,h2;

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

extern unsigned int chr2asc(
   const char*          chr,
         char*          asc,
   const unsigned int   len)
{
   unsigned int         i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
         case '\n' : asc[i]=0x0A; break;
         case '\r' : asc[i]=0x0D; break;

         case ' '  : asc[i]=0x20; break;
         case '\"' : asc[i]=0x22; break;
         case '%'  : asc[i]=0x25; break;
         case '&'  : asc[i]=0x26; break;
         case '\'' : asc[i]=0x27; break;
         case '('  : asc[i]=0x28; break;
         case ')'  : asc[i]=0x29; break;
         case '*'  : asc[i]=0x2A; break;
         case '+'  : asc[i]=0x2B; break;
         case ','  : asc[i]=0x2C; break;
         case '-'  : asc[i]=0x2D; break;
         case '.'  : asc[i]=0x2E; break;
         case '/'  : asc[i]=0x2F; break;

         case '0'  : asc[i]=0x30; break;
         case '1'  : asc[i]=0x31; break;
         case '2'  : asc[i]=0x32; break;
         case '3'  : asc[i]=0x33; break;
         case '4'  : asc[i]=0x34; break;
         case '5'  : asc[i]=0x35; break;
         case '6'  : asc[i]=0x36; break;
         case '7'  : asc[i]=0x37; break;
         case '8'  : asc[i]=0x38; break;
         case '9'  : asc[i]=0x39; break;
         case ':'  : asc[i]=0x3A; break;
         case ';'  : asc[i]=0x3B; break;
         case '<'  : asc[i]=0x3C; break;
         case '='  : asc[i]=0x3D; break;
         case '>'  : asc[i]=0x3E; break;
         case '?'  : asc[i]=0x3F; break;

         case 'A'  : asc[i]=0x41; break;
         case 'B'  : asc[i]=0x42; break;
         case 'C'  : asc[i]=0x43; break;
         case 'D'  : asc[i]=0x44; break;
         case 'E'  : asc[i]=0x45; break;
         case 'F'  : asc[i]=0x46; break;
         case 'G'  : asc[i]=0x47; break;
         case 'H'  : asc[i]=0x48; break;
         case 'I'  : asc[i]=0x49; break;
         case 'J'  : asc[i]=0x4A; break;
         case 'K'  : asc[i]=0x4B; break;
         case 'L'  : asc[i]=0x4C; break;
         case 'M'  : asc[i]=0x4D; break;
         case 'N'  : asc[i]=0x4E; break;
         case 'O'  : asc[i]=0x4F; break;

         case 'P'  : asc[i]=0x50; break;
         case 'Q'  : asc[i]=0x51; break;
         case 'R'  : asc[i]=0x52; break;
         case 'S'  : asc[i]=0x53; break;
         case 'T'  : asc[i]=0x54; break;
         case 'U'  : asc[i]=0x55; break;
         case 'V'  : asc[i]=0x56; break;
         case 'W'  : asc[i]=0x57; break;
         case 'X'  : asc[i]=0x58; break;
         case 'Y'  : asc[i]=0x59; break;
         case 'Z'  : asc[i]=0x5A; break;
         case '_'  : asc[i]=0x5F; break;

         case 'a'  : asc[i]=0x61; break;
         case 'b'  : asc[i]=0x62; break;
         case 'c'  : asc[i]=0x63; break;
         case 'd'  : asc[i]=0x64; break;
         case 'e'  : asc[i]=0x65; break;
         case 'f'  : asc[i]=0x66; break;
         case 'g'  : asc[i]=0x67; break;
         case 'h'  : asc[i]=0x68; break;
         case 'i'  : asc[i]=0x69; break;
         case 'j'  : asc[i]=0x6A; break;
         case 'k'  : asc[i]=0x6B; break;
         case 'l'  : asc[i]=0x6C; break;
         case 'm'  : asc[i]=0x6D; break;
         case 'n'  : asc[i]=0x6E; break;
         case 'o'  : asc[i]=0x6F; break;

         case 'p'  : asc[i]=0x70; break;
         case 'q'  : asc[i]=0x71; break;
         case 'r'  : asc[i]=0x72; break;
         case 's'  : asc[i]=0x73; break;
         case 't'  : asc[i]=0x74; break;
         case 'u'  : asc[i]=0x75; break;
         case 'v'  : asc[i]=0x76; break;
         case 'w'  : asc[i]=0x77; break;
         case 'x'  : asc[i]=0x78; break;
         case 'y'  : asc[i]=0x79; break;
         case 'z'  : asc[i]=0x7A; break;

         default   :
            if (chr[i]==C_EXC) {
               asc[i]=0x21;
            } else if (chr[i]==C_HSH) {
               asc[i]=0x23;
            } else if (chr[i]==C_DLR) {
               asc[i]=0x24;
            } else if (chr[i]==C_ATS) {
               asc[i]=0x40;
            } else if (chr[i]==C_SBO) {
               asc[i]=0x5B;
            } else if (chr[i]==C_BSL) {
               asc[i]=0x5C;
            } else if (chr[i]==C_SBC) {
               asc[i]=0x5D;
            } else if (chr[i]==C_CRT) {
               asc[i]=0x5E;
            } else if (chr[i]==C_GRV) {
               asc[i]=0x60;
            } else if (chr[i]==C_CBO) {
               asc[i]=0x7B;
            } else if (chr[i]==C_VBR) {
               asc[i]=0x7C;
            } else if (chr[i]==C_CBC) {
               asc[i]=0x7D;
            } else if (chr[i]==C_TLD) {
               asc[i]=0x7E;
            } else {
               asc[i]=0x00;
               return(i);
            }
      }
   }
   return(i);
}

extern unsigned int chr2ebc(
   const char*          chr,
         char*          ebc,
   const unsigned int   len)
{
   unsigned int         i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
         case '\n' : ebc[i]=0x25; break;
         case '\r' : ebc[i]=0x0D; break;

         case ' '  : ebc[i]=0x40; break;
         case '\"' : ebc[i]=0x7F; break;
         case '%'  : ebc[i]=0x6C; break;
         case '&'  : ebc[i]=0x50; break;
         case '\'' : ebc[i]=0x7D; break;
         case '('  : ebc[i]=0x4D; break;
         case ')'  : ebc[i]=0x5D; break;
         case '*'  : ebc[i]=0x5C; break;
         case '+'  : ebc[i]=0x4E; break;
         case ','  : ebc[i]=0x6B; break;
         case '-'  : ebc[i]=0x60; break;
         case '.'  : ebc[i]=0x4B; break;
         case '/'  : ebc[i]=0x61; break;

         case '0'  : ebc[i]=0xF0; break;
         case '1'  : ebc[i]=0xF1; break;
         case '2'  : ebc[i]=0xF2; break;
         case '3'  : ebc[i]=0xF3; break;
         case '4'  : ebc[i]=0xF4; break;
         case '5'  : ebc[i]=0xF5; break;
         case '6'  : ebc[i]=0xF6; break;
         case '7'  : ebc[i]=0xF7; break;
         case '8'  : ebc[i]=0xF8; break;
         case '9'  : ebc[i]=0xF9; break;
         case ':'  : ebc[i]=0x7A; break;
         case ';'  : ebc[i]=0x5E; break;
         case '<'  : ebc[i]=0x4C; break;
         case '='  : ebc[i]=0x7E; break;
         case '>'  : ebc[i]=0x6E; break;
         case '?'  : ebc[i]=0x6F; break;

         case 'A'  : ebc[i]=0xC1; break;
         case 'B'  : ebc[i]=0xC2; break;
         case 'C'  : ebc[i]=0xC3; break;
         case 'D'  : ebc[i]=0xC4; break;
         case 'E'  : ebc[i]=0xC5; break;
         case 'F'  : ebc[i]=0xC6; break;
         case 'G'  : ebc[i]=0xC7; break;
         case 'H'  : ebc[i]=0xC8; break;
         case 'I'  : ebc[i]=0xC9; break;
         case 'J'  : ebc[i]=0xD1; break;
         case 'K'  : ebc[i]=0xD2; break;
         case 'L'  : ebc[i]=0xD3; break;
         case 'M'  : ebc[i]=0xD4; break;
         case 'N'  : ebc[i]=0xD5; break;
         case 'O'  : ebc[i]=0xD6; break;

         case 'P'  : ebc[i]=0xD7; break;
         case 'Q'  : ebc[i]=0xD8; break;
         case 'R'  : ebc[i]=0xD9; break;
         case 'S'  : ebc[i]=0xE2; break;
         case 'T'  : ebc[i]=0xE3; break;
         case 'U'  : ebc[i]=0xE4; break;
         case 'V'  : ebc[i]=0xE5; break;
         case 'W'  : ebc[i]=0xE6; break;
         case 'X'  : ebc[i]=0xE7; break;
         case 'Y'  : ebc[i]=0xE8; break;
         case 'Z'  : ebc[i]=0xE9; break;
         case '_'  : ebc[i]=0x6D; break;

         case 'a'  : ebc[i]=0x81; break;
         case 'b'  : ebc[i]=0x82; break;
         case 'c'  : ebc[i]=0x83; break;
         case 'd'  : ebc[i]=0x84; break;
         case 'e'  : ebc[i]=0x85; break;
         case 'f'  : ebc[i]=0x86; break;
         case 'g'  : ebc[i]=0x87; break;
         case 'h'  : ebc[i]=0x88; break;
         case 'i'  : ebc[i]=0x89; break;
         case 'j'  : ebc[i]=0x91; break;
         case 'k'  : ebc[i]=0x92; break;
         case 'l'  : ebc[i]=0x93; break;
         case 'm'  : ebc[i]=0x94; break;
         case 'n'  : ebc[i]=0x95; break;
         case 'o'  : ebc[i]=0x96; break;

         case 'p'  : ebc[i]=0x97; break;
         case 'q'  : ebc[i]=0x98; break;
         case 'r'  : ebc[i]=0x99; break;
         case 's'  : ebc[i]=0xA2; break;
         case 't'  : ebc[i]=0xA3; break;
         case 'u'  : ebc[i]=0xA4; break;
         case 'v'  : ebc[i]=0xA5; break;
         case 'w'  : ebc[i]=0xA6; break;
         case 'x'  : ebc[i]=0xA7; break;
         case 'y'  : ebc[i]=0xA8; break;
         case 'z'  : ebc[i]=0xA9; break;

         default   :
            if (chr[i]==C_EXC) {
               ebc[i]=0x5A;
            } else if (chr[i]==C_HSH) {
               ebc[i]=0x7B;
            } else if (chr[i]==C_DLR) {
               ebc[i]=0x5B;
            } else if (chr[i]==C_ATS) {
               ebc[i]=0x7C;
            } else if (chr[i]==C_SBO) {
               ebc[i]=0xAD;
            } else if (chr[i]==C_BSL) {
               ebc[i]=0xE0;
            } else if (chr[i]==C_SBC) {
               ebc[i]=0xBD;
            } else if (chr[i]==C_CRT) {
               ebc[i]=0x5F;
            } else if (chr[i]==C_GRV) {
               ebc[i]=0x79;
            } else if (chr[i]==C_CBO) {
               ebc[i]=0xC0;
            } else if (chr[i]==C_VBR) {
               ebc[i]=0x4F;
            } else if (chr[i]==C_CBC) {
               ebc[i]=0xD0;
            } else if (chr[i]==C_TLD) {
               ebc[i]=0xA1;
            } else {
               ebc[i]=0x00;
               return(i);
            }
      }
   }
   return(i);
}

extern unsigned int asc2chr(
   const char*          asc,
         char*          chr,
   const unsigned int   len)
{
   unsigned int         i;
   const char           asc_map[256]={
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,'\n',0x0,0x0,'\r',0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   ' ','!','\"','#','$','%','&','\'','(',')','*','+',',','-','.','/',   /*nodiac*/
   '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',     /*nodiac*/
   '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',     /*nodiac*/
   'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_',    /*nodiac*/
   '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',     /*nodiac*/
   'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~',0x0,     /*nodiac*/
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
   for (i=0;i<len;i++) {
      chr[i]=asc_map[*((unsigned char*)(asc+i))];
      if (chr[i]==0x0) return(i);
   }
   return(i);
}

extern void asc_chr(
   const char*          asc,
         char*          chr,
   const unsigned int   len)
{
   unsigned int         i;
   const char           asc_map[256]={
   '_','_','_','_','_','_','_','_','_','_','\n','_','_','\r','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   ' ','!','\"','#','$','%','&','\'','(',')','*','+',',','-','.','/',   /*nodiac*/
   '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',     /*nodiac*/
   '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',     /*nodiac*/
   'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_',    /*nodiac*/
   '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',     /*nodiac*/
   'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~','_',     /*nodiac*/
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_',
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_','_','_'};
   for (i=0;i<len;i++) {
      chr[i]=asc_map[*((unsigned char*)(asc+i))];
   }
}

extern void chr_asc(
   const char*          chr,
         char*          asc,
   const unsigned int   len)
{
   unsigned int         i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
         case 0x00 : asc[i]=0x00; break;
         case '\n' : asc[i]=0x0A; break;
         case '\r' : asc[i]=0x0D; break;

         case ' '  : asc[i]=0x20; break;
         case '\"' : asc[i]=0x22; break;
         case '%'  : asc[i]=0x25; break;
         case '&'  : asc[i]=0x26; break;
         case '\'' : asc[i]=0x27; break;
         case '('  : asc[i]=0x28; break;
         case ')'  : asc[i]=0x29; break;
         case '*'  : asc[i]=0x2A; break;
         case '+'  : asc[i]=0x2B; break;
         case ','  : asc[i]=0x2C; break;
         case '-'  : asc[i]=0x2D; break;
         case '.'  : asc[i]=0x2E; break;
         case '/'  : asc[i]=0x2F; break;

         case '0'  : asc[i]=0x30; break;
         case '1'  : asc[i]=0x31; break;
         case '2'  : asc[i]=0x32; break;
         case '3'  : asc[i]=0x33; break;
         case '4'  : asc[i]=0x34; break;
         case '5'  : asc[i]=0x35; break;
         case '6'  : asc[i]=0x36; break;
         case '7'  : asc[i]=0x37; break;
         case '8'  : asc[i]=0x38; break;
         case '9'  : asc[i]=0x39; break;
         case ':'  : asc[i]=0x3A; break;
         case ';'  : asc[i]=0x3B; break;
         case '<'  : asc[i]=0x3C; break;
         case '='  : asc[i]=0x3D; break;
         case '>'  : asc[i]=0x3E; break;
         case '?'  : asc[i]=0x3F; break;

         case 'A'  : asc[i]=0x41; break;
         case 'B'  : asc[i]=0x42; break;
         case 'C'  : asc[i]=0x43; break;
         case 'D'  : asc[i]=0x44; break;
         case 'E'  : asc[i]=0x45; break;
         case 'F'  : asc[i]=0x46; break;
         case 'G'  : asc[i]=0x47; break;
         case 'H'  : asc[i]=0x48; break;
         case 'I'  : asc[i]=0x49; break;
         case 'J'  : asc[i]=0x4A; break;
         case 'K'  : asc[i]=0x4B; break;
         case 'L'  : asc[i]=0x4C; break;
         case 'M'  : asc[i]=0x4D; break;
         case 'N'  : asc[i]=0x4E; break;
         case 'O'  : asc[i]=0x4F; break;

         case 'P'  : asc[i]=0x50; break;
         case 'Q'  : asc[i]=0x51; break;
         case 'R'  : asc[i]=0x52; break;
         case 'S'  : asc[i]=0x53; break;
         case 'T'  : asc[i]=0x54; break;
         case 'U'  : asc[i]=0x55; break;
         case 'V'  : asc[i]=0x56; break;
         case 'W'  : asc[i]=0x57; break;
         case 'X'  : asc[i]=0x58; break;
         case 'Y'  : asc[i]=0x59; break;
         case 'Z'  : asc[i]=0x5A; break;
         case '_'  : asc[i]=0x5F; break;

         case 'a'  : asc[i]=0x61; break;
         case 'b'  : asc[i]=0x62; break;
         case 'c'  : asc[i]=0x63; break;
         case 'd'  : asc[i]=0x64; break;
         case 'e'  : asc[i]=0x65; break;
         case 'f'  : asc[i]=0x66; break;
         case 'g'  : asc[i]=0x67; break;
         case 'h'  : asc[i]=0x68; break;
         case 'i'  : asc[i]=0x69; break;
         case 'j'  : asc[i]=0x6A; break;
         case 'k'  : asc[i]=0x6B; break;
         case 'l'  : asc[i]=0x6C; break;
         case 'm'  : asc[i]=0x6D; break;
         case 'n'  : asc[i]=0x6E; break;
         case 'o'  : asc[i]=0x6F; break;

         case 'p'  : asc[i]=0x70; break;
         case 'q'  : asc[i]=0x71; break;
         case 'r'  : asc[i]=0x72; break;
         case 's'  : asc[i]=0x73; break;
         case 't'  : asc[i]=0x74; break;
         case 'u'  : asc[i]=0x75; break;
         case 'v'  : asc[i]=0x76; break;
         case 'w'  : asc[i]=0x77; break;
         case 'x'  : asc[i]=0x78; break;
         case 'y'  : asc[i]=0x79; break;
         case 'z'  : asc[i]=0x7A; break;
         default   :
            if (chr[i]==C_EXC) {
               asc[i]=0x21;
            } else if (chr[i]==C_HSH) {
               asc[i]=0x23;
            } else if (chr[i]==C_DLR) {
               asc[i]=0x24;
            } else if (chr[i]==C_ATS) {
               asc[i]=0x40;
            } else if (chr[i]==C_SBO) {
               asc[i]=0x5B;
            } else if (chr[i]==C_BSL) {
               asc[i]=0x5C;
            } else if (chr[i]==C_SBC) {
               asc[i]=0x5D;
            } else if (chr[i]==C_CRT) {
               asc[i]=0x5E;
            } else if (chr[i]==C_GRV) {
               asc[i]=0x60;
            } else if (chr[i]==C_CBO) {
               asc[i]=0x7B;
            } else if (chr[i]==C_VBR) {
               asc[i]=0x7C;
            } else if (chr[i]==C_CBC) {
               asc[i]=0x7D;
            } else if (chr[i]==C_TLD) {
               asc[i]=0x7E;
            } else {
               asc[i]=0x5F;
            }
            break;
      }
   }
}

extern unsigned int ebc2chr(
   const char*          ebc,
         char*          chr,
   const unsigned int   len)
{
   unsigned int         i;
   const char           ebc_map[256]={
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,'\r',0x0, 0x0,
   0x0,0x0,0x0,0x0,0x0,'\n',0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x0, 0x0,
   0x0,0x0,0x0,0x0,0x0,'\n',0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x0, 0x0,
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x0, 0x0,
   ' ',0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,'.','<','(', '+', '|',   /*nodiac*/
   '&',0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,'!','$','*',')', ';', '^',   /*nodiac*/
   '-','/',0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,',','%','_', '>', '\?',  /*nodiac*/
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,'`',':','#','@','\'','=','\"',   /*nodiac*/
   0x0,'a','b','c','d','e','f','g','h','i',0x0,0x0,0x0,0x0, 0x0, 0x0,   /*nodiac*/
   0x0,'j','k','l','m','n','o','p','q','r',0x0,0x0,0x0,0x0, 0x0, 0x0,   /*nodiac*/
   0x0,'~','s','t','u','v','w','x','y','z',0x0,0x0,0x0,'[', 0x0, 0x0,   /*nodiac*/
   0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,']', 0x0, 0x0,   /*nodiac*/
   '{','A','B','C','D','E','F','G','H','I',0x0,0x0,0x0,0x0, 0x0, 0x0,   /*nodiac*/
   '}','J','K','L','M','N','O','P','Q','R',0x0,0x0,0x0,0x0, 0x0, 0x0,   /*nodiac*/
   '\\',0x0,'S','T','U','V','W','X','Y','Z',0x0,0x0,0x0,0x0, 0x0, 0x0,  /*nodiac*/
   '0','1','2','3','4','5','6','7','8','9',0x0,0x0,0x0,0x0, 0x0, 0x0};  /*nodiac*/
   for (i=0;i<len;i++) {
      chr[i]=ebc_map[*((unsigned char*)(ebc+i))];
      switch (chr[i]) {
      case '!' : chr[i]=C_EXC; break;  /*nodiac*/
      case '$' : chr[i]=C_DLR; break;  /*nodiac*/
      case '#' : chr[i]=C_HSH; break;  /*nodiac*/
      case '@' : chr[i]=C_ATS; break;  /*nodiac*/
      case '[' : chr[i]=C_SBO; break;  /*nodiac*/
      case '\\': chr[i]=C_BSL; break;  /*nodiac*/
      case ']' : chr[i]=C_SBC; break;  /*nodiac*/
      case '^' : chr[i]=C_CRT; break;  /*nodiac*/
      case '`' : chr[i]=C_GRV; break;  /*nodiac*/
      case '{' : chr[i]=C_CBO; break;  /*nodiac*/
      case '|' : chr[i]=C_VBR; break;  /*nodiac*/
      case '}' : chr[i]=C_CBC; break;  /*nodiac*/
      case '~' : chr[i]=C_TLD; break;  /*nodiac*/
      case 0x0 : return(i);
      }
   }
   return(i);
}

extern void ebc_chr(
   const char*          ebc,
         char*          chr,
   const unsigned int   len)
{
   unsigned int         i;
   const char           ebc_map[256]={
   '_','_','_','_','_','_','_','_','_','_','_','_','_','\r', '_', '_',  /*nodiac*/
   '_','_','_','_','_','\n','_','_','_','_','_','_','_','_', '_', '_',  /*nodiac*/
   '_','_','_','_','_','\n','_','_','_','_','_','_','_','_', '_', '_',  /*nodiac*/
   '_','_','_','_','_','_','_','_','_','_','_','_','_','_', '_', '_',   /*nodiac*/
   ' ','_','_','_','_','_','_','_','_','_','_','.','<','(', '+', '|',   /*nodiac*/
   '&','_','_','_','_','_','_','_','_','_','!','$','*',')', ';', '^',   /*nodiac*/
   '-','/','_','_','_','_','_','_','_','_','_',',','%','_', '>', '\?',  /*nodiac*/
   '_','_','_','_','_','_','_','_','_','`',':','#','@','\'','=','\"',   /*nodiac*/
   '_','a','b','c','d','e','f','g','h','i','_','_','_','_', '_', '_',   /*nodiac*/
   '_','j','k','l','m','n','o','p','q','r','_','_','_','_', '_', '_',   /*nodiac*/
   '_','~','s','t','u','v','w','x','y','z','_','_','_','[', '_', '_',   /*nodiac*/
   '_','_','_','_','_','_','_','_','_','_','_','_','_',']', '_', '_',   /*nodiac*/
   '{','A','B','C','D','E','F','G','H','I','_','_','_','_', '_', '_',   /*nodiac*/
   '}','J','K','L','M','N','O','P','Q','R','_','_','_','_', '_', '_',   /*nodiac*/
   '\\','_','S','T','U','V','W','X','Y','Z','_','_','_','_', '_', '_',  /*nodiac*/
   '0','1','2','3','4','5','6','7','8','9','_','_','_','_', '_', '_'};  /*nodiac*/
   for (i=0;i<len;i++) {
      chr[i]=ebc_map[*((unsigned char*)(ebc+i))];
      switch (chr[i]) {
      case '!' : chr[i]=C_EXC; break;  /*nodiac*/
      case '$' : chr[i]=C_DLR; break;  /*nodiac*/
      case '#' : chr[i]=C_HSH; break;  /*nodiac*/
      case '@' : chr[i]=C_ATS; break;  /*nodiac*/
      case '[' : chr[i]=C_SBO; break;  /*nodiac*/
      case '\\': chr[i]=C_BSL; break;  /*nodiac*/
      case ']' : chr[i]=C_SBC; break;  /*nodiac*/
      case '^' : chr[i]=C_CRT; break;  /*nodiac*/
      case '`' : chr[i]=C_GRV; break;  /*nodiac*/
      case '{' : chr[i]=C_CBO; break;  /*nodiac*/
      case '|' : chr[i]=C_VBR; break;  /*nodiac*/
      case '}' : chr[i]=C_CBC; break;  /*nodiac*/
      case '~' : chr[i]=C_TLD; break;  /*nodiac*/
      }
   }
}

extern void chr_ebc(
   const char*          chr,
         char*          ebc,
   const unsigned int   len)
{
   unsigned int         i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
      case 0x00 : ebc[i]=0x00; break;
      case '\n' : ebc[i]=0x25; break;
      case '\r' : ebc[i]=0x0D; break;

      case ' '  : ebc[i]=0x40; break;
      case '\"' : ebc[i]=0x7F; break;
      case '%'  : ebc[i]=0x6C; break;
      case '&'  : ebc[i]=0x50; break;
      case '\'' : ebc[i]=0x7D; break;
      case '('  : ebc[i]=0x4D; break;
      case ')'  : ebc[i]=0x5D; break;
      case '*'  : ebc[i]=0x5C; break;
      case '+'  : ebc[i]=0x4E; break;
      case ','  : ebc[i]=0x6B; break;
      case '-'  : ebc[i]=0x60; break;
      case '.'  : ebc[i]=0x4B; break;
      case '/'  : ebc[i]=0x61; break;

      case '0'  : ebc[i]=0xF0; break;
      case '1'  : ebc[i]=0xF1; break;
      case '2'  : ebc[i]=0xF2; break;
      case '3'  : ebc[i]=0xF3; break;
      case '4'  : ebc[i]=0xF4; break;
      case '5'  : ebc[i]=0xF5; break;
      case '6'  : ebc[i]=0xF6; break;
      case '7'  : ebc[i]=0xF7; break;
      case '8'  : ebc[i]=0xF8; break;
      case '9'  : ebc[i]=0xF9; break;
      case ':'  : ebc[i]=0x7A; break;
      case ';'  : ebc[i]=0x5E; break;
      case '<'  : ebc[i]=0x4C; break;
      case '='  : ebc[i]=0x7E; break;
      case '>'  : ebc[i]=0x6E; break;
      case '?'  : ebc[i]=0x6F; break;

      case 'A'  : ebc[i]=0xC1; break;
      case 'B'  : ebc[i]=0xC2; break;
      case 'C'  : ebc[i]=0xC3; break;
      case 'D'  : ebc[i]=0xC4; break;
      case 'E'  : ebc[i]=0xC5; break;
      case 'F'  : ebc[i]=0xC6; break;
      case 'G'  : ebc[i]=0xC7; break;
      case 'H'  : ebc[i]=0xC8; break;
      case 'I'  : ebc[i]=0xC9; break;
      case 'J'  : ebc[i]=0xD1; break;
      case 'K'  : ebc[i]=0xD2; break;
      case 'L'  : ebc[i]=0xD3; break;
      case 'M'  : ebc[i]=0xD4; break;
      case 'N'  : ebc[i]=0xD5; break;
      case 'O'  : ebc[i]=0xD6; break;

      case 'P'  : ebc[i]=0xD7; break;
      case 'Q'  : ebc[i]=0xD8; break;
      case 'R'  : ebc[i]=0xD9; break;
      case 'S'  : ebc[i]=0xE2; break;
      case 'T'  : ebc[i]=0xE3; break;
      case 'U'  : ebc[i]=0xE4; break;
      case 'V'  : ebc[i]=0xE5; break;
      case 'W'  : ebc[i]=0xE6; break;
      case 'X'  : ebc[i]=0xE7; break;
      case 'Y'  : ebc[i]=0xE8; break;
      case 'Z'  : ebc[i]=0xE9; break;
      case '_'  : ebc[i]=0x6D; break;

      case 'a'  : ebc[i]=0x81; break;
      case 'b'  : ebc[i]=0x82; break;
      case 'c'  : ebc[i]=0x83; break;
      case 'd'  : ebc[i]=0x84; break;
      case 'e'  : ebc[i]=0x85; break;
      case 'f'  : ebc[i]=0x86; break;
      case 'g'  : ebc[i]=0x87; break;
      case 'h'  : ebc[i]=0x88; break;
      case 'i'  : ebc[i]=0x89; break;
      case 'j'  : ebc[i]=0x91; break;
      case 'k'  : ebc[i]=0x92; break;
      case 'l'  : ebc[i]=0x93; break;
      case 'm'  : ebc[i]=0x94; break;
      case 'n'  : ebc[i]=0x95; break;
      case 'o'  : ebc[i]=0x96; break;

      case 'p'  : ebc[i]=0x97; break;
      case 'q'  : ebc[i]=0x98; break;
      case 'r'  : ebc[i]=0x99; break;
      case 's'  : ebc[i]=0xA2; break;
      case 't'  : ebc[i]=0xA3; break;
      case 'u'  : ebc[i]=0xA4; break;
      case 'v'  : ebc[i]=0xA5; break;
      case 'w'  : ebc[i]=0xA6; break;
      case 'x'  : ebc[i]=0xA7; break;
      case 'y'  : ebc[i]=0xA8; break;
      case 'z'  : ebc[i]=0xA9; break;

      default   :
         if (chr[i]==C_EXC) {
            ebc[i]=0x5A;
         } else if (chr[i]==C_HSH) {
            ebc[i]=0x7B;
         } else if (chr[i]==C_DLR) {
            ebc[i]=0x5B;
         } else if (chr[i]==C_ATS) {
            ebc[i]=0x7C;
         } else if (chr[i]==C_SBO) {
            ebc[i]=0xAD;
         } else if (chr[i]==C_BSL) {
            ebc[i]=0xE0;
         } else if (chr[i]==C_SBC) {
            ebc[i]=0xBD;
         } else if (chr[i]==C_CRT) {
            ebc[i]=0x5F;
         } else if (chr[i]==C_GRV) {
            ebc[i]=0x79;
         } else if (chr[i]==C_CBO) {
            ebc[i]=0xC0;
         } else if (chr[i]==C_VBR) {
            ebc[i]=0x4F;
         } else if (chr[i]==C_CBC) {
            ebc[i]=0xD0;
         } else if (chr[i]==C_TLD) {
            ebc[i]=0xA1;
         } else {
            ebc[i]=0x6D;
         }
         break;
      }
   }
}

extern int file2str(const void* hdl, const char* filename, char** buf, int* bufsize, char* errmsg, const int msgsiz) {
   int siLen=0, siHlp;
   char* pcHlp;
   FILE* pfFile=NULL;
   const int freadLen=65536;

   (void)(hdl);//unsued

   if (filename==NULL || buf==NULL || bufsize==NULL || (*buf!=NULL && (*bufsize<=0))) {
      if (errmsg!=NULL && msgsiz) {
         snprintf(errmsg,msgsiz,"Illegal parameters passed to file2str(%p,%p,%p) (Bug)",filename,buf,bufsize);
      }
      return -1; // bad args
   }
   if (*buf==NULL)
      *bufsize=0;
#if !defined(__USS__) && !defined(__ZOS__) && defined(__FL5__)
      int r=siGetMFNameNative(filename, &filename, NULL);
      if (r && r!=-4) {
         if (errmsg!=NULL && msgsiz) {
            snprintf(errmsg,msgsiz,"Resolve of host dataset name (%s) over MF-EDZ catalog failed",filename);
         }
         return(-6);
      }
#endif
   errno=0;
   pfFile=fopen_hfq(filename, "rb");
   if (pfFile == NULL) {
      if (errmsg!=NULL && msgsiz) {
         snprintf(errmsg,msgsiz,"Open of file (%s) failed (%d - %s)",filename,errno,strerror(errno));
      }
      return -2; // fopen failed
   }
   while (!ferror(pfFile) && !feof(pfFile)) {
      if (*bufsize-siLen<freadLen+1) {
         if (*bufsize>INT_MAX-(freadLen*2+1)) {
            if (errmsg!=NULL && msgsiz) {
               snprintf(errmsg,msgsiz,"File (%s) is too big (integer overflow)",filename);
            }
            fclose(pfFile);
            return -3; // integer overflow
         }
         siHlp=*bufsize+freadLen*2+1;
         pcHlp=(char*)realloc_nowarn(*buf, siHlp);
         if (pcHlp==NULL) {
            if (errmsg!=NULL && msgsiz) {
               snprintf(errmsg,msgsiz,"Allocation of memory to read file (%s) failed",filename);
            }
            fclose(pfFile);
            return -4; // realloc failed
         }
         *bufsize=siHlp;
         *buf=pcHlp;
      }
      siLen+=fread(*buf+siLen, 1, freadLen, pfFile);
   }
   if (ferror(pfFile)) {
      if (errmsg!=NULL && msgsiz) {
         snprintf(errmsg,msgsiz,"Read of file (%s) to string in memory failed (%d - %s)",filename,errno,strerror(errno));
      }
      fclose(pfFile);
      return -5; // read error
   }
   fclose(pfFile);
   if (*buf!=NULL) // empty file
      (*buf)[siLen]='\0';
   return siLen;
}

extern int arry2str(char* array[], const int count, const char* separ, const int separLen, char** out, int* outlen) {
   if (count<0 || array==NULL || out==NULL || outlen==NULL || (separLen>0 && separ==NULL))
      return -1; // bad args
   size_t uiSumLen=((count)*separLen)+1;
   size_t uiLens[count+1];
   char*  pcHlp;
   char*  pcOut;
   size_t uiOut;


   if (*out==NULL) {
      *outlen=0;
      uiOut=0;
   } else {
      uiOut=strlen(*out);
      uiSumLen+=uiOut;
   }

   long i;
   for (i=0; i<count; i++) {
      uiLens[i]=strlen(array[i]);
      uiSumLen+=uiLens[i];
   }

   if (*outlen<(int)uiSumLen || *out==NULL) {
      if (uiSumLen < 1)
         uiSumLen = 4;
      pcHlp=(char*)realloc_nowarn(*out, uiSumLen);
      if (pcHlp==NULL) {
         return -2; // realloc failed
      }
      *outlen=uiSumLen;
      *out=pcHlp;
   }


   pcOut=(*out)+uiOut;
   if (count>0) {
      if (uiOut) {
         if (separLen>0) {
            memcpy(pcOut, separ, separLen);
            pcOut+=separLen;
         }
      }
      memcpy(pcOut, array[0], uiLens[0]);
      pcOut+=uiLens[0];
      for (i=1; i<count; i++) {
         if (separLen>0) {
            memcpy(pcOut, separ, separLen);
            pcOut+=separLen;
         }
         memcpy(pcOut, array[i], uiLens[i]);
         pcOut+=uiLens[i];
      }
   }
   *pcOut='\0';

   return 0;
}

extern int strxcmp(
   const int            ca,
   const char*          s1,
   const char*          s2,
   const int            n,
   const int            c,
   const int            f)
{
   if (ca) {
      int d=*s1-*s2;
      if (n) {
         if (c==-1) {
            int i=0;
            while (d==0 && *s1!=0 && *s2!=0 && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
            if (i<n) return(n-i);
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else if (c==0) {
            int i=1;
            while (d==0 && *s1!=0 && *s2!=0 && i<n) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
         } else {
            int i=0;
            while (d==0 && *s1!=0 && *s2!=0 && *s1!=c && *s2!=c) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
            if (i<n) return(n-i);
            if (f && (*s1==c || *s2==c)) return(0);
         }
         return(d);
      } else {
         if (c==-1) {
            while (d==0 && *s1!=0 && *s2!=0 && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++;
               d=*s1-*s2;
            }
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else if (c==0) {
            while (d==0 && *s1!=0 && *s2!=0) {
               s1++; s2++;
               d=*s1-*s2;
            }
         } else {
            while (d==0 && *s1!=0 && *s2!=0 && *s1!=c && *s2!=c) {
               s1++; s2++;
               d=*s1-*s2;
            }
            if (f && (*s1==c || *s2==c)) return(0);
         }
         return(d);
      }
   } else {
      int d=tolower(*s1)-tolower(*s2);
      if (n) {
         if (c==-1) {
            int i=0;
            while (d==0 && *s1!=0 && *s2!=0 && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
            if (i<n) return(n-i);
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else if (c==0) {
            int i=1;
            while (d==0 && *s1!=0 && *s2!=0 && i<n) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
         } else {
            int i=0;
            while (d==0 && *s1!=0 && *s2!=0 && tolower(*s1)!=tolower(c) && tolower(*s2)!=tolower(c)) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
            if (i<n) return(n-i);
            if (f && (tolower(*s1)==tolower(c) || tolower(*s2)==tolower(c))) return(0);
         }
         return(d);
      } else {
         if (c==-1) {
            while (d==0 && *s1!=0 && *s2!=0 && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++;
               d=tolower(*s1)-tolower(*s2);
            }
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else if (c==0) {
            while (d==0 && *s1!=0 && *s2!=0) {
               s1++; s2++;
               d=tolower(*s1)-tolower(*s2);
            }
         } else {
            while (d==0 && *s1!=0 && *s2!=0 && tolower(*s1)!=tolower(c) && *s2!=tolower(c)) {
               s1++; s2++;
               d=tolower(*s1)-tolower(*s2);
            }
            if (f && (tolower(*s1)==tolower(c) || tolower(*s2)==tolower(c))) return(0);
         }
         return(d);
      }
   }
}

extern char* cstime(signed long long t, char* p) {
   static char       acBuf[20];
   char*             pcStr=(p!=NULL)?p:acBuf;
   time_t            h=(t)?(time_t)t:time(NULL);
   struct tm         st;
   const struct tm*  x=localtime_r(&h,&st);
   if (x!=NULL) {
      strftime(pcStr,sizeof(acBuf),"%Y-%m-%d %H:%M:%S",x);
   } else {
      snprintf(acBuf,sizeof(acBuf),"NO-VALID-TIME-FOUND");
   }
   return(pcStr);
}

extern int envarInsert(TsEnVarList** ppList,const char* pcName,const char* pcValue) {
   if (ppList!=NULL) {
      TsEnVarList* psNode=(TsEnVarList*)malloc(sizeof(TsEnVarList));
      if (psNode==NULL) {
         resetEnvars(ppList);
         return(CLERTC_MEM);
      }
      psNode->pcName=(char*)malloc(strlen(pcName)+1);
      if (psNode->pcName==NULL) {
         free(psNode);
         resetEnvars(ppList);
         return(CLERTC_MEM);
      }
      strcpy(psNode->pcName,pcName);
      if (pcValue!=NULL) {
         psNode->pcValue=(char*)malloc(strlen(pcValue)+1);
         if (psNode->pcValue==NULL) {
            free(psNode->pcName);
            free(psNode);
            resetEnvars(ppList);
            return(CLERTC_MEM);
         }
         strcpy(psNode->pcValue,pcValue);
      } else {
         psNode->pcValue=NULL;
      }
      psNode->psNext=*ppList;
      *ppList=psNode;
   }
   return(CLERTC_OK);
}

extern int resetEnvars(TsEnVarList** ppList) {
   int r=0;
   int c=0;
   if (ppList!=NULL) {
      while(*ppList!=NULL) {
         TsEnVarList* psHelp=(*ppList);
         (*ppList)=(*ppList)->psNext;
         if (psHelp->pcValue!=NULL) {
            if (SETENV(psHelp->pcName,psHelp->pcValue)) r--; else c++;
         } else {
            if (UNSETENV(psHelp->pcName)) r--; else c++;
         }
         SAFE_FREE(psHelp->pcName);
         SAFE_FREE(psHelp->pcValue);
         free(psHelp);
      }
      *ppList=NULL;
   }
   if (r) return(-1*CLERTC_SYS); else return(c);
}

#ifdef __EBCDIC__
#  define IS_ENVAR_LE(c)  ((c)==';' || (c)==0x15 || (c)==0x25  || (c)==0x0D || (c)=='\f')
#else
#  define IS_ENVAR_LE(c)  ((c)==';' || (c)=='\n' || (c)=='\r' || (c)=='\f')
#endif

extern int loadEnvars(const unsigned int uiLen, const char* pcBuf, FILE* pfOut, FILE* pfErr, TsEnVarList** ppList) {
   int            siErr=0;
   int            c=0;
   if (uiLen && pcBuf!=NULL) {
      int            x=0;
      int            a=0;
      int            e=0;
      char*          pcKey;
      char*          pcVal;
      char*          pcTws;
      char*          pcHlp;
      const char*    pcCnt;
      const char*    pcEnd;
      const char*    pcCmp;
      char*          pcEnv=malloc(uiLen+1);
      // cppcheck-suppress knownConditionTrueFalse
      if (pcEnv==NULL) return(-1*CLERTC_MEM);

   // EBCDIC/ASCII detection and conversion to local character set
      for (pcCnt=pcBuf,pcEnd=pcBuf+uiLen;pcCnt<pcEnd && e==0 && a==0;pcCnt++) {
         if ((unsigned char)(*pcCnt)>=0x80U) x++;
         if ((unsigned char)(*pcCnt)==0x7EU) e++;
         if ((unsigned char)(*pcCnt)==0x3DU) a++;
      }
      if (!a && e && x) {
         // cppcheck-suppress knownConditionTrueFalse
         if ('0'!=0xF0) {
            ebc_chr(pcBuf,pcEnv,uiLen);
         } else {
            memcpy(pcEnv,pcBuf,uiLen);
         }
      } else {
         // cppcheck-suppress knownConditionTrueFalse
         if ('0'!=0x30) {
            asc_chr(pcBuf,pcEnv,uiLen);
         } else {
            memcpy(pcEnv,pcBuf,uiLen);
         }
      }
      pcEnv[uiLen]=0x00;
   // parse and set environment variables
      pcHlp=pcEnv;
      pcEnd=pcEnv+uiLen;
      while(pcHlp<pcEnd) {
         while (pcHlp<pcEnd && isspace(*pcHlp)) pcHlp++;
         if (pcHlp<pcEnd) {
            pcKey=pcHlp;
            while(pcHlp<pcEnd && *pcHlp!='=') pcHlp++;
            if (*pcHlp=='=') {
               *pcHlp=0x00; pcHlp++;
               pcTws=pcKey+strlen(pcKey);
               while (isspace(*(pcTws-1))) {
                  pcTws--; *pcTws=0x00;
               }
               if (pcHlp<pcEnd) {
                  while(pcHlp<pcEnd && isspace(*pcHlp) && !IS_ENVAR_LE(*pcHlp)) pcHlp++;
                  pcVal=pcHlp;
                  while(pcHlp<pcEnd && !IS_ENVAR_LE(*pcHlp)) pcHlp++;
                  *pcHlp=0x00; pcHlp++;
                  pcTws=pcVal+strlen(pcVal);
                  while (isspace(*(pcTws-1))) {
                     pcTws--; *pcTws=EOS;
                  }
                  siErr=envarInsert(ppList,pcKey,GETENV(pcKey));
                  if (siErr) {
                     if(pfErr!=NULL){
                        fprintf(pfErr,"Build envar list for reset failed (%s)\n",pcKey);
                     }
                  }
                  if (*pcVal) {
                     if (SETENV(pcKey,pcVal)) {
                        if (pfErr!=NULL) {
                           fprintf(pfErr,"Put variable (%s=%s) to environment failed (%d - %s)\n",pcKey,pcVal,errno,strerror(errno));
                        }
                        siErr=CLERTC_SYS;
                     } else {
                        pcCmp=GETENV(pcKey);
                        if (pcCmp==NULL || strcmp(pcVal,pcCmp)) {
                           if (pfErr!=NULL) {
                              fprintf(pfErr,"Put variable (%s=%s) to environment failed (strcmp(%s,GETENV(%s)))\n",pcKey,pcVal,pcVal,pcKey);
                           }
                           siErr=CLERTC_SYS;
                        } else {
                           if (pfOut!=NULL) {
                              char acTs[24];
                              fprintf(pfOut,"%s Put variable (%s=%s) to environment was successful\n",cstime(0,acTs),pcKey,pcVal);
                           }
                           c++;
                        }
                     }

                  } else {
                     if (UNSETENV(pcKey)) {
                        if (pfErr!=NULL) {
                           fprintf(pfErr,"Remove variable (%s=%s(EMPTY->UNSET)) from environment failed (%d - %s)\n",pcKey,pcVal,errno,strerror(errno));
                        }
                        siErr=CLERTC_SYS;
                     } else {
                        if (GETENV(pcKey)!=NULL) {
                           if (pfErr!=NULL) {
                              fprintf(pfErr,"Remove variable (%s=%s(EMPTY->UNSET)) from environment failed (GETENV(%s)%c=NULL)\n",pcKey,pcVal,pcKey,C_EXC);
                           }
                           siErr=CLERTC_SYS;
                        } else {
                           if (pfOut!=NULL) {
                              char acTs[24];
                              fprintf(pfOut,"%s Remove variable (%s=%s(EMPTY->UNSET)) from environment was successful\n",cstime(0,acTs),pcKey,pcVal);
                           }
                           c++;
                        }
                     }
                  }
               }
            }
         }
      }
      free(pcEnv);
   }
   return((siErr)?(-1*siErr):c);
}

extern int readEnvars(const char* pcFil, FILE* pfOut, FILE* pfErr, TsEnVarList** ppList) {
   int            c=0;
   FILE*          pfTmp=NULL;
   if(pcFil==NULL || pcFil[0]=='\0'){
#if defined(__ZOS__)
      pfTmp = fopen_nowarn("DD:STDENV","r");
      if (pfTmp==NULL) {
         pfTmp = fopen_nowarn("STDENV","r");
      }
#elif defined (__USS__)
      pfTmp = fopen_nowarn("DD:STDENV","r");
      if (pfTmp==NULL) {
         pfTmp = fopen_nowarn(".stdenv","r");
         if (pfTmp==NULL) {
            char* pcHom=dhomedir(TRUE);
            if (pcHom!=NULL) {
               char*    pcHlp=NULL;
               size_t   szHlp=0;
               srprintf(&pcHlp,&szHlp,strlen(pcHom),"%s.stdenv",pcHom);
               if (pcHlp!=NULL) {
                  pfTmp = fopen_nowarn(pcHlp,"r");
                  free(pcHlp);
               }
               free(pcHom);
            }
            if (pfTmp==NULL) {
               pfTmp = fopen_nowarn("//STDENV","r");
            }
         }
      }
#else
#  if !defined(__USS__) && !defined(__ZOS__) && defined(__FL5__)
      if (siGetMFNameNative("DD:STDENV", &pcFil, NULL)==0) {
         pfTmp = fopen_nowarn(pcFil,"r");
      }
#  endif
      if (pfTmp==NULL) {
         pfTmp = fopen_nowarn(".stdenv","r");
         if (pfTmp==NULL) {
            char* pcHom=dhomedir(TRUE);
            if (pcHom!=NULL) {
               char*    pcHlp=NULL;
               size_t   szHlp=0;
               srprintf(&pcHlp,&szHlp,strlen(pcHom),"%s.stdenv",pcHom);
               if (pcHlp!=NULL) {
                  pfTmp = fopen_nowarn(pcHlp,"r");
                  free(pcHlp);
               }
               free(pcHom);
            }
         }
      }
#endif
   } else {
      char* pcHlp=dcpmapfil(pcFil);
      if (pcHlp!=NULL) {
         pfTmp=fopen_hfq_nowarn(pcHlp,"r");
         free(pcHlp);
      }
   }
   if (pfTmp!=NULL) { /*Ignore if open failed*/
      unsigned int   uiLen;
      unsigned int   uiPos=0;
      unsigned int   uiSiz=4096;
      // cppcheck-suppress knownArgument
      char*          pcBuf=malloc(uiSiz+1);
      char*          pcHlp;
// read the file into buffer
      // cppcheck-suppress knownConditionTrueFalse
      if (pcBuf==NULL) {
         fclose(pfTmp);
         return(-1*CLERTC_MEM);
      }
      uiLen=fread(pcBuf+uiPos,1,4096,pfTmp);
      while(uiLen) {
         uiSiz+=(uiLen>=4096)?4096:uiLen;
         pcHlp=realloc(pcBuf,uiSiz+1);
         if (pcHlp==NULL) {
            free(pcBuf);
            fclose(pfTmp);
            return(-1*CLERTC_MEM);
         }
         pcBuf=pcHlp; uiPos+=uiLen;
         uiLen=fread(pcBuf+uiPos,1,uiSiz-uiPos,pfTmp);
      }
      uiLen+=uiPos;
      pcBuf[uiLen]=0x00;
      fclose(pfTmp);
      // load the environment
      c=loadEnvars(uiLen,pcBuf,pfOut,pfErr,ppList);
      free(pcBuf);
   }
   return c;
}

/**********************************************************************/
