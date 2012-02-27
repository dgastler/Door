#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

int setNonblocking(int fd);
int serialport_init(const char* serialport, int baud);
