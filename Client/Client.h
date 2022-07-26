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
#include <math.h>

#define IP_SERVER "127.0.0.1"/*"120.48.1.142"*///120.48.78.30

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

#define Print(a,b) printf("\033[%dm%s\033[0m",a,b)
// a
#define black 30
#define red 31
#define green 32 
#define yellow 33
#define blue 34
#define purple 35
#define deep_green 36
#define white 37

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

//关闭/打开回显
#define HIDE_INPUT system("stty -echo")
#define UN_HIDE_INPUT system("stty echo")

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
void PreProcess(char *cmd, int socket_other, char *data);
void deleteOneline(int x, int y, int len);
void IsPressurekey(void);
void inputCmd(int);
void update_visible_list();
void isStartMatch(struct Msg_info);


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

void setpng(bool pi,bool g);
void setturn(bool);

struct Text_info
{
    char data[MAX_MSG_SIZE];
    int type;
};




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
void list_pop(struct node* anode);
// {
//     Msg_list->next->next = anode->next;
//     if (anode->next->Msginfo.type == 0)
//         Msg_list = anode->next;
//     free(anode);
// }
