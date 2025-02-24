#include "pch.h"
#include "StorjData.h"
#if __has_include("StorjData.g.cpp")
#include "StorjData.g.cpp"
#endif

namespace winrt::Storj_Tools::implementation
{
    StorjData::StorjData() 
    {
        dispatcher = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    }

    winrt::event_token StorjData::PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged.add(handler);
    }

    void StorjData::PropertyChanged(winrt::event_token const& token)
    {
        propertyChanged.remove(token);
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
        if (dataAdapter.GetNodesUpdateInterval() != value)
        {
			dataAdapter.SetNodesUpdateInterval(value);
            RaisePropertyChanged(L"UpdateNodeInterval");
            OutputDebugString(L"UpdateNodeInterval byl změněn\n");
        }
    }

    FLOAT StorjData::CheckNodeInterval()
    {
		return dataAdapter.GetCheckNodesInterval();
    }

    void StorjData::CheckNodeInterval(FLOAT value)
    {
        if (dataAdapter.GetCheckNodesInterval() != value)
        {
			dataAdapter.SetCheckNodesInterval(value);
            RaisePropertyChanged(L"CheckNodeInterval");
            OutputDebugString(L"CheckNodeInterval byl změněn\n");
        }
    }

    FLOAT StorjData::LogReductionInterval()
    {
		return dataAdapter.GetLogReductionInterval();
    }

    void StorjData::LogReductionInterval(FLOAT value)
    {
		if (dataAdapter.GetLogReductionInterval() != value)
		{
			dataAdapter.SetLogReductionInterval(value);
			RaisePropertyChanged(L"LogReductionInterval");
			OutputDebugString(L"LogReductionInterval byl změněn\n");
		}
    }

    FLOAT StorjData::LogReductionMaxSize()
    {
		return dataAdapter.GetMaxLogSize();
    }

    void StorjData::LogReductionMaxSize(FLOAT value)
    {
        if (dataAdapter.GetMaxLogSize() != value)
        {
			dataAdapter.SetMaxLogSize(value);
            RaisePropertyChanged(L"LogReductionMaxSize");
            OutputDebugString(L"LogReductionMaxSize byl změněn\n");
        }
    }

    FLOAT StorjData::LogReducedSize()
    {
		return dataAdapter.GetLogReducedSize();
    }

    void StorjData::LogReducedSize(FLOAT value)
    {
		if (dataAdapter.GetLogReducedSize() != value)
		{
			dataAdapter.SetLogReducedSize(value);
			RaisePropertyChanged(L"LogReducedSize");
			OutputDebugString(L"LogReducedSize byl změněn\n");
		}
    }

    hstring StorjData::BotToken()
    {
		return StringToHstring(dataAdapter.GetDiscordBotToken());
    }

    void StorjData::BotToken(hstring value)
    {
		if (StringToHstring(dataAdapter.GetDiscordBotToken()) != value)
		{
			dataAdapter.SetDiscordBotToken(HstringToString(value)); 
			RaisePropertyChanged(L"BotToken");
			OutputDebugString(L"BotToken byl změněn\n");
		}
    }

    hstring StorjData::UserID()
    {
		return StringToHstring(dataAdapter.GetDiscordUserID());
    }

    void StorjData::UserID(hstring value)
    {
        if (StringToHstring(dataAdapter.GetDiscordUserID()) != value)
        {
			dataAdapter.SetDiscordUserID(HstringToString(value));
            RaisePropertyChanged(L"UserID");
            OutputDebugString(L"UserID byl změněn\n");
        }
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

}
