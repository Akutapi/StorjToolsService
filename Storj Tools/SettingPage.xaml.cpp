#include "pch.h"
#include "SettingPage.xaml.h"
#if __has_include("SettingPage.g.cpp")
#include "SettingPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml::Navigation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
    Storj_Tools::StorjData SettingPage::Data()
    {
        return storjData;
    }
    void SettingPage::OnNavigatedTo([[maybe_unused]] NavigationEventArgs const& args)
    {
		storjData = App::GetStorjData(); // Pou�it� glob�ln� instance
    }
}
