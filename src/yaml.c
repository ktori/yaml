/*
 * Created by victoria on 16.02.20.
*/

#include "yaml.h"
#include "yaml_impl.h"

#include <stdlib.h>
#include <string.h>

int
yaml_in(struct yaml_s *yaml, const char *line, size_t length, void *user)
{
	enum state
	{
		YS_ERROR,
		YS_KEY,
		YS_KV_SEP,
		YS_WS,
		YS_VALUE,
		YS_DONE
	};

	const char *i = line, *end = line + length;
	const char *value_begin = NULL;
	int consume;
	enum state state = YS_KEY;

	while (i < end && state != YS_ERROR && state != YS_DONE)
	{
		consume = 0;

		switch (state)
		{
			case YS_KEY:
				if (*i == ':')
				{
					if (yaml->callbacks.key)
						yaml->callbacks.key(line, i - line, user);
					state = YS_KV_SEP;
				}
				else
					consume = 1;
				break;
			case YS_KV_SEP:
				state = YS_WS;
				consume = 1;
				break;
			case YS_WS:
				if (*i != ' ')
					state = YS_VALUE;
				else
					consume = 1;
				break;
			case YS_VALUE:
				if (value_begin == NULL)
					value_begin = i;
				if (*i == '\r' || *i == '\n')
				{
					if (yaml->callbacks.value)
						yaml->callbacks.value(value_begin, i - value_begin, user);
					state = YS_DONE;
				}
				else if (i == end - 1)
				{
					if (yaml->callbacks.value)
						yaml->callbacks.value(value_begin, i - value_begin + 1, user);
					state = YS_DONE;
				}
				consume = 1;
				break;
			default:
				break;
		}

		if (consume)
			++i;
	}

	if (state == YS_ERROR)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int
yaml_create(struct yaml_s **yaml)
{
	*yaml = calloc(1, sizeof(struct yaml_s));

	return EXIT_SUCCESS;
}

int
yaml_free(struct yaml_s **yaml)
{
	free(*yaml);
	*yaml = NULL;

	return EXIT_SUCCESS;
}

int
yaml_set_callbacks(struct yaml_s *yaml, struct yaml_callbacks_s *callbacks)
{
	memcpy(&yaml->callbacks, callbacks, sizeof(struct yaml_callbacks_s));

	return EXIT_SUCCESS;
}
