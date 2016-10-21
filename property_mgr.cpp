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


const PROPERTY_LIST& PROPERTY_MANAGER::GetProperties( TYPE_ID aType )
{
    static const PROPERTY_LIST empty;
    auto it = m_classes.find( aType );

    if( it == m_classes.end() )
        return empty;

    CLASS_DESC& classDesc = it->second;

    if( classDesc.m_dirty )
        classDesc.rebuildProperties();

    return classDesc.m_allProperties;
}


void* PROPERTY_MANAGER::TypeCast( void* aSource, TYPE_ID aBase, TYPE_ID aDerived ) const
{
    if( aBase == aDerived )
        return aSource;

    auto classDesc = m_classes.find( aBase );

    if( classDesc == m_classes.end() )
        return aSource;

    auto& converters = classDesc->second.m_typeCasts;
    auto converter = converters.find( aDerived );

    if( converter == converters.end() )
        return aSource;

    return (*converter->second)( aSource );
}


void PROPERTY_MANAGER::AddProperty( PROPERTY_BASE* aProperty )
{
    const wxString& name = aProperty->Name();
    wxASSERT_MSG( m_properties.count( name ) == 0, "Property name conflict" );

    TYPE_ID hash = aProperty->TypeHash();
    m_properties.emplace( name, make_pair( hash, aProperty ) );
    CLASS_DESC& classDesc = getClass( hash );
    classDesc.m_properties.emplace( name, aProperty );
    markClassesDirty();
}


void PROPERTY_MANAGER::AddTypeCast( TYPE_CAST_BASE* aCast )
{
    TYPE_ID derivedHash = aCast->DerivedHash();
    CLASS_DESC& classDesc = getClass( aCast->BaseHash() );
    auto& typeCasts = classDesc.m_typeCasts;
    wxASSERT_MSG( typeCasts.count( derivedHash ) == 0, "Such converter already exists" );
    typeCasts.emplace( derivedHash, aCast );
}


void PROPERTY_MANAGER::InheritsAfter( TYPE_ID aDerived, TYPE_ID aBase )
{
    wxASSERT_MSG( aDerived != aBase, "Class cannot inherit from itself" );

    CLASS_DESC& derived = getClass( aDerived );
    CLASS_DESC& base = getClass( aBase );
    derived.m_bases.push_back( base );
    markClassesDirty();

    wxASSERT_MSG( derived.m_bases.size() == 1
            || derived.m_typeCasts.count( aBase ) == 1,
            "You need to add a TYPE_CAST for classes inheriting from multiple bases" );
}


bool PROPERTY_MANAGER::isOfType( TYPE_ID aDerived, TYPE_ID aBase ) const
{
    if( aDerived == aBase )
        return true;

    auto derived = m_classes.find( aDerived );

    if( derived == m_classes.end() )
        return false;

    // BFS search
    for( auto& base : derived->second.m_bases )
    {
        if( base.get().m_id == aBase )
            return true;
    }

    for( auto& base : derived->second.m_bases )
    {
        if( isOfType( base.get().m_id, aBase ) )
            return true;
    }

    return true;
}


PROPERTY_MANAGER::CLASS_DESC& PROPERTY_MANAGER::getClass( TYPE_ID aTypeId )
{
    auto it = m_classes.find( aTypeId );

    if( it == m_classes.end() )
        tie( it, ignore ) = m_classes.emplace( make_pair( aTypeId, CLASS_DESC( aTypeId ) ) );

    return it->second;
}


void PROPERTY_MANAGER::markClassesDirty()
{
    for( auto& it : m_classes )
        it.second.m_dirty = true;
}


void PROPERTY_MANAGER::CLASS_DESC::rebuildProperties()
{
    m_allProperties.clear();
    collectPropsRecur( m_allProperties );
    m_dirty = false;
}


void PROPERTY_MANAGER::CLASS_DESC::collectPropsRecur( PROPERTY_LIST& aResult ) const
{
    for( auto& property : m_properties )
        aResult.push_back( property.second );

    for( const auto& base : m_bases )
        base.get().collectPropsRecur( aResult );
}
