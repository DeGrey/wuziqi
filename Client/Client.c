#include "Client.h"

int handle_socket = 0;
char* nickname={0};
int socket_atServer=0;
void contoserver(char *address, int port)
{
    int res_socket;
    if ((handle_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket:初始化失败！\n");
    }

    struct sockaddr_in ipv4;
    ipv4.sin_family = AF_INET;
    ipv4.sin_port = port;
    inet_aton(address, &ipv4.sin_addr);
    if (-1 == connect(handle_socket, (struct sockaddr *)&ipv4, sizeof(struct sockaddr_in)))
    {
        printf("connect failed!\n");
        return;
    }
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

void SendToServer(char *data, int len, int socket, int type)
{
    struct Msg_info MsgInfo = {0};
    MsgInfo.type = type;
    MsgInfo.len = len;
    MsgInfo.handle_socket = socket;
    MsgInfo.nickname = nickname;

    strcpy(MsgInfo.data, data);

    Send_Msg((char *)&MsgInfo, sizeof(struct Msg_info));
}

void Recv_Msg(int handle_socket, char *Msg, int len)
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
    while (1)
    {
        struct Msg_info MsgInfo = {0};
        Recv_Msg(handle_socket, (char *)&MsgInfo, sizeof(struct Msg_info));

        if (MsgInfo.len > 0)
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

    while (1)
    {
        if (Msg_list->next->next->Msginfo.type != NOT_USED)
        {
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
               socket_atServer=Msg_list->next->next->Msginfo.handle_socket;
               printf("得到ID：%d\n",socket_atServer);
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

void PreProcess(char *cmd, int socket, char *Msg)
{
    if(cmd==LOGIN)
    {

    }
    if (!socket)
        SendToServer(Msg, strlen(Msg), 0, CHAT_TO_EB);
    else
        SendToServer(Msg, strlen(Msg), socket, CHAT_TO_SB);
}

int main(int argc,char* argv[])//int argc,char* argv[],char* envp[]
{
    nickname=argv[1];
        
    printf("正在连接服务器...\n");

    pthread_t thread_Recv, thread_proMsg;
    list_init();
    pthread_mutex_init(&Msg_process, NULL);

    contoserver("127.0.0.1", 2000);

    printf("连接成功！\n正在分配ID...\n");

    pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, NULL);
    pthread_create(&thread_proMsg, NULL, (void *)&ProcessMsg, NULL);

    // while(!socket_atServer)
    // {
        
    // }
    printf("正在连接大厅...\n");
    printf("%s(ID：%d)  \n",nickname,socket_atServer);

    while (1)
    {
        char cmd[100];
        int obj;
        char msg[MAX_MSG_SIZE];

        scanf("%s %d %s", cmd, &obj, msg);
        PreProcess(cmd, obj, msg);
    }

    pthread_join(thread_Recv, NULL);
    pthread_join(thread_proMsg, NULL);

    return 0;
}