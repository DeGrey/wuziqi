#include "Server.h"

// struct User_info *UsersInfo[MAX_USER_NUMBER];
//  struct Msg_info MsgInfoData[MAX_MSG_LIST]={{NOT_USED}};
pthread_t thread_accept;
// pthread_t thread_Recv[MAX_USER_NUMBER];
int id = 0, max_id = 1;
bool Send_ID = false, server_full = false;

void ReleaseSocket(struct User_list *anode)
{
    anode->U_I.isAlive = false;

    printf("用户%d的socket已关闭！\n", anode->U_I.handle_socket);

    close(anode->U_I.handle_socket);
}

void Recv_Msg(struct User_list *anode, char *Msg, int len)
{

    int offset = 0, res = 0;
    long flag = 0;

    // printf("正在接受%d的消息\n", anode->U_I.handle_socket);
    while (offset < len)
    {

        res = recv(anode->U_I.handle_socket, Msg + offset, len - offset, flag);

        strerror(errno);
        if (res == 0)
        {
            // printf("这是erroor：%d\n", errno);
            if (errno != EINTR)
            {
                ReleaseSocket(anode);
                return;
            }
            else
                break;
        }
        offset += res;
    }
}

void RecvFmClient(void *param)
{
    struct Msg_info MsgInfo = {0};
    struct User_list *anode = (struct User_list *)param;
    printf("用户%d的消息接收线程已启动！\n", anode->U_I.handle_socket);

    while (anode->U_I.isAlive)
    {
        memset(&MsgInfo, 0, sizeof(MsgInfo));
        Recv_Msg(anode, (char *)&MsgInfo, sizeof(struct Msg_info));

        if (MsgInfo.type != NOT_USED)
        {
            //printf("recv 用户%d：%s\n", MsgInfo.socket_self, MsgInfo.data);

            struct node *anode = (struct node *)malloc(sizeof(struct node));
            anode->Msginfo = MsgInfo;

            pthread_mutex_lock(&Msg_process);
            list_push(anode);
            pthread_mutex_unlock(&Msg_process);
        }
    }

    printf("用户%d的消息接收线程已关闭！\n", anode->U_I.handle_socket);

    pthread_mutex_lock(&user_change);
    UL_delet(anode);
    pthread_mutex_unlock(&user_change);
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

void SendaMsg(bool SenDtype, struct Msg_info MsgInfo)
{
    if (SenDtype)
    {
        for (struct User_list *i = UsersInfo->next; i != NULL; i = i->next)
        {
            if (i->U_I.isAlive)
                Send_Msg(i->U_I.handle_socket, (char *)&MsgInfo, sizeof(struct Msg_info));
        }
    }
    else
        Send_Msg(MsgInfo.socket_other, (char *)&MsgInfo, sizeof(struct Msg_info));
}

void SetID(void *param)
{
    struct User_list *anode = (struct User_list *)param;
    while (!anode->U_I.SetID)
    {
        // printf("setid\n");
        struct Msg_info Msg = {0};
        MakeMsg(&Msg, SET_ID, "system(0)", 0, anode->U_I.handle_socket, "setid", 0, 0);
        SendaMsg(false, Msg);
    }
    printf("用户%d的 ID set sucessed\n", anode->U_I.handle_socket);
}

void WaitForUser(void *param)
{
    long socket_handle = (long)param;

    while (1)
    {
        pthread_t thread_Recv, setid;
        struct sockaddr_in Ip_port;
        unsigned int whattouse;
        memset(&Ip_port, 0, sizeof(struct sockaddr_in));

        struct User_list *anode = (struct User_list *)malloc(sizeof(struct User_list));
        memset(&anode->U_I, 0, sizeof(struct User_info));

        int fffd = -1;
        if (-1 == (fffd = accept(socket_handle, (struct sockaddr *)&Ip_port, &whattouse)))
        {
            printf("accept:用户%d连接失败！\n", id);
            continue;
        }

        anode->U_I.handle_socket = fffd;
        anode->U_I.port = Ip_port.sin_port;

        int leng = strlen(inet_ntoa(Ip_port.sin_addr));
        for (int i = 0; i < leng; i++)
        {
            anode->U_I.address[i] = inet_ntoa(Ip_port.sin_addr)[i];
        }
        anode->U_I.address[leng] = '\0';
        // printf("%s\n", anode->U_I.address);

        anode->U_I.SetID = false;
        anode->U_I.isAlive = true;
        // anode->U_I.id = id;

        // printf("用户%d已连接，handle为：%d,address为：%s,port:%d\n", id-1, UsersInfo[id-1].handle_socket,UsersInfo[id-1].address,Ip_port.sin_port);
        pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, (void *)anode);
        pthread_create(&setid, NULL, (void *)&SetID, (void *)anode);
        anode->U_I.thread_Recv = thread_Recv;

        pthread_mutex_lock(&user_change);
        UL_add(anode);
        pthread_mutex_unlock(&user_change);

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
                // SendaMsg(Msg_list->next->next->Msginfo.data, true, 0, CHAT_TO_EB);
                // printf("用户%d：%s\n",Msg_list->next->next->Msginfo.socket_self,Msg_list->next->next->Msginfo.data);
                SendaMsg(true, Msg_list->next->next->Msginfo);
                break;
            }
            // case CHAT_TO_SB:
            // {
            //     // SendaMsg(Msg_list->next->next->Msginfo.data, false, Msg_list->next->next->Msginfo.handle_socket, CHAT_TO_SB);
            //     SendaMsg(false, Msg_list->next->next->Msginfo);
            //     // list_pop(Msg_list->next->next);
            //     break;
            // }
            case ID_ACK:
            {
                // Send_ID = true;
                // printf("受到了 %d 的ID确认\n", Msg_list->next->next->Msginfo.socket_other);
                for (struct User_list *i = UsersInfo->next; i != NULL; i = i->next)
                {
                    if (i->U_I.isAlive && i->U_I.handle_socket == Msg_list->next->next->Msginfo.socket_other)
                    {
                        // printf("用户%d成功设置id标志位！\n",UsersInfo[i]->handle_socket );
                        i->U_I.SetID = true;
                        break;
                    }
                }
                break;
            }
            // case START_MATCH:
            // {
            //     SendaMsg(false, Msg_list->next->next->Msginfo);
            //     break;
            // }
            // case MATCH_ACK:
            // {
            //     SendaMsg(false, Msg_list->next->next->Msginfo);
            //     break;
            // }
            default:
            {
                printf("default项\n");
                SendaMsg(false, Msg_list->next->next->Msginfo);
                break;
            }
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
    UL_init();
    // for(int i=0;i<MAX_USER_NUMBER;i++)
    // {
    //     UsersInfo[i]=(struct User_info*)malloc(sizeof(struct User_info));
    //     //UsersInfo[i]->address=(char*)malloc(sizeof(char)*20);
    // }

    pthread_t processMsg;
    pthread_mutex_init(&Msg_process, NULL);
    pthread_mutex_init(&user_change, NULL);

    struct Msg_info Msg = {0};

    if (!Server_init(IP_ADDRESS, 2000))
        return 0;

    pthread_create(&processMsg, NULL, (void *)&ProcessMsg, NULL);

    while (1)
    {
        char test[200];
        scanf("%s", test);
        MakeMsg(&Msg, CHAT_TO_EB, "system", 0, 0, test, 0, 0);
        printf("命令项目\n");
        SendaMsg(true, Msg);
    }

    pthread_join(thread_accept, NULL);

    return 0;
}