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

#ifndef CUIWIDGET_H
#define CUIWIDGET_H

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <tr1/functional>

#include "ui/CMemPool.h"
#include "ui/CMath.h"

class CUiWidget;
class SUiDrawVisitor
{
public:
	SUiDrawVisitor();

	SVector2i AtLocation;
	SVector2i AllowedSize;

	int StartHeight;
	int MaxAllowedHeight;

	std::map<CUiWidget*, SRect> DrawnZones;
	std::map<CUiWidget*, SRect> VisibleZones;
	std::vector<std::pair<CUiWidget*, SRect>> InteractableZones;

	void MarkHasDrawn(CUiWidget* InWidget, const SRect& DrawZone);
	void SetVisible(CUiWidget* InWidget, const SRect& VisibleZone);
	void SetInteractable(CUiWidget* InWidget, const SRect& VisibleZone);
};

namespace EUiWidgetVisibility
{
	enum Type
	{
		Collapsed = 0,
		Hidden,
		VisibleNotInteractable,
		Visible,
	};
}

struct SUiContextMenuOption
{
	std::string Text;
	std::tr1::function<void()> OnPushFunction;
};

class CUiWidget : public CReferenced
{
private:
	SRect Padding;

public:
	CUiWidget* Parent;
	SVector2i DesiredSize;
	SVector2f PivotPointRatio;
	EUiWidgetVisibility::Type Visibility;
	bool bNeedRedraw;
	bool bFillWidth;
	bool bFillHeight;

	std::string DebugName;

public:
	CUiWidget();
	virtual ~CUiWidget();

	void SetFixedPosition(const SVector2i& InPosition) { Padding.TopLeft = InPosition; Padding.BottomRight = SVector2i(0, 0); }
	void SetPadding(int Left, int Top, int Right, int Bottom);
	void SetBottomPadding(int Bottom);
	const SRect& GetPadding() const { return Padding; }

	SVector2i GetPaddingSize() const { return (GetPadding().TopLeft + GetPadding().BottomRight); }
	int GetPaddingAlongX() const { return (GetPadding().TopLeft.X + GetPadding().BottomRight.X); }
	int GetPaddingAlongY() const { return (GetPadding().TopLeft.Y + GetPadding().BottomRight.Y); }

	template<class T>
	std::vector<T*> FindChildrenRecursive()
	{
		std::vector<T*> Result;
		std::vector<CUiWidget*> WidgetToDo = {this};
		while (!WidgetToDo.empty())
		{
			CUiWidget* Widget = WidgetToDo.back();
			WidgetToDo.pop_back();
			T* CastWidget = dynamic_cast<T*>(*Widget);
			if (CastWidget)
				Result.push_back(CastWidget);

			const std::vector<TSharedPtr<CUiWidget>>* Children = GetChildren();
			if (!Children)
				continue;

			for (TSharedPtr<CUiWidget>& Child : *Children)
			{
				WidgetToDo.push_back(*Child);
			}
		}
		return Result;
	}
	std::vector<TSharedPtr<CUiWidget>> GetWidgetPath();

	virtual std::vector<TSharedPtr<CUiWidget>>* GetChildren() { return nullptr; }
	virtual void CalcDesiredSize(SVector2i AllowedSize) { DesiredSize.X = DesiredSize.Y = 0; };
	virtual void Draw(SUiDrawVisitor& DrawVisitor) { };
	virtual SVector2i GetNextPositionForWrapBox() const { return DesiredSize; };

	virtual void SetIsPushed(bool bInValue) { bIsPushed = bInValue; }
	virtual void OnClick() { }
	virtual void OnGainFocusPath() { }
	virtual void OnLostFocusPath() { }
	virtual void OnFocusPathChangedBelow() { }
	virtual std::vector<SUiContextMenuOption> GetContextMenuOptions() { return {}; };

	virtual bool CanNextPage() const { return false; }
	virtual void NextPage() {}
	virtual bool CanPreviousPage() const { return false; }
	virtual void PreviousPage() {}
	virtual int GetPageIndex() const { return 0; }
	virtual void SetPageIndex(int InIndexPage) { }

	template<class ClassType>
	ClassType* As()
	{
		return dynamic_cast<ClassType*>(this);
	}

protected:
	bool bIsPushed;
};

class CUiCompositeWidget : public CUiWidget
{
public:
	virtual ~CUiCompositeWidget();
	virtual void ClearChildren();

	void AddChild(CUiWidget* Widget);
	void ReplaceChildAt(int Index, CUiWidget* NewWidget);
	void RemoveChild(const CUiWidget* Widget);
	void RemoveChildAt(int Index);

public:
	virtual std::vector<TSharedPtr<CUiWidget>>* GetChildren() { return &Children; }
	virtual void CalcDesiredSize(SVector2i AllowedSize) override;
	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;

private:
	std::vector<TSharedPtr<CUiWidget>> Children;
};

#endif
