/*
 * Created by victoria on 17.02.20.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/document/document.h"
#include "../src/document/map.h"
#include "../src/yaml.h"
#include "../src/document/value.h"

#define MAX_LINE_LENGTH 1024

static const char *YAML_TYPE_NAMES[] = {
		"null",
		"map",
		"list",
		"string",
		"int",
		"float",
		"bool",
		"binary",
		NULL
};

static void
visit_yaml_value(struct yaml_value_s *value, int depth)
{
	int i, k;
	struct yaml_kv_s *kv_i;

	for (i = 0; i < depth; ++i)
		putc(' ', stdout);

	printf("type: %s", YAML_TYPE_NAMES[value->type]);

	switch (value->type)
	{
		case YVT_MAP:
			putc('\n', stdout);

			for (k = 0; k < value->body.map->array_size; ++k)
			{
				for (i = 0; i < depth; ++i)
					putc(' ', stdout);
				printf("key:\n");
				visit_yaml_value(&value->body.map->kv_array[k].key, depth + 2);

				for (i = 0; i < depth; ++i)
					putc(' ', stdout);
				printf("value:\n");
				visit_yaml_value(&value->body.map->kv_array[k].key, depth + 2);
			}
			break;
		case YVT_STRING:
			printf(", value: '%s'\n", value->body.string);
			break;
		case YVT_INT:
			printf(", value: %d\n", value->body.integer);
			break;
		case YVT_FLOAT:
			printf(", value: %f\n", value->body.floating);
			break;
		case YVT_BOOL:
			printf(", value: %s\n", value->body.boolean ? "true" : "false");
			break;
		default:
			putc('\n', stdout);
			break;
	}
}

int
main(int argc, const char **argv)
{
	char buffer[MAX_LINE_LENGTH];
	struct yaml_document_s document;
	struct yaml_s *yaml;
	FILE *input;
	int exit_status = EXIT_SUCCESS;

	if (argc < 2)
		input = fopen("loadme.yaml", "r");
	else
		input = fopen(argv[1], "r");

	if (input == NULL)
	{
		perror("fopen()");
		return EXIT_FAILURE;
	}

	yaml_create(&yaml);
	yaml_document_init(&document);
	yaml_document_bind(&document, yaml);

	while (fgets(buffer, MAX_LINE_LENGTH, input))
	{
		if (yaml_in(yaml, buffer, strlen(buffer), &document) != EXIT_SUCCESS)
		{
			fprintf(stderr, "YAML error\n");
			exit_status = EXIT_FAILURE;
			break;
		}
	}

	printf("YAML document loaded\n");
	visit_yaml_value(&document.root, 0);

	yaml_document_destroy(&document);
	yaml_free(&yaml);

	return exit_status;
}
