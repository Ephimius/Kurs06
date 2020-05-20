#ifndef GENERATOR_H
#define GENERATOR_H

#include <chrono>
#include <math.h>
#include <random>
#include <utility>
#include <vector>

enum Que {REVERSE, MODIFY, GET};

struct Test {
    Que type;
    int l;
    int r;
    int v;
};

std::vector<int> GenerateIntVector() {
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    std::geometric_distribution<> d(0.000015);
    std::vector<int> rez(std::max(1000000 - d(rnd), 10));
    for (int i = 0; i < rez.size(); ++i) {
        rez[i] = rnd() % 10000 * ((int)rnd() > 0 ? 1 : -1);
    }
    return rez;
}

std::vector<int> GenerateIntVector(int len) {
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<int> rez(len);
    for (int i = 0; i < rez.size(); ++i) {
        /*if (i % 333 == 0) {
            std::cout << "g" << i << "v ";
        }
        if (i % 10000 == 9999) {
            std::cout << "\n";
        }*/
        rez[i] = rnd() % 10000 * ((int)rnd() > 0 ? 1 : -1);
    }
    return rez;
}

std::vector<Test> GenerateQuestions(int len) {
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<Test> ques(len / 4 + 5);
    for (int i = 0; i < ques.size(); ++i) {
        /*if (i % 333 == 0) {
            std::cout << "g" << i << "q ";
        }
        if (i % 10000 == 9999) {
            std::cout << "\n";
        }*/
        int tid = rnd() % 4;
        if (tid == 0) {
            ques[i].type = Que::REVERSE;
        } else if (tid == 1) {
            ques[i].type = Que::MODIFY;
        } else {
            ques[i].type = Que::GET;
        }
        ques[i].l = rnd() % len;
        if (tid == 1) {
            ques[i].v = rnd() % 10000 * ((int)rnd() > 0 ? 1 : -1);
        }
        if (tid < 2) {
            ques[i].r = rnd() % len;
            if (ques[i].l > ques[i].r) {
                std::swap(ques[i].l, ques[i].r);
            }
            ++ques[i].r;
        }
    }
    return ques;
}

#endif
