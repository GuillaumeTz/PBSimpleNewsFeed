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

#include "ui/CUiVerticalBox.h"

#include "inkview.h"

#include <climits>

CUiVerticalBox::CUiVerticalBox() : CUiCompositeWidget()
{
	CurrentPage = 0;
	MaxPage = 1;
#ifndef IVSAPP
	MaxSize.Y = 1200;
#else
	MaxSize.Y = 600;
#endif
	bFill = true;
}

CUiVerticalBox::~CUiVerticalBox()
{

}

void CUiVerticalBox::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiCompositeWidget::CalcDesiredSize(AllowedSize);

	int MinHeight = MaxSize.Y > 0 ? CurrentPage * MaxSize.Y : 0;
	int MaxHeight = MaxSize.Y > 0 ? (CurrentPage + 1) * MaxSize.Y : INT_MAX;

	int CurrentHeight = 0;
	bool bHasDrawFirst = false;

	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();
		if (CurrentHeight >= MinHeight && (!bHasDrawFirst || (CurrentHeight /*+ Widget->DesiredSize.Y*/ <= MaxHeight)))
		{
			bHasDrawFirst = true;
			DesiredSize.X = std::max(DesiredSize.X, Widget->DesiredSize.X + Widget->GetPaddingAlongX());
			DesiredSize.Y += Widget->DesiredSize.Y + Widget->GetPaddingAlongY();
		}
		CurrentHeight += Widget->DesiredSize.Y + Widget->GetPaddingAlongY();
	}

	MaxPage = 1;
	if (MaxSize.Y > 0)
	{
		DesiredSize.Y = std::min(MaxSize.Y, DesiredSize.Y);
		MaxPage = (CurrentHeight / MaxSize.Y) + 1;
	}
}

void CUiVerticalBox::NextPage()
{
	CurrentPage = std::min(MaxPage - 1, CurrentPage + 1);
}

void CUiVerticalBox::PreviousPage()
{
	CurrentPage = std::max(0, CurrentPage - 1);
}

bool CUiVerticalBox::HasMultiplePages() const
{
	return MaxPage > 1;
}

bool CUiVerticalBox::CanNextPage() const
{
	return CurrentPage + 1 < MaxPage;
}

bool CUiVerticalBox::CanPreviousPage() const
{
	return CurrentPage > 0;
}

void CUiVerticalBox::Draw(SUiDrawVisitor& DrawVisitor)
{
	const SVector2i OriginalParentSize = DrawVisitor.ParentSize;
	const SVector2i OriginalLocation = DrawVisitor.AtLocation;
	const int MinHeight = MaxSize.Y > 0 ? CurrentPage * MaxSize.Y : 0;
	const int MaxHeight = MaxSize.Y > 0 ? (CurrentPage + 1) * MaxSize.Y : INT_MAX;
	const SVector2i ChildrenSize = DesiredSize;

	/*DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, DesiredSize.X, DesiredSize.Y, 0);
	DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y,OriginalParentSize.X, OriginalParentSize.Y, 128);*/
	//DrawRect(DrawVisitor.AtLocation.X, DrawVisitor.AtLocation.Y, OriginalParentSize.X, OriginalParentSize.Y, 128);

	const int OldStartHeight = DrawVisitor.StartHeight;
	DrawVisitor.StartHeight = 0;
	int CurrentHeight = -OldStartHeight;
	bool bHasDrawFirst = false;
	for (int Index = 0; Index < (*GetChildren()).size(); ++Index)
	{
		CUiWidget* Widget = (*GetChildren())[Index].Get();
		if (CurrentHeight >= MaxHeight)
			break;

		DrawVisitor.ParentSize.Y = Widget->DesiredSize.Y;
		if (bFill)
		{
			DrawVisitor.ParentSize.X = OriginalParentSize.X - Widget->GetPaddingAlongX();
		}
		else
		{
			DrawVisitor.ParentSize.X = ChildrenSize.X - Widget->GetPaddingAlongX();
		}

		if (CurrentHeight < MinHeight && CurrentHeight + Widget->DesiredSize.Y > MinHeight)
		{
			bHasDrawFirst = true;
			DrawVisitor.AtLocation.Y = OriginalLocation.Y + Widget->Padding.TopLeft.Y;
			DrawVisitor.AtLocation.X = OriginalLocation.X + Widget->Padding.TopLeft.X;

			DrawVisitor.AtLocation.X += Widget->PivotPointRatio.X * float(DrawVisitor.ParentSize.X - Widget->DesiredSize.X);
			DrawVisitor.ParentSize.X -= Widget->PivotPointRatio.X * float(DrawVisitor.ParentSize.X - Widget->DesiredSize.X);

			DrawVisitor.StartHeight = (MinHeight - CurrentHeight);
			//std::cerr << "DrawPart " << DrawVisitor.StartHeight << " " << CurrentHeight + DrawVisitor.StartHeight << " -> " << CurrentHeight +  Widget->DesiredSize.Y - DrawVisitor.StartHeight << " at " << DrawVisitor.AtLocation.Y << std::endl;
			Widget->Draw(DrawVisitor);
			DrawVisitor.StartHeight = 0;
		}
		else if (CurrentHeight >= MinHeight && (!bHasDrawFirst || (CurrentHeight /*+ Widget->DesiredSize.Y*/ <= MaxHeight)))
		{
			bHasDrawFirst = true;
			DrawVisitor.AtLocation.Y = OriginalLocation.Y + CurrentHeight - MinHeight + Widget->Padding.TopLeft.Y;
			DrawVisitor.AtLocation.X = OriginalLocation.X + Widget->Padding.TopLeft.X;

			DrawVisitor.AtLocation.X += Widget->PivotPointRatio.X * float(DrawVisitor.ParentSize.X - Widget->DesiredSize.X);
			DrawVisitor.ParentSize.X -= Widget->PivotPointRatio.X * float(DrawVisitor.ParentSize.X - Widget->DesiredSize.X);

			const int OldMaxAllowedHeight = DrawVisitor.MaxAllowedHeight;
			DrawVisitor.MaxAllowedHeight = MaxHeight - CurrentHeight;

			//std::cerr << "Draw " << CurrentHeight << " -> " << CurrentHeight +  std::min(DrawVisitor.MaxAllowedHeight, Widget->DesiredSize.Y) << " at " << DrawVisitor.AtLocation.Y << std::endl;
			Widget->Draw(DrawVisitor);
			DrawVisitor.MaxAllowedHeight = OldMaxAllowedHeight;
		}
		//else
		//{
		//	std::cerr << "CurrentHeight + Widget->DesiredSize.Y " << CurrentHeight + Widget->DesiredSize.Y << " at " << DrawVisitor.AtLocation.Y << std::endl;
		//}
		CurrentHeight += Widget->DesiredSize.Y + Widget->Padding.BottomRight.Y;
	}
	DrawVisitor.AtLocation = OriginalLocation;
	DrawVisitor.ParentSize = OriginalParentSize;
	DrawVisitor.StartHeight = OldStartHeight;
}