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
	bMakePaddingInteractable = true;

	DebugName = "UiButton";
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

	if (bFillWidth)
		DesiredSize.X = AllowedSize.X;
	if (bFillHeight)
		DesiredSize.Y = AllowedSize.Y;
}

void CUiButton::Draw(SUiDrawVisitor& DrawVisitor)
{
	if (*Child && Visibility > EUiWidgetVisibility::Hidden)
	{
		if (bIsPushed)
		{
			SRect Offset;
			if (bMakePaddingInteractable)
			{
				Offset = GetPadding();
			}
			DrawRect(DrawVisitor.AtLocation.X - Offset.TopLeft.X, DrawVisitor.AtLocation.Y - Offset.TopLeft.Y, DesiredSize.X + Offset.Size().X, DesiredSize.Y + Offset.Size().Y, 128);
			DrawVisitor.SetVisible(this, SRect(DrawVisitor.AtLocation - Offset.TopLeft, DrawVisitor.AtLocation + DesiredSize + Offset.BottomRight));
		}

		const SVector2i AtLocation = DrawVisitor.AtLocation;
		DrawVisitor.AtLocation += Child->GetPadding().TopLeft;
		const int OldVisibleZoneNum = DrawVisitor.DrawnZones.size();
		Child->Draw(DrawVisitor);
		DrawVisitor.AtLocation = AtLocation;

		if (Visibility >= EUiWidgetVisibility::Visible && OldVisibleZoneNum != DrawVisitor.DrawnZones.size())
		{
			SRect Offset;
			if (bMakePaddingInteractable)
			{
				Offset = GetPadding();
			}

			auto ItFound = DrawVisitor.DrawnZones.find(Child.Get());
			if (ItFound != DrawVisitor.DrawnZones.end())
			{
				if (bFillWidth)
					DrawVisitor.SetInteractable(this, SRect(DrawVisitor.AtLocation.X - Offset.TopLeft.X, ItFound->second.TopLeft.Y - Offset.TopLeft.Y, 
						DrawVisitor.AtLocation.X + DrawVisitor.AllowedSize.X + Offset.BottomRight.X, ItFound->second.BottomRight.Y + Offset.BottomRight.Y));
				else if (bFillHeight)
					DrawVisitor.SetInteractable(this, SRect(ItFound->second.TopLeft.X, DrawVisitor.AtLocation.Y, ItFound->second.BottomRight.X, DrawVisitor.AtLocation.Y + DrawVisitor.AllowedSize.Y));
				else
					DrawVisitor.SetInteractable(this, SRect(ItFound->second.TopLeft - Offset.TopLeft, ItFound->second.BottomRight + Offset.Size()));
			}
			else
			{
				DrawVisitor.SetInteractable(this, SRect(DrawVisitor.AtLocation - Offset.TopLeft, DrawVisitor.AtLocation + DesiredSize + Offset.Size()));
			}
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
