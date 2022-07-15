#include "checkerboard.h"

void InitBoard()
{
    struct winsize Wsize;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &Wsize);
    // printf("col:%d\trow:%d\n",Wsize.ws_col,Wsize.ws_row);
    pthread_create(&checkPrs, NULL, (void *)&IsProcesslb, NULL);

    // CLEAR();
    // MOVETO(0, 0);
    // for (int i = 0; i < BOARD_SIZE; i++)
    // {
    //     for (int k = 0; k < BOARD_SIZE; k += 1)
    //     {
    //         printf(" ✧"); //◯☀☼☺☻◆◇♔♚♖♜♛♕
    //     }
    //     printf("\n");
    // }

    // HIDE_CURSOR();
    // printf("╋ 🫐 ➕ ✧ ◯ ➕ 😃 😄 ⚫ ⚪ ☀ ☼ ☺ ☻ ◆ ◇ ♔ ♚ ♖ ♜ ♛ ♕\n");
    // RESET_CURSOR();
    // CLEAR();
}

void getCursorPostion(int *col, int *row)
{
    *col = 1;
    *row = 2;
}

void UpdateBoard(int col, int row)
{
}

void ProcessPressure()
{
    int col = 0, row = 0;
    getCursorPostion(&col, &row);
    printf("col:%d row:%d\n", col, row);
}

void IsProcesslb(void)
{
    int fd = -1, ret = 0;
    struct input_event ie = {0};
    memset(&ie, 0, sizeof(struct input_event));
    fd = open("/dev/input/event3", O_RDONLY);
    if (fd < 0)
    {
        printf("鼠标监听失败！\n");
        return;
    }

    while (1)
    {
        ret = read(fd, &ie, sizeof(struct input_event));
        if (ret < 0)
        {
            printf("event3 read failed!\n");
            return;
        }

        switch (ie.type)
        {
        case EV_KEY:
        {
            if (ie.code == 272 && ie.value == 1)
                ProcessPressure();
            break;
        }
        // case EV_ABS:
        //     printf("ABS      type:%d code:%d value:%d\n", ie.type, ie.code, ie.value);
        //     break;
        // case EV_REL:
        //     printf("REL      type:%d code:%d value:%d\n", ie.type, ie.code, ie.value);
        //     break;
        default:
            break;
        }
    }
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

//sudo apt install  xdotool
void getAbsPosetion(int *x,int* y)
{
    FILE *fstream = NULL;
    int error = 0;
    char buff[200] = {0};

    if (NULL == (fstream = popen("xdotool getmouselocation", "w"))) 
    {
        fprintf(stderr, "execute command failed:%s", strerror(error));
        return;
    }

    if (NULL != fgets(buff, sizeof(buff), fstream))
    {
        printf("%s", buff);
    }

    pclose(fstream);
    return ;
}

int wwwwwwtest()
{


//  struct input_event input_buf;
//   int fd, x, y;
//   int flag_ts = 0;
//   int len = 0;
 
//   //open virtual screen device
//   fd = open("/dev/input/event3",O_RDWR);
 
//   if(fd < 0)
//   {
//    perror("open ts");
//    return -1;
//   }
 
//   while(1)
//   {
//    //read direction
//    len = read(fd, &input_buf,sizeof(input_buf));
 
//    if (len < 0)
//    {
//     perror("read us");
//     return -1;
//    }
 
//    //判断是否绝对坐标值,EV_ABS,也就是触摸屏事件
//    //判断是返回的是X坐标还是Y坐标
//    if (input_buf.type == EV_ABS && input_buf.code == ABS_X)
//    {
//     x = input_buf.value;
//     flag_ts|=0x01;
//    }
//    //判断是否绝对坐标值,EV_ABS,也就是触摸屏事件
//    //判断是返回的是X坐标还是Y坐标
//    if (input_buf.type == EV_ABS && input_buf.code ==ABS_Y)
//    {
//     y = input_buf.value;
//     flag_ts|=0x02;
//    }
//    //x、y坐标获取完成
//    if (flag_ts==0x03)
//    {
//     printf("x=%d y=%d\r\n", x, y);
//     flag_ts = 0;
//    }
//    //延时10ms
//    usleep(10*1000);
//   }
//  return 0;
}

// void wwwtest()
// {
//     fd_set readset;
//     Gpm_Event event;
//     Gpm_Connect conn;

//     conn.eventMask = ~0;
//     conn.defaultMask = ~GPM_HARD;
//     conn.maxMod = 0;
//     conn.minMod = 0;

//     if (Gpm_Open(&conn, 0) == -1)
//     {
//         printf("Can not open mouse connection\n");
//         exit(1);
//     }

//     while (1)
//     {
//         FD_ZERO(&readset);
//         FD_SET(gpm_fd, &readset);
//         select(gpm_fd + 1, &readset, 0, 0, 0);

//         if (FD_ISSET(gpm_fd, &readset))
//         {
//             if (Gpm_GetEvent(&event) > 0)
//             {
//                 printf("mouse: event 0x%02X, at %2i %2i (delta %2i %2i),"
//                        "button %i, modifiers 0x%02X\r\n",
//                        event.type,
//                        event.x, event.y,
//                        event.dx, event.dy,
//                        event.buttons,
//                        event.modifiers);
//             }
//         }
//     }
//     while (Gpm_Close())
//         ;
// }

int wwtest()
{
    int fd, retval;
    char buf[6];
    fd_set readfds;
    struct timeval tv;
    int x = 0, y = 0;
    bool times = false;
    // 打开鼠标设备
    fd = open("/dev/input/mice", O_RDONLY);
    // 判断是否打开成功
    if (fd < 0)
    {
        printf("Failed to open \"/dev/input/mice\".\n");
        exit(1);
    }
    else
    {
        printf("open \"/dev/input/mice\" successfuly.\n");
    }

    while (1)
    {
        // 设置最长等待时间
        if (!times)
        {
            printf("( %d,%d )\n", x, y);
            times = true;
        }
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        retval = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (retval == 0)
        {
            printf("Time out!\n");
        }
        if (FD_ISSET(fd, &readfds))
        {
            // 读取鼠标设备中的数据
            if (read(fd, buf, 6) <= 0)
            {
                continue;
            }
            // 打印出从鼠标设备中读取到的数据
            // printf("Button type = %d, X = %d, Y = %d, Z = %d\n", (buf[0] & 0x07), buf[1], buf[2],   buf[3]);
            x = buf[1];
            y = buf[2];
            times = false;
        }
    }
    close(fd);
    return 0;
}

// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <linux/input.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <dirent.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <semaphore.h>

// char **test;
// int p = 0;

// pthread_mutex_t lock;

// void th(void)
// {
//     int fd = -1, ret = 0;
//     struct input_event ev;

//     char dirname[500] = "/dev/input/";
//     memset(&ev, 0, sizeof(struct input_event));

//     pthread_mutex_lock(&lock);
//     strcat(dirname, test[p]);
//     dirname[11 + strlen(test[p++])] = '\0';
//     pthread_mutex_unlock(&lock);

//     fd = open(dirname, O_RDONLY);
//     if (fd < 0)
//         printf("open %s failed\n", dirname);

//     while (1)
//     {
//         ret = read(fd, &ev, sizeof(struct input_event));
//         if (ret < 0)
//         {
//             printf("read failed!\n");
//             break;
//         }
//         switch (ev.type)
//         {
//         case EV_KEY:
//         {
//             printf("按键触发事件(%s)\ttype:%d code:%d value:%d\n", dirname, ev.type, ev.code, ev.value);
//             if (ev.code == 272)
//                 printf("\t按了鼠标左键\n");
//             if (ev.code == 273)
//                 printf("\t按了鼠标右键\n");
//             if (ev.code == 274)
//                 printf("\t按了鼠标中键\n");
//             break;
//         }

//         case EV_ABS:
//             printf("鼠标移动事件(%s)\ttype:%d code:%d value:%d\n", dirname, ev.type, ev.code, ev.value);
//             break;

//         default:
//             printf("其他事件(%s)\ttype:%d code:%d value:%d\n", dirname, ev.type, ev.code, ev.value);
//         }
//     }
//     close(fd);
// }

// int main()
// {
//     int at = 0;

//     test = (char **)malloc(100 * sizeof(char *));
//     for (int i = 0; i < 100; i++)
//         test[i] = (char *)malloc(500 * sizeof(char));

//     pthread_t id[100];
//     pthread_mutex_init(&lock, NULL);

//     struct dirent *di;
//     DIR *dir = NULL;

//     dir = opendir("/dev/input");
//     if (dir == NULL)
//     {
//         printf("failed to open directory!");
//         return 0;
//     }

//     while (NULL != (di = readdir(dir)))
//     {
//         if (di->d_type != DT_DIR && di->d_name[0] != '.')
//         {
//             strcpy(test[at], di->d_name);
//             pthread_create(&id[at++], NULL, (void *)th, NULL);
//         }
//     }

//     for (int i = 0; i < at; i++)
//     {
//         pthread_join(id[i], NULL);
//     }

//     for (int i = 0; i < at; i++)
//         free(test[i]);
//     free(test);

//     return 0;
// }

// #include <unistd.h>
// #include <fcntl.h>
// #include <termios.h>
// #include <errno.h>

// #define   RD_EOF   -1
// #define   RD_EIO   -2

// static inline int rd(const int fd)
// {
//     unsigned char   buffer[4];
//     ssize_t         n;

//     while (1) {

//         n = read(fd, buffer, 1);
//         if (n > (ssize_t)0)
//             return buffer[0];

//         else
//         if (n == (ssize_t)0)
//             return RD_EOF;

//         else
//         if (n != (ssize_t)-1)
//             return RD_EIO;

//         else
//         if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
//             return RD_EIO;
//     }

//    return 0;
// }

// static inline int wr(const int fd, const char *const data, const size_t bytes)
// {
//     const char       *head = data;
//     const char *const tail = data + bytes;
//     ssize_t           n;

//     while (head < tail) {

//         n = write(fd, head, (size_t)(tail - head));
//         if (n > (ssize_t)0)
//             head += n;

//         else
//         if (n != (ssize_t)-1)
//             return EIO;

//         else
//         if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
//             return errno;
//     }

//     return 0;
// }

// /* Return a new file descriptor to the current TTY.
// */
// int current_tty(void)
// {
//     const char *dev;
//     int         fd;

//     dev = ttyname(STDIN_FILENO);
//     if (!dev)
//         dev = ttyname(STDOUT_FILENO);
//     if (!dev)
//         dev = ttyname(STDERR_FILENO);
//     if (!dev) {
//         errno = ENOTTY;
//         return -1;
//     }

//     do {
//         fd = open(dev, O_RDWR | O_NOCTTY);
//     } while (fd == -1 && errno == EINTR);
//     if (fd == -1)
//         return -1;

//     return fd;
// }

// /* As the tty for current cursor position.
//  * This function returns 0 if success, errno code otherwise.
//  * Actual errno will be unchanged.
// */
// int cursor_position(const int tty, int *const rowptr, int *const colptr)
// {
//     struct termios  saved, temporary;
//     int             retval, result, rows, cols, saved_errno;

//     /* Bad tty? */
//     if (tty == -1)
//         return ENOTTY;

//     saved_errno = errno;

//     /* Save current terminal settings. */
//     do {
//         result = tcgetattr(tty, &saved);
//     } while (result == -1 && errno == EINTR);
//     if (result == -1) {
//         retval = errno;
//         errno = saved_errno;
//         return retval;
//     }

//     /* Get current terminal settings for basis, too. */
//     do {
//         result = tcgetattr(tty, &temporary);
//     } while (result == -1 && errno == EINTR);
//     if (result == -1) {
//         retval = errno;
//         errno = saved_errno;
//         return retval;
//     }

//     /* Disable ICANON, ECHO, and CREAD. */
//     temporary.c_lflag &= ~ICANON;
//     temporary.c_lflag &= ~ECHO;
//     temporary.c_cflag &= ~CREAD;

//     /* This loop is only executed once. When broken out,
//      * the terminal settings will be restored, and the function
//      * will return retval to caller. It's better than goto.
//     */
//     do {

//         /* Set modified settings. */
//         do {
//             result = tcsetattr(tty, TCSANOW, &temporary);
//         } while (result == -1 && errno == EINTR);
//         if (result == -1) {
//             retval = errno;
//             break;
//         }

//         /* Request cursor coordinates from the terminal. */
//         retval = wr(tty, "\033[6n", 4);
//         if (retval)
//             break;

//         /* Assume coordinate reponse parsing fails. */
//         retval = EIO;

//         /* Expect an ESC. */
//         result = rd(tty);
//         if (result != 27)
//             break;

//         /* Expect [ after the ESC. */
//         result = rd(tty);
//         if (result != '[')
//             break;

//         /* Parse rows. */
//         rows = 0;
//         result = rd(tty);
//         while (result >= '0' && result <= '9') {
//             rows = 10 * rows + result - '0';
//             result = rd(tty);
//         }

//         if (result != ';')
//             break;

//         /* Parse cols. */
//         cols = 0;
//         result = rd(tty);
//         while (result >= '0' && result <= '9') {
//             cols = 10 * cols + result - '0';
//             result = rd(tty);
//         }

//         if (result != 'R')
//             break;

//         /* Success! */

//         if (rowptr)
//             *rowptr = rows;

//         if (colptr)
//             *colptr = cols;

//         retval = 0;

//     } while (0);

//     /* Restore saved terminal settings. */
//     do {
//         result = tcsetattr(tty, TCSANOW, &saved);
//     } while (result == -1 && errno == EINTR);
//     if (result == -1 && !retval)
//         retval = errno;

//     /* Done. */
//     return retval;
// }

/*获取光标位置
int main(void)
{
    int         fd, row, col;
    char        buffer[64];
    char *const tail = buffer + sizeof(buffer);
    char       *head = buffer + sizeof(buffer);

    fd = current_tty();
    if (fd == -1)
        return 1;

    row = 0;
    col = 0;
    if (cursor_position(fd, &row, &col))
        return 2;

    if (row < 1 || col < 1)
        return 3;

    //Construct response "(row, col) " from right to left,
    //then output it to standard error, and exit.


    *(--head) = ' ';
    *(--head) = ')';

    {   unsigned int    u = col;
        do {
            *(--head) = '0' + (u % 10U);
            u /= 10U;
        } while (u);
    }

    *(--head) = ' ';
    *(--head) = ',';

    {   unsigned int    u = row;
        do {
            *(--head) = '0' + (u % 10U);
            u /= 10U;
        } while (u);
    }

    *(--head) = '(';

    if (wr(STDERR_FILENO, head, (size_t)(tail - head)))
        return 4;

    return 0;
}
*/