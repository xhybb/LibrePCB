/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * http://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "schematic.h"
#include <librepcbcommon/fileio/smartxmlfile.h>
#include <librepcbcommon/fileio/xmldomdocument.h>
#include <librepcbcommon/fileio/xmldomelement.h>
#include <librepcbcommon/scopeguardlist.h>
#include "../project.h"
#include <librepcblibrary/sym/symbolpin.h>
#include "items/si_symbol.h"
#include "items/si_symbolpin.h"
#include "items/si_netsegment.h"
#include "items/si_netpoint.h"
#include "items/si_netline.h"
#include "items/si_netlabel.h"
#include <librepcbcommon/graphics/graphicsview.h>
#include <librepcbcommon/graphics/graphicsscene.h>
#include <librepcbcommon/gridproperties.h>
#include <librepcbcommon/application.h>
#include "schematicselectionquery.h"

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

Schematic::Schematic(Project& project, const FilePath& filepath, bool restore,
                     bool readOnly, bool create, const QString& newName) throw (Exception):
    QObject(&project), IF_AttributeProvider(), mProject(project), mFilePath(filepath),
    mIsAddedToProject(false)
{
    try
    {
        mGraphicsScene.reset(new GraphicsScene());

        // try to open/create the XML schematic file
        if (create)
        {
            mXmlFile.reset(SmartXmlFile::create(mFilePath));

            // set attributes
            mUuid = Uuid::createRandom();
            mName = newName;

            // load default grid properties
            mGridProperties.reset(new GridProperties());
        }
        else
        {
            mXmlFile.reset(new SmartXmlFile(mFilePath, restore, readOnly));
            QSharedPointer<XmlDomDocument> doc = mXmlFile->parseFileAndBuildDomTree(true);
            XmlDomElement& root = doc->getRoot();

            // the schematic seems to be ready to open, so we will create all needed objects

            mUuid = root.getFirstChild("meta/uuid", true, true)->getText<Uuid>(true);
            mName = root.getFirstChild("meta/name", true, true)->getText<QString>(true);

            // Load grid properties
            mGridProperties.reset(new GridProperties(*root.getFirstChild("properties/grid_properties", true, true)));

            // Load all symbols
            for (XmlDomElement* node = root.getFirstChild("symbols/symbol", true, false);
                 node; node = node->getNextSibling("symbol"))
            {
                SI_Symbol* symbol = new SI_Symbol(*this, *node);
                if (getSymbolByUuid(symbol->getUuid())) {
                    throw RuntimeError(__FILE__, __LINE__, symbol->getUuid().toStr(),
                        QString(tr("There is already a symbol with the UUID \"%1\"!"))
                        .arg(symbol->getUuid().toStr()));
                }
                mSymbols.append(symbol);
            }

            // Load all netsegments
            for (XmlDomElement* node = root.getFirstChild("netsegments/netsegment", true, false);
                 node; node = node->getNextSibling("netsegment"))
            {
                SI_NetSegment* netsegment = new SI_NetSegment(*this, *node);
                if (getNetSegmentByUuid(netsegment->getUuid())) {
                    throw RuntimeError(__FILE__, __LINE__, netsegment->getUuid().toStr(),
                        QString(tr("There is already a netsegment with the UUID \"%1\"!"))
                        .arg(netsegment->getUuid().toStr()));
                }
                mNetSegments.append(netsegment);
            }
        }

        // emit the "attributesChanged" signal when the project has emited it
        connect(&mProject, &Project::attributesChanged, this, &Schematic::attributesChanged);

        if (!checkAttributesValidity()) throw LogicError(__FILE__, __LINE__);
    }
    catch (...)
    {
        // free the allocated memory in the reverse order of their allocation...
        qDeleteAll(mNetSegments);       mNetSegments.clear();
        qDeleteAll(mSymbols);           mSymbols.clear();
        mGridProperties.reset();
        mXmlFile.reset();
        mGraphicsScene.reset();
        throw; // ...and rethrow the exception
    }
}

Schematic::~Schematic() noexcept
{
    Q_ASSERT(!mIsAddedToProject);

    // delete all items
    qDeleteAll(mNetSegments);       mNetSegments.clear();
    qDeleteAll(mSymbols);           mSymbols.clear();

    mGridProperties.reset();
    mXmlFile.reset();
    mGraphicsScene.reset();
}

/*****************************************************************************************
 *  Getters: General
 ****************************************************************************************/

bool Schematic::isEmpty() const noexcept
{
    return (mSymbols.isEmpty() && mNetSegments.isEmpty());
}

QList<SI_Base*> Schematic::getItemsAtScenePos(const Point& pos) const noexcept
{
    QPointF scenePosPx = pos.toPxQPointF();
    QList<SI_Base*> list;   // Note: The order of adding the items is very important (the
                            // top most item must appear as the first item in the list)!

    // visible netpoints
    const QList<SI_NetPoint*> netpoints(getNetPointsAtScenePos(pos));
    foreach (SI_NetPoint* netpoint, netpoints) {
        if (netpoint->isVisible()) {
            list.append(netpoint);
        }
    }
    // hidden netpoints
    foreach (SI_NetPoint* netpoint, netpoints) {
        if (!netpoint->isVisible()) {
            list.append(netpoint);
        }
    }
    // netlines
    foreach (SI_NetLine* netline, getNetLinesAtScenePos(pos)) {
        list.append(netline);
    }
    // netlabels
    foreach (SI_NetLabel* netlabel, getNetLabelsAtScenePos(pos)) {
        list.append(netlabel);
    }
    // symbols & pins
    foreach (SI_Symbol* symbol, mSymbols) {
        foreach (SI_SymbolPin* pin, symbol->getPins()) {
            if (pin->getGrabAreaScenePx().contains(scenePosPx))
                list.append(pin);
        }
        if (symbol->getGrabAreaScenePx().contains(scenePosPx))
            list.append(symbol);
    }
    return list;
}

QList<SI_NetPoint*> Schematic::getNetPointsAtScenePos(const Point& pos) const noexcept
{
    QList<SI_NetPoint*> list;
    foreach (SI_NetSegment* segment, mNetSegments) {
        segment->getNetPointsAtScenePos(pos, list);
    }
    return list;
}

QList<SI_NetLine*> Schematic::getNetLinesAtScenePos(const Point& pos) const noexcept
{
    QList<SI_NetLine*> list;
    foreach (SI_NetSegment* segment, mNetSegments) {
        segment->getNetLinesAtScenePos(pos, list);
    }
    return list;
}

QList<SI_NetLabel*> Schematic::getNetLabelsAtScenePos(const Point& pos) const noexcept
{
    QList<SI_NetLabel*> list;
    foreach (SI_NetSegment* segment, mNetSegments) {
        segment->getNetLabelsAtScenePos(pos, list);
    }
    return list;
}

QList<SI_SymbolPin*> Schematic::getPinsAtScenePos(const Point& pos) const noexcept
{
    QList<SI_SymbolPin*> list;
    foreach (SI_Symbol* symbol, mSymbols)
    {
        foreach (SI_SymbolPin* pin, symbol->getPins())
        {
            if (pin->getGrabAreaScenePx().contains(pos.toPxQPointF()))
                list.append(pin);
        }
    }
    return list;
}

/*****************************************************************************************
 *  Setters: General
 ****************************************************************************************/

void Schematic::setGridProperties(const GridProperties& grid) noexcept
{
    *mGridProperties = grid;
}

/*****************************************************************************************
 *  Symbol Methods
 ****************************************************************************************/

SI_Symbol* Schematic::getSymbolByUuid(const Uuid& uuid) const noexcept
{
    foreach (SI_Symbol* symbol, mSymbols) {
        if (symbol->getUuid() == uuid)
            return symbol;
    }
    return nullptr;
}

void Schematic::addSymbol(SI_Symbol& symbol) throw (Exception)
{
    if ((!mIsAddedToProject) || (mSymbols.contains(&symbol))
        || (&symbol.getSchematic() != this))
    {
        throw LogicError(__FILE__, __LINE__);
    }
    // check if there is no symbol with the same uuid in the list
    if (getSymbolByUuid(symbol.getUuid())) {
        throw RuntimeError(__FILE__, __LINE__, symbol.getUuid().toStr(),
            QString(tr("There is already a symbol with the UUID \"%1\"!"))
            .arg(symbol.getUuid().toStr()));
    }
    // add to schematic
    symbol.addToSchematic(); // can throw
    mSymbols.append(&symbol);
}

void Schematic::removeSymbol(SI_Symbol& symbol) throw (Exception)
{
    if ((!mIsAddedToProject) || (!mSymbols.contains(&symbol))) {
        throw LogicError(__FILE__, __LINE__);
    }
    // remove from schematic
    symbol.removeFromSchematic(); // can throw
    mSymbols.removeOne(&symbol);
}

/*****************************************************************************************
 *  NetSegment Methods
 ****************************************************************************************/

SI_NetSegment* Schematic::getNetSegmentByUuid(const Uuid& uuid) const noexcept
{
    foreach (SI_NetSegment* netsegment, mNetSegments) {
        if (netsegment->getUuid() == uuid)
            return netsegment;
    }
    return nullptr;
}

void Schematic::addNetSegment(SI_NetSegment& netsegment) throw (Exception)
{
    if ((!mIsAddedToProject) || (mNetSegments.contains(&netsegment))
        || (&netsegment.getSchematic() != this))
    {
        throw LogicError(__FILE__, __LINE__);
    }
    // check if there is no netsegment with the same uuid in the list
    if (getNetSegmentByUuid(netsegment.getUuid())) {
        throw RuntimeError(__FILE__, __LINE__, netsegment.getUuid().toStr(),
            QString(tr("There is already a netsegment with the UUID \"%1\"!"))
            .arg(netsegment.getUuid().toStr()));
    }
    // add to schematic
    netsegment.addToSchematic(); // can throw
    mNetSegments.append(&netsegment);
}

void Schematic::removeNetSegment(SI_NetSegment& netsegment) throw (Exception)
{
    if ((!mIsAddedToProject) || (!mNetSegments.contains(&netsegment))) {
        throw LogicError(__FILE__, __LINE__);
    }
    // remove from schematic
    netsegment.removeFromSchematic(); // can throw an exception
    mNetSegments.removeOne(&netsegment);
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

void Schematic::addToProject() throw (Exception)
{
    if (mIsAddedToProject) {
        throw LogicError(__FILE__, __LINE__);
    }

    ScopeGuardList sgl(mSymbols.count() + mNetSegments.count());
    foreach (SI_Symbol* symbol, mSymbols) {
        symbol->addToSchematic(); // can throw
        sgl.add([this, symbol](){symbol->removeFromSchematic();});
    }
    foreach (SI_NetSegment* segment, mNetSegments) {
        segment->addToSchematic(); // can throw
        sgl.add([this, segment](){segment->removeFromSchematic();});
    }

    mIsAddedToProject = true;
    updateIcon();
    sgl.dismiss();
}

void Schematic::removeFromProject() throw (Exception)
{
    if (!mIsAddedToProject) {
        throw LogicError(__FILE__, __LINE__);
    }

    ScopeGuardList sgl(mSymbols.count() + mNetSegments.count());
    foreach (SI_NetSegment* segment, mNetSegments) {
        segment->removeFromSchematic(); // can throw
        sgl.add([this, segment](){segment->addToSchematic();});
    }
    foreach (SI_Symbol* symbol, mSymbols) {
        symbol->removeFromSchematic(); // can throw
        sgl.add([this, symbol](){symbol->addToSchematic();});
    }

    mIsAddedToProject = false;
    sgl.dismiss();
}

bool Schematic::save(bool toOriginal, QStringList& errors) noexcept
{
    bool success = true;

    // save schematic XML file
    try
    {
        if (mIsAddedToProject)
        {
            XmlDomDocument doc(*serializeToXmlDomElement());
            doc.setFileVersion(APP_VERSION_MAJOR);
            mXmlFile->save(doc, toOriginal);
        }
        else
        {
            mXmlFile->removeFile(toOriginal);
        }
    }
    catch (Exception& e)
    {
        success = false;
        errors.append(e.getUserMsg());
    }

    return success;
}

void Schematic::showInView(GraphicsView& view) noexcept
{
    view.setScene(mGraphicsScene.data());
}

void Schematic::setSelectionRect(const Point& p1, const Point& p2, bool updateItems) noexcept
{
    mGraphicsScene->setSelectionRect(p1, p2);
    if (updateItems)
    {
        QRectF rectPx = QRectF(p1.toPxQPointF(), p2.toPxQPointF()).normalized();
        foreach (SI_Symbol* symbol, mSymbols) {
            bool selectSymbol = symbol->getGrabAreaScenePx().intersects(rectPx);
            symbol->setSelected(selectSymbol);
            foreach (SI_SymbolPin* pin, symbol->getPins()) {
                bool selectPin = pin->getGrabAreaScenePx().intersects(rectPx);
                pin->setSelected(selectSymbol || selectPin);
            }
        }
        foreach (SI_NetSegment* segment, mNetSegments) {
            segment->setSelectionRect(rectPx);
        }
    }
}

void Schematic::clearSelection() const noexcept
{
    foreach (SI_Symbol* symbol, mSymbols) {
        symbol->setSelected(false);
    }
    foreach (SI_NetSegment* segment, mNetSegments) {
        segment->clearSelection();
    }
}

void Schematic::renderToQPainter(QPainter& painter) const noexcept
{
    mGraphicsScene->render(&painter, QRectF(), mGraphicsScene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

SchematicSelectionQuery* Schematic::createSelectionQuery() const noexcept
{
    return new SchematicSelectionQuery(mSymbols, mNetSegments, const_cast<Schematic*>(this));
}

/*****************************************************************************************
 *  Helper Methods
 ****************************************************************************************/

bool Schematic::getAttributeValue(const QString& attrNS, const QString& attrKey,
                                  bool passToParents, QString& value) const noexcept
{
    if ((attrNS == QLatin1String("PAGE")) || (attrNS.isEmpty()))
    {
        if (attrKey == QLatin1String("NAME"))
            return value = mName, true;
        else if (attrKey == QLatin1String("AUTHOR"))
            return value = mProject.getAuthor(), true;
        else if (attrKey == QLatin1String("CREATED"))
            return value = mProject.getCreated().toString(Qt::SystemLocaleShortDate), true;
        else if (attrKey == QLatin1String("LAST_MODIFIED"))
            return value = mProject.getLastModified().toString(Qt::SystemLocaleShortDate), true;
        else if (attrKey == QLatin1String("NBR"))
            return value = QString::number(mProject.getSchematicIndex(*this) + 1), true;
        else if (attrKey == QLatin1String("CNT"))
            return value = QString::number(mProject.getSchematics().count()), true;
    }

    if ((attrNS != QLatin1String("PAGE")) && (passToParents))
        return mProject.getAttributeValue(attrNS, attrKey, passToParents, value);
    else
        return false;
}

/*****************************************************************************************
 *  Private Methods
 ****************************************************************************************/

void Schematic::updateIcon() noexcept
{
    QRectF source = mGraphicsScene->itemsBoundingRect().adjusted(-20, -20, 20, 20);
    QRect target(0, 0, 297, 210); // DIN A4 format :-)

    QPixmap pixmap(target.size());
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    mGraphicsScene->render(&painter, target, source);
    mIcon = QIcon(pixmap);
}

bool Schematic::checkAttributesValidity() const noexcept
{
    if (mUuid.isNull())     return false;
    if (mName.isEmpty())    return false;
    return true;
}

XmlDomElement* Schematic::serializeToXmlDomElement() const throw (Exception)
{
    if (!checkAttributesValidity()) throw LogicError(__FILE__, __LINE__);

    QScopedPointer<XmlDomElement> root(new XmlDomElement("schematic"));
    XmlDomElement* meta = root->appendChild("meta");
    meta->appendTextChild("uuid", mUuid);
    meta->appendTextChild("name", mName);
    XmlDomElement* properties = root->appendChild("properties");
    properties->appendChild(mGridProperties->serializeToXmlDomElement());
    XmlDomElement* symbols = root->appendChild("symbols");
    foreach (SI_Symbol* symbolInstance, mSymbols) {
        symbols->appendChild(symbolInstance->serializeToXmlDomElement());
    }
    XmlDomElement* netsegments = root->appendChild("netsegments");
    foreach (SI_NetSegment* netsegment, mNetSegments) {
        netsegments->appendChild(netsegment->serializeToXmlDomElement());
    }
    return root.take();
}

/*****************************************************************************************
 *  Static Methods
 ****************************************************************************************/

Schematic* Schematic::create(Project& project, const FilePath& filepath,
                             const QString& name) throw (Exception)
{
    return new Schematic(project, filepath, false, false, true, name);
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
