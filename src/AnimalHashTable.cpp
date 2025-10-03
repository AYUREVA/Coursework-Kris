#include "AnimalHashTable.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

AnimalHashTable::AnimalHashTable(int initialSize)
    : capacity(initialSize), size(0), initialCapacity(initialSize) {
    table = new HashEntry[capacity];
}

AnimalHashTable::~AnimalHashTable() {
    delete[] table;
}

// Хеш-функция: СВЕРТКА (складываем ASCII-коды символов по чанкам)
int AnimalHashTable::primaryHash(const std::string& key) const {
    unsigned long hash = 0;
    // Проходим по строке с шагом 4, "сворачивая" ее
    for (size_t i = 0; i < key.length(); i += 4) {
        unsigned int chunk = 0;
        // Собираем 4-байтовый чанк
        for (size_t j = i; j < i + 4 && j < key.length(); ++j) {
            chunk = (chunk << 8) + static_cast<unsigned char>(key[j]);
        }
        hash += chunk;
    }
    return hash % capacity;
}

// Шаг линейного пробирования: k=3
int AnimalHashTable::getStepSize() const {
    return 3;
}

int AnimalHashTable::findSlot(const std::string& key, bool forInsertion) const {
    int index = primaryHash(key);
    int step = getStepSize();
    int deletedSlot = -1;

    for (int i = 0; i < capacity; ++i) {
        if (table[index].status == 0) { // Слот пуст
            // Если искали место для вставки и нашли удаленный слот, используем его
            return forInsertion && deletedSlot != -1 ? deletedSlot : index;
        }
        else if (table[index].status == 1) { // Слот занят
            if (table[index].key == key) {
                return index; // Ключ найден
            }
        }
        else if (table[index].status == 2) { // Слот удален
            if (forInsertion && deletedSlot == -1) {
                deletedSlot = index; // Запоминаем первый удаленный слот
            }
        }

        index = (index + step) % capacity;
    }

    return forInsertion ? deletedSlot : -1;
}

bool AnimalHashTable::insert(const std::string& nickname, int index) {
    if (static_cast<double>(size + 1) / capacity > LOAD_FACTOR_MAX) {
        rehash(capacity * 2);
    }

    int slot = findSlot(nickname, true);
    if (slot == -1) { // Таблица заполнена
        return false;
    }

    // Если элемент уже существует, обновляем его индекс
    if (table[slot].status == 1 && table[slot].key == nickname) {
        table[slot].index = index;
        return true;
    }

    table[slot].key = nickname;
    table[slot].index = index;
    table[slot].status = 1;
    size++;

    return true;
}

bool AnimalHashTable::remove(const std::string& nickname) {
    int slot = findSlot(nickname, false);
    if (slot == -1 || table[slot].status != 1) {
        return false; // Элемент не найден
    }

    table[slot].status = 2; // Помечаем как удаленный
    size--;

    // Уменьшение размера таблицы при необходимости
    if (capacity > initialCapacity &&
        static_cast<double>(size) / capacity < LOAD_FACTOR_MIN) {
        int newCapacity = (capacity / 2 > initialCapacity) ? (capacity / 2) : initialCapacity;
        rehash(newCapacity);
    }

    return true;
}

int AnimalHashTable::search(const std::string& nickname, int& steps) const {
    int index = primaryHash(nickname);
    int step = getStepSize();
    steps = 0;

    for (int i = 0; i < capacity; ++i) {
        steps++;

        if (table[index].status == 0) { // Пустой слот, значит ключа нет
            return -1;
        }
        if (table[index].status == 1 && table[index].key == nickname) {
            return table[index].index; // Ключ найден
        }
        // Если статус 2 (удален), продолжаем поиск

        index = (index + step) % capacity;
    }

    return -1; // Ключ не найден после полного прохода
}

void AnimalHashTable::clear() {
    delete[] table;
    capacity = initialCapacity;
    size = 0;
    table = new HashEntry[capacity];
}

void AnimalHashTable::resize(int newInitialSize) {
    delete[] table;
    capacity = newInitialSize;
    initialCapacity = newInitialSize;
    size = 0;
    table = new HashEntry[capacity];
}

void AnimalHashTable::rehash(int newCapacity) {
    HashEntry* oldTable = table;
    int oldCapacity = capacity;

    table = new HashEntry[newCapacity];
    capacity = newCapacity;
    size = 0; // Размер будет восстановлен при вставке

    // Переносим все занятые элементы в новую таблицу
    for (int i = 0; i < oldCapacity; ++i) {
        if (oldTable[i].status == 1) {
            insert(oldTable[i].key, oldTable[i].index);
        }
    }

    delete[] oldTable;
}

void AnimalHashTable::print(std::ostream& out) const {
    out << "=== Hash Table Debug Info ===" << std::endl;
    out << "Capacity: " << capacity << ", Size: " << size
        << ", Load Factor: " << std::fixed << std::setprecision(2)
        << getLoadFactor() << std::endl;
    out << std::endl;

    for (int i = 0; i < capacity; ++i) {
        out << "==================================================================\n";
        out << "Slot " << std::setw(3) << i << ": ";

        if (table[i].status == 0) {
            out << "Nickname=None, Index=None, Status=0\n";
        } else {
            out << "Nickname=" << table[i].key
                << ", Index=" << table[i].index
                << ", Hash=" << primaryHash(table[i].key)
                << ", Status=" << table[i].status << "\n";
        }
    }
    out << "==================================================================\n";
}

bool AnimalHashTable::importFromFile(const std::string& filename,
                                     DynamicArray<Animal>& animals,
                                     int maxLines) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    animals.clear();
    clear();

    std::string line;
    int index = 0;
    int linesRead = 0;

    while (std::getline(file, line)) {
        if (maxLines > 0 && linesRead >= maxLines) {
            break;
        }

        std::istringstream iss(line);
        Animal animal;

        // Читаем данные в формате: Кличка Вид Вольер
        if (!(iss >> animal.nickname >> animal.species >> animal.cage)) {
            continue;
        }

        animals.push_back(animal);
        insert(animal.nickname, index);
        index++;
        linesRead++;
    }

    file.close();
    return true;
}

bool AnimalHashTable::exportToFile(const std::string& filename,
                                   const DynamicArray<Animal>& animals) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (size_t i = 0; i < animals.size(); ++i) {
        const auto& animal = animals[i];
        file << animal.nickname << " " << animal.species << " "
             << animal.cage << std::endl;
    }

    file.close();
    return true;
}