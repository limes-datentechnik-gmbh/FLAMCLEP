/**********************************************************************/
/**
 * @file CLPDEF.h
 * @brief Definitions for <b>C</b>ommand <b>L</b>ine <b>P</b>arsing
 * @author limes datentechnik gmbh
 * @date 27.12.2019
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
 *
 **********************************************************************/

/*! @cond PRIVATE */
#ifdef __cplusplus
   extern "C" {
#endif

#ifndef INC_CLPDEF_H
#define INC_CLPDEF_H

/**********************************************************************/

#ifndef FALSE
#  define FALSE               0
#endif
#ifndef TRUE
#  define TRUE                1
#endif

#ifndef __BUILDNR__
#  define __BUILDNR__         0
#endif
#ifdef __RELEASE__
#  define  __BUILD__          "RELEASE"
#endif
#ifdef __DEBUG__
#  define  __BUILD__          "DEBUG"
#endif
#ifndef __BUILD__
#  define  __BUILD__          "UNKNOWN"
#endif

#ifndef INC_TYPDEF_H
#  include <inttypes.h>
#  ifndef I08
#     define I08                int8_t
#  endif
#  ifndef I16
#     define I16                int16_t
#  endif
#  ifndef I32
#     define I32                int32_t
#  endif
#  ifndef I64
#     define I64                int64_t
#  endif
#  ifndef U08
#     define U08                uint8_t
#  endif
#  ifndef U16
#     define U16                uint16_t
#  endif
#  ifndef U32
#     define U32                uint32_t
#  endif
#  ifndef U64
#     define U64                uint64_t
#  endif
#  ifndef C08
#     define C08                char
#  endif
#  ifndef F32
#     define F32                float
#  endif
#  ifndef F64
#     define F64                double
#  endif
#endif
/*! @endcond */

/**********************************************************************/
/**
 * @defgroup CLP_ERR CLP Error Codes
 * @brief Error codes for command line parsing
 * @{
 */
#define CLP_OK                    0                       /**< @brief Return code for a successful parsing: 0, otherwize > 0. */
#define CLPERR_LEX               -1                       /**< @brief Lexical error (determined by scanner). */
#define CLPERR_SYN               -2                       /**< @brief Syntax error (determined by parser). */
#define CLPERR_SEM               -3                       /**< @brief Semantic error (determined by builder). */
#define CLPERR_TYP               -4                       /**< @brief Type error (internal error with argument types). */
#define CLPERR_TAB               -5                       /**< @brief Table error (internal error with argument tables). */
#define CLPERR_SIZ               -6                       /**< @brief Size error (internal error with argument tables and data structures). */
#define CLPERR_PAR               -7                       /**< @brief Parameter error (internal error with argument tables and data structures). */
#define CLPERR_MEM               -8                       /**< @brief Memory error (internal error with argument tables and data structures). */
#define CLPERR_INT               -9                       /**< @brief Internal error (internal error with argument tables and data structures). */
#define CLPERR_SYS               -10                      /**< @brief System error (internal error with argument tables and data structures). */
#define CLPERR_AUT               -11                      /**< @brief Authorization request failed. */
#define CLPSRC_CMD               ":command line:"         /**< @brief From command line*/
#define CLPSRC_PRO               ":property list:"        /**< @brief From property list*/
#define CLPSRC_DEF               ":default value:"        /**< @brief From default value*/
#define CLPSRC_ENV               ":environment variable:" /**< @brief From environment variable*/
#define CLPSRC_PRF               ":property file:"        /**< @brief From property file*/
#define CLPSRC_CMF               ":command file:"         /**< @brief From command file*/
#define CLPSRC_PAF               ":parameter file:"       /**< @brief Parameter file*/
#define CLPSRC_SRF               ":string file:"          /**< @brief String file*/

/**
 * @brief Defines a structure with error information
 *
 * A pointer to this structure can be provided at siClpOpen() to have access to
 * the error information managed in the CLP handle.
 *
 *  The pointers are set by CLP and valid until CLP is closed.
 */
typedef struct ClpError {
   const char**                  ppMsg;   /**< @brief Points to the pointer of a zero-terminated string containing the current error message. */
 /** Points to the pointer of a zero-terminated string containing the current source.
 * The initial source can be defined for command line or property file parsing.
 * If the initial source is not defined the constant definitions below are used:
 * * for command line parsing    ":command line:"   see CLPSRC_CMD
 * * for property string parsing ":property list:"  see CLPSRC_PRO
 */
   const char**                  ppSrc;   /**< @brief If a parameter file assigned and cause of the error *pcSrc* points to this file name. */
   const int*                    piRow;   /**< @brief Points to an integer containing the current row for the error in *pcSrc*e. */
   const int*                    piCol;   /**< @brief Points to an integer containing the current column for the error in *pcSrc*. */
}TsClpError;
/** @} */

/**
 * @defgroup CLP_TYP CLP Data Types
 * @brief Data types of parameter in the argument table
 * @{
 */
#define CLPTYP_NON               0   /**< @brief No type - Mark the end of an argument table. */
#define CLPTYP_SWITCH            1   /**< @brief Switch (single keyword representing a number (OID)). */
#define CLPTYP_NUMBER            2   /**< @brief Signed or unsigned integer number (8, 16, 32 or 64 bit). */
#define CLPTYP_FLOATN            3   /**< @brief Floating point number (32 or 64 bit). */
#define CLPTYP_STRING            4   /**< @brief String literal (binary (HEX, ASCII, EBCDIC, CHARS) or null-terminated (default)). */
#define CLPTYP_OBJECT            5   /**< @brief Object (KEYWORD(parameter_list)) can contain arbitrary list of other types. */
#define CLPTYP_OVRLAY            6   /**< @brief Overlay (KEYWORD.KEYWORD...) contains one of its list as in a C union. */
#define CLPTYP_XALIAS           -1   /**< @brief For alias definition (used in the corresponding table macro)*/
/** @} */

/**
 * @defgroup CLP_CLS_MTD CLP Close Method
 * @brief Method used to close the CLP (see #vdClpClose()).
 * @{
 */
#define CLPCLS_MTD_ALL           1   /**< @brief Complete close, free anything including the dynamic allocated buffers in the CLP structure. */
#define CLPCLS_MTD_KEP           0   /**< @brief Free anything except the allocated memory in CLP structure and keep the handle open to close it later with method ALL. */
#define CLPCLS_MTD_EXC           2   /**< @brief Free anything including the handle except the allocated memory in the CLP structure, the application must free the dynamic allocated buffers in the CLP structure it self. */
/** @} */

/**
 * @defgroup CLP_PRO_MTD CLP Property Method
 * @brief Method for property printing (see #siClpProperties()).
 * @{
 */
#define CLPPRO_MTD_ALL           0   /**< @brief All properties are printed (manual pages added as comment). */
#define CLPPRO_MTD_SET           1   /**< @brief Only defined properties are printed (no manual pages used). */
#define CLPPRO_MTD_CMT           2   /**< @brief All properties are printed, but not defined properties are line comments . */
#define CLPPRO_MTD_DOC           3   /**< @brief All property only parameter are printed as documentation. */
/** @} */

/**
 * @defgroup CLP_FLG CLP Flags
 * @brief Flags for command line parsing.
 * @{
 */
#define CLPFLG_NON               0x00000000U   /**< @brief To define no special flags. */
#define CLPFLG_ALI               0x00000001U   /**< @brief This parameter is an alias for another argument (set by macros). */
#define CLPFLG_CON               0x00000002U   /**< @brief This parameter is a constant definition (no argument, no link, no alias (set by macros)). */
#define CLPFLG_CMD               0x00000004U   /**< @brief If set the parameter is only used within the command line (command line only). */
#define CLPFLG_PRO               0x00000008U   /**< @brief If set the parameter is only used within the property file (property file only). */
#define CLPFLG_SEL               0x00000010U   /**< @brief If set only the predefined constants over the corresponding key words can be selected (useful to define selections). */
#define CLPFLG_FIX               0x00000020U   /**< @brief This argument has a fixed length (only useful for strings if a typedef defines a fixed length per element, else set internally). */
#define CLPFLG_BIN               0x00000040U   /**< @brief This argument can contain binary data without null termination (length must be known or determined with a link). */
#define CLPFLG_DMY               0x00000080U   /**< @brief If set the parameter is not put in the symbol table, meaning it is only a peace of memory in the CLP structure. */
#define CLPFLG_CNT               0x00000100U   /**< @brief This link will be filled by the calculated amount of elements (useful for arrays). */
#define CLPFLG_OID               0x00000200U   /**< @brief This link will be filled by the object identifier (OID) of the chosen argument (useful for overlays). */
#define CLPFLG_IND               0x00000400U   /**< @brief This link will be filled with the index (position) in the CLP string (byte offset of the current key word). */
#define CLPFLG_HID               0x00000800U   /**< @brief If set the parameter is not visible, meaning it is a hidden parameter. */
#define CLPFLG_ELN               0x00001000U   /**< @brief This link will be filled by the calculated length of an element (fixed types == data size, packed types == data length). */
#define CLPFLG_SLN               0x00002000U   /**< @brief This link will be filled by the calculated string length for an element (only for null-terminated strings). */
#define CLPFLG_TLN               0x00004000U   /**< @brief This link will be filled by the calculated total length for the argument (sum of all element lengths). */
#define CLPFLG_DEF               0x00010000U   /**< @brief This flag enables to use the OID as default for numbers if no value is assigned (only the keyword is used (syntax extension)). */
#define CLPFLG_CHR               0x00020000U   /**< @brief This flag will set the default method of interpretation of a binary string to local character string (DEFAULT). */
#define CLPFLG_ASC               0x00040000U   /**< @brief This flag will set the default method of interpretation of a binary string to ASCII. */
#define CLPFLG_EBC               0x00080000U   /**< @brief This flag will set the default method of interpretation of a binary string to EBCDIC. */
#define CLPFLG_HEX               0x00100000U   /**< @brief This flag will set the default method of interpretation of a binary string to hexadecimal. */
#define CLPFLG_PDF               0x00200000U   /**< @brief This flag will be set if a property value was defined from outside, it will be FALSE if the property value was hard coded in the tables. */
#define CLPFLG_TIM               0x00400000U   /**< @brief This flag mark a number as time value (only used to print out the corressponing time stamp). */
#define CLPFLG_DYN               0x00800000U   /**< @brief This flag mark a string or array as dynamic (only a pointer to allocated memory is used and must be freeed by the user). */
#define CLPFLG_PWD               0x01000000U   /**< @brief This flag will ensure that the clear value is only put into the data structure but not traced, logged or given away elsewhere. */
#define CLPFLG_DLM               0x02000000U   /**< @brief This flag ensures that fix size arrays has a empty (initialized) last element (max-1) as delimiter. Additional you enforce 0xFF at the and of a non fix size string array (size-1). */
#define CLPFLG_UNS               0x04000000U   /**< @brief Marks a number as unsigned (prevent negative values). */
#define CLPFLG_XML               0x08000000U   /**< @brief Marks zero terminated string as XML path where '(' and ')' are used to replace environment variables. */
#define CLPFLG_FIL               0x10000000U   /**< @brief Marks zero terminated string as file and replace additional '~' by HOME and corrects the prefix for different platforms. */
#define CLPFLG_LAB               0x20000000U   /**< @brief Marks zero terminated string as label and replace additional '~' by USER, '^' by OWNER and '!' by ENVID . */
#define CLPFLG_UPP               0x40000000U   /**< @brief Converts zero terminated strings to upper case. */
#define CLPFLG_LOW               0x80000000U   /**< @brief Converts zero terminated strings to lower case. */
/** @} */

/**
 * @defgroup CLP_ARGTAB CLP Argument Table
 * @brief The structure and corresponding macros are used to define a entry for argument table.
 * @{
 */
/**
 * @brief Table structure for arguments
 *
 * To simplify the definition of the corresponding data structures and argument tables it is recommended to use the
 * CLPARGTAB macros defined in CLPMAC.h or for constant definitions the CLPCONTAB macros below. With the CLPMAC.h you
 * can generate the tables or the corresponding data structures, depending if DEFINE_STRUCT defined or not.
 *
 * Example:
 *
 * First you must define the table:
 *
 * @code
 * #define CLPINTFMTRED_TABLE\
 *  CLPARGTAB_SKALAR("BIN"         , stBin , TsClpIntRedBin , 0, 1, CLPTYP_OBJECT, CLPFLG_NON, INTCNV_FORMAT_BIN , asClpIntRedBin , NULL, MAN_INTRED_BIN, "Integer in binary format (two's complement)")\
 *  CLPARGTAB_SKALAR("BCD"         , stBcd , TsClpIntRedBcd , 0, 1, CLPTYP_OBJECT, CLPFLG_NON, INTCNV_FORMAT_BCD , asClpIntRedBcd , NULL, MAN_INTRED_BCD, "Binary coded decimal (BCD) number")\
 *  CLPARGTAB_SKALAR("STR"         , stStr , TsClpIntRedStr , 0, 1, CLPTYP_OBJECT, CLPFLG_NON, INTCNV_FORMAT_STR , asClpIntRedStr , NULL, MAN_INTRED_STR, "String representation of an integer")\
 *  CLPARGTAB_SKALAR("ENUM"        , stEnum, TsClpIntRedSel , 0, 1, CLPTYP_OBJECT, CLPFLG_NON, INTCNV_FORMAT_ENUM, asClpIntRedEnum, NULL, MAN_INTRED_ENUM,"Maps enumeration to an integer")\
 *  CLPARGTAB_CLS
 * @endcode
 *
 *  Then you can use this table to define your structure or union, in our case we create a union:
 *
 * @code
 * #define DEFINE_STRUCT
 * #include "CLPMAC.h"
 *
 * typedef union ClpIntFmtRed{
 *    CLPINTFMTRED_TABLE
 * }TuClpIntFmtRed;
 * @endcode
 *
 *  And you use the same define to allocate the corresponding CLP table:
 *
 * @code
 * #undef DEFINE_STRUCT
 * #include "CLPMAC.h"
 *
 * #undef  STRUCT_NAME
 * #define STRUCT_NAME TuClpIntFmtRed
 * TsClpArgument       asClpIntFmtRed[] = {
 *    CLPINTFMTRED_TABLE
 * };
 * @endcode
 */
typedef struct ClpArgument {
   int                           siTyp;   /**< @brief Data type    Type of this parameter (CLPTYP_xxxxxx). The type will be displayed in context sensitive help messages (TYPE: type_name). */
   const char*                   pcKyw;   /**< @brief Keyword (Parameter name)    Pointer to a null-terminated key word for this parameter (:alpha:[:alnum:|'_']*).  */
   const char*                   pcAli;   /**< @brief Alias (other name for a existing parameter)     Pointer to another key word to define an alias (:alpha:[:alnum:|'_']*). */
   int                           siMin;   /**< @brief Minimum amount of entries for this argument (0-optional n-required). */
   int                           siMax;   /**< @brief Maximum amount of entries for this argument (1-scalar n-array (n=0 unlimited array, n>1 limited array)).  */
   int                           siSiz;   /**< @brief Data size    If fixed size type (switch, number, float, object, overlay) then size of this type else (string) available size in memory.
                                                      String type can be defined as FIX with CLPFLG_FIX but this requires a typedef for this string size.
                                                      For dynamic strings an initial size for the first memory allocation can be defined. */
   int                           siOfs;   /**< @brief Data Offset  Offset of an argument in a structure used for address calculation (the offset of(t,m) instruction is used by the macros for it). */
   int                           siOid;   /**< @brief Object identifier    Unique integer value representing the argument (object identifier, used in overlays or for switches). */
   unsigned int                  uiFlg;   /**< @brief Control Flag     Flag value which can be assigned with CLPFLG_SEL/CON/FIX/CNT/SEN/ELN/TLN/OID/ALI to define different characteristics. */
   struct ClpArgument*           psTab;   /**< @brief Table for next level    Pointer to another parameter table for CLPTYP_OBJECT and CLPTYP_OVRLAY describing these structures, for
                                                      CLPTYP_NUMBER, CLPTYP_FLOATN or CLPTYP_STRING to define selections (constant definitions)*/
   /**
    * @brief Default value
    *
    * Pointer to a zero-terminated string to define the default values assigned if no argument was defined.
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
    *  This value can be override by corresponding environment variable or property definition. */
   const char*                   pcDft;

   /**
    * @brief Manual page
    *
    * Pointer to a zero-terminated string for a detailed description of this argument (in ASCIIDOC format, content
    * behind. DESCRIPTION, mainly simply some paragraphs). Can be a NULL pointer or empty string for constant definition
    * or simple arguments. It is recommended to use a header file with a define for this long string (required for objects
    * and overlays). All occurrences of "&{OWN}" or "&{PGM}" (that all their case variations) are replaced with the current
    * owner or program name, respectively. All other content between "&{" and "}" is ignored (comment).
    * The resulting text is converted on EBCDIC systems). */
   const char*                   pcMan;   /**< @brief  */

   /**
    * @brief Help message
    *
    * Pointer to a zero-terminated string for context sensitive help to this argument. Also used as headline in
    * documentation generation. For this only alnum, blank, dot, comma, hyphen and parenthesis are used. At every other
    * separator the headline will be cut, meaning it is possible to have more help information than head line.
    * (converted on EBCDIC systems). */
   const char*                   pcHlp;

/*! @cond PRIVATE */
   signed long long int          siVal;   /**< @brief for internal use only (must be initialized with 0)*/
   double                        flVal;   /**< @brief for internal use only (must be initialized with 0.0)*/
   const unsigned char*          pcVal;   /**< @brief for internal use only (must be initialized with NULL)*/
   const char*                   pcTyp;   /**< @brief for internal use only (must be initialized with the name of the data type (done by the macros))*/
/*! @endcond */
}TsClpArgument;

/**
 * @brief Starts a table with constant definitions
 *
 * @param[in]  name   Name of this table\n
 */
#define CLPCONTAB_OPN(name)      TsClpArgument name[]

/**
 *
 * @brief Defines a number literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_NUMBER(kyw,dat,man,hlp)       {CLPTYP_NUMBER,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),(dat), 0.0 ,NULL       ,NULL},

/**
 * @brief Defines a floating point literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_FLOATN(kyw,dat,man,hlp)       {CLPTYP_FLOATN,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),  0  ,(dat),NULL       ,NULL},

/**
 * @brief Defines a default string literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_STRING(kyw,dat,man,hlp)       {CLPTYP_STRING,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON           ,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat),NULL},

/**
 * @brief Defines a hexadecimal string literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_HEXSTR(kyw,dat,man,hlp)       {CLPTYP_STRING,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON|CLPFLG_HEX,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat),NULL},

/**
 * @brief Defines a ASCII string literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_ASCSTR(kyw,dat,man,hlp)       {CLPTYP_STRING,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON|CLPFLG_ASC,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat),NULL},

/**
 * @brief Defines a EBCDIC string literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_EBCSTR(kyw,dat,man,hlp)       {CLPTYP_STRING,(kyw),NULL,0,0,  0  ,0,0,CLPFLG_CON|CLPFLG_EBC,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat),NULL},

/**
 * @brief Defines a binary literal with the command line keyword *kyw* and the value *dat*.
 *
 * @param[in]   kyw   Pointer to command line keyword *kyw*.
 * @param[in]   dat   Pointer to target definition in case of reading.
 * @param[in]   siz   Size of the binary value.
 * @param[in]   man   Pointer to a null-terminated string for a detailed description of this constant
 *                    (in ASCIIDOC format, content behind .DESCRIPTION, mainly simply some paragraphs)
 *                    Can be a NULL pointer or empty string to produce a bullet list.
 *                    It is recommended to use a header file with a define for this long string.
 * @param[in]   hlp   Pointer to a null-terminated string for context sensitive help for this constant
 *                    (also used as head line or in bullet list in documentation generation).
 */
#define CLPCONTAB_BINARY(kyw,dat,siz,man,hlp)   {CLPTYP_STRING,(kyw),NULL,0,0,(siz),0,0,CLPFLG_CON|CLPFLG_BIN,NULL,NULL,(man),(hlp),  0  , 0.0 ,(U08*)(dat),NULL},

/** Ends a table with constant definitions
 */
#define CLPCONTAB_CLS                           {CLPTYP_NON   , NULL,NULL,0,0,  0  ,0,0,CLPFLG_NON           ,NULL,NULL, NULL, NULL,  0  , 0.0 ,NULL       ,NULL}
/** @} */

/**********************************************************************/
/**
 * @defgroup CLP_FUNCPTR CLP Function Pointer (call backs)
 * @brief Function prototype definitions for call backs used by CLP
 * @{
 */

/**
 * @brief Type definition for string to file call back function
 *
 * Read a file using the specified filename and reads the whole content
 * into the supplied buffer. The buffer is reallocated and buffer size
 * updated, if necessary.
 *
 * @param  [in]     pvGbl Pointer to to the global handle as black box given with CleExecute
 * @param  [in]     pvHdl Pointer to a handle given for this callback
 * @param  [in]     pcFil File name to read
 * @param  [inout]  ppBuf Pointer to a buffer pointer for reallocation
 * @param  [inout]  piBuf Pointer to the buffer size (updated after reallocation)
 * @param  [out]    pcMsg Pointer to a buffer for the error message
 * @param  [in]     siMsg Size of the message buffer (should be 1024)
 * @return bytes read or negative value if error
 */
typedef int (TfF2S)(
   void*                         pvGbl,
   void*                         pvHdl,
   const char*                   pcFil,
   char**                        ppBuf,
   int*                          piBuf,
   char*                         pcMsg,
   const int                     siMsg);

/**
 * @brief Type definition for resource access check
 *
 * The function is called with the complete path and the standard lexeme as value
 * in front of each wrte of data to the CLP structure.
 *
 * @param  [in]     pvGbl Pointer to to the global handle as black box given with CleExecute
 * @param  [in]     pvHdl Pointer to a handle given for this callback
 * @param  [in]     pcVal Path=Value as resource
 * @return 0 if write allowed else a authorization error
 */
typedef int (TfSaf) (
   void*                         pvGbl,
   void*                         pvHdl,
   const char*                   pcVal);

/**
 * @brief Function 'prnHtmlDoc' of library 'libhtmldoc' called if built-in function HTMLDOC used
 *
 * The built-in function HTMLDOC use a service provider interface to create the documentation
 * using a callback function for each page/chapter. This is this callback function. This interface
 * is used with the function siClpPrintDocu() below. The function is called for each page/chapter
 * and the pointer to the original manual page (pcOrg) can be used to determine if this page the
 * first time printed or if the same page printed again. The HTML documentation can now use an link
 * instead to print the same page again to reduce memory and redundancies in the document. The
 * level can be used to insert Headlines to a dictionary, the path shows the real position and can
 * be used to build files names. The path starts always with CLEP followed by COMMAND or OTHERCLP
 * depending which tables are used. This prefix ensures uniqueness if it used for the file names.
 * prepared page is in ASCIIDOC and must be converted to HTML. In this case the headline must be
 * provided for the dictionary, the index terms for the index and other information can be used
 * to build a very powerful HTML documentation.
 *
 * @param  [inout] pvHdl   Handle for the print callback function (e.G. from opnHtmlDoc)
 * @param  [in]    siLev   The hierarchical level for this page/chapter
 * @param  [in]    pcHdl   The headline of the current chapter
 * @param  [in]    pcPat   The path for the corresponding parameter (NULL if the page not inside a command or other CLP string)
 * @param  [in]    pcFil   Unique hierarchical string (can be used as file name (no extension))
 * @param  [in]    pcOrg   The pointer to the original manual page (can be used to determine duplicates and produce links)
 * @param  [in]    pcPge   The prepared ASCIIDOC page for printing (must be converted to HTML)
 * @return  Return code (0 is OK else error)
 */
typedef int (TfClpPrintPage)(
   void*                         pvHdl,
   const int                     siLev,
   const char*                   pcHdl,
   const char*                   pcPat,
   const char*                   pcFil,
   const char*                   pcOrg,
   const char*                   pcPge);

/** @}*/
/**********************************************************************/

/*! @cond PRIVATE */
#define CLPISF_ALI(flg)          ((flg)&CLPFLG_ALI)
#define CLPISF_CON(flg)          ((flg)&CLPFLG_CON)
#define CLPISF_CMD(flg)          ((flg)&CLPFLG_CMD)
#define CLPISF_PRO(flg)          ((flg)&CLPFLG_PRO)
#define CLPISF_DMY(flg)          ((flg)&CLPFLG_DMY)
#define CLPISF_HID(flg)          ((flg)&CLPFLG_HID)
#define CLPISF_SEL(flg)          ((flg)&CLPFLG_SEL)
#define CLPISF_FIX(flg)          ((flg)&CLPFLG_FIX)
#define CLPISF_BIN(flg)          ((flg)&CLPFLG_BIN)
#define CLPISF_CNT(flg)          ((flg)&CLPFLG_CNT)
#define CLPISF_OID(flg)          ((flg)&CLPFLG_OID)
#define CLPISF_IND(flg)          ((flg)&CLPFLG_IND)
#define CLPISF_ELN(flg)          ((flg)&CLPFLG_ELN)
#define CLPISF_SLN(flg)          ((flg)&CLPFLG_SLN)
#define CLPISF_TLN(flg)          ((flg)&CLPFLG_TLN)
#define CLPISF_DEF(flg)          ((flg)&CLPFLG_DEF)
#define CLPISF_PWD(flg)          ((flg)&CLPFLG_PWD)
#define CLPISF_CHR(flg)          ((flg)&CLPFLG_CHR)
#define CLPISF_ASC(flg)          ((flg)&CLPFLG_ASC)
#define CLPISF_EBC(flg)          ((flg)&CLPFLG_EBC)
#define CLPISF_HEX(flg)          ((flg)&CLPFLG_HEX)
#define CLPISF_PDF(flg)          ((flg)&CLPFLG_PDF)
#define CLPISF_TIM(flg)          ((flg)&CLPFLG_TIM)
#define CLPISF_DYN(flg)          ((flg)&CLPFLG_DYN)
#define CLPISF_DLM(flg)          ((flg)&CLPFLG_DLM)
#define CLPISF_UNS(flg)          ((flg)&CLPFLG_UNS)
#define CLPISF_XML(flg)          ((flg)&CLPFLG_XML)
#define CLPISF_FIL(flg)          ((flg)&CLPFLG_FIL)
#define CLPISF_LAB(flg)          ((flg)&CLPFLG_LAB)
#define CLPISF_UPP(flg)          ((flg)&CLPFLG_UPP)
#define CLPISF_LOW(flg)          ((flg)&CLPFLG_LOW)
#define CLPISF_LNK(flg)          (CLPISF_CNT(flg) ||  CLPISF_OID(flg) || CLPISF_IND(flg) ||  CLPISF_ELN(flg) || CLPISF_SLN(flg) ||  CLPISF_TLN(flg))
#define CLPISF_ARG(flg)          ((!CLPISF_LNK(flg)) && (!CLPISF_CON(flg)) && (!CLPISF_ALI(flg)))
#define CLPISF_ENT(flg)          ((!CLPISF_LNK(flg)) && (!CLPISF_ALI(flg)))

#define CLPSYM_NON               0x00000000U   /**< @brief No operation done*/
#define CLPSYM_ROOT              0x00000001U   /**< @brief Go to symbol table root */
#define CLPSYM_OLD               0x00000002U   /**< @brief Go to at last used symbol */
#define CLPSYM_NEXT              0x00000004U   /**< @brief Go to the next symbol in the list */
#define CLPSYM_BACK              0x00000008U   /**< @brief Go to the previous symbol in the list */
#define CLPSYM_DEP               0x00000010U   /**< @brief Go to the deeper level in the tree */
#define CLPSYM_HIH               0x00000020U   /**< @brief Go to the higher level in the tree */
#define CLPSYM_ALIAS             0x00000100U   /**< @brief Go to the alias symbol */
#define CLPSYM_COUNT             0x00001000U   /**< @brief Go to the counter symbol */
#define CLPSYM_LINK              0x00004000U   /**< @brief Go to the entry link symbol */
#define CLPSYM_OID               0x00008000U   /**< @brief Go to the entry object identifier symbol */
#define CLPSYM_ELN               0x00002000U   /**< @brief Go to the element length symbol */
#define CLPSYM_SLN               0x00010000U   /**< @brief Go to the string length symbol */
#define CLPSYM_TLN               0x00020000U   /**< @brief Go to the total length symbol */

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
#define CLPISS_OID(flg)          ((flg)&CLPSYM_OID )
#define CLPISS_SLN(flg)          ((flg)&CLPSYM_SLN)
#define CLPISS_TLN(flg)          ((flg)&CLPSYM_TLN)

typedef struct ClpSymWlk {
  const char*                   pcKyw;      /**< @brief Pointer to the key word */
  const char*                   pcAli;      /**< @brief Pointer to the alias */
  unsigned int                  uiFlg;      /**< @brief Flag value */
  const char*                   pcDft;      /**< @brief Pointer to the default supplement string */
  const char*                   pcMan;      /**< @brief Pointer to the manual page (description) */
  const char*                   pcHlp;      /**< @brief Pointer to the help message */
  const char*                   pcPat;      /**< @brief Pointer to the path */
  int                           siTyp;      /**< @brief Type of the symbol */
  int                           siMin;      /**< @brief Minimum number of entries */
  int                           siMax;      /**< @brief Maximum number of entries */
  int                           siKwl;      /**< @brief Minimum length of keyword string to make it unique (maximal abbreviation) */
  int                           siSiz;      /**< @brief Size of the symbol */
  int                           siOid;      /**< @brief Object identifier for the symbol */
  unsigned int                  uiOpr;      /**< @brief Bitmask for possible operations */
}TsClpSymWlk;

typedef struct ClpSymUpd {
  const char*                   pcPro;      /**< @brief Pointer to a property supplement string replacing the current default value */
}TsClpSymUpd;

#endif // INC_CLPDEF_H

#ifdef __cplusplus
}
#endif
/*! @endcond */
