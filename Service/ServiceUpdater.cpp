/*
 Soubor: ServiceUpdater.cpp
 Popis: Tøída pro aktualizaci služby
 Autor: Akutapi
 Datum: 11-1-2025
 */
#include "ServiceUpdater.h"
#include <filesystem>
#include <vss.h>
#include <vswriter.h>
#include <vsbackup.h>
#include <comdef.h>
#include <format>

#pragma comment(lib, "Vssapi.lib")
#pragma comment(lib, "Version.lib")

ServiceUpdater::ServiceUpdater(Logger& _logger) : logger(_logger)
{
}

ServiceUpdater::~ServiceUpdater()
{
}

// Vytvoøí kopii souboru pomocí VSS
bool ServiceUpdater::CreateShadowCopy(const std::wstring& sourceFilePath)
{
	// Získá cestu k souboru stínové kopie ( cíl kopírování )
	std::wstring shadowFilePath = GetShadowFilePath(sourceFilePath);
	
	// Zkopíruje soubor
	if (!std::filesystem::copy_file(sourceFilePath, shadowFilePath, std::filesystem::copy_options::overwrite_existing))
	{
		logger.LogError(L"Error while copying the file.");
		return false;
	}
	return true;
}

// Smaže stínovou kopii souboru
bool ServiceUpdater::DeleteShadowCopy(const std::wstring& sourceFilePath)
{
	if (!std::filesystem::remove(GetShadowFilePath(sourceFilePath)))
	{
		logger.LogError(L"Error while deleting shadow copy");
		return false;
	}
	return true;
}

// Aktualizuje službu
bool ServiceUpdater::UpdateService(const std::wstring& sourceFilePath, const std::wstring& fileToUpdatePath)
{
	if (!std::filesystem::copy_file(sourceFilePath, fileToUpdatePath, std::filesystem::copy_options::overwrite_existing))
	{
		logger.LogError(L"Error while updating file");
		return false;
	}
	return true;
}

bool ServiceUpdater::NeedUpdate(const std::wstring& sourceFilePath, const std::wstring& fileToUpdatePath) 
{
	return (GetFileVersion(sourceFilePath) > GetFileVersion(fileToUpdatePath));
}

std::wstring ServiceUpdater::GetShadowFilePath(const std::wstring& sourceFilePath) const
{
    std::filesystem::path path(sourceFilePath);
    path.replace_extension(L".shadow");
    return path.wstring();
}

std::wstring ServiceUpdater::GetStringFileVersion(const std::wstring& filePath)
{
	return GetFileVersion(filePath).ToString();
}

bool ServiceUpdater::IsVersionGreater(const std::wstring& filePath, const std::wstring& filePathOnther)
{
	return GetFileVersion(filePath) > GetFileVersion(filePathOnther);
}

ServiceUpdater::Version ServiceUpdater::GetFileVersion(const std::wstring& filePath)
{
	Version version;
	DWORD dummy;
	DWORD size = GetFileVersionInfoSize(filePath.c_str(), &dummy);

	if (size == 0)
	{
		logger.LogError(L"File information is empty.");
		return version;
	}

	BYTE* buffer = new BYTE[size];
	if (!GetFileVersionInfo(filePath.c_str(), 0, size, buffer))
	{
		logger.LogError(L"File information could not be read.");
		delete[] buffer;
		return version;
	}

	VS_FIXEDFILEINFO* fileInfo;
	UINT fileInfoSize;

	if (!VerQueryValue(buffer, L"\\", (LPVOID*)&fileInfo, &fileInfoSize))
	{
		logger.LogError(L"Reading file version failed.");
		delete[] buffer;
		return version;
	}

	version.major = HIWORD(fileInfo->dwFileVersionMS);
	version.minor = LOWORD(fileInfo->dwFileVersionMS);
	version.build = HIWORD(fileInfo->dwFileVersionLS);
	version.revision = LOWORD(fileInfo->dwFileVersionLS);
	delete[] buffer;
	return version;
}
