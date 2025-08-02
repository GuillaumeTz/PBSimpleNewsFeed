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

#ifndef CUIBUTTON_H
#define CUIBUTTON_H

#include "ui/CUiWidget.h"

class CUiButton : public CUiWidget
{
public:
	TSharedPtr<CUiWidget> Child;

	std::tr1::function<void (CUiButton* /* Button */)> OnPushFunction;
	std::tr1::function<void (CUiButton* /* Button */)> OnPushProlongedFunction;

public:
	CUiButton();
	virtual ~CUiButton();

	virtual void CalcDesiredSize(SVector2i AllowedSize) override;
	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;
	virtual void OnClick() override;
	
};

#endif
