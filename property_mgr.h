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

#ifndef PROPERTY_MGR_H
#define PROPERTY_MGR_H

#include <wx/string.h>

#include <map>
#include <memory>
#include <vector>

class PROPERTY_BASE;
class TYPE_CAST_BASE;

using TYPE_ID = size_t;
using PROPERTY_LIST = std::vector<PROPERTY_BASE*>;

class PROPERTY_MANAGER
{
public:
    static PROPERTY_MANAGER& Instance()
    {
        static PROPERTY_MANAGER pm;
        return pm;
    }

    PROPERTY_BASE* GetProperty( TYPE_ID aType, const wxString aProperty ) const;
    const PROPERTY_LIST& GetProperties( TYPE_ID aType );
    void* TypeCast( void* aSource, TYPE_ID aBase, TYPE_ID aDerived ) const;

    void AddProperty( PROPERTY_BASE* aProperty );
    void AddTypeCast( TYPE_CAST_BASE* aCast );
    void InheritsAfter( TYPE_ID aDerived, TYPE_ID aBase );

private:
    PROPERTY_MANAGER()
    {
    }

    struct CLASS_DESC
    {
        CLASS_DESC( TYPE_ID aId )
            : m_id( aId ), m_dirty( true )
        {
        }

        TYPE_ID m_id;
        std::vector<std::reference_wrapper<CLASS_DESC>> m_bases;
        std::map<wxString, std::unique_ptr<PROPERTY_BASE>> m_ownProperties;
        std::map<TYPE_ID, std::unique_ptr<TYPE_CAST_BASE>> m_typeCasts;
        bool m_dirty;
        std::vector<PROPERTY_BASE*> m_allProperties;

        void rebuildProperties();
        void collectPropsRecur( PROPERTY_LIST& aResult ) const;
    };

    std::map<wxString, PROPERTY_BASE*> m_properties;
    std::map<TYPE_ID, CLASS_DESC> m_classes;

    bool isOfType( TYPE_ID aDerived, TYPE_ID aBase ) const;
    CLASS_DESC& getClass( TYPE_ID aTypeId );
    void markClassesDirty();
};

#endif /* PROPERTY_MGR_H */
