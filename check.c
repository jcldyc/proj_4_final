#include <stdio.h>
#include "tree.h"
#include <ctype.h>
#include "y.tab.h"
#include "ST.h"

extern int top;
struct NodeST* HeadST;
int scopeDepth = 0;



//Printing the symbol table here
//---------------------------------------------------------------------------

void prST(void){
  int i;
 // printf("SYMBOL TABLE\n");
  printf("Index\tIdent\tType\n");
  int counter = 0;
  for (i = 1; i <= top; i++) {
    int t1 = HeadST->data[i].type;
    char t1Str[100]; sprintf(t1Str, "%d", t1);
    char name[100]; sprintf(name, "%s", id_name(i));
    if (!isdigit(name[0]) && HeadST->data[i].index != 0) {
      counter++;
      printf("%d\t%s\t%s\n", HeadST->data[i].index, id_name(i), 
        t1 == Integer ? "Integer" : t1 == RealConst ? "RealConst" : t1 == Boolean ? "Boolean" : t1 == Record ? "Record" : t1Str
      );
      int j;
      for (j = 1; j <= top; j++) {
        int t2 = HeadST->data[i].fields[j].type;
        char t2Str[100]; sprintf(t2Str, "%d", t2);
        if (HeadST->data[i].fields[j].index != 0) {
          printf("%d.%d\t%s.%s\t%s\n", HeadST->data[i].index, HeadST->data[i].fields[j].index, id_name(i), id_name(j),
            t2 == Integer ? "Integer" : t2 == RealConst ? "RealConst" : t2 == Boolean ? "Boolean" : t2 == Record ? "Record" : t2Str
          );
        }
      }
    }
  }
  if (counter == 0) {
    printf("<NO IDENT IN SCOPE>");
  }
  printf("\n");
}









static int check_expr(tree t)
{
  if(t == NULL) {
    fprintf(stderr, "EXPRESSION MISSION\n");
    return NoType;
  }
  switch(t->kind) {
    case Eq : case NotEq :
    case Lt : case Le :
    case Gt : case Ge : {
        int t1 = check_expr(t->first);
        int t2 = check_expr(t->second);
        if ((t1 == Integer || t1 == RealConst) && (t2 == Integer || t2 == RealConst)) {
          return (t->value = Boolean);
        } else {
          fprintf(stderr, "TYPE MISMATCH IN COMPARISON\n");
          return (t->value = NoType);
        }
      }
      break;
    case Slash : {
        int t1 = check_expr(t->first);
        int t2 = check_expr(t->second);
        if ((t1 == Integer || t1 == RealConst) && (t2 == Integer || t2 == RealConst)) {
          return (t->value = RealConst);
        } else {
          fprintf(stderr, "TYPE MISMATCH IN BINARY ARITHMETIC EXPRESSION\n");
          return (t->value = NoType);
        }
      }
      break;
     case Dot : {
        int pos1 = t->first->value;
        int isRecord = 0;
        if (HeadST->data[pos1].index == 0) {
          struct NodeST* tmp = HeadST;
          while (tmp->data[pos1].index == 0) {
            tmp = tmp->next;
            if (tmp == NULL) {
              fprintf(stderr, "RECORD '%s' NOT IN SCOPE\n", id_name(pos1));
              return (t->value = NoType);
            }
          }
          if (tmp->data[pos1].type != Record) {
            fprintf(stderr, "'%s' NOT IN RECORD\n", id_name(pos1));
            return (t->value = NoType);
          }
    int pos2 = t->second->value;
          if (tmp->data[pos1].fields[pos2].index == 0) {
            fprintf(stderr, "'%s' NOT PROPERTY OF RECORD '%s'\n", id_name(pos2), id_name(pos1));
            return (t->value = NoType);
          } else {
            return (t->value = tmp->data[pos1].fields[pos2].type);
          }
        }
        if (HeadST->data[pos1].type == Record) {
          int pos2 = t->second->value;
          if (HeadST->data[pos1].fields[pos2].index == 0) {
            fprintf(stderr, "'%s' NOT PROPERTY OF RECORD '%s'\n", id_name(pos2), id_name(pos1));
            return (t->value = NoType);
          } else {
            return (t->value = HeadST->data[pos1].fields[pos2].type);
          }
        } else {
          fprintf(stderr, "'%s' NOT A RECORD\n", id_name(pos1));
          return (t->value = NoType);
        }
      }
      break; 
     case Plus : case Minus : {
        if(t->second != NULL) {
          int t1 = check_expr(t->first);
          int t2 = check_expr(t->second);
          if (t1 == Integer && t2 == Integer) {
            return (t->value = Integer);
          } else if (t1 == RealConst && t2 == RealConst) {
            return (t->value = RealConst);
          } else if ((t1 == Integer && t2 == RealConst)||(t1 == RealConst && t2 == Integer)) {
            return (t->value = RealConst);
          } else {
            fprintf(stderr, "TYPE  MISMATCH IN ARITHMETIC EXPR\n");
            return (t->value = NoType);
          }
        } else {
          if (check_expr(t->first) == Integer) {
            return (t->value = Integer);
          } else if (check_expr(t->first) == RealConst) {
            return (t->value = RealConst);
          } else {
            fprintf(stderr, "TYPE MISMATCH ON OPERATOR\n");
            return (t->value = NoType);
          }
        }
      }
      break;  
    case Div : {
        int t1 = check_expr(t->first);
        int t2 = check_expr(t->second);
        if ((t1 == Integer || t1 == RealConst) && (t2 == Integer || t2 == RealConst)) {
          return (t->value = Integer);
        } else {
          fprintf(stderr, "TYPE MISMATCH IN BINARY ARITHMETIC EXPRESSION\n");
          return (t->value = NoType);
        }
      }
      break;
    case Star : case Mod : {
        int t1 = check_expr(t->first);
        int t2 = check_expr(t->second);
        if (t1 == Integer && t2 == Integer) {
          return (t->value = Integer);
        } else if (t1 == RealConst && t2 == RealConst) {
          return (t->value = RealConst);
        } else if ((t1 == Integer && t2 == RealConst)||(t1 == RealConst && t2 == Integer)) {
          return (t->value = RealConst);
        } else {
          fprintf(stderr, "TYPE MISMATCH IN BINARY ARITHMETIC EXPRESSION\n");
          return (t->value = NoType);
        }
      }
      break;  
    case And : case Or : 
    case Not : {
        int t1 = check_expr(t->first);
        int t2 = check_expr(t->second);
        if (t1 == Boolean && t2 == Boolean) {
          return (t->value = Boolean);
        } else {
          fprintf(stderr, "NON-BOOLEAN ARGUMENTS IN BOOL COMPARE\n");
          return (t->value = NoType);
        }
      }
      break;
    case Ident : {
        int pos = t->value;
        if (HeadST->data[pos].index == 0) {
          struct NodeST* tmp = HeadST;
          while (tmp->data[pos].index == 0) {
            tmp = tmp->next;
            if (tmp == NULL) {
              fprintf(stderr, "IDENTIFIER: '%s' NOT IN SCOPE\n", id_name(pos));
              return (t->value = NoType);
            }
          }
          return (t->value = tmp->data[pos].type);
        }
        return (t->value = HeadST->data[pos].type);
      }
      break;
    case Integer :
      return (t->value = Integer);
      break;
    case RealConst :
      return (t->value = RealConst);
      break;
	case Boolean :
	  return (t->value = Boolean);
    default : 
      fprintf(stderr, "EXPRESSION OPERATOR INVALID\n");
  }
}

int recordIndex = -1;
void check(tree t)
{
  for (t; t != NULL; t = t->next) {
    switch(t->kind) {
      case Prog : {
          {STentry NewST[100];
          push(&HeadST, NewST);
          int i;
          for (i = 0; i < 100; i++) {
            HeadST->data[i].index = 0;
            HeadST->data[i].address = 0;
            HeadST->data[i].type = NoType;
            int j;
            for (j = 0; j < 100; j++) {
              HeadST->data[i].fields[j].index = 0;
              HeadST->data[i].fields[j].type = NoType;
            }
          }}
          check(t->first);
          printf("\nPROG SCOPE ");
          prST();
        }
        break;
      case Assert :
        if (check_expr(t->first) != Boolean) {
           fprintf(stderr, "NON-BOOLEAN IN CONDITION TO ASSERT\n");
        }
        check(t->second);
        break;
      case If :
        if (check_expr(t->first) != Boolean) {
          fprintf(stderr, "NON-BOOLEAN IN CONDITION TO IF\n");
        }
        check(t->second);
        if (t->third != NULL) {check(t->third);}
        break;  
      case Colon : {
           if (t->first->kind == Var) {
             tree tmp = t->first;
             tmp = tmp->first;
             for(; tmp != NULL; tmp = tmp->next) {
             int pos = tmp->value;
               if (HeadST->data[pos].index == 0) {
                 HeadST->data[pos].index = pos;
                 if (t->second->kind == Int) {
                   HeadST->data[pos].type = Integer;
                 } else if (t->second->kind == Real) {
                   HeadST->data[pos].type = RealConst;
                 } else if (t->second->kind == Boolean) {
                   HeadST->data[pos].type = Boolean;
                 } else if (t->second->kind == Record) {
                   HeadST->data[pos].type = Record;
                   recordIndex = pos;
                   tree tmp2 = t->second;
                   check(tmp2);
                   recordIndex = -1;
                 } else {
                   HeadST->data[pos].type = NoType;
                   fprintf(stderr, "IDENTIFIER'%s' UNRECOGNIZED IN DEC\n", id_name(tmp->value));
                 }
               } else {
                 fprintf(stderr, "IDENT'%s' ALREADY IN SCOPE\n", id_name(tmp->value));
               }
             }
           }
        }
        break;
      case ColonEq : {
          int t1 = check_expr(t->first);
          int t2 = check_expr(t->second);
          if (t1 == t2 || (t1 == Integer && t2 == RealConst) || (t1 == RealConst && t2 == Integer)) {
          
          }
          else {
             fprintf(stderr, "type mismatch\n", id_name(t->first->value));
          }
        }
        break;
      case ElsIf : {
          if (check_expr(t->first) != Boolean) {
            fprintf(stderr, "NON-BOOLEAN IN CONDITION TO ELSE IF\n");
          }
          check(t->second);
        }
        break;
      case Then : {
          scopeDepth++;
          {STentry NewST[100];
          push(&HeadST, NewST);
          int i;
          for (i = 0; i < 100; i++) {
            HeadST->data[i].index = 0;
            HeadST->data[i].address = 0;
            HeadST->data[i].type = NoType;
            int j;
            for (j = 0; j < 100; j++) {
              HeadST->data[i].fields[j].index = 0;
              HeadST->data[i].fields[j].type = NoType;
            }
          }}
          check(t->first);
          printf("\n----------THEN SCOPE (level: %d )---------- ", scopeDepth);
          prST();
          pop(&HeadST, HeadST);
          scopeDepth--;
        }
        break;
      case Loop : {
          scopeDepth++;
          {STentry NewST[100];
          push(&HeadST, NewST);
          int i;
          for (i = 0; i < 100; i++) {
            HeadST->data[i].index = 0;
            HeadST->data[i].address = 0;
            HeadST->data[i].type = NoType;
      int j;
            for (j = 0; j < 100; j++) {
              HeadST->data[i].fields[j].index = 0;
              HeadST->data[i].fields[j].type = NoType;
            }
          }}
          check(t->first);
          printf("\n----------LOOP SCOPE (level: %d---------- ) ", scopeDepth);
          prST();
          pop(&HeadST, HeadST);
          scopeDepth--;
        }
        break;  
      case Record : {
          tree tmp = t->first;
          for (; tmp != NULL; tmp = tmp->next) {
            tree tmp2 = tmp->first;
            for (; tmp2 != NULL; tmp2 = tmp2->next) {
              int pos = tmp2->value;
              if (HeadST->data[recordIndex].fields[pos].index == 0) {
                HeadST->data[recordIndex].fields[pos].index = pos;
                if (tmp->second->kind == Int) {
                  HeadST->data[recordIndex].fields[pos].type = Integer;
                } else if (tmp->second->kind == Real) {
                  HeadST->data[recordIndex].fields[pos].type = RealConst;
                } else if (tmp->second->kind == Boolean) {
                  HeadST->data[recordIndex].fields[pos].type = Boolean;
                } else if (tmp->second->kind == Record) {
                  HeadST->data[recordIndex].fields[pos].type = NoType;
                  fprintf(stderr, "RECORDS IN RECORDS NOT SUPPORTED\n");
                } else {
                  HeadST->data[recordIndex].fields[pos].type = NoType;
                  fprintf(stderr, "UNRECOGNIZED TYPE TO IDENT'%s' FOR RECORD'%s'\n", id_name(tmp->value), id_name(recordIndex));
                }
              } else {
                fprintf(stderr, "IDENTIFIER '%s'  IN '%s' ALREADY IN SCOPE\n", id_name(tmp->value), id_name(recordIndex));
              }
            }
          }
        }
        break;
      case Else : {
          scopeDepth++;
          {STentry NewST[100];
          push(&HeadST, NewST);
          int i;
          for (i = 0; i < 100; i++) {
            HeadST->data[i].index = 0;
            HeadST->data[i].address = 0;
            HeadST->data[i].type = NoType;
            int j;
            for (j = 0; j < 100; j++) {
              HeadST->data[i].fields[j].index = 0;
              HeadST->data[i].fields[j].type = NoType;
            }
          }}
          check(t->first);
          printf("\n----------ELSE SCOPE (level:%d )---------- ", scopeDepth);
          prST();
          pop(&HeadST, HeadST);
          scopeDepth--;
        }
        break;
      case Begin : {
          scopeDepth++;
          {STentry NewST[100];
          push(&HeadST, NewST);
          int i;
          for (i = 0; i < 100; i++) {
            HeadST->data[i].index = 0;
            HeadST->data[i].address = 0;
            HeadST->data[i].type = NoType;
            int j;
            for (j = 0; j < 100; j++) {
              HeadST->data[i].fields[j].index = 0;
              HeadST->data[i].fields[j].type = NoType;
            }
          }}
          check(t->first);
          printf("\n----------BEGIN SCOPE (level: %d )---------- ", scopeDepth);
          prST();
          pop(&HeadST, HeadST);
          scopeDepth--;
        }
        break;
      case Exit :
        if (t->first->kind == When) {
          tree tmp = t->first;
          if (check_expr(tmp->first) != Boolean) {
            fprintf(stderr, "NON-BOOLEAN IN CONDITION TO EXIT WHEN BLOCK\n");
          }
        }
        break;
      default :
        fprintf(stderr, "STATEMENT INVALID\n");
    }
  }
}

