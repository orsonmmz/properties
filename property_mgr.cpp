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

#include "property_mgr.h"
#include "property.h"

#include <utility>

using namespace std;

PROPERTY_BASE* PROPERTY_MANAGER::GetProperty( TYPE_ID aType, const wxString aProperty ) const
{
    auto it = m_properties.find( aProperty );

    if( it == m_properties.end() )
        return nullptr;

    TYPE_ID type = it->second.first;

    if( isOfType( aType, type ) )
        return it->second.second;

    return nullptr;
}


void* PROPERTY_MANAGER::TypeCast( void* aSource, TYPE_ID aBase, TYPE_ID aDerived ) const
{
    if( aBase == aDerived )
        return aSource;

    auto classDesc = m_classMap.find( aBase );

    if( classDesc == m_classMap.end() )
        return aSource;

    auto converters = classDesc->second.m_converters;
    auto converter = converters.find( aDerived );

    if( converter == converters.end() )
        return aSource;

    return converter->second( aSource );
}


void PROPERTY_MANAGER::registerProperty( PROPERTY_BASE* aProperty )
{
    const wxString& name = aProperty->GetName();
    TYPE_ID hash = aProperty->TypeHash();
    wxASSERT_MSG( m_properties.count( name ) == 0, "Property name conflict" );
    m_properties.emplace( name, make_pair( hash, aProperty ) );
}


void PROPERTY_MANAGER::registerConverter( TYPE_CONVERTER& aConverter )
{
    TYPE_ID derivedHash = aConverter.GetDerivedHash();
    CLASS_DESC& classDesc = findClass( aConverter.GetBaseHash() );
    wxASSERT_MSG( classDesc.m_converters.count( derivedHash ) == 0,
            "Such converter already exists" );
    classDesc.m_converters.emplace( derivedHash, aConverter );
}


void PROPERTY_MANAGER::inheritsAfter( TYPE_ID aDerived, TYPE_ID aBase )
{
    assert( aDerived != aBase );
    CLASS_DESC& derivedNode = findClass( aDerived );
    CLASS_DESC& baseNode = findClass( aBase );
    derivedNode.m_bases.push_back( baseNode );
}


bool PROPERTY_MANAGER::isOfType( TYPE_ID aDerived, TYPE_ID aBase ) const
{
    if( aDerived == aBase )
        return true;

    auto derived = m_classMap.find( aDerived );

    if( derived == m_classMap.end() )
        return false;

    // BFS search
    for( auto base : derived->second.m_bases )
    {
        if( base.get().m_id == aBase )
            return true;
    }

    for( auto base : derived->second.m_bases )
    {
        if( isOfType( base.get().m_id, aBase ) )
            return true;
    }

    return true;
}


PROPERTY_MANAGER::CLASS_DESC& PROPERTY_MANAGER::findClass( TYPE_ID aTypeId )
{
    auto it = m_classMap.find( aTypeId );

    if( it == m_classMap.end() )
        tie(it, ignore) = m_classMap.emplace( make_pair( aTypeId, CLASS_DESC{ aTypeId } ) );

    return it->second;
}
