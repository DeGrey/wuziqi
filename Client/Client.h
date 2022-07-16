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
#define ID_ACK 101
#define CHAT_TO_SB 102
#define CHAT_TO_EB 103
#define START_MATCH 104
#define MATCH_ACK 105

#define MATCH_SETLOCATION 201

#define LOGIN "login"


struct Msg_info
{
    int type;
    //int len;
    char  nickname[MAX_MSG_SIZE];
    int socket_self;
    int socket_other;
    char  data[MAX_MSG_SIZE];
};

void SendToServer(struct Msg_info MsgInfo);


void MakeMsg(struct Msg_info *Msg,int type,char*nickname,int socket_self,int socket_other,char* data);
// {
//     Msg->type=type;
//     //Msg->nickname=nickname;
//     Msg->socket_other=socket_other;
//     Msg->socket_self=socket_self;

//     strcpy(Msg->nickname,nickname);
//     strcpy(Msg->data,data);

//     return ;
// }

struct node
{
    struct Msg_info Msginfo;
    struct node *next;
};

void list_init();
// {
//     Msg_list = (struct node *)malloc(sizeof(struct node));
//     Msg_list->next = Msg_list;
//     memset(&Msg_list->Msginfo, 0, sizeof(struct Msg_info));
// }
void list_push(struct node *anode);
// {
//     anode->next = Msg_list->next;
//     Msg_list->next = anode;
//     Msg_list = anode;
// }
void list_pop(struct node *anode);
// {
//     Msg_list->next->next = anode->next;
//     if (anode->next->Msginfo.type == 0)
//         Msg_list = anode->next;
//     free(anode);
// }
