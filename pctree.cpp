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
        
        //get terminal path
        getTerminalPath();
        
        //Full Empty Sep
        splitTree();
        
        //Contraction
        contractionStep();
    }
}

//Contract all Cnode neighbours and nodes with only two neighbours (degree)
void PCtree::contractionStep() {
    std::map<PCarc*,bool> marked;
    
    PCarc* currArc = newCnode;
    int i;
    for(i=0;i<newCnode->degree;i++) {
        marked[currArc] = true;
        
        if(currArc->twin->degree <=2) {
            contractEdge(currArc);
        }
        else if(currArc->twin->yPnode == NULL) {
            contractEdge(currArc);
        }
        
        currArc = marked[currArc->a] ? currArc->b : currArc->a;
    }
}

void PCtree::contractEdge(PCtree::PCarc *edge) {
    if(edge->a->a == edge) {
        edge->a->a = edge->twin->a;
    }
    else if(edge->a->b == edge) {
        edge->a->b = edge->twin->a;
    }
    else {
        std::cout << "contractEdge PROBLEM1" << std::endl;
    }
    
    if(edge->b->a == edge) {
        edge->b->a = edge->twin->b;
    }
    else if(edge->b->b == edge) {
        edge->b->b = edge->twin->b;
    }
    else {
        std::cout << "contractEdge PROBLEM2" << std::endl;
    }
    
    if(edge->twin->a->a == edge->twin) {
        edge->twin->a->a = edge->a;
    }
    else if(edge->twin->a->b == edge->twin) {
        edge->twin->a->b = edge->a;
    }
    else {
        std::cout << "contractEdge PROBLEM3" << std::endl;
    }
    
    if(edge->twin->b->a == edge->twin) {
        edge->twin->b->a = edge->b;
    }
    else if(edge->twin->b->b == edge->twin) {
        edge->twin->b->b = edge->b;
    }
    else {
        std::cout << "contractEdge PROBLEM4" << std::endl;
    }
}

void PCtree::sortTerminalPath() {
    std::vector<PCarc*> sortedPath;
    
    if(terminalPath.size() <= 1) return;
    
    //Find one end
    std::map<PCarc*,bool> marked;
    PCarc* t1 = NULL, *t2 = NULL, *currArc;
    int counter;
    for(PCarc* arc : terminalPath) {
        counter = 0;
        
        currArc = arc;
        while(!marked[currArc]) {
            marked[currArc] = true;
            
            if(currArc->twin->label == PARTIAL) counter++;
            
            currArc = marked[currArc->a]? currArc->b : currArc->a;
        }
        
        //Found an end
        if(counter == 1) {
            if(t1 == NULL) t1 = arc;
            else t2 = arc;
        }
        
        assert(counter<=2);
        
        //Found Both Ends
        if(t1 != NULL && t2 != NULL) break;
    }
    
    sortedPath.push_back(t1);
    
    //Get rest of terminal path is sorted order
    PCarc* pr;
    currArc = t1;
    pr = getParent(currArc);
    bool isApex = false;
    //Push into sorted path until we reach the apex
    while(!isApex) {
        isApex = true;
        int i;
        for(i=0;i<terminalPath.size();i++) {
            PCarc* arc = terminalPath[i];
            if( isSameNode(arc, pr) ) {
                sortedPath.push_back(arc);
                
                currArc = arc;
                pr = getParent(currArc);
                
                terminalPath.erase(terminalPath.begin()+i);
                
                isApex = false;
                break;
            }
        }
        
        //isApex if parent of current arc is NOT in terminalPath
        if(isApex) {
            sortedPath.push_back(currArc);
        }
    }
    
    //currArc is currently apex position. Get remaining of sorted path.
    while(currArc != t2) {
        int i;
        for(i=0;i<terminalPath.size();i++) {
            PCarc* arc = terminalPath[i];
            if(isSameNode(getParent(arc),currArc)) {
                sortedPath.push_back(arc);
                currArc = arc;
                
                terminalPath.erase(terminalPath.begin()+i);
                break;
            }
        }
    }
    
    terminalPath.clear();
    terminalPath = sortedPath;
}

//Sets the degree for all arcs around a node
void PCtree::setDegree(PCtree::PCarc *node) {
    std::map<PCarc*,bool> marked;
    PCarc* currArc = node;
    
    int degree = 0;
    while(!marked[currArc]) {
        marked[currArc] = true;
        degree++;
        currArc = marked[currArc->a] ? currArc->b : currArc->a;
    }
    
    marked.clear();
    while(!marked[currArc]) {
        marked[currArc] = true;
        currArc->degree = degree;
        currArc = marked[currArc->a] ? currArc->b : currArc->a;
    }
}

void PCtree::splitTree() {
    //sort terminal path in order (from one end of path to other)
    sortTerminalPath();
    
    //Each terminalPath node, find child arc that is either EMPTY or FULL, store to terminalChild
    std::vector<PCarc*> terminalChild;
    std::map<PCarc*,bool> marked;
    PCarc* currArc;
    
    for(PCarc* arc: terminalPath) {
        marked.clear();
        currArc = arc;
        
        while(true) {
            marked[currArc] = true;
            if( currArc->twin->label == EMPTY || currArc->twin->label == FULL ) {
                terminalChild.push_back(currArc);
                break;
            }
            
            currArc = marked[currArc->a] ? currArc->b : currArc->a ;
        }
    }
    
    //For terminalChild, iterate to find boundaries
    //Determine the start of end positions of FULL and/or EMPTY
    //It is possible one of these are not there, so init as NULL
    //0,1,2,3 for fullA,fullB,emptyA,emptyB
    
    //Initialize terminalBounds
    std::vector<PCarc**> terminalBounds;
    int i;
    for(i=0;i<terminalPath.size();i++) {
        PCarc** bounds = new PCarc*[4];
        bounds[0] = NULL; bounds[1] = NULL; bounds[2] = NULL; bounds[3] = NULL;
        
        terminalBounds.push_back(bounds);
    }
    
    //Find Bounds
    PCarc* nextArc, *prevArc = NULL;
    for(i=0;i<terminalPath.size();i++) {
        PCarc* arc = terminalChild[i];
        
        nextArc = arc->a;
        currArc = arc;
        marked.clear();
        while(!marked[currArc]) {
            marked[currArc] = true;
            
            PClabel currLabel = currArc->twin->label;
            PClabel nextLabel = nextArc->twin->label;
            
            //Found boundary
            if(currLabel != nextLabel) {
                if(currLabel == FULL) terminalBounds[i][0] = currArc;
                else if(currLabel == EMPTY) terminalBounds[i][3] = currArc;
                
                if(nextLabel == FULL) terminalBounds[i][1] = nextArc;
                else if(nextLabel == EMPTY) terminalBounds[i][2] = nextArc;
            }
            
            prevArc = currArc;
            currArc = nextArc;
            nextArc = (nextArc->a == prevArc)? nextArc->b : nextArc->a;
        }
    }

    //Delete edges between nodes of terminal path
    if(terminalPath.size() > 1) {
        int i,j;
        for(i=0;i<terminalPath.size();i++) {
            for(j=i+1;j<terminalPath.size();j++) {
                if(isAdjacent(terminalPath[i], terminalPath[j])) {
                    removeEdge(terminalPath[i], terminalPath[j]);
                }
            }
        }
    }
    
    //For new C-node center, create new arc in between boundaries.
    //At same time, create the twin arc, set parent bit, set neighbours
    //
    //Do FULL ones first
    PCarc* initC, *prevC;
    PCarc* fullA, *fullB;
    for(i=0;i<terminalChild.size();i++) {
        PCarc* fullArc = new PCarc();
        
        //Store first and last arcs
        if(i==0) fullA = fullArc;
        if(i==terminalChild.size()-1) fullB = fullArc;
        
        //If FULL childs exists, then set neighbours
        if(terminalBounds[i][0] != NULL) {
            fullArc->a = terminalBounds[i][0];
            fullArc->b = terminalBounds[i][1];
            
            if(terminalBounds[i][2] == NULL) {
                if(terminalBounds[i][0]->a == terminalBounds[i][1]) terminalBounds[i][0]->a = fullArc;
                else if(terminalBounds[i][0]->b == terminalBounds[i][1]) terminalBounds[i][0]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND1" << std::endl;
                
                if(terminalBounds[i][1]->a == terminalBounds[i][0]) terminalBounds[i][1]->a = fullArc;
                else if(terminalBounds[i][1]->b == terminalBounds[i][0]) terminalBounds[i][1]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND2" << std::endl;
            }
            else {
                if(terminalBounds[i][0]->a == terminalBounds[i][2]) terminalBounds[i][0]->a = fullArc;
                else if(terminalBounds[i][0]->b == terminalBounds[i][2]) terminalBounds[i][0]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND3" << std::endl;
                
                if(terminalBounds[i][1]->a == terminalBounds[i][3]) terminalBounds[i][1]->a = fullArc;
                else if(terminalBounds[i][1]->b == terminalBounds[i][3]) terminalBounds[i][1]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND4" << std::endl;
            }
        }
        
        //Set Twin
        fullArc->twin = new PCarc();
        fullArc->twin->twin = fullArc;

        if(i==0) {
            initC = fullArc->twin;
        }
        else {
            fullArc->twin->a = prevC;
            prevC->b = fullArc->twin;
        }
        
        //if(i==terminalChild.size()-1) {
        //    fullArc->twin->b = initC;
        //    initC->a = fullArc->twin;
       // }
        
        prevC = fullArc->twin;
        
        //Set yParent
        fullArc->yParent = false;
        fullArc->twin->yParent = true;
        
        //Set Degree of Leaves
        int i;
        for(i=0;i<terminalChild.size();i++) {
            if(terminalBounds[i][0] != NULL) setDegree(terminalBounds[i][0]);
        }
        
        //Set new Pnode for FULL
        if(fullArc->yPnode != NULL) {
            setNewPnode(fullArc);
        }
    }
    
    //Do EMPTY ones now
    PCarc* emptA, *emptB;
    for(i=0;i<terminalChild.size();i++) {
        PCarc* fullArc = new PCarc();
        
        //Store first and last arcs
        if(i==0) emptA = fullArc;
        if(i==terminalChild.size()-1) emptB = fullArc;
        
        //If EMPTY childs exists, then set neighbours
        if(terminalBounds[i][2] != NULL) {
            fullArc->a = terminalBounds[i][2];
            fullArc->b = terminalBounds[i][3];
            
            if(terminalBounds[i][1] == NULL) {
                if(terminalBounds[i][2]->a == terminalBounds[i][3]) terminalBounds[i][2]->a = fullArc;
                else if(terminalBounds[i][2]->b == terminalBounds[i][3]) terminalBounds[i][2]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND5" << std::endl;
                
                if(terminalBounds[i][3]->a == terminalBounds[i][2]) terminalBounds[i][3]->a = fullArc;
                else if(terminalBounds[i][3]->b == terminalBounds[i][2]) terminalBounds[i][3]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND6" << std::endl;
            }
            else {
                if(terminalBounds[i][2]->a == terminalBounds[i][0]) terminalBounds[i][2]->a = fullArc;
                else if(terminalBounds[i][2]->b == terminalBounds[i][0]) terminalBounds[i][2]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND7" << std::endl;
                
                if(terminalBounds[i][3]->a == terminalBounds[i][1]) terminalBounds[i][3]->a = fullArc;
                else if(terminalBounds[i][3]->b == terminalBounds[i][1]) terminalBounds[i][3]->b = fullArc;
                else std::cout << "ERROR TERMINAL BOUND8" << std::endl;
            }
        }
        
        //Set Twin
        fullArc->twin = new PCarc();
        fullArc->twin->twin = fullArc;
        
        if(i==0) {
            initC = fullArc->twin;
        }
        else {
            fullArc->twin->a = prevC;
            prevC->b = fullArc->twin;
        }
        
        //if(i==terminalChild.size()-1) {
        //    fullArc->twin->b = initC;
        //    initC->a = fullArc->twin;
        //}
        
        prevC = fullArc->twin;
        
        //Set yParent
        fullArc->yParent = false;
        fullArc->twin->yParent = true;
        
        //Set Degree of Leaves
        int i;
        for(i=0;i<terminalChild.size();i++) {
            if(terminalBounds[i][2] != NULL) setDegree(terminalBounds[i][2]);
        }
        
        //Set new Pnode for FULL
        //if(fullArc->yPnode != NULL) {
        //    setNewPnode(fullArc);
        //}
    }
    
    //Link FULL and EMPTY arcs together
    fullA->twin->a = emptA->twin;
    emptA->twin->a = fullA->twin;
    
    fullB->twin->b = emptB->twin;
    emptB->twin->b = fullB->twin;
    
    //Set Degree of center C node
    newCnode = initC;
    setDegree(newCnode);

    assert(newCnode->degree == terminalChild.size()*2);
}

void PCtree::setNewPnode(PCtree::PCarc *node) {
    std::map<PCarc*,bool> marked;
    PCarc* currArc = node;
    
    Pnode* newPnode = new Pnode();
    
    while(!marked[currArc]) {
        marked[currArc] = true;
        currArc->yPnode = newPnode;
        currArc = marked[currArc->a] ? currArc->b : currArc->a;
    }
    
    newPnode->parentArc = getParent(node);
}

/*void PCtree::splitTree() {

    //Sort terminalPath in order first (if its not)
    
    //Each terminalPath node, find child arc that is either EMPTY or FULL, store
    std::vector<PCarc*> terminalChild;
    std::map<PCarc*,bool> marked;
    PCarc* currArc;
    
    for(PCarc* arc: terminalPath) {
        marked.clear();
        currArc = arc;
        
        while(true) {
            marked[currArc] = true;
            if( currArc->twin->label == EMPTY || currArc->twin->label == FULL ) {
                terminalChild.push_back(currArc);
                break;
            }
            
            currArc = marked[currArc->a] ? currArc->b : currArc->a ;
        }
    }
    
    
    
    //Get List of First/Last arcs of each Empty/Full pair for each terminal node
    std::vector<PCarc**> pathInfo;
    std::vector<PCarc**> pathInfoB;

    for(PCarc* a : terminalChild) {
        //Initialize New Set as array - 1st element for FULL, 2nd for EMPTY
        PCarc** newSet = new PCarc*[2];
        PCarc** endSet = new PCarc*[2];
        
        PCarc* prev = a;
        PCarc* curr = a->a;
        
        //If it has no neighbours...
        if(prev==curr) {
            if(a->label == FULL) {
                newSet[0] = a;
                newSet[1] = NULL;
                
                endSet[0] = a;
                endSet[1] = NULL;
            }
            else if(a->label == EMPTY) {
                newSet[0] = NULL;
                newSet[1] = a;
                
                endSet[0] = NULL;
                endSet[1] = a;
            }
            else {
                std::cout << "Problem! splitTree()!" << std::endl;
            }
        }
        else {
            bool foundA = false;
            std::map<PCarc*,bool> marked;
            marked.clear();
            while(!marked[curr]) {
                if(foundA) {
                    if(curr->twin->label != prev->twin->label &&
                       (curr->twin->label != PARTIAL && prev->twin->label != PARTIAL)) {
                        //Found Point of Interest
                        if(curr->twin->label == FULL) {
                            endSet[0] = curr;
                            endSet[1] = prev;
                        }
                        else if(curr->twin->label == EMPTY) {
                            endSet[1] = curr;
                            endSet[0] = prev;
                        }
                        else {
                            std::cout << "Problem! splitTree()! 5." << std::endl;
                        }
                        break;
                    }
                }
                else if(curr->twin->label != prev->twin->label &&
                        (curr->twin->label != PARTIAL && prev->twin->label != PARTIAL)) {
                    //Found Point of Interest
                    if(curr->twin->label == FULL) {
                        newSet[0] = curr;
                        newSet[1] = prev;
                    }
                    else if(curr->twin->label == EMPTY) {
                        newSet[1] = curr;
                        newSet[0] = prev;
                    }
                    else {
                        std::cout << "Problem! splitTree()! 2." << std::endl;
                    }
                    
                    foundA = true;
                }
                
                marked[curr] = true;
                prev = curr;
                curr = marked[curr->a]? curr->b : curr->a;
            }
            
            //If all leaves around node is same type...
            if(marked[curr]) {
                std::cout << "ALL SAME" << std::endl;
                
                //Find Partial Arc
                marked.clear();
                PCarc* c = curr;
                PCarc* pp;
                while(true) {
                    if(c->twin->label == PARTIAL) {
                        //GOOD
                        pp = c;
                        break;
                    }
                    
                    marked[c] = true;
                    c = marked[c->a]? c->b : c->a;
                }
                
                if(curr->twin->label == FULL) {
                    newSet[0] = pp->a;
                    newSet[1] = NULL;
                    
                    endSet[0] = pp->b;
                    endSet[1] = NULL;
                }
                else  {
                    newSet[1] = pp->a;
                    newSet[0] = NULL;
                    
                    endSet[1] = pp->b;
                    endSet[0] = NULL;
                }
                
            }

        }
        
        pathInfo.push_back(newSet);
        pathInfoB.push_back(endSet);
    }
    
    
    //Delete edges between nodes of terminalPath
    if(terminalPath.size() > 1) {
        int i,j;
        for(i=0;i<terminalPath.size();i++) {
            for(j=i+1;j<terminalPath.size();j++) {
                if(isAdjacent(terminalPath[i], terminalPath[j])) {
                    removeEdge(terminalPath[i], terminalPath[j]);
                }
            }
        }
    }
    
    //For each terminalPath arc, create 2 new PCarcs*. So there are TWO sets.
    //Set 1: Pointing at new C-Node, neighbours with eachother
    //Set 2: Half will have neighbours with FULL, half neighbours with EMPTY
    //We have to adjust neighbours of the arcs carefully... and set P-nodes if necesarry.
    int i;
    PCarc* prev, *init;
    for(i=0;i<terminalChild.size();i++) {
        PCarc *a = new PCarc(),*b = new PCarc();
        
        //Set Twin Arcs
        b->twin = a;
        a->twin = b;
        
        //Let a be the one pointing to parent, the C-Node
        a->yParent = true;
        b->yParent = false;
        
        
        //Set Degree
        a->degree = (int)(terminalChild.size()) * 2;
        
        //Set Neighbours of Split Nodes
        if( i < terminalChild.size()/2 ) {
            //Full First
            if( pathInfo[i][0] == NULL ) {
                //Point b neighbours at itself
            }
            else {
                //Find Other End of Full... May be the Same Arc
                std::map<PCarc*,bool> marked;
                
                PCarc* curr = pathInfo[i][0];
                while(true) {
                    marked[curr] = true;
                    //TODO:
                    curr = marked[curr->a]? curr->b : curr->a;
                }
                
                //Once found Both ends, Set neighbours of b
                
                //Afterwards, set degree of b
                
                //Create NEW P Node if necuessary. Set Pointers to PNode, and parent of Pnode.
            }
        }
        else {
            //Not do the empty ones. Same procedure, but indexing different.
            
            //Use Same P-Node, since FULL ones use a new one.
        }
        
        //Set Neighbours of C-Node
        if(i!=0) {
            a->a = prev;
            prev->b = a;
        }
        else {
            init = a;
        }
        //For End Case
        if(i==terminalChild.size()-1) {
            a->b = init;
            init->a = a;
        }
        prev = a;
    }
    
    //NEXT: Set Parent Bit Accordingly
}*/

//Returns true if two nodes are connected by an edge
bool PCtree::isAdjacent(PCtree::PCarc *nodeA, PCtree::PCarc *nodeB) {
    PCarc* pA = getParent(nodeA);
    PCarc* pB = getParent(nodeB);
    
    if( isSameNode(pA, nodeB) || isSameNode(pB, nodeA) ) return true;
    
    return false;
}

//Removes the edge between the two specified nodes
void PCtree::removeEdge(PCtree::PCarc *nodeA, PCtree::PCarc *nodeB) {
    std::map<PCarc*,bool> marked;
    
    //Mark all arcs of nodeA
    PCarc* currArc = nodeA;
    while(!marked[currArc]) {
        marked[currArc] = true;
        currArc = marked[currArc->a] ? currArc->b : currArc->a ;
    }
    
    //Find arc of nodeB whose twin is marked
    currArc = nodeB;
    while(!marked[currArc->twin]) {
        if(marked[currArc]) std::cout << "PROBLEM! removeEdge()! Cannot find common edge!" << std::endl;
        
        marked[currArc] = true;
        currArc = marked[currArc->a] ? currArc->b : currArc->a ;
    }
    
    removeEdge(currArc);
}

//Remove Arcs, Re-set cyclic neighbouring
void PCtree::removeEdge(PCtree::PCarc *arc) {
    PCarc* prev; PCarc* next;
    prev = arc->a;
    next = arc->b;
    
    if(prev->a == arc) {
        prev->a = next;
    }
    else {
        if(prev->b != arc) std::cout << "Warning! removeEdge()" << std::endl;
        prev->b = next;
    }
    
    if(next->a == arc) {
        next->a = prev;
    }
    else {
        if(next->b != arc) std::cout << "Warning! removeEdge()" << std::endl;
        next->b = prev;
    }
    
    //Do the same for twin arc
    prev = arc->twin->a;
    next = arc->twin->b;
    
    if(prev->a == arc->twin) {
        prev->a = next;
    }
    else {
        if(prev->b != arc->twin) std::cout << "Warning! removeEdge()" << std::endl;
        prev->b = next;
    }
    
    if(next->a == arc->twin) {
        next->a = prev;
    }
    else {
        if(next->b != arc->twin) std::cout << "Warning! removeEdge()" << std::endl;
        next->b = prev;
    }
    
    //Delete Arcs
    delete arc->twin;
    delete arc;
}

void PCtree::initializeTree() {
	Pnode* parent = new Pnode();
	
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
        
        //Set yPnode
        a->yPnode = parent;
        
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
    //Store arc only if arcs of the same node is not already stored
    std::map<PCarc*,bool> marked;
    PCarc* currArc;
    for(PCarc* a : partialArcs) {
        marked.clear();
        currArc = a;
        
        while(!marked[currArc]) {
            marked[currArc] = true;
            if( currArc == arc ) {
                return;
            }
            
            currArc = marked[currArc->a] ? currArc->b : currArc->a ;
        }
    }
    
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
    terminalPath.clear();
    
    //Foreach Partial Arc...
    if(partialArcs.size()==1) {
        std::cout << "Only 1 Partial Node" << std::endl;
        
        terminalPath.push_back(partialArcs[0]);
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
        
        //Find Potential Apex Position/Terminal Path nodes
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
                
                //...or cheat. End here, and say this is apex
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
        
        terminalPathClean();
        
        
    }
}

//Used in Find Terminal Path to remove nodes above apex
void PCtree::terminalPathClean() {
    std::map<PCarc*,bool> marked;
    PCarc *currArc;
    
    //determine Highest Point
    PCarc* highestArc = NULL;
    bool tt;
    int i, highestNum;
    PCarc* k;
    
    for(i=0;i<terminalPath.size();i++) {
        
        k = terminalPath[i];
        tt = true;
        for( PCarc* q : terminalPath ) {
            //Ignore same arc
            if(q==k) continue;
            
            if(!isHigherArc(k, q)) {
                tt = false;
                break;
            }
        }
        
        if(tt) {
            highestArc = k;
            highestNum = i;
            break;
        }
    }
    
    //Get Apex, while storing all between highest point and apex
    //Check if childs are PARTIAL, or if two of childs are from terminal path
    bool isApex = false;
    marked.clear();
    currArc = highestArc;
    
    while(!marked[currArc]) {
        if(currArc->twin->label == PARTIAL) {
            isApex = true;
            break;
        }
        
        marked[currArc] = true;
        currArc = marked[currArc->a]?currArc->b:currArc->a;
    }
    
    //If no childs are partial, check if two paths entering are from terminal path
    marked.clear();
    currArc = highestArc;
    if(!isApex) {
        int numEntering = 0;
        
        while(!marked[currArc]) {
            
            for(PCarc* q : terminalPath) {
                if(q==highestArc) continue;
                
                if(isSameNode(currArc->twin,q)) {
                    numEntering++;
                    break;
                }
            }
            
            if(numEntering>=2) {
                isApex = true;
                break;
            }
            
            marked[currArc] = true;
            currArc = marked[currArc->a]?currArc->b:currArc->a;
        }
    }
    
    //If apex is found, then DONE!
    if(isApex) {
        return;
    }
    else {
        //Remove Highest point and REPEAT.
        terminalPath.erase(terminalPath.begin()+highestNum);
        terminalPathClean();
    }
}

//Used in Find Terminal Path ONLY. Returns true IFF a is higher than b in terminal path.
bool PCtree::isHigherArc(PCtree::PCarc *a, PCtree::PCarc *b) {
    PCarc* p = getParent(b);
    while(p != NULL) {
        
        if( isSameNode(a,p) ) {
            return true;
        }
        
        p = getParent(p);
    }
    
    return false;
}

//returns true IFF arc a and b are pointing towards same node
bool PCtree::isSameNode(PCtree::PCarc *a, PCtree::PCarc *b) {
    std::map<PCarc*,bool> marked;
    PCarc* currArc = a;

    while(!marked[currArc]) {
        marked[currArc] = true;
        
        if(currArc == b) return true;
        
        currArc = marked[currArc->a] ? currArc->b : currArc->a ;
    }
    return false;
}

//Returns an arc whose node is parent of input arc's node
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
