/**
 ***************************************************
 * @Author  ：消逝中的寒光
 * @Date    ：2023/9/20
 * @Copyright (c) 消逝中的寒光 All rights reserved.
 ***************************************************
 */

#include "banker.h"
#include<QDebug>
Banker::Banker() {

    rank.resize(10,string());
    rank[0] = "皇家同花顺";

    rank[1] = "同花顺";

    rank[2] = "四条";

    rank[3] = "满堂彩";

    rank[4] = "同花";

    rank[5] = "顺子";

    rank[6] = "三条";

    rank[7] = "两对";

    rank[8] = "一对";

    rank[9] = "单张大牌";

    winnerNum = 0;
}

Banker::~Banker() {
    rank.clear();
}

/**
 * 给每名玩家发两张牌
 * args:
 *      players,参与游戏的玩家
 *      pokerHeap,游戏中的一副扑克牌
*/
void Banker::licensing(std::vector<Player> &players, PokerHeap &pokerHeap) {
    playerNum =(int)players.size();
    pokerHeap.shuffle();

    for (int i = 0; i < playerNum; i++) {
        if (!players[i].getIsOut()) {
            for (int j = 0; j < 2; j++) {
                Poker poker = pokerHeap.getPokerHeap().back();
                pokerHeap.getPokerHeap().pop_back();
                players[i].getHandCard().getPokerHeap().push_back(poker);
            }

        }
    }
}

/**
 * 发若干张公共牌到桌上
 * @param desk 游戏桌面（需向其中放置公共牌）
 * @param pokerHeap 游戏中的一副扑克牌
 * @param num 发公共牌的数目
 */
void Banker::licensingPublicCard(Table &desk, PokerHeap &pokerHeap, int num) {
    for (int i = 0; i < num; i++) {
        Poker poker = pokerHeap.getPokerHeap().back();       
        desk.getpublicPoker().getPokerHeap().push_back(poker);
        pokerHeap.getPokerHeap().pop_back();

    }
    //qDebug()<<desk.getPublicPokerSize();
}

/**
 * 辨别每个玩家手牌的种类
 * @param players
 */
void Banker::identifyCardType(std::vector<Player> &players) {
    int i;
    for (i = 0; i < playerNum; i++) {
        //qDebug()<<finalCards[i].getPokerHeap().size();
        if (!players[i].getIsOut()) {
            int flag;
            flag = isRoyalFlush(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "皇家同花顺";
                continue;
            }
            flag = isStraightFlush(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "同花顺";
                continue;
            }
            flag = isFourArticle(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "四条";
                continue;
            }
            flag = isFullHouse(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "满堂彩";
                continue;
            }

            flag = isSameSuit(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "同花";
                continue;
            }
            flag = isStraightDraw(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "顺子";
                continue;
            }
            flag = isThreeArticle(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "三条";
                continue;
            }
            flag = isTwoPairs(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "两对";
                continue;
            }
            flag = isAPair(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "一对";
                continue;
            }
            flag = isSingleCard(finalCards[i], i);
            if (flag == 1) {
                players_cards_type[i] = "单张大牌";
                continue;
            }
        }
    }
}

/**
 *
 * @param players
 */
void Banker::findAWinnerId(std::vector<Player> &players) {

    int i, j;

    int minrankindex = 11;

    int index = 0, rankindex = 0;

    for (i = 0; i < 2; i++) {
        if (!players[i].getIsOut()) {
            for (j = 0; j < 10; j++) {
                if (players_cards_type[i] == rank[j]) {
                    rankindex = j;
                    break;
                }
            }
            if (rankindex < minrankindex) {
                minrankindex = rankindex;
                index = i;
            }
        }
    }
    winnerId.push_back(index);
    winnerNum++;
}

/**
 * 根据玩家的手牌，结合桌面上的公共牌，判断输赢，并给玩家分发奖池
 * @param players 参与游戏的玩家，持有手牌
 * @param desk 游戏桌面，放有公共牌，以及奖池
 */
void Banker::distributeReward(vector<Player> &players, Table &desk) {
    // finalCards.clear();
    // finalCards.resize(2,PokerHeap());

    // players_cards_type.clear();
    // players_cards_type.resize(2);

    // subset.clear();
    // subset.resize(2,vector<Poker>());


    //int j = 0;

    for (int i = 0; i < players.size(); i++) {
        if (!players[i].getIsOut()) {
            finalCards[i] = players[i].getHandCard();
            finalCards[i].getPokerHeap().insert(finalCards[i].getPokerHeap().end(),
                                                desk.getpublicPoker().getPokerHeap().begin(),
                                                desk.getpublicPoker().getPokerHeap().end());
        }
    }

    identifyCardType(players);

    findAWinnerId(players);

    findOtherWinnersId(players);

    comparisionBetweenWinners();



    for (int i = 0; i < players.size(); i++) {
        if (!players[i].getIsOut()&&i==0) {
        }
    }

}
vector<int>& Banker::returnWinnerID(){
    return winnerId;
}

void Banker::findOtherWinnersId(vector<Player> &players) {
    int i; //j = 0;
    for (i = 0; i < playerNum; i++) {
        if (!players[i].getIsOut()) {
            if (i != winnerId[0]) {
                if (players_cards_type[i] == players_cards_type[winnerId[0]]) {
                    winnerId.push_back(i);
                    winnerNum++;
                }
            }
        }
    }
}

void Banker::comparisionBetweenWinners() {
    int len = winnerId.size();
    if (len == 1)
        return;
    string ranktype = players_cards_type[winnerId[0]];
    if (ranktype == rank[0]) {
        comparisionOfRoyalFlush();
    } else if (ranktype == rank[1]) {
        comparisionOfFlush();
    } else if (ranktype == rank[2]) {
        comparisionOfFourArticle();
    } else if (ranktype == rank[3]) {
        comparisionOfFullHouse();
    } else if (ranktype == rank[4]) {
        comparisionOfSameSuit();
    } else if (ranktype == rank[5]) {
        comparisionOfStraightDraw();
    } else if (ranktype == rank[6]) {
        comparisionOfThreeArticle();
    } else if (ranktype == rank[7]) {
        comparisionOfTwoPairs();
    } else if (ranktype == rank[8]) {
        comparisionOfAPair();
    } else if (ranktype == rank[9]) {
        comparisionOfSingleCard();
    }

}

void Banker::comparisionOfRoyalFlush() {
    winnerNum =(int)winnerId.size();
    return;
}

bool Banker::isRoyalFlush(PokerHeap playercard, int index) {
    //qDebug()<<playercard.getPokerHeap().size();
    playercard.sort();
    vector<Poker> temp = playercard.getPokerHeap();
    string samecolor = temp[0].getSuit();
    int start = 0, over = 0, counter = 1;
    int i, j;
    for (i = 1; i < 7; i++) {
        if (temp[i].getSuit() != samecolor) {
            samecolor = temp[i].getSuit();
            start = i;
            counter = 1;
        } else {
            counter++;
            if (counter == 5)
                break;
        }
    }
    if (7 - start >= 5) {
        counter = 0;
        for (i = start; i < 7; i++) {
            if (temp[i].getSuit() == samecolor) {
                over = i;
            }
        }
        int royalflush[5] = {10, 11, 12, 13, 14};
        for (i = 0; i < 5; i++) {
            for (j = start; j <= over; j++) {
                if (temp[j].getPoint() == royalflush[i]) {
                    counter++;
                    break;
                }
            }
        }

        if (counter == 5) {
            for (i = 0; i < 5; i++) {
                subset[index].push_back(*(new Poker(samecolor, royalflush[i])));
            }
            temp.clear();
            return 1;
        }
    }
    temp.clear();

    return 0;
}

//已经排除A为最大的情形
bool Banker::isStraightFlush(PokerHeap playercard, int index) {
    playercard.sort();

    vector<Poker> temp = playercard.getPokerHeap();

    string samecolor = temp[0].getSuit();

    int start = 0, over = 0, counter = 1;

    int i, j, k;

    for (i = 1; i < 7; i++) {
        if (temp[i].getSuit() != samecolor) {
            samecolor = temp[i].getSuit();
            start = i;
            counter = 1;
        } else {
            counter++;
            if (counter == 5)
                break;
        }
    }

    if (7 - start >= 5) {
        for (i = start; i < 7; i++) {
            if (temp[i].getSuit() == samecolor) {
                over = i;
            }
        }
        //特殊情况：A,2,3,4,5
        counter = 0;
        int specialCase[5] = {5, 4, 3, 2, 14};
        for (i = 0; i < 5; i++) {
            for (j = start; j <= over; j++) {
                if (temp[j].getPoint() == specialCase[i]) {
                    counter++;
                }
            }
        }
        if (counter == 5) {
            for (i = 0; i < 5; i++) {
                for (j = start; j <= over; j++) {
                    if (temp[j].getPoint() == specialCase[i]) {
                        subset[index].push_back(temp[j]);
                    }
                }
            }
            temp.clear();
            return 1;
        }

        counter = 1;
        for (j = over; j >= start + 1; j--) {
            if (temp[j].getPoint() - temp[j - 1].getPoint() == 1) {
                counter++;
                if (counter == 5) {
                    for (k = j - 1 + 4; k >= j - 1; k--) {
                        subset[index].push_back(temp[k]);
                    }
                    temp.clear();
                    return 1;
                }
            } else {
                counter = 1;
            }
        }
        if (counter != 5) {
            temp.clear();
            return 0;
        }
    } else {
        temp.clear();
        return 0;
    }
    temp.clear();
    return 0;
}

bool Banker::isFourArticle(PokerHeap playercard, int index) {
    playercard.sortByPoint();
    vector<Poker> temp = playercard.getPokerHeap();
    int i, j, k;

    for (i = 0; i < 7; i++) {
        int counter = 1;
        for (j = i + 1; j < 7; j++) {
            if (temp[j].getPoint() == temp[i].getPoint()) {
                counter++;
            }
        }
        if (counter == 4) {
            for (k = i; k < i + 4; k++) {
                subset[index].push_back(temp[k]);
            }

            for (k = 0; k < 7; k++) {
                if (temp[k].getPoint() != subset[index][0].getPoint()) {
                    subset[index].push_back(temp[k]);
                    break;
                }
            }
            temp.clear();
            return 1;
        }
    }
    temp.clear();
    return 0;
}

bool Banker::isFullHouse(PokerHeap playercard, int index) {
    playercard.sortByPoint();
    vector<Poker> temp = playercard.getPokerHeap();
    int i, j, k;
    int counter = 0;
    int mark;
    int flag1 = 0,flag2 = 0;
    for (i = 0; i < 7; i++) {

        counter = 0;
        for (j = i + 1; j < 7; j++) {
            if (temp[j].getPoint() == temp[i].getPoint()) {
                counter++;
            }
        }
        if (counter == 2) {
            mark = i;
            flag1 = 1;
            break;
        }
    }

    if (flag1 == 0) {
        temp.clear();
        return 0;
    }

    for (i = 0; i < 7; i++) {
        counter = 0;
        if (temp[mark].getPoint() != temp[i].getPoint()) {
            for (j = i + 1; j < 7; j++) {

                if (temp[j].getPoint() == temp[i].getPoint()) {
                    counter++;
                    flag2 = 1;
                    int times = 0;
                    for (k = 0; k < 7; k++) {
                        if (temp[k].getPoint() == temp[mark].getPoint()) {
                            subset[index].push_back(temp[k]);
                        }
                    }
                    for (k = 0; k < 7; k++) {
                        if (temp[k].getPoint() == temp[i].getPoint()) {
                            times++;
                            subset[index].push_back(temp[k]);
                            if (times == 2)
                                break;
                        }
                    }
                    temp.clear();
                    return 1;
                }
            }
        }
    }
    temp.clear();
    return 0;
}

bool Banker::isSameSuit(PokerHeap playercard, int index) {
    // qDebug()<<"判断同花";
    playercard.sort();
    vector<Poker> temp = playercard.getPokerHeap();
    vector<int> num(4,0);
    bool flag = false;
    for(int i=0;i<7;i++) num[temp[i].getSuitSize()]++;
    vector<Poker> sameList;
    for(int i=0;i<4;i++) if(num[i] > 3){
            for(int j=0;j<7;j++){
                if(temp[j].getSuitSize() == i){
                    sameList.push_back(temp[j]);
                }
            }
            flag = true;
        }
    if(flag){
        for(int i = 0;i<5;i++){
            subset[index].push_back(temp[i]);
        }
    }
    sameList.clear();
    temp.clear();
    num.clear();
    return flag;
}

bool Banker::isStraightDraw(PokerHeap playercard, int index) {
    playercard.sortByPoint();
    vector<Poker> temp = playercard.getPokerHeap();
    int i, j, counter = 0;
    //特殊情况：A,2,3,4,5
    int specialCase[5] = {5, 4, 3, 2, 14};
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 7; j++) {
            if (temp[j].getPoint() == specialCase[i]) {
                counter++;
                break;
            }
        }
    }
    if (counter == 5) {
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 7; j++) {
                if (temp[j].getPoint() == specialCase[i]) {
                    subset[index].push_back(temp[j]);
                }
            }
        }
        temp.clear();
        return 1;
    }
    counter = 1;

    for (i = 0; i < 7 - 1; i++) {
        if (temp[i].getPoint() - temp[i + 1].getPoint() == 1) {
            counter++;
            if (counter == 5) {
                subset[index].push_back(temp[i + 1]);
                int times = 0;
                for (j = i; j >= 0; j--) {
                    if (temp[j + 1].getPoint() - temp[j].getPoint() == -1) {
                        times++;
                        subset[index].push_back(temp[j]);
                        if (times == 4)
                            break;
                    }
                }
                Poker media;
                media = subset[index][0];
                subset[index][0] = subset[index][4];
                subset[index][4] = media;
                media = subset[index][1];
                subset[index][1] = subset[index][3];
                subset[index][3] = media;
                temp.clear();
                return 1;
            }
        } else if (temp[i].getPoint() - temp[i + 1].getPoint() == 0) {
            continue;
        } else {
            counter = 1;
        }
    }
    temp.clear();
    return 0;
}

bool Banker::isThreeArticle(PokerHeap playercard, int index) {
    int i, j, k;
    int counter = 0;
    playercard.sortByPoint();
    vector<Poker> temp = playercard.getPokerHeap();
    for (i = 0; i < 7; i++) {
        counter = 0;
        for (j = i + 1; j < 7; j++) {
            if (temp[j].getPoint() == temp[i].getPoint()) {
                counter++;
                if (counter == 2) {
                    for (k = j; k >= j - 2; k--) {
                        subset[index].push_back(temp[k]);
                    }
                    int times = 0, mark = -1;
                    for (k = 0; k < 7; k++) {
                        if (temp[k].getPoint() != temp[i].getPoint() && temp[k].getPoint() != mark) {
                            times++;
                            mark = temp[k].getPoint();
                            subset[index].push_back(temp[k]);
                            if (times == 2)
                                break;
                        }
                    }
                    temp.clear();
                    return 1;
                }
            } else {
                counter = 0;
                break;
            }
        }
    }
    temp.clear();
    return 0;
}

bool Banker::isTwoPairs(PokerHeap playercard, int index) {
    int firstpairs_num = -1;
    int secondpairs_num = -1;
    int i, j;
    playercard.sortByPoint();

    vector<Poker> temp = playercard.getPokerHeap();
    for (i = 0; i < 7; i++) {
        for (j = i + 1; j < 7; j++) {
            if (temp[j].getPoint() == temp[i].getPoint()) {
                firstpairs_num = temp[j].getPoint();
                break;
            }
        }
        if (firstpairs_num != -1) {
            break;
        }
    }
    if (firstpairs_num == -1) {
        temp.clear();
        return 0;
    }
    for (i = 0; i < 7; i++) {
        if (temp[i].getPoint() != firstpairs_num) {
            for (j = i + 1; j < 7; j++) {
                if (temp[j].getPoint() == temp[i].getPoint()) {
                    secondpairs_num = temp[j].getPoint();
                    break;
                }
            }
        }
    }
    if (firstpairs_num != -1 && secondpairs_num != -1) {
        for (i = 0; i < 7; i++) {
            if (temp[i].getPoint() == firstpairs_num) {
                subset[index].push_back(temp[i]);
            }
        }

        for (i = 0; i < 7; i++) {
            if (temp[i].getPoint() == secondpairs_num) {
                subset[index].push_back(temp[i]);
            }
        }
        for (i = 0; i < 7; i++) {
            if (temp[i].getPoint() != firstpairs_num && temp[i].getPoint() != secondpairs_num) {
                subset[index].push_back(temp[i]);
                break;
            }
        }
        temp.clear();
        return 1;
    } else
        temp.clear();
    return 0;
}

bool Banker::isAPair(PokerHeap playercard, int index) {
    playercard.sortByPoint();
    int i, j, k;
    vector<Poker> temp = playercard.getPokerHeap();

    for (i = 0; i < 7; i++) {
        for (j = i + 1; j < 7; j++) {
            if (temp[j].getPoint() == temp[i].getPoint()) {
                subset[index].push_back(temp[i]);
                subset[index].push_back(temp[j]);
                int times = 0;
                for (k = 0; k < 7; k++) {
                    if (temp[k].getPoint() != temp[j].getPoint()) {
                        times++;
                        subset[index].push_back(temp[k]);
                        if (times == 3)
                            break;
                    }
                }
                temp.clear();
                return 1;
            }
        }
    }
    temp.clear();
    return 0;
}

bool Banker::isSingleCard(PokerHeap playercard, int index) {
    playercard.sortByPoint();
    vector<Poker> temp = playercard.getPokerHeap();
    for (int i = 0; i < 5; i++) {
        subset[index].push_back(temp[i]);
    }
    temp.clear();
    return 1;
}

//已经排除A为最大的情况
void Banker::comparisionOfFlush() {
    int len = (int)winnerId.size();
    int i, j;
    //将牌点最大的选手下标放到winnerId的第一位；
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
                break;
            } else if (subset[winnerId[i]][j].getPoint() < subset[winnerId[i + 1]][j].getPoint()) {
                break;
            } else
                break;
        }
    }
    //检查是否存在有与下标为winnerId[len-1]等大手牌的候补选手
    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[len - 1]][j].getPoint() != subset[winnerId[i]][j].getPoint()) {
                break;
            }
        }
        if (j == 5) {
            temp.push_back(winnerId[i]);
        }
    }
    winnerId.assign(temp.begin(), temp.end());
    winnerNum =(int) winnerId.size();
    temp.clear();
    return;
}

void Banker::comparisionOfFourArticle() {
    int len =(int) winnerId.size();
    int i;
    for (i = 0; i < len - 1; i++) {
        if (subset[winnerId[i]][0].getPoint() > subset[winnerId[i + 1]][0].getPoint()) {
            int temp = winnerId[i];
            winnerId[i] = winnerId[i + 1];
            winnerId[i + 1] = temp;
        } else if (subset[winnerId[i]][0].getPoint() ==
                   subset[winnerId[i + 1]][0].getPoint())//有可能出现相同的4张同点数的牌在庄家的公共牌中
        {
            if (subset[winnerId[i]][4].getPoint() > subset[winnerId[i + 1]][4].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
            }
        }
    }
    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        if (subset[winnerId[len - 1]][0].getPoint() == subset[winnerId[i]][0].getPoint() &&
            subset[winnerId[len - 1]][4].getPoint() == subset[winnerId[i]][4].getPoint()) {
            temp.push_back(winnerId[i]);
        }
    }
    winnerId.assign(temp.begin(), temp.end());
    winnerNum =(int)winnerId.size();
    temp.clear();
    return;
}

void Banker::comparisionOfFullHouse() {
    int len =(int)winnerId.size();
    int i;
    for (i = 0; i < len - 1; i++) {
        if (subset[winnerId[i]][0].getPoint() > subset[winnerId[i + 1]][0].getPoint()) {
            int temp = winnerId[i];
            winnerId[i] = winnerId[i + 1];
            winnerId[i + 1] = temp;
        } else if (subset[winnerId[i]][0].getPoint() == subset[winnerId[i + 1]][0].getPoint()) {
            if (subset[winnerId[i]][3].getPoint() > subset[winnerId[i + 1]][3].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
            }
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        if (subset[winnerId[len - 1]][0].getPoint() == subset[winnerId[i]][0].getPoint() &&
            subset[winnerId[len - 1]][3].getPoint() == subset[winnerId[i]][3].getPoint()) {
            temp.push_back(winnerId[i]);
        }
    }
    winnerId.assign(temp.begin(), temp.end());
    winnerNum =(int)winnerId.size();
    temp.clear();
    return;

}

void Banker::comparisionOfSameSuit() {
    int len =(int)winnerId.size();
    int i, j;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
                break;
            } else if (subset[winnerId[i]][j].getPoint() < subset[winnerId[i + 1]][j].getPoint()) {
                break;
            } else
                continue;
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[len - 1]][j].getPoint() != subset[winnerId[i]][j].getPoint()) {
                break;
            }
        }
        if (j == 5) {
            temp.push_back(winnerId[i]);
        }
    }

    winnerId.assign(temp.begin(), temp.end());
    winnerNum =(int)winnerId.size();
    temp.clear();
    return;
}


void Banker::comparisionOfStraightDraw() {
    int len =(int)winnerId.size();
    int i, j;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint() ||
                subset[winnerId[i + 1]][4].getPoint() == 14) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
                break;
            } else if (subset[winnerId[i]][j].getPoint() < subset[winnerId[i + 1]][j].getPoint()) {
                break;
            } else
                continue;
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[len - 1]][j].getPoint() != subset[winnerId[i]][j].getPoint()) {
                break;
            }
        }
        if (j == 5) {
            temp.push_back(winnerId[i]);
        }
    }

    winnerId.assign(temp.begin(), temp.end());
    winnerNum =(int)winnerId.size();
    temp.clear();
    return;
}


void Banker::comparisionOfThreeArticle() {
    int len =(int) winnerId.size();
    int i, j;
    for (i = 0; i < len - 1; i++) {
        if (subset[winnerId[i]][0].getPoint() > subset[winnerId[i + 1]][0].getPoint()) {
            int temp = winnerId[i];
            winnerId[i] = winnerId[i + 1];
            winnerId[i + 1] = temp;
        } else if (subset[winnerId[i]][0].getPoint() == subset[winnerId[i + 1]][0].getPoint()) {
            for (j = 3; j <= 4; j++) {
                if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint()) {
                    int temp = winnerId[i];
                    winnerId[i] = winnerId[i + 1];
                    winnerId[i + 1] = temp;
                }
            }
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        if (subset[winnerId[len - 1]][0].getPoint() == subset[winnerId[i]][0].getPoint() &&
            subset[winnerId[len - 1]][3].getPoint() == subset[winnerId[i]][3].getPoint() &&
            subset[winnerId[len - 1]][4].getPoint() == subset[winnerId[i]][4].getPoint()) {
            temp.push_back(winnerId[i]);
        }
    }

    winnerId.assign(temp.begin(), temp.end());
    winnerNum = (int)winnerId.size();
    temp.clear();
    return;

}

void Banker::comparisionOfTwoPairs() {
    int len = (int)winnerId.size();
    int i, j;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
                break;
            } else if (subset[winnerId[i]][j].getPoint() < subset[winnerId[i + 1]][j].getPoint()) {
                break;
            } else
                continue;
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[len - 1]][j].getPoint() != subset[winnerId[i]][j].getPoint()) {
                break;
            }
        }
        if (j == 5) {
            temp.push_back(winnerId[i]);
        }
    }

    winnerId.assign(temp.begin(), temp.end());
    winnerNum = (int)winnerId.size();
    temp.clear();
    return;

}


void Banker::comparisionOfAPair() {
    int len = (int)winnerId.size();
    int i, j;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
                break;
            } else if (subset[winnerId[i]][j].getPoint() < subset[winnerId[i + 1]][j].getPoint()) {
                break;
            } else
                continue;
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[len - 1]][j].getPoint() != subset[winnerId[i]][j].getPoint()) {
                break;
            }
        }
        if (j == 5) {
            temp.push_back(winnerId[i]);
        }
    }

    winnerId.assign(temp.begin(), temp.end());
    winnerNum = (int)winnerId.size();
    temp.clear();
    return;

}


void Banker::comparisionOfSingleCard() {
    int len = (int)winnerId.size();
    int i, j;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[i]][j].getPoint() > subset[winnerId[i + 1]][j].getPoint()) {
                int temp = winnerId[i];
                winnerId[i] = winnerId[i + 1];
                winnerId[i + 1] = temp;
                break;
            } else if (subset[winnerId[i]][j].getPoint() < subset[winnerId[i + 1]][j].getPoint()) {
                break;
            } else
                continue;
        }
    }

    vector<int> temp;
    temp.push_back(winnerId[len - 1]);
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < 5; j++) {
            if (subset[winnerId[len - 1]][j].getPoint() != subset[winnerId[i]][j].getPoint()) {
                break;
            }
        }
        if (j == 5) {
            temp.push_back(winnerId[i]);
        }
    }

    winnerId.assign(temp.begin(), temp.end());
    winnerNum = (int)winnerId.size();
    temp.clear();
    return;
}

void Banker::billing(Table &desk,vector<Player*> &winners){
    int total = desk.getJackPot();
    int num = winners.size();
    // qDebug()<<"奖池："<<total;
    // qDebug()<<"每个胜者分："<<total/num;
    for (int var = 0; var < num; ++var) {
        winners[var]->addMoney(total/num);
        // qDebug()<<"第"<<var<<"个赢家的钱:"<<winners[var]->getMoney();
    }


}







