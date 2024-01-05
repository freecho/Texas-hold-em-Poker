/**
 ***************************************************
 * @Author  ：消逝中的寒光
 * @Date    ：2023/9/11
 * @Copyright (c) 消逝中的寒光 All rights reserved.
 ***************************************************
 */

#include "pokerheap.h"
#include <cstdlib>
#include <ctime>
#include <list>

PokerHeap::PokerHeap() = default;

PokerHeap::PokerHeap(vector<Poker> pokerHeap) { this->heap = pokerHeap; }

PokerHeap::~PokerHeap() = default;

void PokerHeap::sort() {
    vector<list<Poker>> suitList(4);

    for (vector<Poker>::iterator it = this->heap.begin(); it != this->heap.end(); it++) {
        suitList.at(it->getSuitSize()).push_back(*it);
    }


    vector<list<Poker>> rankList(15);
    for (vector<list<Poker>>::iterator it = suitList.begin(); it != suitList.end(); it++) {
        for (list<Poker>::iterator it2 = it->begin(); it2 != it->end(); it2++) {
            rankList.at(it2->getPoint()).push_back(*it2);
        }
    }

    this->heap.clear();

    for (vector<list<Poker> >::iterator it = rankList.begin(); it != rankList.end(); it++) {
        for (list<Poker>::iterator it2 = it->begin(); it2 != it->end(); it2++) {
            this->heap.push_back(*it2);
        }
    }
}

void PokerHeap::init() {
    this->heap.clear();
    string suits[] = {"Club", "Diamond", "Heart", "Spade"};

    for (int i = 2; i < 15; i++) {
        for (int j = 0; j < 4; j++) {
            this->heap.push_back(Poker(suits[j], i));
        }
    }
}

void PokerHeap::shuffle() {
    vector<Poker> tmpHeap;

    srand(time(0));

    while (!this->heap.empty()) {
        int i = rand() % this->heap.size();
        tmpHeap.push_back(this->heap[i]);
        this->heap.erase(this->heap.begin() + i);
    }

    heap = tmpHeap;
}

int PokerHeap::getPokerNum() { return (int) this->heap.size(); }

vector<Poker> &PokerHeap::getPokerHeap() { return this->heap; }

void PokerHeap::sortByPoint() {
    int n = this->heap.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (this->heap[i].getPoint() < this->heap[j].getPoint())
                swap(this->heap[i], this->heap[j]);
        }
    }
}

void PokerHeap::clear() { this->heap.clear(); }






















