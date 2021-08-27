#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<string.h> 
#include<math.h>
#define MAXCHAR 124 //in theory, I don't think many people have a name that's longer than 124 characters
#define FIRSTADD 1000
#define FIRSTDEL 500
struct mB
{
	char * data;
	int size;
	struct mB *next;
};

struct mB *AllocMBHead;
struct mB *AllocMBCurr;
struct mB *FreeMBHead;
struct mB *FreeMBCurr;
int readerCount = 0;
int listCount = 0;

//Adding blocks directly into the chain such as the first 1000
struct mB* add(int size, char * data){
	struct mB *block = (struct mB*)malloc(sizeof(struct mB));
	block->size = size;
	block->data = data;
	if(AllocMBHead == NULL){
		AllocMBHead = block;
		AllocMBCurr = block;
		return;
	}
	AllocMBCurr->next = block;
	AllocMBCurr = block;
	return block;
}	

//This function will either add the block into the freeMb or AllocMb
//based on the boolean passed to it 
struct mB* toFreedom(int size, char * data, bool list){
	struct mB *block = (struct mB*)malloc(sizeof(struct mB));
	if(list){
		block->size = size;
		void *request = sbrk(block->size);
		block->data = request;
		if(FreeMBHead == NULL){
			FreeMBHead = block;
			FreeMBCurr = block;
			return;
		}
		FreeMBCurr->next = block;
		FreeMBCurr = block;
		return block;
	}
	else{
		block->size = size;
		block->data = data;
		if(AllocMBHead == NULL){
			AllocMBHead = block;
			AllocMBCurr = block;
			return;
		}
		AllocMBCurr->next = block;
		AllocMBCurr = block;
		return block;
	}
}

void printList(FILE* fp){
	struct mB *temp = AllocMBHead;
	int count = 1;
	while(temp != NULL){
		printf("count: %d, address=%p, size=%d\n p=%s\n",count, temp->data,temp->size, (char *)temp->data);
		fprintf(fp, "count: %d, address=%p, size=%d p=%s\n",count, temp->data,temp->size, (char *)temp->data);
		count++;
		temp = temp->next;
	}
	temp = FreeMBHead;
	count = 1;
	while(temp != NULL){
		printf("count: %d, address=%p, size=%d\n",count, temp->data,temp->size);
		fprintf(fp, "count: %d, address=%p, size=%d p=%s\n",count, temp->data,temp->size, (char *)temp->data);
		count++;
		temp = temp->next;
	}
	fprintf(fp, "Total memory allocated: %d", listCount);
}

// searches either list depending on the boolean passed to it
struct mB* search(int check, struct mB ** prev, bool list){
	struct mB *test = NULL;
	struct mB * temp = NULL;
	int count = 0;
	int listCheck = 0;
	if(list){
		test = AllocMBHead;
		//printf("No. to delete = %d\n", check);
		while(count != check){
			temp = test;
			test = test->next;
			count++;
		}
		//printf("out of count loop, data for delete: %d , %s\n", test->size, (char *) test->data);
		*prev = temp;
		return test;
	}
	else{
		test = FreeMBHead;
		//printf("No. to seek best size for = %d\n", check);
		count = test->size;
		while(count < check){
			if(test->next == NULL){
				return NULL;
			}
			temp = test;
			test = test->next;
			count = test->size;
			listCheck++;
		}
		//printf("out of Firstfit loop, data for use: %d , %s\n", test->size, (char *) test->data);
		*prev = temp;
		return test;
	}
}


int delete(int check){
	struct mB *prev = NULL;
	struct mB *del = NULL;
	
	del = search(check, &prev, true);
	//printf("Trying to delete: %d, node data: %p, size = %d\n", check, del->data, del->size);
	if(del == NULL){
		//printf("Failed to find node to delete\n");
		return 0;
	}
	else{
		if(prev != NULL){
			prev->next = del->next;
		}
	
		if(del == AllocMBCurr){
			AllocMBCurr = prev;
		}
		else if(del == AllocMBHead){
			AllocMBHead = del->next;
		}
	}
	//printf("Trying to add to free loop\n");
	toFreedom(del->size, NULL, true);
	return 0;
}

//This function first searches if the FreeMB list contains a block that is suitable 
//for the given process
int firstFit(int check, char * data){
	struct mB *prev = NULL;
	struct mB *del = NULL;
	
	del = search(check, &prev, false);
	//if the size of the block is larger than the check size, we must split the block, by pulling back the block by diff
	//and then creating a new block of size diff to add onto the free Block
	if(del == NULL){
		//if the list cannot find a suitable sized block, it must create a new memory block on the end of allocList
		toFreedom(check, data, false);
		listCount += check;
		return 0;
	}
	if(del->size > check){
		//create new block with size of new data
		add(check, data);
		//take the size of the block off of the size of the empty block
		del->size -= check;
		void *request = sbrk(del->size);
		del->data = request;
		return 1;
	}
	else{
		//if the block is the same size, we just have to create a new block and remove the old one
		if(del == NULL){
			printf("Failed to find node to delete\n");
			return -1;
		}
		else{
			if(prev != NULL){
				prev->next = del->next;
			}
	
			if(del == FreeMBCurr){
				FreeMBCurr = prev;
			}
			else if(del == FreeMBHead){
				FreeMBHead = del->next;
			}
		}
		toFreedom(del->size, data, false);
		return 0;
	}
}

//produces a random integer for delete
int randomNo(int lower, int upper){
	int num = (rand() % (upper - lower - 1)) + lower;
	return num;
}

//Randomly deletes a node based on randomNo
void randomDelete(){
	int limit = 0;
	int currNo = 0;
	int i = 0;
	limit = FIRSTDEL;
	srand(time(0));
	while(i != limit){
		currNo = randomNo(0, FIRSTADD - i);
		delete(currNo);
		i++;
	}
}

int main(int argc, char *argv[])
{
	bool printCheck = false;
	char * extraFile = ".txt";
	char * outputFile = "FirstFitFirstN";
	char * outputFile2 = "FirstFitMiddle";
	if(argc != 2){
		printf("Sorry! Please only execute this file in the format: ./FirstFit **.txt\n");
		return EXIT_FAILURE;
	}
	char* filename = argv[1];
	if(strcmp(filename, "first-names.txt") == 0){
		printCheck = true;
	}
	FILE *fp;
	char str[MAXCHAR];
	
	fp = fopen(filename, "r");
	if(fp == NULL){
		printf("Unable to open file %s :(", filename);
		return 1;
	}
	char * token;
	int i = 0;
	int limit = FIRSTADD;
	int currNo = 0;
	//Adding the first 1000
	while(fgets(str, MAXCHAR, fp) != NULL){
		token = strtok(str, "\n");
		size_t size = strlen(token) +1;
		void *request = sbrk(size);
		strcpy((char *)request, token);
		add(size, request);
		listCount += size;
		i++;
		if(i == limit){
			break;
		}
	}
	readerCount += FIRSTADD;
	randomDelete();
	
	//Beginning of the first fit algorithm
	limit = FIRSTADD;
	i = 0;
	while(fgets(str, MAXCHAR, fp) != NULL){
		token = strtok(str, "\n");
		size_t size = strlen(token) +1;
		void *request = sbrk(size);
		strcpy((char *)request, token);
		firstFit(size, request);
		i++;
		//if we reach 1000, we delete another 500
		if(i == limit){
			randomDelete();
			i = 0;	
		}
	}
	
	//printing the output files depending on the argv
	char *outfilename = malloc(strlen(outputFile) + strlen(extraFile) + 1);
	if(printCheck == true){
		strcpy(outfilename, outputFile);
		strcat(outfilename, extraFile);
	}
	else{
		strcpy(outfilename, outputFile2);
		strcat(outfilename, extraFile);
	}
	fp = fopen(outfilename, "w");
	printList(fp);
	fclose(fp);
	printf("Total memory allocated: %d\n", listCount);
	printf("The First Fit algorithm, by Rhys Mallia S3656436 :)\n");
	return 0;
}