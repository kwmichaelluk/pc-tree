#ifndef __project445__pctree__
#define __project445__pctree__

#include <iostream>

class PCtree{
    //Forward declare...
    struct Pnode;
    struct PCarc;
    
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

    
   
    //Node Labelling
    enum PClabel {EMPTY,PARTIAL,FULL};
    
    //Structs
    //Representing arc (x,y)
    struct PCarc {
        //Neighbours a and b
        PCarc* a, *b;
        
        //Twin Arcs
        PCarc* twin;
        
        //Parent Label - for each twin arc, one is true, other is false
        bool yParent;
        
        //If y is P node, point to it
        Pnode* yPnode = NULL;
    };
    
    //Representing P Node
    struct Pnode {
        //Point to one of its parent arcs, unless it's a root
        PCarc* parentArc = NULL;
    };
    
    //Representing leaf node
    struct leafNode {
        int colNum = -1;
    };
    
    
};

#endif /* defined(__project445__pctree__) */
