#pragma once

#include "StorjData.g.h"
#include "DataAdapter.h"
#include "Node.h"
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::Storj_Tools::implementation
{
    struct StorjData : StorjDataT<StorjData>
    {
		StorjData();
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token);

        // Main page
        hstring ServiceStatus();
        void RestartService();

		// Setting from YAML file
		void SaveSettings();
		void LoadSettings();

		// Service page
		FLOAT UpdateNodeInterval();
		void UpdateNodeInterval(FLOAT value);

		FLOAT CheckNodeInterval();
		void CheckNodeInterval(FLOAT value);

		FLOAT LogReductionInterval();
		void LogReductionInterval(FLOAT value);

		FLOAT LogReductionMaxSize();
		void LogReductionMaxSize(FLOAT value);

		FLOAT LogReducedSize();
		void LogReducedSize(FLOAT value);

		// Discord page

		hstring BotToken();
		void BotToken(hstring value);

		hstring UserID();
		void UserID(hstring value);

		void SendTestMessageToDiscord();

		// Node page
		winrt::Windows::Foundation::Collections::IObservableVector<Storj_Tools::Node> Nodes();
		void UpdateNodes();
		void StartNodeService(hstring nodeName);

		// Settings page
		void InitTheme();
		bool IsLightMode();
		void IsLightMode(bool value);
		Microsoft::UI::Xaml::ElementTheme CurrentTheme();
		void CurrentTheme(Microsoft::UI::Xaml::ElementTheme value);
		FLOAT UpdateNodeUIInterval();
		void UpdateNodeUIInterval(FLOAT value);
		void SetTitleBar();


    private:
        void RaisePropertyChanged(hstring const& propertyName);

		winrt::Windows::Foundation::Collections::IObservableVector<Storj_Tools::Node> observableNodes { winrt::single_threaded_observable_vector<Storj_Tools::Node>() };
		std::vector<Storj_Tools::Node> GetNodes();
		void SortNodesByName(std::vector<Storj_Tools::Node>& nodes);
		bool isVectorsNodesEquals(const std::vector<Storj_Tools::Node>& nodes1, const winrt::Windows::Foundation::Collections::IObservableVector<Storj_Tools::Node> nodes2);

		hstring WstringToHstring(const std::wstring& wstring);
		std::wstring HstringToWstring(const hstring& hstring);
		std::string WstringToString(const std::wstring& wstring);
		std::wstring StringToWstring(const std::string& string);
		std::string HstringToString(const winrt::hstring& hstring);
		hstring StringToHstring(const std::string& string);

		bool isLightingMode;
		Microsoft::UI::Xaml::ElementTheme currentTheme;
		FLOAT updateNodeUIInterval{0};
		
		DataAdapter& dataAdapter = DataAdapter::Instance();
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct StorjData : StorjDataT<StorjData, implementation::StorjData>
    {
    };
}
