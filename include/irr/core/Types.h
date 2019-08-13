// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_TYPES_H_INCLUDED__
#define __IRR_TYPES_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "stdint.h"
#include <wchar.h>

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <iterator>

#include "irr/core/memory/new_delete.h"

#include "irr/core/alloc/aligned_allocator.h"
#include "irr/core/alloc/aligned_allocator_adaptor.h"

#include <mutex>

namespace irr
{
namespace core
{
template<typename T>
using allocator = _IRR_DEFAULT_ALLOCATOR_METATYPE<T>;



template<typename T>
using deque = std::deque<T,allocator<T> >;
template<typename T>
using forward_list = std::forward_list<T,allocator<T> >;
template<typename T>
using list = std::list<T,allocator<T> >;


template<typename K, typename T, class Compare=std::less<K>, class Allocator=allocator<std::pair<const K,T> > >
using map = std::map<K,T,Compare,Allocator>;
template<typename K, typename T, class Compare=std::less<K>, class Allocator=allocator<std::pair<const K,T> > >
using multimap = std::multimap<K,T,Compare,Allocator>;

template<typename K, class Compare=std::less<K>, class Allocator=allocator<K> >
using multiset = std::multiset<K,Compare,Allocator>;
template<typename K, class Compare=std::less<K>, class Allocator=allocator<K> >
using set = std::set<K,Compare,Allocator>;

template<typename K,typename T, class Hash=std::hash<K>, class KeyEqual=std::equal_to<K>, class Allocator=allocator<std::pair<const K,T> > >
using unordered_map = std::unordered_map<K,T,Hash,KeyEqual,Allocator>;
template<typename K,typename T, class Hash=std::hash<K>, class KeyEqual=std::equal_to<K>, class Allocator=allocator<std::pair<const K,T> > >
using unordered_multimap = std::unordered_multimap<K,T,Hash,KeyEqual,Allocator>;

template<typename K, class Hash=std::hash<K>, class KeyEqual=std::equal_to<K>, class Allocator=allocator<K> >
using unordered_multiset = std::unordered_multiset<K,Hash,KeyEqual,Allocator>;
template<typename K, class Hash=std::hash<K>, class KeyEqual=std::equal_to<K>, class Allocator=allocator<K> >
using unordered_set = std::unordered_set<K,Hash,KeyEqual,Allocator>;


template<typename T, class Allocator=allocator<T> >
using vector = std::vector<T,Allocator>;



template<typename T, class Container=vector<T>, class Compare=std::less<typename Container::value_type> >
using priority_queue = std::priority_queue<T,Container,Compare>;
template<typename T, class Container=deque<T> >
using queue = std::queue<T,Container>;
template<typename T, class Container=deque<T> >
using stack = std::stack<T,Container>;

template<typename T, class allocator = core::allocator<T> >
class dynamic_array
{
public:
    using allocator_type = allocator;
    using value_type = T;
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
    using iterator = T*;
    using const_iterator = const T*;

protected:
    size_t item_count;
    allocator alctr;
    pointer contents;

public:
    dynamic_array(size_t _length, const allocator& _alctr = allocator()) : item_count(_length), alctr(_alctr), contents(alctr.allocate(item_count))
    {
        for (size_t i = 0ull; i < item_count; ++i)
            std::allocator_traits<allocator>::construct(alctr, contents+i);
    }
    dynamic_array(size_t _length, const T& _val, const allocator& _alctr = allocator()) : item_count(_length), alctr(_alctr), contents(alctr.allocate(item_count))
    {
        for (size_t i = 0ull; i < item_count; ++i)
            std::allocator_traits<allocator>::construct(alctr, contents+i, _val);
    }
    dynamic_array(std::initializer_list<T> _contents, const allocator& _alctr = allocator()) : item_count(_contents.size()), alctr(_alctr), contents(alctr.allocate(item_count))
    {
        for (size_t i = 0ull; i < item_count; ++i)
            std::allocator_traits<allocator>::construct(alctr, contents+i, *(_contents.begin()+i));
    }

    virtual ~dynamic_array()
    {
        for (size_t i = 0ull; i < item_count; ++i)
            std::allocator_traits<allocator>::destroy(alctr, contents+i);
        if (contents)
            alctr.deallocate(contents, item_count);
    }

    bool operator!=(const dynamic_array<T, allocator>& _other) const
    {
        if (size() != _other.size())
            return true;
        for (size_t i = 0u; i < size(); ++i)
            if ((*this)[i] != _other[i])
                return true;
        return false;
    }
    bool operator==(const dynamic_array<T, allocator>& _other) const
    {
        return !((*this) != _other);
    }

    iterator begin() noexcept { return contents; }
    const_iterator begin() const noexcept { return contents; }
    iterator end() noexcept { return contents+item_count; }
    const_iterator end() const noexcept { return contents+item_count; }
    const_iterator cend() const noexcept { return contents+item_count; }
    const_iterator cbegin() const noexcept { return contents; }

    size_t size() const noexcept { return item_count; }
    bool empty() const noexcept { return !size(); }

    const T& operator[](size_t ix) const noexcept { return contents[ix]; }
    T& operator[](size_t ix) noexcept { return contents[ix]; }

    T& front() noexcept { return *begin(); }
    const T& front() const noexcept { return *begin(); }
    T& back() noexcept { return *(end()-1); }
    const T& back() const noexcept { return *(end()-1); }
    pointer data() noexcept { return contents; }
    const_pointer data() const noexcept { return contents; }
};


typedef std::mutex  mutex;
// change to some derivation of FW_FastLock later
typedef std::mutex  fast_mutex;
}
}


#ifdef _IRR_WINDOWS_API_
//! Defines for s{w,n}printf because these methods do not match the ISO C
//! standard on Windows platforms, but it does on all others.
//! These should be int snprintf(char *str, size_t size, const char *format, ...);
//! and int swprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, ...);
#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
#define swprintf swprintf_s
#define snprintf sprintf_s
#elif !defined(__CYGWIN__)
#define swprintf _snwprintf
#define snprintf _snprintf
#endif

#endif // _IRR_WINDOWS_API_



#define _IRR_TEXT(X) X




// memory debugging
#if defined(_IRR_DEBUG) && defined(IRRLICHT_EXPORTS) && defined(_MSC_VER) && \
	(_MSC_VER > 1299) && !defined(_IRR_DONT_DO_MEMORY_DEBUGGING_HERE) && !defined(_WIN32_WCE)

	#define CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC
	#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#include <stdlib.h>
	#include <crtdbg.h>
	#define new DEBUG_CLIENTBLOCK
#endif


//! ignore VC8 warning deprecated
/** The microsoft compiler */
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER >= 1400)
	//#pragma warning( disable: 4996)
	//#define _CRT_SECURE_NO_DEPRECATE 1
	//#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif


//! creates four CC codes used in Irrlicht for simple ids
/** some compilers can create those by directly writing the
code like 'code', but some generate warnings so we use this macro here */
#define MAKE_IRR_ID(c0, c1, c2, c3) \
		((uint32_t)(uint8_t)(c0) | ((uint32_t)(uint8_t)(c1) << 8) | \
		((uint32_t)(uint8_t)(c2) << 16) | ((uint32_t)(uint8_t)(c3) << 24 ))

#if defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#define _strcmpi(a,b) strcmpi(a,b)
#endif

#endif // __IRR_TYPES_H_INCLUDED__

