#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
//#include <tcp.h>

#define IP_ADDRESS /*"192.168.0.4" */ /*"192.168.16.4"*/ "127.0.0.1"

#define MAX_MSG_LIST 2000

#define MAX_USER_NUMBER 200
#define MAX_MSG_SIZE 200

#define NOT_USED 0
#define SET_ID 100
#define ID_ACK 101
#define CHAT_TO_SB 102
#define CHAT_TO_EB 103
#define START_MATCH 104
#define MATCH_ACK 105
#define SERVER_FULL 106

#define MATCH_SET_LOCATION 201
#define MATCH_END 202

pthread_mutex_t Msg_process, user_change;

struct User_info
{
    int handle_socket;
    int port;
    char address[20];
    bool SetID;
    bool isAlive;
    pthread_t thread_Recv;
    int id;
};

struct User_list
{
    struct User_info U_I;
    struct User_list *next;
};

struct User_list *UsersInfo;
void UL_init()
{
    UsersInfo = (struct User_list *)malloc(sizeof(struct User_list));
    memset(&UsersInfo->U_I, 0, sizeof(struct User_info));
    UsersInfo->next = NULL;
}

void UL_add(struct User_list *anode)
{
    struct User_list *p;
    p = UsersInfo;

    while (p->next)
        p = p->next;

    anode->next = p->next;
    p->next = anode;
}

void UL_delet(struct User_list *anode)
{
    struct User_list *p;
    p = UsersInfo;

    while (p->next != anode)
        p = p->next;

    p->next = anode->next;
    printf("用户%d的资源已经释放\n\n", anode->U_I.handle_socket);
    free(anode);
}

struct Msg_info
{
    int type;
    // int len;
    char nickname[MAX_MSG_SIZE];
    int socket_self;
    int socket_other;
    char data[MAX_MSG_SIZE];
    int x;
    int y;
};

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

struct node
{
    struct Msg_info Msginfo;
    struct node *next;
};

struct node *Msg_list;
void list_init()
{
    Msg_list = (struct node *)malloc(sizeof(struct node));
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