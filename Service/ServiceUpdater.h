/*
 Soubor: ServiceUpdater.h
 Popis: Tøída pro aktualizaci služby
 Autor: Akutapi
 Datum: 11-1-2025
 */

#pragma once
#include "Logger.h"
class ServiceUpdater
{
public:

	struct Version
	{
		int major = 0;
		int minor = 0;
		int build = 0;
		int revision = 0;

		bool operator==(const Version& other) const
		{
			return major == other.major && minor == other.minor && build == other.build && revision == other.revision;
		}

		bool operator!=(const Version& other) const
		{
			return !(*this == other);
		}

		bool operator<(const Version& other)
		{
			if (major < other.major)
			{
				return true;
			}
			else if (major == other.major)
			{
				if (minor < other.minor)
				{
					return true;
				}
				else if (minor == other.minor)
				{
					if (build < other.build)
					{
						return true;
					}
					else if (build == other.build)
					{
						if (revision < other.revision)
						{
							return true;
						}
					}
				}
			}
			return false;
		}

		bool operator>(const Version& other)
		{
			return !(*this < other) && *this != other;
		}

		bool operator<=(const Version& other)
		{
			return *this < other || *this == other;
		}

		bool operator>=(const Version& other)
		{
			return *this > other || *this == other;
		}

		std::wstring ToString() const
		{
			return std::to_wstring(major) + L"." + std::to_wstring(minor) + L"." + std::to_wstring(build) + L"." + std::to_wstring(revision);
		}
	};
	
	ServiceUpdater(Logger& logger);
	~ServiceUpdater();
	bool CreateShadowCopy(const std::wstring& sourceFilePath);
	bool DeleteShadowCopy(const std::wstring& sourceFilePath);
	bool UpdateService(const std::wstring& sourceFilePath, const std::wstring& fileToUpdatePath);
	bool NeedUpdate(const std::wstring& sourceFilePath, const std::wstring& fileToUpdatePath);
	std::wstring GetStringFileVersion(const std::wstring& filePath);
	bool IsVersionGreater(const std::wstring& filePath, const std::wstring& filePathOnther);

private:
	std::wstring GetShadowFilePath(const std::wstring& sourceFilePath) const;
	Version GetFileVersion(const std::wstring& filePath);
	Logger logger;
};


