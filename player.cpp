#include "player.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <ctime>
Player::Player(int ID, bool is) {
    this->id = ID;
    this->isAi = is;
    this->betHasPut = 0;
    this->hasAddBet = false;
    this->out = false;
    this->money = 0;
}

Player::~Player() = default;

bool Player::getIsOut() { return this->out; }

bool Player::getIsAI() { return this->isAi; }

int Player::getbetHasPut() { return this->betHasPut; }

void Player::addMoney(int add){this->money += add; }

void Player::bet(Table &desk, int num) {
    desk.addJackPot(num);
    desk.setbetMount(num);
    this->money -= num;
    this->betHasPut = num;
}

void Player::followBet(Table &desk, int bet) {
    desk.addJackPot(bet);
    desk.setbetMount(bet);
    this->money -= bet;
    this->betHasPut += bet;
    if(isAi)this->flag=1;
}

void Player::addBet(Table &desk, int bet) {
    //int addNum = bet - betHasPut + num;
    int addNum = (bet)*2;
    desk.addJackPot(addNum);
    desk.setbetMount(addNum);
    this->money -= addNum;
    this->betHasPut = addNum;
     if(isAi)this->flag=0;
}
int Player::getFlag(){
     return this->flag;
}

int Player::getId() { return this->id; }

void Player::setMoney(int Money) { this->money = Money; }

int Player::getMoney() { return this->money; }

void Player::backToGame() { this->out = false; }

void Player::outGame() { this->out = true; }

void Player::initBetHasPut() {
    this->betHasPut = 0;
    this->hasAddBet = false;
}

void Player::handCardSort() { this->handCard.sort(); }

PokerHeap &Player::getHandCard() { return this->handCard; }

void Player::clearHandCard() { this->handCard.clear(); }

//TODO 完善AI决策与当前bet的逻辑关系

bool Player::isHoldLessThanFour() {
    vector<Poker> &card = getHandCard().getPokerHeap();
    if (card[0].getPoint() == 14 || card[1].getPoint() == 14) {
        return abs(card[0].getPoint() - card[1].getPoint()) % 13 <= 4;
    }
    return abs(card[0].getPoint() - card[1].getPoint()) <= 4;
}

bool Player::isHoldBig() {
    vector<Poker> &card = getHandCard().getPokerHeap();
    return (card[0].getPoint() >= 10) || (card[1].getPoint() >= 10);
}

bool Player::isHoldSameColor() {
    vector<Poker> &card = getHandCard().getPokerHeap();
    return card[0].getSuit() == card[1].getSuit();
}

vector<int> Player::getHoldPubPairValue(PokerHeap &publicPoker) {
    vector<int> res;
    for (auto &i: getHandCard().getPokerHeap()) {
        for (auto &j: publicPoker.getPokerHeap()) {
            if (i.getPoint() == j.getPoint()) {
                res.push_back(i.getPoint());
            }
        }
    }
    return res;
}

vector<int> Player::getPubPairValue(PokerHeap &publicPoker) {
    vector<int> ct(15, 0), res;
    for (auto &x: publicPoker.getPokerHeap()) ct[x.getPoint()]++;
    for (int i = 2; i < 15; i++) {
        if (ct[i] == 2) res.push_back(i);
    }
    return res;
}

void Player::thinkAfterHold(Table &desk, int bet) {
    getHandCard().sortByPoint();
    vector<Poker> &card = getHandCard().getPokerHeap();
    // 如果手牌是大对：AA, KK, QQ, JJ, 1010等
    if (isHoldBigPair()) {
        //加注
        this->addBet(desk, bet);
    }
    // 手牌是小对：2~9中的一对
    else if (isHoldSmallPair()) {
        //加注
        this->addBet(desk, bet);
    }
    // 手牌不相等且都大于10
    else if (isHoldBig()) {
        if (isHoldSameColor()) {
            //加注
            this->addBet(desk, bet);
        } else {
            //跟注
            this->followBet(desk, bet);
        }
    }
    // 手牌其中有一个大于10
    else if (card[0].getPoint() > 10) {
        if (isHoldSameColor() || isHoldLessThanFour()) {
            //跟注
            this->addBet(desk, bet);
        }
        else{
            this->followBet(desk, bet);
        }
    }
    // 手牌都小于10
    else {
        // 手牌同花色或者相差小于4
        if (isHoldSameColor() || isHoldLessThanFour()) {
            //跟注
            this->followBet(desk, bet);
        }
        else {
            //诈唬
            std::srand(static_cast<unsigned int>(time(0)));
            int random_number = rand();
            if (random_number % 2) {
                //加注
                this->followBet(desk, bet);
            }
            else {
                //诈唬
                std::srand(static_cast<unsigned int>(time(0)));
                int random_number = rand();
                if(random_number < 8){
                    this->followBet(desk,bet);
                }
                else{
                    this->outGame();
                }
            }
        }
    }
}

void Player::thinkAfterFlop(Table &desk, int bet) {
    this->getHandCard().sortByPoint();
    PokerHeap card = this->getHandCard(), pp = desk.getpublicPoker();
    CardGroup maxGroup = (new MaxCardComputer(card, pp))->getMaxCardGroup();
    long long power = maxGroup.getPower();
    long long p = std::pow(10, 10);
    vector<Poker> hand = this->getHandCard().getPokerHeap();

    // 两对
    if (power > 3 * p && power < 4 * p) {
        // 获取手牌与公共牌组成对子的value
        vector<int> holdPairValues = getHoldPubPairValue(desk.getpublicPoker());
        // 手牌中只有一张与公共牌组成对子，说明另一对是在公共牌里的
        if (holdPairValues.size() == 1) {
            // 大对
            if (hand[0].getPoint() >= 10) {
                //加注
                this->addBet(desk, bet);
            } else {
                //跟注
                this->followBet(desk, bet);
            }
        }
        // 手牌中的两张分别与公共牌中的一张组成对子
        else if (holdPairValues.size() == 2) {
            //其中一个为大对,就加注
            if (holdPairValues[0] >= 10 || holdPairValues[1] >= 10) {
                this->addBet(desk, bet);
            } else {
                //跟注
                this->followBet(desk, bet);
            }
        }
    }
    // 三条
    else if (power > 4 * p && power < 5 * p) {
        // 手牌相等
        if (hand[0].getPoint() == hand[1].getPoint()) {
            this->addBet(desk, bet);
        }
        // 手牌不相等
        else {
            vector<int> pairValues = getPubPairValue(desk.getpublicPoker());
            // 公共牌中有一对，说明三条中有两个是在公共牌里的
            if (pairValues.size() == 1) {
                if (pairValues[0] >= 10) {
                    this->addBet(desk, bet);
                } else {
                    this->followBet(desk, bet);
                }
            }
            // 说明三条是出现在公共牌里
            else if (pairValues.size() == 0) {
                // 手牌都是大牌
                if (isHoldBig()) {
                    this->addBet(desk, bet);
                }
                //有大牌
                else if (hand[0].getPoint() >= 10) {
                    this->followBet(desk, bet);
                }
                //只有小牌
                else {
                    //诈唬
                    std::srand(static_cast<unsigned int>(time(0)));
                    int random_number = rand()%10;
                    if(random_number < 6){
                        this->followBet(desk, bet);
                    }
                    else{
                        this->outGame();
                    }
                }

            }
        }
    }
    // 顺子及以上
    else if (power > 5 * p) {
        this->addBet(desk, bet);
    }
    // 一对
    else if (power > 2 * p && power < 3 * p) {
        // 手牌是大对
        if (isHoldBigPair()) {
            this->addBet(desk, bet);
        }
        // 手牌是小对，跟注
        else if (isHoldSmallPair()) {
            this->followBet(desk, bet);
        } else {
            // 获取公共牌中的对子的值
            vector<int> pubPair = getPubPairValue(pp);
            // 公共牌中有一对，说明手牌没有和公共牌中的某一张组成对子 ，这种情况跟高牌差不多
            if (pubPair.size() == 1) {
                if (isHoldBig()) {
                    this->followBet(desk, bet);

                }
                // 手牌中有一个大牌
                else if (hand[0].getPoint() >= 10) {
                    this->followBet(desk, bet);

                } else {
                    //诈唬
                    std::srand(static_cast<unsigned int>(time(0)));
                    int random_number = rand()&10;
                    if(random_number < 6){
                        this->followBet(desk, bet);
                    }
                    else{
                        this->outGame();
                    }
                }
            }
            // 说明手牌中的一张牌与公共牌中的一张牌组成对子
            else if (pubPair.size() == 0) {
                // 在这里，pairValues中有且只有一个值
                vector<int> pairValues = getHoldPubPairValue(pp);
                // 大对
                if (pairValues[0] >= 10) {
                    this->addBet(desk, bet);

                } else {
                    this->followBet(desk, bet);

                }
            }
        }
    }
    // 同花或顺子差一张
    else if (computeFlush(pp) <= 1 || computeStraight(pp) <= 1) {
        if (isHoldBig()) {
            this->addBet(desk, bet);
        } else {
            this->followBet(desk, bet);

        }
    }
    // 高牌
    else if (isHoldBig()) {
        this->followBet(desk, bet);
    }
    //纯粹诈唬
    else {
        std::srand(static_cast<unsigned int>(time(0)));
        int random_number = rand() % 10;
        //诈唬
        if (random_number < 2) {
            //加注
            this->addBet(desk, bet);
        }
        //跟注
        else if(random_number <6){
            this->followBet(desk, bet);
        }
        else{
            this->outGame();
        }

    }


}

void Player::thinkAfterTurn(Table &desk, int bet) {
    getHandCard().sortByPoint();
    PokerHeap hp = getHandCard(), pp = desk.getpublicPoker();
    CardGroup maxGroup = (new MaxCardComputer(hp, pp))->getMaxCardGroup();
    vector<Poker> hand = hp.getPokerHeap();

    long long power = maxGroup.getPower(), p = std::pow(10, 10);
    // 两对
    if (power > 3 * p && power < 4 * p) {
        // 获取手牌与公共牌组成对子的value
        vector<int> holdPairValues = getHoldPubPairValue(pp);
        // 手牌中只有一张与公共牌组成对子，说明另一对是在公共牌里的
        if (holdPairValues.size() == 1) {
            // 大对
            if (holdPairValues[0] >= 10) {
                this->addBet(desk, bet);
            }
            // 小对
            else {
                this->followBet(desk, bet);
            }
        }
        // 手牌中的两张分别与公共牌中的一张组成对子
        else if (holdPairValues.size() == 2) {
            // 两对都是大对
            if (holdPairValues[0] >= 10 && holdPairValues[1] >= 10) {
                this->addBet(desk, bet);
            }
            // 其中一个为大对
            else if (holdPairValues[0] >= 10 || holdPairValues[1] >= 10) {
                this->followBet(desk, bet);
            } else {
                //诈唬
                std::srand(static_cast<unsigned int>(time(0)));
                int random_number = rand();
                if(random_number < 5){
                    this->addBet(desk,bet);
                }
                else{
                    this->followBet(desk, bet);
                }
            }
        }

    }
    // 三条
    else if (power > 4 * p && power < 5 * p) {
        // 手牌相等
        if (hand[0].getPoint() == hand[1].getPoint()) {
            this->addBet(desk, bet);
        }
        // 手牌不相等
        else {
            vector<int> pairValues = getPubPairValue(pp);
            // 公共牌中有一对，说明三条中有两个是在公共牌里的
            if (pairValues.size() == 1) {
                if (pairValues[0] >= 10) {
                    this->addBet(desk, bet);
                } else {
                    this->followBet(desk, bet);
                }
            }
            // 说明三条是出现在公共牌里
            else if (pairValues.size() == 0) {
                // 手牌都是大牌
                if (isHoldBig()) {
                    this->followBet(desk, bet);
                } else {
                    //诈唬
                    std::srand(static_cast<unsigned int>(time(0)));
                    int random_number = rand();
                    if(random_number < 4){
                        this->addBet(desk, bet);
                    }
                    else{
                        this->followBet(desk, bet);
                    }
                }
            }

        }
    }
    // 顺子及以上
    else if (power > 5 * p) {
        this->addBet(desk, bet);
    }
    // 一对
    else if (power > 2 * p && power < 3 * p) {
        // 手牌是大对
        if (isHoldBigPair()) {
            this->addBet(desk, bet);
        }
        // 手牌是小对
        else if (isHoldSmallPair()) {
            this->followBet(desk, bet);
        } else {
            // 获取公共牌中的对子的值
            vector<int> pubPair = getPubPairValue(pp);
            // 公共牌中有一对，说明手牌没有和公共牌中的某一张组成对子 ，这种情况跟高牌差不多
            if (pubPair.size() == 1) {
                if (isHoldBig()) {
                    this->addBet(desk, bet);
                }
                // 手牌中有一个大牌
                else if (pubPair[0] >= 10 || pubPair[1] >= 10) {
                    this->followBet(desk, bet);
                }
                // 说明手牌中的一张牌与公共牌中的一张牌组成对子
                else if (pubPair.size() == 0) {
                    // 在这里，pairValues中有且只有一个值
                    vector<int> pairValues = getHoldPubPairValue(pp);
                    // 大对
                    if (pairValues[0] >= 10) {
                        this->followBet(desk, bet);
                    }
                    // 小对，跟注
                    else {
                        this->followBet(desk, bet);
                    }
                }
            }
        }
    }
    // 同花或顺子差一张
    else if (computeFlush(pp) <= 1 || computeStraight(pp) <= 1) {
        if (isHoldBig()) {
            this->addBet(desk, bet);
        } else {
            this->followBet(desk, bet);
        }
    }
    // 高牌
    else if (isHoldBig()) {
        this->followBet(desk, bet);
    }
    //纯粹诈唬
    else {
        std::srand(static_cast<unsigned int>(time(0)));
        int random_number = rand() % 10;
        //诈唬
        if (random_number < 1) {
            //加注
            this->addBet(desk, bet);
        }
        //跟注
        else if (random_number < 4) {
            this->followBet(desk, bet);
        } else {
            //诈唬
            std::srand(static_cast<unsigned int>(time(0)));
            int random_number = rand();
            if(random_number < 4){
                this->followBet(desk,bet);
            }
            else{
                this->outGame();
            }
        }
    }


}

void Player::thinkAfterRiver(Table &desk, int bet) {
    getHandCard().sortByPoint();
    PokerHeap hp = getHandCard(), pp = desk.getpublicPoker();
    CardGroup maxGroup = (new MaxCardComputer(hp, pp))->getMaxCardGroup();
    vector<Poker> hand = hp.getPokerHeap();

    long long power = maxGroup.getPower(), p = std::pow(10, 10);

    // 两对
    if (power > 3 * p && power < 4 * p) {
        // 获取手牌与公共牌组成对子的value
        vector<int> holdPairValues = getHoldPubPairValue(pp);
        // 手牌中只有一张与公共牌组成对子，说明另一对是在公共牌里的
        if (holdPairValues.size() == 1) {
            // 大对
            if (holdPairValues[0] >= 10) {
                this->addBet(desk, bet);
            }
            // 小对
            else {
                this->followBet(desk, bet);
            }
        }
        // 手牌中的两张分别与公共牌中的一张组成对子
        else if (holdPairValues.size() == 2) {
            // 两对都是大对
            if (holdPairValues[0] >= 10 && holdPairValues[1] >= 10) {
                this->addBet(desk, bet);
            }
            // 其中一个为大对
            else if (holdPairValues[0] >= 10 || holdPairValues[1] >= 10) {
                this->followBet(desk, bet);
            } else {
                //诈唬
                std::srand(static_cast<unsigned int>(time(0)));
                int random_number = rand();
                if(random_number < 3){
                    this->addBet(desk, bet);
                }
                else{
                    this->followBet(desk, bet);
                }
            }
        }

    }
    // 三条
    else if (power > 4 * p && power < 5 * p) {
        // 手牌相等
        if (hand[0].getPoint() == hand[1].getPoint()) {
            this->addBet(desk, bet);
        }
        // 手牌不相等
        else {
            vector<int> pairValues = getPubPairValue(pp);
            // 公共牌中有一对，说明三条中有两个是在公共牌里的
            if (pairValues.size() == 1) {
                if (pairValues[0] >= 10) {
                    this->addBet(desk, bet);
                } else {
                    this->followBet(desk, bet);
                }
            }
            // 说明三条是出现在公共牌里
            else if (pairValues.size() == 0) {
                // 手牌都是大牌
                if (isHoldBig()) {
                    this->followBet(desk, bet);
                } else {
                    //诈唬
                    std::srand(static_cast<unsigned int>(time(0)));
                    int random_number = rand();
                    if(random_number < 6){
                        this->followBet(desk,bet);
                    }
                    else{
                        this->outGame();
                    }
                }
            }

        }
    }
    // 顺子及以上
    else if (power > 5 * p) {
        this->addBet(desk, bet);
    }
    // 一对
    else if (power > 2 * p && power < 3 * p) {
        // 手牌是大对
        if (isHoldBigPair()) {
            this->addBet(desk, bet);
        }
        // 手牌是小对
        else if (isHoldSmallPair()) {
            //诈唬
            std::srand(static_cast<unsigned int>(time(0)));
            int random_number = rand();
            if(random_number < 3){
                this->followBet(desk,bet);
            }
            else{
                this->outGame();
            }
        } else {
            // 获取公共牌中的对子的值
            vector<int> pubPair = getPubPairValue(pp);
            // 公共牌中有一对，说明手牌没有和公共牌中的某一张组成对子 ，这种情况跟高牌差不多
            if (pubPair.size() == 1) {
                if (isHoldBig()) {
                    this->followBet(desk, bet);
                }
                // 手牌中有一个大牌
                else if (pubPair[0] >= 10 || pubPair[1] >= 10) {
                    this->followBet(desk, bet);
                }
                // 说明手牌中的一张牌与公共牌中的一张牌组成对子
                else if (pubPair.size() == 0) {
                    //诈唬
                    std::srand(static_cast<unsigned int>(time(0)));
                    int random_number = rand();
                    if(random_number < 4){
                        this->followBet(desk,bet);
                    }
                    else{
                        this->outGame();
                    }
                }
            }
        }
    }
    //        // 高牌
    //    else if (isHoldBig()) {
    //        this->followBet(desk, bet);
    //    }
    //纯粹诈唬
    else {
        std::srand(static_cast<unsigned int>(time(0)));
        int random_number = rand() % 10;
        //诈唬
        if (random_number < 1) {
            //加注
            this->addBet(desk, bet);
        }
        //跟注
        else if (random_number < 4) {
            this->followBet(desk, bet);
        } else {
            this->outGame();
        }
    }


}

bool Player::isHoldBigPair() {
    vector<Poker> &card = getHandCard().getPokerHeap();
    return (card[0].getPoint() == card[1].getPoint()) && (card[0].getPoint() >= 10);
}

bool Player::isHoldSmallPair() {
    vector<Poker> &card = getHandCard().getPokerHeap();
    return (card[0].getPoint() == card[1].getPoint()) && (card[0].getPoint() < 10);
}

int Player::computeFlush(PokerHeap &publicPoker) {
    vector<int> ct(4, 0);
    for (auto &x: getHandCard().getPokerHeap()) ct[x.getSuitSize()]++;
    for (auto &x: publicPoker.getPokerHeap()) ct[x.getSuitSize()]++;
    int mx = 0;
    for (int i = 0; i < 4; i++) {
        if (ct[i] > mx) mx = ct[i];
    }
    return 5 - mx;
}

int Player::computeStraight(PokerHeap &publicPoker) {
    //vector<bool> vis(15, false);
    vector<bool> vis(16, false);
    for (auto &x: getHandCard().getPokerHeap()) {
        if (x.getPoint() == 14) vis[1] = true;
        vis[x.getPoint()] = true;
    }
    for (auto &x: publicPoker.getPokerHeap()) {
        if (x.getPoint() == 14) vis[1] = true;
        vis[x.getPoint()] = true;
    }
    int mx = 0;
    for (int i = 1; i < 15; i++) {
        int ct = 0;
        for (int j = 0; j < 5; j++) {
            if (vis[i + j]) ct++;
            else break;
        }
        if (ct > mx) mx = ct;
    }
    return 5 - mx;

}

void Player::MaxCardComputer::computeMaxCardGroup() {
    int size = this->pokers.getPokerHeap().size();
    if (size == 5) {
        CardGroup group = *new CardGroup(pokers);
        this->maxGroup = group;
    } else if (size == 6) {
        vector<CardGroup> groups;
        for (int i = 0; i < 6; i++) {
            vector<Poker> card = this->pokers.getPokerHeap();
            card.erase(card.begin() + i);
            groups.push_back(*new CardGroup(card));
        }
        std::sort(groups.begin(), groups.end(), [](CardGroup &a, CardGroup &b) {
            return a.getPower() < b.getPower();
        });
        this->maxGroup = groups[0];
    } else if (size == 7) {
        vector<CardGroup> groups;
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (i == j) continue;
                vector<Poker> card = this->pokers.getPokerHeap();
                Poker t1 = card[i], t2 = card[j];
                card.erase(card.begin() + i);
                card.erase(card.begin() + j);
                groups.push_back(*new CardGroup(card));
            }
        }
        std::sort(groups.begin(), groups.end(), [](CardGroup &a, CardGroup &b) {
            return a.getPower() < b.getPower();
        });
        this->maxGroup = groups[0];
    }
}

void Player::MaxCardComputer::computeMaxCardGroup_New(Poker poker) {
    int size = this->pokers.getPokerHeap().size();
    if (size == 5) {
        vector<CardGroup> groups;
        for (int i = 0; i < 5; i++) {
            vector<Poker> card = this->pokers.getPokerHeap();
            card.erase(card.begin() + i);
            groups.push_back(*new CardGroup(card));
        }
        std::sort(groups.begin(), groups.end(), [](CardGroup &a, CardGroup &b) {
            return a.getPower() < b.getPower();
        });
        this->maxGroup = groups[0];
    } else if (size == 6) {
        vector<CardGroup> groups;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (i == j) continue;
                vector<Poker> card = this->pokers.getPokerHeap();
                Poker t1 = card[i], t2 = card[j];
                card.erase(card.begin() + i);
                card.erase(card.begin() + j);
                groups.push_back(*new CardGroup(card));
            }
        }
        std::sort(groups.begin(), groups.end(), [](CardGroup &a, CardGroup &b) {
            return a.getPower() < b.getPower();
        });
        this->maxGroup = groups[0];
    }
}

Player::MaxCardComputer::MaxCardComputer(PokerHeap &handCard, PokerHeap &publicCard) {
    pokers.clear();
    pokers.getPokerHeap().insert(pokers.getPokerHeap().end(), handCard.getPokerHeap().begin(),
                                 handCard.getPokerHeap().end());
    pokers.getPokerHeap().insert(pokers.getPokerHeap().end(), publicCard.getPokerHeap().begin(),
                                 publicCard.getPokerHeap().end());
}

Player::MaxCardComputer::MaxCardComputer(Player::MaxCardComputer
                                             oldComputer, Poker
                                             poker) {
    pokers.clear();
    pokers.getPokerHeap().insert(pokers.getPokerHeap().end(), oldComputer.getPokers().getPokerHeap().begin(),
                                 oldComputer.getPokers().getPokerHeap().end());
    computeMaxCardGroup_New(poker);
    pokers.getPokerHeap().push_back(poker);
}

PokerHeap Player::MaxCardComputer::getPokers() {
    return this->pokers;
}

Player::CardGroup Player::MaxCardComputer::getMaxCardGroup() {
    return this->maxGroup;
}


long long Player::CardGroup::computePower() {
    long long p = (long long) std::pow(10, 10);
    //同花顺
    if (isFlush() && isStraight()) {
        this->type = 8;
        return (9 * p) + computeCardPoint();
    }
    //同花
    if (isFlush()) {
        this->type = 5;
        return (6 * p) + computeCardPoint();
    }
    //顺子
    if (isStraight()) {
        this->type = 4;
        return (5 * p) + computeCardPoint();
    }
    //其他牌型
    this->type = getOtherType();
    switch (this->type) {
    case 7:
        return (8 * p) + computeCardPoint();
    case 6:
        return (7 * p) + computeCardPoint();
    case 3:
        return (4 * p) + computeCardPoint();
    case 2:
        return (3 * p) + computeCardPoint();
    case 1:
        return (2 * p) + computeCardPoint();
    case 0:
        return (1 * p) + computeCardPoint();
    default:
        break;
    }
    return 0;
}

long long Player::CardGroup::computeCardPoint() {
    long long res = 0;
    for (Poker &x: this->pokers.getPokerHeap()) {
        res = res * 100 + x.getPoint();
    }
    return res;
}

bool Player::CardGroup::isFlush() {
    for (int i = 1; i < 5; i++) {
        if (pokers.getPokerHeap()[i].getSuit() != pokers.getPokerHeap()[i - 1].getSuit()) {
            return false;
        }
    }
    return true;
}

bool Player::CardGroup::isStraight() {
    bool flag = true;
    pokers.sortByPoint();
    vector<Poker> &card = pokers.getPokerHeap();
    for (int i = 1; i < 5; i++) {
        if (card[i].getPoint() != card[i - 1].getPoint() - 1) {
            flag = false;
            break;
        }
    }

    //A5432特殊情况
    if (card[0].getPoint() == 14 && card[1].getPoint() == 5 && card[2].getPoint() == 4 && card[3].getPoint() == 3 &&
        card[4].getPoint() == 2) {
        flag = true;
        //将A移动到最后，方便比较
        Poker tmp = card[0];
        for (int i = 0; i < 4; i++) card[i] = card[i - 1];
        card[4] = tmp;
    }

    return flag;

}

int Player::CardGroup::getOtherType() {
    vector<Poker> &card = pokers.getPokerHeap();
    int ct = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            if (card[i].getPoint() == card[j].getPoint()) {
                ct++;
            }
        }
    }
    if (ct == 6) {
        changeCardPositionForFourKind();
        return 7;
    } else if (ct == 4) {
        changeCardPositionForFullHouse();
        return 6;
    } else if (ct == 3) {
        changeCardPositionForThreeKind();
        return 3;
    } else if (ct == 2) {
        changeCardPositionForTwoPair();
        return 2;
    } else if (ct == 1) {
        changeCardPositionForOnePair();
        return 1;
    }
    return 0;
}

void Player::CardGroup::changeCardPositionForFourKind() {
    vector<Poker> &card = pokers.getPokerHeap();
    if (card[0].getPoint() != card[1].getPoint()) {
        Poker tmp = card[0];
        card[0] = card[4];
        card[4] = tmp;
    }
}

void Player::CardGroup::changeCardPositionForFullHouse() {
    vector<Poker> &card = pokers.getPokerHeap();
    if (card[1].getPoint() != card[2].getPoint()) {
        Poker tmp = card[0];
        card[0] = card[3];
        card[3] = tmp;

        tmp = card[1];
        card[1] = card[4];
        card[4] = tmp;
    }
}

void Player::CardGroup::changeCardPositionForThreeKind() {
    vector<Poker> &card = pokers.getPokerHeap();
    if (card[0].getPoint() != card[2].getPoint()) {
        Poker tmp = card[0];
        card[0] = card[3];
        card[3] = tmp;
    }
    if (card[1].getPoint() != card[2].getPoint()) {
        Poker tmp = card[1];
        card[1] = card[4];
        card[4] = tmp;
    }
}

void Player::CardGroup::changeCardPositionForTwoPair() {
    vector<Poker> &card = pokers.getPokerHeap();
    if (card[0].getPoint() != card[1].getPoint()) {
        Poker tmp = card[0];
        card[0] = card[2];
        card[2] = tmp;
    }
    if (card[2].getPoint() != card[3].getPoint()) {
        Poker tmp = card[2];
        card[2] = card[4];
        card[4] = tmp;
    }
}

void Player::CardGroup::changeCardPositionForOnePair() {
    vector<Poker> &card = pokers.getPokerHeap();
    int c = 0;
    for (; c < 4; c++) {
        if (card[c].getPoint() == card[c + 1].getPoint()) {
            break;
        }
    }
    Poker t1 = card[c], t2 = card[c + 1];
    for (int i = c - 1; i >= 0; i--) {
        card[i + 2] = card[i];
    }
    card[0] = t1;
    card[1] = t2;
}

Player::CardGroup::CardGroup(PokerHeap pokers) {
    this->pokers = pokers;
    this->pokers.sortByPoint();
    this->power = computePower();
}

int Player::CardGroup::getPower() {
    return this->power;
}

PokerHeap Player::CardGroup::getPokers() {
    return this->pokers;
}

int Player::CardGroup::getType() {
    return this->type;
}

Player::CardGroup::CardGroup() {}



