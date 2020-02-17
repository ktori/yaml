/*
 * Created by victoria on 17.02.20.
*/

#pragma once

#include <stddef.h>

struct yaml_map_s
{
	struct yaml_kv_s *kv_array;
	size_t array_size;
	size_t array_capacity;
};

int
yaml_map_init(struct yaml_map_s *map);

int
yaml_map_destroy(struct yaml_map_s *map);

int
yaml_kv_add(struct yaml_map_s *map, size_t *out_idx);
