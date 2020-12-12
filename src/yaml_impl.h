/*
 * Created by victoria on 16.02.20.
*/

#pragma once

#include "../include/yaml/callbacks.h"

enum yaml_context_type
{
	YCT_OBJECT,
	YCT_LIST
};

struct yaml_s
{
	struct yaml_callbacks_s callbacks;
	struct yaml_context_s *context_stack;
	size_t stack_size;
	size_t stack_capacity;
};

struct yaml_context_s
{
	unsigned indent;
	enum yaml_context_type type;
};

void
yaml_context_push(struct yaml_s *yaml, struct yaml_context_s ctx);
