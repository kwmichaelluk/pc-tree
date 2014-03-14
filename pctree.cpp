#include "pctree.h"

PCtree::PCtree(int** m) {

}

void PCtree::initializeTree() {
	Pnode parent;
	
    PCarc *prev=NULL;
	PCarc *init=NULL;
    
    leafArcs = new PCarc[numLeaves];
    
	int i;
	for(i = 0;i<numLeaves;i++){
		PCarc a,b;
        
        //Set Twin Arcs
        b.twin = &a;
        a.twin = &b;
        
        //Let a be the one pointing to parent
        a.yParent = true;
        b.yParent = false;
        
        //Leaf Pointers
        leafArcs[i] = b;
        
        //Leaf Neighbours point to self
        b.a = &b; b.b = &b;
        
        //Set Neighbours
        if(i!=0) {
            a.a = prev;
            prev->b = &a;
        }
        else {
            init = &a;
        }
        
        //For End Case
        if(i==numLeaves-1) {
            a.b = init;
            init->a = &a;
        }
        
        prev = &a;
	}
		  

}
