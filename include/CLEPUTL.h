/**
 * @file
 * @brief  Declaration of utility functions for CLP/CLE in ANSI C
 * @author limes datentechnik gmbh
 * @date   06.03.2015
 * @copyright limes datentechnik gmbh
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
 **********************************************************************
 *
 * This interface provides utility function for CLE and CLP. It should
 * static linked to CLP and/or CLE.
 *
 **********************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLEPUTL_H
#define INC_CLEPUTL_H

#include <stdio.h>
#include <stdlib.h>

#ifndef TRUE
#  define TRUE             (1)
#endif

#ifndef FALSE
#  define FALSE            (0)
#endif

#ifndef EOS
   #define EOS             (0x00)
#endif

#ifndef __PRINTF_CHECK__
#  define __PRINTF_CHECK__(A,B)
#endif

#define CLEP_DEFAULT_CCSID_ASCII     819  // "ISO8859-1" Latin-1
#define CLEP_DEFAULT_CCSID_EBCDIC    1047 // "IBM-1047"  Open Systems Latin-1

/** Free memory space */
#define SAFE_FREE(x) do { if ((x) != NULL) {free((void*)(x)); (x)=NULL;} } while(0)

typedef struct EnVarList {
   char*                pcName;
   char*                pcValue;
   struct EnVarList*    psNext;
}TsEnVarList;

#ifdef __WIN__
#  ifndef __FL5__
#     ifndef localtime_r
#        define localtime_r(t,s)   localtime(t)
#     endif
#     ifndef gmtime_r
#        define gmtime_r(t,s)      gmtime(t)
#     endif
#     ifndef asctime_r
#        define asctime_r(s,b)     asctime(s)
#     endif
#     ifndef ctime_r
#        define ctime_r(t,b)       ctime(t)
#     endif
#  endif
extern int win_setenv(const char* name, const char* value);
extern int win_unsetenv(const char* name);
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) win_setenv((name), (value))
#  define UNSETENV(name)      win_unsetenv((name))
#elif defined (__ZOS__) || defined (__ZOS__)
#  define GETENV(name)        __getenv((name))
#  define SETENV(name, value) setenv((name), (value), 1)
#  define UNSETENV(name)      unsetenv((name))
#else
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) setenv((name), (value), 1)
#  define UNSETENV(name)      unsetenv((name))
#endif

#define isStr(c) (isprint(c) || (c)==C_TLD  || (c)==C_DLR || (c)==C_ATS || (c)==C_BSL || (c)==C_CRT || (c)==C_EXC)
#define isKyw(c) (isalnum(c) || (c)=='_')
#define isCon(c) (isKyw(c)   || (c)=='-' || (c)=='/')

#define ISDDNAME(p)     (strlen(p)>3 && toupper((p)[0])=='D' && toupper((p)[1])=='D' && (p)[2]==':')
#define ISPATHNAME(p)   (strchr((p),'/')!=NULL)
#define ISDSNAME(p)     (strlen(p)>2 && toupper((p)[0])=='/' && toupper((p)[1])=='/')
#define ISGDGMBR(m)     ((m)[0]=='0' || (m)[0]=='+' || (m)[0]=='-')
#define ISDDN(c)        (isalnum(c) || (c)==C_DLR || (c)==C_HSH || (c)==C_ATS)

extern FILE* fopen_hfq(const char* name, const char* mode);
extern FILE* fopen_hfq_nowarn(const char* name, const char* mode);
extern FILE* freopen_hfq(const char* name, const char* mode, FILE* stream);
#ifdef __ZOS__
   extern FILE* fopen_tmp(void);
   extern int   fclose_tmp(FILE* fp);
   extern int   remove_hfq(const char* name);
#else
#  define fopen_tmp()            tmpfile()
#  define fclose_tmp(fp)         fclose((fp))
#  define remove_hfq(n)          remove(n)
#endif

/* Definition of return/condition/exit codes **************************/

/** 0  - command line, command syntax, mapping, execution and finish of the command was successful*/
#define CLERTC_OK             0

/** 1  - command line, command syntax, mapping, execution and finish of the command was successful but a warning can be found in the log*/
#define CLERTC_INF            1

/** 2  - command line, command syntax, mapping, execution was successful but cleanup of the command failed (may not happened)*/
#define CLERTC_FIN            2

/** 4  - command line, command syntax and mapping was successful but execution of the command returns with a warning*/
#define CLERTC_WRN            4

/** 8  - command line, command syntax and mapping was successful but execution of the command returns with an error*/
#define CLERTC_RUN            8

/** 12 - command line and command syntax was OK but mapping failed*/
#define CLERTC_MAP            12

/** 16 - command line was OK but command syntax was wrong*/
#define CLERTC_SYN            16

/** 20 - command line was wrong (user error)*/
#define CLERTC_CMD            20

/** 24 - initialization of parameter structure for the command failed (may not happened)*/
#define CLERTC_INI            24

/** 28 - configuration is wrong (user error)*/
#define CLERTC_CFG            28

/** 32 - table error (something within the predefined tables is wrong)*/
#define CLERTC_TAB            32

/** 36 - system error (mainly memory allocation or some thing like this failed)*/
#define CLERTC_SYS            36

/** 40 - access control or license error*/
#define CLERTC_ACS            40

/** 44 - interface error (parameter pointer equals to NULL or something like this)*/
#define CLERTC_ITF            44

/** 48 - memory allocation failed (e.g. dynamic string handling)*/
#define CLERTC_MEM            48

/** 64 - fatal error (basic things are damaged)*/
#define CLERTC_FAT            64

/** maximal condition code value (greater condition codes are special return codes)*/
#define CLERTC_MAX            64

/**********************************************************************/

/**
 * Returns the current user id.
 * @param size    size of the buffer
 * @param buffer  pointer to the buffer
 * @return        pointer to the buffer containing the current user id (null-terminated)
 */
extern char* userid(const int size, char* buffer);

/**
 * Returns the current home directory.
 * @param flag    if true then slash/backslash are added
 * @param size    size of the string buffer
 * @param buffer  pointer to the buffer
 * @return        pointer to the buffer containing the current home directory (null-terminated)
 */
extern char* homedir(const int flag, const int size, char* buffer);

/**
 * Returns the current user id.
 * @return        pointer to the buffer containing the current user id (null-terminated) must be freed by the caller
 */
extern char* duserid(void);

/**
 * Returns the current home directory.
 * @param flag    if true then slash/backslash are added
 * @return        pointer to the buffer containing the current home directory (null-terminated) must be freed by the caller
 */
extern char* dhomedir(const int flag);

/**
 * Gets an environment variable and stores it in the provided buffer. If
 * the buffer is not large enough, the variable value is truncated.
 * @param name Name of the environment variable
 * @param buffer Pointer to the buffer for the variable value
 * @param bufsiz Size of the buffer
 * @return If bufsiz > 0, returns the buffer pointer which contains a null-terminated string
 *         or NULL (variable does not exist). If bufsiz == 0, buffer is returned unmodified.
 */
extern char* safe_getenv(const char* name, char* buffer, size_t bufsiz);

/**
 * Un-escape a string as part of special character support in EBCDIC codepages (static version).
 * @param input     pointer to the input string containing the escape sequences
 * @param output    pointer to the output string for un-escaping (could be equal to the input pointer)
 * @return          pointer to the un-escaped output or NULL if error
 */
extern char* unEscape(const char* input, char* output);

/**
 * Un-escape a string as part of special character support in EBCDIC codepages (dynamic version).
 * @param input     pointer to the input string containing the escape sequences
 * @return          pointer to the un-escaped output or NULL if error (calling application must free the memory)
 */
extern char* dynUnEscape(const char* input);

/**
 * Works like printf but print only in debug mode.
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int printd(const char* format,...) __PRINTF_CHECK__(1, 2);
/**
 * Works like snprintf but concatenates the format string to the buffer.
 * @param buffer  pointer to the string buffer
 * @param size    size of the string buffer
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int snprintc(char* buffer,const size_t size,const char* format,...) __PRINTF_CHECK__(3, 4);

/**
 * Works like snprintf but does reallocation of the buffer (maximal expansion of the format string can be specified).
 * @param buffer    pointer to pointer to the string buffer (is updated, could be NULL at beginning)
 * @param size      pointer to size of the string buffer (is updated, could be 0 at beginning)
 * @param expansion maximal expected expansion of the format string (size must be fit strlen(*buffer)+strlen(format)+expansion+1)
 * @param format    format string
 * @return          amount of characters printed (0 are mainly a error)
 */
extern int srprintc(char** buffer,size_t* size,const size_t expansion,const char* format,...) __PRINTF_CHECK__(4, 5);

/**
 * Works like snprintc but does reallocation of the buffer (maximal expansion of the format string can be specified).
 * @param buffer    pointer to pointer to the string buffer (is updated, could be NULL at beginning)
 * @param size      pointer to size of the string buffer (is updated, could be 0 at beginning)
 * @param expansion maximal expected expansion of the format string (size must be fit strlen(format)+expansion+1)
 * @param format    format string
 * @return          amount of characters printed (0 are mainly a error)
 */
extern int srprintf(char** buffer,size_t* size,const size_t expansion,const char* format,...) __PRINTF_CHECK__(4, 5);

/**
 * Prints man pages to a file, inserting owner and program name into placeholders
 * @param file    pointer to the file
 * @param own     owner name for replacement (&{OWN})
 * @param pgm     program name for replacement (&{PGM})
 * @param man     manpage to print, which can contain &{PGM} and &{OWN}
 * @param cnt     amount of '\n' added to man page (0,1,2 (>2=2))
 */
extern void fprintm(FILE* file,const char* own, const char* pgm, const char* man, const int cnt);

/**
 * This function parses a zero terminated string array of a certain length or terminated with 0xFF.
 * Such a string array is the result of a variable length array of strings provided by CLP.
 * If you don't know the length, please provide a negative number to look for 0xFF termination.
 * For 0xFF termination you must define the CLPFLG_DLM. This is useful if there is no capability
 * to use the ELN link to determine the length of the string array. Each call returns the pointer to
 * the next string in the array, if no string is found anymore NULL is returned.
 * @param hdl     pointer of pointer to string initialized with NULL at beginning
 * @param str     pointer to the string arrays from CLP
 * @param len     -1 for 0xFF delimiter parsing or the ELN of the string array
 * @return        pointer to one string or NULL if no more string
 */
extern const char* prsdstr(const char** hdl, const char* str, int len);

#ifndef __WIN__
/* Force the use of strlcpy instead of strncpy, but not on Windows because it causes trouble with MinGW headers */
#ifdef strncpy
#undef strncpy
#endif
#define strncpy(...) Error: Do not use strncpy! Use strlcpy instead!
#endif

/**
 * Works like strncpy but ensures null-termination.
 * @param dest    pointer to destination string
 * @param src     pointer to source string
 * @param n       size of memory available for buffer
 * @return        number of bytes actually copied (excludes NUL-termination)
 */
extern size_t strlcpy(char *dest, const char *src, size_t n);

/**
 * Get environment variable and handle HOME, USER, CUSEr, Cuser, cuser, OWNER, ENVID if not defined
 * @param name environment variable name
 * @param length optional length of the name if no zero termination (0 if zero termination)
 * @param size size of string
 * @param string containing the value for the corresponding environment variable
 * @return pointer to string
 */
extern char* getenvar(const char* name,const size_t length,const size_t size,char* string);

/**
 * Replace all environment variables enclosed with '<' and '>' to build a string
 * @param string string for replacement
 * @param size size of replacement string
 * @return pointer to string
 */
extern char* mapstr(char* string,int size);

/**
 * Replace all environment variables enclosed with '<' and '>' to build a dynamic string
 * @param string string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapstr(const char* string,int method);

/**
 * Replace all environment variables enclosed with '(' and ')' to build a dynamic string
 * @param string string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapxml(const char* string,int method);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>' to build a file name
 * @param file string for replacement
 * @param size size of replacement string
 * @return pointer to file
 */
extern char* mapfil(char* file,int size);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>' to build a dynamic file name
 * @param file string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapfil(const char* file, int method);

/**
 * Replace '!' with ENVID, '~' with "<SYSUID>", '^' with "<OWNERID>" and all environment variables enclosed with '<' and '> to build a key label'
 * @param label string for replacement
 * @param size size of replacement string
 * @param toUpper for mapping file to upper
 * @return pointer to label
 */
extern char* maplab(char* label,int size,int toUpper);

/**
 * Replace '!' with ENVID, '~' with "<SYSUID>", '^' with "<OWNERID>" and all environment variables enclosed with '<' and '> to build a dynamic key label'
 * @param label string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to new allocated string or NULL if error
 */
extern char* dmaplab(const char* label, int method);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>'
 * @param dest string for replacement
 * @param size size of replacement string
 * @param source original string
 * @return pointer to dest
 */
extern char* cpmapfil(char* dest, int size,const char* source);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>' and returns a dynamic string
 * @param file string for replacement
 * @return pointer to dynamic allocated string
 */
extern char* dcpmapfil(const char* file);

/**
 * Use rpltpl() and maplab() to build key label names, based on key label templates
 * @param label string for replacement
 * @param size size of replacement string
 * @param templ key label template (with %x)
 * @param values value string for replacement (x:%s\n)
 * @param toUpper for mapping label to upper
 * @return pointer to label
 */
extern char* cpmaplab(char* label, int size,const char* templ, const char* values, int toUpper);

/**
 * Use drpltpl() and dmaplab() to build key label names, based on key label templates in dynamic form
 * @param templ key label template (with %x)
 * @param values value string for replacement (x:%s\n)
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to dynamic allocated string
 */
extern char* dcpmaplab(const char* templ, const char* values, int method);

/**
 * Determines the local CCSID by querying nl_langinfo() (POSIX) or GetCPInfoEx() (Windows).
 * If none of both are available or no valid CCSID can be determined,
 * mapl2c() is called. If it also fails to determine the system default CSSID,
 * the CCSID for ASCII (ISO8859-1) or IBM-1047 (EBCDIC platforms) is returned.
 * @return A supported CCSID > 0
 */
extern unsigned int localccsid(void);

/**
 * Map environment variable LANG to CCSID
 * @param isEBCDIC if true returns EBCDIC code pages else ASCII
 * @return NULL in case of an error or pointer to a static string containing the CCSID
 */
extern const char* mapl2c(unsigned isEBCDIC);

/**
 * Map environment variable LANG to CCSID
 * @param pcLang   string containing the value of the environment variable LANG
 * @param isEbcdic if true returns EBCDIC code pages else ASCII
 * @return NULL in case of an error or pointer to a static string containing the CCSID
 */
extern const char* lng2ccsd(const char* pcLang, unsigned isEbcdic);

/**
 * Map CCSID in encoding string
 * @param uiCcsId CCSID
 * @return    encoding string
 */
extern const char* mapccsid(const unsigned int uiCcsId);

/**
 * Map encoding string in CCSID
 * @param p encoding string
 * @return    CCSID
 */
extern unsigned int mapcdstr(const char* p);

/**********************************************************************/

/**
 * Convert binary to hex
 * @param bin binary blob
 * @param hex hex string
 * @param len length of binary blob
 * @return amount of converted bytes
 */
extern unsigned int bin2hex(
   const unsigned char* bin,
         char*          hex,
   const unsigned int   len);

/**
 * Convert from hex to binary
 * @param hex  hex string
 * @param bin  binary string
 * @param len  length of hex string
 * @return     amount of converted bytes
 */
extern unsigned int hex2bin(
   const char*          hex,
         unsigned char* bin,
   const unsigned int   len);

/**
 * Convert character string to US-ASCII(UTF-8) and stops at not convertible chars
 * @param chr  character string
 * @param asc  ASCII string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int chr2asc(
   const char*          chr,
         char*          asc,
   const unsigned int   len);

/**
 * Convert character string to EBCDIC (only non variant characters) and stops at not convertible chars
 * @param chr  character string
 * @param ebc  EBCDIC string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int chr2ebc(
   const char*          chr,
         char*          ebc,
   const unsigned int   len);

/**
 * Convert ASCII to character string (only non variant characters) and stops at not convertible chars
 * @param asc  ASCII string
 * @param chr  character string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int asc2chr(
   const char*          asc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert ASCII to character string (only non variant characters) and replace not convertible chars with '_'
 * @param asc  ASCII string
 * @param chr  character string
 * @param len  length
 */
extern void asc_chr(
   const char*          asc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert character string to US-ASCII(UTF-8) and replace not convertible chars with '_'
 * @param chr  character string
 * @param asc  ASCII string
 * @param len  length
 */
extern void chr_asc(
   const char*          chr,
         char*          asc,
   const unsigned int   len);

/**
 * Convert EBCDIC to character string (only non variant characters) and stops at not convertible chars
 * @param ebc  EBCDIC string
 * @param chr  character string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int ebc2chr(
   const char*          ebc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert EBCDIC to character string (only non variant characters) and replace not convertible chars with '_'
 * @param ebc  EBCDIC string
 * @param chr  character string
 * @param len  length
 */
extern void ebc_chr(
   const char*          ebc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert character string to EBCDIC (only non variant characters) and replace not convertible chars with '_'
 * @param chr  character string
 * @param ebc  EBCDIC string
 * @param len  length
 */
extern void chr_ebc(
   const char*          chr,
         char*          ebc,
   const unsigned int   len);

/**
 * Read a file using the specified filename and reads the whole content
 * into the supplied buffer. The buffer is reallocated and bufsize updated,
 * if necessary.
 *
 * This is the default implementation if no file to string function for CLEP provided
 *
 * @param hdl is ignored and not used (required for default implementation of call back function)
 * @param filename The path and name of the file to read
 * @param buf A pointer to a pointer to a buffer, may be a pointer to NULL for allocation else reallocation
 * @param bufsize A pointer to the size of buf, is updated ater the call
 * @param errmsg Pointer to a provided buffer for the error message (optional (can be NULL), result is null terminated)
 * @param msgsiz The size of the buffer for the error message (optional (can be 0))
 * @return A positive value indicates the number of bytes read and copied into buf.
 *         A negative value indicates an error, in which case the content of buf is undefined.
 *         Error codes:
 *         * -1: invalid arguments
 *         * -2: fopen() failed
 *         * -3: integer overflow, file too big
 *         * -4: realloc() failed
 *         * -5: file read error
 */
extern int file2str(void* hdl, const char* filename, char** buf, int* bufsize, char* errmsg, const int msgsiz);

/**
 * Takes an array of null-terminated strings and concatenates all strings
 * into one single string separated by the specified separator. The resulting
 * string is put into the out buffer which may be reallocated if necessary.
 * If the buffer already contain a string the remaining strings are concatenated.
 * @param array Input array of null-terminated strings.
 * @param count Number of string in array
 * @param separ Separator of arbitrary length (may be NULL if separLen=0)
 * @param separLen length of separator
 * @param out Pointer to an output buffer (may be reallocated)
 * @param outlen Size of output buffer
 * @return Error codes:
 *         *  0: success
 *         * -1: invalid arguments
 *         * -2: realloc() failed
 */
extern int arry2str(char* array[], const int count, const char* separ, const int separLen, char** out, int* outlen);

/**
 * Compare of two string
 *
 * The procedure combines strcmp, stricmp, strncmp and strchr in one function.
 *
 * @param[in]  ca Flag if case sensitiv (TRUE) or not (FALSE)
 * @param[in]  s1 String 1 to compare
 * @param[in]  s2 string 2 to compare
 * @param[in]  n  If c!=0 then minimum else maximum amount of character to compare  (0=disabled)
 * @param[in]  c  Character where the compare stops or -1 for keyword syntax
 * @param[in]  f  If true only compare up to null termination or stop char if false (normal compare) including null termination or stop char
 *
 * @return signed integer with 0 for equal and !=0 for different
 */
extern int strxcmp(
   const int                     ca,
   const char*                   s1,
   const char*                   s2,
   const int                     n,
   const int                     c,
   const int                     f);

/**
 * Build time string
 *
 * Convert a time integer to a 20 byte time string of form YYYY-MM-DD HH:MM:SS.
 *
 * @param[in]  t  time in seconds since 1970 or 0 for current time
 * @param[in]  p  NULL to return a static variable or a pointer where the 20 bytes are copied in
 *
 * @return pointer to the time string
 */

#define CSTIME_BUFSIZ   24

extern char* cstime(signed long long t, char* p);

/**
 * Load environment variables from buffer
 *
 * @param[in] uiLen Length of the buffer with environment variables
 * @param[in] pcBuf Buffer containing list of environment variables (ASCII or EBCDIC separated by new line or semicolon)
 * @param[in] pfOut File pointer for output messages
 * @param[in] pfErr File pointer for error messages
 * @param[out] ppList Pointer to an optional envar list for reset (the list and each string must freed by caller)
 *
 * @return    >=0 amount of successful defined environment variables else -1*CLERTCs
 */
extern int loadEnvars(const unsigned int uiLen, const char* pcBuf, FILE* pfOut, FILE* pfErr, TsEnVarList** ppList);

/**
 * Read and set environment variables from file
 *
 * @param[in] pcFil Filename, if pcFil==NULL use "DD:STDENV" instead
 * @param[in] pfOut File pointer for output messages
 * @param[in] pfErr File pointer for error messages
 * @param[out] ppList Pointer to an optional envar list for reset (the list and each string must freed by caller)
 *
 * @return    >=0 amount of successful defined environment variables else -1*CLERTCs
 */
extern int readEnvars(const char* pcFil, FILE* pfOut, FILE* pfErr, TsEnVarList** ppList);

/**
 * Store envars in a list for reset
 *
 * @param[out] ppList Pointer to envar list for reset
 * @param[in]  pcName Name of the environment variable
 * @param[in]  pcValue Value of the environment variable (NULL for unset)
 */
extern int envarInsert(TsEnVarList** ppList,const char* pcName,const char* pcValue);

/**
 * Reset list of environment variables
 *
 * @param[in] ppList Pointer to envar lisl
 * @return  amount of envars reset or -1*CLERTCs
 */
extern int resetEnvars(TsEnVarList** ppList);


/**********************************************************************/

typedef struct DiaChr {
   char              exc[4];
   char              hsh[4];
   char              dlr[4];
   char              ats[4];
   char              sbo[4];
   char              bsl[4];
   char              sbc[4];
   char              crt[4];
   char              grv[4];
   char              cbo[4];
   char              vbr[4];
   char              cbc[4];
   char              tld[4];
   char              svb[4];
   char              sbs[4];
   char              idt[4];
} TsDiaChr;

extern void          init_diachr(TsDiaChr* psDiaChr,const unsigned int uiCcsId);

#ifdef __EBCDIC__

extern TsDiaChr      gsDiaChr;

extern int           ebcdic_snprintf(char* string, size_t size, const char* format, ...);
extern int           ebcdic_sprintf(char* string, const char* format, ...);
extern int           ebcdic_fprintf(FILE* file, const char* format, ...);

extern char          init_char(char* p);
extern char*         init_string(char* p);

#  define HSH_PBRK   "\x7B\x4A\x63\xB1\x69"
#  define ATS_PBRK   "\x7C\xB5\x80\xEC\x44\xAC"

#  define C_EXC      ((gsDiaChr.exc[0])?gsDiaChr.exc[0]:init_char(gsDiaChr.exc))
#  define C_HSH      ((gsDiaChr.hsh[0])?gsDiaChr.hsh[0]:init_char(gsDiaChr.hsh))
#  define C_DLR      ((gsDiaChr.dlr[0])?gsDiaChr.dlr[0]:init_char(gsDiaChr.dlr))
#  define C_ATS      ((gsDiaChr.ats[0])?gsDiaChr.ats[0]:init_char(gsDiaChr.ats))
#  define C_SBO      ((gsDiaChr.sbo[0])?gsDiaChr.sbo[0]:init_char(gsDiaChr.sbo))
#  define C_BSL      ((gsDiaChr.bsl[0])?gsDiaChr.bsl[0]:init_char(gsDiaChr.bsl))
#  define C_SBC      ((gsDiaChr.sbc[0])?gsDiaChr.sbc[0]:init_char(gsDiaChr.sbc))
#  define C_CRT      ((gsDiaChr.sbo[0])?gsDiaChr.crt[0]:init_char(gsDiaChr.crt))
#  define C_GRV      ((gsDiaChr.grv[0])?gsDiaChr.grv[0]:init_char(gsDiaChr.grv))
#  define C_CBO      ((gsDiaChr.cbo[0])?gsDiaChr.cbo[0]:init_char(gsDiaChr.cbo))
#  define C_VBR      ((gsDiaChr.vbr[0])?gsDiaChr.vbr[0]:init_char(gsDiaChr.vbr))
#  define C_CBC      ((gsDiaChr.cbc[0])?gsDiaChr.cbc[0]:init_char(gsDiaChr.cbc))
#  define C_TLD      ((gsDiaChr.tld[0])?gsDiaChr.tld[0]:init_char(gsDiaChr.tld))
#  define S_EXC      ((gsDiaChr.exc[0])?gsDiaChr.exc:init_string(gsDiaChr.exc))
#  define S_HSH      ((gsDiaChr.hsh[0])?gsDiaChr.hsh:init_string(gsDiaChr.hsh))
#  define S_DLR      ((gsDiaChr.dlr[0])?gsDiaChr.dlr:init_string(gsDiaChr.dlr))
#  define S_ATS      ((gsDiaChr.ats[0])?gsDiaChr.ats:init_string(gsDiaChr.ats))
#  define S_SBO      ((gsDiaChr.sbo[0])?gsDiaChr.sbo:init_string(gsDiaChr.sbo))
#  define S_BSL      ((gsDiaChr.bsl[0])?gsDiaChr.bsl:init_string(gsDiaChr.bsl))
#  define S_SBC      ((gsDiaChr.sbc[0])?gsDiaChr.sbc:init_string(gsDiaChr.sbc))
#  define S_CRT      ((gsDiaChr.sbo[0])?gsDiaChr.crt:init_string(gsDiaChr.crt))
#  define S_GRV      ((gsDiaChr.grv[0])?gsDiaChr.grv:init_string(gsDiaChr.grv))
#  define S_CBO      ((gsDiaChr.cbo[0])?gsDiaChr.cbo:init_string(gsDiaChr.cbo))
#  define S_VBR      ((gsDiaChr.vbr[0])?gsDiaChr.vbr:init_string(gsDiaChr.vbr))
#  define S_CBC      ((gsDiaChr.cbc[0])?gsDiaChr.cbc:init_string(gsDiaChr.cbc))
#  define S_TLD      ((gsDiaChr.tld[0])?gsDiaChr.tld:init_string(gsDiaChr.tld))
#  define S_SVB      ((gsDiaChr.svb[0])?gsDiaChr.svb:init_string(gsDiaChr.svb))
#  define S_SBS      ((gsDiaChr.sbs[0])?gsDiaChr.sbs:init_string(gsDiaChr.sbs))
#  define S_IDT      ((gsDiaChr.idt[0])?gsDiaChr.idt:init_string(gsDiaChr.idt))

#  define esnprintf  ebcdic_snprintf
#  define esprintf   ebcdic_sprintf
#  define efprintf   ebcdic_fprintf

#else

#  define HSH_PBRK   "#"   /*nodiac*/
#  define ATS_PBRK   "@"   /*nodiac*/

#  define C_EXC      '!'   /*nodiac*/
#  define C_HSH      '#'   /*nodiac*/
#  define C_DLR      '$'   /*nodiac*/
#  define C_ATS      '@'   /*nodiac*/
#  define C_SBO      '['   /*nodiac*/
#  define C_BSL      '\\'  /*nodiac*/
#  define C_SBC      ']'   /*nodiac*/
#  define C_CRT      '^'   /*nodiac*/
#  define C_GRV      '`'   /*nodiac*/
#  define C_CBO      '{'   /*nodiac*/
#  define C_VBR      '|'   /*nodiac*/
#  define C_CBC      '}'   /*nodiac*/
#  define C_TLD      '~'   /*nodiac*/
#  define S_EXC      "!"   /*nodiac*/
#  define S_HSH      "#"   /*nodiac*/
#  define S_DLR      "$"   /*nodiac*/
#  define S_ATS      "@"   /*nodiac*/
#  define S_SBO      "["   /*nodiac*/
#  define S_BSL      "\\"  /*nodiac*/
#  define S_SBC      "]"   /*nodiac*/
#  define S_CRT      "^"   /*nodiac*/
#  define S_GRV      "`"   /*nodiac*/
#  define S_CBO      "{"   /*nodiac*/
#  define S_VBR      "|"   /*nodiac*/
#  define S_CBC      "}"   /*nodiac*/
#  define S_TLD      "~"   /*nodiac*/
#  define S_SVB      "=|"  /*nodiac*/
#  define S_SBS      "/\\" /*nodiac*/
#  define S_IDT      "--|" /*nodiac*/

#  define esnprintf  snprintf
#  define esprintf   sprintf
#  define efprintf   fprintf

#endif

/**********************************************************************/

#endif /* INC_CLEPUTL_H */

#ifdef __cplusplus
   }
#endif

