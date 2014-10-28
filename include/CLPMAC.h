/******************************************************************************/
/*******************************************************************************/
/**
 * @file CLPMAC.h
 * @brief macros for single definition of C struct and argument table
 *   for <b>C</b>ommand <b>L</b>ine <b>P</b>arsing
 * LIMES Command Line Parser (CLP) in ANSI-C
 * @author Falk Reichbott
 * @date 20.10.2012\n
 * @copyright 2012 limes datentechnik gmbh
 *
 * This file is commonly included 2 times in a C file where the command line arguments
 * of a program are defined. The first inclusion is used to define the C struct types
 * where the parsed argument values will be stored. The second inclusion is used to define
 * a table which describes the available command line arguments.
 * With this method one source of description is used to define the argument table as well
 * as the C struct where the parsed values will be stored.
 *
 * <b>DEFINE_STRUCT</b> acts as a switch to the CLPARGTAB_* macros and determines if they define members of a C struct
 *  or entries of the argument table.\n
 *  If defined the macros define struct members. Otherwise they define entries of the argument table.
 *
 ******************************************************************************/

#undef CLPARGTAB_SKALAR
#undef CLPARGTAB_STRING
#undef CLPARGTAB_ARRAY
#undef CLPARGTAB_ALIAS
#undef CLPARGTAB_CLS

#ifdef DEFINE_STRUCT


/* Makros zum Struktur-Aufbau */
#define CLPARGTAB_SKALAR(kyw,nam,typ,min,max,atyp,flg,oid,tab,dft,man,hlp) typ   nam;
#define CLPARGTAB_STRING(kyw,nam,siz,min,max,atyp,flg,oid,tab,dft,man,hlp) U08   nam[siz];
#define CLPARGTAB_ARRAY( kyw,nam,typ,min,max,atyp,flg,oid,tab,dft,man,hlp) typ   nam[max];
#define CLPARGTAB_ALIAS( kyw,ali                                         )
#define CLPARGTAB_CLS

#else

/* Makros zum Tabellen-Aufbau */
/*                                                                                                                                                          */
/** defines a skalar (single value) with the command line keyword *kyw* and the member name *nam*
 *
 *  *typ* is the C type of the member\n
 *  *atyp* is one of the CLPTYP_* macros.\n
 *  *min* can be 0 or 1 for a skalar and determines if this argument is optional(0) or required(1)\n
 *  *max* is unused for skalar values.\n
 *  *flg* is an OR-ed list of the flag macros CLPFLG_* which define various parsing options.\n
 *  *oid* is a unique id value used for this argument.\n
 *  *tab* is NULL or a pointer to another argument table describing the object, overlay or selection.\n
 *  *dft* is the hard coded default value of the argument if no input is given on the command line.\n
 *  *man* is a pointer to the long description of the argument.\n
 *  *hlp* is a pointer to the short description of the argument.\n
 */
#define CLPARGTAB_SKALAR(kyw,nam,typ,min,max,atyp,flg,oid,tab,dft,man,hlp) { atyp         ,(kyw), NULL,(min),  1  ,sizeof(typ), offsetof(STRUCT_NAME,nam),(oid),(flg)      ,(tab),(dft),(man),(hlp),0,0.0,NULL},
/** defines a string with the command line keyword *kyw* and the member name *nam*
 *
 *  *siz* is the total available length of the string(s).\n
 *  *atyp* is unused and fixed to CLPTYP_STRING.\n
 *  *min* can be between 0 and *max* for a string and determines if this argument is optional(0) or required(min>=1)\n
 *  *max* defines the maximum number of strings accepted on the command line.\n
 *  *flg* is an OR-ed list of the flag macros CLPFLG_* which define various parsing options.\n
 *  *oid* is a unique id value used for this argument.\n
 *  *tab* is NULL or a pointer to another argument table describing a selection.\n
 *  *dft* is the hard coded default value of the argument if no input is given on the command line.\n
 *  *man* is a pointer to the long description of the argument.\n
 *  *hlp* is a pointer to the short description of the argument.\n
 */
#define CLPARGTAB_STRING(kyw,nam,siz,min,max,atyp,flg,oid,tab,dft,man,hlp) { CLPTYP_STRING,(kyw), NULL,(min),(max),      (siz), offsetof(STRUCT_NAME,nam),(oid),(flg)      ,(tab),(dft),(man),(hlp),0,0.0,NULL},
/** defines an array with the command line keyword *kyw* and the member name *nam*
 *
 *  *typ* is the C type of the member\n
 *  *atyp* is one of the CLPTYP_* macros.\n
 *  *min* can be between 0 or *max* and determines if this argument is optional(0) or required(min>=1)\n
 *  *max* defines the size of the array.\n
 *  *flg* is an OR-ed list of the flag macros CLPFLG_* which define various parsing options.\n
 *  *oid* is a unique id value used for this argument.\n
 *  *tab* is NULL or a pointer to another argument table describing the object, overlay or selection.\n
 *  *dft* is the hard coded default value of the argument if no input is given on the command line.\n
 *  *man* is a pointer to the long description of the argument.\n
 *  *hlp* is a pointer to the short description of the argument.\n
 */
#define CLPARGTAB_ARRAY( kyw,nam,typ,min,max,atyp,flg,oid,tab,dft,man,hlp) { atyp         ,(kyw), NULL,(min),(max),sizeof(typ), offsetof(STRUCT_NAME,nam),(oid),(flg)      ,(tab),(dft),(man),(hlp),0,0.0,NULL},
/** defines an alias name for another argument
 *
 *  *kyw* is the alternative keyword accepted on the command line in place of the keyword given in *ali*\n
 */
#define CLPARGTAB_ALIAS( kyw,ali                                         ) { CLPTYP_XALIAS,(kyw),(ali),  0  ,  0  ,        0  ,                       0  ,  0  , CLPFLG_ALI, NULL, NULL, NULL, NULL,0,0.0,NULL},
/**  will mark the end of an argument table.
 */
#define CLPARGTAB_CLS                                                      { CLPTYP_NON   , NULL, NULL,  0  ,  0  ,        0  ,                       0  ,  0  ,  0        , NULL, NULL, NULL, NULL,0,0.0,NULL}

#endif
