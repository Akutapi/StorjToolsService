#pragma once

#include "SettingPage.g.h"

namespace winrt::Storj_Tools::implementation
{
    struct SettingPage : SettingPageT<SettingPage>
    {
		SettingPage() = default;

		void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        Storj_Tools::StorjData Data();
       
    private:
        Storj_Tools::StorjData storjData;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct SettingPage : SettingPageT<SettingPage, implementation::SettingPage>
    {
    };
}
