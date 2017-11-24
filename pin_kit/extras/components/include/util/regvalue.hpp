/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2015 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
// <ORIGINAL-AUTHOR>: Greg Lueck
// <COMPONENT>: util
// <FILE-TYPE>: component public header

#ifndef UTIL_REGVALUE_HPP
#define UTIL_REGVALUE_HPP

#undef min  // Some broken system headers define "min", which conflicts with std::min().

#include <string.h>
#include <algorithm>
#include "fund.hpp"
#include "util/data.hpp"


namespace UTIL {

/*!
 * Utility that holds the value of a register and the size of the register from which
 * it came.
 *
 * @todo: There are many assumptions that the host is little-endian.
 */
class /*<UTILITY>*/ REGVALUE
{
public:
    /*!
     * Construct an uninitialized register value.
     */
    REGVALUE() : _size(0), _value(0) {}

    /*!
     * Construct a register value that is a copy of an existing value.
     *
     *  @param[in] other    The copied value.
     */
    REGVALUE(const REGVALUE &other)
    {
        SetCopy(other);
    }

    /*!
     * Construct a register value that is the same size as "ADDRINT".
     *
     *  @param[in] val  Value of the register.
     */
    REGVALUE(FUND::ADDRINT val)
    {
        SetAddress(val);
    }

    /*!
     * Construct a register value.
     *
     *  @param[in] val      Value of the register.
     *  @param[in] size     Size (bits) of the register.
     */
    REGVALUE(FUND::UINT64 val, unsigned size)
    {
        Set64(val, size);
    }

    /*!
     * Construct a register value.
     *
     *  @param[in] lo       Low 64 bits of register value.
     *  @param[in] hi       High 64 bits of register value.
     *  @param[in] size     Size (bits) of the register.
     */
    REGVALUE(FUND::UINT64 lo, FUND::UINT64 hi, unsigned size)
    {
        Set128(lo, hi, size);
    }

    /*!
     * Construct a register value from a DATA buffer.  The content of the data
     * buffer is interpreted as big- or little-endian according to the host byte
     * order.
     *
     *  @param[in] val      Value and size of the register.
     *  @param[in] size     If non-zero, the size (bits) of the register.  Otherwise,
     *                       the register's width is the size of \a val.
     */
    REGVALUE(const UTIL::DATA &val, unsigned size=0)
    {
        SetBuffer(val.GetBuf<void>(), val.GetSize(), size);
    }

    /*!
     * Construct a register value from a raw byte buffer.  The content of the buffer is
     * interpreted as a big- or little-endian value according to the host byte order.
     *
     *  @param[in] buf          Pointer to raw buffer of bytes.
     *  @param[in] byteSize     Size (in bytes) of the buffer \a buf.
     *  @param[in] size         If non-zero, the size (bits) of the register.  Otherwise,
     *                           the register's width is \a byteSize.
     */
    template<typename T> REGVALUE(const T *buf, size_t byteSize, unsigned size=0)
    {
        SetBuffer(static_cast<const void *>(buf), byteSize, size);
    }

    ~REGVALUE() {ClearIfNeeded();}

    /*!
     * Re-assign to a copy of another register value.
     *
     *  @param[in] other    Register value that is copied.
     *
     * @return  Reference to the new register value.
     */
    REGVALUE &operator =(const REGVALUE &other)
    {
        ClearIfNeeded();
        SetCopy(other);
        return *this;
    }

    /*!
     * Re-assign to a copy of another register value.
     *
     *  @param[in] other    Register value that is copied.
     */
    void Assign(const REGVALUE &other)
    {
        ClearIfNeeded();
        SetCopy(other);
    }

    /*!
     * Re-assign to a new register value that is the same size as "ADDRINT".
     *
     *  @param[in] val  Value of the register.
     */
    void Assign(FUND::ADDRINT val)
    {
        ClearIfNeeded();
        SetAddress(val);
    }

    /*!
     * Re-assign to a new register value.
     *
     *  @param[in] val      Value of the register.
     *  @param[in] size     Size (bits) of the register.
     */
    void Assign(FUND::UINT64 val, unsigned size)
    {
        ClearIfNeeded();
        Set64(val, size);
    }

    /*!
     * Re-assign to a new register value.
     *
     *  @param[in] lo       Low 64 bits of register value.
     *  @param[in] hi       High 64 bits of register value.
     *  @param[in] size     Size (bits) of the register.
     */
    void Assign(FUND::UINT64 lo, FUND::UINT64 hi, unsigned size)
    {
        ClearIfNeeded();
        Set128(lo, hi, size);
    }

    /*!
     * Re-assign to a new register value from a DATA buffer.  The content of the data
     * buffer is interpreted as big- or little-endian according to the host byte
     * order.
     *
     *  @param[in] val      Value and size of the register.
     *  @param[in] size     If non-zero, the size (bits) of the register.  Otherwise,
     *                       the register's width is the size of \a val.
     */
    void Assign(const UTIL::DATA &val, unsigned size=0)
    {
        ClearIfNeeded();
        SetBuffer(val.GetBuf<void>(), val.GetSize(), size);
    }

    /*!
     * Re-assign to a new register value from a raw byte buffer.  The content of the buffer
     * is interpreted as a big- or little-endian value according to the host byte order.
     *
     *  @param[in] buf          Pointer to raw buffer of bytes.
     *  @param[in] byteSize     Size (in bytes) of the buffer \a buf and of the register.
     *  @param[in] size         If non-zero, the size (bits) of the register.  Otherwise,
     *                           the register's width is \a byteSize.
     */
    template<typename T> void Assign(const T *buf, size_t byteSize, unsigned size=0)
    {
        ClearIfNeeded();
        SetBuffer(static_cast<const void *>(buf), byteSize, size);
    }

    /*!
     * Change the size of the register value, truncating or zero-extending the value
     * as necessary.
     *
     *  @param[in] size     New size (bits).
     */
    void Resize(unsigned size)
    {
        if (_size <= 8*sizeof(FUND::PTRINT))
        {
            Set64(_value, size);
        }
        else
        {
            UTIL::DATA *d = _bigValue;
            SetBuffer(d->GetBuf<void>(), d->GetSize(), size);
            delete d;
        }
    }

    /*!
     * @return  The size (bits) of the register.
     */
    unsigned GetBitSize() const     {return _size;}

    /*!
     * Copy the register value to a DATA buffer.  The value is written as
     * big- or little-endian according to the host byte order.
     *
     *  @param[out] data    Receives the value and size of the register.
     *                       If the register size is not an even multiple
     *                       of bytes, it is rounded up and the high-order
     *                       bits are set to zero.
     */
    void CopyToData(UTIL::DATA *data) const
    {
        if (_size <= 8*sizeof(FUND::PTRINT))
            data->Assign(&_value, GetByteSize());
        else
            data->Assign(*_bigValue);
    }

    /*!
     * Copy the register value to a raw byte buffer.  The value is written
     * as big- or little-endian according to the host byte order.
     *
     *  @param[in] data     Receives the register value.  The \a data buffer
     *                       must be long enough to receive the register's size
     *                       (defined by GetBitSize()).
     */
    void CopyToBuffer(void *data) const
    {
        if (_size <= 8*sizeof(FUND::PTRINT))
            memcpy(data, &_value, GetByteSize());
        else
            memcpy(data, _bigValue->GetBuf<void>(), _bigValue->GetSize());
    }

    /*!
     * Return the value of the register cast to an integral type.  The size
     * of the register need not be the same as the size of the returned type.
     * The value is zero-extended or truncated as necessary.
     *
     * @return  The register value.
     */
    template<typename T> T GetValueAs() const
    {
        if (_size <= 8*sizeof(FUND::PTRINT))
            return static_cast<T>(_value);
        return GetIndexedWord<T>(0);
    }

    /*!
     * Considering the register value as an array of words of type "T", return
     * one word from that array.
     *
     *  @param[in] index    Specifies the word to return.  Index zero specifies the
     *                       least significant word.
     *
     * @return  The word, or zero if \a index is out of range for the register's size.
     */
    template<typename T> T GetIndexedWord(unsigned index) const
    {
        size_t byteSize = GetByteSize();
        const FUND::UINT8 *buf = static_cast<const FUND::UINT8 *>(GetBuffer());
        size_t offset = index * sizeof(T);
        if (offset + sizeof(T) <= byteSize)
        {
            T tmp;
            memcpy(static_cast<void *>(&tmp), static_cast<const void *>(buf+offset), sizeof(T));
            return tmp;
        }
        else if (offset < byteSize)
        {
            T tmp(0);
            memcpy(static_cast<void *>(&tmp), static_cast<const void *>(buf+offset), byteSize-offset);
            return tmp;
        }
        else
        {
            return T(0);
        }
    }

private:
    /*!
     * Set to a copy of another register value.
     *
     *  @param[in] other    The copied register value.
     */
    void SetCopy(const REGVALUE &other)
    {
        _size = other._size;
        if (_size <= 8*sizeof(FUND::PTRINT))
            _value = other._value;
        else
            _bigValue = new UTIL::DATA(*other._bigValue);
    }

    /*!
     * Set to an "ADDRINT" sized value.
     *
     *  @param[in] val  Register value.
     */
    void SetAddress(FUND::ADDRINT val)
    {
        _size = 8*sizeof(FUND::ADDRINT);
#if defined(FUND_ADDRINT_SIZE) && (FUND_ADDRINT_SIZE <= FUND_PTRINT_SIZE)
        _value = val;
#else
        _bigValue = new DATA(&val, sizeof(FUND::ADDRINT));
#endif
    }

    /*!
     * Set the value and size.  Size could be larger than UINT64.
     *
     *  @param[in] val      Register value.
     *  @param[in] size     Size (bits).
     */
    void Set64(FUND::UINT64 val, unsigned size)
    {
        _size = size;
        if (size == 8*sizeof(FUND::PTRINT))
        {
            _value = static_cast<FUND::PTRINT>(val);
        }
        else if (size < 8*sizeof(FUND::PTRINT))
        {
            FUND::PTRINT mask = FUND::PTRINT(1);
            _value = static_cast<FUND::PTRINT>(val) & ((mask << size) - FUND::PTRINT(1));
        }
        else if (size == 8*sizeof(FUND::UINT64))
        {
            _bigValue = new DATA(&val, 8);
        }
        else if (size > 8*sizeof(FUND::UINT64))
        {
            // Size of DATA > sizeof(val), so top bytes of DATA need to be zeroed.
            //
            unsigned byteSize = (size + 7) >> 3;
            _bigValue = new DATA(byteSize);
            memcpy(_bigValue->GetWritableBuf<void>(), static_cast<void *>(&val), sizeof(val));
            FUND::UINT8 *rest = _bigValue->GetWritableBuf<FUND::UINT8>() + sizeof(val);
            memset(static_cast<void *>(rest), 0, byteSize - sizeof(val));
            _bigValue->ReleaseWritableBuf();
        }
        else
        {
            // Register size < UINT64 and > PTRINT.
            // This can't happen on 64-bit hosts.
            //
            FUND::UINT64 mask = FUND::UINT64(1);
            val = val & ((mask << size) - FUND::UINT64(1));
            unsigned byteSize = (size + 7) >> 3;
            _bigValue = new DATA(&val, byteSize);
        }
    }

    /*!
     * Set the value and size.
     *
     *  @param[in] lo       Low 64 bits of register value.
     *  @param[in] hi       High 64 bits of register value.
     *  @param[in] size     Size (bits).
     */
    void Set128(FUND::UINT64 lo, FUND::UINT64 hi, unsigned size)
    {
        FUND::UINT64 buf[2];
        buf[0] = lo;
        buf[1] = hi;
        SetBuffer(static_cast<const void *>(buf), sizeof(buf), size);
    }

    /*!
     * Set to the contents of a raw buffer.  The value in the buffer is interpreted
     * as big- or little-endian according to the host byte order.
     *
     *  @param[in] buf          Buffer with value.
     *  @param[in] byteSize     Size (bytes) of buffer.
     *  @param[in] size         Size (bits) of register.  If zero, size is \a byteSize.
     */
    void SetBuffer(const void *buf, size_t byteSizeIn, unsigned size)
    {
        if (!size)
            size = 8*byteSizeIn;
        _size = size;
        size_t byteSize = (size + 7) >> 3;

        if (size == 8*sizeof(FUND::PTRINT))
        {
            // The input buffer might be smaller than PTRINT, so zero out '_value' first to zero-
            // extend the buffer's value.
            //
            _value = 0;
            memcpy(static_cast<void *>(&_value), buf, std::min(byteSizeIn, sizeof(FUND::PTRINT)));
        }
        else if (size < 8*sizeof(FUND::PTRINT))
        {
            // Same as above, but we also need to mask the final value down to the register size.
            //
            _value = 0;
            memcpy(static_cast<void *>(&_value), buf, std::min(byteSizeIn, byteSize));
            FUND::PTRINT mask = FUND::PTRINT(1);
            _value &= ((mask << size) - FUND::PTRINT(1));
        }
        else
        {
            // Create a DATA large enough for the register size, which might be different than
            // the input buffer size.
            //
            _bigValue = new DATA(byteSize);
            size_t sizeCopied = std::min(byteSizeIn, byteSize);
            memcpy(_bigValue->GetWritableBuf<void>(), buf, sizeCopied);
            memset(static_cast<void *>(_bigValue->GetWritableBuf<FUND::UINT8>()+sizeCopied), 0, byteSize-sizeCopied);

            // The last byte in the DATA might need to be masked if the register size is not an even
            // multiple of bytes.  However, if the buffer size is less than the DATA size, the last
            // byte has already been zeroed by the memset() above.
            //
            if ((byteSize <= byteSizeIn) && (size < 8*byteSize))
            {
                unsigned numBitsLast = 8 - (8*byteSize - size);
                FUND::UINT8 *last = &_bigValue->GetWritableBuf<FUND::UINT8>()[_bigValue->GetSize()-1];
                FUND::UINT8 mask = FUND::UINT8(1);
                *last &= ((mask << numBitsLast) - FUND::UINT8(1));
            }
            _bigValue->ReleaseWritableBuf();
        }
    }

    /*!
     * Clear the _bigValue buffer if it is allocated.
     */
    void ClearIfNeeded()
    {
        if (_size > 8*sizeof(FUND::PTRINT))
            delete _bigValue;
    }

    /*!
     * @return  Size of the register, rounded up to the next byte.
     */
    size_t GetByteSize() const
    {
        return (_size + 7) >> 3;
    }

    /*!
     * @return  Pointer to the buffer containing the register value.  The value in
     *           the buffer has the host's byte order.  The lifetime of the buffer
     *           lasts only until the next operation that changes the REGVALUE.
     */
    const void *GetBuffer() const
    {
        if (_size <= 8*sizeof(FUND::PTRINT))
            return static_cast<const void *>(&_value);
        else
            return _bigValue->GetBuf<void>();
    }


private:
    unsigned _size;             // Register width in bits.

    // Register value.  Any unused high-order bits are zero.
    //
    union
    {
        FUND::PTRINT _value;    // Value if size <= sizeof(void *)
        UTIL::DATA *_bigValue;  // Value if size > sizeof(void *), in host byte order
    };
};

} // namespace
#endif // file guard
