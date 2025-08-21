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

#include "inkview.h"

SUiDrawVisitor::SUiDrawVisitor()
{
	StartHeight = 0;
	MaxAllowedHeight = INT_MAX;
}

void SUiDrawVisitor::MarkHasDrawn(CUiWidget* InWidget, const SRect& DrawZone)
{
	DrawnZones[InWidget].ExtendTo(DrawZone);
	VisibleZones[InWidget].ExtendTo(DrawZone);
}

void SUiDrawVisitor::SetVisible(CUiWidget* InWidget, const SRect& VisibleZone)
{
	VisibleZones[InWidget].ExtendTo(VisibleZone);
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

CUiWidget::CUiWidget() : Parent(nullptr), PivotPointRatio(0.f, 0.f), Visibility(EUiWidgetVisibility::VisibleNotInteractable), bNeedRedraw(true), bFillWidth(false), bFillHeight(false), bIsPushed(false)
{
	DebugName = "Widget";
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

float CInkViewInterface::Scale = 1.f;
