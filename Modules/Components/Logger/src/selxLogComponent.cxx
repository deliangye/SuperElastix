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

#ifndef selxLogComponent_cxx
#define selxLogComponent_cxx

#include "selxLogComponent.h"
#include "selxMacro.h"

namespace selx
{

LogComponent
::LogComponent( void )
{


  this->m_Logger = boost::log::source::severity_logger< SeverityLevel >();

  // Add LineID and TimeStamp (ProcessID and ThreadID is also added, but not used)
  boost::logging::add_common_attributes();

    // TODO: Initialize parameters with values from blueprint
  boost::logging::add_file_log(
    keywords::file_name = "SuperElastix_%Y-%m-%d_%H-%M-%S.%N.log",           
    keywords::rotation_size = 1024 * 1024 * 1024, // 1GB
    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), // Everyday at midnight
    keywords::format = "[%LineID% %TimeStamp% %ComponentName% %SeverityLevel%]: %Message%"
  );

  boost::logging::add_console_log(
    std::cout, 
    boost::log::keywords::format = "[%TimeStamp% %ComponentName% %SeverityLevel%]: %Message%"
  );

  this->m_LogLevel = boost::log::trivial::severity_level::info;
  boost::logging::core::get()->set_filter( boost::logging::trivial::severity >= this->m_LogLevel );
}

void
LogComponent
::Log( SeverityLevel severityLevel, const std::string message )
{
  boost::logging::record record = this->m_Logger.open_record(keywords::severity = severityLevel);
  if( record )
  {
    boost::logging::record_ostream strm( record );
    strm << message;
    strm.flush();
    this->m_Logger.push_record( boost::move( record ) );
  }
}

bool
LogComponent
::MeetsCriterion( const CriterionType & criterion )
{
  bool meetsCriteria( false );
  if( criterion.first == "LogLevel" )
  {
    meetsCriteria = true;
    //for( auto const & criterionValue : criterion.second )  // auto&& preferred?
    //{
      //if( criterionValue != "INFO" )   // e.g. "GradientDescent", "SupportsSparseSamples
      //{
      //  meetsCriteria = false;
      //}
    //}
  }

  return meetsCriteria;
}

}

#endif // selxLogComponent_cxx
