#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

// Check if string contains wildcard characters
int
has_wildcard(char *str)
{
  while(*str) {
    if(*str == '*' || *str == '?')
      return 1;
    str++;
  }
  return 0;
}

// List files matching pattern in directory
void
ls_wildcard(char *pattern)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  char *dir;
  char *file_pattern;
  int found = 0;
  
  // Extract directory and filename pattern
  // For simplicity, assume pattern is in current directory
  dir = ".";
  file_pattern = pattern;
  
  // Check if pattern has a directory component
  for(p = pattern + strlen(pattern); p >= pattern && *p != '/'; p--)
    ;
  
  if(p > pattern) {
    // Pattern has directory component
    *p = '\0';
    dir = pattern;
    file_pattern = p + 1;
  }
  
  if((fd = open(dir, 0)) < 0){
    printf(2, "ls: cannot open %s\n", dir);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    printf(2, "ls: %s is not a directory\n", dir);
    close(fd);
    return;
  }

  strcpy(buf, dir);
  p = buf + strlen(buf);
  *p++ = '/';
  
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    
    // Check if filename matches pattern
    de.name[DIRSIZ] = 0; // Ensure null termination
    if(wildcard_match(file_pattern, de.name)){
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      found = 1;
    }
  }
  
  if(!found) {
    printf(2, "ls: no matches found for pattern '%s'\n", file_pattern);
  }
  
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  
  for(i=1; i<argc; i++){
    if(has_wildcard(argv[i])){
      ls_wildcard(argv[i]);
    } else {
      ls(argv[i]);
    }
  }
  exit();
}
