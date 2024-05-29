-- typedef enum {
--     // NOTE(zaklaus): hardcoded fields for placement ops
--     UKIND_DELETE,
--     UKIND_PLACE,
--     UKIND_PLACE_ITEM,
--     UKIND_PLACE_ITEM_DATA,
--     UKIND_END_PLACE,
--     
--     // NOTE(zaklaus): the rest of possible actions
--     UKIND_HOLD,
--     UKIND_PROXY,
-- } item_usage;
--
-- typedef enum {
--     UDATA_NONE,
--     UDATA_ENERGY_SOURCE,
-- } item_attachment;

INSERT INTO items (kind, usage, attachment, max_quantity, place_kind, directional, proxy_id, place_item_id, producer, energy_level, blueprint_w, blueprint_h, blueprint_plan) VALUES
(0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('FENCE'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('COAL'), 5, 1, 64, NULL, NULL, NULL, NULL, asset('FURNACE'), 15.0, NULL, NULL, NULL),
(asset('WOOD'), 1, 0, 64, asset('WOOD'), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('TREE'), 1, 0, 64, asset('TREE'), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('TEST_TALL'), 1, 0, 64, asset('TEST_TALL'), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('BELT'), 1, 0, 999, asset('BELT'), 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('BELT_LEFT'), 6, 0, NULL, NULL, NULL, asset('BELT'), NULL, NULL, NULL, NULL, NULL, NULL),
(asset('BELT_RIGHT'), 6, 0, NULL, NULL, NULL, asset('BELT'), NULL, NULL, NULL, NULL, NULL, NULL),
(asset('BELT_UP'), 6, 0, NULL, NULL, NULL, asset('BELT'), NULL, NULL, NULL, NULL, NULL, NULL),
(asset('BELT_DOWN'), 6, 0, NULL, NULL, NULL, asset('BELT'), NULL, NULL, NULL, NULL, NULL, NULL),
(asset('CHEST'), 2, 0, 32, NULL, NULL, NULL, asset('CHEST'), NULL, NULL, NULL, NULL, NULL),
(asset('CRAFTBENCH'), 2, 0, 32, NULL, NULL, NULL, asset('CRAFTBENCH'), NULL, NULL, NULL, NULL, NULL),
(asset('FURNACE'), 2, 0, 32, NULL, NULL, NULL, asset('FURNACE'), NULL, NULL, NULL, NULL, NULL),
(asset('SPLITTER'), 2, 0, 32, NULL, NULL, NULL, asset('SPLITTER'), NULL, NULL, NULL, NULL, NULL),
(asset('ASSEMBLER'), 2, 0, 32, NULL, NULL, NULL, asset('ASSEMBLER'), NULL, NULL, NULL, NULL, NULL),
(asset('CREATURE'), 2, 0, 32, NULL, NULL, NULL, asset('CREATURE'), NULL, NULL, NULL, NULL, NULL),
(asset('IRON_ORE'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('IRON_INGOT'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('IRON_PLATES'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('SCREWS'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('LOG'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('PLANK'), 5, 0, 64, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(asset('CREATURE_FOOD'), 5, 0, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
-- (asset('BLUEPRINT_DEMO_HOUSE'), 3, 0, 1, NULL, NULL, NULL, asset('BLUEPRINT'), NULL, NULL, 4, 4, ''),
