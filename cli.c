/* Copyright 2014 Drew Thoreson
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "mcp3008.h"

#define SPI_BUS_MAX 0
#define SPI_CS_MAX  1

static char inputs[8] = {0};
static int spi_bus = 0;
static int spi_cs  = 0;
static int verbose = 0;

static struct option long_options[] = {
	{ "all",         no_argument,       0, 'a' }, 
	{ "bus",         required_argument, 0, 'b' },
	{ "chip-select", required_argument, 0, 'c' },
	{ "help",        no_argument,       0, 'h' },
	{ "input",       required_argument, 0, 'i' },
	{ "verbose",     no_argument,       0, 'v' },
	{ 0, 0, 0, 0 }
};

static void usage(int status)
{
	fprintf(stderr, "usage: mcp3008 [option...]\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t-a,--all              Read all MCP3008 inputs\n");
	fprintf(stderr, "\t-b,--bus <bus>        Use the SPI bus <bus>\n");
	fprintf(stderr, "\t-c,--chip-select <cs> Use the SPI chip-select <cs>\n");
	fprintf(stderr, "\t-h,--help             Display this message and exit\n");
	fprintf(stderr, "\t-i,--input <channel>  Use the MCP3008 input <channel>\n");
	fprintf(stderr, "\t-v,--verbose          Print extra information at runtime\n");
	exit(status);
}

static int parse_int(const char *str, int min, int max)
{
	char *endptr = NULL;
	long chan = strtol(str, &endptr, 10);
	if (chan < min || chan > max || (endptr && *endptr != '\0'))
		usage(EXIT_FAILURE);
	return chan;
}

void parse_opts(int argc, char *argv[])
{
	int explicit_inputs = 0;
	for (;;) {
		int options_index = 0;
		int c = getopt_long(argc, argv, "ab:c:hi:v", long_options, &options_index);

		if (c < 0)
			break;
		switch (c) {
		case 'a':
			for (int i = 0; i < 8; i++)
				inputs[i] = 1;
			break;
		case 'b':
			spi_bus = parse_int(optarg, 0, SPI_BUS_MAX);
			break;
		case 'c':
			spi_cs = parse_int(optarg, 0, SPI_CS_MAX);
			break;
		case 'h':
			usage(EXIT_SUCCESS);
		case 'i':
			inputs[parse_int(optarg, 0, 7)] = 1;
			explicit_inputs = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		case '?':
			break;
		default:
			usage(EXIT_FAILURE);
		}
	}
	/* read input 0 by default */
	if (!explicit_inputs)
		inputs[0] = 1;
}

static void print_level(int spi_fd, int channel)
{
	int level = mcp3008_read(spi_fd, channel);
	if (level < 0) {
		perror("mcp3008_read");
		exit(EXIT_FAILURE);
	}
	if (verbose)
		printf("channel %d: ", channel);
	printf("%d\n", level);
}

int main(int argc, char *argv[])
{
	int fd;
	char spi_dev[20];

	parse_opts(argc, argv);

	sprintf(spi_dev, "/dev/spidev%d.%d", spi_bus, spi_cs);

	if ((fd = mcp3008_open(spi_dev)) < 0) {
		perror("mcp3008_open");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < 8; i++) {
		if (inputs[i])
			print_level(fd, i);
	}

	return 0;
}
