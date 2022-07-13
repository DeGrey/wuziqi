#include "Client.h"

int handle_socket = 0;
char *nickname = {0};
int socket_atServer = 0;

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

void SendToServer(struct Msg_info* MsgInfo)//(char *data, int socket_self, int type,int socket_other)
{
    // struct Msg_info MsgInfo = {0};
    // MsgInfo.type = type;
    // MsgInfo.socket_self=socket_self;
    // MsgInfo.socket_other=socket_other;
    // MsgInfo.nickname = nickname;
    // strcpy(MsgInfo.data, data);

    Send_Msg((char *)MsgInfo, sizeof(struct Msg_info));
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
    printf("消息接受已打开\n");
    while (1)
    {
        struct Msg_info MsgInfo = {0};
        Recv_Msg((char *)&MsgInfo, sizeof(struct Msg_info));

        if (MsgInfo.type!=NOT_USED)
        {
            struct node *anode = (struct node *)malloc(sizeof(struct node));
            anode->Msginfo = MsgInfo;

            pthread_mutex_lock(&Msg_process);
            list_push(anode);
            pthread_mutex_unlock(&Msg_process);
        }
    }
}

void ProcessMsg(void)
{
printf("消息处理已开启\n");
    while (1)
    {
        if (Msg_list->next->next->Msginfo.type != NOT_USED)
        {
            printf("收到消息了\n");
            switch (Msg_list->next->next->Msginfo.type)
            {
            case CHAT_TO_EB:
            {
                // SendaMsg(Msg_list->next->next->Msginfo.data, true, 0);

                printf("%s：%s\n", Msg_list->next->next->Msginfo.nickname, Msg_list->next->next->Msginfo.data);
                // list_pop(Msg_list->next->next);
                break;
            }
            case CHAT_TO_SB:
            {
                printf("%s To %s：%s", Msg_list->next->next->Msginfo.nickname, nickname, Msg_list->next->next->Msginfo.data);
                // SendaMsg(Msg_list->next->next->Msginfo.data, false, Msg_list->next->next->Msginfo.handle_socket);
                // list_pop(Msg_list->next->next);
                break;
            }
            case SET_ID:
            {
                socket_atServer = Msg_list->next->next->Msginfo.socket_other;
                printf("得到ID：%d\n", socket_atServer);

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

void PreProcess(char *cmd, int socket_other,char*data)
{
    struct Msg_info* MsgInfo;
    if (0 == strcmp(cmd, "chat"))
    {
        if(NOT_USED==socket_other)
        MsgInfo=MakeMsg(CHAT_TO_EB,nickname,socket_atServer,socket_other,data);
        else
        MsgInfo=MakeMsg(CHAT_TO_SB,nickname,socket_atServer,socket_other,data);

        //printf("cmd==chat\n");
        //if (NOT_USED == socket)
            //SendToServer(MsgInfo);//(Msg,socket_atServer,CHAT_TO_EB,socket_Sb);
        // else
        //     SendToServer(MsgInfo);//(Msg,socket_atServer,CHAT_TO_SB,socket_Sb);
    }
    if(0 == strcmp(cmd, "getid"))
    {
        //SendToServer(MsgInfo);//(Msg,NOT_USED,SET_ID,socket_Sb);
    }

    SendToServer(MsgInfo);
}

int main(int argc, char *argv[]) // int argc,char* argv[],char* envp[]
{
    nickname = (char *)malloc(sizeof(char));
    strcpy(nickname, argv[1]);

    printf("正在连接服务器...\n");


    list_init();
    pthread_mutex_init(&Msg_process, NULL);

    if (!contoserver("127.0.0.1", 2000))
        return 0;

    printf("连接成功！\n正在分配ID...\n");


    pthread_t thread_Recv, thread_proMsg;
    pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, NULL);
    pthread_create(&thread_proMsg, NULL, (void *)&ProcessMsg, NULL);


    //PreProcess("getid",handle_socket,"");
    // while(!socket_atServer)
    // {

    // }
    printf("正在连接大厅...\n");
    printf("%s(ID：%d)  \n", nickname, socket_atServer);

    while (1)
    {
        char cmd[100];
        int obj;
        char msg[MAX_MSG_SIZE];

        scanf("%s %d %s", cmd, &obj, msg);
        // printf("%s %d %s", cmd, obj, msg);
        PreProcess(cmd, obj, msg);
    }

    pthread_join(thread_Recv, NULL);
    pthread_join(thread_proMsg, NULL);

    return 0;
}