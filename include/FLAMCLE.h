/**
 * @file FLAMCLE.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
 *
 * LIMES Command Line Executor (FLAMCLE) in ANSI-C
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

With the built-in function help a the short help message or the detailed
description can determined for each parameter and command using a dotted path.
The built-in function GENDOCU can be used to generate a part or the complete
user manual. Based on this capability the FLAMCLE completely describes itself.

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
 * * Environment variable replacement in the command string('<'envar'>')
 * * Automatic keyword shortening for arguments
 * * Support for many data types, like:
 *   * Number (decimal, hexadecimal, octal, binary and time)
 *   * Float (decimal in all variants)
 *   * String (binary text/ASCII/EBCDIC/HEX or from a file (for passwords))
 *   * Object (Structure) with parameter file support
 *   * Overlay (Union) with parameter file support
 *   * Array (List (realized as simplified notation) with parameter file support)
 * * Support of constant definitions used as selection of values over keywords
 * * Internal calculated values are available as link (amount of values in an array, length of a string, object identifier in overlays, ...)
 * * The main table for a command can be defined as object or overlay
 * * Keyword, help message and detailed description can be freely defined for the program, each command, argument or constant definition
 * * Aliases for each argument can also be defined and are handled as options for the same value.
 * * Available and usable on each platform including WIN, UNIX, USS, ZOS, ...
 * * Support of 'STDENV' as DD name or DSN '&SYSUID..STDENV' for environment variables on mainframes
 * * Support property definitions over environment variables to override hard coded default properties
 * * Keywords (commands, built-in functions, ON, OFF, ALL, DEPTH1, ...) can start optional with "-" or "--"
 * * Support for parameter files per command, object, overlay or array
 * * File name mapping and DD:NAME support (see man_cle_main.txt)
 * * Return/condition/exit code and reason code handling
 * * On EBCDIC systems we use a code page specific interpretation of punctuation characters (<pre>!$#@[\]^`{|}~</pre>) dependent on the environment variable LANG
 * * Extensive manual page management including replacement of owner (&{OWN}) and program name (&{PGM})
 * * * Own tool to generate description strings from text files including replacement of constant definitions (${__VERSION__})
 * * Definition of maximum and minimum condition code (MAXCC) for command execution
 * * Support SILENT and QUITE to control outputs
 * * Special condition code handling (incl. description for manual and built-in function ERRORS)
 * * Strings can be read from files to increase protection and prevent logging of passwords
 * * Default parameter file name for system supporting static file allocation ("DD:MYPAR")
 * * You can exclude the run after mapping if you provide the corresponding return code (siNoR)
 * * Own file to string callback function for parameter files

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
 * * ERRORS   - Displays information about return and reason codes of the program

To read the manual page, please use:
@code
   program MANPAGE function
@endcode

Below, you can find the syntax for each built-in function:

 * * SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]
 * * HELP [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]
 * * MANPAGE [function | command[.path][=filename]] | [filename]
 * * GENDOCU [command[.paht]=]filename [NONBR] [SHORT]
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
 * * ERRORS

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

       CLEOTH_OPN(asOthTab) = {
          CLETAB_OTH("FLCBYT","FORMAT","Read format string"       ,asClpWrtFmtPar,MAN_FLCBYT_READ_FORMAT ,HLP_FLCBYT_READ_FORMAT ,TRUE)
          CLETAB_OTH("FLCBYT","FORMAT","Write format string"      ,asClpRedFmtPar,MAN_FLCBYT_WRITE_FORMAT,HLP_FLCBYT_WRITE_FORMAT,TRUE)
          CLETAB_OTH("FLCBYT","READ"  ,"Read conversion string"   ,asClpElmCnvRed,MAN_FLCBYT_CONV_READ   ,HLP_FLCBYT_CONV_READ   ,TRUE)
          CLETAB_OTH("FLCBYT","WRITE" ,"Write conversion string"  ,asClpElmCnvWrt,MAN_FLCBYT_CONV_WRITE  ,HLP_FLCBYT_CONV_WRITE  ,TRUE)
          CLETAB_OTH("FLCBYT","CONV"  ,"From-To conversion string",asClpElmCnv   ,MAN_FLCBYT_CONV        ,HLP_FLCBYT_CONV        ,TRUE)
          CLETAB_OTH("FLCBYT","STATE" ,"State string"             ,asClpExtPar   ,MAN_FLCBYT_STATE       ,HLP_FLCBYT_STATE       ,FALSE)
          CLETAB_OTH("FLCBYT","LOG"   ,"Log string"               ,asClpMemoryLog,MAN_FLCBYT_LOG         ,HLP_FLCBYT_LOG         ,FALSE)
          CLEOTH_CLS
       };

      CLEDOC_OPN(asDocTab) = {
         CLETAB_DOC(CLE_DOCTYP_COVER         ,1,NULL   ,NULL               ,"FLCL manual"                       ,MAN_FLCL_COVER)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,NULL               ,"Trademarks"                        ,MAN_FLCL_TRADEMARKS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,NULL               ,"Abstract"                          ,MAN_FLCL_ABSTRACT)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,NULL               ,"Supported systems"                 ,MAN_FLCL_SUPPORTED_SYSTEMS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,NULL               ,"Use cases"                         ,MAN_FLCL_USECASES)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_PREFACE ,"Preface"                           ,MAN_FLCL_PREFACE)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"1."   ,NULL               ,"Command line parser"               ,MAN_CLE_CLEPMAIN)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.1." ,NULL               ,"Command line considerations"       ,MAN_CLE_CLEPMAIN_CONSID)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.2." ,NULL               ,"Used environment variables"        ,MAN_CLE_CLEPMAIN_USEDENV)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.3." ,NULL               ,"Environment variable mapping"      ,MAN_CLE_CLEPMAIN_ENVARMAP)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.4." ,NULL               ,"Filename mapping"                  ,MAN_CLE_CLEPMAIN_FILEMAP)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.5." ,NULL               ,"Key label name mapping"            ,MAN_CLE_CLEPMAIN_KEYLABMAP)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.6." ,NULL               ,"Special EBCDIC code page support"  ,MAN_CLE_CLEPMAIN_EBCDIC)
         CLETAB_DOC(CLE_DOCTYP_BUILTIN       ,3,"1.7." ,NULL               ,"Build-in functions"                ,MAN_CLE_BUILTIN_FUNCTIONS)
         CLETAB_DOC(CLE_DOCTYP_PROGRAM       ,2,"2."   ,NULL               ,"FLCL Utility"                      ,MAN_FLCL_MAIN)
         CLETAB_DOC(CLE_DOCTYP_PGMSYNOPSIS   ,3,"2.1." ,NULL               ,"Synopsis"                          ,MAN_CLE_PROGRAM_SYNOPSIS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.2." ,NULL               ,"Environment Variables"             ,MAN_FLCL_MAIN_ENVIRONMENT_VARIABLES)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.3." ,NULL               ,"Filename Handling"                 ,MAN_FLCL_MAIN_FILENAME_HANDLING)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.4." ,NULL               ,"Directory Support"                 ,MAN_FLCL_MAIN_DIRECTORY_SUPPORT)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.5." ,NULL               ,"Input to Output Name Mapping"      ,MAN_FLCL_MAIN_INPUT_TO_OUTPUT_NAME_MAPPING)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.6." ,NULL               ,"Key Label Handling"                ,MAN_FLCL_MAIN_KEY_LABEL_HANDLING)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.7." ,NULL               ,"Password Handling"                 ,MAN_FLCL_MAIN_PASSWORD_HANDLING)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.8." ,NULL               ,"Handling of Empty Records"         ,MAN_FLCL_MAIN_HANDLING_OF_EMPTY_RECORDS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.9." ,NULL               ,"Table Support"                     ,MAN_FLCL_MAIN_TABLE_SUPPORT)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.10.",NULL               ,"Pre- and Post-processing"          ,MAN_FLCL_MAIN_PRE_AND_POST_PROCESSING)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.11.",NULL               ,"Use of Built-in Functions"         ,MAN_FLCL_MAIN_BUILT_IN_FUNCTIONS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.12.",NULL               ,"CONV versus XCNV and ICNVs"        ,MAN_FLCL_MAIN_CONV_VERSUS_XCNV_AND_ICNV)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.13.",NULL               ,"CONV READ/WRITE overview"          ,MAN_FLCL_MAIN_CONV_READ_WRITE_OVERVIEW)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.14.",NULL               ,"JCL Considerations"                ,MAN_FLCL_MAIN_JCL_CONSIDERATIONS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.15.",NULL               ,"SAF Consideration"                 ,MAN_FLCL_MAIN_SAF_CONSIDERATIONS)
         CLETAB_DOC(CLE_DOCTYP_PGMSYNTAX     ,3,"2.16.",NULL               ,"Syntax of FLCL"                    ,MAN_CLE_PROGRAM_SYNTAX)
         CLETAB_DOC(CLE_DOCTYP_PGMHELP       ,3,"2.17.",NULL               ,"Help for FLCL"                     ,MAN_CLE_PROGRAM_HELP)
         CLETAB_DOC(CLE_DOCTYP_COMMANDS      ,3,"2.18.",NULL               ,"Available FLCL commands"           ,MAN_CLE_PROGRAM_COMMANDS)
         CLETAB_DOC(CLE_DOCTYP_OTHERCLP      ,2,NULL   ,CLE_DOCKYW_APPENDIX,"Other CLP strings"                 ,MAN_FLCL_OTHERCLP)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_APPENDIX,"FLUC Filesystem for Linux"         ,MAN_FLCL_APPENDIX_FLUCFS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_APPENDIX,"FLUC Subsystem for z/OS"           ,MAN_FLCL_APPENDIX_FLUCSUB)
         CLETAB_DOC(CLE_DOCTYP_LEXEM         ,2,NULL   ,CLE_DOCKYW_APPENDIX,"Lexem"                             ,MAN_CLE_APPENDIX_LEXEM)
         CLETAB_DOC(CLE_DOCTYP_GRAMMAR       ,2,NULL   ,CLE_DOCKYW_APPENDIX,"Grammar"                           ,MAN_CLE_APPENDIX_GRAMMAR)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_APPENDIX,"Properties"                        ,MAN_CLE_APPENDIX_PROPERTIES)
         CLETAB_DOC(CLE_DOCTYP_PROPREMAIN    ,3,NULL   ,NULL               ,"Remaining documentation"           ,MAN_CLE_APPENDIX_PROP_REMAINING)
         CLETAB_DOC(CLE_DOCTYP_PROPDEFAULTS  ,3,NULL   ,NULL               ,"Predefined defaults"               ,MAN_CLE_APPENDIX_PROP_DEFAULTS)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_APPENDIX,"Returncodes"                       ,MAN_CLE_APPENDIX_RETURNCODES)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL   ,NULL               ,"Special condition codes"           ,MAN_FLCL_SPECIALCODES)
         CLETAB_DOC(CLE_DOCTYP_REASONCODES   ,3,NULL   ,NULL               ,"Reasoncodes"                       ,MAN_CLE_APPENDIX_REASONCODES)
         CLETAB_DOC(CLE_DOCTYP_VERSION       ,2,NULL   ,CLE_DOCKYW_APPENDIX,"Version"                           ,MAN_CLE_APPENDIX_VERSION)
         CLETAB_DOC(CLE_DOCTYP_ABOUT         ,2,NULL   ,CLE_DOCKYW_APPENDIX,"About"                             ,MAN_CLE_APPENDIX_ABOUT)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_GLOSSARY,"Glossary"                          ,MAN_FLCL_GLOSSARY)
         CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL   ,CLE_DOCKYW_COLOPHON,"Imprint"                           ,MAN_FLCL_IMPRINT)
         CLEDOC_CLS
      };

      siErr=siCleExecute(psGbl,asCmdTab,argc,argv,FLM_CLEP_DEFAULT_OWNER,"flcl",
               FLM_CLEP_CASE_SENSITIVITY,TRUE,TRUE,FALSE,FLM_CLEP_MINIMAL_KEYWORDLEN,
               pfErr,pfTrc,FLM_CLEP_DEPTH_STRING_1047,FLM_CLEP_OPTION_STRING,
               FLM_CLEP_SEPARATION_STRING,psMain->acLicTxt,
               FLM_VSN_STR"-"__BUILDNRSTR__,psMain->acVersion,psMain->acAbout,
               "Frankenstein Limes(R) Command Line for FLUC, FLAM and FLIES",
               NULL,pcFlmErrors,asOthTab,NULL,siClpFile2String,
               NULL,(psGbl->isSafClpControl)?siClpSaf:NULL,pcDpa,0,asDocTab);

    }
@endcode

Lexemes
-------

Lexemes (regular expressions) for argument list or parameter file:

    --| COMMENT   '#' [:print:]* '#'                              (will be ignored)
    --| LCOMMENT  ';' [:print:]* 'nl'                             (will be ignored)
    --| SEPARATOR [:space: | :cntr: | ',']*                  (abbreviated with SEP)
    --| OPERATOR  '=' | '.' | '(' | ')' | '[' | ']'  (SGN, DOT, RBO, RBC, SBO, SBC)
    --| KEYWORD   [:alpha:]+[:alnum: | '_']*                    (always predefined)
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
    --| SUPPLEMENT     '"' [:print:]* '"' |   (null-terminated string (properties))

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


Compilation concerns
--------------------

For compilation the defines below must be set:

    __DEBUG__        for a debug build
    __RELEASE__      for a release build
    __WIN__          for WINDOWS platforms
    __ZOS__          for ZOS mainframe platforms
    __USS__          for unix system services (USS) on ZOS mainframe platforms
    __BUILDNR__      to define the build number (integer, default is 0)
    __BUILD__        to define the build string ("debug", "release", "alpha", ...)
    __HOSTSHORTING__ to short function names to 8 character for mainframes

On z/OS or USS the CELP and the using project must be compiled with the
same CONVLIT() parameter (we recommend IBM-1047) for correct conversion
of the literals. Don't use the literal replacements (S_xxx or C_xxx or
the exprintf() functions) in front of the CleExecute call, to ensure the
environment of the configuration file is used for character conversion.

Few of the strings provided to the CleExecute function are converted for
EBCDIC system. These conversions are done, based on stack allocations.
We expect string literals with a limited length for these values. Please
be aware of the security issues if you provide variable length strings
in this case.

***********************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLE_H
#define INC_CLE_H

#include "FLAMCLP.h"

#ifdef __HOSTSHORTING__
#  define pcCleVersion           FLCLEVSN
#  define pcCleAbout             FLCLEABO
#  define siCleExecute           FLCLEEXE
#endif

/**
* Get version information
*
* The function returns the version information for this library
*
* @param l level of visible hierarchy in the first 2 numbers of the string
*          the number can later be used to better visualize the hierarchy
* @param s size of the provided string buffer (including space for null termination)
* @param b buffer for the version string
*          must contain a null-terminated string
*          the version string will be concatenated
*          the size including the 0-byte is the limit
*          if (strlen(b)==s-1) then more space is required for the complete version string
*          a good size for the version string is 256 byte
*
* @return pointer to a null-terminated version string (return(b))
*/
extern const char* pcCleVersion(const int l, const int s, char* b);

/**
* Get about information
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
* Documentation types used in table below
*/
#define CLE_DOCTYP_COVER               1U    /** Cover page (level must be 1)*/
#define CLE_DOCTYP_CHAPTER             2U    /** A chapter (level must > 1 and < 6)*/
#define CLE_DOCTYP_PROGRAM             10U    /** The main program chapter (like chapter but level must < 5)*/
#define CLE_DOCTYP_PGMSYNOPSIS         11U    /** The program synopsis*/
#define CLE_DOCTYP_PGMSYNTAX           12U    /** The program syntax*/
#define CLE_DOCTYP_PGMHELP             13U    /** The program help*/
#define CLE_DOCTYP_COMMANDS            20U   /** The commands part*/
#define CLE_DOCTYP_OTHERCLP            21U   /** Other CLP strings*/
#define CLE_DOCTYP_BUILTIN             22U   /** The built-in function section*/
#define CLE_DOCTYP_LEXEM               30U   /** The appendix which prints the lexems*/
#define CLE_DOCTYP_GRAMMAR             31U   /** The appendix which prints the grammar*/
#define CLE_DOCTYP_VERSION             32U   /** The appendix which prints the version (pcVsn must be given)*/
#define CLE_DOCTYP_ABOUT               33U   /** The appendix which prints the about (pcAbo must be given)*/
#define CLE_DOCTYP_PROPREMAIN          41U   /** The appendix which prints the remaining parameter documentation*/
#define CLE_DOCTYP_PROPDEFAULTS        42U   /** The appendix which prints the default parameter documentation*/
#define CLE_DOCTYP_SPECIALCODES        51U   /** The appendix which prints the special condition codes*/
#define CLE_DOCTYP_REASONCODES         52U   /** The appendix which prints the reason codes (pfMsg must be provided)*/

/**
 * @brief Map documentation type
 *
 * The function can be used to map the documentation type into a string constant.
 *
 * @param uiTyp Documentation type
 * @return      String with the nam of the documentation type
 */
static inline const char* pcMapDocTyp(const unsigned int uiTyp) {
   switch(uiTyp) {
      case CLE_DOCTYP_COVER:        return("CLE_DOCTYP_COVER");
      case CLE_DOCTYP_CHAPTER:      return("CLE_DOCTYP_CHAPTER");
      case CLE_DOCTYP_BUILTIN:      return("CLE_DOCTYP_BUILTIN");
      case CLE_DOCTYP_PROGRAM:      return("CLE_DOCTYP_PROGRAM");
      case CLE_DOCTYP_PGMSYNOPSIS:  return("CLE_DOCTYP_PGMSYNOPSIS");
      case CLE_DOCTYP_PGMSYNTAX:    return("CLE_DOCTYP_PGMSYNTAX");
      case CLE_DOCTYP_PGMHELP:      return("CLE_DOCTYP_PGMHELP");
      case CLE_DOCTYP_COMMANDS:     return("CLE_DOCTYP_COMMANDS");
      case CLE_DOCTYP_OTHERCLP:     return("CLE_DOCTYP_OTHERCLP");
      case CLE_DOCTYP_LEXEM:        return("CLE_DOCTYP_LEXEM");
      case CLE_DOCTYP_GRAMMAR:      return("CLE_DOCTYP_GRAMMAR");
      case CLE_DOCTYP_PROPREMAIN:   return("CLE_DOCTYP_PROPREMAIN");
      case CLE_DOCTYP_PROPDEFAULTS: return("CLE_DOCTYP_PROPDEFAULTS");
      case CLE_DOCTYP_SPECIALCODES: return("CLE_DOCTYP_SPECIALCODES");
      case CLE_DOCTYP_REASONCODES:  return("CLE_DOCTYP_REASONCODES");
      case CLE_DOCTYP_VERSION:      return("CLE_DOCTYP_VERSION");
      case CLE_DOCTYP_ABOUT:        return("CLE_DOCTYP_ABOUT");
      default:                      return("UNKOWN");
   }
}

/**
* ASCIIDOC key words used in table below
*/
#define CLE_DOCKYW_PREFACE             "preface"  /** Mark level 2 chapter as preface*/
#define CLE_DOCKYW_APPENDIX            "appendix" /** Mark level 2 chapter as appendix*/
#define CLE_DOCKYW_GLOSSARY            "glossary" /** Mark level 2 chapter as glossary*/
#define CLE_DOCKYW_COLOPHON            "colophon" /** Mark level 2 chapter as colophon*/

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
 * \b pcKyW  ASCIIDOC key word (printed in front of headline in square brackets)\n
 * \b pcHdl  Headline for this chapter\n
 * \b pcMan  Optional or required manual page with the content of this chapter)\n
 */
typedef struct CleDoc {
   unsigned int      uiTyp;
   unsigned int      uiLev;
   const char*       pcNum;
   const char*       pcKyw;
   const char*       pcHdl;
   const char*       pcMan;
}TsCleDoc;

/** Starts the documentation generation table
 *  *name* Name of this table\n
 */
#define CLEDOC_OPN(name)               TsCleDoc name[]

/** Starts the documentation generation table
 *  *typ* Documentation type\n
 *  *lev* The level of the chapter\n
 *  *num* Prefix for number string\n
 *  *hdl* ASCIIDOC key word (printed in front of headline in square brackets)\n
 *  *hdl* Headline for this chapter\n
 *  *man* Manual page for this chapter\n
 */
#define CLETAB_DOC(typ,lev,num,kyw,hdl,man)   {(typ),(lev),(num),(kyw),(hdl),(man)},

/** Ends a table with constant definitions
 */
#define CLEDOC_CLS                    {  0  ,  0  , NULL, NULL, NULL, NULL}

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

/**
 * \struct TsCleOtherClp
 *
 * This structure is used to define a table of CLP strings add as appendix to the generated documentation\n
 *
 * Its members are:
 * \par
 * \b pcRot  Pointer to the root key word for this string (:alpha:[:alnum:|'_']*)\n
 * \b pcKyw  Pointer to the key word for this string (:alpha:[:alnum:|'_']*)\n
 * \b pcHdl  Pointer to the headline for the sub chapter for this CLP string (:alpha:[:alnum:|'_']*)\n
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
   const char*                   pcHdl;
   const TsClpArgument*          psTab;
   const char*                   pcMan;
   const char*                   pcHlp;
   const int                     isOvl;
} TsCleOtherClp;

/** Starts a table with command definitions
 *
 *  *nam* Name of this table\n
 */
#define CLETAB_OPN(nam)          TsCleCommand nam[]

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

/** Starts a table with other CLP strings
 *
 *  *nam* Name of this table\n
 */
#define CLEOTH_OPN(nam)               TsCleOtherClp nam[]

/** defines a appendix for the object or overlay *kyw* of the root *rot* with the headline of *hdl* for a certain other CLP string
 *
 *  *tab* Pointer to the main table for this CLP string.\n
 *  *man* Pointer to a null-terminated string for a detailed description for this CLP string.
 *        (in ASCIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs plus .OPTIONS and/or .EXAMPLES)
 *        It is recommended to use a header file with a define for this long string)\n
 *  *hlp* String for a short context sensitive help for this CLP string\n
 */
#define CLETAB_OTH(rot,kyw,hdl,tab,man,hlp,ovl)   {(rot),(kyw),(hdl),(tab),(man),(hlp),(ovl)},

/** Ends a table with other CLP strings
 */
#define CLEOTH_CLS                    { NULL, NULL, NULL, NULL, NULL, NULL, 0}

/**
 * Execute command line
 *
 * The function uses the command line parsers to execute different commands based on argc and argv given in the main function of a program
 * and provides the additional built-in functions below:
 * - SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]
 * - HELP   [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]
 * - MANPAGE [function | command[.path][=filename]] | [filename]
 * - GENDOCU [command[.path]=]filename [NONBR] [SHORT]
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
 * @param[in]  psTab Pointer to the table which defines the commands
 * @param[in]  argc  Number of command line parameters (argc of main(int argc, char* argv[]))
 * @param[in]  argv  List of pointers to the command line parameters (argv of main(int argc, char* argv[]))
 * @param[in]  pcOwn Default owner id (owner ids are used to identify properties and other things "com.company")
 * @param[in]  pcPgm Logical program name (can be different from argv[0] and will be used in the root "com.company.program")
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
 * @param[in]  pcBld String containing the build number / raw version for this program (optional, can be NULL) used in final message about completion - converted on EBCDIC systems
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
 * @param[in]  psDoc Table for flexible documentation generation (NULL then no manual page documentation is possible)
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
   const TsCleCommand*           psTab,
   int                           argc,
   char*                         argv[],
   const char*                   pcOwn,
   const char*                   pcPgm,
   const int                     isCas,
   const int                     isPfl,
   const int                     isRpl,
   const int                     isEnv,
   const int                     siMkl,
   FILE*                         pfOut,
   FILE*                         pfTrc,
   const char*                   pcDep,
   const char*                   pcOpt,
   const char*                   pcEnt,
   const char*                   pcLic,
   const char*                   pcBld,
   const char*                   pcVsn,
   const char*                   pcAbo,
   const char*                   pcHlp,
   const char*                   pcDef,
   TfMsg*                        pfMsg,
   const TsCleOtherClp*          psOth,
   void*                         pvF2S,
   TfF2S*                        pfF2S,
   void*                         pvSaf,
   TfSaf*                        pfSaf,
   const char*                   pcDpa,
   const int                     siNoR,
   const TsCleDoc*               psDoc);

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
    const char*                   pcCmd,
    const char*                   pcMan,
    const char*                   pcHlp,
    const int                     isOvl,
    const char*                   pcStr,
    const TsClpArgument*          psTab,
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

extern int siClePrintCover(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isNbr);
extern int siClePrintChapter(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isNbr);
extern int siClePrintPgmSynopsis(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcHlp, const int isNbr);
extern int siClePrintPgmSyntax(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, const char* pcOwn, const char* pcPgm, const char* pcDep, const char* pcOpt, const char* pcDpa, const int isNbr);
extern int siClePrintPgmHelp(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, const char* pcOwn, const char* pcPgm, const char* pcDep, const int isNbr);
extern int siClePrintBuiltIn(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isNbr);
extern int siClePrintLexem(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isPfl, const int isRpl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr);
extern int siClePrintGrammar(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const int isPfl, const int isRpl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr);
extern int siClePrintPreformatedText(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, const char* pcTxt, const int isNbr);
extern int siClePrintPropRemain(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, void* pvCnf, const char* pcOwn, const char* pcPgm,
      const int isCas, const int isPfl, const int isRpl, const int siMkl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr);
extern int siClePrintPropDefaults(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleCommand* psTab, void* pvCnf, const char* pcOwn, const char* pcPgm,
      const int isCas, const int isPfl, const int isRpl, const int siMkl, const char* pcDep, const char* pcOpt, const char* pcEnt, const int isNbr);
extern int siClePrintReasonCodes(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const char* pcOwn, const char* pcPgm, TfMsg* pfMsg, const int isNbr);

typedef struct CleDocPar {
   const char*       pcOwn;
   const char*       pcPgm;
   const char*       pcHlp;
   const char*       pcDep;
   const char*       pcOpt;
   const char*       pcEnt;
   const char*       pcDpa;
   void*             pvCnf;
   int               isPfl;
   int               isRpl;
   int               isCas;
   int               siMkl;
   TfMsg*            pfMsg;
   const char*       pcVsn;
   const char*       pcAbo;
   int               isNbr;
}TsCleDocPar;

typedef int (TfClePrintPage)(FILE* pfErr, FILE* pfDoc, const TsCleDoc* psDoc, const TsCleDocPar* psPar, const TsCleCommand* psCmd);
typedef int (TfCleHtmlDoc)(FILE* pfOut, FILE* pfErr, const char* pcPat, const TsCleDoc* psDoc, const TsCleCommand* psCmd, const TsCleOtherClp* psOth , const TsCleDocPar* psPar,TfClePrintPage* pfDoc);

/*! @endcond */

#endif /*INC_CLE_H*/

#ifdef __cplusplus
}
#endif
