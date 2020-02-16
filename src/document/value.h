/*
 * Created by victoria on 17.02.20.
*/

#pragma once

enum yaml_value_type
{
	YVT_NULL,
	YVT_MAP,
	YVT_LIST,
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
		struct yaml_map_s *map;
		int integer;
		float floating;
		int boolean;
		const void *binary;
	} body;
};
