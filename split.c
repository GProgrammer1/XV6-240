#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

void itoa(int n, char *buf)
{
    int i = 0;
    char tmp[16];

    if (n == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (n > 0)
    {
        tmp[i++] = '0' + (n % 10);
        n /= 10;
    }

    for (int j = 0; j < i; j++)
        buf[j] = tmp[i - 1 - j];

    buf[i] = '\0';
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf(2, "Error: Usage <number_of_files> <files>\n");
        exit();
    }

    char *file_name = argv[2];
    int splits = atoi(argv[1]);

    if (splits <= 0)
    {
        printf(2, "Error: splits must be > 0\n");
        exit();
    }

    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
    {
        printf(2, "File not found\n");
        exit();
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        printf(2, "Couldn't get file stats\n");
        close(fd);
        exit();
    }

    char buff[1024];
    // Calculate split size (ceiling division)
    int split_size = ((st.size - 1) + splits - 1) / splits;

    for (int i = 0; i < splits; ++i)
    {
        char str_name[128], extention[16];

        // Construct filename (e.g., file0, file1)
        strcpy(str_name, file_name);
        itoa(i, extention);
        int n = strlen(file_name);
        strcpy(str_name + n, extention);

        int fds = open(str_name, O_CREATE | O_WRONLY);
        if (fds < 0)
        {
            printf(2, "Error creating split file\n");
            break;
        }

        int written = 0;

        // Unified loop to handle reading/writing chunks
        while (written < split_size)
        {
            int bytes_to_read;
            int remaining = split_size - written;

            // Determine if we read a full buffer or just the remainder
            if (remaining > sizeof(buff))
                bytes_to_read = sizeof(buff);
            else
                bytes_to_read = remaining;

            int n = read(fd, buff, bytes_to_read);

            if (n == 0)
                break; // EOF
            if (n < 0)
            {
                printf(2, "Read error\n");
                break;
            }

            if (write(fds, buff, n) != n)
            {
                printf(2, "Write error\n");
                break;
            }

            written += n;
        }

        // CRITICAL: Close the file descriptor for the current split
        // otherwise you will run out of FDs (xv6 limit is usually 16)
        close(fds);
    }

    close(fd);
    exit();
}