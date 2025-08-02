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

#include "ui/CUiHtmlMiniRender.h"

#include "ui/CUiText.h"
#include "ui/CUiSpacer.h"

#include "HtmlEntities.h"

#include <stack>
#include <iostream>
#include <sstream>

CUiHtmlMiniRender::CUiHtmlMiniRender() : CUiVerticalBox()
{
	MaxSize.X = 1000;
	MaxSize.Y = 800;
}

void CUiHtmlMiniRender::SetBodyHtml(const std::string& InHtml)
{
	Html = InHtml;
	ClearChildren();

	std::string Text;

	//std::cerr << "SetBodyHtml : " << InHtml << std::endl;
	std::stack<std::string> TagStack;
	std::vector<std::string> LastTags;
	std::string LastTag;
	for (int Index = 0; Index < InHtml.size(); ++Index)
	{
		char Character = InHtml[Index];
		if (Character == '<')
		{
			if (!Text.empty())
			{
				std::string DecodedValue;
				DecodedValue.resize(Text.size());
				decode_html_entities_utf8(&DecodedValue[0], &Text[0]);

				CUiText* UiText = CUiTextAllocator::New();
				UiText->Font = TextFont;
				UiText->Text = DecodedValue;
				AddChild(UiText);
				Text.clear();
			}

			for (; Index < InHtml.size(); ++Index)
			{
				if (InHtml[Index] == '>')
				{
					break;
				}
			}
		}
		else if (Character == '>')
		{
			
		}
		else
		{
			Text += Character;
		}
	}

	if (!Text.empty())
	{
		std::string DecodedValue;
		DecodedValue.resize(Text.size());
		decode_html_entities_utf8(&DecodedValue[0], &Text[0]);

		CUiText* UiText = CUiTextAllocator::New();
		UiText->Font = TextFont;
		UiText->Text = DecodedValue;
		AddChild(UiText);
	}
}
