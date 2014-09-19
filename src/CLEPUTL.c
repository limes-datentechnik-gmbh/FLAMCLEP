/**
 * @file
 * @brief  Implementierung diverser Hilffunktionen in ANSI C
 * @author limes datentechnik gmbh
 * @date  03.01.2012
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
#define _UNIX03_WITHDRAWN
#define _POSIX_SOURCE
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "CLEPUTL.h"

#ifdef __WIN__
#define _WIN32_IE 0x5000
#include <shlobj.h>
#include <windows.h>
extern char* userid(const int size, char* buffer) {
   unsigned int tmp=size;
   buffer[0]=0x00;
   GetUserName(buffer,&tmp);
   return(buffer);
}
extern char* homedir(int flag, const int size, char* buffer) {
   char path[MAX_PATH+1]="";
   buffer[0]=0x00;
   SHGetFolderPath(NULL,CSIDL_PROFILE,NULL,0,path);
   if (flag) {
      snprintf(buffer,size,"%s\\",path);
   } else {
      snprintf(buffer,size,"%s",path);
   }
   return(buffer);
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
   const char*    home=getenv("HOME");
   if (home!=NULL && strlen(home)) {
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
         if (flag) {
            snprintf(buffer,size,"/");
         } else {
            if (size>=0) buffer[0]=0x00;
         }
      }
   }
   return(buffer);
}
#endif

/**********************************************************************/

extern void rplchar(char* name,const size_t size,const char c, const char* value)
{
   char        h[size];
   char*       a=name;
   char*       e=a+strlen(name);
   const char* v=value;
   char*       b;
   char*       p;

   for (b=strchr(a,c);a<e && b!=NULL;b=strchr(a,c)) {
      if (b[1]==c) {
         for (p=b;*p;p++) p[0]=p[1];
         a=b+1;
      } else {
         b[0]=0;
         strncpy(h,b+1,size-1);
         h[size-1]=0;
         if (strlen(a)+strlen(v)<size) strcat(a,v);
         if (strlen(a)+strlen(h)<size) strcat(a,h);
         a=b+strlen(v);
      }
      e=a+strlen(name);
   }
}

extern void rplenvar(char* name,const size_t size,const char opn, const char cls)
{
   char        h[size];
   char        x[size];
   char*       a=name;
   char*       e=a+strlen(name);
   char*       b;
   char*       c;
   char*       v;
   char*       p;

   for (b=strchr(a,opn);a<e && b!=NULL;b=strchr(a,opn)) {
      if (b[1]==opn) {
         for (p=b;*p;p++) p[0]=p[1];
         a=b+1;
      } else {
         c=strchr(b,cls);
         if (c!=NULL) {
            b[0]=c[0]=0;
            strncpy(h,c+1,size-1);
            h[size-1]=0;
            v=getenv(b+1);
            if (v!=NULL && strlen(v)) {
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else if (strcmp(b+1,"HOME")==0) {
               v=homedir(FALSE,size,x);
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else if (strcmp(b+1,"USER")==0) {
               v=userid(size,x);
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else if (strcmp(b+1,"SYSUID")==0) {
               v=userid(size,x);
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else if (strcmp(b+1,"CUSER")==0) {
               v=userid(size,x);
               for(p=v; *p ;p++) *p = toupper(*p);
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else if (strcmp(b+1,"cuser")==0) {
               v=userid(size,x);
               for(p=v; *p ;p++) *p = tolower(*p);
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else if (strcmp(b+1,"Cuser")==0) {
               v=userid(size,x);
               if (*v) {
                  *v = toupper(*v);
                  for(p=v+1; *p ;p++) *p = tolower(*p);
               }
               if (strlen(a)+strlen(v)<size) strcat(a,v);
               if (strlen(a)+strlen(h)<size) strcat(a,h);
               a=b+strlen(v);
            } else {
               strcat(a,h);
               a=b;
            }
         } else {
            a=b+1;
         }
      }
      e=a+strlen(name);
   }
}

extern char* mapfil(char* file,int size) {
   rplchar(file,size,'+',"<HOME>");
   rplenvar(file,size,'<','>');
   return(file);
}

#ifdef __HOST__
extern char* cpmapfil(char* dest, int size,const char* source) {
   if (ISPATHNAME(source)) {
      snprintf(dest,size,"%s",source);
      mapfil(dest,size);
      return("");
   } else if (ISDDNAME(fn2))
      snprintf(dest,size,"%s",source);
      mapfil(dest,size);
      return(", recfm=*");
   } else {
      snprintf(dest,size,"'%s'",source);
      mapfil(dest,size);
      return("");
   }
}
#else
extern char* cpmapfil(char* dest, int size,const char* source) {
   snprintf(dest,size,"%s",source);
   mapfil(dest,size);
   return("");
}
#endif

/* implementation of the external functions ***********************************/

extern int snprintc(char* buffer,size_t size,const char* format,...) {
   va_list  argv;
   int      r,h=strlen(buffer);
   if (size>(h+1)) {
      va_start(argv,format); r=vsnprintf(buffer+h,size-(h+1),format,argv); va_end(argv);
      return(r);
   } else return (0);
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

         case ' ' : asc[i]=0x20; break;
         case '!' : asc[i]=0x21; break;
         case '\"': asc[i]=0x22; break;
         case '#' : asc[i]=0x23; break;
         case '$' : asc[i]=0x24; break;
         case '%' : asc[i]=0x25; break;
         case '&' : asc[i]=0x26; break;
         case '\'': asc[i]=0x27; break;
         case '(' : asc[i]=0x28; break;
         case ')' : asc[i]=0x29; break;
         case '*' : asc[i]=0x2A; break;
         case '+' : asc[i]=0x2B; break;
         case ',' : asc[i]=0x2C; break;
         case '-' : asc[i]=0x2D; break;
         case '.' : asc[i]=0x2E; break;
         case '/' : asc[i]=0x2F; break;

         case '0' : asc[i]=0x30; break;
         case '1' : asc[i]=0x31; break;
         case '2' : asc[i]=0x32; break;
         case '3' : asc[i]=0x33; break;
         case '4' : asc[i]=0x34; break;
         case '5' : asc[i]=0x35; break;
         case '6' : asc[i]=0x36; break;
         case '7' : asc[i]=0x37; break;
         case '8' : asc[i]=0x38; break;
         case '9' : asc[i]=0x39; break;
         case ':' : asc[i]=0x3A; break;
         case ';' : asc[i]=0x3B; break;
         case '<' : asc[i]=0x3C; break;
         case '=' : asc[i]=0x3D; break;
         case '>' : asc[i]=0x3E; break;
         case '?' : asc[i]=0x3F; break;

         case '@' : asc[i]=0x40; break;
         case 'A' : asc[i]=0x41; break;
         case 'B' : asc[i]=0x42; break;
         case 'C' : asc[i]=0x43; break;
         case 'D' : asc[i]=0x44; break;
         case 'E' : asc[i]=0x45; break;
         case 'F' : asc[i]=0x46; break;
         case 'G' : asc[i]=0x47; break;
         case 'H' : asc[i]=0x48; break;
         case 'I' : asc[i]=0x49; break;
         case 'J' : asc[i]=0x4A; break;
         case 'K' : asc[i]=0x4B; break;
         case 'L' : asc[i]=0x4C; break;
         case 'M' : asc[i]=0x4D; break;
         case 'N' : asc[i]=0x4E; break;
         case 'O' : asc[i]=0x4F; break;

         case 'P' : asc[i]=0x50; break;
         case 'Q' : asc[i]=0x51; break;
         case 'R' : asc[i]=0x52; break;
         case 'S' : asc[i]=0x53; break;
         case 'T' : asc[i]=0x54; break;
         case 'U' : asc[i]=0x55; break;
         case 'V' : asc[i]=0x56; break;
         case 'W' : asc[i]=0x57; break;
         case 'X' : asc[i]=0x58; break;
         case 'Y' : asc[i]=0x59; break;
         case 'Z' : asc[i]=0x5A; break;
         case '[' : asc[i]=0x5B; break;
         case '\\': asc[i]=0x5C; break;
         case ']' : asc[i]=0x5D; break;
         case '^' : asc[i]=0x5E; break;
         case '_' : asc[i]=0x5F; break;

         case '`' : asc[i]=0x60; break;
         case 'a' : asc[i]=0x61; break;
         case 'b' : asc[i]=0x62; break;
         case 'c' : asc[i]=0x63; break;
         case 'd' : asc[i]=0x64; break;
         case 'e' : asc[i]=0x65; break;
         case 'f' : asc[i]=0x66; break;
         case 'g' : asc[i]=0x67; break;
         case 'h' : asc[i]=0x68; break;
         case 'i' : asc[i]=0x69; break;
         case 'j' : asc[i]=0x6A; break;
         case 'k' : asc[i]=0x6B; break;
         case 'l' : asc[i]=0x6C; break;
         case 'm' : asc[i]=0x6D; break;
         case 'n' : asc[i]=0x6E; break;
         case 'o' : asc[i]=0x6F; break;

         case 'p' : asc[i]=0x70; break;
         case 'q' : asc[i]=0x71; break;
         case 'r' : asc[i]=0x72; break;
         case 's' : asc[i]=0x73; break;
         case 't' : asc[i]=0x74; break;
         case 'u' : asc[i]=0x75; break;
         case 'v' : asc[i]=0x76; break;
         case 'w' : asc[i]=0x77; break;
         case 'x' : asc[i]=0x78; break;
         case 'y' : asc[i]=0x79; break;
         case 'z' : asc[i]=0x7A; break;
         case '{' : asc[i]=0x7B; break;
         case '|' : asc[i]=0x7C; break;
         case '}' : asc[i]=0x7D; break;
         case '~' : asc[i]=0x7E; break;
         default  : asc[i]=0x00; return(i);
      }
   }
   return(i);
}

extern unsigned int chr2ebc(
   const char*          chr,
         char*          asc,
   const unsigned int   len)
{
   unsigned int         i;
   for (i=0;i<len;i++) {
      switch (chr[i]) {
         case '\n': asc[i]=0x25; break;
         case '\r': asc[i]=0x0D; break;

         case ' ' : asc[i]=0x40; break;
         case '!' : asc[i]=0x5A; break;
         case '\"': asc[i]=0x7F; break;
         case '#' : asc[i]=0x7B; break;
         case '$' : asc[i]=0x5B; break;
         case '%' : asc[i]=0x6C; break;
         case '&' : asc[i]=0x50; break;
         case '\'': asc[i]=0x7D; break;
         case '(' : asc[i]=0x4D; break;
         case ')' : asc[i]=0x5D; break;
         case '*' : asc[i]=0x5C; break;
         case '+' : asc[i]=0x4E; break;
         case ',' : asc[i]=0x6B; break;
         case '-' : asc[i]=0x60; break;
         case '.' : asc[i]=0x4B; break;
         case '/' : asc[i]=0x61; break;

         case '0' : asc[i]=0xF0; break;
         case '1' : asc[i]=0xF1; break;
         case '2' : asc[i]=0xF2; break;
         case '3' : asc[i]=0xF3; break;
         case '4' : asc[i]=0xF4; break;
         case '5' : asc[i]=0xF5; break;
         case '6' : asc[i]=0xF6; break;
         case '7' : asc[i]=0xF7; break;
         case '8' : asc[i]=0xF8; break;
         case '9' : asc[i]=0xF9; break;
         case ':' : asc[i]=0x7A; break;
         case ';' : asc[i]=0x5E; break;
         case '<' : asc[i]=0x4C; break;
         case '=' : asc[i]=0x7E; break;
         case '>' : asc[i]=0x6E; break;
         case '?' : asc[i]=0x6F; break;

         case '@' : asc[i]=0x7C; break;
         case 'A' : asc[i]=0xC1; break;
         case 'B' : asc[i]=0xC2; break;
         case 'C' : asc[i]=0xC3; break;
         case 'D' : asc[i]=0xC4; break;
         case 'E' : asc[i]=0xC5; break;
         case 'F' : asc[i]=0xC6; break;
         case 'G' : asc[i]=0xC7; break;
         case 'H' : asc[i]=0xC8; break;
         case 'I' : asc[i]=0xC9; break;
         case 'J' : asc[i]=0xD1; break;
         case 'K' : asc[i]=0xD2; break;
         case 'L' : asc[i]=0xD3; break;
         case 'M' : asc[i]=0xD4; break;
         case 'N' : asc[i]=0xD5; break;
         case 'O' : asc[i]=0xD6; break;

         case 'P' : asc[i]=0xD7; break;
         case 'Q' : asc[i]=0xD8; break;
         case 'R' : asc[i]=0xD9; break;
         case 'S' : asc[i]=0xE2; break;
         case 'T' : asc[i]=0xE3; break;
         case 'U' : asc[i]=0xE4; break;
         case 'V' : asc[i]=0xE5; break;
         case 'W' : asc[i]=0xE6; break;
         case 'X' : asc[i]=0xE7; break;
         case 'Y' : asc[i]=0xE8; break;
         case 'Z' : asc[i]=0xE9; break;
         case '[' : asc[i]=0x00; break;
         case '\\': asc[i]=0xE0; break;
         case ']' : asc[i]=0x00; break;
         case '^' : asc[i]=0x5F; break;
         case '_' : asc[i]=0x6D; break;

         case '`' : asc[i]=0x79; break;
         case 'a' : asc[i]=0x81; break;
         case 'b' : asc[i]=0x82; break;
         case 'c' : asc[i]=0x83; break;
         case 'd' : asc[i]=0x84; break;
         case 'e' : asc[i]=0x85; break;
         case 'f' : asc[i]=0x86; break;
         case 'g' : asc[i]=0x87; break;
         case 'h' : asc[i]=0x88; break;
         case 'i' : asc[i]=0x89; break;
         case 'j' : asc[i]=0x91; break;
         case 'k' : asc[i]=0x92; break;
         case 'l' : asc[i]=0x93; break;
         case 'm' : asc[i]=0x94; break;
         case 'n' : asc[i]=0x95; break;
         case 'o' : asc[i]=0x96; break;

         case 'p' : asc[i]=0x97; break;
         case 'q' : asc[i]=0x98; break;
         case 'r' : asc[i]=0x99; break;
         case 's' : asc[i]=0xA2; break;
         case 't' : asc[i]=0xA3; break;
         case 'u' : asc[i]=0xA4; break;
         case 'v' : asc[i]=0xA5; break;
         case 'w' : asc[i]=0xA6; break;
         case 'x' : asc[i]=0xA7; break;
         case 'y' : asc[i]=0xA8; break;
         case 'z' : asc[i]=0xA9; break;
         case '{' : asc[i]=0xC0; break;
         case '|' : asc[i]=0x4F; break;
         case '}' : asc[i]=0xD0; break;
         case '~' : asc[i]=0xA1; break;

         default  : asc[i]=0x00; return(i);
      }
   }
   return(i);
}

extern int file2str(const char* filename, char** buf, int* bufsize) {
   int siLen=0, siHlp;
   char* pcHlp;
   FILE* pfFile=NULL;
   const int freadLen=65536;

   if (filename==NULL || buf==NULL || bufsize==NULL || *bufsize<0)
      return -1; // bad args
   if (*buf==NULL)
      *bufsize=0;

   errno=0;
   pfFile=fopen(filename, "r");
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

   if (*outlen<uiSumLen) {
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
         int i=1;
         if (c==-1) {
            while (d==0 && *s1!=0 && *s2!=0 && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
            if (i<n) return(n-i);
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else if (c==0) {
            while (d==0 && *s1!=0 && *s2!=0 && i<n) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
         } else {
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
         int i=1;
         if (c==-1) {
            while (d==0 && *s1!=0 && *s2!=0 && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
            if (i<n) return(n-i);
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else if (c==0) {
            while (d==0 && *s1!=0 && *s2!=0 && i<n) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
         } else {
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

/**********************************************************************/
