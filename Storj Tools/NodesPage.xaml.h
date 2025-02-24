#pragma once

#include "NodesPage.g.h"
#include "StorjData.h"

namespace winrt::Storj_Tools::implementation
{
    struct NodesPage : NodesPageT<NodesPage>
    {
        NodesPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        Storj_Tools::StorjData StorjData();

    private:
        Storj_Tools::StorjData storjData;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct NodesPage : NodesPageT<NodesPage, implementation::NodesPage>
    {
    };
}
