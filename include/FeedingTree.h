#ifndef FEEDING_TREE_H
#define FEEDING_TREE_H

#include <string>
#include "DynamicArray.h"
#include <ostream>
#include "CircularList.h"

struct FeedingEntry {
    std::string nickname;
    std::string feedType;
    int quantity;
    std::string date;
};

struct FeedingNode {
    std::string key;
    int balance;
    FeedingNode *left, *right;
    CircularList indices;

    FeedingNode(std::string k, int idx);
};

class FeedingTree {
public:
    FeedingTree();
    ~FeedingTree();

    void add(const std::string& nickname, int index);
    void remove(const std::string& nickname, int index);
    CircularList search(const std::string& nickname) const;

    void print(std::ostream &out) const;

    bool importFromFile(const std::string &filename, DynamicArray<FeedingEntry> &outEntries, int maxLines = 0);
    bool exportToFile(const std::string &filename, const DynamicArray<FeedingEntry>& feedings) const;

    void clear();

private:
    FeedingNode* root;

    FeedingNode* insertNode(FeedingNode* node, const std::string& key, int index, bool &heightInc);
    FeedingNode* deleteNode(FeedingNode* node, const std::string& key, int index, bool &heightDec);
    FeedingNode* rotateLeft(FeedingNode* a);
    FeedingNode* rotateRight(FeedingNode* a);

    FeedingNode* balanceLeftInsert(FeedingNode* node, bool &heightInc);
    FeedingNode* balanceRightInsert(FeedingNode* node, bool &heightInc);
    FeedingNode* balanceLeft(FeedingNode* node, bool &heightDec);
    FeedingNode* balanceRight(FeedingNode* node, bool &heightDec);

    void prettyPrint(FeedingNode* node, std::ostream &out, const std::string& prefix, int level) const;
    void clearNode(FeedingNode* node);
};

#endif // FEEDING_TREE_H