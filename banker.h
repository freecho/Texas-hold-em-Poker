#ifndef BANKER_H
#define BANKER_H


#include "pokerheap.h"
#include "player.h"
#include<string>
using namespace std;
/**
 * @brief 庄家类
 *        控制扑克牌堆，负责给玩家发牌以及发公共牌,并且能对最后的结局判断输赢，给玩家分发奖池
 */
class Banker {
private:
    int playerNum;//玩家数量
    int winnerNum;//赢家数量
public:
    vector<string> players_cards_type;//玩家手牌的种类

    vector<PokerHeap> finalCards;//公共牌加上手牌

    vector<vector<Poker>> subset;//筛选出来的五张牌

    vector<int> winnerId;//记录赢家的下标,赢的程度递增

public:
    vector<string> rank;

    /**
     * @brief 默认构造&析构函数
     */
    Banker();

    ~Banker();

    //给每名玩家发两张牌
    void licensing(std::vector<Player> &players, PokerHeap &pokerHeap);

    //发若干张公共牌到桌上
    void licensingPublicCard(Table &desk, PokerHeap &pokerHeap, int num);

    //辨别每个玩家手牌的种类
    void identifyCardType(std::vector<Player> &players);

    //根据玩家的手牌，结合桌面上的公共牌，判断输赢，并给玩家分发奖池
    void distributeReward(std::vector<Player> &players, Table &desk);

    /**
     * @brief 最终牌的类别判断，按照从大到小排序：
     * 皇家同花顺 RoyalFlush
     * 同花顺 StraightFlush
     * 四条 FourArticle
     * 葫芦 FullHouse
     * 同花 Flush
     * 顺子 Straight
     * 三条 ThreeArticle
     * 两对 TwoPairs
     * 一对 APair
     * 单牌 SingleCard
     */
    bool isRoyalFlush(PokerHeap playercard, int index);

    bool isStraightFlush(PokerHeap playercard, int index);

    bool isFourArticle(PokerHeap playercard, int index);

    bool isFullHouse(PokerHeap playercard, int index);

    bool isSameSuit(PokerHeap playercard, int index);

    bool isStraightDraw(PokerHeap playercard, int index);

    bool isThreeArticle(PokerHeap playercard, int index);

    bool isTwoPairs(PokerHeap playercard, int index);

    bool isAPair(PokerHeap playercard, int index);

    bool isSingleCard(PokerHeap playercard, int index);


    void findAWinnerId(std::vector<Player> &players);

    void findOtherWinnersId(std::vector<Player> &players);

    void comparisionBetweenWinners();

    void comparisionOfRoyalFlush();

    void comparisionOfFlush();

    void comparisionOfFourArticle();

    void comparisionOfFullHouse();

    void comparisionOfSameSuit();

    void comparisionOfStraightDraw();

    void comparisionOfThreeArticle();

    void comparisionOfTwoPairs();

    void comparisionOfAPair();

    void comparisionOfSingleCard();

    vector<int>&returnWinnerID();

    //金钱结算
    void billing(Table &desk,vector<Player*> &winners);
};


#endif // BANKER_H
