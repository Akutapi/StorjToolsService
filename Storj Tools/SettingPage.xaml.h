#pragma once

#include "SettingPage.g.h"

namespace winrt::Storj_Tools::implementation
{
    struct SettingPage : SettingPageT<SettingPage>
    {
        SettingPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct SettingPage : SettingPageT<SettingPage, implementation::SettingPage>
    {
    };
}
