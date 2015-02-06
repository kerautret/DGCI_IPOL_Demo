//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#include <cstring>
#include "include/ComponentTree.h"
#include "include/Image.h"

using namespace std;
// for LibTIM classes
using namespace LibTIM;


// Main algorithm
// Input:
// -tree: initialized and attributed component-tree
// -alpha: parameter (floating number strictly comprised between 0 and 1)
// Output:
// -selectedNodes: list of nodes selected by the algorithm
vector <Node *> computeSolution(ComponentTree<U8> &tree, double alpha)
{
    vector <Node *> orderedNodes;
    vector <Node *> selectedNodes;

    // Breadth-first scan of the tree
    // The nodes are stored in the vector orderedNodes
    std::queue<Node *> fifo;
    fifo.push(tree.m_root);

    while(!fifo.empty())
    {
        Node *tmp=fifo.front();
        fifo.pop();

        orderedNodes.push_back(tmp);

        for(unsigned int i=0; i<tmp->childs.size(); i++)
        {
            fifo.push(tmp->childs[i]);
        }
    }

    double exprl,exprr;

    // Scan all the nodes from the leafs in reverse order
    // It ensures that all the nodes are processed before their father
    for(int i=orderedNodes.size()-1; i>=0; i--)
    {
        // Take the following node in the list
        Node *tmp=orderedNodes[i];

        // Compute left and right expressions (costs to keep or skip the node)
        exprl=alpha*tmp->n;
        exprr=(1-alpha)*tmp->ps;

        // if tmp is not the root
        if(tmp->father!=tmp)
        {
            // if tmp is a leaf
            if(tmp->childs.size()==0)
            {
                // The node is kept
                if(exprl<exprr)
                {
                    tmp->calpha=exprl;
                    selectedNodes.push_back(tmp);
                }
                // The node is skipped
                else
                {
                    tmp->calpha=exprr;
                }
            }
            // if tmp is not a leaf
            else
            {
                // Compute the sum of costs of all child nodes
                double sum=0.0;
                for(unsigned int i=0;i<tmp->childs.size();i++)
                {
                    sum+=tmp->childs[i]->calpha;
                }

                // Add this sum to exprr (cost to skip the node)
                exprr+=sum;

                if(exprl<exprr)
                {
                    // The node is kept
                    tmp->calpha=exprl;
                    selectedNodes.push_back(tmp);
                }
                else
                {
                    // The node is skipped
                    tmp->calpha=exprr;
                }
            }
        }
    }

    return selectedNodes;
}


// This program is working only with PGM grey-level images
// To convert a grey-level image from any "standard" format in PGM you can use ImageMagick:
//  -convert image.png image.pgm
// Command line: ctseg <source> <marker> <alpha> with:
//  -<source> : name of an existing PGM 8 bits image
//  -<marker> : name of an existing PGM 8 bits image
//  -<alpha>  : floating number (0<= alpha <=1)
//  -[negate] : negate the source image.
//              By default, the program assumes bright objects on dark background.
//              To extract dark objects on bright background, the source image must be negated
//              (which is equivalent to compute the dual component-tree (min-tree)

int main(int argc, char *argv[])
{
    if(argc<4)
    {
        cout<<"Usage: " << argv[0] << " <source> <marker> <alpha> [negate]\n";
        exit(1);
    }

    // Declaration of:
    // - imSrc (source image)
    // - imMarker (marker image)
    // - imRes (result image)
    // of type Image<U8> (grey-scale 8 bits images)

    Image <U8> imSrc;
    Image <U8> imMarker;
    Image <U8> imRes;
    double alpha;

    // Load source image imSrc, 2D grey-scale (8 bits)
    Image<U8>::load(argv[1],imSrc);

    // negate the source image if specified
    if(argc==5 && strcmp(argv[4],"negate")==0)
        {
        for(int i=0; i<imSrc.getBufSize(); i++) imSrc(i)=255-imSrc(i);
        }

    // Load source image imMarker, 2D grey-scale (8 bits)
    // imMarker and imSrc must have the same size
    // imMarker is processed as a binary image with:
    // -black pixel has value 0
    // -white pixel has a value different from 0
    Image<U8>::load(argv[2],imMarker);

    if(imSrc.getSizeX()!=imMarker.getSizeX() || imSrc.getSizeY()!=imMarker.getSizeY() )
    {
        cout<<"Error: source and marker image must have the same size\n";
        exit(1);
    }

    // alpha parameter (alpha is a floating number which must be comprised between 0 and 1)
    alpha=atof(argv[3]);

    if(alpha<0 || alpha>1)
    {
        cout<<"Error: alpha must be comprised between 0 and 1\n";
        exit(1);
    }

    // Note that the sequel of the program is also valid for 3D grey-scale images.

    FlatSE connexity;
    //8-connexity
    connexity.make2DN8();
    //To obtain 4-connexity:
    //connexity.make2DN4();

    // Compute the component-tree structure related to
    // - the source image imSrc
    // - the marker image imMarker
    // - the connexity (usually 4- or 8- in 2D, 6- 18- 26- in 3D)
    // The tree topology depends only on the source image.
    // The marker image is used to compute, for each node of the tree, the n and ps attributes
    // The attributes n and ps for each leaf are computed incrementally during the tree computation
    ComponentTree<U8> tree(imSrc,imMarker,connexity);

    // Compute the selected nodes
    vector<Node *> selectedNodes;
    selectedNodes=computeSolution(tree,alpha);

    // Computation of the result image from the set of selected nodes
    imRes.setSize(imSrc.getSize());
    imRes.fill(0);
    for(unsigned int i=0; i<selectedNodes.size(); i++)
    {
        // Draw the node in imRes
        // i.e. set all pixels belonging to the node to the grey-level of node
        tree.constructNode(imRes, selectedNodes[i]);
    }

    // Save result (grey-scale) image
    // To obtain a binary version, threshold the result at level 1
    imRes.save("result.pgm");
}

