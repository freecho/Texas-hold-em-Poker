#ifndef POKER_H
#define POKER_H


#include <string>
#include <vector>

using namespace std;

/**
 * @brief 卡牌类
 */
class Poker {
private:
    /**
     * @param suit 卡牌花色 Spade(黑桃) Heart(红桃) Diamond(方块) Club(梅花)
     * @param point 卡牌点数 1~14: A 2 3 4 5 6 7 8 9 10 J Q K A
     */
    string suit;
    int point;

public:
    /**
     * @brief 构造函数
     */
    Poker();

    Poker(string suit, int point);

    //析构函数
    ~Poker();

    /**
     * @brief 查询器
     */
    int getPoint() const;

    int getSuitSize() const;

    string getSuit() const;

    /**
     * @brief 重载比较运算符
     */
    bool operator<(const Poker &poker) const;

    bool operator==(const Poker &poker) const;

    bool operator>(const Poker &poker) const;


    bool operator<=(const Poker &poker) const;

    bool operator>=(const Poker &poker) const;

    bool operator!=(const Poker &poker) const;

};

#endif // POKER_H
