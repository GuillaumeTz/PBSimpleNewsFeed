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

#ifndef CUINEWSFEEDBUTTON_H
#define CUINEWSFEEDBUTTON_H

#include "ui/CUiButton.h"

class CUiText;

class CUiNewsFeedButton : public CUiButton
{
public:
	void SetFeed(const std::vector<int>& FeedPath);
	void MarkAsRead();
	void Delete();
	void Refresh();

	virtual std::vector<SUiContextMenuOption> GetContextMenuOptions() override;
	virtual void DeleteItself() override;

private:
	std::vector<int> CurrentPath;
	CUiText* UiText;
};

class CUiNewsFeedButtonAllocator : public CStaticPoolAllocator<CUiNewsFeedButton>
{

};

#endif
