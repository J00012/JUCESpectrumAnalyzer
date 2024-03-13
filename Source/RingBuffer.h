/*
  ==============================================================================

    RingBuffer.h
    Created: 13 Mar 2024 12:53:41am
    Author:  plumm

  ==============================================================================
*/

#pragma once
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <vector>
#include <stdexcept>

template<typename T>
class RingBuffer {
private:
    std::vector<T> buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    bool full;

public:
    RingBuffer(size_t capacity);

    void write(const T& data);
    T read();
    bool is_empty() const;
    size_t size() const;
    void clear();
};

#endif // RINGBUFFER_H

