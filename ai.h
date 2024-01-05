#ifndef AI_H
#define AI_H


#include "player.h"

/**
 * Ai类
 */
class Ai : public Player {
private:

    /**
    * 一套牌（包含5张），可计算该套牌的大小，表示为十进制数abcdef。
    * 1<=a<=9：用1～9分别表示高牌，一对，两对，三条，顺子，同花，葫芦，四条，同花顺
    * 2<=(b-f)<=14：用2～14分别表示牌的点数，从2-10和J-A
    *
    */
    class CardGroup {
    private:
        //5张牌,需要点数从大到小排序
        PokerHeap pokers;
        //牌型 从大到小（皇家同花顺->单牌）为 0->10
        int type;
        //牌的大小
        long long power;

        /**
         * 为当前牌组计算power
         * @return
         */
        long long computePower();

        long long computeCardPoint();

        /**
         * 判断5张牌是否为同花
         * @return
         */
        bool isFlush();

        /**
        * 判断5张牌是否为顺子
        * @return
        */
        bool isStraight();

        /**
        * 判断5张牌为同花和顺子以外的其他具体牌型
        * @return
        */
        int getOtherType();

        //为“四条“牌型修正牌的顺序
        void changeCardPositionForFourKind();

        //为“葫芦“牌型修正牌的顺序
        void changeCardPositionForFullHouse();

        //为“三条“牌型修正牌的顺序
        void changeCardPositionForThreeKind();

        //为“两对“牌型修正牌的顺序
        void changeCardPositionForTwoPair();

        //为“一对“牌型修正牌的顺序
        void changeCardPositionForOnePair();

    public:
        CardGroup(PokerHeap pokers);

        CardGroup();

        int getPower();

        PokerHeap getPokers();

        int getType();

    };

    /**
    * 用于从当前牌中提取最大牌
    * @author wangtao
    *
    */
    class MaxCardComputer {
    private:
        //自己可用的所有牌
        PokerHeap pokers;

        CardGroup maxGroup;

        /**
        * 计算最大的五张牌
        */
        void computeMaxCardGroup();

        void computeMaxCardGroup_New(Poker poker);


    public:
        MaxCardComputer(PokerHeap &handCard, PokerHeap &publicCard);

        MaxCardComputer(MaxCardComputer oldComputer, Poker poker);

        PokerHeap getPokers();

        CardGroup getMaxCardGroup();


    };

    //战胜对手的概率
    float winProb;

    //判断手牌是否相差小于等于4(有可能组成顺子)
    bool isHoldLessThanFour();

    //手牌都大于或等于10
    bool isHoldBig();

    //判断手牌是否同花色
    bool isHoldSameColor();

    //获取公共牌与手牌组成的对子的value
    vector<int> getHoldPubPairValue(PokerHeap &publicPoker);

    //获取公共牌中组成对子的值，返回的vector包含的是这些对子的value
    vector<int> getPubPairValue(PokerHeap &publicPoker);

    //判断手牌是否是大对：AA, KK, QQ, JJ, 1010等
    bool isHoldBigPair();

    //判断手牌是否是小对：2~9中的一对
    bool isHoldSmallPair();

    //计算当前牌组成同花最少还差多少张
    int computeFlush(PokerHeap &publicPoker);

    //计算当前牌组成顺子最少需要多少张牌
    int computeStraight(PokerHeap &publicPoker);

public:
    Ai(int id);

    //收到两张牌之后思考策略
    void thinkAfterHold(Table &desk,int bet);

    //发出三张公共牌之后思考策略
    void thinkAfterFlop(Table &desk,int bet);

    //发出一张转牌之后思考策略
    void thinkAfterTurn(Table &desk,int bet);

    //发出一张河牌之后思考策略
    void thinkAfterRiver(Table &desk,int bet);

};
#endif // AI_H
