/*
 * Class_Octant_3D.tpp
 *
 *  Created on: 15/apr/2014
 *      Author: Marco Cisternino
 */

// =================================================================================== //
// CLASS SPECIALIZATION                                                                //
// =================================================================================== //
template<>
class Class_Octant<3>{
	// ------------------------------------------------------------------------------- //
	// FRIENDSHIPS ------------------------------------------------------------------- //
	template<int dim> friend class Class_Local_Tree;
	template<int dim> friend class Class_Para_Tree;

	// ------------------------------------------------------------------------------- //
	// TYPEDEFS ----------------------------------------------------------------------- //
public:
	typedef vector<Class_Octant<3> > 	OctantsType;
	typedef vector<uint32_t>			u32vector;
	typedef vector<vector<uint32_t>	>	u32vector2D;
	typedef vector<vector<uint64_t>	>	u64vector2D;

	// ------------------------------------------------------------------------------- //
	// MEMBERS ----------------------------------------------------------------------- //

private:
	uint32_t  x, y, z;			// Coordinates
	uint8_t   level;			// Refinement level (0=root)
	int8_t    marker;			// Set for Refinement(m>0) or Coarsening(m<0) |m|-times
	bool      info[16];			// Info[0..5] : true if 0..5 face is a boundary face [bound];
								// Info[6..11]: true if 0..5 face is a process boundary face [pbound];
								// Info[12/13]: true if octant is new after refinement/coarsening;
								// Info[14]   : true if balancing is not required for this octant;
								// Info[15]   : Aux (before : true if octant is a scary ghost).


	// ------------------------------------------------------------------------------- //
	// CONSTRUCTORS AND OPERATORS----------------------------------------------------- //

public:
	Class_Octant(){
		x = y = z = 0;
		level = 0;
		marker = 0;
		bool dummy[16] = {false};
		memcpy(info, dummy, 16);
		for (int i=0; i<global3D.nfaces; i++){
			info[i] = true;
		}
	};

	Class_Octant(int8_t level, int32_t x, int32_t y, int32_t z){
		this->x = x;
		this->y = y;
		this->z = z;
		this->level = level;
		marker = 0;
		bool dummy[16] = {false};
		memcpy(info, dummy, 16);
		if (level==0){
			for (int i=0; i<global3D.nfaces; i++){
				info[i] = true;
			}
		}

	};
	Class_Octant(const Class_Octant<3> &octant){
		x = octant.x;
		y = octant.y;
		z = octant.z;
		level = octant.level;
		marker = octant.marker;
		memcpy(info,octant.info,16);
	};

	bool operator ==(const Class_Octant<3> & oct2){	// Check if two octants are equal (no check on info)
		bool check = true;
		check = check && (x == oct2.x);
		check = check && (y == oct2.y);
		check = check && (z == oct2.z);
		check = check && (level == oct2.level);
		return check;
	}

	// ------------------------------------------------------------------------------- //
	// METHODS ----------------------------------------------------------------------- //

	// Basic Get/Set methods --------------------------------------------------------- //

public:
	uint32_t	getX() const{return x;};
	uint32_t	getY() const{return y;};
	uint32_t	getZ() const{return z;};
	uint8_t		getLevel() const{return level;};
	int8_t		getMarker() const{return marker;};
	bool		getBound(uint8_t face) const{				// Get if face is boundary
		return info[face];
	};

	bool		getPbound(uint8_t face) const{				// Get if face is process boundary
		return info[global3D.nfaces+face];
	};

	bool		getIsNewR() const{return info[12];};		// Get if octant is new after refinement
	bool		getIsNewC() const{return info[13];};		// Get if octant is new after coarsening
	bool		getNotBalance() const{return info[14];};	// Get if balancing-blocked octant
	bool		getIsGhost() const{return info[15];};		// For ghostbusters : get if octant is a ghost

	void		setMarker(int8_t marker){					// Set refinement/coarsening marker
		this->marker = marker;
	};

	void		setBalance(bool balance){					// Set if balancing-blocked octant
		info[14] = balance;
	};

private:
	void		setLevel(uint8_t level){
		this->level = level;
	};

	void 		setPbound(uint8_t face, bool flag){
		info[global3D.nfaces+face] = flag;
	};

	//-------------------------------------------------------------------------------- //
	// Other Get/Set methods --------------------------------------------------------- //

public:
	uint32_t	getSize() const{							// Get the size of octant
		uint32_t size = uint32_t(pow(double(2),double(MAX_LEVEL_3D-level)));
		return size;
	};

	// =================================================================================== //

	uint32_t	getArea() const{							// Get the face area of octant
		uint32_t area = uint32_t(pow(double(getSize()),2.0));
		return area;
	};

	// =================================================================================== //

	uint32_t	getVolume() const{							// Get the volume of octant
		uint64_t volume = uint32_t(pow(double(getSize()),3.0));
		return volume;
	};

	// =================================================================================== //

	double*		getCenter(){								// Get a pointer to an array of DIM with the coordinates of the center of octant
		uint8_t		i;
		double	dh;

		dh = double(getSize())/2.0;
		double *center = new double[3];

		center[0] = (double)x + dh;
		center[1] = (double)y + dh;
		center[2] = (double)z + dh;
		return center;
	};

	// =================================================================================== //

	uint32_t	(*getNodes())[3]{							// Get a pointer to the array (size [nnodes][DIM]) with the nodes of octant
		uint8_t		i, cx, cy, cz;
		uint32_t	dh;

		dh = getSize();
		uint32_t (*nodes)[3] = new uint32_t[global3D.nnodes][3];

		for (i = 0; i < global3D.nnodes; i++){
			cx = uint8_t(i%2);
			cy = uint8_t((i-4*(i/4))/2);
			cz = uint8_t(i/4);
			nodes[i][0] = x + cx*dh;
			nodes[i][1] = y + cy*dh;
			nodes[i][2] = z + cz*dh;

		}
		return nodes;
	};

	// =================================================================================== //

	void		getNodes(u32vector2D & nodes){				// Get a vector (size [nnodes][DIM]) with the nodes of octant
		uint8_t		i, cx, cy, cz;
		uint32_t	dh;

		dh = getSize();
		nodes.clear();
		nodes.resize(global3D.nnodes);

		for (i = 0; i < global3D.nnodes; i++){
			nodes[i].resize(3);
			cx = uint8_t(i%2);
			cy = uint8_t((i-4*(i/4))/2);
			cz = uint8_t(i/4);
			nodes[i][0] = x + cx*dh;
			nodes[i][1] = y + cy*dh;
			nodes[i][2] = z + cz*dh;
			nodes[i].shrink_to_fit();
		}
		nodes.shrink_to_fit();
	};

	// =================================================================================== //

	int8_t*		getNormal(uint8_t & iface){					// Get a pointer to the array (size [DIM]) with the normal of the iface
		uint8_t		i;
		int8_t* normal = new int8_t[3];

		for (i = 0; i < 3; i++){
			normal[i] = global3D.normals[iface][i];
		}
		return normal;
	};

	// =================================================================================== //

	void		getNormal(uint8_t & iface,					// Get a vector (size [nnodes][DIM]) with the normal of the iface
				vector<int8_t> & normal){
		uint8_t		i;

		normal.clear();
		normal.resize(3);
		for (i = 0; i < 3; i++){
			normal[i] = global3D.normals[iface][i];
		}
		normal.shrink_to_fit();
	};

	// =================================================================================== //

	uint64_t	computeMorton() const{						// Compute Morton index of the octant (without level)
		uint64_t morton = 0;
		morton = mortonEncode_magicbits(this->x,this->y,this->z);
		return morton;
	};
	uint64_t	computeMorton(){
		uint64_t morton = 0;
		morton = mortonEncode_magicbits(this->x,this->y,this->z);
		return morton;
	};

	// =================================================================================== //
	// Other methods													    			   //
	// =================================================================================== //

public:
	Class_Octant<3>	buildLastDesc(){								// Build last descendant of octant and return the last descendant octant (no info update)
		uint32_t delta = (uint32_t)pow(2.0,(double)((uint8_t)MAX_LEVEL_3D - level)) - 1;
		Class_Octant<3> last_desc(MAX_LEVEL_3D,x+delta,y+delta,z+delta);
		return last_desc;
	};

	// =================================================================================== //

	Class_Octant<3>	buildFather(){									// Build father of octant and return the father octant (no info update)
		uint32_t deltax = x%(uint32_t(pow(2.0,(double)((uint8_t)MAX_LEVEL_3D - (level-1)))));
		uint32_t deltay = y%(uint32_t(pow(2.0,(double)((uint8_t)MAX_LEVEL_3D - (level-1)))));
		uint32_t deltaz = z%(uint32_t(pow(2.0,(double)((uint8_t)MAX_LEVEL_3D - (level-1)))));
		Class_Octant<3> father(level-1, x-deltax, y-deltay, z-deltaz);
		return father;
	};

	// =================================================================================== //

	Class_Octant<3>*	buildChildren(){								// Builds children of octant and return a pointer to an ordered array children[nchildren] (info update)
		uint8_t xf,yf,zf;

		if (this->level < MAX_LEVEL_3D){
			Class_Octant<3>* children = new Class_Octant<3>[global3D.nchildren];
			for (int i=0; i<global3D.nchildren; i++){
				switch (i) {
				case 0 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					// Update interior face bound and pbound
					xf=1; yf=3; zf=5;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[0] = oct;
				}
				break;
				case 1 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.x += dh;
					// Update interior face bound and pbound
					xf=0; yf=3; zf=5;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[1] = oct;
				}
				break;
				case 2 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.y += dh;
					// Update interior face bound and pbound
					xf=1; yf=2; zf=5;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[2] = oct;
				}
				break;
				case 3 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.x += dh;
					oct.y += dh;
					// Update interior face bound and pbound
					xf=0; yf=2; zf=5;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[3] = oct;
				}
				break;
				case 4 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.z += dh;
					// Update interior face bound and pbound
					xf=1; yf=3; zf=4;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[4] = oct;
				}
				break;
				case 5 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.x += dh;
					oct.z += dh;
					// Update interior face bound and pbound
					xf=0; yf=3; zf=4;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[5] = oct;
				}
				break;
				case 6 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.y += dh;
					oct.z += dh;
					// Update interior face bound and pbound
					xf=1; yf=2; zf=4;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[6] = oct;
				}
				break;
				case 7 :
				{
					Class_Octant<3> oct(*this);
					oct.setMarker(max(0,oct.marker-1));
					oct.setLevel(oct.level+1);
					oct.info[12]=true;
					uint32_t dh = oct.getSize();
					oct.x += dh;
					oct.y += dh;
					oct.z += dh;
					// Update interior face bound and pbound
					xf=0; yf=2; zf=4;
					oct.info[xf] = oct.info[xf+global3D.nfaces] = false;
					oct.info[yf] = oct.info[yf+global3D.nfaces] = false;
					oct.info[zf] = oct.info[zf+global3D.nfaces] = false;
					children[7] = oct;
				}
				break;
				}
			}
			return children;
		}
		else{
			Class_Octant<3>* children = new Class_Octant<3>[0];
			writeLog("Max level reached ---> No Children Built");
			return children;
		}
	};

	// =================================================================================== //

	uint64_t* 		computeHalfSizeMorton(uint8_t iface, 			// Computes Morton index (without level) of "n=sizehf" half-size (or same size if level=maxlevel)
										  uint32_t & sizehf){		// possible neighbours of octant throught face iface (sizehf=0 if boundary octant)
		uint32_t dh,dh2;
		uint64_t morton;
		uint32_t nneigh;
		uint8_t i,cx,cy,cz;

		nneigh = (level < MAX_LEVEL_3D) ? global3D.nchildren/2 : 1;
		dh = (level < MAX_LEVEL_3D) ? getSize()/2 : getSize();
		dh2 = getSize();

		if (info[iface]){
	//		uint64_t* Morton = new uint64_t[0];
			sizehf = 0;
	//		return Morton;
			return NULL;
		}
		else{
			uint64_t* Morton = new uint64_t[nneigh];
			switch (iface) {
			case 0 :
			{
				for (i=0; i<nneigh; i++){
					cy = (i==1)||(i==3);
					cz = (i==2)||(i==3);
					Morton[i] = mortonEncode_magicbits(this->x-dh,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 1 :
			{
				for (i=0; i<nneigh; i++){
					cy = (i==1)||(i==3);
					cz = (i==2)||(i==3);
					Morton[i] = mortonEncode_magicbits(this->x+dh2,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 2 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1)||(i==3);
					cz = (i==2)||(i==3);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y-dh,this->z+dh*cz);
				}
			}
			break;
			case 3 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1)||(i==3);
					cz = (i==2)||(i==3);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2,this->z+dh*cz);
				}
			}
			break;
			case 4 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1)||(i==3);
					cy = (i==2)||(i==3);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z-dh);
				}
			}
			break;
			case 5 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1)||(i==3);
					cy = (i==2)||(i==3);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2);
				}
			}
			break;
			}
			sizehf = nneigh;
			return Morton;
		}


	};

	// =================================================================================== //

	uint64_t* 		computeMinSizeMorton(uint8_t iface, 			// Computes Morton index (without level) of "n=sizem" min-size (or same size if level=maxlevel)
										 const uint8_t & maxdepth,	// possible neighbours of octant throught face iface (sizem=0 if boundary octant)
										 uint32_t & sizem){
		uint32_t dh,dh2;
		uint64_t morton;
		uint32_t nneigh, nline;
		uint32_t i,cx,cy,cz;

		nneigh = (level < MAX_LEVEL_3D) ? uint32_t(pow(2.0,double(2*(maxdepth-level)))) : 1;
		dh = (level < MAX_LEVEL_3D) ? uint32_t(pow(2.0,double(MAX_LEVEL_3D - maxdepth))) : getSize();
		dh2 = getSize();
		nline = uint32_t(pow(2.0,double((maxdepth-level))));

		if (info[iface]){
	//		uint64_t* Morton = new uint64_t[0];
			sizem = 0;
	//		return Morton;
			return NULL;
		}
		else{
			uint64_t* Morton = new uint64_t[nneigh];
			switch (iface) {
			case 0 :
			{
				for (i=0; i<nneigh; i++){
					cy = (i/nline);
					cz = (i%nline);
					Morton[i] = mortonEncode_magicbits(this->x-dh,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 1 :
			{
				for (i=0; i<nneigh; i++){
					cy = (i/nline);
					cz = (i%nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh2,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 2 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cz = (i%nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y-dh,this->z+dh*cz);
				}
			}
			break;
			case 3 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cz = (i%nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2,this->z+dh*cz);
				}
			}
			break;
			case 4 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cy = (i%nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z-dh);
				}
			}
			break;
			case 5 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cy = (i%nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2);
				}
			}
			break;
			}
			sizem = nneigh;
			sort(Morton,Morton+nneigh);
			return Morton;
		}

	};

	// =================================================================================== //

	uint64_t* 		computeVirtualMorton(uint8_t iface, 			// Computes Morton index (without level) of possible (virtual) neighbours of octant throught iface
										 const uint8_t & maxdepth,	// Checks if balanced or not and uses half-size or min-size method (sizeneigh=0 if boundary octant)
										 uint32_t & sizeneigh){
		if (getNotBalance()){
			return computeMinSizeMorton(iface,
					maxdepth,
					sizeneigh);
		}
		else{
			return computeHalfSizeMorton(iface,
					sizeneigh);
		}
	};

	// =================================================================================== //

	uint64_t* 		computeEdgeHalfSizeMorton(uint8_t iedge, 		// Computes Morton index (without level) of "n=sizehf" half-size (or same size if level=maxlevel)
										  uint32_t & sizehf){		// possible neighbours of octant throught face iface (sizehf=0 if boundary octant)
		uint32_t dh,dh2;
		uint64_t morton;
		uint32_t nneigh;
		int8_t i,cx,cy,cz;
		uint8_t iface1, iface2;

		nneigh = (level < MAX_LEVEL_3D) ? 2 : 1;
		dh = (level < MAX_LEVEL_3D) ? getSize()/2 : getSize();
		dh2 = getSize();
		iface1 = global3D.edgeface[iedge][0];
		iface2 = global3D.edgeface[iedge][1];

		if (info[iface1] || info[iface2]){
			sizehf = 0;
			return NULL;
		}
		else{
			uint64_t* Morton = new uint64_t[nneigh];
			switch (iedge) {
			case 0 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = (i==1);
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 1 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = (i==1);
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 2 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1);
					cy = -1;
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 3 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1);
					cy = 1;
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh*cz);
				}
			}
			break;
			case 4 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = -1;
					cz = (i==1);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 5 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = -1;
					cz = (i==1);
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 6 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = 1;
					cz = (i==1);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh*cz);
				}
			}
			break;
			case 7 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = 1;
					cz = (i==1);
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh2*cy,this->z+dh*cz);
				}
			}
			break;
			case 8 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = (i==1);
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2*cz);
				}
			}
			break;
			case 9 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = (i==1);
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh2*cz);
				}
			}
			break;
			case 10 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1);
					cy = -1;
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2*cz);
				}
			}
			break;
			case 11 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i==1);
					cy = 1;
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh2*cz);
				}
			}
			break;
			}
			sizehf = nneigh;
			return Morton;
		}


	};

	// =================================================================================== //

	uint64_t* 		computeEdgeMinSizeMorton(uint8_t iedge, 		// Computes Morton index (without level) of "n=sizem" min-size (or same size if level=maxlevel)
										const uint8_t & maxdepth,	// possible neighbours of octant throught edge iedge (sizem=0 if boundary octant)
										 uint32_t & sizem){
		uint32_t dh,dh2;
		uint64_t morton;
		uint32_t nneigh, nline;
		uint32_t i;
		int32_t cx,cy,cz;
		uint8_t iface1, iface2;


		nneigh = (level < MAX_LEVEL_3D) ? uint32_t(pow(2.0,double((maxdepth-level)))) : 1;
		dh = (level < MAX_LEVEL_3D) ? uint32_t(pow(2.0,double(MAX_LEVEL_3D - maxdepth))) : getSize();
		dh2 = getSize();
		nline = uint32_t(pow(2.0,double((maxdepth-level))));
		iface1 = global3D.edgeface[iedge][0];
		iface2 = global3D.edgeface[iedge][1];

		if (info[iface1] || info[iface2]){
			sizem = 0;
			return NULL;
		}
		else{
			uint64_t* Morton = new uint64_t[nneigh];
			switch (iedge) {
			case 0 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = (i/nline);
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 1 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = (i/nline);
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 2 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cy = -1;
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 3 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cy = 1;
					cz = -1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh*cz);
				}
			}
			break;
			case 4 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = -1;
					cz = (i/nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 5 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = -1;
					cz = (i/nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh*cz);
				}
			}
			break;
			case 6 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = 1;
					cz = (i/nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh*cz);
				}
			}
			break;
			case 7 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = 1;
					cz = (i/nline);
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh2*cy,this->z+dh*cz);
				}
			}
			break;
			case 8 :
			{
				for (i=0; i<nneigh; i++){
					cx = -1;
					cy = (i/nline);
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2*cz);
				}
			}
			break;
			case 9 :
			{
				for (i=0; i<nneigh; i++){
					cx = 1;
					cy = (i/nline);
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh2*cz);
				}
			}
			break;
			case 10 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cy = -1;
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2*cz);
				}
			}
			break;
			case 11 :
			{
				for (i=0; i<nneigh; i++){
					cx = (i/nline);
					cy = 1;
					cz = 1;
					Morton[i] = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh2*cz);
				}
			}
			break;
			}
			sizem = nneigh;
			sort(Morton,Morton+nneigh);
			return Morton;
		}
	};

	// =================================================================================== //

	uint64_t* 		computeEdgeVirtualMorton(uint8_t iedge, 		// Computes Morton index (without level) of possible (virtual) neighbours of octant throught iface
										 const uint8_t & maxdepth,	// Checks if balanced or not and uses half-size or min-size method (sizeneigh=0 if boundary octant)
										 uint32_t & sizeneigh){
		if (getNotBalance()){
			return computeEdgeMinSizeMorton(iedge,
					maxdepth,
					sizeneigh);
		}
		else{
			return computeEdgeHalfSizeMorton(iedge,
					sizeneigh);
		}

	};

	// =================================================================================== //

	uint64_t 		computeNodeHalfSizeMorton(uint8_t inode, 		// Computes Morton index (without level) of "n=sizehf" half-size (or same size if level=maxlevel)
										  uint32_t & sizehf){		// possible neighbours of octant throught face iface (sizehf=0 if boundary octant)
		uint32_t dh,dh2;
		uint64_t morton;
		uint32_t nneigh;
		uint8_t i;
		int8_t cx,cy,cz;
		uint8_t iface1, iface2, iface3;
	 	nneigh = 1;
		dh = (level < MAX_LEVEL_3D) ? getSize()/2 : getSize();
		dh2 = getSize();
		iface1 = global3D.nodeface[inode][0];
		iface2 = global3D.nodeface[inode][1];
		iface3 = global3D.nodeface[inode][2];

		if (info[iface1] || info[iface2] || info[iface3]){
			sizehf = 0;
			return this->computeMorton();
		}
		else{
			uint64_t Morton;
			switch (inode) {
			case 0 :
			{
				cx = -1;
				cy = -1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
			}
			break;
			case 1 :
			{
				cx = 1;
				cy = -1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh*cz);
			}
			break;
			case 2 :
			{
				cx = -1;
				cy = 1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh*cz);
			}
			break;
			case 3 :
			{
				cx = 1;
				cy = 1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh2*cy,this->z+dh*cz);
			}
			break;
			case 4 :
			{
				cx = -1;
				cy = -1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2*cz);
			}
			break;
			case 5 :
			{
				cx = 1;
				cy = -1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh2*cz);
			}
			break;
			case 6 :
			{
				cx = -1;
				cy = 1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh2*cz);
			}
			break;
			case 7 :
			{
				cx = 1;
				cy = 1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh2*cy,this->z+dh2*cz);
			}
			break;
			}
			sizehf = nneigh;
			return Morton;
		}
	};

	// =================================================================================== //

	uint64_t 		computeNodeMinSizeMorton(uint8_t inode, 		// Computes Morton index (without level) of "n=sizem" min-size (or same size if level=maxlevel)
										 const uint8_t & maxdepth,	// possible neighbours of octant throught face iface (sizem=0 if boundary octant)
										 uint32_t & sizehf){
		uint32_t dh,dh2;
		uint64_t morton;
		uint32_t nneigh;
		uint8_t i;
		int8_t cx,cy,cz;
		uint8_t iface1, iface2, iface3;

		nneigh = 1;
		dh = (level < MAX_LEVEL_3D) ? uint32_t(pow(2.0,double(MAX_LEVEL_3D - maxdepth))) : getSize();
		dh2 = getSize();
		iface1 = global3D.nodeface[inode][0];
		iface2 = global3D.nodeface[inode][1];
		iface3 = global3D.nodeface[inode][2];

		if (info[iface1] || info[iface2] || info[iface3]){
			sizehf = 0;
			return this->computeMorton();
		}
		else{
			uint64_t Morton;
			switch (inode) {
			case 0 :
			{
				cx = -1;
				cy = -1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh*cz);
			}
			break;
			case 1 :
			{
				cx = 1;
				cy = -1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh*cz);
			}
			break;
			case 2 :
			{
				cx = -1;
				cy = 1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh*cz);
			}
			break;
			case 3 :
			{
				cx = 1;
				cy = 1;
				cz = -1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh2*cy,this->z+dh*cz);
			}
			break;
			case 4 :
			{
				cx = -1;
				cy = -1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh*cy,this->z+dh2*cz);
			}
			break;
			case 5 :
			{
				cx = 1;
				cy = -1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh*cy,this->z+dh2*cz);
			}
			break;
			case 6 :
			{
				cx = -1;
				cy = 1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh*cx,this->y+dh2*cy,this->z+dh2*cz);
			}
			break;
			case 7 :
			{
				cx = 1;
				cy = 1;
				cz = 1;
				Morton = mortonEncode_magicbits(this->x+dh2*cx,this->y+dh2*cy,this->z+dh2*cz);
			}
			break;
			}
			sizehf = nneigh;
			return Morton;
		}

	};

	// =================================================================================== //

	uint64_t 		computeNodeVirtualMorton(uint8_t inode, 		// Computes Morton index (without level) of possible (virtual) neighbours of octant throught iface
										 const uint8_t & maxdepth,	// Checks if balanced or not and uses half-size or min-size method (sizeneigh=0 if boundary octant)
										 uint32_t & sizeneigh){
		if (getNotBalance()){
			return computeNodeMinSizeMorton(inode,
										maxdepth,
										sizeneigh);
		}
		else{
			return computeNodeHalfSizeMorton(inode,
										sizeneigh);
		}
	};

	// ------------------------------------------------------------------------------- //


};//end Class_Octant<3>
//#endif

