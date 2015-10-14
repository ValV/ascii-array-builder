/*
 * ASCII art to char array converter
 * TODO: Add description
 * Author: Vladimir Valeyev
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

// Variables
volatile int qqq = 1; // TODO: Remove (deprecated)

// Functions prototypes
void main_handler(int); // TODO: Rationalize or remove

int main(int argc, char *argv[]) {
	// Change default application handler
	// with custom one
	void (*ori_handler)(int);
	ori_handler = signal(SIGINT, main_handler);

	// The rest of the program
	int i = 0, j = 0, max = 0;
	char ch = '\0', *infile = NULL, *outfile = NULL;
	FILE *picp = stdin;
	FILE *logo = stdout;

	opterr = 0; // Do not print to stderr (use "?" instead)
	while ((ch = getopt(argc, argv, "i:o:")) != -1) {
		switch(ch) {
			case 'i':
				// Get input file name
				infile = optarg;
				//fprintf(stderr, "DEBUG: Input file: %s.\n", infile);
				//exit(EXIT_SUCCESS);
				break;
			case 'o':
				// Get output file name
				outfile = optarg;
				//fprintf(stderr, "DEBUG: Output file: %s.\n", outfile);
				//exit(EXIT_SUCCESS);
				break;
			case '?':
				// Handle erroneous options
				if ((optopt == 'i') || (optopt == 'o')) {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				} else if (isprint(optopt)) {
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				} else {
					fprintf(stderr, "Unknown character '\\x%X'.\n", optopt);
				}
				exit(EXIT_FAILURE);
				break;
			default:
				//fprintf(stderr, "DEBUG: No options provided - reading stdin.\n");
				exit(EXIT_FAILURE);
		}
	}

	if (infile) {
		errno = 0;
		picp = fopen(infile, "r+b");
		if (errno != 0) {
			perror("fopen (in:read)");
			return (errno);
		}
	}
	if (outfile) {
		errno = 0;
		// NOTE: This will overwrite the file without asking
		logo = fopen(outfile, "w+b");
		if (errno != 0) {
			perror("fopen (out:write)");
			return (errno);
		}
	}

	errno = 0;
	char *instr = malloc(sizeof(char) * (i + 1));
	if (errno != 0) {
		perror("(char *) malloc");
		return (errno);
	}
	*instr = ch = '\0';
	errno = 0;
	char **inbuf = malloc(sizeof(char *) * (i + 1));
	if (errno != 0) {
		perror("(char **) malloc");
		return (errno);
	}

	// Parse the ASCII art
	*inbuf = instr;
	while ((ch = fgetc(picp)) != EOF) {
		if (ch == '\n') {
			j ++;
			errno = 0;
			inbuf = realloc(inbuf, (sizeof(char *) * (j + 1)));
			if (errno != 0) {
				perror("(char **) realloc");
				return (errno);
			}
			i = errno = 0;
			instr = malloc(sizeof(char) * (i + 1));
			if (errno != 0) {
				perror("(char *) malloc");
				return (errno);
			}
			inbuf[j] = instr;
			instr[i] = '\0';
			// Debug (checked: parselation goes well)
			//fprintf(stderr, "%s\n", inbuf[j - 1]);
		} else if (ch != '\r') {
			max = (i >= max) ? i : max; // Maximum ASCII width
			i ++;
			errno = 0;
			// This one tricky step is important:
			// reassign the new reallocated pointer
			inbuf[j] =
			instr = realloc(instr, (sizeof(char) * (i + 1)));
			if (errno != 0) {
				perror("(char *) realloc");
				return (errno);
			}
			instr[i] = '\0';
			instr[i - 1] = ch;
		}
	}

	// Building the array of char for the picture
	int e, k;
	if (*inbuf[j] == '\0') j --;
	fprintf(logo, "char logo[] = {\n\t");
	for (e = k = 0; k <= j; k ++) {
		i = 0;
		while (inbuf[k][i] != '\0') {
			if (i < max) {
				fprintf(logo, "0x%02X,", inbuf[k][i]);
				e ++;
				if ((e % 12) == 0) {
					fprintf(logo, "\n\t");
				} else {
					fprintf(logo, " ");
				}
			}
			if ((i == max) && (k < j)) {
				fprintf(logo, "0x%02X, ", inbuf[k][i]);
				fprintf(logo, "0x%02X,", '\n');
				e += 2;
				if ((e % 12) == 0) {
					fprintf(logo, "\n\t");
				} else {
					fprintf(logo, " ");
				}
			}
			if ((i == max) && (k == j)) {
				fprintf(logo, "0x%02X", inbuf[k][i]);
				e ++;
			}
			i ++;
		}
		// Further loop is for ASCII pictures with variable
		// graphics width
		for (; i <= max; i ++) {
			if (i < max) {
				fprintf(logo, "0x%02X,", ' ');
				e ++;
				if ((e % 12) == 0) {
					fprintf(logo, "\n\t");
				} else {
					fprintf(logo, " ");
				}
			}
			if ((i == max) && (k < j)) {
				fprintf(logo, "0x%02X, ", ' ');
				fprintf(logo, "0x%02X,", '\n');
				e += 2;
				if ((e % 12) == 0) {
					fprintf(logo, "\n\t");
				} else {
					fprintf(logo, " ");
				}
			}
			if ((i == max) && (k == j)) {
				fprintf(logo, "0x%02X", ' ');
				e ++;
			}
		}
	}
	fprintf(logo, "\n};\n");
	// Restore original handler, free pointers and return
	signal(SIGINT, ori_handler);
	for (i = j; i >= 0; i --) free(inbuf[i]);
	fclose(picp);
	fclose(logo);
	return (EXIT_SUCCESS);
}

void main_handler(int sig) {
	// Signal handler function body
	fprintf(stderr, "SIGNAL: Termination has been requested\n");
	qqq = 0;
	exit(EXIT_SUCCESS);
}
