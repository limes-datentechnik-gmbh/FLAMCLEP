/**
 * @file FLAMCLE.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
 * @author limes datentechnik gmbh
 * @date 06.03.2015\n
 * @copyright (c) 2015 limes datentechnik gmbh
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 * <pre>
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 * </pre>
 *
 * If you need professional services or support for this library please
 * contact support@flam.de.
 *
 **********************************************************************/

/*! @cond PRIVATE */
#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_FLAMCLE_H
#define INC_FLAMCLE_H
/*! @endcond */

#include "CLEDEF.h"

/*! @cond PRIVATE */
#ifdef __HOSTSHORTING__
#  define pcCleVersion           FLCLEVSN
#  define pcCleAbout             FLCLEABO
#  define siCleExecute           FLCLEEXE
#endif
/*! @endcond */

/**
 * @defgroup CLE_CLEFUC CLE Functions
 * @brief The function provided by CLE.
 * @{
 */

/**
* @brief Get CLE-version information
*
* The function returns the version information for this library
*
* @param[in]    l level of visible hierarchy in the first 2 numbers of the string
*                 the number can later be used to better visualize the hierarchy
* @param[in]    s size of the provided string buffer (including space for null termination)
* @param[inout] b buffer for the version string
*                 must contain a null-terminated string
*                 the version string will be concatenated
*                 the size including the 0-byte is the limit
*                 if (strlen(b)==s-1) then more space is required for the complete version string
*                 a good size for the version string is 256 byte
*
* @return         pointer to a null-terminated version string (return(b))
*/
extern const char* pcCleVersion(const int l, const int s, char* b);

/**
* @brief Get about CLE-information
*
* The function returns the about information for this library
*
* @param l level of visible hierarchy in the first 2 numbers of the string
*          the number can later be used to better visualize the hierarchy
* @param s size of the provided string buffer (including space for null termination)
* @param b buffer for the about string
*          must contain a null-terminated string
*          the about string will be concatenated
*          the size including the 0-byte is the limit
*          if (strlen(b)==s-1) then more space is required for the complete about string
*          a good size for the about string is 1024 byte
*
* @return pointer to a null-terminated about string (return(b))
*/
extern const char* pcCleAbout(const int l, const int s, char* b);

/**
 * @brief Execute CLE-command line
 *
 * The function uses the command line parsers to execute different commands based on argc and argv given in the main function of a program
 * and provides the additional built-in functions below:
 * - SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]
 * - HELP   [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]
 * - MANPAGE [function | command[.path][=filename]] | [filename]
 * - GENDOCU [command[.path]=]filename [NONBR] [SHORT]
 * - HTMLDOC [path] [NUMBERS]
 * - GENPROP [command=]filename
 * - SETPROP [command=]filename
 * - CHGPROP command [path[=value]]*
 * - DELPROP [command]
 * - GETPROP [command[.path] [DEPTH1 | ... | DEPTH9 | DEPALL | DEFALL]]
 * - SETOWNER name
 * - GETOWNER
 * - SETENV variable=name
 * - GETENV
 * - DELENV variable
 * - TRACE ON | OFF | FILE=filename
 * - CONFIG [CLEAR]
 * - GRAMMAR
 * - LICENSE
 * - LEXEM
 * - VERSION
 * - ABOUT
 * - ERRORS
 *
 * @param[in]  pvGbl Pointer to a global handle given to called functions in the command table
 * @param[in]  psCmd Pointer to the table which defines the commands
 * @param[in]  argc  Number of command line parameters (argc of main(int argc, char* argv[]))
 * @param[in]  argv  List of pointers to the command line parameters (argv of main(int argc, char* argv[]))
 * @param[in]  pcOwn Default owner id (owner ids are used to identify properties and other things "com.company")
 * @param[in]  pcPgm Logical program name (can be different from argv[0] and will be used in the root "com.company.program")
 * @param[in]  pcAut Name of the author for ASCIIDOC header (required for header generation)
 * @param[in]  pcAdr Mail address of the author for the ASCIIDOC header (optional)
 * @param[in]  isCas Switch to enable case sensitive interpretation of the command line (recommended is FLASE)
 * @param[in]  isPfl Switch to enable parameter file support for object, overlays and arrays (recommended is TRUE)
 * @param[in]  isRpl Switch to enable replacement of environment variables (recommended is TRUE)
 * @param[in]  isEnv Switch to load environment variables from default files (recommended is TRUE if no own load done else FALSE)
 * @param[in]  siMkl Integer defining the minimal key word length (siMkl<=0 --> full length, no auto abbreviation)
 * @param[in]  pfOut File pointer for help and error messages (if not defined stderr will be used)
 * @param[in]  pfTrc Default trace file if no trace file is defined with the configuration data management (recommended: NULL, stdout or stderr)
 * @param[in]  pcDep String to visualize hierarchies (recommended: "--|" converted on EBCDIC systems (don't use S_IDT))
 * @param[in]  pcOpt String to separate options (recommended: "/" converted on EBCDIC systems)
 * @param[in]  pcEnt String to separate list entries (recommended: "," converted on EBCDIC systems)
 * @param[in]  pcLic String containing the license information for this program (used by built-in function LICENSE - not converted on EBCDIC systems (don't use dia-critical characters))
 * @param[in]  pcBld String containing the build number / raw version for this program (optional, can be NULL) used in final message and replacements - converted on EBCDIC systems
 * @param[in]  pcVsn String containing the version information for this program (used by built-in function VERSION - not converted on EBCDIC systems (don't use dia-critical characters))
 * @param[in]  pcAbo String containing the about message for this program (used by built-in function ABOUT - not converted on EBCDIC systems (don't use dia-critical characters))
 * @param[in]  pcHlp Short help message for the whole program (converted on EBCDIC systems)
 * @param[in]  pcDef Default command or built-in function, which is executed if the first keyword (argv[1]) don't match (if NULL then no default)
 * @param[in]  pfMsg Pointer to a function which prints a message for an reason code (use to generate the corresponding appendix)\n
 * @param[in]  psOth Pointer to the table with other CLP strings to print (optional could be NULL)
 * @param[in]  pvF2S Pointer to a handle which can be used in file 2 string callback function (if not required then NULL)
 * @param[in]  pfF2S Callback function which reads a file into a null-terminated string in memory (if NULL then default implementation is used)
 * @param[in]  pvSaf Pointer to a handle which can be used in authorization callback function (if not required then NULL)
 * @param[in]  pfSaf Callback function for additional authorization by CLP or NULL if no authorization is requested
 * @param[in]  pcDpa Pointer to a file name for a default parameter file (e.g. "DD:FLAMPAR") or NULL/empty string for nothing,
 *                   The file name is used if only a command without assignment or parameter is provided
 * @param[in]  siNoR Define this reason code to the values the mapping function returns if no run is requested (0 is nothing)
 * @param[in]  psDoc Table for documentation generation (must be defined)
 *
 * @return signed integer with the condition codes below:\n
 * 0  - command line, command syntax, mapping, execution and finish of the command was successful\n
 * 1  - command line, command syntax, mapping, execution and finish of the command was successful but a warning can be found in the log\n
 * 2  - command line, command syntax, mapping, execution was successful but cleanup of the command failed (may not happened)\n
 * 4  - command line, command syntax and mapping was successful but execution of the command returns with a warning\n
 * 8  - command line, command syntax and mapping was successful but execution of the command returns with an error\n
 * 12 - command line and command syntax was OK but mapping failed\n
 * 16 - command line was OK but command syntax was wrong\n
 * 20 - command line was wrong (user error)\n
 * 24 - initialization of parameter structure for the command failed (may not happened)\n
 * 28 - configuration is wrong (user error)\n
 * 32 - table error (something within the predefined tables is wrong)\n
 * 36 - system error (mainly memory allocation or some thing like this failed)\n
 * 40 - access control or license error\n
 * 44 - interface error (parameter pointer equals to NULL or something like this)\n
 * 48 - memory allocation failed (e.g. dynamic string handling)\n
 * 64 - fatal error (basic things are damaged)\n
 *>64 - Special condition code for job control\n
 */
extern int siCleExecute(
   void*                         pvGbl,
   const TsCleCommand*           psCmd,
   int                           argc,
   char*                         argv[],
   const char*                   pcOwn,//
   const char*                   pcPgm,//
   const char*                   pcAut,//
   const char*                   pcAdr,//
   const int                     isCas,//
   const int                     isPfl,//
   const int                     isRpl,//
   const int                     isEnv,
   const int                     siMkl,//
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,//
   const char*                   pcOpt,//
   const char*                   pcEnt,//
   const char*                   pcLic,
   const char*                   pcBld,//
   const char*                   pcVsn,//
   const char*                   pcAbo,//
   const char*                   pcHlp,//
   const char*                   pcDef,
   TfMsg*                        pfMsg,//
   const TsCleOtherClp*          psOth,//
   void*                         pvF2S,//
   TfF2S*                        pfF2S,//
   void*                         pvSaf,//
   TfSaf*                        pfSaf,//
   const char*                   pcDpa,//
   const int                     siNoR,
   const TsCleDoc*               psDoc);
/** @}*/
/**********************************************************************/
/*! @cond PRIVATE */

extern int siCleParseString(
    const int                     uiErr,
    char*                         pcErr,
    const int                     isCas,
    const int                     isPfl,
    const int                     isRpl,
    const int                     siMkl,
    const char*                   pcOwn,
    const char*                   pcPgm,
    const char*                   pcBld,
    const char*                   pcCmd,
    const char*                   pcMan,
    const char*                   pcHlp,
    const int                     isOvl,
    const char*                   pcStr,
    const TsClpArgument*          psCmd,
    const char*                   pcDep,
    const char*                   pcOpt,
    const char*                   pcEnt,
    int*                          piMod,
    void*                         pvDat,
    void*                         pvGbl,
    void*                         pvF2S,
    TfF2S*                        pfF2S,
    void*                         pvSaf,
    TfSaf*                        pfSaf,
    void**                        ppClp);

/**********************************************************************/

#endif /*INC_FLAMCLE_H*/

#ifdef __cplusplus
}
#endif
/*! @endcond */
