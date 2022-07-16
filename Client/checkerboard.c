#include "checkerboard.h"
#include"Client.h"

int x = 0, y = 0;
extern char* nickname;
extern int socket_atServer;
int socket_other=0;

void InitBoard(int s_O)
{
    socket_other=s_O;

    pthread_t checkPrs;
    // struct winsize Wsize;
    // ioctl(STDIN_FILENO, TIOCGWINSZ, &Wsize);
    // printf("col:%d\trow:%d\n",Wsize.ws_col,Wsize.ws_row);
    pthread_create(&checkPrs, NULL, (void *)&IsProcesskey, NULL);

    CLEAR();
    MOVETO(0, 0);
    fflush(stdout);
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int k = 0; k < BOARD_SIZE; k += 1)
        {
            printf(" 🫐"); //◯☀☼☺☻◆◇♔♚♖♜♛♕
        }
        printf("\n");
    }

    MOVETO(0, 0);
    printf(" 😃");
    MOVETO(0, 0);
    HIDE_CURSOR();
    fflush(stdout);
    system("stty -echo");
    // MOVETO(10, 10*3-1);
    // printf("⚪");
    // fflush(stdout);
    // HIDE_CURSOR();
    //  printf("╋ 🫐 ➕ ✧ ◯ ➕ 😃 😄 ⚫ ⚪ ☀ ☼ ☺ ☻ ◆ ◇ ♔ ♚ ♖ ♜ ♛ ♕\n");
    // UpdateBoard(10,10,"🫐");
    //  UpdateBoard(10,15,"😃");
    pthread_join(checkPrs, NULL);
}

void UpdateBoard(int col, int row, char *cr)
{
    MOVETO(col, row);
    // printf("%s",cr);
    // printf("why");
}

void getCursorPostion(int *col, int *row)
{
    *col = 1;
    *row = 2;
}

void ProcessPressure(int key)
{
    switch (key)
    {
    case 17:
    {
        if (--y < 0)
        {
            y++;
            break;
        }
        printf(" 🫐");
        MOVELEFT(3);

        MOVEUP(1);

        printf(" 😃");
        MOVELEFT(3);

        fflush(stdout);
        break;
    }

    case 31:
    {
        if (++y > 19)
        {
            y--;
            break;
        }

        printf(" 🫐");
        MOVELEFT(3);

        MOVEDOWN(1);

        printf(" 😃");
        MOVELEFT(3);

        fflush(stdout);
        break;
    }

    case 30:
    {
        if (--x < 0)
        {
            x++;
            break;
        }

        printf(" 🫐");
        MOVELEFT(3);

        MOVELEFT(3);

        printf(" 😃");
        MOVELEFT(3);


        fflush(stdout);
        break;
    }

    case 32:
    {
        if (++x > 19)
        {
            x--;
            break;
        }

        printf(" 🫐");
        MOVELEFT(3);

        MOVERIGHT(3);

        printf(" 😃");
        MOVELEFT(3);


        fflush(stdout);
        break;
    }
    case 28:
    {
        struct Msg_info MsgInfo={0};
        MakeMsg(&MsgInfo,MATCH_SETLOCATION,nickname,socket_atServer,socket_other,"");
    }

    default:
    {
    }
        break;
    }
}

void IsProcesskey(void)
{

    while (1)
    {
        int times = 0;
        int fd = -1, ret = 0;
        struct input_event ie = {0};
        memset(&ie, 0, sizeof(struct input_event));
        fd = open("/dev/input/event1", O_RDONLY);
        if (fd < 0)
        {
            printf("键盘监听失败！\n");
            return;
        }

        while (1)
        {
            ret = read(fd, &ie, sizeof(struct input_event));
            if (ret < 0)
            {
                printf("event1 read failed!\n");
                return;
            }

            //printf("type:%d ,value,%d ,code:%d\n",ie.type,ie.value,ie.code);
            switch (ie.type)
            {
            case EV_KEY:
            {
                if (ie.value >0)
                {
                    ProcessPressure(ie.code);
                }
                break;
            }
            default:
                break;
            }
        }
        close(fd);
    }
}
