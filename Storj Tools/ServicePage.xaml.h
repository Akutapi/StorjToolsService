#pragma once

#include "ServicePage.g.h"

namespace winrt::Storj_Tools::implementation
{
    struct ServicePage : ServicePageT<ServicePage>
    {
        ServicePage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

		void SaveSettings_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct ServicePage : ServicePageT<ServicePage, implementation::ServicePage>
    {
    };
}
