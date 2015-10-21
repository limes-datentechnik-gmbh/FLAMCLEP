/**********************************************************************/
/**
 * @file FLAMCLP.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>P</b>arsing
 *
 * LIMES Command Line Parser (FLAMCLP) in ANSI-C
 * @author limes datentechnik gmbh
 * @date 06.03.2015
 * @copyright (c) 2015 limes datentechnik gmbh
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

Description
-----------

The command line parser (FLAMCLP) is a complier which reads a command
string using the lexems and grammar below to fill a structure with the
corresponding values given in this line. The FLAMCLP works only in memory
(except parameter files are used for objects or overlays) and the syntax
and semantic will be defined by a tree of tables. Such a table can
represent an object (struct) or an overlay (union). Each argument in
such a table can be a object or overlay again in using another table for
this type. Basic types are switches, numbers, floats or strings. With
each argument you can define the required minimum and possible maximum
amount of occurrences. This means that each argument can be an array and
arrays are implemented as simplified notations.

The FLAMCLP uses these tables as symbol tables to define the syntax and
semantic of a command. The same table provides the offset used to
store the parsed values. This offset occurs in a real data structure
and with CLPMAC.h you can use the same macro to build the tables and
corresponding structures and unions. This is not mandatory, but we
recommend to use the macro in order to be in sync.

The FLAMCLP provides also all internally calculated values in this data
structure. The mechanism is called linking. Thus you have to use the
same keyword for linking eventually with a calculated value of that
argument. For example, if you define an array of numbers then you can
define a link to determine the amount of entered numbers or for an
overlay you can link the corresponding object identifier to determine
which of the arguments are chosen by the user. If You define overlays of
overlays an additional dimension for each level is used. In this case
you must define an array for this link and You get the child (lnk[0])
before the parent (lnk[1]) written in the CLP structure.

You can also get the string length and other features. The kind of link
is defined over the flags field. There are a lot of other flags supported
beside links, for example the PWD flag, which tells CLP that this value
are only clear in the data structure but always obfuscated in logs, traces
and other printouts to keep the value secret. Another flag can be used
for numbers. With CLPFLG_DEF you can activate a extension of the syntax.
If this flag used for a number then the object identifier is assigned as
value if no assignment done for this number. Means with this extended
syntax you can define a switch, which you can assign a number. This is
useful for example to activate a feature with a default value by using
only the key word and the user can change the default value by an optional
assignment of another value.

The FLAMCLP also supports aliases. An alias points to another argument
and is only an additional keyword that can be used.

To be compatible with certain shells the features below are implemented.

* Strings can be enclosed with '' or ""
* Strings can also be defined without quotes
* Keywords can start with "-" or "--" in front of the qualifier
* If it is unique then parenthesis and the dot can be omitted for objects and overlays
* On EBCDIC systems we use a code page specific interpretation of punctuation characters

Besides arguments you can also have a constant definition for
selections. A feature is useful in order to define keywords for values
(numbers, floats and strings). With help of the selection flag you can
enforce the pure acceptance of predefined keywords.

For each argument or constant you must define a keyword and a short
help message. If you provide a detailed description, then this argument
becomes an own chapter in the generated documentation, a manual page
will be available and extensive help is displayed. The description
string can contain &{OWN} for the current owner or &{PGM} for the
current program name. The letter case of the replacement string depends
and the letter case of the keyword: PGM = upper case, pgm = lower case,
Pgm = title case, pGm = original string. All other content inside of
&{...} is ignored. This can be used, for example, to insert comments
into the source of the manual page.

For each argument you can define a default value and use the property
parser or environment variables to overwrite it again. The default value
replaces the entered value. This means that if a default value, environment
variable or property is defined, then this will have the same effect as the
entry of the value in the command line. With the latter you can still override
the hard coded or property default value. The property management can make use
of a function that extracts a property list for the argument table tree.

For each path you can also define the default value as environment variable.
The path are prefixed with the owner ID and the program name first, then
only the program name and at the last the path only starting with the
command name will be use to determine a environment variable. For this the
path is converted to upper case and all '.' are replaced by '_'. The value
of the environment variable must contain the same supplement string which
are required for the property definition. All possible path values can be
determine with the property generation function.

With the CLP flags CMD (for command) and PRO (property) you can define if
a parameter is only visible in the command line or property file. These
flags have no influence of property or command line parsing. It only
reflects the online help/syntax and docu/property generation. This means
you can still use such a parameter in the property file or in the command
line, but it is not directly visible to the user. If the flags CMD and
PRO are not set then the parameter will be visible in both areas. With
the flag DMY (for dummy) you can enforce that this parameter is not
visible in a generated property file, on the command line help, syntax
and documentation. In this case, the parameter is no part of the symbol
table. It is only part of the CLP structure.

For binary strings the default interpretation can be free defined over a
additional set of flags (CLPFLG_HEX/CHR/ASC/EBC). This is useful for hex
strings or passwords.

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

If the isPfl (is parameter file) flag TRUE: For objects and overlays you
can use the assignment letter '=' to define a parameter file containing
the command string for this object or overlay. Means for each object or
overlay a dedicated parameter file can be used. The parameter file must
contain a command string which syntax is valid for the certain object or
overlay. CLP open the file with format string "r". To use DD names on
mainframes the file name must like "DD:name".

If the flag CLPFLG_PWD is used, string outputs containing passwords will
result in "###SECRECT###" and float or number outputs in a value of 0.

Parsing of the properties (can be done a lot of times over different
sources) only change the default values in the symbol table and has no
effect for the CLP structure. First after parsing the command line the
corresponding FLAMCLP structure is filled with the properties or entered
values and the FLAMCLP can be closed or another command line parsed.

Attention: If pointer to values in the CLP handle used (ppLst, psErr) then
you cannot close the CLP or you must copy the values before.

The normal procedure to use the CLP:
@code
   ClpOpen()
   ClpParsePro()
   ClpParseCmd()
   ClpClose()
@endcode

Beside property and command line parsing the FLAMCLP offers an interactive
syntax and help function. Additionally, you can use a very powerful
function to generate single manual pages or complete user manuals,
You can make use of the supported grammar and regular expressions
(lexems). Provided manual pages must be in ASCIIDOC and will be converted
on EBCDIC systems from the compile code page in the local code page.

Only ClpParseCmd() uses the pvDat pointer. All other functions only work
on the symbol table. This means if you don't use ClpParseCmd() the pointer
to the CLP structure (pvDat), it can be NULL. This is useful if only help,
syntax, documentation or property management are required. For these
functions no corresponding CLP structure must be allocated.

The implementation of the FLAMCLP is finished with the Command Line
Executor (FLAMCLE) with which you can define your list of commands by
using an additional table. You can make use of only one new function
that is executed eventually. The FLAMCLE offers an extensive built-in
functionality and is the optimal access method to the FLAMCLP capabilities.


Supported regular expressions
------------------------------

Lexeme
------

    END  0x00\n
    CMT  '#' [:print:]* '#'\n
    LCT  ';' [:print:]* 'nl'\n
    SEP  [:SPACE: | :CNTR: | ',']*\n
    SGN  '='\n
    DOT  '.'\n
    RBO  '('\n
    RBC  ')'\n
    SBO  '['\n
    SBC  ']'\n
    KYW  [-[-]][:alpha:]+[:alnum: | '_' | '-']*\n
    NUM  ([+|-]  [ :digit:]+) |                           decimal (default)\n
         ([+|-]0b[ :digit:]+) |                           binary\n
         ([+|-]0o[ :digit:]+) |                           octal\n
         ([+|-]0d[ :digit:]+) |                           decimal\n
         ([+|-]0x[:xdigit:]+) |                           hexadecimal\n
         ([+|-]0t(yyyy/mm/tt.hh:mm:ss) |                  relative or absolute time\n
    FLT  ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) |\n
         ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)\n
    STR       ''' [:print:]* ''' |     string with null termination (local character set)\n
         [s|S]''' [:print:]* ''' |     string with null termination (local character set)\n
         [c|C]''' [:print:]* ''' |     string in local character set (no null termination)\n
         [x|X]''' [:print:]* ''' |     binary hexadecimal\n
         [a|A]''' [:print:]* ''' |     binary ascii (no null termination)\n
         [e|E]''' [:print:]* ''' |     binary ebcdic (no null termination)\n
         Strings can contain two '' to represent one '\n
         Strings can also be enclosed in " instead of '\n"
         Strings can directly start behind a '=' without enclosing '/"\n
            In this case the string ends at the next separator or operator\n
            and keywords are preferred. To use keywords, separators or\n
            operators in strings, enclosing quotes are required\n
    SUP       '"' [:print:]* '"' |     string with null termination (local character set) for supplement syntax (properties)\n
         Supplements can contain two "" to represent one "\n
         Supplements can also be enclosed in ' instead of "\n"


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

    obj      -> KEYWRD ['('] parlst [')']
             |  KEYWORD '=' STRING # parameter file #

    ovl      -> KEYWRD ['.'] par
             |  KEYWORD '=' STRING # parameter file #

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

A list of objects requires parenthesis to enclose the arguments. Only
for one object of a certain level you can omit the round brackets. If
you want define more than one object of a certain level you must use
parenthesis to separate the objects from each other. In parameter files
the command string for an overlay can be start with a dot '.' or not.
The same is valid for the parenthesis '(...)' of an object.

Grammar for property file
-------------------------

    proper   -> INI prolst END

    prolst   -> pro SEP prolst\n
             |  @

    pro      -> kywlst '=' SUP

    kywlst   -> KYW '.' kywlst\n
             |  KYW

Compiler switches
-----------------

For compilation the defines below must be set:

    __DEBUG__        for a debug build
    __RELEASE__      for a release build
    __WIN__          for WINDOWS platforms
    __HOST__         for mainframe platforms
    __BUILDNR__      to define the build number (integer, default is 0)
    __BUILD__        to define the build string ("debug", "release", "alpha", ...)
    __HOSTSHORTING__ to short function names to 8 character for mainframes
 *
 **********************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLP_H
#define INC_CLP_H


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
   #define siClpLexem            FLCLPLEX
   #define siClpGrammar          FLCLPGRM
   #define vdClpClose            FLCLPCLS
   #define pcClpError            FLCLPERR
#endif

#include <stdint.h>
#include <inttypes.h>

/* Standard defines ***************************************************/

#ifndef __BUILDNR__
#  define __BUILDNR__         0
#endif
#ifdef __RELEASE__
#  define  __BUILD__         "RELEASE"
#endif
#ifdef __DEBUG__
#  define  __BUILD__         "DEBUG"
#endif
#ifndef __BUILD__
#  define  __BUILD__         "UNKNOWN"
#endif

/**
 * Please use this defines for the data types in the tables to be ensure
 * the correct size for the parser.
 */
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
 * @brief Get version information
 *
 * The function returns the version information for this library
 *
 * @param l Level of visible hierarchy in the first 2 numbers of the string
 *          the number can later be used to better visualize the hierarchy
 * @param s Size of the provided string buffer (including space for null termination)
 * @param b Buffer for the version string.
 *          Must contain a null-terminated string.
 *          The version string will be concatenated.
 *          The size including the 0-byte is the limit.
 *          If (strlen(b)==s-1) then more space is required for the complete version string.
 *          A good size for the version string is 128 byte.
 *
 * @return Pointer to a null-terminated version string (return(b))
 */
extern const char* pcClpVersion(const int l, const int s, char* b);

/**
 * @brief Get about information
 *
 * The function returns the about information for this library
 *
 * @param l Level of visible hierarchy in the first 2 numbers of the string
 *          the number can later be used to better visualize the hierarchy
 * @param s Size of the provided string buffer (including space for null termination)
 * @param b Buffer for the about string.
 *          Must contain a null-terminated string.
 *          The about string will be concatenated.
 *          The size including the 0-byte is the limit.
 *          If (strlen(b)==s-1) then more space is required for the complete about string.
 *          A good size for the about string is 512 byte.
 *
 * @return pointer to a null-terminated about string (return(b))
 */
extern const char* pcClpAbout(const int l, const int s, char* b);

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
/** CLPTYP_STRING String literal (binary (HEX, ASCII, EBCDIC, CHARS) or null-terminated (default)) */
#define CLPTYP_STRING            4
/** CLPTYP_OBJECT Object (KEYWORD(parameter_list)) can contain arbitrary list of other types */
#define CLPTYP_OBJECT            5
/** CLPTYP_OVRLAY Overlay (KEYWORD.KEYWORD...) contains one of its list as in a C union */
#define CLPTYP_OVRLAY            6

#define CLPTYP_XALIAS           -1

/**
 * Method for property printing
 */
/** CLPPRO_MTD_ALL All properties are printed */
#define CLPPRO_MTD_ALL           0
/** CLPPRO_MTD_SET Only defined properties are printed */
#define CLPPRO_MTD_SET           1
/** CLPPRO_MTD_CMT All properties are printed, but not defined properties are line comments */
#define CLPPRO_MTD_CMT           2

/**
* Flags for command line parsing
*/
/** CLPFLG_NON To define no special flags */
#define CLPFLG_NON               0x00000000UL
/** CLPFLG_ALI This parameter is an alias for another argument (set by macros) */
#define CLPFLG_ALI               0x00000001UL
/** CLPFLG_CON This parameter is a constant definition (no argument, no link, no alias (set by macros)) */
#define CLPFLG_CON               0x00000002UL
/** CLPFLG_CMD If set the parameter is only used within the command line (command line only) */
#define CLPFLG_CMD               0x00000004UL
/** CLPFLG_PRO If set the parameter is only used within the property file (property file only) */
#define CLPFLG_PRO               0x00000008UL
/** CLPFLG_SEL If set only the predefined constants over the corresponding key words can be selected (useful to define selections) */
#define CLPFLG_SEL               0x00000010UL
/** CLPFLG_FIX This argument has a fixed length (only useful for strings if a typedef defines a fixed length per element, else set internally) */
#define CLPFLG_FIX               0x00000020UL
/** CLPFLG_BIN This argument can contain binary data without null termination (length must be known or determined with a link) */
#define CLPFLG_BIN               0x00000040UL
/** CLPFLG_DMY If set the parameter is not visible, meaning it is a dummy */
#define CLPFLG_DMY               0x00000080UL
/** CLPFLG_CNT This link will be filled by the calculated amount of elements (useful for arrays) */
#define CLPFLG_CNT               0x00000100UL
/** CLPFLG_OID This link will be filled by the object identifier (OID) of the chosen argument (useful for overlays) */
#define CLPFLG_OID               0x00000200UL
/** CLPFLG_ELN This link will be filled by the calculated length of a element (fixed types == data size, packed types == data length) */
#define CLPFLG_ELN               0x00001000UL
/** CLPFLG_SLN This link will be filled by the calculated string length for a element (only for null-terminated strings) */
#define CLPFLG_SLN               0x00002000UL
/** CLPFLG_TLN This link will be filled by the calculated total length for the argument (sum of all element lengths) */
#define CLPFLG_TLN               0x00004000UL
/** CLPFLG_DEF This flag enables to use the OID as default for numbers if no value is assigned (only the keyword is used (syntax extension))*/
#define CLPFLG_DEF               0x00008000UL
/** CLPFLG_CHR This flag will ensure that a binary string are interpreted by default as local character string (DEFAULT) */
#define CLPFLG_CHR               0x00010000UL
/** CLPFLG_ASC This flag will ensure that a binary string are interpreted by default as ASCII character string*/
#define CLPFLG_ASC               0x00020000UL
/** CLPFLG_EBC This flag will ensure that a binary string are interpreted by default as EBCDIC character string*/
#define CLPFLG_EBC               0x00040000UL
/** CLPFLG_HEX This flag will ensure that a binary string are interpreted by default as hexadecimal character string*/
#define CLPFLG_HEX               0x00080000UL
/** CLPFLG_PWD This flag will ensure that the clear value only put to the data structure but not traced, logged or given away elsewhere */
#define CLPFLG_PWD               0x01000000UL

/**
 *  Definition of CLPFLG macros
 */
#define CLPISF_ALI(flg)          ((flg)&CLPFLG_ALI)
#define CLPISF_CON(flg)          ((flg)&CLPFLG_CON)
#define CLPISF_CMD(flg)          ((flg)&CLPFLG_CMD)
#define CLPISF_PRO(flg)          ((flg)&CLPFLG_PRO)
#define CLPISF_DMY(flg)          ((flg)&CLPFLG_DMY)
#define CLPISF_SEL(flg)          ((flg)&CLPFLG_SEL)
#define CLPISF_FIX(flg)          ((flg)&CLPFLG_FIX)
#define CLPISF_BIN(flg)          ((flg)&CLPFLG_BIN)
#define CLPISF_CNT(flg)          ((flg)&CLPFLG_CNT)
#define CLPISF_OID(flg)          ((flg)&CLPFLG_OID)
#define CLPISF_ELN(flg)          ((flg)&CLPFLG_ELN)
#define CLPISF_SLN(flg)          ((flg)&CLPFLG_SLN)
#define CLPISF_TLN(flg)          ((flg)&CLPFLG_TLN)
#define CLPISF_DEF(flg)          ((flg)&CLPFLG_DEF)
#define CLPISF_PWD(flg)          ((flg)&CLPFLG_PWD)
#define CLPISF_CHR(flg)          ((flg)&CLPFLG_CHR)
#define CLPISF_ASC(flg)          ((flg)&CLPFLG_ASC)
#define CLPISF_EBC(flg)          ((flg)&CLPFLG_EBC)
#define CLPISF_HEX(flg)          ((flg)&CLPFLG_HEX)
#define CLPISF_LNK(flg)          (CLPISF_CNT(flg) ||  CLPISF_OID(flg) ||  CLPISF_ELN(flg) || CLPISF_SLN(flg) ||  CLPISF_TLN(flg))
#define CLPISF_ARG(flg)          ((!CLPISF_LNK(flg)) && (!CLPISF_CON(flg)) && (!CLPISF_ALI(flg)))
#define CLPISF_ENT(flg)          ((!CLPISF_LNK(flg)) && (!CLPISF_ALI(flg)))

/**
* Default source strings
*/
#define CLPSRC_CMD               ":command line:"
#define CLPSRC_PRO               ":property list:"
#define CLPSRC_DEF               ":default value:"
#define CLPSRC_ENV               ":environment variable:"
#define CLPSRC_PRF               ":property file:"
#define CLPSRC_CMF               ":command file:"
#define CLPSRC_PAF               ":parameter file:"

/**
* Symbol table walk operations
*/
/** CLPSYM_NON No operation done */
#define CLPSYM_NON               0x00000000UL
/** CLPSYM_ROOT Go to symbol table root */
#define CLPSYM_ROOT              0x00000001UL
/** CLPSYM_OLD Go to at last used symbol */
#define CLPSYM_OLD               0x00000002UL
/** CLPSYM_NEXT Go to the next symbol in the list */
#define CLPSYM_NEXT              0x00000004UL
/** CLPSYM_BACK Go to the previous symbol in the list */
#define CLPSYM_BACK              0x00000008UL
/** CLPSYM_DEP Go to the deeper level in the tree */
#define CLPSYM_DEP               0x00000010UL
/** CLPSYM_HIH Go to the higher level in the tree */
#define CLPSYM_HIH               0x00000020UL
/** CLPSYM_ALIAS Go to the alias symbol */
#define CLPSYM_ALIAS             0x00000100UL
/** CLPSYM_COUNT Go to the counter symbol */
#define CLPSYM_COUNT             0x00001000UL
/** CLPSYM_LINK Go to the entry link symbol */
#define CLPSYM_LINK              0x00004000UL

/** CLPSYM_OID Go to the entry object identifier symbol */
#define CLPSYM_OID               0x00008000UL
/** CLPSYM_ELN Go to the element length symbol */
#define CLPSYM_ELN               0x00002000UL
/** CLPSYM_SLN Go to the string length symbol */
#define CLPSYM_SLN               0x00010000UL
/** CLPSYM_SLN Go to the total length symbol */
#define CLPSYM_TLN               0x00020000UL

/**
 *  Definition of CLPSYM macros
 */
#define CLPISS_ROOT(flg)         ((flg)&CLPSYM_ROOT)
#define CLPISS_OLD(flg)          ((flg)&CLPSYM_OLD)
#define CLPISS_NEXT(flg)         ((flg)&CLPSYM_NEXT)
#define CLPISS_BACK(flg)         ((flg)&CLPSYM_BACK)
#define CLPISS_DEP(flg)          ((flg)&CLPSYM_DEP)
#define CLPISS_HIH(flg)          ((flg)&CLPSYM_HIH)
#define CLPISS_ALIAS(flg)        ((flg)&CLPSYM_ALIAS)
#define CLPISS_COUNT(flg)        ((flg)&CLPSYM_COUNT)
#define CLPISS_ELN(flg)          ((flg)&CLPSYM_ELN)
#define CLPISS_LINK(flg)         ((flg)&CLPSYM_LINK)
#define CLPISS_OID(flg)          ((flg)&CLPSYM_OID)
#define CLPISS_SLN(flg)          ((flg)&CLPSYM_SLN)
#define CLPISS_TLN(flg)          ((flg)&CLPSYM_TLN)

/**
 * @brief Defines a entry for symbol table walk
 */
typedef struct ClpSymWlk {
   /** Pointer to the key word */
   const char*                   pcKyw;
   /** Pointer to the alias */
   const char*                   pcAli;
   /** Flag value */
   unsigned long                 uiFlg;
   /** Pointer to the default supplement string */
   const char*                   pcDft;
   /** Pointer to the manual page (description) */
   const char*                   pcMan;
   /** Pointer to the help message */
   const char*                   pcHlp;
   /** Pointer to the path */
   const char*                   pcPat;
   /** Type of the symbol */
   int                           siTyp;
   /** Minimum number of entries */
   int                           siMin;
   /** Maximum number of entries */
   int                           siMax;
   /** Size of the symbol */
   int                           siSiz;
   /** Object identifier for the symbol */
   int                           siOid;
   /** Bitmask for possible operations */
   unsigned long                 uiOpr;
}TsClpSymWlk;

/**
 * @brief Defines a entry for symbol table update
 */
typedef struct ClpSymUpd {
   /** Pointer to a property supplement string replacing the current default value */
   const char*                   pcPro;
}TsClpSymUpd;

/**
 * Symbol table walk
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
   const unsigned long           uiOpr,
   TsClpSymWlk*                  psSym);

/**
 * Symbol table update
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

/**
 * @brief Defines a table of arguments
 *
 * To simplify the definition of the corresponding data structures and argument tables it is recommended to use the
 * CLPARGTAB macros defined in CLPMAC.h or for constant definitions the CLPCONTAB macros below.
 */
typedef struct ClpArgument {
 /** Type of this parameter (CLPTYP_xxxxxx)\n
 *           The type will be displayed in context sensitive help messages (TYPE: type_name)*/
   int                           siTyp;
   /** Pointer to a null-terminated key word for this parameter (:alpha:[:alnum:|'-'|'_']*) */
   const char*                   pcKyw;
   /** Pointer to another key word to define an alias (:alpha:[:alnum:|'-'|'_']*) */
   const char*                   pcAli;
   /** Minimum amount of entries for this argument (0-optional n-required) */
   int                           siMin;
   /** Maximum amount of entries for this argument (1-scalar n-array) */
   int                           siMax;
   /** If fixed size type (switch, number, float, object, overlay) then size of this type else (string) available size in memory
    * (string type can be defined as FIX with CLPFLG_FIX but this requires a typedef for this string size)*/
   int                           siSiz;
   /** Offset of an argument in a structure used for address calculation (please use offset of(t,m) macro) */
   int                           siOfs;
   /** Unique integer value representing the argument (object identifier, used in overlays or for switches) */
   int                           siOid;
   /** Flag value which can be assigned with CLPFLG_SEL/CON/FIX/CNT/SEN/ELN/TLN/OID/ALI to define different characteristics */
   unsigned long                 uiFlg;
   /** Pointer to another parameter table for CLPTYP_OBJECT and CLPTYP_OVRLAY describing these structures
    *  for CLPTYP_NUMBER, CLPTYP_FLOATN or CLPTYP_STRING to define selections (constant definitions) */
   struct ClpArgument*           psTab;
   /** Pointer to a zero-terminated string to define the default values assigned if no argument was defined.
    *  If this pointer is NULL or empty ("") then no initialization is done.
    *
    *  - for switches a number literal or the special keywords ON/OFF can be defined
    *  - for numbers a number literal or a key word for a constant definition can be defined
    *  - for floats a floating point number literal or a key word for a constant definition can be defined
    *  - for strings a string literal or a key word for a constant definition can be defined
    *  - for objects the special keyword INIT must be defined to initialize the object
    *  - for overlays the keyword of the assigning object must be defined to initialize the overlay
    *
    *  For arrays of these types a list of the corresponding values (literals or key words) can be defined
    *  The default values are displayed in context sensitive help messages (PROPERTY: [value_list])
    *  This value can be overruled by corresponding environment variable or property definition*/
   const char*                   pcDft;
   /** Pointer to a zero-terminated string for a detailed description of this argument (in ASCIIDOC format, content
    *  behind .DESCRIPTION, mainly simply some paragraphs). Can be a NULL pointer or empty string for constant definition
    *  or simple arguments. It is recommended to use a header file with a define for this long string (required for objects
    *  and overlays). All occurrences of "&{OWN}" or "&{PGM}" (that all their case variations) are replaced with the current
    *  owner or program name, respectively. All other content between "&{" and "}" is ignored (comment).
    *  The resulting text is converted on EBCDIC systems)*/
   const char*                   pcMan;
   /** Pointer to a zero-terminated string for context sensitive help to this argument. Also used as headline in
    * documentation generation. For this only alnum, blank, dot, comma, hyphen and parenthesis are used. At every other
    * separator the headline will be cut, meaning it is possible to have more help information than head line.
    * (converted on EBCDIC systems) */
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
 *  *man* Pointer to a null-terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string\n
 *  *hlp* Pointer to a null-terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_NUMBER(kyw,dat,man,hlp)       {CLPTYP_NUMBER,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),(dat), 0.0 ,NULL       },

/** defines a number literal with the command line keyword *kyw* and the value *dat*
 *
 *  *man* Pointer to a null-terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string\n
 *  *hlp* Pointer to a null-terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_FLOATN(kyw,dat,man,hlp)       {CLPTYP_FLOATN,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),  0  ,(dat),NULL       },

/** defines a floating point literal with the command line keyword *kyw* and the value *dat*
 *
 *  *man* Pointer to a null-terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string\n
 *  *hlp* Pointer to a null-terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_STRING(kyw,dat,man,hlp)       {CLPTYP_STRING,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat)},

/** defines a string literal with the command line keyword *kyw* and the value *dat*
 *
 *  *siz* Size of the string value
 *  *man* Pointer to a null-terminated string for a detailed description of this constant
 *        (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *        Can be a NULL pointer or empty string to produce a bullet list.
 *        It is recommended to use a header file with a define for this long string)\n
 *  *hlp* Pointer to a null-terminated string for context sensitive help for this constant
 *        (also used as head line or in bullet list in documentation generation).\n
 */
#define CLPCONTAB_BINARY(kyw,dat,siz,man,hlp)   {CLPTYP_STRING,(kyw),NULL,0,0,(siz),0,0,CLPFLG_CON|CLPFLG_BIN,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat)},

/** Ends a table with constant definitions
 */
#define CLPCONTAB_CLS                           {CLPTYP_NON   , NULL,NULL,0,0,  0  ,0,0,CLPFLG_NON           ,NULL,NULL, NULL, NULL,  0  , 0.0 ,NULL       }


/** Defines a structure with error information
 *
 * A pointer to this structure can be provided at siClpOpen() to have access to
 * the error information managed in the CLP handle.
 *
 *  The pointers are set by CLP and valid until CLP is closed.
 */
typedef struct ClpError {
   /** Points to a zero-terminated string containing the current error message */
   const char*                   pcMsg;
   /** Points to a zero-terminated string containing the current source.
    * The initial source can be defined for command line or property file parsing.
    * If the initial source is not defined the constant definitions below are used:
    * * for command line parsing    ":command line:"   see CLPSRC_CMD
    * * for property string parsing ":property list:"  see CLPSRC_PRO
    *
    * If a parameter file assigned and cause of the error *pcSrc* points to this file name*/
   const char*                   pcSrc;
   /** Points to an integer containing the current row for the error in *pcSrc* */
   const int*                    piRow;
   /** Points to an integer containing the current column for the error in *pcSrc* */
   const int*                    piCol;
}TsClpError;


/**
 * Open command line parser
 *
 * The function uses the argument table and corresponding structure and creates the handle for the command line parser (FLAMCLP)
 *
 * @param[in]  isCas Boolean to enable case sensitive parsing of keywords (recommended is FALSE)
 * @param[in]  isPfl Boolean to enable parameter files per object and overlay (recommended is TRUE)
 * @param[in]  siMkl Integer defining the minimal key word length (siMkl<=0 --> full length, no auto abbreviation)
 * @param[in]  pcOwn String constant containing the owner name for the root in the path ("limes")
 * @param[in]  pcPgm String constant containing the program name for the root in the path ("flcl")
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
 *                   certain error informations in the CLP handle. If pfErr defined all error information are printed
 *                   by CLP. In this case these structure is not required. If pfErr==NULL you can use these structure
 *                   to gather all error information of CLP in memory. The pointer are only valid until vsClpClose().
 *
 * @return void pointer to the memory containing the handle
 */
extern void* pvClpOpen(
   const int                     isCas,
   const int                     isPfl,
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
   const char*                   pcEnt,
   TsClpError*                   psErr);

/**
 * Parse the property list
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
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpParsePro(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcPro,
   const int                     isChk,
   char**                        ppLst);

/**
 * Parse the command line
 *
 * The function parses the command line and returns OK or the error code and error position (byte offset)
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  pcSrc Pointer to a null-terminated string containing the source name for the command line
 *                   If the command line taken from a file it is useful to provide this file name for error printing else use NULL.
 * @param[in]  pcCmd Pointer to a null-terminated string containing the command for parsing
 * @param[in]  isChk Boolean to enable (TRUE) or disable (FALSE) validation of minimum number of entries
 * @param[out] piOid If this pointer is set and the main table is an overlay the corresponding object identifier is returned
 * @param[out] ppLst Pointer to the parsed parameter list (NULL = no list provided) in the CLP handle
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpParseCmd(
   void*                         pvHdl,
   const char*                   pcSrc,
   const char*                   pcCmd,
   const int                     isChk,
   int*                          piOid,
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
 * @param[in]  isAli Print also aliases (if FLASE help don't show aliases)
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
 * @param[in]  pcCmd Qualifier for command head line (Recommended: "COMMAND" NULL pointer cause an error)
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
   const char*                   pcCmd,
   const int                     isDep,
   const int                     isMan,
   const int                     isNbr);

/**
 * Generate properties
 *
 * The function produces a property list with the current default values
 *
 * @param[in]  pvHdl Pointer to the corresponding handle created with \a pvClpOpen
 * @param[in]  siMtd Method for property printing (0-ALL, 1-Defined, 2-All but not defined as comment)
 * @param[in]  siDep Depth of next levels to print (1-One Level, 2-Two Level, ..., <9-All)
 * @param[in]  pcPat Path (root.input...) to limit the amount of properties
 * @param[in]  pfPro File pointer to write the property list (if NULL then pfHlp of FLAMCLP is used)
 *
 * @return signed integer with CLP_OK(0) or an error code (CLPERR_xxxxxx)
 */
extern int siClpProperties(
   void*                         pvHdl,
   const int                     siMtd,
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
 * Provides error message
 *
 * The function provides a error message for the corresponding error code
 *
 * @param[in]  siErr Error code from parser
 */
extern char* pcClpError(
   int               siErr);

/**********************************************************************/

#endif // INC_CLP_H

#ifdef __cplusplus
}
#endif
