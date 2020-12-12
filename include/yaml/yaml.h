/*
 * Created by victoria on 16.02.20.
*/

#pragma once

#include <stddef.h>

struct yaml_s;
struct yaml_callbacks_s;

int
yaml_create(struct yaml_s **yaml);

int
yaml_set_callbacks(struct yaml_s *yaml, struct yaml_callbacks_s *callbacks);

int
yaml_in(struct yaml_s *yaml, const char *line, size_t length, void *user);

int
yaml_free(struct yaml_s **yaml);
