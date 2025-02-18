#pragma once

#include "NodesPage.g.h"

namespace winrt::Storj_Tools::implementation
{
    struct NodesPage : NodesPageT<NodesPage>
    {
        NodesPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct NodesPage : NodesPageT<NodesPage, implementation::NodesPage>
    {
    };
}
