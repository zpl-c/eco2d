#pragma once

#define ITEM_HOLD(asset, qty)\
(item_desc){\
.kind = asset,\
.usage = UKIND_HOLD,\
.max_quantity = qty,\
}

#define ITEM_BLOCK(asset, qty, build_asset)\
(item_desc){\
.kind = asset,\
.usage = UKIND_PLACE,\
.max_quantity = qty,\
.place = {\
.kind = build_asset,\
}\
}

#define ITEM_BLOCK_DIR(asset, qty, build_asset)\
(item_desc){\
.kind = asset,\
.usage = UKIND_PLACE,\
.max_quantity = qty,\
.place = {\
.kind = build_asset,\
.directional = true,\
}\
}

#define ITEM_PROXY(asset, proxy_id)\
(item_desc){\
.kind = asset,\
.usage = UKIND_PROXY,\
.proxy = {\
.id = proxy_id,\
}\
}

#define ITEM_ENT(asset, qty, eid)\
(item_desc){\
.kind = asset,\
.usage = UKIND_PLACE_ITEM,\
.max_quantity = qty,\
.place_item = {\
.id = eid\
}\
}

#define ITEM_SELF(asset, qty) ITEM_BLOCK(asset, qty, asset)
#define ITEM_SELF_DIR(asset, qty) ITEM_BLOCK_DIR(asset, qty, asset)
