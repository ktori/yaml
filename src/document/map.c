/*
 * Created by victoria on 17.02.20.
*/

#include "map.h"
#include "kv.h"

#include <string.h>
#include <malloc.h>

int
yaml_map_init(struct yaml_map_s *map)
{
	memset(map, 0, sizeof(struct yaml_map_s));

	map->array_capacity = 4;
	map->kv_array = calloc(map->array_capacity, sizeof(struct yaml_kv_s));

	return 0;
}

int
yaml_map_destroy(struct yaml_map_s *map)
{
	size_t i;

	for (i = 0; i < map->array_size; ++i)
	{
		yaml_value_free(&map->kv_array[i].key);
		yaml_value_free(&map->kv_array[i].value);
	}

	free(map->kv_array);
	map->kv_array = NULL;
	map->array_size = 0;
	map->array_capacity = 0;

	return 0;
}

int
yaml_kv_add(struct yaml_map_s *map, size_t *out_idx)
{
	*out_idx = map->array_size++;
	if (map->array_size > map->array_capacity)
	{
		map->array_capacity *= 2;
		map->kv_array = realloc(map->kv_array, map->array_capacity * sizeof(struct yaml_kv_s));
		memset(&map->kv_array[map->array_size], 0, (map->array_capacity - map->array_size) * sizeof(struct yaml_kv_s));
	}

	memset(map->kv_array + *out_idx, 0, sizeof(struct yaml_kv_s));

	return 0;
}
