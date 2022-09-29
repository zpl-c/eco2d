#pragma once

#define ITEM_HOLD(asset, qty)\
{\
.kind = asset,\
.usage = UKIND_HOLD,\
.attachment = UDATA_NONE,\
.max_quantity = qty,\
}

#define ITEM_ENERGY(asset, producer_asset, qty, energy_value)\
{\
.kind = asset,\
.usage = UKIND_HOLD,\
.attachment = UDATA_ENERGY_SOURCE,\
.max_quantity = qty,\
.energy_source = {\
.producer = producer_asset,\
.energy_level = energy_value\
}\
}

#define ITEM_BLUEPRINT(asset, qty, w_, h_, plan_)\
{\
.kind = asset,\
.usage = UKIND_PLACE_ITEM_DATA,\
.attachment = UDATA_NONE,\
.max_quantity = qty,\
.blueprint = {\
.w = w_,\
.h = h_,\
.plan = (const asset_id[])plan_\
},\
.place_item = {\
.id = asset\
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
