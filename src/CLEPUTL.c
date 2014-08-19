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
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "CLEPUTL.h"

#ifdef __WIN__
#define _WIN32_IE 0x5000
#include <shlobj.h>
#include <windows.h>
extern const char* CUSERID(void) { // TODO: not thread-safe
   static char uid[L_userid]="";
   static int  uidSet=FALSE;
   if (uidSet) return uid;
   DWORD       siz=sizeof(uid);
   GetUserName(uid,&siz);
   uidSet=TRUE;
   return(uid);
}
extern const char* HOMEDIR(int flg) { // TODO: not thread-safe
   static char dir[2][MAX_PATH+1]={{'\0'}, {'\0'}};
   static int homedirSet=FALSE;
   if (!homedirSet) {
      if (SHGetFolderPath(NULL,CSIDL_PROFILE,NULL,0,dir[0])==S_OK && strlen(dir[0])>0) {
         strcpy(dir[1], dir[0]);
         strcat(dir[0],"\\");
      }
      homedirSet=TRUE;
   }
   if (flg) return dir[0]; else return dir[1];
}
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
extern const char* CUSERID(void) { // TODO: not thread-safe
   static char uid[L_userid]="";
   static int uidSet=FALSE;
   if (uidSet) return uid;
   struct passwd* uP = getpwuid(geteuid());
   if (NULL != uP) {
      strncpy(uid,uP->pw_name,sizeof(uid)-1);
      uid[sizeof(uid)-1]=0x00;
/*
      if (strlen(uid)==0) {
         strncpy(uid,cuserid(NULL),sizeof(uid)-1);
         uid[sizeof(uid)-1]=0x00;
      }
*/
   }
   uidSet=TRUE;
   return(uid);
}
extern const char* HOMEDIR(int flg) { // TODO: not thread-safe
   static char dir[2][L_filnam]={{'\0'}, {'\0'}};
   static int  homedirSet=FALSE;
   if (!homedirSet) {
      size_t len;
      char* home=getenv("HOME");
      if (home!=NULL) {
         len=strlen(home);
         if (len>0 && len+1<sizeof(dir[0])) {
            strcpy(dir[0], home);
            strcpy(dir[1], home);
            strcat(dir[0],"/");
         }
      } else {
         struct passwd* uP = getpwuid(geteuid());
         if (uP != NULL && uP->pw_dir != NULL) {
            len=strlen(uP->pw_dir);
            if (len>0 && len+1<sizeof(dir[0])) {
               strcpy(dir[0], uP->pw_dir);
               strcpy(dir[1], uP->pw_dir);
         /*
               if (strlen(dir)==0) {
         #ifdef __ZUSS__
                  strcpy(dir,"/u/");
         #else
                  strcpy(dir,"/home/");
         #endif
                  strcat(dir,cuserid(NULL));
               }
         */
               strcat(dir[0],"/");
            }
         }
      }
      homedirSet=TRUE;
   }
   if (flg) return dir[0]; else return dir[1];
}
#endif

/* implementation of the external functions ***********************************/

extern U32 hex2bin(
   const C08*           hex,
         U08*           bin,
   const U32            len)
{
   U32                  i,j;
   U08                  h1,h2;

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

extern U32 chr2asc(
   const C08*           chr,
         C08*           asc,
   const U32            len)
{
   U32                  i;
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

extern U32 chr2ebc(
   const C08*           chr,
         C08*           asc,
   const U32            len)
{
   U32                  i;
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
   static const int freadLen=65536;

   if (filename==NULL || buf==NULL || bufsize==NULL || *bufsize<0)
      return -1; // bad args

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
            while (d==0 && *s1!=0 && *s2!=0 && i<n && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else {
            while (d==0 && *s1!=0 && *s2!=0 && i<n && *s1!=c && *s2!=c) {
               s1++; s2++; i++;
               d=*s1-*s2;
            }
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
            while (d==0 && *s1!=0 && *s2!=0 && i<n && isKyw(*s1) && isKyw(*s2)) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
            if (f && (!isKyw(*s1) || !isKyw(*s2))) return(0);
         } else {
            while (d==0 && *s1!=0 && *s2!=0 && i<n && tolower(*s1)!=tolower(c) && tolower(*s2)!=tolower(c)) {
               s1++; s2++; i++;
               d=tolower(*s1)-tolower(*s2);
            }
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
