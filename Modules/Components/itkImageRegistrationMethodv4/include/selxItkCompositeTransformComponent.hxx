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

#include "selxItkCompositeTransformComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class InternalComputationValueType, int Dimensionality >
ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >::ItkCompositeTransformComponent()
{
  m_CompositeTransform = CompositeTransformType::New();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

template< class InternalComputationValueType, int Dimensionality >
ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >::~ItkCompositeTransformComponent()
{
}

template< class InternalComputationValueType, int Dimensionality >
int
ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >
::Set(MultiStageTransformInterface< InternalComputationValueType, Dimensionality >* component)
{
  // todo how do we organize the fixedtransforms?
  this->m_registrationStages.push_back(component);
  return 1;
}

template< class InternalComputationValueType, int Dimensionality >
typename ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >::TransformType::Pointer
ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >::GetItkTransform()
{
  return (typename TransformType::Pointer)this->m_CompositeTransform;
}

template< class InternalComputationValueType, int Dimensionality >
void
ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >::RegistrationControllerStart()
{
  for (auto & stage : this->m_registrationStages)
  {
    stage->SetMovingInitialTransform(this->m_CompositeTransform);
    stage->RunRegistration();
    this->m_CompositeTransform->AppendTransform(stage->GetItkTransform());
  }
  return;
}

template< class InternalComputationValueType, int Dimensionality >
bool
ItkCompositeTransformComponent< InternalComputationValueType, Dimensionality >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );

  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }
  else if( status == CriterionStatus::Failed )
  {
    return false;
  } // else: CriterionStatus::Unknown
  return meetsCriteria;
}
} //end namespace selx