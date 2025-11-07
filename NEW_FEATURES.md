# XV6 Enhancements

This document summarizes the additions made to the xv6 tree: a teaching-style
`diff` user program, shell command history, and kernel-level threads with
`clone`/`join` support and a sample `threadtest` app.

## Diff User Program

- **Files touched**: `diff.c`, `Makefile`
- `diff` compares two user files line by line. It prints the line number and
  emits the differing lines prefixed with `<` (first file) and `>` (second
  file). If one file ends earlier, the output shows `EOF` on that side. Identical
  files result in `files identical`.
- Implementation notes:
  - Reads each line with a helper that stops at newline or EOF (`MAXLINE` 512).
  - Stops on the first read error or truncated line and reports the issue.
  - Keeps a small counter (`line`) and counts total differences to decide
    whether to print the “identical” banner.
- Usage inside xv6 shell:
  - `echo hello > a`, `echo hEllo > b`, `diff a b` → shows the mismatch.
  - `diff a a` → prints `files identical`.

## Shell Command History

- **File touched**: `sh.c`
- Added a 16-entry ring buffer storing recent command strings.
- Features:
  - `history` prints the stored list with numbering.
  - `!!` re-executes the most recent command; `!N` runs the Nth command from the
    history list.
  - The shell echoes recalled commands before execution so users see what runs.
- Implementation details:
  - Simple `memmove` copies into the buffer; no persistent storage.
  - History skips blank lines; trimming removes trailing newline characters
    before storage.

## Kernel Threads (`clone`, `join`)

- **Files touched**: `proc.c`, `proc.h`, `defs.h`, `sysproc.c`, `syscall.c`,
  `syscall.h`, `usys.S`, `user.h`, `threadtest.c`, `Makefile` (added
  `_threadtest`).

### Kernel Changes

- Process structure (`proc.h`) now includes metadata for threads:
  `isthread`, `ustack`, `thread_master`, and a simple `thread_refcount`.
- `clone`:
  - Allocates a proc, shares the caller’s page table, and treats the provided
    user stack as a full page.
  - Sets up a fake user stack frame containing the argument and a dummy return
    address (so the thread function ends via `exit`).
  - Shares open files and working directory by shallow copy.
- `join`:
  - Looks for zombie threads belonging to the caller’s master thread list.
  - Returns the thread’s stack pointer, allowing user space to free the stack
    backing storage.
- Exit path: parent processes keep ownership of open files and inodes; threads
  avoid closing shared resources. When a process exits, any live threads are
  marked killed so they wake and terminate.
- `wait` ignores threads; it still only reaps child processes.
- `growproc` propagates memory size changes to sibling threads sharing the same
  page directory.
- Syscall plumbing: new numbers in `syscall.h`, entries in `syscall.c`, wrappers
  in `usys.S`, prototypes in `user.h`, and handlers in `sysproc.c`.

### User-Space Thread Test

- `threadtest.c` demonstrates basic usage:
  - Allocates page-aligned stacks (by `malloc`-ing 2 pages and aligning).
  - Spawns a few worker threads with `clone`, each printing with delays.
  - `join` is called repeatedly; for each thread it receives the stack pointer
    and frees the original allocation.
  - The program prints progress messages so behavior is clear during testing.

### Notes & Limitations

- All threads share an address space; there is no scheduler distinction between
  processes and threads.
- The implementation relies on user code to provide valid, page-sized stacks
  and to call `exit()` at the end of a thread function.
- Close/wait semantics remain coarse-grained and are sufficient for learning
  purposes.

## Build & Test

- `make` builds the new user programs (`_diff`, `_threadtest`) and the kernel
  with thread support.
- `make qemu` or `make qemu-nox` boots xv6; run the programs from the shell to
  verify behavior.
- Typical manual tests:
  - `diff` on identical and different files, mismatched lengths, etc.
  - `history`, `!!`, `!N` for shell recall.
  - `threadtest` to see multi-threaded output and confirm `join` cleanup.


