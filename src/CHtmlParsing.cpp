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

#include "CHtmlParsing.h"

#include "CApp.h"

#include "utf8.h"
#include "HtmlEntities.h"

#include <iostream>

void CHtmlParsing::SkipTo(const std::string& String, int& Index, char Character)
{
	for (; Index < String.size(); ++Index)
	{
		if (String[Index] == Character)
		{
			++Index;
			break;
		}
	}
}

void CHtmlParsing::SkipTo(const std::string& String, int& Index, const std::string& SkipToString)
{
	for (; Index < String.size(); ++Index)
	{
		if (AreNextCharactersEquals(String, Index, SkipToString, &Index))
		{
			break;
		}
	}
}

bool CHtmlParsing::AreNextCharactersEquals(const std::string& String, int Index, const std::string& EqualTo, int* OutIndex)
{
	int CurrentEqualToIndex = 0;
	for (; Index < String.size(); ++Index)
	{
		if (String[Index] == ' ')
			continue;

		if (String[Index] != EqualTo[CurrentEqualToIndex])
			return false;

		++CurrentEqualToIndex;
		if (CurrentEqualToIndex >= EqualTo.size())
		{
			if (OutIndex)
			{
				*OutIndex = Index + 1;
			}
			return true;
		}
	}
	return false;
}

EHtmlTag::Type CHtmlParsing::GetTagType(const std::string& String, int Index)
{
	if (String[Index] == '<')
		++Index;

	if (AreNextCharactersEquals(String, Index, "a")) return EHtmlTag::a;
	if (AreNextCharactersEquals(String, Index, "br")) return EHtmlTag::br;
	if (AreNextCharactersEquals(String, Index, "div")) return EHtmlTag::div;
	if (AreNextCharactersEquals(String, Index, "em")) return EHtmlTag::em;
	if (AreNextCharactersEquals(String, Index, "p")) return EHtmlTag::p;
	if (AreNextCharactersEquals(String, Index, "img")) return EHtmlTag::img;
	if (AreNextCharactersEquals(String, Index, "span")) return EHtmlTag::span;
	if (AreNextCharactersEquals(String, Index, "h1")) return EHtmlTag::h1;
	if (AreNextCharactersEquals(String, Index, "h2")) return EHtmlTag::h2;
	if (AreNextCharactersEquals(String, Index, "h3")) return EHtmlTag::h3;
	if (AreNextCharactersEquals(String, Index, "h4")) return EHtmlTag::h4;
	if (AreNextCharactersEquals(String, Index, "h5")) return EHtmlTag::h5;

	return EHtmlTag::Unknown;
}

void CHtmlParsing::FixInvalidUTF8(std::string& String)
{
	try
	{
		if (!utf8::is_valid(String.begin(), String.end()))
		{
			//std::cerr << "Fix invalid utf8 : ";
			std::string Temp;
			utf8::replace_invalid(String.begin(), String.end(), std::back_inserter(Temp));
			String = Temp;
			
			//std::cerr << String << std::endl;
		}
	}
	catch (...)
	{
		std::cerr << "FixInvalidUTF8 eception" << std::endl;
	}
}

void CHtmlParsing::DecoteHtmlEntities(std::string& String)
{
	std::string Temp;
	Temp.resize(String.size());
	decode_html_entities_utf8(&Temp[0], &String[0]);
	String = Temp;
}

void CHtmlParsing::CleanXmlValue(std::string& Str)
{
	CApp::ReplaceAll(Str, "\r\n", "");
	CApp::ReplaceAll(Str, "\n", "");
	FixInvalidUTF8(Str);
	DecoteHtmlEntities(Str);
}
