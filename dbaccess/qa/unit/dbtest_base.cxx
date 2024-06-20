/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/config.h>

#include <string_view>

#include <test/unoapi_test.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/sdb/XOfficeDatabaseDocument.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/sdbc/XDataSource.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::uno;

class DBTestBase
    : public UnoApiTest
{
public:
    DBTestBase() : UnoApiTest(u"dbaccess/qa/unit/data"_ustr) {};

    uno::Reference<XOfficeDatabaseDocument> getDocumentForUrl(OUString const & url);

    uno::Reference< XConnection >
        getConnectionForDocument(
            uno::Reference< XOfficeDatabaseDocument > const & xDocument);

    void createDBDocument(const OUString& rDriverURL);
};

uno::Reference<XOfficeDatabaseDocument> DBTestBase::getDocumentForUrl(OUString const & url) {
    mxComponent = loadFromDesktop(url);
    uno::Reference< XOfficeDatabaseDocument > xDocument(mxComponent, UNO_QUERY_THROW);
    return xDocument;
}

uno::Reference< XConnection > DBTestBase::getConnectionForDocument(
    uno::Reference< XOfficeDatabaseDocument > const & xDocument)
{
    uno::Reference< XDataSource > xDataSource = xDocument->getDataSource();
    CPPUNIT_ASSERT(xDataSource.is());

    uno::Reference< XConnection > xConnection = xDataSource->getConnection(u""_ustr,u""_ustr);
    CPPUNIT_ASSERT(xConnection.is());

    return xConnection;
}

void DBTestBase::createDBDocument(const OUString& rDriverURL)
{
    uno::Reference< XOfficeDatabaseDocument > xDocument(
        m_xSFactory->createInstance(u"com.sun.star.sdb.OfficeDatabaseDocument"_ustr),
        UNO_QUERY_THROW);
    uno::Reference< com::sun::star::frame::XStorable > xStorable(xDocument, UNO_QUERY_THROW);

    uno::Reference< XDataSource > xDataSource = xDocument->getDataSource();
    uno::Reference< XPropertySet > xPropertySet(xDataSource, UNO_QUERY_THROW);
    xPropertySet->setPropertyValue(u"URL"_ustr, Any(rDriverURL));

    xStorable->storeAsURL(maTempFile.GetURL(), uno::Sequence< beans::PropertyValue >());

    mxComponent = loadFromDesktop(maTempFile.GetURL());
    CPPUNIT_ASSERT(mxComponent);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
