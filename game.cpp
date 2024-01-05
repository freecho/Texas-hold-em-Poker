#include "game.h"
#include "ui_game.h"
#include<QPixmap>
#include<QDebug>
Game::Game(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Game)
{
    ui->setupUi(this);
    ui->front->setCurrentIndex(1);
    ui->gen->close();
    ui->jia->close();
    ui->qi->close();
    ui->Round->setText("第"+QString::number(num)+"轮");
    connect(ui->giveup,&QPushButton::clicked,[=](){
        ui->front->setCurrentIndex(1);
    });
    connect(ui->begin,&QPushButton::clicked,[=](){
        ui->front->setCurrentIndex(0);
        startGame(1,1);
    });
    connect(ui->exit,&QPushButton::clicked,[=](){
        exit(0);
    });
    connect(this,&Game::NumChanged,[=](int newNUM){
        ui->Round->setText("第"+QString::number(newNUM)+"局"+QString::number(num)+"轮");
    });
    //跟注
    connect(ui->gen,&QPushButton::clicked,[=](){
        ui->ma1->setText(QString::number(players[0].getMoney()));
        players[0].followBet(desk,desk.getbetMount());
        // qDebug()<<desk.getbetMount();
        ui->ma0->setText(QString::number(desk.getJackPot()));
        setNum(num+1);
    });
    //加注
    connect(ui->jia,&QPushButton::clicked,[=](){
        ui->ma1->setText(QString::number(players[0].getMoney()));
            //qDebug()<<desk.getbetMount();
        players[0].addBet(desk,desk.getbetMount());
        setNum(num+1);
    });
    //弃牌
    connect(ui->qi,&QPushButton::clicked,[=](){
        players[0].outGame();
        easyWinner = 1;
        viewAicard();
        m.setText("ai胜利");
        m.show();
        //qDebug()<<"ai胜利";

    });
     connect(&this->res,&QMessageBox::finished,[=](){

        ui->front->setCurrentIndex(1);}

);
    connect(&this->m,&QMessageBox::finished,[=](){
        clearHandPic();
        setNum(1);
        setNUM(Num+1);
        viewMoney();
        if(players[0].getMoney()>0&&players[1].getMoney()>0){

            vector<Player*> winner;
            if(banker.winnerId.size()>0){
                for (int var = 0; var < banker.winnerId.size(); ++var) {
                    winner.push_back(&players[banker.winnerId[var]]);
                }
            }
            else{
                winner.push_back(&players[easyWinner]);
            }

            // qDebug()<<"结算前两个人+奖池一共的钱："<<players[0].getMoney()+players[1].getMoney()+desk.getJackPot();
            banker.billing(desk,winner);
            // qDebug()<<"两个人一共的钱："<<players[0].getMoney()+players[1].getMoney();

            continueGame();}
        else {

            if(banker.winnerId.size()==2)
            {
                res.setText("人类和AI共赢");
            }
            else if(players[0].getMoney()>0){
                res.setText("人类获得了最终的胜利!");
            }
            else{
                res.setText("AI获得了最终的胜利!");
            }
            res.show();

        }
    });
    connect(this,&Game::numChanged,[=](int newNum){
        ui->Round->setText("第"+QString::number(Num)+"局"+QString::number(newNum)+"轮");
        if(newNum==2){
            otherRound(true);
        }else if(newNum>2&&newNum<5){
            otherRound(false);
        }else if(newNum>=5){
             viewAicard();
            ui->gen->close();
            ui->qi->close();
            ui->jia->close();
            judgeWin();


            // qDebug()<<"比较结束！\n";
            QString s;
            for (int var = 0; var < 5; ++var) {
                s+=QString::number(banker.subset[0][var].getPoint());
            }

            if(banker.players_cards_type[0].size()>0)
                qDebug()<<"man: "<<s<<" "<<QString::fromStdString(banker.players_cards_type[0]);
            else qDebug()<<"man: "<<s;
            s.clear();
            for (int var = 0; var < 5; ++var) {
                s+=QString::number(banker.subset[1][var].getPoint());
            }
            if(banker.players_cards_type[1].size()>0)
                qDebug()<<"AI: "<<s<<" "<<QString::fromStdString(banker.players_cards_type[1]);
            else qDebug()<<"AI: "<<s;

            // qDebug()<<"winnerid:";
            // for (int var = 0; var < banker.winnerId.size(); ++var) {
            //     qDebug()<<QString::number(banker.winnerId[var]);
            // }

            if(players[banker.returnWinnerID().back()].getIsAI()){

                m.setText("ai胜利");
                m.show();
            }else{

                m.setText("人类胜利");
                m.show();
            }
            setNum(1);

        }
    });

}

Game::~Game()
{
    delete ui;
}
void Game:: viewAicard(){
    QString point2=QString::number(players[1].getHandCard().getPokerHeap()[0].getPoint());
    QString ColoR2=color[QString::fromStdString(players[1].getHandCard().getPokerHeap()[0].getSuit())];
    QPixmap*p1=new QPixmap(url+point2+ColoR2+end);
    p1->scaled(ui->q1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->q1->setScaledContents(true);
    ui->q1->setPixmap(*p1);

    QString point3=QString::number(players[1].getHandCard().getPokerHeap()[1].getPoint());
    QString ColoR3=color[QString::fromStdString(players[1].getHandCard().getPokerHeap()[1].getSuit())];
    QPixmap*pp1=new QPixmap(url+point3+ColoR3+end);
    pp1->scaled(ui->q2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->q2->setScaledContents(true);
    ui->q2->setPixmap(*pp1);
}
void Game::viewCard(){
    QString point=QString::number(players[0].getHandCard().getPokerHeap()[0].getPoint());
    QString ColoR=color[QString::fromStdString(players[0].getHandCard().getPokerHeap()[0].getSuit())];
    QPixmap*p=new QPixmap(url+point+ColoR+end);
    p->scaled(ui->p1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->p1->setScaledContents(true);
    ui->p1->setPixmap(*p);

    QString point1=QString::number(players[0].getHandCard().getPokerHeap()[1].getPoint());
    QString ColoR1=color[QString::fromStdString(players[0].getHandCard().getPokerHeap()[1].getSuit())];
    QPixmap*pp=new QPixmap(url+point1+ColoR1+end);
    pp->scaled(ui->p2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->p2->setScaledContents(true);
    ui->p2->setPixmap(*pp);
}
void Game::setNUM(int newNUM){
    if(newNUM!=Num){
        Num=newNUM;
    }
    emit NumChanged(newNUM);
}

void Game::setNum(int newNum){
    if(newNum!=num){
        num=newNum;
    }
    emit numChanged(newNum);
}

void Game::clearHandPic(){
    ui->p1->clear();
    ui->p2->clear();
    QPixmap*p=new QPixmap(":/new/prefix1/poke/Back.png");
    p->scaled(ui->q1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->q1->setScaledContents(true);
    ui->q1->setPixmap(*p);
    p->scaled(ui->q2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->q2->setScaledContents(true);
    ui->q2->setPixmap(*p);
}
void Game::viewPublicPoker(PokerHeap Public,int size){
    for(int i=0;i<size;i++){
        QString point=QString::number(Public.getPokerHeap()[i].getPoint());
        QString ColoR=color[QString::fromStdString(Public.getPokerHeap()[i].getSuit())];
        QPixmap*p=new QPixmap(url+point+ColoR+end);
        p->scaled(Labels.at(i)->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        Labels.at(i)->setScaledContents(true);
        Labels.at(i)->setPixmap(*p);
    }
}
void Game::viewMoney(){
    ui->ma1->setText(QString::number(players[0].getMoney()));
    ui->ma2->setText(QString::number(players[1].getMoney()));
}
void Game::choose(bool isRoundOne){
    ui->gen->show();
    ui->qi->show();
    ui->jia->show();
    if(isRoundOne){
        ui->gen->setText("下注");
    }
    else{
        ui->gen->setText("跟注");
    }
    ui->jia->setText("加注");
    ui->qi->setText("弃牌");
}
void Game::round(queue<Player> &playersNeedtoBet,bool isRoundTwo) {
    //int lastBetNum = 0;
    ui->label_4->clear();
    queue<Player> playerBeted;
    if(isRoundTwo){
        banker.licensingPublicCard(desk, pokerHeap, 3);
        desk.publicPokerSort();
    }
    else{
        banker.licensingPublicCard(desk, pokerHeap, 1);
        desk.publicPokerSort();
    }
    viewPublicPoker(desk.getpublicPoker(),desk.getPublicPokerSize());

    if(num==2){
        players[1].thinkAfterFlop(desk,desk.getbetMount());
        // qDebug()<<"2"<<desk.getbetMount();

        viewMoney();
    }if(num==3){
        players[1].thinkAfterTurn(desk,desk.getbetMount());
        // qDebug()<<"3"<<desk.getbetMount();
        viewMoney();
    }if(num==4){
        players[1].thinkAfterRiver(desk,desk.getbetMount());
        // qDebug()<<"4"<<desk.getbetMount();
        viewMoney();
    }
    if(players[1].getFlag()==1){
        ui->label_4->setText("AI跟注");
    }else{
        ui->label_4->setText("AI加注");
    }
    ui->ma0->setText(QString::number(desk.getJackPot()));
    if(players[1].getIsOut()){
        // qDebug()<<"AI投降！\n";
        ui->label_4->clear();
        easyWinner = 0;
        viewAicard();
        m.setText("AI投降，人类胜利");
        m.show();
    }
    choose(false);
    ui->ma0->setText(QString::number(desk.getJackPot()));

    //qDebug()<<desk.getPublicPokerSize();

//    while (!playersNeedtoBet.empty()) {

////        //游戏只剩一人，跳过本轮下注"
////        if (outnum >= personnum) {
////            return;
////        }
//        //当前下注玩家
//        Player player = playersNeedtoBet.front();
//        playersNeedtoBet.pop();


//        //真人下注
//        if (!player.getIsAI()) {
//            //本轮还没有人下注
//            if (desk.getbetMount() == 0) {
//                //显示公共牌


//                //显示手牌


//                //显示余额
//                viewMoney();
//                //选择 下注 让牌 弃牌
//                choose(false);
//                //下注值
//                ui->ma0->setText(QString::number(desk.getJackPot()));

//            }
//            //本轮已经有人下注
//            else {
//                //选择 跟注 加注 弃牌
//                choose(false);

//                //下注值
//                ui->ma0->setText(QString::number(desk.getJackPot()));
//            }

//        }
//        //AI
//        else {
//            if(num==2){
//                players[1].thinkAfterFlop(desk,desk.getbetMount());
//                qDebug()<<"2";
//                 viewMoney();
//            }if(num==3){
//                 players[1].thinkAfterTurn(desk,desk.getbetMount());
//                 qDebug()<<"3";
//                  viewMoney();
//            }if(num==4){
//                 players[1].thinkAfterRiver(desk,desk.getbetMount());
//                  qDebug()<<"4";
//                  viewMoney();
//            }ui->ma0->setText(QString::number(desk.getJackPot()));
//        }

//    }
}

void Game::judgeWin() {
    banker.distributeReward(players, desk);
}

void Game::firstRound() {
    queue<Player> q;
    for (int i = 0; i < players.size(); i++) {
        players[i].initBetHasPut();
        q.push(players[i]);
    }
    choose(true);
    ui->ma0->setText(QString::number(desk.getJackPot()));
}

void Game::otherRound(bool isRoundTwo) {
    queue<Player> q;
    //qDebug()<<"11";
    for (int i = 0; i < players.size(); i++) {
        if (!players[i].getIsOut()) {
            players[i].initBetHasPut();
            q.push(players[i]);
        }
    }
    round(q,isRoundTwo);
}

/**
 *
 * @param personNum 玩家总人数
 * @param AINum AI人数
 */
void Game::startGame(int personNum, int AINum) {
    // 初始化游戏信息
    int id = 0;
    outnum = 0;
    players.clear();
    setNUM(1);
    setNum(1);
    for (int i = 0; i <=personNum - AINum; i++) {
        Player player(id++, false);
        //初始筹码
        player.setMoney(starMoney);
        players.push_back(player);
    }
    for (int i = 0; i <AINum; i++) {
        Player player(id++, true);
        //初始筹码
        player.setMoney(starMoney);
        players.push_back(player);
    }
    initPublicPoker();
    initColor();
    desk = Table();
    banker=Banker();
    personnum = personNum;

    continueGame();

}
void Game::initPublicPoker(){
    Labels.append(ui->a);
    Labels.append(ui->b);
    Labels.append(ui->c);
    Labels.append(ui->d);
    Labels.append(ui->e);
}
void Game::clearPublicPic(){
    for(int i=0;i<Labels.count();i++){
        Labels.at(i)->clear();
    }
}
void Game::initColor(){
    color["Spade"]="1";
    color["Diamond"]="2";
    color["Heart"]="3";
    color["Club"]="4";
}
void Game::continueGame() {

    qDebug()<<"\n";


    ui->label_4->clear();

    banker.finalCards.clear();
    banker.finalCards.resize(2,PokerHeap());

    banker.players_cards_type.clear();
    banker.players_cards_type.resize(2,string());

    banker.subset.clear();
    banker.subset.resize(2,vector<Poker>());

    banker.winnerId.clear();

    int count, f;
    clearPublicPic();
    //clearHandPic();
    // 重置弃牌玩家
    outnum = 0;
    for (int i = 0; i < players.size(); i++) {
        players[i].backToGame();
        players[i].clearHandCard();
    }

    // 重载桌面(牌局)
    desk.initjackPot();
    desk.initBetMount();
    ui->ma0->setText(QString::number(desk.getbetMount()));
    desk.clearPublicPoker();


    // 重置牌堆
    pokerHeap.init();
    pokerHeap.shuffle();
    // 下大小盲注(自动强制进行)
    // desk.initBetMount();
    players[0].bet(desk, 50);
    ui->ma1->setText(QString::number(players[0].getMoney()));
    players[1].bet(desk, 100);
     //qDebug()<<desk.getbetMount()<<' '<<desk.getJackPot();
    ui->ma2->setText(QString::number(players[1].getMoney()));
    ui->ma0->setText(QString::number(desk.getJackPot()));
    // 庄家给每名玩家发牌

    // qDebug()<<"盲注后两个人+奖池一共的钱："<<players[0].getMoney()+players[1].getMoney()+desk.getJackPot();
    banker.licensing(players, pokerHeap);
    // 把每名玩家的手牌排序

    for (int i = 0; i < players.size(); i++) {
        players[i].handCardSort();
    }

    //显示摸到的底牌
    viewCard();

    //下注
    count = 0;
    //players[1].thinkAfterFlop(desk,desk.getbetMount());
    firstRound();

    // for (int i = 0; i < personnum; i++) {
    //     if (!players[i].getIsOut()) {
    //         count++;
    //         f = i;
    //     }
    // }
    // if (count == 1) {
    //     int newmoney = desk.getJackPot();
    //     newmoney += players[f].getMoney();
    //     players[f].setMoney(newmoney);
    //     //胜利


    //     goto end;
    // }

//    // 庄家发1张公共牌
//    banker.licensingPublicCard(desk, pokerHeap, 1);

//    // 公共牌堆排序

//    desk.publicPokerSort();

//    //获取公共牌
//    desk.getpublicPoker();

//    desk.initBetMount();




// end:
//     // 游戏只剩一人
//     if (outnum == personnum - 1) {
//         for (int i = 0; i < personnum; i++) {
//             if (!players[i].getIsOut()) {
//                 players[i].setMoney(players[i].getMoney() + desk.getJackPot());
//                 break;
//             }
//         }
//     }
}



