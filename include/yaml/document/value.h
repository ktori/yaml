/*
 * Created by victoria on 17.02.20.
*/

#pragma once

#include "map.h"
#include "sequence.h"

enum yaml_value_type
{
	YVT_NULL,
	YVT_MAP,
	YVT_SEQUENCE,
	YVT_STRING,
	YVT_INT,
	YVT_FLOAT,
	YVT_BOOL,
	YVT_BINARY
};

struct yaml_value_s
{
	enum yaml_value_type type;
	union
	{
		const char *string;
		struct yaml_map_s map;
		struct yaml_sequence_s sequence;
		int integer;
		float floating;
		int boolean;
		const void *binary;
	} body;
};

int
yaml_value_string(struct yaml_value_s *value, const char *string, size_t length);

int
yaml_value_from_string(struct yaml_value_s *value, const char *string, size_t length);

int
yaml_value_free(struct yaml_value_s *value);
