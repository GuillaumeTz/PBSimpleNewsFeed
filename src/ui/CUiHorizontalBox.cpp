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

#include "ui/CUiHorizontalBox.h"

#include "inkview.h"

CUiHorizontalBox::CUiHorizontalBox() : CUiCompositeWidget()
{
	DebugName = "HorizontalBox";
}

void CUiHorizontalBox::CalcDesiredSize(SVector2i AllowedSize)
{
	DesiredSize = SVector2i(0, 0);

	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		SVector2i ChildAllowedSize = AllowedSize;
		if (bFillWidth)
			ChildAllowedSize.X = AllowedSize.X / GetChildren()->size();

		CUiWidget* Widget = (*GetChildren())[Index].Get();
		Widget->CalcDesiredSize(ChildAllowedSize);

		DesiredSize.X += Widget->DesiredSize.X + Widget->GetPaddingAlongX();
		DesiredSize.Y = std::max(DesiredSize.Y, Widget->DesiredSize.Y + Widget->GetPaddingAlongY());
	}

	if (bFillWidth)
		DesiredSize.X = AllowedSize.X;
	
	if (bFillHeight)
		DesiredSize.Y = AllowedSize.Y;
}

void CUiHorizontalBox::Draw(SUiDrawVisitor& DrawVisitor)
{
	const SVector2i OriginalParentSize = DrawVisitor.AllowedSize;
	const SVector2i OriginalLocation = DrawVisitor.AtLocation;

	SVector2i ChildAllowedSize = DrawVisitor.AllowedSize;
	if (bFillWidth)
	{
		ChildAllowedSize.X = DrawVisitor.AllowedSize.X / GetChildren()->size();
	}

	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();

		DrawVisitor.AllowedSize = ChildAllowedSize;

		const SVector2i OldAtLocation = DrawVisitor.AtLocation;

		DrawVisitor.AtLocation += Widget->GetPadding().TopLeft;
		DrawVisitor.AllowedSize -= Widget->GetPaddingSize();

		DrawVisitor.AtLocation.X += Widget->PivotPointRatio.X * float(DrawVisitor.AllowedSize.X - Widget->DesiredSize.X);
		DrawVisitor.AtLocation.Y += Widget->PivotPointRatio.Y * float(DrawVisitor.AllowedSize.Y - Widget->DesiredSize.Y);

		Widget->Draw(DrawVisitor);

		DrawVisitor.AtLocation.X += Widget->DesiredSize.X + Widget->GetPadding().BottomRight.X;
		if (bFillWidth)
		{
			DrawVisitor.AtLocation.X = OldAtLocation.X + ChildAllowedSize.X;
		}
		DrawVisitor.AtLocation.Y = OldAtLocation.Y;
	}

	DrawVisitor.AtLocation = OriginalLocation;
	DrawVisitor.AllowedSize = OriginalParentSize;
}