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

#ifndef selxLogger_cxx
#define selxLogger_cxx

#include "selxLogger.h"
#include "selxMacro.h"

namespace src = boost::log::sources;

// This goes somewhere special, not on the component itself
//
// boost::logging::add_common_attributes();
// 
// void
// Logger
// ::LogToFile( void )
// {
//   boost::log::add_file_log(
//     boost::log::keywords::file_name = "SuperElastix_%Y-%m-%d_%H-%M-%S.%N.log",           
//     boost::log::keywords::rotation_size = 1024 * 1024 * 1024, // 1GB
//     boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // Everyday at midnight
//     boost::log::keywords::format = "[%TimeStamp% %ComponentName% %SeverityLevel%]: %Message%"
//   );
// }
//
// void
// Logger
// ::LogToConsole( void )
// {
//
// }

namespace selx
{

Logger
::Logger( void )
{
  this->m_Logger = src::severity_logger< SeverityLevel >();
  this->m_Logger.add_attribute( "ComponentName", boost::log::attributes::constant< std::string >( this->GetDescription() ) );
  this->m_Logger->set_filter( boost::log::trivial::severity >= this->m_LogLevel );
}

void
Logger
::Log( const std::string message, SeverityLevel severity_level )
{
  boost::logging::record record = this->m_Logger.open_record(keywords::severity = normal);
  if( record )
  {
    boost::logging::record_ostream stream = logging::record_ostream( record );
    stream << message.c_str();
    stream.flush();
    this->m_Logger.push_record( boost::move( record ) );
  }
}

}

#endif // selxLogger_cxx




