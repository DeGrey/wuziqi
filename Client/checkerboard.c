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
            printf(" π«"); //β―ββΌβΊβ»ββββββββ
        }
        if (i == 5 && !turn)
            printf("  ηΆζοΌε―ΉζΉζ­£ε¨θ½ζ£");
        else if (i == 5 && turn)
            printf("  ηΆζοΌε·±ζΉζ­£ε¨θ½ζ£");
        if (i == 7)
            printf("\tζ§εΆοΌ");
        if (i == 8)
            printf("\tWοΌβ"); //\t=6
        if (i == 9)
            printf("\tSοΌβ");
        if (i == 10)
            printf("\tAοΌβ");
        if (i == 11)
            printf("\tDοΌβ");
        if (i == 12)
            printf("\tEnterοΌθ½ζ£");
        printf("\n");
    }

    MOVETO(0, 0);
    printf(" π");
    MOVETO(0, 0);
    
    HIDE_CURSOR();

    system("stty -echo");
    fflush(stdout);
    // MOVETO(10, 10*3-1);
    // printf("βͺ");
    // fflush(stdout);
    // HIDE_CURSOR();
    //  printf("β π« β β§ β― β π π β« βͺ β βΌ βΊ β» β β β β β β β β\n");
    // UpdateBoard(10,10,"π«");
    //  UpdateBoard(10,15,"π");
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

        printf(" π«");
        MOVELEFT(3);

        MOVEUP(1);

        printf(" π");
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

        printf(" π«");
        MOVELEFT(3);

        MOVEDOWN(1);

        printf(" π");
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

        printf(" π«");
        MOVELEFT(3);

        MOVELEFT(3);

        printf(" π");
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

        printf(" π«");
        MOVELEFT(3);

        MOVERIGHT(3);

        printf(" π");
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
    // if (self_other)
    //     turn = false;
    // else
    //     turn = true;

    isTakeUp[rel_x][rel_y] = true;

    if (!piece)
        printf(" β«");
    else
        printf(" βͺ");
    MOVELEFT(3);

    while (isTakeUp[rel_x][rel_y])
    {
        rel_y++;

        if (rel_y > BOARD_SIZE - 1)
        {

            MOVEUP(20);
            continue;
        }
        MOVEDOWN(1);
    }



    x = rel_x;
    y = rel_y;


    printf(" π");
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
        printf("ι?ηηε¬ε€±θ΄₯οΌ\n");
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
