#ifndef FILTERS_TREE_H
#define FILTERS_TREE_H

#include <ostream>
#include <string>
#include "CircularList.h"

template<typename T>
struct FilterNode {
    T key;
    int balance;
    FilterNode *left, *right;
    CircularList indices;

    FilterNode(const T& k, int idx);
};

template<typename T>
class FiltersTree {
public:
    FiltersTree();
    ~FiltersTree();

    void add(const T& filterValue, int index);
    void remove(const T& filterValue, int index);
    CircularList search(const T& filterValue) const;
    CircularList searchInRange(const T& minValue, const T& maxValue) const;
    CircularList getAllIndices() const;
    void print(std::ostream &out) const;
    void clear();
    bool empty() const { return root == nullptr; }

private:
    FilterNode<T>* root;

    FilterNode<T>* insertNode(FilterNode<T>* node, const T& key, int index, bool &heightInc);
    FilterNode<T>* deleteNode(FilterNode<T>* node, const T& key, int index, bool &heightDec);
    FilterNode<T>* rotateLeft(FilterNode<T>* a);
    FilterNode<T>* rotateRight(FilterNode<T>* a);
    FilterNode<T>* balanceLeft(FilterNode<T>* node, bool &heightDec);
    FilterNode<T>* balanceRight(FilterNode<T>* node, bool &heightDec);
    void prettyPrint(FilterNode<T>* node, std::ostream &out, const std::string& prefix, bool isLast, int level) const;
    void inOrderCollect(FilterNode<T>* node, CircularList &result) const;
    void rangeSearch(FilterNode<T>* node, const T& minVal, const T& maxVal, CircularList &result) const;
    void clearNode(FilterNode<T>* node);
};

// Определения типов деревьев остаются прежними
typedef FiltersTree<double> PriceFiltersTree;
typedef FiltersTree<int> QuantityFiltersTree;
typedef FiltersTree<std::string> DateFiltersTree;

// Обновленное пространство имен для работы с датами
namespace DateUtils {
    // Преобразование "DD.MM.YYYY" в "YYYY-MM-DD" для сравнения
    std::string normalizeDateForComparison(const std::string& date);

    // Проверка корректности формата "DD.MM.YYYY"
    bool isValidDateFormat(const std::string& date);

    // Сравнение дат в формате "DD.MM.YYYY"
    int compareDates(const std::string& date1, const std::string& date2);
}

#endif // FILTERS_TREE_H