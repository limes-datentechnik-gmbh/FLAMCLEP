/**********************************************************************/
/**
 * @file CLEMSG.h
 * @brief messages for <b>C</b>ommand <b>L</b>ine <b>E</b>xecution
 *
 * LIMES Command Line Executor (CLE) in ANSI-C
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

static const char* HLP_CLE_BUILTIN_SYNTAX  ="Provides the syntax for each command";
static const char* SYN_CLE_BUILTIN_SYNTAX  ="SYNTAX [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]]";
static const char* HLP_CLE_BUILTIN_HELP    ="Provides quick help for arguments";
static const char* SYN_CLE_BUILTIN_HELP    ="HELP [command[.path] [DEPTH1 | ... | DEPTH9 | ALL]] [MAN]";
static const char* HLP_CLE_BUILTIN_MANPAGE ="Provides manual pages (detailed help)";
static const char* SYN_CLE_BUILTIN_MANPAGE ="MANPAGE [function | command[.path][=filename]] | [filename]";
static const char* HLP_CLE_BUILTIN_GENDOCU ="Generates auxiliary documentation";
static const char* SYN_CLE_BUILTIN_GENDOCU ="GENDOCU [command[.path]=]filename [NONBR] [SHORT]";
static const char* HLP_CLE_BUILTIN_GENPROP ="Generates a property file";
static const char* SYN_CLE_BUILTIN_GENPROP ="GENPROP [command=]filename";
static const char* HLP_CLE_BUILTIN_SETPROP ="Activate a property file";
static const char* SYN_CLE_BUILTIN_SETPROP ="SETPROP [command=]filename";
static const char* HLP_CLE_BUILTIN_CHGPROP ="Change a property value in the currently active property file";
static const char* SYN_CLE_BUILTIN_CHGPROP ="CHGPROP command [path[=value]]*";
static const char* HLP_CLE_BUILTIN_DELPROP ="Remove a property file from configuration";
static const char* SYN_CLE_BUILTIN_DELPROP ="DELPROP [command]";
static const char* HLP_CLE_BUILTIN_GETPROP ="Show current properties";
static const char* SYN_CLE_BUILTIN_GETPROP ="GETPROP [command[.path] [DEPTH1 | ... | DEPTH9 | DEPALL | DEFALL]]";
static const char* HLP_CLE_BUILTIN_SETOWNER="Defines the current owner";
static const char* SYN_CLE_BUILTIN_SETOWNER="SETOWNER name";
static const char* HLP_CLE_BUILTIN_GETOWNER="Show current owner setting";
static const char* SYN_CLE_BUILTIN_GETOWNER="GETOWNER";
static const char* HLP_CLE_BUILTIN_SETENV  ="Set an environment variable";
static const char* SYN_CLE_BUILTIN_SETENV  ="SETENV variable=value";
static const char* HLP_CLE_BUILTIN_GETENV  ="Show the environment variables";
static const char* SYN_CLE_BUILTIN_GETENV  ="GETENV";
static const char* HLP_CLE_BUILTIN_DELENV  ="Delete an environment variable";
static const char* SYN_CLE_BUILTIN_DELENV  ="DELENV variable";
static const char* HLP_CLE_BUILTIN_TRACE   ="Manage trace capabilities";
static const char* SYN_CLE_BUILTIN_TRACE   ="TRACE ON | OFF | FILE=filename";
static const char* HLP_CLE_BUILTIN_CONFIG  ="Shows or clear all the current configuration settings";
static const char* SYN_CLE_BUILTIN_CONFIG  ="CONFIG [CLEAR]";
static const char* HLP_CLE_BUILTIN_GRAMMAR ="Shows the grammar for commands and properties";
static const char* SYN_CLE_BUILTIN_GRAMMAR ="GRAMMAR";
static const char* HLP_CLE_BUILTIN_LEXEM   ="Shows the regular expressions accepted in a command";
static const char* SYN_CLE_BUILTIN_LEXEM   ="LEXEM";
static const char* HLP_CLE_BUILTIN_LICENSE ="List license information for the program";
static const char* SYN_CLE_BUILTIN_LICENSE ="LICENSE";
static const char* HLP_CLE_BUILTIN_VERSION ="List version information for the program";
static const char* SYN_CLE_BUILTIN_VERSION ="VERSION";
static const char* HLP_CLE_BUILTIN_ABOUT   ="Show information about the program";
static const char* SYN_CLE_BUILTIN_ABOUT   ="ABOUT";
static const char* HLP_CLE_BUILTIN_ERRORS  ="Show information about return and reason codes of the program";
static const char* SYN_CLE_BUILTIN_ERRORS  ="ERRORS";
static const char* HLP_CLE_BUILTIN_HTMLDOC ="Generates the complete HTML documentation into a folder";
static const char* SYN_CLE_BUILTIN_HTMLDOC ="HTMLDOC [path]";

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

