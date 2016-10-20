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

class INSPECTED
{
public:
    virtual ~INSPECTED()
    {
    }

    template<typename T>
    bool Set( const wxString& aProperty, T aValue )
    {
        PROPERTY_MANAGER::TYPE_ID typeHash = typeid(*this).hash_code();
        PROPERTY_BASE* prop = PROPERTY_MANAGER::Instance().GetProperty( typeHash, aProperty );

        if( prop )
            prop->Set<T>( this, aValue );

        return prop != nullptr;
    }

    template<typename T>
    boost::optional<T> Get( const wxString& aProperty )
    {
        PROPERTY_MANAGER::TYPE_ID typeHash = typeid(*this).hash_code();
        PROPERTY_BASE* prop = PROPERTY_MANAGER::Instance().GetProperty( typeHash, aProperty );
        boost::optional<T> ret;

        if( prop )
            ret = prop->Get<T>( this );

        return ret;
    }
};


#endif /* INSPECTED_H */
