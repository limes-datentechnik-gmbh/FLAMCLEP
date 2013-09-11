/******************************************************************************/
/*******************************************************************************/
/**
 * @file FLAMCLP.h
 * @brief definitions for <b>C</b>ommand <b>L</b>ine <b>P</b>arsing
 *
 * LIMES Command Line Parser (FLAMCLP) in ANSI-C
 * @author Falk Reichbott
 * @date 27.02.2013\n
 * @copyright (c) 2013 limes datentechnik gmbh
 * www.flam.de
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
\internal The following description of Grammar, Lexeme and property file grammar
is intended by 4 spaces to be rendered as code block in doxygenerated html.
*/
/**
 *


Description
-----------

The command line parser (FLAMCLP) is a complier which reads a command string
using the lexems and grammar below to fill a structure with the
corresponding values given in this line. The FLAMCLP works only in memory
and the syntax and semantic will be defined by a tree of tables. Such a
table can represent an object (struct) or an overlay (union). Each
argument in such a table can be a struct or overlay again in using
another table for this type. Basic types are switches, numbers, floats
or strings. With each argument you can define the required minimum and
possible maximum amount of occurrences. This means that each argument
can be an array and arrays are implemented as simplified notations.

The FLAMCLP uses these tables as symbol tables to define the syntax and
semantic of a command. The same table provides the offset used to
store the parsed values. This offset occurs in a real data structure
and with CLPMAC.h you can use the same macro to build the tables and
corresponding structs and unions. This is not mandatory, but we
recommend to use the macro in order to be in sync.

The FLAMCLP provides also all internally calculated values in this data
structure. The mechanism is called linking. Thus you have to use the
same keyword for linking eventually with a calculated value of that
argument. For example, if you define an array of numbers then you can
define a link to determine the amount of entered numbers or for an
overlay you can link the corresponding object identifier to determine
which of the arguments are chosen by the user. You can also get the
string length and other features.

The FLAMCLP supports also aliases. An alias points to another argument and
is only an additional keyword that can be used.

Besides arguments you can also have a constant definition for
selections. A feature is useful in order to define keywords for values
(numbers, floats and strings). With help of the selection flag you can
enforce the pure acceptance of predefined keywords.

For each argument or constant you must define a keyword and a short
help message. If you state additionally a detailed description, then
this argument gets an own chapter in the generative documentation, a
manual page will be available and extensive help is displayed.

For each argument you can define a default value and use the property
parser to overwrite it again. The default value replaces the entered
value. This means that if a default value or property
is defined, then this will have the same effect as the entry of
the value in the command line. With the latter you can still override
the hard coded or property default value. The property management
can make use of a function that extracts a property list for the
argument table tree.

The FLAMCLP calculates automatically the minimum amount of letters
required to make the meaning of a keyword unique. Depending on the case
mode the required letters are highlighted in the interactively used help
function. The syntax function provides also data when an argument
is required or optional, based on the minimum amount of occurrences.

If you intend to apply the FLAMCLP first of all an open will be necessary.
Then you are able to parse a property list before doing this with the
command line. Both property list and command line are provided as zero
terminated strings. This means that the FLAMCLP does not know whether the
command line results from a file or argc/argv.
After parsing the command line the corresponding FLAMCLP structure is
filled with the entered values and the FLAMCLP can be closed or another
command line parsed.

Beside property and command line parsing the FLAMCLP offers an interactive
syntax and help function. Additionally you can use a very powerful
function to generate single manual pages or complete user manuals,
You can make also use of the supported grammar and regular expressions
(lexems).

The implementation of the FLAMCLP is finished with the Command Line
Executer (FLAMCLE) with which you can define your list of commands by
using an additional table. You can make use only of one new function
that is executed eventually. The FLAMCLE offers an extensive built-in
functionality and will be the optimal access to the FLAMCLP capabilities.


Supported regular expressions
------------------------------

Lexeme
------

    END  0x00\n
    CMT  '#' [:print:]* '#'\n
    SEP  [:SPACE: | :CNTR: | ',']*\n
    SGN  '='\n
    DOT  '.'\n
    RBO  '('\n
    RBC  ')'\n
    SBO  '['\n
    SBC  ']'\n
    KYW  [:alpha:]+[:alnum: | '_' | '-']*\n
    NUM  ([+|-]  [ :digit:]+) |                           decimal (default)\n
         ([+|-]0b[ :digit:]+) |                           binary\n
         ([+|-]0o[ :digit:]+) |                           octal\n
         ([+|-]0d[ :digit:]+) |                           decimal\n
         ([+|-]0x[:xdigit:]+) |                           hexadecimal\n
         ([+|-]0t(yyyy/mm/tt.hh:mm:ss) |                  relative or absolute time\n
    FLT  ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) |\n
         ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)\n
    STR       ''' [:print:]* ''' |     string with zero termination (local character set)\n
         [s|S]''' [:print:]* ''' |     string with zero termination (local character set)\n
         [c|C]''' [:print:]* ''' |     string in local character set (no zero termination)\n
         [x|X]''' [:print:]* ''' |     binary hexadecimal\n
         [a|A]''' [:print:]* ''' |     binary ascii (no zero termination)\n
         [e|E]''' [:print:]* ''' |     binary ebcdic (no zero termination)\n
    SUP       '"' [:print:]* '"' |     string with zero termination (local character set) for supplement syntax (properties)\n


Grammar for command line
------------------------

    cmd      -> INI parlst END

    parlst   -> par SEP parlst\n
             |  @

    par      -> swt\n
             |  sgn\n
             |  obj\n
             |  ovl\n
             |  ary

    swt      -> KEYWRD

    sgn      -> KEYWRD '=' val

    obj      -> KEYWRD '(' parlst ')'

    ovl      -> KEYWRD '.' par

    ary      -> KEYWRD '[' vallst ']'\n
             |  KEYWRD '[' objlst ']'\n
             |  KEYWRD '[' ovllst ']'

    vallst   -> val SEP vallst\n
             |  @

    objlst   -> '(' parlst ')' SEP objlst\n
             |  @

    ovllst   -> par SEP ovllst\n
             |  @

    val      -> NUM                             * integer\n
             |  FLT                             * floating number\n
             |  STR                             * string literal\n
             |  KYW                             * constant definition


Grammar for property file
-------------------------

    proper   -> INI prolst END

    prolst   -> pro SEP prolst\n
             |  @

    pro      -> kywlst '=' SUP

    kywlst   -> KYW '.' kywlst\n
             |  KYW

 *
 ******************************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLP_H
#define INC_CLP_H

#ifdef __HOST__
   #define pcClpVersion          flclpvsn
   #define pcClpAbout            flclpabo
   #define pvClpOpen             flclpopn
   #define siClpParse            flclpprs
   #define siClpSyntax           flclpsyn
   #define siClpHelp             flclphlp
   #define siClpLexem            flclplex
   #define siClpGrammar          flclpgrm
   #define vdClpClose            flclpcls
#endif

#include <inttypes.h>

/* Standard defines ***********************************************************/

   #ifdef __RELEASE__
      #define  __BUILD__         "RELEASE"
   #endif
   #ifdef __DEBUG__
      #define  __BUILD__         "DEBUG"
   #endif
   #ifndef __BUILD__
      #define  __BUILD__         "UNKNOWN"
   #endif
   #ifndef TRUE
      #define TRUE               (1)
   #endif
   #ifndef FALSE
      #define FALSE              (0)
   #endif
   #ifndef EOS
      #define EOS                (0x00)
   #endif
   #ifndef I08
         #define I08             int8_t
   #endif
   #ifndef I16
         #define I16             int16_t
   #endif
   #ifndef I32
         #define I32             int32_t
   #endif
   #ifndef I64
         #define I64             int64_t
   #endif
   #ifndef U08
         #define U08             uint8_t
   #endif
   #ifndef U16
         #define U16             uint16_t
   #endif
   #ifndef U32
         #define U32             uint32_t
   #endif
   #ifndef U64
         #define U64             uint64_t
   #endif
   #ifndef C08
      #define C08                char
   #endif
   #ifndef F32
      #define F32                float
   #endif
   #ifndef F64
      #define F64                double
   #endif

/**
 * Get version information
 *
 * The function returns the version information for this library
 *
 * @return pointer to a zero terminated string with a new line at the end
 */
extern char* pcClpVersion(void);

/**
 * Get about information
 *
 * The function returns the about information for this library
 *
 * @return pointer to a zero terminated string with a new line at the end
 */
extern char* pcClpAbout(void);

/**
 * Return code for a successful parsing
 */
#define CLP_OK                   0

/**
* Error codes for command line parsing
*/
/** CLPERR_LEX Lexical error (determined by scanner) */
#define CLPERR_LEX               -1
/** CLPERR_SYN Syntax error (determined by parser) */
#define CLPERR_SYN               -2
/** CLPERR_SEM Semantic error (determined by builder) */
#define CLPERR_SEM               -3
/** CLPERR_TYP Type error (internal error with argument types) */
#define CLPERR_TYP               -4
/** CLPERR_TAB Table error (internal error with argument tables) */
#define CLPERR_TAB               -5
/** CLPERR_SIZ Size error (internal error with argument tables and data structures) */
#define CLPERR_SIZ               -6
/** CLPERR_PAR Parameter error (internal error with argument tables and data structures) */
#define CLPERR_PAR               -7
/** CLPERR_MEM Memory error (internal error with argument tables and data structures) */
#define CLPERR_MEM               -8
/** CLPERR_INT Internal error (internal error with argument tables and data structures) */
#define CLPERR_INT               -9
/** CLPERR_SYS System error (internal error with argument tables and data structures)*/
#define CLPERR_SYS               -10

/**
* Data types of parameter in the argument table
*/
/** CLPTYP_NON    No type - Mark the end of an argument table */
#define CLPTYP_NON               0
/** CLPTYP_SWITCH Switch (single keyword representing a number (OID)) */
#define CLPTYP_SWITCH            1
/** CLPTYP_NUMBER Signed or unsigned integer number (8, 16, 32 or 64 bit) */
#define CLPTYP_NUMBER            2
/** CLPTYP_FLOATN Floating point number (32 or 64 bit) */
#define CLPTYP_FLOATN            3
/** CLPTYP_STRING String literal (binary (HEX, ASCII, EBCDIC, CHARS) or zero terminated (default)) */
#define CLPTYP_STRING            4
/** CLPTYP_OBJECT Object (KEYWORD(parameter_list)) can contain arbitrary list of other types */
#define CLPTYP_OBJECT            5
/** CLPTYP_OVRLAY Overlay (KEYWORD.KEYWORD...) contains one of its list as in a C union */
#define CLPTYP_OVRLAY            6

#define CLPTYP_XALIAS           -1

/**
* Flags for command line parsing
*/
#define CLPFLG_NON               0x00000000UL
/** CLPFLG_ALI This parameter is an alias for another argument (set by macros) */
#define CLPFLG_ALI               0x00000001UL
/** CLPFLG_CON This parameter is a constant definition (no argument, no link, no alias (set by macros)) */
#define CLPFLG_CON               0x00000002UL
/** CLPFLG_SEL If set only the predefined constants over the corresponding key words can be selected (useful to define selections) */
#define CLPFLG_SEL               0x00000010UL
/** CLPFLG_FIX This argument has a fixed length (only useful for strings if a typedef defines a fixed length per element, else set internally) */
#define CLPFLG_FIX               0x00000020UL
/** CLPFLG_BIN This argument can contain binary data without zero termination (length must be known or determined with a link) */
#define CLPFLG_BIN               0x00000040UL
/** CLPFLG_CNT This link will be filled by the calculated amount of elements (useful for arrays) */
#define CLPFLG_CNT               0x00000100UL
/** CLPFLG_OID This link will be filled by the object identifier (OID) of the chosen argument (useful for overlays) */
#define CLPFLG_OID               0x00000200UL
/** CLPFLG_ELN This link will be filled by the calculated length of a element (fixed types == data size, packed types == data length) */
#define CLPFLG_ELN               0x00001000UL
/** CLPFLG_SLN This link will be filled by the calculated string length for a element (only for zero terminated strings) */
#define CLPFLG_SLN               0x00002000UL
/** CLPFLG_TLN This link will be filled by the calculated total length for the argument (sum of all element lengths) */
#define CLPFLG_TLN               0x00004000UL

/**
 * \struct TsClpArgument
 *
 * This structure is used to define a table of arguments\n
 *
 * To simplify the definition of the corresponding data structures and argument tables it is recommended to use the
 * CLPARGTAB macros defined in CLPMAC.h or for constant definitions the CLPCONTAB macros below.
 *
 * Its members are:
 * \par
 * \b siTyp  Type of this parameter (CLPTYP_xxxxxx)\n
 *           The type will be displayed in context sensitive help messages (TYPE: type_name)\n
 * \b pcKyw  Pointer to a zero terminated key word for this parameter (:alpha:[:alnum:|'-'|'_']*)\n
 * \b pcAli  Pointer to another key word to define an alias (:alpha:[:alnum:|'-'|'_']*)\n
 * \b siMin  Minimum amount of entries for this argument (0-optional n-required)\n
 * \b siMax  Maximum amount of entries for this argument (1-scalar n-array)\n
 * \b siSiz  If fixed size type (switch, number, float, object, overlay) then size of this type else (string) available size in memory
 *           (string type can be defined as FIX with CLPFLG_FIX but this requires a typedef for this string size)\n
 * \b siOfs  Offset of an argument in a structure used for address calculation (please use offset of(t,m) macro)\n
 * \b siOid  Unique integer value representing the argument (object identifier, used in overlays or for switches)\n
 * \b uiFlg  Flag value which can be assigned with CLPFLG_SEL/CON/FIX/CNT/SEN/ELN/TLN/OID/ALI to define different characteristics\n
 * \b psTab  Pointer to another parameter table
 *           for CLPTYP_OBJECT and CLPTYP_OVRLAY describing these structures
 *           for CLPTYP_NUMBER, CLPTYP_FLOATN or CLPTYP_STRING to define selections (constant definitions)\n
 * \b pcDft  Pointer to a zero terminated string to define the default values assigned if no argument was defined
 *           If this pointer is NULL or empty ("") then no initialization will be done\n
 *            - for switches a number literal or the special keywords ON/OFF can be defined\n
 *            - for numbers a number literal or a key word for a constant definition can be defined\n
 *            - for floats a floating point number literal or a key word for a constant definition can be defined\n
 *            - for strings a string literal or a key word for a constant definition can be defined\n
 *            - for objects the special keyword INIT must be defined to initialize the object\n
 *            - for overlays the keyword of the assigning object must be defined to initialize the overlay\n
 *            For arrays of these types a list of the corresponding values (literals or key words) can be defined
 *            The default values will be displayed in context sensitive help messages (PROPERTY: [value_list])\n
 * \b pcMan  Pointer to a zero terminated string for a detailed description of this argument
 *           (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *           Can be a NULL pointer or empty string for constant definition or simple arguments.
 *           It is recommended to use a header file with a define for this long string (mainly for objects and overlays)\n
 * \b pcHlp  Pointer to a zero terminated string for context sensitive help to this argument
 *           Also used as head line in documentation generation. For this only alnum, blank, dot, comma, hyphen and parenthesis are used.
 *           At each other separator the headline will be cut, means it will be possible to have more help information then head line.\n
 */
typedef struct ClpArgument {
   int                           siTyp;
   const char*                   pcKyw;
   const char*                   pcAli;
   int                           siMin;
   int                           siMax;
   int                           siSiz;
   int                           siOfs;
   int                           siOid;
   unsigned long                 uiFlg;
   struct ClpArgument*           psTab;
   const char*                   pcDft;
   const char*                   pcMan;
   const char*                   pcHlp;
   signed long long int          siVal;
   double                        flVal;
   const unsigned char*          pcVal;
}TsClpArgument;

/** Starts a table with constant definitions
 *
 *  *nam* Name of this table\n
 */
#define CLPCONTAB_OPN(nam) TsClpArgument nam[]

/** defines a number literal with the command line keyword *kyw* and the value *dat*
 *
 *  *man* Pointer to a zero terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string\n
 *  *hlp* Pointer to a zero terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_NUMBER(kyw,dat,man,hlp)       {CLPTYP_NUMBER,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),(dat), 0.0 ,NULL       },

/** defines a number literal with the command line keyword *kyw* and the value *dat*
 *
 *  *man* Pointer to a zero terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string\n
 *  *hlp* Pointer to a zero terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_FLOATN(kyw,dat,man,hlp)       {CLPTYP_FLOATN,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),  0  ,(dat),NULL       },

/** defines a floating point literal with the command line keyword *kyw* and the value *dat*
 *
 *  *man* Pointer to a zero terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string\n
 *  *hlp* Pointer to a zero terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_STRING(kyw,dat,man,hlp)       {CLPTYP_STRING,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat)},

/** defines a string literal with the command line keyword *kyw* and the value *dat*
 *
 *  *siz* Size of the string value
 *  *man* Pointer to a zero terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string)\n
 *  *hlp* Pointer to a zero terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_BINARY(kyw,dat,siz,man,hlp)   {CLPTYP_STRING,(kyw),NULL,0,0,(siz),0,0,CLPFLG_CON|CLPFLG_BIN,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat)},

/** Ends a table with constant definitions
 */
#define CLPCONTAB_CLS                           {CLPTYP_NON   , NULL,NULL,0,0,  0  ,0,0,CLPFLG_NON           ,NULL,NULL, NULL, NULL,  0  , 0.0 ,NULL       }

/**
 * Open command line parser
 *
 * The function uses the argument table and corresponding structure and creates the handle for the command line parser (FLAMCLP)
 *
 * @param[in]  isCas Boolean to enable case sensitive parsing of keywords (recommended is FLASE)
 * @param[in]  siMkl Integer defining the minimal key word length (siMkl<=0 --> full length, no auto abbreviation)
 * @param[in]  pcOwn String constant containing the owner name for the root in the path ("de.limes")
 * @param[in]  pcPgm String constant containing the program name for the root in the path ("flcl")
 * @param[in]  pcCmd String constant containing the command name for the root in the path ("CONV")
 * @param[in]  pcMan String constant containing the manual page for this command
 * @param[in]  pcHlp String constant containing the help message for this command
 * @param[in]  isOvl Boolean if TRUE the main table (psTab) is a overlay else it will be interpreted as object
 * @param[in]  psTab Pointer to the parameter table defining the semantic of the command line
 * @param[out] pvDat Pointer to the structure where the parsed values are stored
 * @param[in]  pfHlp Pointer to the file used for help messages (if not set then stderr)
 * @param[in]  pfErr Pointer to the file used for error messages or NULL for no printing
 * @param[in]  pfSym Pointer to the file used for symbol table trace or NULL for no printing
 * @param[in]  pfScn Pointer to the file used for scanner trace or NULL for no printing
 * @param[in]  pfPrs Pointer to the file used for parser trace or NULL for no printing
 * @param[in]  pfBld Pointer to the file used for builder trace or NULL for no printing
 * @param[in]  pcDep String used for hierarchical print outs (help, errors, trace (recommended "--|"))
 * @param[in]  pcOpt String used to separate options (recommended "/")
 * @param[in]  pcEnt String used to separate list entries (recommended ",")
 *
 * @return void pointer to the memory containing the handle
 */
extern void* pvClpOpen(
   const int                     isCas,
   const int                     siMkl,
   const char*                   pcOwn,
   const char*                   pcPgm,
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
   const char*                   pcEnt);

/**
 * Parse the property list
 *
 * The function parses the property list and returns OK or the error code and error position (byte offset)
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcPro Pointer to a zero terminated string containing the property list for parsing
 * @param[in]  isChk Boolean to enable (TRUE) or disable (FALSE) validation of the root in path
 *             (if FALSE then other properties are ignored, if TRUE then other properties are not possible)
 * @param[out] ppPos Pointer in the provided property list where the error was detected
 * @param[out] ppLst Pointer to the parsed parameter list (NULL = no list provided)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpParsePro(
   void*                         pvHdl,
   const char*                   pcPro,
   const int                     isChk,
   char**                        ppPos,
   char**                        ppLst);

/**
 * Parse the command line
 *
 * The function parses the command line and returns OK or the error code and error position (byte offset)
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcCmd Pointer to a zero terminated string containing the command for parsing
 * @param[in]  isChk Boolean to enable (TRUE) or disable (FALSE) validation of minimum number of entries
 * @param[out] piOid If this pointer is set and the main table is an overlay the corresponding object identifier is returned
 * @param[out] ppPos Pointer in the provided command line where the error was detected
 * @param[out] ppLst Pointer to the parsed parameter list (NULL = no list provided)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpParseCmd(
   void*                         pvHdl,
   const char*                   pcCmd,
   const int                     isChk,
   int*                          piOid,
   char**                        ppPos,
   char**                        ppLst);

/**
 * Print command line syntax
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
 * Print help for command line syntax
 *
 * The function prints the help strings for the command line syntax
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcPat Path (root.input...) to limit help to a certain level
 * @param[in]  siDep Depth of next levels to display (0-Manpage, 1-One Level, 2-Two Level, ..., <9-All)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpHelp(
   void*                         pvHdl,
   const int                     siDep,
   const char*                   pcPat);

/**
 * Generate documentation for command line syntax
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
 * Numbering of headlines for doc type book can be enabled or disabled.
 *
 * The manual page for the command is displayed with headline level 2 '-', if no manual page is
 * available the message below is shown:
 *
 *     'No detailed description available for this command.'
 *
 * The same is valid for objects and overlays, what means that at a minimum each command, overlay and object needs a
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
 * @param[in]  pcNum Leading number for table of contents ("1.2." used or not used depend on isNbr)
 * @param[in]  isDep If TRUE then all deeper parts are printed if FALSE then not.
 * @param[in]  isMan If TRUE then doc type MANPAGE will be generated else doc type book will be used.
 *                   (isMan==TRUE results automatically in isDep==FALSE)
 * @param[in]  isNbr Boolean to enable header numbering for generated documentation (only for doc type book)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpDocu(
   void*                         pvHdl,
   FILE*                         pfDoc,
   const char*                   pcPat,
   const char*                   pcNum,
   const int                     isDep,
   const int                     isMan,
   const int                     isNbr);

/**
 * Generate properties
 *
 * The function produces a property list with the current default values
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcPat Path (root.input...) to limit the amount of properties
 * @param[in]  pfPro File pointer to write the property list (if NULL then pfHlp of FLAMCLP is used)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpProperties(
   void*                         pvHdl,
   const int                     siDep,
   const char*                   pcPat,
   FILE*                         pfPro);

/**
 * Print the lexems of the command line compiler
 *
 * The function prints the regular expressions of the command line compiler
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pfOut Pointer to the file descriptor used to print the regular expressions
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpLexem(
   void*                         pvHdl,
   FILE*                         pfOut);

/**
 * Print the grammar of the command line compiler
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
 * Close the command line parser
 *
 * The function releases the allocated resources in the handle
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 */
extern void vdClpClose(
   void*             pvHdl);

/**
 * Compare of two string
 *
 * The procedure combines strcmp, stricmp, strncmp and strchr in one function.
 *
 * @param[in]  f  Flag if case sensitiv (TRUE) or not (FALSE)
 * @param[in]  s1 String 1 to compare
 * @param[in]  s2 string 2 to compare
 * @param[in]  n  Amount of character to compare
 * @param[in]  c  Character where the compare stops
 *
 * @return signed integer with 0 for equal and !=0 for different
 */
extern int strxcmp(
   const int                     f,
   const char*                   s1,
   const char*                   s2,
   const int                     n,
   const int                     c);

#endif // CMDLINE_H

#ifdef __cplusplus
}
#endif
