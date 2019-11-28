# CLE/P documentation files

CLE_DOCF = main about  delprop  getowner  grammar  lexem    setowner  syntax     version    config  genprop
CLE_DOCF += clepmain clepmain_consid clepmain_usedenv clepmain_envarmap clepmain_filemap clepmain_keylabmap clepmain_ebcdic
CLE_DOCF += getprop  help   manpage  setprop   trace    gendocu  commands  functions  main_help  main_syntax
CLE_DOCF += appendix_otherclp appendix_lexem  appendix_grammar  appendix_properties appendix_prop_defaults appendix_prop_remaining appendix_version  appendix_about
CLE_DOCF += setenv   getenv delenv   license  chgprop  appendix_returncodes  appendix_reasoncodes errors remaining defaults
CLE_DOCF += glossary colophon

CLE_DOCFILES = $(addsuffix .txt, $(CLE_DOCF))

