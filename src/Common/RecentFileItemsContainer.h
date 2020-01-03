#ifndef __RECENT_FILE_ITEMS_CONTAINER_H__
#define __RECENT_FILE_ITEMS_CONTAINER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/



#include <memory>

#include "CaretObjectTracksModification.h"
#include "RecentFileItemSortingKeyEnum.h"
#include "RecentFileTypeEnum.h"

class QXmlStreamReader;

namespace caret {

    class RecentFileItem;
    class RecentFileItemsFilter;
    
    class RecentFileItemsContainer : public CaretObjectTracksModification {
        
    public:
        RecentFileItemsContainer(const AString& directoryPath);
        
        virtual ~RecentFileItemsContainer();
        
        RecentFileItemsContainer(const RecentFileItemsContainer&) = delete;

        RecentFileItemsContainer& operator=(const RecentFileItemsContainer&) = delete;

        std::vector<RecentFileItem*> getItems(const RecentFileItemsFilter& itemsFilter) const;
        
        void readFromPreferences();
        
        void readFromDirectory(const AString& directoryName);
        
        void addItem(RecentFileItem* recentFile);
        
        void clear();
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        void sort(const RecentFileItemSortingKeyEnum::Enum sortingKey);
        
        bool readFromXML(const AString& xml,
                         AString& errorMessageOut);
        
        bool writeToXML(AString& xml,
                        AString& errorMessageOut) const;
        
        void testXmlReadingAndWriting();
        
    private:
        void addFilesInDirectoryToRecentItems(const RecentFileTypeEnum::Enum recentFileType,
                                              const AString& directoryPath);
        
        void readFromXmlVersionOne(QXmlStreamReader& reader);
        
        void readFromXMLVersionOneRecentFileItem(QXmlStreamReader& reader);
        
        std::vector<std::unique_ptr<RecentFileItem>> m_recentFiles;
        
        // ADD_NEW_MEMBERS_HERE

        
        static const AString XML_TAG_RECENT_FILE_ITEMS_CONTAINER;
        static const AString XML_TAG_RECENT_FILE_ITEM;
        static const AString XML_TAG_RECENT_FILE_ITEM_COMMENT;
        static const AString XML_TAG_RECENT_FILE_ITEM_DATE_AND_TIME;
        static const AString XML_TAG_RECENT_FILE_ITEM_FILE_TYPE;
        static const AString XML_TAG_RECENT_FILE_ITEM_FAVORITE;
        static const AString XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME;

    };
    
#ifdef __RECENT_FILE_ITEMS_CONTAINER_DECLARE__
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEMS_CONTAINER = "RecentFileItemsContainer";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM = "RecentFileItem";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_COMMENT = "Comment";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_DATE_AND_TIME = "DateTime";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_FILE_TYPE = "FileType";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_FAVORITE = "Favorite";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME = "PathAndFileName";
#endif // __RECENT_FILE_ITEMS_CONTAINER_DECLARE__

} // namespace
#endif  //__RECENT_FILE_ITEMS_CONTAINER_H__
