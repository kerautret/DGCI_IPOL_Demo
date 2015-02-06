#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/boards/Board2D.h>
#include <DGtal/topology/helpers/Surfaces.h>
#include <DGtal/geometry/curves/FreemanChain.h>
#include <DGtal/io/colormaps/GradientColorMap.h>
#include <DGtal/topology/helpers/Surfaces.h>

using namespace DGtal;
using namespace Z2i;


int main(int argc, char** argv)
{

  Point ptL(-1, -1);
  Point ptU(3, 3);
  Domain d (ptL, ptU);    
    
  Point pt0(0,0);
  Point pt1(1,0);
  Point pt2(0,1);
  Point pt3(2,1);
  Point pt4(1,2);
  Point pt5(2,2);
  
  
  DigitalSet aSet( d );
  aSet.insert(pt0);
  aSet.insert(pt1);
  aSet.insert(pt2);
  aSet.insert(pt3);
  aSet.insert(pt4);
  
  Board2D boardAdj;

  boardAdj << d;    

  
  std::set<SCell> bdry;
  Z2i::KSpace ks;  
  ks.init( ptL, ptU, true );

  boardAdj << aSet;
  
  // Extracting the surface boundary of the shape
  Surfaces<Z2i::KSpace>::sMakeBoundary( bdry, ks, aSet, ks.lowerBound(), ks.upperBound() );

  SurfelAdjacency<Z2i::KSpace::dimension>  sAdjInt( true );
  SurfelAdjacency<Z2i::KSpace::dimension>  sAdjExt( false );
  

  //Displaying the boundary bels
  std::set<SCell>::iterator itBoundary;
  int i=0;

  Board2D boardDisplayAll;
  boardDisplayAll<< d;
  boardAdj.saveFIG("illustrationAdjSRC.fig");
  for(itBoundary= bdry.begin(); itBoundary!= bdry.end(); itBoundary++){
    boardDisplayAll << *itBoundary;
  }
  boardDisplayAll.saveFIG("illustrationAdjBdr.fig");

  itBoundary = bdry.begin();
  for (int i =0; i<10; i++) itBoundary++;
  SCell surfel = *itBoundary;
  
  // Defining surfel Neighborhood given an surfel Adjacency
  SurfelNeighborhood<KSpace> sNeighInt;
  sNeighInt.init( &ks, &sAdjInt, surfel );
  
  SurfelNeighborhood<KSpace> sNeighExt;
  sNeighExt.init( &ks, &sAdjExt, surfel );
  
  
  SCell surfelFollowerInt;
  SCell surfelFollowerExt;

  sNeighInt.getAdjacentOnDigitalSet ( surfelFollowerInt, aSet, *(ks.sDirs(surfel)), true);
  sNeighExt.getAdjacentOnDigitalSet ( surfelFollowerExt, aSet, *(ks.sDirs(surfel)), true);
  
  
  
  boardAdj <<  CustomStyle( surfel.className() ,
			    new CustomColors( Color::Blue,
					      Color::Gray ));
  boardAdj << surfel;
  
  boardAdj <<  CustomStyle( surfel.className() ,
			    new CustomColors( Color::Red,
					      Color::Black ));

  boardAdj << surfelFollowerInt;
  boardAdj <<  CustomStyle( surfel.className() ,
			    new CustomColors( Color::Green,
					      Color::White ));
  boardAdj << surfelFollowerExt;

  
  boardAdj.saveFIG("illustrationAdjIntExt.fig");
  

  
  // Extraction of contour
  std::set<SCell> aContour1;
  Surfaces<Z2i::KSpace>::trackBoundary( aContour1, ks, 
					sAdjExt,
					aSet, surfel ); 
		    
  Board2D boardContour1;
  boardContour1 << d;
  std::set<SCell>::iterator iterOnContour;
  for( iterOnContour= aContour1.begin(); iterOnContour != aContour1.end(); iterOnContour++){
    if(*iterOnContour != surfel){
      boardContour1 <<  CustomStyle( surfel.className() ,
				     new CustomColors( Color::Green,
						       Color::White ));
      boardContour1<< *iterOnContour;
    }else{
				 
      boardContour1 <<  CustomStyle( surfel.className() ,
				     new CustomColors( Color::Blue,
						       Color::Gray ));
      boardContour1<< *iterOnContour;
    }
  }
  boardContour1.saveFIG("illustrationAdjContour.fig");



  // Extraction of contour
  std::set<SCell> aContour2;
  Surfaces<Z2i::KSpace>::trackBoundary( aContour2, ks, 
					sAdjInt,
					aSet, surfel ); 
  Board2D boardContour2;
  boardContour2 << d;
  std::set<SCell>::iterator iterOnContour2;
  for( iterOnContour2= aContour2.begin(); iterOnContour2 != aContour2.end(); iterOnContour2++){
    if(*iterOnContour2 != surfel){
      boardContour2 <<  CustomStyle( surfel.className() ,
				     new CustomColors( Color::Red,
						       Color::White ));
      boardContour2<< *iterOnContour2;
    }else{
      
      boardContour2 <<  CustomStyle( surfel.className() ,
				     new CustomColors( Color::Blue,
						       Color::Gray ));
      boardContour2<< *iterOnContour2;
    }
  }
  boardContour2.saveFIG("illustrationAdjContour2.fig");









  return 0;
}

