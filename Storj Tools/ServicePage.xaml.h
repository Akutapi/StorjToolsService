#pragma once

#include "ServicePage.g.h"
#include "StorjData.h"

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

        Storj_Tools::StorjData StorjData();

    private:
        Storj_Tools::StorjData storjData;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct ServicePage : ServicePageT<ServicePage, implementation::ServicePage>
    {
    };
}
