#include "FeedingTree.h"
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>

FeedingNode::FeedingNode(std::string k, int idx)
    : key(std::move(k)), balance(0), left(nullptr), right(nullptr) {
    indices.add(idx);
}

FeedingTree::FeedingTree() : root(nullptr) {}
FeedingTree::~FeedingTree() { clear(); }

void FeedingTree::clear() {
    clearNode(root);
    root = nullptr;
}

void FeedingTree::clearNode(FeedingNode* node) {
    if (!node) return;
    clearNode(node->left);
    clearNode(node->right);
    delete node;
}

void FeedingTree::add(const std::string& nickname, int index) {
    bool inc = false;
    root = insertNode(root, nickname, index, inc);
}

void FeedingTree::remove(const std::string& nickname, int index) {
    bool dec = false;
    root = deleteNode(root, nickname, index, dec);
}

CircularList FeedingTree::search(const std::string& nickname) const {
    FeedingNode* cur = root;
    while (cur) {
        if (nickname < cur->key) {
            cur = cur->left;
        } else if (nickname > cur->key) {
            cur = cur->right;
        } else {
            return cur->indices;
        }
    }
    return CircularList();
}

void FeedingTree::print(std::ostream &out) const {
    if (!root) {
        out << "Дерево пустое\n";
        return;
    }
    out << "Структура дерева:\n";
    prettyPrint(root, out, "", true, 1);
}

void FeedingTree::prettyPrint(FeedingNode* node, std::ostream &out, const std::string& prefix, bool isLast, int level) const {
    if (!node) return;
    if (node->right) {
        prettyPrint(node->right, out, prefix + "        ", false, level + 1);
    }
    out << prefix << std::string(level, '<') << node->key << "\n";
    if (node->left) {
        prettyPrint(node->left, out, prefix + "        ", true, level + 1);
    }
}

bool FeedingTree::importFromFile(const std::string &filename, DynamicArray<FeedingEntry> &outEntries, int maxLines) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;
    std::string line;
    int linesRead = 0;
    while (std::getline(in, line)) {
        if (maxLines > 0 && linesRead >= maxLines) break;
        std::istringstream iss(line);
        FeedingEntry e;
        if (!(iss >> e.nickname >> e.feedType >> e.quantity >> e.date)) continue;
        outEntries.push_back(e);
        linesRead++;
    }
    return true;
}

bool FeedingTree::exportToFile(const std::string& filename, const DynamicArray<FeedingEntry>& feedings) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (size_t i = 0; i < feedings.size(); ++i) {
        const auto& entry = feedings[i];
        file << entry.nickname << " " << entry.feedType << " "
             << entry.quantity << " " << entry.date << std::endl;
    }

    file.close();
    return true;
}

FeedingNode* FeedingTree::rotateLeft(FeedingNode* a) {
    FeedingNode* b = a->right;
    a->right = b->left;
    b->left = a;

    if (b->balance == 0) { a->balance = 1; b->balance = -1; }
    else { a->balance = 0; b->balance = 0; }
    return b;
}

FeedingNode* FeedingTree::rotateRight(FeedingNode* a) {
    FeedingNode* b = a->left;
    a->left = b->right;
    b->right = a;

    if (b->balance == 0) { a->balance = -1; b->balance = 1; }
    else { a->balance = 0; b->balance = 0; }
    return b;
}

FeedingNode* FeedingTree::balanceLeftInsert(FeedingNode* node, bool &heightInc) {
    if (node->balance == 1) { node->balance = 0; heightInc = false; }
    else if (node->balance == 0) { node->balance = -1; }
    else {
        if (node->left->balance <= 0) {
            node = rotateRight(node);
        } else {
            node->left = rotateLeft(node->left);
            node = rotateRight(node);
        }
        heightInc = false;
    }
    return node;
}

FeedingNode* FeedingTree::balanceRightInsert(FeedingNode* node, bool &heightInc) {
    if (node->balance == -1) { node->balance = 0; heightInc = false; }
    else if (node->balance == 0) { node->balance = 1; }
    else {
        if (node->right->balance >= 0) {
            node = rotateLeft(node);
        } else {
            node->right = rotateRight(node->right);
            node = rotateLeft(node);
        }
        heightInc = false;
    }
    return node;
}

FeedingNode* FeedingTree::balanceLeft(FeedingNode* node, bool &heightDec) {
    if (node->balance == -1) { node->balance = 0; }
    else if (node->balance == 0) { node->balance = 1; heightDec = false; }
    else {
        if (node->right->balance >= 0) {
            node = rotateLeft(node);
        } else {
            node->right = rotateRight(node->right);
            node = rotateLeft(node);
        }
    }
    return node;
}

FeedingNode* FeedingTree::balanceRight(FeedingNode* node, bool &heightDec) {
    if (node->balance == 1) { node->balance = 0; }
    else if (node->balance == 0) { node->balance = -1; heightDec = false; }
    else {
        if (node->left->balance <= 0) {
            node = rotateRight(node);
        } else {
            node->left = rotateLeft(node->left);
            node = rotateRight(node);
        }
    }
    return node;
}

FeedingNode* FeedingTree::insertNode(FeedingNode* node, const std::string& key, int index, bool &heightInc) {
    if (!node) {
        heightInc = true;
        return new FeedingNode(key, index);
    }
    if (key < node->key) {
        node->left = insertNode(node->left, key, index, heightInc);
        if (heightInc) node = balanceLeftInsert(node, heightInc);
    } else if (key > node->key) {
        node->right = insertNode(node->right, key, index, heightInc);
        if (heightInc) node = balanceRightInsert(node, heightInc);
    } else {
        node->indices.add(index);
        heightInc = false;
    }
    return node;
}

FeedingNode* FeedingTree::deleteNode(FeedingNode* node, const std::string& key, int index, bool &heightDec) {
    if (!node) {
        heightDec = false;
        return nullptr;
    }
    if (key < node->key) {
        node->left = deleteNode(node->left, key, index, heightDec);
        if (heightDec) node = balanceLeft(node, heightDec);
    } else if (key > node->key) {
        node->right = deleteNode(node->right, key, index, heightDec);
        if (heightDec) node = balanceRight(node, heightDec);
    } else {
        if (index != -1) {
            node->indices.removeAll(index);
            if (node->indices.size() > 0) {
                heightDec = false;
                return node;
            }
        }
        if (!node->left || !node->right) {
            FeedingNode* child = node->left ? node->left : node->right;
            delete node;
            heightDec = true;
            return child;
        } else {
            FeedingNode* pred = node->left;
            while (pred->right) pred = pred->right;
            node->key = pred->key;
            node->indices = pred->indices;
            bool decL = false;
            node->left = deleteNode(node->left, pred->key, -1, decL);
            if (decL) node = balanceLeft(node, heightDec);
            else heightDec = false;
        }
    }
    return node;
}