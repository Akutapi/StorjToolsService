#pragma once

#include "DiscordPage.g.h"
#include "StorjData.h"

namespace winrt::Storj_Tools::implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage>
    {
        DiscordPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
        
        winrt::Windows::Foundation::IAsyncAction PasteUserId_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::Windows::Foundation::IAsyncAction PasteBotToken_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
	    Storj_Tools::StorjData StorjData();
    private:
		Storj_Tools::StorjData storjData;
        winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> GetClipboardText();
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage, implementation::DiscordPage>
    {
		
    };
}
