/**
 * @file
 * @brief  Declaration of utility functions for CLP/CLE in ANSI C
 * @author limes datentechnik gmbh
 * @date   18.02.2015
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


#if ('0'==0xF0)
   extern int init_char(int* p);
   extern static int gc_sbo;
   extern static int gc_sbc;
   extern static int gc_tilde;
#  define C_SBO                 ((gc_sbo)?gc_sbo:init_char(&gc_sbo))
#  define C_SBC                 ((gc_sbc)?gc_sbc:init_char(&gc_sbc))
#  define C_TILDE               ((gc_tilde)?gc_tilde:init_char(&gc_tilde))
else
#  define C_SBO              '['
#  define C_SBC              ']'
#  define C_TILDE            '~'
#endif

#define isKyw(c) (isalnum(c) || (c)=='_' || (c)=='-')

#define ISDDNAME(p)     (toupper((p)[0])=='D' && toupper((p)[1])=='D' && (p)[2]==':')
#define ISPATHNAME(p)   (strchr((p),'/')!=NULL)

#ifdef __WIN__
extern int win_setenv(const char* name, const char* value);
extern int win_unsetenv(const char* name);
extern int win_snprintf(char *buffer, size_t size, const char *format, ...);
#  define snprintf            win_snprintf
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) win_setenv((name), (value))
#  define UNSETENV(name)      win_unsetenv((name))
#else
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) setenv((name), (value), 1)
#  define UNSETENV(name)      unsetenv((name))
#endif

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

/**********************************************************************/

/**
 * Return the current user id.
 * @param size    size of the buffer
 * @param buffer  pointer to the buffer
 * @return        pointer to the buffer containing the current user id (zero terminated)
 */
extern char* userid(const int size, char* buffer);

/**
 * Return the current home directory.
 * @param flag    if true then slash/backslash are added
 * @param size    size of the string buffer
 * @param buffer  pointer to the buffer
 * @return        pointer to the buffer containing the current home directory (zero terminated)
 */
extern char* homedir(const int flag, const int size, char* buffer);

/**
 * Works like snprintf but concatenate the format string to the buffer.
 * @param buffer  pointer to the string buffer
 * @param size    size of the string buffer
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int snprintc(char* buffer,const size_t size,const char* format,...);

/**
 * Works like strncpy but ensure 0 termination.
 * @param dest    pointer to destination string
 * @param src     pointer to source string
 * @param n       size of memory available for buffer
 * @return        pointer to destination string
 */
extern char* strxcpy(char *dest, const char *src, size_t n);

/**
 * Replace each occurrence of char in name by value
 * @param name string for replacement
 * @param size size of replacement string
 * @param chr  character for replacement
 * @param value value for replacement
 */
extern void rplchar(char* name,const size_t size,const char chr, const char* value);

/**
 * Replace environment variables in a string
 * @param name string for replacement
 * @param size size of replacement string
 * @param opn  character to start enclosing of envar
 * @param cls  character for end of enclosing of envar
 */
extern void rplenvar(char* name,size_t size,const char opn, const char cls);

/**
 * Replace '+' with "<HOME>" and all enviroment variables enclosed with '<' and '>'
 * @param file string for replacement
 * @param size size of replacement string
 * @return pointer to file
 */
extern char* mapfil(char* file,int size);

/**
 * Replace '+' with "<HOME>" and all environment variables enclosed with '<' and '>'
 * @param dest string for replacement
 * @param size size of replacement string
 * @param source original string
 * @param flag if true data set names (only host) are enclosed in apostrophes (requires full qualified data set names)
 * @return pointer to string which must be concatenated to the format string for fopen at write operation ("w/wb/a/...")
 */
extern char* cpmapfil(char* dest, int size,const char* source,const int flag);

/**
 * Map environment variable LANG to CCSID
 * @param isEBCDIC if true returns EBCDIC code pages else ASCII
 * @return NULL in case of an error or pointer to a static string containing the CCSID
 */
extern const char* mapl2c(unsigned isEBCDIC);

/**
 * Map environment variable LANG to CCSID
 * @param pcLang   string containing the value of the environment variable LANG
 * @param uiLen    length of string vaulue
 * @param isEbcdic if true returns EBCDIC code pages else ASCII
 * @return NULL in case of an error or pointer to a static string containing the CCSID
 */
extern const char* lng2ccsd(const char* pcLang, unsigned uiLen, unsigned isEbcdic);


/**********************************************************************/

/**
 * Convert from hex to binary
 * @param hex  hex string
 * @param bin  binary string
 * @param len  length
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
 * @return A positive value indicates the number of bytes read and copied into buf.
 *         A negative value indicates an error, in which case the content of buf is undefined.
 *         Error codes:
 *         * -1: invalid arguments
 *         * -2: fopen() failed
 *         * -3: integer overflow, file too big
 *         * -4: realloc() failed
 *         * -5: file read error
 */
extern int file2str(const char* filename, char** buf, int* bufsize);

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
 * @param[in]  f  If true only compare up to zero termination or stop char if false (normal compare) including zero termination or stop char
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

#endif /* INC_CLEPUTL_H */

#ifdef __cplusplus
   }
#endif

