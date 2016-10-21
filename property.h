/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef PROPERTY_H
#define PROPERTY_H

#include <wx/any.h>
#include <wx/string.h>

#include <boost/optional.hpp>

#include <type_traits>
#include <typeindex>

#ifdef DEBUG_PROPERTY
#include <map>
std::map<size_t, std::string>& typeMap();
#endif

#define TYPE_HASH( x ) typeid( x ).hash_code()
//#define TYPE_HASH( x ) typeid( std::decay<x>::type ).hash_code()

class PROPERTY_BASE
{
public:
    PROPERTY_BASE( const wxString& aName );

    virtual ~PROPERTY_BASE()
    {
    }

    const wxString& GetName() const
    {
        return m_name;
    }

    template<typename T>
    void Set( void* aObject, T aValue )
    {
        wxAny a = aValue;
        setter( aObject, a );
    }

    template<typename T>
    T Get( void* aObject )
    {
        wxAny a = getter( aObject );
        return a.As<T>();
    }

    virtual size_t TypeHash() const = 0;

protected:
    void registerForType( size_t aType );
    virtual void setter( void* aObject, wxAny& aValue ) = 0;
    virtual wxAny getter( void* aObject ) = 0;

private:
    wxString m_name;
};

template<typename Owner, typename T>
class PROPERTY : public PROPERTY_BASE
{
public:
    typedef typename std::decay<T>::type BASE_TYPE;
    typedef T (Owner::*GETTER)() const;
    typedef void (Owner::*SETTER)( T );

    PROPERTY( const wxString& name, SETTER s, GETTER g ) :
        PROPERTY_BASE( name ), m_setter( s ), m_getter( g )
    {
        registerForType( TYPE_HASH( Owner ) );

#ifdef DEBUG_PROPERTY
        typeMap()[TYPE_HASH( Owner )] = typeid( Owner ).name();
#endif

        //static_assert(std::is_same<decltype(&Owner::AddProperty),
                //void (Owner::*)(const wxString&, PROPERTY_BASE*)>::value,
                            //"Class does not define a static AddProperty() method");
    }

    size_t TypeHash() const override
    {
        return TYPE_HASH( Owner );
    }

protected:
    void setter( void* obj, wxAny& v ) override
    {
        Owner* o = reinterpret_cast<Owner*>( obj );
        BASE_TYPE value = v.As<BASE_TYPE>();
        (o->*m_setter)(value);
    }

    wxAny getter( void* obj ) override
    {
        const Owner* o = reinterpret_cast<const Owner*>( obj );
        wxAny res = (o->*m_getter)();
        return res;
    }

    SETTER m_setter;
    GETTER m_getter;
};


class TYPE_CONVERTER
{
public:
    virtual void* operator()( void* aPointer ) const = 0;
    virtual size_t GetBaseHash() const = 0;
    virtual size_t GetDerivedHash() const = 0;

protected:
    void registerConverter();
};


template<typename Base, typename Derived>
class TYPE_CAST : public TYPE_CONVERTER
{
public:
    TYPE_CAST()
    {
        registerConverter();
    }

    void* operator()( void* aPointer ) const override
    {
        Base* base = reinterpret_cast<Base*>( aPointer );
        return static_cast<Derived*>( base );
    }

    size_t GetBaseHash() const override
    {
        return TYPE_HASH( Base );
    }

    size_t GetDerivedHash() const override
    {
        return TYPE_HASH( Derived );
    }
};


struct INHERITS_AFTER_BASE
{
    INHERITS_AFTER_BASE( size_t aDerived, size_t aBase );
};

#endif /* PROPERTY_H */
