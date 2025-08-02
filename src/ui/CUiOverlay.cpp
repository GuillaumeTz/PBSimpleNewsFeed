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

#include "ui/CUiOverlay.h"

#include "inkview.h"

void CUiOverlay::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiCompositeWidget::CalcDesiredSize(AllowedSize);

	SVector2i Max(0);
	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();
		Max.X = std::max(Max.X, Widget->DesiredSize.X + Widget->GetPaddingAlongX());
		Max.Y = std::max(Max.Y, Widget->DesiredSize.Y + Widget->GetPaddingAlongY());
	}
	DesiredSize = Max;
}

void CUiOverlay::Draw(SUiDrawVisitor& DrawVisitor)
{
	const SVector2i OriginalParentSize = DrawVisitor.ParentSize;
	const SVector2i OriginalLocation = DrawVisitor.AtLocation;

	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();

		DrawVisitor.AtLocation = OriginalLocation + Widget->Padding.TopLeft;
		DrawVisitor.ParentSize = OriginalParentSize - Widget->GetPaddingSize();

		Widget->Draw(DrawVisitor);
	}

	DrawVisitor.AtLocation = OriginalLocation;
	DrawVisitor.ParentSize = OriginalParentSize;
}