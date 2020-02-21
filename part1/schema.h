//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_SCHEMA_H
#define CS4500_A4_SCHEMA_H

#pragma once;
#include "object.h"
#include "string.h"
#include "array/array.h"

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object {
public:
    Array* types_array;
    Array* column_names;
    Array* row_names;
public:
    /** Copying constructor */
    Schema(Schema& from) {
        types_array = from.types_array;
        column_names = from.column_names;
        row_names = from.row_names;
    }

    /** Create an empty schema **/
    Schema() {
        types_array = new Array();
        column_names = new Array();
        row_names = new Array();
    }

    ~Schema() {
        delete types_array, column_names, row_names;
    }

    /** Create a schema from a string of types. A string that contains
      * characters other than those identifying the four type results in
      * undefined behavior. The argument is external, a nullptr argument is
      * undefined. **/
    Schema(const char* types) {
        types_array = new Array();
        for(int i=0; i<strlen(types); i++) {
            types_array->add(new String(types+i));
        }
        column_names = new Array();
        row_names = new Array;
    }

    /** Add a column of the given type and name (can be nullptr), name
      * is external. Names are expectd to be unique, duplicates result
      * in undefined behavior. */
    void add_column(char typ, String* name) {
        types_array->add(new String(&typ));
        column_names->add(name);
    }

    /** Add a row with a name (possibly nullptr), name is external.  Names are
     *  expectd to be unique, duplicates result in undefined behavior. */
    void add_row(String* name) {
        row_names->add(name);
    }

    /** Return name of row at idx; nullptr indicates no name. An idx >= width
      * is undefined. */
    String* row_name(size_t idx) {
        Object* object = row_names->get(idx);
        if (object == nullptr) {return nullptr;}
        return reinterpret_cast<String*>(object);
    }

    /** Return name of column at idx; nullptr indicates no name given.
      *  An idx >= width is undefined.*/
    String* col_name(size_t idx) {
        Object* object = column_names->get(idx);
        if (object == nullptr) {return nullptr;}
        return reinterpret_cast<String*>(object);
    }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        Object* object = types_array->get(idx);
        if (object == nullptr) {return ' ';}
        String* result = reinterpret_cast<String*>(object);
        return *(result -> c_str());
    }

    /** Given a column name return its index, or -1. */
    int col_idx(const char* name) {
        return column_names->index_of(new String(name));
    }

    /** Given a row name return its index, or -1. */
    int row_idx(const char* name) {
        return row_names->index_of(new String(name));
    }

    /** The number of columns */
    size_t width() {
        return types_array->get_size();
    }

    /** The number of rows */
    size_t length() {
        return row_names -> get_size();
    }
};


#endif //CS4500_A4_SCHEMA_H
