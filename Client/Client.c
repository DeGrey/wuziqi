#include "Client.h"
#include "checkerboard.h"

int handle_socket = 0;
char *nickname;
int socket_atServer = 0;
pthread_mutex_t Msg_process, Visible_Msg_process;
struct node *Msg_list;
int V_M_L_N = 0;
bool ismatch = false, HasInit = false, isinCmd = false;
struct Text_info Text[10];
int Text_num = 0;

void list_init()
{
    // Msg_list = (struct node *)malloc(sizeof(struct node));
    Msg_list->next = Msg_list;
    memset(&Msg_list->Msginfo, 0, sizeof(struct Msg_info));
}
void list_push(struct node *anode)
{
    anode->next = Msg_list->next;
    Msg_list->next = anode;
    Msg_list = anode;
}
void list_pop(struct node *anode)
{
    Msg_list->next->next = anode->next;
    if (anode->next->Msginfo.type == 0)
        Msg_list = anode->next;
    free(anode);
}

void MakeMsg(struct Msg_info *Msg, int type, char *nickname, int socket_self, int socket_other, char *data, int x, int y)
{
    Msg->type = type;
    // Msg->nickname=nickname;
    Msg->socket_other = socket_other;
    Msg->socket_self = socket_self;

    strcpy(Msg->nickname, nickname);
    strcpy(Msg->data, data);

    return;
}

bool contoserver(char *address, int port)
{
    int res_socket;
    if ((handle_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("初始化失败！\n");
        return false;
    }

    struct sockaddr_in ipv4;
    ipv4.sin_family = AF_INET;
    ipv4.sin_port = port;
    inet_aton(address, &ipv4.sin_addr);
    if (-1 == connect(handle_socket, (struct sockaddr *)&ipv4, sizeof(struct sockaddr_in)))
    {
        printf("connect failed!\n");
        return false;
    }
    return true;
}

void Send_Msg(char *Msg, int len)
{
    int offset = 0, res = 0;
    long flag = 0;

    while (offset < len)
    {
        if (-1 == (res = send(handle_socket, Msg + offset, len - offset, flag)))
        {
            printf("发送数据失败!\n");
            return;
        }
        offset += res;
    }
}

void SendToServer(struct Msg_info MsgInfo) //(char *data, int socket_self, int type,int socket_other)
{
    // struct Msg_info MsgInfo = {0};
    // MsgInfo.type = type;
    // MsgInfo.socket_self=socket_self;
    // MsgInfo.socket_other=socket_other;
    // MsgInfo.nickname = nickname;
    // strcpy(MsgInfo.data, data);
    Send_Msg((char *)&MsgInfo, sizeof(struct Msg_info));
}

void Recv_Msg(char *Msg, int len)
{

    int offset = 0, res = 0;
    long flag = 0;
    while (offset < len)
    {
        if (-1 == (res = recv(handle_socket, Msg + offset, len - offset, flag)))
        {
            return;
        }
        offset += res;
    }
}

void RecvFmClient(void)
{
    // printf("消息接受已打开\n");
    while (1)
    {
        struct Msg_info MsgInfo = {0};
        Recv_Msg((char *)&MsgInfo, sizeof(struct Msg_info));

        if (MsgInfo.type != NOT_USED)
        {
            struct node *anode = (struct node *)malloc(sizeof(struct node));
            anode->Msginfo = MsgInfo;

            pthread_mutex_lock(&Msg_process);
            list_push(anode);
            pthread_mutex_unlock(&Msg_process);
        }
    }
}
char *Setdata(char *name, int id, char *data)
{
    int num;
    char *n = (char *)malloc(sizeof(char));
    n += sizeof(char);

    *(n--) = '\0';
    *(n--) = ':';
    *(n--) = ')';
    while (id)
    {
        num = id % 10;
        *(n--) = '0' + num;
        id /= 10;
    }
    *n-- = ':';
    *n-- = 'D';
    *n-- = 'I';
    *n = '(';

    char *nn = (char *)malloc(200);
    strcpy(nn, name);

    strcat(nn, n);
    strcat(nn, data);

    return nn;
}

void deleteOneline(int x, int y, int len)
{
    MOVETO(x, y);
    for (int i = 0; i < len; i++)
    {
        printf(" ");
    }
    MOVETO(x, y);
    fflush(stdout);
}

void processinput(char *allM)
{

    int k = 0;
    int len = strlen(allM) - 1;

    while (*++allM != '\n')
    {
        if (*allM == ' ')
            k++;
    }
    // printf("k=%d\n", k);
    if (k < 1)
        return;

    char *cmd = (char *)malloc(sizeof(char));
    int sok = 0;
    char *data = (char *)malloc(sizeof(char));

    *cmd-- = '\0';

    if (k != 1)
    {
        *data-- = '\0';
        while (*--allM != ' ')
        {
            *data-- = *allM;
            len--;
        }
        data++;
    }

    int p = 0;
    while (*--allM != ' ')
    {
        sok += ((int)(*allM - '0')) * pow(10, p);
        p++;
        len--;
    }

    if (k == 1)
        len -= 1;
    else
        len -= 2;

    while (len > 0)
    {
        *cmd-- = *--allM;
        len--;
    }
    cmd++;
    // printf("%s,%d,%s\n", cmd, sok, data);

    PreProcess(cmd, sok, data);
}

bool iscmd()
{

    pthread_mutex_lock(&Visible_Msg_process);

    MOVETO(11, 0);
    Print(blue, "命令：");
    fflush(stdout);

    char ch;
    do
    {
        ch = getchar();
    } while (ch != 'i' && ch != 'I');

    char cah[100];
    fgets(cah, 100, stdin);
    char *ToPro = (char *)malloc(sizeof(char) * strlen(cah) + 1);

    strcpy(ToPro, cah);
    // printf("%s\n", ToPro);
    //  char cmd[100];
    //  int obj = -1;
    //  char msg[MAX_MSG_SIZE];

    // scanf("%s %d %s", cmd, &obj, msg);

    // while ((ch = getchar()) != '\n')
    //     ;

    // if (NOT_USED == socket_atServer)
    //     continue;
    // if (obj < 0)
    //     continue;
    //     MOVETO(11, 0);

    deleteOneline(11, 0 /*7*/, 7 + strlen(ToPro) + 10 + 1 + 3 + 1);

    pthread_mutex_unlock(&Visible_Msg_process);

    processinput(ToPro);
    // PreProcess(cmd, obj, msg);
    //  update_visible_list();
    //
    return true;
}

void inputCmd(int key)
{
    if (!HasInit||ismatch)
        return;
    isinCmd = true;

    switch (key)
    {
    case 23:
    {
        iscmd();
        break;
    }

    default:
        break;
    }

    isinCmd = false;
}

void update_visible_list()
{
    // printf("update\n");
    pthread_mutex_lock(&Visible_Msg_process);
    CLEAR();
    MOVETO(0, 0);
    for (int i = Text_num; i < 10; i++)
    {
        if (Text[i].type == NOT_USED)
            continue;
        if (Text[i].type == CHAT_TO_SB)
        {
            Print(green, Text[i].data);
            printf("\n");
            continue;
        }
        printf("%s\n", Text[i].data);
    }

    for (int i = 0; i < Text_num; i++)
    {
        if (Text[i].type == NOT_USED)
            continue;
        if (Text[i].type == CHAT_TO_SB)
        {
            Print(green, Text[i].data);
            printf("\n");
            continue;
        }
        printf("%s\n", Text[i].data);
    }

    pthread_mutex_unlock(&Visible_Msg_process);
}

void insertTOText(struct node *anode)
{
    strcpy(Text[Text_num].data, Setdata(anode->Msginfo.nickname, anode->Msginfo.socket_self, anode->Msginfo.data));
    Text[Text_num++].type = anode->Msginfo.type;
    Text_num %= 10;

    if (!ismatch)
        update_visible_list();
}

void isStartMatch(struct Msg_info Mi)
{
    ismatch = true;


    CLEAR();
    MOVETO(5, 0);
    char mg[100];
    strcpy(mg, Mi.nickname);
    strcat(mg, " 邀请你下五子棋！（y/n）:");
        //printf("start 0 match\n");
    Print(red, mg);
    fflush(stdout);

    char ch;
    do
    {
        ch = getchar();
    } while (ch !='\n');

    // ch=getchar();
    char s[2];
    scanf("%s",s);
    
printf("ch:%s\n",s);
return;
    struct Msg_info msg = {0};
    // if (s == 'y')
    //     MakeMsg(&msg, MATCH_ACK, nickname, socket_atServer, Mi.socket_self, "yes", 0, 0);
    // else
    //     MakeMsg(&msg, MATCH_ACK, nickname, socket_atServer, Mi.socket_self, "no", 0, 0);

    // SendToServer(msg);

    // if(s=='y')
    // {
    //     setpng(true,true);
    //     setturn(false);
    //     InitBoard(Mi.socket_self);
    // }
    // else
    // {
    //     ismatch=false;
    //     update_visible_list();
    // }
}

void ProcessMsg(void)
{
    // printf("消息处理已开启\n");
    while (1)
    {
        if (Msg_list->next->next->Msginfo.type != NOT_USED)
        {
            // printf("收到消息了\n");
            switch (Msg_list->next->next->Msginfo.type)
            {
            case CHAT_TO_EB:
            {
                insertTOText(Msg_list->next->next);
                // printf("shoudao：%s\n",Msg_list->next->next->Msginfo.data);
                break;
            }
            case CHAT_TO_SB:
            {
                insertTOText(Msg_list->next->next);
                break;
            }
            case SET_ID:
            {
                if (socket_atServer == NOT_USED)
                {
                    socket_atServer = Msg_list->next->next->Msginfo.socket_other;
                    Msg_list->next->next->Msginfo.type = ID_ACK;
                    SendToServer(Msg_list->next->next->Msginfo);
                    printf("ID分配成功\n%s(ID：%d)  \n", nickname, Msg_list->next->next->Msginfo.socket_other);
                }
                break;
            }
            case START_MATCH:
            {
                isStartMatch(Msg_list->next->next->Msginfo);
            }
            case MATCH_ACK:
            {
                if(0==strcmp(Msg_list->next->next->Msginfo.data,"yes"))
                {
                    ismatch=true;
                    setpng(false,true);
                    setturn(true);
                    InitBoard(Msg_list->next->next->Msginfo.socket_self);
                }
                else
                {
                    Print(red,"对方已拒绝对局！一秒后回到大厅。。。");
                    printf("\n");
                    ismatch=false;
                    sleep(1);
                    update_visible_list();
                }
                break;
            }

            default:
                break;
            }

            pthread_mutex_lock(&Msg_process);
            list_pop(Msg_list->next->next);
            pthread_mutex_unlock(&Msg_process);
        }
    }
}

void PreProcess(char *cmd, int socket_other, char *data)
{
    // printf("cmd:%s,socket:%d,data:%s\n", cmd, socket_other, data);

    struct Msg_info MsgInfo = {0};
    if (0 == strcmp(cmd, "chat"))
    {
        if (NOT_USED == socket_other)
            MakeMsg(&MsgInfo, CHAT_TO_EB, nickname, socket_atServer, socket_other, data, 0, 0);

        else
            MakeMsg(&MsgInfo, CHAT_TO_SB, nickname, socket_atServer, socket_other, data, 0, 0);
    }

    else if (0 == strcmp(cmd, "match"))
    {
        MakeMsg(&MsgInfo, START_MATCH, nickname, socket_atServer, socket_other, data, 0, 0);
        ismatch=true;
        isinCmd = false;
        CLEAR();
        MOVETO(0,0);
        Print(red,"正在等待对局确认!");
        printf("\n");
        fflush(stdout);
        // return;
    }

    else
    {
        // printf("找不到命令：%s\n", cmd);
        return;
    }

    SendToServer(MsgInfo);
}
void checkCur(void)
{
    int times = 0;
    while (1)
    {

        if (!isinCmd && times == 0)
        {
            HIDE_INPUT;
            times++;
        }

        else if (isinCmd && times == 1)
        {
            UN_HIDE_INPUT;
            times--;
        }
    }
}

int main(int argc, char *argv[])
{
    CLEAR();
    MOVETO(0, 0);
    fflush(stdout);

    nickname = (char *)malloc(sizeof(char));
    strcpy(nickname, argv[1]);

    printf("正在连接服务器...\n");
    Msg_list = (struct node *)malloc(sizeof(struct node));

    list_init();

    for (int i = 0; i < 10; i++)
        memset(&Text[i], 0, sizeof(struct Text_info));

    pthread_t checkPrs, checkCurs;
    pthread_create(&checkPrs, NULL, (void *)&IsPressurekey, NULL);
    pthread_create(&checkCurs, NULL, (void *)&checkCur, NULL);

    pthread_mutex_init(&Msg_process, NULL);
    pthread_mutex_init(&Visible_Msg_process, NULL);

    if (!contoserver("127.0.0.1", 2000))
    {
        // return 0;
    }

    printf("连接成功！\n正在分配ID...\n");

    pthread_t thread_Recv, thread_proMsg;
    pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, NULL);
    pthread_create(&thread_proMsg, NULL, (void *)&ProcessMsg, NULL);

    while (socket_atServer == 0)
        ;

    printf("ID分配成功！\n");

    printf("正在连接大厅...\n");
    printf("%s(ID:%d)已进入大厅\n", nickname, socket_atServer);
    HasInit = true;

    // while (1)
    // {

    //     char cmd[100];
    //     int obj = -1;
    //     char msg[MAX_MSG_SIZE] = "";

    //     scanf("%s %d %s", cmd, &obj, msg);//未完全消除错误命令输入的影响
    //     if (NOT_USED == socket_atServer)
    //         continue;
    //     if (obj <0)
    //         continue;

    //     PreProcess(cmd, obj, msg);
    // }

    pthread_join(thread_Recv, NULL);
    pthread_join(thread_proMsg, NULL);
    pthread_join(checkPrs, NULL);

    // InitBoard(0);

    SHOW_CURSOR();
    return 0;
}