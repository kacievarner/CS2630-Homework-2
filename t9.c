//Kacie Varner
//Fall 2020
/*This application is an implementation of the T9 input text mode on older cell phones.
This implementation uses the 0 as a space, 1 as some forms of punctuation, and 2 through
9 being the english alphabet. 
2 = abc 
3 = def
4 = ghi
5 = jkl
6 = mno
7 = pqrs
8 = tuv
9 = wxyz
In addition to this functionality, the '#' symbol will use the predictive text feature
to find the next word in line, for example 76257 -> rocks and 76257# -> socks*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//sets the max length to 100
#define BUFFER_LEN 100

//constructs the start of the Trie. Creates a struct trieNode that defines
//the input key sequence that will attach each letter to a number
typedef struct trieNode {
	char* word; //creates a pointer to the word
	struct trieNode* charSequence[11]; //charSequence is the keypad functionality
} 
nodeInTrie;

//Initiation of trie creation functions before the program to avoid implicit declaration
//before the main function
nodeInTrie* createTrieNode();
nodeInTrie* searchNode(nodeInTrie* root, char* number);

//Initiation of helper functions before the program to avoid implicit declaration 
//before main function
int convertCharacter(char letter); //converts char to number
void makeTrie(nodeInTrie* root, char* str); //defines the structure of trie
void freeNodes(nodeInTrie* root); //frees the node data allocated in malloc

/*main function has same initial functionality as readfile.c. It reads in the file and
if it is missing arguments or cant open the file it ends in error. Also creates the trie
node root and recursively constructs the trie. From there it loops to search the trie
for the numbers that it takes in from user input and displays the dictionary word for
that representation of numbers. If there is a pound sign included it will also progress
to the next dictionary word in alphabetical order from the number it was given from input,
otherwise print the according error message.
*/
int main(int argc, char **argv) {
	if (argc < 2) { //if two arguments arent given
		printf("missing FILE argument\n"); //print error
		exit(1); //exit in error
	}
	//opens and reads the file
	char *filename = argv[1];
	FILE *file;
	char buffer[BUFFER_LEN]; //max length = buffer length
	file = fopen(filename, "r"); //open and read the file
	nodeInTrie* root = createTrieNode(); //creates trie root
	if (file != NULL){
		while (fgets(buffer, BUFFER_LEN, file) != NULL){
			makeTrie(root, buffer);
		}
	}else{
		printf("Could not open file.\n");
		exit(1); //exit in error
	}
	fclose(file);
	printf("Enter \"exit\" to quit.\n");
	char number[BUFFER_LEN];  //stores the user input numbers
	bool temp = true;  //temp bool to activate while loop
	nodeInTrie* current = root;  //stores the trie traversal
	//handling of user input with the # sign 'flag'
	while (temp) { //while true
		printf("Enter Key Sequence (or \"#\" for next word): \n"); //user input
		scanf("%s", number); //scans the input number
		if (strcmp("exit", number) != 0) { //if not equal to 0
			if (number[0] != '#') { //if not equal to #
				current = searchNode(root, number); //current equals value from letter and search
			} else {
				current = searchNode(current, number); //else search from current node instead of root
			}
			if (current != NULL) { //if current has a value
				if (current->word) { //current points to word
					printf("%s\n", current -> word); //prints the word the numbers at current point to
				} else {
					printf("Not found in current dictionary.\n");//else word not found error
				}
			} else {
				if (number[strlen(number)-1] == '#') { // if # is used and no more synonyms in trie
					printf("There are no more T9onyms\n"); //print no more...
				} else {
					printf("Not found in current dictionary.\n"); //else, if not valid, print not found
				}
			}
		} else {
			temp = false; //exit while
		}
	}
	//frees all allocated space that was malloc'd
	freeNodes(root); //free children
	free(root); //free root
	return 0;
}

//creates a tree node that is malloc'd 
nodeInTrie* createTrieNode() {
	nodeInTrie* node = (nodeInTrie*) malloc(sizeof(nodeInTrie)); //allocates data for node
	node->word = NULL; //node 
	for (int i = 0; i < 11; i++) {
		node->charSequence[i] = NULL;
	}
	return node; //will return constructed node in trie
}

//converts a character to its coresponding numeric value by using a 
//26 digit character array that matches the number with its letter going
//from a to z
//Idea for array implementation derrived from geeksforgeeks.com example
int convertCharacter(char letter) {
	char numberAlph[26] = {2, 2, 2, //abc
						3, 3, 3, //def
						4, 4, 4, //ghi
						5, 5, 5, //jkl
						6, 6, 6, //mno
						7, 7, 7, 7, //pqrs
						8, 8, 8, //tuv
						9, 9, 9, 9}; //wxyz
	int x = letter - 'a';
	return numberAlph[x]; //returns letter indexed at that number
}

/*translates the input word into its numeric sequence, than it builds the trie with the word at
the end of the path corresponding to the numbers to which it matches in the numberAlph char array*/
void makeTrie(nodeInTrie* root, char* str) {
	nodeInTrie* currNode = root; //set the current active node equal to the root
	int length = strlen(str); //length of the char str
	char* text = (char*) malloc(length); //allocates data 
	if (text != NULL) { //if text has value
		strncpy(text, str, length-1); //makes a copy of the string
		text[length-1] = '\0'; // text = NUL
	}
	int x = 0; //temp integer
	while (str[x] != '\n') { //while the str indexed at 0 is not equal to newline
		int number = convertCharacter(str[x]); //set number equal to the numeric equivilent of the letter
		if (currNode->charSequence[number] == NULL) { //if the node is equal to null/doesnt exist yet
			currNode->charSequence[number] = createTrieNode(); //create a new node to store the data
		}
		currNode = currNode->charSequence[number]; //sets currNode equal to the character seq of the num
		x++;//increment temp to teaverse the trie
	}
	while (currNode->charSequence[10] != NULL) { //while data is not null
		currNode = currNode->charSequence[10]; //set currNode equal to the char sequence indexed at 10
	}
	if (currNode->word == NULL) { //if the value at word is equal to NULL
		currNode->word = text; //set equal to char* text
	} else {
		currNode->charSequence[10] = createTrieNode(); //otherwise create new node to store value @ [10]
		currNode = currNode->charSequence[10]; //then put the data in the new node
		currNode->word = text; //set equal to char* text
	}
}

//traverse and search the trie node according to the given key sequence specified by user input
nodeInTrie* searchNode(nodeInTrie* root, char* number) {
	nodeInTrie* cur = root; //set the current node equal to the root node
	for (int i = 0; i < strlen(number); i++) { //while i < the length of char* number
		if (number[i] != '#') { //if # is not used
			if (cur->charSequence[(number[i] - '0')] == NULL) { //if data at character sequence is null
				return NULL; //value is null, return null
			}
			cur = cur->charSequence[(number[i] - '0')]; //set cur variable to data at char seq
		} else {
			if (cur->charSequence[10] == NULL) { //if value[10] is null
				return NULL; //value is null, return null
			}
			cur = cur->charSequence[10]; //set cur variable to data at char seq indexed at 10
		}
	}
	return cur; //returns value of cur
}

//recurses through the tree and frees all the children of the root node 
void freeNodes(nodeInTrie* root) {
	for (int i = 0; i < 11; i++) { //while i < max amount of children
		if (root->charSequence[i] != NULL) { //if root has children
			freeNodes(root->charSequence[i]); //free child nodes allocated in malloc
			free(root->charSequence[i]); //free child nodes
		}
	}
	if (root->word != NULL) { //if the data held by word has a value that isnt null
		free(root->word); //free the data at that word/child
	}
}
