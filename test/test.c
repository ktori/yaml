/*
 * Created by victoria on 16.02.20.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/yaml.h"

#define MAX_LINE_LENGTH 1024

int
main(int argc, const char **argv)
{
	FILE *input;
	struct yaml_s *yaml;
	char buffer[MAX_LINE_LENGTH];

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

	while (fgets(buffer, MAX_LINE_LENGTH, input))
	{
		yaml_in(yaml, buffer, strlen(buffer));
	}

	yaml_free(&yaml);

	return EXIT_SUCCESS;
}
