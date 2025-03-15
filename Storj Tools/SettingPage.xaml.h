#pragma once

#include "SettingPage.g.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml::Navigation;

namespace winrt::Storj_Tools::implementation
{
    struct SettingPage : SettingPageT<SettingPage>
    {
		SettingPage() = default;

		void OnNavigatedTo(NavigationEventArgs const& args);
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
