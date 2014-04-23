/*
 * Class_Local_Tree_2D.tpp
 *
 *  Created on: 18/apr/2014
 *      Author: Marco Cisternino
 */


{
// =================================================================================== //
// CLASS SPECIALIZATION                                                                //
// =================================================================================== //

template<>
class Class_Local_Tree<2>{
	// ------------------------------------------------------------------------------- //
	// FRIENDSHIPS ------------------------------------------------------------------- //
	friend class Class_Para_Tree;

	// ------------------------------------------------------------------------------- //
	// TYPEDEFS ----------------------------------------------------------------------- //
public:
	typedef vector< Class_Octant<2> > 			OctantsType;
	typedef vector< Class_Intersection<2> > 	IntersectionsType;
	typedef vector<uint32_t>					u32vector;
	typedef vector<vector<uint32_t>	>			u32vector2D;
	typedef vector<vector<uint64_t>	>			u64vector2D;


	// ------------------------------------------------------------------------------- //
	// MEMBERS ----------------------------------------------------------------------- //

private:
	OctantsType					octants;			// Local vector of octants ordered with Morton Number
	OctantsType					ghosts;				// Local vector of ghost octants ordered with Morton Number
	IntersectionsType			intersections_int;	// Local vector of internal intersections ordered with Morton Number of first owner octant
	IntersectionsType			intersections_ghost;// Local vector of intersections internal/ghost ordered with Morton Number of internal owner octant
	IntersectionsType			intersections_bord;	// Local vector of border intersections (twice the sam octant is stored in an intersection)
	u32vector 					pborders;			// Local vector of pborder octants ordered with Morton Number
	Class_Octant<2> 			first_desc;			// First (Morton order) most refined octant possible in local partition
	Class_Octant<2> 			last_desc;			// Last (Morton order) most refined octant possible in local partition
	uint32_t 					size_ghosts;		// Size of vector of ghost octants
	uint8_t						local_max_depth;	// Reached max depth in local tree
public:
	u32vector2D					nodes;				// Local vector of nodes (x,y,z) ordered with Morton Number
	u32vector2D					connectivity;		// Local vector of connectivity (node1, node2, ...) ordered with Morton-order.
													// The nodes are stored as index of vector nodes
	u32vector2D					ghostsnodes;		// Local vector of ghosts nodes (x,y,z) ordered with Morton Number
	u32vector2D					ghostsconnectivity;	// Local vector of ghosts connectivity (node1, node2, ...) ordered with Morton-order.
													// The nodes are stored as index of vector nodes

	// ------------------------------------------------------------------------------- //
	// CONSTRUCTORS ------------------------------------------------------------------ //

public:
	Class_Local_Tree(){
		Class_Octant<2> oct0;
		Class_Octant<2> octf(MAX_LEVEL_2D,0,0);
		Class_Octant<2> octl(MAX_LEVEL_2D,global2D.max_length-1,global2D.max_length-1);
		octants.resize(1);
		octants[0] = oct0;
		first_desc = octf;
		last_desc = octl;
		size_ghosts = 0;
		local_max_depth = 0;

	};
	~Class_Local_Tree(){};

	// ------------------------------------------------------------------------------- //
	// METHODS ----------------------------------------------------------------------- //

	// Basic Get/Set methods --------------------------------------------------------- //

public:
	const Class_Octant<2> &  getFirstDesc() const{
		return first_desc;
	};
	const Class_Octant<2> &  getLastDesc() const{
		return last_desc;
	};
	uint32_t getSizeGhost() const{
		return size_ghosts;
	};
	uint32_t getNumOctants() const{
		return octants.size();
	};
	uint8_t getLocalMaxDepth() const{							// Get max depth reached in local tree
		return local_max_depth;
	};
	uint8_t getMarker(int32_t idx){								// Get refinement/coarsening marker for idx-th octant
		return octants[idx].getMarker();
	};
	bool getBalance(int32_t idx){								// Get if balancing-blocked idx-th octant
		return octants[idx].getNotBalance();
	};

	void setMarker(int32_t idx, int8_t marker){					// Set refinement/coarsening marker for idx-th octant
		octants[idx].setMarker(marker);
	};
	void setBalance(int32_t idx, bool balance){					// Set if balancing-blocked idx-th octant
		octants[idx].setBalance(balance);
	};
	void setFirstDesc(){
		OctantsType::const_iterator firstOctant = octants.begin();
		first_desc = Class_Octant<2>(MAX_LEVEL_2D,firstOctant->x,firstOctant->y);
	};
	void setLastDesc(){
		OctantsType::const_iterator lastOctant = octants.end() - 1;
		uint32_t x,y,delta;
		delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - lastOctant->level)) - 1;
		x = lastOctant->x + delta;
		y = lastOctant->y + delta;
		last_desc = Class_Octant<2>(MAX_LEVEL_2D,x,y);

	};

	//-------------------------------------------------------------------------------- //
	// Debug methods ----------------------------------------------------------------- //
	void addOctantToTree(Class_Octant<2> octant){
		octants.push_back(octant);
		octants.shrink_to_fit();
	};

private:

	//-------------------------------------------------------------------------------- //
	// Other Get/Set methods --------------------------------------------------------- //

public:

private:

	//-------------------------------------------------------------------------------- //
	// Other methods ----------------------------------------------------------------- //

public:
	const Class_Octant<2>&	extractOctant(uint32_t idx) const{
		return octants[idx];
	};

	// =================================================================================== //

	bool refine(){									// Refine local tree: refine one time octants with marker >0

		// Local variables
		vector<uint32_t> last_child_index;
		Class_Octant<2>* children;
		uint32_t idx, nocts;
		uint32_t offset = 0, blockidx;
		uint8_t nchm1 = global2D.nchildren-1, ich, iface;
		bool dorefine = false;

		nocts = octants.size();
		for (idx=0; idx<nocts; idx++){
			if(octants[idx].getMarker() > 0 && octants[idx].getLevel() < MAX_LEVEL_2D){
				last_child_index.push_back(idx+nchm1+offset);
				offset += nchm1;
			}
			else{
	//			octants[idx].info[12] = false;
				if (octants[idx].marker > 0)
					octants[idx].marker = 0;
			}
		}
		if (offset > 0){
			octants.resize(octants.size()+offset);
			blockidx = last_child_index[0]-nchm1;
			idx = octants.size();
			while (idx>blockidx){
				idx--;
				//TODO Sostituire questo if con il controllo su last_index_child
				if(octants[idx-offset].getMarker() > 0 && octants[idx-offset].getLevel() < MAX_LEVEL_2D){
					children = octants[idx-offset].buildChildren();
					for (ich=0; ich<global2D.nchildren; ich++){
						octants[idx-ich] = (children[nchm1-ich]);
					}
					offset -= nchm1;
					idx -= nchm1;
					//Update local max depth
					if (children[0].getLevel() > local_max_depth){
						local_max_depth = children[0].getLevel();
					}
					if (children[0].getMarker() > 0){
						//More Refinement to do
						dorefine = true;
					}
					delete []children;
				}
				else {
					octants[idx] = octants[idx-offset];
				}
			}
		}
		octants.shrink_to_fit();

		//Update pborders (adesso inefficiente, loop di nuovo su tutti gli elementi)
		//Si può trovare la maniera di inserirlo nel loop precedente
		pborders.clear();
		nocts = octants.size();
		pborders.reserve(int(pow(double(nocts),2.0/3.0)*double(global2D.nfaces)));
		for(idx=0; idx<nocts; idx++){
			for(iface=0; iface<global2D.nfaces; iface++){
				if (octants[idx].info[iface+global2D.nfaces]){
					pborders.push_back(idx);
					break;
				}
			}
		}
		pborders.shrink_to_fit();

		setFirstDesc();
		setLastDesc();

		return dorefine;

	};

	// =================================================================================== //

	bool coarse(){												// Coarse local tree: coarse one time family of octants with marker <0
		// Local variables										// (if at least one octant of family has marker>=0 set marker=0 for the entire family)
		vector<uint32_t> first_child_index;
		Class_Octant<2> father;
		uint32_t ich, nocts, nghosts;
		int32_t idx, idx2;
		uint32_t offset;
		int32_t idx1_gh, idx2_gh;
		uint32_t nidx;
		int8_t markerfather, marker;
		uint8_t nbro, nstart, nend;
		uint8_t nchm1 = global2D.nchildren-1, iface;
		bool docoarse = false;

		//------------------------------------------ //
		// Initialization

		nbro = nstart = nend = 0;
		nidx = offset = 0;

		idx1_gh = idx2_gh = 0;

		nocts   = octants.size();
		size_ghosts = ghosts.size();


		// Init first and last desc (even if already calculated)
		setFirstDesc();
		setLastDesc();

		//------------------------------------------ //

		// Set index for start and end check for ghosts
		if (ghosts.size()){
			while(idx1_gh < size_ghosts && ghosts[idx1_gh].computeMorton() < first_desc.computeMorton()){
				idx1_gh++;
			}
			idx1_gh = max(0, idx1_gh-1);
			while(idx2_gh < size_ghosts && ghosts[idx2_gh].computeMorton() < last_desc.computeMorton()){
				idx2_gh++;
			}
			idx2_gh = min(int(size_ghosts-1), idx2_gh);

			// Start on ghosts
			if ((ghosts[idx1_gh].getMarker() < 0) & (octants[0].getMarker() < 0)){
				father = ghosts[idx1_gh].buildFather();
				nbro = 0;
				idx = idx1_gh;
				marker = ghosts[idx].getMarker();
				while(marker < 0 & ghosts[idx].buildFather() == father){
					nbro++;
					marker = ghosts[idx].getMarker();
					idx--;
					if (idx<0){
						break;
					}
				}
				nstart = 0;
				idx = 0;
				marker = octants[idx].getMarker();
				while(marker<0 & octants[idx].buildFather() == father){
					nbro++;
					marker = octants[idx].getMarker();
					nstart++;
					idx++;
					if (idx==nocts){
						break;
					}
				}
				if (nbro == global2D.nchildren){
	//				offset = nstart;
					// For update pbound of neighbours only check
					// the odd faces of new father (placed nstart-times
					// in the first nstart positions of octants)
					// If there is father after coarse will be the first
					// element of local octants (lowest Morton)
	/*
					for (int i=0; i<nstart; i++){
						octants[i] = father;
					}
	*/
					uint32_t	 sizeneigh;
					u32vector    neigh;
					vector<bool> isghost;
					for (iface=0; iface<3; iface++){
						uint8_t oddface = ((iface*2)+1);
						findNeighbours(nstart-1, oddface, neigh, isghost);
						sizeneigh = neigh.size();
						for(int i=0; i<sizeneigh; i++){
							if (!isghost[i])
								octants[neigh[i]].setPbound(global2D.oppface[oddface], true);
						}
					}
				}
				else{
					nstart = 0;
				}
			}
		}

		// Check and coarse internal octants
		for (idx=0; idx<nocts; idx++){
			if(octants[idx].getMarker() < 0 && octants[idx].getLevel() > 0){
				nbro = 0;
				father = octants[idx].buildFather();
				// Check if family is to be refined
				for (idx2=idx; idx2<idx+global2D.nchildren; idx2++){
					if (idx2<nocts-1){
						if(octants[idx2].getMarker() < 0 && octants[idx2].buildFather() == father){
							nbro++;
						}
					}
				}
				if (nbro == global2D.nchildren){
					nidx++;
					first_child_index.push_back(idx);
					idx = idx2-1;
				}
				else{
					if (idx < (nocts>global2D.nchildren)*(nocts-global2D.nchildren)){
						octants[idx].setMarker(0);
					}
				}
			}
			else{
	//			octants[idx].info[13] = false;
			}
		}
		//TODO Da mettere dentro il primo ciclo per renderlo meno costoso
		if (nidx!=0){
			uint32_t nblock = nocts - nidx*nchm1 - nstart;
			nidx = 0;
			for (idx=0; idx<nblock; idx++){
				if (idx+offset == first_child_index[nidx]){
					markerfather = -MAX_LEVEL_2D;
					father = octants[idx+offset].buildFather();
					for(idx2=0; idx2<global2D.nchildren; idx2++){
						if (markerfather < octants[idx+offset+idx2].getMarker()+1){
							markerfather = octants[idx+offset+idx2].getMarker()+1;
						}
						for (iface=0; iface<global2D.nfaces; iface++){
							father.info[iface] = (father.info[iface] || octants[idx+offset+idx2].info[iface]);
							father.info[iface+global2D.nfaces] = (father.info[iface+global2D.nfaces] || octants[idx+offset+idx2].info[iface+global2D.nfaces]);
						}
					}
					father.info[13] = true;
					father.setMarker(markerfather);
					if (markerfather < 0){
						docoarse = true;
					}
					octants[idx] = father;
					offset += nchm1;
					nidx++;
				}
				else{
					octants[idx] = octants[idx+offset];
				}
			}
		}
		octants.resize(nocts-offset);
		octants.shrink_to_fit();
		nocts = octants.size();


		// End on ghosts
		if (ghosts.size() && nocts > 0){
			if ((ghosts[idx2_gh].getMarker() < 0) & (octants[nocts-1].getMarker() < 0)){
				father = ghosts[idx2_gh].buildFather();
				markerfather = -MAX_LEVEL_2D;
				nbro = 0;
				idx = idx2_gh;
				marker = ghosts[idx].getMarker();
				while(marker < 0 & ghosts[idx].buildFather() == father){
					nbro++;
					marker = ghosts[idx].getMarker();
					if (markerfather < ghosts[idx].getMarker()+1){
						markerfather = ghosts[idx].getMarker()+1;
					}
					idx++;
					if(idx == size_ghosts){
						break;
					}
				}
				nend = 0;
				idx = nocts-1;
				marker = octants[idx].getMarker();
				while(marker < 0 & octants[idx].buildFather() == father & idx >= 0){
					nbro++;
					marker = octants[idx].getMarker();
					if (markerfather < octants[idx+offset+idx2].getMarker()+1){
						markerfather = octants[idx+offset+idx2].getMarker()+1;
					}
					nend++;
					idx--;
					if (idx<0){
						break;
					}
				}
				if (nbro == global2D.nchildren){
					offset = nend;
				}
				else{
					nend = 0;
					for(int ii=nocts-global2D.nchildren; ii<nocts; ii++){
						octants[ii].setMarker(0);
					}
				}
			}
			if (nend != 0){
				for (idx=0; idx < nend; idx++){
					for (iface=0; iface<global2D.nfaces; iface++){
						father.info[iface] = (father.info[iface] || octants[nocts-idx].info[iface]);
						father.info[iface+global2D.nfaces] = (father.info[iface+global2D.nfaces] || octants[nocts-idx].info[iface+global2D.nfaces]);
					}
				}
				father.info[13] = true;
				father.setMarker(markerfather);
				if (markerfather < 0){
					docoarse = true;
				}
				octants.resize(nocts-offset);
				octants.push_back(father);
				octants.shrink_to_fit();
				nocts = octants.size();
			}

		}

		//Update pborders (adesso inefficiente, loop di nuovo su tutti gli elementi)
		//Si può trovare la maniera di inserirlo nel loop precedente
		pborders.clear();
		nocts = octants.size();
		pborders.reserve(int(pow(double(nocts),2.0/3.0)*double(global2D.nfaces)));
		for(idx=0; idx<nocts; idx++){
			for(iface=0; iface<global2D.nfaces; iface++){
				if (octants[idx].info[iface+global2D.nfaces]){
					pborders.push_back(idx);
					break;
				}
			}
		}
		pborders.shrink_to_fit();

		// Set final first and last desc
		if(nocts>0){
			setFirstDesc();
			setLastDesc();
		}
		return docoarse;

	};

	// =================================================================================== //

	bool refine(u32vector & mapidx){							// Refine local tree: refine one time octants with marker >0
																// mapidx[i] = index in old octants vector of the i-th octant (index of father if octant is new after)
		// Local variables
		vector<uint32_t> last_child_index;
		Class_Octant<2>* children;
		uint32_t idx, nocts;
		uint32_t offset = 0, blockidx;
		uint8_t nchm1 = global2D.nchildren-1, ich, iface;
		bool dorefine = false;

		nocts = octants.size();
		for (idx=0; idx<nocts; idx++){
			if(octants[idx].getMarker() > 0 && octants[idx].getLevel() < MAX_LEVEL_2D){
				last_child_index.push_back(idx+nchm1+offset);
				offset += nchm1;
			}
			else{
	//			octants[idx].info[12] = false;
				if (octants[idx].marker > 0)
					octants[idx].marker = 0;
			}
		}
		if (offset > 0){
			mapidx.resize(octants.size()+offset);
			mapidx.shrink_to_fit();

			octants.resize(octants.size()+offset);
			blockidx = last_child_index[0]-nchm1;
			idx = octants.size();
			//while (idx>blockidx){
			while (idx>0){
				idx--;
				//TODO Sostituire questo if con il controllo su last_index_child
				if(octants[idx-offset].getMarker() > 0 && octants[idx-offset].getLevel() < MAX_LEVEL_2D){
					children = octants[idx-offset].buildChildren();
					for (ich=0; ich<global2D.nchildren; ich++){
						octants[idx-ich] = (children[nchm1-ich]);
						mapidx[idx-ich]  = mapidx[idx-offset];
					}
					offset -= nchm1;
					idx -= nchm1;
					//Update local max depth
					if (children[0].getLevel() > local_max_depth){
						local_max_depth = children[0].getLevel();
					}
					if (children[0].getMarker() > 0){
						//More Refinement to do
						dorefine = true;
					}
					delete []children;
				}
				else {
					octants[idx] = octants[idx-offset];
					mapidx[idx]  = mapidx[idx-offset];
				}
			}
		}
		octants.shrink_to_fit();

		//Update pborders (adesso inefficiente, loop di nuovo su tutti gli elementi)
		//Si può trovare la maniera di inserirlo nel loop precedente
		pborders.clear();
		nocts = octants.size();
		pborders.reserve(int(pow(double(nocts),2.0/3.0)*double(global2D.nfaces)));
		for(idx=0; idx<nocts; idx++){
			for(iface=0; iface<global2D.nfaces; iface++){
				if (octants[idx].info[iface+global2D.nfaces]){
					pborders.push_back(idx);
					break;
				}
			}
		}
		pborders.shrink_to_fit();

		setFirstDesc();
		setLastDesc();

		return dorefine;

	};

	// =================================================================================== //

	bool coarse(u32vector & mapidx){							// Coarse local tree: coarse one time family of octants with marker <0
																// (if at least one octant of family has marker>=0 set marker=0 for the entire family)
																// mapidx[i] = index in old octants vector of the i-th octant (index of father if octant is new after)
		// Local variables
		vector<uint32_t> first_child_index;
		Class_Octant<2> father;
		uint32_t ich, nocts, nghosts, nocts0;
		int32_t idx, idx2;
		uint32_t offset;
		int32_t idx1_gh, idx2_gh;
		uint32_t nidx;
		int8_t markerfather, marker;
		uint8_t nbro, nstart, nend;
		uint8_t nchm1 = global2D.nchildren-1, iface;
		bool docoarse = false;

		//------------------------------------------ //
		// Initialization

		nbro = nstart = nend = 0;
		nidx = offset = 0;

		idx1_gh = idx2_gh = 0;

		nocts = nocts0 = octants.size();
		size_ghosts = ghosts.size();


		// Init first and last desc (even if already calculated)
		setFirstDesc();
		setLastDesc();

		//------------------------------------------ //

		// Set index for start and end check for ghosts
		if (ghosts.size()){
			while(idx1_gh < size_ghosts && ghosts[idx1_gh].computeMorton() < first_desc.computeMorton()){
				idx1_gh++;
			}
			idx1_gh = max(0, idx1_gh-1);
			while(idx2_gh < size_ghosts && ghosts[idx2_gh].computeMorton() < last_desc.computeMorton()){
				idx2_gh++;
			}
			idx2_gh = min(int(size_ghosts-1), idx2_gh);

			// Start on ghosts
			if ((ghosts[idx1_gh].getMarker() < 0) & (octants[0].getMarker() < 0)){
				father = ghosts[idx1_gh].buildFather();
				nbro = 0;
				idx = idx1_gh;
				marker = ghosts[idx].getMarker();
				while(marker < 0 & ghosts[idx].buildFather() == father){
					nbro++;
					marker = ghosts[idx].getMarker();
					idx--;
					if (idx<0){
						break;
					}
				}
				nstart = 0;
				idx = 0;
				marker = octants[idx].getMarker();
				while(marker<0 & octants[idx].buildFather() == father){
					nbro++;
					marker = octants[idx].getMarker();
					nstart++;
					idx++;
					if (idx==nocts){
						break;
					}
				}
				if (nbro == global2D.nchildren){
	//				offset = nstart;
					// For update pbound of neighbours only check
					// the odd faces of new father (placed nstart-times
					// in the first nstart positions of octants)
					// If there is father after coarse will be the first
					// element of local octants (lowest Morton)
	/*
					for (int i=0; i<nstart; i++){
						octants[i] = father;
					}
	*/
					uint32_t	 sizeneigh;
					u32vector    neigh;
					vector<bool> isghost;
					for (iface=0; iface<3; iface++){
						uint8_t oddface = ((iface*2)+1);
						findNeighbours(nstart-1, oddface, neigh, isghost);
						sizeneigh = neigh.size();
						for(int i=0; i<sizeneigh; i++){
							if (!isghost[i])
								octants[neigh[i]].setPbound(global2D.oppface[oddface], true);
						}
					}
				}
				else{
					nstart = 0;
				}
			}
		}

		// Check and coarse internal octants
		for (idx=0; idx<nocts; idx++){
			if(octants[idx].getMarker() < 0 && octants[idx].getLevel() > 0){
				nbro = 0;
				father = octants[idx].buildFather();
				// Check if family is to be refined
				for (idx2=idx; idx2<idx+global2D.nchildren; idx2++){
					if (idx2<nocts-1){
						if(octants[idx2].getMarker() < 0 && octants[idx2].buildFather() == father){
							nbro++;
						}
					}
				}
				if (nbro == global2D.nchildren){
					nidx++;
					first_child_index.push_back(idx);
					idx = idx2-1;
				}
				else{
					if (idx < (nocts>global2D.nchildren)*(nocts-global2D.nchildren)){
						octants[idx].setMarker(0);
					}
				}
			}
			else{
	//			octants[idx].info[13] = false;
			}
		}
		//TODO Da mettere dentro il primo ciclo per renderlo meno costoso
		if (nidx!=0){
			uint32_t nblock = nocts - nidx*nchm1 - nstart;
			nidx = 0;
			//for (idx=0; idx<nblock; idx++){
			for (idx=0; idx<nocts-offset; idx++){
				if (idx+offset == first_child_index[nidx]){
					markerfather = -MAX_LEVEL_2D;
					father = octants[idx+offset].buildFather();
					for(idx2=0; idx2<global2D.nchildren; idx2++){
						if (markerfather < octants[idx+offset+idx2].getMarker()+1){
							markerfather = octants[idx+offset+idx2].getMarker()+1;
						}
						for (iface=0; iface<global2D.nfaces; iface++){
							father.info[iface] = (father.info[iface] || octants[idx+offset+idx2].info[iface]);
							father.info[iface+global2D.nfaces] = (father.info[iface+global2D.nfaces] || octants[idx+offset+idx2].info[iface+global2D.nfaces]);
						}
					}
					father.info[13] = true;
					father.setMarker(markerfather);
					if (markerfather < 0){
						docoarse = true;
					}
					octants[idx] = father;
					mapidx[idx] = mapidx[idx+offset];
					offset += nchm1;
					nidx++;
				}
				else{
					octants[idx] = octants[idx+offset];
					mapidx[idx] = mapidx[idx+offset];
				}
			}
		}
		octants.resize(nocts-offset);
		octants.shrink_to_fit();
		nocts = octants.size();
		mapidx.resize(nocts-offset);
		mapidx.shrink_to_fit();


		// End on ghosts
		if (ghosts.size() && nocts > 0){
			if ((ghosts[idx2_gh].getMarker() < 0) & (octants[nocts-1].getMarker() < 0)){
				father = ghosts[idx2_gh].buildFather();
				markerfather = -MAX_LEVEL_2D;
				nbro = 0;
				idx = idx2_gh;
				marker = ghosts[idx].getMarker();
				while(marker < 0 & ghosts[idx].buildFather() == father){
					nbro++;
					marker = ghosts[idx].getMarker();
					if (markerfather < ghosts[idx].getMarker()+1){
						markerfather = ghosts[idx].getMarker()+1;
					}
					idx++;
					if(idx == size_ghosts){
						break;
					}
				}
				nend = 0;
				idx = nocts-1;
				marker = octants[idx].getMarker();
				while(marker < 0 & octants[idx].buildFather() == father & idx >= 0){
					nbro++;
					marker = octants[idx].getMarker();
					if (markerfather < octants[idx+offset+idx2].getMarker()+1){
						markerfather = octants[idx+offset+idx2].getMarker()+1;
					}
					nend++;
					idx--;
					if (idx<0){
						break;
					}
				}
				if (nbro == global2D.nchildren){
					offset = nend;
				}
				else{
					nend = 0;
					for(int ii=nocts-global2D.nchildren; ii<nocts; ii++){
						octants[ii].setMarker(0);
					}
				}
			}
			if (nend != 0){
				for (idx=0; idx < nend; idx++){
					for (iface=0; iface<global2D.nfaces; iface++){
						father.info[iface] = (father.info[iface] || octants[nocts-idx].info[iface]);
						father.info[iface+global2D.nfaces] = (father.info[iface+global2D.nfaces] || octants[nocts-idx].info[iface+global2D.nfaces]);
					}
				}
				father.info[13] = true;
				father.setMarker(markerfather);
				if (markerfather < 0){
					docoarse = true;
				}
				octants.resize(nocts-offset);
				octants.push_back(father);
				octants.shrink_to_fit();
				nocts = octants.size();
				mapidx.resize(nocts-offset);
				mapidx.push_back(nocts0-nend);
				mapidx.shrink_to_fit();
			}

		}

		//Update pborders (adesso inefficiente, loop di nuovo su tutti gli elementi)
		//Si può trovare la maniera di inserirlo nel loop precedente
		pborders.clear();
		nocts = octants.size();
		pborders.reserve(int(pow(double(nocts),2.0/3.0)*double(global2D.nfaces)));
		for(idx=0; idx<nocts; idx++){
			for(iface=0; iface<global2D.nfaces; iface++){
				if (octants[idx].info[iface+global2D.nfaces]){
					pborders.push_back(idx);
					break;
				}
			}
		}
		pborders.shrink_to_fit();

		// Set final first and last desc
		if(nocts>0){
			setFirstDesc();
			setLastDesc();
		}
		return docoarse;

	};

	// =================================================================================== //

	void checkCoarse(uint64_t lastDescPre,						// Delete overlapping octants after coarse local tree. Check first and last descendants
								uint64_t firstDescPost){		// of process before and after the local process
		int32_t idx;
		uint32_t nocts;
		uint64_t Morton;
		uint8_t toDelete = 0;

		nocts = getNumOctants();
		idx = 0;
		Morton = octants[idx].computeMorton();
		while(Morton <= lastDescPre & idx < nocts & Morton != 0){
			// To delete, the father is in proc before me
			toDelete++;
			idx++;
			Morton = octants[idx].computeMorton();
		}
		for(idx=0; idx<nocts-toDelete; idx++){
			octants[idx] = octants[idx+toDelete];
		}
		octants.resize(nocts-toDelete);
		octants.shrink_to_fit();

		toDelete = 0;
		Morton = last_desc.computeMorton();
		if(int(firstDescPost  - Morton) > 1){
			// To insert, the father is not yet here!!
			idx = nocts - 1;
			Class_Octant<2> father = octants[idx].buildFather();
			while(octants[idx].buildFather() == father & idx >= 0){
				toDelete++;
				idx--;
			}
			father.info[global2D.nfaces+1] = father.info[global2D.nfaces+3] = true;
			if(global2D.nfaces == 6)
				father.info[global2D.nfaces+5] = true;
			octants.resize(nocts-toDelete);
			octants.push_back(father);
			octants.shrink_to_fit();
		}

		//Update pborders (adesso inefficiente, loop di nuovo su tutti gli elementi)
		//Si può trovare la maniera di inserirlo nel loop precedente
		uint8_t iface;
		pborders.clear();
		nocts = octants.size();
		pborders.reserve(int(pow(double(nocts),2.0/3.0)*double(global2D.nfaces)));
		for(idx=0; idx<nocts; idx++){
			for(iface=0; iface<global2D.nfaces; iface++){
				if (octants[idx].info[iface+global2D.nfaces]){
					pborders.push_back(idx);
					break;
				}
			}
		}
		pborders.shrink_to_fit();

		setFirstDesc();
		setLastDesc();

	};

	// =================================================================================== //

	void checkCoarse(uint64_t lastDescPre,						// Delete overlapping octants after coarse local tree. Check first and last descendants
					uint64_t firstDescPost,
					u32vector & mapidx){					// of process before and after the local process
		int32_t idx;
		uint32_t nocts;
		uint64_t Morton;
		uint8_t toDelete = 0;

		nocts = getNumOctants();
		idx = 0;
		Morton = octants[idx].computeMorton();
		while(Morton <= lastDescPre & idx < nocts & Morton != 0){
			// To delete, the father is in proc before me
			toDelete++;
			idx++;
			Morton = octants[idx].computeMorton();
		}
		for(idx=0; idx<nocts-toDelete; idx++){
			octants[idx] = octants[idx+toDelete];
		}
		octants.resize(nocts-toDelete);
		octants.shrink_to_fit();
		mapidx.resize(nocts-toDelete);
		mapidx.shrink_to_fit();

		toDelete = 0;
		Morton = last_desc.computeMorton();
		if(int(firstDescPost  - Morton) > 1){
			// To insert, the father is not yet here!!
			idx = nocts - 1;
			Class_Octant<2> father = octants[idx].buildFather();
			while(octants[idx].buildFather() == father & idx >= 0){
				toDelete++;
				idx--;
			}
			father.info[global2D.nfaces+1] = father.info[global2D.nfaces+3] = true;
			if(global2D.nfaces == 6)
				father.info[global2D.nfaces+5] = true;
			octants.resize(nocts-toDelete);
			octants.push_back(father);
			octants.shrink_to_fit();
			mapidx.resize(nocts-toDelete+1);
			mapidx.shrink_to_fit();
		}

		//Update pborders (adesso inefficiente, loop di nuovo su tutti gli elementi)
		//Si può trovare la maniera di inserirlo nel loop precedente
		uint8_t iface;
		pborders.clear();
		nocts = octants.size();
		pborders.reserve(int(pow(double(nocts),2.0/3.0)*double(global2D.nfaces)));
		for(idx=0; idx<nocts; idx++){
			for(iface=0; iface<global2D.nfaces; iface++){
				if (octants[idx].info[iface+global2D.nfaces]){
					pborders.push_back(idx);
					break;
				}
			}
		}
		pborders.shrink_to_fit();

		setFirstDesc();
		setLastDesc();

	};

	// =================================================================================== //

	void updateLocalMaxDepth(){						// Update max depth reached in local tree
		uint32_t noctants = getNumOctants();
		uint32_t i;

		local_max_depth = 0;
		for(i = 0; i < noctants; i++){
			if(octants[i].getLevel() > local_max_depth){
				local_max_depth = octants[i].getLevel();
			}
		}

	};

	// =================================================================================== //

	void computeConnectivity(){						// Computes nodes vector and connectivity of octants of local tree
		map<uint64_t, vector<uint32_t> > mapnodes;
		map<uint64_t, vector<uint32_t> >::iterator iter, iterend;
		uint32_t i, k, counter;
		uint64_t morton;
		uint32_t noctants = getNumOctants();
		uint32_t (*octnodes)[3];
		uint8_t j;

		//TODO Reserve for vector for 2D and 3D

		if (nodes.size() == 0){
			connectivity.resize(noctants);
			for (i = 0; i < noctants; i++){
				octnodes = octants[i].getNodes();
				for (j = 0; j < global2D.nnodes; j++){
					morton = mortonEncode_magicbits(octnodes[j][0], octnodes[j][1], octnodes[j][2]);
					if (mapnodes[morton].size()==0){
						mapnodes[morton].reserve(12);
						for (k = 0; k < 3; k++){
							mapnodes[morton].push_back(octnodes[j][k]);
						}
					}
					mapnodes[morton].push_back(i);
				}
				delete []octnodes;
			}
			iter	= mapnodes.begin();
			iterend	= mapnodes.end();
			counter = 0;
			uint32_t numnodes = mapnodes.size();
			nodes.resize(numnodes);
			vector<uint32_t>::iterator iter2;
			while (iter != iterend){
				vector<uint32_t> nodecasting(iter->second.begin(), iter->second.begin()+3);
				nodes[counter] = nodecasting;
				nodes[counter].shrink_to_fit();
				for(iter2 = iter->second.begin()+3; iter2 != iter->second.end(); iter2++){
					if (connectivity[(*iter2)].size()==0){
						connectivity[(*iter2)].reserve(8);
					}
					connectivity[(*iter2)].push_back(counter);
				}
				mapnodes.erase(iter++);
				counter++;
			}
			nodes.shrink_to_fit();
			//Lento. Solo per risparmiare memoria
			for (int ii=0; ii<noctants; ii++){
				connectivity[ii].shrink_to_fit();
			}
			connectivity.shrink_to_fit();
		}
		map<uint64_t, vector<uint32_t> >().swap(mapnodes);
		iter = mapnodes.end();

	};

	// =================================================================================== //

	void clearConnectivity(){						// Clear nodes vector and connectivity of octants of local tree
		u32vector2D().swap(nodes);
		u32vector2D().swap(connectivity);
	};

	// =================================================================================== //

	void updateConnectivity(){						// Updates nodes vector and connectivity of octants of local tree
		clearConnectivity();
		computeConnectivity();
	};

	// =================================================================================== //

	void computeghostsConnectivity(){				// Computes ghosts nodes vector and connectivity of ghosts octants of local tree
		map<uint64_t, vector<uint32_t> > mapnodes;
		map<uint64_t, vector<uint32_t> >::iterator iter, iterend;
		uint32_t i, k, counter;
		uint64_t morton;
		uint32_t noctants = size_ghosts;
		uint32_t (*octnodes)[3];
		uint8_t j;

		if (ghostsnodes.size() == 0){
			ghostsconnectivity.resize(noctants);
			for (i = 0; i < noctants; i++){
				octnodes = ghosts[i].getNodes();
				for (j = 0; j < global2D.nnodes; j++){
					morton = mortonEncode_magicbits(octnodes[j][0], octnodes[j][1], octnodes[j][2]);
					if (mapnodes[morton].size()==0){
						for (k = 0; k < 3; k++){
							mapnodes[morton].push_back(octnodes[j][k]);
						}
					}
					mapnodes[morton].push_back(i);
				}
				delete []octnodes;
			}
			iter	= mapnodes.begin();
			iterend	= mapnodes.end();
			uint32_t numnodes = mapnodes.size();
			ghostsnodes.resize(numnodes);
			counter = 0;
			vector<uint32_t>::iterator iter2;
			while (iter != iterend){
				vector<uint32_t> nodecasting(iter->second.begin(), iter->second.begin()+3);
				ghostsnodes[counter] = nodecasting;
				ghostsnodes[counter].shrink_to_fit();
				for(iter2 = iter->second.begin()+3; iter2 != iter->second.end(); iter2++){
					if (ghostsconnectivity[(*iter2)].size()==0){
						ghostsconnectivity[(*iter2)].reserve(8);
					}
					ghostsconnectivity[(*iter2)].push_back(counter);
				}
				mapnodes.erase(iter++);
				counter++;
			}
			ghostsnodes.shrink_to_fit();
			//Lento. Solo per risparmiare memoria
			for (int ii=0; ii<noctants; ii++){
				ghostsconnectivity[ii].shrink_to_fit();
			}
			ghostsconnectivity.shrink_to_fit();
		}
		iter = mapnodes.end();

	};

	// =================================================================================== //

	void clearghostsConnectivity(){					// Clear ghosts nodes vector and connectivity of ghosts octants of local tree
		u32vector2D().swap(ghostsnodes);
		u32vector2D().swap(ghostsconnectivity);
	};

	// =================================================================================== //

	void updateghostsConnectivity(){					// Update ghosts nodes vector and connectivity of ghosts octants of local tree
		clearghostsConnectivity();
		computeghostsConnectivity();
	};

	// =================================================================================== //

	void findNeighbours(uint32_t idx,							// Finds neighbours of idx-th octant through iface in vector octants.
								uint8_t iface,					// Returns a vector (empty if iface is a bound face) with the index of neighbours
								u32vector & neighbours,			// in their structure (octants or ghosts) and sets isghost[i] = true if the
								vector<bool> & isghost){		// i-th neighbour is ghost in the local tree

		uint64_t  Morton, Mortontry;
		uint32_t  noctants = getNumOctants();
		uint32_t idxtry;
		Class_Octant<2>* oct = &octants[idx];
		uint32_t size = oct->getSize();

		//Alternative to switch case
		int8_t cx = int8_t((iface<2)*(int8_t(2*iface-1)));
		int8_t cy = int8_t((iface<4)*(int8_t(iface/2))*(int8_t(2*iface-5)));
		int8_t cz = int8_t((int8_t(iface/4))*(int8_t(2*iface-9)));

		isghost.clear();
		neighbours.clear();

		// Default if iface is nface<iface<0
		if (iface < 0 || iface > global2D.nfaces){
			writeLog("Face index out of range in find neighbours !!!");
			return;
		}

		// Check if octants face is a process boundary
		if (oct->info[global2D.nfaces+iface] == false){

			// Check if octants face is a boundary
			if (oct->info[iface] == false){

				//Build Morton number of virtual neigh of same size
				Class_Octant<2> samesizeoct(oct->level, int32_t(oct->x)+int32_t(cx*size), int32_t(oct->y)+int32_t(cy*size));
				Morton = samesizeoct.computeMorton();
				// Search morton in octants
				// If a even face morton is lower than morton of oct, if odd higher
				// ---> can i search only before or after idx in octants
				int32_t jump = (oct->computeMorton() > Morton) ? int32_t(idx/2+1) : int32_t((noctants -idx)/2+1);
				idxtry = uint32_t(idx +((oct->computeMorton()<Morton)-(oct->computeMorton()>Morton))*jump);
				Mortontry = oct->computeMorton();
				while(abs(jump) > 0){
					Mortontry = octants[idxtry].computeMorton();
					jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
					idxtry += jump;
					if (idxtry > noctants-1){
						if (jump > 0){
							idxtry = noctants - 1;
							jump = 0;
						}
						else if (jump < 0){
							idxtry = 0;
							jump = 0;
						}
					}
				}
				if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
					//Found neighbour of same size
					isghost.push_back(false);
					neighbours.push_back(idxtry);
					return;
				}
				else{
					// Step until the mortontry lower than morton (one idx of distance)
					{
						while(octants[idxtry].computeMorton() < Morton){
							idxtry++;
							if(idxtry > noctants-1){
								idxtry = noctants-1;
								return;
							}
						}
						while(octants[idxtry].computeMorton() > Morton){
							idxtry--;
							if(idxtry > noctants-1){
								idxtry = noctants-1;
								return;
							}
						}
					}
					if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
						//Found neighbour of same size
						isghost.push_back(false);
						neighbours.push_back(idxtry);
						return;
					}
					// Compute Last discendent of virtual octant of same size
					uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - samesizeoct.level)) - 1;
					Class_Octant<2> last_desc = samesizeoct.buildLastDesc();
					uint64_t Mortonlast = last_desc.computeMorton();
					vector<uint32_t> bufferidx;
					Mortontry = octants[idxtry].computeMorton();
					int32_t Dh;
					int32_t eqcoord;
					while(Mortontry < Mortonlast & idxtry < noctants){
						Dh = int32_t(cx)*(int32_t(oct->x) - int32_t(octants[idxtry].x));
						Dh += int32_t(cy)*(int32_t(oct->y) - int32_t(octants[idxtry].y));
						if ((abs(Dh) == ((1-(iface%2))*octants[idxtry].getSize() + (iface%2)*size))){
							neighbours.push_back(idxtry);
							isghost.push_back(false);
						}
						idxtry++;
						if(idxtry>noctants-1){
							break;
						}
						Mortontry = octants[idxtry].computeMorton();
					}
					return;
				}
			}
			else{
				// Boundary Face
				return;
			}
		}
		//--------------------------------------------------------------- //
		//--------------------------------------------------------------- //
		else{
			cout << "idx   " << int(idx) << "  iface  " << int(iface) << " is pbound" << endl;
			// Check if octants face is a boundary
			if (oct->info[iface] == false){
				// IF OCTANT FACE IS A PROCESS BOUNDARY SEARCH ALSO IN GHOSTS

				if (ghosts.size()>0){
					// Search in ghosts

					uint32_t idxghost = uint32_t(size_ghosts/2);
					Class_Octant<2>* octghost = &ghosts[idxghost];

					//Build Morton number of virtual neigh of same size
					Class_Octant<2> samesizeoct(oct->level, oct->x+cx*size, oct->y+cy*size);
					Morton = samesizeoct.computeMorton(); //mortonEncode_magicbits(oct->x-size,oct->y,oct->z);
					// Search morton in octants
					// If a even face morton is lower than morton of oct, if odd higher
					// ---> can i search only before or after idx in octants
					int32_t jump = (octghost->computeMorton() > Morton) ? int32_t(idxghost/2+1) : int32_t((size_ghosts -idxghost)/2+1);
					idxtry = uint32_t(idxghost +((octghost->computeMorton()<Morton)-(octghost->computeMorton()>Morton))*jump);
					while(abs(jump) > 0){
						Mortontry = ghosts[idxtry].computeMorton();
						jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
						idxtry += jump;
						if (idxtry > ghosts.size()-1){
							if (jump > 0){
								idxtry = ghosts.size() - 1;
								jump = 0;
							}
							else if (jump < 0){
								idxtry = 0;
								jump = 0;
							}
						}
					}
					if(octants[idxtry].computeMorton() == Morton && ghosts[idxtry].level == oct->level){
						//Found neighbour of same size
						isghost.push_back(true);
						neighbours.push_back(idxtry);
						return;
					}
					else{
						// Step until the mortontry lower than morton (one idx of distance)
						{
							while(ghosts[idxtry].computeMorton() < Morton){
								idxtry++;
								if(idxtry > ghosts.size()-1){
									idxtry = ghosts.size();
									break;
								}
							}
							while(ghosts[idxtry].computeMorton() > Morton){
								idxtry--;
								if(idxtry > ghosts.size()-1){
									idxtry = ghosts.size();
									break;
								}
							}
						}
						if(idxtry < size_ghosts){
							if(ghosts[idxtry].computeMorton() == Morton && ghosts[idxtry].level == oct->level){
								//Found neighbour of same size
								isghost.push_back(true);
								neighbours.push_back(idxtry);
								return;
							}
							// Compute Last discendent of virtual octant of same size
							uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - samesizeoct.level)) - 1;
							Class_Octant<2> last_desc = samesizeoct.buildLastDesc();
							uint64_t Mortonlast = last_desc.computeMorton();
							vector<uint32_t> bufferidx;
							Mortontry = ghosts[idxtry].computeMorton();
							int32_t Dh;
							int32_t eqcoord;
							while(Mortontry < Mortonlast & idxtry < size_ghosts){
								Dh = int32_t(cx)*(int32_t(oct->x) - int32_t(ghosts[idxtry].x));
								Dh += int32_t(cy)*(int32_t(oct->y) - int32_t(ghosts[idxtry].y));
								if ((abs(Dh) == ((1-(iface%2))*ghosts[idxtry].getSize() + (iface%2)*size))){
									neighbours.push_back(idxtry);
									isghost.push_back(true);
								}
								idxtry++;
								if(idxtry>size_ghosts-1){
									break;
								}
								Mortontry = ghosts[idxtry].computeMorton();
							}
						}
					}

					uint32_t lengthneigh = 0;
					uint32_t sizeneigh = neighbours.size();
					for (idxtry=0; idxtry<sizeneigh; idxtry++){
						lengthneigh += ghosts[neighbours[idxtry]].getSize();
					}
					if (lengthneigh < oct->getSize()){
						// Search in octants

						// Check if octants face is a boundary
						if (oct->info[iface] == false){

							//Build Morton number of virtual neigh of same size
							Class_Octant<2> samesizeoct(oct->level, oct->x+cx*size, oct->y+cy*size);
							Morton = samesizeoct.computeMorton();
							// Search morton in octants
							// If a even face morton is lower than morton of oct, if odd higher
							// ---> can i search only before or after idx in octants
							int32_t jump = (oct->computeMorton() > Morton) ? int32_t(idx/2+1) : int32_t((noctants -idx)/2+1);
							idxtry = uint32_t(idx +((oct->computeMorton()<Morton)-(oct->computeMorton()>Morton))*jump);
							while(abs(jump) > 0){
								Mortontry = octants[idxtry].computeMorton();
								jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
								idxtry += jump;
							}
							if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
								//Found neighbour of same size
								isghost.push_back(false);
								neighbours.push_back(idxtry);
								cout << " idx  " << idx << "   idxtry  "<<idxtry << "/" << noctants << "   face   " << int(iface) << endl;
								writeLog("Face marked pbound but only a non-ghost neighbour found!!!");
								return;
							}
							else{
								// Step until the mortontry lower than morton (one idx of distance)
								{
									while(octants[idxtry].computeMorton() < Morton){
										idxtry++;
										if(idxtry > noctants-1){
											idxtry = noctants;
											break;
										}
									}
									while(octants[idxtry].computeMorton() > Morton){
										idxtry--;
										if(idxtry > noctants-1){
											idxtry = noctants;
											break;
										}
									}
								}
								if (idxtry < noctants){
									if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
										//Found neighbour of same size
										isghost.push_back(false);
										neighbours.push_back(idxtry);
										cout << " idx  " << idx << "   idxtry  "<<idxtry << "/" << noctants << "   face   " << int(iface) << endl;
										writeLog("Face marked pbound but only a non-ghost neighbour found!!!");
										return;
									}
									// Compute Last discendent of virtual octant of same size
									uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - samesizeoct.level)) - 1;
									Class_Octant<2> last_desc = samesizeoct.buildLastDesc();
									uint64_t Mortonlast = last_desc.computeMorton();
									vector<uint32_t> bufferidx;
									Mortontry = octants[idxtry].computeMorton();
									int32_t Dh;
									int32_t eqcoord;
									while(Mortontry < Mortonlast & idxtry < noctants-1){
										Dh = int32_t(cx)*(int32_t(oct->x) - int32_t(octants[idxtry].x));
										Dh += int32_t(cy)*(int32_t(oct->y) - int32_t(octants[idxtry].y));
										if ((abs(Dh) == ((1-(iface%2))*octants[idxtry].getSize() + (iface%2)*size))){
											neighbours.push_back(idxtry);
											isghost.push_back(false);
										}
										idxtry++;
										Mortontry = octants[idxtry].computeMorton();
									}
								}
							}
						}
					}
					return;
				}
			}
			else{
				// Boundary Face
				return;
			}

		}

	};
//
	// =================================================================================== //

	void findGhostNeighbours(uint32_t const idx,		// Finds neighbours of idx-th ghost octant through iface in vector octants.
								uint8_t iface,					// Returns a vector (empty if iface is not the pbound face for ghost) with the index of neighbours
								u32vector & neighbours){		// in the structure octants

		uint64_t  Morton, Mortontry;
		uint32_t  noctants = getNumOctants();
		uint32_t idxtry;
		Class_Octant<2>* oct = &ghosts[idx];
		uint32_t size = oct->getSize();

		//Alternative to switch case
		int8_t cx = int8_t((iface<2)*(int8_t(2*iface-1)));
		int8_t cy = int8_t((iface<4)*(int8_t(iface/2))*(int8_t(2*iface-5)));
		int8_t cz = int8_t((int8_t(iface/4))*(int8_t(2*iface-9)));

		neighbours.clear();

		// Default if iface is nface<iface<0
		if (iface < 0 || iface > global2D.nfaces){
			writeLog("Face index out of range in find neighbours !!!");
			return;
		}

		// Check if octants face is a process boundary
		if (oct->info[global2D.nfaces+iface] == true){

				//Build Morton number of virtual neigh of same size
				Class_Octant<2> samesizeoct(oct->level, int32_t(oct->x)+int32_t(cx*size), int32_t(oct->y)+int32_t(cy*size));
				Morton = samesizeoct.computeMorton();
				// Search morton in octants
				// If a even face morton is lower than morton of oct, if odd higher
				// ---> can i search only before or after idx in octants
				int32_t jump;
				idxtry = uint32_t(getNumOctants()/2);
				Mortontry = octants[idxtry].computeMorton();
				jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
				while(abs(jump) > 0){
					Mortontry = octants[idxtry].computeMorton();
					jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
					idxtry += jump;
					if (idxtry > noctants-1){
						if (jump > 0){
							idxtry = noctants - 1;
							jump = 0;
						}
						else if (jump < 0){
							idxtry = 0;
							jump = 0;
						}
					}
				}
				if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
					//Found neighbour of same size
					neighbours.push_back(idxtry);
					return;
				}
				else{
					// Step until the mortontry lower than morton (one idx of distance)
					{
						while(octants[idxtry].computeMorton() < Morton){
							idxtry++;
							if(idxtry > noctants-1){
								idxtry = noctants-1;
								return;
							}
						}
						while(octants[idxtry].computeMorton() > Morton){
							idxtry--;
							if(idxtry > noctants-1){
								idxtry = noctants-1;
								return;
							}
						}
					}
					if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
						//Found neighbour of same size
						neighbours.push_back(idxtry);
						return;
					}
					// Compute Last discendent of virtual octant of same size
					uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - samesizeoct.level)) - 1;
					Class_Octant<2> last_desc = samesizeoct.buildLastDesc();
					uint64_t Mortonlast = last_desc.computeMorton();
					vector<uint32_t> bufferidx;
					Mortontry = octants[idxtry].computeMorton();
					int32_t Dh;
					int32_t eqcoord;
					while(Mortontry < Mortonlast & idxtry < noctants){
						Dh = int32_t(cx)*(int32_t(oct->x) - int32_t(octants[idxtry].x));
						Dh += int32_t(cy)*(int32_t(oct->y) - int32_t(octants[idxtry].y));
						if ((abs(Dh) == ((1-(iface%2))*octants[idxtry].getSize() + (iface%2)*size))){
							neighbours.push_back(idxtry);
						}
						idxtry++;
						if(idxtry>noctants-1){
							break;
						}
						Mortontry = octants[idxtry].computeMorton();
					}
					return;
				}
		}
		//--------------------------------------------------------------- //
		//-----Not Pbound face------------- ----------------------------- //
		else{
			return;
		}

	};

	// =================================================================================== //

	//	void			findNeighbours(Class_Octant const & oct,	// Finds neighbours of octant oct through iface in vector octants
	//								uint8_t iface,					// It may be slow due to the research of oct in octants.
	//								u32vector & neighbours,			// Returns a vector (empty if iface is a bound face) with the index of neighbours
	//								vector<bool> & isghost);		// in their structure (octants or ghosts) and sets isghost[i] = true if the
	//																// i-th neighbour is ghost in the local tree

	// =================================================================================== //

	bool localBalance(bool doInterior){				// 2:1 balancing on level a local tree already adapted (balance only the octants with info[14] = false) (refinement wins!)
																// Return true if balanced done with some markers modification
																// Seto doInterior = false if the interior octants are already balanced
		// Local variables
		uint32_t 			noctants = getNumOctants();
		uint32_t			sizeneigh, modsize;
		u32vector		 	neigh;
		u32vector		 	modified, newmodified;
		uint32_t 			i, idx, imod;
		uint8_t				iface;
		int8_t				targetmarker;
		vector<bool> 		isghost;
		bool				Bdone = false;

		OctantsType::iterator 	obegin, oend, it;
		u32vector::iterator 	ibegin, iend, iit;

		//If interior octants have to be balanced
		if(doInterior){
			// First loop on the octants
			/*		for(idx=0 ; idx<noctants; idx++){
				if (!octants[idx].getNotBalance()){
					for (iface=1; iface<nface; iface+=2){
						if(!octants[idx].getPbound(iface)){
							findNeighbours(idx, iface, neigh, isghost);
							sizeneigh = neigh.size();
							for(i=0; i<sizeneigh; i++){
								if (!isghost[i]){
									{
										if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) > (octants[idx].getLevel() + octants[idx].getMarker() + 1) ){
											octants[idx].setMarker(octants[neigh[i]].getLevel()+octants[neigh[i]].getMarker()-1-octants[idx].getLevel());
											modified.push_back(idx);
											Bdone = true;
										}
										else if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (octants[idx].getLevel() + octants[idx].getMarker() - 1)){
											octants[neigh[i]].setMarker(octants[idx].getLevel()+octants[idx].getMarker()-octants[neigh[i]].getLevel()-1);
											modified.push_back(neigh[i]);
											Bdone = true;
										}
									};
								}

							else{
								if(ghosts.size()>0){
									if((ghosts[neigh[i]].getLevel() + ghosts[neigh[i]].getMarker())> (octants[idx].getLevel() + octants[idx].getMarker() + 1)){
										octants[idx].setMarker(ghosts[neigh[i]].getLevel()+ghosts[neigh[i]].getMarker()-octants[idx].getLevel()-1);
										modified.push_back(idx);
										Bdone = true;
									}
								}
							}

							}
						}
					}
				}
			}*/
			obegin = octants.begin();
			oend = octants.end();
			idx = 0;
			for (it=obegin; it!=oend; it++){
				it->info[15] = false;
				if (!it->getNotBalance() && it->getMarker() != 0){
					targetmarker = min(MAX_LEVEL_2D, (octants[idx].getLevel() + octants[idx].getMarker()));
					for (iface=0; iface<global2D.nfaces; iface++){
						if(!it->getPbound(iface)){
							findNeighbours(idx, iface, neigh, isghost);
							sizeneigh = neigh.size();
							for(i=0; i<sizeneigh; i++){
								if (!isghost[i]){
									{
										if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) > (targetmarker + 1) ){
											octants[idx].setMarker(octants[neigh[i]].getLevel()+octants[neigh[i]].getMarker()-1-octants[idx].getLevel());
											octants[idx].info[15] = true;
											modified.push_back(idx);
											Bdone = true;
										}
										else if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (targetmarker - 1)){
											octants[neigh[i]].setMarker(targetmarker-octants[neigh[i]].getLevel()-1);
											octants[neigh[i]].info[15] = true;
											modified.push_back(neigh[i]);
											Bdone = true;
										}
									};
								}
							}
						}
					}
				}
				idx++;
			}
			// Loop on ghost octants (influence over interior borders)
			obegin = ghosts.begin();
			oend = ghosts.end();
			idx = 0;
			for (it=obegin; it!=oend; it++){
				if (!it->getNotBalance() && it->getMarker() != 0){
					targetmarker = min(MAX_LEVEL_2D, (it->getLevel()+it->getMarker()));
					for (iface=0; iface<global2D.nfaces; iface++){
						if(it->getPbound(iface) == true){
							neigh.clear();
							findGhostNeighbours(idx, iface, neigh);
							sizeneigh = neigh.size();
							for(i=0; i<sizeneigh; i++){
								if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (targetmarker - 1)){
									octants[neigh[i]].setMarker(targetmarker-octants[neigh[i]].getLevel()-1);
									octants[neigh[i]].info[15] = true;
									modified.push_back(neigh[i]);
									Bdone = true;
								}
							}
						}
					}
				}
				idx++;
			}

			// While loop for iterative balancing
			u32vector().swap(newmodified);
			modsize = modified.size();
			while(modsize!=0){
				ibegin = modified.begin();
				iend = modified.end();
				for (iit=ibegin; iit!=iend; iit++){
					idx = *iit;
					if (!octants[idx].getNotBalance()){
						targetmarker = min(MAX_LEVEL_2D, (octants[idx].getLevel()+octants[idx].getMarker()));
						for (iface=0; iface<global2D.nfaces; iface++){
							if(!octants[idx].getPbound(iface)){
								findNeighbours(idx, iface, neigh, isghost);
								sizeneigh = neigh.size();
								for(i=0; i<sizeneigh; i++){
									if (!isghost[i]){
										{
											if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) >  (targetmarker + 1)){
												octants[idx].setMarker(octants[neigh[i]].getLevel()+octants[neigh[i]].getMarker()-octants[idx].getLevel()-1);
												octants[idx].info[15] = true;
												newmodified.push_back(idx);
												Bdone = true;
											}
											else if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (targetmarker - 1)){
												octants[neigh[i]].setMarker(targetmarker-octants[neigh[i]].getLevel()-1);
												octants[neigh[i]].info[15] = true;
												newmodified.push_back(neigh[i]);
												Bdone = true;
											}
										};
									}
								}
							}
						}
					}
				}
				u32vector().swap(modified);
				swap(modified,newmodified);
				modsize = modified.size();
			}// end while

		}
		else{

			// Loop on ghost octants (influence over interior borders)
			/*	for (idx=0; idx<size_ghosts; idx++){
			if (!ghosts[idx].getNotBalance()){
				for (iface=0; iface<nface; iface++){
					if(ghosts[idx].getPbound(iface) == true){
						neigh.clear();
						findGhostNeighbours(idx, iface, neigh);
						sizeneigh = neigh.size();
						for(i=0; i<sizeneigh; i++){
							if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (ghosts[idx].getLevel() + ghosts[idx].getMarker() - 1)){
								octants[neigh[i]].setMarker(ghosts[idx].getLevel()+ghosts[idx].getMarker()-octants[neigh[i]].getLevel()-1);
								modified.push_back(neigh[i]);
								Bdone = true;
							}
						}
					}
				}
			}
		}*/
			obegin = ghosts.begin();
			oend = ghosts.end();
			idx = 0;
			for (it=obegin; it!=oend; it++){
				if (!it->getNotBalance() && it->info[15]){
					targetmarker = min(MAX_LEVEL_2D, (it->getLevel()+it->getMarker()));
					for (iface=0; iface<global2D.nfaces; iface++){
						if(it->getPbound(iface) == true){
							neigh.clear();
							findGhostNeighbours(idx, iface, neigh);
							sizeneigh = neigh.size();
							for(i=0; i<sizeneigh; i++){
								if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (targetmarker - 1)){
									octants[neigh[i]].setMarker(targetmarker-octants[neigh[i]].getLevel()-1);
									octants[neigh[i]].info[15] = true;
									modified.push_back(neigh[i]);
									Bdone = true;
								}
							}
						}
					}
				}
				idx++;
			}

			// While loop for iterative balancing
			u32vector().swap(newmodified);
			modsize = modified.size();
			while(modsize!=0){
				ibegin = modified.begin();
				iend = modified.end();
				for (iit=ibegin; iit!=iend; iit++){
					idx = *iit;
					if (!octants[idx].getNotBalance()){
						targetmarker = min(MAX_LEVEL_2D, (octants[idx].getLevel()+octants[idx].getMarker()));
						for (iface=0; iface<global2D.nfaces; iface++){
							if(!octants[idx].getPbound(iface)){
								findNeighbours(idx, iface, neigh, isghost);
								sizeneigh = neigh.size();
								for(i=0; i<sizeneigh; i++){
									if (!isghost[i]){
										{
											if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) >  (targetmarker + 1)){
												octants[idx].setMarker(octants[neigh[i]].getLevel()+octants[neigh[i]].getMarker()-octants[idx].getLevel()-1);
												octants[idx].info[15] = true;
												newmodified.push_back(idx);
												Bdone = true;
											}
											else if((octants[neigh[i]].getLevel() + octants[neigh[i]].getMarker()) < (targetmarker - 1)){
												octants[neigh[i]].setMarker(targetmarker-octants[neigh[i]].getLevel()-1);
												octants[neigh[i]].info[15] = true;
												newmodified.push_back(neigh[i]);
												Bdone = true;
											}
										};
									}
								}
							}
						}
					}
				}
				u32vector().swap(modified);
				swap(modified,newmodified);
				modsize = modified.size();
			}// end while
			obegin = oend = octants.end();
			ibegin = iend = modified.end();
		}
		return Bdone;
		// Pay attention : info[15] may be true after local balance for some octants


	};

	// =================================================================================== //

	void			findNodeNeighbours(uint32_t idx,			// Finds neighbours of idx-th octant through inode in vector octants.
									uint8_t inode,				// Returns a vector (empty if inode is a bound node) with the index of neighbours
									u32vector & neighbours,		// in their structure (octants or ghosts) and sets isghost[i] = true if the
									vector<bool> & isghost){	// i-th neighbour is ghost in the local tree

		uint64_t  Morton, Mortontry;
		uint32_t  noctants = getNumOctants();
		uint32_t idxtry;
		Class_Octant<2>* oct = &octants[idx];
		uint32_t size = oct->getSize();
		uint8_t iface1, iface2, iface3;
		int32_t Dhx, Dhy, Dhz;
		int32_t Dhxref, Dhyref, Dhzref;

		//Alternative to switch case
		int8_t Cx[8] = {-1,1,-1,1,-1,1,-1,1};
		int8_t Cy[8] = {-1,-1,1,1,-1,-1,1,1};
		int8_t Cz[8] = {-1,-1,-1,-1,1,1,1,1};
		int8_t cx = Cx[inode];
		int8_t cy = Cy[inode];
		int8_t cz = Cz[inode];

		isghost.clear();
		neighbours.clear();

		// Default if inode is nnodes<inode<0
		if (inode < 0 || inode > global2D.nnodes){
			writeLog("Node index out of range in find neighbours !!!");
			return;
		}

		// Check if octants node is a boundary
		iface1 = global2D.nodeface[inode][0];
		iface2 = global2D.nodeface[inode][1];
		iface3 = global2D.nodeface[inode][2];

		// Check if octants node is a boundary
		if (oct->info[iface1] == false && oct->info[iface2] == false && oct->info[iface3] == false){

			//Build Morton number of virtual neigh of same size
			Class_Octant<2> samesizeoct(oct->level, oct->x+cx*size, oct->y+cy*size);
			Morton = samesizeoct.computeMorton(); //mortonEncode_magicbits(oct->x-size,oct->y,oct->z);

			//SEARCH IN GHOSTS

			if (ghosts.size()>0){
				// Search in ghosts

				uint32_t idxghost = uint32_t(size_ghosts/2);
				Class_Octant<2>* octghost = &ghosts[idxghost];

				// Search morton in octants
				// If a even face morton is lower than morton of oct, if odd higher
				// ---> can i search only before or after idx in octants
				int32_t jump = (octghost->computeMorton() > Morton) ? int32_t(idxghost/2+1) : int32_t((size_ghosts -idxghost)/2+1);
				idxtry = uint32_t(idxghost +((octghost->computeMorton()<Morton)-(octghost->computeMorton()>Morton))*jump);
				if (idxtry > size_ghosts-1)
					idxtry = size_ghosts-1;
				while(abs(jump) > 0){
					Mortontry = ghosts[idxtry].computeMorton();
					jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
					idxtry += jump;
					if (idxtry > ghosts.size()-1){
						if (jump > 0){
							idxtry = ghosts.size() - 1;
							jump = 0;
						}
						else if (jump < 0){
							idxtry = 0;
							jump = 0;
						}
					}
				}
				if(octants[idxtry].computeMorton() == Morton && ghosts[idxtry].level == oct->level){
					//Found neighbour of same size
					isghost.push_back(true);
					neighbours.push_back(idxtry);
					return;
				}
				else{
					// Step until the mortontry lower than morton (one idx of distance)
					{
						while(ghosts[idxtry].computeMorton() < Morton){
							idxtry++;
							if(idxtry > ghosts.size()-1){
								idxtry = ghosts.size();
								break;
							}
						}
						while(ghosts[idxtry].computeMorton() > Morton){
							idxtry--;
							if(idxtry > ghosts.size()-1){
								idxtry = ghosts.size();
								break;
							}
						}
					}
					if(idxtry < size_ghosts){
						if(ghosts[idxtry].computeMorton() == Morton && ghosts[idxtry].level == oct->level){
							//Found neighbour of same size
							isghost.push_back(true);
							neighbours.push_back(idxtry);
							return;
						}
						// Compute Last discendent of virtual octant of same size
						uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - samesizeoct.level)) - 1;
						Class_Octant<2> last_desc = samesizeoct.buildLastDesc();
						uint64_t Mortonlast = last_desc.computeMorton();
						vector<uint32_t> bufferidx;
						Mortontry = ghosts[idxtry].computeMorton();
						while(Mortontry < Mortonlast & idxtry < size_ghosts){
							Dhx = int32_t(cx)*(int32_t(oct->x) - int32_t(ghosts[idxtry].x));
							Dhy = int32_t(cy)*(int32_t(oct->y) - int32_t(ghosts[idxtry].y));
							Dhxref = int32_t(cx<0)*ghosts[idxtry].getSize() + int32_t(cx>0)*size;
							Dhyref = int32_t(cy<0)*ghosts[idxtry].getSize() + int32_t(cy>0)*size;
							Dhzref = int32_t(cz<0)*ghosts[idxtry].getSize() + int32_t(cz>0)*size;
							if ((abs(Dhx) == Dhxref) && (abs(Dhy) == Dhyref) && (abs(Dhz) == Dhzref)){
								neighbours.push_back(idxtry);
								isghost.push_back(true);
							}
							idxtry++;
							if(idxtry>size_ghosts-1){
								break;
							}
							Mortontry = ghosts[idxtry].computeMorton();
						}
					}
				}
			}
			uint32_t lengthneigh = 0;
			uint32_t sizeneigh = neighbours.size();
			for (idxtry=0; idxtry<sizeneigh; idxtry++){
				lengthneigh += ghosts[neighbours[idxtry]].getSize();
			}
			if (lengthneigh < oct->getSize()){

				// Search in octants

				//Build Morton number of virtual neigh of same size
				//Class_Octant samesizeoct(oct->level, oct->x+cx*size, oct->y+cy*size, oct->z+cz*size);
				//Morton = samesizeoct.computeMorton();
				// Search morton in octants
				// If a even face morton is lower than morton of oct, if odd higher
				// ---> can i search only before or after idx in octants
				int32_t jump = (oct->computeMorton() > Morton) ? int32_t(idx/2+1) : int32_t((noctants -idx)/2+1);
				idxtry = uint32_t(idx +((oct->computeMorton()<Morton)-(oct->computeMorton()>Morton))*jump);
				if (idxtry > noctants-1)
					idxtry = noctants-1;
				while(abs(jump) > 0){
					Mortontry = octants[idxtry].computeMorton();
					jump = ((Mortontry<Morton)-(Mortontry>Morton))*abs(jump)/2;
					idxtry += jump;
				}
				if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
					//Found neighbour of same size
					isghost.push_back(false);
					neighbours.push_back(idxtry);
					return;
				}
				else{
					// Step until the mortontry lower than morton (one idx of distance)
					{
						while(octants[idxtry].computeMorton() < Morton){
							idxtry++;
							if(idxtry > noctants-1){
								idxtry = noctants;
								break;
							}
						}
						while(octants[idxtry].computeMorton() > Morton){
							idxtry--;
							if(idxtry > noctants-1){
								idxtry = noctants;
								break;
							}
						}
					}
					if (idxtry < noctants){
						if(octants[idxtry].computeMorton() == Morton && octants[idxtry].level == oct->level){
							//Found neighbour of same size
							isghost.push_back(false);
							neighbours.push_back(idxtry);
							return;
						}
						// Compute Last discendent of virtual octant of same size
						uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_2D - samesizeoct.level)) - 1;
						Class_Octant<2> last_desc = samesizeoct.buildLastDesc();
						uint64_t Mortonlast = last_desc.computeMorton();
						vector<uint32_t> bufferidx;
						Mortontry = octants[idxtry].computeMorton();
						while(Mortontry < Mortonlast & idxtry < noctants-1){
							Dhx = int32_t(cx)*(int32_t(oct->x) - int32_t(octants[idxtry].x));
							Dhy = int32_t(cy)*(int32_t(oct->y) - int32_t(octants[idxtry].y));
							Dhxref = int32_t(cx<0)*octants[idxtry].getSize() + int32_t(cx>0)*size;
							Dhyref = int32_t(cy<0)*octants[idxtry].getSize() + int32_t(cy>0)*size;
							Dhzref = int32_t(cz<0)*octants[idxtry].getSize() + int32_t(cz>0)*size;
							if ((abs(Dhx) == Dhxref) && (abs(Dhy) == Dhyref) && (abs(Dhz) == Dhzref)){
								neighbours.push_back(idxtry);
								isghost.push_back(false);
							}
							idxtry++;
							Mortontry = octants[idxtry].computeMorton();
						}
					}
				}
				return;
			}
		}
		else{
			// Boundary Node
			return;
		}

	};

	// =================================================================================== //

	void computeIntersections() {

		OctantsType::iterator it, obegin, oend;
		Class_Intersection<2> intersection(*this);
		u32vector neighbours;
		vector<bool> isghost;
		uint32_t counter_i, counter_g, counter_b, idx;
		uint32_t i, j, k, nsize;
		uint8_t iface, iface2;


		intersections_int.clear();
		intersections_ghost.clear();
		intersections_bord.clear();
		intersections_int.reserve(2*2*octants.size());
		intersections_ghost.reserve(2*2*ghosts.size());
		intersections_bord.reserve(int(sqrt(octants.size())));

		counter_i = counter_g = counter_b = idx = 0;

		// Loop on ghosts
		obegin = ghosts.begin();
		oend = ghosts.end();
		for (it = obegin; it != oend; it++){
			for (iface = 0; iface < 2; iface++){
				iface2 = iface*2;
				findGhostNeighbours(idx, iface2, neighbours);
				nsize = neighbours.size();
				for (i = 0; i < nsize; i++){
					intersection.finer = (i<1);
					intersection.owners[0]  = neighbours[i];
					intersection.owners[1] = idx;
					intersection.iface = global2D.oppface[iface2];
					intersection.isnew = false;
					intersection.isghost = true;
					intersections_ghost.push_back(intersection);
					counter_g++;
				}
			}
			idx++;
		}

		// Loop on octants
		obegin = octants.begin();
		oend = octants.end();
		for (it = obegin; it != oend; it++){
			for (iface = 0; iface < 2; iface++){
				iface2 = iface*2;
				findNeighbours(idx, iface2, neighbours, isghost);
				nsize = neighbours.size();
				if (nsize) {
					for (i = 0; i < nsize; i++){
						if (isghost[i]){
							intersection.owners[0] = idx;
							intersection.owners[1] = neighbours[i];
							intersection.finer = (i>=1);
							intersection.iface = iface2;
							intersection.isnew = false;
							intersection.isghost = true;
							intersections_ghost.push_back(intersection);
							counter_g++;
						}
						else{
							intersection.owners[0] = idx;
							intersection.owners[1] = neighbours[i];
							intersection.finer = (i>=1);
							intersection.iface = iface2;
							intersection.isnew = false;
							intersection.isghost = false;
							intersections_int.push_back(intersection);
							counter_i++;
						}
					}
				}
				else{
					intersection.owners[0] = idx;
					intersection.owners[1] = idx;
					intersection.finer = 0;
					intersection.iface = iface2;
					intersection.isnew = false;
					intersection.isghost = false;
					intersections_bord.push_back(intersection);
					counter_b++;
				}
				if (it->info[iface]){
					intersection.owners[0] = idx;
					intersection.owners[1] = idx;
					intersection.finer = 0;
					intersection.iface = iface;
					intersection.isnew = false;
					intersection.isghost = false;
					intersections_bord.push_back(intersection);
					counter_b++;
				}
			}
			idx++;
		}
		intersections_int.shrink_to_fit();
		intersections_ghost.shrink_to_fit();
		intersections_bord.shrink_to_fit();

	}

	// =================================================================================== //

	void updateIntersections(u32vector & mapidx,
							u32vector & mapinters_int,
							u32vector & mapinters_ghost,
							u32vector & mapinters_bord) {

		map<uint32_t, uint32_t> invmapidx;
		vector<uint32_t> newocts;
		OctantsType::iterator it, obegin, oend;
		Class_Intersection<2> intersection(*this);
		u32vector neighbours;
		vector<bool> isghost;
		uint32_t counter_g, idx;
		uint32_t i, j, nsize, msize, isize, osize, offset;
		uint8_t iface, iface2;


		if (intersections_int.size()==0){
			computeIntersections();
			mapinters_int.clear();
			mapinters_ghost.clear();
			mapinters_bord.clear();
		}
		else{
			msize = mapidx.size();
			for (i=0; i<msize; i++){
				invmapidx[mapidx[i]] = i;
			}

			//Internal Intersections
			isize = intersections_int.size();
			mapinters_int.clear();
			mapinters_int.resize(isize);
			offset = 0;
			newocts.clear();
			for (i=0; i<isize-offset; i++){
				if (octants[invmapidx[intersections_int[i].owners[0]]].info[8] ||
						octants[invmapidx[intersections_int[i].owners[0]]].info[9]){
					offset++;
					newocts.push_back(invmapidx[intersections_int[i].owners[0]]);
				}
				intersections_int[i] = intersections_int[i+offset];
				intersections_int[i].owners[0] = invmapidx[intersections_int[i+offset].owners[0]];
				intersections_int[i].owners[1] = invmapidx[intersections_int[i+offset].owners[1]];
				mapinters_int[i] = i+offset;
			}
			intersections_int.resize(isize-offset);
			isize = isize-offset;
			mapinters_int.resize(isize);
			osize = newocts.size();
			for (j=0; j<osize; j++){
				idx = newocts[j];
				for (iface = 0; iface < 2; iface++){
					iface2 = iface*2;
					findNeighbours(idx, iface2, neighbours, isghost);
					nsize = neighbours.size();
					if (nsize) {
						for (i = 0; i < nsize; i++){
							if (isghost[i]){
								intersection.owners[0] = idx;
								intersection.owners[1] = neighbours[i];
								intersection.finer = (i>=1);
								intersection.iface = iface2;
								intersection.isnew = true;
								intersection.isghost = false;
								intersections_int.push_back(intersection);
							}
						}
					}
				}
			}

			//Border Intersections
			isize = intersections_bord.size();
			mapinters_bord.clear();
			mapinters_bord.resize(isize);
			offset = 0;
			newocts.clear();
			for (i=0; i<isize-offset; i++){
				if (octants[invmapidx[intersections_bord[i].owners[0]]].info[8] ||
						octants[invmapidx[intersections_bord[i].owners[0]]].info[9]){
					offset++;
					newocts.push_back(invmapidx[intersections_bord[i].owners[0]]);
				}
				intersections_bord[i] = intersections_bord[i+offset];
				intersections_bord[i].owners[0] = invmapidx[intersections_bord[i+offset].owners[0]];
				intersections_bord[i].owners[1] = invmapidx[intersections_bord[i+offset].owners[1]];
				mapinters_bord[i] = i+offset;
			}
			intersections_bord.resize(isize-offset);
			isize = isize-offset;
			mapinters_bord.resize(isize);
			osize = newocts.size();
			for (j=0; j<osize; j++){
				idx = newocts[j];
				for (iface = 0; iface < 2; iface++){
					if (octants[idx].info[iface]){
						intersection.owners[0] = idx;
						intersection.owners[1] = idx;
						intersection.finer = 0;
						intersection.iface = iface;
						intersection.isnew = true;
						intersection.isghost = false;
						intersections_bord.push_back(intersection);
					}
				}
			}

			// TODO GHOSTS INTERSECTIONS. NOW ALL ARE SET AS NEW
			//Ghost Intersections
			mapinters_ghost.clear();
			intersections_ghost.clear();
			intersections_ghost.reserve(2*3*ghosts.size());

			counter_g = idx = 0;

			// Loop on ghosts
			obegin = ghosts.begin();
			oend = ghosts.end();
			for (it = obegin; it != oend; it++){
				for (iface = 0; iface < 2*2; iface++){
					findGhostNeighbours(idx, iface, neighbours);
					nsize = neighbours.size();
					for (i = 0; i < nsize; i++){
						intersection.finer = (i<1);
						intersection.owners[0]  = neighbours[i];
						intersection.owners[1] = idx;
						intersection.iface = global2D.oppface[iface];
						if (octants[idx].info[8] ||
								octants[idx].info[9] ||
								it->info[8] || it->info[9]){
							intersection.isnew = true;
						}
						else{
							intersection.isnew = false;
						}
						intersection.isghost = true;
						intersections_ghost.push_back(intersection);
						counter_g++;
					}
				}
				idx++;
			}

			intersections_int.shrink_to_fit();
			intersections_ghost.shrink_to_fit();
			intersections_bord.shrink_to_fit();
		}

	}

	// =================================================================================== //


};//end Class_Local_Tree<2> specialization;


}