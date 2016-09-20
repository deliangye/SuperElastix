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


#ifndef selxLogComponent_h
#define selxLogComponent_h

#include "selxInterfaces.h"
#include "selxSuperElastixComponent.h"

#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/core.hpp>

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

class LogComponent : 
  public SuperElastixComponent<
    Accepting< >,
    Providing
    <
      LogInterface
    >
  >
{
public:

  // FB: somehow the selxNewMacro does not work. The itkNewMacro, however works. Something to figure out why...
  // selxNewMacro( LogComponent, ComponentBase );

  /** Standard class typedefs. */
  typedef LogComponent                    Self;
  typedef ComponentBase                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** New macro for creation of through the object factory. */
  //selxNewMacro(Self, ComponentBase);
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( LogComponent, ComponentBase) ;

  LogComponent();
  virtual ~LogComponent(){};

  void Log( boost::log::trivial::severity_level severity_level, const std::string message );

  std::ostream& operator<<( std::ostream& )
  {
    this->Log( this->m_SeverityLevel, strm.str() );
  }

  virtual bool MeetsCriterion( const CriterionType & criterion)  override;

  static const char * GetDescription() { return "Log Component"; }

private:
  
  boost::log::severity_level m_LogLevel;
  boost::log::source::severity_logger m_Logger;
  
};

}

#endif // selxLogComponent_h