struct rST {
  int index;
  int type;
};
typedef struct {
  int index;
  int address;
  int type;
  struct rST fields[100];
} STentry;
struct NodeST {
  STentry  *data;
  struct NodeST *prev;
  struct NodeST *next;
};
void push(struct NodeST**, STentry*);
void pop(struct NodeST**, struct NodeST*);

