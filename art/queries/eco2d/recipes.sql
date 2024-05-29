INSERT INTO recipes (product, product_qty, process_ticks, producer) VALUES
    (asset('IRON_PLATES'), 4, 20, asset('FURNACE')),
    (asset('SCREWS'), 8, 40, asset('CRAFTBENCH')),
    (asset('BELT'), 1, 120, asset('ASSEMBLER'));

INSERT INTO reagents (asset_id, qty) VALUES
    (asset('IRON_ORE'), 1), 
    (asset('IRON_PLATES'), 1), 
    (asset('FENCE'), 1), 
    (asset('SCREWS'), 4), 
    (asset('IRON_PLATES'), 2); 

INSERT INTO recipe_reagents (recipe_id, reagent_id) VALUES
    (1, 1), 
    (2, 2), 
    (3, 3), 
    (3, 4), 
    (3, 5); 
