#ifndef FLECS_JSON_IMPL
#include "flecs_json.h"
#endif

/* Simple serializer to turn values into strings. Use this code as a template
 * for when implementing a new serializer. */

static
void json_ser_type(
    ecs_world_t *world,
    ecs_vector_t *ser, 
    const void *base, 
    ecs_strbuf_t *str);

static
void json_ser_type_op(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str);

/* Serialize a primitive value */
static
void json_ser_primitive(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const char *bool_str[] = { "false", "true" };

    switch(op->is.primitive) {
    case EcsBool:
        ecs_strbuf_appendstr(str, bool_str[(int)*(bool*)base]);
        break;
    case EcsChar:
        ecs_strbuf_appendstrn(str, "\"", 1);
        ecs_strbuf_appendstrn(str, (char*)base, 1);
        ecs_strbuf_appendstrn(str, "\"", 1);
        break;
    case EcsByte:
        ecs_strbuf_append(str, "%u", *(uint8_t*)base);
        break;
    case EcsU8:
        ecs_strbuf_append(str, "%u", *(uint8_t*)base);
        break;
    case EcsU16:
        ecs_strbuf_append(str, "%u", *(uint16_t*)base);
        break;
    case EcsU32:
        ecs_strbuf_append(str, "%u", *(uint32_t*)base);
        break;
    case EcsU64:
        ecs_strbuf_append(str, "%u", *(uint64_t*)base);
        break;
    case EcsI8:
        ecs_strbuf_append(str, "%d", *(int8_t*)base);
        break;
    case EcsI16:
        ecs_strbuf_append(str, "%d", *(int16_t*)base);
        break;
    case EcsI32:
        ecs_strbuf_append(str, "%d", *(int32_t*)base);
        break;
    case EcsI64:
        ecs_strbuf_append(str, "%d", *(int64_t*)base);
        break;
    case EcsF32:
        ecs_strbuf_append(str, "%f", *(float*)base);
        break;
    case EcsF64:
        ecs_strbuf_append(str, "%f", *(double*)base);
        break;
    case EcsIPtr:
        ecs_strbuf_append(str, "%i", *(intptr_t*)base);
        break;
    case EcsUPtr:
        ecs_strbuf_append(str, "%u", *(uintptr_t*)base);
        break;
    case EcsString: {
        char *value = *(char**)base;
        if (value) {
            ecs_strbuf_appendstrn(str, "\"", 1);
            ecs_strbuf_appendstr(str, value);
            ecs_strbuf_appendstrn(str, "\"", 1);
        } else {
            ecs_strbuf_appendstr(str, "null");
        }
        break;
    }
    case EcsEntity:
        ecs_strbuf_appendstrn(str, "\"", 1);
        ecs_strbuf_appendstr(str, ecs_get_name(world, *(ecs_entity_t*)base));
        ecs_strbuf_appendstrn(str, "\"", 1);
        break;
    }
}

/* Serialize enumeration */
static
void json_ser_enum(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const EcsEnum *enum_type = ecs_get_ref_w_entity(world, &op->is.constant, 0, 0);
    ecs_assert(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t value = *(int32_t*)base;
    
    /* Enumeration constants are stored in a map that is keyed on the
     * enumeration value. */
    char **constant = ecs_map_get(enum_type->constants, char*, value);
    ecs_assert(constant != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_appendstrn(str, "\"", 1);
    ecs_strbuf_appendstr(str, *constant);
    ecs_strbuf_appendstrn(str, "\"", 1);
}

/* Serialize bitmask */
static
void json_ser_bitmask(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const EcsBitmask *bitmask_type = ecs_get_ref_w_entity(world, &op->is.constant, 0, 0);
    ecs_assert(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t value = *(int32_t*)base;
    ecs_map_key_t key;
    char **constant;

    ecs_strbuf_list_push(str, "[", ",");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(bitmask_type->constants);
    while ((constant = ecs_map_next(&it, char*, &key))) {
        if ((value & key) == key) {
            ecs_strbuf_list_append(str, "\"%s\"", *constant);
        }
    }

    ecs_strbuf_list_pop(str, "]");
}

/* Serialize elements of a contiguous array */
static
void json_ser_elements(
    ecs_world_t *world,
    ecs_vector_t *elem_ops, 
    const void *base, 
    int32_t elem_count, 
    int32_t elem_size,
    ecs_strbuf_t *str)
{
    ecs_strbuf_list_push(str, "[", ",");

    const void *ptr = base;

    int i;
    for (i = 0; i < elem_count; i ++) {
        ecs_strbuf_list_next(str);
        json_ser_type(world, elem_ops, ptr, str);
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    ecs_strbuf_list_pop(str, "]");
}

/* Serialize array */
static
void json_ser_array(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    void *base, 
    ecs_strbuf_t *str) 
{
    const EcsMetaTypeSerializer *ser = ecs_get_ref_w_entity(world, &op->is.collection, 0, 0);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    json_ser_elements(world, ser->ops, base, op->count, op->size, str);
}

/* Serialize vector */
static
void json_ser_vector(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_vector_t *value = *(ecs_vector_t**)base;
    
    int32_t count = ecs_vector_count(value);
    void *array = ecs_vector_first_t(value, op->size, op->alignment);

    const EcsMetaTypeSerializer *ser = ecs_get_ref_w_entity(world, &op->is.collection, 0, 0);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *elem_ops = ser->ops;
    
    ecs_type_op_t *elem_op_hdr = (ecs_type_op_t*)ecs_vector_first(elem_ops, ecs_type_op_t);
    ecs_assert(elem_op_hdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(elem_op_hdr->kind == EcsOpHeader, ECS_INTERNAL_ERROR, NULL);
    size_t elem_size = elem_op_hdr->size;

    /* Serialize contiguous buffer of vector */
    json_ser_elements(world, elem_ops, array, count, elem_size, str);
}

/* Serialize map */
static
void json_ser_map(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_map_t *value = *(ecs_map_t**)base;

    const EcsMetaTypeSerializer *key_ser = ecs_get_ref_w_entity(world, &op->is.map.key, 0, 0);
    ecs_assert(key_ser != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsMetaTypeSerializer *elem_ser = ecs_get_ref_w_entity(world, &op->is.map.element, 0, 0);
    ecs_assert(elem_ser != NULL, ECS_INTERNAL_ERROR, NULL);

    /* 2 instructions, one for the header */
    ecs_assert(ecs_vector_count(key_ser->ops) == 2, ECS_INTERNAL_ERROR, NULL);

    ecs_type_op_t *key_op = ecs_vector_first(key_ser->ops, ecs_type_op_t);
    ecs_assert(key_op->kind == EcsOpHeader, ECS_INTERNAL_ERROR, NULL);
    key_op = &key_op[1];

    ecs_map_iter_t it = ecs_map_iter(value);  
    ecs_map_key_t key; 
    void *ptr;

    ecs_strbuf_list_push(str, "{", ",");

    while ((ptr = _ecs_map_next(&it, 0, &key))) {
        ecs_strbuf_list_next(str);
        ecs_strbuf_appendstrn(str, "\"", 1);
        json_ser_type_op(world, key_op, (void*)&key, str);
        ecs_strbuf_appendstrn(str, "\"", 1);
        ecs_strbuf_appendstr(str, ":");
        json_ser_type(world, elem_ser->ops, ptr, str);

        key = 0;
    }

    ecs_strbuf_list_pop(str, "}");
}

/* Forward serialization to the different type kinds */
static
void json_ser_type_op(
    ecs_world_t *world,
    ecs_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str) 
{
    switch(op->kind) {
    case EcsOpHeader:
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpPrimitive:
        json_ser_primitive(world, op, ECS_OFFSET(base, op->offset), str);
        break;
    case EcsOpEnum:
        json_ser_enum(world, op, ECS_OFFSET(base, op->offset), str);
        break;
    case EcsOpBitmask:
        json_ser_bitmask(world, op, ECS_OFFSET(base, op->offset), str);
        break;
    case EcsOpArray:
        json_ser_array(world, op, ECS_OFFSET(base, op->offset), str);
        break;
    case EcsOpVector:
        json_ser_vector(world, op, ECS_OFFSET(base, op->offset), str);
        break;
    case EcsOpMap:
        json_ser_map(world, op, ECS_OFFSET(base, op->offset), str);
        break;
    }
}

/* Iterate over the type ops of a type */
static
void json_ser_type(
    ecs_world_t *world,
    ecs_vector_t *ser, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_type_op_t *ops = (ecs_type_op_t*)ecs_vector_first(ser, ecs_type_op_t);
    int32_t count = ecs_vector_count(ser);

    for (int i = 0; i < count; i ++) {
        ecs_type_op_t *op = &ops[i];

        if (op->name) {
            if (op->kind != EcsOpHeader) {
                ecs_strbuf_list_next(str);
            }

            ecs_strbuf_append(str, "\"%s\":", op->name);
        }

        switch(op->kind) {
        case EcsOpHeader:
            break;
        case EcsOpPush:
            ecs_strbuf_list_push(str, "{", ",");
            break;
        case EcsOpPop:
            ecs_strbuf_list_pop(str, "}");
            break;
        default:
            json_ser_type_op(world, op, base, str);
            break;
        }
    }
}

char* ecs_ptr_to_json(
    ecs_world_t *world, 
    ecs_entity_t type, 
    void* ptr)
{
    ecs_entity_t ecs_typeid(EcsMetaTypeSerializer) = ecs_lookup(world, "EcsMetaTypeSerializer");
    const EcsMetaTypeSerializer *ser = ecs_get(world, type, EcsMetaTypeSerializer);
    ecs_assert(ser != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_t str = ECS_STRBUF_INIT;
    json_ser_type(world, ser->ops, ptr, &str);
    return ecs_strbuf_get(&str);
}

void json_ser_column(
    ecs_world_t *world,
    const EcsMetaTypeSerializer *ser, 
    void *ptr,
    int32_t count,
    ecs_strbuf_t *str)
{
    ecs_vector_t *ops = ser->ops;
    ecs_type_op_t *hdr = ecs_vector_first(ops, ecs_type_op_t);
    ecs_assert(hdr->kind == EcsOpHeader, ECS_INTERNAL_ERROR, NULL);
    int32_t size = hdr->size;

    ecs_strbuf_list_push(str, "[", ",");

    int i;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_list_next(str);
        json_ser_type(world, ops, ptr, str);
        ptr = ECS_OFFSET(ptr, size);
    }

    ecs_strbuf_list_pop(str, "]");
}

static
void serialize_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_strbuf_t *str)
{
    ecs_strbuf_list_push(str, "[", ",");

    int i, count = ecs_vector_count(type);
    ecs_entity_t *comps = ecs_vector_first(type, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t comp = comps[i];
        bool has_role = (comp & ECS_COMPONENT_MASK) != comp;
        ecs_entity_t comp_e = comp & ECS_COMPONENT_MASK;
        
        if (has_role) {
            ecs_strbuf_list_next(str);
            ecs_strbuf_list_push(str, "[", ",");
            ecs_strbuf_list_append(str, "\"%s\"", ecs_role_str(comp));
        }

        if (ECS_HAS_ROLE(comp, TRAIT)) {
            ecs_entity_t hi = ecs_entity_t_hi(comp_e);
            char *hi_path = ecs_get_fullpath(world, hi);
            ecs_strbuf_list_append(str, "\"%s\"", hi_path);
            ecs_os_free(hi_path);
            comp_e = ecs_entity_t_lo(comp_e);
        }

        char *comp_path = ecs_get_fullpath(world, comp_e);
        if (comp_path) {
            ecs_strbuf_list_append(str, "\"%s\"", comp_path);
        } else {
            ecs_strbuf_list_append(str, "%d", (int32_t)comp);
        }

        if (has_role) {
            ecs_strbuf_list_pop(str, "]");
        }

        ecs_os_free(comp_path);
    }

    ecs_strbuf_list_pop(str, "]");
}

char* ecs_type_to_json(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    serialize_type(world, type, &str);
    return ecs_strbuf_get(&str);
}

char* ecs_iter_to_json(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_iter_next_action_t iter_next,
    ecs_type_t select)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    ecs_entity_t ecs_typeid(EcsMetaTypeSerializer) = 
        ecs_lookup_fullpath(world, "flecs.meta.MetaTypeSerializer");
    ecs_assert(ecs_typeid(EcsMetaTypeSerializer) != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_strbuf_list_push(&str, "[", ",");

    while (iter_next(it)) {
        ecs_type_t table_type = ecs_iter_type(it);
        ecs_entity_t *comps = ecs_vector_first(table_type, ecs_entity_t);
        int32_t i, count = ecs_vector_count(table_type);

        if (!it->count) {
            continue;
        }

        ecs_strbuf_list_next(&str);
        ecs_strbuf_list_push(&str, "{", ",");

        /* Serialize type */
        ecs_strbuf_list_appendstr(&str, "\"type\":");
        serialize_type(world, table_type, &str);

        /* Add entity identifiers */
        ecs_strbuf_list_appendstr(&str, "\"entities\":");
        ecs_strbuf_list_push(&str, "[", ",");
        for (i = 0; i < it->count; i ++) {
            ecs_strbuf_list_append(&str, "%d", (int32_t)it->entities[i]);
        }
        ecs_strbuf_list_pop(&str, "]");

        /* Serialize data */
        ecs_strbuf_list_appendstr(&str, "\"data\":");
        ecs_strbuf_list_push(&str, "{", ",");

        for (i = 0; i < count; i ++) {
            if (select) {
                if (!ecs_type_has_entity(world, select, comps[i])) {
                    continue;
                }
            }
            const EcsMetaTypeSerializer *ser = ecs_get(
                    world, comps[i], EcsMetaTypeSerializer);
                
            /* Don't serialize if there's no metadata for component */
            if (!ser) {
                continue;
            }

            char *comp_path = ecs_get_fullpath(world, comps[i]);
            ecs_strbuf_list_append(&str, "\"%s\":", comp_path);
            ecs_os_free(comp_path);

            json_ser_column(
                world, ser, ecs_table_column(it, i), it->count, &str);
        }

        ecs_strbuf_list_pop(&str, "}");
        ecs_strbuf_list_pop(&str, "}");
    }    

    ecs_strbuf_list_pop(&str, "]");

    return ecs_strbuf_get(&str);
}

char* ecs_entity_to_json(
    ecs_world_t *world, 
    ecs_entity_t entity,
    ecs_type_t select)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    ecs_entity_t ecs_typeid(EcsMetaTypeSerializer) = 
        ecs_lookup_fullpath(world, "flecs.meta.MetaTypeSerializer");
    ecs_assert(ecs_typeid(EcsMetaTypeSerializer) != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_strbuf_list_push(&str, "{", ",");

    /* Serialize type */
    ecs_type_t type = ecs_get_type(world, entity);
    ecs_strbuf_list_appendstr(&str, "\"type\":");
    serialize_type(world, type, &str);

    /* Serialize entity id */
    ecs_strbuf_list_append(&str, "\"entity\":%d", (int32_t)entity);

    /* Serialize entity path */
    char *path = ecs_get_fullpath(world, entity);
    ecs_strbuf_list_append(&str, "\"path\":\"%s\"", path);
    ecs_os_free(path);

    /* Serialize data */
    if (type) {
        ecs_strbuf_list_appendstr(&str, "\"data\":");
        ecs_strbuf_list_push(&str, "{", ",");

        int i, count = ecs_vector_count(type);
        ecs_entity_t *comps = ecs_vector_first(type, ecs_entity_t);
        for (i = 0; i < count; i ++) {
            if (select) {
                if (!ecs_type_has_entity(world, select, comps[i])) {
                    continue;
                }
            }

            const EcsMetaTypeSerializer *ser = ecs_get(
                    world, comps[i], EcsMetaTypeSerializer);
                
            /* Don't serialize if there's no metadata for component */
            if (!ser) {
                continue;
            }
            
            char *comp_path = ecs_get_fullpath(world, comps[i]);
            ecs_strbuf_list_append(&str, "\"%s\":", comp_path);
            ecs_os_free(comp_path);

            const void *comp_ptr = ecs_get_w_entity(world, entity, comps[i]);
            ecs_assert(comp_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

            json_ser_type(world, ser->ops, comp_ptr, &str);
        }

        ecs_strbuf_list_pop(&str, "}");
    }

    ecs_strbuf_list_pop(&str, "}");

    return ecs_strbuf_get(&str);
}