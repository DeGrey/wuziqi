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

    Msg->x = x;
    Msg->y = y;

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

int getCmd(char *obj, char *allm, int location, int len)
{
    int k = 0;
    for (int i = location; i < len; i++)
    {
        if (allm[i] != ' ')
            *(obj + k++) = allm[i];
        else
            break;
        location++;
    }
    *(obj + k) = '\0';

    return ++location;
}

void processinput(char *allm)
{
    // printf("allM:%s\n", allm);
    int len = strlen(allm);
    int k = 0;
    int location = 0;

    for (int i = 0; i < len; i++)
    {
        if (allm[i] == ' ')
            k++;
    }
    if (k < 1 || k > 2)
    {
        return;
    }

    char data[MAX_MSG_SIZE]; // = (char *)malloc(sizeof(char) * MAX_MSG_SIZE);
    char cmd[11];            //= (char *)malloc(sizeof(char) * 10);
    char sokt[4];            // = (char *)malloc(sizeof(char) * 10);

    location = getCmd(cmd, allm, location, len);
    location = getCmd(sokt, allm, location, len);
    location = getCmd(data, allm, location, len);

    int sok = 0, p = 0;
    len = strlen(sokt);
    for (int i = len - 1; i >= 0; i--)
    {
        sok += ((int)(sokt[i] - '0')) * pow(10, p);
        p++;
    }

    // printf("last:%s,%d,%s\n", cmd, sok, data);

    PreProcess(cmd, sok, data);
}

void GetFromStdio()
{
    char buffer[MAX_MSG_SIZE];
    char ch;
    int counter = 0;
    bool isio = false;

    do
    {
        ch = getchar();
        if (isio)
            buffer[counter++] = ch;

        if (ch == 'i' || ch == 'I')
            isio = true;
    } while (ch != '\n');

    if (counter <= 6)
        return;

    buffer[counter - 1] = '\0';

    // printf("kkk%s\n", buffer);
    //  char* allm=(char*)malloc(sizeof(char)*200);
    //  strcpy(allm,buffer);
    //  printf("%s/n",allm);
    processinput(buffer);
}

bool iscmd()
{

    pthread_mutex_lock(&Visible_Msg_process);

    MOVETO(11, 0);
    Print(blue, "命令：");
    fflush(stdout);

    GetFromStdio();

    //
    // deleteOneline(11, 0 /*7*/, 7 + /*strlen(ToPro)*/80 + 10 + 1 + 3 + 1);

    pthread_mutex_unlock(&Visible_Msg_process);

    if (!ismatch)
        update_visible_list();

    return true;
}

void inputCmd(int key)
{
    if (!HasInit || ismatch)
        return;

    HIDE_INPUT;
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

    if (!ismatch)
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
    while (isinCmd)
        ;

    ismatch = true;
    isinCmd = true;

    CLEAR();
    MOVETO(5, 0);
    char mg[100];
    strcpy(mg, Mi.nickname);
    strcat(mg, " 邀请你下五子棋！（y/n）:");
    // printf("start 0 match\n");
    Print(red, mg);
    fflush(stdout);

    // char ch;
    // do
    // {
    //     ch = getchar();
    // } while (ch !='\n');

    // ch=getchar();

    char ch;
    ch = getchar();
    // printf("ch:%c\n", ch);

    // return;

    struct Msg_info msg = {0};
    if (ch == 'y')
        MakeMsg(&msg, MATCH_ACK, nickname, socket_atServer, Mi.socket_self, "yes", 0, 0);
    else
        MakeMsg(&msg, MATCH_ACK, nickname, socket_atServer, Mi.socket_self, "no", 0, 0);

    SendToServer(msg);

    if (ch == 'y')
    {
        setpng(true, true);
        setturn(false);
        InitBoard(Mi.socket_self);
    }
    else
    {
        ismatch = false;
        isinCmd = false;
        update_visible_list();
    }
}

void ProcessMsg(void)
{
    // printf("消息处理已开启\n");
    struct node *anode = {0};
    anode = Msg_list->next->next;
    while (1)
    {
        // while(isinCmd);

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
                    // printf("ID分配成功\n%s(ID：%d)  \n", nickname, Msg_list->next->next->Msginfo.socket_other);
                }
                break;
            }
            case START_MATCH:
            {
                isStartMatch(Msg_list->next->next->Msginfo);
                break;
            }
            case MATCH_ACK:
            {
                if (0 == strcmp(Msg_list->next->next->Msginfo.data, "yes"))
                {
                    ismatch = true;
                    setpng(false, true);
                    setturn(true);
                    InitBoard(Msg_list->next->next->Msginfo.socket_self);
                }
                else
                {
                    // printf("对方以及拒绝\n");
                    Print(red, "对方已拒绝对局！一秒后回到大厅");
                    printf("\n");
                    ismatch = false;
                    isinCmd = false;
                    // sleep(1);
                    update_visible_list();
                }
                break;
            }
            case MATCH_SET_LOCATION:
            {
                //printf("%d,%d\n", Msg_list->next->next->Msginfo.x, Msg_list->next->next->Msginfo.y);
                ProcessState(Msg_list->next->next->Msginfo.x, Msg_list->next->next->Msginfo.y, false);
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
        ismatch = true;
        isinCmd = false;
        CLEAR();
        MOVETO(0, 0);
        Print(red, "正在等待对局确认!");
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
    // SHOW_CURSOR();
    // // return 0;
    // // hhhhhhhhh

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

    // InitBoard(0);
    // while (1)
    // {
    //     /* code */
    // }

    pthread_mutex_init(&Msg_process, NULL);
    pthread_mutex_init(&Visible_Msg_process, NULL);

    if (!contoserver(IP_SERVER, 2000))
    {
        UN_HIDE_INPUT;
        return 0;
    }

    printf("连接成功！\n正在分配ID...\n");

    pthread_t thread_Recv, thread_proMsg;
    pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, NULL);
    pthread_create(&thread_proMsg, NULL, (void *)&ProcessMsg, NULL);

    while (socket_atServer == 0)
        ;

    printf("ID分配成功！\n");

    printf("正在连接大厅...\n");
    sleep(2);
    // printf("%s(ID:%d)已进入大厅\n", nickname, socket_atServer);

    struct Msg_info tongzhi = {0};
    MakeMsg(&tongzhi, CHAT_TO_EB, nickname, socket_atServer, 0, "已进入大厅！", 0, 0);
    SendToServer(tongzhi);

    HasInit = true;

    pthread_join(thread_Recv, NULL);
    pthread_join(thread_proMsg, NULL);
    pthread_join(checkPrs, NULL);

    // InitBoard(0);

    SHOW_CURSOR();
    return 0;
}