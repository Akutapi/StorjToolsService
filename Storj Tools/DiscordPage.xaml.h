#pragma once

#include "DiscordPage.g.h"

namespace winrt::Storj_Tools::implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage>
    {
        DiscordPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
        
        void PasteUserId_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
		void PasteBotToken_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
		void SaveSettings_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
		void SendTestMessage_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage, implementation::DiscordPage>
    {
		
    };
}
