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

#define isKyw(c) (isalnum(c) || (c)=='_' || (c)=='-')

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
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>'
 * @param file string for replacement
 * @param size size of replacement string
 * @return pointer to file
 */
extern char* mapfil(char* file,int size);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>'
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

/**********************************************************************/

#ifdef __EBCDIC__

extern int ebcdic_snprintf(char* string, size_t size, const char* format, ...);
extern int ebcdic_sprintf(char* string, const char* format, ...);
extern int ebcdic_fprintf(FILE* file, const char* format, ...);

static int        gc_exc=0;
static char       gs_exc[2]={0,0};
static int        gc_hsh=0;
static char       gs_hsh[2]={0,0};
static int        gc_dlr=0;
static char       gs_dlr[2]={0,0};
static int        gc_ats=0;
static char       gs_ats[2]={0,0};
static int        gc_sbo=0;
static char       gs_sbo[2]={0,0};
static int        gc_bsl=0;
static char       gs_bsl[2]={0,0};
static int        gc_sbc=0;
static char       gs_sbc[2]={0,0};
static int        gc_crt=0;
static char       gs_crt[2]={0,0};
static int        gc_grv=0;
static char       gs_grv[2]={0,0};
static int        gc_cbo=0;
static char       gs_cbo[2]={0,0};
static int        gc_vbr=0;
static char       gs_vbr[2]={0,0};
static int        gc_cbc=0;
static char       gs_cbc[2]={0,0};
static int        gc_tld=0;
static char       gs_tld[2]={0,0};
static char       gs_svb[4]={0,0,0,0};
static char       gs_sbs[4]={0,0,0,0};
static char       gs_idt[4]={0,0,0,0};

static int init_char(int* p) {
   unsigned int ccsid=mapcdstr(mapl2c(TRUE));
   switch (ccsid) {
   case 37:
   case 424:
   case 1140:
      gc_exc=0x5A; gs_exc[0]=0x5A; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0xBA; gs_sbo[0]=0xBA; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0xBB; gs_sbc[0]=0xBB; gs_sbc[1]=0x00;
      gc_crt=0xB0; gs_crt[0]=0xB0; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x4F; gs_vbr[0]=0x4F; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xA1; gs_tld[0]=0xA1; gs_tld[1]=0x00;
      break;
   case 273:
   case 1141:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0xB5; gs_ats[0]=0xB5; gs_ats[1]=0x00;
      gc_sbo=0x63; gs_sbo[0]=0x63; gs_sbo[1]=0x00;
      gc_bsl=0xEC; gs_bsl[0]=0xEC; gs_bsl[1]=0x00;
      gc_sbc=0xFC; gs_sbc[0]=0xFC; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0x43; gs_cbo[0]=0x43; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0xDC; gs_cbc[0]=0xDC; gs_cbc[1]=0x00;
      gc_tld=0x59; gs_tld[0]=0x59; gs_tld[1]=0x00;
      break;
   case 1142:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x4A; gs_hsh[0]=0x4A; gs_hsh[1]=0x00;
      gc_dlr=0x67; gs_dlr[0]=0x67; gs_dlr[1]=0x00;
      gc_ats=0x80; gs_ats[0]=0x80; gs_ats[1]=0x00;
      gc_sbo=0x9E; gs_sbo[0]=0x9E; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0x9F; gs_sbc[0]=0x9F; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0x9C; gs_cbo[0]=0x9C; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0x47; gs_cbc[0]=0x47; gs_cbc[1]=0x00;
      gc_tld=0xDC; gs_tld[0]=0xDC; gs_tld[1]=0x00;
      break;
   case 1143:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x63; gs_hsh[0]=0x63; gs_hsh[1]=0x00;
      gc_dlr=0x67; gs_dlr[0]=0x67; gs_dlr[1]=0x00;
      gc_ats=0xEC; gs_ats[0]=0xEC; gs_ats[1]=0x00;
      gc_sbo=0xB5; gs_sbo[0]=0xB5; gs_sbo[1]=0x00;
      gc_bsl=0x71; gs_bsl[0]=0x71; gs_bsl[1]=0x00;
      gc_sbc=0x9F; gs_sbc[0]=0x9F; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x51; gs_grv[0]=0x51; gs_grv[1]=0x00;
      gc_cbo=0x43; gs_cbo[0]=0x43; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0x47; gs_cbc[0]=0x47; gs_cbc[1]=0x00;
      gc_tld=0xDC; gs_tld[0]=0xDC; gs_tld[1]=0x00;
      break;
   case 1144:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0xB1; gs_hsh[0]=0xB1; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0xB5; gs_ats[0]=0xB5; gs_ats[1]=0x00;
      gc_sbo=0x90; gs_sbo[0]=0x90; gs_sbo[1]=0x00;
      gc_bsl=0x48; gs_bsl[0]=0x48; gs_bsl[1]=0x00;
      gc_sbc=0x51; gs_sbc[0]=0x51; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0xDD; gs_grv[0]=0xDD; gs_grv[1]=0x00;
      gc_cbo=0x44; gs_cbo[0]=0x44; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0x54; gs_cbc[0]=0x54; gs_cbc[1]=0x00;
      gc_tld=0x58; gs_tld[0]=0x58; gs_tld[1]=0x00;
      break;
   case 1145:
      gc_exc=0xBB; gs_exc[0]=0xBB; gs_exc[1]=0x00;
      gc_hsh=0x69; gs_hsh[0]=0x69; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0x4A; gs_sbo[0]=0x4A; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0x5A; gs_sbc[0]=0x5A; gs_sbc[1]=0x00;
      gc_crt=0xBA; gs_crt[0]=0xBA; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x4F; gs_vbr[0]=0x4F; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xBD; gs_tld[0]=0xBD; gs_tld[1]=0x00;
      break;
   case 1146:
      gc_exc=0x5A; gs_exc[0]=0x5A; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x4A; gs_dlr[0]=0x4A; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0xB1; gs_sbo[0]=0xB1; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0xBB; gs_sbc[0]=0xBB; gs_sbc[1]=0x00;
      gc_crt=0xBA; gs_crt[0]=0xBA; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x4F; gs_vbr[0]=0x4F; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xBC; gs_tld[0]=0xBC; gs_tld[1]=0x00;
      break;
   case 1147:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0xB1; gs_hsh[0]=0xB1; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x44; gs_ats[0]=0x44; gs_ats[1]=0x00;
      gc_sbo=0x90; gs_sbo[0]=0x90; gs_sbo[1]=0x00;
      gc_bsl=0x48; gs_bsl[0]=0x48; gs_bsl[1]=0x00;
      gc_sbc=0xB5; gs_sbc[0]=0xB5; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0xA0; gs_grv[0]=0xA0; gs_grv[1]=0x00;
      gc_cbo=0x51; gs_cbo[0]=0x51; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0x54; gs_cbc[0]=0x54; gs_cbc[1]=0x00;
      gc_tld=0xBD; gs_tld[0]=0xBD; gs_tld[1]=0x00;
      break;
   case 500:
   case 875:
   case 1148:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0x4A; gs_sbo[0]=0x4A; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0x5A; gs_sbc[0]=0x5A; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xA1; gs_tld[0]=0xA1; gs_tld[1]=0x00;
      break;
   case 1149:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0xAC; gs_ats[0]=0xAC; gs_ats[1]=0x00;
      gc_sbo=0xAE; gs_sbo[0]=0xAE; gs_sbo[1]=0x00;
      gc_bsl=0xBE; gs_bsl[0]=0xBE; gs_bsl[1]=0x00;
      gc_sbc=0x9E; gs_sbc[0]=0x9E; gs_sbc[1]=0x00;
      gc_crt=0xEC; gs_crt[0]=0xEC; gs_crt[1]=0x00;
      gc_grv=0x8C; gs_grv[0]=0x8C; gs_grv[1]=0x00;
      gc_cbo=0x8E; gs_cbo[0]=0x8E; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0x9C; gs_cbc[0]=0x9C; gs_cbc[1]=0x00;
      gc_tld=0xCC; gs_tld[0]=0xCC; gs_tld[1]=0x00;
      break;
   case 1153:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0x4A; gs_sbo[0]=0x4A; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0x5A; gs_sbc[0]=0x5A; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x6A; gs_vbr[0]=0x6A; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xA1; gs_tld[0]=0xA1; gs_tld[1]=0x00;
      break;
   case 1154:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0x4A; gs_sbo[0]=0x4A; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0x5A; gs_sbc[0]=0x5A; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x6A; gs_vbr[0]=0x6A; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xA1; gs_tld[0]=0xA1; gs_tld[1]=0x00;
      break;
   case 1156:
      gc_exc=0x5A; gs_exc[0]=0x5A; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0xBA; gs_sbo[0]=0xBA; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0xBB; gs_sbc[0]=0xBB; gs_sbc[1]=0x00;
      gc_crt=0xB0; gs_crt[0]=0xB0; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x4F; gs_vbr[0]=0x4F; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xA1; gs_tld[0]=0xA1; gs_tld[1]=0x00;
      break;
   case 1122:
      gc_exc=0x4F; gs_exc[0]=0x4F; gs_exc[1]=0x00;
      gc_hsh=0x63; gs_hsh[0]=0x63; gs_hsh[1]=0x00;
      gc_dlr=0x67; gs_dlr[0]=0x67; gs_dlr[1]=0x00;
      gc_ats=0xEC; gs_ats[0]=0xEC; gs_ats[1]=0x00;
      gc_sbo=0xB5; gs_sbo[0]=0xB5; gs_sbo[1]=0x00;
      gc_bsl=0x71; gs_bsl[0]=0x71; gs_bsl[1]=0x00;
      gc_sbc=0x9F; gs_sbc[0]=0x9F; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x51; gs_grv[0]=0x51; gs_grv[1]=0x00;
      gc_cbo=0x43; gs_cbo[0]=0x43; gs_cbo[1]=0x00;
      gc_vbr=0xBB; gs_vbr[0]=0xBB; gs_vbr[1]=0x00;
      gc_cbc=0x47; gs_cbc[0]=0x47; gs_cbc[1]=0x00;
      gc_tld=0xDC; gs_tld[0]=0xDC; gs_tld[1]=0x00;
      break;
   case 1047:
      gc_exc=0x5A; gs_exc[0]=0x5A; gs_exc[1]=0x00;
      gc_hsh=0x7B; gs_hsh[0]=0x7B; gs_hsh[1]=0x00;
      gc_dlr=0x5B; gs_dlr[0]=0x5B; gs_dlr[1]=0x00;
      gc_ats=0x7C; gs_ats[0]=0x7C; gs_ats[1]=0x00;
      gc_sbo=0xAD; gs_sbo[0]=0xAD; gs_sbo[1]=0x00;
      gc_bsl=0xE0; gs_bsl[0]=0xE0; gs_bsl[1]=0x00;
      gc_sbc=0xBD; gs_sbc[0]=0xBD; gs_sbc[1]=0x00;
      gc_crt=0x5F; gs_crt[0]=0x5F; gs_crt[1]=0x00;
      gc_grv=0x79; gs_grv[0]=0x79; gs_grv[1]=0x00;
      gc_cbo=0xC0; gs_cbo[0]=0xC0; gs_cbo[1]=0x00;
      gc_vbr=0x4F; gs_vbr[0]=0x4F; gs_vbr[1]=0x00;
      gc_cbc=0xD0; gs_cbc[0]=0xD0; gs_cbc[1]=0x00;
      gc_tld=0xA1; gs_tld[0]=0xA1; gs_tld[1]=0x00;
      break;
   default:
      gc_exc='!'; gs_exc[0]='!'; gs_exc[1]=0x00;/*nodiac*/
      gc_hsh='#'; gs_hsh[0]='#'; gs_hsh[1]=0x00;/*nodiac*/
      gc_dlr='$'; gs_dlr[0]='$'; gs_dlr[1]=0x00;/*nodiac*/
      gc_ats='@'; gs_ats[0]='@'; gs_ats[1]=0x00;/*nodiac*/
      gc_sbo='['; gs_sbo[0]='['; gs_sbo[1]=0x00;/*nodiac*/
      gc_bsl='\\'; gs_bsl[0]='\\'; gs_bsl[1]=0x00;/*nodiac*/
      gc_sbc=']'; gs_sbc[0]=']'; gs_sbc[1]=0x00;/*nodiac*/
      gc_crt='^'; gs_crt[0]='^'; gs_crt[1]=0x00;/*nodiac*/
      gc_grv='`'; gs_grv[0]='`'; gs_grv[1]=0x00;/*nodiac*/
      gc_cbo='{'; gs_cbo[0]='{'; gs_cbo[1]=0x00;/*nodiac*/
      gc_vbr='|'; gs_vbr[0]='|'; gs_vbr[1]=0x00;/*nodiac*/
      gc_cbc='}'; gs_cbc[0]='}'; gs_cbc[1]=0x00;/*nodiac*/
      gc_tld='~'; gs_tld[0]='~'; gs_tld[1]=0x00;/*nodiac*/
      break;
   }
   gs_svb[0]='=';
   gs_svb[1]=gs_vbr[0];
   gs_svb[2]=0x00;
   gs_sbs[0]='/';
   gs_sbs[1]=gs_bsl[0];
   gs_sbs[2]=0x00;
   gs_idt[0]='-';
   gs_idt[1]='-';
   gs_idt[2]=gs_vbr[0];
   gs_idt[3]=0x00;
   return(*p);
}

static char* init_string(char* p) {
   init_char(&gc_exc);
   return(p);
}

#  define C_EXC               ((gc_exc)?gc_exc:init_char(&gc_exc))
#  define C_HSH               ((gc_hsh)?gc_hsh:init_char(&gc_hsh))
#  define C_DLR               ((gc_dlr)?gc_dlr:init_char(&gc_dlr))
#  define C_ATS               ((gc_ats)?gc_ats:init_char(&gc_ats))
#  define C_SBO               ((gc_sbo)?gc_sbo:init_char(&gc_sbo))
#  define C_BSL               ((gc_bsl)?gc_bsl:init_char(&gc_bsl))
#  define C_SBC               ((gc_sbc)?gc_sbc:init_char(&gc_sbc))
#  define C_CRT               ((gc_sbo)?gc_crt:init_char(&gc_crt))
#  define C_GRV               ((gc_grv)?gc_grv:init_char(&gc_grv))
#  define C_CBO               ((gc_cbo)?gc_cbo:init_char(&gc_cbo))
#  define C_VBR               ((gc_vbr)?gc_vbr:init_char(&gc_vbr))
#  define C_CBC               ((gc_cbc)?gc_cbc:init_char(&gc_cbc))
#  define C_TLD               ((gc_tld)?gc_tld:init_char(&gc_tld))
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

