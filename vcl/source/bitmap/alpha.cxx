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

#include <tools/color.hxx>
#include <vcl/alpha.hxx>

#include <bitmap/BitmapWriteAccess.hxx>
#include <salinst.hxx>
#include <svdata.hxx>
#include <salbmp.hxx>
#include <sal/log.hxx>
#if HAVE_FEATURE_SKIA
#include <vcl/skia/SkiaHelper.hxx>
#endif


AlphaMask::AlphaMask() = default;

AlphaMask::AlphaMask( const Bitmap& rBitmap ) :
    Bitmap( rBitmap )
{
    // no need to do any conversion if it is already an AlphaMask
    if ( typeid(rBitmap) != typeid(AlphaMask) && !rBitmap.IsEmpty() )
        Convert( BmpConversion::N8BitNoConversion );
#if HAVE_FEATURE_SKIA
    // Related tdf#156866 force snapshot of alpha mask when using Skia
    // In release builds, tdf#156629 and tdf#156630 reappear in many
    // cases because a BitmapInfoAccess is in a debug block. So, instead
    // of relying on other code to a create a BitmapInfoAccess instance,
    // create one here to force the alpha mask to handle any pending
    // scaling and make the alpha mask immutable.
    else if ( SkiaHelper::isVCLSkiaEnabled() )
        BitmapInfoAccess aInfoAccess( *this );
#endif
    assert( (IsEmpty() || getPixelFormat() == vcl::PixelFormat::N8_BPP) && "alpha bitmap should be 8bpp" );
    assert( (IsEmpty() || HasGreyPalette8Bit()) && "alpha bitmap should have greyscale palette" );
}

AlphaMask::AlphaMask( const AlphaMask& ) = default;

AlphaMask::AlphaMask( AlphaMask&& ) = default;

AlphaMask::AlphaMask( const Size& rSizePixel, const sal_uInt8* pEraseTransparency )
    : Bitmap(rSizePixel, vcl::PixelFormat::N8_BPP, &Bitmap::GetGreyPalette(256))
{
    if( pEraseTransparency )
    {
        sal_uInt8 nAlpha = 255 - *pEraseTransparency;
        Bitmap::Erase( Color( nAlpha, nAlpha, nAlpha ) );
    }
    else
        Bitmap::Erase( COL_ALPHA_OPAQUE );
}

AlphaMask::~AlphaMask() = default;

AlphaMask& AlphaMask::operator=( const Bitmap& rBitmap )
{
    *static_cast<Bitmap*>(this) = rBitmap;

    if( !rBitmap.IsEmpty() )
        Convert( BmpConversion::N8BitNoConversion );

    assert( getPixelFormat() == vcl::PixelFormat::N8_BPP && "alpha bitmap should be 8bpp" );
    assert( HasGreyPalette8Bit() && "alpha bitmap should have greyscale palette" );

    return *this;
}

const Bitmap& AlphaMask::ImplGetBitmap() const
{
    return *this;
}

Bitmap const & AlphaMask::GetBitmap() const
{
    return ImplGetBitmap();
}

void AlphaMask::Erase( sal_uInt8 cTransparency )
{
    sal_uInt8 nAlpha = 255 - cTransparency;
    Bitmap::Erase( Color( nAlpha, nAlpha, nAlpha ) );
}

void AlphaMask::BlendWith(const AlphaMask& rOther)
{
    std::shared_ptr<SalBitmap> xImpBmp(ImplGetSVData()->mpDefInst->CreateSalBitmap());
    if (xImpBmp->Create(*ImplGetSalBitmap()) && xImpBmp->AlphaBlendWith(*rOther.ImplGetSalBitmap()))
    {
        ImplSetSalBitmap(xImpBmp);
        assert( getPixelFormat() == vcl::PixelFormat::N8_BPP && "alpha bitmap should be 8bpp" );
        assert( HasGreyPalette8Bit() && "alpha bitmap should have greyscale palette" );
        return;
    }
    Bitmap::ScopedReadAccess pOtherAcc(const_cast<AlphaMask&>(rOther));
    AlphaScopedWriteAccess pAcc(*this);
    assert (pOtherAcc && pAcc && pOtherAcc->GetBitCount() == 8 && pAcc->GetBitCount() == 8 && "cannot BlendWith this combination");
    if (!(pOtherAcc && pAcc && pOtherAcc->GetBitCount() == 8 && pAcc->GetBitCount() == 8))
    {
        SAL_WARN("vcl", "cannot BlendWith this combination");
        return;
    }

    const tools::Long nHeight = std::min(pOtherAcc->Height(), pAcc->Height());
    const tools::Long nWidth = std::min(pOtherAcc->Width(), pAcc->Width());
    for (tools::Long y = 0; y < nHeight; ++y)
    {
        Scanline scanline = pAcc->GetScanline( y );
        ConstScanline otherScanline = pOtherAcc->GetScanline( y );
        for (tools::Long x = 0; x < nWidth; ++x)
        {
            // Use sal_uInt16 for following multiplication
            const sal_uInt16 nGrey1 = *scanline;
            const sal_uInt16 nGrey2 = *otherScanline;
            // Awkward calculation because the original used transparency, and to replicate
            // the logic we need to translate into transparency, perform the original logic,
            // then translate back to alpha.
            auto tmp = 255 - ((255 - nGrey1) + (255 - nGrey2) - (255 - nGrey1) * (255 - nGrey2) / 255);
            *scanline = static_cast<sal_uInt8>(tmp);
            ++scanline;
            ++otherScanline;
        }
    }
    pAcc.reset();
    assert( getPixelFormat() == vcl::PixelFormat::N8_BPP && "alpha bitmap should be 8bpp" );
    assert( HasGreyPalette8Bit() && "alpha bitmap should have greyscale palette" );
}

bool AlphaMask::hasAlpha() const
{
    // no content, no alpha
    if(IsEmpty())
        return false;

    ScopedReadAccess pAcc(const_cast<AlphaMask&>(*this));
    const tools::Long nHeight(pAcc->Height());
    const tools::Long nWidth(pAcc->Width());

    // no content, no alpha
    if(0 == nHeight || 0 == nWidth)
        return false;

    for (tools::Long y = 0; y < nHeight; ++y)
    {
        for (tools::Long x = 0; x < nWidth; ++x)
        {
            if (255 != pAcc->GetColor(y, x).GetRed())
            {
                return true;
            }
        }
    }

    return false;
}

void AlphaMask::ReleaseAccess( BitmapReadAccess* pAccess )
{
    if( pAccess )
    {
        Bitmap::ReleaseAccess( pAccess );
        Convert( BmpConversion::N8BitNoConversion );
    }
    assert( getPixelFormat() == vcl::PixelFormat::N8_BPP && "alpha bitmap should be 8bpp" );
    assert( HasGreyPalette8Bit() && "alpha bitmap should have greyscale palette" );
}

bool AlphaMask::AlphaCombineOr(const AlphaMask& rMask)
{
    ScopedReadAccess pMaskAcc(const_cast<AlphaMask&>(rMask));
    AlphaScopedWriteAccess pAcc(*this);

    if (!pMaskAcc || !pAcc)
        return false;

    assert (pMaskAcc->GetBitCount() == 8 && pAcc->GetBitCount() == 8);

    const tools::Long nWidth = std::min(pMaskAcc->Width(), pAcc->Width());
    const tools::Long nHeight = std::min(pMaskAcc->Height(), pAcc->Height());

    for (tools::Long nY = 0; nY < nHeight; nY++)
    {
        Scanline pScanline = pAcc->GetScanline(nY);
        ConstScanline pScanlineMask = pMaskAcc->GetScanline(nY);
        for (tools::Long nX = 0; nX < nWidth; nX++)
        {
            if (*pScanlineMask != 255 || *pScanline != 255)
                *pScanline = 0;
            else
                *pScanline = 255;
            ++pScanline;
            ++pScanlineMask;
        }
    }

    return true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
