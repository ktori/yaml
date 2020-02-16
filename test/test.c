/*
 * Created by victoria on 16.02.20.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/yaml.h"
#include "../src/callbacks.h"

#define MAX_LINE_LENGTH 1024

static void
key_callback(const char *begin, size_t size, void *user)
{
	printf("key: %.*s\n", (int) size, begin);
}

static void
value_callback(const char *begin, size_t size, void *user)
{
	printf("value: %.*s\n", (int) size, begin);
}

static void
indent_callback(unsigned indent, void *user)
{
	printf("indent: %u\n", indent);
}

int
main(int argc, const char **argv)
{
	FILE *input;
	struct yaml_s *yaml;
	char buffer[MAX_LINE_LENGTH];
	struct yaml_callbacks_s callbacks = {0};
	callbacks.key = key_callback;
	callbacks.value = value_callback;
	callbacks.indent = indent_callback;

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
	yaml_set_callbacks(yaml, &callbacks);

	while (fgets(buffer, MAX_LINE_LENGTH, input))
	{
		yaml_in(yaml, buffer, strlen(buffer), NULL);
	}

	yaml_free(&yaml);

	return EXIT_SUCCESS;
}
