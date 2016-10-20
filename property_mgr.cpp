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


void PROPERTY_MANAGER::registerProperty( size_t aType, PROPERTY_BASE* aProperty )
{
    const wxString& name = aProperty->GetName();
    wxASSERT_MSG( m_properties.count( name ) == 0, "Property name conflict" );
    m_properties[name] = make_pair( aType, aProperty );
}


void PROPERTY_MANAGER::inheritsAfter( TYPE_ID aDerived, TYPE_ID aBase )
{
    assert( aDerived != aBase );
    CLASS_TREE_NODE& derivedNode = findClassNode( aDerived );
    CLASS_TREE_NODE& baseNode = findClassNode( aBase );
    derivedNode.m_bases.push_back( baseNode );
}


bool PROPERTY_MANAGER::isOfType( TYPE_ID aDerived, TYPE_ID aBase ) const
{
    if( aDerived == aBase )
        return true;

    auto derived = m_classTree.find( aDerived );

    if( derived == m_classTree.end() )
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


PROPERTY_MANAGER::CLASS_TREE_NODE& PROPERTY_MANAGER::findClassNode( TYPE_ID aTypeId )
{
    auto it = m_classTree.find( aTypeId );

    if( it == m_classTree.end() )
        tie(it, ignore) = m_classTree.emplace( make_pair( aTypeId, CLASS_TREE_NODE{ aTypeId } ) );

    return it->second;
}
