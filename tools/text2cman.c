/**
 * @file text2cman.c
 *
 * LIMES Command Line Parser (FLAMCLEP) in ANSI-C
 * @author limes datentechnik gmbh
 * @date 27.02.2013\n
 * @copyright (c) 2013 limes datentechnik gmbh
 * www.flam.de
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
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#ifdef __FL5__
#include "../CINC/FLMVSN.h"
#endif

static const int FALSE = 0;
static const int TRUE = 1;
static int  quiet=0;

struct var_map_entry {
        char* name;
        char* value;
};
static struct var_map_entry vmap[] = {
#ifdef __FL5__
        { "FLM_VSN_STR",   FLM_VSN_STR   },
        { "FLM_VSN_DATE",  FLM_VSN_DATE  },
        { "FLM_VSN_STATE", FLM_VSN_STATE },
#endif
        { NULL, NULL }
};

static char* getVarValue(char* name)
{
        struct var_map_entry* vP = vmap;
        for(; vP->name != NULL; vP++) {
                if (strcmp(vP->name, name) == 0)
                        return vP->value;
        }
        return NULL;
}

int escape_file(char* inputName, FILE* inFile, FILE* outFile, int count, FILE* depFile)
{
        int n,i,k;
        char linebuf[1024];
        char pageName[2048];
        int linecount = count;

        if (NULL == fgets(linebuf, sizeof(linebuf), inFile))
                return 0;

        linecount++;
        n = strlen(linebuf);
        if (n > 2 && strncmp("(1)\n", &(linebuf[n-4]),4) == 0) {
                fprintf(stderr, "%s:%d:1: skip line starting with '(1)'\n", inputName, linecount);
                if (NULL == fgets(linebuf, sizeof(linebuf), inFile))
                        return linecount;

                linecount++;
                n = strlen(linebuf);
                if (strncmp("===", linebuf, 3) != 0)
                        fprintf(stderr, "%s:%d:1: WARNING unexpected start of file\n", inputName, linecount);
                if (NULL == fgets(linebuf, sizeof(linebuf), inFile))
                        return linecount;

                linecount++;
                n = strlen(linebuf);
        }
        while (n <= 2) {
                if (NULL == fgets(linebuf, sizeof(linebuf), inFile))
                        return linecount;

                linecount++;
                n = strlen(linebuf);
                if (n > 2 && linebuf[0] != '\n' && linebuf[0] != '\r')
                        break;
        }
        if (!quiet && linecount > 1) {
                fprintf(stderr, "%s:1:1: skipped %d empty lines.\n", inputName, linecount-1);
        }
        while (1) {
                if (strncmp(linebuf, "#include ", 9) == 0) {
                        int l = 0, c = 0;
                        FILE* incFile;
                        char includeName[1024];
                        int j = 8;
                        while (linebuf[++j] == ' ');
                        if (linebuf[j] != '/') { /* prepend dirname if filename is NOT absolut */
                                char* p2inf = inputName;
                                char* p2f = strrchr(inputName, '/');
                                if (NULL != p2f) {
                                        while (p2inf != p2f)
                                                includeName[c++] = *p2inf++;
                                        includeName[c++] = '/';
                                }
                        }
                        while (linebuf[j] != '\n' && linebuf[j] != 0) {
                                includeName[c++] = linebuf[j++];
                                l++;
                        }
                        includeName[c] = 0;
                        if (l) {
                                incFile = fopen(includeName, "r");
                                if (NULL == incFile) {
                                        fprintf(stderr, "%s:%d:%d: open of include file %s failed.\n%s\n", inputName, linecount, j,
                                                includeName, strerror(errno));
                                } else {
                                        fprintf(depFile, " %s", includeName);
                                        escape_file(includeName, incFile, outFile, 0, depFile);
                                }
                        } else {
                                fprintf(stderr, "%s:%d:%d: missing name of include file\n", inputName, linecount, j);
                        }
                } else {
                        pageName[0] = '"';
                        for (i=0, k=1 ; i < n ; i++) {
                                if (linebuf[i] == '\n') { /* escape literal newline */
                                        pageName[k++] = '\\';
                                        pageName[k++] = 'n';
                                        pageName[k++] = '"';
                                        pageName[k++] = '\n';
                                } else if (linebuf[i] == '\\') { /* escape backslash */
                                        pageName[k++] = '\\';
                                        pageName[k++] = '\\';
                                } else if (linebuf[i] == '"') { /* escape quotation mark */
                                        pageName[k++] = '\\';
                                        pageName[k++] = '"';
                                } else if (linebuf[i] == '$' && linebuf[i+1] == '{') { /* replace or skip variables */
                                        char vName[128];
                                        int ri, ni;
                                        for(ni=0,ri=i+2; linebuf[ri] != '}' ; ni++,ri++) {
                                                vName[ni] = linebuf[ri];
                                                if (ni == 126) {
                                                        vName[127] = 0;
                                                        fprintf(stderr, "name of variable %s... is to long\n", vName);
                                                        exit(-1);
                                                }
                                        }
                                        vName[ni] = 0;
                                        char* vP = getVarValue(vName);
                                        if (vP != NULL) {
                                                for(; *vP != 0 ; vP++) {
                                                        pageName[k++] = *vP;
                                                }
                                                i += ni+3;
                                        } else {
                                                fprintf(stderr, "%s:%d:%d: warning: Unknown variable name %s\n", inputName, linecount, i, vName);
                                                pageName[k++] = '"';
                                                pageName[k++] = ' ';
                                                for (i+=2 ; i < n ; i++) {
                                                        if (linebuf[i] == '}') {
                                                                pageName[k++] = ' ';
                                                                pageName[k++] = '"';
                                                                break;
                                                        }
                                                        pageName[k++] = linebuf[i];
                                                }
                                        }
                                } else {
                                        pageName[k++] = linebuf[i];
                                }
                        }
                        if (pageName[k-1] != '\n') { /* add newline if it is missing at end of line */
                                pageName[k++] = '\\';
                                pageName[k++] = 'n';
                                pageName[k++] = '"';
                                pageName[k++] = '\n';
                        }
                        pageName[k++] = 0;
                        fputs(pageName, outFile);
                }
                if (NULL == fgets(linebuf, sizeof(linebuf), inFile))
                        break;
                linecount++;
                n = strlen(linebuf);
        }
        return linecount;
}

int main(int argc, char* argv[])
{
        char  c;
        int   n, i, j;
        int   singleFiles = TRUE;
        FILE* inFile;
        FILE* outFile = NULL;
        FILE* depFile = NULL;
        char* inputName = NULL;
        char* outputName = NULL;
        char* depName = NULL;
        char linebuf[1024];
        char pageName[2048];
        char stringName[512];
        char* prefix = "";
        int linecount = 0;

        while ((n = getopt(argc, argv, "d:o:p:q")) != -1) {
                switch (n) {
                case 'd':
                        depName = optarg;
                        break;
                case 'o':
                        outputName = optarg;
                        singleFiles = FALSE;
                        break;
                case 'p':
                        prefix = optarg;
                        break;
                case 'q':
                        quiet = 1;
                        break;
                default: /* '?' */
                        fprintf(stderr, "Usage: %s [-q] [-p prefix] [-d dependency-file] [-o output-file] filename[s]\n", argv[0]);
                        exit(EXIT_FAILURE);
                }
        }
        if (singleFiles) {
                outputName = malloc(1024);
                if (NULL == outputName) {
                        perror("malloc");
                        return -1;
                }
        }
        for (j=optind ; j < argc ; j++) {
                linecount = 0;
                inputName = argv[j];
                if (singleFiles) {
                        n = strlen(inputName);
                        for(n--; n >= 0 ; n--)
                                if (inputName[n] == '.')
                                        break;
                        strncpy(outputName, inputName, n);
                        strcpy(&(outputName[n]),".h");
                }
                if (!quiet) {
                        printf("convert %s to header file %s\n", inputName, outputName);
                        printf("\n");
                }
                inFile = fopen(inputName,"r");
                if (NULL == inFile) {
                        fprintf(stderr,"Unable to open input file %s\n", inputName);
                        perror("fopen");
                        exit(1);
                }
                if (NULL == outFile) {
                        outFile = fopen(outputName,"w");
                        if (NULL == outFile) {
                                fprintf(stderr,"Unable to open output file %s\n", outputName);
                                perror("fopen");
                                exit(1);
                        }
                        fputs("/* DO NOT EDIT THIS FILE! IT IS GENERATED BY: text2man.c */\n", outFile);
                }
                if (NULL == depFile && NULL != depName) {
                        depFile = fopen(depName, "w");
                        if (NULL == depFile) {
                                fprintf(stderr,"Unable to open dependency file %s\n", depName);
                                perror("fopen");
                                exit(1);
                        }
                        fprintf(depFile, "%s:", outputName);
                }
                if (NULL != depFile) {
                        fprintf(depFile, " \\\n   %s", inputName);
                }
/* TODO:  protection against multiple inclusion */
                for (c=' ',i=0, n=0; c != 0 ; n++) {
                        switch (inputName[n]) {
                        case '.': c = (n > 1) ? 0 : '_'; break;
                        case ' ': c = '_'; break;
                        case '-': c = '_'; break;
                        case ')': c = '_'; break;
                        default:  c = inputName[n]; break;
                        }
                        stringName[i++] = toupper(c);
                        if (c == '/' || c == '\\') /* skip directory name */
                                i = 0;
                }
                sprintf(pageName, "static const char %s%s[] =\n", prefix, stringName);
                fputs(pageName, outFile);
                linecount = escape_file(inputName, inFile, outFile, linecount, depFile);
                fclose(inFile);
                strcpy(pageName, "#ifdef __SHOW_TEXT_SOURCE\n");
                fputs(pageName, outFile);
                n = (strncmp(inputName, "../", 3) == 0) ? 3 : 0;
                sprintf(pageName, "\"\\000%s\"\n#endif\n", inputName+n);/* skip leading ../ */
                fputs(pageName, outFile);
                strcpy(linebuf, ";\n\n");
                fputs(linebuf, outFile);
                if (singleFiles) {
                        fclose(outFile);
                        outFile = NULL;
                }
        }
        if (!singleFiles)
                fclose(outFile);
        if (NULL != depFile)
                fclose(depFile);
        return 0;
}
