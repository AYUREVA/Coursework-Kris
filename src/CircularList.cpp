#include "CircularList.h"

CircularList::Node::Node(int d)
    : data(d), next(this), prev(this) {}

CircularList::CircularList() : head(nullptr) {}

CircularList::~CircularList() {
    if (!head) return;
    Node* cur = head->next;
    while (cur != head) {
        Node* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
    delete head;
}

void CircularList::clear() {
    if (!head) return;
    Node* cur = head->next;
    while (cur != head) {
        Node* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
    delete head;
    head = nullptr;
}

void CircularList::add(int value) {
    Node* node = new Node(value);
    if (!head) {
        head = node;
    } else {
        Node* tail = head->prev;
        tail->next = node;
        node->prev = tail;
        node->next = head;
        head->prev = node;
    }
}

void CircularList::removeAll(int value) {
    if (!head) return;
    Node* cur = head;
    do {
        if (cur->data == value) {
            Node* toDel = cur;
            if (cur->next == cur) {
                head = nullptr;
                delete toDel;
                return;
            }
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            if (toDel == head) head = head->next;
            cur = cur->next;
            delete toDel;
        } else cur = cur->next;
    } while (cur != head);
}

void CircularList::removeBeforeValue(int value) {
    if (!head || head->next == head) return;

    Node* cur = head;
    do {
        if (cur->data == value) {
            Node* toDel = cur->prev;

            toDel->prev->next = cur;
            cur->prev = toDel->prev;

            if (toDel == head) {
                head = cur;
            }

            delete toDel;

        }
        cur = cur->next;
    } while (cur != head);
}

int CircularList::find(int value) const {
    if (!head) return -1;
    Node* cur = head;
    int idx = 0;
    do {
        if (cur->data == value) return idx;
        cur = cur->next;
        idx++;
    } while (cur != head);
    return -1;
}

int CircularList::size() const {
    if (!head) return 0;
    int cnt = 1;
    Node* cur = head->next;
    while (cur != head) { cnt++; cur = cur->next; }
    return cnt;
}

int CircularList::toNumber() const {
    if (!head) return 0;
    int num = 0;
    Node* cur = head;
    do {
        num = num * 10 + cur->data;
        cur = cur->next;
    } while (cur != head);
    return num;
}

void CircularList::print(std::ostream &out) const {
    if (!head) { out << "[empty]"; return; }
    Node* cur = head;
    do {
        out << cur->data;
        cur = cur->next;
        if (cur != head) out << " -> ";
    } while (cur != head);
}

void CircularList::copyFrom(const CircularList &other) {
    clear();
    if (!other.head) return;
    Node* cur = other.head;
    do {
        add(cur->data);
        cur = cur->next;
    } while (cur != other.head);
}

int CircularList::get(int index) const {
    if (!head || index < 0 || index >= size()) {
        return -1;
    }
    Node* cur = head;
    for (int i = 0; i < index; ++i) {
        cur = cur->next;
    }
    return cur->data;
}

CircularList::CircularList(const CircularList &other) : head(nullptr) {
    copyFrom(other);
}

CircularList& CircularList::operator=(const CircularList &other) {
    if (this != &other) {
        copyFrom(other);
    }
    return *this;
}