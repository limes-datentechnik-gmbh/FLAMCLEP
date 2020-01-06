/**
 * @file CLEDEF.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
 *
 * LIMES Command Line Executor (FLAMCLE) in ANSI-C
 * @author limes datentechnik gmbh
 * @date 27.12.2019\n
 * @copyright (c) 2019 limes datentechnik gmbh
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

***********************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLEDEF_H
#define INC_CLEDEF_H

#include "stdio.h"
#include "CLPDEF.h"

/**********************************************************************/

/**
* Documentation types used in table below
*/
#define CLE_DOCTYP_COVER                  1U    /** Cover page (level must be 1)*/
#define CLE_DOCTYP_CHAPTER                2U    /** A chapter (level must > 1 and < 6)*/
#define CLE_DOCTYP_PROGRAM                10U   /** The main program chapter (like chapter but level must < 5)*/
#define CLE_DOCTYP_PGMSYNOPSIS            11U   /** The program synopsis*/
#define CLE_DOCTYP_PGMSYNTAX              12U   /** The program syntax*/
#define CLE_DOCTYP_PGMHELP                13U   /** The program help*/
#define CLE_DOCTYP_COMMANDS               20U   /** The commands part*/
#define CLE_DOCTYP_OTHERCLP               21U   /** Other CLP strings*/
#define CLE_DOCTYP_BUILTIN                22U   /** The built-in function section*/
#define CLE_DOCTYP_LEXEM                  30U   /** The appendix which prints the lexems*/
#define CLE_DOCTYP_GRAMMAR                31U   /** The appendix which prints the grammar*/
#define CLE_DOCTYP_VERSION                32U   /** The appendix which prints the version (pcVsn must be given)*/
#define CLE_DOCTYP_ABOUT                  33U   /** The appendix which prints the about (pcAbo must be given)*/
#define CLE_DOCTYP_PROPREMAIN             41U   /** The appendix which prints the remaining parameter documentation*/
#define CLE_DOCTYP_PROPDEFAULTS           42U   /** The appendix which prints the default parameter documentation*/
#define CLE_DOCTYP_SPECIALCODES           51U   /** The appendix which prints the special condition codes*/
#define CLE_DOCTYP_REASONCODES            52U   /** The appendix which prints the reason codes (pfMsg must be provided)*/

/**
* ASCIIDOC key words used in table below
*/
#define CLE_DOCKYW_PREFACE                "preface"  /** Mark level 2 chapter as preface*/
#define CLE_DOCKYW_APPENDIX               "appendix" /** Mark level 2 chapter as appendix*/
#define CLE_DOCKYW_GLOSSARY               "glossary" /** Mark level 2 chapter as glossary*/
#define CLE_DOCKYW_COLOPHON               "colophon" /** Mark level 2 chapter as colophon*/

/**
 * The anchors for standard chapters
 *
 * This anchors are required to fulfill the CLEP internal links
 * and must be assigned to the corresponding chapters in the table below.
 */
#define CLE_ANCHOR_BUILTIN_FUNCTIONS      "CLEP.BUILTIN.FUNCTIONS"
#define CLE_ANCHOR_APPENDIX_ABOUT         "CLEP.APPENDIX.ABOUT"         /** Appendix About*/
#define CLE_ANCHOR_APPENDIX_VERSION       "CLEP.APPENDIX.VERSION"       /** Appendix Version*/
#define CLE_ANCHOR_APPENDIX_LEXEM         "CLEP.APPENDIX.LEXEM"         /** Appendix Lexem*/
#define CLE_ANCHOR_APPENDIX_GRAMMAR       "CLEP.APPENDIX.GRAMMAR"       /** Appendix Grammar*/
#define CLE_ANCHOR_APPENDIX_RETURNCODES   "CLEP.APPENDIX.RETURNCODES"   /** Appendix Returncodes*/
#define CLE_ANCHOR_APPENDIX_REASONCODES   "CLEP.APPENDIX.REASONCODES"   /** Appendix Reasoncodes*/
#define CLE_ANCHOR_APPENDIX_PROPERTIES    "CLEP.APPENDIX.PROPERTIES"    /** Appendix Properties*/

/**
 * \struct TsCleDoc
 *
 * This structure is used to define a table used to generate the documentation\n
 *
 * Its members are:
 * \par
 * \b uiTyp  One of the documentation types above\n
 * \b uiLev  The level of the chapter in the document (cover page is 1 all other chapter > 1)\n
 * \b pcNum  String for numbering or NULL for no number prefix\n
 * \b pcKyW  Optional ASCIIDOC key word (printed in front of headline in single square brackets)\n
 * \b pcAnc  Optional anchor for this chapter (printed in front of headline in double square brackets)\n
 * \b pcHdl  Headline for this chapter\n
 * \b pcMan  Optional or required manual page with the content of this chapter)\n
 * \b pcIdt  Optional new line separated list of index term for this chapter (printed at the end (indexterm:[]))\n
 */
typedef struct CleDoc {
   unsigned int      uiTyp;
   unsigned int      uiLev;
   const char*       pcNum;
   const char*       pcKyw;
   const char*       pcAnc;
   const char*       pcHdl;
   const char*       pcMan;
   const char*       pcIdt;
}TsCleDoc;

/** Starts the documentation generation table
 *  *name* Name of this table\n
 */
#define CLEDOC_OPN(name)               TsCleDoc name[]

/** Starts the documentation generation table
 *  *typ* Documentation type\n
 *  *lev* The level of the chapter\n
 *  *num* Prefix for number string\n
 *  *kyw* Optional ASCIIDOC key word (printed in front of headline in single square brackets)\n
 *  *anc* Optional anchor for this chapter (printed in front of headline in double square brackets)\n
 *  *hdl* Headline for this chapter\n
 *  *man* Optional manual page for this chapter\n
 *  *ind* Optional new line separated list of index terms for this chapter\n
 */
#define CLETAB_DOC(typ,lev,num,kyw,anc,hdl,man,idt)   {(typ),(lev),(num),(kyw),(anc),(hdl),(man),(idt)},

/** Ends a table with constant definitions
 */
#define CLEDOC_CLS                    {  0  ,  0  , NULL, NULL, NULL, NULL, NULL, NULL}

/**
 * Type definition for initialization function
 *
 * This function is used to initialize the FLAMCLP structure for a command.
 * The FLAMCLP structure is used by the command line processor (FLAMCLP) to save
 * the parsed values. The INI function can be used to pre-define values
 * in this structure. These values are still valid, if no value defined
 * over the properties or command line. The current owner and program
 * name are given by CleExecute to know these values at initialization.
 *
 * If additional dynamic memory required in the CLP structure the provided
 * handle can be used for pvClpAlloc.
 *
 * @param[in]  pvHdl Pointer to the CLP handle for allocation of memory in the CLP structure
 * @param[in]  pfOut File pointer for outputs (given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute)
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute)
 * @param[in]  pcOwn Current owner name (given over CleExecute)
 * @param[in]  pcPgm Current program name (given over CleExecute)
 * @param[out] pvClp Pointer to the corresonding FLAMCLP structure for initialisation
 *
 * @return     Reason code (!=0) for termination or 0 for success
 */
typedef int (TfIni)(
   void*                         pvHdl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   void*                         pvGbl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   void*                         pvClp);

/**
 * Type definition for mapping function
 *
 * This function is used to map the parsed values from the FLAMCLP structure
 * to the parameter structure of the corresponding command. This mapping
 * can simply a memcpy or pointer assignment but it is often useful to
 * make a real mapping for example:
 *
 * The executed subprogram needs a table with some definitions. These
 * definitions can easily managed in a file. Over the command line the
 * user define the file name and the mapping function open this file
 * read the definition, allocates the memory and stores the definition
 * in the parameter structure. This means that the user defines the file
 * name, but the executed subprogram gets the content of this file. Such
 * things are realized over a real mapping between the parsed values and
 * the needed arguments.
 *
 * For overlay based commands the pointer to the object identifier is
 * provided (taken from siCleExecute). This integer can then be used to
 * choose the correct data structure from the corresponding CLP union
 * for mapping. (The piOid can also (mis)used to give back a integer to
 * the caller of siCleExecute() from mapping if this pointer not NULL.)
 *
 * If additional dynamic memory required in the CLP structure the provided
 * handle can be used for pvClpAlloc.
 *
 * @param[in]  pvClp Pointer to the CLP handle for allocation of memory in the CLP structure
 * @param[in]  pfOut File pointer for outputs (mainly error messages, given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (mainly for complex stuff, given over CleExecute)
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute)
 * @param[in]  piOid Pointer to the object identifier for overlay commands, if the pointer set at siCleExecute()
 * @param[in]  pvClp Pointer to the filled FLAMCLP structure (output from the command line parser)
 * @param[out] pvPar Pointer to the parameter structure, which will be filled based on the FLAMCLP structure with this function
 *
 * @return     Reason code (!=0) for termination or 0 for success.
 *             If a no run reason code (!=0) defined then the run function is not executed
 */
typedef int (TfMap)(
   void*                         pvHdl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   void*                         pvGbl,
   int*                          piOid,
   void*                         pvClp,
   void*                         pvPar);

/**
 * Type definition for the run function
 *
 * The run function is used to execute the corresponding subprogram for
 * a command using the mapped parameter structure. For logging or other
 * purpose all collected information are also provided at input.
 *
 * The run function returns like all other functions executed by CLE a
 * reason code. Additional to the other functions there will be another
 * indicator to define the condition code (return code of the program).
 * For the run function 2 possible condition codes are defined. First an
 * error with value 8 and additional 4 in a case of a warning. The warning
 * means that the run don't fail, but something was happen.
 *
 * If additional dynamic memory required in the FLC structure (from mapping)
 * the provided handle can be used for pvClpAlloc.
 *
 * @param[in]  pvHdl Pointer to the CLP handle for allocation of memory in the FLC structure
 * @param[in]  pfOut File pointer for outputs (given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute)
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute)
 * @param[in]  pcOwn Current owner name (given over CleExecute)
 * @param[in]  pcPgm Current program name (given over CleExecute)
 * @param[in]  pcVsn Current version information (given from CleExecute)
 * @param[in]  pcAbo Current about information (given from CleExecute)
 * @param[in]  pcLic Current license text (given from CleExecute)
 * @param[in]  pcFkt Current function name (key word of the command)
 * @param[in]  pcCmd Current command (complete entered line of user)
 * @param[in]  pcLst Current list of parsed arguments (given from FLAMCLP, could be NULL or empty)
 * @param[in]  pvPar Pointer to the filled parameter for the run of the subprogram
 * @param[out] piWrn Pointer to an integer (the fist half word is true (0x0001), if warnings collated by directory walk, the second halfword is true (0x0001) if warnings are logged)
 * @param[out] piScc Pointer to an integer containing a special condition code (if greater CLERTC_MAX(64) then used instead of CLERTC_RUN(8))
 *
 * @return     Reason code (!=0) for termination or warning, 0 for success
 */
typedef int (TfRun)(
   void*                         pvHdl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   void*                         pvGbl,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcVsn,
   const char*                   pcAbo,
   const char*                   pcLic,
   const char*                   pcFkt,
   const char*                   pcCmd,
   const char*                   pcLst,
   const void*                   pvPar,
   int*                          piWrn,
   int*                          piScc);

/**
 * Type definition for the fin function
 *
 * This function is called at the end after the run to clean up the
 * parameter structure. For example, it could be there was a file pointer
 * which must be closed or memory which must be freed.
 *
 * @param[in]  pfOut File pointer for outputs (given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute)
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute)
 * @param[in]  pvPar Pointer to the filled parameter structure for cleanup
 * @return     Reason code (!=0) for termination or 0 for success
 */
typedef int (TfFin)(
   FILE*                         pfOut,
   FILE*                         pfTrc,
   void*                         pvGbl,
   void*                         pvPar);

/**
 * Type definition for the message function
 *
 * This function is called to generate a appendix for the reason codes
 * and to provide better messages in a case of an error.
 *
 * In a loop starting with 1 the messages are printed to the appendix.
 * If a NULL pointer (no message) returned the loop is finished. A empty
 * message ("") indicates an reason code which is not printed to the
 * appendix. Is a NULL pointer provided for this function the appendix
 * for the reason codes and additional error messages are not generated.
 *
 * @param[in]  siRsn Reason code from INI, MAP, RUN and FIN function
 * @return     Pointer to the corresponding message
 */
typedef const char* (TfMsg)(const int siRsn);

/**
 * \struct TsCleCommand
 *
 * This structure is used to define a table of commands\n
 *
 * To simplify the definition of command tables it is recommended to use the CLPTAB macros below.
 *
 * Its members are:
 * \par
 * \b pcKyw  Pointer to the key word for this command (:alpha:[:alnum:|'_']*)\n
 * \b psTab  Pointer to the main argument table for this command (defines the semantic for the parser)\n
 * \b pvClp  Pointer to the corresponding argument structure (filled up by the parser)\n
 * \b pvPar  Pointer to the corresponding parameter structure (filled up by the mapping function)\n
 * \b pfIni  Pointer to the initialization function (initialize the argument structure in front of parsing)\n
 * \b pfMap  Pointer to the mapping function (transfers the argument structure to the parameter structure)\n
 * \b pfRun  Pointer to the executed function (use the mapped parameter structure to execute the command
 *           (for logging the function name, original command line and parsed argument list and other values are also provided))\n
 * \b pfFin  Pointer to the finish function for cleanup (free memory, close files in parameter structure)\n
 * \b siFlg  Flag to indicate a hidden (==0) or visible (!=0) command,
 *           For correct numbering, put hidden commands to the end of the table\n
 * \b pcMan  Pointer to a null-terminated string for a detailed description of this command
 *           (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES)
 *           It is recommended to use a header file with a define for this long string.
 *           "&{OWN}" and "&{PGM}" are replaced with the current owner and program name.
 *           The resulting text is converted on EBCDIC systems)\n
 * \b pcHlp  String for a short context sensitive help to this command (converted on EBCDIC systems)\n
 */
typedef struct CleCommand {
   const char*                   pcKyw;
   const TsClpArgument*          psTab;
   void*                         pvClp;
   void*                         pvPar;
   int*                          piOid;
   TfIni*                        pfIni;
   TfMap*                        pfMap;
   TfRun*                        pfRun;
   TfFin*                        pfFin;
   int                           siFlg;
   const char*                   pcMan;
   const char*                   pcHlp;
} TsCleCommand;

/** Starts a table with command definitions
 *
 *  *nam* Name of this table\n
 */
#define CLETAB_OPN(name)         TsCleCommand name[]

/** defines a command with the command line keyword *kyw*
 *
 *  *tab* Pointer to the main table for this command.\n
 *  *clp* Pointer to the corresponding FLAMCLP structure (generated with the CLPMAC.h marcros).\n
 *  *par* Pointer to the corresponding parameter structure.\n
 *  *oid* Pointer to an integer to define the main table as overlay or NULL to define the main table as object
 *        If the pointer is set the object identifier of the chosen argument of the overlay is given back.\n
 *  *ini* Pointer to the initialization function for the FLAMCLP structure (see TfIni)\n
 *  *map* Pointer to the mapping function (see TfMap).
 *        The mapping functions maps the parsed content of the FLAMCLP structure in the PAR structure.\n
 *  *run* Pointer to the run function to execute the subprogram with the PAR structure (see TfRun).\n
 *  *fin* Pointer to the finalization function to clean up the parameter structure (see TfFin)\n
 *  *flg* Flag to indicate a hidden (==0) or visible (!=0) command,
 *        For correct numbering, put hidden commands to the end of the table\n
 *  *man* Pointer to a null-terminated string for a detailed description of this command.
 *        (in ASCIDOC format, content behind .DESCRIPTION, usually some paragraphs plus .OPTIONS and/or .EXAMPLES)
 *        It is recommended to use a header file with a define for this long string)\n
 *  *hlp* String for a short context sensitive help to this command\n
 */
#define CLETAB_CMD(kyw,tab,clp,par,oid,ini,map,run,fin,flg,man,hlp)   {(kyw),(tab),(clp),(par),(oid),(ini),(map),(run),(fin),(flg),(man),(hlp)},

/** Ends a table with constant definitions
 */
#define CLETAB_CLS                                                    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0   , NULL, NULL}

/**
 * \struct TsCleOtherClp
 *
 * This structure is used to define a table of CLP strings add as appendix to the generated documentation\n
 *
 * Its members are:
 * \par
 * \b pcRot  Pointer to the program/root key word for this string (:alpha:[:alnum:|'_']*)\n
 * \b pcKyw  Pointer to the key word for this string (:alpha:[:alnum:|'_']*)\n
 * \b psTab  Pointer to the main argument table for this CLP string\n
 * \b pcMan  Pointer to a null-terminated string for a detailed description for this CLP string
 *           (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES)
 *           It is recommended to use a header file with a define for this long string.
 *           "&{OWN}" and "&{PGM}" are replaced with the current owner and program name.
 *           The resulting text is converted on EBCDIC systems)\n
 * \b pcHlp  String for a short context sensitive help for this CLP string (converted on EBCDIC systems)\n
 * \b isOvl  True if provided table must be interpreted as overlay else as object\n
 */
typedef struct CleOtherClp {
   const char*                   pcRot;
   const char*                   pcKyw;
   const TsClpArgument*          psTab;
   const char*                   pcMan;
   const char*                   pcHlp;
   const int                     isOvl;
} TsCleOtherClp;

/** Starts a table with other CLP strings
 *
 *  *nam* Name of this table\n
 */
#define CLEOTH_OPN(name)               TsCleOtherClp name[]

/** defines a appendix for the object or overlay *cmd* of the root *rot* with the headline of *hdl* for a certain other CLP string
 *
 * *rot*  Pointer to the program/root key word for this string (:alpha:[:alnum:|'_']*)\n
 * *kyw*  Pointer to the key word for this string (:alpha:[:alnum:|'_']*)\n
 * *hdl*  Pointer to the headline for the sub chapter for this CLP string (:alpha:[:alnum:|'_']*)\n
 * *tab*  Pointer to the main argument table for this CLP string\n
 * *man*  Pointer to a null-terminated string for a detailed description for this CLP string
 *           (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES)
 *           It is recommended to use a header file with a define for this long string.
 *           "&{OWN}" and "&{PGM}" are replaced with the current owner and program name.
 *           The resulting text is converted on EBCDIC systems)\n
 * *hlp*  String for a short context sensitive help for this CLP string (converted on EBCDIC systems)\n
 * *ovl*  True if provided table must be interpreted as overlay else as object\n
 */
#define CLETAB_OTH(rot,kyw,tab,man,hlp,ovl)  {(rot),(kyw),(tab),(man),(hlp),(ovl)},

/** Ends a table with other CLP strings
 */
#define CLEOTH_CLS                           { NULL, NULL, NULL, NULL, NULL, 0}

/**
 * Function 'opnHtmlDoc' of library 'libhtmldoc' called if built-in function HTMLDOC used
 *
 * The built-in function HTMLDOC use a service provider interface to create the documentation
 * using a callback function for each page/chapter. This function is called in front of the
 * generation process to establish a handle for the callback function.
 *
 * The resulting handle is given to the callback function TfClpPrintPage defined by CLP. This
 * callback function is called for each page/chapter written. After the documentation generation
 * process the handle will be released with the function TfCleClosePrint.
 *
 * @param pfOut   File pointer for normal output (NULL for no output)
 * @param pfErr   File pointer for error messages (NULL for no output)
 * @param pcPat   Path where the documentation is written to
 * @param pcOwn   String of the current Owner
 * @param pcPgm   String with the current program name
 * @param pcBld   Optional build/version number (could be NULL)
 * @param piAnc   Boolean to define if anchors generated by CLEP
 * @param piIdt   Boolean to define if index terms generated by CLEP
 * @param piPat   Boolean to define if path string part of the synopsis
 * @param piSep   Character to separate parts of the file paht (use '/' for sub folders or '-' to get file names)
 * @param piChr   Character to replace non alpha-numerical characters in the file name
 *
 * @return        Pointer to an handle or NULL if open failed
 */
typedef void* (TfCleOpenPrint)(FILE* pfOut, FILE* pfErr, const char* pcPat, const char* pcOwn, const char* pcPgm, const char* pcBld, int* piAnc, int* piIdt, int* piPat, int* psPs1, int* piPs2, int* piPr3);

/**
 * Function 'clsHtmlDoc' of library 'libhtmldoc' called if built-in function HTMLDOC used
 *
 * The built-in function HTMLDOC use a service provider interface to create the documentation
 * using a callback function for each page/chapter. This function is called after the
 * generation process to free resources associated with this handle. The handle will be generated
 * with the function TfCleOpenPrint in front of documentation generation.
 *
 * @param pvHdl   Pointer the the print handle
 *
 * @return        Return code (0 is OK else error)
 */
typedef int (TfCleClosePrint)(void* pvHdl);

/**********************************************************************/

#endif /*INC_CLEDEF_H*/

#ifdef __cplusplus
}
#endif
