#include "lrc.h"
/**************************************************************************
*	函数功能:初始化双向链表
*	参数：
*		list  LLIST结构体指针的地址
*
* 	返回值:无
**************************************************************************/
void init_linklist(LLIST** list1)
{
    NODE *header = (NODE *)malloc(sizeof(NODE));
    header->prev =NULL;
    header->next = NULL;
    header->times = 0;

    LLIST *tmp_list = (LLIST *)malloc(sizeof(LLIST));
    tmp_list->head = header;
    tmp_list->tail = header;
    tmp_list->size = 0;
    *list1 =tmp_list;
}

/**************************************************************************
*	函数功能:把新的节点插入到链表
*	参数：
*		list    LLIST结构体指针的地址
*		pnew    新的节点的地址
* 	返回值:无
**************************************************************************/
void insert_linklist_header(LLIST*list,NODE *pnew)
{
    if(list== NULL || list->head== NULL || pnew== NULL)
        return;
    NODE *head = list->head;//获得头结点
    //如果第一次没有首节点,插入新的节点时,尾指针应该指向新的节点
    if(head->next == NULL)
        list->tail = pnew;

    pnew->next = head->next;//新的节点的next指向原来的首节点
    pnew->prev = head;//新的节点的prev指向头节点

    if(head->next != NULL)//代表有首节点,
        head->next->prev = pnew;//首节点的prev指向新的节点
    head->next= pnew; //头节点的next指向新的节点

    list->size++;
}

/**************************************************************************
*	函数功能:解析歌词,将歌词保存在二维数据 和链表中
*	参数：
*		tmp  二维数据首行地址
*		lrc    当前解析的一句中的歌词地址
*		count  当前解析的一句中的时间个数
*		list    LLIST结构体指针的地址
*
* 	返回值:无
**************************************************************************/
void deposit_link(char (*tmp)[LRC_SIZE],char *lrc,int count,LLIST*list)
{
    for(int i=0;i<count;i++)
    {
        int m=0;
        int s=0;
        sscanf(tmp[i],"%d:%d",&m,&s);
        NODE *pnew = (NODE *)malloc(sizeof(NODE));
        pnew->times=m*60+s;
        strcpy(pnew->lrc,lrc);
        pnew->lrc[strlen(pnew->lrc)-2]=0;// ...\r\n 歌词 将后面的\r置0
        insert_linklist_header(list,pnew);
    }
}

/**************************************************************************
*	函数功能:遍历链表
*	参数：
*		list    LLIST结构体指针的地址
*
* 	返回值:无
**************************************************************************/
void print_linklist_head(LLIST* list)
{
    if(list == NULL || list->head==NULL)
        return;
    NODE *pcur = list->head->next;//pcur指向首节点
    while(pcur != NULL)
    {
        printf(" %d  =%s\n",pcur->times,pcur->lrc);
        pcur= pcur->next;
    }
}

/**************************************************************************
*	函数功能:通过时间遍历链表,查找和时间t相同的节点
*	参数：
*		list  LLIST结构体指针的地址
        t     和节点比较的时间 单位s
*
* 	返回值: 查找到与时间t相等的节点
**************************************************************************/
NODE *search_lrc_by_time(LLIST*list,int t)
{
    if(list == NULL || list->head==NULL)
        return NULL;

    NODE *pcur = list->head->next;
    while(pcur != NULL)
    {
        if(pcur->times == t)
            return pcur;

        pcur=pcur->next;

    }
    return NULL;
}

/**************************************************************************
*	函数功能:链表选择法排序
*	参数：
*		list  LLIST结构体指针
*
* 	返回值:无
**************************************************************************/
void sort_linklist( LLIST*list)
{
    if(list == NULL || list->head==NULL)
        return ;
    NODE *head =  list->head;
    NODE *p1 = NULL;
    NODE *p2 =NULL;
    for(p1=head->next;p1->next != NULL;p1=p1->next)
    {
        for(p2=p1->next;p2!=NULL;p2=p2->next)
        {
            if( p1->times > p2->times)
            {
                //先整体交换 在交换指针
                NODE tmp;
                tmp = *p1;
                *p1  = *p2;
                *p2 = tmp;

                tmp.next = p1->next;
                p1->next = p2->next;
                p2->next = tmp.next;

                tmp.prev = p1->prev;
                p1->prev = p2->prev;
                p2->prev = tmp.prev;
            }
        }
    }

}



/**************************************************************************
*	函数功能:解析歌词
*	参数：
*		lrc_header  二维数据首行地址
*		list    LLIST结构体指针的地址
*		songpath 歌词文件的路径
*
* 	返回值:无
**************************************************************************/
void analysis_lrc(char (*lrc_header)[LRC_SIZE],LLIST*list,char *songpath)
{
    FILE *fp =fopen(songpath,"r");
    if(!fp)
    {
        perror("");
    }
    //先读取前4行
    char buf[1024]="";
    for(int i=0;i<4;i++)
    {
        char *p = fgets(buf,sizeof(buf),fp);//读取一行
        if(p == NULL)
            break;
        sscanf(buf,"%*[^:]:%[^]]",lrc_header[i]);
        //printf("%s\n",lrc_header[i]);
    }
    //在读时间+歌词标签
    while(1)
    {
        char *p = fgets(buf,sizeof(buf),fp);//读取一行
        if(p == NULL)
            break;
        int count =0;
        char tmp[10][LRC_SIZE];
        while(*p == '[')
        {
            sscanf(p+1,"%[^]]",tmp[count]);
            //printf("tmp=%s\n",tmp[count]);
            while(*p != ']')
                p++;
            p++;
            count++;
        }
        deposit_link(tmp,p,count,list);	//解析
    }
    fclose(fp);

}
/**************************************************************************
*	函数功能:销毁链表
*	参数：
*		list    LLIST结构体指针的地址
* 	返回值:无
**************************************************************************/
void destroy_linklist(LLIST* list)
{
    NODE *head = list->head;
    NODE *tmp=NULL;
    while( head != NULL)
    {
        tmp = head->next;
        free(head);
        head = tmp;
    }
    free(list);

}







