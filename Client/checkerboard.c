#include"checkerboard.h"

void InitBoard()
{
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    //printf("col:%d\trow:%d\n",size.ws_col,size.ws_row);

    //MOVETO(70,20);
    for(int i=0;i<size.ws_row;i++)
    {
        for(int k=0;k<size.ws_col;k+=1)
        {
            if(i=0)
            {
                printf(" ");
                continue;
            }
            if(k==5)
            {
                printf("⚫");
                continue;
            }
            if(i==10)
            {
                printf("⚪");
                continue;
            }
            printf("➕");//◯☀☼☺☻◆◇♔♚♖♜♛♕
        }
    }
    //printf("➕ ◯ 😃 😄 ⚫ ⚪ ☀ ☼ ☺ ☻ ◆ ◇ ♔ ♚ ♖ ♜ ♛ ♕\n");
    RESET_CURSOR();
    //CLEAR();

}

void getCursorPostion(int col,int row)
{

}

void UpdateBoard(int col,int row)
{

}

void ProcessPressure()
{
    int col=0,row=0;
    
}




#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define   RD_EOF   -1
#define   RD_EIO   -2

static inline int rd(const int fd)
{
    unsigned char   buffer[4];
    ssize_t         n;

    while (1) {

        n = read(fd, buffer, 1);
        if (n > (ssize_t)0)
            return buffer[0];

        else
        if (n == (ssize_t)0)
            return RD_EOF;

        else
        if (n != (ssize_t)-1)
            return RD_EIO;

        else
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
            return RD_EIO;
    }

   return 0;
}

static inline int wr(const int fd, const char *const data, const size_t bytes)
{
    const char       *head = data;
    const char *const tail = data + bytes;
    ssize_t           n;

    while (head < tail) {

        n = write(fd, head, (size_t)(tail - head));
        if (n > (ssize_t)0)
            head += n;

        else
        if (n != (ssize_t)-1)
            return EIO;

        else
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
            return errno;
    }

    return 0;
}

/* Return a new file descriptor to the current TTY.
*/
int current_tty(void)
{
    const char *dev;
    int         fd;

    dev = ttyname(STDIN_FILENO);
    if (!dev)
        dev = ttyname(STDOUT_FILENO);
    if (!dev)
        dev = ttyname(STDERR_FILENO);
    if (!dev) {
        errno = ENOTTY;
        return -1;
    }

    do {
        fd = open(dev, O_RDWR | O_NOCTTY);
    } while (fd == -1 && errno == EINTR);
    if (fd == -1)
        return -1;

    return fd;
}

/* As the tty for current cursor position.
 * This function returns 0 if success, errno code otherwise.
 * Actual errno will be unchanged.
*/
int cursor_position(const int tty, int *const rowptr, int *const colptr)
{
    struct termios  saved, temporary;
    int             retval, result, rows, cols, saved_errno;

    /* Bad tty? */
    if (tty == -1)
        return ENOTTY;

    saved_errno = errno;

    /* Save current terminal settings. */
    do {
        result = tcgetattr(tty, &saved);
    } while (result == -1 && errno == EINTR);
    if (result == -1) {
        retval = errno;
        errno = saved_errno;
        return retval;
    }

    /* Get current terminal settings for basis, too. */
    do {
        result = tcgetattr(tty, &temporary);
    } while (result == -1 && errno == EINTR);
    if (result == -1) {
        retval = errno;
        errno = saved_errno;
        return retval;
    }

    /* Disable ICANON, ECHO, and CREAD. */
    temporary.c_lflag &= ~ICANON;
    temporary.c_lflag &= ~ECHO;
    temporary.c_cflag &= ~CREAD;

    /* This loop is only executed once. When broken out,
     * the terminal settings will be restored, and the function
     * will return retval to caller. It's better than goto.
    */
    do {

        /* Set modified settings. */
        do {
            result = tcsetattr(tty, TCSANOW, &temporary);
        } while (result == -1 && errno == EINTR);
        if (result == -1) {
            retval = errno;
            break;
        }

        /* Request cursor coordinates from the terminal. */
        retval = wr(tty, "\033[6n", 4);
        if (retval)
            break;

        /* Assume coordinate reponse parsing fails. */
        retval = EIO;

        /* Expect an ESC. */
        result = rd(tty);
        if (result != 27)
            break;

        /* Expect [ after the ESC. */
        result = rd(tty);
        if (result != '[')
            break;

        /* Parse rows. */
        rows = 0;
        result = rd(tty);
        while (result >= '0' && result <= '9') {
            rows = 10 * rows + result - '0';
            result = rd(tty);
        }

        if (result != ';')
            break;

        /* Parse cols. */
        cols = 0;
        result = rd(tty);
        while (result >= '0' && result <= '9') {
            cols = 10 * cols + result - '0';
            result = rd(tty);
        }

        if (result != 'R')
            break;

        /* Success! */

        if (rowptr)
            *rowptr = rows;

        if (colptr)
            *colptr = cols;

        retval = 0;

    } while (0);

    /* Restore saved terminal settings. */
    do {
        result = tcsetattr(tty, TCSANOW, &saved);
    } while (result == -1 && errno == EINTR);
    if (result == -1 && !retval)
        retval = errno;

    /* Done. */
    return retval;
}



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