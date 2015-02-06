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
 * @file testFrechetShortcut.cpp
 * @ingroup Tests
 * @author Isabelle Sivignon (\c isabelle.sivignon@gipsa-lab.grenoble-inp.fr )
 * gipsa-lab Grenoble Images Parole Signal Automatique (CNRS, UMR 5216), CNRS, France
 *
 * @date 2012/03/26
 *
 * Functions for testing class FrechetShortcut.
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iterator>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>

#include "DGtal/base/Common.h"
#include "DGtal/base/Exceptions.h"
#include "DGtal/kernel/SpaceND.h"

#include "DGtal/kernel/domains/HyperRectDomain.h"
#include "DGtal/geometry/curves/FrechetShortcut.h"
#include "DGtal/io/boards/Board2D.h"

#include "DGtal/io/boards/CDrawableWithBoard2D.h"
#include "DGtal/geometry/curves/CForwardSegmentComputer.h"
#include "DGtal/helpers/StdDefs.h"

#include "DGtal/geometry/curves/GreedySegmentation.h"

#include "ImaGene/Arguments.h"

///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace DGtal;
using namespace LibBoard;
using namespace Z2i;


static ImaGene::Arguments args;


typedef Curve::PointsRange::ConstIterator Iterator;
typedef FrechetShortcut<Iterator,int> SegmentComputer;
typedef GreedySegmentation<SegmentComputer> Segmentation;



void displayContour(const std::vector<Z2i::Point> &contour, Board2D &aBoard){
  aBoard.setPenColor(Color::Blue);
  aBoard.setFillColor(Color::White);
  aBoard.setLineStyle (LibBoard::Shape::SolidStyle );
  aBoard.setLineWidth (3);  
  vector<LibBoard::Point> contourPt;
  for(unsigned int j=0; j<contour.size(); j++){
    LibBoard::Point pt((double)(contour.at(j)[0]),
		       (double)(contour.at(j)[1]));
    contourPt.push_back(pt);
  } 
  aBoard.drawPolyline(contourPt);
}





void processContour(const std::vector<Z2i::Point> &contour, Board2D & aBoard, double error,ofstream &f,
		    bool flagWidthOnly, bool displayPolygonInline=true){ 
  Curve aCurve; //grid curve
  aCurve.initFromVector(contour);
  typedef Curve::PointsRange Range; //range
  Range r = aCurve.getPointsRange(); //range
  clock_t time1, time2;
  time1 = clock();
  Segmentation theSegmentation( r.begin(), r.end(), SegmentComputer(error,flagWidthOnly) );
  time2 = clock();
  double cpuTime;
  cpuTime =  ((double)time2-(double)time1)/((double)CLOCKS_PER_SEC/1000);
  
  Segmentation::SegmentComputerIterator it = theSegmentation.begin();
  Segmentation::SegmentComputerIterator itEnd = theSegmentation.end();
  
  int simplificationSize=0;

  aBoard.setPenColor(Color::Red);
  aBoard.setLineStyle (LibBoard::Shape::SolidStyle );
  
  std::vector<SegmentComputer> vectSeg;
  for ( ; it != itEnd; ++it) {
    SegmentComputer s(*it);
    vectSeg.push_back(*it);
    //output vertices of the simplification 
    if(displayPolygonInline){
      f << (*(s.begin()))[0] << " " << (*(s.begin()))[1] <<  " " ;
    }else{
      f << (*(s.begin()))[0] << " " << (*(s.begin()))[1] <<  endl;
    }
    
  // size of the simpification
    simplificationSize++;
  }
  f << endl;

  std::cout << aCurve.size()<<" " << error<<" " << simplificationSize<<" "<< cpuTime << std::endl;
  
  displayContour(contour, aBoard);
  
  aBoard << r;
  
  for( int i=0; i < vectSeg.size(); i++){
    aBoard << CustomStyle( vectSeg.at(i).className(),  new CustomPen( Color::Red, Color::Red, 3.0, 
								      Board2D::Shape::SolidStyle,
								      Board2D::Shape::RoundCap,
								      Board2D::Shape::RoundJoin ) );
    aBoard<< vectSeg.at(i);
  }
}





///////////////////////////////////////////////////////////////////////////////
// Standard services - public :

int main( int argc, char** argv )
{
  args.addOption( "-error", "-error <val>:parameter used in the simplification algorithm (Frechet or width) (default is 2)", "2" );
  args.addOption("-sdp", "-sdp <contour.sdp> : Import a contour as a Sequence of Discrete Points (SDP format)", "contour.sdp" );
  args.addOption( "-imageSize", "-imageSize <width> <height>: used to improve the output display to correspond to an source image by displaying an empty box of width 0 (to force the correspondance of the BB)", "", "" );
  args.addBooleanOption("-w", "-w: compute the simplification using the width only");
  args.addBooleanOption("-allContours", "-allContours: compute the simplification of all the contours (one contour per line given in sdp file)");
  
  bool parseOK=  args.readArguments( argc, argv );
  
  if ( ( argc <= 1 ) ||  ! parseOK ) 
    {
      cerr << args.usage( "frechetSimplification: ", 
			  "Description: apply a simplification algorithm using the Frechet distance, or the width distance (default setting: Frechet is used) : \n frechetSimplification -error .... ",
			  "" )
	   << endl;
      return 1;
    }  
  
  Board2D board;   
  double error = args.getOption("-error")->getFloatValue(0);
  ofstream f;
  f.open("output.txt", std::ofstream::out);
  
  bool flagWidthOnly = false;
  if(args.check("-w"))
    flagWidthOnly = true;


  if( args.check("-sdp") && !args.check("-allContours")){
    std::vector<Z2i::Point> contour;
    string fileName = args.getOption("-sdp")->getValue(0);
    contour =   PointListReader< Z2i::Point >::getPointsFromFile(fileName); 
    std::cout << "# curve_size error simplification_size cpu_time  "<< std::endl;
    processContour(contour, board, error, f, flagWidthOnly, false);     
    board.saveEPS("output.eps", 800, 800 ); 
  }


  if( args.check("-sdp") && args.check("-allContours")  ){
    string fileName = args.getOption("-sdp")->getValue(0);
    std::vector< std::vector<Z2i::Point> > vectContours =   PointListReader< Z2i::Point >::getPolygonsFromFile(fileName);
    std::cout << "# curve_size error simplification_size cpu_time  " << std::endl;
    for (int j=0; j<vectContours.size(); j++){
      trace.info() << "# Processing contour " << j << endl;
      processContour(vectContours.at(j), board, error,  f, flagWidthOnly, true); 
    }    

      if(args.check("-imageSize")){
    unsigned int width = args.getOption("-imageSize")->getIntValue(0);
    unsigned int height = args.getOption("-imageSize")->getIntValue(1);
    board.setLineWidth(0.0);
    board.setFillColor( DGtal::Color::None);
    board.drawRectangle(0,height, width, height);
  }

    board.saveEPS("output.eps", 800, 800); 
  }


  
  
  return 0;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
