//
// Created by Jiawen Liu on 2/12/20.
//

#ifndef CS4500_A4_MMDataFrame_H
#define CS4500_A4_MMDataFrame_H

#pragma once;

#include <thread>
#include "schema.h"
#include "column.h"
#include "intcolumn.h"
#include "row.h"
#include "boolcolumn.h"
#include "floatcolumn.h"
#include "stringcolumn.h"
#include "string.h"
#include "array/array.h"


/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A DataFrame has a schema that
 * describes it.
 */
class DataFrame : public Object {
public:
    Schema* schema;
    Array* column_array;
public:

    /** Create a data frame with the same columns as the given df but with no rows or rownmaes */
    DataFrame(DataFrame& df) {
        schema = df.schema;
        column_array = df.column_array;
    }

    /** Create a data frame from a schema and columns. All columns are created
      * empty. */
    DataFrame(Schema& schema) {
        this->schema = &schema;
        column_array = new Array(schema.width());
        for(int i=0; i<schema.width(); i++) {
            switch(schema.col_type(i)) {
                case 'I':
                    column_array->add(new IntColumn());
                    break;
                case 'B':
                    column_array->add(new BoolColumn());
                    break;
                case 'F':
                    column_array->add(new FloatColumn());
                    break;
                case 'S':
                    column_array->add(new StringColumn());
                    break;
                default:
                    exit(1);
            }
        }
    }

    /** Returns the DataFrame's schema. Modifying the schema after a DataFrame
      * has been created in undefined. */
    Schema& get_schema() {
        return *schema;
    }

    /** Adds a column this DataFrame, updates the schema, the new column
      * is external, and appears as the last column of the DataFrame, the
      * name is optional and external. A nullptr colum is undefined. */
    void add_column(Column* col, String* name) {
        schema -> add_column(col -> get_type(), name);
        column_array -> add(col);
    }

    /** Return the value at the given column and row. Accessing rows or
     *  columns out of bounds, or request the wrong type is undefined.*/
    int get_int(size_t col, size_t row) {
        Column* target = reinterpret_cast<Column*>(column_array->get(col));
        IntColumn* int_col= target->as_int();
        IntWrapper* wrapper = reinterpret_cast<IntWrapper*>(int_col->col->get(row));
        return wrapper->data;
    }

    bool get_bool(size_t col, size_t row) {
        Column* target = reinterpret_cast<Column*>(column_array->get(col));
        BoolColumn* bool_col= target->as_bool();
        BoolWrapper* wrapper = reinterpret_cast<BoolWrapper*>(bool_col->col->get(row));
        return wrapper->data;
    }

    float get_float(size_t col, size_t row) {
        Column* target = reinterpret_cast<Column*>(column_array->get(col));
        FloatColumn* float_col= target->as_float();
        FloatWrapper* wrapper = reinterpret_cast<FloatWrapper*>(float_col->col->get(row));
        return wrapper->data;
    }

    String*  get_string(size_t col, size_t row) {
        Column* target = reinterpret_cast<Column*>(column_array->get(col));
        StringColumn* str_col= target->as_string();
        String* wrapper = reinterpret_cast<String*>(str_col->col->get(row));
        return wrapper;
    }

    /** Return the offset of the given column name or -1 if no such col. */
    int get_col(String& col) {
        return schema -> col_idx(col.c_str());
    }

    /** Return the offset of the given row name or -1 if no such row. */
    int get_row(String& col) {
        return schema -> row_idx(col.c_str());
    }

    /** Set the value at the given column and row to the given value.
      * If the column is not  of the right type or the indices are out of
      * bound, the result is undefined. */
    void set(size_t col, size_t row, int val) {
        if (row >= column_array->get_size() || schema->col_type(col) != 'I') {return;}
        IntColumn* target = reinterpret_cast<IntColumn*>(column_array -> get(col));
        delete target->col->replace(row, new IntWrapper(val));
    }

    void set(size_t col, size_t row, bool val) {
        if (row >= column_array->get_size() || schema->col_type(col) != 'B') {return;}
        BoolColumn* target = reinterpret_cast<BoolColumn*>(column_array -> get(col));
        delete target->col->replace(row, new BoolWrapper(val));
    }

    void set(size_t col, size_t row, float val) {
        if (row >= column_array->get_size() || schema->col_type(col) != 'F') {return;}
        FloatColumn* target = reinterpret_cast<FloatColumn*>(column_array -> get(col));
        delete target->col->replace(row, new FloatWrapper(val));
    }

    void set(size_t col, size_t row, String* val) {
        if (row >= reinterpret_cast<Column*>(column_array->get(col))->size() || schema->col_type(col) != 'S') {return;}
        StringColumn* target = reinterpret_cast<StringColumn*>(column_array -> get(col));
        target->col->replace(row, val);
    }

    /** Set the fields of the given row object with values from the columns at
      * the given offset.  If the row is not form the same schema as the
      * DataFrame, results are undefined.
      */
    void fill_row(size_t idx, Row& row) {
        if (row.width() != schema->width() || idx > schema->length()) {return;}
        for(int i=0; i<row.width(); i++) {
            if (row.col_type(i) != schema -> col_type(i)) {return;}
        }
        for (int i=0; i<row.width(); i++) {
            switch(row.col_type(i)) {
                case 'I': {
                    IntColumn* column = reinterpret_cast<IntColumn*>(column_array->get(i));
                    IntWrapper* wrapper = reinterpret_cast<IntWrapper*>(column->col->get(idx));
                    row.set(i, wrapper->data);
                    break;
                }
                case 'B': {
                    BoolColumn* column = reinterpret_cast<BoolColumn*>(column_array->get(i));
                    BoolWrapper* wrapper = reinterpret_cast<BoolWrapper*>(column->col->get(idx));
                    row.set(i, wrapper->data);
                    break;
                }
                case 'F': {
                    FloatColumn* column = reinterpret_cast<FloatColumn*>(column_array->get(i));
                    FloatWrapper* wrapper = reinterpret_cast<FloatWrapper*>(column->col->get(idx));
                    row.set(i, wrapper->data);
                    break;
                }
                case 'S': {
                    StringColumn* column = reinterpret_cast<StringColumn*>(column_array->get(i));
                    String* wrapper = reinterpret_cast<String*>(column->col->get(idx));
                    row.set(i, wrapper);
                    break;
                }
            }
        }
    }

    /** Add a row at the end of this DataFrame. The row is expected to have
     *  the right schema and be filled with values, otherwise undedined.  */
    void add_row(Row& row) {
        if (row.width() != schema->width()) {return;}
        for(int i=0; i<row.width(); i++) {
            if (row.col_type(i) != schema -> col_type(i)) {return;}
        }
        for (int i=0; i<row.width(); i++) {
            switch(row.col_type(i)) {
                case 'I': {
                    IntColumn* column = reinterpret_cast<IntColumn*>(column_array->get(i));
                    column->push_back(row.get_int(i));
                    break;
                }
                case 'B': {
                    BoolColumn* column = reinterpret_cast<BoolColumn*>(column_array->get(i));
                    column->push_back(row.get_bool(i));
                    break;
                }
                case 'F': {
                    FloatColumn* column = reinterpret_cast<FloatColumn*>(column_array->get(i));
                    column->push_back(row.get_float(i));
                    break;
                }
                case 'S': {
                    StringColumn* column = reinterpret_cast<StringColumn*>(column_array->get(i));
                    column -> push_back(row.get_string(i));
                    break;
                }
            }
        }
    }

    /** The number of rows in the DataFrame. */
    size_t nrows() {
        if (ncols() < 1 || column_array -> get_size() < 1) {return 0;}
        return reinterpret_cast<Column*>(column_array->get(0)) -> size();
    }

    /** The number of columns in the DataFrame.*/
    size_t ncols() {
        return schema -> width();
    }

    /** Visit rows in order */
    void map(Rower& r) {
        for (int i=0; i<nrows(); i++) {
            Row* row = new Row(*schema);
            row->set_idx(i);
            fill_row(i, *row);
            r.accept(*row);
            delete row;
        }
    }

    /** Create a new DataFrame, constructed from rows for which the given Rower
      * returned true from its accept method. */
    DataFrame* filter(Rower& r) {
        DataFrame* result = new DataFrame(*(new Schema(*schema)));
        for(int i=0; i<nrows(); i++) {
            Row* temp = new Row(*schema);
            fill_row(i, *temp);
            if (r.accept(*temp)) {
                result->add_row(*temp);
            } else {
                delete temp;
            }
        }
        return result;
    }

    /** Print the DataFrame in SoR format to standard output. */
    void print() {
        for(int i=0; i<schema->width(); i++) {
            String* col_name = schema->col_name(i);
            if (col_name != nullptr) {
                printf("%s ", col_name->c_str());
            } else {
                printf(" ");
            }
        }
        printf("\n");
        int size = reinterpret_cast<Column*>(column_array->get(0)) -> size();
        for(int i=0; i<size; i++) {
            String* row_name = schema->row_name(i);
            if (row_name != nullptr) {printf("%s ", row_name->c_str());}
            for (int j=0; j<schema->width(); j++) {
                Column* column = reinterpret_cast<Column*>(column_array->get(j));
                Object* object = column->col->get(i);
                printf("%s ", object->c_str());
            }
            printf("\n");
        }
    }

    static void pmap_helper(Rower* r, size_t from, size_t to, DataFrame* df) {
        for(int i=from; i<to; i++) {
            Row* row = new Row(df -> get_schema());
            row->set_idx(i);
            df -> fill_row(i, *row);
            r-> accept(*row);
        }
    }

    /** Visit rows using muti-threading **/
    void pmap(Rower& r) {
        std::thread* pool[2];
        Rower* rower[2];
        size_t length = reinterpret_cast<Column*>(column_array->get(0)) -> col -> get_size();
        for (int i = 0; i < 2; i++) {
            Rower* copy = r.clone();
            rower[i] = copy;
            pool[i] = new std::thread(&DataFrame::pmap_helper, copy, i * length/2, i==1 ? length : length/2, this);
        }
        for (int i = 0; i < 2; i++) {
            pool[i]->join();
        }
        for (int i = 0; i < 2; i++) {
            rower[i]->join_delete(&r);
        }
    }
};


#endif //CS4500_A4_MMDataFrame_H
