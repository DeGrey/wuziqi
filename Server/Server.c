#include "Server.h"

struct User_info UsersInfo[MAX_USER_NUMBER];
// struct Msg_info MsgInfoData[MAX_MSG_LIST]={{NOT_USED}};
pthread_t thread_accept;
// pthread_t thread_Recv[MAX_USER_NUMBER];
int id = 0, M_D_list = 0;
bool Send_ID = false;

void ReleaseSocket(int sk)
{
    printf("连接%d已断开\n",sk);
}

void Recv_Msg(int handle_socket, char *Msg, int len)
{

    int offset = 0, res = 0;
    long flag = 0;
    int er = errno;
    while (offset < len)
    {
        res = recv(handle_socket, Msg + offset, len - offset, flag);
        //printf("res:%d,offset:%d\n",res,offset);
        if (res == 0)
        {
            printf("er:%d,enter:%d\n",er,EINTR);
            // if (er == EINTR)
            //     return;
            // else
            // {
            //     ReleaseSocket(handle_socket);
            // }
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

        if (MsgInfo.type != NOT_USED)
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

void isalive()
{
}

void SendaMsg(bool SenDtype, struct Msg_info MsgInfo)
{
    if (SenDtype)
    {
        for (int i = 0; i < id; i++)
            Send_Msg(UsersInfo[i].handle_socket, (char *)&MsgInfo, sizeof(struct Msg_info));
    }
    else
        Send_Msg(MsgInfo.socket_other, (char *)&MsgInfo, sizeof(struct Msg_info));
}

void SetID(void *param)
{
    struct User_info *U_I = (struct User_info *)param;
    while (!U_I->SetID)
    {
        struct Msg_info Msg = {0};
        MakeMsg(&Msg, SET_ID, "system(0)", 0, U_I->handle_socket, "setid", 0, 0);
        SendaMsg(false, Msg);
    }
    // printf("ID set sucessed\n");
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
        memset(&UsersInfo[id], 0, sizeof(struct User_info));

        if (-1 == (UsersInfo[id].handle_socket = accept(socket_handle, (struct sockaddr *)&Ip_port, &whattouse)))
        {
            printf("accept:用户%d连接失败！\n", id);
            continue;
        }
        UsersInfo[id].port = Ip_port.sin_port;
        UsersInfo[id].address = inet_ntoa(Ip_port.sin_addr);
        UsersInfo[id].SetID = false;
        // printf("用户%d已连接，handle为：%d,address为：%s,port:%d\n", id-1, UsersInfo[id-1].handle_socket,UsersInfo[id-1].address,Ip_port.sin_port);
        pthread_create(&thread_Recv, NULL, (void *)&RecvFmClient, (void *)(long)UsersInfo[id].handle_socket);

        pthread_create(&setid, NULL, (void *)&SetID, (void *)&UsersInfo[id++]);
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
                for (int i = 0; i < id; i++)
                {
                    if (UsersInfo[i].handle_socket == Msg_list->next->next->Msginfo.socket_other)
                    {
                        UsersInfo[i].SetID = true;
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

    pthread_t processMsg;
    pthread_mutex_init(&Msg_process, NULL);
    struct Msg_info Msg = {0};

    if (!Server_init(IP_ADDRESS, 2000))
        return 0;

    pthread_create(&processMsg, NULL, (void *)&ProcessMsg, NULL);

    while (1)
    {
        char test[200];
        scanf("%s", test);
        MakeMsg(&Msg, CHAT_TO_EB, "system", 0, 0, test, 0, 0);
        SendaMsg(true, Msg);
    }

    pthread_join(thread_accept, NULL);

    return 0;
}