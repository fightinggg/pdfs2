#pragma once

#include "../allheader.h"

using func = void *(*)(void *);

BlockingQueue<pthread_t *> blockingQueue;
pthread_t th1;

volatile bool stopThread = false;

void *inner_join(void *) {
    while (!stopThread) {
        printf("x");
        ::fflush(stdout);
        pthread_t *pthread = nullptr;
        if (blockingQueue.pop(pthread, 1000)) {
            printf("join pthread start = %ld\n", (long) pthread);
            pthread_join(*pthread, nullptr);
            printf("join pthread end = %ld\n",  (long) pthread);
            delete pthread;
        } else {
            if (stopThread) {
                break;
            }
        }
    }
    ::printf("thread join exit\n");
    return nullptr;
}

void threadinit() {
    pthread_create(&th1, nullptr, inner_join, nullptr);
}

void threadstopAndjoin() {
    stopThread = true;
    pthread_join(th1, nullptr);
}

// simple now
void submit(func f, void *args) {
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
//        pthread_detach(pthread_self());
        return res;
    };

    auto *th1 = new pthread_t();
    pthread_create(th1, nullptr, proxyf, (void *) node);
    blockingQueue.push(th1);
}