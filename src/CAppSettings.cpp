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

#include "CAppSettings.h"

#include "ui/CUiWidget.h"

#include "tinyxml2.h"
#include <sstream>
#include <algorithm>

CAppSettings::CAppSettings()
{
	Reset();
}

void CAppSettings::SetScale(float InNewScale)
{
	Scale = std::min(std::max(InNewScale, 0.25f), 4.f);

	MenuButtonFont = CUiFont(DEFAULTFONT, int(28.f * Scale));
	DefaultFont = CUiFont(DEFAULTFONT, int(20.f * Scale));
	FeedUnReadFont = CUiFont(DEFAULTFONTB, int(20.f * Scale));
	FeedReadFont = CUiFont(DEFAULTFONT, int(20.f * Scale));

	InterlineDateFont = CUiFont(DEFAULTFONT, int(16.f * Scale));

	EntryTitleFont = CUiFont(DEFAULTFONT, int(24.f * Scale));
	EntryTitleFontBold = CUiFont(DEFAULTFONTB, int(24.f * Scale));
	EntryTextFont = CUiFont(DEFAULTFONT, int(16.f * Scale));
	EntryTextFontBold = CUiFont(DEFAULTFONTB, int(16.f * Scale));
	EntryTextLinkFont = CUiFont(DEFAULTFONTB, int(16.f * Scale));
	ReallySmallFont = CUiFont(DEFAULTFONT, int(10.f * Scale));

	FeedPathFont = CUiFont(DEFAULTFONTI, int(16.f * Scale));
	SwitchViewButtonsFont = CUiFont(DEFAULTFONTB, int(16.f * Scale));

	ContextMenuFont = CUiFont(DEFAULTFONT, int(20.f * Scale));
}

void CAppSettings::Reset()
{
	MaxEntryToKeepByFeed = 50;
	PathToOPML = FLASHDIR "/Dropbox PocketBook/subscriptions.opml";
	PathToSavedOPML = APP_FOLDER "/savedOPML.xml";
	ConfigFilePath = APP_FOLDER "/config.xml";
	bSynchronizeAtStart = false;
	bHideFooterNavigation = false;
	ResolutionWidth = ScreenWidth();
	ResolutionHeight = ScreenHeight();
	Scale = 1.f;
#ifndef IVSAPP
	OffsetTop = 160;
#endif
	OffsetBottom = 0;

	icanvas* Canvas = GetCanvas();
	Scale = float(Canvas->width) / 600.f;
	OffsetTop = float(OffsetTop) * float(Canvas->height) / 1872.f; // I took my inkpad 3 as reference for the scaling
	
	SetScale(Scale);
}

void CAppSettings::LoadConfig()
{
	Reset();

	{
		iv_buildpath(ConfigFilePath.c_str());
		tinyxml2::XMLDocument XmlDoc;
		tinyxml2::XMLError Error = XmlDoc.LoadFile(ConfigFilePath.c_str());
		if (Error == tinyxml2::XML_NO_ERROR && XmlDoc.RootElement() != NULL)
		{
			std::cerr << "Load conf file " << std::endl;
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("PathToOPML"))
			{
				PathToOPML = Element->GetText();
				std::cerr << "PathToOPML " << PathToOPML << std::endl;
			}
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("MaxEntryToKeepByFeed"))
			{
				MaxEntryToKeepByFeed = atoi(Element->GetText());
				std::cerr << "MaxEntryToKeepByFeed " << MaxEntryToKeepByFeed << std::endl;
			}
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("ResolutionWidth"))
			{
				ResolutionWidth = atoi(Element->GetText());
				std::cerr << "ResolutionWidth " << ResolutionWidth << std::endl;
			}
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("ResolutionHeight"))
			{
				ResolutionHeight = atoi(Element->GetText());
				ResolutionHeight = std::max(ResolutionHeight, 640);
				std::cerr << "ResolutionHeight " << ResolutionHeight << std::endl;
			}
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("Scale"))
			{
				Scale = atof(Element->GetText());
				std::cerr << "Scale " << Scale << std::endl;
			}
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("bSynchronizeAtStart"))
			{
				bSynchronizeAtStart = atoi(Element->GetText()) != 0;
				std::cerr << "bSynchronizeAtStart " << bSynchronizeAtStart << std::endl;
			}
			if (tinyxml2::XMLElement* Element = XmlDoc.RootElement()->FirstChildElement("bHideFooterNavigation"))
			{
				bHideFooterNavigation = atoi(Element->GetText()) != 0;
				std::cerr << "bHideFooterNavigation " << bHideFooterNavigation << std::endl;
			}
		}
	}

	if (ResolutionWidth <= 0)
	{
		ResolutionWidth = ScreenWidth(); 
	}
	if (ResolutionHeight <= 0)
	{
		ResolutionHeight = ScreenHeight();
	}

	ResolutionWidth = std::max(ResolutionWidth, 480);
	ResolutionWidth = std::min(ResolutionWidth, ScreenWidth());
	ResolutionHeight = std::max(ResolutionHeight, 640);
	ResolutionHeight = std::min(ResolutionHeight, ScreenHeight());

	SetScale(Scale);

	icanvas* Canvas = GetCanvas();
	std::cerr << "Resolution : " << ResolutionWidth << " x " << ResolutionHeight << " scale " << Scale << std::endl;
	std::cerr << "Canvas size : " << Canvas->width << " x " << Canvas->height << " ClipX " << Canvas->clipx1 << " => " << Canvas->clipx2 << " ClipY" << Canvas->clipy1 << " = > " << Canvas->clipy2 << std::endl;
}

void CAppSettings::SaveConfig()
{
	iv_buildpath(ConfigFilePath.c_str());
	tinyxml2::XMLDocument XmlDoc;
	XmlDoc.InsertFirstChild(XmlDoc.NewElement("config"));

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("PathToOPML");
		Element->InsertFirstChild(XmlDoc.NewText(PathToOPML.c_str()));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("MaxEntryToKeepByFeed");
		std::stringstream stream;
		stream << MaxEntryToKeepByFeed;
		Element->InsertFirstChild(XmlDoc.NewText(stream.str().c_str()));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("ResolutionWidth");
		std::stringstream stream;
		stream << ResolutionWidth;
		Element->InsertFirstChild(XmlDoc.NewText(stream.str().c_str()));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("ResolutionHeight");
		std::stringstream stream;
		stream << ResolutionHeight;
		Element->InsertFirstChild(XmlDoc.NewText(stream.str().c_str()));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("Scale");
		std::stringstream stream;
		stream << Scale;
		Element->InsertFirstChild(XmlDoc.NewText(stream.str().c_str()));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("bSynchronizeAtStart");
		Element->InsertFirstChild(XmlDoc.NewText(bSynchronizeAtStart ? "1" : "0"));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	{
		tinyxml2::XMLElement* Element = XmlDoc.NewElement("bHideFooterNavigation");
		Element->InsertFirstChild(XmlDoc.NewText(bHideFooterNavigation ? "1" : "0"));
		XmlDoc.RootElement()->InsertEndChild(Element);
	}

	XmlDoc.SaveFile(ConfigFilePath.c_str());
}
