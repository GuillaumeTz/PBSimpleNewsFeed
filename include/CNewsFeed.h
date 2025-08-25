/* Copyright(C) 2025 guillaume.taze@proton.me

This program is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#ifndef CNEWSFEED_H
#define CNEWSFEED_H

#include <string>
#include <vector>
#include <ctime>

#include "tinyxml2.h"

#include "CDownload.h"

namespace ERssType
{
	enum Type
	{
		Rss,
		Atom,
	};
}

class CNewsEntry
{
public:
	CNewsEntry();

	std::tm GetTime() const;

	tinyxml2::XMLElement* ToElement(tinyxml2::XMLDocument* Doc);
	void ParseElement(tinyxml2::XMLElement* InXmlElement);

	void MarkAsRead();

	std::string Title;
	std::string Text;
	std::string Time;
	std::string Link;
	std::string ExternalLink;
	std::string UniqueId;
	std::string ParentUniqueId;

	bool bHasRead;
	bool bIsNew;

private:
	friend class CNewsFeed;

	ERssType::Type RssType;
};

struct SReaderModeSettings
{
	SReaderModeSettings() : bShowOnlyTextualRelevantTags(true), bHideElementsBeforeFirstPhrase(true) {}

	bool bShowOnlyTextualRelevantTags;
	bool bHideElementsBeforeFirstPhrase;
};

class CNewsFeed
{
public:
	CNewsFeed();

	std::string Title;
	std::string Url;
	std::string UniqueId;
	std::string ParentUniqueId;

	bool bIsFolder;

	std::vector<CNewsFeed> NewsFeeds;
	std::vector<CNewsEntry> Entries;

	SReaderModeSettings ReaderModeSettings;
	std::time_t LastEntryTime;
	bool bDisplayLastEntryFirst;
	bool bDeleted;

private:
	int NbUnRead;
	int NbNew;
	bool bIsLoaded;

public:
	bool IsLoaded() const { return bIsLoaded; }
	void LoadFeeds(bool bForce, bool bRecursive);
	void LoadDocument(bool bForce = false);
	void LoadDocument(const std::string& InFilePath);
	void SaveDocument(bool bSaveAll);

	void TransferFromOldFeed(const CNewsFeed& OldFeed);

	std::vector<CDownload> Sync();
	std::vector<CDownload> AdditionalSync();

	std::string GetLocalFilePath(const std::string& Suffix) const;

	void MarkAsRead();
	int GetNbNew() const;
	int GetNbUnRead() const;

	std::vector<CNewsFeed*> GetChildrenRecursive();
	const CNewsFeed* FindFeedByUniqueId(const std::string& InUniqueId, bool bRecursive = true) const;
	const CNewsEntry* FindEntryByUniqueId(const std::string& InUniqueId) const;

	void ParseOutlineElementFromOpml(tinyxml2::XMLElement* OutlineElement);
	void ClearCache();

private:
	void ReadRss(tinyxml2::XMLDocument* XmlDoc);
	void ReadAtom(tinyxml2::XMLDocument* XmlDoc);

	friend class CFeedList;
	ERssType::Type RssType;
	tinyxml2::XMLElement* XmlElement;		//weak ref
};

#endif
