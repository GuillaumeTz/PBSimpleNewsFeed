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

#ifndef CUINEWSENTRYBUTTON_H
#define CUINEWSENTRYBUTTON_H

#include "ui/CUiButton.h"

class CUiText;

class CUiNewsEntryButton : public CUiButton
{
public:
	CUiNewsEntryButton();
	void SetEntry(const std::vector<int>& FeedPath, int EntryIndex, const std::string& Tag = "");
	void MarkAsRead();
	void Delete();
	void Refresh();

	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;
	virtual std::vector<SUiContextMenuOption> GetContextMenuOptions() override;
	virtual void DeleteItself() override;

private:
	std::vector<int> CurrentPath;
	int CurrentEntryIndex;
	std::string CurrentTag;
	bool bHasBeenDrawn;

	CUiText* UiText;
};

class CUiNewsEntryButtonAllocator : public CStaticPoolAllocator<CUiNewsEntryButton>
{

};

#endif
