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

#define MATCH_SET_LOCATION 201


// 清除屏幕
#define CLEAR() printf("\033[2J")
// 上移光标
#define MOVEUP(x) printf("\033[%dA", (x))
// 下移光标
#define MOVEDOWN(x) printf("\033[%dB", (x))
// 左移光标
#define MOVELEFT(y) printf("\033[%dD", (y))
// 右移光标
#define MOVERIGHT(y) printf("\033[%dC", (y))
// 定位光标
#define MOVETO(x, y) printf("\033[%d;%dH", (x), (y))
// 光标复位
#define RESET_CURSOR() printf("\033[H")
// 隐藏光标
#define HIDE_CURSOR() printf("\033[?25l")
// 显示光标
#define SHOW_CURSOR() printf("\033[?25h")
//反显
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")


struct Msg_info
{
    int type;
    //int len;
    char  nickname[MAX_MSG_SIZE];
    int socket_self;
    int socket_other;
    char  data[MAX_MSG_SIZE];
    int x;
    int y;
};

void SendToServer(struct Msg_info MsgInfo);


void MakeMsg(struct Msg_info *Msg,int type,char*nickname,int socket_self,int socket_other,char* data,int x,int y);
// {
//     Msg->type=type;
//     //Msg->nickname=nickname;
//     Msg->socket_other=socket_other;
//     Msg->socket_self=socket_self;

//     strcpy(Msg->nickname,nickname);
//     strcpy(Msg->data,data);

//     return ;
// }

void setpiece();

struct node
{
    struct Msg_info Msginfo;
    struct node *next;
};

void list_init(struct node *Msg_list);
// {
//     Msg_list = (struct node *)malloc(sizeof(struct node));
//     Msg_list->next = Msg_list;
//     memset(&Msg_list->Msginfo, 0, sizeof(struct Msg_info));
// }
void list_push(struct node *M_list,struct node *anode);
// {
//     anode->next = Msg_list->next;
//     Msg_list->next = anode;
//     Msg_list = anode;
// }
void list_pop(struct node *M_list,struct node *anode);
// {
//     Msg_list->next->next = anode->next;
//     if (anode->next->Msginfo.type == 0)
//         Msg_list = anode->next;
//     free(anode);
// }
