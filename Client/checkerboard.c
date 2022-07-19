#include "checkerboard.h"
#include "Client.h"

int x = 0, y = 0;
extern char *nickname;
extern int socket_atServer;
int socket_other = 0;
bool turn = true, piece = false, isforgame = false;
bool isTakeUp[20][20] = {false};

void InitBoard(int s_O)
{
    socket_other = s_O;

    // struct winsize Wsize;
    // ioctl(STDIN_FILENO, TIOCGWINSZ, &Wsize);
    // printf("col:%d\trow:%d\n",Wsize.ws_col,Wsize.ws_row);

    CLEAR();
    MOVETO(0, 0);
    fflush(stdout);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int k = 0; k < BOARD_SIZE; k += 1)
        {
            printf(" 🫐"); //◯☀☼☺☻◆◇♔♚♖♜♛♕
        }
        if (i == 5 && !turn)
            printf("  状态：对方正在落棋");
        else if (i == 5 && turn)
            printf("  状态：己方正在落棋");
        if (i == 7)
            printf("\t控制：");
        if (i == 8)
            printf("\tW：↑"); //\t=6
        if (i == 9)
            printf("\tS：↓");
        if (i == 10)
            printf("\tA：←");
        if (i == 11)
            printf("\tD：→");
        if (i == 12)
            printf("\tEnter：落棋");
        printf("\n");
    }

    MOVETO(0, 0);
    printf(" 😃");
    MOVETO(0, 0);
    
    HIDE_CURSOR();

    system("stty -echo");
    fflush(stdout);
    // MOVETO(10, 10*3-1);
    // printf("⚪");
    // fflush(stdout);
    // HIDE_CURSOR();
    //  printf("╋ 🫐 ➕ ✧ ◯ ➕ 😃 😄 ⚫ ⚪ ☀ ☼ ☺ ☻ ◆ ◇ ♔ ♚ ♖ ♜ ♛ ♕\n");
    // UpdateBoard(10,10,"🫐");
    //  UpdateBoard(10,15,"😃");
}

void setpng(bool pi,bool g)
{
    piece = pi;
    isforgame=g;
}
void setturn(bool t)
{
    turn=t;
}

void ProcessPressure(int key)
{
    if (!turn)
        return;
    switch (key)
    {
    case 17:
    {
        if (--y < 0 || isTakeUp[x][y])
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
        if (++y > 19 || isTakeUp[x][y])
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
        if (--x < 0 || isTakeUp[x][y])
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
        if (++x > 19 || isTakeUp[x][y])
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
        ProcessState(x, y, true);

        struct Msg_info MsgInfo = {0};
        MakeMsg(&MsgInfo, MATCH_SET_LOCATION, nickname, socket_atServer, socket_other, "", x, y);
        SendToServer(MsgInfo);
    }

    default:
    {
    }
    break;
    }
}

void ProcessState(int rel_x, int rel_y, bool self_other)
{
    if (self_other)
        turn = false;
    else
        turn = true;

    isTakeUp[x][y] = true;

    MOVETO(rel_x, rel_y);
    if (self_other)
    {
        if (!piece)
            printf(" ⚫");
        else
            printf(" ⚪");
    }
    else
    {
        if (!piece)
            printf(" ⚪");
        else
            printf(" ⚫");
    }

    if (!self_other)
    {
        int temp_x = x, temp_y = y;
        while (!isTakeUp[temp_x][temp_y])
        {
            temp_x++;
            temp_x %= BOARD_SIZE;
        }

        x = temp_x;
        y = temp_y;
        MOVETO(temp_x, temp_y);
        printf(" 😃");
    }

    MOVELEFT(3);
    fflush(stdout);
}

void IsPressurekey(void)
{

    // while (1)
    // {
    int times = 0;
    int fd = -1, ret = 0;
    struct input_event ie = {0};
    memset(&ie, 0, sizeof(struct input_event));
    fd = open(KEY_DEVICE, O_RDONLY);
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

        // printf("type:%d ,value,%d ,code:%d\n",ie.type,ie.value,ie.code);
        switch (ie.type)
        {
        case EV_KEY:
        {

            if (ie.value > 0)
            {
                if (!isforgame && ie.value == 1)
                    inputCmd(ie.code);
                else if(isforgame)
                    ProcessPressure(ie.code);
            }

            break;
        }
        default:
            break;
        }
    }
    close(fd);
    //}
}
