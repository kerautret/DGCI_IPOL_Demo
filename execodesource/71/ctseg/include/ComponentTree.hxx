//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include <iostream>
#include <set>
#include <stack>
#include <map>

#include "ComponentTree.h"

namespace LibTIM {

using std::vector;
using std::map;

template <class T>
ComponentTree<T>::ComponentTree( Image< T > & img , Image <U8> &gt)
    :m_root(0),m_img(img)
{
    FlatSE connexity;
    connexity.make2DN8();
    SalembierRecursiveImplementation<T> strategy(this,connexity);

    m_root=strategy.computeTree(gt);
    strategy.computeAttributes(m_root);
}


template <class T>
ComponentTree<T>::ComponentTree( Image< T > & img , Image <U8> &gt, FlatSE &connexity)
    :m_root(0),m_img(img)
{

    SalembierRecursiveImplementation<T> strategy(this,connexity);

    m_root=strategy.computeTree(gt);
    strategy.computeAttributes(m_root);
}

template <class T>
ComponentTree<T>::~ComponentTree()
{
    erase_tree();
}

template <class T>
void ComponentTree<T>::erase_tree()
{
    int tot=0;
    if(m_root!=0)
    {
        std::queue <Node *> fifo;
        fifo.push(m_root);
        while(!fifo.empty() )
        {
            Node *curNode=fifo.front();
            fifo.pop();

            for(unsigned int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
            delete curNode;
            tot++;
        }
    }
}


template <class T>
void ComponentTree<T>::constructNode(Image <T> &res, Node *node)
{
    std::queue<Node *> fifo;
    fifo.push(node);

    while(!fifo.empty() )
    {
        Node *tmp=fifo.front();
        fifo.pop();

        for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
        {
            if(res(*it)<(T)node->h)
                res(*it)=(T)node->h;
        }
        for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
            fifo.push(*it);
    }
}


template <class T>
void ComponentTree<T>::setFalse()
{
    if(m_root!=0)
    {
        std::queue<Node *> fifo;
        fifo.push(m_root);

        while(!fifo.empty())
        {
            Node *tmp=fifo.front();
            fifo.pop();

            tmp->active=false;

            for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
                fifo.push(*it);
        }
    }
}

template <class T>
void ComponentTree<T>::printSize()
{
    int totalSize=0;
    if(m_root!=0)
    {
        std::queue <Node *> fifo;
        fifo.push(m_root);
        totalSize+=sizeof(*m_root);
        while(!fifo.empty() )
        {
            Node *tmp=fifo.front();
            fifo.pop();

            totalSize+=sizeof(*tmp);

            for(int i=0; i<tmp->childs.size(); i++)
            {

                fifo.push(tmp->childs[i]);
            }
        }
    }
    std::cout << "Total size of tree is " << totalSize/1024 << "kO  (" << totalSize << " bytes)\n";
}

template <class T>
int ComponentTree<T>::writeDot(const char *filename)
{
    if(m_root!=0)
    {
        std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
        if(!file)
        {
            std::cerr << "File I/O error\n";
            return 0;
        }

        file << "digraph G {\n";

        std::queue <Node *> fifo;
        fifo.push(m_root);
        while(!fifo.empty() )
        {
            Node *tmp=fifo.front();
            fifo.pop();
            if(tmp->active==true)
            {
                // write father->son relation if the node is not the root
                if(tmp->father!=tmp)
                {
                    file << "\t" << " \"" << tmp->father->h << "," << tmp->father->label <<	";\n";
                }
            }
            // push the childs
            for(int i=0; i<tmp->childs.size(); i++)
            {
                fifo.push(tmp->childs[i]);
            }


        }

        file << "}\n";

        file.close();
        return 1;
    }
    else
        return 0;
}



//////////////////////////////////////////////////////////////
//
//
// Attributes computation
//
//
//
//////////////////////////////////////////////////////////////


template <class T>
int SalembierRecursiveImplementation<T>::computeN(Node *tree)
{
    if(tree!=0)
    {
        Node::ContainerChilds::iterator it;
        for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
        {
            tree->n+=computeN(*it);
        }
        return tree->n;
    }
    // error
    else return -1;
}





template <class T>
int SalembierRecursiveImplementation<T>::computeAttributes(Node *tree)
{
    if(tree!=0)
    {
        tree->n=computeN(tree);
    }
    return 0;
}

//////////////////////////////////////////////////////////////


template <class T>
inline int SalembierRecursiveImplementation<T>::update_attributes(Node *n, TOffset &imBorderOffset, Image<U8> &gt)
{
    //conversion offset imBorder->im
    Point <TCoord> imCoord=imBorder.getCoord(imBorderOffset);
    imCoord.x-=back[0];
    imCoord.y-=back[1];
    imCoord.z-=back[2];
    TOffset imOffset=imCoord.x+imCoord.y*oriSize[0]+imCoord.z*oriSize[0]*oriSize[1];

    n->pixels.push_back(imOffset);
    this->indexNodes(imOffset)=n;

    // update attributes related to ps and n (see paper)
    if(gt(imCoord)!=0)
    {
        n->ps++;
    }
    else
    {
        n->n++;
    }
    return 0;
}


template <class T>
void SalembierRecursiveImplementation<T>::link_node(Node *tree, Node *child)
{
    child->father=tree;
    tree->childs.push_back(child);
}

template <class T>
Node *SalembierRecursiveImplementation<T>::new_node(int h, int n)
{
    Node *res=new Node;
    res->h=h;
    res->label=n;

    totalNodes++;

    return res;
}


// Implementation of component-tree (max-tree) computation based on Salembier's algorithm published in:
// P. Salembier, A. Oliveras, L. Garrido
// Anti-extensive Connected Operators for Image and Sequence Processing IEEE Transactions on Image Processing
// Vol.7 Num. 4, p. 555-670, 1998
template <class T>
inline int SalembierRecursiveImplementation<T>::flood(int h, Image<U8> &gt)
{
    int m;

    while(!hq[h].empty())
    {

        TOffset p=hq[h].front();
        hq[h].pop();

        STATUS(p)=number_nodes[h];

        if(index[h][STATUS(p)]==0)
        {
            index[h][STATUS(p)]=this->new_node(indexToH(h),STATUS(p));;
        }

        update_attributes(index[h][STATUS(p)],p,gt);

        FlatSE::iterator it;
        FlatSE::iterator end=se.end();

        for(it=se.begin(); it!=end; ++it)
        {
            TOffset q=p+*it;

            if(STATUS(q)==ACTIVE)
            {

                hq[hToIndex(imBorder(q))].push(q);
                STATUS(q)=NOT_ACTIVE;

                node_at_level[hToIndex(imBorder(q))]=true;

                if(imBorder(q)>imBorder(p))
                {
                    m=hToIndex(imBorder(q));
                    do 	{
                        m=this->flood(m,gt);
                    } while(m!=h);
                }
            }
        }

    }
    //End of recursion: we have reached a regional maximum
    number_nodes[h]=number_nodes[h]+1;

    m=h-1;
    while(m>=hToIndex(hMin) && node_at_level[m]==false) m--;

    if(m>=hToIndex(hMin) )
    {
        int i=number_nodes[h]-1;
        int j=number_nodes[m];
        if(index[m][j]==0)
        {
            index[m][j]=new_node(indexToH(m),j);
        }
        this->link_node(index[m][j],index[h][i]);
    }
    else
    {
        //The father of root is itself
        index[hToIndex(hMin)][0]->father=index[hToIndex(hMin)][0];
    }
    node_at_level[h]=false;
    return m;
}

template <class T>
Node * SalembierRecursiveImplementation<T>::computeTree(Image<U8> &gt)
{
    //Put the first pixel with value hMin in the queue
    typename Image<T>::iterator it;
    typename Image<T>::iterator end=imBorder.end();
    TOffset offset=0;
    for(it=imBorder.begin(); it!=end; ++it,offset++)
        if(*it==hMin && STATUS(offset)==ACTIVE)
        {
            hq[hToIndex(hMin)].push(offset);
            break;
        }

    node_at_level[hToIndex(hMin)]=true;

    this->flood(hToIndex(hMin),gt);

    Node *root=index[hToIndex(hMin)][0];

    this->m_parent->STATUS=this->STATUS.crop(back[0],this->STATUS.getSizeX()-front[0],
                                             back[1],this->STATUS.getSizeY()-front[1],
                                             back[2],this->STATUS.getSizeZ()-front[2]);

    this->m_parent->index=this->index;
    this->m_parent->indexNodes=this->indexNodes;
    this->m_parent->totalNodes=this->totalNodes;

    return root;
}

//initialize global index for nodes
template <class T>
int SalembierRecursiveImplementation<T>::init(Image <T> &img, FlatSE &connexity)
{
    FlatSE se=connexity;

    const TSize *tmpSize=img.getSize();
    const TCoord *tmpBack=se.getNegativeOffsets();
    const TCoord *tmpFront=se.getPositiveOffsets();

    for(int i=0; i<=2; i++)
    {
        oriSize[i]=tmpSize[i];
        back[i]=tmpBack[i];
        front[i]=tmpFront[i];
    }

    STATUS.setSize(img.getSize());
    STATUS.fill(ACTIVE);

    imBorder=img.addBorders(back,front,BORDER);
    STATUS=STATUS.addBorders(back,front,BORDER_STATUS);
    se.setContext(imBorder.getSize());

    indexNodes.setSize(img.getSize());

    this->hMin=img.getMin();
    this->hMax=img.getMax();
    this->numberOfLevels=hMax-hMin+1;

    index.resize(numberOfLevels);

    hq=new std::queue<TOffset> [numberOfLevels];

    //we take a (max-min+1) * (number of grey-levels at level h)
    // so we compute histogram

    int *histo;
    histo =new int[numberOfLevels];
    for(int i=0; i<numberOfLevels; i++)
        histo[i]=0;

    typename Image<T>::iterator it;
    typename Image<T>::iterator end=img.end();

    for(it=img.begin(); it!=end; ++it)
    {
        int n=*it-hMin;
        assert(n>=0 && n<numberOfLevels);
        histo[n]++;
    }

    for(int i=0; i<numberOfLevels; i++)
    {
        int sizeOfLevelH=histo[i];
        index[i].resize(sizeOfLevelH);

        for(int j=0; j<sizeOfLevelH; j++)
            index[i][j]=0;
    }


    this->se=se;

    this->number_nodes.resize(numberOfLevels);
    this->node_at_level.resize(numberOfLevels);

    for(int i=0; i<numberOfLevels; i++)
    {
        this->number_nodes[i]=0;
        this->node_at_level[i]=false;
    }

    delete[] histo;

    return 0;
}


}
