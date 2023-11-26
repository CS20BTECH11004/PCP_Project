#pragma once 

#include <atomic>
#include <memory>

template <typename T>
class LFQueue {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(T value) : data(value), next(nullptr) {};
    };
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LFQueue() : head(new Node(T())), tail(head.load()) {}

    void enqueue(T value) {
        std::unique_ptr<Node> newNode(new Node(std::move(value)));
        Node* oldTail = tail.exchange(newNode.get());
        oldTail->next = std::move(newNode);
    }

    bool is_empty(){
        Node* oldHead = head.load();
        return oldHead == tail.load();
    }

    bool dequeue(T& result) {
        Node* oldHead = head.load();
        Node* newHead = oldHead->next.get();

        if (oldHead == tail.load()) return false;  

        result = std::move(newHead->data);
        head.store(newHead);

        return true;
    }
};
