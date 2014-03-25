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
    for(i=0;i<1;i++) {
        //Set Current Row
        this->currRow = i;
        
        partialArcs.clear();
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
    
    int i;
    for(i=0;i<numLeaves;i++) {
        if(M[this->currRow][i]==0) continue;
        
        setFullNode(leafArcs[i]);
    }
    
    /* Test
    for(i=0;i<numLeaves;i++) {
        PClabel ll = leafArcs[i]->label;
        if( ll == EMPTY ) {
            std::cout << "EMPTY" << std::endl;
        }
        else if( ll == PARTIAL ) {
            std::cout << "PARTIAL" << std::endl;
        }
        else if( ll == FULL ) {
            std::cout << "FULL" << std::endl;
        }
        else {
            std::cout << "ERRRROR" << std::endl;
        }
    } */
}

void PCtree::setFullNode(PCarc *arc) {
    if(arc->label == FULL) return;
    
    std::map<PCarc*,bool> marked;
    
    arc->label = FULL;
    marked[arc] = true;
    
    //Increment Counter of Twin
    incrementCounter(arc->twin);
    
    PCarc *currArc = arc->a;
    while(currArc != arc) {
        currArc->label = FULL;
        marked[currArc] = true;
        
        incrementCounter(currArc->twin);
        
        if(marked[currArc->a]==false) {
            currArc = currArc->a;
        }
        else {
            currArc = currArc->b;
        }
    }
}

//Increment counter of a node(set of arcs)
void PCtree::incrementCounter(PCarc *arc) {
    std::map<PCarc*,bool> marked;
    
    arc->fullCounter++;
    if(arc->fullCounter >= arc->degree-1) {
        setFullNode(arc);
        return;
    }
    
    //Set Partial Node
    arc->label = PARTIAL;

    marked[arc] = true;
    
    //Store Partial Arc
    storePartialArc(arc);
    
    PCarc *currArc = arc->a;
    while(currArc != arc) {
        currArc->label = PARTIAL;
        currArc->fullCounter++;
        marked[currArc] = true;
        
        if(marked[currArc->a]!=true) {
            currArc = currArc->a;
        }
        else {
            currArc = currArc->b;
        }
    }
}

void PCtree::storePartialArc(PCarc *arc) {
    partialArcs.push_back(arc);
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

void PCtree::getTerminalPath() {
    std::vector<PCarc*> terminalPath;
    
    //Foreach Partial Arc...
    if(partialArcs.size()==1) {
        
    }
    else {
        std::map<PCarc*,bool> marked;
        PCarc *currArc;
        //Initialize - Mark all Partial Node Arcs
        for(PCarc* arc : partialArcs) {
            marked[arc] = true;
            
            //Add To Terminal Path
            terminalPath.push_back(arc);
            
            currArc = arc->a;
            while(currArc != arc) {
                marked[currArc] = true;
                
                if(marked[currArc->a]!=true) currArc = currArc->a;
                else currArc = currArc->b;
            }
        }
        
        //Find Potential Apex Position
        std::vector<PCarc*> potentialApex;
        for(PCarc* arc: partialArcs) {
            //Find Parent
            PCarc* p = getParent(arc);
            
            //Add To Terminal Path
            terminalPath.push_back(p);
            
            //Check and Mark Parent Arcs
            if(marked[p]) {
                //FOUND COLLISION
                //STORE POTENTIAL APEX
                potentialApex.push_back(p);
                
                //POP ARC
                partialArcs.erase(partialArcs.begin());
                
                //CONTINUE
                continue;
            }
            else {
                //Mark all arcs around the parent node
                marked[p] = true;
                
                currArc = p->a;
                while(currArc != p) {
                    marked[currArc] = true;
                    
                    if(marked[currArc->a]!=true) currArc = currArc->a;
                    else currArc = currArc->b;
                }
                
                //POP ARC
                partialArcs.erase(partialArcs.begin());
                
                //ADD PARENT ARC
                partialArcs.push_back(p);
            }
        }
        
        //determine Highest Point from potentialApex / (or terminalPath)
        
        //Get Apex, while storing all between highest point and apex
        
        //Remove those from terminal path
        
        //Terminal path is now our final terminal path
        
    }
}

PCtree::PCarc* PCtree::getParent(PCarc* arc) {
    PCarc* parent;
    
    if(arc->yPnode != NULL && arc->yPnode->parentArc != NULL) {
        parent = arc->yPnode->parentArc;
    }
    else {
        std::map<PCarc*,bool> marked;
        
        PCarc* currArc = arc;
        while(true) {
            if(currArc->twin->yParent) {
                parent = currArc->twin;
                break;
            }
            else {
                marked[currArc] = true;
                
                currArc = marked[currArc->a]? currArc->b : currArc->a;
            }
            
            //IF NO PARENT...
            if(marked[currArc]) {
                std::cout << "ERROR getParent() NO PARENT" << std::endl;
                
                parent = NULL;
                break;
            }
        }
    }
    
    return parent;
}
