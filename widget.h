#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <QDebug>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <QPainter>
#include <QSlider>
#include <dirent.h>
#include <QLabel>
#include <QTableWidget>
#include <unistd.h>
#include "lrc.h"
#include "sys/wait.h"
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QListWidget>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    void setSlider(int *arg);
    void setSongInfo(char arg[4][256]);
    void readSong();
    void getPic();
    void displayLabel(char *lrc);
    void getSongList();
    void getLrcInfo(LLIST *list);
    void dispalyLrc();
    NODE *search_lrc_by_time(LLIST*list,const char *lrc);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void leftBntton();
    void midButton();
    void rightButton();
    void volumeButton();
    void playClickDouble(int row, int column);
    void setValue(int value);
    void randomPlay();
    void loopPlay();
    void setSliderPos(int value);
    void search(const QString &arg);
    void selectLrc(QListWidgetItem *item);


private:
    QPushButton *m_leftbutton;
    QPushButton *m_rightbutton;
    QPushButton *m_midbutton;
    QPushButton *m_volumebutton;
    QPushButton *m_randomPlay;
    QPushButton *m_loopPlay;
    QPainter *m_painter;
    QSlider *m_slider;
    QSlider *m_volume ;
    QLabel *m_CurrentTime;
    QLabel *m_EndTime;
    QLabel *m_Singer;
    QLabel *m_SongTitle;
    QTableWidget *m_SongList;
    QStringList m_header;
    QLineEdit *m_search;
    QListWidget *m_LrcList;



    char m_songname[128][512];
    char m_Songs[128][128];
    char m_songPic[128][128];
    int m_volumeFlag;
    int m_CuurentPos;
    int m_LrcNum;
    int m_PicNum;
    int m_TimeLenth;



public:
    int m_th;
    int m_fd;
    int m_flag;
    int m_SongNum;
    int m_lyricTime;
    char m_LyricName[128][512];
    char m_LrcInfo_[128][128];
    LLIST *list;
    bool m_currentstate;
    bool m_playFlag;
};

#endif // WIDGET_H
