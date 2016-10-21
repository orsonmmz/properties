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

#ifndef INSPECTED_H
#define INSPECTED_H

#include "property_mgr.h"
#include "property.h"

#include <boost/optional.hpp>

class INSPECTED
{
public:
    virtual ~INSPECTED()
    {
    }

    template<typename T>
    void Set( PROPERTY_BASE* aProperty, T aValue )
    {
        // TODO check if property belongs to this class?
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        TYPE_ID thisType = TYPE_HASH( *this );
        void* object = propMgr.TypeCast( this, thisType, aProperty->TypeHash() );
        aProperty->set<T>( object, aValue );
    }

    template<typename T>
    bool Set( const wxString& aProperty, T aValue )
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        TYPE_ID thisType = TYPE_HASH( *this );
        PROPERTY_BASE* prop = propMgr.GetProperty( thisType, aProperty );

        if( prop )
        {
            void* object = propMgr.TypeCast( this, thisType, prop->TypeHash() );
            prop->set<T>( object, aValue );
        }

        return prop != nullptr;
    }

    template<typename T>
    T Get( PROPERTY_BASE* aProperty )
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        TYPE_ID thisType = TYPE_HASH( *this );
        void* object = propMgr.TypeCast( this, thisType, aProperty->TypeHash() );
        return aProperty->get<T>( object );
    }

    template<typename T>
    boost::optional<T> Get( const wxString& aProperty )
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        TYPE_ID thisType = TYPE_HASH( *this );
        PROPERTY_BASE* prop = propMgr.GetProperty( thisType, aProperty );
        boost::optional<T> ret;

        if( prop )
        {
            void* object = propMgr.TypeCast( this, thisType, prop->TypeHash() );
            ret = prop->get<T>( object );
        }

        return ret;
    }
};

#endif /* INSPECTED_H */
