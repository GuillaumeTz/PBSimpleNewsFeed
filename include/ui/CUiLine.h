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

#ifndef CUILINE_H
#define CUILINE_H

#include "ui/CUiWidget.h"

class CUiLine : public CUiWidget
{
public:
	CUiLine();

	bool bVertical;

	virtual void CalcDesiredSize(SVector2i AllowedSize) override { DesiredSize = SVector2i(1); };
	virtual void Draw(SUiDrawVisitor& DrawVisitor) override;
	virtual void DeleteItself() override;
	
};

class CUiLineAllocator : public CStaticPoolAllocator<CUiLine>
{

};

#endif
