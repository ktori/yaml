/*
 * Created by victoria on 17.02.20.
*/

#pragma once

#include <stddef.h>

struct yaml_sequence_s
{
	struct yaml_value_s *values;
	size_t count;
	size_t size;
};

int
yaml_sequence_init(struct yaml_sequence_s *sequence);

int
yaml_sequence_push(struct yaml_sequence_s *sequence, size_t *out_index);

int
yaml_sequence_destroy(struct yaml_sequence_s *sequence);
