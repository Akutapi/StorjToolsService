#pragma once

#include "StorjData.g.h"
#include "DataAdapter.h"
#include "Node.h"
#include <winrt/Windows.Foundation.Collections.h>

using namespace std;
using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Data;
using namespace Microsoft::UI::Xaml::Media;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Storj_Tools::implementation
{
    struct StorjData : StorjDataT<StorjData>
    {
		StorjData();
        event_token PropertyChanged(PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

        // Main page
        hstring ServiceStatus();
		SolidColorBrush ServiceStatusColor();
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
		IObservableVector<Storj_Tools::Node> Nodes();
		void UpdateNodes();
		void StartNodeService(hstring nodeName);

		// Settings page
		void InitTheme();
		bool IsLightMode();
		void IsLightMode(bool value);
		ElementTheme CurrentTheme();
		void CurrentTheme(ElementTheme value);
		FLOAT UpdateNodeUIInterval();
		void UpdateNodeUIInterval(FLOAT value);
		


    private:
        void RaisePropertyChanged(hstring const& propertyName);

		IObservableVector<Storj_Tools::Node> observableNodes { single_threaded_observable_vector<Storj_Tools::Node>() };
		vector<Storj_Tools::Node> GetNodes();
		void SortNodesByName(vector<Storj_Tools::Node>& nodes);
		bool isVectorsNodesEquals(const vector<Storj_Tools::Node>& nodes1, const IObservableVector<Storj_Tools::Node> nodes2);

		hstring WstringToHstring(const wstring& wstring);
		wstring HstringToWstring(const hstring& hstring);
		string WstringToString(const wstring& wstring);
		wstring StringToWstring(const string& string);
		string HstringToString(const hstring& hstring);
		hstring StringToHstring(const string& string);

		bool isLightingMode;
		ElementTheme currentTheme;
		FLOAT updateNodeUIInterval{0};

		DataAdapter& dataAdapter = DataAdapter::Instance();
        event<PropertyChangedEventHandler> propertyChanged;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct StorjData : StorjDataT<StorjData, implementation::StorjData>
    {
    };
}
