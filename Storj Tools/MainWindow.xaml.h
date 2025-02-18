#pragma once
#include "MainWindow.g.h"
#include "App.xaml.h"
#include "DataAdapter.h"
namespace winrt::Storj_Tools::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();

		void NavView_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        
        hstring Test();
		void Test(hstring const& value);

		void BTN_RestartService(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
	private:
		DataAdapter& data = DataAdapter::Instance();
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
