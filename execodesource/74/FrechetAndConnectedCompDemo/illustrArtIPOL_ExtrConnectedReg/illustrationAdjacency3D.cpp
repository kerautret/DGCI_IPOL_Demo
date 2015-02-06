#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <QtGui/qapplication.h>
#include "DGtal/io/DrawWithDisplay3DModifier.h"

#include <DGtal/topology/helpers/Surfaces.h>
#include <DGtal/geometry/curves/FreemanChain.h>
#include <DGtal/io/colormaps/GradientColorMap.h>
#include <DGtal/topology/helpers/Surfaces.h>
#include "DGtal/io/viewers/Viewer3D.h"



using namespace DGtal;
using namespace Z3i;

  

int main(int argc, char** argv)
{


  QApplication application(argc,argv);


  Point ptL(-1, -1, -1);
  Point ptU(3, 3, 1);
  Domain d (ptL, ptU);    
    
  Point pt0(0,0, 0);
  Point pt1(1,0,0 );
  Point pt2(0,1,0 );
  Point pt3(2,1,0);
  Point pt4(1,2,0);
  Point pt5(2,2,0);
  

  DigitalSet aSet( d );
  aSet.insert(pt0);
  aSet.insert(pt1);
  aSet.insert(pt2);
  aSet.insert(pt3);
  aSet.insert(pt4);



  std::set<SCell> bdry;
  Z3i::KSpace ks;  
  ks.init( ptL, ptU, true );
 

  SCell v0 = ks.sSpel(pt0, KSpace::POS);
  SCell v1 = ks.sSpel(pt1, KSpace::POS);
  SCell v2 = ks.sSpel(pt2, KSpace::POS);
  SCell v3 = ks.sSpel(pt3, KSpace::POS);
  SCell v4 = ks.sSpel(pt4, KSpace::POS);



  Viewer3D viewerAdjSRC;
  viewerAdjSRC.show();
  viewerAdjSRC << d;
  viewerAdjSRC << SetMode3D( v0.className(), "Illustration" );
  viewerAdjSRC << pt0 << pt1 << pt2 << pt3 << pt4;



  Viewer3D viewerAdj;
  viewerAdj.show();

  viewerAdj << SetMode3D( v0.className(), "Illustration" );
  viewerAdj << v0 << v1 << v2 << v3 << v4;

  Viewer3D viewerAdj2;
  viewerAdj2.show();



  
  // Extracting the surface boundary of the shape
  Surfaces<Z3i::KSpace>::sMakeBoundary( bdry, ks, aSet, ks.lowerBound(), ks.upperBound() );

  SurfelAdjacency<Z3i::KSpace::dimension>  sAdjInt( true );
  SurfelAdjacency<Z3i::KSpace::dimension>  sAdjExt( false );
  
  viewerAdj << SetMode3D( v0.className(), "Illustration" );
  viewerAdj << v0 << v1 << v2 << v3 << v4;

  //Displaying the boundary bels
  std::set<SCell>::iterator itBoundary;
  int i=0;


  for(itBoundary= bdry.begin(); itBoundary!= bdry.end(); itBoundary++){
    viewerAdj << *itBoundary;
  }
  
  viewerAdj2 << SetMode3D( v0.className(), "Illustration" );
  viewerAdj2 << v0 << v1 << v2 << v3 << v4;

  itBoundary = bdry.begin();
  for (int i =0; i<17; i++) itBoundary++;
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

  viewerAdj2 <<  CustomColors3D(Color(220, 220,255),Color(220, 220,255));
  viewerAdj2 << surfel;
  
  viewerAdj2 <<  CustomColors3D(Color(250, 0,0),Color(250, 0,0));

  viewerAdj2 << surfelFollowerInt;
  viewerAdj2 <<  CustomColors3D(Color(0, 250,0),Color(0, 250,0));
  viewerAdj2 << surfelFollowerExt;
  


  // Extraction of contour
  std::set<SCell> aContour1;
  Surfaces<Z3i::KSpace>::trackBoundary( aContour1, ks, 
  					sAdjExt,
  					aSet, surfel ); 


  Viewer3D viewerContour1;
  viewerContour1.show();

  viewerContour1 << SetMode3D( v0.className(), "Illustration" );
  viewerContour1 << v0 << v1 << v2 << v3 << v4;
	    

  std::set<SCell>::iterator iterOnContour;
  for( iterOnContour= aContour1.begin(); iterOnContour != aContour1.end(); iterOnContour++){
    if(*iterOnContour != surfel){
      viewerContour1 <<  CustomColors3D(Color(0, 250,0),Color(0, 250,0));
      viewerContour1<< *iterOnContour;
    }else{
				 
      viewerContour1 <<  CustomColors3D(Color(220, 220,255),Color(220, 220,255));
      viewerContour1<< *iterOnContour;
    }
  }
  



  // Extraction of contour
  std::set<SCell> aContour2;
  Surfaces<Z3i::KSpace>::trackBoundary( aContour2, ks, 
  					sAdjInt,
  					aSet, surfel ); 

  Viewer3D viewerContour2;


  viewerContour2.show();
  viewerContour2 << SetMode3D( v0.className(), "Illustration" );
  viewerContour2 << v0 << v1 << v2 << v3 << v4;


  std::set<SCell>::iterator iterOnContour2;
  for( iterOnContour2= aContour2.begin(); iterOnContour2 != aContour2.end(); iterOnContour2++){
    if(*iterOnContour2 != surfel){
      viewerContour2 <<  CustomColors3D(Color(250, 0,0),Color(250, 0,0));
      viewerContour2<< *iterOnContour2;
    }else{
      
      viewerContour2 <<  CustomColors3D(Color(220, 220,255),Color(220, 220,255));
      viewerContour2<< *iterOnContour2;
    }
  }
  
  viewerAdjSRC << Viewer3D:: updateDisplay;  
  viewerAdj<< Viewer3D::updateDisplay;
  viewerAdj2<< Viewer3D::updateDisplay;
  viewerContour1 << Viewer3D::updateDisplay;
  viewerContour2 << Viewer3D::updateDisplay;

 
  return application.exec();


  return 0;
}

