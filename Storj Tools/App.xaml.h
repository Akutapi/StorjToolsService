#pragma once
#include "App.xaml.g.h"
#include "StorjData.h"

namespace winrt::Storj_Tools::implementation
{
    struct App : AppT<App>
    {
        App();
        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        static Storj_Tools::StorjData& GetStorjData();

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        inline static Storj_Tools::StorjData storjData{};
    };
}
