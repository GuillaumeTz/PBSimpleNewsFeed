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

#include "CDownload.h"

#include "CApp.h"

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <set>

CDownloadManager* CDownloadManager::DownloadManager = NULL;

CDownload::CDownload(const std::string& InUrl, const std::string& InOutFilePath, std::tr1::function<void (const CDownload&)> InOnFinished, int InTimeout /*= 10000*/)
{
	Url = InUrl;
	OutFilePath = InOutFilePath;
	OnFinished = InOnFinished;
	Timeout = InTimeout;
	NbRemainingTries = 2;

	bHasSucceded = false;
	bLaunched = false;
	bFinished = false;

	HostName = CApp::GetHostName(InUrl);
}

CDownload::CDownload(const std::string& InUrl, const std::string& InOutFilePath, int InTimeout /*= 10000*/)
{	
	Url = InUrl;
	OutFilePath = InOutFilePath;
	Timeout = InTimeout;
	NbRemainingTries = 2;

	bHasSucceded = false;
	bLaunched = false;
	bFinished = false;

	HostName = CApp::GetHostName(InUrl);
}

CDownload::~CDownload()
{
	Terminate();
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, std::stringstream* Stream)
{
	size_t result = 0;
	if (size * nmemb > 0)
	{
		Stream->write(ptr, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

void* CDownload::DownloadThread(void* Data)
{
	CDownloadWriter* Download = (CDownloadWriter*)Data;

	std::string Url = Download->Url;

	// Create curl handle
	CURL* curl = curl_easy_init();
			 
	if (curl)
	{
		char errorBuffer[CURL_ERROR_SIZE + 1];
		// Now set up all of the curl options
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, Url.c_str());
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Download->Buffer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 60000);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 10000);
		//if (post)
		//{
		//    curl_easy_setopt(curl, CURLOPT_POST,1);
		//    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,postparamstring.c_str());
		//}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,  2);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux i686; rv:78.0) Gecko/20100101 SimpleNewsFeed/0.1");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // this line makes it work under https
		//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, LIB_CURL_COOKIES_FILE);//read from
		//curl_easy_setopt(curl, CURLOPT_COOKIEJAR, LIB_CURL_COOKIES_FILE); //write to
			 
		// Attempt to retrieve the remote page
		//std::cerr << "Perform " << curl << std::endl;
		CURLcode result = curl_easy_perform(curl);

		if (result != 0)
		{
			//std::string Error = errorBuffer;
			std::cerr << "Curl error code " << result << " : " << errorBuffer << std::endl;
		}
			 
		// Always cleanup
		curl_easy_cleanup(curl);
	}
	else
	{
		std::cerr << "Curl initialization failed !" << std::endl;
	}

	return 0;
}

void CDownload::Launch()
{
	bLaunched = true;
	bHasSucceded = false;
	bFinished = false;

	DownloadWriter = new CDownloadWriter();
	DownloadWriter->Url = Url;

	if (pthread_create(&Thread, NULL, &CDownload::DownloadThread, (void*)*DownloadWriter) != 0)
	{
		std::cerr << "pthread_create error " << Url << std::endl;
		bHasSucceded = false;
		bFinished = true;
		Terminate();
	}
}

void CDownload::Check()
{
	if (bFinished || !bLaunched)
		return;

	int Result = pthread_tryjoin_np(Thread, 0);
	if (Result == EBUSY)
		return;

	if (Result == 0)
	{
		iv_buildpath(OutFilePath.c_str());
		std::ofstream File(OutFilePath.c_str());
		File << DownloadWriter->Buffer.rdbuf();

		Terminate();
		bHasSucceded = File.tellp() > 0;
		bFinished = true;
		return;
	}

	Terminate();
	bHasSucceded = false;
	bFinished = true;
	return;
}

void CDownload::Terminate()
{
	NbRemainingTries--;
	DownloadWriter.Reset();
}

CDownloadGroup::CDownloadGroup(const std::vector<CDownload>& InDownloads) : Downloads(InDownloads)
{

}

bool CDownloadGroup::IsFinished() const
{
	for (int Index = 0; Index < Downloads.size(); ++Index)
	{
		if (!Downloads[Index].IsFinished())
			return false;
	}
	return true;
}

CDownloadManager::CDownloadManager() : MaxParallelDownloads(16), MaxParallelDownloadByHostname(5)
{

}

CDownloadManager* CDownloadManager::Get()
{
	if (!DownloadManager)
	{
		DownloadManager = new CDownloadManager();
	}
	return DownloadManager;
}

void CDownloadManager::AddDownload(const CDownload& Download, bool bFirst)
{
	AddDownloadGroup(CDownloadGroup(Download), bFirst);
}

void CDownloadManager::AddDownloadGroup(const CDownloadGroup& DownloadGroup, bool bFirst)
{
	//check that wifi is connected
	if (!ConnectToWifi())
	{
		return;		
	}

	if (Downloads.empty())
	{
		SetHardTimer("DownloadManager", &CDownloadManager::TickHandler, 500);
	}

	if (bFirst)
	{
		Downloads.insert(Downloads.begin(), DownloadGroup);
	}
	else
	{
		Downloads.push_back(DownloadGroup);
	}
	RemoveDuplicates();
}

bool CDownloadManager::ConnectToWifi()
{
	int QueryNetworkRes = QueryNetwork();
	std::cerr << "Query network : " << QueryNetworkRes << "(QueryNetwork() & NET_CONNECTED) " << (QueryNetworkRes & NET_CONNECTED) << std::endl;
	if ((QueryNetworkRes & NET_CONNECTED) == 0)
	{
		NetConnect(0);
		int QueryNetworkRes = QueryNetwork();
		return (QueryNetworkRes & NET_CONNECTED) != 0;
	}
	return true;
}

void CDownloadManager::RemoveDuplicates()
{
	std::set<std::string> Urls;
	for (int IndexGroup = 0; IndexGroup < Downloads.size(); ++IndexGroup)
	{
		CDownloadGroup& Group(Downloads[IndexGroup]);
		for (std::vector<CDownload>::iterator It = Group.Downloads.begin(); It != Group.Downloads.end(); )
		{
			const CDownload& Download(*It);
			if (!Download.IsLaunched() && !Urls.insert(Download.GetFilePath()).second)
			{
				It = Group.Downloads.erase(It);
			}
			else
			{
				++It;
			}
		}
	}
}

bool CDownloadManager::Check()
{
	//std::cerr << " DownloadManager::Check " << std::endl;
	for (int IndexGroup = 0; IndexGroup < Downloads.size(); ++IndexGroup)
	{
		CDownloadGroup& Group(Downloads[IndexGroup]);
		for (int Index = 0; Index < Group.Downloads.size(); ++Index)
		{
			Group.Downloads[Index].Check();
		}
	}

	std::vector<CDownload> DownloadFinished;
	int RemainingSlots = MaxParallelDownloads;
	std::map<std::string, int> NbSlotByHostName;
	for (int IndexGroup = 0; IndexGroup < Downloads.size(); ++IndexGroup)
	{
		CDownloadGroup& Group(Downloads[IndexGroup]);

		for (int Index = 0; Index < Group.Downloads.size(); ++Index)
		{
			CDownload& Download(Group.Downloads[Index]);

			auto ItFound = NbSlotByHostName.find(Download.GetHostName());
			if (ItFound == NbSlotByHostName.end())
			{
				NbSlotByHostName.insert(std::make_pair(Download.GetHostName(), 0));
				ItFound = NbSlotByHostName.find(Download.GetHostName());
			}

			if (Download.IsFinished())
			{
				if (!Download.HasSucceeded())
				{
					std::cerr << Download.HasSucceeded() << " " << Download.GetUrl() << " : " << Download.GetFilePath() << std::endl;
					if (Download.GetNbRemainingTries() > 0)
					{
						if (!ConnectToWifi())
						{
							//stop all downloads
							Downloads.clear();
							return false;
						}

						//retry it
						Download.Launch();
						NbSlotByHostName[Download.GetHostName()] += 1;
						--RemainingSlots;
						continue;
					}
				}
				DownloadFinished.push_back(Download);
				continue;
			}

			if (RemainingSlots > 0 && ItFound->second < MaxParallelDownloadByHostname)
			{
				if (!Download.IsLaunched())
				{
					if (Download.OnStarted)
					{
						Download.OnStarted(Download);
					}
					Download.Launch();
				}
				--RemainingSlots;
				ItFound->second += 1;
				//std::cerr << "Slot by hostname " << Download.GetHostName() << " " << NbSlotByHostName[Download.GetHostName()] << std::endl;
			}
		}

		for (std::vector<CDownload>::iterator It = Group.Downloads.begin(); It != Group.Downloads.end(); )
		{
			if (It->IsFinished())
			{
				It = Group.Downloads.erase(It);
			}
			else
			{
				++It;
			}
		}
	}

	for (int Index = 0; Index < DownloadFinished.size(); ++Index)
	{
		CDownload& Download(DownloadFinished[Index]);

		if (Download.OnFinished)
		{
			Download.OnFinished(Download);
		}
	}

	for (int IndexGroup = 0; IndexGroup < Downloads.size(); ++IndexGroup)
	{
		CDownloadGroup& Group(Downloads[IndexGroup]);
		if (Group.IsFinished())
		{
			std::cerr << "Group finished : removing" << std::endl;
			std::tr1::function<void ()> OnGroupFinished = Group.OnGroupFinished;

			Downloads.erase(Downloads.begin() + IndexGroup);
			--IndexGroup;

			if (OnGroupFinished)
			{
				OnGroupFinished();
			}
			continue;
		}
	}

	return !Downloads.empty();
}

void CDownloadManager::Init()
{
	
}

void CDownloadManager::ClearAllDownloads()
{

}

void CDownloadManager::TickHandler()
{
	if (!CDownloadManager::Get()->Check())
	{
		ClearTimer(&CDownloadManager::TickHandler);
	}
	else
	{
		//Need to do this on reader
		SetHardTimer("DownloadManager", &CDownloadManager::TickHandler, 500);
	}
}
