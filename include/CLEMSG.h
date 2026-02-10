/**********************************************************************/
/**
 * @file CLEMSG.h
 * @brief messages for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
 * @author limes datentechnik gmbh
 * @date 05.09.2013\n
 * @copyright (c) 2013 limes datentechnik gmbh
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
 ***********************************************************************/

/*! @cond PRIVATE */
#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLEMSG_H
#define INC_CLEMSG_H
/*! @endcond */

/**
 * @defgroup CLE_MESSAGES CLE Message Strings
 * @brief Help and syntax messages for CLE built-in functions
 *
 * This group contains all help (HLP_*) and syntax (SYN_*) message strings
 * used by the Command Line Executor's built-in functions. Each built-in
 * function has two associated strings:
 * - HLP_* provides a brief description of the function's purpose
 * - SYN_* provides the command syntax pattern
 *
 * @{
 */

/** @brief Help message for SYNTAX built-in function */
static const char* HLP_CLE_BUILTIN_SYNTAX  ="Provides the syntax for each command";
/** @brief Syntax pattern for SYNTAX built-in function */
static const char* SYN_CLE_BUILTIN_SYNTAX  ="SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]";
/** @brief Help message for HELP built-in function */
static const char* HLP_CLE_BUILTIN_HELP    ="Provides quick help for arguments";
/** @brief Syntax pattern for HELP built-in function */
static const char* SYN_CLE_BUILTIN_HELP    ="HELP [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]";
/** @brief Help message for MANPAGE built-in function */
static const char* HLP_CLE_BUILTIN_MANPAGE ="Provides manual pages (detailed help)";
/** @brief Syntax pattern for MANPAGE built-in function */
static const char* SYN_CLE_BUILTIN_MANPAGE ="MANPAGE [function | command[.path][=filename]] | [filename]";
/** @brief Help message for GENDOCU built-in function */
static const char* HLP_CLE_BUILTIN_GENDOCU ="Generates auxiliary documentation (ASCIIDOC text format)";
/** @brief Syntax pattern for GENDOCU built-in function */
static const char* SYN_CLE_BUILTIN_GENDOCU ="GENDOCU [command[.path]=]filename [NONBR] [SHORT]";
/** @brief Help message for HTMLDOC built-in function */
static const char* HLP_CLE_BUILTIN_HTMLDOC ="Generates the complete HTML documentation into a folder";
/** @brief Syntax pattern for HTMLDOC built-in function */
static const char* SYN_CLE_BUILTIN_HTMLDOC ="HTMLDOC [path] [NUMBERS] [TYPES] [SHORT]";
/** @brief Help message for GENPROP built-in function */
static const char* HLP_CLE_BUILTIN_GENPROP ="Generates a property file";
/** @brief Syntax pattern for GENPROP built-in function */
static const char* SYN_CLE_BUILTIN_GENPROP ="GENPROP [command=]filename";
/** @brief Help message for SETPROP built-in function */
static const char* HLP_CLE_BUILTIN_SETPROP ="Activate a property file";
/** @brief Syntax pattern for SETPROP built-in function */
static const char* SYN_CLE_BUILTIN_SETPROP ="SETPROP [command=]filename";
/** @brief Help message for CHGPROP built-in function */
static const char* HLP_CLE_BUILTIN_CHGPROP ="Change a property value in the currently active property file";
/** @brief Syntax pattern for CHGPROP built-in function */
static const char* SYN_CLE_BUILTIN_CHGPROP ="CHGPROP command [path[=value]]*";
/** @brief Help message for DELPROP built-in function */
static const char* HLP_CLE_BUILTIN_DELPROP ="Remove a property file from configuration";
/** @brief Syntax pattern for DELPROP built-in function */
static const char* SYN_CLE_BUILTIN_DELPROP ="DELPROP [command]";
/** @brief Help message for GETPROP built-in function */
static const char* HLP_CLE_BUILTIN_GETPROP ="Show current properties";
/** @brief Syntax pattern for GETPROP built-in function */
static const char* SYN_CLE_BUILTIN_GETPROP ="GETPROP [command[.path] [DEPTH1 | ... | DEPTH9 | DEPALL | DEFALL]]";
/** @brief Help message for SETOWNER built-in function */
static const char* HLP_CLE_BUILTIN_SETOWNER="Defines the current owner";
/** @brief Syntax pattern for SETOWNER built-in function */
static const char* SYN_CLE_BUILTIN_SETOWNER="SETOWNER name";
/** @brief Help message for GETOWNER built-in function */
static const char* HLP_CLE_BUILTIN_GETOWNER="Show current owner setting";
/** @brief Syntax pattern for GETOWNER built-in function */
static const char* SYN_CLE_BUILTIN_GETOWNER="GETOWNER";
/** @brief Help message for SETENV built-in function */
static const char* HLP_CLE_BUILTIN_SETENV  ="Set an environment variable (defined in the config file)";
/** @brief Syntax pattern for SETENV built-in function */
static const char* SYN_CLE_BUILTIN_SETENV  ="SETENV variable=value";
/** @brief Help message for GETENV built-in function */
static const char* HLP_CLE_BUILTIN_GETENV  ="Show the environment variables (defined in the config file)";
/** @brief Syntax pattern for GETENV built-in function */
static const char* SYN_CLE_BUILTIN_GETENV  ="GETENV";
/** @brief Help message for DELENV built-in function */
static const char* HLP_CLE_BUILTIN_DELENV  ="Delete an environment variable (defined in the config file)";
/** @brief Syntax pattern for DELENV built-in function */
static const char* SYN_CLE_BUILTIN_DELENV  ="DELENV variable";
/** @brief Help message for LSTENV built-in function */
static const char* HLP_CLE_BUILTIN_LSTENV  ="List status of all possible usable environment variables";
/** @brief Syntax pattern for LSTENV built-in function */
static const char* SYN_CLE_BUILTIN_LSTENV  ="LSTENV";
/** @brief Help message for HLPENV built-in function */
static const char* HLP_CLE_BUILTIN_HLPENV  ="List help message for all possible usable environment variables";
/** @brief Syntax pattern for HLPENV built-in function */
static const char* SYN_CLE_BUILTIN_HLPENV  ="HLPENV";
/** @brief Help message for TRACE built-in function */
static const char* HLP_CLE_BUILTIN_TRACE   ="Manage trace capabilities";
/** @brief Syntax pattern for TRACE built-in function */
static const char* SYN_CLE_BUILTIN_TRACE   ="TRACE ON | OFF | FILE=filename";
/** @brief Help message for CONFIG built-in function */
static const char* HLP_CLE_BUILTIN_CONFIG  ="Shows or clear all the current configuration settings";
/** @brief Syntax pattern for CONFIG built-in function */
static const char* SYN_CLE_BUILTIN_CONFIG  ="CONFIG [CLEAR]";
/** @brief Help message for GRAMMAR built-in function */
static const char* HLP_CLE_BUILTIN_GRAMMAR ="Shows the grammar for commands and properties";
/** @brief Syntax pattern for GRAMMAR built-in function */
static const char* SYN_CLE_BUILTIN_GRAMMAR ="GRAMMAR";
/** @brief Help message for LEXEMES built-in function */
static const char* HLP_CLE_BUILTIN_LEXEMES ="Shows the regular expressions accepted in a command";
/** @brief Syntax pattern for LEXEMES built-in function */
static const char* SYN_CLE_BUILTIN_LEXEMES ="LEXEMES";
/** @brief Help message for LICENSE built-in function */
static const char* HLP_CLE_BUILTIN_LICENSE ="List license information for the program";
/** @brief Syntax pattern for LICENSE built-in function */
static const char* SYN_CLE_BUILTIN_LICENSE ="LICENSE";
/** @brief Help message for VERSION built-in function */
static const char* HLP_CLE_BUILTIN_VERSION ="List version information for the program";
/** @brief Syntax pattern for VERSION built-in function */
static const char* SYN_CLE_BUILTIN_VERSION ="VERSION";
/** @brief Help message for ABOUT built-in function */
static const char* HLP_CLE_BUILTIN_ABOUT   ="Show information about the program";
/** @brief Syntax pattern for ABOUT built-in function */
static const char* SYN_CLE_BUILTIN_ABOUT   ="ABOUT";
/** @brief Help message for ERRORS built-in function */
static const char* HLP_CLE_BUILTIN_ERRORS  ="Show information about return and reason codes of the program";
/** @brief Syntax pattern for ERRORS built-in function */
static const char* SYN_CLE_BUILTIN_ERRORS  ="ERRORS";

/**
 * @brief Template header text for generated property files
 *
 * This multiline string contains a formatted comment block that explains
 * the syntax and usage of property files. It is automatically inserted
 * at the beginning of property files generated by the GENPROP built-in function.
 * The text provides examples for different data types including numbers,
 * strings, floats, switches, objects, and overlays.
 */
static const char* HLP_CLE_PROPFIL =""
"#------------------------------------------------------------------- #\n"
"#                                                                    #\n"
"# The property file can be used to overwrite the default values      #\n"
"# used if the argument is not defined on the command line.           #\n"
"#                                                                    #\n"
"#------------------------------------------------------------------- #\n"
"#                                                                    #\n"
"# The values is assigned between \"\" conforming to the syntax.        #\n"
"#   Example NUMBER array  : \"4711, 1, 2, 3\"                          #\n"
"#   Example STRING array  : \"'str1' x'303000''str2',a'abc'\"          #\n"
"#   Example FLOAT array   : \"123.34, 1.58, PI\" (PI as constant)      #\n"
"#                                                                    #\n"
"# The last example shows that one can use key words if a list        #\n"
"# of constants is defined for this argument.                         #\n"
"#                                                                    #\n"
"# For a SWITCH one can use the special key words ON/OFF to enable or #\n"
"# disable the SWITCH or define the value as a number                 #\n"
"#   Examples for a SWITCH : \"ON\"/\"OFF\"/\"4711\"                        #\n"
"#                                                                    #\n"
"# For an OBJECT only the special key word INIT can be used to ensure #\n"
"# that this OBJECT will be initialized if it was not set on the      #\n"
"# command line. If INIT is not set, then the elements of the OBJECT  #\n"
"# will be initialized only if you type at least obj() in the command #\n"
"# line. If you want an object to be initialized when you don't use it#\n"
"# on the command line then the special key word must be set.         #\n"
"#   Examples for an OBJECT: \"\"/\"INIT\"                                #\n"
"#                                                                    #\n"
"# For an OVERLAY it is the same logic as for objects but the keyword #\n"
"# of the element to initialize must be used if the overlay is not    #\n"
"# set in the command line.                                           #\n"
"#   Example OVERLAY array : \"LINE TRIANGLE LINE RECTANGLE\"           #\n"
"#   Example for a OVERLAY : \"RECTANGLE\"                              #\n"
"#                                                                    #\n"
"#------------------------------------------------------------------- #\n\n";

/** @} */ /* end of CLE_MESSAGES group */

/*! @cond PRIVATE */
#endif /*INC_CLEMSG_H*/

#ifdef __cplusplus
}
#endif
/*! @endcond */
