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

#include "tinyxml2.h"
#include <sstream>

CAppSettings::CAppSettings()
{
	MaxEntryToKeepByFeed = 200;
	PathToOPML = FLASHDIR "/Dropbox PocketBook/subscriptions.opml";
	PathToLastImportedOPML = APP_FOLDER "/lastImportedOPML.xml";
	PathToSavedOPML = APP_FOLDER "/savedOPML.xml";
	ConfigFilePath = APP_FOLDER "/config.xml";
	bSynchronizeAtStart = false;
}

void CAppSettings::LoadConfig()
{
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
		}
	}

	icanvas* Canvas = GetCanvas();
	float CoeffWidth = float(Canvas->width) / 600.f;

	MenuButtonFont = CUiFont(DEFAULTFONT, int(28.f * CoeffWidth));
	DefaultFont = CUiFont(DEFAULTFONT, int(20.f * CoeffWidth));
	FeedUnReadFont = CUiFont(DEFAULTFONTB, int(20.f * CoeffWidth));
	FeedReadFont = CUiFont(DEFAULTFONT, int(20.f * CoeffWidth));

	InterlineDateFont = CUiFont(DEFAULTFONT, int(16.f * CoeffWidth));

	EntryTitleFont = CUiFont(DEFAULTFONT, int(24.f * CoeffWidth));
	EntryTitleFontBold = CUiFont(DEFAULTFONTB, int(24.f * CoeffWidth));
	EntryTextFont = CUiFont(DEFAULTFONT, int(16.f * CoeffWidth));
	EntryTextFontBold = CUiFont(DEFAULTFONTB, int(16.f * CoeffWidth));
	EntryTextLinkFont = CUiFont(DEFAULTFONTB, int(16.f * CoeffWidth));

	FeedPathFont = CUiFont(DEFAULTFONTI, int(16.f * CoeffWidth));
	SwitchViewButtonsFont = CUiFont(DEFAULTFONTB, int(16.f * CoeffWidth));

	ContextMenuFont = CUiFont(DEFAULTFONT, int(20.f * CoeffWidth));
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

	XmlDoc.SaveFile(ConfigFilePath.c_str());
}
