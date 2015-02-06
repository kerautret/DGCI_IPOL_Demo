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
 * @file extract3D.cpp
 * @authors David Coeurjolly, Bertrand Kerautret, Jacques-Olivier Lachaud
 *
 * @date 2014/28/02
 *
 * Extract 3d connected component from 3D volumic file. 
 *
 * This file is part of the IPOL source demo (http://dx.doi.org/10.5201/ipol.2014.74).
 */

#include <iostream>
#include "DGtal/base/Common.h"
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"

#include "DGtal/images/ImageSelector.h"
#include "DGtal/helpers/StdDefs.h"

#include "DGtal/io/Display3D.h"
#include "DGtal/io/readers/VolReader.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/Color.h"

#include "DGtal/topology/KhalimskySpaceND.h"
#include "DGtal/topology/helpers/Surfaces.h"

#include "ImaGene/Arguments.h"


using namespace std;
using namespace DGtal;
using namespace Z3i;


static ImaGene::Arguments args;


int main( int argc, char** argv )
{
  args.addOption("-image", "-image <filename>  ", "aFile.vol ");
  args.addOption("-output", "-output <filename> the output filename with .off extension", "output.off"); 
  args.addOption("-exportSRC", "-exportSRC <filename> export the source set of voxels", "src.off"); 
  args.addOption("-threshold", "-threshold <min> <max> (default: min = 128, max 255  ", "128", "255");
  args.addOption( "-badj", "-badj <0/1>: 0 is interior bel adjacency, 1 is exterior (def. is 0).", "0" );

  if ( ( argc <= 1 ) ||  ! args.readArguments( argc, argv ) ) 
    {
      cerr << args.usage( "extract3D: ", 
			  "Extracts all 3D connected components from a .vol 3D image and generate a resulting 3D mesh on .OFF format. \nTypical use: \n extract3D -threshold 200 -image image.pgm > imageContour.fc ",
			  "" )
	   << endl;
      return 1;
    }  
  
  string imageFileName = args.getOption("-image")->getValue(0);  
  string outputFileName = args.getOption("-output")->getValue(0);  
  string srcFileName = args.getOption("-exportSRC")->getValue(0);  
  
  int minThreshold = args.getOption("-threshold")->getIntValue(0);
  int maxThreshold = args.getOption("-threshold")->getIntValue(1);
  bool badj = (args.getOption("-badj")->getIntValue(0))!=1;
  
  
  typedef ImageSelector < Domain, int>::Type Image;
  typedef IntervalThresholder<Image::Value> Binarizer; 
  Image image =   VolReader<Image>::importVol(imageFileName);

  Binarizer b(minThreshold, maxThreshold); 
  PointFunctorPredicate<Image,Binarizer> predicate(image, b); 
 
 
  //A KhalimskySpace is constructed from the domain boundary points.
  Point pUpper = image.domain().upperBound();
  Point pLower = image.domain().lowerBound();
 
  KSpace K;
  K.init(pLower, pUpper, true);
 
  SurfelAdjacency<3> sAdj(  badj );
  vector<vector<SCell> > vectConnectedSCell;
 
 
  Surfaces<KSpace>::extractAllConnectedSCell(vectConnectedSCell,K, sAdj, predicate, false);

  Display3D exportSurfel;
 
  // Each connected compoments are simply displayed with a specific color.
  GradientColorMap<long> gradient(0, (const long)vectConnectedSCell.size());
  gradient.addColor(Color::Red);
  gradient.addColor(Color::Yellow);
  gradient.addColor(Color::Green);
  gradient.addColor(Color::Cyan);
  gradient.addColor(Color::Blue);
  gradient.addColor(Color::Magenta);
  gradient.addColor(Color::Red);  
 
  // Processing ViewerInt  
  for(int i=0; i< vectConnectedSCell.size();i++){
    DGtal::Color col= gradient(i);
    exportSurfel << CustomColors3D(Color(250, 0,0), Color(col.red(), 
							  col.green(),
							  col.blue()));
    for(int j=0; j< vectConnectedSCell.at(i).size();j++){
      exportSurfel << vectConnectedSCell.at(i).at(j);
    }    
  }

  Z3i::DigitalSet imageSet(image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(imageSet, image, minThreshold, maxThreshold);
  
  
  exportSurfel << CustomColors3D(Color(250, 0,0),Color(250, 200,200, 200));
  //exportSurfel << imageSet;  
  exportSurfel >> outputFileName;

  if(args.check("-exportSRC")){
    Display3D exportSRC;
    exportSRC << imageSet;
    exportSRC >> srcFileName;
  }

}



