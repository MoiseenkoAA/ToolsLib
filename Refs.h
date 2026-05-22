
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Refs.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform class for simple reference count
 * implementation.
 * The library implementation written
 * by Andrey A. Moiseenko (support@maasoftware.ru).
 * This library and applications are
 * FREE FOR COMMERCIAL AND NON-COMMERCIAL USE
 * as long as the following conditions are aheared to.
 *
 * Copyright remains Andrey A. Moiseenko, and as such any Copyright notices in
 * the code are not to be removed.  If this code is used in a product,
 * Andrey A. Moiseenko should be given attribution as the author of the parts used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Andrey A. Moiseenko (support@maasoftware.ru)
 *
 * THIS SOFTWARE IS PROVIDED BY ANDREY A. MOISEENKO ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifndef __TOOLSLIB_REFS_H
#define __TOOLSLIB_REFS_H

#if defined(TOOLSLIB_SINGLE_THREAD) || defined(__unix__)
#define CMaaReferenceBase 1
#else
#define CMaaReferenceBase 0
#endif

typedef int TOOLSLIB_REFS_TYPE;

//------------------------------------------------------------------------------
// class CMaaReference
//------------------------------------------------------------------------------
class ToolsExport CMaaReference // 0 - based: -1 means its deleting
{
#ifdef TOOLSLIB_SINGLE_THREAD
    TOOLSLIB_REFS_TYPE m_Refs;
#else
    std::atomic<TOOLSLIB_REFS_TYPE> m_Refs;
#endif

public:
    CMaaReference() noexcept
    :   m_Refs(CMaaReferenceBase)
    {
    }

    // true if and only if ONE reference encountered
    // i.e. you have no restrictions in object modification
    // In addition You need to use synchronization dealing multithread under Windows
    bool IsSingleOwner() const noexcept
    {
        return GetRefsCount() == CMaaReferenceBase;
    }

    virtual TOOLSLIB_REFS_TYPE AddRef() noexcept
    {
        return ++m_Refs;
    }

    virtual TOOLSLIB_REFS_TYPE Release() noexcept
    {
#if defined(TOOLSLIB_SINGLE_THREAD) || defined(__unix__)
        const TOOLSLIB_REFS_TYPE r = --m_Refs;
#else
        const TOOLSLIB_REFS_TYPE r = m_Refs.fetch_sub(1, std::TL_memory_order_acq_rel);
#endif
        if (!r)
        {
            delete this;
            return 0;
        }
        return r;
    }

    TOOLSLIB_REFS_TYPE GetRefsCount() const noexcept
    {
#ifdef TOOLSLIB_SINGLE_THREAD
        return m_Refs;
#else
        return m_Refs.load(std::TL_memory_order_acquire);
#endif
    }

protected:
    virtual ~CMaaReference() noexcept {}
};

//------------------------------------------------------------------------------
// class CMaaManualReference // non-virtual manual delete Reference, 0 - based: -1 means its need to free
//------------------------------------------------------------------------------
class ToolsExport CMaaManualReference
{
    union _u
    {
#ifdef TOOLSLIB_SINGLE_THREAD
        mutable TOOLSLIB_REFS_TYPE m_Refs;
#else
        mutable std::atomic<TOOLSLIB_REFS_TYPE> m_Refs;
#endif
        mutable TOOLSLIB_REFS_TYPE m_SimpelRefs;
        constexpr _u(TOOLSLIB_REFS_TYPE x) noexcept : m_SimpelRefs(x) {}
    } m_u;

public:
    constexpr CMaaManualReference() noexcept
    :   m_u(CMaaReferenceBase)
    {
    }
    constexpr CMaaManualReference(TOOLSLIB_REFS_TYPE x) noexcept
    :   m_u(x)
    {
    }

    // true if and only if ONE reference encountered
    // i.e. you have no restrictions in object modification
    // In addition You need to use synchronization dealing multithread under Windows
    bool IsSingleOwner() const noexcept
    {
        return GetRefsCount() == CMaaReferenceBase;
    }

    TOOLSLIB_REFS_TYPE AddRef() noexcept
    {
        return ++m_u.m_Refs;
    }

    TOOLSLIB_REFS_TYPE Release() noexcept
    {
#if defined(TOOLSLIB_SINGLE_THREAD) || defined(__unix__)
        return --m_u.m_Refs;
#else
        return m_u.m_Refs.fetch_sub(1, std::TL_memory_order_acq_rel);
#endif
    }

    TOOLSLIB_REFS_TYPE GetRefsCount() const noexcept
    {
#ifdef TOOLSLIB_SINGLE_THREAD
        return m_u.m_Refs;
#else
        return m_u.m_Refs.load(std::TL_memory_order_acquire);
#endif
    }

#if 0
    constexpr CMaaManualReference(const CMaaManualReference& That) noexcept
    :   m_u(That.m_u.m_SimpelRefs)
    {
    }
    constexpr CMaaManualReference& operator=(const CMaaManualReference& That) noexcept
    {
        m_u.m_SimpelRefs = That.m_u.m_SimpelRefs;
        return *this;
    }
#endif

protected:
    constexpr ~CMaaManualReference() noexcept {}
};

//------------------------------------------------------------------------------
// class CMaaManualReference // non-virtual manual delete Reference
//------------------------------------------------------------------------------
class ToolsExport CMaaManualReferenceDLink : public CMaaDLink // 0 - based : -1 means its need to free
{
#ifdef TOOLSLIB_SINGLE_THREAD
    TOOLSLIB_REFS_TYPE m_Refs;
#else
    std::atomic<TOOLSLIB_REFS_TYPE> m_Refs;
#endif

public:
    CMaaManualReferenceDLink() noexcept
    :   CMaaDLink(eNotInit), // 18.08.2025 opt: skipping CMaaDLink init
        m_Refs(CMaaReferenceBase)
    {
    }

    // _true if and only if ONE reference encountered
    // i.e. you have no restrictions in object modification
    // In addition You need to use synchronization dealing multithread under Windows
    bool IsSingleOwner() const noexcept
    {
        return GetRefsCount() == CMaaReferenceBase;
    }

    TOOLSLIB_REFS_TYPE AddRef() noexcept
    {
        return ++m_Refs;
    }

    TOOLSLIB_REFS_TYPE Release() noexcept
    {
#if defined(TOOLSLIB_SINGLE_THREAD) || defined(__unix__)
        return --m_Refs;
#else
        return m_Refs.fetch_sub(1, std::TL_memory_order_acq_rel);
#endif
    }

    TOOLSLIB_REFS_TYPE GetRefsCount() const noexcept
    {
#ifdef TOOLSLIB_SINGLE_THREAD
        return m_Refs;
#else
        return m_Refs.load(std::TL_memory_order_acquire);
#endif
    }

//protected:
//    ~CMaaManualReferenceDLink() noexcept {}
};

#endif // __TOOLSLIB_REFS_H

//==============================================================================
