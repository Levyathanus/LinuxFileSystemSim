#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

/* base NODE struct */
typedef struct node {
  char name[64];           // name of the node
  char type;               // node type: 'D' for directory or 'F' for file
  struct node *child_ptr;   // points to child node
  struct node *sibling_ptr; // points to sibling node
  struct node *parent_ptr;  // points to parent node
}NODE;

/* RECORD struct for utility */
typedef struct record {
  char name[64];
  struct record *next;
  struct record *prev;
}RECORD;

/* Global variables section */
NODE *root, *cwd; // root and CWD (Current Working Directory) pointers
char line[128];   // user input command line
char command[16], argument[64]; // command and argument strings
char dname[64], bname[64];      // dirname and basename string holders
char prompt_text[32];           // command prompt text
unsigned long node_number;      // number of nodes in the file system tree
char* cmd[] = {"mkdir","rmdir","ls","cd","pwd","create","rm","reload","save",
	       "menu","prompt","quit",NULL}; // possible commands

/* <command> /a/b/c -> pathname = /a/b/c
                    -> dname = /a/b, bname = c
		    -> '/' is the root node. */

/* initialize function: initialize root of the file system tree */
int initialize() {
  strcpy(line, "");
  strcpy(command, "");
  strcpy(argument, "");
  strcpy(dname, "");
  strcpy(bname, "");
  strcpy(prompt_text, "Input a command line >");
  node_number = 1;
  root = (NODE*)malloc(sizeof(NODE));
  strcpy(root->name, "/");
  root->type = 'D';
  root->parent_ptr = root;
  root->sibling_ptr = root;
  root->child_ptr = NULL;
  cwd = root;
  return 0;
}

/* reset function: reset global string variables */
int reset() {
  strcpy(line, "");
  strcpy(command, "");
  strcpy(argument, "");
  strcpy(dname, "");
  strcpy(bname, "");

  return 0;
}

/* find_cmd function: return the cmd index if the user typed a valid cmd */
int find_cmd(char* command) {
  int i = 0;
  while(cmd[i]) {
    if(strcmp(command, cmd[i]) == 0) {
      return i; // found command: return index i
    }
    i++;
  }
  return -1; // command not found
}

/* dbname function: separate pathname string into dname and bname */
int dbname(char* pathname) {
  char temp[128]; // dirname() and basename() destroy original pathname 
  strcpy(temp, pathname);
  strcpy(dname, dirname(temp));
  strcpy(temp, pathname);
  strcpy(bname, basename(temp));

  return 0;
}

/* is_empty function: return 1 if the directory node is empty, 0 otherwise */
int is_empty(NODE* dir) {
  if(dir->type == 'F') {
    return -1;
  }
  return ((dir->child_ptr == NULL) ? 1 : 0);
}

/* pwd function: command pwd (print working directory) */
void pwd() {
  NODE* p = cwd;
  RECORD* slider_a = (RECORD*)malloc(sizeof(RECORD));
  RECORD* path = slider_a;
  strcpy(slider_a->name, p->name);
  path->prev = NULL;
  path->next = NULL;
  RECORD* slider_b;
  printf("/");
  while(p != root) {
    slider_b = (RECORD*)malloc(sizeof(RECORD));
    strcpy(slider_b->name, p->name);
    slider_a->next = slider_b;
    slider_b->prev = slider_a;
    slider_b->next = NULL;
    slider_a = slider_a->next;
    slider_b = slider_b->next;
    p = p->parent_ptr;
  }
  while(path->next != NULL) {
    path = path->next;
  }
  while(path->prev != NULL) {
    if(path->prev->prev == NULL)
      printf("%s", path->name);
    else
      printf("%s/", path->name);
    path = path->prev;
  }
  printf("\n");
}

/* mkdir function: command mkdir (make (new) directory) */
int mkdir(char* pathname) {
  NODE* dir = NULL;
  NODE* p = NULL;
  char* path = NULL;
  char temp[128];
  
  if(pathname) {
    dbname(pathname);
    if(strcmp(bname, ".") == 0) {
      reset();
      return -1;
    }
    
    if(strcmp(dname, ".") == 0) { // no dname specified
      dir = (NODE*)malloc(sizeof(NODE));
      strcpy(dir->name, bname);
      dir->type = 'D';
      dir->child_ptr = NULL;
      dir->sibling_ptr = NULL;
      dir->parent_ptr = cwd;
      if(is_empty(cwd)) {
	printf("cwd was empty\n");
	cwd->child_ptr = dir;
      } else {
	p = cwd->child_ptr;
	if(strcmp(p->name, bname) == 0) {
	  printf("File or directory already exists!\n");
	  reset();
	  return -1;
	}
	while(p->sibling_ptr != NULL) {
	  if(strcmp(p->sibling_ptr->name, bname) == 0) {
	    printf("File or directory already exists!\n");
	    reset();
	    return -1;
	  }
	  p = p->sibling_ptr;
	}
	p->sibling_ptr = dir;
      }
    } else {
      if(dname[0] == '/') { // absolute pathname
	if(strcmp(dname, "/") == 0) {
	  if(is_empty(root)) {
	    printf("root was empty\n");
	    dir = (NODE*)malloc(sizeof(NODE));
	    strcpy(dir->name, bname);
	    dir->type = 'D';
	    dir->child_ptr = NULL;
	    dir->sibling_ptr = NULL;
	    dir->parent_ptr = root;
	    root->child_ptr = dir;
	    node_number++;
	    reset();
	    return 0;
	  } else {
	    p = root->child_ptr;
	    dir = (NODE*)malloc(sizeof(NODE));
	    strcpy(dir->name, bname);
	    dir->type = 'D';
	    dir->child_ptr = NULL;
	    dir->sibling_ptr = NULL;
	    dir->parent_ptr = root;
	    if(strcmp(p->name, bname) == 0) {
	      printf("File or directory already exists!\n");
	      reset();
	      return -1;
	    }
	    while(p->sibling_ptr != NULL) {
	      if(strcmp(p->sibling_ptr->name, bname) == 0) {
	        printf("File or directory already exists!\n");
	        reset();
	        return -1;
	      }
	      p = p->sibling_ptr;
	    }
	    p->sibling_ptr = dir;  
	  }
	  node_number++;
	  reset();
	  return 0;
	}
	p = root->child_ptr;
        strcpy(temp, dname);
	path = strtok(dname, "/");
	if(strcmp(temp, path) == 0) {
          path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = strtok(NULL, "/");
	  if(p->child_ptr != NULL && path != NULL) {
	    p = p->child_ptr;
	  }
        }
	// now p is the last directory of the dname
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	dir = (NODE*)malloc(sizeof(NODE));
	strcpy(dir->name, bname);
        dir->type = 'D';
        dir->child_ptr = NULL;
        dir->sibling_ptr = NULL;
        dir->parent_ptr = p;
	if(is_empty(p)) {
	  p->child_ptr = dir;
        } else {
	  p = p->child_ptr;
	  if(strcmp(p->name, bname) == 0) {
	    printf("File or directory already exists!\n");
	    reset();
	    return -1;
	  }
	  while(p->sibling_ptr != NULL) {
	    if(strcmp(p->sibling_ptr->name, bname) == 0) {
	      printf("File or directory already exists!\n");
	      reset();
	      return -1;
	    }
	    p = p->sibling_ptr;
	  }
	  p->sibling_ptr = dir;
        }
      } else { // relative pathname
	if(is_empty(cwd)) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	p = cwd->child_ptr;
        strcpy(temp, dname);
	path = strtok(dname, "/");
	if(strcmp(temp, path) == 0) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
      	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  if(p->child_ptr != NULL) {
	    p = p->child_ptr;
	  }
	  path = strtok(NULL, "/");
        }
	// now p is the last directory of the dname
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	dir = (NODE*)malloc(sizeof(NODE));
	strcpy(dir->name, bname);
        dir->type = 'D';
        dir->child_ptr = NULL;
        dir->sibling_ptr = NULL;
        dir->parent_ptr = p;
	if(is_empty(p)) {
	  p->child_ptr = dir;
        } else {
	  p = p->child_ptr;
	  if(strcmp(p->name, bname) == 0) {
	    printf("File or directory already exists!\n");
	    reset();
	    return -1;
	  }
	  while(p->sibling_ptr != NULL) {
	    if(strcmp(p->sibling_ptr->name, bname) == 0) {
	      printf("File or directory already exists!\n");
	      reset();
	      return -1;
	    }
	    p = p->sibling_ptr;
	  }
	  p->sibling_ptr = dir;
        }
      }
    }
    node_number++;
    reset();
    return 0;
  }
  reset();
  return -1;
}

/* rmdir function: command rmdir (remove (empty) directory) */
int rmdir(char* pathname) {
  NODE* p = NULL;
  NODE* target = NULL;
  char* path = NULL;
  char temp[128];
  
  if(strcmp(pathname, "") == 0) { // no pathname specified
    printf("Invalid path!\n");
    reset();
    return -1;
  } else {
    if(pathname[0] == '/') { // absolute pathname
      if(strcmp(pathname, "/") == 0) {
	printf("Cannot delete root!\n");
	reset();
	return -1;
      } else {
	if(is_empty(root)) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	p = root->child_ptr;
	strcpy(temp, pathname);
	path = strtok(pathname, "/");
	if(strcmp(temp, path) == 0) {
          path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = strtok(NULL, "/");
	  if(p->child_ptr != NULL && path != NULL) {
	    p = p->child_ptr;
	  }
        }
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	// p is now pointing to the last pathname directory
	target = p;
	p = p->parent_ptr;
	if(!is_empty(target)) {
	  printf("Directory not empty!\n");
	  reset();
	  return -1;
	} else {
	  if(p->child_ptr == target) { // target directory is the child
	    if(target->sibling_ptr != NULL) {
	      p->child_ptr = target->sibling_ptr;
	    } else {
	      p->child_ptr = NULL;
	    }
	  } else { // target directory is a sibling
	    p = p->child_ptr;
	    while(p->sibling_ptr != target) {
	      p = p->sibling_ptr;
	    }
	    p->sibling_ptr = target->sibling_ptr;
	    target->sibling_ptr = NULL;
	    target->parent_ptr = NULL;
	  }
	  free(target);
	}
      }
    } else { // relative pathname
      if(is_empty(cwd)) {
	printf("Invalid path!\n");
	reset();
	return -1;
      }
      p = cwd->child_ptr;
      strcpy(temp, pathname);
      path = strtok(pathname, "/");
      if(strcmp(temp, path) == 0) {
	while(p != NULL && strcmp(p->name, path) != 0) {
      	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = NULL;
      }
      while(path != NULL) {
	while(p != NULL && strcmp(p->name, path) != 0) {
	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = strtok(NULL, "/");
	if(p->child_ptr != NULL && path != NULL) {
	  p = p->child_ptr;
	}
      }
      if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
      }
      // p is now pointing to the last pathname directory
      target = p;
      p = p->parent_ptr;
      if(!is_empty(target)) {
	printf("Directory not empty!\n");
	reset();
	return -1;
      } else {
	if(p->child_ptr == target) { // target directory is the child
          if(target->sibling_ptr != NULL) {
	    p->child_ptr = target->sibling_ptr;
	  } else {
	    p->child_ptr = NULL;
	  }
	} else { // target directory is a sibling
	  p = p->child_ptr;
	  while(p->sibling_ptr != target) {
	    p = p->sibling_ptr;
	  }
	  p->sibling_ptr = target->sibling_ptr;
	  target->sibling_ptr = NULL;
	  target->parent_ptr = NULL;
	}
        free(target);
      }
    }   
  }
  printf("Done!\n");
  node_number--;
  reset();
  return 0;
}

/* cd function: command cd (change (current) directory) */
int cd(char* pathname) {
  NODE* p = NULL;
  char* path = NULL;
  char temp[128];
  
  if(strcmp(pathname, "") == 0 || strcmp(pathname, ".") == 0) { // fake cd
    reset();
    return 0;
  } else if(strcmp(pathname, "..") == 0) { // cd to parent
    if(cwd == root) {
      printf("Invalid path!\n");
      reset();
      return -1;
    }
    cwd = cwd->parent_ptr;
  } else { // other cases
    if(pathname[0] == '/') { // absolute pathname
      if(strcmp(pathname, "/") == 0) {
	cwd = root;
	reset();
	return 0;
      } else {
	if(is_empty(root)) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	p = root->child_ptr;
	strcpy(temp, pathname);
	path = strtok(pathname, "/");
	if(strcmp(temp, path) == 0) {
          path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = strtok(NULL, "/");
	  if(p->child_ptr != NULL && path != NULL) {
	    p = p->child_ptr;
	  }
        }
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	cwd = p;
      }
    } else { // relative pathname
      if(is_empty(cwd)) {
	printf("Invalid path!\n");
	reset();
	return -1;
      }
      p = cwd->child_ptr;
      strcpy(temp, pathname);
      path = strtok(pathname, "/");
      if(strcmp(temp, path) == 0) {
	while(p != NULL && strcmp(p->name, path) != 0) {
      	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = NULL;
      }
      while(path != NULL) {
	while(p != NULL && strcmp(p->name, path) != 0) {
	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = strtok(NULL, "/");
	if(p->child_ptr != NULL && path != NULL) {
	  p = p->child_ptr;
	}
      }
      if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
      }
      cwd = p;
    }   
  }
  printf("cwd = ");
  pwd();
  reset();
  return 0;
}

/* ls function: command ls (list) */
int ls(char* pathname) {
  NODE* p = NULL;
  char* path = NULL;
  char temp[128];
  
  if(strcmp(pathname, "") == 0) { // no pathname specified
    if(!is_empty(cwd)) {
      p = cwd->child_ptr;
      while(p != NULL) {
	printf("[%c %s]\t", p->type, p->name);
	p = p->sibling_ptr;
      }
    }
  } else {
    if(pathname[0] == '/') { // absolute pathname
      if(strcmp(pathname, "/") == 0) {
	if(!is_empty(root)) {
	  p = root->child_ptr;
	  while(p != NULL) {
	    printf("[%c %s]\t", p->type, p->name);
	    p = p->sibling_ptr;
	  }  
	}
      } else {
	if(is_empty(root)) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	p = root->child_ptr;
	strcpy(temp, pathname);
	path = strtok(pathname, "/");
	if(strcmp(temp, path) == 0) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
      	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = strtok(NULL, "/");
	  if(p->child_ptr != NULL && path != NULL) {
	    p = p->child_ptr;
	  }
        }
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	if(!is_empty(p)) {
	  p = p->child_ptr;
	  while(p != NULL) {
	    printf("[%c %s]\t", p->type, p->name);
	    p = p->sibling_ptr;
	  }
	}
      }
    } else { // relative pathname
      if(is_empty(cwd)) {
	printf("Invalid path!\n");
	reset();
	return -1;
      }
      p = cwd->child_ptr;
      strcpy(temp, pathname);
      path = strtok(pathname, "/");
      if(strcmp(temp, path) == 0) {
	while(p != NULL && strcmp(p->name, path) != 0) {
      	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = NULL;
      }
      while(path != NULL) {	
	while(p != NULL && strcmp(p->name, path) != 0) {
	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = strtok(NULL, "/");
	if(p->child_ptr != NULL && path != NULL) {
	  p = p->child_ptr;
	}
      }
      if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
      }
      if(!is_empty(p)) {
        p = p->child_ptr;
	while(p != NULL) {
	  printf("[%c %s]\t", p->type, p->name);
	  p = p->sibling_ptr;
	}
      }
    }   
  }
  printf("\n");
  reset();
  return 0;
}

/* create function: command create (create file) */
int create(char* pathname) {
  NODE* file = NULL;
  NODE* p = NULL;
  char* path = NULL;
  char temp[128];
  
  if(pathname) {
    dbname(pathname);
    if(strcmp(bname, ".") == 0) {
      reset();
      return -1;
    }
    
    if(strcmp(dname, ".") == 0) { // no dname specified
      file = (NODE*)malloc(sizeof(NODE));
      strcpy(file->name, bname);
      file->type = 'F';
      file->child_ptr = NULL;
      file->sibling_ptr = NULL;
      file->parent_ptr = cwd;
      if(is_empty(cwd)) {
	printf("cwd was empty\n");
	cwd->child_ptr = file;
      } else {
	p = cwd->child_ptr;
	if(strcmp(p->name, bname) == 0) {
	  printf("File or directory already exists!\n");
	  reset();
	  return -1;
	}
	while(p->sibling_ptr != NULL) {
	  if(strcmp(p->sibling_ptr->name, bname) == 0) {
	    printf("File or directory already exists!\n");
	    reset();
	    return -1;
	  }
	  p = p->sibling_ptr;
	}
	p->sibling_ptr = file;
      }
    } else {
      if(dname[0] == '/') { // absolute pathname
	if(strcmp(dname, "/") == 0) {
	  if(is_empty(root)) {
	    printf("root was empty\n");
	    file = (NODE*)malloc(sizeof(NODE));
	    strcpy(file->name, bname);
	    file->type = 'F';
	    file->child_ptr = NULL;
	    file->sibling_ptr = NULL;
	    file->parent_ptr = root;
	    root->child_ptr = file;
	    node_number++;
	    reset();
	    return 0;
	  } else {
	    p = root->child_ptr;
	    file = (NODE*)malloc(sizeof(NODE));
	    strcpy(file->name, bname);
	    file->type = 'F';
	    file->child_ptr = NULL;
	    file->sibling_ptr = NULL;
	    file->parent_ptr = root;
	    if(strcmp(p->name, bname) == 0) {
	      printf("File or directory already exists!\n");
	      reset();
	      return -1;
	    }
	    while(p->sibling_ptr != NULL) {
	      if(strcmp(p->sibling_ptr->name, bname) == 0) {
	        printf("File or directory already exists!\n");
	        reset();
	        return -1;
	      }
	      p = p->sibling_ptr;
	    }
	    p->sibling_ptr = file;  
	    node_number++;
	    reset();
	    return 0;
	  }
	}

	p = root->child_ptr;
        strcpy(temp, dname);
	path = strtok(dname, "/");
	if(strcmp(temp, path) == 0) {
	  path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = strtok(NULL, "/");
	  if(p->child_ptr != NULL && path != NULL) {
	    p = p->child_ptr;
	  }
        }
	// now p is the last directory of the dname
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	file = (NODE*)malloc(sizeof(NODE));
	strcpy(file->name, bname);
        file->type = 'F';
        file->child_ptr = NULL;
        file->sibling_ptr = NULL;
        file->parent_ptr = p;
	if(is_empty(p)) {
	  p->child_ptr = file;
        } else {
	  p = p->child_ptr;
	  if(strcmp(p->name, bname) == 0) {
	    printf("File or directory already exists!\n");
	    reset();
	    return -1;
	  }
	  while(p->sibling_ptr != NULL) {
	    if(strcmp(p->sibling_ptr->name, bname) == 0) {
	      printf("File or directory already exists!\n");
	      reset();
	      return -1;
	    }
	    p = p->sibling_ptr;
	  }
	  p->sibling_ptr = file;
        }
      } else { // relative pathname
	if(is_empty(cwd)) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	p = cwd->child_ptr;
        strcpy(temp, dname);
	path = strtok(dname, "/");
	if(strcmp(temp, path) == 0) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
      	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL || p->type == 'F') {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  if(p->child_ptr != NULL) {
	    p = p->child_ptr;
	  }
	  path = strtok(NULL, "/");
        }
	// now p is the last directory of the dname
	if(p == NULL || p->type == 'F') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	file = (NODE*)malloc(sizeof(NODE));
	strcpy(file->name, bname);
        file->type = 'F';
        file->child_ptr = NULL;
        file->sibling_ptr = NULL;
        file->parent_ptr = p;
	if(is_empty(p)) {
	  p->child_ptr = file;
        } else {
	  p = p->child_ptr;
	  if(strcmp(p->name, bname) == 0) {
	    printf("File or directory already exists!\n");
	    reset();
	    return -1;
	  }
	  while(p->sibling_ptr != NULL) {
	    if(strcmp(p->sibling_ptr->name, bname) == 0) {
	      printf("File or directory already exists!\n");
	      reset();
	      return -1;
	    }
	    p = p->sibling_ptr;
	  }
	  p->sibling_ptr = file;
        }
      }
    }
    node_number++;
    reset();
    return 0;
  }
  reset();
  return -1;
}

/* rm function: command rm (remove file) */
int rm(char* pathname) {
  NODE* p = NULL;
  NODE* target = NULL;
  char* path = NULL;
  char temp[128];
  
  if(strcmp(pathname, "") == 0) { // no pathname specified
    printf("Invalid path!\n");
    reset();
    return -1;
  } else {
    if(pathname[0] == '/') { // absolute pathname
      if(strcmp(pathname, "/") == 0) {
	printf("Cannot delete root!\n");
	reset();
	return -1;
      } else {
	if(is_empty(root)) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	p = root->child_ptr;
	strcpy(temp, pathname);
	path = strtok(pathname, "/");
	if(strcmp(temp, path) == 0) {
          path = NULL;
        }
	while(path != NULL) {
	  while(p != NULL && strcmp(p->name, path) != 0) {
	    p = p->sibling_ptr;
	  }
	  if(p == NULL) {
	    printf("Invalid path!\n");
	    reset();
	    return -1;
	  }
	  path = strtok(NULL, "/");
	  if(p->child_ptr != NULL && path != NULL) {
	    p = p->child_ptr;
	  }
        }
	if(p == NULL || p->type == 'D') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	// p is now pointing to the last pathname file
	target = p;
	p = p->parent_ptr;
	if(p->child_ptr == target) { // target file is the child
	  if(target->sibling_ptr != NULL) {
	    p->child_ptr = target->sibling_ptr;
          } else {
            p->child_ptr = NULL;
          }
        } else { // target file is a sibling
          p = p->child_ptr;
          while(p->sibling_ptr != target) {
            p = p->sibling_ptr;
          }
          p->sibling_ptr = target->sibling_ptr;
	  target->sibling_ptr = NULL;
	  target->parent_ptr = NULL;
        }
	free(target);
      }
    } else { // relative pathname
      if(is_empty(cwd)) {
	printf("Invalid path!\n");
	reset();
	return -1;
      }
      p = cwd->child_ptr;
      strcpy(temp, pathname);
      path = strtok(pathname, "/");
      if(strcmp(temp, path) == 0) {
	while(p != NULL && strcmp(p->name, path) != 0) {
      	  p = p->sibling_ptr;
	}
	if(p == NULL || p->type == 'D') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = NULL;
      }
      while(path != NULL) {
	while(p != NULL && strcmp(p->name, path) != 0) {
	  p = p->sibling_ptr;
	}
	if(p == NULL) {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
	}
	path = strtok(NULL, "/");
	if(p->child_ptr != NULL && path != NULL) {
	  p = p->child_ptr;
	}
      }
      if(p == NULL || p->type == 'D') {
	  printf("Invalid path!\n");
	  reset();
	  return -1;
      }
      // p is now pointing to the last pathname file
      target = p;
      p = p->parent_ptr;
      if(p->child_ptr == target) { // target file is the child
        if(target->sibling_ptr != NULL) {
	  p->child_ptr = target->sibling_ptr;
	} else {
          p->child_ptr = NULL;
	}
      } else { // target file is a sibling
	p = p->child_ptr;
	while(p->sibling_ptr != target) {
	  p = p->sibling_ptr;
	}
	p->sibling_ptr = target->sibling_ptr;
	target->sibling_ptr = NULL;
	target->parent_ptr = NULL;
      }
      free(target);
    }   
  }
  printf("Done!\n");
  node_number--;
  reset();
  return 0;
}

/* printn function: pre-order traversal (save tree to file) */
void printn(NODE* p, FILE* fp) {
  if(p == NULL) {
    return;
  }
  NODE* p_ref = p;
  RECORD* slider_a = (RECORD*)malloc(sizeof(RECORD));
  RECORD* path = slider_a;
  strcpy(slider_a->name, p->name);
  path->prev = NULL;
  path->next = NULL;
  RECORD* slider_b;
  fprintf(fp, "  %c\t/", p->type);
  while(p_ref != root) {
    slider_b = (RECORD*)malloc(sizeof(RECORD));
    strcpy(slider_b->name, p_ref->name);
    slider_a->next = slider_b;
    slider_b->prev = slider_a;
    slider_b->next = NULL;
    slider_a = slider_a->next;
    slider_b = slider_b->next;
    p_ref = p_ref->parent_ptr;
  }
  while(path->next != NULL) {
    path = path->next;
  }
  while(path->prev != NULL) {
    if(path->prev->prev == NULL)
      fprintf(fp, "%s", path->name);
    else
      fprintf(fp, "%s/", path->name);
    path = path->prev;
  }
  fprintf(fp,"\n");
  printn(p->child_ptr, fp);
  printn(p->sibling_ptr, fp);
}

/* save function: command save (save file system tree to file (see printn() function 
   above for the recursive, pre-order implementation) */
int save(char* filename) {
  if(strcmp(filename, "") == 0) {
    strcpy(filename, "file_system_tree.txt");
  }
  FILE *fp = fopen(filename, "w+");
  fprintf(fp, "%s\t%s\n", " Type", " Pathname");
  fprintf(fp, "%s\t%s\n", "------", "----------");
  fprintf(fp, "  %c\t%s\n", root->type, root->name);

  if(is_empty(root)) {
    reset();
    return 0;
  }
  NODE* p = root->child_ptr;
  printn(p, fp);
  p = NULL;
  fclose(fp);

  printf("Saved %lu nodes to: %s\n", node_number, filename);
  
  reset();
  return 0;
}

/* reload function: command reload (reload file system tree from file) */
int reload(char* filename) {
  FILE* fp = fopen(filename, "r");
  if(fp == NULL) {
    printf("File error!\n");
    reset();
    return -1;
  }
  initialize();
  char type;
  char pathname[1024];

  while(EOF != fscanf(fp, "  %c\t%s", &type, pathname)){
    // skip the first 3 lines
    if(strcmp(pathname, "/") == 0 || strchr(pathname, '/') == NULL) {
      continue;
    }
    if(type == 'D') {
      mkdir(pathname);
    } else if(type == 'F') {
      create(pathname);
    } else {
      printf("Format error!\n");
      fclose(fp);
      reset();
      return -1;
    }
  }
  
  fclose(fp);

  if(node_number == 1) {
    printf("Success: 1 node was created!\n");
  } else {
    printf("Success: %lu nodes were created!\n", node_number);
  }
  
  reset();
  return 0;
}

/* menu function: command menu (help menu for commands and syntax) */
void menu() {
  printf("\nCommand list and syntax:\n");
  printf("1) mkdir <pathname> : create a new directory node in the file system tree.\n");
  printf("2) rmdir <pathname> : remove an empty directory from the file system tree.\n");
  printf("3) cd [<pathname>] : change the current working directory.\n");
  printf("4) ls [<pathname>] : list the directory contents.\n");
  printf("5) pwd : print the (absolute) path to the current working directory.\n");
  printf("6) create <pathname> : create a new file node in the file system tree.\n");
  printf("7) rm <pathname> : remove a file from the file system tree.\n");
  printf("8) save <filename> : save the current file system tree as a file.\n");
  printf("9) reload <filename> : construct a file system tree from a file.\n");
  printf("10) menu : show the current menu.\n");
  printf("11) prompt <prompt> : change the command prompt text.\n");
  printf("12) quit : save the file system tree as \"file_system_tree.txt\" and quit.\n\n");
}

/* prompt function: command prompt (change command prompt text) */
int prompt(char* text) {
  if(strlen(text) <= 31) {
    strcpy(prompt_text, text);
    reset();
    return 0;
  }
  reset();
  return -1;
}

/* quit function: command quit (save & quit) */
void quit() {
  char* filename = "file_system_tree.txt"; 
  save(filename);
  exit(0);
}

/* main */
int main() {
  int index = -1;
  
  if(initialize() != 0) exit(0);

  while(1) {
    printf("%s ", prompt_text);
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;
    sscanf(line, "%s %s", command, argument);
    index = find_cmd(command);
    switch(index) {
        case 0: mkdir(argument); break;
        case 1: rmdir(argument); break;
        case 2: ls(argument); break;
        case 3: cd(argument); break;
        case 4: pwd(); break;
        case 5: create(argument); break;
        case 6: rm(argument); break;
        case 7: reload(argument); break;
        case 8: save(argument); break;
        case 9: menu(); break;
        case 10: prompt(argument); break;
        case 11: quit(); break;
        default: printf("Invalid command: %s!\n", command);
    }
  }
  return 0;
}
