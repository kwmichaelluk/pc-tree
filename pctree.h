#ifndef __project445__pctree__
#define __project445__pctree__

#include <iostream>
#include <map>
#include <vector>
#include <assert.h>

class PCtree{
    //Forward declare...
    struct Pnode;
    struct PCarc;
    
public:
    //Constructor for 01 Matrix m stored as 2D array. Maybe write class for 01matrix if have time.
    PCtree(int** m, int row, int col);
    
    //Methods for a constructed PC tree
    //Get all permutations of PC tree
    int* getPermutations(FILE* output);

    
private:
    //************************************************
    //Data
    //************************************************
    //01-Matrix Copy
    int** M;
    
    //Current Row
    int currRow;
    
    //Number of Leaves
    int numLeaves;
    
    //leaf Arcs initialized such that column 1 is leafArcs[0]
    PCarc** leafArcs;
    
    //Store all PARTIAL arcs (nodes)
    std::vector<PCarc*> partialArcs;
    
    //Terminal Path
    std::vector<PCarc*> terminalPath;
    
    //Used in ContractionStep
    PCarc* newCnode = NULL;
    
    //
    PCarc* rootArc = NULL;
    
    //************************************************
    //Helper Methods
    //************************************************
    //Constructor will require the following to construct tree for main algorithm:
    void initializeTree();
    
    void labelTree();
    
    void getTerminalPath();
    
//    void separateFullEmpty();
    
    void splitTree();
    
//    void deleteReplace();
    
    void contractionStep();
    
    //Helper Methods for labelTree
    void resetArcSet(PCarc* arc, std::map<PCarc*,bool> &marked);
    void setFullNode(PCarc* arc);
    void incrementCounter(PCarc* arc);
    void storePartialArc(PCarc* arc);
    
    //Helper Method for getTerminalPath
    void terminalPathClean();
    bool isHigherArc(PCarc* a, PCarc* b);
    bool isSameNode(PCarc* a, PCarc* b);
    
    //Helper Method for splitTree
    void sortTerminalPath();
    void setDegree(PCarc* node);
    void setNewPnode(PCarc* node);
    
    //Helper Method for contractionStep
    void contractEdge(PCarc* edge);
    
    //
    bool isAdjacent(PCarc* nodeA, PCarc* nodeB);
    void removeEdge(PCarc* arc);
    void removeEdge(PCarc* nodeA, PCarc* nodeB);
    
    //Helper Methods for getPermutations()
    int frontier();
    
    //General Helper Methods
    PCarc* getParent(PCarc* arc);
    
   
    //Node Labelling
    enum PClabel {EMPTY,PARTIAL,FULL};
    
	//Visualization methods
	void printNode(PCarc* arc,FILE* output);
	




    //************************************************
    //Structs
    //************************************************
    
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
        
        //Needed for Labelling Stage
        PClabel label = EMPTY;
        int degree;
        int fullCounter;
    };
    
    //Representing P Node
    struct Pnode {
        //Point to one of its parent arcs, unless it's the root
        PCarc* parentArc = NULL;
    };
    
    
};

#endif /* defined(__project445__pctree__) */
