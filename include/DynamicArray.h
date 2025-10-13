#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <utility>

template <typename T>
class DynamicArray {
private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;

    void reallocate(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < m_size; ++i) {
            new_data[i] = std::move(m_data[i]);
        }
        delete[] m_data;
        m_data = new_data;
        m_capacity = new_capacity;
    }

public:
    DynamicArray() : m_data(nullptr), m_size(0), m_capacity(0) {}

    ~DynamicArray() {
        delete[] m_data;
    }

    DynamicArray(const DynamicArray& other) : m_size(other.m_size), m_capacity(other.m_capacity) {
        m_data = new T[m_capacity];
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_data = new T[m_capacity];
            for (size_t i = 0; i < m_size; ++i) {
                m_data[i] = other.m_data[i];
            }
        }
        return *this;
    }

    DynamicArray(DynamicArray&& other) noexcept : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] m_data;
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    void push_back(const T& value) {
        if (m_size >= m_capacity) {
            size_t new_capacity = (m_capacity == 0) ? 8 : m_capacity * 2;
            reallocate(new_capacity);
        }
        m_data[m_size++] = value;
    }
     void push_back(T&& value) {
        if (m_size >= m_capacity) {
            size_t new_capacity = (m_capacity == 0) ? 8 : m_capacity * 2;
            reallocate(new_capacity);
        }
        m_data[m_size++] = std::move(value);
    }

    void erase(size_t index) {
        if (index >= m_size) {
            return;
        }
        for (size_t i = index; i < m_size - 1; ++i) {
            m_data[i] = std::move(m_data[i + 1]);
        }
        --m_size;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity > m_capacity) {
            reallocate(new_capacity);
        }
    }

    void clear() {
        m_size = 0;
    }

    T& operator[](size_t index) {
        return m_data[index];
    }

    const T& operator[](size_t index) const {
        return m_data[index];
    }

    size_t size() const {
        return m_size;
    }

    bool empty() const {
        return m_size == 0;
    }
};

#endif // DYNAMIC_ARRAY_H