//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// $Header: $
// $NoKeywords: $
//
// Serialization buffer
//===========================================================================//

#pragma warning (disable : 4514)

#include "UtlBuffer.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "characterset.hpp"

const char* V_strnchr(const char* pStr, char c, int n)
{
    char const* pLetter = pStr;
    char const* pLast = pStr + n;

    // Check the entire string
    while((pLetter < pLast) && (*pLetter != 0)) {
        if(*pLetter == c)
            return pLetter;
        ++pLetter;
    }
    return NULL;
}
//-----------------------------------------------------------------------------
// Finds a string in another string with a case insensitive test w/ length validation
//-----------------------------------------------------------------------------
char const* V_strnistr(char const* pStr, char const* pSearch, int n)
{
    if(!pStr || !pSearch)
        return 0;

    char const* pLetter = pStr;

    // Check the entire string
    while(*pLetter != 0) {
        if(n <= 0)
            return 0;

        // Skip over non-matches
        if(tolower(*pLetter) == tolower(*pSearch)) {
            int n1 = n - 1;

            // Check for match
            char const* pMatch = pLetter + 1;
            char const* pTest = pSearch + 1;
            while(*pTest != 0) {
                if(n1 <= 0)
                    return 0;

                // We've run off the end; don't bother.
                if(*pMatch == 0)
                    return 0;

                if(tolower(*pMatch) != tolower(*pTest))
                    break;

                ++pMatch;
                ++pTest;
                --n1;
            }

            // Found a match!
            if(*pTest == 0)
                return pLetter;
        }

        ++pLetter;
        --n;
    }

    return 0;
}
//-----------------------------------------------------------------------------
// Character conversions for C strings
//-----------------------------------------------------------------------------
class CUtlCStringConversion : public CUtlCharConversion
{
public:
    CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray);

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength);

private:
    char m_pConversion[255];
};


//-----------------------------------------------------------------------------
// Character conversions for no-escape sequence strings
//-----------------------------------------------------------------------------
class CUtlNoEscConversion : public CUtlCharConversion
{
public:
    CUtlNoEscConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
        CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
    {
    }

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength) { *pLength = 0; return 0; }
};


//-----------------------------------------------------------------------------
// List of character conversions
//-----------------------------------------------------------------------------
BEGIN_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\')
{
    '\n', "n"
},
{ '\t', "t" },
{ '\v', "v" },
{ '\b', "b" },
{ '\r', "r" },
{ '\f', "f" },
{ '\a', "a" },
{ '\\', "\\" },
{ '\?', "\?" },
{ '\'', "\'" },
{ '\"', "\"" },
END_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\');

    CUtlCharConversion *GetCStringCharConversion()
    {
        return &s_StringCharConversion;
    }

    BEGIN_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F)
    {
        0x7F, ""
    },
        END_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F);

        CUtlCharConversion *GetNoEscCharConversion()
        {
            return &s_NoEscConversion;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCStringConversion::CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
            CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
        {
            memset(m_pConversion, 0x0, sizeof(m_pConversion));
            for(int i = 0; i < nCount; ++i) {
                m_pConversion[pArray[i].m_pReplacementString[0]] = pArray[i].m_nActualChar;
            }
        }

        // Finds a conversion for the passed-in string, returns length
        char CUtlCStringConversion::FindConversion(const char *pString, int *pLength)
        {
            char c = m_pConversion[pString[0]];
            *pLength = (c != '\0') ? 1 : 0;
            return c;
        }



        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCharConversion::CUtlCharConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray)
        {
            m_nEscapeChar = nEscapeChar;
            m_pDelimiter = pDelimiter;
            m_nCount = nCount;
            m_nDelimiterLength = strlen(pDelimiter);
            m_nMaxConversionLength = 0;

            memset(m_pReplacements, 0, sizeof(m_pReplacements));

            for(int i = 0; i < nCount; ++i) {
                m_pList[i] = pArray[i].m_nActualChar;
                ConversionInfo_t &info = m_pReplacements[m_pList[i]];
                assert(info.m_pReplacementString == 0);
                info.m_pReplacementString = pArray[i].m_pReplacementString;
                info.m_nLength = strlen(info.m_pReplacementString);
                if(info.m_nLength > m_nMaxConversionLength) {
                    m_nMaxConversionLength = info.m_nLength;
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Escape character + delimiter
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::GetEscapeChar() const
        {
            return m_nEscapeChar;
        }

        const char *CUtlCharConversion::GetDelimiter() const
        {
            return m_pDelimiter;
        }

        int CUtlCharConversion::GetDelimiterLength() const
        {
            return m_nDelimiterLength;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        const char *CUtlCharConversion::GetConversionString(char c) const
        {
            return m_pReplacements[c].m_pReplacementString;
        }

        int CUtlCharConversion::GetConversionLength(char c) const
        {
            return m_pReplacements[c].m_nLength;
        }

        int CUtlCharConversion::MaxConversionLength() const
        {
            return m_nMaxConversionLength;
        }


        //-----------------------------------------------------------------------------
        // Finds a conversion for the passed-in string, returns length
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::FindConversion(const char *pString, int *pLength)
        {
            for(int i = 0; i < m_nCount; ++i) {
                if(!strcmp(pString, m_pReplacements[m_pList[i]].m_pReplacementString)) {
                    *pLength = m_pReplacements[m_pList[i]].m_nLength;
                    return m_pList[i];
                }
            }

            *pLength = 0;
            return '\0';
        }


        //-----------------------------------------------------------------------------
        // constructors
        //-----------------------------------------------------------------------------
        CUtlBuffer::CUtlBuffer(int growSize, int initSize, int nFlags) :
            m_Memory(growSize, initSize), m_Error(0)
        {
            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if((initSize != 0) && !IsReadOnly()) {
                m_nMaxPut = -1;
                AddNullTermination();
            } else {
                m_nMaxPut = 0;
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }

        CUtlBuffer::CUtlBuffer(const void *pBuffer, int nSize, int nFlags) :
            m_Memory((unsigned char*)pBuffer, nSize), m_Error(0)
        {
            assert(nSize != 0);

            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if(IsReadOnly()) {
                m_nMaxPut = nSize;
            } else {
                m_nMaxPut = -1;
                AddNullTermination();
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }


        //-----------------------------------------------------------------------------
        // Modifies the buffer to be binary or text; Blows away the buffer and the CONTAINS_CRLF value. 
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetBufferType(bool bIsText, bool bContainsCRLF)
        {
#ifdef _DEBUG
            // If the buffer is empty, there is no opportunity for this stuff to fail
            if(TellMaxPut() != 0) {
                if(IsText()) {
                    if(bIsText) {
                        assert(ContainsCRLF() == bContainsCRLF);
                    } else {
                        assert(ContainsCRLF());
                    }
                } else {
                    if(bIsText) {
                        assert(bContainsCRLF);
                    }
                }
            }
#endif

            if(bIsText) {
                m_Flags |= TEXT_BUFFER;
            } else {
                m_Flags &= ~TEXT_BUFFER;
            }
            if(bContainsCRLF) {
                m_Flags |= CONTAINS_CRLF;
            } else {
                m_Flags &= ~CONTAINS_CRLF;
            }
        }


        //-----------------------------------------------------------------------------
        // Attaches the buffer to external memory....
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.SetExternalBuffer((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Assumes an external buffer but manages its deletion
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AssumeMemory(void *pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.AssumeMemory((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Makes sure we've got at least this much memory
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EnsureCapacity(int num)
        {
            // Add one extra for the null termination
            num += 1;
            if(m_Memory.IsExternallyAllocated()) {
                if(IsGrowable() && (m_Memory.NumAllocated() < num)) {
                    m_Memory.ConvertToGrowableMemory(0);
                } else {
                    num -= 1;
                }
            }

            m_Memory.EnsureCapacity(num);
        }


        //-----------------------------------------------------------------------------
        // Base Get method from which all others derive
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Get(void* pMem, int size)
        {
            if(CheckGet(size)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], size);
                m_Get += size;
            }
        }


        //-----------------------------------------------------------------------------
        // This will Get at least 1 uint8_t and up to nSize bytes. 
        // It will return the number of bytes actually read.
        //-----------------------------------------------------------------------------
        int CUtlBuffer::GetUpTo(void *pMem, int nSize)
        {
            if(CheckArbitraryPeekGet(0, nSize)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], nSize);
                m_Get += nSize;
                return nSize;
            }
            return 0;
        }


        //-----------------------------------------------------------------------------
        // Eats whitespace
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EatWhiteSpace()
        {
            if(IsText() && IsValid()) {
                while(CheckGet(sizeof(char))) {
                    if(!isspace(*(const unsigned char*)PeekGet()))
                        break;
                    m_Get += sizeof(char);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Eats C++ style comments
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::EatCPPComment()
        {
            if(IsText() && IsValid()) {
                // If we don't have a a c++ style comment next, we're done
                const char *pPeek = (const char *)PeekGet(2 * sizeof(char), 0);
                if(!pPeek || (pPeek[0] != '/') || (pPeek[1] != '/'))
                    return false;

                // Deal with c++ style comments
                m_Get += 2;

                // read complete line
                for(char c = GetChar(); IsValid(); c = GetChar()) {
                    if(c == '\n')
                        break;
                }
                return true;
            }
            return false;
        }


        //-----------------------------------------------------------------------------
        // Peeks how much whitespace to eat
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekWhiteSpace(int nOffset)
        {
            if(!IsText() || !IsValid())
                return 0;

            while(CheckPeekGet(nOffset, sizeof(char))) {
                if(!isspace(*(unsigned char*)PeekGet(nOffset)))
                    break;
                nOffset += sizeof(char);
            }

            return nOffset;
        }


        //-----------------------------------------------------------------------------
        // Peek size of sting to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekStringLength()
        {
            if(!IsValid())
                return 0;

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                if(!IsText()) {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(pTest[i] == 0)
                            return (i + nOffset - nStartingOffset + 1);
                    }
                } else {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(isspace((unsigned char)pTest[i]) || (pTest[i] == 0))
                            return (i + nOffset - nStartingOffset + 1);
                    }
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Peek size of line to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekLineLength()
        {
            if(!IsValid())
                return 0;

            int nOffset = 0;
            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                for(int i = 0; i < nPeekAmount; ++i) {
                    // The +2 here is so we eat the terminating '\n' and 0
                    if(pTest[i] == '\n' || pTest[i] == '\r')
                        return (i + nOffset - nStartingOffset + 2);
                    // The +1 here is so we eat the terminating 0
                    if(pTest[i] == 0)
                        return (i + nOffset - nStartingOffset + 1);
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Does the next bytes of the buffer match a pattern?
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PeekStringMatch(int nOffset, const char *pString, int nLen)
        {
            if(!CheckPeekGet(nOffset, nLen))
                return false;
            return !strncmp((const char*)PeekGet(nOffset), pString, nLen);
        }


        //-----------------------------------------------------------------------------
        // This version of PeekStringLength converts \" to \\ and " to \, etc.
        // It also reads a " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekDelimitedStringLength(CUtlCharConversion *pConv, bool bActualSize)
        {
            if(!IsText() || !pConv)
                return PeekStringLength();

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            if(!PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return 0;

            // Try to read ending ", but don't accept \"
            int nActualStart = nOffset;
            nOffset += pConv->GetDelimiterLength();
            int nLen = 1;	// Starts at 1 for the '\0' termination

            do {
                if(PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                    break;

                if(!CheckPeekGet(nOffset, 1))
                    break;

                char c = *(const char*)PeekGet(nOffset);
                ++nLen;
                ++nOffset;
                if(c == pConv->GetEscapeChar()) {
                    int nLength = pConv->MaxConversionLength();
                    if(!CheckArbitraryPeekGet(nOffset, nLength))
                        break;

                    pConv->FindConversion((const char*)PeekGet(nOffset), &nLength);
                    nOffset += nLength;
                }
            } while(true);

            return bActualSize ? nLen : nOffset - nActualStart + pConv->GetDelimiterLength() + 1;
        }


        //-----------------------------------------------------------------------------
        // Reads a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetString(char* pString, int nMaxChars)
        {
            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekStringLength();

            if(IsText()) {
                EatWhiteSpace();
            }

            if(nLen == 0) {
                *pString = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pString, nLen - 1);
                pString[nLen - 1] = 0;
            } else {
                Get(pString, nMaxChars - 1);
                pString[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }

            // Read the terminating NULL in binary formats
            if(!IsText()) {
                assert(GetChar() == 0);
            }
        }


        //-----------------------------------------------------------------------------
        // Reads up to and including the first \n
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetLine(char* pLine, int nMaxChars)
        {
            assert(IsText() && !ContainsCRLF());

            if(!IsValid()) {
                *pLine = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekLineLength();
            if(nLen == 0) {
                *pLine = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pLine, nLen - 1);
                pLine[nLen - 1] = 0;
            } else {
                Get(pLine, nMaxChars - 1);
                pLine[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }
        }


        //-----------------------------------------------------------------------------
        // This version of GetString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        char CUtlBuffer::GetDelimitedCharInternal(CUtlCharConversion *pConv)
        {
            char c = GetChar();
            if(c == pConv->GetEscapeChar()) {
                int nLength = pConv->MaxConversionLength();
                if(!CheckArbitraryPeekGet(0, nLength))
                    return '\0';

                c = pConv->FindConversion((const char *)PeekGet(), &nLength);
                SeekGet(SEEK_CURRENT, nLength);
            }

            return c;
        }

        char CUtlBuffer::GetDelimitedChar(CUtlCharConversion *pConv)
        {
            if(!IsText() || !pConv)
                return GetChar();
            return GetDelimitedCharInternal(pConv);
        }

        void CUtlBuffer::GetDelimitedString(CUtlCharConversion *pConv, char *pString, int nMaxChars)
        {
            if(!IsText() || !pConv) {
                GetString(pString, nMaxChars);
                return;
            }

            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            EatWhiteSpace();
            if(!PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return;

            // Pull off the starting delimiter
            SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());

            int nRead = 0;
            while(IsValid()) {
                if(PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength())) {
                    SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());
                    break;
                }

                char c = GetDelimitedCharInternal(pConv);

                if(nRead < nMaxChars) {
                    pString[nRead] = c;
                    ++nRead;
                }
            }

            if(nRead >= nMaxChars) {
                nRead = nMaxChars - 1;
            }
            pString[nRead] = '\0';
        }


        //-----------------------------------------------------------------------------
        // Checks if a Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckGet(int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            if(TellMaxPut() < m_Get + nSize) {
                m_Error |= GET_OVERFLOW;
                return false;
            }

            if((m_Get < m_nOffset) || (m_Memory.NumAllocated() < m_Get - m_nOffset + nSize)) {
                if(!OnGetOverflow(nSize)) {
                    m_Error |= GET_OVERFLOW;
                    return false;
                }
            }

            return true;
        }


        //-----------------------------------------------------------------------------
        // Checks if a peek Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPeekGet(int nOffset, int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            // Checking for peek can't Set the overflow flag
            bool bOk = CheckGet(nOffset + nSize);
            m_Error &= ~GET_OVERFLOW;
            return bOk;
        }


        //-----------------------------------------------------------------------------
        // Call this to peek arbitrarily long into memory. It doesn't fail unless
        // it can't read *anything* new
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckArbitraryPeekGet(int nOffset, int &nIncrement)
        {
            if(TellGet() + nOffset >= TellMaxPut()) {
                nIncrement = 0;
                return false;
            }

            if(TellGet() + nOffset + nIncrement > TellMaxPut()) {
                nIncrement = TellMaxPut() - TellGet() - nOffset;
            }

            // NOTE: CheckPeekGet could modify TellMaxPut for streaming files
            // We have to call TellMaxPut again here
            CheckPeekGet(nOffset, nIncrement);
            int nMaxGet = TellMaxPut() - TellGet();
            if(nMaxGet < nIncrement) {
                nIncrement = nMaxGet;
            }
            return (nIncrement != 0);
        }


        //-----------------------------------------------------------------------------
        // Peek part of the butt
        //-----------------------------------------------------------------------------
        const void* CUtlBuffer::PeekGet(int nMaxSize, int nOffset)
        {
            if(!CheckPeekGet(nOffset, nMaxSize))
                return NULL;
            return &m_Memory[m_Get + nOffset - m_nOffset];
        }


        //-----------------------------------------------------------------------------
        // Change where I'm reading
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SeekGet(SeekType_t type, int offset)
        {
            switch(type) {
                case SEEK_HEAD:
                    m_Get = offset;
                    break;

                case SEEK_CURRENT:
                    m_Get += offset;
                    break;

                case SEEK_TAIL:
                    m_Get = m_nMaxPut - offset;
                    break;
            }

            if(m_Get > m_nMaxPut) {
                m_Error |= GET_OVERFLOW;
            } else {
                m_Error &= ~GET_OVERFLOW;
                if(m_Get < m_nOffset || m_Get >= m_nOffset + Size()) {
                    OnGetOverflow(-1);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Parse...
        //-----------------------------------------------------------------------------

#pragma warning ( disable : 4706 )

        int CUtlBuffer::VaScanf(const char* pFmt, va_list list)
        {
            assert(pFmt);
            if(m_Error || !IsText())
                return 0;

            int numScanned = 0;
            int nLength;
            char c;
            char* pEnd;
            while(c = *pFmt++) {
                // Stop if we hit the end of the buffer
                if(m_Get >= TellMaxPut()) {
                    m_Error |= GET_OVERFLOW;
                    break;
                }

                switch(c) {
                    case ' ':
                        // eat all whitespace
                        EatWhiteSpace();
                        break;

                    case '%':
                    {
                        // Conversion character... try to convert baby!
                        char type = *pFmt++;
                        if(type == 0)
                            return numScanned;

                        switch(type) {
                            case 'c':
                            {
                                char* ch = va_arg(list, char *);
                                if(CheckPeekGet(0, sizeof(char))) {
                                    *ch = *(const char*)PeekGet();
                                    ++m_Get;
                                } else {
                                    *ch = 0;
                                    return numScanned;
                                }
                            }
                            break;

                            case 'i':
                            case 'd':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'x':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 16);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'u':
                            {
                                unsigned int* u = va_arg(list, unsigned int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *u = 0;
                                    return numScanned;
                                }

                                *u = strtoul((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'f':
                            {
                                float* f = va_arg(list, float *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *f = 0.0f;
                                    return numScanned;
                                }

                                *f = (float)strtod((char*)PeekGet(), &pEnd);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 's':
                            {
                                char* s = va_arg(list, char *);
                                GetString(s);
                            }
                            break;

                            default:
                            {
                                // unimplemented scanf type
                                assert(0);
                                return numScanned;
                            }
                            break;
                        }

                        ++numScanned;
                    }
                    break;

                    default:
                    {
                        // Here we have to match the format string character
                        // against what's in the buffer or we're done.
                        if(!CheckPeekGet(0, sizeof(char)))
                            return numScanned;

                        if(c != *(const char*)PeekGet())
                            return numScanned;

                        ++m_Get;
                    }
                }
            }
            return numScanned;
        }

#pragma warning ( default : 4706 )

        int CUtlBuffer::Scanf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            int count = VaScanf(pFmt, args);
            va_end(args);

            return count;
        }


        //-----------------------------------------------------------------------------
        // Advance the Get index until after the particular string is found
        // Do not eat whitespace before starting. Return false if it failed
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::GetToken(const char *pToken)
        {
            assert(pToken);

            // Look for the token
            int nLen = strlen(pToken);

            int nSizeToCheck = Size() - TellGet() - m_nOffset;

            int nGet = TellGet();
            do {
                int nMaxSize = TellMaxPut() - TellGet();
                if(nMaxSize < nSizeToCheck) {
                    nSizeToCheck = nMaxSize;
                }
                if(nLen > nSizeToCheck)
                    break;

                if(!CheckPeekGet(0, nSizeToCheck))
                    break;

                const char *pBufStart = (const char*)PeekGet();
                const char *pFoundEnd = V_strnistr(pBufStart, pToken, nSizeToCheck);
                if(pFoundEnd) {
                    size_t nOffset = (size_t)pFoundEnd - (size_t)pBufStart;
                    SeekGet(CUtlBuffer::SEEK_CURRENT, nOffset + nLen);
                    return true;
                }

                SeekGet(CUtlBuffer::SEEK_CURRENT, nSizeToCheck - nLen - 1);
                nSizeToCheck = Size() - (nLen - 1);

            } while(true);

            SeekGet(CUtlBuffer::SEEK_HEAD, nGet);
            return false;
        }


        //-----------------------------------------------------------------------------
        // (For text buffers only)
        // Parse a token from the buffer:
        // Grab all text that lies between a starting delimiter + ending delimiter
        // (skipping whitespace that leads + trails both delimiters).
        // Note the delimiter checks are case-insensitive.
        // If successful, the Get index is advanced and the function returns true,
        // otherwise the index is not advanced and the function returns false.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ParseToken(const char *pStartingDelim, const char *pEndingDelim, char* pString, int nMaxLen)
        {
            int nCharsToCopy = 0;
            int nCurrentGet = 0;

            size_t nEndingDelimLen;

            // Starting delimiter is optional
            char emptyBuf = '\0';
            if(!pStartingDelim) {
                pStartingDelim = &emptyBuf;
            }

            // Ending delimiter is not
            assert(pEndingDelim && pEndingDelim[0]);
            nEndingDelimLen = strlen(pEndingDelim);

            int nStartGet = TellGet();
            char nCurrChar;
            int nTokenStart = -1;
            EatWhiteSpace();
            while(*pStartingDelim) {
                nCurrChar = *pStartingDelim++;
                if(!isspace((unsigned char)nCurrChar)) {
                    if(tolower(GetChar()) != tolower(nCurrChar))
                        goto parseFailed;
                } else {
                    EatWhiteSpace();
                }
            }

            EatWhiteSpace();
            nTokenStart = TellGet();
            if(!GetToken(pEndingDelim))
                goto parseFailed;

            nCurrentGet = TellGet();
            nCharsToCopy = (nCurrentGet - nEndingDelimLen) - nTokenStart;
            if(nCharsToCopy >= nMaxLen) {
                nCharsToCopy = nMaxLen - 1;
            }

            if(nCharsToCopy > 0) {
                SeekGet(CUtlBuffer::SEEK_HEAD, nTokenStart);
                Get(pString, nCharsToCopy);
                if(!IsValid())
                    goto parseFailed;

                // Eat trailing whitespace
                for(; nCharsToCopy > 0; --nCharsToCopy) {
                    if(!isspace((unsigned char)pString[nCharsToCopy - 1]))
                        break;
                }
            }
            pString[nCharsToCopy] = '\0';

            // Advance the Get index
            SeekGet(CUtlBuffer::SEEK_HEAD, nCurrentGet);
            return true;

        parseFailed:
            // Revert the Get index
            SeekGet(SEEK_HEAD, nStartGet);
            pString[0] = '\0';
            return false;
        }


        //-----------------------------------------------------------------------------
        // Parses the next token, given a Set of character breaks to stop at
        //-----------------------------------------------------------------------------
        int CUtlBuffer::ParseToken(characterset_t *pBreaks, char *pTokenBuf, int nMaxLen, bool bParseComments)
        {
            assert(nMaxLen > 0);
            pTokenBuf[0] = 0;

            // skip whitespace + comments
            while(true) {
                if(!IsValid())
                    return -1;
                EatWhiteSpace();
                if(bParseComments) {
                    if(!EatCPPComment())
                        break;
                } else {
                    break;
                }
            }

            char c = GetChar();

            // End of buffer
            if(c == 0)
                return -1;

            // handle quoted strings specially
            if(c == '\"') {
                int nLen = 0;
                while(IsValid()) {
                    c = GetChar();
                    if(c == '\"' || !c) {
                        pTokenBuf[nLen] = 0;
                        return nLen;
                    }
                    pTokenBuf[nLen] = c;
                    if(++nLen == nMaxLen) {
                        pTokenBuf[nLen - 1] = 0;
                        return nMaxLen;
                    }
                }

                // In this case, we hit the end of the buffer before hitting the end qoute
                pTokenBuf[nLen] = 0;
                return nLen;
            }

            // parse single characters
            if(IN_CHARACTERSET(*pBreaks, c)) {
                pTokenBuf[0] = c;
                pTokenBuf[1] = 0;
                return 1;
            }

            // parse a regular word
            int nLen = 0;
            while(true) {
                pTokenBuf[nLen] = c;
                if(++nLen == nMaxLen) {
                    pTokenBuf[nLen - 1] = 0;
                    return nMaxLen;
                }
                c = GetChar();
                if(!IsValid())
                    break;

                if(IN_CHARACTERSET(*pBreaks, c) || c == '\"' || c <= ' ') {
                    SeekGet(SEEK_CURRENT, -1);
                    break;
                }
            }

            pTokenBuf[nLen] = 0;
            return nLen;
        }



        //-----------------------------------------------------------------------------
        // Serialization
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Put(const void *pMem, int size)
        {
            if(size && CheckPut(size)) {
                memcpy(&m_Memory[m_Put - m_nOffset], pMem, size);
                m_Put += size;

                AddNullTermination();
            }
        }


        //-----------------------------------------------------------------------------
        // Writes a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::PutString(const char* pString)
        {
            if(!IsText()) {
                if(pString) {
                    // Not text? append a null at the end.
                    size_t nLen = strlen(pString) + 1;
                    Put(pString, nLen * sizeof(char));
                    return;
                } else {
                    PutTypeBin<char>(0);
                }
            } else if(pString) {
                int nTabCount = (m_Flags & AUTO_TABS_DISABLED) ? 0 : m_nTab;
                if(nTabCount > 0) {
                    if(WasLastCharacterCR()) {
                        PutTabs();
                    }

                    const char* pEndl = strchr(pString, '\n');
                    while(pEndl) {
                        size_t nSize = (size_t)pEndl - (size_t)pString + sizeof(char);
                        Put(pString, nSize);
                        pString = pEndl + 1;
                        if(*pString) {
                            PutTabs();
                            pEndl = strchr(pString, '\n');
                        } else {
                            pEndl = NULL;
                        }
                    }
                }
                size_t nLen = strlen(pString);
                if(nLen) {
                    Put(pString, nLen * sizeof(char));
                }
            }
        }


        //-----------------------------------------------------------------------------
        // This version of PutString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        inline void CUtlBuffer::PutDelimitedCharInternal(CUtlCharConversion *pConv, char c)
        {
            int l = pConv->GetConversionLength(c);
            if(l == 0) {
                PutChar(c);
            } else {
                PutChar(pConv->GetEscapeChar());
                Put(pConv->GetConversionString(c), l);
            }
        }

        void CUtlBuffer::PutDelimitedChar(CUtlCharConversion *pConv, char c)
        {
            if(!IsText() || !pConv) {
                PutChar(c);
                return;
            }

            PutDelimitedCharInternal(pConv, c);
        }

        void CUtlBuffer::PutDelimitedString(CUtlCharConversion *pConv, const char *pString)
        {
            if(!IsText() || !pConv) {
                PutString(pString);
                return;
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());

            int nLen = pString ? strlen(pString) : 0;
            for(int i = 0; i < nLen; ++i) {
                PutDelimitedCharInternal(pConv, pString[i]);
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());
        }


        void CUtlBuffer::VaPrintf(const char* pFmt, va_list list)
        {
            char temp[2048];
            int nLen = vsnprintf(temp, sizeof(temp), pFmt, list);
            assert(nLen < 2048);
            PutString(temp);
        }

        void CUtlBuffer::Printf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            VaPrintf(pFmt, args);
            va_end(args);
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc)
        {
            m_GetOverflowFunc = getFunc;
            m_PutOverflowFunc = putFunc;
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::OnPutOverflow(int nSize)
        {
            return (this->*m_PutOverflowFunc)(nSize);
        }

        bool CUtlBuffer::OnGetOverflow(int nSize)
        {
            return (this->*m_GetOverflowFunc)(nSize);
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PutOverflow(int nSize)
        {
            if(m_Memory.IsExternallyAllocated()) {
                if(!IsGrowable())
                    return false;

                m_Memory.ConvertToGrowableMemory(0);
            }

            while(Size() < m_Put - m_nOffset + nSize) {
                m_Memory.Grow();
            }

            return true;
        }

        bool CUtlBuffer::GetOverflow(int nSize)
        {
            return false;
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPut(int nSize)
        {
            if((m_Error & PUT_OVERFLOW) || IsReadOnly())
                return false;

            if((m_Put < m_nOffset) || (m_Memory.NumAllocated() < m_Put - m_nOffset + nSize)) {
                if(!OnPutOverflow(nSize)) {
                    m_Error |= PUT_OVERFLOW;
                    return false;
                }
            }
            return true;
        }

        void CUtlBuffer::SeekPut(SeekType_t type, int offset)
        {
            int nNextPut = m_Put;
            switch(type) {
                case SEEK_HEAD:
                    nNextPut = offset;
                    break;

                case SEEK_CURRENT:
                    nNextPut += offset;
                    break;

                case SEEK_TAIL:
                    nNextPut = m_nMaxPut - offset;
                    break;
            }

            // Force a write of the data
            // FIXME: We could make this more optimal potentially by writing out
            // the entire buffer if you seek outside the current range

            // NOTE: This call will write and will also seek the file to nNextPut.
            OnPutOverflow(-nNextPut - 1);
            m_Put = nNextPut;

            AddNullTermination();
        }


        void CUtlBuffer::ActivateByteSwapping(bool bActivate)
        {
            m_Byteswap.ActivateByteSwapping(bActivate);
        }

        void CUtlBuffer::SetBigEndian(bool bigEndian)
        {
            m_Byteswap.SetTargetBigEndian(bigEndian);
        }

        bool CUtlBuffer::IsBigEndian(void)
        {
            return m_Byteswap.IsTargetBigEndian();
        }


        //-----------------------------------------------------------------------------
        // null terminate the buffer
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AddNullTermination(void)
        {
            if(m_Put > m_nMaxPut) {
                if(!IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)) {
                    // Add null termination value
                    if(CheckPut(1)) {
                        m_Memory[m_Put - m_nOffset] = 0;
                    } else {
                        // Restore the overflow state, it was valid before...
                        m_Error &= ~PUT_OVERFLOW;
                    }
                }
                m_nMaxPut = m_Put;
            }
        }


        //-----------------------------------------------------------------------------
        // Converts a buffer from a CRLF buffer to a CR buffer (and back)
        // Returns false if no conversion was necessary (and outBuf is left untouched)
        // If the conversion occurs, outBuf will be cleared.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ConvertCRLF(CUtlBuffer &outBuf)
        {
            if(!IsText() || !outBuf.IsText())
                return false;

            if(ContainsCRLF() == outBuf.ContainsCRLF())
                return false;

            int nInCount = TellMaxPut();

            outBuf.Purge();
            outBuf.EnsureCapacity(nInCount);

            bool bFromCRLF = ContainsCRLF();

            // Start reading from the beginning
            int nGet = TellGet();
            int nPut = TellPut();
            int nGetDelta = 0;
            int nPutDelta = 0;

            const char *pBase = (const char*)Base();
            int nCurrGet = 0;
            while(nCurrGet < nInCount) {
                const char *pCurr = &pBase[nCurrGet];
                if(bFromCRLF) {
                    const char *pNext = V_strnistr(pCurr, "\r\n", nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 2;
                    if(nGet >= nCurrGet - 1) {
                        --nGetDelta;
                    }
                    if(nPut >= nCurrGet - 1) {
                        --nPutDelta;
                    }
                } else {
                    const char *pNext = V_strnchr(pCurr, '\n', nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\r');
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 1;
                    if(nGet >= nCurrGet) {
                        ++nGetDelta;
                    }
                    if(nPut >= nCurrGet) {
                        ++nPutDelta;
                    }
                }
            }

            assert(nPut + nPutDelta <= outBuf.TellMaxPut());

            outBuf.SeekGet(SEEK_HEAD, nGet + nGetDelta);
            outBuf.SeekPut(SEEK_HEAD, nPut + nPutDelta);

            return true;
        }


        //---------------------------------------------------------------------------
        // Implementation of CUtlInplaceBuffer
        //---------------------------------------------------------------------------

        CUtlInplaceBuffer::CUtlInplaceBuffer(int growSize /* = 0 */, int initSize /* = 0 */, int nFlags /* = 0 */) :
            CUtlBuffer(growSize, initSize, nFlags)
        {
            NULL;
        }

        bool CUtlInplaceBuffer::InplaceGetLinePtr(char **ppszInBufferPtr, int *pnLineLength)
        {
            assert(IsText() && !ContainsCRLF());

            int nLineLen = PeekLineLength();
            if(nLineLen <= 1) {
                SeekGet(SEEK_TAIL, 0);
                return false;
            }

            --nLineLen; // because it accounts for putting a terminating null-character

            char *pszLine = (char *) const_cast< void * >(PeekGet());
            SeekGet(SEEK_CURRENT, nLineLen);

            // Set the out args
            if(ppszInBufferPtr)
                *ppszInBufferPtr = pszLine;

            if(pnLineLength)
                *pnLineLength = nLineLen;

            return true;
        }

        char * CUtlInplaceBuffer::InplaceGetLinePtr(void)
        {
            char *pszLine = NULL;
            int nLineLen = 0;

            if(InplaceGetLinePtr(&pszLine, &nLineLen)) {
                assert(nLineLen >= 1);

                switch(pszLine[nLineLen - 1]) {
                    case '\n':
                    case '\r':
                        pszLine[nLineLen - 1] = 0;
                        if(--nLineLen) {
                            switch(pszLine[nLineLen - 1]) {
                                case '\n':
                                case '\r':
                                    pszLine[nLineLen - 1] = 0;
                                    break;
                            }
                        }
                        break;

                    default:
                        assert(pszLine[nLineLen] == 0);
                        break;
                }
            }
            return pszLine;
        }
// Junk Code By Troll Face & Thaisen's Gen
void mLGHHKZMlbZawtkajtDtPGePGvfWMogdYyVVbiHZBdoot33665609() {     int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq45534233 = -344055371;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47265923 = -907472512;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq27028430 = -171430332;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq10794543 = -767159429;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq68898806 = -532763074;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81023238 = -657359595;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq78314903 = -942966607;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq80028814 = -680343096;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq74483515 = 49606482;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq56306255 = 11678729;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq58430113 = -511615934;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97583706 = -234143362;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47439021 = -861627383;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53579631 = -994955626;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95429996 = -105498122;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq67150503 = -221903908;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq23764472 = -455496768;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97563601 = -416362562;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq7157500 = -985704965;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq66229487 = -441937704;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq46114382 = 96874708;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19162781 = -867712477;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq24024849 = -127450021;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81290565 = -817908488;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq12732055 = -97918029;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq57798652 = -209738509;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28746361 = -846074782;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq13540061 = -544735464;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq58037123 = -846811589;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37090344 = -945657357;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq12341870 = -262580060;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq11088685 = -96061863;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq42086891 = -701074737;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq27833762 = -643952733;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81698655 = -582301635;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq18331810 = -764927666;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq20523207 = -931620105;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53916879 = 35179463;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq83638035 = -708965761;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq72754079 = -945610552;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq63032045 = -31487450;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq50533948 = -795775901;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37189879 = -471429035;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq70407105 = -468570850;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq48132030 = -879784842;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq15128146 = -719858551;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28016850 = -315733848;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47205491 = -390646071;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98265511 = 3414810;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq89103380 = -452846876;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq996721 = -463674008;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19110420 = -169672613;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq63463139 = 83468743;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98616213 = -167192381;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq87551227 = -988178087;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq26371453 = -476342895;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq23241075 = -680022492;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq45737864 = -353521845;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98062488 = -569241400;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq11100155 = -223024565;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq52276878 = -811284813;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq64774842 = -298231144;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq21991692 = -833531508;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37393172 = -4736161;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq43964386 = -725741212;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47341428 = -315554072;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55496816 = -533068625;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19605260 = -117674650;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq71880976 = -312653992;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq77098187 = -340570456;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq46627297 = -290283803;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq69847593 = -390676232;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq13925566 = -707396802;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq34403421 = 59905586;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq3197443 = -310450254;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95580433 = -107349391;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81972901 = -296283442;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53617744 = -658879171;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq33158536 = -938123646;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97603908 = -378059478;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq29781802 = -894004662;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81540869 = -355428712;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq15274549 = -448150274;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq68933742 = -293964714;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq36093623 = -381983350;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq93231449 = 7092553;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47625546 = -79530606;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq43470677 = -433882357;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq40282535 = -655774647;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55327202 = -5958741;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95090735 = 15094825;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq74785343 = -478098261;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55854391 = -395579137;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq72537881 = -385941197;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq20477202 = -34325739;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98257202 = -733256307;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28542257 = -962244394;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq99796707 = -366692875;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq26442720 = -742829639;    int EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq790602 = -344055371;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq45534233 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47265923;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47265923 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq27028430;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq27028430 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq10794543;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq10794543 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq68898806;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq68898806 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81023238;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81023238 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq78314903;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq78314903 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq80028814;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq80028814 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq74483515;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq74483515 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq56306255;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq56306255 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq58430113;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq58430113 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97583706;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97583706 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47439021;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47439021 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53579631;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53579631 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95429996;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95429996 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq67150503;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq67150503 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq23764472;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq23764472 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97563601;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97563601 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq7157500;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq7157500 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq66229487;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq66229487 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq46114382;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq46114382 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19162781;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19162781 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq24024849;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq24024849 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81290565;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81290565 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq12732055;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq12732055 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq57798652;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq57798652 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28746361;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28746361 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq13540061;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq13540061 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq58037123;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq58037123 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37090344;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37090344 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq12341870;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq12341870 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq11088685;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq11088685 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq42086891;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq42086891 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq27833762;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq27833762 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81698655;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81698655 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq18331810;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq18331810 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq20523207;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq20523207 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53916879;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53916879 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq83638035;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq83638035 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq72754079;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq72754079 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq63032045;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq63032045 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq50533948;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq50533948 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37189879;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37189879 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq70407105;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq70407105 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq48132030;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq48132030 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq15128146;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq15128146 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28016850;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28016850 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47205491;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47205491 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98265511;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98265511 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq89103380;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq89103380 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq996721;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq996721 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19110420;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19110420 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq63463139;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq63463139 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98616213;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98616213 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq87551227;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq87551227 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq26371453;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq26371453 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq23241075;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq23241075 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq45737864;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq45737864 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98062488;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98062488 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq11100155;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq11100155 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq52276878;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq52276878 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq64774842;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq64774842 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq21991692;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq21991692 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37393172;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq37393172 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq43964386;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq43964386 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47341428;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47341428 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55496816;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55496816 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19605260;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq19605260 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq71880976;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq71880976 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq77098187;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq77098187 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq46627297;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq46627297 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq69847593;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq69847593 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq13925566;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq13925566 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq34403421;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq34403421 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq3197443;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq3197443 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95580433;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95580433 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81972901;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81972901 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53617744;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq53617744 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq33158536;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq33158536 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97603908;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq97603908 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq29781802;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq29781802 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81540869;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq81540869 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq15274549;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq15274549 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq68933742;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq68933742 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq36093623;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq36093623 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq93231449;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq93231449 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47625546;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq47625546 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq43470677;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq43470677 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq40282535;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq40282535 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55327202;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55327202 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95090735;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq95090735 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq74785343;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq74785343 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55854391;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq55854391 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq72537881;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq72537881 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq20477202;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq20477202 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98257202;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq98257202 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28542257;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq28542257 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq99796707;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq99796707 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq26442720;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq26442720 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq790602;     EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq790602 = EWUGChpsOXghqBLeDAwvvBxYPyAmdslRpWBZLPJGq45534233;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DIfowxtCAwtKkqsvHeyXYHYqdejkaCiPIPtgmUqYKgHixxKwJjJboxGwkpLRNJcyOdBUwMJnnZNszpsgUnbq90713263() {     float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47878577 = -857925701;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63568338 = -808086051;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM7888527 = 47287830;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM1142331 = -336180590;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM41846774 = -486274554;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM31657629 = -327567202;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM15324613 = -669783201;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5462287 = -150981939;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34430604 = -428466660;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34620028 = -958163929;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5604027 = -673591444;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM31708492 = 58760202;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM37835676 = -457791607;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42896389 = -720476274;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM97919494 = -778325509;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM10752401 = -150271991;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM95030557 = -398968660;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20526953 = -632936190;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM92802248 = -508609263;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM74838472 = -33682980;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM10316731 = -794691224;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM36027074 = -641648253;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20640842 = -624339521;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM66321681 = -312844145;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM68123266 = -904555659;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM17468160 = -713437716;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73187206 = -58927465;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM51433499 = -58900894;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM83206749 = -918951849;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM62335372 = -743579918;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM83822068 = -283621566;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM67486486 = -995334321;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM84108886 = -741387180;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42040628 = -380191753;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM46139589 = 41611678;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM16085117 = -45423676;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63134064 = -53841194;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21248787 = -462048494;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM29675552 = -535978353;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21952329 = -270775985;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM57709335 = -637756260;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM33723478 = -535033897;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM53849077 = -306289436;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73964272 = -363751024;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM74046508 = -949239612;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM60515450 = -154026385;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM13241771 = -204968978;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM91964784 = -308996781;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42181597 = -63408191;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM94320649 = -51427040;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34912026 = -569030913;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20871110 = -140916461;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM48033380 = -592098445;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM69086713 = -611487581;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM80396532 = -810090123;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM11851503 = -116277449;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42927497 = -83746531;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM41566845 = -639868025;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM33019064 = -431624931;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM24378615 = -772836838;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM58470422 = -168639738;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63891114 = -510882308;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM22255537 = -232030090;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM72095232 = -684886743;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM50797960 = -574542364;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM38117540 = -678257123;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47599605 = -199852618;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM95795048 = 22400146;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM96756800 = -662087953;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81834378 = -632901834;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47618336 = 3569203;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73781771 = -936920166;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM90851400 = 3042162;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM70849920 = -137833278;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM17129138 = -395926721;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM76593253 = -159657328;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM82177997 = -235358818;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM46676569 = -160588497;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM92275172 = -363604534;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM7607817 = -650529275;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM4226389 = -408468738;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81222421 = -749930684;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM9251902 = -995492703;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM88886099 = -767524809;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM27423347 = -74549005;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM62950958 = -42705105;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM19453106 = -303235876;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM15022174 = -29899600;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM61644095 = -570101631;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34288086 = -842110874;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73157620 = -961677145;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21567220 = -413973170;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM88229722 = 69576436;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5296937 = -763141515;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63481906 = -2136247;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM93818220 = -26873952;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM11467941 = -203003807;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81753845 = -621402693;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM23166313 = -789208661;    float sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM35928969 = -857925701;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47878577 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63568338;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63568338 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM7888527;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM7888527 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM1142331;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM1142331 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM41846774;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM41846774 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM31657629;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM31657629 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM15324613;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM15324613 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5462287;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5462287 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34430604;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34430604 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34620028;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34620028 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5604027;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5604027 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM31708492;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM31708492 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM37835676;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM37835676 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42896389;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42896389 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM97919494;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM97919494 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM10752401;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM10752401 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM95030557;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM95030557 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20526953;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20526953 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM92802248;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM92802248 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM74838472;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM74838472 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM10316731;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM10316731 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM36027074;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM36027074 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20640842;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20640842 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM66321681;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM66321681 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM68123266;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM68123266 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM17468160;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM17468160 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73187206;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73187206 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM51433499;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM51433499 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM83206749;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM83206749 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM62335372;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM62335372 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM83822068;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM83822068 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM67486486;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM67486486 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM84108886;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM84108886 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42040628;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42040628 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM46139589;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM46139589 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM16085117;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM16085117 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63134064;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63134064 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21248787;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21248787 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM29675552;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM29675552 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21952329;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21952329 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM57709335;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM57709335 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM33723478;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM33723478 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM53849077;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM53849077 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73964272;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73964272 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM74046508;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM74046508 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM60515450;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM60515450 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM13241771;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM13241771 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM91964784;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM91964784 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42181597;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42181597 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM94320649;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM94320649 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34912026;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34912026 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20871110;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM20871110 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM48033380;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM48033380 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM69086713;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM69086713 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM80396532;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM80396532 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM11851503;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM11851503 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42927497;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM42927497 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM41566845;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM41566845 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM33019064;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM33019064 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM24378615;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM24378615 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM58470422;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM58470422 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63891114;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63891114 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM22255537;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM22255537 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM72095232;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM72095232 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM50797960;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM50797960 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM38117540;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM38117540 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47599605;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47599605 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM95795048;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM95795048 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM96756800;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM96756800 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81834378;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81834378 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47618336;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47618336 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73781771;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73781771 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM90851400;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM90851400 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM70849920;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM70849920 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM17129138;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM17129138 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM76593253;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM76593253 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM82177997;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM82177997 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM46676569;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM46676569 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM92275172;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM92275172 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM7607817;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM7607817 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM4226389;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM4226389 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81222421;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81222421 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM9251902;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM9251902 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM88886099;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM88886099 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM27423347;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM27423347 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM62950958;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM62950958 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM19453106;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM19453106 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM15022174;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM15022174 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM61644095;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM61644095 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34288086;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM34288086 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73157620;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM73157620 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21567220;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM21567220 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM88229722;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM88229722 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5296937;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM5296937 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63481906;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM63481906 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM93818220;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM93818220 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM11467941;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM11467941 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81753845;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM81753845 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM23166313;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM23166313 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM35928969;     sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM35928969 = sGbLJxvxJZLAEJTTwpAxaVnOjNwvxexahlFICpLyQMeONMnqVYkfmeQXSMcvBAzeclSTELQGdBgNrZAXaFaYuRnDUhqqNPxQM47878577;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void NPYrHvIOPJfyafrQLoOAgXpviwVlbLkoyAbTgRuxnIVqhgMzvBp73496033() {     long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph24634408 = -233025003;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph52759768 = -474519381;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph6526054 = -460096778;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph38308119 = -358149270;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49872789 = -627041415;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph82685458 = -792510212;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31287147 = -216596822;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph50492659 = -193027576;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph10418482 = -817138794;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph92095175 = -861848499;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48604688 = -504875104;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph37661460 = -172366518;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph67578009 = -850536703;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph17575540 = -644049155;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph66726158 = -443963382;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph33915530 = -358846422;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph94424906 = -563928042;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph83874665 = 18231727;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph77196412 = -657530835;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph58322872 = 42998893;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph13344527 = 20209432;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph40536395 = -800847041;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31932236 = -756615836;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph71463809 = -989391084;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph32832684 = -707044087;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph5935570 = -506789341;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph20350934 = -956366868;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43129899 = -747887592;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph6838387 = -527833400;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph22302845 = -470308603;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49961971 = -719562659;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph79056283 = -716721822;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47670043 = -984850591;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2801842 = -548626668;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph50354553 = -962696056;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph30460668 = -498207824;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph12070221 = -395377028;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47966847 = -8192969;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph52446287 = -810993319;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48303350 = -580686943;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph99163178 = -162936829;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph65752247 = -962149409;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph1449736 = -392108580;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph86344110 = -668997820;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49989137 = -766302964;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph59716417 = -621668998;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph12547134 = -302375559;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph16303098 = -888545321;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph94615557 = -362436871;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph55750635 = -695496500;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph65320768 = -215006720;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2646849 = -675154000;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2254192 = -95794091;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph75783440 = -456678925;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph60151506 = -35361375;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph84098013 = -432177962;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph20827533 = -717903545;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph35062245 = -470705694;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph5475436 = -651105184;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43937220 = -20252075;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph62334525 = -836143344;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph88157247 = -468709231;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43654272 = -665194176;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph88115637 = -246830192;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42133204 = -42285841;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph69548404 = -788153282;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph89991416 = -187515927;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph64776168 = -201910035;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph67220986 = -681353100;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph36265491 = -945755559;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph21845309 = -963469395;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph46458060 = -455735074;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31428378 = -170774954;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph28893062 = 23156108;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph59159694 = -794064278;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47592279 = -17641159;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph39086659 = -308738462;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph45588125 = 12381984;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph21474672 = -123088121;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph73116267 = 14624911;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph93388436 = -104413782;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph4047836 = 32178452;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48514342 = -285450721;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph51087752 = -832336901;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph56982077 = -155301884;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47315123 = 55591340;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph76802092 = -520927732;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph71886602 = -428171667;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42650335 = -413265293;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph66256539 = -430518094;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph9633135 = -780304279;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph77007976 = -924671334;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42491411 = -357087786;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph8509068 = -690741244;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph85968825 = -744543599;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph11005931 = -694227599;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph22097976 = -196955233;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph13334098 = -45278389;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph44210906 = -526711980;    long VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph80440732 = -233025003;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph24634408 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph52759768;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph52759768 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph6526054;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph6526054 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph38308119;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph38308119 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49872789;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49872789 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph82685458;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph82685458 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31287147;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31287147 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph50492659;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph50492659 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph10418482;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph10418482 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph92095175;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph92095175 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48604688;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48604688 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph37661460;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph37661460 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph67578009;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph67578009 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph17575540;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph17575540 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph66726158;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph66726158 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph33915530;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph33915530 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph94424906;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph94424906 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph83874665;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph83874665 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph77196412;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph77196412 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph58322872;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph58322872 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph13344527;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph13344527 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph40536395;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph40536395 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31932236;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31932236 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph71463809;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph71463809 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph32832684;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph32832684 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph5935570;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph5935570 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph20350934;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph20350934 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43129899;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43129899 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph6838387;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph6838387 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph22302845;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph22302845 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49961971;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49961971 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph79056283;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph79056283 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47670043;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47670043 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2801842;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2801842 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph50354553;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph50354553 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph30460668;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph30460668 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph12070221;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph12070221 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47966847;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47966847 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph52446287;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph52446287 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48303350;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48303350 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph99163178;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph99163178 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph65752247;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph65752247 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph1449736;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph1449736 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph86344110;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph86344110 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49989137;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph49989137 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph59716417;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph59716417 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph12547134;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph12547134 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph16303098;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph16303098 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph94615557;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph94615557 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph55750635;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph55750635 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph65320768;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph65320768 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2646849;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2646849 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2254192;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph2254192 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph75783440;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph75783440 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph60151506;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph60151506 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph84098013;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph84098013 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph20827533;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph20827533 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph35062245;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph35062245 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph5475436;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph5475436 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43937220;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43937220 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph62334525;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph62334525 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph88157247;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph88157247 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43654272;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph43654272 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph88115637;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph88115637 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42133204;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42133204 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph69548404;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph69548404 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph89991416;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph89991416 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph64776168;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph64776168 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph67220986;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph67220986 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph36265491;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph36265491 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph21845309;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph21845309 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph46458060;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph46458060 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31428378;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph31428378 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph28893062;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph28893062 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph59159694;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph59159694 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47592279;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47592279 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph39086659;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph39086659 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph45588125;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph45588125 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph21474672;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph21474672 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph73116267;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph73116267 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph93388436;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph93388436 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph4047836;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph4047836 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48514342;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph48514342 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph51087752;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph51087752 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph56982077;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph56982077 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47315123;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph47315123 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph76802092;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph76802092 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph71886602;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph71886602 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42650335;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42650335 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph66256539;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph66256539 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph9633135;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph9633135 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph77007976;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph77007976 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42491411;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph42491411 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph8509068;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph8509068 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph85968825;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph85968825 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph11005931;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph11005931 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph22097976;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph22097976 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph13334098;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph13334098 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph44210906;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph44210906 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph80440732;     VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph80440732 = VBTdqacJKZWnDZrzZDYkutRMXByOBkWqZWPmfXksDHkXRVSRph24634408;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void XYLkWlSHTRYQprMOuJmbbwHrVjdISPECKKTLdMnNWcVuswRexPHvCRlbRUPnHKpDMVFfXAliWZEybMyZSzKDHJAsZs67518860() {     double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB94705000 = -916295856;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB44984381 = -886706291;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB56735994 = -52776460;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57307515 = -848016378;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB55383442 = -867529987;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68834000 = -993608173;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB7899228 = -268044517;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB91598936 = -437872163;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB62494455 = -846200677;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB76180061 = -932438092;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB75029771 = 2409485;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB79216157 = -779262531;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB93018442 = -183536417;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10384839 = -530328034;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB20898969 = -799859721;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB6570352 = -271518134;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB71535339 = -130694495;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68218378 = -983591750;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB23330461 = -647707975;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB99887347 = -68039142;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB23925020 = -5227036;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB48259584 = -323096906;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB32250117 = -201800349;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25216452 = -130199305;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB56454217 = -89021255;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB29241202 = -258879910;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB80112856 = -772761477;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB69089312 = -651422609;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB11681344 = -110056203;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38359514 = -673200436;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB47440125 = -660141026;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46727523 = -48390865;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB88527255 = -99933128;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB82024339 = -805553461;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38878732 = -443989827;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25408492 = 74130660;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB85357670 = -599772826;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25865295 = -494380060;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB31335883 = -269784745;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB65573811 = 6800507;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB54494106 = -147777046;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB86473171 = -45576070;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92942302 = -833124499;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB39582896 = -644826421;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB67920871 = -985643014;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB98298369 = 18679036;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB21293532 = -124244181;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57617337 = -94540004;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB9307171 = -43010217;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB33255410 = -883102520;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46251119 = -596779408;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB3423904 = -944296891;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB8859181 = -595411063;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB59374999 = -712688301;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB43593840 = -458968118;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46445417 = -493198950;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB12734264 = -584905943;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB31519543 = -922577155;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB853299 = -658995124;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB26142241 = -508650077;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB88721143 = -120846696;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38809915 = -616621908;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB79917592 = -227815960;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB24134942 = -73000241;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB28739936 = -172297067;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB28302249 = -949199650;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB90688901 = -579329403;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10994104 = -377982956;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB71506106 = 13661792;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB95490477 = -773990381;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB21212681 = -671745308;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB45670044 = -636314436;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB36882496 = -613807006;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57756649 = -554508483;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB45393242 = -920262097;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB37451849 = -959650966;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB55317282 = -489972408;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92667221 = -556973929;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57295581 = -144556292;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB58155847 = -7700291;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB7947670 = -34635729;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB22495520 = -578221474;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB59782142 = -508412393;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB78425933 = -226953684;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92108394 = 23578972;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB44016222 = -715844136;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB37868343 = -452979802;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB29152257 = -387244828;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38430499 = -246585343;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92433315 = -950790878;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB12674228 = -340963398;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB53838128 = -677195671;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25011997 = -835384937;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB5193643 = -761134669;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB76852668 = -872352797;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB15684191 = -531155138;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB6555579 = -817760111;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68807360 = -660124258;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10842960 = -372529355;    double uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB39618623 = -916295856;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB94705000 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB44984381;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB44984381 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB56735994;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB56735994 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57307515;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57307515 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB55383442;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB55383442 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68834000;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68834000 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB7899228;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB7899228 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB91598936;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB91598936 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB62494455;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB62494455 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB76180061;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB76180061 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB75029771;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB75029771 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB79216157;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB79216157 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB93018442;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB93018442 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10384839;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10384839 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB20898969;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB20898969 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB6570352;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB6570352 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB71535339;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB71535339 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68218378;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68218378 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB23330461;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB23330461 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB99887347;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB99887347 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB23925020;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB23925020 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB48259584;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB48259584 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB32250117;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB32250117 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25216452;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25216452 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB56454217;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB56454217 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB29241202;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB29241202 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB80112856;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB80112856 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB69089312;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB69089312 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB11681344;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB11681344 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38359514;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38359514 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB47440125;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB47440125 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46727523;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46727523 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB88527255;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB88527255 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB82024339;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB82024339 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38878732;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38878732 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25408492;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25408492 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB85357670;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB85357670 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25865295;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25865295 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB31335883;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB31335883 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB65573811;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB65573811 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB54494106;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB54494106 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB86473171;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB86473171 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92942302;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92942302 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB39582896;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB39582896 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB67920871;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB67920871 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB98298369;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB98298369 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB21293532;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB21293532 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57617337;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57617337 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB9307171;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB9307171 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB33255410;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB33255410 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46251119;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46251119 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB3423904;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB3423904 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB8859181;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB8859181 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB59374999;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB59374999 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB43593840;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB43593840 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46445417;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB46445417 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB12734264;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB12734264 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB31519543;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB31519543 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB853299;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB853299 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB26142241;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB26142241 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB88721143;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB88721143 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38809915;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38809915 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB79917592;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB79917592 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB24134942;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB24134942 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB28739936;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB28739936 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB28302249;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB28302249 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB90688901;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB90688901 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10994104;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10994104 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB71506106;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB71506106 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB95490477;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB95490477 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB21212681;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB21212681 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB45670044;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB45670044 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB36882496;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB36882496 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57756649;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57756649 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB45393242;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB45393242 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB37451849;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB37451849 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB55317282;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB55317282 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92667221;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92667221 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57295581;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB57295581 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB58155847;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB58155847 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB7947670;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB7947670 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB22495520;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB22495520 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB59782142;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB59782142 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB78425933;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB78425933 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92108394;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92108394 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB44016222;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB44016222 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB37868343;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB37868343 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB29152257;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB29152257 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38430499;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB38430499 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92433315;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB92433315 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB12674228;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB12674228 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB53838128;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB53838128 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25011997;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB25011997 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB5193643;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB5193643 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB76852668;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB76852668 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB15684191;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB15684191 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB6555579;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB6555579 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68807360;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB68807360 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10842960;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB10842960 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB39618623;     uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB39618623 = uJxAFLnRhWqmGwdZYcNmzEVpUIAlwDVMGfGblWTsKJebjgsNZikXfmspnHfxLCVeBdfaqB94705000;}
// Junk Finished
