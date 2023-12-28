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

#include <optional>

#include <svl/itemset.hxx>
#include <svl/languageoptions.hxx>
#include <tools/degree.hxx>
#include <editeng/svxenum.hxx>
#include "scdllapi.h"
#include "fonthelper.hxx"
#include "scitems.hxx"
#include <unordered_set>

namespace vcl { class Font; }
namespace model { class ComplexColor; }
class OutputDevice;
class Fraction;
class ScStyleSheet;
class SvNumberFormatter;
class ScDocument;
enum class ScRotateDir : sal_uInt8;

///  how to treat COL_AUTO in GetFont:

enum class ScAutoFontColorMode
{
    Raw,         ///< COL_AUTO is returned
    Print,       ///< black or white, depending on background
    Display,     ///< from style settings, or black/white if needed
    IgnoreFont,  ///< like DISPLAY, but ignore stored font color (assume COL_AUTO)
    IgnoreBack,  ///< like DISPLAY, but ignore stored background color (use configured color)
    IgnoreAll    ///< like DISPLAY, but ignore stored font and background colors
};

class ScPatternAttr;

class SC_DLLPUBLIC CellAttributeHelper final
{
    friend class CellAttributeHolder;

    SfxItemPool&                                        mrSfxItemPool;
    mutable ScPatternAttr*                              mpDefaultCellAttribute;
    mutable std::unordered_set<const ScPatternAttr*>    maRegisteredCellAttributes;
    mutable const ScPatternAttr*                        mpLastHit;
    mutable sal_uInt64                                  mnCurrentMaxKey;

    // only to be used from CellAttributeHolder, so private
    const ScPatternAttr* registerAndCheck(const ScPatternAttr& rCandidate, bool bPassingOwnership) const;
    void doUnregister(const ScPatternAttr& rCandidate);

public:
    explicit CellAttributeHelper(SfxItemPool& rSfxItemPool);
    ~CellAttributeHelper();

    const ScPatternAttr& getDefaultCellAttribute() const;
    SfxItemPool& GetPool() const { return mrSfxItemPool; }

    void CellStyleDeleted(const ScStyleSheet& rStyle);
    void CellStyleCreated(ScDocument& rDoc, std::u16string_view rName);
    void UpdateAllStyleSheets(ScDocument& rDoc);
    void AllStylesToNames();
};

class SC_DLLPUBLIC CellAttributeHolder final
{
    const ScPatternAttr*    mpScPatternAttr;

public:
    CellAttributeHolder(const ScPatternAttr* pScPatternAttr = nullptr, bool bPassingOwnership = false);
    CellAttributeHolder(const CellAttributeHolder& rHolder);
    ~CellAttributeHolder();

    const CellAttributeHolder& operator=(const CellAttributeHolder& rHolder);
    bool operator==(const CellAttributeHolder& rHolder) const;

    const ScPatternAttr* getScPatternAttr() const { return mpScPatternAttr; }
    void setScPatternAttr(const ScPatternAttr* pNew, bool bPassingOwnership = false);

    bool operator!() const { return nullptr == mpScPatternAttr; }
    explicit operator bool() const { return nullptr != mpScPatternAttr; }

    // version that allows nullptrs
    static bool areSame(const CellAttributeHolder* p1, const CellAttributeHolder* p2);
};

class SC_DLLPUBLIC ScPatternAttr final
{
    friend class CellAttributeHelper;

    SfxItemSet                  maLocalSfxItemSet;
    std::optional<OUString>     pName;
    mutable std::optional<bool> mxVisible;
    ScStyleSheet*               pStyle;
    CellAttributeHelper*        pCellAttributeHelper;
    sal_uInt64                  mnPAKey;
    mutable size_t              mnRefCount;
#ifdef DBG_UTIL
    sal_uInt32                  m_nSerialNumber;
    bool                        m_bDeleted;
#endif

public:
    ScPatternAttr(CellAttributeHelper& rHelper, const SfxItemSet* pItemSet = nullptr, const OUString* pStyleName = nullptr);
    ScPatternAttr(const ScPatternAttr& rPatternAttr);
    ~ScPatternAttr();

    virtual bool operator==(const ScPatternAttr& rCmp) const;

    // version that allows nullptrs
    static bool areSame(const ScPatternAttr* pItem1, const ScPatternAttr* pItem2);
    bool isRegistered() const { return 0 != mnRefCount; }
    bool isDefault() const { return this == &pCellAttributeHelper->getDefaultCellAttribute(); }
    CellAttributeHelper& getCellAttributeHelper() const { return *pCellAttributeHelper; }

    const SfxItemSet& GetItemSet() const { return maLocalSfxItemSet; }
    SfxItemSet& GetItemSet() { return maLocalSfxItemSet; }

    const SfxPoolItem& GetItem(sal_uInt16 nWhichP) const { return maLocalSfxItemSet.Get(nWhichP); }
    template<class T> const T& GetItem( TypedWhichId<T> nWhich ) const
        { return static_cast<const T&>(GetItem(sal_uInt16(nWhich))); }
    static const SfxPoolItem& GetItem(sal_uInt16 nWhich, const SfxItemSet& rItemSet, const SfxItemSet* pCondSet);
    template<class T> static const T& GetItem(TypedWhichId<T> nWhich, const SfxItemSet& rItemSet, const SfxItemSet* pCondSet)
        { return static_cast<const T&>(GetItem(sal_uInt16(nWhich), rItemSet, pCondSet)); }
    const SfxPoolItem& GetItem( sal_uInt16 nWhich, const SfxItemSet* pCondSet ) const;
    template<class T> const T& GetItem(TypedWhichId<T> nWhich, const SfxItemSet* pCondSet) const
        { return static_cast<const T&>(GetItem(sal_uInt16(nWhich), pCondSet)); }

    /// @param pWhich are no ranges, but single IDs, 0-terminated
    bool HasItemsSet( const sal_uInt16* pWhich ) const;
    void ClearItems( const sal_uInt16* pWhich );
    void DeleteUnchanged( const ScPatternAttr* pOldAttrs );

    static SvxCellOrientation GetCellOrientation( const SfxItemSet& rItemSet, const SfxItemSet* pCondSet );
    SvxCellOrientation GetCellOrientation( const SfxItemSet* pCondSet = nullptr ) const;

    /** Static helper function to fill a font object from the passed item set. */
    static void fillFontOnly(vcl::Font& rFont, const SfxItemSet& rItemSet,
                                        const OutputDevice* pOutDev = nullptr,
                                        const Fraction* pScale = nullptr,
                                        const SfxItemSet* pCondSet = nullptr,
                                        SvtScriptType nScript = SvtScriptType::NONE);

    static void fillFont( vcl::Font& rFont, const SfxItemSet& rItemSet,
                                        ScAutoFontColorMode eAutoMode,
                                        const OutputDevice* pOutDev = nullptr,
                                        const Fraction* pScale = nullptr,
                                        const SfxItemSet* pCondSet = nullptr,
                                        SvtScriptType nScript = SvtScriptType::NONE, const Color* pBackConfigColor = nullptr,
                                        const Color* pTextConfigColor = nullptr);

    static void fillColor(model::ComplexColor& rComplexColor,
                            const SfxItemSet& rItemSet,
                            ScAutoFontColorMode eAutoMode,
                            const SfxItemSet* pCondSet = nullptr,
                            const Color* pBackConfigColor = nullptr,
                            const Color* pTextConfigColor = nullptr);


    static ScDxfFont        GetDxfFont(const SfxItemSet& rSet, SvtScriptType nScript);

    void fillColor(model::ComplexColor& rComplexColor,
                    ScAutoFontColorMode eAutoMode,
                    const SfxItemSet* pCondSet = nullptr,
                    const Color* pBackConfigColor = nullptr,
                    const Color* pTextConfigColor = nullptr) const
    {
        fillColor(rComplexColor, maLocalSfxItemSet, eAutoMode, pCondSet, pBackConfigColor, pTextConfigColor);
    }

    void fillFontOnly(vcl::Font& rFont,
                    const OutputDevice* pOutDev = nullptr,
                    const Fraction* pScale = nullptr,
                    const SfxItemSet* pCondSet = nullptr,
                    SvtScriptType nScript = SvtScriptType::NONE) const
    {
        fillFontOnly(rFont, maLocalSfxItemSet, pOutDev, pScale, pCondSet, nScript);
    }

    /** Fills a font object from the own item set. */
    void fillFont(vcl::Font& rFont, ScAutoFontColorMode eAutoMode,
                    const OutputDevice* pOutDev = nullptr,
                    const Fraction* pScale = nullptr,
                    const SfxItemSet* pCondSet = nullptr,
                    SvtScriptType nScript = SvtScriptType::NONE,
                    const Color* pBackConfigColor = nullptr,
                    const Color* pTextConfigColor = nullptr) const
    {
        fillFont(rFont, maLocalSfxItemSet, eAutoMode, pOutDev, pScale, pCondSet, nScript, pBackConfigColor, pTextConfigColor);
    }

    /** Converts all Calc items contained in rSrcSet to edit engine items and puts them into rEditSet. */
    static void             FillToEditItemSet( SfxItemSet& rEditSet, const SfxItemSet& rSrcSet, const SfxItemSet* pCondSet = nullptr );
    /** Converts all Calc items contained in the own item set to edit engine items and puts them into pEditSet. */
    void                    FillEditItemSet( SfxItemSet* pEditSet, const SfxItemSet* pCondSet = nullptr ) const;

    /** Converts all edit engine items contained in rEditSet to Calc items and puts them into rDestSet. */
    static void             GetFromEditItemSet( SfxItemSet& rDestSet, const SfxItemSet& rEditSet );
    /** Converts all edit engine items contained in pEditSet to Calc items and puts them into the own item set. */
    void                    GetFromEditItemSet( const SfxItemSet* pEditSet );

    void                    FillEditParaItems( SfxItemSet* pSet ) const;

    CellAttributeHolder     MigrateToDocument( ScDocument* pDestDoc, ScDocument* pSrcDoc ) const;

    void                    SetStyleSheet(ScStyleSheet* pNewStyle, bool bClearDirectFormat = true);
    const ScStyleSheet*     GetStyleSheet() const  { return pStyle; }
    const OUString*         GetStyleName() const;
    void                    UpdateStyleSheet(const ScDocument& rDoc);
    void                    StyleToName();

    bool                    IsVisible() const;
    bool                    IsVisibleEqual( const ScPatternAttr& rOther ) const;

                            /** If font is an old symbol font StarBats/StarMath
                                with text encoding RTL_TEXTENC_SYMBOL */
    bool                    IsSymbolFont() const;

    sal_uInt32              GetNumberFormat( SvNumberFormatter* ) const;
    sal_uInt32              GetNumberFormat( SvNumberFormatter* pFormatter,
                                             const SfxItemSet* pCondSet ) const;

    Degree100               GetRotateVal( const SfxItemSet* pCondSet ) const;
    ScRotateDir             GetRotateDir( const SfxItemSet* pCondSet ) const;

    void                    SetPAKey(sal_uInt64 nKey);
    sal_uInt64              GetPAKey() const;

private:
    bool                    CalcVisible() const;
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
