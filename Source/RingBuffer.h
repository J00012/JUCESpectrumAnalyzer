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
    RingBuffer();

    void write(const T& data);
    T read();
    bool is_empty() const;
    size_t size() const;
    void copyVector(const std::vector<T>& processBuffer);
    void copyArray(const T* processBuffer, size_t bufferSize);
    void clear();
    void setSize(size_t bufferSize);
};

#endif // RINGBUFFER_H

