/*
 * Created by victoria on 17.02.20.
*/

#include "yaml/document/sequence.h"

#include "yaml/document/value.h"

#include <stdlib.h>
#include <string.h>

int
yaml_sequence_init(struct yaml_sequence_s *sequence)
{
	memset(sequence, 0, sizeof(struct yaml_sequence_s));

	sequence->size = 2;
	sequence->values = calloc(sequence->size, sizeof(struct yaml_value_s));

	return EXIT_SUCCESS;
}

int
yaml_sequence_push(struct yaml_sequence_s *sequence, size_t *out_index)
{
	if (sequence->count == sequence->size)
	{
		sequence->size *= 2;
		sequence->values = realloc(sequence->values, sequence->size * sizeof(struct yaml_value_s));
		memset(sequence->values + sequence->count, 0, (sequence->size - sequence->count) * sizeof(struct yaml_value_s));
	}
	else
		memset(sequence->values + sequence->count, 0, sizeof(struct yaml_value_s));

	*out_index = sequence->count++;

	return EXIT_SUCCESS;
}

int
yaml_sequence_destroy(struct yaml_sequence_s *sequence)
{
	size_t i;
	for (i = 0; i < sequence->count; ++i)
		yaml_value_free(sequence->values + i);

	free(sequence->values);
	memset(sequence, 0, sizeof(struct yaml_sequence_s));

	return EXIT_SUCCESS;
}
