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

#include "CUiReaderModeBrowser.h"

#include "ui/CUiText.h"
#include "ui/CUiButton.h"
#include "ui/CUiSpacer.h"

#include "CHtmlParsing.h"
#include "CApp.h"
#include "CDownload.h"

#include "HtmlEntities.h"

#include "gumbo.h"
#include "deque"

CUiReaderModeBrowser::CUiReaderModeBrowser() : CUiVerticalBox()
{
	bEnabled = true;
	Feed = nullptr;
	bIsDownloading = false;
	Feed = NULL;
	RefreshButton = NULL;
	BackButton = NULL;
	bFillWidth = true;

	SubVerticalBox = new CUiVerticalBox();
	SubVerticalBox->bSupportMultiplePages = true;

	DebugName = "UiReaderModeBrowser";
}

void CUiReaderModeBrowser::ShowUrl(const std::string& InUrl, int InPageIndex, bool bForceDownload, CNewsFeed* InFeed)
{
	if (bIsDownloading)
		return;

	Url = InUrl;
	Feed = InFeed;

	CDownload Download = CApp::GetReaderModeDownloadFor(Url, Feed);
	LocalFilePath = Download.GetFilePath();
	bIsDownloading = bForceDownload || !CApp::IsFileValid(LocalFilePath.c_str());

	CApp* App = CApp::Get();

	if (!HistoryUrls.empty())
	{
		HistoryUrls.top().PageIndex = GetPageIndex();
	}
	HistoryUrls.push(SReaderModeBrowserHistoryItem(InUrl, InPageIndex));
	SetPageIndex(InPageIndex);

	ClearChildren();

	CUiHorizontalBox* BarHBox = new CUiHorizontalBox();
	BarHBox->bFillWidth = true;
	BarHBox->SetBottomPadding(10);
	AddChild(BarHBox);

	if (!Url.empty())
	{
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		BarHBox->AddChild(HorizontalBox);
		CUiText* UrlText = CUiTextAllocator::New();
		UrlText->Font = App->AppSettings.ReallySmallFont;
		UrlText->Text = Url;
		HorizontalBox->AddChild(UrlText);
	}

	if (bEnabled)
	{
		CUiHorizontalBox* HorizontalBox = new CUiHorizontalBox();
		HorizontalBox->PivotPointRatio.X = 1.f;
		BarHBox->AddChild(HorizontalBox);
		{
			if (!BackButton)
			{
				BackButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.EntryTextFontBold;
				Text->Text = "Back";
				BackButton->Child = Text;
				BackButton->OnPushFunction = std::tr1::bind(&CUiReaderModeBrowser::GoBack, this);
				BackButton->Visibility = EUiWidgetVisibility::Hidden;
				BackButton->SetPadding(0, 0, 20, 0);
			}
			else
			{
				BackButton->Visibility = EUiWidgetVisibility::Hidden;
				CApp::Get()->RedrawWidget(*BackButton);
			}
			HorizontalBox->AddChild(*BackButton);
		}
		
		{
			if (!RefreshButton)
			{
				RefreshButton = new CUiButton();
				CUiText* Text = CUiTextAllocator::New();
				Text->Font = App->AppSettings.EntryTextFontBold;
				Text->Text = "Refresh";
				RefreshButton->Child = Text;
				RefreshButton->OnPushFunction = std::tr1::bind(&CUiReaderModeBrowser::Refresh, this);
				RefreshButton->Visibility = EUiWidgetVisibility::Hidden;
			}
			else
			{
				RefreshButton->Visibility = EUiWidgetVisibility::Hidden;
				CApp::Get()->RedrawWidget(*RefreshButton);
			}
			
			HorizontalBox->AddChild(*RefreshButton);
		}
	}

	SubVerticalBox->ClearChildren();
	AddChild(*SubVerticalBox);

	if (!bIsDownloading)
	{
		std::cerr << "Reusing " << LocalFilePath << std::endl;
		OnDownloadFinished(false);
	}
	else
	{
		std::cerr << "Downloading " << InUrl << std::endl;
		
		Download.OnFinished = std::tr1::bind(&CUiReaderModeBrowser::OnDownloadFinished, this, true);
		CDownloadManager::Get()->AddDownload(Download, true);
	}
}

void CUiReaderModeBrowser::Refresh()
{
	if (bIsDownloading)
		return;

	if (!HistoryUrls.empty())
	{
		ShowUrl(HistoryUrls.top().Url, 0, true, Feed);
		HistoryUrls.pop();
	}
	else
	{
		ShowUrl(Url, 0, true, Feed);
	}

	if (RefreshButton)
	{
		RefreshButton->Visibility = EUiWidgetVisibility::Hidden;
		CApp::Get()->RedrawWidget(*RefreshButton);
	}

	if (BackButton)
	{
		BackButton->Visibility = EUiWidgetVisibility::Hidden;
		CApp::Get()->RedrawWidget(*BackButton);
	}
}

void CUiReaderModeBrowser::GoBack()
{
	if (bIsDownloading)
		return;

	if (!HistoryUrls.empty())
	{
		HistoryUrls.pop();
		if (!HistoryUrls.empty())
		{
			const SReaderModeBrowserHistoryItem TopItem = HistoryUrls.top();
			HistoryUrls.pop();
			ShowUrl(TopItem.Url, TopItem.PageIndex, false, Feed);
		}
	}
}

struct GumboNodeContext
{
	GumboNodeContext() : gumboNode(NULL), bInsideTextuallyRelevantTag(false), CoeffFont(1.f) {};
	GumboNodeContext(GumboNode* inGumboNode) : gumboNode(inGumboNode), CoeffFont(1.f), bInsideTextuallyRelevantTag(false) {};

	GumboNode* gumboNode;
	float CoeffFont;
	bool bInsideTextuallyRelevantTag;
};

void CUiReaderModeBrowser::OnDownloadFinished(bool bIsFromDownload)
{
	CApp* App = CApp::Get();
	bIsDownloading = false;
	if (bIsFromDownload)
	{
		//CloseProgressbar();
	}
	std::cerr << "Load file " << LocalFilePath.c_str() << std::endl;

	FILE* FileDesc = iv_fopen(LocalFilePath.c_str(), "rb");
	if (!FileDesc)
		return;

	fseek(FileDesc, 0, SEEK_END);
	size_t size = ftell(FileDesc);
	fseek(FileDesc, 0, SEEK_SET);

	if (size == 0) 
	{
		std::cerr << "Empty document error !" << std::endl;
		iv_fclose(FileDesc);
		return;
	}

	char* charBuffer = new char[size+1];
	size_t read = fread(charBuffer, 1, size, FileDesc);
	if (read != size)
	{
		std::cerr << "File read error !" << std::endl;
		iv_fclose(FileDesc);
		delete[] charBuffer;
		return;
	}

	charBuffer[size] = 0;
	iv_fclose(FileDesc);

	std::set<GumboTag> TextuallyRelevantTags = {
		GUMBO_TAG_ARTICLE, GUMBO_TAG_P, GUMBO_TAG_H1, GUMBO_TAG_H2, GUMBO_TAG_H3, GUMBO_TAG_H4, GUMBO_TAG_H5, GUMBO_TAG_H6
		, GUMBO_TAG_EM, GUMBO_TAG_STRONG, GUMBO_TAG_B, GUMBO_TAG_I, GUMBO_TAG_MARK, GUMBO_TAG_SMALL
		, GUMBO_TAG_DEL, GUMBO_TAG_INS
		, GUMBO_TAG_BLOCKQUOTE, GUMBO_TAG_Q, GUMBO_TAG_CITE, GUMBO_TAG_PRE, GUMBO_TAG_CODE, GUMBO_TAG_SAMP, GUMBO_TAG_VAR
		, GUMBO_TAG_UL, GUMBO_TAG_OL, GUMBO_TAG_DL };

	CUiText* LastUiText = nullptr;
	bool bAppendToLastUiText = false;
	bool bUseLastUIJustOnce = false;
	bool bPreprendToLastUiText = false;
	bool bHasDetectedFirstPhrase = false;

	GumboOutput* gumboOutput = gumbo_parse(charBuffer);
	std::deque<GumboNodeContext> NodesToDo = { GumboNodeContext(gumboOutput->root) };
	while (!NodesToDo.empty())
	{
		GumboNodeContext gumboNodeCtx = NodesToDo.front();
		NodesToDo.pop_front();

		GumboVector* Children = nullptr;
		switch (gumboNodeCtx.gumboNode->type)
		{
			case GUMBO_NODE_DOCUMENT:
			{
				Children = &gumboNodeCtx.gumboNode->v.document.children;
				break;
			}
			case GUMBO_NODE_ELEMENT:
			{
				Children = &gumboNodeCtx.gumboNode->v.element.children;

				if (TextuallyRelevantTags.find(gumboNodeCtx.gumboNode->v.element.tag) != TextuallyRelevantTags.end())
				{
					gumboNodeCtx.bInsideTextuallyRelevantTag = true;
				}

				bAppendToLastUiText = false;
				if ((gumboNodeCtx.bInsideTextuallyRelevantTag || (Feed && !Feed->ReaderModeSettings.bShowOnlyTextualRelevantTags)))
				{
					switch (gumboNodeCtx.gumboNode->v.element.tag)
					{
					case GUMBO_TAG_A:
					{
						std::string Link;
						for (unsigned int Index = 0; Index < gumboNodeCtx.gumboNode->v.element.attributes.length; ++Index)
						{
							GumboAttribute* Attribute = ((GumboAttribute**)gumboNodeCtx.gumboNode->v.element.attributes.data)[Index];
							if (strcmp(Attribute->name, "href") == 0)
							{
								Link = Attribute->value;
							}
						}

						if (Link.find("youtube.com/") != std::string::npos)
						{
							bAppendToLastUiText = true;
						}
						else if (Link.size() > 0 && Link.find("#") != 0)
						{
							CUiText* UiText = CUiTextAllocator::New();
							UiText->Font = App->AppSettings.EntryTextLinkFont;
							UiText->Text = " [" + Link + "]";
							LastUiText = UiText;
							bUseLastUIJustOnce = true;
							bPreprendToLastUiText = true;

							CUiButton* Button = new CUiButton();
							Button->Child = UiText;
							if (Link.find("http") != 0)
							{
								if (Link.find("/") == 0)
								{
									const size_t IndexOfProtocolStandard = Url.find("://");
									if (IndexOfProtocolStandard != std::string::npos)
									{
										Link = Url.substr(0, Url.find("/", IndexOfProtocolStandard + 3)) + Link;
									}
								}
								else
								{
									Link = Url + Link;
								}
							}
							if (bEnabled)
								Button->OnPushFunction = std::tr1::bind(&CUiReaderModeBrowser::ShowUrl, this, Link, 0, false, Feed);
							SubVerticalBox->AddChild(Button);
						}
						break;
					}
					case GUMBO_TAG_IMG: case GUMBO_TAG_SUP: case GUMBO_TAG_SUB:
					{
						gumboNodeCtx.bInsideTextuallyRelevantTag = false;
						break;
					}
					case GUMBO_TAG_H1: bHasDetectedFirstPhrase = true; gumboNodeCtx.CoeffFont = 1.5f; break;
					case GUMBO_TAG_H2: bHasDetectedFirstPhrase = true; gumboNodeCtx.CoeffFont = 1.4f; break;
					case GUMBO_TAG_H3: bHasDetectedFirstPhrase = true; gumboNodeCtx.CoeffFont = 1.3f; break;
					case GUMBO_TAG_H4: bHasDetectedFirstPhrase = true; gumboNodeCtx.CoeffFont = 1.2f; break;
					case GUMBO_TAG_H5: bHasDetectedFirstPhrase = true; gumboNodeCtx.CoeffFont = 1.1f; break;
					case GUMBO_TAG_H6: bHasDetectedFirstPhrase = true; gumboNodeCtx.CoeffFont = 1.05f; break;
					case GUMBO_TAG_P: bHasDetectedFirstPhrase = true; break;

					case GUMBO_TAG_EM: case GUMBO_TAG_STRONG: case GUMBO_TAG_B: case GUMBO_TAG_I: case GUMBO_TAG_MARK: case GUMBO_TAG_SMALL: case GUMBO_TAG_DEL: case GUMBO_TAG_INS:
					case GUMBO_TAG_CODE: case GUMBO_TAG_SAMP: case GUMBO_TAG_VAR: case GUMBO_TAG_UL: case GUMBO_TAG_OL: case GUMBO_TAG_DL:
						bAppendToLastUiText = true;
						break;
					}
				}

				break;
			}
			case GUMBO_NODE_TEXT:
			{
				// try to detect a the first phrase of the doc
				std::string Text = gumboNodeCtx.gumboNode->v.text.text;
				if (!bHasDetectedFirstPhrase)
				{
					bHasDetectedFirstPhrase = std::count(Text.begin(), Text.end(), ' ') > 2 && Text.find_first_of(".,!?");
				}

				if ((!Feed || !Feed->ReaderModeSettings.bHideElementsBeforeFirstPhrase || bHasDetectedFirstPhrase)
					&& (gumboNodeCtx.bInsideTextuallyRelevantTag || (Feed && !Feed->ReaderModeSettings.bShowOnlyTextualRelevantTags))
					)
				{
					if (!LastUiText)
					{
						bAppendToLastUiText = false;
						bPreprendToLastUiText = false;
						bUseLastUIJustOnce = false;
					}

					if (bAppendToLastUiText || bPreprendToLastUiText)
					{
						if (bPreprendToLastUiText)
						{
							LastUiText->Text = gumboNodeCtx.gumboNode->v.text.text + LastUiText->Text;
						}
						else
						{
							LastUiText->Text += gumboNodeCtx.gumboNode->v.text.text;
						}
						if (bUseLastUIJustOnce)
						{
							bPreprendToLastUiText = false;
							bAppendToLastUiText = false;
							bUseLastUIJustOnce = false;
						}
					}
					else
					{
						CUiText* UiText = CUiTextAllocator::New();
						if (gumboNodeCtx.CoeffFont > 1.f)
						{
							UiText->SetPadding(0, 5, 0, 5);
							UiText->Font = CUiFont(App->AppSettings.EntryTextFont.Font->name, App->AppSettings.EntryTextFont.Font->size * gumboNodeCtx.CoeffFont);
						}
						else
						{
							UiText->SetPadding(5, 0, 5, 5);
							UiText->Font = App->AppSettings.EntryTextFont;
						}
						UiText->Text = std::move(Text);
						SubVerticalBox->AddChild(UiText);
						LastUiText = UiText;
					}
				}
				break;
			}
		}

		if (Children && Children->data && Children->length > 0)
		{
			for (int Index = int(Children->length) - 1; Index >= 0; --Index)
			{
				GumboNode* Child = ((GumboNode**)Children->data)[Index];
				GumboNodeContext ChildCtx = gumboNodeCtx;
				ChildCtx.gumboNode = Child;
				NodesToDo.push_front(ChildCtx);
			}
		}
	}

	gumbo_destroy_output(&kGumboDefaultOptions, gumboOutput);
	delete[] charBuffer;

	CApp::Get()->Draw();
}

void CUiReaderModeBrowser::Draw(SUiDrawVisitor& DrawVisitor)
{
	if (RefreshButton)
		RefreshButton->Visibility = bIsDownloading ? EUiWidgetVisibility::Hidden : EUiWidgetVisibility::Visible;

	if (BackButton)
		BackButton->Visibility = bIsDownloading || HistoryUrls.size() <= 1 ? EUiWidgetVisibility::Hidden : EUiWidgetVisibility::Visible;

	CUiVerticalBox::Draw(DrawVisitor);
}
