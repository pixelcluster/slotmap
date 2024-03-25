/*
 * Copyright © 2022 Friedrich Vock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

using SlotmapHandle = size_t;

template <typename T> class Slotmap {
public:
    class iterator {
    public:
        // using iterator_category = std::contiguous_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using reference = T&;
        using pointer = T*;

        iterator() = default;

        explicit iterator(pointer elementPointer) : elementPointer(elementPointer) {}

        iterator& operator++() {
            ++elementPointer;
            return *this;
        }
        iterator operator++(int32_t) {
            iterator returnValue = *this;
            ++elementPointer;
            return returnValue;
        }
        iterator& operator--() {
            --elementPointer;
            return *this;
        }
        iterator operator--(int32_t) {
            iterator returnValue = *this;
            --elementPointer;
            return returnValue;
        }
        iterator& operator+=(size_t amount) {
            elementPointer += amount;
            return *this;
        }
        iterator& operator-=(size_t amount) {
            elementPointer -= amount;
            return *this;
        }
        iterator operator-(size_t amount) const {
            iterator returnValue = iterator(elementPointer);
            returnValue.elementPointer -= amount;
            return returnValue;
        }
        iterator operator+(size_t amount) const {
            iterator returnValue = iterator(elementPointer);
            returnValue.elementPointer += amount;
            return returnValue;
        }
        difference_type operator-(const iterator& other) const { return elementPointer - other.elementPointer; }
        difference_type operator+(const iterator& other) const { return elementPointer + other.elementPointer; }

        bool operator==(iterator other) const { return elementPointer == other.elementPointer; }

        bool operator!=(iterator other) const { return elementPointer != other.elementPointer; }

        reference operator*() const { return *elementPointer; }
        pointer operator->() { return elementPointer; }
        pointer operator->() const { return elementPointer; }

        operator pointer() const { return elementPointer; }

    private:
        pointer elementPointer;
    };

    class const_iterator {
    public:
        // using iterator_category = std::contiguous_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using reference = const T&;
        using pointer = const T*;

        const_iterator() = default;

        explicit const_iterator(pointer elementPointer) : elementPointer(elementPointer) {}

        const_iterator& operator++() {
            ++elementPointer;
            return *this;
        }
        const_iterator operator++(int32_t) {
            const_iterator returnValue = *this;
            ++elementPointer;
            return returnValue;
        }
        const_iterator& operator--() {
            --elementPointer;
            return *this;
        }
        const_iterator operator--(int32_t) {
            const_iterator returnValue = *this;
            --elementPointer;
            return returnValue;
        }
        const_iterator& operator+=(size_t amount) {
            elementPointer += amount;
            return *this;
        }
        const_iterator& operator-=(size_t amount) {
            elementPointer -= amount;
            return *this;
        }
        const_iterator operator-(size_t amount) const {
            const_iterator returnValue = const_iterator(elementPointer);
            returnValue.elementPointer -= amount;
            return returnValue;
        }
        const_iterator operator+(size_t amount) const {
            const_iterator returnValue = const_iterator(elementPointer);
            returnValue.elementPointer += amount;
            return returnValue;
        }
        difference_type operator-(const const_iterator& other) const { return elementPointer - other.elementPointer; }
        difference_type operator+(const const_iterator& other) const { return elementPointer + other.elementPointer; }

        bool operator==(const const_iterator& other) const { return elementPointer == other.elementPointer; }

        bool operator!=(const const_iterator& other) const { return elementPointer != other.elementPointer; }

        reference operator*() const { return *elementPointer; }
        pointer operator->() { return elementPointer; }
        pointer operator->() const { return elementPointer; }

    private:
        pointer elementPointer;
    };

    inline SlotmapHandle addElement(const T& newElement);
    inline SlotmapHandle addElement(T&& newElement);

    inline T& elementAt(SlotmapHandle handle);
    inline const T& elementAt(SlotmapHandle handle) const;

    inline void removeElement(SlotmapHandle handle);
    inline T& operator[](SlotmapHandle handle);

    inline const T& operator[](SlotmapHandle handle) const;

    inline void clear();

    inline size_t size() const;
    inline iterator begin();
    inline iterator end();
    inline const_iterator cbegin() const;
    inline const_iterator cend() const;

    inline iterator find(SlotmapHandle handle);
    inline const_iterator find(SlotmapHandle handle) const;

    /*
     * Returns the handle of a given iterator.
     */
    inline SlotmapHandle handle(const iterator& handleIterator);

private:
    std::vector<T> elements;
    std::vector<size_t> keys;
    std::vector<size_t> eraseMap;
    size_t freeKeyHead = 0;
    size_t freeKeyTail = 0;
};

template <typename T> inline SlotmapHandle Slotmap<T>::addElement(const T& newElement) {
    if (keys.size() == 0)
        keys.push_back(0);
    elements.push_back(newElement);
    eraseMap.push_back(freeKeyHead);
    if (freeKeyHead == freeKeyTail) {
        size_t newFreeSlotIndex = keys.size();

        keys.push_back(newFreeSlotIndex);

        keys[freeKeyTail] = newFreeSlotIndex;
        freeKeyTail = newFreeSlotIndex;
    }
    size_t nextFreeIndex = keys[freeKeyHead];

    keys[freeKeyHead] = elements.size() - 1;

    size_t returnIndex = freeKeyHead;
    freeKeyHead = nextFreeIndex;

    return returnIndex;
}

template <typename T> inline SlotmapHandle Slotmap<T>::addElement(T&& newElement) {
    if (keys.size() == 0)
        keys.push_back(0);
    elements.push_back(std::forward<T>(newElement));
    eraseMap.push_back(freeKeyHead);
    if (freeKeyHead == freeKeyTail) {
        size_t newFreeSlotIndex = keys.size();

        keys.push_back(newFreeSlotIndex);

        keys[freeKeyTail] = newFreeSlotIndex;
        freeKeyTail = newFreeSlotIndex;
    }
    size_t nextFreeIndex = keys[freeKeyHead];

    keys[freeKeyHead] = elements.size() - 1;

    size_t returnIndex = freeKeyHead;
    freeKeyHead = nextFreeIndex;

    return returnIndex;
}

template <typename T> inline T& Slotmap<T>::elementAt(SlotmapHandle handle) {
    assert(keys.size() > handle);
    assert(eraseMap[keys[handle]] == handle);
    return elements[keys[handle]];
}

template <typename T> inline const T& Slotmap<T>::elementAt(SlotmapHandle handle) const {
    assert(keys.size() > handle);
    assert(eraseMap[keys[handle]] == handle);
    return elements[keys[handle]];
}

template <typename T> inline void Slotmap<T>::removeElement(SlotmapHandle handle) {
    assert(keys.size() > handle);

    size_t eraseElementIndex = keys[handle];

    // Move last element in, update erase table
    elements[eraseElementIndex] = std::move(elements[elements.size() - 1]);
    eraseMap[eraseElementIndex] = eraseMap[elements.size() - 1];

    // Update key index of what was the last element
    keys[eraseMap[eraseElementIndex]] = eraseElementIndex;

    // Update erase table/element std::vector sizes
    elements.erase(elements.begin() + (elements.size() - 1));
    eraseMap.erase(eraseMap.begin() + (eraseMap.size() - 1));

    // Update free list nodes
    keys[freeKeyTail] = handle;
    keys[handle] = handle;
    freeKeyTail = handle;
}

template <typename T> inline T& Slotmap<T>::operator[](SlotmapHandle handle) { return elementAt(handle); }

template <typename T> inline const T& Slotmap<T>::operator[](SlotmapHandle handle) const { return elementAt(handle); }

template <typename T> inline void Slotmap<T>::clear() {
    elements.clear();
    keys.clear();
    eraseMap.clear();
    freeKeyHead = 0;
    freeKeyTail = 0;
}

template <typename T> inline size_t Slotmap<T>::size() const { return elements.size(); }

template <typename T> inline typename Slotmap<T>::iterator Slotmap<T>::begin() { return iterator(elements.data()); }

template <typename T> inline typename Slotmap<T>::iterator Slotmap<T>::end() {
    return iterator(elements.data() + elements.size());
}

template <typename T> inline typename Slotmap<T>::const_iterator Slotmap<T>::cbegin() const {
    return const_iterator(elements.data());
}

template <typename T> inline typename Slotmap<T>::const_iterator Slotmap<T>::cend() const {
    return const_iterator(elements.data() + elements.size());
}

template <typename T> inline typename Slotmap<T>::iterator Slotmap<T>::find(SlotmapHandle handle) {
    if (handle >= keys.size())
        return iterator(elements.data() + elements.size());
    return iterator(elements.data() + keys[handle]);
}

template <typename T> inline typename Slotmap<T>::const_iterator Slotmap<T>::find(SlotmapHandle handle) const {
    if (handle >= keys.size())
        return const_iterator(elements.data() + elements.size());
    return const_iterator(elements.data() + keys[handle]);
}

template <typename T> inline SlotmapHandle Slotmap<T>::handle(const Slotmap<T>::iterator& handleIterator) {
    return { eraseMap[static_cast<T*>(handleIterator) - elements.data()] };
}