#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct symbol {
	int freq;
	int is_leaf;

	union {
		struct {
			struct symbol * left;
			struct symbol * right;
		} compound;

		char c;
	} u;
};

int * count_freqs(char * filename);
struct symbol * construct_huffman_tree(int * freqs, int nFreqs);
struct symbol * create_compound_symbol(struct symbol * p1, struct symbol * p2);
int find_lowest_freq(struct symbol ** list, int size);
int find_height(struct symbol * compound_root);
void compute_huffman_codes(int code[], int index, struct symbol * compound_root);

int * count_freqs(char * filename)
{
	int * freqs = calloc(256, sizeof(int));

  	char c;
  	FILE * file = fopen(filename, "r");

  	assert(file != NULL);

  	for (c=fgetc(file); !feof(file); c=getc(file))
  	{
  		freqs[c]++;
  	}
    
    for (int i=0; i < 256; i++)
    {
        if (freqs[i] == 0)
        {
            freqs[i] = 1;
        }
    }

  	return freqs;
}

struct symbol * construct_huffman_tree(int * freqs, int nFreqs)
{

	struct symbol ** list;

	list = malloc(sizeof(struct symbol *) * nFreqs);

	for (int i=0; i<nFreqs; i++)
	{
		struct symbol * p = malloc(sizeof(struct symbol));
		p -> freq = freqs[i];
		p -> is_leaf = 1;
		p -> u.c = (char) i;

		list[i] = p;
	}

	int size = nFreqs;

	while (size > 1)
	{
		int smallest, secondSmallest;

		smallest = find_lowest_freq(list, size);

		struct symbol * p1, * p2, * comp;

		p1 = list[smallest];
		list[smallest] = list[size-1];

		size--;

		secondSmallest = find_lowest_freq(list, size);

		p2 = list[secondSmallest];

		comp = create_compound_symbol(p1, p2);

		list[secondSmallest] = comp;
	}

	return list[0];
}

int find_lowest_freq(struct symbol ** list, int size)
{
	int minFreq = list[0] -> freq;
	int minFreqIndex = 0;

	for (int i=1; i<size; i++)
	{
		if (list[i] -> freq < minFreq)
		{
			minFreqIndex = i; 
		}
	}

	return minFreqIndex;
}

struct symbol * create_compound_symbol(struct symbol * p1, struct symbol * p2)
{
    struct symbol * result;
    result = malloc(sizeof(struct symbol));
    
    result -> freq = p1 -> freq + p2 -> freq;
    result -> is_leaf = 0;
    
    result -> u.compound.left = p1;
    result -> u.compound.right = p2;
    
    return result;
}

int find_height(struct symbol * compound_root)
{
    if (compound_root -> is_leaf)
    {
        return -1;
    }
    else
    {
        int leftHeight = find_height(compound_root -> u.compound.left);
        int rightHeight = find_height(compound_root -> u.compound.right);
        
        if (leftHeight > rightHeight)
        {
            return leftHeight + 1;
        }
        else
        {
            return rightHeight + 1;
        }
    }
}

void compute_huffman_codes(int code[], int index, struct symbol * compound_root)
{
    if (compound_root != NULL)
    {
        if (!compound_root -> is_leaf)
        {
            code[index] = 0;
            
            compute_huffman_codes(code, index + 1, compound_root -> u.compound.left);
            
            
            code[index] = 1;
            compute_huffman_codes(code, index + 1, compound_root -> u.compound.right);
        }
        else
        {
            printf("%c: ",compound_root -> u.c);
            
            for (int i = 0; i < index; i++)
            {
                printf("%d", code[i]);
            }
            
            printf("\n");
            
            return;
        }
    }
}

int main (int argc, char ** argv)
{
  	if ( argc != 2 ) {
    	fprintf(stderr, "Useage: huffman <filename>\n");
   	exit(1);	    // exit with error code
	}

  	int * freqs = count_freqs(argv[1]);
 	struct symbol * compound_root = construct_huffman_tree(freqs, 256);
    
    int max_height = find_height(compound_root);
    printf("height: %d\n\n", max_height);
    
    int code[max_height];
    
    int index = 0;
    
    compute_huffman_codes(code, index, compound_root);
    
	return 0;  // exit without error code
}
