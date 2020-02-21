//
// Created by ZIyi Peng on 2/12/20.
//

#ifndef CS4500_A4_WRAPPER_H
#define CS4500_A4_WRAPPER_H

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "../object.h"

class IntWrapper: public Object {
public:
    int data;
public:
    IntWrapper(int value) {
        data = value;
    }

    ~IntWrapper(){}

    char* to_string() {
        return "IntWrapper";
    }

    bool equals(Object* object) {
        IntWrapper* target = reinterpret_cast<IntWrapper*>(object);
        return (target != nullptr && target->data == data);
    }

    void print() {
        printf("%d", data);
    }

    size_t hash_me() {
        return data;
    }

    char* c_str() {
        char result[int(log10(abs(data)+1)) + 2];
        sprintf(result, "%d", data);
        return result;
    }
};

class BoolWrapper: public Object {
public:
    bool data;
public:
    BoolWrapper(bool value) {
        data = value;
    }
    ~BoolWrapper(){}

    char* to_string() {
        return "IntWrapper";
    }

    bool equals(Object* object) {
        BoolWrapper* target = reinterpret_cast<BoolWrapper*>(object);
        return (target != nullptr && target->data == data);
    }

    void print() {
        data ? printf("true") : printf("false");
    }

    size_t hash_me() {
        return data ? 1 : 0;
    }

    char* c_str() {
        return const_cast<char *>(data ? "true" : "false");
    }
};

class FloatWrapper: public Object {
public:
    float data;
public:
    FloatWrapper(float value) {
        data = value;
    }
    ~FloatWrapper(){}

    char* to_string() {
        return "FloatWrapper";
    }

    bool equals(Object* object) {
        FloatWrapper* target = reinterpret_cast<FloatWrapper*>(object);
        return (target != nullptr && target->data == data);
    }

    void print() {
        printf("%.6f", data);
    }

    size_t hash_me() {
        return (size_t) data;
    }

    char* c_str() {
        char result[int(log10(abs(data)+1)) + 4];
        sprintf(result, "%.2f", data);
        return result;
    }
};
#endif //CS4500_A4_WRAPPER_H
