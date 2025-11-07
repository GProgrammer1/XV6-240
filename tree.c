#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
  char *p;
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void
tree(char *path, int depth)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "tree: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    for (int i = 0; i < depth; i++) printf(1, "  "); // Indentation
    printf(1, "%s\n", fmtname(path));
    break;

  case T_DIR:
    for (int i = 0; i < depth; i++) printf(1, "  "); // Indentation
    printf(1, "%s/\n", fmtname(path)); // Indicate directory

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "tree: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      tree(buf, depth + 1); // Recursive call for subdirectories
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    tree(".", 0); // Default to current directory
    exit();
  }
  tree(argv[1], 0); // Start from specified directory
  exit();
}