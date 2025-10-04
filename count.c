#include "types.h"
#include "stat.h"
#include "user.h"

#define SYSCALLS_COUNT 23
int
main (int argc, char **argv)
{
  printf (1, "===================================\n");
  if (argc > 1)
    {
      if (!strcmp (argv[1], "--all"))
	{
	  for (int i = 1; i <= SYSCALLS_COUNT; ++i)
	    {
	      int total = count (i);
	      printf (1, "Syscall %d: %d.\n", i, total);
	    }
	}
      else
	{
	  for (int i = 1; i < argc; ++i)
	    {
	      int call_number = atoi (argv[i]);
	      int total = count (call_number);
	      printf (1, "Syscall %d: %d.\n", call_number, total);
	    }
	}
    }
  printf (1, "Total calls: %d\n", count (-1));
  printf (1, "===================================\n");
  exit ();
}
