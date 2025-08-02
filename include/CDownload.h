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

#ifndef CDOWNLOAD_H
#define CDOWNLOAD_H

#include "inkview.h"

#include <string>
#include <tr1/functional>
#include <vector>
#include <pthread.h>
#include <fstream>
#include <sstream>

#include "ui/CMemPool.h"

class CDownloadWriter : public CReferenced
{
public:
	std::string Url;
	std::stringstream Buffer;
};

class CDownload
{
public:
	CDownload(const std::string& InUrl, const std::string& InOutFilePath, int InTimeout = 10000);
	CDownload(const std::string& InUrl, const std::string& InOutFilePath, std::tr1::function<void (const CDownload&)> InOnFinished, int InTimeout = 10000);
	virtual ~CDownload();

	static void* DownloadThread(void* DownloadPtr);
	void Launch();
	void Check();
	void Terminate();

	int GetNbRemainingTries() const { return NbRemainingTries; }
	bool HasSucceeded() const { return bHasSucceded; }
	bool IsFinished() const { return bLaunched && bFinished; }
	bool IsLaunched() const { return bLaunched; }

	const std::string& GetUrl() const { return Url; }
	const std::string& GetFilePath() const { return OutFilePath; }
	const std::string& GetHostName() const { return HostName; }

	std::tr1::function<void (const CDownload&)> OnStarted;
	std::tr1::function<void (const CDownload&)> OnFinished;

private:
	std::string HostName;
	std::string Url;
	std::string OutFilePath;
	int Timeout;
	int NbRemainingTries;

	pthread_t Thread;
	TSharedPtr<CDownloadWriter> DownloadWriter;

	bool bHasSucceded;
	bool bLaunched;
	bool bFinished;
};

class CDownloadGroup
{
public:
	CDownloadGroup() { }
	CDownloadGroup(const CDownload& InDownload) { Downloads.push_back(InDownload); }
	CDownloadGroup(const std::vector<CDownload>& InDownloads);

	std::vector<CDownload> Downloads;

	std::tr1::function<void ()> OnGroupFinished;

	bool IsFinished() const;
};

class CDownloadManager
{
public:
	static CDownloadManager* Get();

	std::vector<CDownloadGroup> Downloads;

	void AddDownload(const CDownload& Download, bool bFirst);
	void AddDownloadGroup(const CDownloadGroup& DownloadGroup, bool bFirst);

	bool ConnectToWifi();
	void RemoveDuplicates();
	bool Check();
	void Init();
	void ClearAllDownloads();

	static void TickHandler();

	int MaxParallelDownloads;
	int MaxParallelDownloadByHostname;

private:
	CDownloadManager();

	static CDownloadManager* DownloadManager;
};

#endif
