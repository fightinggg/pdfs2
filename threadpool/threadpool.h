#pragma once

#include "../allheader.h"

using func = void *(*)(void *);

pthread_t th1;
std::atomic<int> counter(0);


void *inner_join(void *) {
    int sz = counter;
    ::printf("wait %d thread to exit\n", sz);

    while ((sz = counter) > 0) {
        ::printf("wait %d thread to exit\n", sz);
        usleep(1000000);
    }

    return nullptr;
}

void threadinit() {
//    pthread_create(&th1, nullptr, inner_join, nullptr);
}

void threadstopAndjoin() {
//    pthread_join(th1, nullptr);
    inner_join(nullptr);
}

// simple now
void submit(func f, void *args) {
    counter++;

    struct Node {
        func f;
        void *args;
    };

    Node *node = new Node();
    node->f = f;
    node->args = args;


    auto proxyf = [](void *nodevoid) -> void * {
        Node *node = (Node *) nodevoid;
        auto res = node->f(node->args);
        delete node;
        pthread_detach(pthread_self());
        counter--;
        return res;
    };

//    auto *newthread = new pthread_t();
    pthread_t newthread;
    pthread_create(&newthread, nullptr, proxyf, (void *) node);
}