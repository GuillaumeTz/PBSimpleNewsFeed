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

#include "CFeedList.h"

#include "datetimelite.h"

#include <iostream>

CFeedList::CFeedList()
{
	RootFeed.bIsFolder = true;
	RootFeed.Title = "root";
	XmlDoc = NULL;
}

CFeedList::~CFeedList()
{
	delete XmlDoc;
	XmlDoc = NULL;
}

void CFeedList::LoadFeeds(bool bForce)
{
	RootFeed.LoadFeeds(true, true);
}

void CFeedList::LoadDocument(const std::string& InPath)
{
	if (!XmlDoc)
	{
		XmlDoc = new tinyxml2::XMLDocument();
	}

	RootFeed.NewsFeeds.clear();

	std::cerr << "Load feed list : " << InPath << std::endl;
	FILE* file = iv_fopen(InPath.c_str(), "rb");
	if (!file)
		return;

	tinyxml2::XMLError Error = XmlDoc->LoadFile(file);
	iv_fclose(file);
	if (Error != tinyxml2::XML_NO_ERROR)
		return;

	RootFeed.NewsFeeds.reserve(1000);
	RootFeed.bDisplayLastEntryFirst = false; // keep order based on xml file
	RootFeed.UniqueId = "root";
	unsigned int Index = 0;
	if (tinyxml2::XMLElement* BodyElement = tinyxml2::XMLHandle(XmlDoc).FirstChildElement("opml").FirstChildElement("body").ToElement())
	{
		ParseOutlineElement(BodyElement, &RootFeed, Index);
		Index++;
	}
	std::cerr << "End Load feed list : " << InPath << std::endl;
}

void CFeedList::ParseOutlineElement(tinyxml2::XMLElement* OutlineElement, CNewsFeed* Parent, unsigned int Index)
{
	// std::cout << "Parsing outline elements";
	unsigned int SubIndex = 0;
	for (tinyxml2::XMLElement* Element = tinyxml2::XMLHandle(OutlineElement).FirstChildElement("outline").ToElement(); Element != NULL; Element = Element->NextSiblingElement("outline"))
	{
		CNewsFeed NewsFeed;
		NewsFeed.ParseOutlineElementFromOpml(Element);
		if (NewsFeed.UniqueId.empty())
		{
			std::stringstream stream;
			stream << Index << "_" << NewsFeed.Title;
			NewsFeed.UniqueId = stream.str();
		}
		Parent->NewsFeeds.push_back(NewsFeed);
		if (NewsFeed.bIsFolder)
		{
			Parent->NewsFeeds.back().NewsFeeds.reserve(1000);
			ParseOutlineElement(Element, &Parent->NewsFeeds.back(), Index * 1000 + SubIndex);
		}
		SubIndex++;
	}
	// std::cout << std::endl;
}

void CFeedList::SaveDocument(const std::string& InPath, bool bSaveAll)
{
	RootFeed.SaveDocument(bSaveAll);

	if (XmlDoc)
	{
		std::cerr << "Save feed list : " << InPath << std::endl;
		iv_unlink(InPath.c_str());
		FILE* file = iv_fopen(InPath.c_str(), "w");
		if (!file)
			return;

		XmlDoc->SaveFile(file);
		iv_fclose(file);
	}
}

const CNewsFeed* CFeedList::FindFeedByUniqueId(const std::string& InUniqueId, bool bRecursive /*= true*/) const
{
	if (RootFeed.UniqueId == InUniqueId)
		return &RootFeed;

	return RootFeed.FindFeedByUniqueId(InUniqueId, bRecursive);
}

void CFeedList::ClearCache()
{
	RootFeed.ClearCache();
}

std::vector<CDownload> CFeedList::Sync()
{
	return RootFeed.Sync();
}
