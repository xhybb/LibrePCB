/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
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

#ifndef LIBRARY_LIBRARYELEMENT_H
#define LIBRARY_LIBRARYELEMENT_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/

#include <QtCore>
#include "librarybaseelement.h"

/*****************************************************************************************
 *  Class LibraryElement
 ****************************************************************************************/

namespace library {

/**
 * @brief The LibraryElement class extends the LibraryBaseElement class with some
 *        attributes and methods which are used for all library classes except categories.
 */
class LibraryElement : public LibraryBaseElement
{
        Q_OBJECT

    public:

        // Constructors / Destructor
        explicit LibraryElement(const QString& xmlRootNodeName,
                                const QUuid& uuid = QUuid::createUuid(),
                                const Version& version = Version(),
                                const QString& author = QString(),
                                const QString& name_en_US = QString(),
                                const QString& description_en_US = QString(),
                                const QString& keywords_en_US = QString()) throw (Exception);
        explicit LibraryElement(const FilePath& xmlFilePath,
                                const QString& xmlRootNodeName) throw (Exception);
        virtual ~LibraryElement() noexcept;

        // Getters: Attributes
        const QList<QUuid>& getCategories() const noexcept {return mCategories;}


    private:

        // make some methods inaccessible...
        LibraryElement(const LibraryElement& other);
        LibraryElement& operator=(const LibraryElement& rhs);


    protected:

        // Protected Methods
        virtual void parseDomTree(const XmlDomElement& root) throw (Exception);
        virtual XmlDomElement* serializeToXmlDomElement() const throw (Exception);
        virtual bool checkAttributesValidity() const noexcept;

        // General Library Element Attributes
        QList<QUuid> mCategories;
};

} // namespace library

#endif // LIBRARY_LIBRARYELEMENT_H