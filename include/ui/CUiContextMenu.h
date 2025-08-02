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

#ifndef CUICONTEXTMENU_H
#define CUICONTEXTMENU_H

#include "ui/CUiVerticalBox.h"

class CUiContextMenu : public CUiVerticalBox
{
public:
	CUiContextMenu();

	std::vector<SUiContextMenuOption> ContextMenuOptions;
	bool bDirty;

	void ActivateOption(class CUiButton* Button, SUiContextMenuOption Option);

	virtual void CalcDesiredSize(SVector2i AllowedSize) override;
	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;
	virtual void OnLostFocusPath() override;
};

#endif
