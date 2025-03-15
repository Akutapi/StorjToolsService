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


using namespace std;
using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Data;
using namespace Microsoft::UI::Xaml::Media;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;

namespace winrt::Storj_Tools::implementation
{
	StorjData::StorjData() : isLightingMode(true), currentTheme(ElementTheme::Light)
    {
    }

    event_token StorjData::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return propertyChanged.add(handler);
    }

    void StorjData::PropertyChanged(event_token const& token)
    {
        propertyChanged.remove(token);
    }

    IObservableVector<Storj_Tools::Node> StorjData::Nodes()
    {
        return observableNodes;
    }

    void StorjData::UpdateNodes()
    {
		vector<Storj_Tools::Node> vectorNodes = GetNodes();
		if (isVectorsNodesEquals(vectorNodes, observableNodes)) return;
		observableNodes.Clear();
		for (const auto& node : vectorNodes)
		{
			observableNodes.Append(node);
		}
    }

    void StorjData::StartNodeService(hstring nodeName)
    {
		dataAdapter.StartNode(HstringToWstring(nodeName));
    }

    hstring StorjData::ServiceStatus()
    {
		return WstringToHstring(dataAdapter.GetServiceStatus());
    }

    SolidColorBrush StorjData::ServiceStatusColor()
    {
        return (dataAdapter.GetServiceStatus() == L"Running") ? SolidColorBrush(Colors::Green()) : SolidColorBrush(Colors::Red());
    }

    void StorjData::RestartService()
    {
        dataAdapter.RestartService();
    }

    void StorjData::SaveSettings()
    {
		dataAdapter.SaveSettings();
    }

    void StorjData::LoadSettings()
    {
		dataAdapter.LoadSettings();
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
    }

    void StorjData::SendTestMessageToDiscord()
    {
		dataAdapter.SendTestMessageToDiscord();
    }

    void StorjData::RaisePropertyChanged(hstring const& propertyName)
    {
        propertyChanged(*this, PropertyChangedEventArgs(propertyName));
    }

    vector<Storj_Tools::Node> StorjData::GetNodes()
    {
		vector<Storj_Tools::Node> nodes;
		vector<wstring> nodesNames = dataAdapter.GetNodesNames();
		for (const auto& nodeName : nodesNames)
		{
			Storj_Tools::Node node;
			node.Name(WstringToHstring(nodeName));
			node.Path(WstringToHstring(dataAdapter.GetNodePath(nodeName)));
			node.SVersion(WstringToHstring(dataAdapter.GetNodeVersion(nodeName)));
			node.DStatus(dataAdapter.GetNodeStatus(nodeName));
			nodes.push_back(node);
		}
        return nodes;
    }

    void StorjData::SortNodesByName(vector<Storj_Tools::Node>& nodes)
    {
        sort(nodes.begin(), nodes.end());
    }

    bool StorjData::isVectorsNodesEquals(const vector<Storj_Tools::Node>& nodes1, const IObservableVector<Storj_Tools::Node> nodes2)
    {
		if (nodes1.size() != nodes2.Size()) return false;
        return std::equal(nodes1.begin(), nodes1.end(), nodes2.begin());
    }

    hstring StorjData::WstringToHstring(const wstring& wstring)
    {
        return hstring(wstring.c_str());
    }

    wstring StorjData::HstringToWstring(const hstring& hstring)
    {
        return wstring(hstring.c_str());
    }

    string StorjData::WstringToString(const wstring& wstring)
    {
        if (wstring.empty()) return "";
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0) return "";
        string str(size_needed - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
        return str;
    }

    wstring StorjData::StringToWstring(const string& string)
    {
        if (string.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);
        if (size_needed <= 0) return L"";
        wstring wstr(size_needed - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &wstr[0], size_needed);
        return wstr;
    }

    string StorjData::HstringToString(const hstring& hstring)
    {
		return WstringToString(HstringToWstring(hstring));
    }

    hstring StorjData::StringToHstring(const string& string)
    {
		return WstringToHstring(StringToWstring(string));
    }

    void StorjData::InitTheme()
    {
        auto settings = ApplicationData::Current().LocalSettings();
        auto value = settings.Values().TryLookup(L"AppThemeIsLight");
        isLightingMode = value ? unbox_value<bool>(value) : true;
        CurrentTheme(isLightingMode ? ElementTheme::Light : ElementTheme::Dark);
    }

    bool StorjData::IsLightMode()
    {
		return isLightingMode;
    }

    void StorjData::IsLightMode(bool value)
    {
		if (isLightingMode == value) return;
		isLightingMode = value;
        auto settings = ApplicationData::Current().LocalSettings();
        settings.Values().Insert(L"AppThemeIsLight", box_value(value));
		RaisePropertyChanged(L"IsLightMode");
		CurrentTheme(value ? ElementTheme::Light : ElementTheme::Dark);
    }

    ElementTheme StorjData::CurrentTheme()
    {
		return currentTheme;
    }

    void StorjData::CurrentTheme(ElementTheme value)
    {
		if (currentTheme == value) return;
		currentTheme = value;
		RaisePropertyChanged(L"CurrentTheme");
    }

    FLOAT StorjData::UpdateNodeUIInterval()
    {
		if (updateNodeUIInterval != 0) return updateNodeUIInterval;
		auto settings = ApplicationData::Current().LocalSettings();
		auto value = settings.Values().TryLookup(L"UpdateNodeUIInterval");
		updateNodeUIInterval = value ? unbox_value<FLOAT>(value) : 30;
    	RaisePropertyChanged(L"UpdateNodeUIInterval");
		return updateNodeUIInterval;
    }

    void StorjData::UpdateNodeUIInterval(FLOAT value)
    {
		if (updateNodeUIInterval == value) return;
		updateNodeUIInterval = value;
		RaisePropertyChanged(L"UpdateNodeUIInterval");
		auto settings = ApplicationData::Current().LocalSettings();
		settings.Values().Insert(L"UpdateNodeUIInterval", box_value(value));
    }
}
