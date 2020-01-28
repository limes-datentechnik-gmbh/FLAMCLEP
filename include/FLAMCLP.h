/**********************************************************************/
/**
 * @file FLAMCLP.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>P</b>arsing
 * @author limes datentechnik gmbh
 * @date 26.01.2017
 * @copyright (c) 2017 limes datentechnik gmbh
 *
 *@mainpage LIMES Command Line Parser (FLAMCLP) in ANSI-C
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
 * @section DESC CLP-Description
 * The command line parser (FLAMCLP) is a complier which reads a command
 * string using the lexems and grammar below to fill a structure with the
 * corresponding values given in this line. The FLAMCLP works only in memory
 * (except parameter files are used for objects, overlays, arrays, arguments or
 * string files) and the syntax and semantic will be defined by a tree of
 * tables. Such a table can represent an object (struct) or an overlay (union).
 * Each argument in such a table can be a object or overlay again in using
 * another table for this type. Basic types are switches, numbers, floats or
 * strings (time and date are implemented as number in seconds from 1970).
 * With each argument you can define the required minimum and possible maximum
 * amount of occurrences. This means that each argument can be an array and
 * arrays are implemented as simplified notations. Arrays and strings can be
 * a fixed length part of the data structure or dynamic allocated by CLP. In
 * the last case, the fix part of the data structure is a pointer to the dynamic
 * allocated data area (use '->' instead of '.'). All dynamic allocated data
 * blocks are managed by CLP. If you close the CLP you can define if anything
 * including the dynamic parts of the CLP structure is closed. Or anything is
 * freed except the dynamic blocks allocated for the CLP structure. In this case
 * you can keep the CLP handle open, to free the remaining buffers later or
 * you can close the CLP handle and the dynamic allocated memory of the CLP
 * structure must be free by the application.
 *
 * For object, overlays and arrays you can provide parameter files (OBJECT='filename')
 * containing the parameter string in the corresponding syntax for these object,
 * overlay or array (KYW[='filename']). With '=>' you can also use parameter files
 * for normal arguments. The operator '=>' is also permitted for objects, overlays
 * and arrays.
 *
 * To read such a parameter file as string into the memory a handle and a
 * callback function can be provided. If the parameter NULL a default
 * implementation is used. If you provide your own function you can include
 * for example URL support, remote access, character conversion aso. The
 * handle is given to the callback function. The default implementation don't
 * need any handle, but you can use it for example for the character conversion
 * module, a remote session or something else.
 *
 * To handle passwords and passphrase more secure, you can provide a filename
 * as string (PASSWD=f'filename'), which contains the corresponding string
 * value. This prevents for example passwords from logging.
 *
 * An optional callback function with handle for additional authorization
 * checking can be provided. The resource will be each path written to the
 * CLP structure. If the pointer to the callback function is NULL then the
 * function is not called. This feature is mainly for RACF on z/OS.
 *
 * To support critical punctuation characters on EBCDIC systems a complex
 * support was implemented to make the whole source independent of the
 * used EBCDIC code page. The code page to use must be defined in the
 * environment variable LANG or just for CLP strings with the environment
 * variable CLP_STRING_CCSID or inside the CLP string ("&nnnn;"). Last but
 * not least single character escaping ("&xxx;") is supported as well.
 *
 * In the command string (everywhere, where the scanner start to read a lexem)
 * each value in angle brackets will be transparently replaced by the corresponding
 * environment variable, except in strings.
 *
 * The FLAMCLP uses these tables as symbol tables to define the syntax and
 * semantic of a command. The same table provides the offset used to
 * store the parsed values. This offset occurs in a real data structure
 * and with CLPMAC.h you can use the same macro to build the tables and
 * corresponding structures and unions. This is not mandatory, but we
 * recommend to use the macro in order to be in sync.
 *
 * The FLAMCLP provides also all internally calculated values in this data
 * structure. The mechanism is called linking. Thus you have to use the
 * same keyword for linking eventually with a calculated value of that
 * argument. For example, if you define an array of numbers then you can
 * define a link to determine the amount of entered numbers or for an
 * overlay you can link the corresponding object identifier to determine
 * which of the arguments are chosen by the user. If You define overlays of
 * overlays an additional dimension for each level is used. In this case
 * you must define an array for this link and You get the child (lnk[0])
 * before the parent (lnk[1]) written in the CLP structure. If the OID is
 * 0, then it will not be add to the array. This is useful if the OIDs of
 * the children are already unique.
 *
 * You can also get the string length and other features. The kind of link
 * is defined over the flags field. There are a lot of other flags supported
 * beside links, for example the PWD flag, which tells CLP that this value
 * are only clear in the data structure but always obfuscated in logs, traces
 * and other printouts to keep the value secret. Another flag can be used
 * for numbers. With CLPFLG_DEF you can activate a extension of the syntax.
 * If this flag used for a number then the object identifier is assigned as
 * value if no assignment done for this number. This means that with this extended
 * syntax you can define a switch, which you can assign a number. This is
 * useful for example to activate a feature with a default value by using
 * only the key word and the user can change the default value by an optional
 * assignment of another value.
 *
 * The FLAMCLP also supports aliases. An alias points to another argument
 * and is only an additional keyword that can be used. The maximum length
 * of a keyword or alias cannot exceed 63 character.
 *
 * To be compatible with certain shells the features below are implemented.
 *
 * * Strings can be enclosed with '' or "" or ``
 * * Strings can also be defined without quotes
 * * Explicit keywords can start with "-" or "--" in front of the qualifier
 * * If it is unique then parenthesis and the dot can be omitted for objects and overlays
 * * On EBCDIC systems we use a code page specific interpretation of punctuation characters
 *
 * Besides arguments you can also have a constant definition for
 * selections. A feature is useful in order to define keywords for values
 * (numbers, floats and strings). With help of the selection flag you can
 * enforce the pure acceptance of predefined keywords.
 *
 * Additional hard coded key words (see lexems) can be used in constant
 * expressions to build values and strings (value=64KiB).
 *
 * For each argument or constant you must define a keyword and a short
 * help message. If you provide a detailed description, then this argument
 * becomes an own chapter in the generated documentation, a manual page
 * will be available and extensive help is displayed. The description
 * string can contain &{OWN} for the current owner or &{PGM} for the
 * current program name. The letter case of the replacement string depends
 * and the letter case of the keyword: PGM = upper case, pgm = lower case,
 * Pgm = title case, pGm = original string. All other content inside of
 * &{...} is ignored. This can be used, for example, to insert comments
 * into the source of the manual page.
 *
 * For each argument you can define a default value and use the property
 * parser or environment variables to overwrite it again. The default value
 * replaces the entered value. This means that if a default value, environment
 * variable or property is defined, then this will have the same effect as the
 * entry of the value in the command line. With the latter you can still override
 * the hard coded or property default value. The property management can make use
 * of a function that extracts a property list for the argument table tree.
 *
 * For each path you can also define the default value as environment variable.
 * The path are prefixed with the owner ID and the program name first, then
 * only the program name and at the last the path only starting with the
 * command name will be use to determine a environment variable. For this the
 * path is converted to upper case and all '.' are replaced by '_'. The value
 * of the environment variable must contain the same supplement string which
 * are required for the property definition. All possible path values can be
 * determine with the property generation function.
 *
 * With the CLP flags CMD (for command) and PRO (property) you can define if
 * a parameter is only visible in the command line or property file. These
 * flags have no influence of property or command line parsing. It only
 * reflects the online help/syntax and docu/property generation. This means
 * that you can still use such a parameter in the property file or in the
 * command line, but it is not directly visible to the user. If the flags CMD
 * and PRO are not set then the parameter will be visible in both areas. With
 * the flag DMY (for dummy) you can enforce that this parameter is not
 * visible in a generated property file, on the command line help, syntax
 * and documentation. In this case, the parameter is no part of the symbol
 * table. It is only part of the CLP structure.
 *
 * For binary strings the default interpretation can be free defined over a
 * additional set of flags (CLPFLG_HEX/CHR/ASC/EBC). This is useful for hex
 * strings or passwords. If you want use arrays in overlays you cannot use
 * a link to determine the count or length. In this case you can use the DLM
 * flag. In this case for fix size types an additional empty element are used
 * as delimiter. For the static case the max count are reduced by 1 and in the
 * dynamic case one additional element is allocated to determine the end of
 * the array. For variable (CLPFLG_FIX is not defined) strings the end of the
 * list of strings are marked with 0xFF.
 *
 * The FLAMCLP calculates automatically the minimum amount of letters
 * required to make the meaning of a keyword unique. Depending on the case
 * mode the required letters are highlighted in the interactively used help
 * function. The syntax function provides also data when an argument
 * is required or optional, based on the minimum amount of occurrences.
 *
 * If you intend to apply the FLAMCLP first of all an open will be necessary.
 * Then you are able to parse a property list before doing this with the
 * command line. Both property list and command line are provided as zero
 * terminated strings. This means that the FLAMCLP does not know whether the
 * command line results from a file or argc/argv.
 *
 * If the isPfl (is parameter file) flag TRUE: For objects, overlays and arrays
 * you can use the assignment letter '=' or '=>' to define a parameter file containing
 * the command string for this object, overlay or array. For simple arguments
 * you must use '=>' to define a parameter file but all these capabilities are
 * only supported if the flag defined to true. This means that for each object,
 * overlay, array or argument a dedicated parameter file can be used. The
 * parameter file must contain a command string which syntax is valid for the
 * certain object, overlay, array or argument. CLP open the file with format
 * string "r". To use DD names on mainframes the file name must like "DD:name".
 *
 * If the flag CLPFLG_PWD is used, string outputs containing passwords will
 * result in "###SECRECT###" and float or number outputs in a value of 0.
 *
 * For zero terminated strings in local character set (s'...') several special
 * mapping and conversions can be activated over the flags CLPFLG_FIL/LAB/UPP.
 * The replacement of environment variables is done for each string but you can
 * also activate prefix adjustment and tilde replacement for files, and tilde,
 * circumflex and exclamation mark replacement for key labels. Additional you
 * can ensure that each such string are converted to upper case.
 *
 * Parsing of the properties (can be done a lot of times over different
 * sources) only change the default values in the symbol table and has no
 * effect for the CLP structure. First after parsing the command line the
 * corresponding FLAMCLP structure is filled with the properties or entered
 * values and the FLAMCLP can be closed or another command line parsed.
 *
 * Attention: If pointer to values in the CLP handle used (ppLst, psErr) then
 * you cannot close the CLP or you must copy the values before.
 *
 * The normal procedure to use the CLP:
 * <pre>
 *    ClpOpen()
 *    ClpParsePro()
 *    ClpParseCmd()
 *    ClpClose()
 * </pre>
 *
 * Beside property and command line parsing the FLAMCLP offers an interactive
 * syntax and help function. Additionally, you can use a very powerful
 * function to generate single manual pages or complete user manuals,
 * You can make use of the supported grammar and regular expressions
 * (lexems). Provided manual pages must be in ASCIIDOC and will be converted
 * on EBCDIC systems from the compile code page in the local code page.
 *
 * Only ClpParseCmd() uses the pvDat pointer. All other functions only work
 * on the symbol table. This means if you don't use ClpParseCmd() the pointer
 * to the CLP structure (pvDat), it can be NULL. This is useful if only help,
 * syntax, documentation or property management are required. For these
 * functions no corresponding CLP structure must be allocated.
 *
 * The implementation of the FLAMCLP is finished with the Command Line
 * Executor (FLAMCLE) with which you can define your list of commands by
 * using an additional table. You can make use of only one new function
 * that is executed eventually. The FLAMCLE offers an extensive built-in
 * functionality and is the optimal access method to the FLAMCLP capabilities.
 *
 * Additional there is an interface to browse the symbol table. These interface
 * can for example used to build several graphical user interfaces or other
 * things based on the tables.
 *
 * @section SUPREGEXP Supported regular expressions (lexems) and grammar
 *
 * Call siClpLexem() or siClpGrammar() to get the current supported lexems
 * and grammar. The list below could be a older state of the implementation.
 *
 * @section LEXEME CLP-Lexeme
 *
 *
 * <pre>
 * Lexemes (regular expressions) for argument list or parameter file:
 * --| COMMENT   '#' [:print:]* '#'                              (will be ignored)
--| LCOMMENT  ';' [:print:]* 'nl'                             (will be ignored)
--| SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)
--| OPERATOR '=' | '.' | '(' | ')' | '[' | ']' | (SGN, DOT, RBO, RBC, SBO, SBC)
--|  '=>'| '+' | '-' | '*' | '/' | '{' | '}' (SAB, ADD, SUB, MUL, DIV, CBO,CBC)
--| KEYWORD   ['-'['-']][:alpha:]+[:alnum: | '_']*          (always predefined)
--| NUMBER    ([+|-]  [ :digit:]+)  |                       (decimal (default))
--| num       ([+|-]0b[ :digit:]+)  |                                  (binary)
--| num       ([+|-]0o[ :digit:]+)  |                                   (octal)
--| num       ([+|-]0d[ :digit:]+)  |                                 (decimal)
--| num       ([+|-]0x[ :xdigit:]+) |                             (hexadecimal)
--| num       ([+|-]0t(yyyy/mm/tt.hh:mm:ss)) |  (relativ (+|-) or absolut time)
--| FLOAT     ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) | (decimal(default))
--| flt       ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)            (decimal)
--| STRING         ''' [:print:]* ''' |          (default (if binary c else s))
--| str       [s|S]''' [:print:]* ''' |                (null-terminated string)
--| str       [c|C]''' [:print:]* ''' |  (binary string in local character set)
--| str       [a|A]''' [:print:]* ''' |                (binary string in ASCII)
--| str       [e|E]''' [:print:]* ''' |               (binary string in EBCDIC)
--| str       [x|X]''' [:print:]* ''' |         (binary string in hex notation)
--| str       [f|F]''' [:print:]* ''' | (read string from file (for passwords))
--|           Strings can contain two '' to represent one '
--|           Strings can also be enclosed in " or ` instead of '
--|           Strings can directly start behind a '=' without enclosing ('`")
--|              In this case the string ends at the next separator or operator
--|              and keywords are preferred. To use keywords, separators or
--|              operators in strings, enclosing quotes are required.
--|
--| The predefined constant keyword below can be used in a value expressions
--| NOW       NUMBER - current time in seconds since 1970 (+0t0000)
--| MINUTE    NUMBER - minute in seconds (60)
--| HOUR      NUMBER - hour in seconds   (60*60)
--| DAY       NUMBER - day in seconds    (24*60*60)
--| YEAR      NUMBER - year in seconds   (365*24*60*60)
--| KiB       NUMBER - kilobyte          (1024)
--| MiB       NUMBER - megabyte          (1024*1024)
--| GiB       NUMBER - gigabyte          (1024*1024*1024)
--| TiB       NUMBER - terrabyte         (1024*1024*1024*1024)
--| RNDn      NUMBER - simple random number with n * 8 bit in length (1,2,4,8)
--| PI        FLOAT  - PI (3.14159265359)
--| LCSTAMP   STRING - current local stamp in format:           YYYYMMDD.HHMMSS
--| LCDATE    STRING - current local date in format:            YYYYMMDD
--| LCYEAR    STRING - current local year in format:            YYYY
--| LCYEAR2   STRING - current local year in format:            YY
--| LCMONTH   STRING - current local month in format:           MM
--| LCDAY     STRING - current local day in format:             DD
--| LCTIME    STRING - current local time in format:            HHMMSS
--| LCHOUR    STRING - current local hour in format:            HH
--| LCMINUTE  STRING - current local minute in format:          MM
--| LCSECOND  STRING - current local second in format:          SS
--| GMSTAMP   STRING - current Greenwich mean stamp in format:  YYYYMMDD.HHMMSS
--| GMDATE    STRING - current Greenwich mean date in format:   YYYYMMDD
--| GMYEAR    STRING - current Greenwich mean year in format:   YYYY
--| GMYEAR2   STRING - current Greenwich mean year in format:   YY
--| GMMONTH   STRING - current Greenwich mean month in format:  MM
--| GMDAY     STRING - current Greenwich mean day in format:    DD
--| GMTIME    STRING - current Greenwich mean time in format:   HHMMSS
--| GMHOUR    STRING - current Greenwich mean hour in format:   HH
--| GMMINUTE  STRING - current Greenwich mean minute in format: MM
--| GMSECOND  STRING - current Greenwich mean second in format: SS
--| GMSECOND  STRING - current Greenwich mean second in format: SS
--| SnRND10   STRING - decimal random number of length n (1 to 8)
--| SnRND16   STRING - hexadecimal random number of length n (1 to 8)
--|
--| SUPPLEMENT     '"' [:print:]* '"' |   (null-terminated string (properties))
--|           Supplements can contain two "" to represent one "
--|           Supplements can also be enclosed in ' or ` instead of "
--|           Supplements can also be enclosed in ' or ` instead of "
--| ENVIRONMENT VARIABLES '\<'varnam'\>' will replaced by the corresponding value
--| Escape sequences for critical punctuation characters on EBCDIC systems
--|    '!' = '\&EXC;'   - Exclamation mark
--|    '$' = '\&DLR;'   - Dollar sign
--|    '#' = '\&HSH;'   - Hashtag (number sign)
--|    '@' = '\&ATS;'   - At sign
--|    '[' = '\&SBO;'   - Square bracket open
--|    '\' = '\&BSL;'   - Backslash
--|    ']' = '\&SBC;'   - Square bracket close
--|    '^' = '\&CRT;'   - Caret (circumflex)
--|    '`' = '\&GRV;'   - Grave accent
--|    '{' = '\&CBO;'   - Curly bracket open
--|    '|' = '\&VBR;'   - Vertical bar
--|    '}' = '\&CBC;'   - Curly bracket close
--|    '~' = '\&TLD;'   - Tilde
--| Define CCSIDs for certain areas in CLP strings on EBCDIC systems (0-reset)
--|    '&' [:digit:]+ ';  (..."&1047;get.file='&0;%s&1047;'",f)
--| Escape sequences for hexadecimal byte values
--|    '&' ['X''x'] :xdigit: :xdigit: ';' ("&xF5;")
</pre>

 * @section LEXEME Grammar for CLP-command line
------------------------

Grammar for argument list or parameter file
--| Command Line Parser
--| command        -> ['('] parameter_list [')']       (main=object)
--|                |  ['.'] parameter                  (main=overlay)
--| parameter_list -> parameter SEP parameter_list
--|                |  EMPTY
--| parameter      -> switch | assignment | object | overlay | array
--| switch         -> KEYWORD
--| assignment     -> KEYWORD '=' value
--|                |  KEYWORD '=' KEYWORD # SELECTION #
--|                |  KEYWORD '=>' STRING # parameter file #
--| object         -> KEYWORD ['('] parameter_list [')']
--|                |  KEYWORD '=' STRING # parameter file #
--|                |  KEYWORD '=>' STRING # parameter file #
--| overlay        -> KEYWORD ['.'] parameter
--|                |  KEYWORD '=' STRING # parameter file #
--|                |  KEYWORD '=>' STRING # parameter file #
--| array          -> KEYWORD '[' value_list   ']'
--|                |  KEYWORD '[' object_list  ']'
--|                |  KEYWORD '[' overlay_list ']'
--|                |  KEYWORD '=' value_list # with certain limitations #
--|                |  KEYWORD '[=' STRING ']' # parameter file #
--|                |  KEYWORD '[=>' STRING ']' # parameter file #
--| value_list     -> value SEP value_list
--|                |  EMPTY
--| object_list    -> object SEP object_list
--|                |  EMPTY
--| overlay_list   -> overlay SEP overlay_list
--|                |  EMPTY
--| A list of objects requires parenthesis to enclose the arguments
--|
--| value          -> term '+' value
--|                |  term '-' value
--|                |  term
--| term           -> factor '*' term
--|                |  factor '/' term
--|                |  factor
--| factor         -> NUMBER | FLOAT | STRING
--|                |  selection | variable | constant
--|                |  '(' value ')'
--| selection      -> KEYWORD # value from a selection table        #
--| variable       -> KEYWORD # value from a previous assignment    #
--|                |  KEYWORD '{' NUMBER '}' # with index for arrays #
--| constant       -> KEYWORD # see predefined constants at lexem   #
--| For strings only the operator '+' is implemented as concatenation
--| Strings without an operator in between are also concatenated
--| A number followed by a constant is a multiplication (4KiB=4*1024)

A list of objects requires parenthesis to enclose the arguments. Only
for one object of a certain level you can omit the round brackets. If
you want define more than one object of a certain level you must use
parenthesis to separate the objects from each other. In parameter files
the command string for an overlay can be start with a dot '.' or not.
The same is valid for the parenthesis '(...)' of an object.

 * @section LEXEME Grammar for property file
-------------------------

Grammar for property file
--| Property File Parser
--| properties     -> property_list
--| property_list  -> property SEP property_list
--|                |  EMPTY
--| property       -> keyword_list '=' SUPPLEMENT
--| keyword_list   -> KEYWORD '.' keyword_list
--|                |  KEYWORD
--| SUPPLEMENT is a string in double quotation marks ("property")

 * @section LEXEME Compiler switches
-----------------

For compilation the defines below must be set:

    __DEBUG__        for a debug build
    __RELEASE__      for a release build
    __WIN__          for WINDOWS platforms
    __ZOS__          for ZOS mainframe platforms
    __USS__          for unix system services (USS) on ZOS mainframe platforms
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

#include <stdio.h>
#include "CLPDEF.h"

/**********************************************************************/

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
   #define siClpLexem            FLCLPLEX
   #define siClpGrammar          FLCLPGRM
   #define vdClpClose            FLCLPCLS
   #define pcClpError            FLCLPERR
#endif

/**********************************************************************/

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
 * @param pvHdl Pointer to the corresponding handle created with \a pvClpOpen
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
extern int siClpLexem(
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
   void*             pvHdl,
   const int         siMtd);

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

/**
 * @brief Generate documentation using a callback function
 *
 * This function works like siClpDocu, but it gives each page to a
 * callback function and don't print it to a certain file.
 *
 * @param pvHdl   Pointer to the corresponding handle created with \a pvClpOpen
 * @param pcFil   Prefix for file name building
 * @param pcNum   Leading number for table of contents ("1.2." used or not used depend on isNbr, NULL pointer cause an error)
 * @param pcKnd   Qualifier for command/otherclp head line (Recommended: "COMMAND" or "STRING" NULL pointer cause an error)
 * @param isCmd   If TRUE for command and FALSE for other CLP strings (required for anchor generation).
 * @param isDep   If TRUE then all deeper parts are printed if FALSE then not.
 * @param isAnc   Boolean to enable write of generated anchors for each command (only for doc type book)
 * @param isNbr   Boolean to enable header numbering for generated documentation (only for doc type book)
 * @param isShl   Boolean to enable short headline without type specification (only for doc type book)
 * @param isIdt   Boolean to enable printing of generated index terms (only for doc type book)
 * @param isPat   Boolean to enable printing of path as part of the synopsis (only for doc type book)
 * @param uiLev   If > 0 then headlines are written with this amount of '=' in front instead of underlining (only for doc type book)
 * @param siPs1   Character to separate parts to build filename outside command path (only for doc type book)
 * @param siPs2   Character to separate parts to build filename inside command path (only for doc type book)
 * @param siPr3   Character to replace non alpha-numerical characters in file names (only for doc type book)
 * @param pvPrn   Handle for the print callback function (created with TfCleOpenPrint (opnHtmlDoc))
 * @param pfPrn   Pointer to the callback function TfClpPrintPage (prnHtmlDoc)
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

/**********************************************************************/

#endif // INC_FLAMCLP_H

#ifdef __cplusplus
}
#endif
