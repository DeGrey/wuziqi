#include "Server.h"

struct User_info UsersInfo[MAX_USER_NUMBER];
// struct Msg_info MsgInfoData[MAX_MSG_LIST]={{NOT_USED}};
pthread_t thread_accept;
// pthread_t thread_Recv[MAX_USER_NUMBER];
int id = 0, M_D_list = 0;

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

void RecvFmClient(void *param)
{
    struct Msg_info MsgInfo = {0};
    int handle_socket = (int)(long)param;
    // printf("消息接收线程已启动！socket:%d\n",handle_socket);
    while (1)
    {
        memset(&MsgInfo, 0, sizeof(MsgInfo));
        Recv_Msg(handle_socket, (char *)&MsgInfo, sizeof(struct Msg_info));

        if (MsgInfo.len > 0)
        {
            // printf("handle_socket：%d,Client：%s,id:%d\n",handle_socket, MsgInfo.data,id);

            struct node *anode = (struct node *)malloc(sizeof(struct node));
            anode->Msginfo = MsgInfo;

            pthread_mutex_lock(&Msg_process);
            list_push(anode);
            pthread_mutex_unlock(&Msg_process);
        }
    }
}

void Send_Msg(int handle_socket, char *Msg, int len)
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

void SendToClient(int handle_socket, char *data, int len, int type)
{
    struct Msg_info MsgInfo = {0};
    MsgInfo.type = type;
    MsgInfo.len = len;
    MsgInfo.handle_socket = 0;
    strcpy(MsgInfo.data, data);

    Send_Msg(handle_socket, (char *)&MsgInfo, sizeof(struct Msg_info));
}

void SendaMsg(char *data, bool type, int handle_socket, int MSG_type)
{
    if (type)
    {
        for (int i = 0; i < id; i++)
        {
            SendToClient(UsersInfo[i].handle_socket, data, strlen(data), MSG_type);
        }
    }
    else
    {
        SendToClient(handle_socket, data, strlen(data), MSG_type);
    }
}

void WaitForUser(void *param)
{
    long socket_handle = (long)param;
    pthread_t thread_Recv;
    while (1)
    {

        struct sockaddr_in Ip_port;
        unsigned int whattouse;
        memset(&Ip_port, 0, sizeof(struct sockaddr_in));
        memset(&UsersInfo[id], 0, sizeof(struct User_info));

        if (-1 == (UsersInfo[id].handle_socket = accept(socket_handle, (struct sockaddr *)&Ip_port, &whattouse)))
        {
            printf("accept:用户%d连接失败！\n", id);
            continue;
        }
        UsersInfo[id].port = Ip_port.sin_port;
        UsersInfo[id].address = inet_ntoa(Ip_port.sin_addr);
        // printf("用户%d已连接，handle为：%d,address为：%s,port:%d\n", id-1, UsersInfo[id-1].handle_socket,UsersInfo[id-1].address,Ip_port.sin_port);
        pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, (void *)(long)UsersInfo[id].handle_socket);
        
        printf("用户 ID:%d 已连接\n",UsersInfo[id].handle_socket);
        SendaMsg("", false, UsersInfo[id++].handle_socket, SET_ID);
    }
}

void ProcessMsg(void)
{

    while (1)
    {
        if (Msg_list->next->next->Msginfo.type != NOT_USED)
        {
            // printf("开始处理\n");
            switch (Msg_list->next->next->Msginfo.type)
            {
            case CHAT_TO_EB:
            {
                SendaMsg(Msg_list->next->next->Msginfo.data, true, 0, CHAT_TO_EB);
                break;
            }
            case CHAT_TO_SB:
            {
                SendaMsg(Msg_list->next->next->Msginfo.data, false, Msg_list->next->next->Msginfo.handle_socket, CHAT_TO_SB);
                // list_pop(Msg_list->next->next);
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

bool Server_init(char *address, int port)
{
    int res_socket;
    // pthread_t thread_accept;
    if ((res_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket:服务器启动失败！\n");
        return false;
    }

    struct sockaddr_in ipv4;
    memset(&ipv4, 0, sizeof(struct sockaddr_in));
    ipv4.sin_family = AF_INET;
    ipv4.sin_port = port;
    inet_aton(address, &ipv4.sin_addr);

    if (-1 == bind(res_socket, (struct sockaddr *)&ipv4, sizeof(struct sockaddr_in)))
    {
        printf("bind:服务器启动失败！\n");
        return false;
    }

    if (-1 == listen(res_socket, __INT_MAX__))
    {
        printf("listen:服务器启动失败！\n");
        return false;
    }

    pthread_create(&thread_accept, NULL, (void *)&WaitForUser, (void *)(long)res_socket);
    return true;
}

int main()
{
    list_init();
    pthread_t processMsg;
    pthread_mutex_init(&Msg_process, NULL);

    if (!Server_init("127.0.0.1", 2000))
        return 0;

    pthread_create(&processMsg, NULL, (void *)&ProcessMsg, NULL);

    while (1)
    {
        char test[200];
        scanf("%s", test);
        SendaMsg(test, true, 0, CHAT_TO_EB);
    }

    pthread_join(thread_accept, NULL);

    return 0;
}