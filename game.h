#ifndef GAME_H
#define GAME_H

#include<map>
#include <QWidget>
#include<QLabel>
#include<QMessageBox>
#include <queue>
#include "banker.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Game; }
QT_END_NAMESPACE
using namespace std;
class Game : public QWidget
{
    Q_OBJECT

signals:
    //用于设置第几个步骤
    void numChanged(int newNum);

    //用于设置为第几局
    void NumChanged(int newNUM);

public:
    Game(QWidget *parent = nullptr);
    ~Game();
    //初始化所有信息，开始新的游戏
    void startGame(int personNum, int AINum);

    //在目前的玩家信息下，继续一轮游戏
    void continueGame();

    //结束游戏
    void endGame();

private:
    Ui::Game *ui;

    QString url=QString(":/new/prefix1/poke/");

    QString end=QString(".png");

    //花色对应图片命名
    map<QString,QString>color;

    //步骤数
    int num=1;

    //局数
    int Num=1;
    //当对局因为有一方弃牌而结束时的胜利者的id
    int easyWinner;

    QMessageBox m;

     QMessageBox res;

    QMessageBox next;

    Banker banker;  //庄家

    vector<Player> players;//玩家

    QList<QLabel*>Labels;

    Table desk; //牌局

    PokerHeap pokerHeap; //扑克牌堆

    int personnum;

    int outnum;

    static const int starMoney = 20000;


    /**
     * @brief 下注函数
     * @param playersNeedtoBet
     * @param isRoundOne
     */
    void round(queue<Player> &playersNeedtoBet,bool isRoundTwo);

    //第一轮下注
    void firstRound();

    //其他轮下注
    void otherRound(bool isRoundTwo);

    //最终牌局判断
    void judgeWin();

    //清空公共牌
    void clearPublicPic();

    //加载公共牌到容器中
    void initPublicPoker();

    //花色与图片名称对应
    void initColor();

    //显示公共牌
    void viewPublicPoker(PokerHeap Public,int size);

    //显示余额
    void viewMoney();

    //选择跟注，加注，弃牌
    void choose(bool isRoundOne);

    //清除手牌
    void clearHandPic();

    //用于发送num改变的信号
    void setNum(int newNum);

    //用于发送Num改变的信号
    void setNUM(int newNUM);

    void viewAicard();

    void viewCard();
};
#endif // GAME_H
