#include "server.h"

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOLEAN
} DataType;

typedef struct {
    char *name;
    DataType type
} Column;

typedef struct {
    char *name;
    Column *columns;
    size_t column_count;
    void **rows;
    size_t row_count;
    size_t row_capacity;
} Table;

typedef struct {
    Table **tables;
    size_t table_count;
    size_t table_capacity;
} Database;

Database* create_database() {
    Database *db = malloc(sizeof(Database));
    db->tables = NULL;
    db->table_count = 0;
    db->table_capacity = 0;

    return db;
}

Table* create_table(const char *name, Column *columns, size_t column_count) {
    Table *table = malloc(sizeof(Table));
    table->name = strdup(name);
    table->columns = malloc(sizeof(Column) * column_count);
    memcpy(table->columns, columns, sizeof(Column) * column_count);
    table->column_count = column_count;
    table->rows = NULL;
    table->row_count = 0;
    table->row_capacity = 0;

    return table;
}

void add_table(Database *db, Table *table) {
    if (db->table_count >= db->table_capacity) {
        db->table_capacity = db->table_capacity == 0 ? 10 : db->table_capacity * 2;
        db->tables = realloc(db->tables, db->table_capacity * sizeof(Table *));
    }
    db->tables[db->table_count++] = table;
}

void insert_row(Table *table, void **row) {
    if (table->row_count >= table->row_capacity) {
        table->row_capacity = table->row_capacity == 0 ? 10 : table->row_capacity * 2;
        table->rows = realloc(table->rows, table->row_capacity * sizeof(void *));
    }
    table->rows[table->row_count++] = row;
}

void* select_rows(Table *table, const char* column_name, size_t *results_count) {
    size_t results_capacity = 10;
    void **results = malloc(results_capacity * sizeof(void *));
    *results_count = 0;

    // Iterate through rows
    for (size_t i = 0; i < table->row_count; i++) {
        void **row = table->rows[i];

        // Iterate through columns in each row
        for (size_t j = 0; j < table->column_count; j++) {
            if (strcmp(table->columns[i].name, column_name) == 0) {
                if (results_count >= results_capacity) {
                    results_capacity *= 2;
                    results = realloc(results, results_capacity * sizeof(void *));
                }
                results[(*results_count)++] = row;
                break;
            }
        }
    }

    return results;
}

void free_table(Table *table) {
    if (!table) {
        return;
    }

    for (size_t i = 0; i < table->column_count; i++) {
        free(table->columns[i].name);
    }
    free(table->columns);

    for (size_t i = 0; i < table->row_count; i++) {
        free(table->rows[i]);
    }
    free(table->rows);

    free(table->name);

    free(table);
}

void free_database(Database *db) {
    if (!db) {
        return;
    }

    for (size_t i = 0; i < db->table_capacity; i++) {
        free_table(db->tables[i]);
    }
    free(db->tables);

    free(db);
}
