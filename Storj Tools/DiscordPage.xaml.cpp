#include "pch.h"
#include "DiscordPage.xaml.h"
#if __has_include("DiscordPage.g.cpp")
#include "DiscordPage.g.cpp"
#endif
#include <winrt/Windows.ApplicationModel.DataTransfer.h>


using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::DataTransfer;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	void DiscordPage::OnNavigatedTo([[maybe_unused]] NavigationEventArgs const& args)
	{
		storjData = App::GetStorjData(); // Použití globální instance
	}

	IAsyncAction DiscordPage::PasteUserId_Click([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
	{
		hstring clipboardText = co_await GetClipboardText();
		if (!clipboardText.empty())
		{
			storjData.UserID(clipboardText);
		}
		co_return;
	}

	IAsyncAction DiscordPage::PasteBotToken_Click([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
	{
		hstring clipboardText = co_await GetClipboardText();
		if (!clipboardText.empty())
		{
			storjData.BotToken(clipboardText);
		}
		co_return;
	}

	Storj_Tools::StorjData DiscordPage::Data()
	{
		return storjData;
	}

    IAsyncOperation<hstring> DiscordPage::GetClipboardText()
    {
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Text()))
        {
            co_return co_await dataPackageView.GetTextAsync();
        }
        co_return L"";
    }

}
