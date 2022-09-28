#pragma once

#define ITEM_HOLD(asset, qty)\
{\
.kind = asset,\
.usage = UKIND_HOLD,\
.attachment = UDATA_NONE,\
.max_quantity = qty,\
}

#define ITEM_FUEL(asset, qty, fuel_value)\
{\
.kind = asset,\
.usage = UKIND_HOLD,\
.attachment = UDATA_FUEL,\
.max_quantity = qty,\
.fuel = {\
.burn_time = fuel_value\
}\
}

#define ITEM_INGREDIENT(asset, qty, _producer, _product, _additional)\
{\
.kind = asset,\
.usage = UKIND_HOLD,\
.attachment = UDATA_INGREDIENT,\
.max_quantity = qty,\
.ingredient = {\
.producer = _producer,\
.product = _product,\
.additional_ingredient = _additional,\
}\
}

#define ITEM_BLOCK(asset, qty, build_asset)\
{\
.kind = asset,\
.usage = UKIND_PLACE,\
.attachment = UDATA_NONE,\
.max_quantity = qty,\
.place = {\
.kind = build_asset,\
}\
}

#define ITEM_BLOCK_DIR(asset, qty, build_asset)\
{\
.kind = asset,\
.usage = UKIND_PLACE,\
.attachment = UDATA_NONE,\
.max_quantity = qty,\
.place = {\
.kind = build_asset,\
.directional = true,\
}\
}

#define ITEM_PROXY(asset, proxy_id)\
{\
.kind = asset,\
.usage = UKIND_PROXY,\
.attachment = UDATA_NONE,\
.proxy = {\
.id = proxy_id,\
}\
}

#define ITEM_ENT(asset, qty, eid)\
{\
.kind = asset,\
.usage = UKIND_PLACE_ITEM,\
.attachment = UDATA_NONE,\
.max_quantity = qty,\
.place_item = {\
.id = eid\
}\
}

#define ITEM_SELF(asset, qty) ITEM_BLOCK(asset, qty, asset)
#define ITEM_SELF_DIR(asset, qty) ITEM_BLOCK_DIR(asset, qty, asset)
