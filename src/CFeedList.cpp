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

CFeedList::CFeedList()
{
	RootFeed.bIsFolder = true;
	RootFeed.Title = "root";
	XmlDoc = NULL;
}

CFeedList::~CFeedList()
{
	delete XmlDoc;
}

void CFeedList::LoadDocument(const std::string& InPath)
{
	if (!XmlDoc)
	{
		XmlDoc = new tinyxml2::XMLDocument();
	}

	RootFeed.NewsFeeds.clear();
	XmlDoc->Clear();

	std::cerr << "Load feed list : " << InPath << std::endl;
	tinyxml2::XMLError Error = XmlDoc->LoadFile(InPath.c_str());
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
	unsigned int SubIndex = 0;
	for (tinyxml2::XMLElement* Element = tinyxml2::XMLHandle(OutlineElement).FirstChildElement("outline").ToElement(); Element != NULL; Element = Element->NextSiblingElement("outline"))
	{
		CNewsFeed NewsFeed;
		NewsFeed.bDeleted = (Element->GetAttribute("bDeleted", 0) == 1);
		NewsFeed.Url = Element->GetAttribute("xmlUrl", "");
		NewsFeed.Title = Element->GetAttribute("title", Element->GetAttribute("text", ""));
		NewsFeed.UniqueId = Element->GetAttribute("UniqueId");
		if (NewsFeed.UniqueId.empty())
		{
			NewsFeed.UniqueId = NewsFeed.Url;
		}
		if (NewsFeed.UniqueId.empty())
		{
			std::stringstream stream;
			stream << Index << "_" << NewsFeed.Title;
			NewsFeed.UniqueId = stream.str();
		}
		NewsFeed.bIsFolder = NewsFeed.Url.empty();
		NewsFeed.XmlElement = Element;
		NewsFeed.NbUnRead = Element->GetAttribute("NbUnRead", 0);
		NewsFeed.NbNew = Element->GetAttribute("NbNew", 0);
		NewsFeed.LastEntryTime = Element->GetAttribute("LastEntryTime", 0);
		NewsFeed.bDisplayLastEntryFirst = (Element->GetAttribute("bDisplayLastEntryFirst", 1) != 0);
		Parent->NewsFeeds.push_back(NewsFeed);
		if (NewsFeed.bIsFolder)
		{
			Parent->NewsFeeds.back().NewsFeeds.reserve(1000);
			ParseOutlineElement(Element, &Parent->NewsFeeds.back(), Index * 1000 + SubIndex);
		}
		SubIndex++;
	}
}

void CFeedList::SaveDocument(const std::string& InPath)
{
	RootFeed.SaveDocument();

	if (XmlDoc)
	{
		std::cerr << "Save feed list : " << InPath << std::endl;
		FILE* file = iv_fopen(InPath.c_str(), "w");
		XmlDoc->SaveFile(file);
		iv_fclose(file);
	}
}

std::vector<CDownload> CFeedList::Sync()
{
	return RootFeed.Sync();
}

std::vector<SFeedDiff> CFeedList::MakeDiffWith(const CFeedList& RhsFeedList) const
{

}
