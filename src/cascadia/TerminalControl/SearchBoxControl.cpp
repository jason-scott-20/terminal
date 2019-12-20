﻿// Copyright (c) Microsoft Corporation
// Licensed under the MIT license.

#include "pch.h"
#include "SearchBoxControl.h"
#include "SearchBoxControl.g.cpp"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Core;

namespace winrt::Microsoft::Terminal::TerminalControl::implementation
{
    // Constructor
    SearchBoxControl::SearchBoxControl()
    {
        InitializeComponent();

        this->CharacterReceived({ this, &SearchBoxControl::_CharacterHandler });

        _textBox = TextBox();

        if (_textBox)
            _focusableElements.insert(_textBox);
        _focusableElements.insert(CloseButton());
        _focusableElements.insert(CaseSensitivityButton());
        _focusableElements.insert(GoForwardButton());
        _focusableElements.insert(GoBackwardButton());
    }

    // Method Description:
    // - Check if the current search direction is forward
    // Arguments:
    // - <none>
    // Return Value:
    // - bool: the current search direction, determined by the
    //         states of the two direction buttons
    bool SearchBoxControl::_GoForward()
    {
        return GoForwardButton().IsChecked().GetBoolean();
    }

    // Method Description:
    // - Check if the current search is case sensitive
    // Arguments:
    // - <none>
    // Return Value:
    // - bool: whether the current search is case sensitive (case button is checked )
    //   or not
    bool SearchBoxControl::_CaseSensitive()
    {
        return CaseSensitivityButton().IsChecked().GetBoolean();
    }

    // Method Description:
    // - Handler for pressing Enter on TextBox, trigger
    //   text search
    // Arguments:
    // - sender: not used
    // - e: event data
    // Return Value:
    // - <none>
    void SearchBoxControl::TextBoxKeyDown(winrt::Windows::Foundation::IInspectable const& /*sender*/, Input::KeyRoutedEventArgs const& e)
    {
        if (e.OriginalKey() == winrt::Windows::System::VirtualKey::Enter)
        {
            auto const state = CoreWindow::GetForCurrentThread().GetKeyState(winrt::Windows::System::VirtualKey::Shift);
            if (WI_IsFlagSet(state, CoreVirtualKeyStates::Down))
            {
                _SearchHandlers(_textBox.Text(), !_GoForward(), _CaseSensitive());
            }
            else
            {
                _SearchHandlers(_textBox.Text(), _GoForward(), _CaseSensitive());
            }
            e.Handled(true);
        }
    }

    // Method Description:
    // - Handler for pressing Enter on TextBox, trigger
    //   text search
    // Arguments:
    // - <none>
    // Return Value:
    // - <none>
    void SearchBoxControl::SetFocusOnTextbox()
    {
        if (_textBox)
        {
            Input::FocusManager::TryFocusAsync(_textBox, FocusState::Keyboard);
            _textBox.SelectAll();
        }
    }

    // Method Description:
    // - Check if the current focus is on any element within the
    //   search box
    // Arguments:
    // - <none>
    // Return Value:
    // - bool: whether the current focus is on the search box
    bool SearchBoxControl::ContainsFocus()
    {
        auto focusedElement = Input::FocusManager::GetFocusedElement(this->XamlRoot());
        if (_focusableElements.count(focusedElement) > 0)
        {
            return true;
        }

        return false;
    }

    // Method Description:
    // - Handler for clicking the GoBackward button. This change the value of _goForward,
    //   mark GoBackward button as checked and ensure GoForward button
    //   is not checked
    // Arguments:
    // - sender: not used
    // - e: not used
    // Return Value:
    // - <none>
    void SearchBoxControl::GoBackwardClicked(winrt::Windows::Foundation::IInspectable const& /*sender*/, RoutedEventArgs const& /*e*/)
    {
        GoBackwardButton().IsChecked(true);
        if (GoForwardButton().IsChecked())
        {
            GoForwardButton().IsChecked(false);
        }

        // kick off search
        _SearchHandlers(_textBox.Text(), _GoForward(), _CaseSensitive());
    }

    // Method Description:
    // - Handler for clicking the GoForward button. This change the value of _goForward,
    //   mark GoForward button as checked and ensure GoBackward button
    //   is not checked
    // Arguments:
    // - sender: not used
    // - e: not used
    // Return Value:
    // - <none>
    void SearchBoxControl::GoForwardClicked(winrt::Windows::Foundation::IInspectable const& /*sender*/, RoutedEventArgs const& /*e*/)
    {
        GoForwardButton().IsChecked(true);
        if (GoBackwardButton().IsChecked())
        {
            GoBackwardButton().IsChecked(false);
        }

        // kick off search
        _SearchHandlers(_textBox.Text(), _GoForward(), _CaseSensitive());
    }

    // Method Description:
    // - Handler for clicking the close button. This destructs the
    //   search box object in TermControl
    // Arguments:
    // - sender: not used
    // - e: event data
    // Return Value:
    // - <none>
    void SearchBoxControl::CloseClick(winrt::Windows::Foundation::IInspectable const& /*sender*/, RoutedEventArgs const& e)
    {
        _ClosedHandlers(*this, e);
    }

    // Method Description:
    // - To avoid Characters input bubbling up to terminal, we implement this handler here,
    //   simply mark the key input as handled
    // Arguments:
    // - sender: not used
    // - e: event data
    // Return Value:
    // - <none>
    void SearchBoxControl::_CharacterHandler(winrt::Windows::Foundation::IInspectable const& /*sender*/, Input::CharacterReceivedRoutedEventArgs const& e)
    {
        e.Handled(true);
    }
}
