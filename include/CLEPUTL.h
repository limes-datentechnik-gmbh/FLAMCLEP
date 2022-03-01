/**
 * @file CLEPUTL.h
 * @brief  Declaration of utility functions for CLP/CLE in ANSI C
 * @author limes datentechnik gmbh
 * @date   06.03.2015
 * @copyright limes datentechnik gmbh
 *
 * @mainpage LIMES Command Line Executor and Processor (FLAMCLEP)
 *
 * @section CLEPLIC License
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
 * @section MAIN Overview
 *
 * We developed CLE/P because we didn't find an existing library meeting our
 * requirements for a consistent command line over all platforms. We decided
 * therefore to build such a compiler which is table-controlled and which
 * transforms property files and command line inputs into a machine-processable
 * data structure of arbitrary nesting depth.
 *
 * The result is a very powerful tool briefly described in the following. The
 * full interface documentation, programming reference, the GIT-repository, and
 * the license can be downloaded from GITHUB.
 *
 * We are posting CLP/E as OpenSource on the terms of a Zlib-based license above,
 * making it freely available to everyone in the form of a GIT project.
 *
 * With the command line executor (CLE), you can simply realize a complex command
 * line through the definition of some tables. No programming is required to get
 * the values parsed and stored in a free defined data structure. For this, a compiler
 * with its own language was implemented to provide the same command line interface
 * on each available platform. This command line parser (CLP) is used by the CLE.
 * Both components provide extensive help for each command and many other support
 * functions. For a provided list of commands, the CLE uses a dedicated command line
 * processor (CLP) to make all these commands available, together with several
 * built-in functions.
 *
 * To achieve this, a table must be defined where each row describes one command.
 * This table provides the input for the execution function doing the command line
 * interpretation. The whole library consists of only one function and a structure
 * to define the command table.
 *
 * Beside the specified user defined commands, the CLE provides several powerful
 * built-in functions. All built-in functions contain a manual page to get more
 * information at runtime.
 *
 * Based on the keyword, the short help message and the detailed description, the
 * built-in function GENDOCU or HTMLDOC can be used to generate a complete user
 * manual. Based on this capability, the CLE/P completely describes itself.
 *
 * The self explanation of the whole program was one of the main targets of this
 * general command line interface. To understand this interface specification it
 * is advisable to read the CLE/P documentation.
 *
 * **********************************************************************
 * @section COMPILE Compilation concerns
 *
 * For compilation the defines below must be set:
 *
 * @verbatim
   __DEBUG__        for a debug build
   __RELEASE__      for a release build
   __WIN__          for WINDOWS platforms
   __ZOS__          for ZOS mainframe platforms
   __USS__          for unix system services (USS) on ZOS mainframe platforms
   __BUILDNR__      to define the build number (integer, default is 0)
   __BUILD__        to define the build string ("debug", "release", "alpha", ...)
   __HOSTSHORTING__ to short function names to 8 character for mainframes
   @endverbatim
 *
 * On z/OS or USS the CELP and the using project must be compiled with the
 * same CONVLIT() parameter (we recommend IBM-1047) for correct conversion
 * of the literals. Don't use the literal replacements (S_xxx or C_xxx or
 * the exprintf() functions) in front of the CleExecute call (see siCleExecute())
 * to ensure the environment of the configuration file is used for character conversion.
 *
 * Few of the strings provided to the CleExecute function are converted for
 * EBCDIC system. These conversions are done, based on stack allocations.
 * We expect string literals with a limited length for these values. Please
 * be aware of the security issues if you provide variable length strings
 * in this case.
 *
 * @section CLEMAIN Command Line Executor (FLAMCLE)
 *
 * With the command line executor (FLAMCLE) the user can simply realize a complex
 * command line using command and keyword definitions contained in some tables.
 *
 * Command Line Executor (FLAMCLE) uses the Command Line Parser (FLAMCLP) to
 * provide the selected commands and keywords on each used platform.
 * To achieve this, a table is defined where each row describes one
 * command or keyword. This table provides the input for the execution function
 * doing the command line interpretation. The whole library consists of only one
 * function and a structure to define the command table. One of these
 * commands or a built-in function can be defined as default, which will be
 * executed if the first keyword (argv[1]) don't fit one of the user-
 * defined commands or built-in functions. If no command or built-in
 * function is defined and no default set the built-in function syntax will be
 * executed to show the capabilities of the command line program.
 *
 * Beside the specified user-defined commands, the FLAMCLE provides several
 * powerful built-in functions (listed below). All built-in functions have
 * a manual page, implemented to display more information at runtime.
 *
 * With the built-in function help a short help message or the detailed
 * description can determined for each parameter and command using a dotted path.
 * The built-in function GENDOCU can be used to generate a part or the complete
 * user manual. Based on this capability the FLAMCLE completely describes itself.
 *
 * The self-documenting style of the whole program was one of the main
 * targets of this general command line interface. To understand the
 * interface specification, it is recommended to read the FLAMCLE
 * documentation.
 *
 * @subsection CLEFEATURES FLAMCLE-Features
 *
 * Below, you can find a possibly incomplete list of FLAMCLE feature:
 *
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
 * * File name mapping and DD:NAME support (see <<CLEP.MAIN,CLEP>>)
 * * Return/condition/exit code and reason code handling
 * * On EBCDIC systems we use a code page specific interpretation of punctuation characters <b><span style="color:#000000">(!$#@[\]^`{|}~)</span></b> dependent on the environment variable LANG
 * * Extensive manual page management including replacement of owner (&{OWN}) and program name (&{PGM})
 * * Own tool to generate description strings from text files including replacement of constant definitions (${__VERSION__})
 * * Definition of maximum and minimum condition code (MAXCC) for command execution
 * * Support SILENT and QUITE to control outputs
 * * Special condition code handling (incl. description for manual and built-in function ERRORS)
 * * Strings can be read from files to increase protection and prevent logging of passwords
 * * Default parameter file name for system supporting static file allocation ("DD:MYPAR")
 * * You can exclude the run after mapping if you provide the corresponding return code (siNoR)
 * * Own file to string callback function for parameter files
 *
 * @subsection CLEBUILTINFUNC FLAMCLE-Built-in Functions
 *
 * All these built-in functions are available:
 *
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
 * * SETENV   - Set an environment variable (defined in the config file)
 * * GETENV   - Show the environment variables (defined in the config file)
 * * DELENV   - Delete an environment variable (defined in the config file)
 * * LSTENV   - List status of all possible usable environment variables
 * * HLPENV   - List help message for all possible usable environment variables
 * * TRACE    - Manages trace capabilities
 * * CONFIG   - Displays or clears the current configuration settings
 * * GRAMMAR  - Displays the grammar for commands and properties
 * * LEXEM    - Displays the regular expressions accepted in a command
 * * LICENSE  - Displays the license text for the program
 * * VERSION  - Lists version information for the program
 * * ABOUT    - Displays information about the program
 * * ERRORS   - Displays information about return and reason codes of the program
 *
 * To read the manual page, please use:
 * @verbatim
   program MANPAGE function
   @endverbatim
 *
 * Below, you can find the syntax for each built-in function:
 *
 * * SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]
 * * HELP [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]
 * * MANPAGE [function | command[.path][=filename]] | [filename]
 * * GENDOCU [command[.paht]=]filename [NONBR] [SHORT]
 * * GENPROP [command=]filename
 * * SETPROP [command=]filename
 * * CHGPROP command [path[=value]]
 * * DELPROP [command]
 * * GETPROP [command[.path] [DEPTH1 | ... | DEPTH9 | DEPALL | DEFALL]]
 * * SETOWNER name
 * * GETOWNER
 * * SETENV variable=value
 * * GETENV
 * * DELENV variable
 * * LSTENV
 * * HLPENV
 * * TRACE ON | OFF | FILE=filename
 * * CONFIG [CLEAR]
 * * GRAMMAR
 * * LICENSE
 * * LEXEM
 * * VERSION
 * * ABOUT
 * * ERRORS
 *
 * @subsection CLESAMPLEPROG FLAMCLE-Sample program
 *
 * This sample program is the main of our FLCL command line utility.
 * This code is not functional and results in compile errors because
 * of missing other parts of the FL5 project, but it visible the
 * principles of CLE usage.
 *
 * @code
 * #include "CLEPUTL.h"
 * #include "FLAMCLE.h"
 * #include "CLEMAN.h"
 *
 * #define DEFINE_STRUCT
 * #include "CLPMAC.h"
 * #include "FL5TAB.h"
 * #include "FL5STC.h"
 *
 * int main(const int argc, const char * argv[])
 * {
 *    static TsFlcConvPar     stFlcConvPar;
 *    static TsClpConvPar     stClpXcnvPar;
 *    static TsClpConvPar     stClpConvPar;
 *    static TsClpIcnvPar     stClpIcnvPar;
 *    static TsFlcInfoPar     stFlcInfoPar;
 *    static TsClpInfoPar     stClpInfoPar;
 *
 *    #undef DEFINE_STRUCT
 *    #include "CLPMAC.h"
 *    #include "FL5CON.h"
 *    #include "FL5ARG.h"
 *
 *    CLECMD_OPN(asCmdTab) = {
 *       CLETAB_CMD("CONV",asClpConvPar,&stClpConvPar,&stFlcConvPar,NULL,siIniConv,siMapConv2Conv,siFluc,siFinConv,1,MAN_FLCL_CONV,"Simple data conversion")
 *       CLETAB_CMD("XCBV",asClpXcnvPar,&stClpXcnvPar,&stFlcConvPar,NULL,siIniXcnv,siMapXcnv2Conv,siFluc,siFinConv,0,MAN_FLCL_XCNV,"Complex data conversion")
 *       CLETAB_CMD("ICNV",asClpIcnvPar,&stClpIcnvPar,&stFlcConvPar,NULL,siIniIcnv,siMapIcnv2Conv,siFluc,siFinConv,1,MAN_FLCL_ICNV,"Character conversion with libiconv")
 *       CLETAB_CMD("INFO",asClpInfoPar,&stClpInfoPar,&stFlcInfoPar,NULL,siIniInfo,siMapInfo2Info,siInfo,siFinInfo,1,MAN_FLCL_INFO,"Provide information")
 *       CLECMD_CLS
 *    };
 *
 *    CLEOTH_OPN(asOthTab) = {
 *       CLETAB_OTH("flcbyt","READ-FORMAT"  ,asClpWrtFmtPar,MAN_FLCBYT_READ_FORMAT ,HLP_FLCBYT_READ_FORMAT ,TRUE)
 *       CLETAB_OTH("flcbyt","WRITE-FORMAT" ,asClpRedFmtPar,MAN_FLCBYT_WRITE_FORMAT,HLP_FLCBYT_WRITE_FORMAT,TRUE)
 *       CLETAB_OTH("flcbyt","CONV-READ"    ,asClpElmCnvRed,MAN_FLCBYT_CONV_READ   ,HLP_FLCBYT_CONV_READ   ,TRUE)
 *       CLETAB_OTH("flcbyt","CONV-WRITE"   ,asClpElmCnvWrt,MAN_FLCBYT_CONV_WRITE  ,HLP_FLCBYT_CONV_WRITE  ,TRUE)
 *       CLETAB_OTH("flcbyt","FROM-TO-CONV" ,asClpElmCnv   ,MAN_FLCBYT_CONV        ,HLP_FLCBYT_CONV        ,TRUE)
 *       CLETAB_OTH("flcbyt","STATE"        ,asClpExtPar   ,MAN_FLCBYT_STATE       ,HLP_FLCBYT_STATE       ,FALSE)
 *       CLETAB_OTH("flcbyt","LOG"          ,asClpMemoryLog,MAN_FLCBYT_LOG         ,HLP_FLCBYT_LOG         ,FALSE)
 *       CLEOTH_CLS
 *    };
 *
 *    CLEDOC_OPN(asDocTab) = {
 *       CLETAB_DOC(CLE_DOCTYP_COVER         ,1,NULL           ,NULL               ,NULL                             ,"FLCL Manual"                       ,MAN_FLCL_COVER                            ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL           ,CLE_DOCKYW_PREFACE ,NULL                             ,"Preface"                           ,MAN_FLCL_PREFACE                          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"Trademarks"                        ,MAN_LEGAL_TRADEMARKS                       ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"License Terms"                     ,MAN_LEGAL_LICENSE_TERMS                    ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"About Limes"                       ,MAN_FLCL_ABOUT_LIMES                      ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"Overview"                          ,MAN_FLCL_OVERVIEW                         ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"Use Cases"                         ,MAN_FLCL_USECASES                         ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"Supported Systems"                 ,MAN_FLCL_SUPPORTED_SYSTEMS                ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,NULL           ,NULL               ,NULL                             ,"Technical Support"                 ,MAN_FLCL_SUPPORT                          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"1."           ,NULL               ,"clep.main"                      ,"Command Line Parser"               ,MAN_CLEP_MAIN                             ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.1."         ,NULL               ,NULL                             ,"Command Line Considerations"       ,MAN_CLEP_MAIN_CONSID                      ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_USEDENVAR     ,3,"1.2."         ,NULL               ,"clep.main.usedenv"              ,"Used Environment Variables"        ,MAN_CLEP_MAIN_USEDENV                     ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.3."         ,NULL               ,NULL                             ,"Environment Variable Mapping"      ,MAN_CLEP_MAIN_ENVARMAP                    ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.4."         ,NULL               ,"clep.main.filemap"              ,"Filename Mapping"                  ,MAN_CLEP_MAIN_FILEMAP                     ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.5."         ,NULL               ,NULL                             ,"Key Label Mapping"                 ,MAN_CLEP_MAIN_KEYLABMAP                   ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"1.6."         ,NULL               ,CLE_ANCHOR_MAIN_EBCDIC           ,"Special EBCDIC Codepage Support"   ,MAN_CLEP_MAIN_EBCDIC                      ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_BUILTIN       ,2,"2."           ,NULL               ,CLE_ANCHOR_BUILTIN_FUNCTIONS     ,"Built-in Functions"                ,MAN_CLEP_BUILTIN_FUNCTIONS                ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_PROGRAM       ,2,"3."           ,NULL               ,"flcl.utility"                   ,"FLCL Utility"                      ,MAN_FLCL_MAIN                             ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.1."         ,NULL               ,NULL                             ,"Installation"                      ,MAN_FLCL_MAIN_INSTALLATION                ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,4,"3.1.1."       ,NULL               ,NULL                             ,"License"                           ,MAN_FLCL_MAIN_INSTALLATION_LICENSE        ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,4,"3.1.2."       ,NULL               ,NULL                             ,"Download"                          ,MAN_FLCL_MAIN_INSTALLATION_DOWNLOAD       ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,4,"3.1.3."       ,NULL               ,NULL                             ,"Replace External Libraries"        ,MAN_LEGAL_REPLACE_EXTERNAL_LIBRARIES      ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.2."         ,NULL               ,NULL                             ,"Filename Handling"                 ,MAN_FLCL_MAIN_FILENAME_HANDLING           ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.3."         ,NULL               ,NULL                             ,"Directory Support"                 ,MAN_FLCL_MAIN_DIRECTORY_SUPPORT           ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.4."         ,NULL               ,"flcl.input.output.name.mapping" ,"Input to Output Name Mapping"      ,MAN_FLCL_MAIN_INPUT_TO_OUTPUT_NAME_MAPPING,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.5."         ,NULL               ,NULL                             ,"Key Label Handling"                ,MAN_FLCL_MAIN_KEY_LABEL_HANDLING          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.6."         ,NULL               ,NULL                             ,"Password Handling"                 ,MAN_FLCL_MAIN_PASSWORD_HANDLING           ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.7."         ,NULL               ,"flcl.handling.empty.records"    ,"Handling of Empty Records"         ,MAN_FLCL_MAIN_HANDLING_OF_EMPTY_RECORDS   ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.8."         ,NULL               ,"flcl.handling.spanned.records"  ,"Handling of Spanned Records"       ,MAN_FLCL_MAIN_HANDLING_OF_SPANNED_RECORDS ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.9."         ,NULL               ,NULL                             ,"Table Support"                     ,MAN_FLCL_MAIN_TABLE_SUPPORT               ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.10."        ,NULL               ,NULL                             ,"Pre- and Post-processing"          ,MAN_FLCL_MAIN_PRE_AND_POST_PROCESSING     ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.11."        ,NULL               ,NULL                             ,"Use of Built-in Functions"         ,MAN_FLCL_MAIN_BUILT_IN_FUNCTIONS          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.12."        ,NULL               ,NULL                             ,"CONV versus XCNV and ICNVs"        ,MAN_FLCL_MAIN_CONV_VERSUS_XCNV_AND_ICNV   ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.13."        ,NULL               ,NULL                             ,"CONV READ/WRITE Overview"          ,MAN_FLCL_MAIN_CONV_READ_WRITE_OVERVIEW    ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.14."        ,NULL               ,NULL                             ,"JCL Considerations"                ,MAN_FLCL_MAIN_JCL_CONSIDERATIONS          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,3,"3.15."        ,NULL               ,NULL                             ,"SAF Consideration"                 ,MAN_FLCL_MAIN_SAF_CONSIDERATIONS          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_PGMSYNOPSIS   ,3,"3.16."        ,NULL               ,NULL                             ,"FLCL Synopsis"                     ,MAN_CLEP_PROGRAM_SYNOPSIS                 ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_PGMSYNTAX     ,3,"3.17."        ,NULL               ,NULL                             ,"Syntax of FLCL"                    ,MAN_CLEP_PROGRAM_SYNTAX                   ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_PGMHELP       ,3,"3.18."        ,NULL               ,NULL                             ,"Help for FLCL"                     ,MAN_CLEP_PROGRAM_HELP                     ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_COMMANDS      ,2,"4."           ,NULL               ,"flcl.commands"                  ,"FLCL Commands"                     ,MAN_CLEP_PROGRAM_COMMANDS                 ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_OTHERCLP      ,2,"Appendix A."  ,NULL               ,"flcl.api.strings"               ,"API Strings"                       ,MAN_FLCL_OTHERCLP                         ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"Appendix B."  ,NULL               ,"flcl.appendix.flucfs"           ,"FLUC Filesystem for Linux"         ,MAN_FLCL_APPENDIX_FLUCFS                  ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"Appendix C."  ,NULL               ,"flcl.appendix.flucsub"          ,"FLUC Subsystem for z/OS"           ,MAN_FLCL_APPENDIX_FLUCSUB                 ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_LEXEMES       ,2,"Appendix D."  ,NULL               ,CLE_ANCHOR_APPENDIX_LEXEMES      ,"Lexemes"                           ,MAN_CLEP_APPENDIX_LEXEMES                 ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_GRAMMAR       ,2,"Appendix E."  ,NULL               ,CLE_ANCHOR_APPENDIX_GRAMMAR      ,"Grammar"                           ,MAN_CLEP_APPENDIX_GRAMMAR                 ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"Appendix F."  ,NULL               ,CLE_ANCHOR_APPENDIX_PROPERTIES   ,"Properties"                        ,MAN_CLEP_APPENDIX_PROPERTIES              ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_PROPREMAIN    ,3,"Appendix F.1.",NULL               ,NULL                             ,"Remaining Documentation"           ,MAN_CLEP_APPENDIX_PROP_REMAINING          ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_PROPDEFAULTS  ,3,"Appendix F.2.",NULL               ,NULL                             ,"Predefined Defaults"               ,MAN_CLEP_APPENDIX_PROP_DEFAULTS           ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,"Appendix G."  ,NULL               ,CLE_ANCHOR_APPENDIX_RETURNCODES  ,"Return Codes"                      ,MAN_CLEP_APPENDIX_RETURNCODES             ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_SPECIALCODES  ,3,"Appendix G.1.",NULL               ,NULL                             ,"Special Condition Codes"           ,MAN_FLCL_SPECIALCODES                     ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_REASONCODES   ,3,"Appendix G.2.",NULL               ,CLE_ANCHOR_APPENDIX_REASONCODES  ,"Reason Codes"                      ,MAN_CLEP_APPENDIX_REASONCODES             ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_VERSION       ,2,"Appendix H."  ,NULL               ,CLE_ANCHOR_APPENDIX_VERSION      ,"Version"                           ,MAN_CLEP_APPENDIX_VERSION                 ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_ABOUT         ,2,"Appendix I."  ,NULL               ,CLE_ANCHOR_APPENDIX_ABOUT        ,"About"                             ,MAN_CLEP_APPENDIX_ABOUT                   ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL           ,CLE_DOCKYW_GLOSSARY,NULL                             ,"Glossary"                          ,MAN_FLCL_GLOSSARY                         ,NULL)
 *       CLETAB_DOC(CLE_DOCTYP_CHAPTER       ,2,NULL           ,CLE_DOCKYW_COLOPHON,NULL                             ,"Imprint"                           ,MAN_FLCL_IMPRINT                          ,NULL)
 *       CLEDOC_CLS
 *    };
 *
 *    TsFl5Gbl*   psGbl=psFl5GblOpn(sizeof(acMsg),acMsg);
 *     siErr=siCleExecute(psGbl,asCmdTab,argc,argv,FLM_CLEP_DEFAULT_OWNER,FLM_CLEP_PROGRAM,
 *              "limes datentechnik(R) gmbh","support@flam.de",
 *              FLM_CLEP_CASE_SENSITIVITY,TRUE,TRUE,FALSE,FLM_CLEP_MINIMAL_KEYWORDLEN,
 *              pfErr,pfTrc,FLM_CLEP_DEPTH_STRING_1047,FLM_CLEP_OPTION_STRING,
 *              FLM_CLEP_SEPARATION_STRING,psMain->acLicTxt,
 *              FLM_VSN_STR"-"__BUILDNRSTR__,psMain->acVersion,psMain->acAbout,
 *              FLM_CLEP_PROGRAM_HELP,
 *              NULL,pcFlmErrors,asOthTab,NULL,siClpFile2String,
 *              NULL,(psGbl->isSafClpControl)?siClpSaf:NULL,pcDpa,0,asDocTab,asFl5EnvVarTab,MAN_FLCL_MAIN_USEDENV);
 *    vdFl5GblCls(psGbl);
 * }
 * @endcode
 *
 **********************************************************************
 *
 * @section CLPMAIN Command Line Parser (FLAMCLP)
 *
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
 * for example URL support, remote access, character conversion, etc. The
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
 * which of the arguments are chosen by the user. If you define overlays of
 * overlays an additional dimension for each level is used. In this case
 * you must define an array for this link and you get the child (lnk[0])
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
 * @code
 * ClpOpen()
 * ClpParsePro()
 * ClpParseCmd()
 * ClpClose()
 * @endcode
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
 * @subsection CLPLEXEMES FLAMCLP-Lexemes
 *
 * Call siClpLexemes() to get the current supported lexemes. The list below could be
 * a older state of the implementation.
 *
 * Lexemes (regular expressions) for argument list or parameter file:
 * @verbatim
   --| COMMENT   '#' [:print:]* '#'                              (will be ignored)
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
   --|           Strings can contain two '' to represent one '.
   --|           Strings can also be enclosed in " or ` instead of '.
   --|           Strings can directly start behind a '=' without enclosing ('`").
   --|           In this case the string ends at the next separator or operator
   --|           and keywords are preferred. To use keywords, separators or
   --|           operators in strings, enclosing quotes are required.
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
   --| SUPPLEMENT     '"' [:print:]* '"' |   (null-terminated string (properties)).
   --|           Supplements can contain two "" to represent one ".
   --|           Supplements can also be enclosed in ' or ` instead of ".
   --|           Supplements can also be enclosed in ' or ` instead of ".
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
   @endverbatim
 *
 * @subsection CLPGRAMMARL FLAMCLP-Grammar for command line
 *
 * Call siClpGrammar() to get the current supported grammar for the command lines.
 * The list below could be a older state of the implementation.
 *
 * Grammar for argument list or parameter file:
 * @verbatim
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
   --| It is recommended to use only enclosed array lists to know the end
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
   --|
   --| Property File Parser
   --| properties     -> property_list
   --| property_list  -> property SEP property_list
   --|                |  EMPTY
   --| property       -> keyword_list '=' SUPPLEMENT
   --| keyword_list   -> KEYWORD '.' keyword_list
   --|                |  KEYWORD
   --| SUPPLEMENT is a string in double quotation marks ("property")
   @endverbatim
 *
 * A list of objects requires parenthesis to enclose the arguments. Only
 * for one object of a certain level you can omit the round brackets. If
 * you want define more than one object of a certain level you must use
 * parenthesis to separate the objects from each other. In parameter files
 * the command string for an overlay can be start with a dot '.' or not.
 * The same is valid for the parenthesis '(...)' of an object.
 *
 * @subsection CLPGRAMMARP FLAMCLP-Grammar for property file
 *
 * Call siClpGrammar() to get the current supported grammar for property files.
 * The list below could be a older state of the implementation.
 *
 * Grammar for property file:
 * @verbatim
   --| Property File Parser
   --| properties     -> property_list
   --| property_list  -> property SEP property_list
   --|                |  EMPTY
   --| property       -> keyword_list '=' SUPPLEMENT
   --| keyword_list   -> KEYWORD '.' keyword_list
   --|                |  KEYWORD
   --| SUPPLEMENT is a string in double quotation marks ("property")
   @endverbatim
 *
 **********************************************************************
 *
 * @section CLEPUTIL Utility functions for FLAMCLE/CLP
 *
 * This interface provides additional several utility functions for the
 * Command Line Executor (CLE) and Processor (CLP). These help functions
 * makes the CLEP project platform independent and can also be used for
 * other things. The CLEPUTL object should be static linked to CLP and/or
 * CLE.
 *
 **********************************************************************/
/*! @cond PRIVATE */
#ifdef __cplusplus
   extern "C" {
#endif
#ifndef INC_CLEPUTL_H
#define INC_CLEPUTL_H
/*! @endcond */

#include <stdio.h>
#include <stdlib.h>

/*! @cond PRIVATE */
#ifndef TRUE
#  define TRUE             (1)
#endif

#ifndef FALSE
#  define FALSE            (0)
#endif

#ifndef EOS
   #define EOS             (0x00)
#endif

#ifndef __PRINTF_CHECK__
#  define __PRINTF_CHECK__(A,B)
#endif
/*! @endcond */

#define CLEP_DEFAULT_CCSID_ASCII     819  // "ISO8859-1" Latin-1
#define CLEP_DEFAULT_CCSID_EBCDIC    1047 // "IBM-1047"  Open Systems Latin-1

/** Free memory space */
#define SAFE_FREE(x) do { if ((x) != NULL) {free((void*)(x)); (x)=NULL;} } while(0)

#define CHECK_ENVAR_ON(e)     ((e)!=NULL && (strcmp((e),"ON")==0 || strcmp((e),"YES")==0))
#define CHECK_ENVAR_OFF(e)    ((e)!=NULL && (strcmp((e),"OFF")==0 || strcmp((e),"NO")==0))

typedef struct EnVarList {
/**
 * @brief Environment variable list\n
 */
   char*                pcName;
   char*                pcValue;
   struct EnVarList*    psNext;
}TsEnVarList;

#ifdef __WIN__
#  ifndef __FL5__
#     ifndef localtime_r
#        define localtime_r(t,s)   localtime(t)
#     endif
#     ifndef gmtime_r
#        define gmtime_r(t,s)      gmtime(t)
#     endif
#     ifndef asctime_r
#        define asctime_r(s,b)     asctime(s)
#     endif
#     ifndef ctime_r
#        define ctime_r(t,b)       ctime(t)
#     endif
#  endif
extern char* windowsversionstring(char** vstr, size_t* size);
extern int win_setenv(const char* name, const char* value);
extern int win_unsetenv(const char* name);
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) win_setenv((name), (value))
#  define UNSETENV(name)      win_unsetenv((name))
#elif defined (__ZOS__) || defined (__ZOS__)
#  define GETENV(name)        __getenv((name))
#  define SETENV(name, value) setenv((name), (value), 1)
#  define UNSETENV(name)      unsetenv((name))
#else
#  define GETENV(name)        getenv((name))
#  define SETENV(name, value) setenv((name), (value), 1)
#  define UNSETENV(name)      unsetenv((name))
#endif

#define isStr(c) (isprint(c) || (c)==C_TLD  || (c)==C_DLR || (c)==C_ATS || (c)==C_BSL || (c)==C_CRT || (c)==C_EXC)
#define isKyw(c) (isalnum(c) || (c)=='_')
#define isCon(c) (isKyw(c)   || (c)=='-' || (c)=='/')

#define ISDDNAME(p)     (strlen(p)>3 && toupper((p)[0])=='D' && toupper((p)[1])=='D' && (p)[2]==':')
#define ISPATHNAME(p)   (strchr((p),'/')!=NULL)
#define ISDSNAME(p)     (strlen(p)>2 && toupper((p)[0])=='/' && toupper((p)[1])=='/')
#define ISGDGMBR(m)     ((m)[0]=='0' || (m)[0]=='+' || (m)[0]=='-')
#define ISDDN(c)        (isalnum(c) || (c)==C_DLR || (c)==C_HSH || (c)==C_ATS)

extern FILE* fopen_hfq(const char* name, const char* mode);
extern FILE* fopen_hfq_nowarn(const char* name, const char* mode);
extern FILE* freopen_hfq(const char* name, const char* mode, FILE* stream);
extern long long getFileSize(const char* name);
#ifdef __ZOS__
   extern FILE* fopen_tmp(void);
   extern int   fclose_tmp(FILE* fp);
   extern int   remove_hfq(const char* name);
#else
#  define fopen_tmp()            tmpfile()
#  define fclose_tmp(fp)         fclose((fp))
#  define remove_hfq(n)          remove(n)
#endif

/* Definition of return/condition/exit codes **************************/

/** 0  - command line, command syntax, mapping, execution and finish of the command was successful*/
#define CLERTC_OK             0

/** 1  - command line, command syntax, mapping, execution and finish of the command was successful but a warning can be found in the log*/
#define CLERTC_INF            1

/** 2  - command line, command syntax, mapping, execution was successful but cleanup of the command failed (may not happened)*/
#define CLERTC_FIN            2

/** 4  - command line, command syntax and mapping was successful but execution of the command returns with a warning*/
#define CLERTC_WRN            4

/** 8  - command line, command syntax and mapping was successful but execution of the command returns with an error*/
#define CLERTC_RUN            8

/** 12 - command line and command syntax was OK but mapping failed*/
#define CLERTC_MAP            12

/** 16 - command line was OK but command syntax was wrong*/
#define CLERTC_SYN            16

/** 20 - command line was wrong (user error)*/
#define CLERTC_CMD            20

/** 24 - initialization of parameter structure for the command failed (may not happened)*/
#define CLERTC_INI            24

/** 28 - configuration is wrong (user error)*/
#define CLERTC_CFG            28

/** 32 - table error (something within the predefined tables is wrong)*/
#define CLERTC_TAB            32

/** 36 - system error (mainly memory allocation or some thing like this failed)*/
#define CLERTC_SYS            36

/** 40 - access control or license error*/
#define CLERTC_ACS            40

/** 44 - interface error (parameter pointer equals to NULL or something like this)*/
#define CLERTC_ITF            44

/** 48 - memory allocation failed (e.g. dynamic string handling)*/
#define CLERTC_MEM            48

/** 64 - fatal error (basic things are damaged)*/
#define CLERTC_FAT            64

/** maximal condition code value (greater condition codes are special return codes)*/
#define CLERTC_MAX            64

/**********************************************************************/

/**
 * Calculates FNV1A hash value
 * @param uiLen   length of the data
 * @param pcDat   pointer to the data
 * @return
 */
static inline unsigned int fnvHash(const unsigned int uiLen, const unsigned char* pcDat) {
   unsigned int         uiHsh=0x811C9DC5U;
   const unsigned char* pcEnd=pcDat+uiLen;
   while(pcDat<pcEnd) {
      uiHsh=0x01000193U * (uiHsh ^ *pcDat++);
   }
   return(uiHsh);
}

/**
 * Ensure memset is not eliminate by the compiler
 * @param ptr  pointer to the data
 * @param len  size of the data
 */
extern void secure_memset(void *ptr,size_t len);

/**
 * Returns the current user id.
 * @param size    size of the buffer
 * @param buffer  pointer to the buffer
 * @return        pointer to the buffer containing the current user id (null-terminated)
 */
extern char* userid(const int size, char* buffer);

/**
 * Returns the current home directory.
 * @param flag    if true then slash/backslash are added
 * @param size    size of the string buffer
 * @param buffer  pointer to the buffer
 * @return        pointer to the buffer containing the current home directory (null-terminated)
 */
extern char* homedir(const int flag, const int size, char* buffer);

/**
 * Returns the current user id.
 * @return        pointer to the buffer containing the current user id (null-terminated) must be freed by the caller
 */
extern char* duserid(void);

/**
 * Returns the current home directory.
 * @param flag    if true then slash/backslash are added
 * @return        pointer to the buffer containing the current home directory (null-terminated) must be freed by the caller
 */
extern char* dhomedir(const int flag);

/**
 * Gets an environment variable and stores it in the provided buffer. If
 * the buffer is not large enough, the variable value is truncated.
 * @param name Name of the environment variable
 * @param buffer Pointer to the buffer for the variable value
 * @param bufsiz Size of the buffer
 * @return If bufsiz > 0, returns the buffer pointer which contains a null-terminated string
 *         or NULL (variable does not exist). If bufsiz == 0, buffer is returned unmodified.
 */
extern char* safe_getenv(const char* name, char* buffer, size_t bufsiz);

/**
 * Un-escape a string as part of special character support in EBCDIC codepages (static version).
 * @param input     pointer to the input string containing the escape sequences
 * @param output    pointer to the output string for un-escaping (could be equal to the input pointer)
 * @return          pointer to the un-escaped output or NULL if error
 */
extern char* unEscape(const char* input, char* output);

/**
 * Un-escape a string as part of special character support in EBCDIC codepages (dynamic version).
 * @param input     pointer to the input string containing the escape sequences
 * @return          pointer to the un-escaped output or NULL if error (calling application must free the memory)
 */
extern char* dynUnEscape(const char* input);

/**
 * Works like printf but print only in debug mode.
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int printd(const char* format,...) __PRINTF_CHECK__(1, 2);

/**
 * Works like snprintf but concatenates the format string to the buffer.
 * @param buffer  pointer to the string buffer
 * @param size    size of the string buffer
 * @param format  format string
 * @return        amount of characters printed (0 are mainly a error)
 */
extern int snprintc(char* buffer,const size_t size,const char* format,...) __PRINTF_CHECK__(3, 4);

/**
 * Works like snprintf but does reallocation of the buffer (maximal expansion of the format string can be specified).
 * @param buffer    pointer to pointer to the string buffer (is updated, could be NULL at beginning)
 * @param size      pointer to size of the string buffer (is updated, could be 0 at beginning)
 * @param expansion maximal expected expansion of the format string (size must be fit strlen(*buffer)+strlen(format)+expansion+1)
 * @param format    format string
 * @return          amount of characters printed (0 are mainly a error)
 */
extern int srprintc(char** buffer,size_t* size,const size_t expansion,const char* format,...) __PRINTF_CHECK__(4, 5);

/**
 * Works like snprintc but does reallocation of the buffer (maximal expansion of the format string can be specified).
 * @param buffer    pointer to pointer to the string buffer (is updated, could be NULL at beginning)
 * @param size      pointer to size of the string buffer (is updated, could be 0 at beginning)
 * @param expansion maximal expected expansion of the format string (size must be fit strlen(format)+expansion+1)
 * @param format    format string
 * @return          amount of characters printed (0 are mainly a error)
 */
extern int srprintf(char** buffer,size_t* size,const size_t expansion,const char* format,...) __PRINTF_CHECK__(4, 5);

/**
 * Prints man pages to a file, inserting owner, program name, build number, state and date into placeholders
 * @param file    pointer to the file
 * @param own     owner name for replacement (&{OWN})
 * @param pgm     program name for replacement (&{PGM})
 * @param bld     build/version string for replacement (&{BLD})
 * @param man     manpage to print, which can contain &{PGM}, &{OWN}, &{BLD}, &{DATE} and &{STATE}
 * @param cnt     amount of '\n' added to man page (0,1,2 (>2=2))
 */
extern void fprintm(FILE* file,const char* own, const char* pgm, const char* bld, const char* man, const int cnt);

/**
 * Prints man pages to a buffer, inserting owner, program name, build number, state and date into placeholders
 * @param buffer  pointer to the buffer
 * @param size    size of the buffer
 * @param own     owner name for replacement (&{OWN})
 * @param pgm     program name for replacement (&{PGM})
 * @param bld     build/version string for replacement (&{BLD})
 * @param man     manpage to print, which can contain &{PGM}, &{OWN}, &{BLD}, &{DATE} and &{STATE}
 * @param cnt     amount of '\n' added to man page (0,1,2 (>2=2))
 * @return        same as snprintf
 */
extern int snprintm(char* buffer, size_t size, const char* own, const char* pgm, const char* bld, const char* man, const int cnt);


/**
 * This function parses a zero terminated string array of a certain length or terminated with 0xFF.
 * Such a string array is the result of a variable length array of strings provided by CLP.
 * If you don't know the length, please provide a negative number to look for 0xFF termination.
 * For 0xFF termination you must define the CLPFLG_DLM. This is useful if there is no capability
 * to use the ELN link to determine the length of the string array. Each call returns the pointer to
 * the next string in the array, if no string is found anymore NULL is returned.
 * @param hdl     pointer of pointer to string initialized with NULL at beginning
 * @param str     pointer to the string arrays from CLP
 * @param len     -1 for 0xFF delimiter parsing or the ELN of the string array
 * @return        pointer to one string or NULL if no more string
 */
extern const char* prsdstr(const char** hdl, const char* str, int len);

#ifndef __WIN__
/* Force the use of strlcpy instead of strncpy, but not on Windows because it causes trouble with MinGW headers */
#ifdef strncpy
#undef strncpy
#endif
#define strncpy(...) Error: Do not use strncpy! Use strlcpy instead!
#endif

/**
 * Works like strncpy but ensures null-termination.
 * @param dest    pointer to destination string
 * @param src     pointer to source string
 * @param n       size of memory available for buffer
 * @return        number of bytes actually copied (excludes NUL-termination)
 */
extern size_t strlcpy(char *dest, const char *src, size_t n);

/**
 * Get environment variable and handle HOME, USER, CUSEr, Cuser, cuser, OWNER, ENVID if not defined
 * @param name environment variable name
 * @param length optional length of the name if no zero termination (0 if zero termination)
 * @param size size of string
 * @param string containing the value for the corresponding environment variable
 * @return pointer to string
 */
extern char* getenvar(const char* name,const size_t length,const size_t size,char* string);

/**
 * Replace all environment variables enclosed with '<' and '>' to build a string
 * @param string string for replacement
 * @param size size of replacement string
 * @return pointer to string
 */
extern char* mapstr(char* string,int size);

/**
 * Replace all environment variables enclosed with '<' and '>' to build a dynamic string
 * @param string string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapstr(const char* string,int method);

/**
 * Replace all environment variables enclosed with '(' and ')' to build a dynamic string
 * @param string string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapxml(const char* string,int method);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>' to build a file name
 * @param file string for replacement
 * @param size size of replacement string
 * @return pointer to file
 */
extern char* mapfil(char* file,int size);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>' to build a dynamic file name
 * @param file string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to the new allocated string or NULL if error
 */
extern char* dmapfil(const char* file, int method);

/**
 * Replace '!' with ENVID, '~' with "<SYSUID>", '^' with "<OWNERID>" and all environment variables enclosed with '<' and '> to build a key label'
 * @param label string for replacement
 * @param size size of replacement string
 * @param toUpper for mapping file to upper
 * @return pointer to label
 */
extern char* maplab(char* label,int size,int toUpper);

/**
 * Replace '!' with ENVID, '~' with "<SYSUID>", '^' with "<OWNERID>" and all environment variables enclosed with '<' and '> to build a dynamic key label'
 * @param label string for replacement
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to new allocated string or NULL if error
 */
extern char* dmaplab(const char* label, int method);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>'
 * @param dest string for replacement
 * @param size size of replacement string
 * @param source original string
 * @return pointer to dest
 */
extern char* cpmapfil(char* dest, int size,const char* source);

/**
 * Replace '~' with "<HOME>" and all environment variables enclosed with '<' and '>' and returns a dynamic string
 * @param file string for replacement
 * @return pointer to dynamic allocated string
 */
extern char* dcpmapfil(const char* file);

/**
 * Use rpltpl() and maplab() to build key label names, based on key label templates
 * @param label string for replacement
 * @param size size of replacement string
 * @param templ key label template (with %x)
 * @param values value string for replacement (x:%s\n)
 * @param toUpper for mapping label to upper
 * @return pointer to label
 */
extern char* cpmaplab(char* label, int size,const char* templ, const char* values, int toUpper);

/**
 * Use drpltpl() and dmaplab() to build key label names, based on key label templates in dynamic form
 * @param templ key label template (with %x)
 * @param values value string for replacement (x:%s\n)
 * @param method conversion method (1 - to upper, 2 - to lower, else nothing)
 * @return pointer to dynamic allocated string
 */
extern char* dcpmaplab(const char* templ, const char* values, int method);

/**
 * Determines the local CCSID by querying nl_langinfo() (POSIX) or GetCPInfoEx() (Windows).
 * If none of both are available or no valid CCSID can be determined,
 * mapl2c() is called. If it also fails to determine the system default CSSID,
 * the CCSID for ASCII (ISO8859-1) or IBM-1047 (EBCDIC platforms) is returned.
 * @return A supported CCSID > 0
 */
extern unsigned int localccsid(void);

/**
 * Map environment variable LANG to CCSID
 * @param isEBCDIC if true returns EBCDIC code pages else ASCII
 * @return NULL in case of an error or pointer to a static string containing the CCSID
 */
extern const char* mapl2c(unsigned isEBCDIC);

/**
 * Map environment variable LANG to CCSID
 * @param pcLang   string containing the value of the environment variable LANG
 * @param isEbcdic if true returns EBCDIC code pages else ASCII
 * @return NULL in case of an error or pointer to a static string containing the CCSID
 */
extern const char* lng2ccsd(const char* pcLang, unsigned isEbcdic);

/**
 * Map CCSID in encoding string
 * @param uiCcsId CCSID
 * @return    encoding string
 */
extern const char* mapccsid(const unsigned int uiCcsId);

/**
 * Map encoding string in CCSID
 * @param p encoding string
 * @return    CCSID
 */
extern unsigned int mapcdstr(const char* p);

/**********************************************************************/

/**
 * Convert binary to hex
 * @param bin binary blob
 * @param hex hex string
 * @param len length of binary blob
 * @return amount of converted bytes
 */
extern unsigned int bin2hex(
   const unsigned char* bin,
         char*          hex,
   const unsigned int   len);

/**
 * Convert from hex to binary
 * @param hex  hex string
 * @param bin  binary string
 * @param len  length of hex string
 * @return     amount of converted bytes
 */
extern unsigned int hex2bin(
   const char*          hex,
         unsigned char* bin,
   const unsigned int   len);

/**
 * Convert character string to US-ASCII(UTF-8) and stops at not convertible chars
 * @param chr  character string
 * @param asc  ASCII string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int chr2asc(
   const char*          chr,
         char*          asc,
   const unsigned int   len);

/**
 * Convert character string to EBCDIC (only non variant characters) and stops at not convertible chars
 * @param chr  character string
 * @param ebc  EBCDIC string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int chr2ebc(
   const char*          chr,
         char*          ebc,
   const unsigned int   len);

/**
 * Convert ASCII to character string (only non variant characters) and stops at not convertible chars
 * @param asc  ASCII string
 * @param chr  character string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int asc2chr(
   const char*          asc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert ASCII to character string (only non variant characters) and replace not convertible chars with '_'
 * @param asc  ASCII string
 * @param chr  character string
 * @param len  length
 */
extern void asc_chr(
   const char*          asc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert character string to US-ASCII(UTF-8) and replace not convertible chars with '_'
 * @param chr  character string
 * @param asc  ASCII string
 * @param len  length
 */
extern void chr_asc(
   const char*          chr,
         char*          asc,
   const unsigned int   len);

/**
 * Convert EBCDIC to character string (only non variant characters) and stops at not convertible chars
 * @param ebc  EBCDIC string
 * @param chr  character string
 * @param len  length
 * @return     amount of converted bytes
 */
extern unsigned int ebc2chr(
   const char*          ebc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert EBCDIC to character string (only non variant characters) and replace not convertible chars with '_'
 * @param ebc  EBCDIC string
 * @param chr  character string
 * @param len  length
 */
extern void ebc_chr(
   const char*          ebc,
         char*          chr,
   const unsigned int   len);

/**
 * Convert character string to EBCDIC (only non variant characters) and replace not convertible chars with '_'
 * @param chr  character string
 * @param ebc  EBCDIC string
 * @param len  length
 */
extern void chr_ebc(
   const char*          chr,
         char*          ebc,
   const unsigned int   len);

/**
 * Read a file using the specified filename and reads the whole content
 * into the supplied buffer. The buffer is reallocated and bufsize updated,
 * if necessary.
 *
 * This is the default implementation if no file to string function for CLEP provided
 *
 * @param hdl is ignored and not used (required for default implementation of call back function)
 * @param filename The path and name of the file to read
 * @param buf A pointer to a pointer to a buffer, may be a pointer to NULL for allocation else reallocation
 * @param bufsize A pointer to the size of buf, is updated ater the call
 * @param errmsg Pointer to a provided buffer for the error message (optional (can be NULL), result is null terminated)
 * @param msgsiz The size of the buffer for the error message (optional (can be 0))
 * @return A positive value indicates the number of bytes read and copied into buf.
 *         A negative value indicates an error, in which case the content of buf is undefined.
 *         Error codes:
 *         * -1: invalid arguments
 *         * -2: fopen() failed
 *         * -3: integer overflow, file too big
 *         * -4: realloc() failed
 *         * -5: file read error
 */
extern int file2str(void* hdl, const char* filename, char** buf, int* bufsize, char* errmsg, const int msgsiz);

/**
 * Takes an array of null-terminated strings and concatenates all strings
 * into one single string separated by the specified separator. The resulting
 * string is put into the out buffer which may be reallocated if necessary.
 * If the buffer already contain a string the remaining strings are concatenated.
 * @param array Input array of null-terminated strings.
 * @param count Number of string in array
 * @param separ Separator of arbitrary length (may be NULL if separLen=0)
 * @param separLen length of separator
 * @param out Pointer to an output buffer (may be reallocated)
 * @param outlen Size of output buffer
 * @return Error codes:
 *         *  0: success
 *         * -1: invalid arguments
 *         * -2: realloc() failed
 */
extern int arry2str(char* array[], const int count, const char* separ, const int separLen, char** out, int* outlen);

/**
 * Compare of two string
 *
 * The procedure combines strcmp, stricmp, strncmp and strchr in one function.
 *
 * @param[in]  ca Flag if case sensitiv (TRUE) or not (FALSE)
 * @param[in]  s1 String 1 to compare
 * @param[in]  s2 string 2 to compare
 * @param[in]  n  If c!=0 then minimum else maximum amount of character to compare  (0=disabled)
 * @param[in]  c  Character where the compare stops or -1 for keyword syntax
 * @param[in]  f  If true only compare up to null termination or stop char if false (normal compare) including null termination or stop char
 *
 * @return signed integer with 0 for equal and !=0 for different
 */
extern int strxcmp(
   const int                     ca,
   const char*                   s1,
   const char*                   s2,
   const int                     n,
   const int                     c,
   const int                     f);

/**
 * Build time string
 *
 * Convert a time integer to a 20 byte time string of form YYYY-MM-DD HH:MM:SS.
 *
 * @param[in]  t  time in seconds since 1970 or 0 for current time
 * @param[in]  p  NULL to return a static variable or a pointer where the 20 bytes are copied in
 *
 * @return pointer to the time string
 */

#define CSTIME_BUFSIZ   24

extern char* cstime(signed long long t, char* p);

/**
 * Load environment variables from buffer
 *
 * @param[in] uiLen Length of the buffer with environment variables
 * @param[in] pcBuf Buffer containing list of environment variables (ASCII or EBCDIC separated by new line or semicolon)
 * @param[in] pfOut File pointer for output messages
 * @param[in] pfErr File pointer for error messages
 * @param[out] ppList Pointer to an optional envar list for reset (the list and each string must freed by caller)
 *
 * @return    >=0 amount of successful defined environment variables else -1*CLERTCs
 */
extern int loadEnvars(const unsigned int uiLen, const char* pcBuf, FILE* pfOut, FILE* pfErr, TsEnVarList** ppList);

/**
 * Read and set environment variables from file
 *
 * @param[in] pcFil Filename, if pcFil==NULL use "DD:STDENV" instead
 * @param[in] pfOut File pointer for output messages
 * @param[in] pfErr File pointer for error messages
 * @param[out] ppList Pointer to an optional envar list for reset (the list and each string must freed by caller)
 *
 * @return    >=0 amount of successful defined environment variables else -1*CLERTCs
 */
extern int readEnvars(const char* pcFil, FILE* pfOut, FILE* pfErr, TsEnVarList** ppList);

/**
 * Store envars in a list for reset
 *
 * @param[out] ppList Pointer to envar list for reset
 * @param[in]  pcName Name of the environment variable
 * @param[in]  pcValue Value of the environment variable (NULL for unset)
 */
extern int envarInsert(TsEnVarList** ppList,const char* pcName,const char* pcValue);

/**
 * Reset list of environment variables
 *
 * @param[in] ppList Pointer to envar lisl
 * @return  amount of envars reset or -1*CLERTCs
 */
extern int resetEnvars(TsEnVarList** ppList);


/**********************************************************************/

typedef struct DiaChr {
/**
 * @brief Reset list structure of environment variables\n
 */
   char              exc[4];
   char              hsh[4];
   char              dlr[4];
   char              ats[4];
   char              sbo[4];
   char              bsl[4];
   char              sbc[4];
   char              crt[4];
   char              grv[4];
   char              cbo[4];
   char              vbr[4];
   char              cbc[4];
   char              tld[4];
   char              svb[4];
   char              sbs[4];
   char              idt[4];
} TsDiaChr;

extern void          init_diachr(TsDiaChr* psDiaChr,const unsigned int uiCcsId);

#ifdef __EBCDIC__

extern TsDiaChr      gsDiaChr;

extern int           ebcdic_srprintc(char** buffer, size_t* size, const size_t expansion, const char* format, ...);
extern int           ebcdic_snprintf(char* string, size_t size, const char* format, ...);
extern int           ebcdic_sprintf(char* string, const char* format, ...);
extern int           ebcdic_fprintf(FILE* file, const char* format, ...);

extern char          init_char(const char* p);
extern const char*   init_string(const char* p);

#  define HSH_PBRK   "\x7B\x4A\x63\xB1\x69"
#  define ATS_PBRK   "\x7C\xB5\x80\xEC\x44\xAC"

#  define C_EXC      ((gsDiaChr.exc[0])?gsDiaChr.exc[0]:init_char(gsDiaChr.exc))
#  define C_HSH      ((gsDiaChr.hsh[0])?gsDiaChr.hsh[0]:init_char(gsDiaChr.hsh))
#  define C_DLR      ((gsDiaChr.dlr[0])?gsDiaChr.dlr[0]:init_char(gsDiaChr.dlr))
#  define C_ATS      ((gsDiaChr.ats[0])?gsDiaChr.ats[0]:init_char(gsDiaChr.ats))
#  define C_SBO      ((gsDiaChr.sbo[0])?gsDiaChr.sbo[0]:init_char(gsDiaChr.sbo))
#  define C_BSL      ((gsDiaChr.bsl[0])?gsDiaChr.bsl[0]:init_char(gsDiaChr.bsl))
#  define C_SBC      ((gsDiaChr.sbc[0])?gsDiaChr.sbc[0]:init_char(gsDiaChr.sbc))
#  define C_CRT      ((gsDiaChr.sbo[0])?gsDiaChr.crt[0]:init_char(gsDiaChr.crt))
#  define C_GRV      ((gsDiaChr.grv[0])?gsDiaChr.grv[0]:init_char(gsDiaChr.grv))
#  define C_CBO      ((gsDiaChr.cbo[0])?gsDiaChr.cbo[0]:init_char(gsDiaChr.cbo))
#  define C_VBR      ((gsDiaChr.vbr[0])?gsDiaChr.vbr[0]:init_char(gsDiaChr.vbr))
#  define C_CBC      ((gsDiaChr.cbc[0])?gsDiaChr.cbc[0]:init_char(gsDiaChr.cbc))
#  define C_TLD      ((gsDiaChr.tld[0])?gsDiaChr.tld[0]:init_char(gsDiaChr.tld))
#  define S_EXC      ((gsDiaChr.exc[0])?gsDiaChr.exc:init_string(gsDiaChr.exc))
#  define S_HSH      ((gsDiaChr.hsh[0])?gsDiaChr.hsh:init_string(gsDiaChr.hsh))
#  define S_DLR      ((gsDiaChr.dlr[0])?gsDiaChr.dlr:init_string(gsDiaChr.dlr))
#  define S_ATS      ((gsDiaChr.ats[0])?gsDiaChr.ats:init_string(gsDiaChr.ats))
#  define S_SBO      ((gsDiaChr.sbo[0])?gsDiaChr.sbo:init_string(gsDiaChr.sbo))
#  define S_BSL      ((gsDiaChr.bsl[0])?gsDiaChr.bsl:init_string(gsDiaChr.bsl))
#  define S_SBC      ((gsDiaChr.sbc[0])?gsDiaChr.sbc:init_string(gsDiaChr.sbc))
#  define S_CRT      ((gsDiaChr.sbo[0])?gsDiaChr.crt:init_string(gsDiaChr.crt))
#  define S_GRV      ((gsDiaChr.grv[0])?gsDiaChr.grv:init_string(gsDiaChr.grv))
#  define S_CBO      ((gsDiaChr.cbo[0])?gsDiaChr.cbo:init_string(gsDiaChr.cbo))
#  define S_VBR      ((gsDiaChr.vbr[0])?gsDiaChr.vbr:init_string(gsDiaChr.vbr))
#  define S_CBC      ((gsDiaChr.cbc[0])?gsDiaChr.cbc:init_string(gsDiaChr.cbc))
#  define S_TLD      ((gsDiaChr.tld[0])?gsDiaChr.tld:init_string(gsDiaChr.tld))
#  define S_SVB      ((gsDiaChr.svb[0])?gsDiaChr.svb:init_string(gsDiaChr.svb))
#  define S_SBS      ((gsDiaChr.sbs[0])?gsDiaChr.sbs:init_string(gsDiaChr.sbs))
#  define S_IDT      ((gsDiaChr.idt[0])?gsDiaChr.idt:init_string(gsDiaChr.idt))

#  define esrprintc  ebcdic_srprintc
#  define esnprintf  ebcdic_snprintf
#  define esprintf   ebcdic_sprintf
#  define efprintf   ebcdic_fprintf

#else

#  define HSH_PBRK   "#"   /*nodiac*/
#  define ATS_PBRK   "@"   /*nodiac*/

#  define C_EXC      '!'   /*nodiac*/
#  define C_HSH      '#'   /*nodiac*/
#  define C_DLR      '$'   /*nodiac*/
#  define C_ATS      '@'   /*nodiac*/
#  define C_SBO      '['   /*nodiac*/
#  define C_BSL      '\\'  /*nodiac*/
#  define C_SBC      ']'   /*nodiac*/
#  define C_CRT      '^'   /*nodiac*/
#  define C_GRV      '`'   /*nodiac*/
#  define C_CBO      '{'   /*nodiac*/
#  define C_VBR      '|'   /*nodiac*/
#  define C_CBC      '}'   /*nodiac*/
#  define C_TLD      '~'   /*nodiac*/
#  define S_EXC      "!"   /*nodiac*/
#  define S_HSH      "#"   /*nodiac*/
#  define S_DLR      "$"   /*nodiac*/
#  define S_ATS      "@"   /*nodiac*/
#  define S_SBO      "["   /*nodiac*/
#  define S_BSL      "\\"  /*nodiac*/
#  define S_SBC      "]"   /*nodiac*/
#  define S_CRT      "^"   /*nodiac*/
#  define S_GRV      "`"   /*nodiac*/
#  define S_CBO      "{"   /*nodiac*/
#  define S_VBR      "|"   /*nodiac*/
#  define S_CBC      "}"   /*nodiac*/
#  define S_TLD      "~"   /*nodiac*/
#  define S_SVB      "=|"  /*nodiac*/
#  define S_SBS      "/\\" /*nodiac*/
#  define S_IDT      "--|" /*nodiac*/

#  define esrprintc  srprintc
#  define esnprintf  snprintf
#  define esprintf   sprintf
#  define efprintf   fprintf

#endif

/**********************************************************************/

/*! @cond PRIVATE */
#endif /* INC_CLEPUTL_H */

#ifdef __cplusplus
   }
#endif
/*! @endcond */
