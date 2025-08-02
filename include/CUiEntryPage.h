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

#ifndef CUIENTRYPAGE_H
#define CUIENTRYPAGE_H

#include "ui/CUiVerticalBox.h"

#include "CNewsFeed.h"

class CUiText;

class CUiEntryPage : public CUiVerticalBox
{
public:
	CUiEntryPage();

	void SetEntry(const std::vector<int>& FeedPath, int EntryIndex);

	virtual void CalcDesiredSize(SVector2i AllowedSize) override;

private:
	void OnOpenExternalLink();
	void OnOpenInReaderMode();
	void RefreshReaderMode();
	void RefreshEnded();

private:
	std::vector<int> CurrentFeedPath;
	int CurrentEntryIndex;
	TSharedPtr<CUiText> PageNumText;
};

#endif
