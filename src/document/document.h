/*
 * Created by victoria on 17.02.20.
*/

#pragma once

#include "value.h"

#include <stddef.h>

struct yaml_s;

struct yaml_document_s
{
	struct yaml_value_s root;
};

int
yaml_document_init(struct yaml_document_s *document);

int
yaml_document_bind(struct yaml_document_s *document, struct yaml_s *yaml);

int
yaml_document_destroy(struct yaml_document_s *document);
