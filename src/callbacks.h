/*
 * Created by victoria on 16.02.20.
*/

#pragma once

#include <stddef.h>

struct yaml_callbacks_s
{
	void
	(*key)(const char *start, size_t length, void *user);

	void
	(*value)(const char *start, size_t length, void *user);
};
