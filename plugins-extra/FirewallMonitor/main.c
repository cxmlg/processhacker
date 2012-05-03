/*
 * Process Hacker Firewall Monitor -
 *   main program
 *
 * Copyright (C) 2012 dmex
 * Copyright (C) 2012 wj32
 *
 * This file is part of Process Hacker.
 *
 * Process Hacker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Process Hacker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Process Hacker.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fwmon.h"
#include "resource.h"
#define FIREWALL_MENUITEM 1

VOID NTAPI LoadCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    );

VOID NTAPI UnloadCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    );

VOID NTAPI ShowOptionsCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    );

VOID NTAPI MainWindowShowingCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    );

VOID NTAPI MenuItemCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    );

PPH_PLUGIN PluginInstance;
PH_CALLBACK_REGISTRATION PluginLoadCallbackRegistration;
PH_CALLBACK_REGISTRATION PluginUnloadCallbackRegistration;
PH_CALLBACK_REGISTRATION PluginShowOptionsCallbackRegistration;
PH_CALLBACK_REGISTRATION MainWindowShowingCallbackRegistration;
PH_CALLBACK_REGISTRATION PluginMenuItemCallbackRegistration;

LOGICAL DllMain(
    __in HINSTANCE Instance,
    __in ULONG Reason,
    __reserved PVOID Reserved
    )
{
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        {
            PPH_PLUGIN_INFORMATION info;

            PluginInstance = PhRegisterPlugin(L"ProcessHacker.FirewallMonitor", Instance, &info);

            if (!PluginInstance)
                return FALSE;

            info->DisplayName = L"Firewall Monitor";
            info->Author = L"dmex";
            info->Description = L"Monitors firewall events.";
            info->HasOptions = FALSE;

            PhRegisterCallback(
                PhGetPluginCallback(PluginInstance, PluginCallbackLoad),
                LoadCallback,
                NULL,
                &PluginLoadCallbackRegistration
                );
            PhRegisterCallback(
                PhGetPluginCallback(PluginInstance, PluginCallbackUnload),
                UnloadCallback,
                NULL,
                &PluginUnloadCallbackRegistration
                );
            PhRegisterCallback(
                PhGetPluginCallback(PluginInstance, PluginCallbackShowOptions),
                ShowOptionsCallback,
                NULL,
                &PluginShowOptionsCallbackRegistration
                );
            PhRegisterCallback(
                PhGetPluginCallback(PluginInstance, PluginCallbackMenuItem),
                MenuItemCallback,
                NULL,
                &PluginMenuItemCallbackRegistration
                );

            PhRegisterCallback(
                PhGetGeneralCallback(GeneralCallbackMainWindowShowing),
                MainWindowShowingCallback,
                NULL,
                &MainWindowShowingCallbackRegistration
                );

            {
                PH_SETTING_CREATE settings[] =
                {
                    { StringSettingType, SETTING_NAME_FW_TREE_LIST_COLUMNS, L"" },
                    { IntegerPairSettingType, SETTING_NAME_FW_TREE_LIST_SORT, L"0,2" }
                };

                PhAddSettings(settings, _countof(settings));
            }
        }
        break;
    }

    return TRUE;
}

VOID NTAPI LoadCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    )
{
    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX) };
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;

    InitCommonControlsEx(&icex);
}

VOID NTAPI UnloadCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    )
{
    StopFwMonitor();
}

VOID NTAPI ShowOptionsCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    )
{
    // Options?
}

VOID NTAPI MainWindowShowingCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    )
{
    //InitializeFwTab();
    PhPluginAddMenuItem(PluginInstance, PH_MENU_ITEM_LOCATION_VIEW, NULL, FIREWALL_MENUITEM, L"Windows Firewall", NULL);
}

VOID NTAPI MenuItemCallback(
    __in_opt PVOID Parameter,
    __in_opt PVOID Context
    )
{
    PPH_PLUGIN_MENU_ITEM menuItem = (PPH_PLUGIN_MENU_ITEM)Parameter;

    if (menuItem != NULL)
    {
        switch (menuItem->Id)
        {
        case FIREWALL_MENUITEM:
            {
                PFIREWALL_CONTEXT context = PhAllocate(sizeof(FIREWALL_CONTEXT));
                ZeroMemory(context, sizeof(FIREWALL_CONTEXT));

                DialogBoxParam(
                    (HINSTANCE)PluginInstance->DllBase,
                    MAKEINTRESOURCE(IDD_DIALOG1),
                    NULL,
                    FirewallDlgProc,
                    (LPARAM)context
                    );
            }
            break;
        }
    }
}