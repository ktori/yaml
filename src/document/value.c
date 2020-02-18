/*
 * Created by victoria on 17.02.20.
*/

#include "value.h"

#include <malloc.h>
#include <string.h>
#include <ctype.h>

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

static int
yaml_strtoi(const char *string, size_t length, int *out_value)
{
	const char *i = string, *j, *end = string + length;
	int base = 10, base_acc = 1;

	if (length == 0)
		return 1;

	if (*i == '-')
	{
		i++;
		base_acc = -1;
	}

	if (i < end && *i == '0')
	{
		++i;
		if (i < end)
		{
			if (*i == 'x')
			{
				++i;
				base = 16;
			}
			else if (isdigit(*i))
			{
				base = 8;
			}
			else
			{
				return 1;
			}
		}
		else
		{
			*out_value = 0;
			return 0;
		}
	}

	*out_value = 0;

	/* TODO: overflow check */
	for (j = end - 1; j >= i; --j)
	{
		if (!isdigit(*j))
			return 1;
		*out_value += (*j - '0') * base_acc;
		base_acc *= base;
	}

	return 0;
}

int
yaml_unquote(const char *string, size_t length, char **out_buffer, size_t *out_length)
{
	const char *i = string, *last = string + length - 1;
	char *w;
	int single_quote, was_escape = 0;

	if (length < 2)
		return 1;

	if (*i == '\'' && *last == '\'')
		single_quote = 1;
	else if (*i == '\"' && *last == '\'')
		single_quote = 0;
	else
		return 1;

	*out_buffer = calloc(length - 1, sizeof(char));
	*out_length = 0;

	for (w = *out_buffer; i < last; ++i)
	{
		if (!single_quote)
		{
			if (*i == '\\' && !was_escape)
			{
				was_escape = 1;
				continue;
			}
		}
		else
		{
			if (*i == '\'' && !was_escape)
			{
				was_escape = 1;
				continue;
			}
		}

		if (!single_quote && was_escape)
		{
			switch (*i)
			{
				case 'n':
					*w = '\n';
					break;
				case 't':
					*w = '\t';
					break;
				/* TODO */
			}
		}
		else
			*w = *i;

		++w;
		was_escape = 0;
	}

	*w = 0;
	*out_length = w - *out_buffer;

	return 0;
}

int
yaml_value_from_string(struct yaml_value_s *value, const char *string, size_t length)
{
	int out_int;
	char *out_str;
	size_t out_str_len;

	if (value->type != YVT_NULL)
		return 1;

	if (yaml_strtoi(string, length, &out_int) == 0)
	{
		value->type = YVT_INT;
		value->body.integer = out_int;
		return 0;
	}
	if (yaml_unquote(string, length, &out_str, &out_str_len) == 0)
	{
		value->type = YVT_STRING;
		value->body.string = out_str;
		return 0;
	}

	return yaml_value_string(value, string, length);
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

