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

// http://stackoverflow.com/questions/20086754/how-to-use-boost-log-from-multiple-files-with-gloa/22068278#22068278

#ifndef selxLogger_h
#define selxLogger_h

#include "selxInterfaces.h"
#include "selxSuperElastixComponent.h"

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace src = boost::log::sources;

namespace selx 
{

enum SeverityLevel
{
  TRACE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL
};
  
class Logger : public itk::LightObject
{
public:

  //@kasper: somehow the selxNewMacro does not work. The itkNewMacro, however works. Something to figure out why...
  //selxNewMacro( Logger, ComponentBase );

  /** Standard class typedefs. */
  typedef Logger                          Self;
  typedef ComponentBase                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** New macro for creation of through the object factory. */
  //selxNewMacro(Self, ComponentBase);
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Logger, ComponentBase );

  Logger();
  virtual ~Logger(){};

  void Log( const std::string message, SeverityLevel severity_level );

private:
  
  src::severity_logger< SeverityLevel > m_Logger;
  SeverityLevel m_LogLevel;
  
};

}

#endif // selxLogger_h