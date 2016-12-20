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
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <locale.h>
#include <time.h>
#ifdef __UNIX__
#include <langinfo.h>
#endif
#ifdef __WIN__
#include <windows.h>
#endif

#include "CLEPUTL.h"

#ifdef __EBCDIC__

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
   va_list  argv;
   char*    help;
   char*    temp;
   size_t   size=65536;
   int      r=0;
   temp=(char*)malloc(size);
   if (temp==NULL) return(0);
   *temp='\0';
   va_start(argv, format);
   vsnprintf(temp,size,format,argv);
   va_end(argv);
   temp[size-1]='\0';
   if (*temp) {
      RPLDIAC(temp);
      r=fprintf(file,"%s",temp);
   }
   free(temp);
   return(r);
}

#endif

#ifdef __WIN__
#undef _WIN32_IE
#define _WIN32_IE 0x5000
#include <windows.h>
#include <shlobj.h>
extern char* userid(const int size, char* buffer) {
   DWORD tmp=size;
   buffer[0]=0x00;
   GetUserName(buffer,&tmp);
   return(buffer);
}
extern char* homedir(int flag, const int size, char* buffer) {
   char path[MAX_PATH+1]={0};
   buffer[0]=0x00;
   if (SHGetFolderPath(NULL,CSIDL_PROFILE,NULL,0,path)==S_OK) {
      if (flag) {
         snprintf(buffer,size,"%s%c",path,C_BSL);
      } else {
         snprintf(buffer,size,"%s",path);
      }
   }
   return(buffer);
}

extern int win_setenv(const char* name, const char* value)
{
   char* envstr;
   int rc;

   envstr = (char*) malloc(strlen(name) + strlen(value) + 2);
   if (NULL == envstr) {
       errno = ENOMEM;
       return -1;
   }
   sprintf(envstr,"%s=%s",name,value);
   rc = _putenv(envstr);
   free(envstr);
   return rc;
}

extern int win_unsetenv(const char* name)
{
  char* envstr;
  int rc;

  envstr = (char*) malloc(strlen(name) + 2);
  if (NULL == envstr) {
      errno = ENOMEM;
      return -1;
  }
  sprintf(envstr,"%s=",name);
  rc = _putenv(envstr);
  free(envstr);
  return rc;
}

#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
extern char* userid(const int size, char* buffer) {
   struct passwd* uP = getpwuid(geteuid());
   if (NULL != uP) {
      snprintf(buffer,size,"%s",uP->pw_name);
   } else {
      if (size>=0) buffer[0]=0x00;
   }
   return(buffer);
}
extern char* homedir(int flag, const int size, char* buffer) {
   const char*    home=GETENV("HOME");
   if (home!=NULL && *home) {
      if (flag) {
         snprintf(buffer,size,"%s/",home);
      } else {
         snprintf(buffer,size,"%s",home);
      }
   } else {
      struct passwd* uP = getpwuid(geteuid());
      if (uP != NULL && uP->pw_dir != NULL) {
         if (flag) {
            snprintf(buffer,size,"%s/",uP->pw_dir);
         } else {
            snprintf(buffer,size,"%s",uP->pw_dir);
         }
      } else {
         char acUsr[64]={0};
         userid(sizeof(acUsr),acUsr);
         if (acUsr[0]) {
#ifdef __ZOS__
         if (flag) {
            snprintf(buffer,size,"%s.",acUsr);
         } else {
            snprintf(buffer,size,"%s",acUsr);
         }
#elif defined(__USS__)
         if (flag) {
            snprintf(buffer,size,"/u/%s/",acUsr);
         } else {
            snprintf(buffer,size,"/u/%s",acUsr);
         }
#else
         if (flag) {
            snprintf(buffer,size,"/home/%s/",acUsr);
         } else {
            snprintf(buffer,size,"/home/%s",acUsr);
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
#endif

extern char* envid(const int size, char* buffer) {
   snprintf(buffer,size,"T");
   return(buffer);
}

extern char* safe_getenv(const char* name, char* buffer, size_t bufsiz) {
   char* env;
   if (buffer!=NULL) {
      env = GETENV(name);
      if(env!=NULL)
         snprintf(buffer, bufsiz, "%s", env);
      else
         return NULL;
   }
   return buffer;
}

/**********************************************************************/

#ifdef __EBCDIC__
#  define HSH_CHR "\x7B\x4A\x63\xB1\x69"
#  define ATS_CHR "\x7C\xB5\x80\xEC\x44\xAC"
#else
#  define HSH_CHR "#" /*nodiac*/
#  define ATS_CHR "@" /*nodiac*/
#endif

extern void fprintm(FILE* file,const char* own, const char* pgm, const char* man, const int cnt) {
   char*       hlp;
   char*       ptr;
   char        tmp[strlen(man)+4];
   switch (cnt) {
   case  0:snprintf(tmp,sizeof(tmp),"%s",man);
   case  1:snprintf(tmp,sizeof(tmp),"%s\n",man);
   default:snprintf(tmp,sizeof(tmp),"%s\n\n",man);
   }
   ptr=tmp;
   hlp=strstr(ptr,"&{"); /*nodiac*/;
   while (hlp!=NULL) {
      *hlp='\0';
      efprintf(file,"%s",ptr);
      if (strncmp(hlp+2,"OWN}",4)==0) { /*nodiac*/
         for (const char* p=own;*p;p++) fprintf(file,"%c",toupper(*p));
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"own}",4)==0) { /*nodiac*/
         for (const char* p=pgm;*p;p++) fprintf(file,"%c",tolower(*p));
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"Own}",4)==0) { /*nodiac*/
         fprintf(file,"%c",toupper(*own));
         for (const char* p=pgm+1;*p;p++) fprintf(file,"%c",tolower(*p));
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"oWn}",4)==0) { /*nodiac*/
         fprintf(file,"%s",own);
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"PGM}",4)==0) { /*nodiac*/
         for (const char* p=pgm;*p;p++) fprintf(file,"%c",toupper(*p));
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"pgm}",4)==0) { /*nodiac*/
         for (const char* p=pgm;*p;p++) fprintf(file,"%c",tolower(*p));
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"Pgm}",4)==0) { /*nodiac*/
         fprintf(file,"%c",toupper(*pgm));
         for (const char* p=pgm+1;*p;p++) fprintf(file,"%c",tolower(*p));
         ptr=hlp+6;
      } else if (strncmp(hlp+2,"pGm}",4)==0) { /*nodiac*/
         fprintf(file,"%s",pgm);
         ptr=hlp+6;
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

extern unsigned int sysccsid(void) {
   unsigned int ccsid = 0;
   const char* charset;
#ifdef __UNIX__

   // From man page:
   // On startup of the main program, the portable "C" locale is selected
   // as default.
   setlocale(LC_ALL, "");

   charset = nl_langinfo(CODESET);
   ccsid = mapcdstr(charset);
#elif defined(__WIN__)
   static CPINFOEX info;

   if (GetCPInfoEx(CP_ACP, 0, &info) && mapccsid(info.CodePage) != NULL) {
      // We got a supported CCSID => return it
      return info.CodePage;
   }
#endif

   if (ccsid == 0) {
      // fallback to LANG variable
      charset = mapl2c(' ' == 0x40);
      ccsid = mapcdstr(charset);

      if (ccsid == 0) {
         // fallback to platform default (aka. we don't know any better)
         if (' ' == 0x40)
            return 1047; // IBM-1047
         else
            return 367;  // US-ASCII
      }
   }

   return ccsid;
}

extern const char* mapl2c(unsigned isEBCDIC) {
   const char* pcPtr=NULL;
   const char* pcEnv=GETENV("LANG");
   static char acHlp[32];
   size_t      i;
   if (pcEnv!=NULL) {
      if ((isEBCDIC && '0'==0xF0) || (!isEBCDIC && '0'==0x30)) {
         pcPtr=strchr(pcEnv,'.');
         if (pcPtr!=NULL) {
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

   pcPtr=strpbrk(pcLang, ATS_CHR);
   if (pcPtr!=NULL) {
      if (((pcPtr-pcLang)+4 < uiLen) && tolower(pcPtr[1])=='e' && tolower(pcPtr[2])=='u' && tolower(pcPtr[3])=='r' && tolower(pcPtr[4])=='o') {
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
   int            o;
   if (p!=NULL) {
      while (1) {
         if (*p==0x00) {
            return(0);
         } else if (isspace(*p)) {
            p++;
         } else if (toupper(p[0])=='U' && toupper(p[1])=='T' && toupper(p[2])=='F') { /*UTF-xxxx*/
            p+=3; if (p[0]=='-' || p[0]=='_') p++;
            if (p[0]=='8' && (p[1]==0x00 || isspace(p[1]))) {
               return(1208);
            } else if (p[0]=='1' && p[1]=='6') {
               o=0;  p+=2;
            } else if (p[0]=='3' && p[1]=='2') {
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
            if (p[0]=='1' && p[1]=='6') {
               o=0;  p+=2;
            } else if (p[0]=='3' && p[1]=='2') {
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
            if (toupper(p[0]=='J') && toupper(p[1]=='P') && (p[2]==0x00 || isspace(p[2]))) {
               return(954);
            } else if (toupper(p[0]=='T') && toupper(p[1]=='W') && (p[2]==0x00 || isspace(p[2]))) {
               return(964);
            } else if (toupper(p[0]=='K') && toupper(p[1]=='R') && (p[2]==0x00 || isspace(p[2]))) {
               return(949);
            } else if (toupper(p[0]=='C') && toupper(p[1]=='N') && (p[2]==0x00 || isspace(p[2]))) {
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
   case 13496: return("UTF-8");
   case 13497: return("UTF-8");
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

static void rplchar(char* name,const size_t size,const char c, const char* value)
{
   char        h[size];
   char*       a=name;
   const char* v=value;
   char*       b;
   char*       p;
   int         catlen;

   for (b=strchr(a,c); b!=NULL ;b=strchr(a,c)) {
      if (b[1]==c) {
         for (p=b;*p;p++) p[0]=p[1];
         a=b+1;
      } else {
         b[0]=0;
         strncpy(h,b+1,size-1);
         h[size-1]=0;
         catlen = size - strlen(name) - 1;
         if (catlen > 0)
               strncat(a, v, catlen);
         else
               catlen = 0;
         if (strlen(v) >= catlen)
               name[size-1] = 0;
         catlen = size - strlen(name) - 1;
         if (catlen > 0)
               strncat(a, h, catlen);
         else
               catlen = 0;
         if (strlen(h) >= catlen)
               name[size-1] = 0;
         a=b+strlen(v);
      }
   }
}

static void rplenvar(char* name,const size_t size,const char opn, const char cls)
{
   int         lv;
   char        h[size];
   char        x[size];
   char*       a=name;
   char*       b;
   char*       c;
   char*       v;
   char*       p;
   int         match;
   int         catlen;

   for (b=strchr(a,opn); b!=NULL ;b=strchr(a,opn)) {
      if (b[1]==opn) {
         for (p=b;*p;p++) p[0]=p[1];
         a=b+1;
      } else {
         c=strchr(b,cls);
         if (c!=NULL) {
            b[0]=c[0]=0;
            strncpy(h,c+1,size-1);
            h[size-1]=0;
            match = 1;
            v=GETENV(b+1);
            if (v!=NULL && *v) {
               for (lv=strlen(v);lv>0 && isspace(v[lv-1]);lv--);
               lv = (lv < size) ? lv : size-1;
               memcpy(x, v, lv);
               x[lv] = 0;
               v = x;
            } else if (strcmp(b+1,"HOME")==0) {
               v=homedir(FALSE,size,x);
            } else if (strcmp(b+1,"USER")==0) {
               v=userid(size,x);
            } else if (strcmp(b+1,"SYSUID")==0) {
               v=userid(size,x);
            } else if (strcmp(b+1,"CUSER")==0) {
               v=userid(size,x);
               for(p=v; *p ;p++) *p = toupper(*p);
            } else if (strcmp(b+1,"cuser")==0) {
               v=userid(size,x);
               for(p=v; *p ;p++) *p = tolower(*p);
            } else if (strcmp(b+1,"Cuser")==0) {
               v=userid(size,x);
               if (*v) {
                  *v = toupper(*v);
                  for(p=v+1; *p ;p++) *p = tolower(*p);
               }
            } else if (strcmp(b+1,"OWNERID")==0) {
               v=userid(size,x);
            } else if (strcmp(b+1,"ENVID")==0) {
               v=envid(size,x);
            } else {
               b[0]=opn; c[0]=cls; a=c+1;
               match = 0;
            }
            if (match) {
               catlen = size - strlen(name) - 1;
               if (catlen > 0)
                  strncat(a, v, catlen);
               else
                  catlen = 0;
               if (strlen(v) >= catlen)
                  name[size-1] = 0;
               catlen = size - strlen(name) - 1;
               if (catlen > 0)
                  strncat(a, h, catlen);
               else
                  catlen = 0;
               if (strlen(h) >= catlen)
                  name[size-1] = 0;
               a=b+strlen(v);
            }
         } else {
            a=b+1;
         }
      }
   }
}

static char* rpltpl(char* string,int size,const char* templ,const char* values) {
   char*       s;
   char*       e;
   const char* t;
   for (s=string,e=string+(size-1),t=templ;t[0] && s<e;t++) {
      if (t[0]=='%' && t[1]=='%') {
         s[0]='%'; s++; t++;
      } else if ((t[0]=='%' && t[1])) {
         for (const char* v=values;v[0];v++) {
            if (v[0]==t[1] && v[1]==':') {
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

static const char* mapprefix(char* file, int size)
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
    } else if (file[0]=='/' && file[1]=='/' && file[2]=='\'') {
       for (p1=file,p2=file+3 ; *p2 && *p2!='\'' ; p1++,p2++) {
          *p1 = *p2;
       }
       *p1 = 0;
    } else if (file[0]=='/' && file[1]=='/') {
       file[0] = C_TLD;
       if (ISPATHNAME(file)) {
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
# elif __USS__
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

extern char* mapfil(char* file,int size)
{
   rplchar(file,size,C_TLD,mapprefix(file,size));
   rplenvar(file,size,'<','>');
   return(file);
}

#ifdef __ZOS__
extern char* cpmapfil(char* dest, int size,const char* source) {
   if (ISPATHNAME(source) || ISDDNAME(source) || source[0]=='\'' || source[0]==':') {
      snprintf(dest,size,"%s",source);
   } else {
      snprintf(dest,size,"'%s'",source);
   }
   return mapfil(dest,size);
}

extern char* filemode(const char* mode) {
   if(mode==NULL) return NULL;

   if(strcmp(mode,"r")==0){
      return "r, noseek, samethread, abend=recover";
   }
   if(strcmp(mode,"rb")==0){
      return "rb, noseek, samethread, abend=recover";
   }
   if(strcmp(mode,"rb+")==0){
      return "rb+, noseek, samethread, abend=recover";
   }
   if(strcmp(mode,"rs")==0){
      return "r, byteseek, samethread, abend=recover";
   }
   if(strcmp(mode,"rbs")==0){
      return "rb, byteseek, samethread, abend=recover";
   }
   if(strcmp(mode,"rbs+")==0){
      return "rb+, byteseek, samethread, abend=recover";
   }

   if(strcmp(mode,"w")==0){
      return "w, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"w+")==0){
      return "w+, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"wb")==0){
      return "wb, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"wb+")==0){
      return "wb+, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"ws")==0){
      return "w, byteseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"wbs")==0){
      return "wb, byteseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"wbs+")==0){
      return "wb+, byteseek, samethread, abend=recover, recfm=*";
   }

   if(strcmp(mode,"a")==0){
      return "a, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"ab")==0){
      return "ab, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"ab+")==0){
      return "ab+, noseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"as")==0){
      return "a, byteseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"abs")==0){
      return "ab, byteseek, samethread, abend=recover, recfm=*";
   }
   if(strcmp(mode,"abs+")==0){
      return "ab+, byteseek, samethread, abend=recover, recfm=*";
   }
   return NULL;
}
#else
extern char* cpmapfil(char* dest, int size,const char* source) {
   snprintf(dest,size,"%s",source);
   return mapfil(dest,size);
}

extern char* filemode(const char* mode) {
   if(mode==NULL) return NULL;

   if(strcmp(mode,"r")==0 || strcmp(mode,"rs")==0){
      return "r";
   }
   if(strcmp(mode,"rb")==0 || strcmp(mode,"rbs")==0){
      return "rb";
   }
   if(strcmp(mode,"rb+")==0 || strcmp(mode,"rbs+")==0){
      return "rb+";
   }
   if(strcmp(mode,"w")==0 || strcmp(mode,"ws")==0){
      return "w";
   }
   if(strcmp(mode,"w+")==0 || strcmp(mode,"ws+")==0){
      return "w+";
   }
   if(strcmp(mode,"wb")==0 || strcmp(mode,"wbs")==0){
      return "wb";
   }
   if(strcmp(mode,"wb+")==0 || strcmp(mode,"wbs+")==0){
      return "wb+";
   }
   if(strcmp(mode,"a")==0 || strcmp(mode,"as")==0){
      return "a";
   }
   if(strcmp(mode,"ab")==0 || strcmp(mode,"abs")==0){
      return "ab";
   }
   if(strcmp(mode,"ab+")==0 || strcmp(mode,"abs+")==0){
      return "ab+";
   }
   return NULL;
}
#endif

extern char* maplab(char* label,int size, int toUpper) {
   rplchar(label,size,C_EXC,"<ENVID>");
   rplchar(label,size,C_TLD,"<SYSUID>");
   rplchar(label,size,C_CRT,"<OWNERID>");
   rplenvar(label,size,'<','>');
   if(toUpper){
      for(int i=0,l=strlen(label);i<l;i++){
         label[i] = toupper(label[i]);
      }
   }
   return(label);
}

extern char* cpmaplab(char* label, int size,const char* templ, const char* values, int toUpper) {
   rpltpl(label,size,templ,values);
   maplab(label,size,toUpper);
   return(label);
}

/* implementation of the external functions ***********************************/

extern char* strxcpy(char *dest, const char *src, size_t n)
{
   size_t i;
   for (i=0;i<(n-1) && src[i];i++) dest[i]=src[i];
   dest[i] = '\0';
   return dest;
}

extern int printd(const char* format,...)
{
#ifdef __DEBUG__
   int r;
   va_list  argv;
   va_start(argv, format);
   r=vfprintf (stderr, format, argv );
   va_end(argv);
   return r;
#else
   return 0;
#endif
}

extern int snprintc(char* buffer,size_t size,const char* format,...)
{
   va_list  argv;
   int      r, h = strlen(buffer);
   if (size > (h+1)) {
      va_start(argv, format);
      r = vsnprintf(buffer+h, size-h, format, argv);
      va_end(argv);
      buffer[size-1]=0;
      return(h+r);
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
   if ((*size)<s) {
      char* b=(char*)realloc(*buffer,2*s);
      if (b==NULL) return(0);
      (*buffer)=b;
      (*size)=2*s;
   }
   va_start(argv, format);
   r = vsnprintf((*buffer)+h, (*size)-h, format, argv);
   va_end(argv);
   *buffer[(*size)-1]=0;
   return(h+r);
}

extern int srprintf(char** buffer,size_t* size,const size_t expansion,const char* format,...)
{
   va_list  argv;
   int      r;
   size_t   s=strlen(format)+expansion+1;
   if ((*size)<s) {
      char* b=(char*)realloc(*buffer,2*s);
      if (b==NULL) return(0);
      (*buffer)=b;
      (*size)=2*s;
   }
   va_start(argv, format);
   r = vsnprintf((*buffer), (*size), format, argv);
   va_end(argv);
   *buffer[(*size)-1]=0;
   return(r);
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
            }
            return(i);
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
            }
            return(i);
      }
   }
   return(i);
}

extern int file2str(const char* filename, char** buf, int* bufsize, const char* format) {
   int siLen=0, siHlp;
   char* pcHlp;
   FILE* pfFile=NULL;
   const int freadLen=65536;

   if (filename==NULL || buf==NULL || bufsize==NULL || *bufsize<0)
      return -1; // bad args
   if (*buf==NULL)
      *bufsize=0;

   errno=0;
   if (format!=NULL && format[0]=='r') {
      pfFile=fopen(filename, format);
   } else {
      pfFile=fopen(filename, "r");
   }
   if (pfFile == NULL) {
      return -2; // fopen failed
   }

   while (!ferror(pfFile) && !feof(pfFile)) {
      if (*bufsize-siLen<freadLen+1) {
         if (*bufsize>INT_MAX-(freadLen*2+1)) {
            fclose(pfFile);
            return -3; // integer overflow
         }
         siHlp=*bufsize+freadLen*2+1;
         pcHlp=(char*)realloc(*buf, siHlp);
         if (pcHlp==NULL) {
            fclose(pfFile);
            return -4; // realloc failed
         }
         *bufsize=siHlp;
         *buf=pcHlp;
      }
      siLen+=fread(*buf+siLen, 1, freadLen, pfFile);
   }
   if (ferror(pfFile)) {
      fclose(pfFile);
      return -5; // read error
   }

   fclose(pfFile);
   if (*buf!=NULL) // empty file
      (*buf)[siLen]='\0';
   return siLen;
}

extern int arry2str(char* array[], const int count, const char* separ, const int separLen, char** out, int* outlen) {
   size_t uiSumLen=((count-1)*separLen)+1;
   size_t uiLens[count];
   char*  pcHlp;
   char*  pcOut;

   if (array==NULL || out==NULL || outlen==NULL || (separLen>0 && separ==NULL))
      return -1; // bad args
   if (*out==NULL)
      *outlen=0;

   size_t i;
   for (i=0; i<count; i++) {
      uiLens[i]=strlen(array[i]);
      uiSumLen+=uiLens[i];
   }

   if (*outlen<uiSumLen || *out==NULL) {
      if (uiSumLen < 1)
         uiSumLen = 4;
      pcHlp=(char*)realloc(*out, uiSumLen);
      if (pcHlp==NULL) {
         return -2; // realloc failed
      }
      *outlen=uiSumLen;
      *out=pcHlp;
   }


   pcOut=*out;
   if (count>0) {
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
   static char    acBuf[20];
   char*          pcStr=(p!=NULL)?p:acBuf;
   time_t         h=(t)?(time_t)t:time(NULL);
   strftime(pcStr,sizeof(acBuf),"%Y-%m-%d %H:%M:%S",localtime(&h));
   return(pcStr);
}

/**********************************************************************/
