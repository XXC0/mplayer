#include <QApplication>
#include "pthread.h"
#include "widget.h"
#include <stdlib.h>
#include <QTextCodec>

int m_pipe[2];
pthread_mutex_t mutex;

void send_cmd(int m_fd,char *cmd)
{
    write(m_fd,cmd,strlen(cmd));
}

void *getTime(void *arg)
{
    Widget *w = (Widget *)arg;
    while(1)
    {

        char buf[1024]="";
        read(m_pipe[0],buf,sizeof(buf));
        char *p = (char *)malloc(sizeof(char)*128);
        strtok(buf,"=");
        int time[2];
        int i =0;
        while(p=strtok(NULL,"="))
        {


            char temp[128];
            sscanf(p,"%s[^\n]",temp);
            time[i%2] = atoi(temp);
            i++;
        }
        w->setSlider(time);
        free(p);
    }
}

void *getSongInfo(void *arg)
{
    Widget *w = (Widget *)arg;

    while(1)
    {
        pthread_mutex_lock(&mutex);
        sleep(1);
        if(w->m_flag == 0)
        {
            char *tmp[2] = {"get_time_pos\n","get_time_length\n"};
            for(int i=0;i<2;i++)
            {
                send_cmd(w->m_fd,tmp[i]);

            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

void* analysis_lrc(void *arg)
{
    Widget *w = (Widget *)arg;
    while(1)
    {
        char lrc_header[4][LRC_SIZE]={0};//二维数组保存歌词前4行
        //LLIST*list=NULL;//初始化list指针,用来维护链表
        init_linklist(&(w->list));//初始化链表
        analysis_lrc(lrc_header,w->list,w->m_LyricName[w->m_th%w->m_SongNum]);
        //print_linklist_head(w->list);
        w->setSongInfo(lrc_header);
        sort_linklist(w->list);//链表根据时间排序
        while(1)
        {

            NODE *pcur=search_lrc_by_time(w->list,w->m_lyricTime);
            if(pcur != NULL)
            {
                char temp[256]= "";
                strcpy(temp,pcur->lrc);
                w->displayLabel(temp);
            }
            if(w->m_currentstate == false)
            {
                for(int i=0;i<128;i++)
                    memset(w->m_LrcInfo_[i],0,strlen(w->m_LrcInfo_[i]));
                w->m_currentstate = true;
                break;
            }
        }
        destroy_linklist(w->list);//销毁链表
    }
}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("windows");
    //解决汉字乱码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");       //UTF-8
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    Widget w;
    w.show();
    pipe(m_pipe);
    pid_t pid = fork();
    if(pid < 0)
        perror("***");
    else if(pid == 0)
    {
        close(m_pipe[0]);
        dup2(m_pipe[1],1);
        execlp("mplayer","mplayer",
               "-slave", "-quiet","-idle",
               "-input", "file=./fifo_cmd","./song/听你听我.mp3",NULL);
    }
    else
    {


        pthread_attr_t attr;
        pthread_mutex_init(&mutex,NULL);
        pthread_attr_init(&attr);//初始化线程属性
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

        pthread_t pthid1,pthid2,pthid3;

        pthread_create(&pthid1,&attr,getTime,(void *)&w);
        pthread_create(&pthid2,&attr,getSongInfo,(void *)&w);
        pthread_create(&pthid3,&attr,analysis_lrc,(void *)&w);
    }

    return a.exec();
}


