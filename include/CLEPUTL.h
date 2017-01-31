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
 * static linked to CLP and or CLE.
 *
 **********************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLEPUTL_H
#define INC_CLEPUTL_H

#ifndef TRUE
#  define TRUE             (1)
#endif

#ifndef FALSE
#  define FALSE            (0)
#endif

#ifndef EOS
   #define EOS             (0x00)
#endif

#ifndef L_userid
#  define L_userid         (257)
#endif

#ifndef L_filnam
#  define L_filnam         (1025)
#endif

#ifdef __WIN__
extern int win_setenv(const char* name, const char* value);
extern int win_unsetenv(const char* name);
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) win_setenv((name), (value))
#  define UNSETENV(name)      win_unsetenv((name))
#else
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) setenv((name), (value), 1)
#  define UNSETENV(name)      unsetenv((name))
#endif

#define isStr(c) (isprint(c) || (c)==C_TLD  || (c)==C_DLR || (c)==C_ATS || (c)==C_BSL || (c)==C_CRT || (c)==C_EXC)
#define isKyw(c) (isalnum(c) || (c)=='_')

#define ISDDNAME(p)     (toupper((p)[0])=='D' && toupper((p)[1])=='D' && (p)[2]==':')
#define ISPATHNAME(p)   (strchr((p),'/')!=NULL)

/* Definition of return/condition/exit codes **************************/

#define CLERTC_OK             0
#define CLERTC_FIN            2
#define CLERTC_WRN            4
#define CLERTC_RUN            8
#define CLERTC_MAP            12
#define CLERTC_SYN            16
#define CLERTC_CMD            20
#define CLERTC_INI            24
#define CLERTC_CFG            28
#define CLERTC_TAB            32
#define CLERTC_SYS            36
#define CLERTC_ACS            40
#define CLERTC_ITF            44
#define CLERTC_FAT            64
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
 * Works like printf but print only in debug mode.
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int printd(const char* format,...);
/**
 * Works like snprintf but concatenates the format string to the buffer.
 * @param buffer  pointer to the string buffer
 * @param size    size of the string buffer
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int snprintc(char* buffer,const size_t size,const char* format,...);

/**
 * Works like snprintf but does reallocation of the buffer (maximal expansion of the format string can be specified).
 * @param buffer  pointer to pointer to the string buffer (is updated, could be NULL at beginning)
 * @param size    pointer to size of the string buffer (is updated, could be 0 at beginning)
 * @param expan   maximal expected expansion of the format string (size must be fit strlen(*buffer)+strlen(format)+expansion+1)
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int srprintc(char** buffer,size_t* size,const size_t expan,const char* format,...);

/**
 * Works like snprintc but does reallocation of the buffer (maximal expansion of the format string can be specified).
 * @param buffer  pointer to pointer to the string buffer (is updated, could be NULL at beginning)
 * @param size    pointer to size of the string buffer (is updated, could be 0 at beginning)
 * @param expan   maximal expected expansion of the format string (size must be fit strlen(format)+expansion+1)
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int srprintf(char** buffer,size_t* size,const size_t expan,const char* format,...);


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
 * This function parse a zero terminated string array of a certain length or terminated with 0xFF.
 * Such a string array is the result of a variable (not fix) array of strings provided by CLP.
 * If you don't know the length, please provide a negative number to look for 0xFF termination.
 * For 0xFF termination you must define the CLPFLG_DLM. This is useful if there is no capability
 * to use the ELN link to determine the length of the string array. Each call give the pointer to
 * the next string in the array if no string found anymore, then NULL is returned.
 * @param hdl     pointer of pointer to string initialized with NULL at beginning
 * @param str     pointer to the string arrays from CLP
 * @param len     -1 for 0xFF delimiter parsing or the ELN of the string array
 * @return        pointer to one string or NULL if no more string
 */
extern const char* prsdstr(const char** hdl, const char* str, int len);

/**
 * Works like strncpy but ensures null-termination.
 * @param dest    pointer to destination string
 * @param src     pointer to source string
 * @param n       size of memory available for buffer
 * @return        pointer to destination string
 */
extern char* strxcpy(char *dest, const char *src, size_t n);

/**
 * Get environment variable and handle HOME, USER, CUSEr, Cuser, cuser, OWNER, ENVID if not defined
 * @param name environment variable name
 * @param size size of string
 * @param string containing the value for the corresponding environment variable
 * @return pointer to string
 */
extern char* getenvar(const char* name,size_t size,char* string);

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
 * @param toUpper for mapping string to upper
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapstr(const char* string,int toUpper);

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
 * @param toUpper for mapping file to upper
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapfil(const char* file, int toUpper);

/**
 * Replace '!' with ENVID, '~' with "<SYSUID>", '^' with "<OWNERID>" and all environment variables enclosed with '<' and '> to build a key label'
 * @param label string for replacement
 * @param size size of replacement string
 * @param toUpper for mapping label to upper
 * @return pointer to label
 */
extern char* maplab(char* label,int size,int toUpper);

/**
 * Replace '!' with ENVID, '~' with "<SYSUID>", '^' with "<OWNERID>" and all environment variables enclosed with '<' and '> to build a dynamic key label'
 * @param label string for replacement
 * @param toUpper for mapping label to upper
 * @return pointer to new allocated string or NULL if error
 */
extern char* dmaplab(const char* label, int toUpper);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>'
 * @param dest string for replacement
 * @param size size of replacement string
 * @param source original string
 * @return pointer to format string for fopen()
 */
extern char* cpmapfil(char* dest, int size,const char* source);

/**
 * set the right mode for fopen, private extension use 's' for fopen with seek
 * @param mode for fopen
 * @return mapped mode
 */
extern char* filemode(const char* mode);

/**
 * Use rpltpl() and maplab() to build key label names, based on key label templates
 * @param label string for replacement
 * @param size size of replacement string
 * @param templ key label template (with %x)
 * @param values value string for replacement (x:%s\n)
 * @param toUpper for mapping label to upper
 * @return pointer to string
 */
extern char* cpmaplab(char* label, int size,const char* templ, const char* values, int toUpper);

/**
 * Determines the system default CCSID by querying nl_langinfo() (POSIX) or GetCPInfoEx() (Windows).
 * If none of both are available or no valid CCSID can be determined,
 * mapl2c() is called. If it also fails to determine the system default CSSID,
 * the CCSID for US-ASCII (ASCII platforms) or IBM-1047 (EBCDIC platforms) is returned.
 * @return A supported CCSID > 0
 */
extern unsigned int sysccsid(void);

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
 * @param csn CCSID
 * @return    encoding string
 */
extern const char* mapccsid(const unsigned int csn);

/**
 * Map encoding string in CCSID
 * @param csn encoding string
 * @return    CCSID
 */
extern unsigned int mapcdstr(const char* csn);

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
 * Convert character string to EBCDIC(only non variant characters) and stops at not convertible chars
 * @param chr  character string
 * @param asc  EBCDIC string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int chr2ebc(
   const char*          chr,
         char*          asc,
   const unsigned int   len);

/**
 * Read a file using the specified filename and reads the whole content
 * into the supplied buffer. The buffer is reallocated and bufsize updated,
 * if necessary.
 * @param filename The path and name of the file to read
 * @param buf A pointer to a buffer, may be a pointer to NULL
 * @param bufsize The size of buf
 * @param format The format string for the file (must start with 'r' otherwise ignored)
 * @return A positive value indicates the number of bytes read and copied into buf.
 *         A negative value indicates an error, in which case the content of buf is undefined.
 *         Error codes:
 *         * -1: invalid arguments
 *         * -2: fopen() failed
 *         * -3: integer overflow, file too big
 *         * -4: realloc() failed
 *         * -5: file read error
 */
extern int file2str(const char* filename, char** buf, int* bufsize, const char* format);

/**
 * Takes an array of null-terminated strings and concatenates all strings
 * into one single string separated by the specified separator. The resulting
 * string is put into the out buffer which may be reallocated if necessary.
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

/**********************************************************************/

#ifdef __EBCDIC__

extern int ebcdic_snprintf(char* string, size_t size, const char* format, ...);
extern int ebcdic_sprintf(char* string, const char* format, ...);
extern int ebcdic_fprintf(FILE* file, const char* format, ...);

static char       gs_exc[4]={0,0,0,0};
static char       gs_hsh[4]={0,0,0,0};
static char       gs_dlr[4]={0,0,0,0};
static char       gs_ats[4]={0,0,0,0};
static char       gs_sbo[4]={0,0,0,0};
static char       gs_bsl[4]={0,0,0,0};
static char       gs_sbc[4]={0,0,0,0};
static char       gs_crt[4]={0,0,0,0};
static char       gs_grv[4]={0,0,0,0};
static char       gs_cbo[4]={0,0,0,0};
static char       gs_vbr[4]={0,0,0,0};
static char       gs_cbc[4]={0,0,0,0};
static char       gs_tld[4]={0,0,0,0};
static char       gs_svb[4]={0,0,0,0};
static char       gs_sbs[4]={0,0,0,0};
static char       gs_idt[4]={0,0,0,0};

static char init_char(char* p) {
   unsigned int ccsid=mapcdstr(mapl2c(TRUE));
   switch (ccsid) {
   case 37:
   case 424:
   case 1140:
      gs_exc[0]=0x5A;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0xBA;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0xBB;
      gs_crt[0]=0xB0;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x4F;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xA1;
      break;
   case 273:
   case 1141:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0xB5;
      gs_sbo[0]=0x63;
      gs_bsl[0]=0xEC;
      gs_sbc[0]=0xFC;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x79;
      gs_cbo[0]=0x43;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0xDC;
      gs_tld[0]=0x59;
      break;
   case 1142:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x4A;
      gs_dlr[0]=0x67;
      gs_ats[0]=0x80;
      gs_sbo[0]=0x9E;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0x9F;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x79;
      gs_cbo[0]=0x9C;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0x47;
      gs_tld[0]=0xDC;
      break;
   case 1143:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x63;
      gs_dlr[0]=0x67;
      gs_ats[0]=0xEC;
      gs_sbo[0]=0xB5;
      gs_bsl[0]=0x71;
      gs_sbc[0]=0x9F;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x51;
      gs_cbo[0]=0x43;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0x47;
      gs_tld[0]=0xDC;
      break;
   case 1144:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0xB1;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0xB5;
      gs_sbo[0]=0x90;
      gs_bsl[0]=0x48;
      gs_sbc[0]=0x51;
      gs_crt[0]=0x5F;
      gs_grv[0]=0xDD;
      gs_cbo[0]=0x44;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0x54;
      gs_tld[0]=0x58;
      break;
   case 1145:
      gs_exc[0]=0xBB;
      gs_hsh[0]=0x69;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0x4A;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0x5A;
      gs_crt[0]=0xBA;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x4F;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xBD;
      break;
   case 1146:
      gs_exc[0]=0x5A;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x4A;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0xB1;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0xBB;
      gs_crt[0]=0xBA;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x4F;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xBC;
      break;
   case 1147:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0xB1;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x44;
      gs_sbo[0]=0x90;
      gs_bsl[0]=0x48;
      gs_sbc[0]=0xB5;
      gs_crt[0]=0x5F;
      gs_grv[0]=0xA0;
      gs_cbo[0]=0x51;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0x54;
      gs_tld[0]=0xBD;
      break;
   case 500:
   case 875:
   case 1148:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0x4A;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0x5A;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xA1;
      break;
   case 1149:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0xAC;
      gs_sbo[0]=0xAE;
      gs_bsl[0]=0xBE;
      gs_sbc[0]=0x9E;
      gs_crt[0]=0xEC;
      gs_grv[0]=0x8C;
      gs_cbo[0]=0x8E;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0x9C;
      gs_tld[0]=0xCC;
      break;
   case 1153:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0x4A;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0x5A;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x6A;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xA1;
      break;
   case 1154:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0x4A;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0x5A;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x6A;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xA1;
      break;
   case 1156:
      gs_exc[0]=0x5A;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0xBA;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0xBB;
      gs_crt[0]=0xB0;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x4F;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xA1;
      break;
   case 1122:
      gs_exc[0]=0x4F;
      gs_hsh[0]=0x63;
      gs_dlr[0]=0x67;
      gs_ats[0]=0xEC;
      gs_sbo[0]=0xB5;
      gs_bsl[0]=0x71;
      gs_sbc[0]=0x9F;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x51;
      gs_cbo[0]=0x43;
      gs_vbr[0]=0xBB;
      gs_cbc[0]=0x47;
      gs_tld[0]=0xDC;
      break;
   case 1047:
      gs_exc[0]=0x5A;
      gs_hsh[0]=0x7B;
      gs_dlr[0]=0x5B;
      gs_ats[0]=0x7C;
      gs_sbo[0]=0xAD;
      gs_bsl[0]=0xE0;
      gs_sbc[0]=0xBD;
      gs_crt[0]=0x5F;
      gs_grv[0]=0x79;
      gs_cbo[0]=0xC0;
      gs_vbr[0]=0x4F;
      gs_cbc[0]=0xD0;
      gs_tld[0]=0xA1;
      break;
   default:
      gs_exc[0]='!' ;/*nodiac*/
      gs_hsh[0]='#' ;/*nodiac*/
      gs_dlr[0]='$' ;/*nodiac*/
      gs_ats[0]='@' ;/*nodiac*/
      gs_sbo[0]='[' ;/*nodiac*/
      gs_bsl[0]='\\';/*nodiac*/
      gs_sbc[0]=']' ;/*nodiac*/
      gs_crt[0]='^' ;/*nodiac*/
      gs_grv[0]='`' ;/*nodiac*/
      gs_cbo[0]='{' ;/*nodiac*/
      gs_vbr[0]='|' ;/*nodiac*/
      gs_cbc[0]='}' ;/*nodiac*/
      gs_tld[0]='~' ;/*nodiac*/
      break;
   }
   gs_svb[0]='=';
   gs_svb[1]=gs_vbr[0];
   gs_sbs[0]='/';
   gs_sbs[1]=gs_bsl[0];
   gs_idt[0]='-';
   gs_idt[1]='-';
   gs_idt[2]=gs_vbr[0];
   return(p[0]);
}

static char* init_string(char* p) {
   init_char(p);
   return(p);
}

#  define C_EXC               ((gs_exc[0])?gs_exc[0]:init_char(gs_exc))
#  define C_HSH               ((gs_hsh[0])?gs_hsh[0]:init_char(gs_hsh))
#  define C_DLR               ((gs_dlr[0])?gs_dlr[0]:init_char(gs_dlr))
#  define C_ATS               ((gs_ats[0])?gs_ats[0]:init_char(gs_ats))
#  define C_SBO               ((gs_sbo[0])?gs_sbo[0]:init_char(gs_sbo))
#  define C_BSL               ((gs_bsl[0])?gs_bsl[0]:init_char(gs_bsl))
#  define C_SBC               ((gs_sbc[0])?gs_sbc[0]:init_char(gs_sbc))
#  define C_CRT               ((gs_sbo[0])?gs_crt[0]:init_char(gs_crt))
#  define C_GRV               ((gs_grv[0])?gs_grv[0]:init_char(gs_grv))
#  define C_CBO               ((gs_cbo[0])?gs_cbo[0]:init_char(gs_cbo))
#  define C_VBR               ((gs_vbr[0])?gs_vbr[0]:init_char(gs_vbr))
#  define C_CBC               ((gs_cbc[0])?gs_cbc[0]:init_char(gs_cbc))
#  define C_TLD               ((gs_tld[0])?gs_tld[0]:init_char(gs_tld))
#  define S_EXC               ((gs_exc[0])?gs_exc:init_string(gs_exc))
#  define S_HSH               ((gs_hsh[0])?gs_hsh:init_string(gs_hsh))
#  define S_DLR               ((gs_dlr[0])?gs_dlr:init_string(gs_dlr))
#  define S_ATS               ((gs_ats[0])?gs_ats:init_string(gs_ats))
#  define S_SBO               ((gs_sbo[0])?gs_sbo:init_string(gs_sbo))
#  define S_BSL               ((gs_bsl[0])?gs_bsl:init_string(gs_bsl))
#  define S_SBC               ((gs_sbc[0])?gs_sbc:init_string(gs_sbc))
#  define S_CRT               ((gs_sbo[0])?gs_crt:init_string(gs_crt))
#  define S_GRV               ((gs_grv[0])?gs_grv:init_string(gs_grv))
#  define S_CBO               ((gs_cbo[0])?gs_cbo:init_string(gs_cbo))
#  define S_VBR               ((gs_vbr[0])?gs_vbr:init_string(gs_vbr))
#  define S_CBC               ((gs_cbc[0])?gs_cbc:init_string(gs_cbc))
#  define S_TLD               ((gs_tld[0])?gs_tld:init_string(gs_tld))
#  define S_SVB               ((gs_svb[0])?gs_svb:init_string(gs_svb))
#  define S_SBS               ((gs_sbs[0])?gs_sbs:init_string(gs_sbs))
#  define S_IDT               ((gs_idt[0])?gs_idt:init_string(gs_idt))


#  define esnprintf           ebcdic_snprintf
#  define esprintf            ebcdic_sprintf
#  define efprintf            ebcdic_fprintf

#else

#  define C_EXC              '!' /*nodiac*/
#  define C_HSH              '#' /*nodiac*/
#  define C_DLR              '$' /*nodiac*/
#  define C_ATS              '@' /*nodiac*/
#  define C_SBO              '[' /*nodiac*/
#  define C_BSL              '\\'/*nodiac*/
#  define C_SBC              ']' /*nodiac*/
#  define C_CRT              '^' /*nodiac*/
#  define C_GRV              '`' /*nodiac*/
#  define C_CBO              '{' /*nodiac*/
#  define C_VBR              '|' /*nodiac*/
#  define C_CBC              '}' /*nodiac*/
#  define C_TLD              '~' /*nodiac*/
#  define S_EXC              "!" /*nodiac*/
#  define S_HSH              "#" /*nodiac*/
#  define S_DLR              "$" /*nodiac*/
#  define S_ATS              "@" /*nodiac*/
#  define S_SBO              "[" /*nodiac*/
#  define S_BSL              "\\"/*nodiac*/
#  define S_SBC              "]" /*nodiac*/
#  define S_CRT              "^" /*nodiac*/
#  define S_GRV              "`" /*nodiac*/
#  define S_CBO              "{" /*nodiac*/
#  define S_VBR              "|" /*nodiac*/
#  define S_CBC              "}" /*nodiac*/
#  define S_TLD              "~" /*nodiac*/
#  define S_SVB              "=|" /*nodiac*/
#  define S_SBS              "/\\" /*nodiac*/
#  define S_IDT              "--|" /*nodiac*/

#  define esnprintf          snprintf
#  define esprintf           sprintf
#  define efprintf           fprintf

#endif

/**********************************************************************/

#endif /* INC_CLEPUTL_H */

#ifdef __cplusplus
   }
#endif

