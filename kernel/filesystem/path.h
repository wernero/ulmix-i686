#ifndef PATH_H
#define PATH_H

struct _fnode;
typedef struct _fnode fnode_t;

fnode_t *get_node(const char *path);

#endif // PATH_H
