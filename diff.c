#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAXLINE 512

static int
readline(int fd, char *buf, int max)
{
  int i = 0;
  char c;

  while(i < max - 1){
    int n = read(fd, &c, 1);
    if(n < 0)
      return -2;
    if(n == 0)
      break;
    buf[i++] = c;
    if(c == '\n')
      break;
  }

  if(i == 0)
    return -1;

  buf[i] = 0;
  if(i == max - 1 && buf[i-1] != '\n')
    return -3;
  return i;
}

static void
printline(const char *prefix, const char *line, int len)
{
  printf(1, "%s", prefix);
  if(line)
    printf(1, "%s", line);
  if(len <= 0 || line[len-1] != '\n')
    printf(1, "\n");
}

int
main(int argc, char *argv[])
{
  int fd1, fd2;
  char line1[MAXLINE];
  char line2[MAXLINE];
  int len1, len2;
  int line = 1;
  int differences = 0;
  int error = 0;

  if(argc != 3){
    printf(2, "usage: diff file1 file2\n");
    exit();
  }

  fd1 = open(argv[1], O_RDONLY);
  if(fd1 < 0){
    printf(2, "diff: cannot open %s\n", argv[1]);
    exit();
  }

  fd2 = open(argv[2], O_RDONLY);
  if(fd2 < 0){
    printf(2, "diff: cannot open %s\n", argv[2]);
    close(fd1);
    exit();
  }

  for(;; line++){
    len1 = readline(fd1, line1, sizeof(line1));
    len2 = readline(fd2, line2, sizeof(line2));

    if(len1 == -2 || len1 == -3){
      printf(2, "diff: error reading %s\n", argv[1]);
      error = 1;
      break;
    }
    if(len2 == -2 || len2 == -3){
      printf(2, "diff: error reading %s\n", argv[2]);
      error = 1;
      break;
    }

    if(len1 == -1 && len2 == -1)
      break;

    if(len1 == -1 && len2 != -1){
      printf(1, "line %d\n", line);
      printline("< EOF", 0, 0);
      printline("> ", line2, len2);
      differences++;
      continue;
    }

    if(len2 == -1 && len1 != -1){
      printf(1, "line %d\n", line);
      printline("< ", line1, len1);
      printline("> EOF", 0, 0);
      differences++;
      continue;
    }

    if(len1 != len2 || strcmp(line1, line2) != 0){
      printf(1, "line %d\n", line);
      printline("< ", line1, len1);
      printline("> ", line2, len2);
      differences++;
    }
  }

  close(fd1);
  close(fd2);

  if(!error && differences == 0)
    printf(1, "files identical\n");

  exit();
}

