#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdbool.h>

//#include <gpm.h>



#define BOARD_SIZE 20


void InitBoard(int);
void UpdateBoard(int col, int row, char *cr);

void ProcessPressure(int);

void ProcessState(int,int,bool);

