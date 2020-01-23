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
 * * HTMLDOC  - Generates documentation using a call pack interface for each page
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
          CLETAB_OTH("FLCBYT","RFMT"  ,"Read format string"       ,asClpWrtFmtPar,MAN_FLCBYT_READ_FORMAT ,HLP_FLCBYT_READ_FORMAT ,TRUE)
          CLETAB_OTH("FLCBYT","WFMT"  ,"Write format string"      ,asClpRedFmtPar,MAN_FLCBYT_WRITE_FORMAT,HLP_FLCBYT_WRITE_FORMAT,TRUE)
          CLETAB_OTH("FLCBYT","READ"  ,"Read conversion string"   ,asClpElmCnvRed,MAN_FLCBYT_CONV_READ   ,HLP_FLCBYT_CONV_READ   ,TRUE)
          CLETAB_OTH("FLCBYT","WRITE" ,"Write conversion string"  ,asClpElmCnvWrt,MAN_FLCBYT_CONV_WRITE  ,HLP_FLCBYT_CONV_WRITE  ,TRUE)
          CLETAB_OTH("FLCBYT","CONV"  ,"From-To conversion string",asClpElmCnv   ,MAN_FLCBYT_CONV        ,HLP_FLCBYT_CONV        ,TRUE)
          CLETAB_OTH("FLCBYT","STATE" ,"State string"             ,asClpExtPar   ,MAN_FLCBYT_STATE       ,HLP_FLCBYT_STATE       ,FALSE)
          CLETAB_OTH("FLCBYT","LOG"   ,"Log string"               ,asClpMemoryLog,MAN_FLCBYT_LOG         ,HLP_FLCBYT_LOG         ,FALSE)
          CLEOTH_CLS
       };

   CLEDOC_OPN(asDocTab) = {
      CLETAB_DOC(CLE_DOCTYP_COVER         ,1,NULL     ,NULL               ,NULL                             ,"FLCL manual"                       ,MAN_FLCL_COVER                            ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,NULL               ,NULL                             ,"Trademarks"                        ,MAN_FLCL_TRADEMARKS                       ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,NULL               ,NULL                             ,"Abstract"                          ,MAN_FLCL_ABSTRACT                         ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,NULL               ,NULL                             ,"Supported systems"                 ,MAN_FLCL_SUPPORTED_SYSTEMS                ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,NULL               ,NULL                             ,"Use cases"                         ,MAN_FLCL_USECASES                         ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_PREFACE ,NULL                             ,"Preface"                           ,MAN_FLCL_PREFACE                          ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"1."     ,NULL               ,"clep.main"                      ,"Command line parser"               ,MAN_CLE_CLEPMAIN                          ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.1."   ,NULL               ,NULL                             ,"Command line considerations"       ,MAN_CLE_CLEPMAIN_CONSID                   ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.2."   ,NULL               ,"clep.main.usedenv"              ,"Used environment variables"        ,MAN_CLE_CLEPMAIN_USEDENV                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.3."   ,NULL               ,NULL                             ,"Environment variable mapping"      ,MAN_CLE_CLEPMAIN_ENVARMAP                 ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.4."   ,NULL               ,"clep.main.filemap"              ,"Filename mapping"                  ,MAN_CLE_CLEPMAIN_FILEMAP                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.5."   ,NULL               ,NULL                             ,"Key label name mapping"            ,MAN_CLE_CLEPMAIN_KEYLABMAP                ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.6."   ,NULL               ,"clep.main.ebcdic"               ,"Special EBCDIC code page support"  ,MAN_CLE_CLEPMAIN_EBCDIC                   ,NULL)
      CLETAB_DOC(CLE_DOCTYP_BUILTIN       ,3,"1.7."   ,NULL               ,CLE_ANCHOR_BUILTIN_FUNCTIONS     ,"Built-in functions"                ,MAN_CLE_BUILTIN_FUNCTIONS                 ,NULL)
      CLETAB_DOC(CLE_DOCTYP_PROGRAM       ,2,"2."     ,NULL               ,NULL                             ,"FLCL Utility"                      ,MAN_FLCL_MAIN                             ,NULL)
      CLETAB_DOC(CLE_DOCTYP_PGMSYNOPSIS   ,3,"2.1."   ,NULL               ,NULL                             ,"Synopsis"                          ,MAN_CLE_PROGRAM_SYNOPSIS                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.2."   ,NULL               ,NULL                             ,"Environment Variables"             ,MAN_FLCL_MAIN_ENVIRONMENT_VARIABLES       ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.3."   ,NULL               ,NULL                             ,"Filename Handling"                 ,MAN_FLCL_MAIN_FILENAME_HANDLING           ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.4."   ,NULL               ,NULL                             ,"Directory Support"                 ,MAN_FLCL_MAIN_DIRECTORY_SUPPORT           ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.5."   ,NULL               ,NULL                             ,"Input to Output Name Mapping"      ,MAN_FLCL_MAIN_INPUT_TO_OUTPUT_NAME_MAPPING,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.6."   ,NULL               ,NULL                             ,"Key Label Handling"                ,MAN_FLCL_MAIN_KEY_LABEL_HANDLING          ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.7."   ,NULL               ,NULL                             ,"Password Handling"                 ,MAN_FLCL_MAIN_PASSWORD_HANDLING           ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.8."   ,NULL               ,NULL                             ,"Handling of Empty Records"         ,MAN_FLCL_MAIN_HANDLING_OF_EMPTY_RECORDS   ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.9."   ,NULL               ,NULL                             ,"Table Support"                     ,MAN_FLCL_MAIN_TABLE_SUPPORT               ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.10."  ,NULL               ,NULL                             ,"Pre- and Post-processing"          ,MAN_FLCL_MAIN_PRE_AND_POST_PROCESSING     ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.11."  ,NULL               ,NULL                             ,"Use of Built-in Functions"         ,MAN_FLCL_MAIN_BUILT_IN_FUNCTIONS          ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.12."  ,NULL               ,NULL                             ,"CONV versus XCNV and ICNVs"        ,MAN_FLCL_MAIN_CONV_VERSUS_XCNV_AND_ICNV   ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.13."  ,NULL               ,NULL                             ,"CONV READ/WRITE overview"          ,MAN_FLCL_MAIN_CONV_READ_WRITE_OVERVIEW    ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.14."  ,NULL               ,NULL                             ,"JCL Considerations"                ,MAN_FLCL_MAIN_JCL_CONSIDERATIONS          ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.15."  ,NULL               ,NULL                             ,"SAF Consideration"                 ,MAN_FLCL_MAIN_SAF_CONSIDERATIONS          ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"2.16."  ,NULL               ,NULL                             ,"Installation"                      ,MAN_FLCL_MAIN_INSTALLATION                ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,4,"2.16.1.",NULL               ,NULL                             ,"License"                           ,MAN_FLCL_MAIN_INSTALLATION_LICENSE        ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,4,"2.16.2.",NULL               ,NULL                             ,"Download"                          ,MAN_FLCL_MAIN_INSTALLATION_DOWNLOAD       ,NULL)
      CLETAB_DOC(CLE_DOCTYP_PGMSYNTAX     ,3,"2.17."  ,NULL               ,NULL                             ,"Syntax of FLCL"                    ,MAN_CLE_PROGRAM_SYNTAX                    ,NULL)
      CLETAB_DOC(CLE_DOCTYP_PGMHELP       ,3,"2.18."  ,NULL               ,NULL                             ,"Help for FLCL"                     ,MAN_CLE_PROGRAM_HELP                      ,NULL)
      CLETAB_DOC(CLE_DOCTYP_COMMANDS      ,3,"2.19."  ,NULL               ,NULL                             ,"Available FLCL commands"           ,MAN_CLE_PROGRAM_COMMANDS                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_OTHERCLP      ,2,NULL     ,CLE_DOCKYW_APPENDIX,NULL                             ,"Other CLP strings"                 ,MAN_FLCL_OTHERCLP                         ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_APPENDIX,NULL                             ,"FLUC Filesystem for Linux"         ,MAN_FLCL_APPENDIX_FLUCFS                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_APPENDIX,NULL                             ,"FLUC Subsystem for z/OS"           ,MAN_FLCL_APPENDIX_FLUCSUB                 ,NULL)
      CLETAB_DOC(CLE_DOCTYP_LEXEM         ,2,NULL     ,CLE_DOCKYW_APPENDIX,CLE_ANCHOR_APPENDIX_LEXEM        ,"Lexemes"                           ,MAN_CLE_APPENDIX_LEXEM                    ,NULL)
      CLETAB_DOC(CLE_DOCTYP_GRAMMAR       ,2,NULL     ,CLE_DOCKYW_APPENDIX,CLE_ANCHOR_APPENDIX_GRAMMAR      ,"Grammar"                           ,MAN_CLE_APPENDIX_GRAMMAR                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_APPENDIX,CLE_ANCHOR_APPENDIX_PROPERTIES   ,"Properties"                        ,MAN_CLE_APPENDIX_PROPERTIES               ,NULL)
      CLETAB_DOC(CLE_DOCTYP_PROPREMAIN    ,3,NULL     ,NULL               ,NULL                             ,"Remaining documentation"           ,MAN_CLE_APPENDIX_PROP_REMAINING           ,NULL)
      CLETAB_DOC(CLE_DOCTYP_PROPDEFAULTS  ,3,NULL     ,NULL               ,NULL                             ,"Predefined defaults"               ,MAN_CLE_APPENDIX_PROP_DEFAULTS            ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_APPENDIX,CLE_ANCHOR_APPENDIX_RETURNCODES  ,"Return Codes"                      ,MAN_CLE_APPENDIX_RETURNCODES              ,NULL)
      CLETAB_DOC(CLE_DOCTYP_SPECIALCODES  ,3,NULL     ,NULL               ,NULL                             ,"Special condition codes"           ,MAN_FLCL_SPECIALCODES                     ,NULL)
      CLETAB_DOC(CLE_DOCTYP_REASONCODES   ,3,NULL     ,NULL               ,CLE_ANCHOR_APPENDIX_REASONCODES  ,"Reason Codes"                      ,MAN_CLE_APPENDIX_REASONCODES              ,NULL)
      CLETAB_DOC(CLE_DOCTYP_VERSION       ,2,NULL     ,CLE_DOCKYW_APPENDIX,CLE_ANCHOR_APPENDIX_VERSION      ,"Version"                           ,MAN_CLE_APPENDIX_VERSION                  ,NULL)
      CLETAB_DOC(CLE_DOCTYP_ABOUT         ,2,NULL     ,CLE_DOCKYW_APPENDIX,CLE_ANCHOR_APPENDIX_ABOUT        ,"About"                             ,MAN_CLE_APPENDIX_ABOUT                    ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_GLOSSARY,NULL                             ,"Glossary"                          ,MAN_FLCL_GLOSSARY                         ,NULL)
      CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL     ,CLE_DOCKYW_COLOPHON,NULL                             ,"Imprint"                           ,MAN_FLCL_IMPRINT                          ,NULL)
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

#ifndef INC_FLAMCLE_H
#define INC_FLAMCLE_H

#include "CLEDEF.h"

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
 * @brief Map documentation type
 *
 * The function can be used to map the documentation type into a string constant.
 *
 * @param uiTyp Documentation type
 * @return      String with the nam of the documentation type
 */

/**
 * Execute command line
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

/*! @endcond */
/**********************************************************************/

#endif /*INC_FLAMCLE_H*/

#ifdef __cplusplus
}
#endif
