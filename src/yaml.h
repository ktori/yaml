/*
 * Created by victoria on 16.02.20.
*/

#pragma once

#include <stddef.h>

struct yaml_s;

int
yaml_create(struct yaml_s **yaml);

int
yaml_in(struct yaml_s *yaml, const char *line, size_t length);

int
yaml_free(struct yaml_s **yaml);
