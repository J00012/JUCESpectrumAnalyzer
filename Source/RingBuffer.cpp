/*
  ==============================================================================

    RingBuffer.cpp
    Created: 13 Mar 2024 12:53:41am
    Author:  plumm

  ==============================================================================
*/

#include "RingBuffer.h"

//default constructor
template<typename T>
RingBuffer<T>::RingBuffer(size_t capacity) : buffer(capacity), capacity(capacity), head(0), tail(0), full(false) {}

template<typename T>
void RingBuffer<T>::write(const T* processBuffer, size_t bufferSize) {

    if (full)
        throw std::overflow_error("Buffer is full");

    for (size_t sample = 0; sample < bufferSize; ++sample) {
        if (full)
            throw std::overflow_error("Buffer is full");

        buffer[head] = processBuffer[sample];
        head = (head + 1) % capacity;
        if (head == tail)
            full = true;
    }
}

template<typename T>
void RingBuffer<T>::read(T* processBuffer, size_t bufferIndex) {
    if (is_empty())
        throw std::underflow_error("Buffer is empty");

    for (size_t i = 0; i < bufferIndex; ++i) {
        if (is_empty())
            throw std::underflow_error("Buffer is empty");

        processBuffer[i] = buffer[tail];
        tail = (tail + 1) % capacity;
        full = false;
    }
}

template<typename T>
bool RingBuffer<T>::is_empty() const {
    return !full && head == tail;
}


//size increase as more information is written
//size decreases as more information is read
template<typename T>
size_t RingBuffer<T>::size() const {
    if (full)
        return capacity;
    return (head - tail + capacity) % capacity;
}


template<typename T>
void RingBuffer<T>::clear() {
    
    std::fill(buffer.begin(), buffer.end(), T{}); 
    head = tail = 0;
    full = false;
}


template<typename T>
void RingBuffer<T>::setSize(size_t bufferSize) {
    buffer.resize(bufferSize);
    capacity = bufferSize;
}

template class RingBuffer<float>;
