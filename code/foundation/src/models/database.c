#include "models/database.h"
#include "models/assets.h"

#include "3rd_sqlite3.h"
#include "zpl.h"

#include "world/blocks.h"
#include "models/crafting.h"
#include "models/items.h"

#ifndef ECO2D_DB
#define ECO2D_DB "eco2d.db"
#endif

static sqlite3 *db;
static zpl_array(zpl_csv_object) csv_stack;

static
void sql_asset(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    if (argc == 1) {
        const char *name = (const char *)sqlite3_value_text(argv[0]);
        if (name) {
            sqlite3_result_int(ctx, atoi(db_get(zpl_bprintf("SELECT id FROM assets WHERE name = '%s'", name), false)));
            return;
        }
    }

    sqlite3_result_null(ctx);
}

bool db_init() {
    bool new_db = !zpl_fs_exists(ECO2D_DB);
    sqlite3_open(ECO2D_DB, &db);
    assert(db && "Failed to open database.");

    zpl_array_init(csv_stack, zpl_heap());
    sqlite3_create_function(db, "asset", 1, SQLITE_UTF8, NULL, sql_asset, NULL, NULL);

    if (new_db) {
        zpl_printf("[INFO] Creating new database...\n");
        db_exec_file("art/queries/tables.sql");
        assets_db_init();

        db_exec_file("art/queries/resources.sql");
        db_exec_file("art/queries/blocks.sql");
        db_exec_file("art/queries/recipes.sql");
        db_exec_file("art/queries/items.sql");
    }

    // initialise models db
    zpl_printf("[INFO] Loading models from database...\n");
    assets_db(); zpl_printf("[INFO] Assets loaded.\n");
    blocks_db(); zpl_printf("[INFO] Blocks loaded.\n");
    craft_db(); zpl_printf("[INFO] Recipes loaded.\n");
    item_db(); zpl_printf("[INFO] Items loaded.\n");
    return new_db;
}

void db_shutdown() {
    sqlite3_close(db);
}

char *db_get(char *query, bool header) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    if (rc) {
        zpl_printf("[ERROR] Failed to prepare query: %s\n", sqlite3_errmsg(db));
        return 0;
    }
        
    zpl_string buf = zpl_string_make(zpl_heap(), "");
    bool is_first=header;
    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (is_first) {
            is_first = 0;
            for (int i = 0; i < cols; i++) {
                buf = zpl_string_appendc(buf, (const char *)sqlite3_column_name(stmt, i));

                if (i < cols - 1) {
                    buf = zpl_string_appendc(buf, ",");
                }
            }

            buf = zpl_string_appendc(buf, "\n");
        }
        for (int i = 0; i < cols; i++) {
            const char *val = (const char *)sqlite3_column_text(stmt, i);
            if (val == NULL)
                val = "\"(null)\"";
            buf = zpl_string_appendc(buf, val);

            if (i < cols - 1) {
                buf = zpl_string_appendc(buf, ",");
            }
        }

        buf = zpl_string_appendc(buf, "\n");
    }
    sqlite3_finalize(stmt);
#if 0
    char *data = zpl_bprintf("%s", buf);
    zpl_string_free(buf);
#else //@leak
    char *data = (char *)buf;
#endif
    return data;
}

char *db_row(char *table, char *name) {
    int kind = atoi(db_get(zpl_bprintf("SELECT id FROM assets WHERE name='%s';", name), 0));
    return db_get(zpl_bprintf("SELECT * FROM %s WHERE kind='%d';", table, kind), 1);
}

bool db_csv(zpl_csv_object *csv, char *query) {
    zpl_zero_item(csv);
    char *res = db_get(query, 1);
    assert(res);
    return zpl_csv_parse(csv, res, zpl_heap(), 1);
}

bool db_row_csv(zpl_csv_object *csv, char *table, char *name) {
    zpl_zero_item(csv);
    char *res = db_row(table, name);
    assert(res);
    return zpl_csv_parse(csv, res, zpl_heap(), 1);
}

bool db_push(char *query) {
    zpl_csv_object csv;
    if (db_csv(&csv, query)) {
        return false;
    }
    zpl_array_append(csv_stack, csv);
    return true;
}

bool db_row_push(char *table, char *name) {
    zpl_csv_object csv;
    if (db_row_csv(&csv, table, name)) {
        return false;
    }
    zpl_array_append(csv_stack, csv);
    return true;
}

zpl_adt_node *db_field(char *field, int row) {
    zpl_csv_object *csv = db_last();
    for (int i = 0; i < zpl_array_count(csv->nodes); i++) {
        if (!zpl_strcmp(csv->nodes[i].name, field)) {
            return &csv->nodes[i].nodes[row];
        }
    }
    assert(0 && "Field not found");
    return NULL;
}

size_t db_rows() {
    zpl_csv_object *csv = db_last();
    return zpl_array_count(csv->nodes[0].nodes);
}

zpl_csv_object *db_last() {
    assert(zpl_array_count(csv_stack) > 0);
    return zpl_array_end(csv_stack);
}

void db_pop() {
    assert(zpl_array_count(csv_stack) > 0);
    zpl_csv_free(zpl_array_end(csv_stack));
    zpl_array_pop(csv_stack);
}

bool db_exec_file(const char *sql) {
    zpl_file_contents contents = zpl_file_read_contents(zpl_heap(), true, sql);
    if (!db_exec(contents.data)) {
        assert(0 && "Failed to create tables");
        return false;
    }
    zpl_file_free_contents(&contents);
    return true;
}

bool db_exec(const char *query) {
    bool ok = !sqlite3_exec(db, query, 0, 0, 0);
    if (!ok) {
        zpl_printf("[ERROR] Failed to execute query: %s\n", sqlite3_errmsg(db));
    }
    return ok;
}

