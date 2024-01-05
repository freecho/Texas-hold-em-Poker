#ifndef TABLE_H
#define TABLE_H

#include "pokerheap.h"
using namespace std;

/**
 * @brief 桌面类，控制公共牌筹码等信息
 */
class Table {
private:
    /**
     * @param betMount 本轮需投注的筹码
     * @param jackPot 奖池
     */
    int betMount;
    int jackPot;

public:
    PokerHeap publicPoker;//公共牌

    Table();

    ~Table();

    void initBetMount();//初始化筹码

    void initjackPot();//初始化奖池

    void publicPokerSort();//公共牌排序

    PokerHeap&getpublicPoker();//获取公共牌的引用

    void setbetMount(int num);//设置筹码

    int getPublicPokerSize();//获取公共牌数量

    int getJackPot();//获取奖池

    int getbetMount();//获取筹码

    void addJackPot(int num);//添加筹码到奖池

    void clearPublicPoker();//清除公共牌


};
#endif // TABLE_H
