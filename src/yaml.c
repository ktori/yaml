/*
 * Created by victoria on 16.02.20.
*/

#include "yaml.h"
#include "yaml_impl.h"

#include <stdlib.h>

int
yaml_in(struct yaml_s *yaml, const char *line, size_t length)
{
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
