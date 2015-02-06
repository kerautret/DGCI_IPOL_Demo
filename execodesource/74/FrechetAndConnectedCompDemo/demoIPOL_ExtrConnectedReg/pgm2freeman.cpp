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
 * @file pgm2freeman.cpp
 * @authors David Coeurjolly, Bertrand Kerautret, Jacques-Olivier Lachaud
 *
 * @date 2014/28/02
 *
 * Convert grey scales image to fremann contour. 
 *
 * This file is part of the IPOL source demo (http://dx.doi.org/10.5201/ipol.2014.74).
 */


#include "DGtal/io/colormaps/GrayscaleColorMap.h"
#include "DGtal/io/readers/PNMReader.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/images/ImageSelector.h"

#include "DGtal/geometry/curves/FreemanChain.h"
#include "DGtal/geometry/helpers/ContourHelper.h"
#include "DGtal/topology/helpers/Surfaces.h"

#include <vector>
#include <string>
#include <climits>

#include "ImaGene/Arguments.h"

using namespace DGtal;

static ImaGene::Arguments args;
static const std::string PROG_VERSION=" 1.2 (17-04-2014)";

typedef ImageSelector < Z2i::Domain, unsigned char>::Type Image;



std::vector<unsigned int> getHistoFromImage(const Image &image){
  const Image::Domain &imgDom = image.domain();
  std::vector<unsigned int> vectHisto(UCHAR_MAX+1);
  for(Image::Domain::ConstIterator it=imgDom.begin(); it!= imgDom.end(); ++it){
    vectHisto[image(*it)]++;
  }
  return vectHisto;
}




unsigned int 
getOtsuThreshold(const Image &image){
  std::vector<unsigned int> histo = getHistoFromImage(image);
  unsigned int imageSize = image.domain().size();
  unsigned int sumA = 0;
  unsigned int sumB = imageSize;
  unsigned int muA=0;
  unsigned int muB=0;
  unsigned int sumMuAll= 0;
  for( unsigned int t=0; t< histo.size();t++){
    sumMuAll+=histo[t]*t;
  }
  
  unsigned int thresholdRes=0;
  double valMax=0.0;
  for( unsigned int t=0; t< histo.size(); t++){
    sumA+=histo[t];
    if(sumA==0)
      continue; 
    sumB=imageSize-sumA;
    if(sumB==0){
      break;
    }
    
    muA+=histo[t]*t;
    muB=sumMuAll-muA;
    double muAr=muA/(double)sumA;
    double muBr=muB/(double)sumB;
    double sigma=  (double)sumA*(double)sumB*(muAr-muBr)*(muAr-muBr);
    if(valMax<=sigma){
      valMax=sigma;
      thresholdRes=t;
    }
  }
  return thresholdRes;
}




struct compContours {
  bool operator() ( std::vector< Z2i::Point > cntA, std::vector< Z2i::Point > cntB ) { return (cntA.size()>cntB.size());}
} myCompContour;


void saveAllContoursAsFc(std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels, unsigned int minSize){
  for(unsigned int k=0; k<vectContoursBdryPointels.size(); k++){
    if(vectContoursBdryPointels.at(k).size()>minSize){
      FreemanChain<Z2i::Integer> fc (vectContoursBdryPointels.at(k));    
      std::cout << fc.x0 << " " << fc.y0   << " " << fc.chain << std::endl; 
	  
    }
  }
}


void saveLargestContourAsSDP(std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels, unsigned int minSize){
  std::vector< std::vector< Z2i::Point >  >  vectContoursToSort;
  for(unsigned int k=0; k<vectContoursBdryPointels.size(); k++){
    if(vectContoursBdryPointels.at(k).size()>minSize){
      vectContoursToSort.push_back(vectContoursBdryPointels.at(k));
    }
  }
  std::sort (vectContoursToSort.begin(), vectContoursToSort.end(), myCompContour);
  for(unsigned int i=0; i<vectContoursToSort.at(0).size(); i++){
    std::cout << vectContoursToSort.at(0).at(i)[0] << " " <<  vectContoursToSort.at(0).at(i)[1] << std::endl; 
  }

}


void saveAllContourAsSDP(std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels, unsigned int minSize){
  std::vector< std::vector< Z2i::Point >  >  vectContoursToSort;
  for(unsigned int k=0; k<vectContoursBdryPointels.size(); k++){
    if(vectContoursBdryPointels.at(k).size()>minSize){
      vectContoursToSort.push_back(vectContoursBdryPointels.at(k));
    }
  }
  std::sort (vectContoursToSort.begin(), vectContoursToSort.end(), myCompContour);  
  for( int j=0; j < vectContoursToSort.size(); j++){
    for(unsigned int i=0; i<vectContoursToSort.at(j).size(); i++){
      std::cout << vectContoursToSort.at(j).at(i)[0] << " " <<  vectContoursToSort.at(j).at(i)[1] << " "; 
    }
    std::cout << std::endl;
  }
}


void saveSelContoursAsFC(std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels, 
			 unsigned int minSize, Z2i::Point refPoint, double selectDistanceMax){
  for(unsigned int k=0; k<vectContoursBdryPointels.size(); k++){
    if(vectContoursBdryPointels.at(k).size()>minSize){
      Z2i::Point ptMean = ContourHelper::getMeanPoint(vectContoursBdryPointels.at(k));
      unsigned int distance = (unsigned int)ceil(sqrt((double)(ptMean[0]-refPoint[0])*(ptMean[0]-refPoint[0])+
						      (ptMean[1]-refPoint[1])*(ptMean[1]-refPoint[1])));
      if(distance<=selectDistanceMax){
	FreemanChain<Z2i::Integer> fc (vectContoursBdryPointels.at(k));    
	std::cout << fc.x0 << " " << fc.y0   << " " << fc.chain << std::endl; 
      }      
    }    
  }
}

void saveLargestContourSelContoursAsSDP(std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels, 
			 unsigned int minSize, Z2i::Point refPoint, double selectDistanceMax){
  std::vector< std::vector< Z2i::Point >  >  vectContoursToSort;
  
  for(unsigned int k=0; k<vectContoursBdryPointels.size(); k++){
    if(vectContoursBdryPointels.at(k).size()>minSize){
      Z2i::Point ptMean = ContourHelper::getMeanPoint(vectContoursBdryPointels.at(k));
      unsigned int distance = (unsigned int)ceil(sqrt((double)(ptMean[0]-refPoint[0])*(ptMean[0]-refPoint[0])+
						      (ptMean[1]-refPoint[1])*(ptMean[1]-refPoint[1])));
      if(distance<=selectDistanceMax){
	vectContoursToSort.push_back(vectContoursBdryPointels.at(k));
      }
      
    }    
  }
  std::sort (vectContoursToSort.begin(), vectContoursToSort.end(), myCompContour);
  for(unsigned int i=0; i<vectContoursToSort.at(0).size(); i++){
    std::cout << vectContoursToSort.at(0).at(i)[0] << " " <<  vectContoursToSort.at(0).at(i)[1] << std::endl; 
  }
 
}




int main( int argc, char** argv )
{

  args.addIOArgs(  false, false );  
  
  args.addBooleanOption( "-h", "-h: display this message." );
  args.addOption("-image",  "-image: set the input image filename ", "image.pgm"); 
  args.addOption( "-badj", "-badj <0/1>: 0 is interior bel adjacency, 1 is exterior (def. is 0).", "0" );
  args.addOption( "-minThreshold", "-minThreshold <val>: minimal threshold value for binarizing PGM gray values (def. is 128).", "0" );
  args.addOption( "-maxThreshold", "-maxThreshold <val>: maximal threshold value for binarizing PGM gray values (def. is 255).", "128" );
  args.addOption("-thresholdRange", "-thresholdRange <min> <incr> <max> use a range interval as threshold: for each possible i, it define a digital sets [min, min+((i+1)*increment)] such that min+((i+1)*increment)< max  and extract their boundary.","0", "10", "255");
  args.addOption( "-min_size", "-min_size <m>: minimum digital length of contours for output (def. is 4).", "4" );
  
  args.addOption("-selectContour", "-selectContour <x0> <y0> <distanceMax>: select the contours for which the first point is near (x0, y0) with a distance less than <distanceMax>","0", "0", "0" );
  args.addBooleanOption("-invertVerticalAxis", "-invertVerticalAxis used to transform the contour representation (need for DGtal), used o nly for the contour displayed, not for the contour selection (-selectContour). ");
  args.addBooleanOption("-outputSDP", "-outputSDP export as a sequence of discrete points instead of freemanchain (use the largest contour if more contours appears)");
  args.addBooleanOption("-outputSDPAll", "-outputSDPAll export as a sequence of discrete points instead of freemanchain (all contours are exported: one per line)");
  args.addBooleanOption("-version", "-version : display version");    

 
  if ( ( argc <= 1 ) ||  ! args.readArguments( argc, argv ) ) 
    {
      std::cerr << args.usage( "pgm2freeman: ", 
			       "Extracts all 2D contours from a PGM image given on the standard input and writes them on the standard output as FreemanChain's. \nTypical use: \n pgm2freeman -threshold 200 -image image.pgm > imageContour.fc \n  Note that if you don't specify any threshold a threshold t is automatically defined from the Otsu algorithm. ",
                               "" )
                << std::endl;
      return 1;
    }  

 if(args.check("-version")){
    std::cout << "version: " << PROG_VERSION << std::endl;
    return 0;
  }

  
 

  
 
 
  int minThreshold = args.getOption("-minThreshold")->getIntValue(0);
  int maxThreshold = args.getOption("-maxThreshold")->getIntValue(0);
  unsigned int minSize = args.getOption("-min_size")->getIntValue(0);

 
  bool select=false;
  bool thresholdRange= args.check("-thresholdRange");
  bool exportSDP=args.check("-outputSDP");
  bool exportSDPALL= args.check("-outputSDPAll");
  
  int min, max, increment;
  if(thresholdRange){
    minThreshold = args.getOption("-thresholdRange")->getIntValue(0);
    increment = args.getOption("-thresholdRange")->getIntValue(1);
    maxThreshold = args.getOption("-thresholdRange")->getIntValue(2);
  }
 
  Z2i::Point selectCenter;
  unsigned int selectDistanceMax = 0; 
 

  if(args.check("-selectContour")){
    select=true;   
    selectCenter[0] = args.getOption("-selectContour")->getIntValue(0);
    selectCenter[1] = args.getOption("-selectContour")->getIntValue(1);
    selectDistanceMax = args.getOption("-selectContour")->getIntValue(2);
  }
 
  typedef ImageSelector < Z2i::Domain, unsigned char>::Type Image;
  typedef IntervalThresholder<Image::Value> Binarizer; 
  std::string imageFileName = args.getOption("-image")->getValue(0);
  Image image = PNMReader<Image>::importPGM( imageFileName ); 
  
  Z2i::KSpace ks;
  if(! ks.init( image.domain().lowerBound(), 
		image.domain().upperBound(), true )){
    trace.error() << "Problem in KSpace initialisation"<< std::endl;
  }
  
  bool badj = (args.getOption("-badj")->getIntValue(0))!=1;
  
  if (!thresholdRange){
    if (!args.check("-maxThreshold")&& !args.check("-minThreshold")){
      minThreshold=0;
      trace.info() << "Min/Max threshold values not specified, set min to 0 and computing max with the otsu algorithm...";
      maxThreshold = getOtsuThreshold(image);
      trace.info() << "[done] (max= " << maxThreshold << ") "<< std::endl;
    }


    Binarizer b(minThreshold, maxThreshold); 
    PointFunctorPredicate<Image,Binarizer> predicate(image, b); 
    trace.info() << "DGtal contour extraction from thresholds ["<<  minThreshold << "," << maxThreshold << "]" ;
    
    SurfelAdjacency<2> sAdj( badj );
    std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels;
    Surfaces<Z2i::KSpace>::extractAllPointContours4C( vectContoursBdryPointels,
  						      ks, predicate, sAdj );  
    if(select){
      if(!exportSDP){
	saveSelContoursAsFC(vectContoursBdryPointels,  minSize, selectCenter,  selectDistanceMax);
      }else{
	saveLargestContourSelContoursAsSDP(vectContoursBdryPointels,  minSize, selectCenter,  selectDistanceMax);
      }
    }else{
      if(!exportSDP && ! exportSDPALL){
	saveAllContoursAsFc(vectContoursBdryPointels,  minSize); 
      }else{
	if(exportSDPALL){
	  saveAllContourAsSDP(vectContoursBdryPointels,  minSize) ;
	}else{
	  saveLargestContourAsSDP(vectContoursBdryPointels,  minSize) ;
	}
      }
    }
    trace.info()<< " [done] " << std::endl;
  }else{
    for(int i=0; minThreshold+(i+1)*increment< maxThreshold; i++){
      min = minThreshold;
      max = minThreshold+(i+1)*increment;
      
      Binarizer b(min, max); 
      PointFunctorPredicate<Image,Binarizer> predicate(image, b); 
      
      trace.info() << "DGtal contour extraction from thresholds ["<<  min << "," << max << "]" ;
      SurfelAdjacency<2> sAdj( badj );
      std::vector< std::vector< Z2i::Point >  >  vectContoursBdryPointels;
      Surfaces<Z2i::KSpace>::extractAllPointContours4C( vectContoursBdryPointels,
  							ks, predicate, sAdj );  
      if(select){
  	if(!exportSDP){
	  saveSelContoursAsFC(vectContoursBdryPointels,  minSize, selectCenter,  selectDistanceMax);
	}else{
	  saveLargestContourSelContoursAsSDP(vectContoursBdryPointels,  minSize, selectCenter,  selectDistanceMax);
	}
      }else{
	if(!exportSDP){
	  saveAllContoursAsFc(vectContoursBdryPointels,  minSize); 
	}else{
	  saveLargestContourAsSDP(vectContoursBdryPointels,  minSize); 
	}
      }
      trace.info() << " [done]" << std::endl;
    }
  }
  return 0;
}

