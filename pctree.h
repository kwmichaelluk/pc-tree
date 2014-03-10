#ifndef __project445__pctree__
#define __project445__pctree__

#include <iostream>


class PCtree{
    //Forward declare...
    struct PCnode;
    
public:
    //Constructor for 01 Matrix m stored as 2D array. Maybe write class for 01matrix if have time.
    PCtree(int** m);
    
    //Methods for a constructed PC tree
    //Get all permutations of PC tree
    int* getPermutations();

    
private:
    //Data
    int numLeaves;
    
    //Helper Methods
    //Constructor will require the following to construct tree for main algorithm:
    void initializeTree();
    
    void labelTree();
    
    void getTerminalPath();
    
    void separateFullEmpty();
    
    void splitTree();
    
    void deleteReplace();
    
    void contractionStep();
    
    //Helper Methods for getPermutations()
    int frontier();
    
    //General Helper Methods
    void transpose(PCnode* x);
   
    //Structs
    struct PCnode{
        
    };
    
    
};

#endif /* defined(__project445__pctree__) */
