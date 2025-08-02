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

#include "ui/CUiWidget.h"

#include <climits>

void SRect::ExtendTo(const SVector2i& InPoint)
{
	if (!bIsValid)
	{
		bIsValid = true;
		TopLeft = BottomRight = InPoint;
	}
	else
	{
		TopLeft.X = std::min(TopLeft.X, InPoint.X);
		TopLeft.Y = std::min(TopLeft.Y, InPoint.Y);

		BottomRight.X = std::max(BottomRight.X, InPoint.X);
		BottomRight.Y = std::max(BottomRight.Y, InPoint.Y);
	}
}

void SRect::ExtendTo(const SRect& RhsRect)
{
	if (!bIsValid)
	{
		TopLeft = RhsRect.TopLeft;
		BottomRight = RhsRect.BottomRight;
		bIsValid = RhsRect.bIsValid;
	}
	else
	{
		TopLeft.X = std::min(TopLeft.X, RhsRect.TopLeft.X);
		TopLeft.Y = std::min(TopLeft.Y, RhsRect.TopLeft.Y);

		BottomRight.X = std::max(BottomRight.X, RhsRect.BottomRight.X);
		BottomRight.Y = std::max(BottomRight.Y, RhsRect.BottomRight.Y);
	}
}

bool SRect::Intersercts(const SRect& RhsRect) const
{
	if (!bIsValid || !RhsRect.bIsValid)
		return false;

	if ((TopLeft.X > RhsRect.BottomRight.X) || (RhsRect.TopLeft.X > BottomRight.X))
		return false;

	if ((TopLeft.Y > RhsRect.BottomRight.Y) || (RhsRect.TopLeft.Y > BottomRight.Y))
		return false;

	return true;
}

bool SRect::Contains(const SVector2i& Point) const
{
	if (!bIsValid)
		return false;

	return Point.X >= TopLeft.X && Point.X <= BottomRight.X && Point.Y >= TopLeft.Y && Point.Y <= BottomRight.Y;
}

void SRect::Clamp(const SVector2i& Min, const SVector2i& Max)
{
	if (!bIsValid)
		return;

	BottomRight.Clamp(Min, Max);
	TopLeft.Clamp(Min, BottomRight);
}

SUiDrawVisitor::SUiDrawVisitor()
{
	StartHeight = 0;
	MaxAllowedHeight = INT_MAX;
}

void SUiDrawVisitor::HasDraw(CUiWidget* InWidget, const SRect& DrawZone)
{
	DirtyZones[InWidget] = DrawZone;
	VisibleZones[InWidget] = DrawZone;
}

void SUiDrawVisitor::SetVisible(CUiWidget* InWidget, const SRect& VisibleZone)
{
	VisibleZones[InWidget] = VisibleZone;
}

void SUiDrawVisitor::SetInteractable(CUiWidget* InWidget, const SRect& VisibleZone)
{
	InteractableZones.push_back(std::make_pair(InWidget, VisibleZone));
}

void CUiCompositeWidget::Draw(SUiDrawVisitor& DrawVisitor)
{
	for (int Index = 0; Index < Children.size(); ++Index)
	{
		CUiWidget* Widget = Children[Index].Get();
		Widget->Draw(DrawVisitor);
	}
}

CUiCompositeWidget::~CUiCompositeWidget()
{
	ClearChildren();
}

void CUiCompositeWidget::ClearChildren()
{
	for (TSharedPtr<CUiWidget>& Widget : Children)
	{
		Widget->Parent = nullptr;
	}
	Children.clear();
}

void CUiCompositeWidget::AddChild(CUiWidget* Widget)
{
	Widget->Parent = this;
	Children.push_back(Widget);
}

void CUiCompositeWidget::RemoveChild(const CUiWidget* Widget)
{
	for (auto It = Children.begin(); It != Children.end(); ++It)
	{
		if (*It == Widget)
		{
			(*It)->Parent = nullptr;
			Children.erase(It);
			return;
		}
	}
}

void CUiCompositeWidget::RemoveChildAt(int Index)
{
	if (Index < 0 || Index >= Children.size())
		return;

	RemoveChild(*Children[Index]);
}

void CUiCompositeWidget::CalcDesiredSize(SVector2i AllowedSize)
{
	CUiWidget::CalcDesiredSize(AllowedSize);
	
	for (int Index = 0; Index < Children.size(); ++Index)
	{
		CUiWidget* Widget = Children[Index].Get();
		Widget->CalcDesiredSize(AllowedSize - Widget->GetPaddingSize());
	}
}

CUiWidget::CUiWidget() : Parent(nullptr), PivotPointRatio(0.f, 0.f), Visibility(EUiWidgetVisibility::VisibleNotInteractable), bNeedRedraw(true), bFill(false), bIsPushed(false)
{

}

CUiWidget::~CUiWidget()
{

}

std::vector<TSharedPtr<CUiWidget>> CUiWidget::GetWidgetPath()
{
	std::vector<TSharedPtr<CUiWidget>> Path;
	Path.push_back(this);
	while (Path[0]->Parent)
	{
		Path.insert(Path.begin(), Path[0]->Parent);
	}
	return Path;
}
