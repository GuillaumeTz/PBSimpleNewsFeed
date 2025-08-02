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

#ifndef CAPPSETTINGS_H
#define CAPPSETTINGS_H

#include <iostream>

#include "inkview.h"
#include "ui/CUiFont.h"

#define APP_FOLDER CURRENTPROFILE "/SimpleNewsFeed"
#define CACHE_FOLDER APP_FOLDER "/Cache"

class CAppSettings
{
public:
	CAppSettings();

	CUiFont MenuButtonFont;

	CUiFont FeedPathFont;
	CUiFont SwitchViewButtonsFont;

	CUiFont DefaultFont;
	CUiFont FeedUnReadFont;
	CUiFont FeedReadFont;

	CUiFont InterlineDateFont;

	CUiFont EntryTitleFont;
	CUiFont EntryTitleFontBold;
	CUiFont EntryTextFont;
	CUiFont EntryTextFontBold;
	CUiFont EntryTextLinkFont;

	CUiFont ContextMenuFont;

	std::string PathToOPML;
	std::string PathToLastImportedOPML;
	std::string PathToSavedOPML;
	std::string ConfigFilePath;
	int MaxEntryToKeepByFeed;
	bool bSynchronizeAtStart;

public:
	void LoadConfig();
	void SaveConfig();
};


#endif
