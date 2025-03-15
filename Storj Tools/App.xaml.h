#pragma once
#include "App.xaml.g.h"
#include "StorjData.h"

using namespace  winrt::Microsoft::UI::Xaml;

namespace winrt::Storj_Tools::implementation
{
    struct App : AppT<App>
    {
        App();
        void OnLaunched(LaunchActivatedEventArgs const&);
        static Storj_Tools::StorjData& GetStorjData();

    private:
        Window window{ nullptr };
        inline static Storj_Tools::StorjData storjData{};
    };
}
