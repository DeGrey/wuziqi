#include "checkerboard.h"
#include "Client.h"

int x = 0, y = 0;
extern char *nickname;
extern int socket_atServer;
int socket_other = 0;
bool turn = true, /*piece = false,**/ isforgame = false;
int piece = 0;
int isTakeUp[BOARD_SIZE][BOARD_SIZE];

void InitBoard(int s_O)
{
    socket_other = s_O;
    x = 0;
    y = 0;

    CLEAR();
    MOVETO(0, 0);
    fflush(stdout);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int k = 0; k < BOARD_SIZE; k += 1)
        {
            isTakeUp[i][k] = 0;
            printf(BOROAD_STYLE); //◯☀☼☺☻◆◇♔♚♖♜♛♕
        }
        // if (i == 5 && !turn)
        //     printf("  状态：对方正在落棋");
        // else if (i == 5 && turn)
        //     printf("  状态：己方正在落棋");
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

    //  printf("╋ 🫐 ➕ ✧ ◯ ➕ 😃 😄 ⚫ ⚪ 💠☀ ☼ ☺ ☻ ◆ ◇ ♔ ♚ ♖ ♜ ♛ ♕\n");

}

void setpng(int pi, bool g)
{
    piece = pi;
    isforgame = g;
}
void setturn(bool t)
{
    turn = t;
}

void ProcessPressure(int key)
{

    if (!turn)
        return;
    // printf("按了%d\n",key);
    switch (key)
    {
    case 17:
    {
        if (--y < 0 || isTakeUp[y][x] > 0)
        {
            y++;
            break;
        }

        printf(BOROAD_STYLE);
        MOVELEFT(3);

        MOVEUP(1);

        printf(" 😃");
        MOVELEFT(3);

        fflush(stdout);
        break;
    }

    case 31:
    {
        if (++y > 19 || isTakeUp[y][x] > 0)
        {
            y--;
            break;
        }

        printf(BOROAD_STYLE);
        MOVELEFT(3);

        MOVEDOWN(1);

        printf(" 😃");
        MOVELEFT(3);

        fflush(stdout);
        break;
    }

    case 30:
    {
        if (--x < 0 || isTakeUp[y][x] > 0)
        {
            x++;
            break;
        }

        printf(BOROAD_STYLE);
        MOVELEFT(3);

        MOVELEFT(3);

        printf(" 😃");
        MOVELEFT(3);

        fflush(stdout);
        break;
    }

    case 32:
    {
        if (++x > 19 || isTakeUp[y][x] > 0)
        {
            x--;
            break;
        }

        printf(BOROAD_STYLE);
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
        break;

    }
    }

}
bool isMatchEnd(int rel_x, int rel_y)
{
    int num, i;


    i = 1, num = 1;
    while (isTakeUp[rel_y][rel_x - i++] == piece && rel_x - i - 1 >= 0)
        num++;
    i = 1;
    while (isTakeUp[rel_y][rel_x + i++] == piece && rel_x + i - 1 < 20)
        num++;
    if (num == 5)
        return true;

    i = 1, num = 1;
    while (isTakeUp[rel_y - i++][rel_x] == piece && rel_y - i - 1 >= 0)
        num++;
    i = 1;
    while (isTakeUp[rel_y + i++][rel_x] == piece && rel_y + i - 1 < 20)
        num++;
    if (num == 5)
        return true;

    i = 1, num = 1;
    while (isTakeUp[rel_y - i][rel_x - i++] == piece && rel_y - i - 1 >= 0 && rel_x - i - 1 >= 0)
        num++;
    i = 1;
    while (isTakeUp[rel_y + i][rel_x + i++] == piece && rel_y + i - 1 < 20 && rel_x + i - 1 < 20)
        num++;
    if (num == 5)
        return true;

    i = 1, num = 1;
    while (isTakeUp[rel_y + i][rel_x - i++] == piece && rel_y + i - 1 < 20 && rel_x - i - 1 >= 0)
        num++;
    i = 1;
    while (isTakeUp[rel_y - i][rel_x + i++] == piece && rel_y - i - 1 >= 0 && rel_x + i - 1 < 20)
        num++;
    if (num == 5)
        return true;


    return false;
}

void ProcessState(int rel_x, int rel_y, bool self_other)
{
    if (isTakeUp[rel_y][rel_x] > 0)
        return;

    if (self_other)
        turn = false;

    if (self_other)
        isTakeUp[rel_y][rel_x] = piece;
    else if (!self_other && piece == 1)
        isTakeUp[rel_y][rel_x] = 2;
    else if (!self_other && piece == 2)
        isTakeUp[rel_y][rel_x] = 1;

    if (!self_other)
    {
        printf(BOROAD_STYLE);
        MOVELEFT(3);
    }

    if (self_other)
    {
        struct Msg_info MsgInfo = {0};
        if (isMatchEnd(rel_x, rel_y))
        {
            isforgame = false;

            MakeMsg(&MsgInfo, MATCH_END, nickname, socket_atServer, socket_other, "", x, y);
            SendToServer(MsgInfo);

            CLEAR();
            MOVETO(5, 0);
            printf("\t\t\t\t对局胜利！2s后回到大厅\n");
            sleep(2);

            setmnc(false, false);
            SHOW_CURSOR();
            update_visible_list();
            return;
        }
        else
            MakeMsg(&MsgInfo, MATCH_SET_LOCATION, nickname, socket_atServer, socket_other, "", x, y);

        SendToServer(MsgInfo);
    }

    if (!self_other)
    {
        int x_dif = rel_x - x;
        int y_dif = rel_y - y;

        if (x_dif > 0)
            MOVERIGHT(x_dif * 3);
        else if (x_dif < 0)
            MOVELEFT(-x_dif * 3);
        if (y_dif > 0)
            MOVEDOWN(y_dif);
        else if (y_dif < 0)
            MOVEUP(-y_dif);
    }

    if (self_other)
    {
        if (piece == 1)
            printf(" ⚫");
        else
            printf(" ⚪");
    }
    else
    {
        if (piece == 1)
            printf(" ⚪");
        else
            printf(" ⚫");
    }
    MOVELEFT(3);

    while (isTakeUp[rel_y][rel_x] > 0)
    {
        rel_y++;

        if (rel_y > BOARD_SIZE - 1)
        {
            rel_y = 0;
            MOVEUP(20 - 1);
            continue;
        }
        MOVEDOWN(1);
    }

    x = rel_x;
    y = rel_y;

    printf(" 😃");
    MOVELEFT(3);

    fflush(stdout);

    if (!self_other)
        turn = true;
}

void IsPressurekey(void)
{
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
                // printf("anle \n");
                if (!isforgame && ie.value == 1)
                    inputCmd(ie.code);
                else if (isforgame)
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
