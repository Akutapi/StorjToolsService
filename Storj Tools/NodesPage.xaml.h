#pragma once

#include "NodesPage.g.h"
#include "StorjData.h"

namespace winrt::Storj_Tools::implementation
{
    struct NodesPage : NodesPageT<NodesPage>
    {
        NodesPage() = default;

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
		void OnNavigatedFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);

        Storj_Tools::StorjData Data();
        void StartNodeService(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:

        Storj_Tools::StorjData storjData;
        void OnUpdateTimerTick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer updateTimer{ nullptr };
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct NodesPage : NodesPageT<NodesPage, implementation::NodesPage>
    {
    };
}
