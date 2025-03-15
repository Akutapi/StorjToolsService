#pragma once
#include "MainWindow.g.h"
#include "App.xaml.h"
#include "StorjData.h"
#include <stack>

using namespace winrt;
using namespace Microsoft::UI::Windowing;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Data;
using namespace Windows::Foundation;

namespace winrt::Storj_Tools::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();

        void OnWindowClosing(Microsoft::UI::Windowing::AppWindow const& sender, AppWindowClosingEventArgs const& args);
		void NavView_SelectionChanged(NavigationView const& sender, NavigationViewSelectionChangedEventArgs const& args);
        void NavView_BackRequested(NavigationView const& sender, NavigationViewBackRequestedEventArgs const& args);
        void StorjData_PropertyChanged(IInspectable const& sender, PropertyChangedEventArgs const& args);
		
        Storj_Tools::StorjData Data();

    private:
        Storj_Tools::StorjData storjData;
        
		void SelectPage(hstring const& tag);
		void LoadAppData();
		void SaveAppData();
		void SetTitleBar();
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
