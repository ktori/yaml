/*
 * Created by victoria on 17.02.20.
*/

#include "document.h"
#include "../callbacks.h"
#include "../yaml.h"
#include "map.h"
#include "kv.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define KV_INDEX_UNDEFINED ((size_t) -1)

struct document_load_ctx_s
{
	struct yaml_document_s *document;
	struct yaml_map_s *target_map;
	struct yaml_sequence_s *target_sequence;
	size_t kv_index;

	struct document_load_ctx_s *top;
	struct document_load_ctx_s *before;
};

static void
yd_key_callback(const char *key, size_t size, struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;
	struct document_load_ctx_s *before;

	printf("(debug) key %.*s\n", (int) size, key);

	if (top->target_map == NULL)
	{
		if (top->target_sequence != NULL)
		{
			before = ctx->top;
			top = calloc(1, sizeof(struct document_load_ctx_s));
			top->before = before;
			if (before->target_sequence->count < top->kv_index)
			{
				fprintf(stderr, "kv out of bounds\n");
				return;
			}

			before->target_sequence->values[before->kv_index].type = YVT_MAP;
			top->target_map = &before->target_sequence->values[before->kv_index].body.map;
			yaml_map_init(top->target_map);
		}
		else
		{
			before = top->before;
			if (top->before == NULL)
			{
				if (ctx->document->root.type != YVT_NULL)
				{
					fprintf(stderr, "unexpected key: %.*s\n", (int) size, key);
					return;
				}

				ctx->document->root.type = YVT_MAP;
				yaml_map_init(&ctx->document->root.body.map);
				top->target_map = &ctx->document->root.body.map;
			}
			else if (before->target_map != NULL && before->kv_index != KV_INDEX_UNDEFINED &&
					 before->target_map->kv_array[before->kv_index].value.type == YVT_NULL)
			{
				before->target_map->kv_array[before->kv_index].value.type = YVT_MAP;
				yaml_map_init(&before->target_map->kv_array[before->kv_index].value.body.map);
				top->target_map = &before->target_map->kv_array[before->kv_index].value.body.map;
			}
			else if (before->target_sequence != NULL && before->kv_index != KV_INDEX_UNDEFINED &&
					 before->target_sequence->values[before->kv_index].type == YVT_NULL)
			{
				before->target_sequence->values[before->kv_index].type = YVT_MAP;
				yaml_map_init(&before->target_sequence->values[before->kv_index].body.map);
				top->target_map = &before->target_sequence->values[before->kv_index].body.map;
			}
			else
			{
				fprintf(stderr, "unexpected key: %.*s\n", (int) size, key);
				return;
			}
		}
	}

	yaml_kv_add(top->target_map, &top->kv_index);
	yaml_value_string(&top->target_map->kv_array[top->kv_index].key, key, size);
}

static void
yd_value_callback(const char *value, size_t size, struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;

	printf("(debug) value %.*s\n", (int) size, value);

	if (top->target_map != NULL)
	{
		if (top->target_map->array_size < top->kv_index)
		{
			fprintf(stderr, "kv out of bounds\n");
			return;
		}

		yaml_value_string(&top->target_map->kv_array[top->kv_index].value, value, size);
		return;
	}
	if (top->target_sequence != NULL)
	{
		if (top->target_sequence->count < top->kv_index)
		{
			fprintf(stderr, "kv out of bounds\n");
			return;
		}

		yaml_value_string(&top->target_sequence->values[top->kv_index], value, size);
		return;
	}

	fprintf(stderr, "no target\n");
}

static void
yd_indent_callback(unsigned next, unsigned current, struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;

	printf("(debug) indent %u -> %u\n", current, next);

	if (current > next)
	{
		if (top->before == NULL)
		{
			fprintf(stderr, "ctx underflow\n");
			return;
		}
		ctx->top = top->before;
		free(top);
	}
	else
	{
		top = calloc(1, sizeof(struct document_load_ctx_s));
		top->before = ctx->top;
		ctx->top = top;
	}
}

static void
yd_sequence_entry_callback(struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;
	struct document_load_ctx_s *before;

	printf("(debug) sequence_entry\n");

	if (top->target_sequence == NULL)
	{
		if (top->target_map != NULL)
		{
			fprintf(stderr, "unexpected sequence entry in map\n");
			return;
		}
		before = top->before;
		if (before == NULL)
		{
			if (ctx->document->root.type != YVT_NULL)
			{
				fprintf(stderr, "unexpected sequence\n");
				return;
			}

			ctx->document->root.type = YVT_SEQUENCE;
			yaml_sequence_init(&ctx->document->root.body.sequence);
			top->target_sequence = &ctx->document->root.body.sequence;
		}
		else if (before->target_map != NULL && before->kv_index != KV_INDEX_UNDEFINED &&
				 before->target_map->kv_array[before->kv_index].value.type == YVT_NULL)
		{
			before->target_map->kv_array[before->kv_index].value.type = YVT_SEQUENCE;
			yaml_sequence_init(&before->target_map->kv_array[before->kv_index].value.body.sequence);
			top->target_sequence = &before->target_map->kv_array[before->kv_index].value.body.sequence;
		}
		else if (before->target_sequence != NULL && before->kv_index != KV_INDEX_UNDEFINED &&
				 before->target_sequence->values[before->kv_index].type == YVT_NULL)
		{
			before->target_sequence->values[before->kv_index].type = YVT_SEQUENCE;
			yaml_sequence_init(&before->target_sequence->values[before->kv_index].body.sequence);
			top->target_sequence = &before->target_sequence->values[before->kv_index].body.sequence;
		}
		else
		{
			fprintf(stderr, "unexpected sequence\n");
			return;
		}
	}

	yaml_sequence_push(top->target_sequence, &top->kv_index);
}

int
yaml_document_init(struct yaml_document_s *document)
{
	memset(document, 0, sizeof(struct yaml_document_s));
	document->ctx = calloc(1, sizeof(struct document_load_ctx_s));
	document->ctx->document = document;
	document->ctx->kv_index = KV_INDEX_UNDEFINED;
	document->ctx->top = document->ctx;

	return EXIT_SUCCESS;
}

int
yaml_document_bind(struct yaml_document_s *document, struct yaml_s *yaml)
{
	struct yaml_callbacks_s callbacks = {0};

	callbacks.key = (void (*)(const char *, size_t, void *)) yd_key_callback;
	callbacks.value = (void (*)(const char *, size_t, void *)) yd_value_callback;
	callbacks.indent = (void (*)(unsigned int, unsigned int, void *)) yd_indent_callback;
	callbacks.sequence_entry = (void (*)(void *)) yd_sequence_entry_callback;

	yaml_set_callbacks(yaml, &callbacks);

	return EXIT_SUCCESS;
}

int
yaml_document_destroy(struct yaml_document_s *document)
{
	free(document->ctx);

	return EXIT_SUCCESS;
}
