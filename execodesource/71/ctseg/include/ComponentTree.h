//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef ComponentTree_h
#define ComponentTree_h

#include <vector>
#include <limits>
#include <queue>

#include <include/Image.h>
#include <include/FlatSE.h>
#include <include/Types.h>

namespace LibTIM {

using std::vector;

/** \defgroup ccTree Component-Tree Based Algorithms
	\ingroup Morpho
**/

/*@{*/

/** @brief Component tree representation of an image
 *	The component tree (or max-tree) is a particular image structure. Nodes represents flat zones.
 *	Leafs represents regional maxima (max-tree) or minima (min-tree). Father-son relation represents
 *	an inclusion relation (with respect to level sets representation of image).
 *	This structure is efficient for computing attribute openings (area, contrast, ...)
**/
//const int localMax=std::numeric_limits<int>::max();
//const int localMin=std::numeric_limits<int>::min();

struct Node {
        Node():
        label(-1),  n(0), ps(0), calpha(0), status(true), active(true), father(0)
        {
			pixels.reserve(7); childs.reserve(5);
        }
        //Attributes
		int label;
        int h;

        // attributes for nodes selection (see paper)
		int n;
		int ps;
		double calpha;

		bool status;
		bool active;

		Node *father;
		typedef std::vector<TOffset> ContainerPixels;
		ContainerPixels pixels;
                ContainerPixels pixels_tmp;
		typedef  std::vector<Node *> ContainerChilds;
		ContainerChilds childs;
		};


typedef std::vector<std::vector<Node *> > IndexType;


template <class T>
class ComponentTreeStrategy;

template <class T>
class SalembierRecursiveImplementation;

template <class T>
class ComponentTree {
	public:
		// constructor based on binary ground-truth used for nodes selection (see paper)
		ComponentTree(Image <T> &img, Image <U8> &gt);
		ComponentTree(Image <T> &img, Image <U8> &gt,FlatSE &connexity);
		//Copy constructor
		ComponentTree(ComponentTree <T> &tree);
		~ComponentTree();

		/**
		  * @brief Print size of tree (in kilobytes)
		**/
		void printSize();

		/**
		  * @brief Print tree in a .dot file to generate clean drawing
		**/

		int writeDot(const char *filename);

        /**
          * @brief Compute the binary image (in res) containing the node
        **/

        void constructNode(Image <T> &res, Node *node);


        void setFalse();


		void erase_tree();

		// Internal structure
		// root node

		Node *m_root;

		//original data
		Image<T> m_img;

		//STATUS image
		Image <int> STATUS;

		//max-tree index
		IndexType index;

        Image<Node *> indexNodes;

		int totalNodes;

};

/** @brief Abstract class for strategy to compute component tree
  *	Abstract class encapsulating the various strategies to compute Max-tree
  *
  *
**/
template <class T>
class ComponentTreeStrategy {
	public:

    ComponentTreeStrategy() {}
    virtual ~ComponentTreeStrategy() {}

    virtual Node *computeTree(Image<U8> &gt)=0;
	virtual int computeAttributes(Node *tree)=0;

};

/** @brief Salembier recursive implementation
 **/

template <class T>
class SalembierRecursiveImplementation: public ComponentTreeStrategy <T> {
	public:

    SalembierRecursiveImplementation(ComponentTree <T> *parent, FlatSE &connexity)
	:m_parent(parent)
		{
		this->totalNodes=0;
        this->init(m_parent->m_img, connexity);
		}

	~SalembierRecursiveImplementation()
		{
		delete[] hq;
		}

	Node *computeTree(Image<U8> &gt);

	int computeAttributes(Node *tree);
    int computeN(Node *tree);

	private:
		//Helper functions
		inline int update_attributes(Node *n, TOffset &imBorderOffset, Image<U8> &gt);
		inline int flood(int m, Image<U8> &gt) ;
		void link_node(Node *tree, Node *child) ;
		Node *new_node(int h, int n)  ;
        int init(Image <T> &img, FlatSE &connexity) ;

		int hToIndex(int h)  {return h-hMin;}
		int indexToH(int h)  {return h+hMin;}

		//members
		Image <T> imBorder;
		FlatSE se;
		TSize oriSize[3];

		static const T BORDER=T(0);
		static const int NOT_ACTIVE=-1;
		static const int ACTIVE=-2;
		static const int BORDER_STATUS=-3;
		TCoord back[3];
		TCoord front[3];
		/** @brief Hierarchical queue
		**/
		//typedef std::map <int, std::queue<TOffset> > HierarchicalQueue;
		typedef std::queue<TOffset> * HierarchicalQueue;

        HierarchicalQueue hq;

        T hMin;
        T hMax;
		// size of different containers
        int numberOfLevels;

        int totalNodes;

        Image <int> STATUS;
        vector <int> number_nodes;
        vector <bool> node_at_level;
		// For now, container for accessing nodes by level and cc number
		//typedef std::map <T, std::map<TLabel,  Node *> > IndexType;
		//typedef Node *** IndexType;

		IndexType index;
        Image<Node *> indexNodes;

		ComponentTree<T> *m_parent;
};


/*@}*/

}//end namespace

#include "ComponentTree.hxx"
#endif
