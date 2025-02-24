#pragma once

#include "StorjData.g.h"
#include "DataAdapter.h"

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

    private:
        void RaisePropertyChanged(hstring const& propertyName);

		hstring WstringToHstring(const std::wstring& wstring);
		std::wstring HstringToWstring(const hstring& hstring);
		std::string WstringToString(const std::wstring& wstring);
		std::wstring StringToWstring(const std::string& string);
		std::string HstringToString(const winrt::hstring& hstring);
		hstring StringToHstring(const std::string& string);

		
		DataAdapter& dataAdapter = DataAdapter::Instance();
        winrt::Microsoft::UI::Dispatching::DispatcherQueue dispatcher{ nullptr };
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;

		// Node page


    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct StorjData : StorjDataT<StorjData, implementation::StorjData>
    {
    };
}
