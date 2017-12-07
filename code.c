#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "y.tab.h"
#include "instr.h"
#include "code.h"

int LC = 0;

#define MAXLINE 70
static int linelen = 0;

void pr_directive (char *s)
{
  fprintf (outfile, "%s", s);
}

void code (int opcode)
{
   char s[30];
   sprintf (s, "%s ", instrs[opcode].name);
   if ((linelen += strlen (s)) > MAXLINE) {
     linelen = 8; fprintf (outfile, "\n\t");
   }
   fprintf (outfile, "%s", s); LC += 1;
}

void code1 (int opcode, int arg)
{
  char s[30];
  sprintf (s, "%s %d ", instrs[opcode].name, arg);
  if ((linelen += strlen (s)) > MAXLINE) {
    linelen = 8; fprintf (outfile, "\n\t");
  }
  fprintf (outfile, "%s", s); LC += 1 + instrs[opcode].arg_size[0];
}

struct FR codeFR (int opcode)
{
  struct FR ret;
  char s[30];
  sprintf (s, "%s ", instrs[opcode].name);
  if ((linelen += strlen (s) + 5) > MAXLINE) {
    linelen = 8; fprintf (outfile, "\n\t");
  }
  fprintf (outfile, "%s", s);
  ret.LChere = LC += 1 + instrs[opcode].arg_size[0];
  ret.filep = ftell (outfile);
  fprintf (outfile, "%4s ", " ");
  return ret;
}

void fixFR (struct FR fr, int i)
{
  long x = ftell (outfile);
  fseek (outfile, fr.filep, 0);
  fprintf (outfile, "%d", i - fr.LChere);
  fseek (outfile, x, 0);
}

void prLC (void)
{
  fprintf (outfile, "%d:\t", LC);
  linelen = 8;
}

void prNL (void)
{
  fprintf (outfile, "\n");
}

