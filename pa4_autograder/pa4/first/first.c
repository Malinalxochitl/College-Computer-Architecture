#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

void hextobin (char* address, char* input) {
	const char *binary[16] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
	const char *digits = "0123456789abcdef";
	int i = 0;
	strcpy(address,"\0");
//	printf("the address is %s\n", input);
	while(input[i] != '\0') {
		const char *v = strchr(digits, tolower(input[i++]));
//		printf("The character is %c\n", *v);
		if (v != NULL) {
			if (*v == '0') {
				strcat(address,binary[0]);
			} else if (*v == '1') {
				strcat(address,binary[1]);
			} else if (*v == '2') {
				strcat(address,binary[2]);
			} else if (*v == '3') {
				strcat(address,binary[3]);
			} else if (*v == '4') {
				strcat(address,binary[4]);
			} else if (*v == '5') {
				strcat(address,binary[5]);
			} else if (*v == '6') {
				strcat(address,binary[6]);
			} else if (*v == '7') {
				strcat(address,binary[7]);
			} else if (*v == '8') {
				strcat(address,binary[8]);
			} else if (*v == '9') {
				strcat(address,binary[9]);
			} else if (*v == 'a') {
				strcat(address,binary[10]);
			} else if (*v == 'b') {
				strcat(address,binary[11]);
			} else if (*v == 'c') {
				strcat(address,binary[12]);
			} else if (*v == 'd') {
				strcat(address,binary[13]);
			} else if (*v == 'e') {
				strcat(address,binary[14]);
			} else if (*v == 'f') {
				strcat(address,binary[15]);
			}
		}
	}
	strcat(address,"\0");
}

typedef struct {
	char* tag;
	int valid;
} Line;

typedef struct {
	Line* baseLine;
	int new;
} Set;

FILE *fp;
int reads; //memory reads
int writes; //memory writes
int hits; //cache hits
int misses; //cache misses

int main (int argc, char** argv) {
	reads = 0;
	writes = 0;
	hits = 0;
	misses = 0;
	int prefetch;
	int i, j;
	Set *currSet;
	Set *baseSet;
	Line *currLine;
	int cacheSize = atoi(argv[1]);
	char* assoc = argv[2];
	char* policy = argv[3];
	int blockSize = atoi(argv[4]);
	int offsetSize = log(blockSize) / log(2);
	int setSize, numSets;
	if (strcmp("direct",assoc) == 0) {
		setSize = 1;
	} else if (strcmp("assoc",assoc) == 0) {
		setSize = 2;
	} else {
		char* temp = (char *)malloc(3*sizeof(char));
		strcpy(temp,assoc+6);
		setSize = atoi(temp);
	}
	numSets = cacheSize / (setSize * blockSize);
	int indexSize = floor(log(numSets) / log(2));
	int tagSize = 48 - (indexSize + offsetSize);
	
	for(prefetch = 0;prefetch < 2;prefetch++) {
	
	fp = fopen(argv[5], "r");
	
	baseSet = (Set *)malloc(numSets*sizeof(Set));
	for (i = 0;i < numSets;i++) {
		currSet = baseSet+i;
		currSet->baseLine = (Line *)malloc(setSize*sizeof(Line));
		for (j = 0;j < setSize;j++) {
			currLine = currSet->baseLine+j;
			currLine->tag = (char *)malloc((tagSize+1)*sizeof(char));
		}
		currSet->new = 1;
	}
	currSet = baseSet;
	char action[2];
	while (1) {
		char* address = (char *)malloc(49*sizeof(char));
		fscanf(fp, "%s", address);
		if (strcmp(address, "#eof") == 0)
			break;
		char* input = (char *)malloc(16*sizeof(char));
		fscanf(fp, "%s", action);
//		printf("the action is |%s|\n",action);
		fscanf(fp, "%s", input);
		input+=2;
//		printf("the input is |%s|\n", input);
		char* temp = (char *)malloc(16*sizeof(char));
		while (strlen(input) < 12) {
			strcpy(temp,input);
			strcpy(input,"0");
			strcat(input,temp);
		}
		free(temp);
//		printf("the input is |%s|\n", input);
		hextobin(address, input);
//		free(input);
		char index[indexSize+1];
		char tag[tagSize+1];
		memcpy(index, address+tagSize, indexSize+1);
		index[indexSize] = '\0';
		memcpy(tag, address, tagSize+1);
		tag[tagSize] = '\0';
		i = strtol(index, NULL, 2);
		currSet = baseSet+i;
		currLine = currSet->baseLine;
		int hit = 0;
		for (i = 0;i < setSize;i++) {
			currLine = currSet->baseLine+i;
			if ((currLine->valid == 1 || currLine->valid == -1) && strcmp(currLine->tag, tag) == 0) {
				hit = 1;
				break;
			}
		}
		if (hit == 1 && strcmp(action, "R") == 0) {
			hits++;
		} else if (hit == 1 && strcmp(action, "W") == 0) {
			hits++;
			writes++;
		} else if (hit == 0 && strcmp(action, "W") == 0) {
			misses++;
			writes++;
			reads++;
			if (strcmp(policy,"fifo") == 0) {
				i = 0;
				while (1) {
					if (i == setSize) {
						i = 0;
						currSet->new *= -1;
					}
					currLine = currSet->baseLine+i;
					if (currLine->valid != currSet->new) {
						currLine->valid = currSet->new;
						memcpy(currLine->tag, tag, tagSize+1);
						currLine->tag[tagSize] = '\0';
						break;
					}
					i++;
				}
			} else {
				printf("Not implemented\n");
				return 0;
			}
		} else if (hit == 0 && strcmp(action, "R") == 0) {
			misses++;
			reads++;
			i = 0;
			if (strcmp(policy,"fifo") == 0) {
				while (1) {
					if (i == setSize) {
						i = 0;
						currSet->new *= -1;
					}
					currLine = currSet->baseLine+i;
					if (currLine->valid != currSet->new) {
						currLine->valid = currSet->new;
						memcpy(currLine->tag, tag, tagSize+1);
						currLine->tag[tagSize] = '\0';
						break;
					}
					i++;
				}
			} else {
				printf("Not implemented\n");
				return 0;
			}	
		}
		if (hit == 0 && prefetch == 1) {
			int address_2 = strtol(input, NULL, 16);
			address_2+= blockSize;
			sprintf(&input[0], "%x", address_2);
			char* temp = (char *)malloc(16*sizeof(char));
			while (strlen(input) < 12) {
				strcpy(temp,input);
				strcpy(input,"0");
				strcat(input,temp);
			}
			free(temp);
			hextobin(address, input);
			memcpy(index, address+tagSize, indexSize+1);
			index[indexSize] = '\0';
			memcpy(tag, address, tagSize+1);
			tag[tagSize] = '\0';
			i = strtol(index, NULL, 2);
			currSet = baseSet+i;
			currLine = currSet->baseLine;
			hit = 0;
			for (i = 0;i < setSize;i++) {
				currLine = currSet->baseLine+i;
				if ((currLine->valid == 1 || currLine->valid == -1) && strcmp(currLine->tag, tag) == 0) {
					hit = 1;
					break;
				}
			}
			i = 0;
			if (hit == 0 && strcmp(policy,"fifo") == 0) {
				reads++;
				while (1) {
					if (i == setSize) {
						i = 0;
						currSet->new *= -1;
					}
					currLine = currSet->baseLine+i;
					if (currLine->valid != currSet->new) {
						currLine->valid = currSet->new;
						memcpy(currLine->tag, tag, tagSize+1);
						currLine->tag[tagSize] = '\0';
						break;
					}
					i++;
				}
			} else if (strcmp(policy,"fifo") != 0) {
				printf("Not implemented\n");
				return 0;
			}
		}
//		free(address);
	}
	if (prefetch == 0) {
		printf("no-prefetch\n"); }
	else {
		printf("with-prefetch\n"); }
	printf("Memory reads: %d\n", reads);
	printf("Memory writes: %d\n", writes);
	printf("Cache hits: %d\n", hits);
	printf("Cache misses: %d\n", misses);
	
	reads = 0;
	writes = 0;
	hits = 0;
	misses = 0;
	}
	fclose(fp);
	return 0;
}
