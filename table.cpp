#include "table.h"
#include<QDebug>
Table::Table() { this->betMount = this->jackPot = 0; }

Table::~Table() = default;
void Table::setbetMount(int amount) {
    this->betMount=amount;
}

void Table::initBetMount() { this->betMount = 0; }

void Table::initjackPot() { this->jackPot = 0; }

void Table::publicPokerSort() { this->publicPoker.sort(); }

int Table::getPublicPokerSize() { return this->publicPoker.getPokerNum(); }

int Table::getbetMount() { return this->betMount; }

int Table::getJackPot() { return this->jackPot; }

void Table::addJackPot(int num) { this->jackPot += num; }

PokerHeap& Table::getpublicPoker() { return this->publicPoker; }

void Table::clearPublicPoker() { this->publicPoker.clear(); }

