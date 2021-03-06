#ifndef SC_BOOST_DETAIL_SP_COUNTED_IMPL_HPP_INCLUDED
#define SC_BOOST_DETAIL_SP_COUNTED_IMPL_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  detail/sp_counted_impl.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//  Copyright 2004-2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <sysc/packages/boost/config.hpp>

#if defined(SC_BOOST_SP_USE_STD_ALLOCATOR) && defined(SC_BOOST_SP_USE_QUICK_ALLOCATOR)
# error SC_BOOST_SP_USE_STD_ALLOCATOR and SC_BOOST_SP_USE_QUICK_ALLOCATOR are incompatible.
#endif

#include <sysc/packages/boost/checked_delete.hpp>
#include <sysc/packages/boost/detail/sp_counted_base.hpp>

#if defined(SC_BOOST_SP_USE_QUICK_ALLOCATOR)
#include <sysc/packages/boost/detail/quick_allocator.hpp>
#endif

#include <memory>           // std::allocator
#include <typeinfo>         // std::type_info in get_deleter
#include <cstddef>          // std::size_t

namespace sc_boost
{

#if defined(SC_BOOST_SP_ENABLE_DEBUG_HOOKS)

void sp_scalar_constructor_hook( void * px, std::size_t size, void * pn );
void sp_scalar_destructor_hook( void * px, std::size_t size, void * pn );

#endif

namespace sc_detail
{

template<class X> class sp_counted_impl_p: public sp_counted_base
{
private:

    X * px_;

    sp_counted_impl_p( sp_counted_impl_p const & );
    sp_counted_impl_p & operator= ( sp_counted_impl_p const & );

    typedef sp_counted_impl_p<X> this_type;

public:

    explicit sp_counted_impl_p( X * px ): px_( px )
    {
#if defined(SC_BOOST_SP_ENABLE_DEBUG_HOOKS)
        sc_boost::sp_scalar_constructor_hook( px, sizeof(X), this );
#endif
    }

    virtual void dispose() // nothrow
    {
#if defined(SC_BOOST_SP_ENABLE_DEBUG_HOOKS)
        sc_boost::sp_scalar_destructor_hook( px_, sizeof(X), this );
#endif
        sc_boost::checked_delete( px_ );
    }

    virtual void * get_deleter( std::type_info const & )
    {
        return 0;
    }

#if defined(SC_BOOST_SP_USE_STD_ALLOCATOR)

    void * operator new( std::size_t )
    {
        return std::allocator<this_type>().allocate( 1, static_cast<this_type *>(0) );
    }

    void operator delete( void * p )
    {
        std::allocator<this_type>().deallocate( static_cast<this_type *>(p), 1 );
    }

#endif

#if defined(SC_BOOST_SP_USE_QUICK_ALLOCATOR)

    void * operator new( std::size_t )
    {
        return quick_allocator<this_type>::alloc();
    }

    void operator delete( void * p )
    {
        quick_allocator<this_type>::dealloc( p );
    }

#endif
};

//
// Borland's Codeguard trips up over the -Vx- option here:
//
#ifdef __CODEGUARD__
# pragma option push -Vx-
#endif

template<class P, class D> class sp_counted_impl_pd: public sp_counted_base
{
private:

    P ptr; // copy constructor must not throw
    D del; // copy constructor must not throw

    sp_counted_impl_pd( sp_counted_impl_pd const & );
    sp_counted_impl_pd & operator= ( sp_counted_impl_pd const & );

    typedef sp_counted_impl_pd<P, D> this_type;

public:

    // pre: d(p) must not throw

    sp_counted_impl_pd( P p, D d ): ptr(p), del(d)
    {
    }

    virtual void dispose() // nothrow
    {
        del( ptr );
    }

    virtual void * get_deleter( std::type_info const & ti )
    {
        return ti == typeid(D)? &del: 0;
    }

#if defined(SC_BOOST_SP_USE_STD_ALLOCATOR)

    void * operator new( std::size_t )
    {
        return std::allocator<this_type>().allocate( 1, static_cast<this_type *>(0) );
    }

    void operator delete( void * p )
    {
        std::allocator<this_type>().deallocate( static_cast<this_type *>(p), 1 );
    }

#endif

#if defined(SC_BOOST_SP_USE_QUICK_ALLOCATOR)

    void * operator new( std::size_t )
    {
        return quick_allocator<this_type>::alloc();
    }

    void operator delete( void * p )
    {
        quick_allocator<this_type>::dealloc( p );
    }

#endif
};

#ifdef __CODEGUARD__
# pragma option pop
#endif

} // namespace sc_detail

} // namespace sc_boost

#endif  // #ifndef SC_BOOST_DETAIL_SP_COUNTED_IMPL_HPP_INCLUDED
