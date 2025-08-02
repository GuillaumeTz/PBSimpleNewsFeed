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

#ifndef CUIMAINPAGE_H
#define CUIMAINPAGE_H

#include "ui/CUiOverlay.h"

class CUiText;
class CUiHorizontalBox;

class CUiMainPage : public CUiOverlay
{
public:
	CUiMainPage();

	void Refresh();
	void RefreshDownloadCounter();
	void SetMainElement(CUiWidget* Widget);
	CUiWidget* GetMainElement();
	void SetPath(const std::vector<int>& InFeedPath);

public:
	CUiText* DownloadCounterText;

protected:
	CUiHorizontalBox* BreadcrumbHBox;
};

#endif
