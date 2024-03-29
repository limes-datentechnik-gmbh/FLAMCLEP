# CLE/P documentation files

CLE_DOCF = main main_consid main_usedenv main_envarmap main_filemap main_keylabmap main_ebcdic
CLE_DOCF += program_synopsis program_help program_syntax program_commands builtin_functions
CLE_DOCF += builtin_version builtin_about builtin_license builtin_grammar builtin_lexemes builtin_syntax  
CLE_DOCF += builtin_setenv builtin_getenv builtin_delenv builtin_lstenv builtin_hlpenv builtin_getowner builtin_setowner builtin_errors
CLE_DOCF += builtin_getprop builtin_genprop builtin_setprop builtin_chgprop builtin_delprop
CLE_DOCF += builtin_help builtin_manpage builtin_trace builtin_gendocu builtin_config builtin_htmldoc
CLE_DOCF += appendix_lexemes appendix_grammar appendix_properties appendix_prop_defaults appendix_prop_remaining
CLE_DOCF += appendix_otherclp appendix_version appendix_about appendix_returncodes appendix_reasoncodes
CLE_DOCF += glossary colophon

CLE_DOCFILES = $(addsuffix .txt, $(CLE_DOCF))

