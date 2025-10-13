#ifndef ANIMAL_HASH_TABLE_H
#define ANIMAL_HASH_TABLE_H

#include "DynamicArray.h"
#include <string>
#include <ostream>

struct Animal {
    std::string nickname;
    std::string species;
    std::string cage;

    Animal() = default;
    Animal(std::string nick, std::string sp, std::string c)
        : nickname(std::move(nick)), species(std::move(sp)), cage(std::move(c)) {}
};

struct HashEntry {
    std::string key;
    int index;
    int status;

    HashEntry() : index(-1), status(0) {}
    HashEntry(std::string k, int idx) : key(std::move(k)), index(idx), status(1) {}
};

class AnimalHashTable {
public:
    AnimalHashTable(int initialSize = 16);
    ~AnimalHashTable();

    bool insert(const std::string& nickname, int index);
    bool remove(const std::string& nickname);
    int search(const std::string& nickname, int& steps) const;
    void clear();
    void resize(int newInitialSize);

    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
    double getLoadFactor() const { return static_cast<double>(size) / capacity; }

    HashEntry getSlotInfo(int slotIndex) const {
        if (slotIndex >= 0 && slotIndex < capacity) {
            return table[slotIndex];
        }
        return HashEntry();
    }

    int getPrimaryHash(const std::string& key) const { return primaryHash(key); }
    int getSecondaryHash() const { return getStepSize(); }

    void print(std::ostream& out) const;

    bool importFromFile(const std::string& filename, DynamicArray<Animal>& animals, int maxLines = 0);
    bool exportToFile(const std::string& filename, const DynamicArray<Animal>& animals) const;

private:
    HashEntry* table;
    int capacity;
    int size;
    int initialCapacity;
    static constexpr double LOAD_FACTOR_MAX = 0.75;
    static constexpr double LOAD_FACTOR_MIN = 0.25;

    int primaryHash(const std::string& key) const;
    int getStepSize() const;

    void rehash(int newCapacity);
    int findSlot(const std::string& key, bool forInsertion = false) const;
};

#endif // ANIMAL_HASH_TABLE_H