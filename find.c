// Find command to recursively search directories for files matching a name or pattern

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[512];

// Recursively search directory for files matching pattern
void
find_recursive (char *path, char *pattern)
{
  char *p;
  int fd;
  struct dirent de;
  struct stat st;
  int path_len;

  if ((fd = open (path, 0)) < 0)
    {
      printf (2, "find: cannot open %s\n", path);
      return;
    }

  if (fstat (fd, &st) < 0)
    {
      printf (2, "find: cannot stat %s\n", path);
      close (fd);
      return;
    }

  // Only process directories
  if (st.type != T_DIR)
    {
      close (fd);
      return;
    }

  // Check path length to avoid overflow
  path_len = strlen (path);
  if (path_len + 1 + DIRSIZ + 1 > sizeof (buf))
    {
      printf (2, "find: path too long\n");
      close (fd);
      return;
    }

  // Build path for subdirectories
  strcpy (buf, path);
  p = buf + path_len;
  *p++ = '/';

  // Read directory entries
  while (read (fd, &de, sizeof (de)) == sizeof (de))
    {
      if (de.inum == 0)
	continue;

      // Skip . and .. entries
      de.name[DIRSIZ] = 0;	// Ensure null termination
      if (strcmp (de.name, ".") == 0 || strcmp (de.name, "..") == 0)
	continue;

      // Build full path
      memmove (p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      // Stat the entry
      if (stat (buf, &st) < 0)
	{
	  printf (2, "find: cannot stat %s\n", buf);
	  continue;
	}

      // Check if filename matches pattern
      if (wildcard_match (pattern, de.name))
	{
	  printf (1, "%s\n", buf);
	}

      // Recursively search subdirectories
      if (st.type == T_DIR)
	{
	  find_recursive (buf, pattern);
	}
    }

  close (fd);
}

int
main (int argc, char *argv[])
{
  char *start_dir;
  char *pattern;

  if (argc < 3)
    {
      printf (2, "usage: find <directory> <pattern>\n");
      printf (2, "       Example: find . *.c\n");
      printf (2, "       Example: find / file.txt\n");
      exit ();
    }

  start_dir = argv[1];
  pattern = argv[2];

  // Verify start directory is actually a directory
  struct stat st;
  if (stat (start_dir, &st) < 0)
    {
      printf (2, "find: cannot stat %s\n", start_dir);
      exit ();
    }

  if (st.type != T_DIR)
    {
      printf (2, "find: %s is not a directory\n", start_dir);
      exit ();
    }

  // Start recursive search
  find_recursive (start_dir, pattern);

  exit ();
}
