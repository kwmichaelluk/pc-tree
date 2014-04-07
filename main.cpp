#include <iostream>
#include <stdio.h>
#include "pctree.h"

int main() {
    std::cout << "Hello, World!\n";
    
    //Test Case
    int rows = 5, cols = 6;
    int** sample = new int*[rows];
    for(int i=0;i<rows;i++) {
        sample[i] = new int[cols];
    }
    
    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            sample[i][j] = 0;
        }
    }
	
	/* Sample matrix
	1 1 0 0 0 0
	0 1 1 0 0 0
	1 1 1 1 0 0
	0 0 0 1 1 1
	1 0 0 0 1 1
	*/

    sample[0][0]=1; sample[0][1]=1;
    sample[1][1]=1; sample[1][2]=1;
    sample[2][0]=1; sample[2][1]=1; sample[2][2]=1; sample[2][3]=1;
    sample[3][3]=1; sample[3][4]=1; sample[3][5]=1;
    sample[4][0]=1; sample[4][4]=1; sample[4][5]=1;
    

	FILE* texOut;
	texOut = fopen("pctree.tex","w");
    
    PCtree myTree(sample,rows,cols);
	myTree.getPermutations(texOut);

    fclose(texOut);
    return 0;
}

