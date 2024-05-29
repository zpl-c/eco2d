CREATE TABLE assets (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(255) NOT NULL
);

CREATE TABLE resources (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    asset INTEGER NOT NULL,
    kind INTEGER NOT NULL
);

CREATE TABLE blocks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    kind INTEGER NOT NULL,
    flags INTEGER DEFAULT 0,
    drag REAL,
    friction REAL,
    bounce REAL,
    velx REAL,
    vely REAL,
    FOREIGN KEY (kind) REFERENCES assets(id)
);

CREATE TABLE items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    kind INTEGER NOT NULL,
    usage INTEGER,
    attachment INTEGER,
    max_quantity INTEGER,
    has_storage BOOLEAN,
    
    place_kind INTEGER,
    directional BOOLEAN,
    
    proxy_id INTEGER,
    
    place_item_id INTEGER,
    
    producer INTEGER,
    energy_level REAL,
    
    blueprint_w INTEGER,
    blueprint_h INTEGER,
    blueprint_plan TEXT, 

    FOREIGN KEY (kind) REFERENCES assets(id)
);

CREATE TABLE reagents (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    asset_id INTEGER NOT NULL,
    qty INTEGER NOT NULL,
    FOREIGN KEY (asset_id) REFERENCES assets(id)
);

CREATE TABLE recipes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    product INTEGER NOT NULL,
    product_qty INTEGER NOT NULL,
    process_ticks INTEGER NOT NULL,
    producer INTEGER NOT NULL,
    FOREIGN KEY (product) REFERENCES assets(id),
    FOREIGN KEY (producer) REFERENCES assets(id)
);

CREATE TABLE recipe_reagents (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    recipe_id INTEGER NOT NULL,
    reagent_id INTEGER NOT NULL,
    FOREIGN KEY (recipe_id) REFERENCES recipes(id),
    FOREIGN KEY (reagent_id) REFERENCES reagents(id)
);
