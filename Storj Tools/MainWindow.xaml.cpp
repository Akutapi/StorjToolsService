#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <functional>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Navigation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	MainWindow::MainWindow() : storjData(App::GetStorjData())
	{
        InitializeComponent();
        ContentFrame().Navigate(winrt::xaml_typename<Storj_Tools::NodesPage>());
	}

    void MainWindow::NavView_SelectionChanged([[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if (!args.SelectedItem()) return;

        auto selectedItem = args.SelectedItem().as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
        auto tag = winrt::unbox_value<winrt::hstring>(selectedItem.Tag());

        if (tag.empty()) return;
        static const std::unordered_map<std::wstring, std::function<void()>> navigationMap =
        {
            {
                L"NodesPage", [this]()
                {
                    ContentFrame().Navigate(winrt::xaml_typename<Storj_Tools::NodesPage>());
                }
            },
            {
                L"ServicePage", [this]()
                {
                    ContentFrame().Navigate(winrt::xaml_typename<Storj_Tools::ServicePage>());
                }
            },
            {
                L"DiscordPage", [this]()
                {
                    ContentFrame().Navigate(winrt::xaml_typename<Storj_Tools::DiscordPage>());
                }
            },
            {
                L"Settings", [this]()
                {
                    ContentFrame().Navigate(winrt::xaml_typename<Storj_Tools::SettingPage>());
                }
            }
        };

		auto navigation = navigationMap.find(tag.c_str());
		if (navigation != navigationMap.end())
		{
			navigation->second();
		}
    }

    Storj_Tools::StorjData MainWindow::Data()
    {
        return storjData;
    }
}