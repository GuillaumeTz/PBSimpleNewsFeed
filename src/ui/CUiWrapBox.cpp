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

#include "ui/CUiWrapBox.h"
#include "ui/CUiText.h"

void CUiWrapBox::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiCompositeWidget::CalcDesiredSize(AllowedSize);

	LastChildEndOffset = SVector2i();
	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();
		if (LastChildEndOffset.X <= 0.001f)
		{
			Widget->CalcDesiredSize(AllowedSize);
			LastChildEndOffset = Widget->GetNextPositionForWrapBox();
		}
		else
		{
			const float RemainingSpaceX = AllowedSize.X - LastChildEndOffset.X;
			//if next widget is text we can cut it
			CUiText* Text = dynamic_cast<CUiText*>(Widget);
			if (Text)
			{

			}
			else
			{
				if (Widget->DesiredSize.X + Widget->GetPaddingAlongX() >= RemainingSpaceX)
				{
					//go to next line
				}
				else
				{
					//works !
					Widget->CalcDesiredSize(SVector2i(RemainingSpaceX, Widget->DesiredSize.Y));
				}
			}
		}
	}

	int MaxY = 0;
	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();
		MaxY = std::max(MaxY, Widget->DesiredSize.Y + Widget->GetPaddingAlongY());
		DesiredSize.X += Widget->DesiredSize.X + Widget->GetPaddingAlongX();
	}
	DesiredSize.Y += MaxY;
}

void CUiWrapBox::Draw(SUiDrawVisitor& DrawVisitor)
{
	const SVector2i OriginalParentSize = DrawVisitor.ParentSize;
	const SVector2i OriginalLocation = DrawVisitor.AtLocation;
	
	SVector2i ChildrenSize = DesiredSize;
	float SpacingRatio = 1.f;
	if (bFill)
	{
		SpacingRatio = float(DrawVisitor.ParentSize.X) / float(ChildrenSize.X);
	}

	//if (DrawVisitor.AtLocation.X > 10)
	{
		//DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DesiredSize.X, DesiredSize.Y, 0);
		//DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, OriginalParentSize.X, OriginalParentSize.Y, 0);
	}

	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();

		DrawVisitor.ParentSize.X = (Widget->DesiredSize.X + Widget->GetPaddingAlongX()) * SpacingRatio - Widget->GetPaddingAlongX();
		if (bFill)
		{
			DrawVisitor.ParentSize.Y = OriginalParentSize.Y - Widget->GetPaddingAlongY();
		}
		else
		{
			DrawVisitor.ParentSize.Y = ChildrenSize.Y - Widget->GetPaddingAlongY();
		}

		DrawVisitor.AtLocation.X += Widget->Padding.TopLeft.X;
		const SVector2i AtLocation = DrawVisitor.AtLocation;
		DrawVisitor.AtLocation.X += Widget->PivotPointRatio.X * float(DrawVisitor.ParentSize.X - Widget->DesiredSize.X);
		DrawVisitor.AtLocation.Y += Widget->Padding.TopLeft.Y + Widget->PivotPointRatio.Y * float(DrawVisitor.ParentSize.Y - Widget->DesiredSize.Y);
		DrawVisitor.ParentSize.X -= Widget->PivotPointRatio.X * float(DrawVisitor.ParentSize.X - Widget->DesiredSize.X);
		DrawVisitor.ParentSize.Y -= Widget->PivotPointRatio.Y * float(DrawVisitor.ParentSize.Y - Widget->DesiredSize.Y);

		//DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DrawVisitor.ParentSize.X, DrawVisitor.ParentSize.Y, 0);
		Widget->Draw(DrawVisitor);

		DrawVisitor.AtLocation.X += DrawVisitor.ParentSize.X + Widget->Padding.BottomRight.X;
		DrawVisitor.AtLocation.Y = AtLocation.Y;
	}

	DrawVisitor.AtLocation = OriginalLocation;
	DrawVisitor.ParentSize = OriginalParentSize;
}