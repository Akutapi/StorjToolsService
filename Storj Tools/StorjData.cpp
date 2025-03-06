#include "pch.h"
#include "StorjData.h"
#if __has_include("StorjData.g.cpp")
#include "StorjData.g.cpp"
#endif
#include <algorithm>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "App.xaml.h"

// dočasné jen pro testování
#include <random>
#include <ctime>

namespace winrt::Storj_Tools::implementation
{
	StorjData::StorjData()
	{
	}

    winrt::event_token StorjData::PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged.add(handler);
    }

    void StorjData::PropertyChanged(winrt::event_token const& token)
    {
        propertyChanged.remove(token);
    }

    winrt::Windows::Foundation::Collections::IObservableVector<Storj_Tools::Node> StorjData::Nodes()
    {
        return observableNodes;
    }

    void StorjData::UpdateNodes()
    {
		OutputDebugString(L"UpdateNodes byl vyžádán\n");
		std::vector<Storj_Tools::Node> vectorNodes = GetNodes();
		if (!isVectorsNodesEquals(vectorNodes, observableNodes))
		{
			observableNodes.Clear();
			for (const auto& node : vectorNodes)
			{
				observableNodes.Append(node);
			}
		}
    }

    void StorjData::StartNodeService(hstring nodeName)
    {
        std::wstring msg = HstringToWstring(nodeName);
        std::wstring debugMsg = L"StartService byl vyžádán pro uzel: " + msg + L"\n";
        OutputDebugString(debugMsg.c_str());
    }

    hstring StorjData::ServiceStatus()
    {
        OutputDebugString(L"ServiceStatus byl vyžádán\n");
		return WstringToHstring(dataAdapter.GetServiceStatus());
    }

    void StorjData::RestartService()
    {
        OutputDebugString(L"RestartService byl vyžádán\n");
        dataAdapter.RestartService();
    }

    void StorjData::SaveSettings()
    {
		dataAdapter.SaveSettings();
		OutputDebugString(L"SaveSetting byl vyžádán\n");
    }

    void StorjData::LoadSettings()
    {
		dataAdapter.LoadSettings();
		OutputDebugString(L"LoadSetting byl vyžádán\n");
    }

    FLOAT StorjData::UpdateNodeInterval()
    {
		return dataAdapter.GetNodesUpdateInterval();
    }

    void StorjData::UpdateNodeInterval(FLOAT value)
    {
		if (dataAdapter.GetNodesUpdateInterval() == value) return;
    	dataAdapter.SetNodesUpdateInterval(value);
        RaisePropertyChanged(L"UpdateNodeInterval");
        OutputDebugString(L"UpdateNodeInterval byl změněn\n");
    }

    FLOAT StorjData::CheckNodeInterval()
    {
		return dataAdapter.GetCheckNodesInterval();
    }

    void StorjData::CheckNodeInterval(FLOAT value)
    {
		if (dataAdapter.GetCheckNodesInterval() == value) return;
     	dataAdapter.SetCheckNodesInterval(value);
        RaisePropertyChanged(L"CheckNodeInterval");
        OutputDebugString(L"CheckNodeInterval byl změněn\n");
    }

    FLOAT StorjData::LogReductionInterval()
    {
		return dataAdapter.GetLogReductionInterval();
    }

    void StorjData::LogReductionInterval(FLOAT value)
    {
		if (dataAdapter.GetLogReductionInterval() == value) return;
		dataAdapter.SetLogReductionInterval(value);
		RaisePropertyChanged(L"LogReductionInterval");
		OutputDebugString(L"LogReductionInterval byl změněn\n");
    }

    FLOAT StorjData::LogReductionMaxSize()
    {
		return dataAdapter.GetMaxLogSize();
    }

    void StorjData::LogReductionMaxSize(FLOAT value)
    {
		if (dataAdapter.GetMaxLogSize() == value) return;
  		dataAdapter.SetMaxLogSize(value);
        RaisePropertyChanged(L"LogReductionMaxSize");
        OutputDebugString(L"LogReductionMaxSize byl změněn\n");
    }

    FLOAT StorjData::LogReducedSize()
    {
		return dataAdapter.GetLogReducedSize();
    }

    void StorjData::LogReducedSize(FLOAT value)
    {
		if (dataAdapter.GetLogReducedSize() == value) return;
		dataAdapter.SetLogReducedSize(value);
		RaisePropertyChanged(L"LogReducedSize");
		OutputDebugString(L"LogReducedSize byl změněn\n");
    }

    hstring StorjData::BotToken()
    {
		return StringToHstring(dataAdapter.GetDiscordBotToken());
    }

    void StorjData::BotToken(hstring value)
    {
		if (StringToHstring(dataAdapter.GetDiscordBotToken()) == value) return;
		dataAdapter.SetDiscordBotToken(HstringToString(value)); 
		RaisePropertyChanged(L"BotToken");
		OutputDebugString(L"BotToken byl změněn\n");
    }

    hstring StorjData::UserID()
    {
		return StringToHstring(dataAdapter.GetDiscordUserID());
    }

    void StorjData::UserID(hstring value)
    {
		if (StringToHstring(dataAdapter.GetDiscordUserID()) == value) return;
        dataAdapter.SetDiscordUserID(HstringToString(value));
        RaisePropertyChanged(L"UserID");
        OutputDebugString(L"UserID byl změněn\n");
    }

    void StorjData::SendTestMessageToDiscord()
    {
		dataAdapter.SendTestMessageToDiscord();
		OutputDebugString(L"SendTestMessageToDiscord byl vyžádán\n");
    }

    void StorjData::RaisePropertyChanged(hstring const& propertyName)
    {
        propertyChanged(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }

    std::vector<Storj_Tools::Node> StorjData::GetNodes()
    {
		// Dočasně naplněno dummy daty
		std::vector<Storj_Tools::Node> nodes;

        // Inicializace generátoru náhodných čísel
        std::random_device rd; // Pro získání náhodného seedu
        std::mt19937 gen(rd()); // Mersenne Twister engine, dobrý default
        std::uniform_int_distribution<> distrib(0, 1); // Rozdělení pro 0 nebo 1

        for (int i = 0; i < 5; i++)
        {
            Storj_Tools::Node node;
			node.Name(L"Node " + std::to_wstring(i));
			node.Path(L"C:\\Program Files\\Storj\\Node " + std::to_wstring(i));
            node.SVersion(L"1.0.0");
			// Náhodně nastavíme stav služby
            int randomNumber = distrib(gen); // Generování 0 nebo 1 náhodně
            node.DStatus((randomNumber == 0) ? SERVICE_RUNNING : SERVICE_STOPPED); // Pokud 0, SERVICE_RUNNING, jinak SERVICE_STOPPED
            nodes.push_back(node);
        }
        return nodes;
    }

    void StorjData::SortNodesByName(std::vector<Storj_Tools::Node>& nodes)
    {
        std::sort(nodes.begin(), nodes.end());
    }

    bool StorjData::isVectorsNodesEquals(const std::vector<Storj_Tools::Node>& nodes1, const winrt::Windows::Foundation::Collections::IObservableVector<Storj_Tools::Node> nodes2)
    {
		if (nodes1.size() != nodes2.Size()) return false;
        return std::equal(nodes1.begin(), nodes1.end(), nodes2.begin());
    }

    hstring StorjData::WstringToHstring(const std::wstring& wstring)
    {
        return winrt::hstring(wstring.c_str());
    }

    std::wstring StorjData::HstringToWstring(const hstring& hstring)
    {
        return std::wstring(hstring.c_str());
    }

    std::string StorjData::WstringToString(const std::wstring& wstring)
    {
        if (wstring.empty()) return "";
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0) return "";
        std::string str(size_needed - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
        return str;
    }

    std::wstring StorjData::StringToWstring(const std::string& string)
    {
        if (string.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);
        if (size_needed <= 0) return L"";
        std::wstring wstr(size_needed - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &wstr[0], size_needed);
        return wstr;
    }

    std::string StorjData::HstringToString(const winrt::hstring& hstring)
    {
		return WstringToString(HstringToWstring(hstring));
    }

    hstring StorjData::StringToHstring(const std::string& string)
    {
		return WstringToHstring(StringToWstring(string));
    }

    void StorjData::InitTheme()
    {
        auto settings = winrt::Windows::Storage::ApplicationData::Current().LocalSettings();
        auto value = settings.Values().TryLookup(L"AppThemeIsLight");

        if (value)
        {
            isLightingMode = winrt::unbox_value<bool>(value);
        }
        else
        {
            isLightingMode = true;
        }
        CurrentTheme(isLightingMode ? Microsoft::UI::Xaml::ElementTheme::Light : Microsoft::UI::Xaml::ElementTheme::Dark);
    }

    bool StorjData::IsLightMode()
    {
		return isLightingMode;
    }

    void StorjData::IsLightMode(bool value)
    {
		if (isLightingMode == value) return;
		isLightingMode = value;
        // Uložení do ApplicationData pro budoucí použití
        auto settings = winrt::Windows::Storage::ApplicationData::Current().LocalSettings();
        settings.Values().Insert(L"AppThemeIsLight", winrt::box_value(value));
		RaisePropertyChanged(L"IsLightMode");
		CurrentTheme(value ? Microsoft::UI::Xaml::ElementTheme::Light : Microsoft::UI::Xaml::ElementTheme::Dark);
    }

    Microsoft::UI::Xaml::ElementTheme StorjData::CurrentTheme()
    {
		return currentTheme;
    }

    void StorjData::CurrentTheme(Microsoft::UI::Xaml::ElementTheme value)
    {
		if (currentTheme == value) return;
		currentTheme = value;
		RaisePropertyChanged(L"CurrentTheme");
		SetTitleBar();
    }


    FLOAT StorjData::UpdateNodeUIInterval()
    {

		if (updateNodeUIInterval != 0) // Kontrola, zda je `updateNodeUIInterval` neinicializované
		{
			return updateNodeUIInterval;
		}

		auto settings = winrt::Windows::Storage::ApplicationData::Current().LocalSettings();
		auto value = settings.Values().TryLookup(L"UpdateNodeUIInterval");
        if (value)
        {
			updateNodeUIInterval = winrt::unbox_value<FLOAT>(value);
        }
        else
        {
	    	updateNodeUIInterval = 30;
        }
		RaisePropertyChanged(L"UpdateNodeUIInterval");
		return updateNodeUIInterval;
    }

    void StorjData::UpdateNodeUIInterval(FLOAT value)
    {
		if (updateNodeUIInterval == value) return;
		updateNodeUIInterval = value;
		RaisePropertyChanged(L"UpdateNodeUIInterval");
		auto settings = winrt::Windows::Storage::ApplicationData::Current().LocalSettings();
		settings.Values().Insert(L"UpdateNodeUIInterval", winrt::box_value(value));
    }

    void StorjData::SetTitleBar()
    {
        //nastavý barvy tilebaru

    }
}
