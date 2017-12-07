typedef struct Node {
   int		kind, value;
   struct	Node *first, *second, *third, *next;
} node;
typedef node	*tree;
extern char TokName[][12];
tree buildTree (int, tree, tree, tree);
tree buildIntTree (int, int);
void printTree (tree);

