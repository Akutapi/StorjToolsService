#pragma once

#include "ServicePage.g.h"
#include "StorjData.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml::Navigation;

namespace winrt::Storj_Tools::implementation
{
    struct ServicePage : ServicePageT<ServicePage>
    {
		ServicePage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);
        Storj_Tools::StorjData Data();

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
