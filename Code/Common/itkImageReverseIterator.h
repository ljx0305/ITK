/*==========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageReverseIterator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkImageReverseIterator_h
#define __itkImageReverseIterator_h

#include "itkIndex.h"
#include "itkSize.h"
#include "itkImage.h"
#include "itkImageIterator.h"
#include <memory>

namespace itk
{

/**
 * \class ImageReverseIterator
 * \brief Multi-dimensional image iterator.
 * 
 * ImageReverseIterator is a templated class to represent a multi-dimensional
 * iterator. ImageReverseIterator is templated over the dimension of the image
 * and the data type of the image.
 *
 * ImageReverseIterator is a base class for all the reverse image
 * iterators. It provides the basic construction and comparison
 * operations.  However, it does not provide mechanisms for moving the
 * iterator.  A subclass of ImageReverseIterator must be used to move
 * the iterator.
 * 
 * ImageReverseIterator is a multi-dimensional iterator, requiring
 * more information be specified before the iterator can be used than
 * conventional iterators.  Whereas the std::vector::iterator from the
 * STL only needs to be passed a pointer to establish the iterator,
 * the multi-dimensional image iterator needs a pointer, the size of
 * the buffer, the size of the region, the start index of the buffer,
 * and the start index of the region. To gain access to this
 * information, ImageReverseIterator holds a reference to the image
 * over which it is traversing.
 *
 * ImageReverseIterator assumes a particular layout of the image
 * data. In particular, the data is arranged in a 1D array as if it
 * were [][][][slice][row][col] with Index[0] = col, Index[1] = row,
 * Index[2] = slice, etc.
 *
 *
 */
template<typename TImage>
class ImageReverseIterator {
public:
  /**
   * Standard "Self" typedef.
   */
  typedef ImageReverseIterator Self;
  
  /**
   * Dimension of the image the iterator walks.  This enum is needed so that
   * functions that are templated over image iterator type (as opposed to
   * being templated over pixel type and dimension) can have compile time
   * access to the dimension of the image that the iterator walks.
   */
  enum { ImageIteratorDimension = TImage::ImageDimension };

  /** 
   * Index typedef support.
   */
  typedef typename TImage::IndexType  IndexType;

  /** 
   * Size typedef support.
   */
  typedef typename TImage::SizeType    SizeType;

  /** 
   * Region typedef support.
   */
  typedef typename TImage::RegionType   RegionType;

  /**
   * Image typedef support.
   */
  typedef TImage   ImageType;

  /** 
   * PixelContainer typedef support. Used to refer to the container for
   * the pixel data. While this was already typdef'ed in the superclass
   * it needs to be redone here for this subclass to compile properly with gcc.
   */
  typedef typename TImage::PixelContainer PixelContainer;
  typedef typename PixelContainer::Pointer PixelContainerPointer;

  /**
   * Internal Pixel Type
   */
  typedef typename TImage::InternalPixelType   InternalPixelType;

  /**
   * External Pixel Type
   */
  typedef typename TImage::PixelType   PixelType;

  /** 
   *  Accessor type that convert data between internal and external
   *  representations.
   */
  typedef typename TImage::AccessorType     AccessorType;

  /**
   * Default Constructor. Need to provide a default constructor since we
   * provide a copy constructor.
   */
  ImageReverseIterator()
    :m_DataAccessor()
  {
    m_Buffer = 0;
    m_Offset = 0;
    m_BeginOffset = 0;
    m_EndOffset = 0;
  }

  /**
   * Default Destructor.
   */
  virtual ~ImageReverseIterator() {};

  /**
   * Copy Constructor. The copy constructor is provided to make sure the
   * handle to the image is properly reference counted.
   */
  ImageReverseIterator(const Self& it)
  {
    m_Image = it.m_Image;     // copy the smart pointer

    m_Region = it.m_Region;
    
    m_Buffer = it.m_Buffer;
    m_Offset = it.m_Offset;
    m_BeginOffset = it.m_BeginOffset;
    m_EndOffset = it.m_EndOffset;
    m_DataAccessor = it.m_DataAccessor;
  }

  /**
   * Constructor establishes an iterator to walk a particular image and a
   * particular region of that image.
   */
  ImageReverseIterator(ImageType *ptr,
                const RegionType &region)
  {
    unsigned long offset;
    m_Image = ptr;
    m_Buffer = m_Image->GetBufferPointer();
    m_Region = region;

    // Compute the end offset, one pixel before the first pixel
    offset = m_Image->ComputeOffset( m_Region.GetIndex() );
    m_EndOffset = offset-1;
    
    // Compute the begin offset, the last pixel in the region
    IndexType ind(m_Region.GetIndex());
    SizeType size(m_Region.GetSize());
    for (unsigned int i=0; i < TImage::ImageDimension; ++i)
      {
      ind[i] += (size[i] - 1);
      }
    m_BeginOffset = m_Image->ComputeOffset( ind );
    m_Offset = m_BeginOffset;

    m_DataAccessor = ptr->GetDataAccessor();
  }
  
  /**
   * Constructor that can be used to cast from an ImageIterator to an
   * ImageRegionReverseIterator. Many routines return an ImageIterator
   * but for a particular task, you may want an
   * ImageRegionReverseIterator.  Rather than provide overloaded APIs
   * that return different types of Iterators, itk returns
   * ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageRegionReverseIterator.
   */
  ImageReverseIterator( const ImageIterator<TImage> &it)
  {
    m_Image = it.GetImage();
    m_Region = it.GetRegion();
    m_Buffer = m_Image.GetBufferPointer();
    
    IndexType ind = it.GetIndex();

    m_Offset = m_Image.ComputeOffset( ind );

    // Compute the end offset, one pixel before the first pixel
    m_EndOffset = m_Image->ComputeOffset( m_Region.GetIndex() ) - 1;
    
    // Compute the begin offset, the last pixel in the region
    IndexType regInd(m_Region.GetIndex());
    SizeType regSize(m_Region.GetSize());
    for (unsigned int i=0; i < TImage::ImageDimension; ++i)
      {
      regInd[i] += (regSize[i] - 1);
      }
    m_BeginOffset = m_Image->ComputeOffset( regInd );
    
    m_DataAccessor = it.GetDataAccessor();
  }

  /**
   * operator= is provided to make sure the handle to the image is properly
   * reference counted.
   */
  Self &operator=(const Self& it)
  {
    m_Image = it.m_Image;     // copy the smart pointer
    m_Region = it.m_Region;
    
    m_Buffer = it.m_Buffer;
    m_Offset = it.m_Offset;
    m_BeginOffset = it.m_BeginOffset;
    m_EndOffset = it.m_EndOffset;
    m_DataAccessor = it.m_DataAccessor;

    return *this;
  }
  
  /**
   * Get the dimension (size) of the index.
   */
  static unsigned int GetImageIteratorDimension() 
    {return TImage::ImageDimension;}

  /**
   * Comparison operator. Two iterators are the same if they "point to" the
   * same memory location
   */
  bool
  operator!=(const Self &it) const
    {
    // two iterators are the same if they "point to" the same memory location
    return (m_Buffer + m_Offset) != (it.m_Buffer + it.m_Offset);
    };

  /**
   * Comparison operator. Two iterators are the same if they "point to" the
   * same memory location
   */
  bool
  operator==(const Self &it) const
    {
    // two iterators are the same if they "point to" the same memory location
    return (m_Buffer + m_Offset) == (it.m_Buffer + it.m_Offset);
    };
  
#if 0
  /**
   * I am not sure what the semantics of <=, <, >, >= should be for reverse
   * iterators.  So, I am taking them out for now.
   */

  
  /**
   * Comparison operator. An iterator is "less than" another if it "points to"
   * a lower memory location.
   */
  bool
  operator<=(const Self &it) const
    {
    // an iterator is "less than" another if it "points to" a lower
    // memory location
    return (m_Buffer + m_Offset) <= (it.m_Buffer + it.m_Offset);
    };

  /**
   * Comparison operator. An iterator is "less than" another if it "points to"
   * a lower memory location.
   */
  bool
  operator<(const Self &it) const
    {
    // an iterator is "less than" another if it "points to" a lower
    // memory location
    return (m_Buffer + m_Offset) < (it.m_Buffer + it.m_Offset);
    };

  /**
   * Comparison operator. An iterator is "greater than" another if it
   * "points to" a higher location.
   */
  bool
  operator>=(const Self &it) const
    {
    // an iterator is "greater than" another if it "points to" a higher
    // memory location
    return (m_Buffer + m_Offset) >= (it.m_Buffer + it.m_Offset);
    };

  /**
   * Comparison operator. An iterator is "greater than" another if it
   * "points to" a higher location.
   */
  bool
  operator>(const Self &it) const
    {
    // an iterator is "greater than" another if it "points to" a higher
    // memory location
    return (m_Buffer + m_Offset) > (it.m_Buffer + it.m_Offset);
    };
#endif

  
  /**
   * Get the index. This provides a read only reference to the index.
   * This causes the index to be calculated from pointer arithmetic and is
   * therefore an expensive operation.
   * \sa SetIndex
   */
  const IndexType GetIndex()
    { return m_Image->ComputeIndex( m_Offset );  }

  /**
   * Set the index. No bounds checking is performed.
   * \sa GetIndex
   */
  virtual void SetIndex(const IndexType &ind)
    { m_Offset = m_Image->ComputeOffset( ind ); }


  /**
   * Get the region that this iterator walks. ImageReverseIterators know the
   * beginning and the end of the region of the image to iterate over.
   */
  const RegionType& GetRegion() const
    { return m_Region; };

  /**
   * Get the pixel value
   */
  PixelType & Get(void) const  
    { return m_DataAccessor.Get(*(m_Buffer+m_Offset)); }
  
  /**
   * Set the pixel value
   */
  void Set( const PixelType & value) const  
    { m_DataAccessor.Set(*(m_Buffer+m_Offset),value); }

  /**
   * Return a const reference to the pixel 
   * This method will provide the fastest access to pixel
   * data, but it will NOT support ImageAdaptors.
   */
  const PixelType & Value(void) const  
    { return *(m_Buffer+m_Offset); }
 
  /**
   * Return a reference to the pixel 
   * This method will provide the fastest access to pixel
   * data, but it will NOT support ImageAdaptors.
   */
  PixelType & Value(void) 
    { return *(m_Buffer+m_Offset); }

  /**
   * Return an iterator for the beginning of the region.
   */
  Self Begin();

  /**
   * Return an iterator for the end of the region.
   */
  Self End();

  /**
   * Is the iterator at the beginning of the (reverse) region?
   */
  bool IsAtBegin()
    {
    return (m_Offset == m_BeginOffset);
    }

  /**
   * Is the iterator at the end of the (reverse) region?
   */
  bool IsAtEnd()
    {
    return (m_Offset == m_EndOffset);
    }
  
protected: //made protected so other iterators can access 
  SmartPointer<ImageType> m_Image;
  RegionType              m_Region;      // region to iterate over
  
  unsigned long  m_Offset;
  unsigned long  m_BeginOffset; // offset to last pixel in region
  unsigned long  m_EndOffset;   // offset to one pixel before first pixel

  InternalPixelType        *m_Buffer;

  AccessorType           m_DataAccessor;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageReverseIterator.txx"
#endif

#endif 
