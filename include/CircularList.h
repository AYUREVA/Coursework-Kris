#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

#include <ostream>

class CircularList {
public:
    CircularList();
    ~CircularList();
    CircularList(const CircularList &other);
    CircularList& operator=(const CircularList &other);

    void clear();
    void add(int value);
    void removeAll(int value);
    void removeBeforeValue(int value);
    int get(int index) const;
    int find(int value) const;
    int size() const;
    int toNumber() const;
    void print(std::ostream &out) const;

private:
    struct Node {
        int data;
        Node *next, *prev;
        explicit Node(int d);
    };
    Node *head;

    // Вспомогательный метод для копирования
    void copyFrom(const CircularList &other);
};

#endif // CIRCULAR_LIST_H
