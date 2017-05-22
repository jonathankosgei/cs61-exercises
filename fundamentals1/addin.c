#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>

// filesize(f)
//    Return the size of `f` in bytes. Returns -1 if `f` does not have a
//    well-defined size (for instance, if it is a pipe).
/*TODO: 
 * create a stat structure, call fstat on the file and a ref to the stat structure
 * returns some value, 0 or greater for success, also check if it's a reg file
 * in which case return size in stated struct, else it's a demon, like a pipe or samtn
 */
off_t filesize(int fd) {
    struct stat s;
    int r = fstat(fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode))
        return s.st_size;
    else
        return -1;
}

// die(file)
//    Die with an error message.
void die(const char* file) {
    fprintf(stderr, "%s: %s\n", file, strerror(errno));
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc <= 4) {
        fprintf(stderr, "Usage: addin FILE OFFSET A B\n\
    Prints A + B.\n");
        exit(1);
    }

    const char* file = argv[1];
    size_t offset = strtoul(argv[2], 0, 0);
    int a = strtol(argv[3], 0, 0);
    int b = strtol(argv[4], 0, 0);

    // Load the file into memory
    int fd = open(file, O_RDONLY);
    if (fd < 0)
        die(file);

    off_t size = filesize(fd);
    if (size < 0)
        die(file);

    void* data = mmap(NULL, size, PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        die(file);
    uintptr_t data_address = (uintptr_t) data;

    // Call `add`!
    // TODO: check this stuff out again!
    // weird function. function call by ref, pointing to functions
    int (*add)(int, int) = (int (*)(int, int)) (data_address + offset);

    printf("%d + %d = %d\n",
           a,
           b,
           add(a, b));
}
