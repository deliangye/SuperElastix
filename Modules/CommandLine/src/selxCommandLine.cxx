/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "selxSuperElastixFilter.h"
#include "selxBlueprint.h"
#include "selxConfigurationReader.h"
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"

#include "selxDefaultComponents.h"

#include <boost/algorithm/string.hpp>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>

template< class T >
std::ostream &
operator<<( std::ostream & os, const std::vector< T > & v )
{
  std::copy( v.begin(), v.end(), std::ostream_iterator< T >( os, " " ) );
  return os;
}


int
main( int ac, char * av[] )
{
  try
  {
    typedef std::vector< std::string > VectorOfStringsType;

    selx::SuperElastixFilter< selx::DefaultComponents >::Pointer superElastixFilter = selx::SuperElastixFilter< selx::DefaultComponents >::New();

    fs::path            configurationPath;
    VectorOfStringsType inputPairs;
    VectorOfStringsType outputPairs;

    // Store the reader so that they will not be destroyed before the pipeline is executed.
    std::vector< selx::AnyFileReader::Pointer > fileReaders;
    // Store the writers for the update call
    //vector<ImageWriter2DType::Pointer> fileWriters;
    std::vector< selx::AnyFileWriter::Pointer > fileWriters;

    po::options_description desc( "Allowed options" );
    desc.add_options()
      ( "help", "produce help message" )
      ( "conf", po::value< fs::path >( &configurationPath )->required(), "Configuration file" )
      ( "in", po::value< VectorOfStringsType >( &inputPairs )->multitoken(), "Input data: images, labels, meshes, etc. Usage <name>=<path>" )
      ( "out", po::value< VectorOfStringsType >( &outputPairs )->multitoken(), "Output data: images, labels, meshes, etc. Usage <name>=<path>" )
      ( "graphout", po::value< fs::path >(), "Output Graphviz dot file" )
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( ac, av, desc ), vm );
    po::notify( vm );

    if( vm.count( "help" ) )
    {
      std::cout << desc << "\n";
      return 0;
    }

    selx::Blueprint::Pointer blueprint;
    if( configurationPath.extension() == ".xml" )
    {
      // TODO: open file here and pass a stream to the ConfigurationReader
      blueprint = selx::ConfigurationReader::FromXML( configurationPath.string() );
    }
    else if( configurationPath.extension() == ".json" )
    {
      // TODO: open file here and pass a stream to the ConfigurationReader
      blueprint = selx::ConfigurationReader::FromJson( configurationPath.string() );
    }
    else
    {
      throw std::invalid_argument( "Configuration file requires extension .xml or .json" );
    }

    if( vm.count( "graphout" ) )
    {
      blueprint->WriteBlueprint( vm[ "graphout" ].as< fs::path >().string() );
    }

    superElastixFilter->SetBlueprint( blueprint );

    if( vm.count( "in" ) )
    {
      std::cout << "Number of input data: " << inputPairs.size() << "\n";
      int index = 0;
      for( const auto & inputPair : inputPairs )
      {
        VectorOfStringsType nameAndPath;
        boost::split( nameAndPath, inputPair, boost::is_any_of( "=" ) );  // NameAndPath == { "name","path" }
        const std::string & name = nameAndPath[ 0 ];
        const std::string & path = nameAndPath[ 1 ];

        std::cout << " " << index << " " << name << " : " << path << "\n";
        ++index;

        // since we do not know which reader type we should instantiate for input "name",
        // we ask SuperElastix for a reader that matches the type of the source component "name"

        selx::AnyFileReader::Pointer reader = superElastixFilter->GetInputFileReader( name );
        reader->SetFileName( path );
        superElastixFilter->SetInput( name, reader->GetOutput() );
        fileReaders.push_back( reader );
      }
    }

    if( vm.count( "out" ) )
    {
      std::cout << "Number of output data: " << outputPairs.size() << "\n";
      int index = 0;
      for( const auto & outputPair : outputPairs )
      {
        VectorOfStringsType nameAndPath;
        boost::split( nameAndPath, outputPair, boost::is_any_of( "=" ) );  // NameAndPath == { "name","path" }
        const std::string & name = nameAndPath[ 0 ];
        const std::string & path = nameAndPath[ 1 ];

        std::cout << " " << index << " " << name << " : " << path << "\n";
        ++index;

        // since we do not know which writer type we should instantiate for output "name",
        // we ask SuperElastix for a writer that matches the type of the sink component "name"
        selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( name );

        //ImageWriter2DType::Pointer writer = ImageWriter2DType::New();
        writer->SetFileName( path );
        //writer->SetInput(superElastixFilter->GetOutput<Image2DType>(name));
        writer->SetInput( superElastixFilter->GetOutput( name ) );
        fileWriters.push_back( writer );
      }
    }

    /* Execute SuperElastix by updating the writers */

    for( auto & writer : fileWriters )
    {
      writer->Update();
    }
  }
  catch( std::exception & e )
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch( ... )
  {
    std::cerr << "Exception of unknown type!\n";
  }

  return 0;
}
