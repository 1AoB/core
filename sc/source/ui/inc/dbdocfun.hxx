/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#pragma once

#include <address.hxx>
#include <vector>

struct ScImportParam;
struct ScQueryParam;
struct ScSortParam;
struct ScSubTotalParam;

class SfxViewFrame;
class ScDBData;
class ScDocShell;
class ScDPObject;
class ScDBCollection;

namespace com::sun::star::uno { template <typename > class Sequence; }
namespace com::sun::star::beans { struct PropertyValue; }

namespace svx {
    class ODataAccessDescriptor;
}

class ScDBDocFunc
{
friend class ScDBFunc;

private:
    ScDocShell&     rDocShell;

public:
                    ScDBDocFunc( ScDocShell& rDocSh ): rDocShell(rDocSh) {}

    void            UpdateImport( const OUString& rTarget, const svx::ODataAccessDescriptor& rDescriptor );

    bool DoImport( SCTAB nTab, const ScImportParam& rParam,
                   const svx::ODataAccessDescriptor* pDescriptor);      // used for selection an existing ResultSet

    void DoImportUno( const ScAddress& rPos,
                      const css::uno::Sequence<css::beans::PropertyValue>& aArgs );

    static void     ShowInBeamer( const ScImportParam& rParam, const SfxViewFrame* pFrame );

    SC_DLLPUBLIC bool Sort(
        SCTAB nTab, const ScSortParam& rSortParam, bool bRecord, bool bPaint, bool bApi );

    SC_DLLPUBLIC bool           Query( SCTAB nTab, const ScQueryParam& rQueryParam,
                            const ScRange* pAdvSource, bool bRecord, bool bApi );

    void            DoSubTotals( SCTAB nTab, const ScSubTotalParam& rParam,
                                    bool bRecord, bool bApi );

    SC_DLLPUBLIC bool AddDBRange( const OUString& rName, const ScRange& rRange );
    bool DeleteDBRange( const OUString& rName );
    bool RenameDBRange( const OUString& rOld, const OUString& rNew );
    void ModifyDBData( const ScDBData& rNewData );  // Name unchanged

    void ModifyAllDBData( const ScDBCollection& rNewColl, const std::vector<ScRange>& rDelAreaList );

    bool RepeatDB( const OUString& rDBName, bool bApi, bool bIsUnnamed, SCTAB aTab = 0);

    bool DataPilotUpdate( ScDPObject* pOldObj, const ScDPObject* pNewObj,
                          bool bRecord, bool bApi, bool bAllowMove = false );

    bool RemovePivotTable(const ScDPObject& rDPObj, bool bRecord, bool bApi);
    bool CreatePivotTable(const ScDPObject& rDPObj, bool bRecord, bool bApi);
    bool UpdatePivotTable(ScDPObject& rDPObj, bool bRecord, bool bApi);

    /**
     * Reload the referenced pivot cache, and refresh all pivot tables that
     * reference the cache.
     */
    void RefreshPivotTables(const ScDPObject* pDPObj, bool bApi);

    /**
     * Refresh the group dimensions of all pivot tables referencing the same
     * cache.
     */
    void RefreshPivotTableGroups(ScDPObject* pDPObj);
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
