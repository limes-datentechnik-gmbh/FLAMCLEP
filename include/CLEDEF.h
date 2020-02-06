/**
 * @file CLEDEF.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
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

/*! @cond PRIVATE */
#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLEDEF_H
#define INC_CLEDEF_H
/*! @endcond */

#include "stdio.h"
#include "CLPDEF.h"

/**********************************************************************/
/**
 * @defgroup CLE_DOCTYP CLE Docu Types
 * @brief Documentation types used in columns of table below.
 * @{
 */
#define CLE_DOCTYP_COVER                  1U    /**< @brief Cover page (level must be 1). */
#define CLE_DOCTYP_CHAPTER                2U    /**< @brief A chapter (level must > 1 and < 6). */
#define CLE_DOCTYP_PROGRAM                10U   /**< @brief The main program chapter (like chapter but level must < 5). */
#define CLE_DOCTYP_PGMSYNOPSIS            11U   /**< @brief The program synopsis. */
#define CLE_DOCTYP_PGMSYNTAX              12U   /**< @brief The program syntax. */
#define CLE_DOCTYP_PGMHELP                13U   /**< @brief The program help. */
#define CLE_DOCTYP_COMMANDS               20U   /**< @brief The commands part. */
#define CLE_DOCTYP_OTHERCLP               21U   /**< @brief Other CLP strings. */
#define CLE_DOCTYP_BUILTIN                22U   /**< @brief The built-in function section. */
#define CLE_DOCTYP_LEXEMES                30U   /**< @brief The appendix which prints the lexemes. */
#define CLE_DOCTYP_GRAMMAR                31U   /**< @brief The appendix which prints the grammar. */
#define CLE_DOCTYP_VERSION                32U   /**< @brief The appendix which prints the version (pcVsn must be given). */
#define CLE_DOCTYP_ABOUT                  33U   /**< @brief The appendix which prints the about (pcAbo must be given). */
#define CLE_DOCTYP_PROPREMAIN             41U   /**< @brief The appendix which prints the remaining parameter documentation. */
#define CLE_DOCTYP_PROPDEFAULTS           42U   /**< @brief The appendix which prints the default parameter documentation. */
#define CLE_DOCTYP_SPECIALCODES           51U   /**< @brief The appendix which prints the special condition codes. */
#define CLE_DOCTYP_REASONCODES            52U   /**< @brief The appendix which prints the reason codes (pfMsg must be provided). */
/** @} */

/**
 * @defgroup CLE_DOCKYW CLE Docu Keywords
 * @brief ASCIIDOC key words used in fourth column of table below.
 * @{
 */
#define CLE_DOCKYW_PREFACE                "preface"  /**< @brief Mark level 2 chapter as preface. */
#define CLE_DOCKYW_APPENDIX               "appendix" /**< @brief Mark level 2 chapter as appendix. */
#define CLE_DOCKYW_GLOSSARY               "glossary" /**< @brief Mark level 2 chapter as glossary. */
#define CLE_DOCKYW_COLOPHON               "colophon" /**< @brief Mark level 2 chapter as colophon. */
/** @} */

/**
 * @defgroup CLE_DOCANC CLE Docu Anchors
 * @brief The anchors for chapters and appendixes (5. column).
 *
 * This anchors are required to fulfill the CLEP internal links
 * and must be assigned to the corresponding chapters in the table below.
 * @{
 */
#define CLE_ANCHOR_BUILTIN_FUNCTIONS      "CLEP.BUILTIN.FUNCTIONS"      /**< @brief Chapter built-in functions. */
#define CLE_ANCHOR_APPENDIX_ABOUT         "CLEP.APPENDIX.ABOUT"         /**< @brief Appendix About. */
#define CLE_ANCHOR_APPENDIX_VERSION       "CLEP.APPENDIX.VERSION"       /**< @brief Appendix Version. */
#define CLE_ANCHOR_APPENDIX_LEXEMES       "CLEP.APPENDIX.LEXEMES"       /**< @brief Appendix Lexemes. */
#define CLE_ANCHOR_APPENDIX_GRAMMAR       "CLEP.APPENDIX.GRAMMAR"       /**< @brief Appendix Grammar. */
#define CLE_ANCHOR_APPENDIX_RETURNCODES   "CLEP.APPENDIX.RETURNCODES"   /**< @brief Appendix Return codes. */
#define CLE_ANCHOR_APPENDIX_REASONCODES   "CLEP.APPENDIX.REASONCODES"   /**< @brief Appendix Reason codes. */
#define CLE_ANCHOR_APPENDIX_PROPERTIES    "CLEP.APPENDIX.PROPERTIES"    /**< @brief Appendix Properties. */
/** @}*/

/**
 * @defgroup CLE_DOCTAB CLE Docu Table
 * @brief The structure and corresponding macros are used to define a table in order to generate documentation.
 * @{
 */
/**
 * @brief CLE Structure for documentation table
 *
 * This structure is used to build a table with the macros #CLEDOC_OPN, #CLETAB_DOC and #CLEDOC_CLS.
 * This table must be provided to the function #siCleExecute for documentation generation using the
 * built-in functions GENDOCU and HTMLDOC.
 *
 */
typedef struct CleDoc {
   unsigned int      uiTyp; /**< @brief One of the documentation types above. */
   unsigned int      uiLev; /**< @brief The level of the chapter in the document (cover page is 1 all other chapter > 1). */
   const char*       pcNum; /**< @brief String for numbering or NULL for no number prefix. */
   const char*       pcKyw; /**< @brief Optional ASCIIDOC key word (printed in front of headline in single square brackets). */
   const char*       pcAnc; /**< @brief Optional anchor for this chapter (printed in front of headline in double square brackets). */
   const char*       pcHdl; /**< @brief Headline for this chapter. */
   const char*       pcMan; /**< @brief Optional or required manual page with the content of this chapter). */
   const char*       pcIdt; /**< @brief Optional new line separated list of index term for this chapter (printed at the end (indexterm:[])). */
}TsCleDoc;

/**
 * @brief Starts the documentation generation table overview.
 *
 * @param name  Name of this table. \n
 */
#define CLEDOC_OPN(name)               TsCleDoc name[]

/**
 * @brief Starts the documentation generation table
 *
 * @param[in] typ Documentation type.
 * @param[in] lev Level of the chapter.
 * @param[in] num Prefix for number string.
 * @param[in] kyw Optional ASCIIDOC key word (printed in front of headline in single square brackets).
 * @param[in] anc Optional anchor for this chapter (printed in front of headline in double square brackets).
 * @param[in] hdl Headline for this chapter.
 * @param[in] man Optional manual page for this chapter.
 * @param[in] idt Optional new line separated list of index terms for this chapter.
 *  */
#define CLETAB_DOC(typ,lev,num,kyw,anc,hdl,man,idt) {(typ),(lev),(num),(kyw),(anc),(hdl),(man),(idt)},

/**
 * @brief Ends a table with constant definitions
 */
#define CLEDOC_CLS                    {  0  ,  0  , NULL, NULL, NULL, NULL, NULL, NULL}
/** @}*/

/**
 * @defgroup CLE_FUNCPTR CLE Function Pointer (call backs)
 * @brief Type definitions for the callback functions used by CLE.
 * @{
 */

/**
 * @brief Function 'opnHtmlDoc' of library 'libhtmldoc' called if built-in function HTMLDOC used
 *
 * The built-in function HTMLDOC use a service provider interface to create the documentation
 * using a callback function for each page/chapter. This function is called in front of the
 * generation process to establish a handle for the callback function.
 *
 * The resulting handle is given to the callback function TfClpPrintPage defined by CLP. This
 * callback function is called for each page/chapter written. After the documentation generation
 * process the handle will be released with the function TfCleClosePrint.
 *
 * @param[in]    pfOut   File pointer for normal output (NULL for no output).
 * @param[in]    pfErr   File pointer for error messages (NULL for no output).
 * @param[in]    pcPat   Path where the documentation is written to.
 * @param[in]    pcOwn   String of the current Owner.
 * @param[in]    pcPgm   String with the current program name.
 * @param[in]    pcBld   Optional build/version number (could be NULL).
 * @param[inout] piHdr   Boolean to define if header information for cover page generated by CLEP.
 * @param[inout] piAnc   Boolean to define if anchors generated by CLEP.
 * @param[inout] piIdt   Boolean to define if index terms generated by CLEP.
 * @param[inout] piPat   Boolean to define if path string part of the synopsis.
 * @param[inout] piPs1   Character to separate parts before command strings of the file path (use '/' for sub folders or '-' to get file names).
 * @param[inout] piPs2   Character to separate parts inside command strings of the file path (use '/' for sub folders or '-' to get file names).
 * @param[inout] piPr3   Character to replace non alpha-numerical characters in the file name.
 *
 * @return Pointer to an handle or NULL if open failed
 */
typedef void* (TfCleOpenPrint)(FILE* pfOut, FILE* pfErr, const char* pcPat, const char* pcOwn, const char* pcPgm, const char* pcBld, int* piHdr, int* piAnc, int* piIdt, int* piPat, int* psPs1, int* piPs2, int* piPr3);

/**
 * @brief Function 'clsHtmlDoc' of library 'libhtmldoc' called if built-in function HTMLDOC used.
 *
 * The built-in function HTMLDOC use a service provider interface to create the documentation
 * using a callback function for each page/chapter.
 * A callback function is a function that is passed as a parameter to another function and is
 * called later by this function under defined conditions with defined arguments.
 *
 * This built-in function HTMLDOC is called after the generation process to free resources
 * associated with this handle. The handle will be generated with the function TfCleOpenPrint
 * in front of documentation generation.
 *
 * @param[in] pvHdl   Pointer the the print handle.
 *
 * @return            Return code (0 is OK else error).
 */
typedef int (TfCleClosePrint)(void* pvHdl);

/**********************************************************************/

/**
 * @brief Type definition for initialization FLAMCLE command structure.
 *
 * This FLAMCLE structure is used by the command line processor (FLAMCLP) to save
 * the parsed values.
 * The INI function uses this structure to pre-define values. These values are
 * still valid, if no values are defined over the properties or command line.
 *
 * The current owner and the program name are provided by the function CleExecute
 * (see siCleExecute()), to know these values during initialization.
 *
 * If additional dynamic memory is required in the CLP structure the provided
 * handle can be used for calling function pvClpAlloc.
 *
 * @param[in]  pvHdl Pointer to the CLP handle for allocation of memory in the CLP structure.
 * @param[in]  pfOut File pointer for outputs (given over CleExecute, see siCleExecute()).
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute, see siCleExecute()).
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute, see siCleExecute()).
 * @param[in]  pcOwn Current owner name (given over CleExecute, see siCleExecute()).
 * @param[in]  pcPgm Current program name (given over CleExecute, see siCleExecute()).
 * @param[out] pvClp Pointer to the corresonding FLAMCLP structure for initialisation.
 *
 * @return     Reason code (!=0) for termination or 0 for success.
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
 * @brief Type definition for mapping parsed values from the FLAMCLP structure.
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
 * provided (taken from siCleExecute()). This integer can then be used to
 * choose the correct data structure from the corresponding CLP union
 * for mapping. (The piOid can also (mis)used to give back a integer to
 * the caller of siCleExecute() from mapping if this pointer not NULL.)
 *
 * If additional dynamic memory required in the CLP structure the provided
 * handle can be used for pvClpAlloc.
 *
 * @param[in]  pvClp Pointer to the CLP handle for allocation of memory in the CLP structure.
 * @param[in]  pfOut File pointer for outputs (mainly error messages, given over CleExecute, see siCleExecute()).
 * @param[in]  pfTrc File pointer for tracing (mainly for complex stuff, given over CleExecute, see siCleExecute()).
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute, see siCleExecute()).
 * @param[in]  piOid Pointer to the object identifier for overlay commands, if the pointer set at siCleExecute().
 * @param[in]  pvClp Pointer to the filled FLAMCLP structure (output from the command line parser).
 * @param[out] pvPar Pointer to the parameter structure, which will be filled based on the FLAMCLP structure with this function.
 *
 * @return     Reason code (!=0) for termination or 0 for success.
 *             If a no run reason code (!=0) defined then the run function is not executed.
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
 * @brief Type definition for CLE run function.
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
 * the provided handle can be used for pvClpAlloc().
 *
 * @param[in]  pvHdl Pointer to the CLP handle for allocation of memory in the FLC structure.
 * @param[in]  pfOut File pointer for outputs (given over structure CleExecute, see siCleExecute()).
 * @param[in]  pfTrc File pointer for tracing (given over structure CleExecute, see siCleExecute()).
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over structure CleExecute, see siCleExecute()).
 * @param[in]  pcOwn Current owner name (given over structure CleExecute, see siCleExecute()).
 * @param[in]  pcPgm Current program name (given over structure CleExecute, see siCleExecute())
 * @param[in]  pcVsn Current version information (given from structure CleExecute, see siCleExecute()).
 * @param[in]  pcAbo Current about information (given from structure CleExecute, see siCleExecute()).
 * @param[in]  pcLic Current license text (given from structure CleExecute, see siCleExecute()).
 * @param[in]  pcFkt Current function name (key word of the command).
 * @param[in]  pcCmd Current command (complete entered line of user).
 * @param[in]  pcLst Current list of parsed arguments (given from FLAMCLP, could be NULL or empty).
 * @param[in]  pvPar Pointer to the filled parameter for the run of the subprogram.
 * @param[out] piWrn Pointer to an integer (the fist half word is true (0x0001), if warnings collated by directory walk, the second halfword is true (0x0001) if warnings are logged).
 * @param[out] piScc Pointer to an integer containing a special condition code (if greater CLERTC_MAX(64) then used instead of CLERTC_RUN(8)).
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
 * @brief Type definition for the CLE fin function.
 *
 * This function is called at the end after the run to clean up the
 * parameter structure. For example, it could be there was a file pointer
 * which must be closed or memory which must be freed.
 *
 * @param[in]  pfOut File pointer for outputs (given over CleExecute, see siCleExecute()).
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute, see siCleExecute()).
 * @param[in]  pvGbl Pointer to a global handle as black box pass through (given over CleExecute, see siCleExecute()).
 * @param[in]  pvPar Pointer to the filled parameter structure for cleanup.
 * @return     Reason code (!=0) for termination or 0 for success.
 */
typedef int (TfFin)(
   FILE*                         pfOut,
   FILE*                         pfTrc,
   void*                         pvGbl,
   void*                         pvPar);

/**
 * @brief Type definition for the CLE message function
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
 * @param[in]  siRsn Reason code from INI, MAP, RUN and FIN function.
 * @return     Pointer to the corresponding message.
 */
typedef const char* (TfMsg)(const int siRsn);

/** @}*/
/**********************************************************************/

/**
 * @defgroup CLE_CMDTAB CLE Command Table
 * @brief Command structure and corresponding macros used to define CLE command tables.
 * @{
 */
/**
 * @brief CLE structure for command table
 *
 * The command table defines all the commands which could be executed by CLE.
 * To simplify the definition of command tables it is recommended to use the
 * CLPTAB macros.
 */
typedef struct CleCommand {
   const char*                   pcKyw;/**< @brief Pointer to the key word for this command (:alpha:[:alnum:|'_']*). */
   const TsClpArgument*          psTab;/**< @brief Pointer to the main argument table for this command (defines the semantic for the parser). */
   void*                         pvClp;/**< @brief Pointer to the corresponding argument structure (filled up by the parser). */
   void*                         pvPar;/**< @brief Pointer to the corresponding parameter structure (filled up by the mapping function).*/
   int*                          piOid;/**< @brief Pointer to the object identifier for overlay commands (filled up by the parser, see siCleExecute()). */
   TfIni*                        pfIni;/**< @brief Pointer to the initialization function (initialize the argument structure in front of parsing). */
   TfMap*                        pfMap;/**< @brief Pointer to the mapping function (transfers the argument structure to the parameter structure). */
   TfRun*                        pfRun;/**< @brief Pointer to the executed function (use the mapped parameter structure to execute the command.
                                            (for logging the function name, original command line and parsed argument list and other values are also provided)). */
   TfFin*                        pfFin;/**< @brief Pointer to the finish function for cleanup (free memory, close files in parameter structure). */
   int                           siFlg;/**< @brief Flag to indicate a hidden (==0) or visible (!=0) command. For correct numbering, put hidden commands to the end of the table. */
   const char*                   pcMan;/**< @brief Pointer to a null-terminated string for a detailed description of this command (in ASCIDOC format, content behind .DESCRIPTION,
                                            mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES). It is recommended to use a header file with a define for this long
                                            string. "&{OWN}" and "&{PGM}" are replaced with the current owner and program name. The resulting text is converted on EBCDIC systems). */
   const char*                   pcHlp;/**< @brief String for a short context sensitive help to this command (converted on EBCDIC systems). */
} TsCleCommand;
/**********************************************************************/

/**
 * @brief Starts a table with command definitions
 *
 * @param[in] name Name of this table.\n
 */
#define CLECMD_OPN(name)         TsCleCommand name[]

/**
 * @brief Defines a command with the command line keyword *kyw*.
 *
 * @param[in] kyw Pointer to command line keyword *kyw*. \n
 * @param[in] tab Pointer to the main table for this command. \n
 * @param[in] clp Pointer to the corresponding FLAMCLP structure (generated with the CLPMAC.h marcros). \n
 * @param[in] par Pointer to the corresponding parameter structure. \n
 * @param[in] oid Pointer to an integer to define the main table as overlay or NULL to define the main table as object
 *                If the pointer is set the object identifier of the chosen argument of the overlay is given back. \n
 * @param[in] ini Pointer to the initialization function for the FLAMCLP structure (see TfIni). \n
 * @param[in] map Pointer to the mapping function (see TfMap).
 *            The mapping functions maps the parsed content of the FLAMCLP structure in the PAR structure. \n
 * @param[in] run Pointer to the run function to execute the subprogram with the PAR structure (see TfRun). \n
 * @param[in] fin Pointer to the finalization function to clean up the parameter structure (see TfFin). \n
 * @param[in] flg Flag to indicate a hidden (==0) or visible (!=0) command,
 *                For correct numbering, put hidden commands to the end of the table. \n
 * @param[in] man Pointer to a null-terminated string for a detailed description of this command.
 *                (in ASCIDOC format, content behind .DESCRIPTION, usually some paragraphs plus .OPTIONS and/or .EXAMPLES)
 *                It is recommended to use a header file with a define for this long string). \n
 * @param[in] hlp String for a short context sensitive help to this command. \n
 */
#define CLETAB_CMD(kyw,tab,clp,par,oid,ini,map,run,fin,flg,man,hlp)   {(kyw),(tab),(clp),(par),(oid),(ini),(map),(run),(fin),(flg),(man),(hlp)},

/**
 * @brief Ends a table with constant definitions
 */
#define CLECMD_CLS                                                    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0   , NULL, NULL}
/** @}*/

/**
 * @defgroup CLE_STRTAB CLE Other CLP string table
 * @brief Structure and corresponding macros containing CLP string table in order to append to generated documentation.
 * @{
 */

/**
 * @brief CLE table structure for other CLP strings
 *
 * This structure is used to define a table of CLP strings in order to add as appendix to the generated documentation.
 */
typedef struct CleOtherClp {
   const char*                   pcRot;/**< @brief Pointer to the program/root key word for this string (:alpha:[:alnum:|'_']*). */
   const char*                   pcKyw;/**< @brief Pointer to the key word for this string (:alpha:[:alnum:|'_']*). */
   const TsClpArgument*          psTab;/**< @brief Pointer to the main argument table for this command (defines the semantic for the parser). */
   const char*                   pcMan;/**< @brief Pointer to a null-terminated string for a detailed description for this CLP string
                                                   (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES)
                                                   It is recommended to use a header file with a define for this long string.
                                                   "&{OWN}" and "&{PGM}" are replaced with the current owner and program name.
                                                   The resulting text is converted on EBCDIC systems). */
   const char*                   pcHlp;/**< @brief String for a short context sensitive help for this CLP string (converted on EBCDIC systems). */
   const int                     isOvl;/**< @brief True if provided table must be interpreted as overlay else as object. */
} TsCleOtherClp;
/**********************************************************************/

/**
 * @brief Starts a table with other CLP strings.
 *
 * @param[in] name Name of this table.
 */
#define CLEOTH_OPN(name)               TsCleOtherClp name[]

/**
 * @brief Defines a appendix for the object or overlay *cmd* of the root *rot* with the headline of *hdl* for a certain other CLP string.\n
 *
 * @param[in] rot Pointer to the program/root key word for this string (:alpha:[:alnum:|'_']*). \n
 * @param[in] kyw Pointer to the key word for this string (:alpha:[:alnum:|'_']*). \n
 * @param[in] tab Pointer to the main argument table for this CLP string. \n
 * @param[in] man Pointer to a null-terminated string for a detailed description for this CLP string
 *            (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES).
 *            It is recommended to use a header file with a define for this long string.
 *            "&{OWN}" and "&{PGM}" are replaced with the current owner and program name.
 *            The resulting text is converted on EBCDIC systems. \n
 * @param[in] hlp String for a short context sensitive help for this CLP string (converted on EBCDIC systems).\ n
 * @param[in] ovl True if provided table must be interpreted as overlay else as object. \n
 */
#define CLETAB_OTH(rot,kyw,tab,man,hlp,ovl)  {(rot),(kyw),(tab),(man),(hlp),(ovl)},

/**
 * @brief Ends a table with other CLP strings
 *
 */
#define CLEOTH_CLS                           { NULL, NULL, NULL, NULL, NULL, 0}
/** @}*/

/**********************************************************************/

/*! @cond PRIVATE */
#endif /*INC_CLEDEF_H*/

#ifdef __cplusplus
}
#endif
/*! @endcond */
