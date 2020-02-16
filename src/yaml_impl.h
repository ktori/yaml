/*
 * Created by victoria on 16.02.20.
*/

#pragma once

#include "callbacks.h"

struct yaml_s
{
	unsigned last_indent;
	struct yaml_callbacks_s callbacks;
};
