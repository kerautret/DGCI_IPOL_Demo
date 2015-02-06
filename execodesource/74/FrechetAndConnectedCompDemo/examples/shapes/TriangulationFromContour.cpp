/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * @file TriangulationFromContour.cpp
 * @ingroup Examples
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2013/01/14
 *
 * Example of TriangulationFromContour from discrete contour extracted from pgm image
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"

#include "DGtal/shapes/fromPoints/TriangularMeshFrom2DPoints.h"
#include "DGtal/io/boards/Board2D.h"
#include "DGtal/geometry/helpers/ContourHelper.h"
#include "DGtal/topology/helpers/Surfaces.h"

#include "DGtal/io/readers/PNMReader.h"
#include "ConfigExamples.h"


///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace DGtal;
using namespace DGtal::Z2i;


int main( int argc, char** argv )
{


  std::string inputFilename = examplesPath + "samples/klokan.pgm"; 

  typedef ImageContainerBySTLVector<Z2i::Domain, unsigned char> Image;
  typedef IntervalThresholder<Image::Value> Binarizer; 
  
  Image image = PNMReader<Image>::importPGM( inputFilename ); 
  
   Binarizer b(0, 130); 
   PointFunctorPredicate<Image,Binarizer> predicate(image, b); 
   SurfelAdjacency<2> sAdj( true );
   std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels;
   Z2i::KSpace ks;
   if(! ks.init( image.domain().lowerBound(), 
		 image.domain().upperBound(), true )){
     trace.error() << "Problem in KSpace initialisation"<< endl;
   }
   Surfaces<Z2i::KSpace>::extractAllPointContours4C( vectContoursBdryPointels,
						     ks, predicate, sAdj );      
   
   
   
  
  //! [TriangularMeshFrom2DPointsINIT] 
  
  // The flag of the last argument is set to true to save the map
  // associating a Point to a Triangle (needed for the voronoi diagram)
  
  TriangularMeshFrom2DPoints<Point> aMesh (image.domain().lowerBound(), image.domain().upperBound(), true);  
  
  for(unsigned int i=0; i< vectContoursBdryPointels.size(); i++){
    std::vector<Point> vect = vectContoursBdryPointels.at(i);
    unsigned int k=0;
    for(unsigned int j=0; j< vect.size(); j++){
      if(k%10==0)
	aMesh.addPointInsideDelaunayMesh(vect.at(j));

      k++;
    }
  }

 
  
  aMesh.removeTrianglesOfBoundingVertex();
  //! [TriangularMeshFrom2DPointsDISPLAYRES] 
  Board2D aBoard;
  aBoard << aMesh;    
  aBoard <<image.domain().lowerBound()<< image.domain().upperBound() ;
  aBoard.saveEPS("exampleDelaunayTriangulation.eps");
  //! [TriangularMeshFrom2DPointsDISPLAYRES] 

  Board2D aBoard2;
  aBoard2.setPenColor(DGtal::Color(20,20,20));
  std::vector< std::vector<Point> > vectPolygons = aMesh.getVoronoiDiagram();
  for(unsigned int i=0;i<vectPolygons.size();  i++){
    std::vector< Point> aPolygon = vectPolygons.at(i);
    std::vector< LibBoard::Point > tmpCnt;
    for(unsigned int j=0; j< aPolygon.size(); j++){
      tmpCnt.push_back( LibBoard::Point(aPolygon.at(j)[0], aPolygon.at(j)[1]));
    }
    aBoard2.drawPolyline(tmpCnt);
  }

 
  aBoard2.saveEPS("exampleVoronoi.eps");

  
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

