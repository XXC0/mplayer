#include "widget.h"
#include "string.h"
#include <stdlib.h>


Widget::Widget(QWidget *parent): QWidget(parent)
{
    unlink("fifo_cmd");
    mkfifo("fifo_cmd",0666);
    m_fd = open("fifo_cmd",O_RDWR);
    if(m_fd < 0)
        perror("#######");

    srand(time(NULL));

    m_currentstate = true;
    m_playFlag = true;
    list = NULL;

    this->resize(600,400);
    this->setWindowIcon(QIcon(":/new/prefix1/Image/1193417.gif"));
    this->setFixedSize(600,400);
    this->readSong();

    m_th = 0;
    m_flag = 0;
    m_volumeFlag = 0;

    m_leftbutton  = new QPushButton(this);
    m_leftbutton->resize(40,30);
    m_leftbutton->move(0,350);
    m_leftbutton->setFlat(true);
    m_leftbutton->setIcon(QIcon(":/new/prefix1/Image/1160177.gif"));
    m_leftbutton->setIconSize(QSize(this->m_leftbutton->size()));
    connect(m_leftbutton,SIGNAL(clicked()),this,SLOT(leftBntton()));

    m_midbutton   = new QPushButton(this);
    m_midbutton->resize(40,30);
    m_midbutton->move(40,350);
    m_midbutton->setFlat(true);
    m_midbutton->setIcon(QIcon(":/new/prefix1/Image/1160182.gif"));
    m_midbutton->setIconSize(QSize(this->m_midbutton->size()));
    connect(m_midbutton,SIGNAL(clicked()),this,SLOT(midButton()));

    m_rightbutton = new QPushButton(this);
    m_rightbutton->resize(40,30);
    m_rightbutton->move(80,350);
    m_rightbutton->setFlat(true);
    m_rightbutton->setIcon(QIcon(":/new/prefix1/Image/1160180.gif"));
    m_rightbutton->setIconSize(QSize(this->m_rightbutton->size()));
    connect(m_rightbutton,SIGNAL(clicked()),this,SLOT(rightButton()));

    m_volumebutton = new QPushButton(this);
    m_volumebutton->resize(40,30);
    m_volumebutton->move(560,300);
    m_volumebutton->setFlat(true);
    m_volumebutton->setIcon(QIcon(":/new/prefix1/Image/1193404.gif"));
    m_volumebutton->setIconSize(QSize(this->m_volumebutton->size()));
    connect(m_volumebutton,SIGNAL(clicked()),this,SLOT(volumeButton()));

    m_randomPlay = new QPushButton(this);
    m_randomPlay->resize(40,30);
    m_randomPlay->move(560,370);
    m_randomPlay->setFlat(true);
    m_randomPlay->setIcon(QIcon(":/new/prefix1/Image/1160184.gif"));
    m_randomPlay->setIconSize(QSize(this->m_randomPlay->size()));
    connect(m_randomPlay,SIGNAL(clicked()),this,SLOT(randomPlay()));

    m_loopPlay = new QPushButton(this);
    m_loopPlay->resize(40,30);
    m_loopPlay->move(560,335);
    m_loopPlay->setFlat(true);
    m_loopPlay->setIcon(QIcon(":/new/prefix1/Image/1160185.gif"));
    m_loopPlay->setIconSize(QSize(this->m_loopPlay->size()));
    connect(m_loopPlay,SIGNAL(clicked()),this,SLOT(loopPlay()));

    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setParent(this);
    m_slider->resize(300,30);
    m_slider->move(170,350);


    m_slider->setStyleSheet("QSlider::groove:horizontal{ \
                                                    height: 10px; \
                                                    left: 5px; \
                                                    right: 5px; \
                                                    background: #dcdcdc; \
                                                  } \
                              QSlider::handle:horizontal{ \
                                                    border-radius: 10px; \
                                                    width:  20px; \
                                                    margin: -5px -1px; \
                                                    background: green; } \
                              QSlider::sub-page:horizontal{background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(27, 5, 27, 255), stop:0.25 rgba(99, 20, 102, 255), stop:0.5 rgba(154, 30, 158, 255), stop:1 rgba(173, 57, 176, 255));}");

    connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(setSliderPos(int)));
    //connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(setSliderPos(int)));

    m_volume = new QSlider(Qt::Vertical);
    m_volume->setParent(this);
    m_volume->resize(30,100);
    m_volume->move(530,300);
    m_volume->setMaximum(100);
    m_volume->setValue(100);
    m_volume->setMinimum(0);
    connect(m_volume, SIGNAL(sliderMoved(int)), this, SLOT(setValue(int)));

    m_CurrentTime = new QLabel(this);
    m_CurrentTime->resize(40,30);
    m_CurrentTime->move(120,350);
    m_CurrentTime->setAttribute(Qt::WA_TranslucentBackground);
    m_CurrentTime->setStyleSheet("background-color: rgb(255, 255, 255);font-size:13spx;color:red");

    m_EndTime = new QLabel(this);
    m_EndTime->resize(40,30);
    m_EndTime->move(490,350);
    m_EndTime->setAttribute(Qt::WA_TranslucentBackground);
    m_EndTime->setStyleSheet("background-color: rgb(255, 255, 255);font-size:13px;color:red");

    m_SongTitle = new QLabel(this);
    m_SongTitle->setStyleSheet("background-color: rgb(255, 255, 255);font-size:14px;color:red");
    m_SongTitle->setAttribute(Qt::WA_TranslucentBackground);
    m_SongTitle->resize(150,50);
    m_SongTitle->move(0,0);

    m_Singer = new QLabel(this);
    m_Singer->setStyleSheet("background-color: rgb(255, 255, 255);font-size:14px;color:red");
    m_Singer->setAttribute(Qt::WA_TranslucentBackground);
    m_Singer->resize(150,50);
    m_Singer->move(0,50);

    m_SongList = new QTableWidget(this);
    m_SongList->setColumnCount(1);
    m_SongList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_SongList->resize(150,200);
    m_SongList->move(450,30);
    m_header<<"歌曲";
    m_SongList->setShowGrid(false);
    m_SongList->setHorizontalHeaderLabels(m_header);
    m_SongList->setFrameShape(QFrame::NoFrame);
    m_SongList->setStyleSheet("selection-background-color:red;");
    QPalette pll = m_SongList->palette();
    pll.setBrush(QPalette::Base,QBrush(QColor(255,255,255,0)));
    m_SongList->setPalette(pll);
    m_SongList->setAttribute(Qt::WA_TranslucentBackground, true);
    m_SongList->hide();
    m_SongList->setFrameStyle(QFrame::NoFrame);
    m_SongList->setAutoScroll(false);
    m_SongList->setStyleSheet("background-color:transparent ");
    m_SongList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_SongList,SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT( playClickDouble(int,int)));

    m_search = new QLineEdit(this);
    m_search->resize(150,30);
    m_search->move(450,0);
    m_search->setPlaceholderText("Search...");
    m_search->hide();
    connect(m_search,SIGNAL(textChanged(QString)),this,SLOT(search(QString)));


    m_LrcList = new QListWidget(this);
    m_LrcList->move(50,100);
    m_LrcList->resize(300,200);
    m_LrcList->setStyleSheet("background-color:transparent");
    m_LrcList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_LrcList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_LrcList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(selectLrc(QListWidgetItem*)));


    this->getSongList();
    this->getPic();
}

void Widget::setSlider(int *arg)
{
    m_lyricTime = arg[0];
    m_CuurentPos = arg[0];
    m_TimeLenth  = arg[1];
    m_slider->setMinimum(0);
    m_slider->setValue(arg[0]);
    m_slider->setMaximum(arg[1]);

    char current[128]="";
    char lenth[128]="";
    sprintf(current,"%02d:%02d",arg[0]/60,arg[0]%60);
    sprintf(lenth,"%02d:%02d",arg[1]/60,arg[1]%60);
    m_CurrentTime->setText(current);
    m_EndTime->setText(lenth);

    if(m_CuurentPos == (m_TimeLenth-4) &&  m_CuurentPos != 0  && m_playFlag == true)
    {
        m_th++;
        write(m_fd,m_songname[m_th],strlen(m_songname[m_th]));
        m_currentstate = false;
    }
    else if(m_CuurentPos == (m_TimeLenth-4) && m_CuurentPos != 0 && m_playFlag == false)
    {
        write(m_fd,m_songname[m_th],strlen(m_songname[m_th]));
    }

}

void Widget::setSongInfo(char arg[4][256])
{
    char m_title[256]="歌名：  ";
    char m_songer[128]="歌手：  ";
    strcat(m_title,arg[0]);
    strcat(m_songer,arg[1]);
    m_SongTitle->setText(m_title);
    m_Singer->setText(m_songer);
    this->getLrcInfo(list);
    dispalyLrc();
}

void Widget::readSong()
{
    int i=0;
    DIR *dir = opendir("./song");
    if(dir == NULL)
    {
        perror("");
    }
    struct dirent *dirp=NULL;
    while( dirp = readdir(dir))
    {
        if(strlen(dirp->d_name) > 3)
        {
            char lrc[128]="";
            sprintf(m_songname[i%128],"loadfile ./song/%s\n",dirp->d_name);
            sscanf(dirp->d_name,"%[^.]",lrc);
            strcpy(m_Songs[i%128],lrc);
            sprintf(m_LyricName[i%128],"./lyrics/%s.lrc",lrc);
            i++;
        }
    }
    m_SongNum = i;
    closedir(dir);
}

void Widget::getPic()
{
    int i=0;
    char tmp[20] = "./pic/";
    DIR *dir = opendir("./pic");
    if(dir == NULL)
    {
        perror("");
    }
    struct dirent *dirp=NULL;
    while( dirp = readdir(dir))
    {
        if(strlen(dirp->d_name) > 3)
        {
            strcat(m_songPic[i%128],tmp);
            strcat(m_songPic[i%128],dirp->d_name);
            i++;
        }
    }
    m_PicNum = i;
    closedir(dir);
}

void Widget::setValue(int value)
{
    char buf[128];
    sprintf(buf,"volume %d 1\n",value);
    write(m_fd,buf,strlen(buf));
    memset(buf,0,strlen(buf));
}

void Widget::randomPlay()
{
    m_th = rand()%m_SongNum;
    write(m_fd,m_songname[m_th],strlen(m_songname[m_th]));
    m_currentstate = false;
    m_playFlag = true;
}

void Widget::loopPlay()
{
    if(m_playFlag == true)
        m_playFlag = false;
    else
        m_playFlag = true;
}

void Widget::setSliderPos(int value)
{

    char buf[128];
    sprintf(buf,"seek %d 2\n",value);
    write(m_fd,buf,strlen(buf));
    memset(buf,0,strlen(buf));

}

void Widget::search(const QString &arg)
{
    if(arg.isEmpty())
    {
        int rowCount = m_SongList->rowCount();
        for(int row = 0;row<rowCount;row++)
        {
            m_SongList->showRow(row);
        }
        return ;
    }
    int rowCount = m_SongList->rowCount();
    int columnCount = m_SongList->columnCount();
    for(int row = 0;row <rowCount;row++)
    {
        QString rowData;

        for(int column = 0;column<columnCount;column++)
        {
            if(m_SongList->item(row,column))
            {
                rowData += m_SongList->item(row,column)->text();
            }
        }
        if(!rowData.isEmpty())
        {
            if(rowData.contains(arg))
            {
                m_SongList->showRow(row);
            }
            else
            {
                m_SongList->hideRow(row);
            }
        }
        else
        {
            m_SongList->hideRow(row);
        }
    }

}

void Widget::selectLrc(QListWidgetItem *item)
{
    NODE *pcur = search_lrc_by_time(list,item->text().toStdString().data());
    if(pcur != NULL)
    {
        char buf[128];
        sprintf(buf,"seek %d 2\n",pcur->times);
        write(m_fd,buf,strlen(buf));
    }
}

void Widget::displayLabel(char *lrc)
{
    char buf[128];
    int flag = 0;
    strcpy(buf,lrc);
    for(int i=0;i<m_LrcList->count();i++)
    {
        if(strcmp(m_LrcList->item(i)->text().toStdString().data(),lrc)==0 && strlen(lrc) != 0)
        {
            sleep(1);
            if(flag == 0)
            {
                m_LrcList->setCurrentRow(i);
                flag = 1;
            }
        }

    }
}

void Widget::getSongList()
{
    m_SongList->setRowCount(m_SongNum);
    for(int i=0;i<m_SongNum;i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(m_Songs[i]);
        item->setTextColor(QColor(200,111,100));
        item->setFont(QFont("Helvetica"));
        item->setBackground(QBrush(QColor(255,255,255,0)));
        m_SongList->setItem(i,0,item);
    }
}

void Widget::getLrcInfo(LLIST *list)
{
    if(list == NULL || list->head==NULL)
        return;
    int i=0;
    NODE *pcur = list->head->next;//pcur指向首节点
    while(pcur != NULL)
    {
        if(strlen(pcur->lrc) != 0)
        {
            sprintf(m_LrcInfo_[i%128],"%s",pcur->lrc);

            i++;
        }
        pcur= pcur->next;
    }
    m_LrcNum = i;
}

void Widget::dispalyLrc()
{
    int counter =m_LrcList->count();
    for(int index=0;index<counter;index++)
    {
          QListWidgetItem *item = m_LrcList->takeItem(0);
          delete item;
    }
    for(int i=0;i<m_LrcNum;i++)
    {
        QListWidgetItem *item = new QListWidgetItem(m_LrcInfo_[m_LrcNum-i]);
        item->setTextColor(QColor(200,111,100));
        m_LrcList->insertItem(i,item);
    }

}

NODE *Widget::search_lrc_by_time(LLIST *list, const char *lrc)
{
    if(list == NULL || list->head==NULL)
        return NULL;

    NODE *pcur = list->head->next;
    while(pcur != NULL)
    {
        while(strstr(pcur->lrc,lrc) != NULL)
        {
            return pcur;
        }

        pcur=pcur->next;

    }
    return NULL;


}

void Widget::playClickDouble(int row, int column)
{
    m_th = row;
    write(m_fd,m_songname[m_th],strlen(m_songname[m_th]));
    m_currentstate = false;
}

Widget::~Widget()
{

}

void Widget::paintEvent(QPaintEvent *event)
{
    m_painter = new QPainter(this);
    m_painter->drawPixmap(this->rect(),QPixmap(m_songPic[m_th%m_PicNum]));
    this->update();
}

void Widget::keyPressEvent(QKeyEvent *event)
{

    switch(event->key())
    {
        case Qt::Key_S:
                m_SongList->show();
                m_search->show();
                break;
        case Qt::Key_H:
                m_SongList->hide();
                m_search->hide();
                break;
        default:
            break;
    }
}


void Widget::leftBntton()
{

    if(m_th == 0)
        m_th = m_SongNum;
    else
        m_th--;
    write(m_fd,m_songname[m_th],strlen(m_songname[m_th]));
    m_currentstate = false;

}

void Widget::midButton()
{
    if(m_flag == 0)
        m_flag = 1;
    else if(m_flag == 1)
        m_flag = 0;
    write(m_fd,"pause\n",strlen("pause\n"));
}

void Widget::rightButton()
{

    if(m_th == m_SongNum)
        m_th = 0;
    else
        m_th++;
    write(m_fd,m_songname[m_th%m_SongNum],strlen(m_songname[m_th%m_SongNum]));
    m_currentstate = false;

}

void Widget::volumeButton()
{
    if(m_volumeFlag == 0)
    {
        write(m_fd,"mute 1\n",strlen("mute 1\n"));
        m_volumeFlag =1;

    }
    else
    {

        write(m_fd,"mute 0\n",strlen("mute 0\n"));
        if(m_volumeFlag == 1 && m_flag == 1)
            write(m_fd,"pause\n",strlen("pause\n"));
        m_volumeFlag =0;
    }

}

