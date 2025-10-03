#include "FiltersTree.h"
#include <sstream>
#include <iomanip>

template<typename T>
FilterNode<T>::FilterNode(const T& k, int idx)
    : key(k), balance(0), left(nullptr), right(nullptr), indices()
{
    indices.add(idx);
}

template<typename T>
FiltersTree<T>::FiltersTree() : root(nullptr) {}

template<typename T>
FiltersTree<T>::~FiltersTree() {
    clear();
}

template<typename T>
void FiltersTree<T>::clear() {
    clearNode(root);
    root = nullptr;
}

template<typename T>
void FiltersTree<T>::clearNode(FilterNode<T>* node) {
    if (!node) return;
    clearNode(node->left);
    clearNode(node->right);
    delete node;
}

template<typename T>
void FiltersTree<T>::add(const T& filterValue, int index) {
    bool inc = false;
    root = insertNode(root, filterValue, index, inc);
}

template<typename T>
void FiltersTree<T>::remove(const T& filterValue, int index) {
    bool dec = false;
    root = deleteNode(root, filterValue, index, dec);
}

template<typename T>
CircularList FiltersTree<T>::search(const T& filterValue) const {
    FilterNode<T>* cur = root;
    while (cur) {
        if (filterValue < cur->key) {
            cur = cur->left;
        } else if (filterValue > cur->key) {
            cur = cur->right;
        } else {
            return cur->indices;
        }
    }
    return CircularList();
}

template<typename T>
CircularList FiltersTree<T>::searchInRange(const T& minValue, const T& maxValue) const {
    CircularList result;
    rangeSearch(root, minValue, maxValue, result);
    return result;
}

template<typename T>
CircularList FiltersTree<T>::getAllIndices() const {
    CircularList result;
    inOrderCollect(root, result);
    return result;
}

template<typename T>
void FiltersTree<T>::print(std::ostream &out) const {
    if (!root) {
        out << "[Empty filter tree]" << std::endl;
        return;
    }
    out << "Структура дерева:\n";
    prettyPrint(root, out, "", true, 1);
}

template<typename T>
void FiltersTree<T>::prettyPrint(FilterNode<T>* node, std::ostream &out, const std::string& prefix, bool isLast, int level) const {
    if (!node) return;
    if (node->right) {
        prettyPrint(node->right, out, prefix + "        ", false, level + 1);
    }
    out << prefix << std::string(level, '<') << node->key << "\n";
    if (node->left) {
        prettyPrint(node->left, out, prefix + "        ", true, level + 1);
    }
}

template<typename T>
void FiltersTree<T>::inOrderCollect(FilterNode<T>* node, CircularList &result) const {
    if (!node) return;

    inOrderCollect(node->left, result);

    for (int i = 0; i < node->indices.size(); ++i) {
        result.add(node->indices.get(i));
    }

    inOrderCollect(node->right, result);
}

template<typename T>
void FiltersTree<T>::rangeSearch(FilterNode<T>* node, const T& minVal, const T& maxVal, CircularList &result) const {
    if (!node) return;

    if (node->key > maxVal) {
        rangeSearch(node->left, minVal, maxVal, result);
    }
    else if (node->key < minVal) {
        rangeSearch(node->right, minVal, maxVal, result);
    }
    else {
        for (int i = 0; i < node->indices.size(); ++i) {
            result.add(node->indices.get(i));
        }

        rangeSearch(node->left, minVal, maxVal, result);
        rangeSearch(node->right, minVal, maxVal, result);
    }
}

template<typename T>
FilterNode<T>* FiltersTree<T>::rotateLeft(FilterNode<T>* a) {
    FilterNode<T>* b = a->right;
    a->right = b->left;
    b->left = a;

    if (b->balance == 0) {
        a->balance = 1;
        b->balance = -1;
    } else {
        a->balance = 0;
        b->balance = 0;
    }
    return b;
}

template<typename T>
FilterNode<T>* FiltersTree<T>::rotateRight(FilterNode<T>* a) {
    FilterNode<T>* b = a->left;
    a->left = b->right;
    b->right = a;

    if (b->balance == 0) {
        a->balance = -1;
        b->balance = 1;
    } else {
        a->balance = 0;
        b->balance = 0;
    }
    return b;
}

template<typename T>
FilterNode<T>* FiltersTree<T>::balanceLeft(FilterNode<T>* node, bool &heightDec) {
    if (node->balance == -1) {
        node->balance = 0;
    } else if (node->balance == 0) {
        node->balance = 1;
        heightDec = false;
    } else {
        FilterNode<T>* r = node->right;
        if (r->balance >= 0) {
            node = rotateLeft(node);
        } else {
            int oldBalance = r->left->balance;
            node->right = rotateRight(r);
            node = rotateLeft(node);

            if (oldBalance == 0) {
                node->left->balance = 0;
                node->right->balance = 0;
            } else if (oldBalance == -1) {
                node->left->balance = 0;
                node->right->balance = 1;
            } else {
                node->left->balance = -1;
                node->right->balance = 0;
            }
            node->balance = 0;
        }
    }
    return node;
}

template<typename T>
FilterNode<T>* FiltersTree<T>::balanceRight(FilterNode<T>* node, bool &heightDec) {
    if (node->balance == 1) {
        node->balance = 0;
    } else if (node->balance == 0) {
        node->balance = -1;
        heightDec = false;
    } else {
        FilterNode<T>* l = node->left;
        if (l->balance <= 0) {
            node = rotateRight(node);
        } else {
            int oldBalance = l->right->balance;
            node->left = rotateLeft(l);
            node = rotateRight(node);

            if (oldBalance == 0) {
                node->left->balance = 0;
                node->right->balance = 0;
            } else if (oldBalance == -1) {
                node->left->balance = -1;
                node->right->balance = 0;
            } else {
                node->left->balance = 0;
                node->right->balance = 1;
            }
            node->balance = 0;
        }
    }
    return node;
}

template<typename T>
FilterNode<T>* FiltersTree<T>::insertNode(FilterNode<T>* node, const T& key, int index, bool &heightInc) {
    if (!node) {
        heightInc = true;
        return new FilterNode<T>(key, index);
    }

    if (key < node->key) {
        node->left = insertNode(node->left, key, index, heightInc);
        if (heightInc) {
            if (node->balance == 1) {
                node->balance = 0;
                heightInc = false;
            } else if (node->balance == 0) {
                node->balance = -1;
            } else {
                if (node->left->balance <= 0) {
                    node = rotateRight(node);
                } else {
                    int oldBalance = node->left->right->balance;
                    node->left = rotateLeft(node->left);
                    node = rotateRight(node);

                    if (oldBalance == 0) {
                        node->left->balance = 0;
                        node->right->balance = 0;
                    } else if (oldBalance == -1) {
                        node->left->balance = -1;
                        node->right->balance = 0;
                    } else {
                        node->left->balance = 0;
                        node->right->balance = 1;
                    }
                    node->balance = 0;
                }
                heightInc = false;
            }
        }
    } else if (key > node->key) {
        node->right = insertNode(node->right, key, index, heightInc);
        if (heightInc) {
            if (node->balance == -1) {
                node->balance = 0;
                heightInc = false;
            } else if (node->balance == 0) {
                node->balance = 1;
            } else {
                if (node->right->balance >= 0) {
                    node = rotateLeft(node);
                } else {
                    int oldBalance = node->right->left->balance;
                    node->right = rotateRight(node->right);
                    node = rotateLeft(node);

                    if (oldBalance == 0) {
                        node->left->balance = 0;
                        node->right->balance = 0;
                    } else if (oldBalance == -1) {
                        node->left->balance = 0;
                        node->right->balance = 1;
                    } else {
                        node->left->balance = -1;
                        node->right->balance = 0;
                    }
                    node->balance = 0;
                }
                heightInc = false;
            }
        }
    } else {
        node->indices.add(index);
        heightInc = false;
    }
    return node;
}

template<typename T>
FilterNode<T>* FiltersTree<T>::deleteNode(FilterNode<T>* node, const T& key, int index, bool &heightDec) {
    if (!node) {
        heightDec = false;
        return nullptr;
    }

    if (key < node->key) {
        node->left = deleteNode(node->left, key, index, heightDec);
        if (heightDec)
            node = balanceLeft(node, heightDec);
    } else if (key > node->key) {
        node->right = deleteNode(node->right, key, index, heightDec);
        if (heightDec)
            node = balanceRight(node, heightDec);
    } else {
        if (index != -1) {
            node->indices.removeAll(index);
            if (node->indices.size() > 0) {
                heightDec = false;
                return node;
            }
        }

        if (!node->left || !node->right) {
            FilterNode<T>* child = node->left ? node->left : node->right;
            delete node;
            heightDec = true;
            return child;
        } else {
            FilterNode<T>* pred = node->left;
            while (pred->right) pred = pred->right;

            node->key = pred->key;
            node->indices = pred->indices;

            bool decL = false;
            node->left = deleteNode(node->left, pred->key, -1, decL);
            if (decL)
                node = balanceLeft(node, heightDec);
            else
                heightDec = false;
        }
    }
    return node;
}

template class FiltersTree<double>;
template class FiltersTree<int>;
template class FiltersTree<std::string>;
template class FilterNode<double>;
template class FilterNode<int>;
template class FilterNode<std::string>;

// НОВАЯ РЕАЛИЗАЦИЯ DateUtils
namespace DateUtils {
    // Конвертирует DD.MM.YYYY в YYYY-MM-DD. Возвращает пустую строку при ошибке.
    std::string normalizeDateForComparison(const std::string& date) {
        if (date.length() != 10 || date[2] != '.' || date[5] != '.') {
            return ""; // Неверный формат/длина
        }

        try {
            int day = std::stoi(date.substr(0, 2));
            int month = std::stoi(date.substr(3, 2));
            int year = std::stoi(date.substr(6, 4));

            if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 2025) {
                return ""; // Неверный диапазон
            }

            std::ostringstream oss;
            oss << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day;
            return oss.str();
        } catch (...) {
            return ""; // Ошибка конвертации в число
        }
    }

    bool isValidDateFormat(const std::string& date) {
        return !normalizeDateForComparison(date).empty();
    }

    int compareDates(const std::string& date1, const std::string& date2) {
        std::string norm1 = normalizeDateForComparison(date1);
        std::string norm2 = normalizeDateForComparison(date2);

        if (norm1.empty() || norm2.empty()) {
            return 0; // Считаем некорректные даты равными
        }

        if (norm1 < norm2) return -1;
        if (norm1 > norm2) return 1;
        return 0;
    }
}