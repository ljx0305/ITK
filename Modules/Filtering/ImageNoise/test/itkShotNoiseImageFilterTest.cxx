/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkShotNoiseImageFilter.h"
#include "itkTestingMacros.h"

int itkShotNoiseImageFilterTest(int argc, char * argv[])
{

  if( argc < 3 )
    {
    std::cerr << "usage: " << argv[0] << " input output [scale]" << std::endl;
    return EXIT_FAILURE;
    }

  constexpr unsigned int Dimension = 2;

  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType, Dimension >;

  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  using ShotNoiseImageFilterType =
      itk::ShotNoiseImageFilter< ImageType, ImageType >;
  ShotNoiseImageFilterType::Pointer shotNoiseImageFilter = ShotNoiseImageFilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( shotNoiseImageFilter, ShotNoiseImageFilter,
    NoiseBaseImageFilter );

  double scale = 1.0;
  if( argc >= 4 )
    {
    scale = atof( argv[3] );
    }
  shotNoiseImageFilter->SetScale( scale );
  TEST_SET_GET_VALUE( scale, shotNoiseImageFilter->GetScale() );


  shotNoiseImageFilter->SetInput( reader->GetOutput() );

  itk::SimpleFilterWatcher watcher( shotNoiseImageFilter, "ShotNoiseImageFilter" );

  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( shotNoiseImageFilter->GetOutput() );
  writer->SetFileName( argv[2] );

  TRY_EXPECT_NO_EXCEPTION( writer->Update() );

  return EXIT_SUCCESS;
}
