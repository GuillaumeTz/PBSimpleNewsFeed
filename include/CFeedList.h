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

#ifndef CFEEDLIST_H
#define CFEEDLIST_H

#include <string>
#include <vector>
#include <ctime>

#include "tinyxml2.h"

#include "CNewsFeed.h"
#include "CDownload.h"

class CFeedList
{
public:
	CFeedList();
	CFeedList(const CFeedList& Rhs) = delete;
	virtual ~CFeedList();

	CFeedList& operator=(const CFeedList& Rhs) = delete;

	CNewsFeed RootFeed;
	
	tinyxml2::XMLDocument* XmlDoc;

	void LoadFeeds(bool bForce);
	void LoadDocument(const std::string& InPath);
	void ParseOutlineElement(tinyxml2::XMLElement* OutlineElement, CNewsFeed* Parent, unsigned int Index);
	void SaveDocument(const std::string& InPath);
	const CNewsFeed* FindFeedByUniqueId(const std::string& InUniqueId, bool bRecursive = true) const;

	std::vector<CDownload> Sync();
};

#endif
