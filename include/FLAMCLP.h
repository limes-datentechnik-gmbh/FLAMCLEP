/**********************************************************************
 * @file FLAMCLP.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>P</b>arsing
 * @author limes datentechnik gmbh
 * @date 26.01.2017
 * @copyright (c) 2017 limes datentechnik gmbh
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

/*! @cond PRIVATE */
#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLP_H
#define INC_CLP_H
/*! @endcond */

#include <stdio.h>
#include "CLPDEF.h"

/**********************************************************************/
/*! @cond PRIVATE */
#ifdef __HOSTSHORTING__
   #define pcClpVersion          FLCLPVSN
   #define pcClpAbout            FLCLPABO
   #define pvClpOpen             FLCLPOPN
   #define siClpParsePro         FLCLPRSP
   #define siClpParseCmd         FLCLPRSC
   #define siClpProperties       FLCLPPRO
   #define siClpSyntax           FLCLPSYN
   #define siClpHelp             FLCLPHLP
   #define siClpDocu             FLCLPDOC
   #define siClpPrint            FLCLPPRN
   #define siClpLexemes          FLCLPLEX
   #define siClpGrammar          FLCLPGRM
   #define vdClpClose            FLCLPCLS
   #define pcClpError            FLCLPERR
#endif
/*! @endcond */
/**********************************************************************/

/**
 * @defgroup CLP_FUNC CLP Functions
 * @brief The function provided by CLP.
 * @{
 */

/**
 * @brief Get version information
 *
 * The function returns the version information for this library
 *
 * @param[in]    l Level of visible hierarchy in the first 2 numbers of the string
 *                 the number can later be used to better visualize the hierarchy
 * @param[in]    s Size of the provided string buffer (including space for null termination)
 * @param[inout] b Buffer for the version string.
 *                 Must contain a null-terminated string.
 *                 The version string will be concatenated.
 *                 The size including the 0-byte is the limit.
 *                 If (strlen(b)==s-1) then more space is required for the complete version string.
 *                 A good size for the version string is 128 byte.
 *
 * @return Pointer to a null-terminated version string (return(b))
 */
extern const char* pcClpVersion(const int l, const int s, char* b);

/**
 * @brief Get about information
 *
 * The function returns the about information for this library
 *
 * @param[in]     l Level of visible hierarchy in the first 2 numbers of the string
 *                  the number can later be used to better visualize the hierarchy
 * @param[in]     s Size of the provided string buffer (including space for null termination)
 * @param[inout]  b Buffer for the about string.
 *                  Must contain a null-terminated string.
 *                  The about string will be concatenated.
 *                  The size including the 0-byte is the limit.
 *                  If (strlen(b)==s-1) then more space is required for the complete about string.
 *                  A good size for the about string is 512 byte.
 *
 * @return pointer to a null-terminated about string (return(b))
 */
extern const char* pcClpAbout(const int l, const int s, char* b);

/**
 * @brief Open command line parser
 *
 * The function uses the argument table and corresponding structure and creates the handle for the command line parser (FLAMCLP)
 *
 * @param[in]  isCas Boolean to enable case sensitive parsing of keywords (recommended is FALSE)
 * @param[in]  isPfl Boolean to enable parameter files per object and overlay (recommended is TRUE(1), if you provide 2 the parameter file is not parsed but the syntax is accepted)
 * @param[in]  isEnv Boolean to enable replacement of environment variables (recommended is TRUE)
 * @param[in]  siMkl Integer defining the minimal key word length (siMkl<=0 --> full length, no auto abbreviation)
 * @param[in]  pcOwn String constant containing the owner name for the root in the path ("limes")
 * @param[in]  pcPgm String constant containing the program name for the root in the path ("flcl")
 * @param[in]  pcBld String constant containing the build/version string for replacement
 * @param[in]  pcCmd String constant containing the command name for the root in the path ("CONV")
 * @param[in]  pcMan String constant containing the manual page for this command (converted on EBCDIC systems)
 * @param[in]  pcHlp String constant containing the help message for this command (converted on EBCDIC systems)
 * @param[in]  isOvl Boolean if TRUE the main table (psTab) is a overlay else it will be interpreted as object
 * @param[in]  psTab Pointer to the parameter table defining the semantic of the command line
 * @param[out] pvDat Pointer to the structure where the parsed values are stored (can be NULL if command line parsing not used)
 * @param[in]  pfHlp Pointer to the file used for help messages (if not set then stderr)
 * @param[in]  pfErr Pointer to the file used for error messages or NULL for no printing
 * @param[in]  pfSym Pointer to the file used for symbol table trace or NULL for no printing
 * @param[in]  pfScn Pointer to the file used for scanner trace or NULL for no printing
 * @param[in]  pfPrs Pointer to the file used for parser trace or NULL for no printing
 * @param[in]  pfBld Pointer to the file used for builder trace or NULL for no printing
 * @param[in]  pcDep String used for hierarchical print outs (help, errors, trace (recommended S_IDT="--|"))
 * @param[in]  pcOpt String used to separate options (recommended "/")
 * @param[in]  pcEnt String used to separate list entries (recommended ",")
 * @param[out] psErr Pointer to the error structure. If the pointer != NULL the structure is filled with pointers to
 *                   certain error information in the CLP handle. If pfErr defined all error information are printed
 *                   by CLP. In this case these structure is not required. If pfErr==NULL you can use these structure
 *                   to gather all error information of CLP in memory. The pointer are only valid until vsClpClose().
 * @param[in]  pvGbl Pointer to the global handle as black box given over CleExecute
 * @param[in]  pvF2S Pointer to a handle which can be used in file 2 string callback function (if not required then NULL)
 * @param[in]  pfF2S Callback function which reads a file into a variable null-terminated string in memory (if NULL then default implementation is used)
 * @param[in]  pvSaf Pointer to a handle which can be used in authorization callback function (if not required then NULL)
 * @param[in]  pfSaf Callback function to authorize each write to CLP structure or NULL for no additional check
 *
 * @return void pointer to the memory containing the handle
 */
extern void* pvClpOpen(
   const int                     isCas,
   const int                     isPfl,
   const int                     isEnv,
   const int                     siMkl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcBld,
   const char*                   pcCmd,
   const char*                   pcMan,
   const char*                   pcHlp,
   const int                     isOvl,
   const TsClpArgument*          psTab,
   void*                         pvDat,
   FILE*                         pfHlp,
   FILE*                         pfErr,
   FILE*                         pfSym,
   FILE*                         pfScn,
   FILE*                         pfPrs,
   FILE*                         pfBld,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   TsClpError*                   psErr,
   void*                         pvGbl,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf);

/**
 * @brief Reset command line parser
 *
 * Required after an error which was handled by the calling application to parse properties or commands correctly
 *
 * @param[inout] pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 */
extern void vdClpReset(
   void*                         pvHdl);

/**
 * @brief Parse the property list
 *
 * The function parses the property list
 *
 * Attention: Property parsing only effects the default values in the symbol table and don't write anything
 * to the CLP structure. You must use the same CLP handle for property and command line parsing.
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcSrc Pointer to a null-terminated string containing the source name for the property list
 *                   Property list are mainly taken from a file. It is useful to provide this file name for error printing
 * @param[in]  pcPro Pointer to a null-terminated string containing the property list for parsing
 * @param[in]  isChk Boolean to enable (TRUE) or disable (FALSE) validation of the root in path
 *             (if FALSE then other properties are ignored, if TRUE then other properties are not possible)
 * @param[out] ppLst Pointer to the parsed parameter list (NULL = no list provided) in the CLP handle
 *
 * @return signed integer with CLP_OK (0 - nothing parsed) or an error code (CLPERR_xxxxxx (<0)) or the amount of parsed entities (>0)
 */
extern int siClpParsePro(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcPro,
   const int                     isChk,
   char**                        ppLst);

/**
 * @brief Parse the command line
 *
 * The function parses the command line and returns OK or the error code and error position (byte offset)
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcSrc Pointer to a null-terminated string containing the source name for the command line
 *                   If the command line taken from a file it is useful to provide this file name for error printing else use NULL.
 * @param[in]  pcCmd Pointer to a null-terminated string containing the command for parsing
 * @param[in]  isChk Boolean to enable (TRUE) or disable (FALSE) validation of minimum number of entries
 * @param[in]  isPwd Boolean to enable (TRUE) or disable (FALSE) '*** SECRET ***' replacement in parsed parameter list below
 * @param[out] piOid If this pointer is set and the main table is an overlay the corresponding object identifier is returned
 * @param[out] ppLst Pointer to the parsed parameter list (NULL = no list provided) in the CLP handle
 *
 * @return signed integer with CLP_OK (0 - nothing parsed) or an error code (CLPERR_xxxxxx (<0)) or the amount of parsed entities (>0)
 */
extern int siClpParseCmd(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcCmd,
   const int                     isChk,
   const int                     isPwd,
   int*                          piOid,
   char**                        ppLst);

/**
 * @brief Print command line syntax
 *
 * The function prints the command line syntax
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  isSkr If true the syntax will be printed in structured form
 * @param[in]  isMin If true each element will be prefixed with '!' for required and '?' for optional
 * @param[in]  siDep Depth of next levels to display (0-Nothing, 1-One Level, 2-Two Level, ..., <9-All)
 * @param[in]  pcPat Path (root.input...) to limit syntax to a certain level
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpSyntax(
   void*                         pvHdl,
   const int                     isSkr,
   const int                     isMin,
   const int                     siDep,
   const char*                   pcPat);


/**
 * @brief Give help message for given path
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcPat Path (root.input...) to limit help to a certain level
 *
 * @return string to message or emty message if error
 */
extern const char* pcClpInfo(
   void*                         pvHdl,
   const char*                   pcPat);

/**
 * @brief Print help for command line syntax
 *
 * The function prints the help strings for the command line syntax
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcPat Path (root.input...) to limit help to a certain level
 * @param[in]  siDep Depth of next levels to display (0-Manpage, 1-One Level, 2-Two Level, ..., <9-All)
 * @param[in]  isAli Print also aliases (if FALSE help don't show aliases)
 * @param[in]  isMan Print manpage if no further arguments are available
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpHelp(
   void*                         pvHdl,
   const int                     siDep,
   const char*                   pcPat,
   const int                     isAli,
   const int                     isMan);

/**
 * @brief Generate documentation for command line syntax
 *
 * The function generates the documentation for a whole command or if a path is assigned
 * a part of the command. The format will be ASCIIDOC. If one of the arguments
 * has a manual page (detailed description) a headline with numbering dependent from the keyword
 * is generated. After a generated synopsis including the help message, the path, the type
 * and the syntax the detailed description (pcMan) is printed. If one of theses arguments has
 * no manual page then a bullet list with the keyword, type, syntax and help message will be built.
 *
 * The headline will be 'number ARGUMENT keyword'. For arguments the headline level 3 '~'
 * is used. The same is valid for constant definitions, but in this case the headline level 4 '^'
 * and the key word 'CONSTANT' in Headline or 'SELECTIONS' in bullet list is used.
 *
 * Numbering of headlines for doc type book can be enabled or disabled. If a level given (>0) then
 * the headlines are prefixed with this amount of '=' for arguments and with one '=' more for
 * constants or selections. A valid minimum level would be 3 and the maximum should not greater
 * then 5.
 *
 * The manual page for the command is displayed with headline level 2 '-', if no manual page is
 * available the message below is shown:
 *
 *     'No detailed description available for this command.'
 *
 * The same is valid for objects and overlays, which means that at a minimum each command, overlay and object needs a
 * detailed description of all normal arguments can be printed as bullet list.
 *
 * There will be one level of headlines left for the CleExecute, where the program itself, the
 * commands, built-in functions and the appendix are separated.
 *
 * If only one level of depth is used for printing, then doc type book (isMan==FALSE) or manual page (isMan=TRUE) can be selected.
 * With doc type book the corresponding section of the user manual is generated, with manual page option the asciidoc MANPAGE format
 * is made.
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pfDoc File handle used to write the documentation in ASCIIDOC format
 * @param[in]  pcPat Path (root.input...) to limit documentation certain level
 * @param[in]  pcNum Leading number for table of contents ("1.2." used or not used depend on isNbr, NULL pointer cause an error)
 * @param[in]  pcKnd Qualifier for command/otherclp head line (Recommended: "COMMAND" or "STRING" NULL pointer cause an error)
 * @param[in]  isCmd If TRUE for command and FALSE for other CLP strings (required for anchor generation).
 * @param[in]  isDep If TRUE then all deeper parts are printed if FALSE then not.
 * @param[in]  isMan If TRUE then doc type MANPAGE will be generated else doc type book will be used.
 *                   (isMan==TRUE results automatically in isDep==FALSE)
 * @param[in]  isAnc Boolean to enable write of generated anchors for each command (only for doc type book)
 * @param[in]  isNbr Boolean to enable header numbering for generated documentation (only for doc type book)
 * @param[in]  isIdt Boolean to enable printing of generated index terms (only for doc type book)
 * @param[in]  isPat Boolean to enable printing of path as part of the synopsis (only for doc type book)
 * @param[in]  uiLev If > 0 then headlines are written with this amount of '=' in front instead of underlining (only for doc type book)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpDocu(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const char*                   pcPat,
   const char*                   pcNum,
   const char*                   pcKnd,
   const int                     isCmd,
   const int                     isDep,
   const int                     isMan,
   const int                     isAnc,
   const int                     isNbr,
   const int                     isIdt,
   const int                     isPat,
   const unsigned int            uiLev);

/**
 * @brief Generate documentation using a callback function
 *
 * This function works like siClpDocu, but it gives each page to a
 * callback function and don't print it to a certain file.
 *
 * @param[in]     pvHdl   Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]     pcFil   Prefix for file name building
 * @param[in]     pcNum   Leading number for table of contents ("1.2." used or not used depend on isNbr, NULL pointer cause an error)
 * @param[in]     pcKnd   Qualifier for command/otherclp head line (Recommended: "COMMAND" or "STRING" NULL pointer cause an error)
 * @param[in]     isCmd   If TRUE for command and FALSE for other CLP strings (required for anchor generation).
 * @param[in]     isDep   If TRUE then all deeper parts are printed if FALSE then not.
 * @param[in]     isAnc   Boolean to enable write of generated anchors for each command (only for doc type book)
 * @param[in]     isNbr   Boolean to enable header numbering for generated documentation (only for doc type book)
 * @param[in]     isShl   Boolean to enable short headline without type specification (only for doc type book)
 * @param[in]     isIdt   Boolean to enable printing of generated index terms (only for doc type book)
 * @param[in]     isPat   Boolean to enable printing of path as part of the synopsis (only for doc type book)
 * @param[in]     uiLev   If > 0 then headlines are written with this amount of '=' in front instead of underlining (only for doc type book)
 * @param[in]     siPs1   Character to separate parts to build filename outside command path (only for doc type book)
 * @param[in]     siPs2   Character to separate parts to build filename inside command path (only for doc type book)
 * @param[in]     siPr3   Character to replace non alpha-numerical characters in file names (only for doc type book)
 * @param[in]     pvPrn   Handle for the print callback function (created with TfCleOpenPrint (opnHtmlDoc))
 * @param[inout]  pfPrn   Pointer to the callback function TfClpPrintPage (prnHtmlDoc)
 *
 * @return        signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpPrint(
   void*                         pvHdl,
   const char*                   pcFil,
   const char*                   pcNum,
   const char*                   pcKnd,
   const int                     isCmd,
   const int                     isDep,
   const int                     isAnc,
   const int                     isNbr,
   const int                     isShl,
   const int                     isIdt,
   const int                     isPat,
   const unsigned int            uiLev,
   const int                     siPs1,
   const int                     siPs2,
   const int                     siPr3,
   void*                         pvPrn,
   TfClpPrintPage*               pfPrn);


/**
 * @brief Generate properties
 *
 * The function produces a property list with the current default values
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  siMtd Method for property printing (0-ALL, 1-Defined, 2-All but not defined as comment)
 * @param[in]  siDep Depth of next levels to print (1-One Level, 2-Two Level, ..., <9-All)
 * @param[in]  pcPat Path (root.input...) to limit the amount of properties
 * @param[in]  pfOut File pointer to write the property list (if NULL then pfHlp of FLAMCLP is used)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpProperties(
   void*                         pvHdl,
   const int                     siMtd,
   const int                     siDep,
   const char*                   pcPat,
   FILE*                         pfOut);

/**
 * @brief Print the lexems of the command line compiler
 *
 * The function prints the regular expressions of the command line compiler
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pfOut Pointer to the file descriptor used to print the regular expressions
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpLexemes(
   void*                         pvHdl,
   FILE*                         pfOut);

/**
 * @brief Print the grammar of the command line compiler
 *
 * The function prints the context free grammar of the command line compiler
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pfOut Pointer to the file descriptor used to print the grammar
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpGrammar(
   void*                         pvHdl,
   FILE*                         pfOut);

/**
 * @brief Close the command line parser
 *
 * The function releases the allocated resources in the handle. If dynamic allocation
 * of data fields used in the CLP structure, you can close the CLP handle except the
 * list of dynamic allocated pointer in the CLP structure. If the siMtd is set to
 * CLPCLS_MTD_KEP the CLP handle is still open and can later be used to free the
 * remaining pointers of the CLP structure. If the method EXC (except) used, then
 * the CLP handle is closed and the application must free the allocated memory in
 * the CLP structure. This is useful if you need the CLP no longer but the CLP data
 * structure must be valid. You can later call the vdClpClose function again to
 * release the dynamic allocated data fields of the CLP structure with method keep.
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  siMtd Define the close method (EXC/KEP/ALL)
 */
extern void vdClpClose(
   void*                         pvHdl,
   const int                     siMtd);

/**
 * @brief Allocate memory in CLP structure
 *
 * This function allocates memory for the CLP structure and can be used to extend
 * the structure where dynamic array or strings must be extended. If the pointer (pvPtr)
 * not NULL and not known by CLP the function pvClpAlloc is called like pvPtr=NULL.
 * This mechanism can be used to use a pointer to a literal or a static variable in the
 * initialization phase
 *
 * @param[in]    pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]    pvPtr Pointer of the dynamic allocated area or NULL if it is a new one
 * @param[in]    siSiz Required size for the dynamic area
 * @param[inout] piInd Pointer to the index of the memory area or NULL (improves performance)
 *
 * @return Pointer to allocated and initialized memory or NULL if error
 */
extern void* pvClpAlloc(
   void*                         pvHdl,
   void*                         pvPtr,
   int                           siSiz,
   int*                          piInd);

/**
 * @brief Provides error message
 *
 * The function provides a error message for the corresponding error code
 *
 * @param[in]  siErr Error code from parser
 */
extern char* pcClpError(
   int               siErr);

/** @}*/
/**********************************************************************/

/*! @cond PRIVATE */
/**
 * @brief Symbol table walk
 *
 * The function can be use to read the symbol table of CLP
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  uiOpr Operation on symbol table
 * @param[out] psSym Entry to read values from symbol table
 *
 * @return signed integer with 0 for end of list, > 0 for the type or < 0 an error code (CLPERR_xxxxxx)
 */
extern int siClpSymbolTableWalk(
   void*                         pvHdl,
   const unsigned int            uiOpr,
   TsClpSymWlk*                  psSym);

/**
 * @brief Symbol table update
 *
 * The function can be use to update values for symbol table entries
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[out] psSym Entry to read values from symbol table
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpSymbolTableUpdate(
   void*                         pvHdl,
   TsClpSymUpd*                  psSym);

/**********************************************************************/

#endif // INC_FLAMCLP_H

#ifdef __cplusplus
}
#endif
/*! @endcond */
