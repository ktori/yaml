/*
 * Created by victoria on 16.02.20.
*/

#include "yaml.h"
#include "yaml_impl.h"

#include <stdlib.h>
#include <string.h>

#define YF_EOL 0x1u
#define YF_EOL_INC 0x2u
#define YF_CONSUME 0x4u
#define YF_QUOTE_SWITCH 0x8u
#define YF_NO_QUOTE 0x10u

int
yaml_in(struct yaml_s *yaml, const char *line, size_t length, void *user)
{
	enum state
	{
		YS_ERROR,
		YS_INDENT,
		YS_ENTRY,
		YS_KEY,
		YS_KV_SEP,
		YS_WS,
		YS_VALUE,
		YS_DONE
	};

	const char *i = line, *end = line + length;
	const char *key_begin = NULL, *value_begin = NULL, *ows_end = NULL;
	unsigned flags = 0;
	int is_quote = 0, is_single_quote = 0, was_single_quote = 0;
	enum state state = YS_INDENT, next_state = YS_ERROR;
	unsigned ex_indent, acc_indent = 0;

	while (i < end && !(flags & YF_EOL) && state != YS_ERROR && state != YS_DONE)
	{
		flags = 0;

		if (*i == '\'' && !was_single_quote && !is_quote)
		{
			flags |= YF_QUOTE_SWITCH | YF_CONSUME;
			is_single_quote = !is_single_quote;
		}
		if (*i == '\"' && !is_single_quote)
		{
			flags |= YF_QUOTE_SWITCH | YF_CONSUME;
			is_quote = !is_quote;
		}

		if (!(is_quote || is_single_quote))
			flags |= YF_NO_QUOTE;

		if ((*i == '#' && flags & YF_NO_QUOTE) || *i == '\r' || *i == '\n')
			flags |= YF_EOL_INC;
		if (i == end - 1 || flags & YF_EOL_INC)
			flags |= YF_EOL;

		if (flags & YF_EOL)
		{
			switch (state)
			{
				case YS_INDENT:
				case YS_WS:
					state = YS_DONE;
					break;
				case YS_VALUE:
					if (flags & YF_EOL_INC)
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
					flags |= YF_CONSUME;
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
					state = YS_ENTRY;
				}
				break;
			case YS_ENTRY:
				if (*i == '-')
				{
					flags |= YF_CONSUME;
					state = YS_WS;
					next_state = YS_VALUE;
					if (yaml->callbacks.sequence_entry)
						yaml->callbacks.sequence_entry(user);
				}
				else
					state = YS_KEY;
				break;
			case YS_KEY:
				if (*i == ':' && (flags & YF_NO_QUOTE))
					state = YS_KV_SEP;
				else
				{
					flags |= YF_CONSUME;
					if (*i != ' ')
						ows_end = i;
				}
				break;
			case YS_KV_SEP:
				if (yaml->callbacks.key)
					yaml->callbacks.key(key_begin, (ows_end ? ows_end + 1 : i) - key_begin, user);
				state = YS_WS;
				next_state = YS_VALUE;
				flags |= YF_CONSUME;
				break;
			case YS_WS:
				if (*i != ' ')
					state = next_state;
				else
					flags |= YF_CONSUME;
				break;
			case YS_VALUE:
				if (value_begin == NULL)
				{
					value_begin = i;
					ows_end = NULL;
				}
				if (*i != ' ')
					ows_end = i;
				flags |= YF_CONSUME;
				break;
			default:
				break;
		}

		was_single_quote = *i == '\'';

		if (flags & YF_CONSUME || was_single_quote)
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
