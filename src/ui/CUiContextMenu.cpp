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

#include "ui/CUiContextMenu.h"

#include "CApp.h"

#include "ui/CUiButton.h"
#include "ui/CUiLine.h"
#include "ui/CUiText.h"

#include "inkview.h"

CUiContextMenu::CUiContextMenu() : CUiVerticalBox()
{
	bDirty = true;
	bFill = false;
	MaxSize.Y = -1;
}

void CUiContextMenu::ActivateOption( CUiButton* Button, SUiContextMenuOption Option)
{
	if (Option.OnPushFunction)
	{
		Option.OnPushFunction();
	}
	CApp::Get()->Viewport.PopOverlayWidget(this);
	CApp::Get()->Draw(true);
}

void CUiContextMenu::CalcDesiredSize(SVector2i AllowedSize)
{
	if (bDirty)
	{
		ClearChildren();
		bool bFirst = true;
		for (const SUiContextMenuOption& Option : ContextMenuOptions)
		{
			if (!bFirst)
			{
				CUiLine* Line = CUiLineAllocator::New();
				Line->bFill = true;
				Line->Padding.BottomRight.Y = 8;
				Line->Padding.TopLeft.Y = 8;
				AddChild(Line);
			}
			bFirst = false;

			CUiText* TextWidget = CUiTextAllocator::New();
			TextWidget->Text = Option.Text;
			TextWidget->Font = CApp::Get()->AppSettings.ContextMenuFont;

			CUiButton* Button = new CUiButton();
			Button->bFill = true;
			Button->Child = TextWidget;
			Button->OnPushFunction = std::tr1::bind(&CUiContextMenu::ActivateOption, this, std::tr1::placeholders::_1, Option);
			AddChild(Button);
		}
		bDirty = false;
	}

	CUiVerticalBox::CalcDesiredSize(AllowedSize);
}

void CUiContextMenu::Draw(SUiDrawVisitor& DrawVisitor)
{
	CalcDesiredSize(DrawVisitor.ParentSize);

	FillArea(DrawVisitor.AtLocation.X - 10, DrawVisitor.AtLocation.Y - 10, DesiredSize.X + 20, DesiredSize.Y + 20, 0x00FFFFFF);
	DrawRect(DrawVisitor.AtLocation.X - 10, DrawVisitor.AtLocation.Y - 10, DesiredSize.X + 20, DesiredSize.Y + 20, 0);

	const SVector2i OriginalParentSize = DrawVisitor.ParentSize;
	DrawVisitor.ParentSize = DesiredSize;
	CUiVerticalBox::Draw(DrawVisitor);
	DrawVisitor.ParentSize = OriginalParentSize;
}

void CUiContextMenu::OnLostFocusPath()
{
	CApp::Get()->Viewport.PopOverlayWidget(this);
	CApp::Get()->Draw(true);
}
