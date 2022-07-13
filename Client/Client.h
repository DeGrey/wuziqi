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

#define MAX_USER_NUMBER 200
#define MAX_MSG_SIZE 200
#define MAX_CLIENT_NAME 10

#define NOT_USED 0
#define SET_ID 100
#define CHAT_TO_SB 101
#define CHAT_TO_EB 102

#define LOGIN "login"

pthread_mutex_t  Msg_process;


struct Msg_info
{
    int type;
    //int len;
    char* nickname;
    int socket_self;
    int socket_other;
    char  data[MAX_MSG_SIZE];
};


struct Msg_info *MakeMsg(int type,char*nickname,int socket_self,int socket_other,char* data)
{
    struct Msg_info *Msg=(struct Msg_info*)malloc(sizeof(struct Msg_info));
    Msg->type=type;
    Msg->nickname=nickname;
    Msg->socket_other=socket_other;
    Msg->socket_self=socket_self;
    strcpy(Msg->data,data);

    return Msg;
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
