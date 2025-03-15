#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Storage.h>
#include <functional>

using namespace std;
using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Microsoft::UI::Windowing;
using namespace Microsoft::UI::Xaml::Data;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Graphics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	MainWindow::MainWindow() : storjData(App::GetStorjData())
	{
        InitializeComponent();
        storjData.PropertyChanged({ this, &MainWindow::StorjData_PropertyChanged });
		LoadAppData();
        ContentFrame().Navigate(winrt::xaml_typename<Storj_Tools::NodesPage>());
        AppWindow().Closing({ this, &MainWindow::OnWindowClosing });
    }

    void MainWindow::OnWindowClosing([[maybe_unused]] winrt::Microsoft::UI::Windowing::AppWindow const& sender, [[maybe_unused]] AppWindowClosingEventArgs const& args)
    {
        SaveAppData();
    }

    void MainWindow::NavView_SelectionChanged([[maybe_unused]] NavigationView const& sender,NavigationViewSelectionChangedEventArgs const& args)
    {   
        sender.IsBackEnabled(ContentFrame().CanGoBack());
        if (!args.SelectedItem()) return;
        auto selectedItem = args.SelectedItem().as<NavigationViewItem>();
        hstring tag = unbox_value<hstring>(selectedItem.Tag());
		if (tag.empty()) return;
		
		SelectPage(tag);
    }

    void MainWindow::NavView_BackRequested([[maybe_unused]] NavigationView const& sender, [[maybe_unused]] NavigationViewBackRequestedEventArgs const& args)
    {   
        sender.IsBackEnabled(ContentFrame().CanGoBack());
		if (!ContentFrame().CanGoBack()) return;
        ContentFrame().GoBack();
    }

    Storj_Tools::StorjData MainWindow::Data()
    {
        return storjData;
    }

    void MainWindow::SelectPage(hstring const& tag)
    {
        if (tag.empty()) return;
        static const unordered_map<wstring, function<void()>> navigationMap =
        {
            {L"NodesPage", [this]() {ContentFrame().Navigate(xaml_typename<Storj_Tools::NodesPage>()); }},
            {L"ServicePage", [this]() {ContentFrame().Navigate(xaml_typename<Storj_Tools::ServicePage>()); }},
            {L"DiscordPage", [this]() {ContentFrame().Navigate(xaml_typename<Storj_Tools::DiscordPage>()); }},
            {L"Settings", [this]() {ContentFrame().Navigate(xaml_typename<Storj_Tools::SettingPage>()); }}
        };

        auto navigation = navigationMap.find(tag.c_str());
        if (navigation != navigationMap.end())
        {
            navigation->second();
        }
    }

    void MainWindow::LoadAppData()
    {
		SetTitleBar();
        auto settings = ApplicationData::Current().LocalSettings();
        // Naètení velikosti okna
        if (settings.Values().HasKey(L"WindowWidth") && settings.Values().HasKey(L"WindowHeight"))
        {
            int32_t width = unbox_value<int32_t>(settings.Values().Lookup(L"WindowWidth"));
            int32_t height = unbox_value<int32_t>(settings.Values().Lookup(L"WindowHeight"));
			this->AppWindow().Resize(SizeInt32{ width, height });
        }

        // Naètení pozice okna
        if (settings.Values().HasKey(L"WindowLeft") && settings.Values().HasKey(L"WindowTop"))
        {
            int32_t left = unbox_value<int32_t>(settings.Values().Lookup(L"WindowLeft"));
            int32_t top = unbox_value<int32_t>(settings.Values().Lookup(L"WindowTop"));
			this->AppWindow().Move(PointInt32{ left, top });

        }

        if (settings.Values().HasKey(L"NavigationViewExpanded"))
        {
			NavView().IsPaneOpen(unbox_value<bool>(settings.Values().Lookup(L"NavigationViewExpanded")));
        }
    }

    void MainWindow::SaveAppData()  
    {  
       auto settings = ApplicationData::Current().LocalSettings();  
       settings.Values().Insert(L"WindowWidth", box_value(this->AppWindow().ClientSize().Width));
	   settings.Values().Insert(L"WindowHeight", box_value(this->AppWindow().ClientSize().Height));
	   settings.Values().Insert(L"WindowLeft", box_value(this->AppWindow().Position().X));
	   settings.Values().Insert(L"WindowTop", box_value(this->AppWindow().Position().Y));
	   settings.Values().Insert(L"NavigationViewExpanded", box_value(NavView().IsPaneOpen()));
    }

    void MainWindow::SetTitleBar()
    {
        auto titleBar = this->AppWindow().TitleBar();
		if (!titleBar) return;
        if (!titleBar.ExtendsContentIntoTitleBar()) titleBar.ExtendsContentIntoTitleBar(true);
        titleBar.ButtonBackgroundColor(storjData.IsLightMode() ? ColorHelper::FromArgb(0xFF, 0xF3, 0xF3, 0xF3) : ColorHelper::FromArgb(0xFF, 0x20, 0x20, 0x20));
    }

    void MainWindow::StorjData_PropertyChanged([[maybe_unused]] IInspectable const& sender, PropertyChangedEventArgs const& args)
    {
		if (args.PropertyName() != L"CurrentTheme") return;
		SetTitleBar();
	}
}