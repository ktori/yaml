/*
 * Created by victoria on 17.02.20.
*/

#pragma once

#include <stddef.h>

#include "value.h"

struct yaml_kv_s
{
	struct yaml_value_s key;
	struct yaml_value_s value;
};

struct yaml_map_s
{
	struct yaml_kv_s *kv_array;
	size_t array_size;
	size_t array_capacity;
};
