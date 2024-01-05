#ifndef POKERHEAP_H
#define POKERHEAP_H


#include "poker.h"
#include <vector>

using namespace std;

/**
 * @brief 牌堆类
 */
class PokerHeap {
private:
    vector<Poker> heap;

public:
    /**
     * @brief 构造以及析构函数
     */
    PokerHeap();

    PokerHeap(vector<Poker> pokerHeap);

    ~PokerHeap();

    /**
     * @brief 按照从小到大排序
     */
    void sort();

    //初始化，产生一副新的有序牌堆
    void init();

    //洗牌，打乱顺序
    void shuffle();

    /**
     * @return 牌堆中牌的数量
     */
    int getPokerNum();

    /**
     * @return 牌堆的引用
     */
    vector<Poker> &getPokerHeap();

    //根据点数降序排序
    void sortByPoint();

    //清空牌堆
    void clear();
};


#endif // POKERHEAP_H
