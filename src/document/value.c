/*
 * Created by victoria on 17.02.20.
*/

#include "value.h"

#include <malloc.h>
#include <string.h>

int
yaml_value_string(struct yaml_value_s *value, const char *string, size_t length)
{
	char *buffer = malloc(length + 1);
	strncpy(buffer, string, length);
	buffer[length] = 0;

	value->type = YVT_STRING;
	value->body.string = buffer;

	return 0;
}

int
yaml_value_free(struct yaml_value_s *value)
{
	switch (value->type)
	{
		case YVT_STRING:
			free((void *) value->body.string);
			value->body.string = NULL;
			break;
		case YVT_MAP:
			yaml_map_destroy(&value->body.map);
			break;
		case YVT_SEQUENCE:
			yaml_sequence_destroy(&value->body.sequence);
			break;
		default:
			break;
	}

	memset(&value->body, 0, sizeof(value->body));
	value->type = YVT_NULL;

	return 0;
}

