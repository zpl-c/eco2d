#pragma once
#include "platform/system.h"
#include "zpl.h"

bool db_init();
void db_shutdown();

// raw query data getters
char *db_get(char *query, bool header);
char *db_row(char *table, char *name);
bool  db_csv(zpl_csv_object *csv, char *query);
bool  db_row_csv(zpl_csv_object *csv, char *table, char *name);

// csv backed data queries
bool db_push(char *query);
bool db_row_push(char *table, char *name);
zpl_adt_node *db_field(char *field, int row);
size_t db_rows();
#define db_str(fld, row) (db_field(fld, row)->string)
#define db_flt(fld, row) (db_field(fld, row)->type == ZPL_ADT_TYPE_STRING ? 0.0f : db_field(fld, row)->real)
#define db_int(fld, row) (db_field(fld, row)->type == ZPL_ADT_TYPE_STRING ? 0    : db_field(fld, row)->integer)
zpl_csv_object *db_last();
void db_pop();

// sql execution
bool db_exec_file(const char *sql);
bool db_exec(const char *query);

