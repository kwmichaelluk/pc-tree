#include "pctree.h"

PCtree::PCtree(int** m, int row, int col) {
    this->numLeaves = col;
    
    //Copy 01 Matrix
    int i,j;

    this->M = new int*[row];
    for(int i=0;i<row;i++) {
        this->M[i] = new int[col];
    }
    
    for(i=0;i<row;i++) {
        for(j=0;j<col;j++) {
            this->M[i][j] = m[i][j];
        }
    }
    
    //Create Initial Tree
    initializeTree();
    
    //For Each Row
    for(i=0;i<row;i++) {
        //Set Current Row
        this->currRow = i;
        
        //Reset All Arcs/Nodes
        labelTree();
    }
}

void PCtree::initializeTree() {
	Pnode parent;
	
    PCarc *prev=NULL;
	PCarc *init=NULL;
    
    leafArcs = new PCarc*[this->numLeaves];
    
	int i;
	for(i = 0;i<this->numLeaves;i++) {
		PCarc *a = new PCarc(),*b = new PCarc();
        
        //Set Twin Arcs
        b->twin = a;
        a->twin = b;
        
        //Let a be the one pointing to parent
        a->yParent = true;
        b->yParent = false;
        
        //Leaf Pointers
        leafArcs[i] = b;
        
        //Leaf Neighbours point to self
        b->a = b; b->b = b;
        
        //Set Degree - A node is represented by its arcs...
        a->degree = this->numLeaves;
        b->degree = 1;
        
        //Set Neighbours
        if(i!=0) {
            a->a = prev;
            prev->b = a;
        }
        else {
            init = a;
        }
        
        //For End Case
        if(i==numLeaves-1) {
            a->b = init;
            init->a = a;
        }
        
        prev = a;
	}
}

void PCtree::labelTree() {
    //Reset all counters and set all label to EMPTY
    std::map<PCarc*,bool> marked;
    
    PCarc* currArc = leafArcs[0];
    resetArcSet(currArc,marked);
    
    //std::cout << marked.size() << std::endl;
}

//Used in labelTree() for resetting all arcs
void PCtree::resetArcSet(PCarc* arc, std::map<PCarc*,bool> &marked) {
    //If already marked, return;
    if(marked[arc]==true) return;
    
    //Reset Arc
    arc->fullCounter = 0;
    arc->label = EMPTY;
    marked[arc] = true;
    
    //Perform Same Thing on Twin
    resetArcSet(arc->twin, marked);
    
    //End if there are no neighbours but itself
    if(arc->a == arc) return;
    
    resetArcSet(arc->a, marked);
    resetArcSet(arc->b, marked);
}
