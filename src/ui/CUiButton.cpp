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

#include "ui/CUiButton.h"

#include "inkview.h"

CUiButton::CUiButton() : CUiWidget()
{
	Visibility = EUiWidgetVisibility::Visible;
}

CUiButton::~CUiButton()
{

}

void CUiButton::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiWidget::CalcDesiredSize(AllowedSize);

	if (*Child)
	{
		Child->CalcDesiredSize(AllowedSize - Child->GetPaddingSize());
		DesiredSize += Child->DesiredSize + Child->GetPaddingSize();
	}
}

void CUiButton::Draw(SUiDrawVisitor& DrawVisitor)
{
	if (*Child && Visibility > EUiWidgetVisibility::Hidden)
	{
		const SVector2i Size = bFill ? DrawVisitor.ParentSize : DesiredSize;
		if (bIsPushed)
		{
			int OffsetSizeY = 5;
			DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, Size.X, Size.Y + OffsetSizeY, 128);
			DrawVisitor.HasDraw(this, SRect(DrawVisitor.AtLocation, DrawVisitor.AtLocation + Size + SVector2i(0, OffsetSizeY)));
		}
		Child->Draw(DrawVisitor);

		if (Visibility >= EUiWidgetVisibility::Visible)
		{
			//std::cerr << "IsInteractable" << this << std::endl;
			DrawVisitor.SetInteractable(this, SRect(DrawVisitor.AtLocation, DrawVisitor.AtLocation + Size));
		}
	}
}

void CUiButton::OnClick()
{
	if (OnPushFunction)
	{
		OnPushFunction(this);
	}
}
