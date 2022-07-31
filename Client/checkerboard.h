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



#define KEY_DEVICE  /*"/dev/input/event4"*/"/dev/input/event1"
#define BOARD_SIZE 20
#define BOROAD_STYLE " 💠"

void InitBoard(int);
void UpdateBoard(int col, int row, char *cr);

void ProcessPressure(int);

void ProcessState(int,int,bool);

