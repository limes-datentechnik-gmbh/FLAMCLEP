# CLE/P documentation files

CLE_DOCF = main     about  delprop  getowner  grammar  lexem    setowner  syntax     version    config  genprop
CLE_DOCF += getprop  help   manpage  setprop   trace    gendocu  commands  functions  main_help  main_syntax
CLE_DOCF += appendix_lexem  appendix_grammar  appendix_properties  appendix_version  appendix_about  glossary
CLE_DOCF += setenv   getenv delenv   license  chgprop  appendix_returncodes  appendix_reasoncodes errors remaining defaults

CLE_DOCFILES = $(addsuffix .txt, $(CLE_DOCF))

