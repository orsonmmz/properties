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
#include <vector>

class PROPERTY_BASE;

class PROPERTY_MANAGER
{
public:
    using TYPE_ID = size_t;

    static PROPERTY_MANAGER& Instance()
    {
        static PROPERTY_MANAGER pm;
        return pm;
    }

    PROPERTY_BASE* GetProperty( TYPE_ID aType, const wxString aProperty ) const;

private:
    PROPERTY_MANAGER()
    {
    }

    void registerProperty( TYPE_ID aType, PROPERTY_BASE* aProperty );
    void inheritsAfter( TYPE_ID aDerived, TYPE_ID aBase );

    std::map<wxString, std::pair<TYPE_ID, PROPERTY_BASE*>> m_properties;

    struct CLASS_TREE_NODE
    {
        TYPE_ID m_id;
        std::vector<std::reference_wrapper<CLASS_TREE_NODE>> m_bases;
    };

    bool isOfType( TYPE_ID aDerived, TYPE_ID aBase ) const;

    CLASS_TREE_NODE& findClassNode( TYPE_ID aTypeId );

    std::map<TYPE_ID, CLASS_TREE_NODE> m_classTree;

    friend class PROPERTY_BASE;
    friend struct INHERITS_AFTER_BASE;
};

#endif /* PROPERTY_MGR_H */