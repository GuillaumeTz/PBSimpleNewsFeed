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

#ifndef CUITEXT_H
#define CUITEXT_H

#include "ui/CUiWidget.h"
#include "ui/CUiFont.h"

namespace UiTextFlags
{
	enum Type
	{
		None = 0,
		Int,
		Float,
	};
}

class CUiText : public CUiWidget
{
public:
	CUiFont Font;
	std::string Text;
	int MaxWidth;
	UiTextFlags::Type Type;

	std::tr1::function<void(CUiText* /* Text */)> OnEditedFunction;

public:
	CUiText();

	void SetHtml(const std::string& InHtml);
	void SetEditable(bool bInEditable);

	virtual void CalcDesiredSize(SVector2i AllowedSize) override;
	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;
	virtual void DeleteItself() override;
	virtual void OnClick() override;

	void OnEdited(char* Text);

private:
	std::vector<int> CuttingTextIndex;
	bool bIsEditable;
};

class CUiTextAllocator : public CStaticPoolAllocator<CUiText>
{

};

#endif
