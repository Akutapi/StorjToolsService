#include "pch.h"
#include "DiscordPage.xaml.h"
#if __has_include("DiscordPage.g.cpp")
#include "DiscordPage.g.cpp"
#endif
#include <winrt/Windows.ApplicationModel.DataTransfer.h>


using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	winrt::Windows::Foundation::IAsyncAction DiscordPage::PasteUserId_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args)
	{
		hstring clipboardText = co_await GetClipboardText();
		if (!clipboardText.empty())
		{
			storjData.UserID(clipboardText);
		}
		co_return;
	}
	winrt::Windows::Foundation::IAsyncAction DiscordPage::PasteBotToken_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args)
	{
		hstring clipboardText = co_await GetClipboardText();
		if (!clipboardText.empty())
		{
			storjData.BotToken(clipboardText);
		}
		co_return;
	}

	Storj_Tools::StorjData DiscordPage::StorjData()
	{
		return storjData;
	}

    winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> DiscordPage::GetClipboardText()
    {
        auto dataPackageView = winrt::Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
        if (dataPackageView.Contains(winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats::Text()))
        {
            co_return co_await dataPackageView.GetTextAsync();
        }
        co_return L"";
    }
}
