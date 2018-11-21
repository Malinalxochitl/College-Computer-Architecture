#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

void makeGray(int inputs, int** graycode/*[2 << (inputs-1)][inputs]*/) { //generates graycode into the given array
	graycode[0][inputs-1] = 0;
	graycode[1][inputs-1] = 1; //sets up 1bit graycode
	int i, j, k, l;
	for (i = 1;i < inputs; i++) { //at the start of this loop the array contains i-bit gray code
		for (l = 0;l <= i;l++) { //l goes goes from least to most significant bit
			j = 0;
			k = (2 << i)-1;
			while (j < k) { //mirrors the column of bits in the array
				if (l == i) { //most significant bit has been reached, populates with 0s then 1s instead of mirroring
					graycode[j][(inputs-1)-l] = 0;
					graycode[k][(inputs-1)-l] = 1;
				} else {
					graycode[k][(inputs-1)-l] = graycode[j][(inputs-1)-l];
				}
				j++;
				k--;
			}
		}
	}
}

int NOT(int a) { //The initial bits of the integer become 1s using bitwise, so the gate is done manually
	if (a == 1) {
		return 0;
	} else {
		return 1;
	}
}

int AND(int a, int b) { //bitwise AND gate
	return a&b;
}

int OR(int a, int b) { //bitwise OR gate
	return a|b;
}

int NAND(int a, int b) { //NOR gate
	if ((a&b) == 1) {
		return 0;
	}else {
		return 1;
	}
}

int NOR(int a, int b) { //NOR gate, 
	if ((a|b) == 1) {
		return 0;
	} else {
		return 1;
	}
}

int XOR(int a, int b) { //bitwise XOR gate
	return a^b;
}
/*creates an a to 2^a decoder
 *pointer arrays are used here in order to directly change the value regardless of location in memory */
void DECODER(int a, int inputs[a], int* outputs[2 << (a-1)]) {
	int** graycode/*[2 << (a-1)][a]*/;
	int i, j;
	graycode = (int**)malloc(sizeof(int*)*(2 << (a-1)));
	graycode[0] = (int*)malloc(sizeof(int)*a*(2 << (a-1)));
	for (i = 0;i < 2 << (a-1);i++) {
		graycode[i] = (*graycode + a * i);
	}
	makeGray(a, graycode); //graycode will be used to determine if a NOT gate is needed
	for (i = 0; i < 2 << (a-1);i++) { //Goes through inputs in graycode order
		if (graycode[i][0] == 0) {
			*(outputs[i]) = NOT(inputs[0]);
		} else {
			*(outputs[i]) = inputs[0];
		}
		for (j = 1; j < a;j++) { //AND gates all the inputs into the output
			if(graycode[i][j] == 0) {
				
				*(outputs[i]) &= NOT(inputs[j]);
			} else {
				*(outputs[i]) &= inputs[j];
			}
		}
	}
	free(graycode[0]);
	free(graycode);
}
/*creates an a to 1 multiplexer
 * chooses a specific input to output depending on the selector bits*/
int MULTIPLEXER(int a, int input[a], int select[]) { 
	int b, i, j, k;
	for(b = 0;a >> b != 2;b++){}; //calculates # of selectors
	b++;
	int** graycode/*[a][b]*/;
	graycode = (int**)malloc(sizeof(int*) * a);
	graycode[0] = (int*)malloc(sizeof(int) * b * a);
	for (i = 0;i < a;i++) {
		graycode[i] = (*graycode + b * i);
	}
	makeGray(b, graycode); //generate graycode to compare the selectors to
	for (i = 0;i < a;i++) { //i is used as a counter to determine the correct input
		for (j = 0;j < b;j++) {
			if (select[j] == graycode[i][j]) { //graycode bit matches selector bit
				k = 1;
			} else {
				k = 0;
			}
			if (k != 1) { //move on to next set of graycode bits if they do not match
				break;
			}
		}
		if (k == 1) { //all graycode bits match the selector bits
			break;
		}
	}
	free(graycode[0]);
	free(graycode);
	if (k == 1) {
		return input[i];
	} else {
		printf("Multiplexor error\n");
		exit(0);
	}
}

typedef struct node {
	int num;
	char* name;
	struct node* next;
} node;

int main (int argc, char** argv) {
	FILE* fp = NULL;
	char* stream = (char*) malloc(15*sizeof(char));
	
	node* inputhead = NULL, *temphead = NULL, *outputhead = NULL, *pointer = NULL; //head nodes of the linked lists for the input, output, and temp variables
	int** graycode = NULL;														   //as well as a pointer for traversing the linked lists
	int i = 0, j, max, inputnum, outputnum, loop = 0;
	
	while(1) { //looping through the gray code
		fp = fopen(argv[1],"r");
		//fscanf(fp, "%s", stream);
		while(fscanf(fp, "%s", stream) != EOF) { //Selecting the directive	
			if (strcmp("INPUTVAR", stream) == 0) {                                                    	//INPUTVAR
				if (!inputhead) { //checks for a NULL pointer
					fscanf(fp, "%d", &inputnum);
					max = 2 << (inputnum-1);
					graycode = (int**)malloc(sizeof(int*) * max);
					graycode[0] = (int *)malloc(sizeof(int) * inputnum * max);
					for (j = 0; j < max;j++) {
						graycode[j] = (*graycode + inputnum * j);
					}
					makeGray(inputnum, graycode); //graycode has been created based off of num of inputs
					inputhead = (node*)malloc(sizeof(node));
					inputhead->num = graycode[i][0];
					fscanf(fp, "%s", stream);
					inputhead->name = strdup(stream);  //creating the first node for the input
					inputhead->next = NULL;
					pointer = inputhead;
					for (j = 1;j < inputnum;j++) { //creating an additional node in the linked list for each input in graycode order
						pointer->next = (node*)malloc(sizeof(node));
						pointer = pointer->next;
						fscanf(fp, "%s", stream);
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = graycode[i][j];
					}
				} else { //changing the inputs to the next graycode
					pointer = inputhead;
					fscanf(fp, "%d*", &j);
					for (j = 0;j < inputnum;j++) {
						fscanf(fp, "%s*", pointer->name);
						pointer->num = graycode[i][j];
						pointer = pointer->next;
					}
				}
			} else if (strcmp("OUTPUTVAR", stream) == 0) {                                            	//OUTPUTVAR
				if (!outputhead) {
					fscanf(fp, "%d", &outputnum);
					outputhead = (node*)malloc(sizeof(node));
					fscanf(fp, "%s", stream);
					outputhead->name = strdup(stream);
					outputhead->next = NULL;
					outputhead->num = -1;              //-1 will be used to identify an invalid input
					pointer = outputhead;
					for (j = 1;j < outputnum;j++) {
						pointer->next = (node*)malloc(sizeof(node));
						pointer = pointer->next;
						fscanf(fp, "%s", stream);
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				} else if (loop != 1) { //resets the outputs
					pointer = outputhead;
					fscanf(fp, "%d*", &j);
					for (j = 0;j < outputnum;j++) {
						fscanf(fp, "%s*", pointer->name);
						pointer->num = -1;
						pointer = pointer->next;
					}
				} else { //carry over the output data from last time
					fscanf(fp, "%d*", &j);
					for (j = 0;j < outputnum;j++) {
						fscanf(fp, "%s*",stream);
					}
					loop = 0;
				}
			} else if (strcmp("AND", stream) == 0) {                                                   	//AND
				int a[2], temp, invalid = 0;
				invalid = 0;
				for (j = 0; j < 2; j++) { //collecting inputs
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //uppercase first letter, signifying an input
						pointer = inputhead;
					} else if (islower(temp) != 0) { //lowercase first letter, signifying a temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						a[j] = 1;
						continue;
					} else { //boolean literal 0
						a[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						a[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				if (invalid == 1) {
					loop = 1;
					for (;j < 2;j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				fscanf(fp, "%s", stream); //collecting output
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(pointer->name, stream)!=0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(pointer->name, stream) != 0) {
						pointer = pointer->next;
					}
					if (pointer == NULL) { //variable not in LL
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) { //LL is empty
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = AND(a[0], a[1]);
				}
			} else if (strcmp("DECODER", stream) == 0) {                                                //DECODER
				int a, b, temp, invalid = 0, done = 0;
				done = 0;
				invalid = 0;
				fscanf(fp, "%d", &a);
				int inputs[a];
				for (j = 0; j < a; j++) { //collects inputs
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //uppercase first letter, signifying an input
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase first letter, signifying a temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						inputs[j] = 1;
						continue;
					} else { //boolean literal 0
						inputs[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						inputs[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				b = 2 << (a-1);
				if (invalid == 1) {
					loop = 1;
					for (j += 1;j < (b+a);j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				int* outputs[b];
				for (j = 0;j < b;j++) { //collects addresses of the outputs
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //output variable
						pointer = outputhead;
						while(strcmp(stream, pointer->name) != 0) {
							pointer = pointer->next;
						}
					} else { //temp variable
						pointer = temphead;
						while(pointer != NULL && strcmp(stream, pointer->name)) {
							pointer = pointer->next;
						}
						if (pointer == NULL) { //variable not in LL
							pointer = temphead;
							while(pointer != NULL && pointer->next != NULL) {
								pointer = pointer->next;
							}
							if (pointer == NULL) { //temp variable LL is empty
								pointer = (node*)malloc(sizeof(node));
								temphead = pointer;
							} else {
								pointer->next = (node*)malloc(sizeof(node));
								pointer = pointer->next;
							}
							pointer->next = NULL;
							pointer->name = strdup(stream);
							pointer->num = -1;
						}
					}
					if (pointer->num == -1) {
						outputs[j] = &(pointer->num);
					} else {
						done = 1;
						break;
					}
				}
				if (done == 0) {
					DECODER(a, inputs, outputs);
				} else {
					for(j+=1;j<b;j++) {
						fscanf(fp, "%s", stream);
					}
				}
			} else if (strcmp("MULTIPLEXER", stream) == 0) {                                            //MULTIPLEXER
				int a, b, temp, invalid = 0, k;
				invalid = 0;
				fscanf(fp, "%d", &a);
				int inputs[a];
				for (j = 0;j < a;j++) { //collects the inputs
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp)) { //uppercase letter, input variable
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase letter, temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						inputs[j] = 1;
						continue;
					} else { //boolean literal 0
						inputs[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name)) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						inputs[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				for (k = 0;a >> k != 2;k++){}; //# selectors calculation
				b = k+1;
				if (invalid == 1) {
					loop = 1;
					for(;j < a+b;j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				int selectors[b];
				for (j = 0;j < b;j++) { //collects selectors
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp)) { //uppercase letter, input variable
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase letter, temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						selectors[j] = 1;
						continue;
					} else { //boolean literal 0
						selectors[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name)) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						selectors[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				fscanf(fp, "%s", stream); //collect address of output
				if (invalid == 1) {
					loop = 1;
					for (j+=1;j < b;j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(stream, pointer->name)) {
						pointer = pointer->next;
					}
					if (pointer == NULL) { //variable not in LL
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) { //temp variable LL is empty
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->next = NULL;
						pointer->name = strdup(stream);
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = MULTIPLEXER(a, inputs, selectors);
				}
			} else if (strcmp("NAND", stream) == 0) {                                                   //NAND
				int a[2], temp, invalid = 0;
				invalid = 0;
				for (j = 0; j < 2; j++) {
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //uppercase first letter, signifying an input
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase first letter, signifying a temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						a[j] = 1;
						continue;
					} else { //boolean literal 0
						a[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						a[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				fscanf(fp, "%s", stream);
				if (invalid == 1) {
					loop = 1;
					for(j+=1;j < 2;j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(pointer->name, stream)!=0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(pointer->name, stream) != 0) {
						pointer = pointer->next;
					}
					if (pointer == NULL) {
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) {
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = NAND(a[0], a[1]);
				}
			} else if (strcmp("NOR", stream) == 0) {                                                    //NOR
				int a[2], temp, invalid = 0;
				invalid = 0;
				for (j = 0; j < 2; j++) {
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //uppercase first letter, signifying an input
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase first letter, signifying a temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						a[j] = 1;
						continue;
					} else { //boolean literal 0
						a[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						a[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				fscanf(fp, "%s", stream);
				if (invalid == 1) {
					loop = 1;
					for (j+=1;j < 2;j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(pointer->name, stream)!=0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(pointer->name, stream) != 0) {
						pointer = pointer->next;
					}
					if (pointer == NULL) {
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) {
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = NOR(a[0], a[1]);
				}
			} else if (strcmp("NOT", stream) == 0) {                                                    //NOT
				int a, temp;
				fscanf(fp, "%s", stream);
				temp = stream[0];
				if (isupper(temp)) { //uppercase first letter, signifying an input
					pointer = inputhead;
				} else if (islower(temp)) { //lowercase first letter, signifying a temp variable
					pointer = temphead;
				} else if (temp == 49) { //boolean literal 1
					a = 1;
					goto skip;
				} else { //boolean literal 0
					a = 0;
					goto skip;
				}
				while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
					pointer = pointer->next;
				}
				if (pointer != NULL) {
					a = pointer->num;
				} else {
					loop = 1;
					fscanf(fp, "%s", stream);
					continue;
				}
				skip:fscanf(fp, "%s", stream); //skip label to avoid pointer usage in case of a boolean
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer == NULL) {
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) {
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = NOT(a);
				}
			} else if (strcmp("OR", stream) == 0) {                                                     //OR
				int a[2], temp, invalid = 0;
				invalid = 0;
				for (j = 0; j < 2; j++) {
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //uppercase first letter, signifying an input
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase first letter, signifying a temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						a[j] = 1;
						continue;
					} else { //boolean literal 0
						a[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						a[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				fscanf(fp, "%s", stream);
				if (invalid == 1) {
					loop = 1;
					for(j+=1;j<2;j++) {
						fscanf(fp, "%s", stream);
					}
					continue;
				}
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(pointer->name, stream)!=0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(pointer->name, stream) != 0) {
						pointer = pointer->next;
					}
					if (pointer == NULL) {
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) {
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = OR(a[0], a[1]);
				}
			} else if (strcmp("XOR", stream) == 0) {                                                    //XOR
				int a[2], temp, invalid = 0;
				invalid = 0;
				for (j = 0; j < 2; j++) {
					fscanf(fp, "%s", stream);
					temp = stream[0];
					if (isupper(temp) != 0) { //uppercase first letter, signifying an input
						pointer = inputhead;
					} else if (islower(temp)) { //lowercase first letter, signifying a temp variable
						pointer = temphead;
					} else if (temp == 49) { //boolean literal 1
						a[j] = 1;
						continue;
					} else { //boolean literal 0
						a[j] = 0;
						continue;
					}
					while(pointer != NULL && strcmp(stream, pointer->name) != 0) {
						pointer = pointer->next;
					}
					if (pointer != NULL) {
						a[j] = pointer->num;
					} else {
						invalid = 1;
						break;
					}
				}
				fscanf(fp, "%s", stream);
				if (invalid == 1) {
					loop = 1;
					for(j+=1;j<2;j++) {
						fscanf(fp,"%s",stream);
					}
					continue;
				}
				temp = stream[0];
				if (isupper(temp)) { //output variable
					pointer = outputhead;
					while(strcmp(pointer->name, stream)!=0) {
						pointer = pointer->next;
					}
				} else { //temp variable
					pointer = temphead;
					while(pointer != NULL && strcmp(pointer->name, stream) != 0) {
						pointer = pointer->next;
					}
					if (pointer == NULL) {
						pointer = temphead;
						while(pointer != NULL && pointer->next != NULL) {
							pointer = pointer->next;
						}
						if (pointer == NULL) {
							pointer = (node*)malloc(sizeof(node));
							temphead = pointer;
						} else {
							pointer->next = (node*)malloc(sizeof(node));
							pointer = pointer->next;
						}
						pointer->name = strdup(stream);
						pointer->next = NULL;
						pointer->num = -1;
					}
				}
				if (pointer->num == -1) {
					pointer->num = XOR(a[0], a[1]);
				}
			} else {																					//undefined directive
				printf("invalid directive: %s\n", stream);
				exit(0);
			}
			//fscanf(fp, "%s", stream);
		}
		if (loop != 1) {
			i++;
			while (temphead != NULL) {
				pointer = temphead->next;
				free(temphead->name);
				free(temphead);
				temphead = pointer;
			}
			pointer = inputhead;
			while(pointer != NULL) {
				printf("%d ", pointer->num);
				pointer = pointer->next;
			}
			pointer = outputhead;
			while(pointer != NULL) {
				printf("%d ", pointer->num);
				pointer = pointer->next;
			}
			printf("\n");
		}
		if (i == max) {
			break;
		}
		fclose(fp);
		fp = NULL;
	}
	free(stream);
	fclose(fp);
	return 0;
}
