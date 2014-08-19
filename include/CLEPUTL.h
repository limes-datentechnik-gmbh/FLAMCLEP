/**
 * @file
 * @brief  Deklaration diverser Hilfsfunktionen in ANSI C
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

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLEPUTL_H
#define INC_CLEPUTL_H

#include"FLAMCLP.h"

#ifndef L_userid
   #define L_userid        257
#endif
#ifndef L_filnam
   #define L_filnam        1025
#endif

#define isKyw(c) (isalnum(c) || (c)=='_' || (c)=='-')

/**********************************************************************/

/**
 *
 * @return
 */
extern const char* CUSERID(void);

/**
 * @param flg
 * @return
 */
extern const char* HOMEDIR(int flg);

/**
 *
 * @param hex
 * @param bin
 * @param len
 * @return
 */
extern U32 hex2bin(
   const C08*           hex,
         U08*           bin,
   const U32            len);

/**
 *
 * @param chr
 * @param asc
 * @param len
 * @return
 */
extern U32 chr2asc(
   const C08*           chr,
         C08*           asc,
   const U32            len);

/**
 *
 * @param chr
 * @param asc
 * @param len
 * @return
 */
extern U32 chr2ebc(
   const C08*           chr,
         C08*           asc,
   const U32            len);

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
 * Compare of two string
 *
 * The procedure combines strcmp, stricmp, strncmp and strchr in one function.
 *
 * @param[in]  ca Flag if case sensitiv (TRUE) or not (FALSE)
 * @param[in]  s1 String 1 to compare
 * @param[in]  s2 string 2 to compare
 * @param[in]  n  Amount of character to compare
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

