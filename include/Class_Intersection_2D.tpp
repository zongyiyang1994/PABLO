/*
 * Class_Intersection_2D.tpp
 *
 *  Created on: 22/apr/2014
 *      Author: Marco Cisternino
 */

/*
// =================================================================================== //
// CLASS SPECIALIZATION                                                                //
// =================================================================================== //

//class Class_Octant<2>;
template<int dim> class Class_Local_Tree;
class Class_Local_Tree<2>;


template<>
class Class_Intersection<2> {
	// ------------------------------------------------------------------------------- //
	// FRIENDSHIPS ------------------------------------------------------------------- //
	//template<int dim> friend class Class_Local_Tree;
	friend class Class_Para_Tree;

	// ------------------------------------------------------------------------------- //
	// TYPEDEFS ----------------------------------------------------------------------- //
public:
	typedef vector<uint32_t>			u32vector;
	typedef vector<vector<uint32_t>	>	u32vector2D;
	typedef vector<vector<uint64_t>	>	u64vector2D;

	// ------------------------------------------------------------------------------- //
	// MEMBERS ----------------------------------------------------------------------- //

	uint32_t 	owners[2];			// Owner octants of the intersection (first is the internal octant)
	uint8_t   	iface;				// Index of the face of the first owner
	bool		finer;				// 0/1 finer octant (if same level =0)
	bool		isghost;			// The intersection has a member ghost
	bool		isnew;				// The intersection is new after a mesh adapting?

	Class_Local_Tree<2> *octree;

	// ------------------------------------------------------------------------------- //
	// CONSTRUCTORS AND OPERATORS----------------------------------------------------- //

public:
	Class_Intersection(Class_Local_Tree<2> & tree){
		owners[0] = 0;
		owners[1] = 0;
		iface = 0;
		isnew = false;
		isghost = false;
		finer = 0;
		octree = &tree;

	};
	Class_Intersection(){
		owners[0] = 0;
		owners[1] = 0;
		iface = 0;
		isnew = false;
		isghost = false;
		finer = 0;
		octree = NULL;
	};
	~Class_Intersection(){
		octree = NULL;
	};
	Class_Intersection(const Class_Intersection<2> & intersection){
		owners[0] = intersection.owners[0];
		owners[1] = intersection.owners[1];
		iface = intersection.iface;
		isnew = intersection.isnew;
		isghost = intersection.isghost;
		finer = intersection.finer;
		octree = intersection.octree;

	};
	Class_Intersection<2>& operator =(const Class_Intersection<2> & intersection){
		owners[0] = intersection.owners[0];
		owners[1] = intersection.owners[1];
		iface = intersection.iface;
		isnew = intersection.isnew;
		isghost = intersection.isghost;
		finer = intersection.finer;
		octree = intersection.octree;
		return *this;
	};
	bool operator ==(const Class_Intersection<2> & intersection){
		bool check = true;
		check = check && (owners[0] == intersection.owners[0]);
		check = check && (owners[1] == intersection.owners[1]);
		check = check && (iface == intersection.iface);
		check = check && (isnew == intersection.isnew);
		check = check && (isghost == intersection.isghost);
		check = check && (finer == intersection.finer);
		check = check && (octree == intersection.octree);
		return check;

	};

	// ------------------------------------------------------------------------------- //
	// METHODS ----------------------------------------------------------------------- //

	// Basic Get/Set methods --------------------------------------------------------- //

	uint32_t getOut(){						// Get the owner with exiting normal
		return owners[0];
	};
	uint32_t getIn(){						// Get the owner with entering normal
		return owners[1];
	};
	void getNormal(int8_t normal[2]){		// Get the normal of the intersection
		for (int i=0; i<2; i++){
			normal[i] = global2D.normals[iface][i];
		}
	};
	uint32_t getSize(){
		const Class_Octant<2> & oct = octree<2>->extractOctant(owners[finer]);
				//<2>->extractOctant(10);
				//extractOctant(owners[finer]);
		return oct.getSize();
	};
	uint64_t getArea(){

	};
//	void getNodes(u32vector2D & nodes);
//	uint32_t (*getNodes())[DIM];
//	void getCenter(vector<double> & center);
//	double* getCenter();
//
}; // end of Class_Intersection_3D.tpp

*/