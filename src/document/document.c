/*
 * Created by victoria on 17.02.20.
*/

#include "document.h"
#include "../callbacks.h"
#include "../yaml.h"

#include <stdlib.h>
#include <string.h>

static void
yd_key_callback(const char *key, size_t size, struct yaml_document_s *document)
{

}

static void
yd_value_callback(const char *key, size_t size, struct yaml_document_s *document)
{

}

int
yaml_document_init(struct yaml_document_s *document)
{
	memset(document, 0, sizeof(struct yaml_document_s));

	return EXIT_SUCCESS;
}

int
yaml_document_bind(struct yaml_document_s *document, struct yaml_s *yaml)
{
	struct yaml_callbacks_s callbacks = {0};

	callbacks.key = (void (*)(const char *, size_t, void *)) yd_key_callback;
	callbacks.value = (void (*)(const char *, size_t, void *)) yd_value_callback;

	yaml_set_callbacks(yaml, &callbacks);

	return EXIT_SUCCESS;
}

int
yaml_document_destroy(struct yaml_document_s *document)
{
	return EXIT_SUCCESS;
}
