-- typedef enum {
--     BLOCK_FLAG_COLLISION = (1 << 1),
--     BLOCK_FLAG_HAZARD = (1 << 2),
--     BLOCK_FLAG_ESSENTIAL = (1 << 3),
--     BLOCK_FLAG_DESTROY_ON_COLLISION = (1 << 4),
--     BLOCK_FLAG_ENTITY = (1 << 5),
-- } block_flags;

INSERT INTO blocks (kind, flags, drag, friction, bounce, velx, vely) VALUES
(asset('EMPTY'), NULL, NULL, NULL, NULL, NULL, NULL),
(asset('GROUND'), NULL, 1.0, 1.0, NULL, NULL, NULL),
(asset('DIRT'), NULL, 2.1, 1.0, NULL, NULL, NULL),
(asset('WALL'), (1<<1), 1.0, 1.0, 1.0, NULL, NULL),
(asset('HILL'), (1<<1), 1.0, 1.0, NULL, NULL, NULL),
(asset('HILL_SNOW'), (1<<1), 1.0, 1.0, NULL, NULL, NULL),
(asset('WATER'), 0, 0.11, 1.0, NULL, NULL, NULL),
(asset('LAVA'), (1<<2), 6.2, 4.0, NULL, NULL, NULL),
(asset('FENCE'), (1<<1), 1.0, 1.0, 1.0, NULL, NULL),
(asset('WOOD'), (1<<1), 1.0, 1.0, 0.0, NULL, NULL),
(asset('TREE'), (1<<1)|(1<<4), 1.0, 1.0, 0.0, NULL, NULL),
(asset('CHEST'), (1<<5), NULL, NULL, NULL, NULL, NULL),
(asset('FURNACE'), (1<<5), NULL, NULL, NULL, NULL, NULL),
(asset('TEST_TALL'), (1<<1), NULL, NULL, NULL, NULL, NULL),
(asset('BELT_LEFT'), NULL, 1.0, 1.0, NULL, -150.0, NULL),
(asset('BELT_RIGHT'), NULL, 1.0, 1.0, NULL, 150.0, NULL),
(asset('BELT_UP'), NULL, 1.0, 1.0, NULL, NULL, -150.0),
(asset('BELT_DOWN'), NULL, 1.0, 1.0, NULL, NULL, 150.0);

