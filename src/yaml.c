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
		YS_INDENT,
		YS_KEY,
		YS_KV_SEP,
		YS_WS,
		YS_VALUE,
		YS_DONE
	};

	const char *i = line, *end = line + length;
	const char *key_begin = NULL, *value_begin = NULL, *ows_end = NULL;
	int consume, eol = 0, comment = 0, inc_eol;
	enum state state = YS_INDENT;
	unsigned ex_indent, acc_indent = 0;

	while (i < end && !eol && state != YS_ERROR && state != YS_DONE)
	{
		consume = 0;
		comment = *i == '#';
		inc_eol = comment || *i == '\r' || *i == '\n';
		eol = i == end - 1 || inc_eol;

		if (eol)
		{
			switch (state)
			{
				case YS_INDENT:
				case YS_WS:
					state = YS_DONE;
					break;
				case YS_VALUE:
					if (inc_eol)
					{
						if (yaml->callbacks.value)
							yaml->callbacks.value(value_begin, (ows_end ? ows_end + 1 : i) - value_begin, user);
						state = YS_DONE;
					}
					else
					{
						if (yaml->callbacks.value)
							yaml->callbacks.value(value_begin, (ows_end ? ows_end + 1 : i) - value_begin + 1, user);
						state = YS_DONE;
					}
					break;
				default:
					state = YS_ERROR;
					break;
			}
			break;
		}

		switch (state)
		{
			case YS_INDENT:
				if (*i == ' ')
				{
					++acc_indent;
					consume = 1;
				}
				else
				{
					if (yaml->stack_size == 0 ||
						(ex_indent = yaml->context_stack[yaml->stack_size].indent) != acc_indent)
					{
						if (yaml->stack_size == 0 || acc_indent > ex_indent)
						{
							struct yaml_context_s ctx = {0};
							ctx.indent = acc_indent;
							yaml_context_push(yaml, ctx);
							if (yaml->callbacks.indent && yaml->stack_size > 1)
								yaml->callbacks.indent(acc_indent, ex_indent, user);
						}
						else
						{
							yaml->stack_size--;
							if (yaml->stack_size && yaml->context_stack[yaml->stack_size - 1].indent > acc_indent)
							{
								/* previous indent value was larger */
								state = YS_ERROR;
								break;
							}
							if (yaml->callbacks.indent)
								yaml->callbacks.indent(acc_indent, ex_indent, user);
						}
					}
					key_begin = i;
					ows_end = NULL;
					state = YS_KEY;
				}
				break;
			case YS_KEY:
				if (*i == ':')
					state = YS_KV_SEP;
				else
				{
					consume = 1;
					if (*i != ' ')
						ows_end = i;
				}
				break;
			case YS_KV_SEP:
				if (yaml->callbacks.key)
					yaml->callbacks.key(key_begin, (ows_end ? ows_end + 1 : i) - key_begin, user);
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
				{
					value_begin = i;
					ows_end = NULL;
				}
				if (*i != ' ')
					ows_end = i;
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
	(*yaml)->stack_capacity = 4;
	(*yaml)->context_stack = calloc((*yaml)->stack_capacity, sizeof(struct yaml_context_s));

	return EXIT_SUCCESS;
}

int
yaml_free(struct yaml_s **yaml)
{
	free((*yaml)->context_stack);
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

void
yaml_context_push(struct yaml_s *yaml, struct yaml_context_s ctx)
{
	yaml->stack_size += 1;
	if (yaml->stack_size > yaml->stack_capacity)
	{
		yaml->stack_capacity *= 2;
		yaml->context_stack = realloc(yaml->context_stack, yaml->stack_capacity * sizeof(struct yaml_context_s));
		memset(yaml->context_stack + yaml->stack_size, 0,
			   (yaml->stack_capacity - yaml->stack_size) * sizeof(struct yaml_context_s));
	}
	yaml->context_stack[yaml->stack_size] = ctx;
}
