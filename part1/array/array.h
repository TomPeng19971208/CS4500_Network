//
// Created by ZIyi Peng on 2/12/20.
//

#pragma once
#ifndef CS4500_A4_ARRAY_H
#define CS4500_A4_ARRAY_H
#include <math.h>
#include <stdio.h>
#include "../object.h"
// LANGUAGE: CwC
class Array: public Object {
public:
    size_t length;
    size_t capacity;
    size_t bucket_count;
    Object*** data;
public:
    Array() : Object() {
        length = 0;
        bucket_count = 1;
        capacity = 8;
        data = new Object**[bucket_count];
        data[0] = new Object*[capacity];
        for (int i=0; i<capacity; i++) {
            data[0][i] = nullptr;
        }
    }

    Array(size_t size) : Object() {
        bucket_count = int(log2(double(size/8)+1)) + 1;
        capacity = (1 << (bucket_count+3)) - 8;
        length = 0;
        data = new Object**[bucket_count];
        for (int i=0; i<bucket_count; i++) {
            data[i] = new Object*[calc_bucket_size(i)];
            for (int j=0; j<calc_bucket_size(i); j++) {
                data[i][j] = nullptr;
            }
        }
    }

    ~Array() {
        for(int i=0; i<bucket_count; i++) {
            delete [] data[i];
        }
        delete [] data;
    }

public:
    Object* get(size_t idx);
    void add(Object* object);
    void resize();
    // returns the old element on success
    Object* replace(size_t idx, Object* object);
    bool equals(Object  * other);
    int index_of(Object* object);
    size_t hash_me();
    size_t get_size();
    size_t calc_bucket_size(size_t bucket_idx);
    size_t calc_bucket_index(size_t item_idx);
    size_t calc_relative_index(int idx, int target_bucket_idx);
};

Object* Array::get(size_t idx) {
    if (idx >= length){return nullptr;}
    int bucket_idx = calc_bucket_index(idx);
    int relative_idx = calc_relative_index(idx, bucket_idx);
    return data[bucket_idx][relative_idx];
}

void Array::add(Object* object) {
    if (length < capacity) {
        int bucket_idx = calc_bucket_index(length);
        int relative_idx = calc_relative_index(length, bucket_idx);
        data[bucket_idx][relative_idx] = object;
        length += 1;
    } else {
        resize();
        add(object);
    }
}

void Array::resize() {
    int new_capacity = capacity + calc_bucket_size(bucket_count);
    Object*** new_data = new Object**[bucket_count+1];
    memmove(new_data, data, bucket_count * sizeof(Object**));
    delete [] data;
    new_data[bucket_count] = new Object*[calc_bucket_size(bucket_count)];
    for(int i=0; i< calc_bucket_size(bucket_count); i++) {
        new_data[bucket_count][i] = nullptr;
    }
    data = new_data;
    capacity = new_capacity;
    bucket_count += 1;
}

Object* Array::replace(size_t idx, Object* object) {
    if(idx > length) {return nullptr;}
    int bucket_idx = calc_bucket_index(idx);
    int relative_idx = calc_relative_index(idx, bucket_idx);
    Object* result = data[bucket_idx][relative_idx];
    data[bucket_idx][relative_idx] = object;
    return result;
}

bool Array::equals(Object  * other) {
    if(Object::equals(other)) {return true;}
    Array* target = dynamic_cast<Array*>(other);
    if(target== nullptr || target->capacity != capacity || target -> length != length) {return false;}
    for(int i=0; i<bucket_count; i++) {
        for(int j=0; j<calc_bucket_size(i); j++) {
            if (data[i][j] == nullptr && (target->data)[i][j] != nullptr) {return false;}
            if (data[i][j] == nullptr) {break;}
            if (!(data[i][j] -> equals((target->data)[i][j]))) {return false;}
        }
    }
    return true;
}

size_t Array::hash_me() {
    size_t result = 0;
    for(int i=0; i<bucket_count; i++) {
        for (int j = 0; j < calc_bucket_size(i); j++) {
            if (data[i][j] != nullptr) {
                result += (data[i][j] -> hash_me())*i;
            }
        }
    }
    return result;
}

int Array::index_of(Object* object) {
    if (object == nullptr) {return -1;}
    for(int i=0; i<length; i++) {
        int bucket_idx = calc_bucket_index(i);
        int relative_idx = calc_relative_index(i, bucket_idx);
        if (data[bucket_idx][relative_idx] -> equals(object)) {
            return i;
        }
    }
    return -1;
}

size_t Array::get_size() {
    return length;
}

size_t Array::calc_bucket_size(size_t bucket_idx) {
    return 1 << (bucket_idx + 3);
}

size_t Array::calc_bucket_index(size_t item_idx) {
    return int(log2(double(item_idx/8)+1));
}

size_t Array::calc_relative_index(int idx, int target_bucket_idx) {
    return idx - ((1 << (target_bucket_idx + 3)) - 8);
}

#endif //CS4500_A4_ARRAY_H