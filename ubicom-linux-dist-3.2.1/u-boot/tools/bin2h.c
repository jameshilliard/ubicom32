/*
 * bin2h.c
 *	Creates a header file given a binary file.
 *
 * (C) Copyright 2010
 * Ubicom, Inc. www.ubicom.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	unsigned char *buf;
	FILE *fin;
	FILE *fout;
	int cnt;

	if (argc < 4) {
		printf("Usage: %s <binary file> <h file> <variable name>\n", argv[0]);
		return 1;
	}

	buf = malloc(4096);
	if (!buf) {
		printf("Could not alloc memory\n");
		return 1;
	}

	fin = fopen(argv[1], "rb");
	if (!fin) {
		printf("Could not open '%s' for reading\n", argv[1]);
		free(buf);
		return 1;
	}
	fout = fopen(argv[2], "wt");
	if (!fout) {
		printf("Could not open '%s' for writing\n", argv[2]);
		free(buf);
		fclose(fin);
		return 1;
	}


	fprintf(fout, "/* GENERATED FILE from %s */\n", argv[1]);
	fprintf(fout, "const unsigned char %s[] = {\n\t", argv[3]);

	cnt = 0;
	while (!feof(fin)) {
		int i;
		int res = fread(buf, 1, 4096, fin);
		if (res < 0) {
			break;
		}

		for (i = 0; i < res; i++) {
			fprintf(fout, "0x%02x, ", buf[i]);
			cnt++;
			if ((cnt % 32) == 0) {
				fprintf(fout, "\n\t");
			}
		}
	}
	fprintf(fout, "\n};\n");

	free(buf);
	fclose(fin);
	fclose(fout);
	return 0;
}

