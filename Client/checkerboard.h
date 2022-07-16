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

// 清除屏幕
#define CLEAR() printf("\033[2J")
// 上移光标
#define MOVEUP(x) printf("\033[%dA", (x))
// 下移光标
#define MOVEDOWN(x) printf("\033[%dB", (x))
// 左移光标
#define MOVELEFT(y) printf("\033[%dD", (y))
// 右移光标
#define MOVERIGHT(y) printf("\033[%dC", (y))
// 定位光标
#define MOVETO(x, y) printf("\033[%d;%dH", (x), (y))
// 光标复位
#define RESET_CURSOR() printf("\033[H")
// 隐藏光标
#define HIDE_CURSOR() printf("\033[?25l")
// 显示光标
#define SHOW_CURSOR() printf("\033[?25h")
//反显
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")

#define BOARD_SIZE 20


void InitBoard(int);
void UpdateBoard(int col, int row, char *cr);
void getCursorPostion(int *col, int *row);
void ProcessPressure(int);
void IsProcesskey();
//char getCH();

int wwtest();
void getRelPosetion(int *x, int *y);
