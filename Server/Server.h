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
//#include<list>

#define MAX_MSG_LIST 2000

#define MAX_USER_NUMBER 200
#define MAX_MSG_SIZE 200

#define NOT_USED 0
#define CHAT_TO_SB 101
#define CHAT_TO_EB 102

pthread_mutex_t  Msg_process;


struct User_info
{
    int handle_socket;
    int port;
    char *address;
};

struct Msg_info
{
    int type;
    int len;
    int handle_socket;
    char data[MAX_MSG_SIZE];
};

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