#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VERSION "TkRun Version 0.2c 25-nov-00 PJT"
/* note printf error in this code */

				/* tcl/tk startup code */
char *preamble=\
    "#! /usr/local/bin/wish -f\n"
    "# This script has been generated by tkrun,\n"
    "# it is probably not a good idea to edit this file\n"
    "# %s\n"
    "#\n"
    "#\n"
    "set startup $env(HOME)/.tkrunrc\n"
    "if {[file exists $startup]} {source $startup}\n"
    "frame .menu\n"
    "pack .menu\n"
    "\n"
    "button  .menu.run   -text Run  -bg green    -command my_run\n"
    "button  .menu.save  -text Save -bg blue     -command my_save\n"
    "button  .menu.quit  -text Quit -bg red      -command exit\n"
    "button  .menu.help  -text Help -bg yellow   -command my_help\n"
    "\n"
    "pack .menu.run .menu.save .menu.quit -side left  -fill x\n"
    "pack .menu.help -side right -fill x\n"
    "\n"
    "frame .keys\n"
    "pack .keys\n"
    "\n";
				/* remaining tcl/tk code to close loops */
char *postamble=\
    "focus [lindex $keyList 0]\n"
    "\n"
    "foreach w $keyList {\n"
    "    if {1} {\n"
    "       bind $w <Return> {tab $keyList}\n"
    "       $w insert end {}\n"
    "    }\n"
    "}\n"
    "\n"
    "proc tab list {\n"
    "    set i [lsearch -exact $list [focus]]\n"
    "    incr i\n"
    "    if {$i >= [llength $list]} {\n"
    "        set i 0\n"
    "    }\n"
    "    focus [lindex $list $i]\n"
    "}\n"
    "\n"
    "proc my_save {} {\n"
    "  puts \"my_save:\"\n"
    "}\n"
    "proc my_help {lev} {\n"
    "  puts \"my_help:\"\n"
    "}\n"
    "# end of script \n"
    "\n";



typedef char *string;		/* handy definition */

typedef struct simple_widget {	/* our simple widget */
    int type;			/* W_ENTRY, W_SCALE .... */
    char *key;			/* name */
    char *val;			/* default value, if any */
    char *par;			/* parameters for the widget */
    char *opt;			/* additional tk-style options */
    char *help;			/* our one line helpers */
    int  list;			/* 0/1 boolean if widget can focus */
} Widget, *WidgetPtr;

#define MAXW  32

#define W_ENTRY  1
#define W_SCALE  2
#define W_RADIO  3
#define W_CHECK  4
#define W_IFILE 11
#define W_OFILE 12
#define W_MEXEC 21
#define W_PEXEC 22


int parse(FILE *, FILE *, FILE *, int, WidgetPtr);
int merge2(FILE *, FILE *);
string *burststring(string, string);
void freestrings(string *);
void example(void);


int main(int argc, char **argv)
{
    FILE *fp, *tp, *sp;
    Widget widgets[MAXW];
    char key[16], *runpath, *wishpath, *scriptpath, *tkpath, *cp;
    char *scratchpath = "scratchrun.tk";
    char cmd[256];
    int i,nw,noexec;

    fprintf(stderr,"%s\n",VERSION);

    if (argc == 1) {
        fprintf(stderr,"Usage: %s [-options] script\n",argv[0]);
        fprintf(stderr,"  Extracts simple widgets from \"script\", writes a tcl/tk script\n");
        fprintf(stderr,"  and runs this script to execute \"script\"\n");
        exit(0);
    } else {
        runpath = NULL;
        scriptpath = NULL;
        tkpath = NULL;
        wishpath = "wish";
        noexec=0;
    	for (i=1; i<argc; i++) {
    	    cp = argv[i];
    	    if (*cp != '-')
    	        scriptpath = (char *) strdup(cp);
    	    else {
    	        cp++;
    	        switch (*cp) {
    	        case 'r':
                    runpath = (char *) strdup(argv[++i]);
    	            break;
    	        case 'w':
                    wishpath = (char *) strdup(argv[++i]);
    	            break;
    	        case 't':
    	            tkpath = (char *) strdup(argv[++i]);
    	            break;
                case 'x':
                    noexec = 1;
                    break;
                case 'e':
                    example();      /* will exit the program */
                    break;
    	        default:
    	            fprintf(stderr,"Option -%s not supported yet\n",cp);
    	            break;
    	        }
    	    }
    	}
    }
    if (runpath == NULL) runpath = scriptpath;
    if (tkpath == NULL) {
        tkpath = (char *) malloc(strlen(scriptpath)+4);
        sprintf(tkpath,"%s.tk",scriptpath);
    }

    if (strcmp(scriptpath,tkpath)==0) {
        fprintf(stderr,"Error: Run and Script are the same (%s)\n",runpath);
        exit(1);
    }

    fp = fopen(scriptpath,"r");
    if (fp == NULL) {
        fprintf(stderr,"Error: ScriptFile %s could not be opened\n",scriptpath);
        exit(1);
    }

    tp = fopen(tkpath,"w");
    if (tp == NULL) {
        fprintf(stderr,"Error: TkFile %s could not be written\n",tkpath);
        exit(1);
    }

    sp = fopen(scratchpath,"w");
    if (sp == NULL) {
        fprintf(stderr,"Error: ScratchFile %s could not be written\n",scratchpath);
        exit(1);
    }
    
    
    fprintf(tp,preamble,VERSION);
    nw = parse(fp, tp, sp, MAXW, widgets);
    fclose(fp);
    fclose(sp);


    fprintf(tp,"set keyList {");
    for (i=0; i<nw; i++) {
        if (widgets[i].list)
            fprintf(tp," .keys.%s.frame.val",widgets[i].key);
    }
    fprintf(tp," }\n");


    fprintf(tp,"proc my_run {} {\n");
    fprintf(tp,"  puts \"my_run:\"\n");
    for (i=0; i<nw; i++) {
    	sprintf(key,widgets[i].key);
        fprintf(tp,"  global var_%s\n",key);
        fprintf(tp,"  lappend args \"%s=$var_%s\"\n",key,key);
    }
    fprintf(tp,"   eval exec <@stdin >@stdout %s $args\n",runpath);
    fprintf(tp,"}\n");

    

    sp = fopen(scratchpath,"r");
    if (sp == NULL) {
        fprintf(stderr,"Error: ScratchFile %s could not be read\n",scratchpath);
        exit(1);
    }
    (void) merge2(tp, sp);

    fprintf(tp,"%s",postamble);

    fprintf(tp,"# Found %d widgets\n",nw);
    for (i=0; i<nw; i++) {
        fprintf(tp,"# %d: %s=%s\n",widgets[i].type,
                widgets[i].key, widgets[i].val);
    }
    
    
    fclose(tp);

    if (noexec) exit(0);

    sprintf(cmd,"%s -f %s",wishpath,tkpath);
    fprintf(stderr,"Running %s\n",cmd);
    system(cmd);
    return 0;
}

#define MAXLINELEN 256

char *keypdef = "#>";           /* for csh/sh type shells */
char *keycdef = "#<";           /* key continuation marker */

int parse(FILE *fp, FILE *tp, FILE *sp, int maxw, Widget *w)
{
    char *cp, *vp, line[MAXLINELEN];
    char t[64], tframe[64], tkey[64], tval[64], tvar[64], tbut[64];
    string *wp, *ip;
    int i, nip, nw, len, cdeflen, pdeflen;

    cdeflen = strlen(keycdef);
    pdeflen = strlen(keypdef);
    nw = 0;

    fprintf(sp,"# This file is temporary; for merging into main tkrun script\n");

    while (fgets(line,MAXLINELEN,fp) != NULL) {
        len = strlen(line);
        if (line[len-1]=='\n') line[len-1]='\0';
 
        if (strncmp(line,keycdef,cdeflen)==0) {
            cp = strchr(line,'-');
            if (cp) {
                if (nw==0) continue;
                if (w[nw-1].opt) 
                    fprintf(stderr,"Warning: overwriting %s for %s\n",
                        w[nw-1].opt, w[nw-1].key);
                w[nw-1].opt = (char *) strdup(cp);
            }
            continue;
        }

        if (strncmp(line,keypdef,pdeflen)==0) {
            fprintf(tp,"%s\n",line);
            wp = burststring(line," \t");

            if (wp[0]==0 || wp[1]==0) continue;

            if (strcmp(wp[1],"ENTRY")==0) {
                vp = strchr(wp[2],'=');
                if (vp==NULL) {
                    fprintf(tp,"### Warning: missing '=' in keyword %s\n",wp[1]);
                    continue;
                }
                *vp++ = 0;
                w[nw].type = W_ENTRY;
                w[nw].key = (char *) strdup(wp[2]);
                w[nw].val = (char *) strdup(vp);
                w[nw].par = 0;
                w[nw].opt = 0;
                w[nw].help = 0;
                w[nw].list = 1;

                sprintf(t,      ".keys.%s",            w[nw].key);
                sprintf(tframe, ".keys.%s.frame",      w[nw].key);
                sprintf(tkey,   ".keys.%s.key",        w[nw].key);
                sprintf(tval,   ".keys.%s.frame.val",  w[nw].key);
                sprintf(tvar,   "var_%s",              w[nw].key);

                fprintf(tp,"frame %s -borderwidth {2} -relief {raised}\n",t);
                fprintf(tp,"pack %s\n",t);
                fprintf(tp,"frame %s\n",tframe);
                fprintf(tp,"global %s\n",tvar);
                if (*w[nw].val)
                    fprintf(tp,"set %s %s\n",tvar,w[nw].val);

                fprintf(tp,"entry %s -textvariable %s  -relief {sunken}\n", 
                        tval,tvar);
                fprintf(tp,"pack append %s %s {top frame center expand fill}\n",
                        tframe,tval);
                fprintf(tp,"label %s -text %s:\n",tkey,w[nw].key);
                fprintf(tp,"bind %s <1> \"my_help 0 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <2> \"my_help 1 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <3> \"set %s \\\"\\\"; focus %s\"\n",tkey,tvar,tval);

                fprintf(tp,"pack append %s %s %s %s %s\n",
                        t,tkey,  "{left frame center filly}",
                          tframe,"{top frame center fillx}");

                fprintf(tp,"pack append .keys %s %s\n",
                          t, "{top frame center fill}");
                nw++;

            } else if (strcmp(wp[1],"IFILE")==0 || strcmp(wp[1],"OFILE")==0) {
                vp = strchr(wp[2],'=');
                if (vp==NULL) {
                    fprintf(tp,"### Warning: missing '=' in keyword %s\n",wp[1]);
                    continue;
                }
                *vp++ = 0;
                w[nw].type = W_IFILE;
                w[nw].key = (char *) strdup(wp[2]);
                w[nw].val = (char *) strdup(vp);
                w[nw].par = 0;
                w[nw].opt = 0;
                w[nw].help = 0;
                w[nw].list = 0;

                sprintf(t,      ".keys.%s",            w[nw].key);
                sprintf(tframe, ".keys.%s.frame",      w[nw].key);
                sprintf(tkey,   ".keys.%s.key",        w[nw].key);
                sprintf(tval,   ".keys.%s.frame.val",  w[nw].key);
                sprintf(tbut,   ".keys.%s.frame.but",  w[nw].key);
                sprintf(tvar,   "var_%s",              w[nw].key);

                fprintf(tp,"frame %s -borderwidth {2} -relief {raised}\n",t);
                fprintf(tp,"pack %s\n",t);
                fprintf(tp,"frame %s\n",tframe);
                fprintf(tp,"global %s\n",tvar);
                if (*w[nw].val)
                    fprintf(tp,"set %s %s\n",tvar,w[nw].val);

                fprintf(tp,"entry %s -textvariable %s  -relief {sunken}\n", 
                        tval,tvar);
		fprintf(tp,"button %s -text Browse... -command new_%s\n",
			tbut,tvar);
                fprintf(tp,"pack append %s %s %s %s %s\n",
                        tframe,
                        tbut, "{left frame center filly}",
			tval, "{top frame center expand fill}");
                fprintf(tp,"label %s -text %s:\n",tkey,w[nw].key);
                fprintf(tp,"bind %s <1> \"my_help 0 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <2> \"my_help 1 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <3> \"set %s \\\"\\\"; focus %s\"\n",tkey,tvar,tval);

                fprintf(tp,"pack append %s %s %s %s %s\n",
                        t,tkey,  "{left frame center filly}",
                          tframe,"{top frame center fillx}");

                fprintf(tp,"pack append .keys %s %s\n",
                          t, "{top frame center fill}");

                /* write new_ */

                fprintf(sp,"#  For: %s\n",line);
                fprintf(sp,"proc new_%s {} {\n",tvar);
                fprintf(sp,"   global %s\n",tvar);
                if (strcmp(wp[1],"IFILE")==0)
                    fprintf(sp,"   set %s [tk_getOpenFile -parent %s]\n",
                            tvar,tval);
                else if (strcmp(wp[1],"OFILE")==0)
                    fprintf(sp,"   set %s [tk_getSaveFile -parent %s]\n",
                            tvar,tval);
                fprintf(sp,"   focus %s\n",tval);
                fprintf(sp,"}\n");

                nw++;

            } else if (strcmp(wp[1],"SCALE")==0) {
                vp = strchr(wp[2],'=');
                if (vp==NULL) {
                    fprintf(tp,"### Warning: missing '=' in keyword %s\n",wp[1]);
                    continue;
                }
                *vp++ = 0;
                w[nw].type = W_SCALE;
                w[nw].key = (char *) strdup(wp[2]);
                w[nw].val = (char *) strdup(vp);
                w[nw].par = (char *) strdup(wp[3]);
                w[nw].opt = 0;
                w[nw].help = 0;
                w[nw].list = 0;

                sprintf(t,      ".keys.%s",            w[nw].key);
                sprintf(tframe, ".keys.%s.frame",      w[nw].key);
                sprintf(tkey,   ".keys.%s.key",        w[nw].key);
                sprintf(tval,   ".keys.%s.frame.val",  w[nw].key);
                sprintf(tvar,   "var_%s",              w[nw].key);
                if (*w[nw].val)
                    fprintf(tp,"set %s %s\n",tvar,w[nw].val);

                fprintf(tp,"frame %s -borderwidth {2} -relief {raised}\n",t);
                fprintf(tp,"pack %s\n",t);
                fprintf(tp,"frame %s\n",tframe);
                fprintf(tp,"global %s\n",tvar);


                fprintf(tp,"scale %s -variable %s  -relief {sunken}",tval,tvar);
                ip = burststring(wp[3],":,");
                if (ip[0]) {
                    fprintf(tp," -from %s",ip[0]);
                    if (ip[1]) {
                        fprintf(tp," -to %s",ip[1]);
                        if (ip[2]) 
                            fprintf(tp," -resolution %s",ip[2]);
                    }
                }
                fprintf(tp," -length 10c -orient horizontal\n");

                fprintf(tp,"pack append %s %s {top frame center expand fill}\n",
                        tframe,tval);
                fprintf(tp,"label %s -text %s:\n",tkey,w[nw].key);
                fprintf(tp,"bind %s <1> \"my_help 0 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <2> \"my_help 1 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <3> \"set %s %s; focus %s\"\n",tkey,tvar,ip[0],tval);

                fprintf(tp,"pack append %s %s %s %s %s\n",
                        t,tkey,  "{left frame center filly}",
                          tframe,"{top frame center fillx}");

                fprintf(tp,"pack append .keys %s %s\n",
                          t, "{top frame center fill}");
                nw++;

            } else if (strcmp(wp[1],"RADIO")==0) {
                vp = strchr(wp[2],'=');
                if (vp==NULL) {
                    fprintf(tp,"### Warning: missing '=' in keyword %s\n",wp[1]);
                    continue;
                }
                *vp++ = 0;
                w[nw].type = W_RADIO;
                w[nw].key = (char *) strdup(wp[2]);
                w[nw].val = (char *) strdup(vp);
                w[nw].par = (char *) strdup(wp[3]);
                w[nw].opt = 0;
                w[nw].help = 0;
                w[nw].list = 0;

                sprintf(t,      ".keys.%s",            w[nw].key);
                sprintf(tframe, ".keys.%s.frame",      w[nw].key);
                sprintf(tkey,   ".keys.%s.key",        w[nw].key);
                sprintf(tval,   ".keys.%s.frame.val",  w[nw].key);
                sprintf(tvar,   "var_%s",              w[nw].key);

                fprintf(tp,"frame %s -borderwidth {2} -relief {raised}\n",t);
                fprintf(tp,"pack %s\n",t);
                fprintf(tp,"frame %s\n",tframe);
                fprintf(tp,"global %s\n",tvar);
                if (*w[nw].val)
                    fprintf(tp,"set %s %s\n",tvar,w[nw].val);

                ip = burststring(wp[3],",");

		for (i=0; ip[i] != 0; i++) {
	            fprintf(tp,"radiobutton %s.%s -variable %s -text %s -value %s -anchor w \n",
					tframe,ip[i],tvar,ip[i],ip[i]);
                }
                nip = i;
                fprintf(tp,"pack append %s ",tframe);
		for (i=0; ip[i] != 0; i++) {
	            fprintf(tp," %s.%s %s",
                        tframe,ip[i],
                        (i < nip-1 ? "{left frame center filly}" : "{top frame center fillx}"));
                }
                fprintf(tp,"\n");

                fprintf(tp,"label %s -text %s:\n",tkey,w[nw].key);
                fprintf(tp,"bind %s <1> \"my_help 0 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <2> \"my_help 1 $%s\"\n",tkey,tvar);
		fprintf(tp,"# fix bug in next line\n");
                fprintf(tp,"bind %s <3> \"set %s \\\"\\\"\"\n",tkey,tvar,ip[0]);

                fprintf(tp,"pack append %s %s %s %s %s\n",
                        t,tkey,  "{left frame center filly}",
                          tframe,"{top frame center fillx}");

                fprintf(tp,"pack append .keys %s %s\n",
                          t, "{top frame center fill}");
                nw++;

            } else if (strcmp(wp[1],"CHECK")==0) {
                vp = strchr(wp[2],'=');
                if (vp==NULL) {
                    fprintf(tp,"### Warning: missing '=' in keyword %s\n",wp[1]);
                    continue;
                }
                *vp++ = 0;
                w[nw].type = W_CHECK;
                w[nw].key = (char *) strdup(wp[2]);
                w[nw].val = (char *) strdup(vp);
                w[nw].par = (char *) strdup(wp[3]);
                w[nw].opt = 0;
                w[nw].help = 0;
                w[nw].list = 0;

                sprintf(t,      ".keys.%s",            w[nw].key);
                sprintf(tframe, ".keys.%s.frame",      w[nw].key);
                sprintf(tkey,   ".keys.%s.key",        w[nw].key);
                sprintf(tval,   ".keys.%s.frame.val",  w[nw].key);
                sprintf(tvar,   "var_%s",              w[nw].key);

                fprintf(tp,"frame %s -borderwidth {2} -relief {raised}\n",t);
                fprintf(tp,"pack %s\n",t);
                fprintf(tp,"frame %s\n",tframe);
                fprintf(tp,"global %s\n",tvar);


                fprintf(tp,"set %s %s\n",tvar,w[nw].val);
                if (*w[nw].val) {
                    fprintf(tp,"foreach i [split %s \",\"] {\n",w[nw].val);
                    fprintf(tp,"   set %s_$i 1\n",tvar);
                    fprintf(tp,"}\n");
		}

                ip = burststring(wp[3],",");

		for (i=0; ip[i] != 0; i++) {
	            fprintf(tp,"checkbutton %s.%s -variable %s_%s -text %s -anchor w -command new_%s \n",
					tframe,ip[i],tvar,ip[i],ip[i],tvar);
                    fprintf(tp,"set arr_%s(%s_%s) %s\n",tvar,tvar,ip[i],ip[i]);
                    fprintf(tp,"global %s_%s\n",tvar,ip[i]);
                }
                fprintf(tp,"global arr_%s\n",tvar);
                nip = i;

                /* write new_ */

                fprintf(sp,"# For: %s\n",line);
                fprintf(sp,"proc new_%s {} {\n",tvar);
                fprintf(sp,"   global arr_%s %s\n",tvar,tvar);
                fprintf(sp,"   set %s \"\"\n",tvar);
                for (i=0; ip[i] != 0; i++) {
                    fprintf(sp,"   global %s_%s\n",tvar,ip[i]);
                    fprintf(sp,"   if {$%s_%s} { lappend %s $arr_%s(%s_%s)}\n",
                                    tvar,ip[i], tvar, tvar, tvar, ip[i]);
                }
                fprintf(sp,"   set %s [join $%s \",\"]\n",tvar,tvar);
                fprintf(sp,"}\n");

                fprintf(tp,"pack append %s ",tframe);
		for (i=0; ip[i] != 0; i++) {
	            fprintf(tp," %s.%s %s",
                        tframe,ip[i],
                        (i<nip-1 ? "{left frame center filly}" : "{top frame center fillx}"));
                }
                fprintf(tp,"\n");

                fprintf(tp,"label %s -text %s:\n",tkey,w[nw].key);
                fprintf(tp,"bind %s <1> \"my_help 0 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <2> \"my_help 1 $%s\"\n",tkey,tvar);
                fprintf(tp,"bind %s <3> \"",tkey);
                for (i=0; ip[i] != 0; i++)
                    fprintf(tp,"set %s_%s 0;",tvar,ip[i]);
                fprintf(tp,"new_%s\"\n",tvar);

                fprintf(tp,"pack append %s %s %s %s %s\n",
                        t,tkey,  "{left frame center filly}",
                          tframe,"{top frame center fillx}");

                fprintf(tp,"pack append .keys %s %s\n",
                          t, "{top frame center fill}");
                nw++;

            } else {
                fprintf(tp,"### Warning: unknown tag %s\n",wp[1]);
                fprintf(stderr,"### Warning: skipping unknown tag %s\n",wp[1]);
                continue;
            }
        }
    }

    return nw;
}

int merge2(FILE *tp, FILE *sp)
{
    char line[MAXLINELEN];
    int nw=0;

    while (fgets(line,MAXLINELEN,sp) != NULL) {
        fputs(line,tp);
        nw++;
    }
    return nw;
}

/*  burststring (borrowed from NEMO, but using strdup instead of copxstr)
 *  See: $NEMO/src/kernel/core/burststring.c for full version
 *
 * BURSTSTRING: break a string of the form "word1, word2, ..." into
 * separate strings "word1", "word2", ... and return them in an
 * extended-string (ie, NULL-terminated sequence of pointers).
 */

#define MWRD 512	/* max words in list */
#define MSTR 256	/* max chars per word */

string *burststring(string lst, string sep)
{
    string wrdbuf[MWRD], *wp, *bs;
    char strbuf[MSTR], *sp, *lp;
    int i, nwrd = 0;

    wp = wrdbuf;
    sp = strbuf;
    lp = lst;
    do {						/* scan over list */
	if (*lp == 0 || strchr(sep, *lp) != 0) {	/*   is this a sep? */
	    if (sp > strbuf) {				/*     got a word? */
		*sp = 0;
		*wp++ = (string) strdup(strbuf);
                nwrd++;
		if (wp == &wrdbuf[MWRD]) {		/*       no room? */
		    printf("Error: burststring: too many words\n");
	            exit(1);
		}
		sp = strbuf;				/*       for next 1 */
	    }
	} else {					/*   part of word */
	    *sp++ = *lp;				/*     so copy it */
	    if (sp == &strbuf[MSTR]) {			/*     no room? */
		printf("Error: burststring: word too long\n");
                exit(1);
            }
	}
    } while (*lp++ != 0);				/* until list ends */
    *wp = NULL;

    bs = (string *) malloc((nwrd+1)*sizeof(string));
    for (i=0; i<=nwrd; i++)
        bs[i] = wrdbuf[i];
    return bs;
}

void freestrings(string *strptr)        /* free allocated array of strings */
{
    string *s;

    for (s=strptr; *s != NULL; s++)             /* loop over all strings */
        free(*s);                               /* free each one */
    free(strptr);                               /* and free the 'parent' */
}

void example(void)
{
    printf("#! /bin/csh -f\n");
    printf("#\n");
    printf("#   Example template for tkrun\n");
    printf("#\n");
    printf("#\n");
    printf("#>  IFILE   in=\n");
    printf("#>  OFILE   out=\n");
    printf("#>  ENTRY   eps=0.01\n");
    printf("#>  RADIO   mode=gauss              gauss,newton,leibniz\n");
    printf("#>  CHECK   options=mean,sigma      sum,mean,sigma,skewness,kurtosis\n");
    printf("#>  SCALE   n=3.141592              0:10:0.01\n");
    printf("#\n");
    printf("# Here is an example of automatically setting default:\n");
    printf("awk '{if ($1==\"#>\") print \"set\",$3}' $0 > $0.keys; source $0.keys\n");


    exit(0);

}
