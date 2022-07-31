#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

char **test;
int p = 0;

pthread_mutex_t lock;

void th(void)
{
    int fd = -1, ret = 0;
    struct input_event ev;

    char dirname[500] = "/dev/input/";
    memset(&ev, 0, sizeof(struct input_event));

    pthread_mutex_lock(&lock);
    strcat(dirname, test[p]);
    dirname[11 + strlen(test[p++])] = '\0';
    pthread_mutex_unlock(&lock);

    fd = open(dirname, O_RDONLY);
    if (fd < 0)
        printf("open %s failed\n", dirname);

    while (1)
    {
        ret = read(fd, &ev, sizeof(struct input_event));
        if (ret < 0)
        {
            printf("read failed!\n");
            break;
        }
        switch (ev.type)
        {
        case EV_KEY:
        {
            printf("按键触发事件(%s)\ttype:%d code:%d value:%d\n", dirname, ev.type, ev.code, ev.value);
            if (ev.code == 272)
                printf("\t按了鼠标左键\n");
            if (ev.code == 273)
                printf("\t按了鼠标右键\n");
            if (ev.code == 274)
                printf("\t按了鼠标中键\n");
            break;
        }

        case EV_ABS:
            printf("鼠标移动事件(%s)\ttype:%d code:%d value:%d\n", dirname, ev.type, ev.code, ev.value);
            break;

        default:
            printf("其他事件(%s)\ttype:%d code:%d value:%d\n", dirname, ev.type, ev.code, ev.value);
        }
    }
    close(fd);
}

int main()
{
    int at = 0;

    test = (char **)malloc(100 * sizeof(char *));
    for (int i = 0; i < 100; i++)
        test[i] = (char *)malloc(500 * sizeof(char));

    pthread_t id[100];
    pthread_mutex_init(&lock, NULL);

    struct dirent *di;
    DIR *dir = NULL;

    dir = opendir("/dev/input");
    if (dir == NULL)
    {
        printf("failed to open directory!");
        return 0;
    }

    while (NULL != (di = readdir(dir)))
    {
        if (di->d_type != DT_DIR && di->d_name[0] != '.')
        {
            strcpy(test[at], di->d_name);
            pthread_create(&id[at++], NULL, (void *)th, NULL);
        }
    }

    for (int i = 0; i < at; i++)
    {
        pthread_join(id[i], NULL);
    }

    for (int i = 0; i < at; i++)
        free(test[i]);
    free(test);

    return 0;
}