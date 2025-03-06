#pragma once
#include "MainWindow.g.h"
#include "App.xaml.h"
#include "StorjData.h"
namespace winrt::Storj_Tools::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();

		void NavView_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

		Storj_Tools::StorjData Data();

	private:
        Storj_Tools::StorjData storjData;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
