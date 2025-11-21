// Search tool to find keywords inside files

#include "types.h"
#include "stat.h"
#include "user.h"

char buf[1024];

// Simple string search function
// Returns 1 if keyword is found in text, 0 otherwise
int
search_string(char *keyword, char *text)
{
  char *p, *q, *start;
  
  if(*keyword == '\0')
    return 0;
  
  // Search for keyword in text
  for(start = text; *start != '\0'; start++){
    p = start;
    q = keyword;
    while(*q != '\0' && *p != '\0' && *p == *q){
      p++;
      q++;
    }
    if(*q == '\0')
      return 1;
  }
  return 0;
}

// Search for keyword in file
void
search_in_file(char *keyword, char *filename, int show_filename)
{
  int fd, n, m;
  char *p, *q;
  int found = 0;
  
  if((fd = open(filename, 0)) < 0){
    printf(2, "search: cannot open %s\n", filename);
    return;
  }
  
  m = 0;
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    m += n;
    buf[m] = '\0';
    p = buf;
    
    // Process each line
    while((q = strchr(p, '\n')) != 0){
      *q = 0;
      if(search_string(keyword, p)){
        if(show_filename)
          printf(1, "%s:", filename);
        printf(1, "%s\n", p);
        found = 1;
      }
      *q = '\n';
      p = q+1;
    }
    
    // Handle incomplete line at end of buffer
    if(p == buf)
      m = 0;
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
  
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  char *keyword;
  int show_filename = 0;
  
  if(argc < 2){
    printf(2, "usage: search keyword [file ...]\n");
    exit();
  }
  
  keyword = argv[1];
  
  if(argc <= 2){
    // Read from stdin
    int n, m;
    char *p, *q;
    
    m = 0;
    while((n = read(0, buf+m, sizeof(buf)-m-1)) > 0){
      m += n;
      buf[m] = '\0';
      p = buf;
      while((q = strchr(p, '\n')) != 0){
        *q = 0;
        if(search_string(keyword, p))
          printf(1, "%s\n", p);
        *q = '\n';
        p = q+1;
      }
      if(p == buf)
        m = 0;
      if(m > 0){
        m -= p - buf;
        memmove(buf, p, m);
      }
    }
    exit();
  }
  
  // Search in multiple files
  if(argc > 3)
    show_filename = 1;
  
  for(i = 2; i < argc; i++){
    search_in_file(keyword, argv[i], show_filename);
  }
  
  exit();
}

