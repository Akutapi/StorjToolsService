#pragma once

#include "NodesPage.g.h"
#include "StorjData.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Dispatching;


namespace winrt::Storj_Tools::implementation
{
    struct NodesPage : NodesPageT<NodesPage>
    {
        NodesPage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);
		void OnNavigatedFrom(NavigationEventArgs const& args);

        Storj_Tools::StorjData Data();
        void StartNodeService(IInspectable const& sender, RoutedEventArgs const& args);

    private:

        Storj_Tools::StorjData storjData;
        void OnUpdateTimerTick(IInspectable const& sender, IInspectable const& args);
        DispatcherQueueTimer updateTimer{ nullptr };
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct NodesPage : NodesPageT<NodesPage, implementation::NodesPage>
    {
    };
}
