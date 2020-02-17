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
	size_t kv_index;

	struct document_load_ctx_s *top;
	struct document_load_ctx_s *before;
};

static void
yd_key_callback(const char *key, size_t size, struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;
	struct document_load_ctx_s *before;

	if (top->target_map == NULL)
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
		else
		{
			fprintf(stderr, "unexpected key: %.*s\n", (int) size, key);
			return;
		}
	}

	yaml_kv_add(top->target_map, &top->kv_index);
	yaml_value_string(&top->target_map->kv_array[top->kv_index].key, key, size);
}

static void
yd_value_callback(const char *value, size_t size, struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;

	if (top->target_map == NULL)
	{
		fprintf(stderr, "no target map\n");
		return;
	}
	if (top->target_map->array_size < top->kv_index)
	{
		fprintf(stderr, "kv out of bounds\n");
		return;
	}

	yaml_value_string(&top->target_map->kv_array[top->kv_index].value, value, size);
}

static void
yd_indent_callback(unsigned next, unsigned current, struct document_load_ctx_s *ctx)
{
	struct document_load_ctx_s *top = ctx->top;

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

	yaml_set_callbacks(yaml, &callbacks);

	return EXIT_SUCCESS;
}

int
yaml_document_destroy(struct yaml_document_s *document)
{
	free(document->ctx);

	return EXIT_SUCCESS;
}
