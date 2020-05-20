#ifndef TESTER_H
#define TESTER_H

#include "treap_library.h"
#include "generator.h"

#include <algorithm>

template <class F>
int Testing(std::vector<int> data, std::vector<Test> que, F func, int bas) {
    Treap<int, F> treap0(data, func);
    Treap<int, F> treap1(std::vector<int>(), func);
    treap1 = std::move(treap0);
    Treap<int, F> treap(std::move(treap1));
    for (int i = 0; i < que.size(); ++i) {
        /*if (i % 333 == 0) {
            std::cout << "l" << que.size() - i << " ";
        }*/
        if (que[i].type == Que::REVERSE) {
            treap.Reverse(que[i].l, que[i].r);
            std::reverse(data.begin() + que[i].l, data.begin() + que[i].r);
        } else if (que[i].type == Que::MODIFY) {
            treap.Modify(que[i].v, que[i].l, que[i].r);
            for (int j = que[i].l; j < que[i].r; ++j) {
                data[j] = func(data[j], que[i].v);
            }
        } else {
            if (treap.GetAt(que[i].l) != data[que[i].l]) {
                return i;
            }
        }
    }
    for (int i = 0; i < data.size(); ++i) {
        if (treap.GetAt(i) != data[i]) {
            return que.size();
        }
    }
    return -1;
}

template <class F>
int TestingToVector(size_t mins, size_t maxs, F func, int farg) {
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    for (size_t cur_size = mins; cur_size < maxs; ++cur_size) {
        std::vector<int> data = GenerateIntVector(cur_size);
        Treap<int, F> treap(data, func);
        size_t l = rnd() % cur_size;
        size_t r = rnd() % cur_size;
        if (l > r) {
            std::swap(l, r);
        }
        ++r;
        std::vector<int> curve = treap.ToVector(l, r);
        for (size_t i = l; i < r; ++i) {
            if (curve[i - l] != data[i]) {
                return cur_size;
            }
        }
    }
    return -1;
}

template <class F>
int TestingSM(size_t mins, size_t maxs, F func, int farg) {
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    for (size_t cur_size = mins; cur_size < maxs; ++cur_size) {
        std::vector<int> data = GenerateIntVector(cur_size);
        Treap<int, F> treap(data, func);
        Treap<int, F> treapl({}, func);
        Treap<int, F> treapr({}, func);
        size_t ls = rnd() % (cur_size - 1) + 1;
        Treap<int, F>::Split(&treap, &treapl, &treapr, ls);
        for (size_t i = 0; i < ls; ++i) {
            if (treapl.GetAt(i) != data[i]) {
                return cur_size;
            }
        }
        for (size_t i = ls; i < cur_size; ++i) {
            if (treapr.GetAt(i - ls) != data[i]) {
                return cur_size;
            }
        }
        Treap<int, F>::Merge(&treap, &treapr, &treapl);
        for (size_t i = 0; i < ls; ++i) {
            if (treap.GetAt(i + cur_size - ls) != data[i]) {
                return cur_size;
            }
        }
        for (size_t i = ls; i < cur_size; ++i) {
            if (treap.GetAt(i - ls) != data[i]) {
                return cur_size;
            }
        }
        treap.MoveToEnd(cur_size - ls);
        for (size_t i = 0; i < cur_size; ++i) {
            if (treap.GetAt(i) != data[i]) {
                return cur_size;
            }
        }
    }
    return -1;
}

#endif
