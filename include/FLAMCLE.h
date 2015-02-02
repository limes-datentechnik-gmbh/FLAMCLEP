/******************************************************************************/
/*******************************************************************************/
/**
 * @file FLAMCLE.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
 *
 * LIMES Command Line Executor (FLAMCLE) in ANSI-C
 * @author limes datentechnik gmbh
 * @date 24.08.2014\n
 * @copyright (c) 2014 limes datentechnik gmbh
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

Description
-----------

With the command line executor (FLAMCLE), you can simply realize a complex
command line through the definition of some tables. No programming is
required to get the values parsed and stored in a free defined data
structure.

For this to work, a compiler with its own language was implemented, to
provide the same command line interface on each available platform. This
command line parser (FLAMCLP) is used by the FLAMCLE. Both components
provide extensive help for each command and many other support functions.

For a provided list of commands, the FLAMCLE uses a dedicated command
line processor (CLP) to make all these commands available, together with
several built-in functions.

To achieve this, a table must be defined where each row describes one
command. This table provides the input for the execution function doing
the command line interpretation. The whole library consists of only one
function and a structure to define the command table. One of these
commands or a built-in function can be defined as default, which will be
executed if the first keyword (argv[1]) don't fit one of the user-
defined commands or built-in functions. If no command or built-in
function defined and no default set the built-in function syntax will be
executed to show the capabilities of the command line program.

Beside the specified user-defined commands, the FLAMCLE provides several
powerful built-in functions (listed below). All built-in functions have
a manual page, implemented to display more information at runtime.

Based on the keyword, the short help message and the detailed
description, the built-in function GENDOCU can be used to generate a
complete user manual. Based on this capability the FLAMCLE completely
describes itself.

The self-documenting style of the whole program was one of the main
targets of this general command line interface. To understand the
interface specification, it is recommended to also read the FLAMCLP
documentation.

Features
--------

Below, you can find a possibly incomplete list of FLAMCLE feature:

 * * Support of an unlimited amount of commands
 * * Support of hidden commands (not documented)
 * * Support of a default command (optional)
 * * Includes a lot of useful built-in functions
 * * Simple owner management to differentiate configurations
 * * The logical program name can be freely defined
 * * Different view for property and command line parsing
 * * Case sensitive or in-sensitive command line interpretation
 * * Output file can be defined (stdout, stderr, or a real file)
 * * Complete trace file management for FLAMCLP and commands
 * * The look and feel can be defined freely
 * * Syntax, help and manpage support for program, commands and arguments
 * * Extensive documentation generation in ASCIIDOC format for the user manual
 * * Powerful property file management (generation, activation, update, ...)
 * * Simple configuration data management (own environment variables)
 * * Automatic keyword shortening for arguments
 * * Support for many data types, like:
 *   * Number (decimal, hexadecimal, octal, binary and time)
 *   * Float (decimal in all variants)
 *   * String (binary text/ASCII/EBCDIC/HEX or zero terminated)
 *   * Object (Structure)
 *   * Overlay (Union)
 *   * Array (List (realized as simplified notation))
 * * Support of constant definitions used as selection of values over keywords
 * * Internal calculated values are available as link (amount of values in an array, length of a string, object identifier in overlays, ...)
 * * The main table for a command can be defined as object or overlay
 * * Keyword, help message and detailed description can be freely defined for the program, each command, argument or constant definition
 * * Aliases for each argument can also be defined and are handled as options for the same value.
 * * Available and usable on each platform including WIN, UNIX, USS, ZOS, ...
 * * Support of STDENV as DD name for environment variables on mainframes
 * * Support property definitions over environment variables to overrule hard coded default properties
 * * Keywords (commands, built-in functions, ON, OFF, ALL, DEPTH1, ...) can start optional with "-" or "--"
 * * Support for parameter files per command, object or overlay
 * * File name mapping (+/<envar>) and DD:NAME support (see man_cle_main.txt)

Built-in Functions
------------------

All these built-in functions are available:

 * * SYNTAX   - Provides the syntax for each command
 * * HELP     - Provides quick help for arguments
 * * MANPAGE  - Provides manual pages (detailed help)
 * * GENDOCU  - Generates auxiliary documentation
 * * GENPROP  - Generates a property file
 * * SETPROP  - Activates a property file
 * * CHGPROP  - Updates property values in the current property file
 * * DELPROP  - Removes a property file from configuration
 * * GETPROP  - Displays current properties
 * * SETOWNER - Defines the current owner
 * * GETOWNER - Displays current owner setting
 * * SETENV   - Defines environment variables in the config file
 * * GETENV   - Displays the environment variables set in the config file
 * * DELENV   - Deletes environment variables in the config file
 * * TRACE    - Manages trace capabilities
 * * CONFIG   - Displays or clears the current configuration settings
 * * GRAMMAR  - Displays the grammar for commands and properties
 * * LEXEM    - Displays the regular expressions accepted in a command
 * * LICENSE  - Displays the license text for the program
 * * VERSION  - Lists version information for the program
 * * ABOUT    - Displays information about the program

To read the manual page, please use:
@code
   program MANPAGE function
@endcode

Below, you can find the syntax for each built-in function:

 * * SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]
 * * HELP [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]
 * * MANPAGE [function | command[.path][=filename]] | [filename]
 * * GENDOCU [command[.paht]=]filename [NONBR]
 * * GENPROP [command=]filename
 * * SETPROP [command=]filename
 * * CHGPROP command [path[=value]]*
 * * DELPROP [command]
 * * GETPROP [command[.path] [DEPTH1 | ... | DEPTH9 | DEPALL | DEFALL]]
 * * SETOWNER name
 * * GETOWNER
 * * SETENV variable=value
 * * GETENV
 * * DELENV variable
 * * TRACE ON | OFF | FILE=filename
 * * CONFIG [CLEAR]
 * * GRAMMAR
 * * LICENSE
 * * LEXEM
 * * VERSION
 * * ABOUT

Sample program
--------------
@code
    #include FLAMCLP.h
    #include FLAMCLE.h

    #define DEFINE_STRUCT
    #include "CLPMAC.h"
    #include "FL5TAB.h"
    #include "FL5STC.h"

    int main(const int argc, const char * argv[])
    {
       static TsFlcConvPar     stFlcConvPar;
       static TsClpConvPar     stClpConvPar;
       static TsClpFconPar     stClpFconPar;
       static TsClpGzipPar     stClpGzipPar;
       static TsClpIcnvPar     stClpIcnvPar;
       static TsFlcInfoPar     stFlcInfoPar;
       static TsClpInfoPar     stClpInfoPar;

    #undef DEFINE_STRUCT
    #include "CLPMAC.h"
    #include "FL5CON.h"
    #include "FL5ARG.h"

       CLETAB_OPN(asCmdTab) = {
          CLETAB_CMD("CONV",asClpConvPar,&stClpConvPar,&stFlcConvPar,NULL,siIniConv,siMapConv2Conv,siFluc,siFinConv,1,MAN_FLCL_CONV,"Data conversion")
          CLETAB_CMD("FCON",asClpFconPar,&stClpFconPar,&stFlcConvPar,NULL,siIniFcon,siMapFcon2Conv,siFluc,siFinConv,0,MAN_FLCL_FCON,"File conversion")
          CLETAB_CMD("GZIP",asClpGzipPar,&stClpGzipPar,&stFlcConvPar,NULL,siIniGzip,siMapGzip2Conv,siFluc,siFinConv,1,MAN_FLCL_GZIP,"Compression or decompression with zlib")
          CLETAB_CMD("ICNV",asClpIcnvPar,&stClpIcnvPar,&stFlcConvPar,NULL,siIniIcnv,siMapIcnv2Conv,siFluc,siFinConv,1,MAN_FLCL_ICNV,"Character conversion with libiconv")
          CLETAB_CMD("INFO",asClpInfoPar,&stClpInfoPar,&stFlcInfoPar,NULL,siIniInfo,siMapInfo2Info,siInfo,siFinInfo,1,MAN_FLCL_INFO,"Provide information")
          CLETAB_CLS
       };

       return(siCleExecute(asCmdTab,argc,argv,"de.limes","flcl",FALSE,0,stderr,stdout,
                           "--|","/",pcFlclVersion(),pcFlclAbout(),"TEST-LICENSE",
                           "Frankenstein Limes(R) Command Line for FLUC, FLAM and FLIES",
                           MAN_FLCL_MAIN,MAN_FLCL_COV,MAN_FLCL_GLS,MAN_FLCL_FIN,"CONV"));
    }
@endcode

Lexemes
-------

Lexemes (regular expressions) for argument list or parameter file:

    --| COMMENT   '#' [:print:]* '#'                              (will be ignored)
    --| LCOMMENT  ';' [:print:]* 'nl'                             (will be ignored)
    --| SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)
    --| OPERATOR  '=' | '.' | '(' | ')' | '[' | ']'  (SGN, DOT, RBO, RBC, SBO, SBC)
    --| KEYWORD   [:alpha:]+[:alnum: | '_' | '-']*              (always predefined)
    --| NUMBER    ([+|-]  [ :digit:]+)  |                       (decimal (default))
    --| num       ([+|-]0b[ :digit:]+)  |                                  (binary)
    --| num       ([+|-]0o[ :digit:]+)  |                                   (octal)
    --| num       ([+|-]0d[ :digit:]+)  |                                 (decimal)
    --| num       ([+|-]0x[ :xdigit:]+) |                             (hexadecimal)
    --| num       ([+|-]0t(yyyy/mm/tt.hh:mm:ss)) |  (relativ (+|-) or absolut time)
    --| FLOAT     ([+|-]  [ :digit:]+.[:digit:]+e|E[:digit:]+) | (decimal(default))
    --| flt       ([+|-]0d[ :digit:]+.[:digit:]+e|E[:digit:]+)            (decimal)
    --| STRING         ''' [:print:]* ''' |          (default (if binary c else s))
    --| str       [s|S]''' [:print:]* ''' |                (zero terminated string)
    --| str       [c|C]''' [:print:]* ''' |  (binary string in local character set)
    --| str       [a|A]''' [:print:]* ''' |                (binary string in ASCII)
    --| str       [e|E]''' [:print:]* ''' |               (binary string in EBCDIC)
    --| str       [x|X]''' [:print:]* ''' |         (binary string in hex notation)
    --| SUPPLEMENT     '"' [:print:]* '"' |   (zero terminated string (properties))

Grammar for command line
------------------------

Grammar for argument list, parameter file or property file

    --| Command Line Parser
    --|--| command        -> ['('] parameter_list [')']       (main=object)
    --|--|                |  ['.'] parameter                  (main=overlay)
    --|--| parameter_list -> parameter SEP parameter_list
    --|--|                |  EMPTY
    --|--| parameter      -> switch | assignment | object | overlay | array
    --|--| switch         -> KEYWORD
    --|--| assignment     -> KEYWORD '=' value
    --|--| object         -> KEYWORD '(' parameter_list ')'
    --|--| overlay        -> KEYWORD '.' parameter
    --|--| array          -> KEYWORD '[' value_list   ']'
    --|--|                |  KEYWORD '[' object_list  ']'
    --|--|                |  KEYWORD '[' overlay_list ']'
    --|--| value_list     -> value SEP value_list
    --|--|                |  EMPTY
    --|--| object_list    -> object SEP object_list
    --|--|                |  EMPTY
    --|--| overlay_list   -> overlay SEP overlay_list
    --|--|                |  EMPTY
    --|--| value          -> NUMBER | FLOAT | STRING | KEYWORD
    --| Property File Parser
    --|--| properties     -> property_list
    --|--| property_list  -> property SEP property_list
    --|--|                |  EMPTY
    --|--| property       -> keyword_list '=' SUPPLEMENT
    --|--| keyword_list   -> KEYWORD '.' keyword_list
    --|--|                |  KEYWORD
    --|--| SUPPLEMENT is a string in double quotation marks ("property")


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
 ******************************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLE_H
#define INC_CLE_H

#ifdef __HOSTSHORTING__
   #define pcCleVersion          FLCLEVSN
   #define pcCleAbout            FLCLEABO
   #define siCleExecute          FLCLEEXE
#endif


/**
* Get version information
*
* The function returns the version information for this library
*
* @param l level of visible hierarchy in the first 2 numbers of the string
*          the number can later be used to better visualize the hierarchy
* @param s size of the provided string buffer (including space for zero termination)
* @param b buffer for the version string
*          must contain a zero terminated string
*          the version string will be concatenated
*          the size including the 0-byte is the limit
*          if (strlen(b)==s-1) then more space is required for the complete version string
*          a good size for the version string is 256 byte
*
* @return pointer to a zero terminated version string (return(b))
*/
extern const char* pcCleVersion(const int l, const int s, char* b);

/**
* Get about information
*
* The function returns the about information for this library
*
* @param l level of visible hierarchy in the first 2 numbers of the string
*          the number can later be used to better visualize the hierarchy
* @param s size of the provided string buffer (including space for zero termination)
* @param b buffer for the about string
*          must contain a zero terminated string
*          the about string will be concatenated
*          the size including the 0-byte is the limit
*          if (strlen(b)==s-1) then more space is required for the complete about string
*          a good size for the about string is 1024 byte
*
* @return pointer to a zero terminated about string (return(b))
*/
extern const char* pcCleAbout(const int l, const int s, char* b);

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
 * @param[in]  pfOut File pointer for outputs (given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute)
 * @param[in]  pcOwn Current owner name (given over CleExecute)
 * @param[in]  pcPgm Current program name (given over CleExecute)
 * @param[out] pvClp Pointer to the corresonding FLAMCLP structure for initialisation
 *
 * @return     Error code (!=0) for termination or 0 for success
 */
typedef int (*tpfIni)(
   FILE*                         pfOut,
   FILE*                         pfTrc,
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
 * in the parameter structure. Means the user defines the file name, but
 * the executed subprogram gets the content of this file. Such things
 * are realized over a real mapping between the parsed values and the
 * needed arguments.
 *
 * For overlay based commands the pointer to the object identifier is
 * provided (taken from siCleExecute). This integer can then be used to
 * choose the correct data structure from the corresponding CLP union
 * for mapping. (The piOid can also (mis)used to give back a integer to
 * the caller of siCleExecute() from mapping if this pointer not NULL.)
 *
 * @param[in]  pfOut File pointer for outputs (mainly error messages, given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (mainly for complex stuff, given over CleExecute)
 * @param[in]  piOid Pointer to the object identifier for overlay commands, if the pointer set at siCleExecute()
 * @param[in]  pvClp Pointer to the filled FLAMCLP structure (output from the the command line parser)
 * @param[out] pvPar Pointer to the parameter structure, which will be filled based on the FLAMCLP structure with this function

 * @return     Error code (!=0) for termination or 0 for success
 */
typedef int (*tpfMap)(
   FILE*                         pfOut,
   FILE*                         pfTrc,
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
 * @param[in]  pfOut File pointer for outputs (given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute)
 * @param[in]  pcOwn Current owner name (given over CleExecute)
 * @param[in]  pcPgm Current program name (given over CleExecute)
 * @param[in]  pcVsn Current version information (given from CleExecute)
 * @param[in]  pcAbo Current about information (given from CleExecute)
 * @param[in]  pcLic Current license text (given from CleExecute)
 * @param[in]  pcFkt Current function name (key word of the command)
 * @param[in]  pcCmd Current command (complete entered line of user)
 * @param[in]  pcLst Current list of parsed arguments (given from FLAMCLP, could be NULL or empty)
 * @param[in]  pvPar Pointer to the filled parameter for the run of the subprogram

 * @return     Error code (!=0) for termination or 0 for success
 */
typedef int (*tpfRun)(
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcOwn,
   const char*                   pcPgm,
   const char*                   pcVsn,
   const char*                   pcAbo,
   const char*                   pcLic,
   const char*                   pcFkt,
   const char*                   pcCmd,
   const char*                   pcLst,
   const void*                   pvPar);

/**
 * Type definition for the fin function
 *
 * This function is called at the end after the run to clean up the
 * parameter structure. For example it could be there was a file pointer
 * which must be closed or memory which must be freed.
 *
 * @param[in]  pfOut File pointer for outputs (given over CleExecute)
 * @param[in]  pfTrc File pointer for tracing (given over CleExecute)
 * @param[in]  pvPar Pointer to the filled parameter structure for cleanup
 * @return     Error code (!=0) for termination or 0 for success
 */
typedef int (*tpfFin)(
   FILE*                         pfOut,
   FILE*                         pfTrc,
   void*                         pvPar);

/**
 * \struct TsCleCommand
 *
 * This structure is used to define a table of commands\n
 *
 * To simplify the definition of command tables it is recommended to use the CLPTAB macros below.
 *
 * Its members are:
 * \par
 * \b pcKyw  Pointer to the key word for this command (:alpha:[:alnum:|'-'|'_']*)\n
 * \b psTab  Pointer to the main argument table for this command (defines the semantic for the parser)\n
 * \b pvClp  Pointer to the corresponding argument structure (filled up by the parser)\n
 * \b pvPar  Pointer to the corresponding parameter structure (filled up by the mapping function)\n
 * \b pfIni  Pointer to the initialization function (initialize the argument structure in front of parsing)\n
 * \b pfMap  Pointer to the mapping function (transfers the argument structure to the parameter structure)\n
 * \b pfRun  Pointer to the executed function (use the mapped parameter structure to execute the command
 *           (for logging the function name, original command line and parsed argument list and other values are also provided))\n
 * \b pfFin  Pointer to the finish function for cleanup (free memory, close files in parameter structure)\n
 * \b uiFlg  Flag to indicate a hidden (==0) or visible (!=0) command,
 *           For correct numbering, put hidden commands to the end of the table\n
 * \b pcMan  Pointer to a zero terminated string for a detailed description of this command
 *           (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or.EXAMPLES)
 *           It is recommended to use a header file with a define for this long string)\n
 * \b pcHlp  String for a short context sensitive help to this command\n
 */
typedef struct CleCommand {
   const char*                   pcKyw;
   const TsClpArgument*          psTab;
   void*                         pvClp;
   void*                         pvPar;
   int*                          piOid;
   tpfIni                        pfIni;
   tpfMap                        pfMap;
   tpfRun                        pfRun;
   tpfFin                        pfFin;
   int                           siFlg;
   const char*                   pcMan;
   const char*                   pcHlp;
} TsCleCommand;

/** Starts a table with command definitions
 *
 *  *nam* Name of this table\n
 */
#define CLETAB_OPN(nam)                                           TsCleCommand nam[]

/** defines a command with the command line keyword *kyw*
 *
 *  *tab* Pointer to the main table for this command.\n
 *  *clp* Pointer to the corresponding FLAMCLP structure (generated with the CLPMAC.h marcros).\n
 *  *par* Pointer to the corresponding parameter structure.\n
 *  *oid* Pointer to an integer to define the main table as overlay or NULL to define the main table as object
 *        If the pointer set then the object identifier of the chosen argument of the overlay is given back.\n
 *  *ini* Pointer to the initialization function for the FLAMCLP structure (see tpfIni)\n
 *  *map* Pointer to mapping function (see tpfMap).
 *        The mapping functions maps the parsed content of the FLAMCLP structure in the PAR structure.\n
 *  *run* Pointer to the run function to execute the subprogram with the PAR structure (see tpfRun).\n
 *  *fin* Pointer to the finalization function to clean up the parameter structure (see tpfFin)\n
 *  *flg* Flag to indicate a hidden (==0) or visible (!=0) command,
 *        For correct numbering, put hidden commands to the end of the table\n
 *  *man* Pointer to a zero terminated string for a detailed description of this command.
 *        (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or .EXAMPLES)
 *        It is recommended to use a header file with a define for this long string)\n
 *  *hlp* String for a short context sensitive help to this command\n
 */
#define CLETAB_CMD(kyw,tab,clp,par,oid,ini,map,run,fin,flg,man,hlp)   {(kyw),(tab),(clp),(par),(oid),(ini),(map),(run),(fin),(flg),(man),(hlp)},

/** Ends a table with constant definitions
 */
#define CLETAB_CLS                                                    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0   , NULL, NULL}

/**
 * Execute command line
 *
 * The function uses the command line parsers to execute different commands based on argc and argv given in the main function of a program
 * and provides the additional built-in functions below:
 * - SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]
 * - HELP   [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]
 * - MANPAGE [function | command[.path][=filename]] | [filename]
 * - GENDOCU [command[.path]=]filename [NONBR]
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
 *
 * @param[in]  psTab Pointer to the table which defines the commands
 * @param[in]  argc  Number of command line parameters (argc of main(int argc, char* argv[]))
 * @param[in]  argv  List of pointers to the command line parameters (argv of main(int argc, char* argv[]))
 * @param[in]  pcOwn Default owner id (owner ids are used to identify properties and other things "com.company")
 * @param[in]  pcPgm Logical program name (can be different from argv[0] and will be used in the root "com.company.program")
 * @param[in]  isCas Switch to enable case sensitive interpretation of the command line (recommended is FLASE)
 * @param[in]  isPfl Switch to enable parameter file support for object and overlays (recommended is TRUE)
 * @param[in]  siMkl Integer defining the minimal key word length (siMkl<=0 --> full length, no auto abbreviation)
 * @param[in]  pfOut File pointer for help and error messages (if not defined stderr will be used)
 * @param[in]  pfTrc Default trace file if no trace file is defined with the configuration data management (recommended: NULL, stdout or stderr)
 * @param[in]  pcDep String to visualize hierarchies (recommended: "--|")
 * @param[in]  pcOpt String to separate options (recommended: "/")
 * @param[in]  pcEnt String to separate list entries (recommended: ",")
 * @param[in]  pcLic String containing the license information for this program (used by built-in function LICENSE)
 * @param[in]  pcVsn String containing the version information for this program (used by built-in function VERSION)
 * @param[in]  pcAbo String containing the about message for this program (used by built-in function ABOUT)
 * @param[in]  pcHlp Short help message for the whole program
 * @param[in]  pcMan Manual page for the whole program (as == 2.1 DESCRIPTION == in ASCIIDOC format, Level 3-4 can be used for sub chapters)
 * @param[in]  pcCov Cover sheets for documentation generation (Header (Title, Autor, Revision) and Preample in ASCIIDOC format)
 * @param[in]  pcGls Glossary for documentation generation (in ASCIIDOC format (term:: explanation)),
 *             if NULL then no glossary are generated, if "" then only the FLAMCLP glossary is added)
 * @param[in]  pcFin Final pages for documentation generation (colophon, copyright, closing aso. in ASCIIDOC format)
 * @param[in]  pcDef Default command or built-in function, which is executed if the first keyword (argv[1]) don't match (if NULL then no default)
 *
 * @return signed integer with values below:\n
 * 0  - command line, command syntax, mapping, execution and finish of the command was successfull\n
 * 1  - command line, command syntax, mapping, execution was ok but finish of the command failed\n
 * 2  - command line, command syntax and mapping was ok but execution of the command failed\n
 * 4  - command line and command syntax was ok but mapping failed\n
 * 6  - command line was ok but command syntax was wrong\n
 * 8  - command line was wrong (user error)\n
 * 10 - initialization is wrong (user error)\n
 * 11 - configuration is wrong (user error)\n
 * 12 - table error (something within the predefined tables is wrong)\n
 * 16 - system error (mainly memory allocation or some thing like this failed)\n
 * 20 - access control or license error\n
 * 24 - fatal error (basic things are damaged)
 */
extern int siCleExecute(
   const TsCleCommand*           psTab,
   int                           argc,
   char*                         argv[],
   const char*                   pcOwn,
   const char*                   pcPgm,
   const int                     isCas,
   const int                     isPfl,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   const char*                   pcLic,
   const char*                   pcVsn,
   const char*                   pcAbo,
   const char*                   pcHlp,
   const char*                   pcMan,
   const char*                   pcCov,
   const char*                   pcGls,
   const char*                   pcFin,
   const char*                   pcDef);

#endif /*INC_CLE_H*/

#ifdef __cplusplus
}
#endif
