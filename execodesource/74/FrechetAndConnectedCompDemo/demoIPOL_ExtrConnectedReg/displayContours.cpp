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
 * @file displayContours.cpp
 * @authors David Coeurjolly, Bertrand Kerautret, Jacques-Olivier Lachaud
 *
 * @date 2014/28/02
 *
 * Display digital contours. 
 *
 * This file is part of the IPOL source demo (http://dx.doi.org/10.5201/ipol.2014.74).
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "DGtal/base/Common.h"

#include "DGtal/helpers/StdDefs.h"

#include "DGtal/shapes/ShapeFactory.h"
#include "DGtal/shapes/Shapes.h"
#include "DGtal/topology/helpers/Surfaces.h"

//image
#include "DGtal/images/imagesSetsUtils/ImageFromSet.h"
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/images/ImageSelector.h"
#include "DGtal/io/readers/PointListReader.h"
#include "DGtal/io/boards/Board2D.h"
#include "DGtal/io/Color.h"

#ifdef WITH_MAGICK
#include "DGtal/io/readers/MagickReader.h"
#endif

//contour
#include "DGtal/geometry/curves/FreemanChain.h"


//STL
#include <vector>
#include <string>

#include "ImaGene/Arguments.h"



using namespace DGtal;

static ImaGene::Arguments args;



int main( int argc, char** argv )
{
  
  // Contour import
  args.addOption("-fc", "-fc <freemanChain.fc> : FreemanChain file name", "freeman.fc" );
  args.addOption("-sdp", "-sdp <contour.sdp> : Import a contour as a Sequence of Discrete Points (SDP format)", "contour.sdp" );
  args.addOption("-sfp", "-sdp <contour.sdp> : Import a contour as a Sequence of Floating Points (SFP format)", "contour.sdp" );
  
  // Display options
  args.addOption("-drawContourPoint", "-drawContourPoint <size> (double): display contour points as disk of radius <size> (default 1.0) ", "1.0" );
  args.addBooleanOption("-fillContour", "-fillContour fill the contours with default color");
  args.addOption("-lineWidth", "-lineWidth <width> : define the linewidth <width> of the contour (default 1.0) (SDP format)", "1.0");
  
  args.addOption("-outputEPS", "-outputEPS <filename> specify eps format (default format output.eps)", "output.eps");
  args.addOption("-outputSVG", "-outputSVG <filename> specify svg format. (default name output.svg)", "output.svg");
  args.addOption("-outputFIG", "-outputFIG <filename> specify svg format. (default name output.fig)", "output.fig");
  
#ifdef WITH_CAIRO
  args.addOption("-outputPDF", "-outputPDF <filename> specify svg format. (default name output.pdf)", "output.pdf");
  args.addOption("-outputPNG", "-outputPNG <filename> specify png format. (default name output.png)", "output.png");
  args.addBooleanOption("-invertYaxis", "-invertYaxis: invert the Y axis for display contours (used only with --SDP) ");  
#endif
  
#ifdef WITH_MAGICK
  args.addOption("-backgroundImage", "-backgroundImage <filename> <alpha> : display image as background with transparency alpha (defaut 1) (transparency works only if cairo is available)", "imageBG.png", "1.0"  );
#endif

  args.addOption("-backgroundImageXFIG", "-backgroundImageXFIG <filename> <width> <height> : display image as background in XFIG format", "imageBG.png", "256","256"  );
  args.addOption("-scale", "-scale <value> 1: normal; >1 : larger ; <1 lower resolutions  ) (default 1.0) ", "1.0");
  
  bool parseOK=  args.readArguments( argc, argv );
  
  
  if(!parseOK || args.check("-h") || (! args.check("-fc") && ! args.check("-sdp") && ! args.check("-sfp"))){
    trace.info()<<args.usage("displayContours", "Display discrete contours. \n Basic usage: \n \t displayContours [options] -fc  <fileName>  \n", "");
    
      return 1;
  } 


  
  double lineWidth =  args.getOption("-lineWidth")->getFloatValue(0);
  double scale = args.getOption("-scale")->getIntValue(0);
  bool filled = args.check("-fillContour");
  Board2D aBoard;
  aBoard.setUnit (0.05*scale, LibBoard::Board::UCentimeter);
  
  
  
#ifdef WITH_MAGICK
  double alpha=args.getOption("-alphaBG")->getFloatValue(0);
  if(args.check("-backgroundImage")){
    std::string imageName = args.check("-backgroundImage")->getValue(0);
    typedef ImageSelector<Z2i::Domain, unsigned char>::Type Image;
    DGtal::MagickReader<Image> reader;
    Image img = reader.importImage( imageName );
    Z2i::Point ptInf = img.domain().lowerBound(); 
    Z2i::Point ptSup = img.domain().upperBound(); 
    unsigned int width = abs(ptSup.at(0)-ptInf.at(0)+1);
    unsigned int height = abs(ptSup.at(1)-ptInf.at(1)+1);
    aBoard.drawImage(imageName, 0-0.5,height-0.5, width, height, -1, alpha );
  }
#endif
 
  if(args.check("-backgroundImageXFIG")){
    std::string imageName = args.getOption("-backgroundImageXFIG")->getValue(0);
    unsigned int width = args.getOption("-backgroundImageXFIG")->getIntValue(1);
    unsigned int height = args.getOption("-backgroundImageXFIG")->getIntValue(2);
    aBoard.drawImage(imageName, 0,height-1, width, height, -1, 1.0 );
    }
 
  if(args.check("-fc")){
    std::string fileName = args.getOption("-fc")->getValue(0);
    std::vector< FreemanChain<int> > vectFc =  PointListReader< Z2i::Point>:: getFreemanChainsFromFile<int> (fileName); 
    //aBoard <<  SetMode( vectFc.at(0).className(), "InterGrid" );
    aBoard << CustomStyle( vectFc.at(0).className(), 
			   new CustomColors( Color::Red  ,  (filled ? (Color::Black) : (Color::None))  ) );    
    for(unsigned int i=0; i<vectFc.size(); i++){
      aBoard <<  vectFc.at(i) ;
    }
  } 
 

  if( args.check("-sdp") || args.check("-sfp")){
    bool drawPoints= args.check("-drawContourPoint");
    bool invertYaxis = args.check("-invertYaxis");
    
    double pointSize = args.getOption("-drawContourPoint")->getFloatValue(0);
    
    std::vector<LibBoard::Point> contourPt;
    if(args.check("-sdp")){
      std::string fileName = args.getOption("-sdp")->getValue(0);
      std::vector< Z2i::Point >  contour = 
	PointListReader< Z2i::Point >::getPointsFromFile(fileName); 
      for(unsigned int j=0; j<contour.size(); j++){
	LibBoard::Point pt((double)(contour.at(j)[0]),
			   (invertYaxis? (double)(-contour.at(j)[1]+contour.at(0)[1]):(double)(contour.at(j)[1])));
	contourPt.push_back(pt);
	if(drawPoints){
	  aBoard.fillCircle(pt.x, pt.y, pointSize);
	}
      }
    }
    
    if(args.check("-sfp")){
      std::string fileName = args.getOption("-sfp")->getValue(0);
      std::vector< PointVector<2,double>  >  contour = 
	PointListReader<  PointVector<2,double>  >::getPointsFromFile(fileName); 
      for(unsigned int j=0; j<contour.size(); j++){
	LibBoard::Point pt((double)(contour.at(j)[0]),
			   (invertYaxis? (double)(-contour.at(j)[1]+contour.at(0)[1]):(double)(contour.at(j)[1])));
	contourPt.push_back(pt);
	if(drawPoints){
	  aBoard.fillCircle(pt.x, pt.y, pointSize);
	}
      }
    }
    aBoard.setPenColor(Color::Red);
    aBoard.setLineStyle (LibBoard::Shape::SolidStyle );
    aBoard.setLineWidth (lineWidth);
    if(!filled){
      aBoard.drawPolyline(contourPt);
    }else{
      aBoard.fillPolyline(contourPt);
    }
    
  
  }

 
  
  if (args.check("-outputSVG")){
    std::string outputFileName= args.getOption("-outputSVG")->getValue(0);
    aBoard.saveSVG(outputFileName.c_str());
  } else   
    if (args.check("-outputFIG")){
      std::string outputFileName= args.getOption("-outputFIG")->getValue(0);
      aBoard.saveFIG(outputFileName.c_str());
    } else
      if (args.check("-outputEPS")){
	std::string outputFileName= args.getOption("-outputEPS")->getValue(0);
	aBoard.saveEPS(outputFileName.c_str());
      }  
#ifdef WITH_CAIRO
      else
	if (args.check("-outputEPS")){
	  std::string outputFileName= args.getOption("-outputSVG")->getValue(0);
	  aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoEPS );
	} else 
	  if (args.check("-outputPDF")){
	    std::string outputFileName= args.getOption("-outputPDF")->getValue(0);
	    aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoPDF );
	  } else 
	    if (args.check("-outputPNG")){
	      std::string outputFileName= args.getOption("-outputPNG")getValue(0);
	      aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoPNG );
	    }
#endif
	    else { //default output
	      std::string outputFileName= "output.eps";
	      aBoard.saveEPS(outputFileName.c_str());
	    }
  
}
