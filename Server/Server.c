#include "Server.h"

struct User_info *UsersInfo[MAX_USER_NUMBER];
// struct Msg_info MsgInfoData[MAX_MSG_LIST]={{NOT_USED}};
pthread_t thread_accept;
// pthread_t thread_Recv[MAX_USER_NUMBER];
int id = 0, max_id = 1;
bool Send_ID = false, server_full = false;

void ReleaseSocket(struct User_info *user)
{
    user->isAlive = false;

    pthread_mutex_lock(&id_change);
    if (server_full)
    {

        id = user->id;
        server_full = false;
    }
    pthread_mutex_unlock(&id_change);

    printf("用户%d已经离开！\n", user->handle_socket);
    close(user->handle_socket);
    free(user);
}

void Recv_Msg(struct User_info *user, char *Msg, int len)
{

    int offset = 0, res = 0;
    long flag = 0;

    while (offset < len)
    {
        res = recv(user->handle_socket, Msg + offset, len - offset, flag);
        // printf("res:%d,offset:%d\n",res,offset);
        int times = 0;
        if (res == 0 && times++ == 0)
        {
            if (errno != EINTR)
                ReleaseSocket(user);
        }
        offset += res;
    }
}

void RecvFmClient(void *param)
{
    struct Msg_info MsgInfo = {0};
    struct User_info *user = (struct User_info *)param;
    printf("用户%d的消息接收线程已启动！\n", user->handle_socket);

    while (user->isAlive)
    {
        memset(&MsgInfo, 0, sizeof(MsgInfo));
        Recv_Msg(user, (char *)&MsgInfo, sizeof(struct Msg_info));

        if (MsgInfo.type != NOT_USED)
        {
            printf("recv 用户%d：%s\n", MsgInfo.socket_self, MsgInfo.data);

            struct node *anode = (struct node *)malloc(sizeof(struct node));
            anode->Msginfo = MsgInfo;

            pthread_mutex_lock(&Msg_process);
            list_push(anode);
            pthread_mutex_unlock(&Msg_process);
        }
    }
    printf("用户%d的消息接收线程已                  关闭！\n", user->handle_socket);
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
        for (int i = 0; i < max_id; i++)
        {
            if (UsersInfo[i]->isAlive)
                Send_Msg(UsersInfo[i]->handle_socket, (char *)&MsgInfo, sizeof(struct Msg_info));
        }
    }
    else
        Send_Msg(MsgInfo.socket_other, (char *)&MsgInfo, sizeof(struct Msg_info));
}

void SetID(void *param)
{
    struct User_info *U_I = (struct User_info *)param;
    while (!U_I->SetID)
    {
        // printf("setid\n");
        struct Msg_info Msg = {0};
        MakeMsg(&Msg, SET_ID, "system(0)", 0, U_I->handle_socket, "setid", 0, 0);
        SendaMsg(false, Msg);
    }
    printf("用户%d的 ID set sucessed\n", U_I->handle_socket);
}

void WaitForUser(void *param)
{
    long socket_handle = (long)param;
    pthread_t thread_Recv, setid;
    while (1)
    {

        struct sockaddr_in Ip_port;
        unsigned int whattouse;
        memset(&Ip_port, 0, sizeof(struct sockaddr_in));

        if (!server_full)
        {
            UsersInfo[id] = (struct User_info *)malloc(sizeof(struct User_info));
            memset(UsersInfo[id], 0, sizeof(struct User_info));
            //UsersInfo[id]->address = (char *)malloc(sizeof(20));
        }

        int fffd = -1;
        if (-1 == (fffd = accept(socket_handle, (struct sockaddr *)&Ip_port, &whattouse)))
        {
            printf("accept:用户%d连接失败！\n", id);
            continue;
        }

        if (server_full)
        {
            printf("服务器已满！");
            struct Msg_info mif = {0};
            MakeMsg(&mif, SERVER_FULL, "system", 0, fffd, "", 0, 0);
            SendaMsg(false, mif);
            close(fffd);
            continue;
        }

        UsersInfo[id]->handle_socket = fffd;
        UsersInfo[id]->port = Ip_port.sin_port;

        printf("wwwwwwhat\n");
        //strcpy((char *)(UsersInfo[id])->address, inet_ntoa(Ip_port.sin_addr));
        int leng=strlen(inet_ntoa(Ip_port.sin_addr));
        for(int i=0;i<leng;i++)
        {
            UsersInfo[id]->address[i]=inet_ntoa(Ip_port.sin_addr)[i];
        }
         UsersInfo[id]->address[leng]='\0';
         printf( "%s\n",UsersInfo[id]->address);

        UsersInfo[id]->SetID = false;
        UsersInfo[id]->isAlive = true;
        UsersInfo[id]->id = id;

        // printf("用户%d已连接，handle为：%d,address为：%s,port:%d\n", id-1, UsersInfo[id-1].handle_socket,UsersInfo[id-1].address,Ip_port.sin_port);
        pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, (void *)UsersInfo[id]);
        pthread_create(&setid, NULL, (void *)&SetID, (void *)UsersInfo[id]);

        int num = 0, temp_id = id;
        do
        {
            id++;
            id %= MAX_USER_NUMBER;
            if (num++ == MAX_USER_NUMBER)
            {
                server_full = true;
                max_id = MAX_USER_NUMBER;
                break;
            }
            max_id = id > temp_id ? id : temp_id + 1;
        } while (UsersInfo[id]->isAlive);

        // printf("client(%d)已连接  id:%d",UsersInfo[id-1].handle_socket ,id-1);
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
                for (int i = 0; i < max_id; i++)
                {
                    if (UsersInfo[i]->isAlive && UsersInfo[i]->handle_socket == Msg_list->next->next->Msginfo.socket_other)
                    {
                        // printf("用户%d成功设置id标志位！\n",UsersInfo[i]->handle_socket );
                        UsersInfo[i]->SetID = true;
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
    // for(int i=0;i<MAX_USER_NUMBER;i++)
    // {
    //     UsersInfo[i]=(struct User_info*)malloc(sizeof(struct User_info));
    //     //UsersInfo[i]->address=(char*)malloc(sizeof(char)*20);
    // }

    pthread_t processMsg;
    pthread_mutex_init(&Msg_process, NULL);
    pthread_mutex_init(&id_change, NULL);

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