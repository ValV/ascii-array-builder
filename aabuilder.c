#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

// Variables
volatile int qqq = 1;

// Functions prototypes
void main_handler(int);

int main(int argc, char *argv[]) {
	// Change default application handler
	// with custom one
	void (*ori_handler)(int);
	ori_handler = signal(SIGINT, main_handler);

	// The rest of the program
	int i = 0, j = 0, max = 0;
	char ch = '\0';
	errno = 0;
	FILE *picp = fopen("spider-1.ascii", "r+b");
	if (errno != 0) {
		perror("fopen");
		return (errno);
	}
	errno = 0;
	char *instr = malloc(sizeof(char) * (i + 1));
	if (errno != 0) {
		perror("(char *) malloc");
		return (errno);
	}
	*instr = ch;
	errno = 0;
	char **inbuf = malloc(sizeof(char *) * (i + 1));
	if (errno != 0) {
		perror("(char **) malloc");
		return (errno);
	}
	*inbuf = instr;
	while ((ch = fgetc(picp)) != EOF) {
		// The cycle which parses the picture
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
			//printf("%s\n", inbuf[j - 1]);
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
	printf("char logo[] = {\n\t");
	for (e = k = 0; k <= j; k ++) {
		i = 0;
		while (inbuf[k][i] != '\0') {
			if (i < max) {
				printf("0x%02X,", inbuf[k][i]);
				e ++;
				if ((e % 12) == 0) {
					printf("\n\t");
				} else {
					printf(" ");
				}
			}
			if ((i == max) && (k < j)) {
				printf("0x%02X, ", inbuf[k][i]);
				printf("0x%02X,", '\n');
				e += 2;
				if ((e % 12) == 0) {
					printf("\n\t");
				} else {
					printf(" ");
				}
			}
			if ((i == max) && (k == j)) {
				printf("0x%02X", inbuf[k][i]);
				e ++;
			}
			i ++;
		}
		// Further loop is for ASCII pictures with variable
		// graphics width
		for (; i <= max; i ++) {
			if (i < max) {
				printf("0x%02X,", ' ');
				e ++;
				if ((e % 12) == 0) {
					printf("\n\t");
				} else {
					printf(" ");
				}
			}
			if ((i == max) && (k < j)) {
				printf("0x%02X, ", ' ');
				printf("0x%02X,", '\n');
				e += 2;
				if ((e % 12) == 0) {
					printf("\n\t");
				} else {
					printf(" ");
				}
			}
			if ((i == max) && (k == j)) {
				printf("0x%02X", ' ');
				e ++;
			}
		}
	}
	printf("\n}\n");
	// Restore original handler, free pointers and return
	signal(SIGINT, ori_handler);
	for (i = j; i >= 0; i --) free(inbuf[i]);
	fclose(picp);
	return (0);
}

void main_handler(int sig) {
	// Handler function body
	printf("\nSIGNAL: Termination has been requested\n");
	qqq = 0;
	//exit(EXIT_SUCCESS);
}
