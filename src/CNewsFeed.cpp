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

#include "CNewsFeed.h"

#include "CDownload.h"
#include "CApp.h"

#include "CAppSettings.h"
#include "datetimelite.h"

#include "CHtmlParsing.h"

CNewsEntry::CNewsEntry()
{
	RssType = ERssType::Rss;
	bHasRead = false;
	bIsNew = true;
}

std::tm CNewsEntry::GetTime() const
{
	try
	{
		return datetimelite::time_from_string(Time);
	}
	catch (...)
	{
	}

	return std::tm();
}

tinyxml2::XMLElement* CNewsEntry::ToElement(tinyxml2::XMLDocument* XmlDoc)
{
	tinyxml2::XMLElement* Elem = NULL;

	if (RssType == ERssType::Rss)
	{
		Elem = XmlDoc->NewElement("item");
		Elem->InsertEndChild(XmlDoc->NewElement("description"))->ToElement()->InsertEndChild(XmlDoc->NewText(Text.c_str()));
		Elem->InsertEndChild(XmlDoc->NewElement("pubDate"))->ToElement()->InsertEndChild(XmlDoc->NewText(Time.c_str()));

		tinyxml2::XMLElement* LinkElement = Elem->InsertEndChild(XmlDoc->NewElement("link"))->ToElement();
		LinkElement->SetAttribute("href", Link.c_str());
	}
	else
	{
		Elem = XmlDoc->NewElement("entry");
		Elem->InsertEndChild(XmlDoc->NewElement("content"))->ToElement()->InsertEndChild(XmlDoc->NewText(Text.c_str()));
		Elem->InsertEndChild(XmlDoc->NewElement("updated"))->ToElement()->InsertEndChild(XmlDoc->NewText(Time.c_str()));

		tinyxml2::XMLElement* LinkElement = Elem->InsertEndChild(XmlDoc->NewElement("link"))->ToElement();
		LinkElement->InsertEndChild(XmlDoc->NewText(Link.c_str()));
	}

	Elem->InsertEndChild(XmlDoc->NewElement("title"))->ToElement()->InsertEndChild(XmlDoc->NewText(Title.c_str()));
	Elem->InsertEndChild(XmlDoc->NewElement("UniqueId"))->ToElement()->InsertEndChild(XmlDoc->NewText(UniqueId.c_str()));

	Elem->SetAttribute("IsNew", bIsNew);
	Elem->SetAttribute("HasRead", bHasRead);

	return Elem;
}

void CNewsEntry::ParseElement(tinyxml2::XMLElement* InXmlElement)
{
	Title = tinyxml2::XMLHandle(InXmlElement).FirstChildElement("title").GetText();
	if (RssType == ERssType::Rss)
	{
		Text = tinyxml2::XMLHandle(InXmlElement).FirstChildElement("description").GetText();
		Time = tinyxml2::XMLHandle(InXmlElement).FirstChildElement("pubDate").GetText();
	}
	else
	{
		Text = tinyxml2::XMLHandle(InXmlElement).FirstChildElement("content").GetText();
		Time =  tinyxml2::XMLHandle(InXmlElement).FirstChildElement("updated").GetText();
	}

	//fix invalid utf8 characters
	CHtmlParsing::CleanXmlValue(Title);
	CHtmlParsing::CleanXmlValue(Text);
	CHtmlParsing::CleanXmlValue(Time);

	if (tinyxml2::XMLElement* Elem = tinyxml2::XMLHandle(InXmlElement).FirstChildElement("link").ToElement())
	{
		Link = Elem->GetAttribute("href", Elem->GetText());
		CHtmlParsing::CleanXmlValue(Link);
	}
	std::string UniqueIdLocal = tinyxml2::XMLHandle(InXmlElement).FirstChildElement("UniqueId").GetText();
	CHtmlParsing::CleanXmlValue(UniqueIdLocal);
	if (!UniqueIdLocal.empty() && UniqueId.empty())
	{
		UniqueId = UniqueIdLocal;
	}
	else
	{
		UniqueId = Link;
		if (UniqueId.empty())
		{
			UniqueId = Time;
		}
		if (UniqueId.empty())
		{
			UniqueId = Title;
		}
	}

	InXmlElement->QueryBoolAttribute("IsNew", &bIsNew);
	InXmlElement->QueryBoolAttribute("HasRead", &bHasRead);

	ExternalLink = "";
	if (Link.find("www.reddit.com") != std::string::npos && Text.find("submitted by") <= 10)
	{
		//if reddit and no text this is an external link
		size_t SpanPos = Text.find("<span>");
		if (SpanPos != std::string::npos)
		{
			size_t HRefPos = Text.find("href=", SpanPos);
			if (HRefPos != std::string::npos && HRefPos + 5 < Text.size())
			{
				char CharDelim = Text[HRefPos + 5];
				size_t EndLinkPos = Text.find(CharDelim, HRefPos + 6);
				if (EndLinkPos != std::string::npos)
				{
					ExternalLink = Text.substr(HRefPos + 6, EndLinkPos - (HRefPos + 6));
				}
			}
		}
	}
}

void CNewsEntry::MarkAsRead()
{
	bIsNew = false;
	bHasRead = true;
}

CNewsFeed::CNewsFeed()
{
	bIsFolder = false;
	bDeleted = false;
	bIsLoaded = false;

	bDisplayLastEntryFirst = true;

	NbUnRead = 0;
	NbNew = 0;
	LastEntryTime = 0;
}

void CNewsFeed::LoadDocument(bool bForce)
{
	if (bIsFolder)
	{
		bIsLoaded = true;
		std::time_t LastChildItemTime = 0;
		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			NewsFeeds[Index].LoadDocument(bForce);
			LastChildItemTime = std::max(LastChildItemTime, NewsFeeds[Index].LastEntryTime);
		}
		if (LastChildItemTime != 0)
		{
			LastEntryTime = LastChildItemTime;
		}

		NbUnRead = GetNbUnRead();
		NbNew = GetNbNew();

		return;
	}

	if (bIsLoaded && !bForce)
	{
		return;
	}
	bIsLoaded = true;

	std::string LocalFilePath = GetLocalFilePath("");
	std::string FilePathDL = GetLocalFilePath("_dl");

	//if there is no main file then just load the dl
	if (!CApp::IsFileValid(LocalFilePath.c_str()))
	{
		LoadDocument(FilePathDL);
		iv_unlink(FilePathDL.c_str());
	}
	else
	{
		LoadDocument(LocalFilePath);

		//if filepath dl exists we need to merge it with the main file
		if (CApp::IsFileValid(FilePathDL.c_str()))
		{
			//load dl document
			CNewsFeed DLNewsFeed;
			DLNewsFeed.Url = Url;
			DLNewsFeed.LoadDocument(FilePathDL);

			//go over each entry and copy the new ones
			for (int NewIndex = DLNewsFeed.Entries.size() - 1; NewIndex >= 0; --NewIndex)
			{
				CNewsEntry Entry = DLNewsFeed.Entries[NewIndex];
				//search if exists
				bool bAlreadyExists = false;
				for (int ExistingIndex = 0; ExistingIndex < Entries.size(); ++ExistingIndex)
				{
					if (Entries[ExistingIndex].UniqueId == Entry.UniqueId)
					{
						bAlreadyExists = true;
						break;
					}
				}

				if (bAlreadyExists)
					continue;

				Entries.insert(Entries.begin(), Entry);
				//std::cerr << "New entry " << Entry.Title << " ! " << std::endl;
			}

			iv_unlink(FilePathDL.c_str());

			CApp* App = CApp::Get();
			if (Entries.size() > App->AppSettings.MaxEntryToKeepByFeed)
			{
				for (int Index = App->AppSettings.MaxEntryToKeepByFeed; Index < Entries.size(); ++Index)
				{
					//Need to delete cache files
					//std::cerr << "Deleting entry " << Entries[Index].Title << std::endl;
					//std::cerr << "Unlink file " << CApp::GetReaderModeDownloadFor(Entries[Index].Link, this).GetFilePath().c_str() << std::endl;
					iv_unlink(CApp::GetReaderModeDownloadFor(Entries[Index].Link, this).GetFilePath().c_str());
					if (!Entries[Index].ExternalLink.empty())
					{
						//std::cerr << "Unlink file " << CApp::GetReaderModeDownloadFor(Entries[Index].ExternalLink, this).GetFilePath().c_str() << std::endl;
						iv_unlink(CApp::GetReaderModeDownloadFor(Entries[Index].ExternalLink, this).GetFilePath().c_str());
					}
				}
				Entries.resize(App->AppSettings.MaxEntryToKeepByFeed);
			}
		}
	}

	std::time_t LocalLastItemTime = 0;
	for (int Index = 0; Index < Entries.size(); ++Index)
	{
		std::tm TimeTM = Entries[Index].GetTime();
		LocalLastItemTime = std::max(LocalLastItemTime, std::mktime(&TimeTM));
	}
	if (LocalLastItemTime != 0)
	{
		LastEntryTime = LocalLastItemTime;
	}

	NbUnRead = GetNbUnRead();
	NbNew = GetNbNew();
	SaveDocument();
}

void CNewsFeed::LoadDocument(const std::string& InFilePath)
{	
	std::cerr << "LoadDocument " << InFilePath << std::endl;

	Entries.clear();

	tinyxml2::XMLDocument XmlDoc;
	tinyxml2::XMLError Error = XmlDoc.LoadFile(InFilePath.c_str());
	if (Error != tinyxml2::XML_NO_ERROR)
		return;

	if (tinyxml2::XMLElement* TitleElement = XmlDoc.FirstChildElement("title"))
	{
		Title = TitleElement->GetText();
	}

	if (XmlDoc.FirstChildElement("feed"))
	{
		RssType = ERssType::Atom;
		ReadAtom(&XmlDoc);
	}
	else if (XmlDoc.FirstChildElement("rss"))
	{
		RssType = ERssType::Rss;
		ReadRss(&XmlDoc);
	}
}

void CNewsFeed::SaveDocument()
{
	if (XmlElement)
	{
		XmlElement->SetAttribute("NbUnRead", GetNbUnRead());
		XmlElement->SetAttribute("NbNew", GetNbNew());
		XmlElement->SetAttribute("LastEntryTime", int(LastEntryTime));
		XmlElement->SetAttribute("bDisplayLastEntryFirst", bDisplayLastEntryFirst);
		XmlElement->SetAttribute("UniqueId", UniqueId);
		if (bDeleted)
			XmlElement->SetAttribute("bDeleted", bDeleted);
	}

	if (bDeleted)
		return;

	if (bIsFolder)
	{
		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			NewsFeeds[Index].SaveDocument();
		}
	}
	else if (!Entries.empty())
	{
		if (!bIsLoaded)
			return;

		tinyxml2::XMLDocument XmlDoc;
		XmlDoc.InsertFirstChild(XmlDoc.NewDeclaration());

		tinyxml2::XMLElement* ParentElement = NULL;

		if (RssType == ERssType::Rss)
		{
			tinyxml2::XMLElement* RssElement = XmlDoc.NewElement("rss");
			ParentElement = XmlDoc.NewElement("channel");

			RssElement->InsertEndChild(ParentElement);
			XmlDoc.InsertEndChild(RssElement);
		}
		else
		{
			ParentElement = XmlDoc.NewElement("feed");
			XmlDoc.InsertEndChild(ParentElement);
		}

		for (int Index = 0; Index < Entries.size(); ++Index)
		{
			tinyxml2::XMLElement* Elem = Entries[Index].ToElement(&XmlDoc);
			//std::cerr << "InsertEndChild " << Entries[Index].Title << " !" << std::endl;
			ParentElement->InsertEndChild(Elem);
		}

		std::string LocalFilePath = GetLocalFilePath("");
		std::cerr << "Saving file to " << LocalFilePath << std::endl;
		FILE* file = iv_fopen(LocalFilePath.c_str(), "w");
		XmlDoc.SaveFile(file);
		iv_fclose(file);
	}
}

std::vector<CDownload> CNewsFeed::Sync()
{
	std::vector<CDownload> Downloads;
	if (bIsFolder)
	{
		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			std::vector<CDownload> NewDownloads = NewsFeeds[Index].Sync();
			Downloads.insert(Downloads.end(), NewDownloads.begin(), NewDownloads.end());
		}
	}
	else
	{
		std::string FixedUrl = Url;
		//fix for reddit
		if (FixedUrl.find("https://www.reddit.com/r/") != std::string::npos || FixedUrl.find("reddit.com/r/") != std::string::npos)
		{
			CApp::ReplaceAll(FixedUrl, "/.rss", "/new/.rss?limit=50");
		}
		Downloads.push_back(CDownload(FixedUrl, GetLocalFilePath("_dl"), 5000));
	}
	return Downloads;
}

std::vector<CDownload> CNewsFeed::AdditionalSync()
{
	std::vector<CDownload> Downloads;
	if (bIsFolder)
	{
		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			std::vector<CDownload> NewDownloads = NewsFeeds[Index].AdditionalSync();
			Downloads.insert(Downloads.end(), NewDownloads.begin(), NewDownloads.end());
		}
	}
	else
	{
		for (int Index = 0; Index < Entries.size(); ++Index)
		{
			const CNewsEntry& NewsEntry = Entries[Index];
			size_t ReadMoreLinkPos = NewsEntry.Text.rfind(">Read More");
			if ((NewsEntry.Link.find("www.reddit.com") != std::string::npos)
			|| (NewsEntry.Text.rfind("[...]") != std::string::npos)
			|| (NewsEntry.Link.find("twitter.com") == std::string::npos && NewsEntry.Text.size() < 512)
			|| (ReadMoreLinkPos != std::string::npos && ReadMoreLinkPos > NewsEntry.Text.size() - size_t(std::max(NewsEntry.Text.size() * 0.75f, 20.f))))
			{
				CDownload Download = CApp::GetReaderModeDownloadFor(NewsEntry.Link, this);
				//dont redownload same file
				if (!CApp::IsFileValid(Download.GetFilePath().c_str()))
				{
					Downloads.push_back(Download);
				}
			}

			if (!NewsEntry.ExternalLink.empty())
			{
				CDownload Download = CApp::GetReaderModeDownloadFor(NewsEntry.ExternalLink, this);
				//dont redownload same file
				if (!CApp::IsFileValid(Download.GetFilePath().c_str()))
				{
					Downloads.push_back(Download);
				}
			}
		}
	}
	return Downloads;
}

std::string CNewsFeed::GetLocalFilePath(const std::string& Suffix) const
{
	std::string Result = Url;
	CApp::ReplaceAll(Result, "http://", "");
	CApp::ReplaceAll(Result, "https://", "");
	size_t FirstPos = Result.find("/");
	if (FirstPos != std::string::npos)
	{
		std::string HostName = Result.substr(0, FirstPos);
		Result = Result.substr(FirstPos);
		CApp::ConvertToValidFilename(Result);
		Result = HostName + "/" + Result;
	}
	else
	{
		CApp::ConvertToValidFilename(Result);
	}
	Result = CACHE_FOLDER "/" + Result + Suffix + ".xml";
	return Result;
}

void CNewsFeed::MarkAsRead()
{
	NbUnRead = 0;
	NbNew = 0;

	for (int Index = 0; Index < NewsFeeds.size(); ++Index)
	{
		NewsFeeds[Index].MarkAsRead();
	}

	for (int Index = 0; Index < Entries.size(); ++Index)
	{
		Entries[Index].MarkAsRead();
	}
}

void CNewsFeed::MarkAsDeleted()
{
	bDeleted = true;
}

int CNewsFeed::GetNbNew() const
{
	if (!IsLoaded())
	{
		return NbNew;
	}

	int LocalNbNew = 0;
	if (bIsFolder)
	{
		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			LocalNbNew += NewsFeeds[Index].GetNbNew();
		}
	}
	else
	{
		for (int Index = 0; Index < Entries.size(); ++Index)
		{
			LocalNbNew += Entries[Index].bIsNew ? 1 : 0;
		}
	}
	return LocalNbNew;
}

int CNewsFeed::GetNbUnRead() const
{
	int LocalNbUnread = 0;
	if (bIsFolder)
	{
		if (NewsFeeds.empty())
			return NbUnRead;

		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			LocalNbUnread += NewsFeeds[Index].GetNbUnRead();
		}
	}
	else
	{
		if (Entries.empty())
			return NbUnRead;

		for (int Index = 0; Index < Entries.size(); ++Index)
		{
			LocalNbUnread += !Entries[Index].bHasRead ? 1 : 0;
		}
	}
	return LocalNbUnread;
}

std::vector<CNewsFeed*> CNewsFeed::GetChildrenRecursive()
{
	std::vector<CNewsFeed*> Result;
	if (bIsFolder)
	{
		for (int Index = 0; Index < NewsFeeds.size(); ++Index)
		{
			std::vector<CNewsFeed*> NewResult = NewsFeeds[Index].GetChildrenRecursive();
			Result.insert(Result.end(), NewResult.begin(), NewResult.end());
		}
	}
	else
	{
		Result.push_back(this);
	}
	return Result;
}

void CNewsFeed::ReadRss(tinyxml2::XMLDocument* XmlDoc)
{
	//std::cerr << "ReadRss" << std::endl;
	for (tinyxml2::XMLElement* Element = tinyxml2::XMLHandle(XmlDoc).FirstChildElement("rss").FirstChildElement("channel").ToElement()->FirstChildElement("item"); Element != NULL; Element = Element->NextSiblingElement("item"))
	{
		//std::cerr << "AddEntry Rss " << std::endl;
		CNewsEntry Entry;
		Entry.RssType = ERssType::Rss;
		Entry.ParseElement(Element);
		if (Entry.Link.find("http://") == std::string::npos && Entry.Link.find("https://") == std::string::npos)
		{
			Entry.Link = Url + "/" + Entry.Link;
		}
		Entries.push_back(Entry);
	}
}

void CNewsFeed::ReadAtom(tinyxml2::XMLDocument* XmlDoc)
{
	//std::cerr << "ReadAtom" << std::endl;
	for (tinyxml2::XMLElement* Element = tinyxml2::XMLHandle(XmlDoc).FirstChildElement("feed").ToElement()->FirstChildElement("entry"); Element != NULL; Element = Element->NextSiblingElement("entry"))
	{
		//std::cerr << "AddEntry Atom " << std::endl;
		CNewsEntry Entry;
		Entry.RssType = ERssType::Atom;
		Entry.ParseElement(Element);
		if (Entry.Link.find("http://") == std::string::npos && Entry.Link.find("https://") == std::string::npos)
		{
			Entry.Link = Url + "/" + Entry.Link;
		}
		Entries.push_back(Entry);
	}
}
